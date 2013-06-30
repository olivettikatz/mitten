#include "scope.h"

namespace parsing
{
	string ScopeParser::pad(unsigned int l)
	{
		return string(l, ' ');
	}

	void ScopeParser::enableDebugging()
	{
		debug = true;
	}

	void ScopeParser::bindScope(string b, string e)
	{
		bounds[b] = e;
	}

	AST ScopeParser::parse(vector<Token> p, unsigned int l)
	{
		AST rtn = AST();
		map<string, unsigned int> starts;
		map<string, unsigned int> levels;
		unsigned int masterLevel = 0;

		if (debug)
		{
			cout << "[Mitten Scope Parser] " << pad(l) << "parsing";
			for (vector<Token>::iterator i = p.begin(); i != p.end() && i < p.begin()+10; i++)
				cout << " '\033[0;32m" << i->get() << "\033[0;0m'";
			cout << "...\n";
		}

		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
		{
			if (bounds.find(i->getType()) != bounds.end())
			{
				if (debug)
					cout << "[Mitten Scope Parser] " << pad(l) << "starting bound \033[0;33m" << i->getType() << " \033[0;31m" << i->get() << "\033[0;0m";
			
				if (levels.find(bounds[i->getType()]) == levels.end())
				{
					levels[bounds[i->getType()]] = 1;
					starts[bounds[i->getType()]] = i-p.begin();
					if (debug)
						cout << " (level 1)\n";
				}
				else
				{
					levels[bounds[i->getType()]]++;
					if (debug)
						cout << " (level " << levels[bounds[i->getType()]] << ")\n";
				}

				masterLevel++;
			}
			else if (starts.find(i->getType()) != starts.end())
			{
				if (debug)
					cout << "[Mitten Scope Parser] " << pad(l) << "ending bound [" << starts[i->getType()] << ":" << (i-p.begin()) << "] after \033[0;33m" << i->getType() << " \033[31m" << i->get() << "\033[0;0m";
				
				levels[i->getType()]--;
				if (debug)
					cout << " (level " << levels[i->getType()] << ")\n";

				if (levels[i->getType()] == 0)
				{
					vector<Token> tmp(p.begin()+starts[i->getType()]+1, i);
					AST ast = parse(tmp, l+1);
					rtn.add(ast);
				}

				masterLevel--;
			}
			else if (masterLevel == 0)
			{
				rtn.add(AST("", *i));
			}
		}

		if (debug)
			cout << "[Mitten Scope Parser] " << pad(l) << "returning " << rtn.display() << "\n";
		return rtn;
	}

	AST ScopeParser::parse(vector<Token> p)
	{
		return parse(p, 0);
	}
}