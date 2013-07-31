/*
 * This file is part of Mitten.
 * 
 * Mitten is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mitten is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mitten.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 Oliver Katz */

#include "rdp.h"

namespace parsing
{
	ASTE RDP::operator () ()
	{
		return ASTE();
	}

	ASTE RDP::operator () (string n, ASTE e)
	{
		ASTE root = ASTE(e.getName(), e.getExpectationType(), e.getArgument());

		ASTE tmp = name(n) << root;
		for (unsigned int i = 0; i < e.size(); i++)
			tmp << e[i];

		addElement(tmp);
		return e;
	}

	ASTE RDP::operator () (ASTE e)
	{
		addElement(e);
		return e;
	}

	ASTE RDP::name(string n)
	{
		return ASTE(n);
	}

	ASTE RDP::type(string n, string t)
	{
		return ASTE(n, t);
	}

	ASTE RDP::type(string nt)
	{
		return ASTE(nt, nt);
	}

	ASTE RDP::scope(string n)
	{
		return ASTE(n, ASTE::_scope);
	}

	ASTE RDP::tag(string n, string t)
	{
		return ASTE(n, ASTE::_tag, t);
	}

	ASTE RDP::tag(string nt)
	{
		return ASTE(nt, ASTE::_tag, nt);
	}

	void RDP::addTagsFromAST(AST ast)
	{
		for (unsigned int i = 0; i < ast.size()-1; i++)
		{
			for (vector<tagCondition>::iterator j = tagConditionList.begin(); j != tagConditionList.end(); j++)
			{
				unsigned int ci = i;
				AST tmp = parse(ast, j->condition, ci);
				if (tmp.containsErrors() == false)
				{
					if (j->relative == _beginning)
					{
						if (i+j->position >= ast.size())
							continue;
						TRACE_COUT << "tagged '" << ast[i+j->position].getContent().get() << "' (begin " << i << " + " << j->position << ") as '" << j->tag << "'\n";
						setTag(ast[i+j->position].getContent().get(), j->tag);
					}
					else if (j->relative == _end)
					{
						if (ci+j->position >= ast.size())
							continue;
						TRACE_COUT << "tagged '" << ast[ci+j->position].getContent().get() << "' (end " << ci << " + " << j->position << ") as '" << j->tag << "'\n";
						setTag(ast[ci+j->position].getContent().get(), j->tag);
					}

					break;
				}
			}
		}
	}

	vector<vector<ASTE>::iterator> RDP::findElements(string name)
	{
		vector<vector<ASTE>::iterator> rtn;
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
			if (i->getName().compare(name) == 0)
				rtn.push_back(i);
		return rtn;
	}

