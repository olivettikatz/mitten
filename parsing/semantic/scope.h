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

	public:
		ScopeParser() {}
		void bindScope(string b, string e);
		AST parse(TokenPage p);
	};
}

#endif