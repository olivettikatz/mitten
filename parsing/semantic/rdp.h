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

#ifndef __MITTEN_PARSING_SEMANTIC_RDP_H
#define __MITTEN_PARSING_SEMANTIC_RDP_H

#include <stdexcept>
#include "scope.h"
#include <trace/trace.h>

namespace parsing
{
	using namespace std;
	using namespace trace;

	/*! for recursive-descent parsing existing ASTs. */
	class RDP
	{
	public:
		/*! for auto-tagging. */
		typedef enum
		{
			_beginning,
			_end
		} relativeTo;

	private:
		map<string, unsigned int> precedences;
		vector<ASTE> content;
		map<string, string> tags;

		typedef struct
		{
			ASTE condition;
			relativeTo relative;
			int position;
			string tag;
		} tagCondition;

		vector<tagCondition> tagConditionList;
		void addTagsFromAST(AST ast);
		
		vector<vector<ASTE>::iterator> findElements(string name);
		AST parse(AST ast, ASTE e, unsigned int &ci);
		AST parse(AST ast, unsigned int &ci);
		bool verifyASTForParsing(AST &ast);
		AST reorganizeOnPrecedence(AST ast, AST subast, unsigned int prec);
		AST subparse(AST ast, ASTE e, unsigned int &i, unsigned int prec);
		AST subparse(AST ast, unsigned int &i, unsigned int prec);
		unsigned int getMaxPrecedence(AST ast);
		bool verifyExpectation(ASTE e);
		bool verifyExpectation(ASTE e, AST ast, unsigned int ci);

	public:
		/*! initializes an empty parser. */
		RDP() {}

		ASTE operator () ();
		ASTE operator () (string n, ASTE e);
		ASTE operator () (ASTE e);

		ASTE name(string n);
		ASTE type(string n, string t);
		ASTE scope(string n);
		ASTE tag(string n, string t);

		/*! sets the precedence of a token.
		\param content the content of the token of which to set the precedence
		\param level the precedence level (from 1 to infinity)
		*/
		void setPrecedence(string content, unsigned int level);

		/*! gets the precedence of a token with a certain content. */
		unsigned int getPrecedence(string content);

		/*! gets the precedence of a root token of an AST with a certian content. */
		unsigned int getPrecedence(AST content);

		/*! sets the tag of a symbol. */
		string setTag(string symbol, string tag);

		/*! gets the tag of a symbol if one exists, or an empty string if not. */
		string getTag(string symbol);

		/*! adds a new ASTE. */
		void addElement(ASTE e);

		/*! adds an auto-tagging condition.
		\param cond the condition to expect
		\param rel what `pos' is relative to
		\param pos the offset of the symbol to be tagged
		\param tag the tag to use
		*/
		void addTagCondition(ASTE cond, relativeTo rel, int pos, string tag);

		/*! performs the actual parsing. */
		AST parse(AST ast);
	};
}

#endif
