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

#include "trace.h"
#include "tokenizer.h"

namespace trace
{
	using namespace parsing;

	map<string, string> moduleTable;
	map<pid_t, Backtrace> stackTable;
	map<pid_t, bool> debugTable;
	map<string, bool> flags;

	void Backtrace::setFramesToCapture(unsigned int f)
	{
		framesToCapture = f;
	}

	void Backtrace::capture()
	{
		void *stack[framesToCapture];
		int frames = backtrace(stack, framesToCapture);
		char **symbols = backtrace_symbols(stack, frames);

		Tokenizer t = Tokenizer();
		t.whitespace(t() == " ");
		t.whitespace(t() == "\t");
		t.whitespace(t() == "\n");
		t.whitespace(t() == "\r");

		for (int i = 0; i < frames; i++)
		{
			vector<Token> toks = t.tokenize(string(symbols[i]), "--");
			traceElement e;
			e.exec = toks[1].get();
			e.addr = toks[2].get();
			size_t namebufsize = 1024;
			int status;
			char *namebuf = (char *)malloc(namebufsize);
			if ((namebuf = abi::__cxa_demangle(toks[3].get().c_str(), namebuf, &namebufsize, &status)) && status == 0)
			{
				if (string(namebuf).find("trace::") != string::npos)
					continue;
				e.sym = string(namebuf);
			}
			else
			{
				e.sym = toks[3].get();
			}
			//free(namebuf);
			data.push_back(e);
		}
		//free(symbols);
	}

	unsigned int Backtrace::size()
	{
		return data.size();
	}

	Backtrace::traceElement &Backtrace::operator [] (int i)
	{
		return data[i];
	}

	void Backtrace::push(string e, string a, string s)
	{
		traceElement tmp;
		tmp.exec = e;
		tmp.addr = a;
		tmp.sym = s;
		data.push_back(tmp);
	}

	void Backtrace::push(string s)
	{
		traceElement tmp;
		tmp.sym = s;
		data.push_back(tmp);
	}

	Backtrace::traceElement Backtrace::pop()
	{
		Backtrace::traceElement tmp;
		if (data.empty() == false)
		{
			tmp = data.back();
			data.pop_back();
		}
		return tmp;
	}

	string Backtrace::dump()
	{
		stringstream ss;
		for (vector<traceElement>::iterator i = data.begin(); i != data.end(); i++)
			ss << i->exec << " " << i->addr << " " << i->sym << "\n";
		return ss.str();
	}

	void Backtrace::clear()
	{
		data.clear();
	}

	string getParentMethod()
	{
		Backtrace b;
		b.setFramesToCapture(3);
		b.capture();
		return b[0].sym;
	}

	void initFlags(int argc, char *argv[])
	{
		flags["show-data"] = false;
		flags["show-debug"] = false;
		flags["use-color"] = true;

		for (int i = 1; i < argc; i++)
		{
			if (string(argv[i]).compare("--trace-show-data") == 0)
				flags["show-data"] = true;
			else if (string(argv[i]).compare("--trace-show-debug") == 0)
				flags["show-debug"] = true;
			else if (string(argv[i]).compare("--trace-disable-color") == 0)
				flags["use-color"] = false;
			else if (string(argv[i]).compare("--trace-help") == 0)
			{
				cout << TRACE_GREEN << "Trace 0.01 Alpha\n\n" << TRACE_DEFAULT;
				cout << argv[0] << " [TRACE OPTIONS]\n\n";
				cout << "  TRACE OPTIONS:\n";
				cout << "--trace-show-data      show data flow messages from TRACE_INDATA and TRACE_OUTDATA\n";
				cout << "--trace-show-debug     show debug messages from TRACE_COUT\n";
				cout << "--trace-disable-color  disable use of terminal colors\n";
				cout << "--trace-help           show this help screen\n\n";
				cout << TRACE_GREEN << "* * *\n" << TRACE_DEFAULT;
				cout.flush();
				_exit(0);
			}
		}
	}
}