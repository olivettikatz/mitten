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
				TRACE_COUT << "found symbol - " << ast[i].getContent().get() << "\n";
				if (ast[i+1].size() == 0 && ast[i+1].getContent().get().compare("(") == 0)
				{
					TRACE_COUT << "  with arguments...\n";
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
					TRACE_COUT << "  line ends...\n";
				}
				else
				{
					errors.push_back(parsing::ASTError(ast[i+1].getContent(), "unexpected token"));
					return llvmObjectNull();
				}
			}
			else if (ast[i].size() == 0 && types.find(ast[i].getContent().get()) != types.end())
			{
				TRACE_COUT << "found type - " << ast[i].getContent().get() << "\n";
				for (i++; i < ast.size(); i++)
				{
					if (ast[i].size() == 0 && types.find(ast[i].getContent().get()) != types.end())
					{
						TRACE_COUT << "found type - " << ast[i].getContent().get() << "\n";
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
						TRACE_COUT << "found symbol after types - " << ast[i].getContent().get() << "\n";
						addSymbol(ast[i].getContent().get());
						if (ast.findLeafContent(";", i) == parsing::AST::npos)
						{
							errors.push_back(parsing::ASTError(ast[i].getContent(), "no semicolon at the end of the line"));
							return llvmObjectNull();
						}
						parsing::AST tmp = ast.subtree(i, ast.findLeafContent(";", i)+1-i);
						llvmObject res = compile(tmp);
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
				TRACE_COUT << "found value - " << ast[i].getContent().get() << "\n";
			}
		}

		return llvmObjectNull();
	}

	int Compiler::errorNumber()
	{
		return errors.size();
	}

	string Compiler::dumpErrors()
	{
		stringstream ss;
		for (vector<parsing::ASTError>::iterator i = errors.begin(); i != errors.end(); i++)
		{
			ss << i->display() << "\n";
		}
		return ss.str();
	}
}