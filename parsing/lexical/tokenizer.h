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

#ifndef __KITTEN_PARSING_LEXICAL_TOKENIZER_H
#define __KITTEN_PARSING_LEXICAL_TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include "pattern.h"
#include "../../trace/trace.h"

namespace parsing
{
	using namespace std;

	/*! for storing token data. */
	class Token
	{
	private:
		string content;
		unsigned int line;
		unsigned int column;
		string type;
		string file;

	public:
		/*! initializes the token with no data. */
		Token() : line(0), column(0) {}

		/*! initializes the token with just a string value. */
		Token(string s) : content(s), line(0), column(0) {}

		/*! initializes the token with a string value and a location. 
		\param s string value
		\param l line number
		\param c column number
		*/
		Token(string s, unsigned int l, unsigned int c) : content(s), line(l), column(c) {}

		/*! initializes the token with a string value, a location, and a type string. 
		\param s string value
		\param l line number
		\param c column number
		\param t type string
		*/
		Token(string s, unsigned int l, unsigned int c, string t) : content(s), line(l), column(c), type(t) {}

		/*! initializes the token with a string value, a location, a type string, and a filename. 
		\param s string value
		\param l line number
		\param c column number
		\param t type string
		\param f file path
		*/
		Token(string s, unsigned int l, unsigned int c, string t, string f) : content(s), line(l), column(c), type(t), file(f) {}

		/*! sets the string value. 
		\return a reference to the current token
		*/
		Token &setContent(string s);

		/*! sets the line number of the location. 
		\return a reference to the current token
		*/
		Token &setLine(unsigned int l);

		/*! sets the column number of the location. 
		\return a reference to the current token
		*/
		Token &setColumn(unsigned int c);

		/*! sets the type. 
		\return a reference to the current token
		*/
		Token &setType(string t);

		/*! sets the filename. 
		\return a reference to the current token
		*/
		Token &setFile(string f);

		/*! checks if the token contains no data. 
		\return `true' if empty, `false' if not
		*/
		bool empty();

		/*! gets the string value. */
		string get();

		/*! sets the string value. */
		string set(string s);

		/*! gets the string value. */
		string getContent();

		/*! gets the line number of the location. */
		unsigned int getLine();

		/*! gets the column number of the location. */
		unsigned int getColumn();

		/*! gets the type. */
		string getType();

		/*! gets the filename. */
		string getFile();

		/*! compares two tokens. */
		bool operator == (Token other);

		/*! compares the token's string value. */
		bool operator == (string other);

		/*! inverse compares two tokens. */
		bool operator != (Token other);

		/*! inverse compares the token's string value. */
		bool operator != (string other);
	};

	/*! for performing tokenization of a string. */
	class Tokenizer
	{
	private:
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
		/*! initializes an empty tokenizer. */
		Tokenizer() {}

		/*! shortcut for creating new patterns. 
		\return the equivalent of `Pattern()'
		*/
		Pattern operator () ();

		/*! gets the list of categorizers (used internally). */
		vector<pair<string, Pattern> > getCategorizers();

		/*! adds two patterns as `noDelim' boundaries. 
		\param ps the start of the `noDelim' segment
		\param pe the end of the `noDelim' segment
		*/
		Tokenizer &noDelim(Pattern ps, Pattern pe);

		/*! adds two patterns as `skip' boundaries. 
		\param ps the start of the `skip' segment
		\param pe the end of the `skip' segment
		*/
		Tokenizer &skip(Pattern ps, Pattern pe);

		/*! adds a pattern as a whitespace identifier. */
		Tokenizer &whitespace(Pattern p);

		/*! adds a pattern as a deliminator identifier. */
		Tokenizer &deliminator(Pattern p);

		/*! adds a pattern as a categorizer for existing tokens. 
		\param t the name of the categorizer
		\param p the pattern to categorize by
		*/
		Tokenizer &categorizer(string t, Pattern p);

		/*! adds a pattern as both a deliminator and a categorizer. 
		\param t the name of the token
		\param p the pattern to tokenize by
		*/
		Tokenizer &token(string t, Pattern p);

		/*! adds two patterns as combinators. 
		\param pa the first pattern to combine by
		\param pb the second pattern to combine by
		*/
		Tokenizer &combine(Pattern pa, Pattern pb);

		/*! tokenizes a string with a given filename. */
		vector<Token> tokenize(string s, string f);

		/*! reads a file and calls `tokenize(s, path)' where `s' is the contents of the file. */
		vector<Token> tokenizeFile(string path);
	};
}

#endif
