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

#ifndef __MITTEN_MC_COMPILE_H
#define __MITTEN_MC_COMPILE_H

#include <iostream>
#include <parsing/semantic/ast.h>
#include <parsing/semantic/asterror.h>
#include "core/vm.h"

namespace mc
{
	using namespace std;

	class Compiler
	{
	private:
		typedef enum
		{
			_null,
			_value,
			_function
		} llvmObjectType;

		typedef struct
		{
			llvmObjectType type;
			llvm::Value *value;
			llvm::Function *function;
		} llvmObject;

		llvmObject llvmObjectNull();

		map<string, int> types;
		map<string, int> symbols;
		vector<parsing::ASTError> errors;

		vector<parsing::AST> tokenizeAST(parsing::AST ast, string delim);
		bool isValueToken(parsing::Token t);
		bool isSymbolToken(parsing::Token t);

	public:
		Compiler();
		int addType(string n);
		int addSymbol(string n);
		llvmObject compile(parsing::AST ast);
		int errorNumber();
		string dumpErrors();
	};
}

#endif