	AST RDP::parse(AST ast, ASTE p, unsigned int &ci)
	{
		TRACE_INDATA(TRACE_GREEN << p.getName() << TRACE_DEFAULT << " (from " << ci << " to " << (p.size() < ast.size() ? p.size() : ast.size()) << ") - " << ast.display());

		if (!verifyExpectation(p, ast, ci))
		{
			TRACE_COUT << "could not verify expectation, returning...\n";
			TRACE_OUTDATA(ast.display());
			ast.error("expectation '"+p.getName()+"' does not match this syntax");
			ast.setStatusRecursive(AST::statusRDP);
			return ast;
		}

		ast.setName(p.getName());
		bool fixed = true;
		unsigned int maxPrec = getMaxPrecedence(ast);

		unsigned int ai = ci, pi = 0;
		for (; pi < p.size(); pi++, ai++)
		{
			TRACE_COUT << "expecting " << p.getName() << "[" << pi << ":" << p.size() << "] " << p[pi].getName() << " from AST[" << ai << ":" << ast.size() << "]\n";

			if (ai >= ast.size())
			{
				TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " overflowed the current AST\n";
				ast.error("expectation "+p.getName()+" overflowed");
				fixed = false;
				break;
			}

			if (p[pi].getExpectationType() == ASTE::_name)
			{
				if (ast[ai].getName().compare(p[pi].getName()) == 0)
				{
					TRACE_COUT << TRACE_GREEN << p[pi].getName() << TRACE_DEFAULT << " succeeded.\n";
				}
				else
				{
					vector<vector<ASTE>::iterator> fi = findElements(p[pi].getName());

					TRACE_COUT << "  '"+p[pi].getName()+"' matches " << fi.size() << " expectations\n";
					bool haveSuccess = false;
					AST errtmp;

					for (vector<vector<ASTE>::iterator>::iterator j = fi.begin(); j != fi.end(); j++)
					{
						unsigned int oldai = ai;
						ast[oldai] = subparse(ast[ai], **j, ai, getPrecedence(ast));
						ast[oldai].pullUpErrors();
						if (ast[oldai].containsErrors() == false)
						{
							haveSuccess = true;
							break;
						}
					}

					if (haveSuccess == false)
					{
						TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " could not match the sub-expectation\n";
						ast.error(errtmp.getErrors());
						fixed = false;
					}
				}
			}
			else if (p[pi].getExpectationType() == ASTE::_type)
			{
				TRACE_COUT << "  "+p[pi].getName()+" expects type '"+p[pi].getArgument()+"', got '"+ast[ai].getContent().getType()+"' "+ast[ai].getContent().get() << "\n";
				if (ast[ai].getContent().getType().compare(p[pi].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " failed - (expectation '" << p[pi].getArgument() << "' != actual[" << ai << "] '" << ast[ai].getContent().getType() << "')\n";
					ast[ai].error("expected '"+p[pi].getArgument()+"', but got '"+ast[ai].getContent().getType()+"'");
					ast[ai].setStatus(AST::statusRDP);
				}
				else if (ast[ai].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " failed\n";
					ast.error("expected a leaf, but got a branch with now-orphaned children");
					ast[ai].setStatus(AST::statusRDP);
				}
				else if (getTag(ast[ai].getContent().get()).empty() == false)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " failed\n";
					ast.error("expected type '"+p[pi].getArgument()+"', but got tag '"+getTag(ast[ai].getContent().get())+"'");
					ast[ai].setStatus(AST::statusRDP);
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[pi].getName() << TRACE_DEFAULT << " succeeded.\n";
					ast[ai].setName(p[pi].getName());
					ast[ai].setStatus(AST::statusRDP);
				}
			}
			else if (p[pi].getExpectationType() == ASTE::_scope)
			{
				TRACE_COUT << "  "+p[pi].getName()+" expects scope\n";
				unsigned int ci2 = 0;
				ast[ai] = subparse(ast[ai], ci2, getPrecedence(ast));
				ast[ai].setName(p[pi].getName());
			}
			else if (p[pi].getExpectationType() == ASTE::_tag)
			{
				TRACE_COUT << "  "+p[pi].getName()+" expects tag '"+p[pi].getArgument()+"', got '"+getTag(ast[ai].getContent().get())+"' "+ast[ai].getContent().get() << "\n";
				if (getTag(ast[ai].getContent().get()).compare(p[pi].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " failed - (expectation '" << p[pi].getArgument() << "' != actual[" << ai << "] '" << getTag(ast[ai].getContent().get()) << "')\n";
					ast[ai].error("expected '"+p[pi].getArgument()+"', but got '"+getTag(ast[ai].getContent().get())+"'");
					ast[ai].setStatus(AST::statusRDP);
				}
				else if (ast[ai].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[pi].getName() << TRACE_DEFAULT << " failed\n";
					ast[ai].error("expected a leaf, but got a branch with now-orphaned children");
					ast[ai].setStatus(AST::statusRDP);
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[pi].getName() << TRACE_DEFAULT << " succeeded.\n";
					ast[ai].setName(p[pi].getName());
					ast[ai].setStatus(AST::statusRDP);
				}
			}
			else
			{
				TRACE_COUT << "unknown expectation type: " << p[pi].getExpectationType() << " (" << p.getName() << "[" << pi << "]: " << p[pi].getName() << ") - " << p.display() << "\n";
				throw runtime_error("unknown expectation type");
				return ast;
			}
		}

		ast.pullUpErrors();

		if (fixed == false)
		{
			ast.setName("");
		}
		else if (ast.containsErrors() == false && ai < ast.size())
		{
			TRACE_COUT << "more tokens to parse...\n";
			ast = parse(ast, ai);
			ast.setStatus(AST::statusRDP);
			if (maxPrec < getPrecedence(ast[ai].getContent().get()))
			{
				TRACE_COUT << "time to fix precedence (root " << maxPrec << ", child '" << ast[ai].getContent().get() << "' " << getPrecedence(ast[ai].getContent().get()) << ")...\n";
				AST tmp = AST();
				for (int j = ai; j < ast.size(); j++)
					tmp.add(ast[j]);
				tmp.addAtBeginning(ast[ai-1]);
				ast[ai-1] = tmp;
				ast.removeAfter(ai);
			}
		}

		ast.setStatus(AST::statusRDP);

		TRACE_OUTDATA(ast.display());

		ci = ai;
		return ast;
	}

	AST RDP::parse(AST ast, unsigned int &ci)
	{
		TRACE_INDATA(ast.display());

		if (verifyASTForParsing(ast) == false)
		{
			TRACE_COUT << "couldn't verify AST, skipping...\n";
			TRACE_OUTDATA(ast.display());
			return ast;
		}

		TRACE_COUT << "any-parsing...\n";
		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].getStatus() < AST::statusRDP && ast[i].size() > 0)
			{
				unsigned int ci2 = 0;
				ast[i] = parse(ast[i], ci2);
				ast[i].setStatusRecursive(AST::statusRDP);
				ast.error(ast[i].getErrors());
			}
		}

		//addTagsFromAST(ast);

		AST tmp;
		unsigned int tmpci;
		bool anyPotentialMatches = false;
		for (vector<ASTE>::iterator i = content.begin(); i != content.end(); i++)
		{
			tmp = ast;
			tmpci = ci;

			TRACE_COUT << "trying " << TRACE_GREEN << i->getName() << TRACE_DEFAULT << " (" << (i-content.begin()) << "/" << content.size() << ", from " << ci << " to " << ast.size() << ") - " << i->display() << "\n";

			if (tmpci < tmp.size() && tmp.getStatus() < AST::statusRDP)
			{
				tmp = parse(tmp, *i, tmpci);
				tmp.pullUpErrors();
			}
			else if (tmpci >= tmp.size())
			{
				TRACE_COUT << "there aren't actually any more tokens to parse, halting... (" << tmpci << " >= " << tmp.size() << ")\n";
				break;
			}
			else
			{
				TRACE_COUT << "already parsed, skipping...\n";
				continue;
			}

			if (tmp.containsNamedBranch())
				anyPotentialMatches = true;

			if (tmp.containsErrors())
			{
				TRACE_COUT << "contains errors, skipping...\n";
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

	bool RDP::verifyASTForParsing(AST &ast)
	{
		if (ast.getStatus() >= AST::statusRDP)
		{
			TRACE_COUT << "already parsed, skipping...\n";
			return false;
		}

		return true;
	}

	AST RDP::reorganizeOnPrecedence(AST ast, AST subast, unsigned int prec)
	{
		if (prec >= getPrecedence(subast))
		{
			TRACE_COUT << "precedence of root is >= that of child, keeping current structure...\n";
		}
		else
		{
			TRACE_COUT << "precedence of root is < that of child, reorganizing structure...\n";
			ast.addAtBeginning(subast);
		}

		return ast;
	}

	AST RDP::subparse(AST ast, ASTE e, unsigned int &i, unsigned int prec)
	{
		TRACE_COUT << "trying " << TRACE_GREEN << e.getName() << TRACE_DEFAULT << " (from " << i << " to " << ast.size() << ") - " << e.display() << "\n";

		if (verifyASTForParsing(ast) == false)
		{
			ast.setName(e.getName());
			return ast;
		}

		AST rtn = parse(ast, e, i);
		rtn = reorganizeOnPrecedence(ast, rtn, prec);
		rtn.setStatus(AST::statusRDP);

		return rtn;
	}

	AST RDP::subparse(AST ast, unsigned int &i, unsigned int prec)
	{
		if (verifyASTForParsing(ast) == false)
		{
			return ast;
		}

		AST rtn = parse(ast, i);
		rtn = reorganizeOnPrecedence(ast, rtn, prec);
		rtn.setStatus(AST::statusRDP);

		return rtn;
	}

	unsigned int RDP::getMaxPrecedence(AST ast)
	{
		unsigned int maxPrec = 0;
		for (unsigned int i = 0; i < ast.size(); i++)
			if (getPrecedence(ast[i].getContent().get()) > maxPrec)
				maxPrec = getPrecedence(ast[i].getContent().get());
		return maxPrec;
	}

	bool RDP::verifyExpectation(ASTE e)
	{
		if (e.size() == 0)
		{
			TRACE_COUT << "verification failed because of empty expectation\n";
			throw runtime_error("empty expectation");
			return false;
		}

		for (unsigned int i = 0; i < e.size(); i++)
		{
			if (e[i].getExpectationType() == ASTE::_name)
			{
				vector<vector<ASTE>::iterator> pi = findElements(e[i].getName());
				if (pi.empty())
				{
					TRACE_COUT << "verification failed because expectation contained reference to another expectation that does not exist\n";
					throw runtime_error("no such pattern '"+e[i].getName()+"'");
					return false;
				}
			}
		}

		return true;
	}

	bool RDP::verifyExpectation(ASTE e, AST ast, unsigned int ci)
	{
		if (verifyExpectation(e) == false)
			return false;

		if (!verifyASTForParsing(ast))
			return false;
		
		return true;
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

	unsigned int RDP::getPrecedence(AST ast)
	{
		return getPrecedence(ast.getContent().get());
	}

	string RDP::setTag(string symbol, string tag)
	{
		return (tags[symbol] = tag);
	}

	string RDP::getTag(string symbol)
	{
		if (tags.find(symbol) == tags.end())
			return "";
		else
			return tags[symbol];
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

	void RDP::addTagCondition(ASTE cond, RDP::relativeTo rel, int pos, string tag)
	{
		tagCondition tmp;
		tmp.condition = cond;
		tmp.relative = rel;
		tmp.position = pos;
		tmp.tag = tag;

		if (tagConditionList.empty())
		{
			tagConditionList.push_back(tmp);
		}
		else
		{
			for (vector<tagCondition>::iterator i = tagConditionList.begin(); i != tagConditionList.end(); i++)
			{
				if (i->condition.size() < tmp.condition.size())
				{
					tagConditionList.insert(i, tmp);
					return ;
				}
			}

			tagConditionList.push_back(tmp);
		}
	}

	AST RDP::parse(AST ast)
	{
		unsigned int ci = 0;
		return parse(ast, ci);
	}
}
