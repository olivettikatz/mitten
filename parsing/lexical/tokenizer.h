#ifndef __KITTEN_PARSING_LEXICAL_TOKENIZER_H
#define __KITTEN_PARSING_LEXICAL_TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include "pattern.h"

namespace parsing
{
	using namespace std;

	class Token
	{
	private:
		string content;
		unsigned int line;
		unsigned int column;
		string type;
		string file;

	public:
		Token() : line(0), column(0) {}
		Token(string s) : content(s), line(0), column(0) {}
		Token(string s, unsigned int l, unsigned int c) : content(s), line(l), column(c) {}
		Token(string s, unsigned int l, unsigned int c, string t) : content(s), line(l), column(c), type(t) {}
		Token(string s, unsigned int l, unsigned int c, string t, string f) : content(s), line(l), column(c), type(t), file(f) {}
		Token &setContent(string s);
		Token &setLine(unsigned int l);
		Token &setColumn(unsigned int c);
		Token &setType(string t);
		Token &setFile(string f);

		bool empty();

		string get();
		string set(string s);
		string getContent();
		unsigned int getLine();
		unsigned int getColumn();
		string getType();
		string getFile();

		bool operator == (Token other);
		bool operator == (string other);
		bool operator != (Token other);
		bool operator != (string other);
	};

	class TokenPage;

	class TokenPageIterator
	{
	private:
		TokenPage *page;
		unsigned int loc;

	public:
		TokenPageIterator();
		TokenPageIterator(TokenPage &p);
		TokenPage &getPage();
		unsigned int getLocation();
		Token &operator * ();
		Token *operator -> ();
		TokenPageIterator &operator ++ (int);
		bool isGood();
		void operator = (unsigned int i);
		bool operator == (TokenPageIterator i);
		bool operator != (TokenPageIterator i);
		bool operator < (TokenPageIterator i);
		bool operator < (unsigned int i);
		TokenPageIterator &operator + (unsigned int i);
		TokenPageIterator &operator + (TokenPageIterator i);
	};

	class TokenPage
	{
	public:
		typedef TokenPageIterator iterator;

	private:
		vector<Token> content;
		iterator off;

	public:
		TokenPage() : off(*this) {}
		void operator << (Token t);
		void operator >> (unsigned int i);
		unsigned int size();
		Token &operator [] (unsigned int i);
		void rewind();
		Token operator * ();
		iterator pullOffset();
		void pushOffset(unsigned int i);
		string display();
	};

	class Tokenizer
	{
	private:
		bool debug;

		vector<pair<Pattern, Pattern> > _noDelim;
		vector<pair<Pattern, Pattern> > _skip;
		vector<Pattern> _whitespace;
		vector<Pattern> _deliminator;
		vector<pair<string, Pattern> > categorizers;
		vector<pair<Pattern, Pattern> > combinators;

		void appendPatternToVectorSorted(vector<Pattern> &v, Pattern p);
		unsigned int fitsPatternVector(string s, vector<Pattern> v);
		unsigned int isWhitespace(string s);
		unsigned int isDeliminator(string s);
		string categorize(Token t);
		string current(string s, unsigned int i);

	public:
		Tokenizer() {}
		void enableDebugging();
		Pattern operator () ();
		vector<pair<string, Pattern> > getCategorizers();
		Tokenizer &noDelim(Pattern ps, Pattern pe);
		Tokenizer &skip(Pattern ps, Pattern pe);
		Tokenizer &whitespace(Pattern p);
		Tokenizer &deliminator(Pattern p);
		Tokenizer &categorizer(string t, Pattern p);
		Tokenizer &token(string t, Pattern p);
		Tokenizer &combine(Pattern pa, Pattern pb);

		unsigned int getMemorySize();

		TokenPage tokenize(string s, string f);
		TokenPage tokenizeFile(string path);
	};
}

#endif
