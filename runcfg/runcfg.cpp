#include "runcfg.h"

namespace runcfg
{
	Value::type Value::getType()
	{
		return _type;
	}

	int Value::getInt()
	{
		return _intv;
	}

	int Value::setInt(int v)
	{
		return (_intv = v);
	}

	string Value::getString()
	{
		return _stringv;
	}

	string Value::setString(string v)
	{
		return (_stringv = v);
	}

	bool Value::getBool()
	{
		return _intv;
	}

	bool Value::setBool(bool v)
	{
		return (_intv = v);
	}

	Value &Configuration::operator [] (string k)
	{
		return content[k].value;
	}

	void Configuration::desc(string k, string d)
	{
		content[k].desc = d;
	}

	void Configuration::help(string k, string h)
	{
		content[k].help = h;
	}

	void Configuration::setBuiltinName(string n)
	{
		content["__name__"].value = Value(n);
	}

	void Configuration::setBuiltinVersion(string v)
	{
		content["__version__"].value = Value(v);
	}

	void Configuration::setBuiltinAuthor(string a)
	{
		content["__author__"].value = Value(a);
	}

	void Configuration::setBuiltinDate(string d)
	{
		content["__date__"].value = Value(d);
	}

	void Configuration::setBuiltinCopying(string c)
	{
		content["__copying__"].value = Value(c);
	}

	void Configuration::setBuiltinDescription(string d)
	{
		content["__name__"].desc = d;
	}

	void Configuration::setBuiltinHelp(string h)
	{
		content["__name__"].help = h;
	}

	void Configuration::shorthand(char c, string k)
	{
		shorthandTable[c] = k;
	}

	void Configuration::init(int argc, char *argv[])
	{
		for (int i = 1; i < argc; i++)
		{
			string a = argv[i];
			string k, v;

			if (a.size() > 2 && (a[0] == '-' && a[1] == '-'))
			{
				if (a.find("=") != string::npos)
				{
					k = a.substr(2, a.find("="));
					v = a.substr(a.find("=")+1);
				}
				else
				{
					k = a.substr(2);
				}
			}
			else if (a.size() > 1 && a[0] == '-')
			{
				if (i < argc-1 && string(argv[i+1])[0] != '-')
				{
					k = shorthandTable[a[1]];
					v = argv[i+1];
					i++;
				}
				else
				{
					k = shorthandTable[a[1]];
				}
			}

			if (content.find(k) == content.end())
			{
				cerr << "error: unknown parameter: " << k << "\n";
				_exit(1);
			}

			if (content[k].value.getType() == Value::_int)
			{
				content[k].value.setInt(atoi(v.c_str()));
			}
			else if (content[k].value.getType() == Value::_string)
			{
				content[k].value.setString(v);
			}
			else if (content[k].value.getType() == Value::_bool)
			{
				content[k].value.setBool(true);
			}
		}
	}
}