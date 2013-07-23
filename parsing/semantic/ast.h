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

#ifndef __MITTEN_PARSING_SEMANTIC_AST_H
#define __MITTEN_PARSING_SEMANTIC_AST_H

#include "../lexical/tokenizer.h"
#include "asterror.h"
#include <sstream>

namespace parsing
{
	using namespace std;

	/*! for storing abstract syntax tree data. */
	class AST
	{
	private:
		string name;
		Token content;
		vector<AST> children;
		vector<ASTError> errors;
		int status;

		string display(unsigned int l);
		AST flatten(vector<AST> &rtn);
		string displaySome(unsigned int l, unsigned int limit);

	public:
		const static int npos = -1;
		const static int statusRaw = 0;
		const static int statusScope = 1;
		const static int statusRDP = 2;

		/*! initializes an empty AST. */
		AST() : status(statusRaw) {}

		/*! initializes an AST with a name and a token content. */
		AST(string n, Token c) : name(n), content(c), status(statusRaw) {}

		/*! initializes an AST with only a name. */
		AST(string n) : name(n), status(statusRaw) {}

		/*! initializes an AST with only a token content. */
		AST(Token c) : content(c), status(statusRaw) {}

		void error(string msg);

		/*! adds an error. */
		void error(ASTError e);

		/*! adds multiple errors. */
		void error(vector<ASTError> e);

		void pullUpErrors();

		/*! returns `true' if there are errors. */
		bool containsErrors();

		/*! returns the errors in this AST. */
		vector<ASTError> getErrors();

		/*! returns `true' if the AST is empty. */
		bool empty();

		/*! adds an AST as a child. */
		AST &add(AST a);

		/*! creates a new AST With a token content and adds it as a child. */
		AST &add(Token t);

		/*! creates a new AST with a name and adds it as a child. */
		AST &add(string n);

		/*! appending algorithm for lower precedence ASTs. */
		AST &addLowerPrecedence(AST a);

		/*! adds an AST as a child at the beginning of the child vector. */
		AST &addAtBeginning(AST a);

		/*! adds an AST as a child. */
		AST &operator << (AST a);

		/*! creates a new AST with a token content and adds it as a child. */
		AST &operator << (Token t);

		/*! creates a new AST with a name and adds it as a child. */
		AST &operator << (string n);

		/*! removes a child at a certain index. */
		void remove(unsigned int idx);

		/*! removes all children after a certain index. */
		void removeAfter(unsigned int idx);

		/*! gets the name of the AST. */
		string getName();

		/*! sets the name of the AST. */
		string setName(string n);

		/*! gets the token content. */
		Token getContent();

		/*! sets the token content. */
		Token setContent(Token t);

		/*! gets the size of the children vector. */
		unsigned int size();

		/*! gets the recursive size of the AST (the number of AST objects in the current AST). */
		unsigned int sizeRecursive();

		/*! gets the AST child at a certain index. */
		AST &operator [] (unsigned int idx);

		/*! finds the index of the first leaf with content matching `c'. */
		int findLeafContent(string c);

		/*! finds the index of the first leaf with content matching `c' after `after'. */
		int findLeafContent(string c, int after);

		/*! finds the index of the first leaf with type matching `t'. */
		int findLeafType(string t);

		/*! finds the index of the first leaf with type matching `t' after `after'. */
		int findLeafType(string t, int after);

		/*! returns an AST with the same root as the current AST, but containing only children after index `start'. */
		AST subtree(int start);

		/*! returns an AST with the same root as the current AST, but containing only `length' children after index `start'. */
		AST subtree(int start, int length);

		/*! displays the current AST. */
		string display();

		/*! displays some of the current AST. */
		string displaySome(unsigned int limit);

		/*! displays the errors in the current AST. */
		string dumpErrors();

		/*! returns a flat version of the current AST. */
		AST flatten();

		/*! returns true if the current AST contains any named ASTs. */
		bool containsNamedBranch();

		/*! returns true if the current AST contains any ASTs with a status of `s' or higher. */
		bool containsStatusBranch(int s);

		/*! sets the status of the current AST. */
		int setStatus(int s);

		/*! recursively sets the status of the current AST and all of its children. */
		int setStatusRecursive(int s);

		/*! gets the status of the current AST. */
		int getStatus();
	};

	/*! for expecting ASTs in recursive-descent parsing. */
	class ASTE
	{
	public:
		/*! the differnt expectation types. */
		typedef enum
		{
			_name,
			_type,
			_scope,
			_tag
		} expectationType;

	private:
		string name;
		expectationType type;
		string argument;
		vector<ASTE> children;

	public:
		/*! initializes an empty ASTE. */
		ASTE() {}

		/*! initializes an ASTE with only a name. */
		ASTE(string n) : name(n) {}

		/*! initializes an ASTE with a name and an expected type. */
		ASTE(string n, string t) : name(n), type(_type), argument(t) {}

		/*! initializes an ASTE with a name and an expectation type. */
		ASTE(string n, expectationType t) : name(n), type(t) {}

		/*! initializes an ASTE with a name, an expectation type, and an argument. */
		ASTE(string n, expectationType t, string a) : name(n), type(t), argument(a) {}

		/*! returns `true' if the ASTE is empty. */
		bool empty();

		/*! adds an ASTE as a child. */
		ASTE &add(ASTE a);

		/*! creates a new ASTE with a name and an expected type and adds it as a child. */
		ASTE &add(string n, string t);

		/*! creates a new ASTE with a name and adds it as a child. */
		ASTE &add(string n);

		/*! adds an ASTE as a child. */
		ASTE &operator << (ASTE a);

		/*! creates a new ASTE with a name and adds it as a child. */
		ASTE &operator << (string n);

		/*! gets the name of the ASTE. */
		string getName();

		/*! sets the name of the ASTE. */
		string setName(string n);

		/*! gets the expectation type. */
		expectationType getExpectationType();

		/*! sets the expectation type. */
		expectationType setExpectationType(expectationType t);

		/*! gets the argument. */
		string getArgument();

		/*! sets the argument. */
		string setArgument(string a);

		/*! gets the size of the children vector. */
		unsigned int size();

		/*! gets the child at a certain index. */
		ASTE &operator [] (unsigned int l);

		string display(unsigned int l);

		/*! displays the ASTE. */
		string display();
	};
}

#endif
