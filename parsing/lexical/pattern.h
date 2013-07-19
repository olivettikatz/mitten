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

#ifndef __MITTEN_PARSING_LEXICAL_PATTERN_H
#define __MITTEN_PARSING_LEXICAL_PATTERN_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

namespace parsing
{
	using namespace std;

	/*! for matching patterns in strings. */
	class Pattern
	{
	public:
		/*! different types for algorithms. */
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

		/*! different types of attachments. */
		typedef enum
		{
			attachAnd,
			attachOr,
			attachAndNot,
			attachOrNot,
		} attachmentType;

		/*! the callback type, for when you need to define your own algorithm type. */
		typedef unsigned int (*callback)(string s);

	private:
		algorithmType algorithm;
		string argument;
		callback alternateCallback;

		vector<pair<attachmentType, Pattern> > attachments;

	public:
		/*! initiailizes the pattern with no algorithm. */
		Pattern() : algorithm(alternate), alternateCallback(NULL) {}

		/*! initializes the pattern with an algorithm type and a string argument. */
		Pattern(algorithmType a, string s) : algorithm(a), argument(s), alternateCallback(NULL) {}
		
		/*! initializes the pattern with a callback algorithm. */
		Pattern(callback a) : alternateCallback(a) {}

		/*! sets the algorithm type. 
		\return a reference to the current pattern
		*/
		Pattern &setAlgorithm(algorithmType a);

		/*! sets the algorithm's string argument. 
		\return a reference to the current pattern
		*/
		Pattern &setArgument(string a);

		/*! sets the callback. 
		\return a reference to the current pattern
		*/
		Pattern &setAlternate(callback a);

		/*! attaches another pattern. 
		\return a reference to the current pattern
		*/
		Pattern &attach(attachmentType t, Pattern p);
		
		/*! initializes the pattern with `Pattern(isEqualTo, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator == (string s);

		/*! initializes the pattern with `Pattern(isNotEqualTo, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator != (string s);

		/*! initializes the pattern with `Pattern(onlyContains, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator += (string s);

		/*! initializes the pattern with `Pattern(doesNotContain, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator -= (string s);

		/*! initializes the pattern with `Pattern(startsWith, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator < (string s);

		/*! initializes the pattern with `Pattern(endsWith, s)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator > (string s);

		/*! initializes the pattern with `Pattern(a)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator () (callback a);

		/*! attaches another pattern with `attach(attachAnd, other)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator && (Pattern other);

		/*! attaches another pattern with `attach(attachOr, other)'. 
		\return a reference to the current pattern
		*/
		Pattern &operator || (Pattern other);

		/*! compares the current pattern with `other'. 
		\return `true' if the patterns are equivalent, `false' otherwise
		*/
		bool compare(Pattern other);

		/*! gets the minimum possible length of the pattern (used for sorting). 
		\return the minimum possible length
		*/
		unsigned int getLengthRequest();

		/*! checks if the pattern matches `s'. 
		\return `0' if there is no match, `n' if there is where `n' is the number of tokens contained within the pattern
		*/
		unsigned int match(string s);

		/*! displays the contents of the pattern. 
		\return a formatted string
		*/
		string display();
	};

	extern string digits;
	extern string lettersLower;
	extern string lettersUpper;
}

#endif
