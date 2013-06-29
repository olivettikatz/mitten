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
		string eid;
		Token content;
		vector<AST> children;
		vector<ASTError> errors;

	public:
		AST() {}
		AST(string e, Token c) : eid(e), content(c) {}
		void error(ASTError e);
		void error(vector<ASTError> e);
		bool containsErrors();
		vector<ASTError> getErrors();
		bool good();
		bool empty();
		AST &addAtBeginning(AST a);
		AST &add(AST a);
		string getExpectationID();
		Token getContent();
		unsigned int getChildrenSize();
		AST getChild(unsigned int idx);
		string display();
		string dumpErrors();
	};
}

#endif
