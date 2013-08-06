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
	void Environment::debugStacks()
	{
		for (map<string, vector<vector<ASTError> > >::iterator i = errorStacks.begin(); i != errorStacks.end(); i++)
		{
			stringstream ss;
			ss << i->first << ":";
			for (vector<vector<ASTError> >::iterator j = i->second.begin(); j != i->second.end(); j++)
			{
				ss << " " << j->size();
			}
			TRACE_COUT(ss.str() << "\n");
		}
	}

	void Environment::error(AST ast, string m)
	{
		if (errorStacks.find(ast.getFile()) == errorStacks.end() || errorStacks[ast.getFile()].empty())
			pushErrors(ast.getFile());
		TRACE_COUT("error in '" << ast.getFile() << "' - " << ASTError(ast.getContent(), m).display() << "\n");
		errorStacks[ast.getFile()].back().push_back(ASTError(ast.getContent(), m));
	}

	void Environment::error(string file, AST ast, string m)
	{
		if (file.empty())
			throw runtime_error("empty file in error");
		if (errorStacks.find(file) == errorStacks.end() || errorStacks[file].empty())
			pushErrors(file);
		errorStacks[file].back().push_back(ASTError(ast.getContent(), m));
	}

	void Environment::error(ASTError e)
	{
		if (e.getSource().getFile().empty())
			throw runtime_error("empty file in error");
		if (errorStacks.find(e.getSource().getFile()) == errorStacks.end() || errorStacks[e.getSource().getFile()].empty())
			pushErrors(e.getSource().getFile());
		errorStacks[e.getSource().getFile()].back().push_back(e);
	}

	void Environment::error(string file, vector<ASTError> e)
	{
		if (file.empty())
			throw runtime_error("empty file in error");
		if (errorStacks.find(file) == errorStacks.end() || errorStacks[file].empty())
			pushErrors(file);
		for (vector<ASTError>::iterator i = e.begin(); i != e.end(); i++)
			errorStacks[file].back().push_back(*i);
	}

	void Environment::pushErrors(string file)
	{
		if (file.empty())
			throw runtime_error("empty file in error");
		TRACE_COUT("pushing errors in '" << file << "'\n");
		//TRACE_BACKTRACE();
		vector<ASTError> tmp;
		errorStacks[file].push_back(tmp);
	}

	void Environment::pushErrors(AST ast)
	{
		pushErrors(ast.getFile());
	}

	vector<ASTError> Environment::popErrors(string file)
	{
		if (file.empty())
			throw runtime_error("empty file in error");
		vector<ASTError> rtn = errorStacks[file].back();
		if (!errorStacks[file].empty())
			errorStacks[file].pop_back();
		return rtn;
	}

	vector<ASTError> Environment::popErrors(AST ast)
	{
		TRACE_COUT("popping errors in '" << ast.getFile() << "'\n");
		//TRACE_BACKTRACE();
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
		TRACE_COUT("merging errors in '" << file << "'\n");
		//TRACE_BACKTRACE();
		debugStacks();
		vector<ASTError> tmp = popErrors(file);
		//errorStacks[file].back().insert(errorStacks[file].back().begin(), tmp.begin(), tmp.end());
		error(file, tmp);
		debugStacks();
	}

	void Environment::mergeErrors(string file, string dst)
	{
		TRACE_COUT("merging errors from '" << file << "' to '" << dst << "'\n");
		//TRACE_BACKTRACE();
		debugStacks();
		vector<ASTError> tmp = popErrors(file);
		if (errorStacks.find(dst) == errorStacks.end() || errorStacks[dst].empty())
			pushErrors(dst);
		//errorStacks[dst].back().insert(errorStacks[dst].back().begin(), tmp.begin(), tmp.end());
		error(dst, tmp);
		debugStacks();
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
