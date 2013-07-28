/*
 * This file is part of Mitten.
 * 
 * Mitten is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mitten is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mitten.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 Oliver Katz */

#include "tokenizer.h"

namespace parsing
{
	Token &Token::setContent(string s)
	{
		content = s;
		return *this;
	}

	Token &Token::setLine(unsigned int l)
	{
		line = l;
		return *this;
	}

	Token &Token::setColumn(unsigned int c)
	{
		column = c;
		return *this;
	}

	Token &Token::setType(string t)
	{
		type = t;
		return *this;
	}

	Token &Token::setFile(string f)
	{
		file = f;
		return *this;
	}

	bool Token::empty()
	{
		return content.empty();
	}

	string Token::get()
	{
		return content;
	}

	string Token::set(string s)
	{
		return (content = s);
	}

	string Token::getContent()
	{
		return content;
	}

	unsigned int Token::getLine()
	{
		return line;
	}

	unsigned int Token::getColumn()
	{
		return column;
	}

	string Token::getType()
	{
		return type;
	}

	string Token::getFile()
	{
		return file;
	}

	bool Token::operator == (Token other)
	{
		return (content.compare(other.content) == 0);
	}

	bool Token::operator == (string other)
	{
		return (content.compare(other) == 0);
	}

	bool Token::operator != (Token other)
	{
		return (content.compare(other.content) != 0);
	}

	bool Token::operator != (string other)
	{
		return (content.compare(other) != 0);
	}

	void Tokenizer::appendPatternToVectorSorted(vector<Pattern> &v, Pattern p)
	{
		unsigned int idx = 0;
		for (; idx < v.size(); idx++)
		{
			if (v[idx].getLengthRequest() < p.getLengthRequest())
			{
				break;
			}
		}
		v.insert(v.begin()+idx, p);
	}

	unsigned int Tokenizer::fitsPatternVector(string s, vector<Pattern> v)
	{
		for (vector<Pattern>::iterator i = v.begin(); i != v.end(); i++)
		{
			unsigned int n = i->match(s);
			if (n > 0)
			{
				return n;
			}
		}

		return 0;
	}

	unsigned int Tokenizer::isWhitespace(string s)
	{
		return fitsPatternVector(s, _whitespace);
	}

	unsigned int Tokenizer::isDeliminator(string s)
	{
		return fitsPatternVector(s, _deliminator);
	}

	string Tokenizer::categorize(Token t)
	{
		for (vector<pair<string, Pattern> >::iterator i = categorizers.begin(); i != categorizers.end(); i++)
		{
			if (i->second.match(t.get()))
			{
				return i->first;
			}
		}

		return "";
	}

	string Tokenizer::current(string s, unsigned int i)
	{
		string rtn;
		for (unsigned int j = i; j < i+10 && j < s.size(); j++)
		{
			if (s[j] == '\n')
				rtn += "\\n";
			else if (s[j] == '\t')
				rtn += "\\t";
			else
				rtn += s[j];
		}
		return rtn;
	}

	void Tokenizer::calcLocation(vector<unsigned int> ll, int off, Token &rtn)
	{
		unsigned int line = 0, column = 0;
		for (vector<unsigned int>::iterator i = ll.begin(); i != ll.end(); i++)
		{
			off -= *i;
			if (off <= 0 && *i > 0)
			{
				column = off+*i+1;
				line = i-ll.begin()+1;
				break;
			}
			line++;
			off--;
		}

		rtn.setLine(line);
		rtn.setColumn(column);
	}

	Pattern Tokenizer::operator () ()
	{
		return Pattern();
	}

	vector<pair<string, Pattern> > Tokenizer::getCategorizers()
	{
		return categorizers;
	}

	Tokenizer &Tokenizer::noDelim(Pattern ps, Pattern pe)
	{
		pair<Pattern, Pattern> tmp;
		tmp.first = ps;
		tmp.second = pe;
		_noDelim.push_back(tmp);
		return *this;
	}

