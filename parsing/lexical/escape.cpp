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

#include "escape.h"

namespace parsing
{
	string escape(string s)
	{
		string rtn;

		for (string::iterator i = s.begin(); i != s.end(); i++)
		{
			if (*i == '\\')
			{
				i++;
				if (*i == '0')
					rtn += '\0';
				else if (*i == 'a')
					rtn += '\a';
				else if (*i == 't')
					rtn += '\t';
				else if (*i == 'n')
					rtn += '\n';
				else if (*i == 'v')
					rtn += '\v';
				else if (*i == 'f')
					rtn += '\f';
				else if (*i == 'r')
					rtn += '\r';
				else if (*i == '\\')
					rtn += '\\';
				else if (*i == 'x')
				{
					stringstream ss;
					i++;
					ss << *i;
					i++;
					ss << *i;
					char c;
					ss >> hex >> c;
					rtn += c;
				}
				else
				{
					rtn += *i;
				}
			}
			else
			{
				rtn += *i;
			}
		}

		return rtn;
	}
}