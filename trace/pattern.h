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

#ifndef __KITTEN_PARSING_LEXICAL_PATTERN_H
#define __KITTEN_PARSING_LEXICAL_PATTERN_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace parsing
{
	using namespace std;

	class Pattern
	{
	public:
		typedef enum
		{
			isEqualTo,
			isNotEqualTo,
			onlyContains,
			doesNotContain,
			startsWith,
			endsWith,
			alternate
		} algorithmType;

		typedef enum
		{
			attachAnd,
			attachOr,
			attachAndNot,
			attachOrNot,
		} attachmentType;

		typedef unsigned int (*callback)(string s);

	private:
		algorithmType algorithm;
		string argument;
		callback alternateCallback;

		vector<pair<attachmentType, Pattern> > attachments;

	public:
		Pattern() : algorithm(alternate), alternateCallback(NULL) {}
		Pattern(algorithmType a, string s) : algorithm(a), argument(s), alternateCallback(NULL) {}
		Pattern(callback a) : alternateCallback(a) {}
		Pattern &setAlgorithm(algorithmType a);
		Pattern &setArgument(string a);
		Pattern &setAlternate(callback a);

		Pattern &attach(attachmentType t, Pattern p);
		
		Pattern &operator == (string s);
		Pattern &operator != (string s);
		Pattern &operator += (string s);
		Pattern &operator -= (string s);
		Pattern &operator < (string s);
		Pattern &operator > (string s);
		Pattern &operator () (callback a);

		Pattern &operator && (Pattern other);
		Pattern &operator || (Pattern other);

		bool compare(Pattern other);

		unsigned int getLengthRequest();

		unsigned int match(string s);

		string display();
	};

	extern string digits;
	extern string lettersLower;
	extern string lettersUpper;
}

#endif