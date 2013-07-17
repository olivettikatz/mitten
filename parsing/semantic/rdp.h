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

#ifndef __MITTEN_PARSING_RDP_H
#define __MITTEN_PARSING_RDP_H

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
	private:
		map<string, unsigned int> precedences;
		vector<ASTE> content;
		
		vector<vector<ASTE>::iterator> findElements(string name);
		AST parse(AST &ast, ASTE e, unsigned int &ci);
		AST parse(AST &ast, unsigned int &ci);

	public:
		/*! initializes an empty parser. */
		RDP() {}

		/*! sets the precedence of a token.
		\param content the content of the token of which to set the precedence
		\param level the precedence level (from 1 to infinity)
		*/
		void setPrecedence(string content, unsigned int level);

		/*! gets the precedence of a token with a certain content. */
		unsigned int getPrecedence(string content);

		/*! adds a new ASTE. */
		void addElement(ASTE e);

		/*! performs the actual parsing. */
		AST parse(AST ast);
	};
}

#endif