	Tokenizer &Tokenizer::skip(Pattern ps, Pattern pe)
	{
		pair<Pattern, Pattern> tmp;
		tmp.first = ps;
		tmp.second = pe;
		_skip.push_back(tmp);
		return *this;
	}

	Tokenizer &Tokenizer::whitespace(Pattern p)
	{
		appendPatternToVectorSorted(_whitespace, p);
		return *this;
	}

	Tokenizer &Tokenizer::deliminator(Pattern p)
	{
		appendPatternToVectorSorted(_deliminator, p);
		return *this;
	}

	Tokenizer &Tokenizer::categorizer(string t, Pattern p)
	{
		pair<string, Pattern> tmp;
		tmp.first = t;
		tmp.second = p;
		categorizers.push_back(tmp);
		return *this;
	}

	Tokenizer &Tokenizer::token(string t, Pattern p)
	{
		bool isdelim = false;
		for (vector<Pattern>::iterator i = _deliminator.begin(); i != _deliminator.end(); i++)
		{
			if (i->compare(p))
			{
				isdelim = true;
				break;
			}
		}

		if (isdelim == false)
			deliminator(p);

		return categorizer(t, p);
	}

	Tokenizer &Tokenizer::combine(Pattern pa, Pattern pb)
	{
		pair<Pattern, Pattern> tmp;
		tmp.first = pa;
		tmp.second = pb;
		combinators.push_back(tmp);
		return *this;
	}

