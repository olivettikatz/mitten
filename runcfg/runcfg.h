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

#ifndef __MITTEN_RUNCFG_H
#define __MITTEN_RUNCFG_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <ctype.h>
#include <unistd.h>

namespace runcfg
{
	using namespace std;

	/*! for storing configuration values. */
	class Value
	{
	public:
		typedef enum
		{
			_int,
			_string,
			_bool
		} type;

	private:
		type _type;
		int _intv;
		string _stringv;

	public:
		/*! initializes `(bool)false' value. */
		Value() : _type(_bool), _intv(0) {}

		/*! initializes integer value. */
		Value(int v) : _type(_int), _intv(v) {}

		/*! initializes string value. */
		Value(string v) : _type(_string), _stringv(v) {}

		/*! initializes value with type, integer initializer. */
		Value(type t, int v) : _type(t), _intv(v) {}

		/*! initializes value with type, string initializer. */
		Value(type t, string v) : _type(t), _stringv(v) {}

		/*! gets the type. */
		type getType();

		/*! gets the integer value. */
		int getInt();

		/*! sets the integer value. */
		int setInt(int v);

		/*! get the string value. */
		string getString();

		/*! sets the string value. */
		string setString(string v);

		/*! gets the bool value. */
		bool getBool();

		/*! sets the bool value. */
		bool setBool(bool v);
	};

	/*! for storing all configuration data. */
	class Configuration
	{
	private:
		typedef struct
		{
			Value value;
			string desc;
			string help;
		} contentElement;

		map<char, string> shorthandTable;
		map<string, contentElement> content;

	public:
		/*! initializes empty configuration. */
		Configuration() {}

		/*! gets a reference to a value of a certain name. */
		Value &operator [] (string k);

		/*! assigns a description to a key. */
		void desc(string k, string d);

		/*! assigns a help string to a key. */
		void help(string k, string h);

		/*! sets the name of the application. */
		void setBuiltinName(string n);

		/*! sets the version of the application. */
		void setBuiltinVersion(string v);

		/*! sets the author of the application. */
		void setBuiltinAuthor(string a);

		/*! sets the date of publishing of the application. */
		void setBuiltinDate(string d);

		/*! sets the copying string of the application (licensing or copyright information). */
		void setBuiltinCopying(string c);

		/*! sets the description of the application. */
		void setBuiltinDescription(string d);

		/*! sets the help string of the application. */
		void setBuiltinHelp(string h);

		/*! assigns a key to a character for command line shorthanding. */
		void shorthand(char c, string k);

		/*! initializes the configuration from the command line. */
		void init(int argc, char *argv[]);
	};
}

#endif