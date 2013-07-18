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

#ifndef __MITTEN_PARSING_SCOPE_H
#define __MITTEN_PARSING_SCOPE_H

#include <stdexcept>
#include <map>
#include "../lexical/tokenizer.h"
#include "ast.h"
#include <trace/trace.h>
#include <pthread.h>

namespace parsing
{
	using namespace std;

	class TokenStream
	{
	private:
		pthread_mutex_t *mutex;
		pthread_cond_t *cond;
		vector<Token> buffer;

	public:
		TokenStream() : mutex(NULL), cond(NULL) {}
		TokenStream(pthread_mutex_t *m, pthread_cond_t *c) : mutex(m), cond(c) {}
		void push();
	};

	/*! for parsing scopes in token vectors into ASTs. */
	class ScopeParser
	{
	private:
		map<string, string> bounds;
		string pad(unsigned int l);
		AST parse(vector<Token> p, unsigned int l);

	public:
		/*! initializes an empty parser. */
		ScopeParser() {}

		/*! creates a new scope.
		\param b the beginning token type of a scope
		\param e the end token type of that scope
		*/
		void bindScope(string b, string e);

		/*! performs actual parsing. */
		AST parse(vector<Token> p);
	};

	extern "C" void *parseAsyncWrapper(void *ptr);
}

#endif