Overiew {#mainpage}
=======

Welcome to the documentation for the trace library. It was developed alongside Mitten and is used extensively in Mitten. The library is thread-safe and is accessed mainly through a set of macros. 

Basic Use
---------
example.cpp:

	#include <iostream>
	#include <trace.h>

	int add(int a, int b)
	{
		TRACE_INDATA(a << " + " << b);
		TRACE_COUT << "adding " << a << " to " << b << "\n";
		TRACE_OUTDATA((a+b));
		return a+b;
	}

	int main(int argc, char *argv[])
	{
		TRACE_INIT(argc, argv);
		return add(5, 2);
	}

will produce the output:

	[example.cpp:15] <= 5 + 2
	[example.cpp:16] adding 5 to 2
	[example.cpp:17] => 7

you need to call TRACE_INIT at the beginning of the main function for tracing to work. 

Further Info
------------

Call the executable with '--trace-help' to see more info about command line arguments. See the documentation for more info about using the library.