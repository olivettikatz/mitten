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
		unsigned int sizeRecursive();
		AST &operator [] (unsigned int idx);
		string display(unsigned int l);
		string display();
		string displaySome(unsigned int l, unsigned int limit);
		string displaySome(unsigned int limit);
		string dumpErrors();
		AST flatten(vector<AST> &rtn);
		AST flatten();
	};

	class ASTE
	{
	public:
		typedef enum
		{
			_name,
			_type,
			_scope
		} expectationType;

	private:
		string name;
		expectationType type;
		string argument;
		vector<ASTE> children;

	public:
		ASTE() {}
		ASTE(string n) : name(n) {}
		ASTE(string n, string t) : name(n), type(_type), argument(t) {}
		ASTE(string n, expectationType t) : name(n), type(t) {}
		ASTE(string n, expectationType t, string a) : name(n), type(t), argument(a) {}
		bool empty();
		ASTE &add(ASTE a);
		ASTE &add(string n, string t);
		ASTE &add(string n);
		ASTE &operator << (ASTE a);
		ASTE &operator << (string n);
		string getName();
		string setName(string n);
		expectationType getExpectationType();
		expectationType setExpectationType(expectationType t);
		string getArgument();
		string setArgument(string a);
		unsigned int size();
		ASTE &operator [] (unsigned int l);
		string display(unsigned int l);
		string display();
	};
}

#endif
