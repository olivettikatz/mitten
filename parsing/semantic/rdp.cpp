#include "rdp.h"

namespace parsing
{
	vector<ASTE>::iterator RDP::findPattern(string name)
	{
		for (vector<ASTE>::iterator i = patterns.begin(); i != patterns.end(); i++)
			if (i->getName().compare(name) == 0)
				return i;
		return patterns.end();
	}

	AST RDP::parse(AST ast, ASTE p, unsigned int l)
	{
		AST rtn = AST(p.getName());

		if (p.size() > ast.size())
		{
			rtn.error(ASTError(ast.getContent(), "scope segment is too long to be a "+p.getName()));
		}
		else if (p.size() < ast.size())
		{
			rtn.error(ASTError(ast.getContent(), "scope segment is too short to be a "+p.getName()));
		}
		else
		{
			for (unsigned int i = 0; i < p.size(); i++)
			{
				if (p[i].getExpectationType() == ASTE::_name)
				{
					vector<ASTE>::iterator pi = findPattern(p[i].getArgument());
					if (pi == patterns.end())
					{
						cout << "[Mitten RDP] Error - no such pattern '" << p[i].getArgument() << "'\n";
						throw runtime_error("no such pattern");
					}
					else
					{
						rtn << parse(ast[i], *pi, l+1);
					}
				}
				else if (p[i].getExpectationType() == ASTE::_type)
				{
					if (ast[i].getContent().getType().compare(p[i].getArgument()) != 0)
					{
						rtn.error(ASTError(ast[i].getContent(), "expected '"+p.getArgument()+"', but got '"+ast[i].getContent().getType()+"'"));
					}
					else if (ast[i].size() > 0)
					{
						rtn.error(ASTError(ast[i].getContent(), "expected a leaf, but got a branch with now-orphaned children"));
					}
					else
					{
						AST tmp = AST(p[i].getName(), ast[i].getContent());
						rtn << tmp;
					}
				}
				else if (p[i].getExpectationType() == ASTE::_scope)
				{
					if (ast[i].size() == 0)
					{
						rtn.error(ASTError(ast[i].getContent(), "expected a scope beginning with '"+p[i].getArgument()+"', but got a leaf"));
					}
					else if (ast[i][0].getContent().getType().compare(p[i].getArgument()) != 0)
					{
						rtn.error(ASTError(ast[i].getContent(), "expected a scope beginning with '"+p[i].getArgument()+"', but got one beginning with '"+ast[i][0].getContent().getType()+"'"));
					}
					else
					{
						AST tmp = parse(ast[i], l+1);
						tmp.setName(p[i].getName());
						rtn << tmp;
					}
				}
			}
		}

		return rtn;
	}

	AST RDP::parse(AST ast, unsigned int l)
	{
		AST rtn;
		for (vector<ASTE>::iterator i = patterns.begin(); i != patterns.end(); i++)
		{
			AST tmp = parse(ast, *i, l);
			if (tmp.containsErrors())
			{
				rtn.error(tmp.getErrors());
			}
			else
			{
				return tmp;
			}
		}

		rtn.error(ASTError(ast.getContent(), "error when parsing scope segment"));
		return rtn;
	}

	void RDP::setPrecedence(string type, unsigned int level)
	{
		precedences[type] = level;
	}

	void RDP::addPattern(ASTE p)
	{
		if (patterns.empty())
		{
			patterns.push_back(p);
		}
		else
		{
			for (vector<ASTE>::iterator i = patterns.begin(); i != patterns.end(); i++)
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