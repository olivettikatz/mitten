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

	TokenPageIterator::TokenPageIterator()
	{
		loc = 0;
	}

	TokenPageIterator::TokenPageIterator(TokenPage &p)
	{
		page = &p;
		loc = 0;
	}

	TokenPage &TokenPageIterator::getPage()
	{
		return *page;
	}

	unsigned int TokenPageIterator::getLocation()
	{
		return loc;
	}

	Token &TokenPageIterator::operator * ()
	{
		return (*page)[loc];
	}

	Token *TokenPageIterator::operator -> ()
	{
		return &(*page)[loc];
	}

	TokenPageIterator &TokenPageIterator::operator ++ (int)
	{
		loc++;
		return *this;
	}

	bool TokenPageIterator::isGood()
	{
		if (loc >= page->size())
			return false;
		return true;
	}

	void TokenPageIterator::operator = (unsigned int i)
	{
		if (i < page->size())
			loc = i;
	}

	bool TokenPageIterator::operator == (TokenPageIterator i)
	{
		return (page == i.page) && (loc == i.loc);
	}

	bool TokenPageIterator::operator != (TokenPageIterator i)
	{
		return (page != i.page) || (loc != i.loc);
	}

	bool TokenPageIterator::operator < (unsigned int i)
	{
		return (loc < i);
	}

	bool TokenPageIterator::operator < (TokenPageIterator i)
	{
		return (page == i.page) && (loc < i.loc);
	}

	TokenPageIterator &TokenPageIterator::operator + (unsigned int i)
	{
		loc += i;
		return *this;
	}

	TokenPageIterator &TokenPageIterator::operator + (TokenPageIterator i)
	{
		if (page != i.page)
			return *this;

		loc += i.loc;
		return *this;
	}

	void TokenPage::operator << (Token t)
	{
		content.push_back(t);
		if (content.size() == 1)
			off = 0;
	}

	void TokenPage::operator >> (unsigned int i)
	{
		content.erase(content.begin()+i);
	}

	unsigned int TokenPage::size()
	{
		return content.size();
	}

	Token &TokenPage::operator [] (unsigned int i)
	{
		return content[i];
	}

	Token TokenPage::operator * ()
	{
		if (content.empty())
			return Token();
		return *off;
	}

	void TokenPage::rewind()
	{
		off = 0;
	}

	TokenPage::iterator TokenPage::pullOffset()
	{
		if (content.empty())
			off = 0;
		return off;
	}

	void TokenPage::pushOffset(unsigned int i)
	{
		off = i;
	}

	string TokenPage::display()
	{
		stringstream ss;
		for (vector<Token>::iterator i = content.begin(); i != content.end(); i++)
			ss << " '" << i->get() << "'";
		string rtn = ss.str();
		if (rtn.empty() == false && rtn[0] == ' ')
			rtn = rtn.substr(1);
		return rtn;
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

	void Tokenizer::enableDebugging()
	{
		debug = true;
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

	unsigned int Tokenizer::getMemorySize()
	{
		unsigned int rtn = 0;
		for (vector<pair<Pattern, Pattern> >::iterator i = _noDelim.begin(); i != _noDelim.end(); i++)
			rtn += i->first.getMemorySize()+i->second.getMemorySize();
		for (vector<pair<Pattern, Pattern> >::iterator i = _skip.begin(); i != _skip.end(); i++)
			rtn += i->first.getMemorySize()+i->second.getMemorySize();
		for (vector<Pattern>::iterator i = _whitespace.begin(); i != _whitespace.end(); i++)
			rtn += i->getMemorySize();
		for (vector<Pattern>::iterator i = _deliminator.begin(); i != _deliminator.end(); i++)
			rtn += i->getMemorySize();
		for (vector<pair<string, Pattern> >::iterator i = categorizers.begin(); i != categorizers.end(); i++)
			rtn += i->first.size()*sizeof(char)+i->second.getMemorySize();
		for (vector<pair<Pattern, Pattern> >::iterator i = combinators.begin(); i != combinators.end(); i++)
			rtn += i->first.getMemorySize()+i->second.getMemorySize();
		return rtn;
	}

	TokenPage Tokenizer::tokenize(string s, string f)
	{
		TokenPage rtn;
		unsigned int last = 0;
		unsigned int line = 0;
		unsigned int column = 0;

		for (unsigned int i = 0; i < s.size(); i++)
		{
			unsigned int toksize = 0;
			
			for (vector<pair<Pattern, Pattern> >::iterator j = _noDelim.begin(); j != _noDelim.end(); j++)
			{
				if ((toksize = j->first.match(s.substr(i))))
				{
					if (debug)
						cout << "[Mitten Tokenizer] found the beginning of a no-delimination segment (\033[0;33m" << j->first.display() << "\033[0;0m - \033[0;33m" << j->second.display() << "\033[0;0m)... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";
					if (i-last > 0)
					{
						Token tmp = Token(s.substr(last, i-last), line, column);
						tmp.setType(categorize(tmp));
						tmp.setFile(f);
						rtn << tmp;
						if (debug)
							cout << "  appending last token... '\033[0;31m" << tmp.get() << "\033[0;31m'\n";
					}

					last = i;
					i += toksize;

					for (; i < s.size(); i++)
					{
						if ((toksize = j->second.match(s.substr(i))))
						{
							if (debug)
								cout << "[Mitten Tokenizer] found the end of the no-delimination segment... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";
							Token tmp = Token(s.substr(last, i-last+toksize), line, column);
							tmp.setType(categorize(tmp));
							tmp.setFile(f);
							rtn << tmp;
							last = i+toksize;
							if (debug)
								cout << "  appending segment token... '\033[0;31m" << tmp.get() << "\033[0;0m'\n";
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
					if (debug)
						cout << "[Mitten Tokenizer] found the beginning of a skip segment (\033[0;33m" << j->first.display() << "\033[0;0m - \033[0;33m" << j->second.display() << "\033[0;0m)... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";
					if (i-last > 0)
					{
						Token tmp = Token(s.substr(last, i-last), line, column);
						tmp.setType(categorize(tmp));
						tmp.setFile(f);
						rtn << tmp;
						if (debug)
							cout << "  appending last token... '\033[0;31m" << tmp.get() << "\033[0;0m'\n";
					}

					last = i+1;
					i += toksize;

					for (; i < s.size(); i++)
					{
						if ((toksize = j->second.match(s.substr(i))))
						{
							if (debug)
								cout << "[Mitten Tokenizer] found the end of the skip segment... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";
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
				if (debug)
					cout << "[Mitten Tokenizer] found a deliminator (token - \033[0;33m" << s.substr(i, toksize) << "\033[0;0m, size - " << toksize << ")... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";;
				if (i-last > 0)
				{
					Token tmp = Token(s.substr(last, i-last), line, column);
					tmp.setType(categorize(tmp));
					tmp.setFile(f);
					rtn << tmp;
					if (debug)
						cout << "  appending last token... '\033[0;31m" << tmp.get() << "\033[0;0m'\n";
				}
				Token tmp = Token(s.substr(i, toksize), line, column);
				tmp.setType(categorize(tmp));
				tmp.setFile(f);
				rtn << tmp;
				i += toksize-1;
				last = i+1;
				if (debug)
					cout << "  appending deliminator token... '\033[0;31m" << tmp.get() << "\033[0;0m'\n";
			}
			else if ((toksize = isWhitespace(s.substr(i))))
			{
				if (debug)
					cout << "[Mitten Tokenizer] found a piece of whitespace (size - " << toksize << ")... '\033[0;32m" << current(s, i) << "\033[0;0m'\n";
				if (i-last > 0)
				{
					Token tmp = Token(s.substr(last, i-last), line, column);
					tmp.setType(categorize(tmp));
					tmp.setFile(f);
					rtn << tmp;
					if (debug)
						cout << "  appending last token... '\033[0;31m" << tmp.get() << "\033[0;0m'\n";
				}
				last = i+toksize;
			}

			if (s[i] == '\n')
			{
				column = 0;
				line++;
			}
			else
			{
				column++;
			}
		}

		for (int i = 1; i < rtn.size(); i++)
		{
			for (vector<pair<Pattern, Pattern> >::iterator j = combinators.begin(); j != combinators.end(); j++)
			{
				if (j->first.match(rtn[i-1].get()) && j->second.match(rtn[i].get()))
				{
					if (debug)
						cout << "[Mitten Tokenizer] combining '\033[0;31m" << rtn[i-1].get() << "\033[0;0m' and '\033[0;31m" << rtn[i].get() << "\033[0;0m'\n";
					rtn[i-1].set(rtn[i-1].get()+rtn[i].get());
					rtn >> i;
					i--;
				}
			}
		}

		if (debug)
			cout << "[Mitten Tokenizer] result: " << rtn.display() << "\n";

		return rtn;
	}

	TokenPage Tokenizer::tokenizeFile(string path)
	{
		ifstream f(path.c_str());
		string s((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		return tokenize(s, path);
	}
}
