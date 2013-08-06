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

	void ScopeParser::bindSeparator(string sb, string s, string n)
	{
		separators[sb].first = s;
		separators[sb].second = n;
	}

	void ScopeParser::bindSeparator(string s, string n)
	{
		separators[""].first = s;
		separators[""].second = n;
	}

	AST ScopeParser::sepParse(AST ast, string bound)
	{
		TRACE_COUT("separator parsing with bound '"+bound+"', separator '"+separators[bound].first+"', segments named '"+separators[bound].second+"'\n");
		TRACE_INDATA(ast.display());

		AST rtn = AST(ast.getName(), ast.getContent());
		AST buf = AST();

		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].getContent().getType().compare(separators[bound].first) == 0)
			{
				TRACE_COUT("AST[" << i << "] '" << ast[i].getContent().getType() << "':'"+ast[i].getContent().get() << "' is a separator...\n");
				if (buf.size() > 0)
				{
					buf.setName(separators[bound].second);
					buf.setStatus(AST::statusScope);
					rtn << buf;
					buf = AST();
				}
			}
			else
			{
				TRACE_COUT("AST[" << i << "] '" << ast[i].getContent().getType() << "':'"+ast[i].getContent().get() << "' is NOT a separator (" << separators[bound].first << ")...\n");
				for (map<string, pair<string, string> >::iterator j = separators.begin(); j != separators.end(); j++)
				{
					if (j->first.compare(bound) != 0 && ast[i].getContent().getType().compare(j->second.first) == 0)
					{
						environment.error(ast[i], "illegal separator '"+ast[i].getContent().getType()+"' in scope starting with '"+bound+"'");
						break;
					}
				}

				if (ast[i].size() > 0)
				{
					if (i > 0)
					{
						if (bounds.find(ast[i-1].getContent().getType()) == bounds.end())
						{
							environment.error(ast[i-1], "unrecognized scope starting with '"+ast[i-1].getContent().getType()+"'");
						}

						buf << sepParse(ast[i], ast[i-1].getContent().getType());
					}
					else
					{
						buf << sepParse(ast[i]);
					}
				}
				else
				{
					buf << ast[i];
				}
			}
		}

		if (buf.size() > 0)
		{
			buf.setName(separators[bound].second);
			buf.setStatus(AST::statusScope);
			rtn << buf;
		}

		TRACE_OUTDATA(rtn.display());
		return rtn;
	}

	AST ScopeParser::sepParse(AST ast)
	{
		return sepParse(ast, "");
	}

	AST ScopeParser::parse(vector<Token> p, unsigned int l)
	{
		AST rtn = AST();
		rtn.setStatus(AST::statusScope);
		map<string, unsigned int> starts;
		map<string, unsigned int> levels;
		unsigned int masterLevel = 0;

		string inp;
		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
		{
			if (i > p.begin()+10)
			{
				inp += "...";
				break;
			}
			else
			{
				inp += string(" '")+TRACE_GREEN+i->get()+TRACE_DEFAULT+"'";
			}
		}
		TRACE_INDATA(inp);

		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
		{
			if (bounds.find(i->getType()) != bounds.end())
			{
				TRACE_COUT("starting bound " << TRACE_YELLOW << i->getType() << " " << TRACE_RED << i->get() << TRACE_DEFAULT << "\n");
			
				if (levels.find(bounds[i->getType()]) == levels.end())
				{
					levels[bounds[i->getType()]] = 1;
					starts[bounds[i->getType()]] = i-p.begin();
					TRACE_COUT("  (level 1)\n");
				}
				else
				{
					levels[bounds[i->getType()]]++;
					TRACE_COUT("  (level " << levels[bounds[i->getType()]] << ")\n");
				}

				masterLevel++;
			}
			else if (starts.find(i->getType()) != starts.end())
			{
				TRACE_COUT("ending bound [" << starts[i->getType()] << ":" << (i-p.begin()) << "] after " << TRACE_YELLOW << i->getType() << " " << TRACE_RED << i->get() << TRACE_DEFAULT << "\n");
				
				levels[i->getType()]--;
				TRACE_COUT("  (level " << levels[i->getType()] << ")\n");

				string largestStart = "";
				unsigned int largestStartValue = (unsigned int)-1;
				for (map<string, unsigned int>::iterator j = starts.begin(); j != starts.end(); j++)
				{
					if (j->second < largestStartValue)
					{
						largestStartValue = j->second;
						largestStart = j->first;
					}
				}

				if (levels[i->getType()] == 0 && i->getType().compare(largestStart) == 0)
				{
					vector<Token> tmp(p.begin()+starts[i->getType()]+1, i);
					AST ast = parse(tmp, l+1);
					TRACE_INDATA(ast.display());
					AST tmpast = AST(p[starts[i->getType()]]);
					tmpast.setStatus(AST::statusScope);
					rtn.add(tmpast);
					ast.setStatus(AST::statusScope);
					rtn.add(ast);
					tmpast = AST(*i);
					tmpast.setStatus(AST::statusScope);
					rtn.add(tmpast);
					levels.erase(levels.find(i->getType()));
					starts.erase(starts.find(i->getType()));
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
		AST rtn = parse(p, 0);
		if (separators.empty())
			return rtn;
		else
			return sepParse(rtn);
	}
}