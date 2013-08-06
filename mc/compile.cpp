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

#include "compile.h"

namespace mc
{
	Compiler::llvmObject Compiler::llvmObjectNull()
	{
		llvmObject rtn = {
			_null, NULL, NULL
		};
		return rtn;
	}

	vector<parsing::AST> Compiler::tokenizeAST(parsing::AST ast, string delim)
	{
		vector<parsing::AST> rtn;
		parsing::AST buf = parsing::AST();

		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].size() == 0 && ast[i].getContent().get().compare(delim) == 0)
			{
				rtn.push_back(buf);
				buf = parsing::AST();
			}
			else
			{
				buf << ast[i];
			}
		}

		rtn.push_back(buf);

		return rtn;
	}

	bool Compiler::isValueToken(parsing::Token t)
	{
		return (t.getType().find("Value") != string::npos);
	}

	bool Compiler::isSymbolToken(parsing::Token t)
	{
		return (t.getType().compare("Symbol") == 0);
	}

	bool Compiler::isOperatorToken(parsing::Token t)
	{
		return (t.getType().find("Operator") != string::npos);
	}

	string Compiler::getLineFromPage(string page, unsigned int linenum)
	{
		size_t i = 0;
		for (; linenum > 1; linenum--)
		{
			i = page.find("\n", i+1);
		}
		return page.substr(i+1, page.find("\n", i+1)-i-1);
	}

	Compiler::Compiler()
	{
		addSymbol("import");

		addType("int");
	}

	int Compiler::addType(string n)
	{
		types[n] = types.size();
		return types[n];
	}

	int Compiler::addSymbol(string n)
	{
		symbols[n] = symbols.size();
		return symbols[n];
	}

	Compiler::llvmObject Compiler::compile(parsing::AST ast)
	{
		TRACE_INDATA(ast.display());

		for (unsigned int i = 0; i < ast.size(); i++)
		{
			if (ast[i].size() == 0 && symbols.find(ast[i].getContent().get()) != symbols.end())
			{
				TRACE_COUT("found symbol - " << ast[i].getContent().get() << "\n");
				if (ast[i+1].size() == 0 && ast[i+1].getContent().get().compare("(") == 0)
				{
					TRACE_COUT("  with arguments...\n");
					vector<parsing::AST> args = tokenizeAST(ast[i+2], ",");
					for (vector<parsing::AST>::iterator j = args.begin(); j != args.end(); j++)
					{
						if (j->size() == 0)
						{
							errors.push_back(parsing::ASTError(ast[i+1].getContent(), "empty argument"));
							return llvmObjectNull();
						}
						else
						{
							llvmObject tmp = compile(*j);
						}
					}

					if (ast[i+3].size() != 0 || ast[i+3].getContent().get().compare(")") != 0)
					{
						errors.push_back(parsing::ASTError(ast[i+3].getContent(), "expected a right parenthesis"));
					}

					if (ast[i+4].size() != 0 || ast[i+4].getContent().get().compare(";") != 0)
					{
						errors.push_back(parsing::ASTError(ast[i+4].getContent(), "expected a semicolon"));
					}
				}
				else if (ast[i+1].size() == 0 && ast[i+1].getContent().get().compare(";") == 0)
				{
					TRACE_COUT("  line ends...\n");
				}
				else if (ast[i+1].size() == 0 && ast[i+1].getContent().get().compare("=") == 0)
				{
					TRACE_COUT("found assignment of expression - " << ast[i+1].getContent().get() << "\n");
				}
				else
				{
					errors.push_back(parsing::ASTError(ast[i+1].getContent(), "unexpected token"));
					return llvmObjectNull();
				}
			}
			else if (ast[i].size() == 0 && types.find(ast[i].getContent().get()) != types.end())
			{
				TRACE_COUT("found type - " << ast[i].getContent().get() << "\n");
				for (i++; i < ast.size(); i++)
				{
					if (ast[i].size() == 0 && types.find(ast[i].getContent().get()) != types.end())
					{
						TRACE_COUT("found type - " << ast[i].getContent().get() << "\n");
					}
					else if (ast[i].size() == 0 && ast[i].getContent().get().compare("(") == 0)
					{
						if (ast[i+1].size() == 0)
						{
							errors.push_back(parsing::ASTError(ast[i].getContent(), "expected scope, but got a leaf"));
							return llvmObjectNull();
						}
						else
						{
							llvmObject tmp = compile(ast[i+1]);
						}

						if (ast[i+2].size() != 0 || ast[i+2].getContent().get().compare(")") != 0)
						{
							errors.push_back(parsing::ASTError(ast[i+2].getContent(), "expected a left parenthesis"));
							return llvmObjectNull();
						}
					}
					else if (ast[i].size() == 0 && isSymbolToken(ast[i].getContent()))
					{
						TRACE_COUT("found symbol after types - " << ast[i].getContent().get() << "\n");
						addSymbol(ast[i].getContent().get());
						break;
					}
					else
					{
						errors.push_back(parsing::ASTError(ast[i].getContent(), "expected either type, type expression, or an expression symbol"));
						return llvmObjectNull();
					}
				}
			}
			else if (ast[i].size() == 0 && isValueToken(ast[i].getContent()))
			{
				TRACE_COUT("found value - " << ast[i].getContent().get() << "\n");
			}
			else if (ast[i].size() == 0 && isOperatorToken(ast[i].getContent()))
			{
				TRACE_COUT("found operator - " << ast[i].getContent().get() << "\n");
				if (ast[i].getContent().get().compare("!") == 0 || ast[i].getContent().get().compare("~") == 0)
				{
					TRACE_COUT("  unary right operation\n");
				}
				else if (ast[i].getContent().get().compare("++") == 0 || ast[i].getContent().get().compare("--") == 0)
				{
					TRACE_COUT("  unary left operation\n");
				}
				else
				{
					TRACE_COUT("  binary operation\n");
				}
			}
		}

		return llvmObjectNull();
	}

	int Compiler::errorNumber()
	{
		return errors.size();
	}

	string Compiler::dumpErrors(string page)
	{
		stringstream ss;
		for (vector<parsing::ASTError>::iterator i = errors.begin(); i != errors.end(); i++)
		{
			ss << i->display();
			string tmp = getLineFromPage(page, i->getSource().getLine());
			ss << "  " << tmp.substr(0, i->getSource().getColumn()-1) << "\033[0;31m" << tmp.substr(i->getSource().getColumn()-1, i->getSource().get().size()) << "\033[0;0m" << tmp.substr(i->getSource().getColumn()-1+i->getSource().get().size()) << "\n";
			ss << string(i->getSource().getColumn()+1, ' ') << "\033[1;28m^\033[0;0m\n";
		}
		return ss.str();
	}
}