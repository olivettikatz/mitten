#ifndef __MITTEN_PARSING_RDP_H
#define __MITTEN_PARSING_RDP_H

#include "scope.h"

namespace parsing
{
	using namespace std;

	class RDP
	{
	private:
		map<string, unsigned int> precedences;
		vector<AST> patterns;
		
		AST parse(AST ast, unsigned int l);

	public:
		RDP() {}
		void setPrecedence(string type, unsigned int level);
		void addPattern(AST p);
		AST parse(AST ast);
	};
}

#endif