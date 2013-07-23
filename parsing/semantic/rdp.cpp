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
	void RDP::addTagsFromAST(AST &ast)
	{
		for (unsigned int i = 0; i < ast.size()-1; i++)
		{
			for (vector<tagCondition>::iterator j = tagConditionList.begin(); j != tagConditionList.end(); j++)
			{
				AST astdup = ast;
				unsigned int ci = i;
				AST tmp = parse(astdup, j->condition, ci);
				if (tmp.containsErrors() == false)
				{
					if (j->relative == _beginning)
					{
						setTag(ast[i+j->position].getContent().get(), j->tag);
					}
					else if (j->relative == _end)
					{
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
		TRACE_INDATA(TRACE_GREEN << p.getName() << TRACE_DEFAULT << " (from " << ci << " to " << (p.size() < ast.size() ? p.size() : ast.size()) << ") " << ast.displaySome(2));

		if (!verifyExpectation(p, ast, ci))
		{
			TRACE_COUT << "could not verify expectation, returning...\n";
			TRACE_OUTDATA(ast.displaySome(2));
			return ast;
		}

		ast.setName(p.getName());
		bool fixed = true;
		unsigned int maxPrec = getMaxPrecedence(ast);

		unsigned int i;
		for (i = ci; i < p.size() && i < ast.size(); i++)
		{
			if (p[i].getExpectationType() == ASTE::_name)
			{
				vector<vector<ASTE>::iterator> pi = findElements(p[i].getArgument());
				if (ast[i].getStatus() < AST::statusRDP)
				{
					bool haveSuccess = false;
					AST errtmp;

					for (vector<vector<ASTE>::iterator>::iterator j = pi.begin(); j != pi.end(); j++)
					{
						ast[i] = subparse(ast[i], **j, i, getPrecedence(ast));
						if (ast[i].containsErrors() == false)
							haveSuccess = true;
					}
				}
			}
			else if (p[i].getExpectationType() == ASTE::_type)
			{
				if (ast[i].getContent().getType().compare(p[i].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - (" << p[i].getArgument() << " != " << ast[i].getContent().getType() << ") - " << ast[i].displaySome(2) << "\n";
					ast[i].error("expected '"+p[i].getArgument()+"', but got '"+ast[i].getContent().getType()+"'");
					ast[i].setStatus(AST::statusRDP);
				}
				else if (ast[i].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					ast.error("expected a leaf, but got a branch with now-orphaned children");
					ast[i].setStatus(AST::statusRDP);
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					ast[i].setName(p[i].getName());
					ast[i].setStatus(AST::statusRDP);
				}
			}
			else if (p[i].getExpectationType() == ASTE::_scope)
			{
				unsigned int ci2 = 0;
				ast[i] = subparse(ast[i], ci2, getPrecedence(ast));
				ast[i].setName(p[i].getName());
			}
			else if (p[i].getExpectationType() == ASTE::_tag)
			{
				if (getTag(ast[i].getContent().get()).compare(p[i].getArgument()) != 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - (" << p[i].getArgument() << " != " << getTag(ast[i].getContent().get()) << ") - " << ast[i].displaySome(2) << "\n";
					ast[i].error("expected '"+p[i].getArgument()+"', but got '"+getTag(ast[i].getContent().get())+"'");
					ast[i].setStatus(AST::statusRDP);
				}
				else if (ast[i].size() > 0)
				{
					fixed = false;
					TRACE_COUT << TRACE_RED << p[i].getName() << TRACE_DEFAULT << " failed - " << ast[i].displaySome(2) << "\n";
					ast[i].error("expected a leaf, but got a branch with now-orphaned children");
					ast[i].setStatus(AST::statusRDP);
				}
				else
				{
					TRACE_COUT << TRACE_GREEN << p[i].getName() << TRACE_DEFAULT << " succeeded.\n";
					ast[i].setName(p[i].getName());
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

	AST RDP::parse(AST ast, unsigned int &ci)
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

	bool RDP::verifyASTForParsing(AST &ast)
	{
		if (ast.size() == 0)
		{
			ast.error("expected a scope, but got a leaf");
			ast.setStatus(AST::statusRDP);
			return false;
		}

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
		if (verifyASTForParsing(ast) == false)
		{
			ast.setName(e.getName());
			return ast;
		}

		AST rtn = parse(ast, e, i);
		rtn = reorganizeOnPrecedence(ast, rtn, prec);
		rtn.setStatus(AST::statusRDP);

		TRACE_INDATA(rtn.display());
		return rtn;
	}

	AST RDP::subparse(AST ast, unsigned int &i, unsigned int prec)
	{
		if (verifyASTForParsing(ast) == false)
			return ast;

		AST rtn = parse(ast, i);
		rtn = reorganizeOnPrecedence(ast, rtn, prec);
		rtn.setStatus(AST::statusRDP);

		TRACE_INDATA(rtn.display());
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
			throw runtime_error("empty expectation");
			return false;
		}

		for (unsigned int i = 0; i < e.size(); i++)
		{
			if (e[i].getExpectationType() == ASTE::_name)
			{
				vector<vector<ASTE>::iterator> pi = findElements(e[i].getArgument());
				if (pi.empty())
				{
					throw runtime_error("no such pattern '"+e[i].getArgument()+"'");
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

		if (ast.size() == 0 || ci >= (e.size() < ast.size() ? e.size() : ast.size())-1)
			return false;
		
		return false;
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
