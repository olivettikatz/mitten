#include "scope.h"

namespace parsing
{
	void ScopeParser::bindScope(string b, string e)
	{
		bounds[b] = [e];
	}

	AST ScopeParser::parse(TokenPage p)
	{
		AST rtn = AST();
		map<string, unsigned int> starts;

		for (TokenPage::iterator i = p.begin(); i != p.end(); i++)
		{
			if (bounds.find(i->getType()) != bounds.end())
			{
				starts[bounds[i->getType()]] = i.getLocation();
			}
			else if (starts.find(i->getType()) != starts.end())
			{
				
			}
		}
	}
}