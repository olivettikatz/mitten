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