	string Tokenizer::readFile(string path)
	{
		TRACE_COUT << "reading " << path << "\n";
		ifstream f(path.c_str());
		if (f.good() == false)
		{
			cerr << "error: cannot open " << path << "\n";
			_exit(1);
		}

		string s((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		if (s.size() <= 1)
		{
			cerr << "error: " << path << " is empty\n";
			_exit(1);
		}

		return s;
	}

	vector<Token> Tokenizer::tokenize(string s, string f)
	{
		TRACE_INDATA("'" << s << "', from '" << f << "'");
		vector<Token> rtn;
		unsigned int last = 0;

		vector<unsigned int> lineLengths;
		unsigned int ll = 0;
		for (string::iterator i = s.begin(); i != s.end(); i++)
		{
			if (*i == '\n' || *i == '\r')
			{
				lineLengths.push_back(ll);
				ll = 0;
			}
			else
			{
				ll++;
			}
		}

		for (unsigned int i = 0; i < s.size(); i++)
		{
			unsigned int toksize = 0;
			
			for (vector<pair<Pattern, Pattern> >::iterator j = _noDelim.begin(); j != _noDelim.end(); j++)
			{
				if ((toksize = j->first.match(s.substr(i))))
				{
					TRACE_COUT << "found the beginning of a no-delimination segment (" << TRACE_YELLOW << j->first.display() << TRACE_DEFAULT << " - " << TRACE_YELLOW << j->second.display() << TRACE_DEFAULT << ")... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
					if (i-last > 0)
					{
						Token tmp = Token(s.substr(last, i-last), 0, 0);
						calcLocation(lineLengths, last, tmp);
						tmp.setType(categorize(tmp));
						tmp.setFile(f);
						rtn.push_back(tmp);
						TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
					}

					last = i;
					i += toksize;

					for (; i < s.size(); i++)
					{
						if ((toksize = j->second.match(s.substr(i))))
						{
							TRACE_COUT << "found the end of the no-delimination segment... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
							i += toksize;
							Token tmp = Token(s.substr(last, i-last), 0, 0);
							calcLocation(lineLengths, last, tmp);
							tmp.setType(categorize(tmp));
							tmp.setFile(f);
							rtn.push_back(tmp);
							last = i;
							TRACE_COUT << "  appending segment token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
							break;
						}
					}

					break;
				}
			}
			
			for (vector<pair<Pattern, Pattern> >::iterator j = _skip.begin(); j != _skip.end(); j++)
			{
				if ((toksize = j->first.match(s.substr(i))))
				{
					TRACE_COUT << "found the beginning of a skip segment (" << TRACE_YELLOW << j->first.display() << TRACE_DEFAULT << " - " << TRACE_YELLOW << j->second.display() << TRACE_DEFAULT << ")... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
					if (i-last > 0)
					{
						Token tmp = Token(s.substr(last, i-last), 0, 0);
						calcLocation(lineLengths, last, tmp);
						tmp.setType(categorize(tmp));
						tmp.setFile(f);
						rtn.push_back(tmp);
						TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
					}

					last = i+1;
					i += toksize;

					for (; i < s.size(); i++)
					{
						if ((toksize = j->second.match(s.substr(i))))
						{
							TRACE_COUT << "found the end of the skip segment... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
							last = i+toksize;
							i += toksize;
							break;
						}
					}

					break;
				}
			}

			if ((toksize = isDeliminator(s.substr(i))))
			{
				TRACE_COUT << "found a deliminator (token - " << TRACE_YELLOW << s.substr(i, toksize) << TRACE_DEFAULT << ", size - " << toksize << ")... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
				if (i-last > 0)
				{
					Token tmp = Token(s.substr(last, i-last), 0, 0);
					calcLocation(lineLengths, last, tmp);
					tmp.setType(categorize(tmp));
					tmp.setFile(f);
					rtn.push_back(tmp);
					TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
				}
				Token tmp = Token(s.substr(i, toksize), 0, 0);
				calcLocation(lineLengths, i, tmp);
				tmp.setType(categorize(tmp));
				tmp.setFile(f);
				rtn.push_back(tmp);
				i += toksize-1;
				last = i+1;
				TRACE_COUT << "  appending deliminator token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
			}
			else if ((toksize = isWhitespace(s.substr(i))))
			{
				TRACE_COUT << "found a piece of whitespace (size - " << toksize << ")... '" << TRACE_GREEN << current(s, i) << TRACE_DEFAULT << "'\n";
				if (i-last > 0)
				{
					Token tmp = Token(s.substr(last, i-last), 0, 0);
					calcLocation(lineLengths, last, tmp);
					tmp.setType(categorize(tmp));
					tmp.setFile(f);
					rtn.push_back(tmp);
					TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
				}
				last = i+toksize;
			}
		}

		for (int i = 1; i < rtn.size(); i++)
		{
			for (vector<pair<Pattern, Pattern> >::iterator j = combinators.begin(); j != combinators.end(); j++)
			{
				if (j->first.match(rtn[i-1].get()) && j->second.match(rtn[i].get()))
				{
					TRACE_COUT << "combining '" << TRACE_RED << rtn[i-1].get() << TRACE_DEFAULT << "' and '" << TRACE_RED << rtn[i].get() << TRACE_DEFAULT << "'\n";
					rtn[i-1].set(rtn[i-1].get()+rtn[i].get());
					rtn.erase(rtn.begin()+i);
					i--;
				}
			}
		}

		string rslt;
		for (vector<Token>::iterator i = rtn.begin(); i != rtn.end(); i++)
			rslt +=  " '"+i->get()+"'";
		TRACE_OUTDATA(rslt);

		return rtn;
	}

	vector<Token> Tokenizer::tokenizeFile(string path)
	{
		return tokenize(readFile(path), path);
	}

	string Tokenizer::operator [] (string s)
	{
		for (vector<pair<string, Pattern> >::iterator i = categorizers.begin(); i != categorizers.end(); i++)
		{
			if ((i->first.compare(s) == 0 && i->second.attachmentSize() == 0) && i->second.getAlgorithm() == Pattern::isEqualTo)
			{
				return i->second.getArgument();
			}
		}

		return "";
	}
}
