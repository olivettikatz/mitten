#include "rdp.h"

namespace parsing
{
	vector<ASTE>::iterator RDP::findElement(string name)
	{
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
			if (i->getName().compare(name) == 0)
				return i;
		return content.end();
	}

	AST RDP::parse(AST ast, ASTE p, unsigned int l, unsigned int &ci)
	{
		TRACE_INDATA(TRACE_GREEN << p.getName() << TRACE_DEFAULT << " (from " << ci << " to " << (p.size() < ast.size() ? p.size() : ast.size()) << ") " << ast.displaySome(2));
		//if (debug)
		//	cout << "[Mitten RDP] " << pad(l) << "\033[0;32m" << p.getName() << "\033[0;0m (from " << ci << " to " << (p.size() < ast.size() ? p.size() : ast.size()) << ") " << ast.displaySome(2) << "\n";

		if (p.size() == 0)
		{
			//cout << "[Mitten RDP] Error - empty pattern '" << p.getName() << "'\n";
			throw runtime_error("empty pattern '"+p.getName()+"'");
		}

		if (ast.size() == 0 || ci >= (p.size() < ast.size() ? p.size() : ast.size())-1)
		{
			TRACE_COUT << "cannot parse leaf at this level, returning...\n";
			TRACE_OUTDATA(ast.displaySome(2));
			//if (debug)
			//	cout << "[Mitten RDP] " << pad(l) << "cannot parse leaf at this level, returning...\n";
			return ast;
		}

		ast.setName(p.getName());
		bool fixed = true;

		unsigned int i;
		for (i = ci; i < p.size() && i < ast.size(); i++)
		{
			if (p[i].getExpectationType() == ASTE::_name)
			{
				vector<ASTE>::iterator pi = findElement(p[i].getArgument());
				if (pi == content.end())
				{
					//cout << "[Mitten RDP] Error - no such pattern '" << p[i].getArgument() << "'\n";
					throw runtime_error("no such pattern '"+p[i].getArgument()+"'");
				}
				else
				{
					if (ast[i].getStatus() < AST::statusRDP)
					{
						ast[i] = parse(ast[i], *pi, l+1, i);
						TRACE_INDATA(ast[i].display());
					}
					ast.error(ast[i].getErrors());
					ast[i].setStatus(AST::statusRDP);
				}
			}
			else if (p[i].getExpectationType() == ASTE::_type)
			{
				if (ast[i].getContent().getType().compare(p[i].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					//if (debug)
					//	cout << "[Mitten RDP] " << pad(l) << "\033[0;31m" << p[i].getName() << "\033[0;0m failed - " << ast[i].displaySome(1) << "\n";;
					ast.error(ASTError(ast[i].getContent(), "expected '"+p[i].getArgument()+"', but got '"+ast[i].getContent().getType()+"'"));
					ast[i].setStatus(AST::statusRDP);
					//if (debug)
					//	cout << ast.dumpErrors();
				}
				else if (ast[i].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					if (debug)
						cout << "[Mitten RDP] " << pad(l) << "\033[0;31m" << p[i].getName() << "\033[0;0m failed - " << ast[i].displaySome(1) << "\n";
					ast.error(ASTError(ast[i].getContent(), "expected a leaf, but got a branch with now-orphaned children"));
					ast[i].setStatus(AST::statusRDP);
					//if (debug)
					//	cout << ast.dumpErrors();
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					//if (debug)
					//	cout << "[Mitten RDP] " << pad(l) << "\033[0;32m" << p[i].getName() << "\033[0;0m succeeded.\n";
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
					//if (debug)
					//	cout << "[Mitten RDP] " << pad(l) << "\033[0;31m" << p[i].getName() << "\033[0;0m failed - " << ast[i].displaySome(1) << "\n";;
					ast.error(ASTError(ast[i].getContent(), "expected a scope, but got a leaf"));
					ast[i].setStatus(AST::statusRDP);
					//if (debug)
					//	cout << ast.dumpErrors();
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					//if (debug)
					//	cout << "[Mitten RDP] " << pad(l) << "\033[0;32m" << p[i].getName() << "\033[0;0m succeeded.\n";
					unsigned int ci2 = 0;
					if (ast[i].getStatus() < AST::statusRDP)
					{
						ast[i] = parse(ast[i], l+1, ci2);
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
			if (debug)
			{
				TRACE_COUT << "working tree - " << ast.display() << "\n";
				//cout << "[Mitten RDP] " << pad(l) << "working tree - " << ast.display() << "\n";
				TRACE_COUT << "more tokens to parse, starting with " << ast[i].displaySome(2) << "\n";
				//cout << "[Mitten RDP] " << pad(l) << "more tokens to parse in this scope segment, starting with " << ast[i].displaySome(3) << "\n";
			}
			ast.setStatus(AST::statusRDP);
			ast = parse(ast, l+1, i);
			TRACE_INDATA(ast.display());
		}

		TRACE_OUTDATA(ast.display());
		//if (debug)
		//	cout << "[Mitten RDP] " << pad(l) << "returning " << ast.display() << "\n";

		ci = i;
		return ast;
	}

	AST RDP::parse(AST ast, unsigned int l, unsigned int &ci)
	{
		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].getStatus() < AST::statusRDP && ast[i].size() > 0)
			{
				unsigned int ci2 = 0;
				ast[i] = parse(ast[i], l+1, ci2);
				TRACE_INDATA(ast[i].display());
				ast.error(ast[i].getErrors());
			}
		}

		AST tmp;
		AST errtmp;
		bool anyPotentialMatches = false;
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
		{
			tmp = ast;

			TRACE_COUT << "trying " << TRACE_GREEN << i->getName() << TRACE_DEFAULT << " (" << (i-content.begin()) << "/" << content.size() << ", from " << ci << " to " << ast.size() << ") on " << ast.displaySome(2) << "\n";
			//if (debug)
			//	cout << "[Mitten RDP] " << pad(l) << " trying \033[0;32m" << i->getName() << "\033[0;0m (" << (i-content.begin()) << "/" << content.size() << ", from " << ci << " to " << ast.size() << ") on " << ast.displaySome(3) << "\n";

			//if (debug)
			//	if (tmp.sizeRecursive() != ast.sizeRecursive())
			//		cout << "[Mitten RDP] " << pad(l) << "detected orphaned nodes within \033[0;31m" << tmp.displaySome(3) << "\033[0;0m\n";

			if (tmp.getStatus() < AST::statusRDP)
			{
				tmp = parse(tmp, *i, l+1, ci);
				TRACE_INDATA(tmp.display());
			}

			if (tmp.containsNamedBranch())
				anyPotentialMatches = true;

			if (tmp.containsErrors())
			{
				TRACE_COUT << "contains errors, skipping... " << tmp.displaySome(2) << "\n";
				//cout << "[Mitten RDP] " << pad(l) << "contains errors, skipping... " << tmp.displaySome(3) << "\n";
				errtmp.error(tmp.getErrors());
			}
			else
			{
				TRACE_COUT << "have successful result.\n";
				TRACE_OUTDATA(tmp.display());
				//cout << "[Mitten RDP] " << pad(l) << "returning " << tmp.display() << "\n";
				tmp.setStatusRecursive(AST::statusRDP);
				return tmp;
			}
		}

		if (anyPotentialMatches)
		{
			TRACE_COUT << "match found with errors, returning errors...\n";
			TRACE_OUTDATA(ast.display());
			//if (debug)
			//	cout << "[Mitten RDP] " << pad(l) << "match found with errors, returning errors... " << tmp.displaySome(3) << "\n";
			ast.error(errtmp.getErrors());
			ast.error(ASTError(ast.getContent(), "error when parsing scope segment"));
			ast.setStatusRecursive(AST::statusRDP);
			return ast;
		}
		else
		{
			TRACE_COUT << "no match found, returning original structure...\n";
			TRACE_OUTDATA(ast.display());
			//if (debug)
			//	cout << "[Mitten RDP] " << pad(l) << "no match found, returning original structure... " << ast.displaySome(3) << "\n";
			ast.setStatusRecursive(AST::statusRDP);
			return ast;
		}
	}

	string RDP::pad(unsigned int l)
	{
		return string(l, ' ');
	}

	void RDP::enableDebugging()
	{
		debug = true;
	}

	void RDP::setPrecedence(string type, unsigned int level)
	{
		precedences[type] = level;
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
					break;
				}
			}
		}
	}

	AST RDP::parse(AST ast)
	{
		unsigned int ci = 0;
		return parse(ast, 0, ci);
	}
}
