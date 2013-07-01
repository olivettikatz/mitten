#ifndef __MITTEN_PARSING_RDP_H
#define __MITTEN_PARSING_RDP_H

#include <stdexcept>
#include "scope.h"

namespace parsing
{
	using namespace std;

	class RDP
	{
	private:
		map<string, unsigned int> precedences;
		vector<ASTE> patterns;
		
		vector<ASTE>::iterator findPattern(string name);
		AST parse(AST ast, ASTE p, unsigned int l);
		AST parse(AST ast, unsigned int l);

	public:
		RDP() {}
		void setPrecedence(string type, unsigned int level);
		void addPattern(ASTE p);
		AST parse(AST ast);
	};
}

#endif