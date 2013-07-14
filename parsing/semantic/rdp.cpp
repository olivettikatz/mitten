#include "rdp.h"

namespace parsing
{
	vector<vector<ASTE>::iterator> RDP::findElements(string name)
	{
		vector<vector<ASTE>::iterator> rtn;
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
			if (i->getName().compare(name) == 0)
				rtn.push_back(i);
		return rtn;
	}

	AST RDP::parse(AST &ast, ASTE p, unsigned int &ci)
	{
		TRACE_INDATA(TRACE_GREEN << p.getName() << TRACE_DEFAULT << " (from " << ci << " to " << (p.size() < ast.size() ? p.size() : ast.size()) << ") " << ast.displaySome(2));

		if (p.size() == 0)
		{
			throw runtime_error("empty pattern '"+p.getName()+"'");
		}

		if (ast.size() == 0 || ci >= (p.size() < ast.size() ? p.size() : ast.size())-1)
		{
			TRACE_COUT << "cannot parse leaf at this level, returning...\n";
			TRACE_OUTDATA(ast.displaySome(2));
			return ast;
		}

		ast.setName(p.getName());
		bool fixed = true;
		unsigned int maxPrec = 0;

		unsigned int i;
		for (i = ci; i < p.size() && i < ast.size(); i++)
		{
			if (p[i].getExpectationType() == ASTE::_name)
			{
				vector<vector<ASTE>::iterator> pi = findElements(p[i].getArgument());
				if (pi.empty())
				{
					throw runtime_error("no such pattern '"+p[i].getArgument()+"'");
				}
				else
				{
					if (ast[i].getStatus() < AST::statusRDP)
					{
						bool haveSuccess = false;
						AST errtmp;

						for (vector<vector<ASTE>::iterator>::iterator j = pi.begin(); j != pi.end(); j++)
						{
							AST tmp = parse(ast[i], **j, i);
							TRACE_INDATA(tmp.display());

							if (tmp.containsErrors())
							{
								errtmp.error(tmp.getErrors());
							}
							else if (getPrecedence(ast.getContent().get()) >= getPrecedence(tmp.getContent().get()))
							{
								TRACE_COUT << "precedence of root is >= that of child, appending normally...\n";
								haveSuccess = true;
								ast[i] = tmp;
								ast.error(ast[i].getErrors());
								ast[i].setStatus(AST::statusRDP);
							}
							else
							{
								TRACE_COUT << "precedence of root is < that of child, appending stealing latest child...\n";
								haveSuccess = true;
								tmp.addAtBeginning(ast[i]);
								ast[i] = tmp;
								ast.error(ast[i].getErrors());
								ast[i].setStatus(AST::statusRDP);
							}
						}

						if (haveSuccess == false)
						{
							ast.error(errtmp.getErrors());
						}
					}
				}
			}
			else if (p[i].getExpectationType() == ASTE::_type)
			{
				if (ast[i].getContent().getType().compare(p[i].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - (" << p[i].getArgument() << " != " << ast[i].getContent().getType() << ") - " << ast[i].displaySome(2) << "\n";
					ast.error(ASTError(ast[i].getContent(), "expected '"+p[i].getArgument()+"', but got '"+ast[i].getContent().getType()+"'"));
					ast[i].setStatus(AST::statusRDP);
				}
				else if (ast[i].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					ast.error(ASTError(ast[i].getContent(), "expected a leaf, but got a branch with now-orphaned children"));
					ast[i].setStatus(AST::statusRDP);
				}
				else
				{
					if (getPrecedence(ast[i].getContent().get()) > maxPrec)
						maxPrec = getPrecedence(ast[i].getContent().get());
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					ast[i].setName(p[i].getName());
					ast[i].setStatus(AST::statusRDP);
				}
			}
			else if (p[i].getExpectationType() == ASTE::_scope)
			{
				if (ast[i].size() == 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					ast.error(ASTError(ast[i].getContent(), "expected a scope, but got a leaf"));
					ast[i].setStatus(AST::statusRDP);
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					unsigned int ci2 = 0;
					if (ast[i].getStatus() < AST::statusRDP)
					{
						TRACE_COUT << "parsing sub-scope - " << ast[i].display() << "\n";
						ast[i] = parse(ast[i], ci2);
						TRACE_INDATA(ast[i].display());
					}
					ast[i].setName(p[i].getName());
					ast.error(ast[i].getErrors());
					ast[i].setStatus(AST::statusRDP);
				}
			}
		}

		if (fixed == false)
		{
			ast.setName("");
		}
		else if (ast.containsErrors() == false && i < ast.size())
		{
			TRACE_COUT << "working tree - " << ast.display() << "\n";
			TRACE_COUT << "more tokens to parse, starting with " << ast[i].displaySome(2) << "\n";
			ast.setStatus(AST::statusRDP);
			ast = parse(ast, i);
			if (maxPrec < getPrecedence(ast[i].getContent().get()))
			{
				TRACE_COUT << "time to fix precedence (root " << maxPrec << ", child '" << ast[i].getContent().get() << "' " << getPrecedence(ast[i].getContent().get()) << ")...\n";
				AST tmp = AST();
				for (int j = i; j < ast.size(); j++)
					tmp.add(ast[j]);
				tmp.addAtBeginning(ast[i-1]);
				ast[i-1] = tmp;
				ast.removeAfter(i);
			}
			TRACE_INDATA(ast.display());
		}

		TRACE_OUTDATA(ast.display());

		ci = i;
		return ast;
	}

	AST RDP::parse(AST &ast, unsigned int &ci)
	{
		TRACE_COUT << "any-parsing " << ast.displaySome(3) << "\n";
		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].getStatus() < AST::statusRDP && ast[i].size() > 0)
			{
				unsigned int ci2 = 0;
				ast[i] = parse(ast[i], ci2);
				TRACE_INDATA(ast[i].display());
				ast.error(ast[i].getErrors());
			}
		}

		AST tmp;
		unsigned int tmpci;
		bool anyPotentialMatches = false;
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
		{
			tmp = ast;
			tmpci = ci;

			TRACE_COUT << "trying " << TRACE_GREEN << i->getName() << TRACE_DEFAULT << " (" << (i-content.begin()) << "/" << content.size() << ", from " << ci << " to " << ast.size() << ") on " << ast.displaySome(2) << "\n";

			if (tmpci < tmp.size() && tmp[tmpci].getStatus() < AST::statusRDP)
			{
				tmp = parse(tmp, *i, tmpci);
				TRACE_INDATA(tmp.display());
			}
			else if (tmpci >= tmp.size())
			{
				TRACE_COUT << "there aren't actually any more tokens to parse, skipping... (" << tmpci << " >= " << tmp.size() << ")\n";
				continue;
			}
			else
			{
				TRACE_COUT << "already parsed, skipping... (" << tmp[tmpci].getStatus() << " < " << AST::statusRDP << ")\n";
				continue;
			}

			if (tmp.containsNamedBranch())
				anyPotentialMatches = true;

			if (tmp.containsErrors())
			{
				TRACE_COUT << "contains errors, skipping... " << tmp.displaySome(2) << "\n";
			}
			else
			{
				TRACE_COUT << "have successful result.\n";
				TRACE_OUTDATA(tmp.display());
				tmp.setStatusRecursive(AST::statusRDP);
				return tmp;
			}
		}

		if (anyPotentialMatches)
		{
			TRACE_COUT << "match found with errors, returning errors...\n";
			TRACE_OUTDATA(ast.display());
			ast.setStatusRecursive(AST::statusRDP);
			return ast;
		}
		else
		{
			TRACE_COUT << "no match found, returning original structure...\n";
			TRACE_OUTDATA(ast.display());
			ast.setStatusRecursive(AST::statusRDP);
			return ast;
		}
	}

	void RDP::setPrecedence(string content, unsigned int level)
	{
		precedences[content] = level;
	}

	unsigned int RDP::getPrecedence(string content)
	{
		if (precedences.find(content) == precedences.end())
			return 0;
		else
			return precedences[content];
	}

	void RDP::addElement(ASTE e)
	{
		if (content.empty())
		{
			content.push_back(e);
		}
		else
		{
			for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
			{
				if (i->size() < e.size())
				{
					content.insert(i, e);
					return ;
				}
			}

			content.push_back(e);
		}
	}

	AST RDP::parse(AST ast)
	{
		unsigned int ci = 0;
		return parse(ast, ci);
	}
}
