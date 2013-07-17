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
		if (k.size() > 2 && (k[0] == '_' && k[1] == '_'))
			return content[""].value;
		return content[k].value;
	}

	void Configuration::desc(string k, string d)
	{
		if (k.size() > 2 && (k[0] == '_' && k[1] == '_'))
			return ;
		content[k].desc = d;
	}

	void Configuration::help(string k, string h)
	{
		if (k.size() > 2 && (k[0] == '_' && k[1] == '_'))
			return ;
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

			if (a.compare("--help") == 0)
			{
				cout << content["__name__"].value.getString() << " " << content["__version__"].value.getString() << "\n";
				cout << content["__name__"].desc << "\n";
				cout << "\n";
				cout << content["__name__"].help << "\n";

				map<string, bool> doneAlready;

				for (map<char, string>::iterator j = shorthandTable.begin(); j != shorthandTable.end(); j++)
				{
					cout << "-" << j->first << ", --" << j->second << "  " << content[j->second].desc << "\n  ";
					if (content[j->second].help.empty())
					{
						cout << "\n";
					}
					else
					{
						cout << content[j->second].help << "\n\n";
					}
					doneAlready[j->second] = true;
				}

				for (map<string, contentElement>::iterator j = content.begin(); j != content.end(); j++)
				{
					if (doneAlready.find(j->first) != doneAlready.end())
						continue;
					else if (j->first.size() > 2 && (j->first[0] == '_' && j->first[1] == '_'))
						continue;
					cout << "--" << j->first << "  " << j->second.desc << "\n  ";
					if (j->second.help.empty())
						cout << "\n";
					else
						cout << j->second.help << "\n\n";
				}

				cout << "--help  display this help\n\n";
				cout << "--copying  display copying information\n\n";
				cout << "--origin  display origin information\n\n";
				cout << "--version  display version information\n\n";

				_exit(0);
			}
			else if (a.compare("--copying") == 0)
			{
				cout << content["__copying__"].value.getString() << "\n";
				_exit(0);
			}
			else if (a.compare("--origin") == 0)
			{
				cout << content["__author__"].value.getString() << " " << content["__date__"].value.getString() << "\n";
				_exit(0);
			}
			else if (a.compare("--version") == 0)
			{
				cout << content["__name__"].value.getString() << " " << content["__version__"].value.getString() << "\n";
				_exit(0);
			}

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

			if (k.size() > 128)
			{
				cerr << "error: paramter key longer than 128 characters: " << k.substr(0, 32) << " (first 32)\n";
				_exit(1);
			}

			if (v.size() > 128)
			{
				cerr << "error: paramter value longer than 128 characters: " << k << " (key)\n";
				_exit(1);
			}

			for (string::iterator j = v.begin(); j != v.end(); j++)
			{
				if (!isprint(*j))
				{
					cerr << "error: paramter value contains unprintable character(s): " << k << " (key)\n";
					_exit(1);
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