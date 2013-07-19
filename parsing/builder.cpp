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

#include "builder.h"

namespace parsing
{
	Builder::Builder()
	{
		internalCommandTokenizer = Tokenizer();
		internalCommandTokenizer.noDelim(internalCommandTokenizer() == "\"", internalCommandTokenizer() == "\"");
		internalCommandTokenizer.noDelim(internalCommandTokenizer() == "'", internalCommandTokenizer() == "'");
		internalCommandTokenizer.whitespace(internalCommandTokenizer() == " ");
		internalCommandTokenizer.whitespace(internalCommandTokenizer() == "\t");
		internalCommandTokenizer.token("Separator", internalCommandTokenizer() == ",");

		internalLineTokenizer = Tokenizer();
		internalLineTokenizer.token("NewLine", internalLineTokenizer() == "\n" || internalLineTokenizer() == "\r");
	}

	vector<Token> Builder::tokenize(string s)
	{
		return internalCommandTokenizer.tokenize(s, "--");
	}

	Pattern Builder::buildPattern(vector<Token> p)
	{
		Pattern rtn = Pattern();
		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
			rtn || (Pattern() == escape(i->get().substr(1, i->get().size()-2)));
		return rtn;
	}

	vector<Pattern> Builder::buildMultiPatterns(vector<Token> p)
	{
		vector<Pattern> rtn;
		vector<Token> tmp;
		
		for (vector<Token>::iterator i = p.begin(); i != p.end(); i++)
		{
			if (i->get().compare(",") == 0)
			{
				rtn.push_back(buildPattern(tmp));
				tmp.clear();
			}
			else
			{
				tmp.push_back(*i);
			}
		}

		return rtn;
	}

	void Builder::build(string page)
	{
		vector<Token> lines = internalLineTokenizer.tokenize(page, "--");
		for (vector<Token>::iterator i = lines.begin(); i != lines.end(); i++)
		{
			if (i->get().empty())
				continue;

			vector<Token> toks = internalCommandTokenizer.tokenize(i->get(), "--");
			if (toks.empty())
				continue;

			if (toks.size() < 2)
			{
				cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m commands require at least 1 argument\033[0;0m\n";
				return ;
			}

			vector<Token> args(toks.begin()+1, toks.end());
			vector<Pattern> pats = buildMultiPatterns(args);

			if (toks[0].get().compare("nodelim") == 0 || toks[0].get().compare("quote") == 0)
			{
				if (pats.size() != 2)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m nodelim/quote command requires 2 patterns\033[0;0m\n";
					return ;
				}
				resultTokenizer.noDelim(pats[0], pats[1]);
			}
			else if (toks[0].get().compare("skip") == 0 || toks[0].get().compare("comment") == 0)
			{
				if (pats.size() != 2)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m skip/comment command requires 2 patterns\033[0;0m\n";
					return ;
				}
				resultTokenizer.skip(pats[0], pats[1]);
			}
			else if (toks[0].get().compare("whitespace") == 0)
			{
				if (pats.size() != 1)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m whitespace command requires 1 pattern\033[0;0m\n";
					return ;
				}
				resultTokenizer.whitespace(pats[0]);
			}
			else if (toks[0].get().compare("category") == 0 || toks[0].get().compare("tag") == 0)
			{
				if (args.size() < 2)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m category/tag command requires at least 2 arguments\033[0;0m\n";
					return ;
				}

				string name = args[0].get();
				args.erase(args.begin());
				pats = buildMultiPatterns(args);
				if (pats.size() != 1)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m category/tag command requires 1 pattern\033[0;0m\n";
					return ;
				}
				resultTokenizer.categorizer(name, pats[0]);
			}
			else if (toks[0].get().compare("token") == 0)
			{
				if (args.size() < 2)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m token command requires at least 2 arguments\033[0;0m\n";
					return ;
				}

				string name = args[0].get();
				args.erase(args.begin());
				pats = buildMultiPatterns(args);
				if (pats.size() != 1)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m token command requires 1 pattern\033[0;0m\n";
					return ;
				}
				resultTokenizer.token(name, pats[0]);
			}
			else if (toks[0].get().compare("combine") == 0)
			{
				if (pats.size() != 2)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m combine command requires 2 patterns\033[0;0m\n";
					return ;
				}
				resultTokenizer.combine(pats[0], pats[1]);
			}
			else if (toks[0].get().compare("scope") == 0)
			{
				if (toks.size() != 3)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m scope command requires 2 arguments\033[0;0m\n";
					return ;
				}
				resultScopeParser.bindScope(toks[1].get(), toks[2].get());
			}
			else if (toks[0].get().compare("precedence") == 0)
			{
				if (toks.size() != 3)
				{
					cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m precedence command requires 2 arguments\033[0;0m\n";
					return ;
				}
				resultRDP.setPrecedence(toks[1].get(), atoi(toks[2].get().c_str()));
			}
			else if (toks[0].get().compare("expect") == 0)
			{
				string name = toks[1].get();
				ASTE aste = ASTE(name);
				for (int i = 2; i < toks.size(); i++)
				{
					if (toks[i].get().find(":") != string::npos)
					{
						aste << ASTE(toks[i].get().substr(0, toks[i].get().find(":")), toks[i].get().substr(toks[i].get().find(":")+1));
					}
					else if (toks[i].get()[0] == '(')
					{
						aste << ASTE(toks[i].get().substr(0, toks[i].get().size()-2), ASTE::_scope);
					}
					else
					{
						aste << ASTE(toks[i].get());
					}
				}
				resultRDP.addElement(aste);
			}
			else
			{
				cerr << "\033[1;28mError \033[0;0m - line \033[1;28m" << (i-lines.begin()) << "\033[0;0m - \033[0;31m unknown command " << toks[0].get() << "\033[0;0m\n";
				return ;
			}
		}
	}

	Tokenizer Builder::getTokenizer()
	{
		return resultTokenizer;
	}

	ScopeParser Builder::getScopeParser()
	{
		return resultScopeParser;
	}

	RDP Builder::getRDP()
	{
		return resultRDP;
	}

	AST Builder::parsePage(string page, string file)
	{
		vector<Token> toks = resultTokenizer.tokenize(page, file);
		AST rtn = resultScopeParser.parse(toks);
		rtn = resultRDP.parse(rtn);
		return rtn;
	}

	AST Builder::parseFile(string path)
	{
		vector<Token> toks = resultTokenizer.tokenizeFile(path);
		AST rtn = resultScopeParser.parse(toks);
		rtn = resultRDP.parse(rtn);
		return rtn;
	}
}