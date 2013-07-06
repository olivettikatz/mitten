#include "trace.h"
#include "../parsing/lexical/tokenizer.h"

namespace trace
{
	using namespace parsing;

	map<string, string> moduleTable;
	map<pid_t, Backtrace> stackTable;
	map<pid_t, bool> debugTable;

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
}