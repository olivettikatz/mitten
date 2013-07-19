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

#ifndef __MITTEN_PARSING_SEMANTIC_ASTERROR_H
#define __MITTEN_PARSING_SEMANTIC_ASTERROR_H

#include "../lexical/tokenizer.h"
#include <sstream>
#include <memory>

namespace parsing
{
	using namespace std;

	/*! for storing errors in parsing ASTs. */
	class ASTError
	{
	private:
		Token src;
		string msg;

	public:
		/*! initializes an empty error. */
		ASTError() {}

		/*! initializes an error with a source token and a message. */
		ASTError(Token s, string m) : src(s), msg(m) {}

		string format(string fmt);

		/*! returns a formatted string in a specified format. */
		string display(string fmt);

		/*! returns a formatted string in the default format. */
		string display();
	};
}

#endif