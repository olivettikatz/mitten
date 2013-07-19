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

#include "asterror.h"

namespace parsing
{
	string ASTError::format(string fmt)
	{
		string tmp = fmt;
		if (tmp.find("%l") != string::npos)
		{
			stringstream ss;
			ss << src.getLine();
			tmp.replace(tmp.find("%l"), 2, ss.str());
		}

		if (tmp.find("%c") != string::npos)
		{
			stringstream ss;
			ss << src.getColumn();
			tmp.replace(tmp.find("%c"), 2, ss.str());
		}

		if (tmp.find("%f") != string::npos)
		{
			tmp.replace(tmp.find("%f"), 2, src.getFile());
		}

		if (tmp.find("%t") != string::npos)
		{
			tmp.replace(tmp.find("%t"), 2, src.getType());
		}

		if (tmp.find("%s") != string::npos)
		{
			tmp.replace(tmp.find("%s"), 2, src.get());
		}

		if (tmp.find("%m") != string::npos)
		{
			tmp.replace(tmp.find("%m"), 2, format(msg));
		}

		return tmp;
	}

	string ASTError::display(string fmt)
	{
		return format(fmt);
	}

	string ASTError::display()
	{
		return display("\033[1;28mError \033[0;0m- %f:\033[1;28m%l\033[0;0m:%c - \033[0;31m%m\033[0;0m\n");
	}
}