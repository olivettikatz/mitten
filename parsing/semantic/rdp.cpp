#include "rdp.h"

namespace parsing
{
	AST RDP::parse(AST ast, unsigned int l)
	{
		
	}

	void RDP::setPrecedence(string type, unsigned int level)
	{
		precedences[type] = level;
	}

	void RDP::addPattern(AST p)
	{
		if (patterns.empty())
		{
			patterns.push_back(p);
		}
		else
		{
			for (vector<AST>::iterator i = patterns.begin(); i != patterns.end(); i++)
			{
				if (i->size() < p.size())
				{
					patterns.insert(i, p);
					break;
				}
			}
		}
	}

	AST RDP::parse(AST ast)
	{
		return parse(ast, 0);
	}
}