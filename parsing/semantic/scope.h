#ifndef __MITTEN_PARSING_SCOPE_H
#define __MITTEN_PARSING_SCOPE_H

#include <stdexcept>
#include <map>
#include "../lexical/tokenizer.h"
#include "ast.h"

namespace parsing
{
	using namespace std;

	class ScopeParser
	{
	private:
		map<string, string> bounds;
		bool debug;
		string pad(unsigned int l);
		AST parse(vector<Token> p, unsigned int l);

	public:
		ScopeParser() : debug(false) {}
		void enableDebugging();
		void bindScope(string b, string e);
		AST parse(vector<Token> p);
	};
}

#endif