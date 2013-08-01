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

#ifndef __MITTEN_PARSING_SEMANTIC_ENVIRONMENT_H
#define __MITTEN_PARSING_SEMANTIC_ENVIRONMENT_H

#include <stdexcept>
#include "scope.h"
#include <trace/trace.h>

namespace parsing
{
	using namespace std;
	using namespace trace;

	class Environment
	{
	private:
		map<string, vector<vector<ASTError> > > errorStacks;
		void debugStacks();

	public:
		Environment() {}
		void error(AST ast, string m);
		void error(string file, AST ast, string m);
		void error(ASTError e);
		void pushErrors(string file);
		void pushErrors(AST ast);
		vector<ASTError> popErrors(string file);
		vector<ASTError> popErrors(AST ast);
		string dumpErrors(string file);
		string dumpErrors(AST ast);
		void mergeErrors(string file);
		void mergeErrors(string file, string dst);
		void mergeErrors(string file, AST dst);
		void mergeErrors(AST ast);
		void mergeErrors(AST ast, string dst);
		void mergeErrors(AST ast, AST dst);
		bool areErrors(string file);
		bool areErrors(AST ast);
	};

	extern Environment environment;
}

#endif