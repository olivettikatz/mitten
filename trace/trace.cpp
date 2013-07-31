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

namespace trace
{
	map<string, string> moduleTable;
	map<pid_t, Backtrace> stackTable;
	map<pid_t, bool> debugTable;
	map<string, bool> flags;
	vector<string> moduleFilters;
	vector<string> methodFilters;
	vector<string> dataFilters;
	int msgCounter;
	int msgCounterFilter;

	void Backtrace::setFramesToCapture(unsigned int f)
	{
		framesToCapture = f;
	}

	vector<string> Backtrace::tokenize(string s)
	{
		vector<string> rtn;
		string buf;

		for (string::iterator i = s.begin(); i != s.end(); i++)
		{
			if (string(" \t\r\n").find(*i) != string::npos)
			{
				if (buf.empty() == false)
				{
					rtn.push_back(buf);
					buf = "";
				}
			}
			else
			{
				buf += *i;
			}
		}

		if (buf.empty() == false)
		{
			rtn.push_back(buf);
		}

		return rtn;
	}

	void Backtrace::capture()
	{
		void *stack[framesToCapture];
		int frames = backtrace(stack, framesToCapture);
		char **symbols = backtrace_symbols(stack, frames);

		for (int i = 0; i < frames; i++)
		{
			vector<string> toks = tokenize(string(symbols[i]));
			traceElement e;
			e.exec = toks[1];
			e.addr = toks[2];
			size_t namebufsize = 1024;
			int status;
			char *namebuf = (char *)malloc(namebufsize);
			if ((namebuf = abi::__cxa_demangle(toks[3].c_str(), namebuf, &namebufsize, &status)) && status == 0)
			{
				if (string(namebuf).find("trace::") != string::npos)
					continue;
				e.sym = string(namebuf);
			}
			else
			{
				e.sym = toks[3];
			}
			data.push_back(e);
		}
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

	void initFlags(int *argc, char *argv[])
	{
		msgCounter = 0;
		msgCounterFilter = 0;

		flags["show-data"] = false;
		flags["show-debug"] = false;
		flags["use-color"] = true;
		flags["use-n"] = false;

		vector<char *> newargv;

		for (int i = 1; i < *argc; i++)
		{
			if (string(argv[i]).compare("--trace-show-data") == 0)
			{
				flags["show-data"] = true;
			}
			else if (string(argv[i]).compare("--trace-show-debug") == 0)
			{
				flags["show-debug"] = true;
			}
			else if (string(argv[i]).compare("--trace-show-all") == 0)
			{
				flags["show-data"] = true;
				flags["show-debug"] = true;
			}
			else if (string(argv[i]).compare("--trace-disable-color") == 0)
			{
				flags["use-color"] = false;
			}
			else if (string(argv[i]).compare("--trace-enable-n") == 0)
			{
				flags["use-n"] = true;
			}
			else if (string(argv[i]).compare("--trace-filter-module") == 0)
			{
				if (++i >= *argc)
				{
					cerr << "error: --trace-filter-module requires an argument\n";
					_exit(1);
				}

				moduleFilters.push_back(string(argv[i]));
			}
			else if (string(argv[i]).compare("--trace-filter-method") == 0)
			{
				if (++i >= *argc)
				{
					cerr << "error: --trace-filter-method requires an argument\n";
					_exit(1);
				}

				methodFilters.push_back(string(argv[i]));
			}
			else if (string(argv[i]).compare("--trace-filter-data") == 0)
			{
				if (++i >= *argc)
				{
					cerr << "error: --trace-filter-data requires an argument\n";
					_exit(1);
				}

				dataFilters.push_back(string(argv[i]));
				TRACE_DISABLE_DEBUG();
			}
			else if (string(argv[i]).compare("--trace-filter-n") == 0)
			{
				flags["use-n"] = true;

				if (++i >= *argc)
				{
					cerr << "error: --trace-filter-n requires an argument\n";
					_exit(1);
				}

				msgCounterFilter = b64toi(string(argv[i]));
			}
			else if (string(argv[i]).compare("--trace-help") == 0)
			{
				cout << TRACE_GREEN << "Trace 0.01 Alpha\n\n" << TRACE_DEFAULT;
				cout << argv[0] << " [TRACE OPTIONS]\n\n";
				cout << "  TRACE OPTIONS:\n";
				cout << "--trace-show-data                       show data flow messages from\n";
				cout << "                                        TRACE_INDATA and TRACE_OUTDATA\n";
				cout << "--trace-show-debug                      show debug messages from\n";
				cout << "                                        TRACE_COUT\n";
				cout << "--trace-show-all                        show all messages\n";
				cout << "--trace-filter-module <MODULE PATTERN>  show only messages from modules\n";
				cout << "                                        with names containing\n";
				cout << "                                        <MODULE PATTERN> (can be used\n";
				cout << "                                        multiple times)\n";
				cout << "--trace-filter-method <METHOD PATTERN>  show only messages from methods\n";
				cout << "                                        with names containing\n";
				cout << "                                        <METHOD PATTERN> (can be used\n";
				cout << "                                        multiple times)\n";
				cout << "--trace-filter-data <DATA PATTERN>      show only messages from methods\n";
				cout << "                                        with input data containing\n";
				cout << "                                        <DATA PATTERN> (can be used\n";
				cout << "                                        multiple times)\n";
				cout << "--trace-filter-n <MSG CODE>             only display messages around and\n";
				cout << "                                        after a message with <MSG CODE>\n";
				cout << "                                        is generated\n";
				cout << "--trace-disable-color                   disable use of terminal colors\n";
				cout << "--trace-help                            show this help screen\n";
				cout << TRACE_GREEN << "* * *\n" << TRACE_DEFAULT;
				cout.flush();
				_exit(0);
			}
			else
			{
				newargv.push_back(argv[i]);
			}
		}

		for (int i = 0; i < newargv.size(); i++)
		{
			argv[i+1] = newargv[i];
		}

		for (int i = newargv.size()+1; i < *argc; i++)
		{
			argv[i] = NULL;
		}

		*argc = newargv.size()+1;
	}

	bool canDisplayMessages(string module, string method, int &mc, int mcf)
	{
		bool rtn = false;

		for (vector<string>::iterator i = moduleFilters.begin(); i != moduleFilters.end(); i++)
		{
			if (module.find(*i) != string::npos)
			{
				rtn = true;
				break;
			}
		}

		for (vector<string>::iterator i = methodFilters.begin(); i != methodFilters.end(); i++)
		{
			if (method.find(*i) != string::npos)
			{
				rtn = true;
				break;
			}
		}

		if (rtn == false && (moduleFilters.empty() && methodFilters.empty()))
			rtn = true;

		if (rtn == true)
		{
			return TRACE_IS_DEBUG() && (mc++ > mcf-5);
		}
		else
		{
			return rtn;
		}
	}

	bool canDisplayMessages(string data)
	{
		bool rtn = false;

		for (vector<string>::iterator i = dataFilters.begin(); i != dataFilters.end(); i++)
		{
			if (data.find(*i) != string::npos)
			{
				rtn = true;
			}
		}

		if (dataFilters.empty())
			rtn = true;

		if (rtn)
		{
			return TRACE_IS_DEBUG();
		}
		else
		{
			return false;
		}
	}

	string base64 = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

	string itob64(int i)
	{
		if (i == 0)
		{
			return "0";
		}

		string tmp;

		while (i > 0)
		{
			tmp.insert(tmp.begin(), base64[i%64]);
			i /= 64;
		}

		return tmp;
	}

	int b64toi(string b64)
	{
		int i = 0;
		for (int e = 0; e < b64.size(); e++)
		{
			i += base64.find(b64[e])*pow(64, (b64.size()-e-1));
		}
		return i;
	}

	string getModule(string file)
	{
		if (moduleTable.find(file) == moduleTable.end())
		{
			return file;
		}
		else
		{
			return moduleTable[file];
		}
	}

	Backtrace getStack()
	{
		if (stackTable.find(TRACE_PID) == stackTable.end())
		{
			return Backtrace();
		}
		else
		{
			return stackTable[TRACE_PID];
		}
	}

	string createCoutPrefix(string module, unsigned int line)
	{
		stringstream ss;
		ss << "[" << module << ":" << line;
		if (flags["use-n"])
			ss << ":" << itob64(msgCounter);
		ss << "] " << string(getStack().size()+(unsigned int)log10((double)line), ' ');
		return ss.str();
	}

	void indata(string d, string m)
	{
		if (dataFilters.empty() == false)
			debugTable[TRACE_PID] = canDisplayMessages(d);

		if (stackTable.find(TRACE_PID) == stackTable.end())
		{
			stackTable[TRACE_PID] = Backtrace();
			stackTable[TRACE_PID].push(m);
		}

		if (flags["show-data"])
			TRACE_COUT_SHOW << "<= " << d << "\n";
	}
}