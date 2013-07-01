	#ifndef __KITTEN_PARSING_SEMANTIC_AST_H
#define __KITTEN_PARSING_SEMANTIC_AST_H

#include "../lexical/tokenizer.h"
#include "asterror.h"
#include <sstream>

namespace parsing
{
	using namespace std;

	class AST
	{
	private:
		string name;
		Token content;
		vector<AST> children;
		vector<ASTError> errors;

	public:
		AST() {}
		AST(string n, Token c) : name(n), content(c) {}
		AST(string n) : name(n) {}
		AST(Token c) : content(c) {}
		void error(ASTError e);
		void error(vector<ASTError> e);
		bool containsErrors();
		vector<ASTError> getErrors();
		bool empty();
		AST &add(AST a);
		AST &add(Token t);
		AST &add(string n);
		AST &operator << (AST a);
		AST &operator << (Token t);
		AST &operator << (string n);
		string getName();
		string setName(string n);
		Token getContent();
		Token setContent(Token t);
		unsigned int size();
		AST &operator [] (unsigned int idx);
		string display(unsigned int l);
		string display();
		string dumpErrors();
	};
}

#endif
