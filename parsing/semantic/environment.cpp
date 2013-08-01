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

#include "environment.h"

namespace parsing
{
	void Environment::error(AST ast, string m)
	{
		if (errorStacks.find(ast.getFile()) == errorStacks.end())
			pushErrors(ast.getFile());
		TRACE_COUT << "error in '" << ast.getFile() << "' - " << ASTError(ast.getContent(), m).display() << "\n";
		errorStacks[ast.getFile()].back().push_back(ASTError(ast.getContent(), m));
	}

	void Environment::error(string file, AST ast, string m)
	{
		errorStacks[file].back().push_back(ASTError(ast.getContent(), m));
	}

	void Environment::pushErrors(string file)
	{
		TRACE_COUT << "pushing errors in '" << file << "'\n";
		vector<ASTError> tmp;
		errorStacks[file].push_back(tmp);
	}

	void Environment::pushErrors(AST ast)
	{
		pushErrors(ast.getFile());
	}

	vector<ASTError> Environment::popErrors(string file)
	{
		vector<ASTError> rtn = errorStacks[file].back();
		if (!errorStacks[file].empty())
			errorStacks[file].pop_back();
		return rtn;
	}

	vector<ASTError> Environment::popErrors(AST ast)
	{
		TRACE_COUT << "popping errors in '" << ast.getFile() << "'\n";
		return popErrors(ast.getFile());
	}

	string Environment::dumpErrors(string file)
	{
		vector<ASTError> tmp = popErrors(file);
		stringstream ss;
		for (vector<ASTError>::iterator i = tmp.begin(); i != tmp.end(); i++)
			ss << i->display() << "\n";
		return ss.str();
	}

	string Environment::dumpErrors(AST ast)
	{
		return dumpErrors(ast.getFile());
	}

	void Environment::mergeErrors(string file)
	{
		TRACE_COUT << "merging errors in '" << file << "'\n";
		vector<ASTError> tmp = popErrors(file);
		errorStacks[file].back().insert(errorStacks[file].back().begin(), tmp.begin(), tmp.end());
	}

	void Environment::mergeErrors(string file, string dst)
	{
		TRACE_COUT << "merging errors from '" << file << "' to '" << dst << "'\n";
		vector<ASTError> tmp = popErrors(file);
		if (errorStacks.find(dst) == errorStacks.end())
			pushErrors(dst);
		errorStacks[dst].back().insert(errorStacks[dst].back().begin(), tmp.begin(), tmp.end());
	}

	void Environment::mergeErrors(string file, AST dst)
	{
		mergeErrors(file, dst.getFile());
	}

	void Environment::mergeErrors(AST ast)
	{
		mergeErrors(ast.getFile());
	}

	void Environment::mergeErrors(AST ast, string dst)
	{
		mergeErrors(ast.getFile(), dst);
	}

	void Environment::mergeErrors(AST ast, AST dst)
	{
		mergeErrors(ast.getFile(), dst.getFile());
	}

	bool Environment::areErrors(string file)
	{
		if (errorStacks.find(file) == errorStacks.end())
			return false;
		return !errorStacks[file].back().empty();
	}

	bool Environment::areErrors(AST ast)
	{
		return areErrors(ast.getFile());
	}

	Environment environment;
}