#include "scope.h"

namespace parsing
{
	string ScopeParser::pad(unsigned int l)
	{
		return string(l, ' ');
	}

	void ScopeParser::bindScope(string b, string e)
	{
		bounds[b] = e;
	}

	AST ScopeParser::parse(vector<Token> p, unsigned int l)
	{
		AST rtn = AST();
		rtn.setStatus(AST::statusScope);
		map<string, unsigned int> starts;
		map<string, unsigned int> levels;
		unsigned int masterLevel = 0;

		string inp;
		for (vector<Token>::iterator i = p.begin(); i != p.end() && i < p.begin()+10; i++)
			inp += string(" '")+TRACE_GREEN+i->get()+TRACE_DEFAULT+"'";
		TRACE_INDATA(inp);

		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
		{
			if (bounds.find(i->getType()) != bounds.end())
			{
				TRACE_COUT << "starting bound " << TRACE_YELLOW << i->getType() << " " << TRACE_RED << i->get() << TRACE_DEFAULT << "\n";
			
				if (levels.find(bounds[i->getType()]) == levels.end())
				{
					levels[bounds[i->getType()]] = 1;
					starts[bounds[i->getType()]] = i-p.begin();
					TRACE_COUT << "  (level 1)\n";
				}
				else
				{
					levels[bounds[i->getType()]]++;
					TRACE_COUT << "  (level " << levels[bounds[i->getType()]] << ")\n";
				}

				masterLevel++;
			}
			else if (starts.find(i->getType()) != starts.end())
			{
				TRACE_COUT << "ending bound [" << starts[i->getType()] << ":" << (i-p.begin()) << "] after " << TRACE_YELLOW << i->getType() << " " << TRACE_RED << i->get() << TRACE_DEFAULT << "\n";
				
				levels[i->getType()]--;
				TRACE_COUT << "  (level " << levels[i->getType()] << ")\n";

				if (levels[i->getType()] == 0)
				{
					vector<Token> tmp(p.begin()+starts[i->getType()]+1, i);
					AST ast = parse(tmp, l+1);
					AST tmpast = AST(p[starts[i->getType()]]);
					tmpast.setStatus(AST::statusScope);
					rtn.add(tmpast);
					ast.setStatus(AST::statusScope);
					rtn.add(ast);
					tmpast = AST(*i);
					tmpast.setStatus(AST::statusScope);
					rtn.add(tmpast);
				}

				masterLevel--;
			}
			else if (masterLevel == 0)
			{
				AST tmpast = AST("", *i);
				tmpast.setStatus(AST::statusScope);
				rtn.add(tmpast);
			}
		}

		TRACE_OUTDATA(rtn.display());
		return rtn;
	}

	AST ScopeParser::parse(vector<Token> p)
	{
		return parse(p, 0);
	}
}