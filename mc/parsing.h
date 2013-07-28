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

#ifndef __MITTEN_MC_PARSING_H
#define __MITTEN_MC_PARSING_H

#include <iostream>
#include <parsing/parsing.h>

namespace mc
{
	using namespace std;

	parsing::Tokenizer createTokenizer();
	parsing::ScopeParser createScopeParser();
	parsing::RDP createRDP(parsing::Tokenizer t);
}

#endif
