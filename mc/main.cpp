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

#include <iostream>
#include <trace/trace.h>
#include <runcfg/runcfg.h>
#include <parsing/lexical/tokenizer.h>
#include <parsing/semantic/scope.h>
#include <parsing/semantic/rdp.h>
#include "parsing.h"
#include "compile.h"

using namespace std;

parsing::Tokenizer tokenizer;
parsing::ScopeParser scopeParser;
parsing::RDP rdp;
mc::Compiler compiler;

void usage();
void compile(string path);

int main(int argc, char *argv[])
{
	TRACE_INIT(argc, argv);
	TRACE_MODULE_MAP("main.cpp", "MC");
	TRACE_MODULE_MAP("semantic/rdp.cpp", "RDP");
	TRACE_MODULE_MAP("tokenizer.cpp", "Tokenizer");
	TRACE_MODULE_MAP("semantic/scope.cpp", "Scope Parser");
	TRACE_MODULE_MAP("compile.cpp", "Compiler");
	TRACE_ENABLE_DEBUG();

	TRACE_COUT << "reading command line paramters...\n";
	vector<string> infiles;
	string outfile;
	vector<string> objects;
	bool performLinking = true;
	for (int i = 1; i < argc; i++)
	{
		string a = string(argv[i]);

		if (a.find(".n") != string::npos)
		{
			infiles.push_back(a);
			outfile = "a.out";
			performLinking = true;
		}
		else if (a.find(".o") != string::npos)
		{
			if (performLinking == false)
			{
				cerr << "error: cannot specify *.o files as input files on top of compilation specifications (-c <FILE>)\n";
				return 1;
			}

			infiles.push_back(a);
		}
		else if (a.compare("-c") == 0)
		{
			if (infiles.empty() == false)
			{
				cerr << "error: cannot specify file to compile (-c <FILE>) on top of other input files\n";
				return 1;
			}

			string tmp = argv[i+1];
			outfile = tmp.substr(0, tmp.find(".n"))+".o";
			infiles.clear();
			infiles.push_back(tmp);
			performLinking = false;
			i++;
		}
		else if (a.compare("-o") == 0)
		{
			outfile = argv[i+1];
			i++;
		}
		else if (a.compare("-h") == 0 || a.compare("--help") == 0)
		{
			usage();
			return 0;
		}
		else
		{
			cerr << "error: unknown paramter: " << a << "\n";
			return 1;
		}
	}

	if (infiles.empty())
	{
		cerr << "error: no input files\n";
		return 1;
	}

	TRACE_COUT << "creating parsers...\n";
	tokenizer = mc::createTokenizer();
	scopeParser = mc::createScopeParser();
	rdp = mc::createRDP();

	TRACE_COUT << "creating compiler...\n";
	compiler = mc::Compiler();

	for (vector<string>::iterator i = infiles.begin(); i != infiles.end(); i++)
	{
		if (i->find(".n") != string::npos)
		{
			compile(*i);
			objects.push_back(i->substr(0, i->find(".n"))+".o");
		}
		else
		{
			objects.push_back(*i);
		}
	}

	return 0;
}

void usage()
{
	cout << "mc 0.01 alpha\n";
	cout << "\n";
	cout << "usage: mc <INPUT FILES> [-c <INPUT FILE>] [-o <OUTPUT FILE>] [OPTIONS]\n";
	cout << "\n";
	cout << "OPTIONS\n";
	cout << "  -c <INPUT FILE>   compiles the file into an object without linking\n";
	cout << "  -o <OUTPUT FILE>  specifies the output file\n";
	cout << "  -h, --help        displays this help\n";
	cout << "\n";
	cout << "Use --trace-help to view trace options.\n";
}

void compile(string path)
{
	TRACE_COUT << "compiling " << path << "...\n";
	vector<parsing::Token> toks = tokenizer.tokenizeFile(path);
	parsing::AST ast = scopeParser.parse(toks);
	ast = rdp.parse(ast);
	compiler.compile(ast);

	if (compiler.errorNumber() > 0)
	{
		cerr << compiler.dumpErrors();
		if (compiler.errorNumber() == 1)
			cerr << "1 error.\n";
		else
			cerr << compiler.errorNumber() << " errors.\n";
		_exit(1);
	}
}
