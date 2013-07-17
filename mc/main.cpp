#include <iostream>
#include <trace/trace.h>
#include <runcfg/runcfg.h>

int main(int argc, char *argv[])
{
	TRACE_INIT(argc, argv);

	runcfg::Configuration cfg;
	cfg.setBuiltinName("mc");
	cfg.setBuiltinVersion("0.01 Alpha");
	cfg.setBuiltinAuthor("Oliver Katz");
	cfg.setBuiltinDate("July 17, 2013");
	cfg.setBuiltinCopying("Copyright 2013 Oliver Katz");
	cfg.setBuiltinDescription("A compiler for the Mitten programming language.");
	cfg.setBuiltinHelp("for example, mc -l test.n -o test.bin");
	cfg.init(argc, argv);

	return 0;
}