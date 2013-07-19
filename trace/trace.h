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

#ifndef __MITTEN_TRACE_H
#define __MITTEN_TRACE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>

namespace trace
{
	using namespace std;

	/*! for containing backtrace and stack data. */
	class Backtrace
	{
	public:
		/*! for containing a single frame in the stack. */
		typedef struct
		{
			string exec; //!< the executable binary name
			string addr; //!< the address in memory
			string sym; //!< the symbol associated with the frame
		} traceElement;

	private:
		unsigned int framesToCapture;
		vector<traceElement> data;

		vector<string> tokenize(string s);

	public:
		/*! an initializer. */
		Backtrace() : framesToCapture(128) {}

		/*! sets the number of frames (starting from the top) to capture when capturing backtrace data from the system. 
		\param f the number of frames to capture (default: 128)
		*/
		void setFramesToCapture(unsigned int f);

		/*! captures backtrace data from the system. */
		void capture();

		/*! returns the size of the backtrace. 
		\return the size of the backtrace
		*/
		unsigned int size();

		/*! returns a reference to the i-th element in the backtrace. 
		\param i the index of the element (must not exceed the upper limit of the stack)
		\return a reference to the element
		*/
		traceElement &operator [] (int i);

		/*! push a new element to the top of the stack. 
		\param e the name of the executable
		\param a the address in memory
		\param s the symbol associated with the frame
		*/
		void push(string e, string a, string s);

		/*! push a new element to the top of the stack. 
		\param s the symbol associated with the frame
		*/
		void push(string s);

		/*! pops an element from the top of the stack. 
		\return the element on the top of the stack
		*/
		traceElement pop();

		/*! dumps the contents into a formatted string. 
		\return a formatted string
		*/
		string dump();

		/*! erases all data in the backtrace. */
		void clear();
	};

	/*! returns the parent method's declaration. 
	\return a string containing the declaration line of the parent method
	*/
	string getParentMethod();
	extern map<string, string> moduleTable;
	extern map<pid_t, Backtrace> stackTable;
	extern map<pid_t, bool> debugTable;
	extern map<string, bool> flags;

	/*! initializes the trace library from command line arguments (call executable using this function with argument `--trace-help' for more info). 
	\param argc pass 'argc' from the main function
	\param argv pass 'argv' from the main function
	*/
	void initFlags(int *argc, char *argv[]);
}

/*! initializer for the library. 
\param argc pass 'argc' from the main function
\param argv pass 'argv' from the main function
\sa initFlags
*/
#define TRACE_INIT(argc, argv) trace::initFlags(&argc,argv)

/*! current line number in file. */
#define TRACE_LINE __LINE__

/*! current method declaration. */
#define TRACE_METHOD __PRETTY_FUNCTION__

/*! parent method's declaration. */
#define TRACE_METHOD_PARENT trace::getParentMethod()

/*! current file. */
#define TRACE_FILE __FILE__

/*! current process ID. */
#define TRACE_PID getpid()

/*! maps a module name to a filename. 
\param file the filename to map to a certain module name
\param name the module name to use (i.e. `RDP Parser`)
*/
#define TRACE_MODULE_MAP(file, name) (trace::moduleTable[file]=string(name))

/*! gets the current module name in the current file. */
#define TRACE_MODULE (trace::moduleTable.find(TRACE_FILE)==trace::moduleTable.end()?TRACE_FILE:trace::moduleTable[TRACE_FILE])

/*! gets the Backtrace object of the current stack in this thread. */
#define TRACE_STACK (trace::stackTable.find(TRACE_PID)==trace::stackTable.end()?trace::Backtrace():trace::stackTable[TRACE_PID])
#define TRACE_COUT_SHOW if(TRACE_IS_DEBUG())std::cout<<"["<<TRACE_MODULE<<":"<<TRACE_LINE<<"] "<<string(TRACE_STACK.size()+(TRACE_LINE<10?1:0)+(TRACE_LINE<100?1:0)+(TRACE_LINE<1000?1:0),' ')

/*! sends a debug message to stdout with prefix if `--trace-show-debug' is enabled (acts like `cout'). */
#define TRACE_COUT if(trace::flags["show-debug"])TRACE_COUT_SHOW

/*! displays data flow to stdout with prefix if `--trace-show-data' is enabled. 
\param d a string to display as the data coming into the method
*/
#define TRACE_INDATA(d) {if(trace::stackTable.find(TRACE_PID)==trace::stackTable.end())trace::stackTable[TRACE_PID]=trace::Backtrace();trace::stackTable[TRACE_PID].push(TRACE_METHOD);if(trace::flags["show-data"]){TRACE_COUT_SHOW<<"<= "<<d<<"\n";}}

/*! displays data flow to stdout with prefix if `--trace-show-data' is enabled. 
\param d a string to display as the data going out of the method
*/
#define TRACE_OUTDATA(d) {if(trace::flags["show-data"]){TRACE_COUT_SHOW<<"=> "<<d<<"\n";}trace::stackTable[TRACE_PID].pop();}

/*! returns if debugging is enabled in program. */
#define TRACE_IS_DEBUG() (trace::debugTable.find(TRACE_PID)==trace::debugTable.end()?false:trace::debugTable[TRACE_PID])

/*! enabled debugging in program. */
#define TRACE_ENABLE_DEBUG() (trace::debugTable[TRACE_PID]=true)

/*! disabled debugging in program. */
#define TRACE_DISABLE_DEBUG() (trace::debugTable[TRACE_PID]=false)

/*! forces the enabling of '--trace-show-data'. */
#define TRACE_SHOW_DATA() (trace::flags["show-data"]=true)

/*! forces the enabling of '--trace-show-debug'. */
#define TRACE_SHOW_DEBUG() (trace::flags["show-debug"]=true)

/*! disables the use of terminal colors. */
#define TRACE_DISABLE_COLOR() (trace::flags["use-color"]=false)

#ifndef __TRACE_DISABLE_COLOR
/*! a string containing red. */
#define TRACE_RED (trace::flags["use-color"]?"\033[0;31m":"")

/*! a string containing green. */
#define TRACE_GREEN (trace::flags["use-color"]?"\033[0;32m":"")

/*! a string containing yellow. */
#define TRACE_YELLOW (trace::flags["use-color"]?"\033[0;33m":"")

/*! a string containing blue. */
#define TRACE_BLUE (trace::flags["use-color"]?"\033[0;34m":"")

/*! a string containing magenta. */
#define TRACE_MAGENTA (trace::flags["use-color"]?"\033[0;35m":"")

/*! a string containing cyan. */
#define TRACE_CYAN (trace::flags["use-color"]?"\033[0;36m":"")

/*! a string containing the default terminal text color. */
#define TRACE_DEFAULT (trace::flags["use-color"]?"\033[0;0m":"")
#else
#define TRACE_RED ""
#define TRACE_GREEN ""
#define TRACE_YELLOW ""
#define TRACE_BLUE ""
#define TRACE_MAGENTA ""
#define TRACE_CYAN ""
#define TRACE_DEFAULT ""
#endif

#endif