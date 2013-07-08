#ifndef __MITTEN_PARSING_RDP_H
#define __MITTEN_PARSING_RDP_H

#include <stdexcept>
#include "scope.h"
#include "../../trace/trace.h"

namespace parsing
{
	using namespace std;
	using namespace trace;

	class RDP
	{
	private:
		map<string, unsigned int> precedences;
		vector<ASTE> content;
		
		vector<ASTE>::iterator findElement(string name);
		AST parse(AST ast, ASTE e, unsigned int &ci);
		AST parse(AST ast, unsigned int &ci);

	public:
		RDP() {}
		void setPrecedence(string type, unsigned int level);
		void addElement(ASTE e);
		AST parse(AST ast);
	};
}

#endif
