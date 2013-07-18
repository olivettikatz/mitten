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

	void CharStream::push(string s)
	{
		pthread_mutex_unlock(mutex);
		pthread_cond_signal(cond);
		buffer += s;
	}

	bool CharStream::push(ifstream f, size_t s)
	{
		if (f.is_open() && f.good())
		{
			char *tmp = new char[s];
			f.read(tmp, s);
			push(string(tmp));
			return true;
		}
		else
		{
			return false;
		}
	}

	string CharStream::peek(size_t s)
	{
		pthread_cond_wait(cond, mutex);

		if (s > buffer.size())
		{
			return buffer;
		}
		else
		{
			return buffer.substr(0, s);
		}
	}

	string CharStream::pop(size_t s)
	{
		string rtn = peek(s);
		if (s > buffer.size())
		{
			buffer.clear();
		}
		else
		{
			buffer = buffer.substr(s);
		}
		
		if (buffer.empty())
		{
			pthread_mutex_lock(mutex);
		}
		
		return rtn;
	}

	char CharStream::pop()
	{
		string tmp = pop(1);
		if (tmp.empty())
			return 0;
		else
			return tmp[0];
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

	extern "C" void *tokenizeAsyncWrapper(void *ptr)
	{
		Tokenizer *tok = (Tokenizer *)ptr;
		tok->tokenizeAsync();
		return NULL;
	}

	void Tokenizer::tokenizeAsync()
	{
		unsigned int blocksize = 1024;
		unsigned int maxsize = 0;

		for (vector<pair<Pattern, Pattern> >::iterator i = _noDelim.begin(); i != _noDelim.end(); i++)
		{
			if (i->first.getLengthRequest() > maxsize)
				maxsize = i->first.getLengthRequest();
			if (i->second.getLengthRequest() > maxsize)
				maxsize = i->second.getLengthRequest();
		}

		for (vector<pair<Pattern, Pattern> >::iterator i = _skip.begin(); i != _skip.end(); i++)
		{
			if (i->first.getLengthRequest() > maxsize)
				maxsize = i->first.getLengthRequest();
			if (i->second.getLengthRequest() > maxsize)
				maxsize = i->second.getLengthRequest();
		}

		for (vector<Pattern>::iterator i = _whitespace.begin(); i != _whitespace.end(); i++)
		{
			if (i->getLengthRequest() > maxsize)
				maxsize = i->getLengthRequest();
		}

		for (vector<Pattern>::iterator i = _deliminator.begin(); i != _deliminator.end(); i++)
		{
			if (i->getLengthRequest() > maxsize)
				maxsize = i->getLengthRequest();
		}

		if (maxsize >= blocksize)
			blocksize += maxsize;

		TRACE_INDATA("'" << _cs.peek(20) << "', from '" << _f << "'");
		unsigned int last = 0;
		unsigned int line = 0;
		unsigned int column = 0;

		string buffer = _cs.pop(blocksize);
		for (unsigned int i = 0; i < buffer.size(); i++)
		{
			if (buffer[i] == MITTEN_EOT)
				break;

			if (buffer.size()-i <= maxsize)
				buffer += _cs.pop(blocksize);

			unsigned int toksize = 0;
			
			for (vector<pair<Pattern, Pattern> >::iterator j = _noDelim.begin(); j != _noDelim.end(); j++)
			{
				if ((toksize = j->first.match(buffer.substr(i))))
				{
					TRACE_COUT << "found the beginning of a no-delimination segment (" << TRACE_YELLOW << j->first.display() << TRACE_DEFAULT << " - " << TRACE_YELLOW << j->second.display() << TRACE_DEFAULT << ")... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
					if (i-last > 0)
					{
						Token tmp = Token(buffer.substr(last, i-last), line, column);
						tmp.setType(categorize(tmp));
						tmp.setFile(_f);
						_rtn.push_back(tmp);
						TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
					}

					last = i;
					i += toksize;

					for (; i < buffer.size(); i++)
					{
						if ((toksize = j->second.match(buffer.substr(i))))
						{
							TRACE_COUT << "found the end of the no-delimination segment... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
							Token tmp = Token(buffer.substr(last, i-last+toksize), line, column);
							tmp.setType(categorize(tmp));
							tmp.setFile(_f);
							_rtn.push_back(tmp);
							i++;
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
				if ((toksize = j->first.match(buffer.substr(i))))
				{
					TRACE_COUT << "found the beginning of a skip segment (" << TRACE_YELLOW << j->first.display() << TRACE_DEFAULT << " - " << TRACE_YELLOW << j->second.display() << TRACE_DEFAULT << ")... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
					if (i-last > 0)
					{
						Token tmp = Token(buffer.substr(last, i-last), line, column);
						tmp.setType(categorize(tmp));
						tmp.setFile(_f);
						_rtn.push_back(tmp);
						TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
					}

					last = i+1;
					i += toksize;

					for (; i < buffer.size(); i++)
					{
						if ((toksize = j->second.match(buffer.substr(i))))
						{
							TRACE_COUT << "found the end of the skip segment... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
							last = i+toksize;
							i += toksize;
							break;
						}
					}

					break;
				}
			}

			if ((toksize = isDeliminator(buffer.substr(i))))
			{
				TRACE_COUT << "found a deliminator (token - " << TRACE_YELLOW << buffer.substr(i, toksize) << TRACE_DEFAULT << ", size - " << toksize << ")... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
				if (i-last > 0)
				{
					Token tmp = Token(buffer.substr(last, i-last), line, column);
					tmp.setType(categorize(tmp));
					tmp.setFile(_f);
					_rtn.push_back(tmp);
					TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
				}
				Token tmp = Token(buffer.substr(i, toksize), line, column);
				tmp.setType(categorize(tmp));
				tmp.setFile(_f);
				_rtn.push_back(tmp);
				i += toksize-1;
				last = i+1;
				TRACE_COUT << "  appending deliminator token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
			}
			else if ((toksize = isWhitespace(buffer.substr(i))))
			{
				TRACE_COUT << "found a piece of whitespace (size - " << toksize << ")... '" << TRACE_GREEN << current(buffer, i) << TRACE_DEFAULT << "'\n";
				if (i-last > 0)
				{
					Token tmp = Token(buffer.substr(last, i-last), line, column);
					tmp.setType(categorize(tmp));
					tmp.setFile(_f);
					_rtn.push_back(tmp);
					TRACE_COUT << "  appending last token... '" << TRACE_RED << tmp.get() << TRACE_DEFAULT << "'\n";
				}
				last = i+toksize;
			}

			if (buffer[i] == '\n')
			{
				column = 0;
				line++;
			}
			else
			{
				column++;
			}
		}

		for (int i = 1; i < _rtn.size(); i++)
		{
			for (vector<pair<Pattern, Pattern> >::iterator j = combinators.begin(); j != combinators.end(); j++)
			{
				if (j->first.match(_rtn[i-1].get()) && j->second.match(_rtn[i].get()))
				{
					TRACE_COUT << "combining '" << TRACE_RED << _rtn[i-1].get() << TRACE_DEFAULT << "' and '" << TRACE_RED << _rtn[i].get() << TRACE_DEFAULT << "'\n";
					_rtn[i-1].set(_rtn[i-1].get()+_rtn[i].get());
					_rtn.erase(_rtn.begin()+i);
					i--;
				}
			}
		}

		string rslt;
		for (vector<Token>::iterator i = _rtn.begin(); i != _rtn.end(); i++)
			rslt +=  " '"+i->get()+"'";
		TRACE_OUTDATA(rslt);
	}

	vector<Token> Tokenizer::tokenize(string s, string f)
	{
		pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t c = PTHREAD_COND_INITIALIZER;
		pthread_t t;
		void *(*func)(void*);
		func = (void *(*)(void*))&tokenizeAsyncWrapper;
		_cs = CharStream(&m, &c);
		_f = f;
		_rtn.clear();
		pthread_create(&t, NULL, func, (void *)this);
		pthread_join(t, NULL);
		return _rtn;
	}

	vector<Token> Tokenizer::tokenizeFile(string path)
	{
		ifstream f(path.c_str());
		string s((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
		return tokenize(s, path);
	}
}
