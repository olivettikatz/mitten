#ifndef __TRACE_H
#define __TRACE_H

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

	class Backtrace
	{
	public:
		typedef struct
		{
			string exec;
			string addr;
			string sym;
		} traceElement;

	private:
		unsigned int framesToCapture;
		vector<traceElement> data;

	public:
		Backtrace() : framesToCapture(128) {}
		void setFramesToCapture(unsigned int f);
		void capture();
		unsigned int size();
		traceElement &operator [] (int i);
		void push(string e, string a, string s);
		void push(string s);
		traceElement pop();
		string dump();
		void clear();
	};

	string getParentMethod();
	extern map<string, string> moduleTable;
	extern map<pid_t, Backtrace> stackTable;
	extern map<pid_t, bool> debugTable;
	extern map<string, bool> flags;
	void initFlags(int argc, char *argv[]);
}

#define TRACE_INIT(argc, argv) trace::initFlags(argc,argv)
#define TRACE_LINE __LINE__
#define TRACE_METHOD __PRETTY_FUNCTION__
#define TRACE_METHOD_PARENT trace::getParentMethod()
#define TRACE_FILE __FILE__
#define TRACE_PID getpid()
#define TRACE_MODULE_MAP(file, name) (trace::moduleTable[file]=string(name))
#define TRACE_MODULE (trace::moduleTable.find(TRACE_FILE)==trace::moduleTable.end()?TRACE_FILE:trace::moduleTable[TRACE_FILE])
#define TRACE_STACK (trace::stackTable.find(TRACE_PID)==trace::stackTable.end()?trace::Backtrace():trace::stackTable[TRACE_PID])
#define TRACE_COUT_SHOW if(TRACE_IS_DEBUG())cout<<"["<<TRACE_MODULE<<":"<<TRACE_LINE<<"] "<<string(TRACE_STACK.size()+(TRACE_LINE<10?1:0)+(TRACE_LINE<100?1:0)+(TRACE_LINE<1000?1:0),' ')
#define TRACE_COUT if(trace::flags["show-debug"])TRACE_COUT_SHOW
#define TRACE_INDATA(d) {if(trace::stackTable.find(TRACE_PID)==trace::stackTable.end())trace::stackTable[TRACE_PID]=trace::Backtrace();trace::stackTable[TRACE_PID].push(TRACE_METHOD);if(trace::flags["show-data"]){TRACE_COUT_SHOW<<"<= "<<d<<"\n";}}
#define TRACE_OUTDATA(d) {if(trace::flags["show-data"]){TRACE_COUT_SHOW<<"=> "<<d<<"\n";}trace::stackTable[TRACE_PID].pop();}
#define TRACE_IS_DEBUG() (trace::debugTable.find(TRACE_PID)==trace::debugTable.end()?false:trace::debugTable[TRACE_PID])
#define TRACE_ENABLE_DEBUG() (trace::debugTable[TRACE_PID]=true)
#define TRACE_DISABLE_DEBUG() (trace::debugTable[TRACE_PID]=false)
#define TRACE_SHOW_DATA() (trace::flags["show-data"]=true)
#define TRACE_SHOW_DEBUG() (trace::flags["show-debug"]=true)
#define TRACE_DISABLE_COLOR() (trace::flags["use-color"]=false)

#ifndef __TRACE_DISABLE_COLOR
#define TRACE_RED (trace::flags["use-color"]?"\033[0;31m":"")
#define TRACE_GREEN (trace::flags["use-color"]?"\033[0;32m":"")
#define TRACE_YELLOW (trace::flags["use-color"]?"\033[0;33m":"")
#define TRACE_BLUE (trace::flags["use-color"]?"\033[0;34m":"")
#define TRACE_MAGENTA (trace::flags["use-color"]?"\033[0;35m":"")
#define TRACE_CYAN (trace::flags["use-color"]?"\033[0;36m":"")
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