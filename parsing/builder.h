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

#ifndef __MITTEN_PARSING_BUILDER_H
#define __MITTEN_PARSING_BUILDER_H

#include "parsing.h"

namespace parsing
{
	using namespace std;

	class Builder
	{
	private:
		Tokenizer resultTokenizer;
		ScopeParser resultScopeParser;
		RDP resultRDP;

		Tokenizer internalCommandTokenizer;
		Tokenizer internalLineTokenizer;

	public:
		Builder();
		vector<Token> tokenize(string s);
		Pattern buildPattern(vector<Token> p);
		vector<Pattern> buildMultiPatterns(vector<Token> p);
		void build(string page);
		void buildFromFile(string path);
		Tokenizer getTokenizer();
		ScopeParser getScopeParser();
		RDP getRDP();
		AST parsePage(string page, string file);
		AST parseFile(string path);
	};
}

#endif