#include "parser.h"

namespace parsing
{
	void ParsingEnviron::error(string msg)
	{
		errors.push_back(ASTError(**page, msg));
	}

	void ParsingEnviron::error(vector<ASTError> e)
	{
		errors.insert(errors.end(), e.begin(), e.end());
	}

	void ParsingEnviron::errorUnexpected(Expectation &e)
	{
		errors.push_back(ASTError(**page, e, ""));
	}

	void ParsingEnviron::dump()
	{
		unsigned int stackmem = 0;
		for (vector<stackElement>::iterator i = stack.begin(); i != stack.end(); i++)
			stackmem += (i->cls.size()+i->name.size())*sizeof(char)+sizeof(TokenPage::iterator);
		cout << pad() << "[Mitten Parsing Environment] Stack size: " << stack.size() << " (" << stackmem/1000.0 << "Kb)\n";
	}

	void ParsingEnviron::dumpStack()
	{
		cout << "[Mitten Parsing Environment] Stack dump -";
		for (vector<stackElement>::iterator i = stack.begin(); i != stack.end(); i++)
			cout << " " << i->cls << ":" << i->name << "@" << i->off.getLocation();
		cout << "\n";
	}

	string ParsingEnviron::dumpErrors()
	{
		stringstream ss;
		for (vector<ASTError>::iterator i = errors.begin(); i != errors.end(); i++)
			ss << i->display();
		return ss.str();
	}

	string ParsingEnviron::pad()
	{
		return string(stack.size(), ' ');
	}

	void Expectation::debugInput(ParsingEnviron &env)
	{
		if (isDebugging())
		{
			if (env.alreadyDisplayed != env.page->pullOffset())
			{
				env.alreadyDisplayed = env.page->pullOffset();
				cout << env.pad() << "Parsing [" << cls << ":\033[0;31m" << name << "\033[0;0m]:";
				for (TokenPage::iterator i = env.page->pullOffset(); i < env.page->pullOffset()+10 && i < env.page->size(); i++)
					cout << " '\033[0;32m" << i->get() << "\033[0;0m'";
				cout << "\n";
			}
			else
			{
				cout << env.pad() << "Passing to [" << cls << ":\033[0;31m" << name << "\033[0;0m]\n";
			}
		}
	}

	void Expectation::debugOutput(AST rtn, ParsingEnviron &env)
	{
		if (isDebugging() && rtn.empty() == false)
			cout << env.pad() << "Returning (" << cls << " '" << name << "'): \033[0;33m" << rtn.display() << "\033[0;0m\n";
	}

	unsigned int Expectation::minLength()
	{
		return handlerMinLength(*this);
	}

	void Expectation::enableDebuggingNonRecursive()
	{
		debug = true;
	}

	void Expectation::enableDebugging()
	{
		if (debug == false)
		{
			enableDebuggingNonRecursive();
			for (vector<Expectation *>::iterator i = children.begin(); i != children.end(); i++)
			{
				(*i)->enableDebugging();
			}
		}
	}

	bool Expectation::isDebugging()
	{
		return debug;
	}

	string Expectation::prefix(ParsingEnviron &env)
	{
		stringstream ss;
		ss << env.pad() << "[" << cls << ":" << name << "]";
		return ss.str();
	}

	string Expectation::current(ParsingEnviron &env)
	{
		stringstream ss;
		ss << "'\033[0;32m" << env.page->pullOffset()->get() << "\033[0;0m'";
		return ss.str();
	}

	void Expectation::assumeSize(unsigned int s)
	{
		sizeOverride = (signed int)s;
	}

	void Expectation::append(Expectation &e)
	{
		if (appendRanked)
		{
			vector<Expectation * >::iterator i = children.begin();
			for (; i != children.end(); i++)
			{
				if ((*i)->minLength() <= e.minLength())
				{
					if (i != children.begin())
						i--;
					break;
				}
			}
			children.insert(i, &e);
		}
		else
		{
			children.push_back(&e);
		}
	}

	bool Expectation::compare(Expectation other)
	{
		if (prec != other.prec)
			return false;

		if (cls.compare(other.cls) != 0)
			return false;

		if (expecting.compare(other.expecting) != 0)
			return false;

		if (keep != other.keep)
			return false;

		if (minLength() != other.minLength())
			return false;

		if (children.size() != other.children.size())
			return false;

		for (unsigned int i = 0; i < children.size(); i++)
			if (children[i]->compare(*(other.children[i])) == false)
				return false;
		
		return true;
	}

	AST Expectation::parse(ParsingEnviron &env)
	{
		if (env.page->pullOffset().isGood() == false)
		{
			if (isDebugging())
				cout << prefix(env) << "overflowed token vector before it started, erroring...\n";
			env.error("unexpected end of token page");
			return AST();
		}

		env.curname = name;
		debugInput(env);
		ParsingEnviron::stackElement tmp;
		tmp.cls = cls;
		tmp.name = name;
		tmp.off = env.page->pullOffset();
		env.stack.push_back(tmp);
		if (env.stack.size() >= 100)
			env.dumpStack();
		AST rtn = handlerParse(*this, env);
		for (vector<ASTError>::iterator i = env.errors.begin(); i != env.errors.end(); i++)
			rtn.error(*i);
		env.errors.clear();
		debugOutput(rtn, env);
		env.stack.pop_back();
		return rtn;
	}

	Expectation Expectation::expect(string e)
	{
		expecting = e;
		return *this;
	}

	Expectation Expectation::enableKeep()
	{
		keep = true;
		return *this;
	}

	unsigned int Expectation::getMemorySize()
	{
		unsigned int rtn = sizeof(int)*2+(cls.size()+name.size()+expecting.size())*sizeof(char)+sizeof(bool)*2+sizeof(void *)*2;
		return rtn;
	}

	bool Expectation::isUninitialized()
	{
		return (prec == 0 && (cls.empty() && (name.empty() && (expecting.empty() && (keep == false && (sizeOverride == -1 && (appendRanked == false && (children.empty() && (handlerMinLength == NULL && (handlerParseManyUntil == NULL))))))))));
	}

	unsigned int handlerMinLengthOne(Expectation &e)
	{
		if (e.expecting.empty())
			return 0;
		else
			return 1;
	}

	AST handlerParseOne(Expectation &e, ParsingEnviron &env)
	{
		env.curexpecting = e.expecting;

		if (env.page->pullOffset()->getType().compare(e.expecting) == 0)
		{
			if (e.keep)
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met, keeping...\n";
				Token tmp = *(env.page->pullOffset());
				env.page->pushOffset(env.page->pullOffset().getLocation()+1);
				return AST(e.expecting, tmp);
			}
			else
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met.\n";
				env.page->pushOffset(env.page->pullOffset().getLocation()+1);	
				return AST();
			}
		}
		else
		{
			if (e.isDebugging())
				cout << e.prefix(env) << " NOT met, erroring... " << e.current(env) << "\n";
			env.errorUnexpected(e);
			return AST();
		}
	}

	unsigned int handlerMinLengthSequence(Expectation &e)
	{
		if (e.sizeOverride >= 0)
			return (unsigned int)e.sizeOverride;

		unsigned int rtn = 0;
		for (vector<Expectation *>::iterator i = e.children.begin(); i != e.children.end(); i++)
			if (*i != &e)
				rtn += (*i)->minLength();
		return rtn;
	}

	AST handlerParseSequence(Expectation &e, ParsingEnviron &env)
	{
		AST rtn = AST(e.name, *(env.page->pullOffset()));
		for (vector<Expectation * >::iterator i = e.children.begin(); i != e.children.end(); i++)
		{
			AST tmpast = (*i)->parse(env);
			if (tmpast.containsErrors() == true)
			{
				rtn = AST();
				rtn.error(tmpast.getErrors());
				env.error(e.name+" failed because of "+(*i)->name);
				if (e.isDebugging())
					cout << e.prefix(env) << " NOT met (because of '" << (*i)->name << "'), erroring..." << e.current(env) << "\n";
				return rtn;
			}
			else
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met.\n";
				if (e.prec > (*i)->prec)
				{
					if (e.isDebugging())
						cout << e.prefix(env) << " has higher precedence element, appending above root...\n";
					tmpast.addAtBeginning(rtn);
					rtn = tmpast;
				}
				else
				{
					rtn.add(tmpast);
				}
			}
		}

		return rtn;
	}

	unsigned int handlerMinLengthParallel(Expectation &e)
	{
		if (e.sizeOverride >= 0)
			return (unsigned int)e.sizeOverride;

		unsigned int lowest = (unsigned int)-1;
		for (vector<Expectation *>::iterator i = e.children.begin(); i != e.children.end(); i++)
		{
			if (*i != &e)
			{
				if ((*i)->minLength() < lowest)
				{
					lowest = (*i)->minLength();
				}
			}
		}
		if (e.children.empty())
			lowest = 0;
		return lowest;	
	}

	AST handlerParseParallel(Expectation &e, ParsingEnviron &env)
	{
		for (unsigned int i = env.stack.size()-2; i > 0; i--)
		{
			if (env.stack[i].name.compare(e.name) == 0 && env.stack[i].off == env.page->pullOffset())
			{
				if (e.isDebugging())
				{
					cout << e.prefix(env) << " falling into infinite mutual recursion, halting...\n";
					env.dumpStack();
				}
				env.error("infinite mutual recursion (if you see this, there's probably an issue with the compiler)");
				return AST();
			}
		}

		unsigned int lastParallel = 0;
		for (unsigned int i = env.stack.size()-2; i > 0; i--)
		{
			if (env.stack[i].cls.compare(e.cls) == 0)
			{
				lastParallel = i;
				break;
			}
		}

		ParsingEnviron tmpenv = env;

		for (vector<Expectation *>::iterator i = e.children.begin(); i != e.children.end(); i++)
		{
			ParsingEnviron tmptmpenv = tmpenv;

			if ((*i)->name.compare(env.stack[lastParallel].name) == 0)
				continue;

			if (*i == &e)
			{
				cout << "[Mitten Grammar] Error: parallel expectations cannot expect themselves directly, as it creates an unstoppable recursion\n";
				throw runtime_error("Parallel expectation expecting itself");
			}

			AST tmpast = (*i)->parse(tmptmpenv);
			if (tmpast.containsErrors() == false)
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met.\n";
				tmpenv.page->pushOffset(tmptmpenv.page->pullOffset().getLocation());
				return tmpast;
			}
			else
			{
				tmpenv.error(tmptmpenv.errors);
			}
		}

		env.page->pushOffset(tmpenv.page->pullOffset().getLocation());

		if (e.isDebugging())
			cout << e.prefix(env) << " NOT met, erroring... " << e.current(env) << "\n";
		env.error(tmpenv.errors);
		env.error(e.name+" failed");
		return AST();
	}

	unsigned int handlerMinLengthMany(Expectation &e)
	{
		if (e.children.empty())
			return 0;
		else if (e.children[0] != &e)
			return e.children[0]->minLength();
		else
			return 0;
	}

	AST handlerParseMany(Expectation &e, ParsingEnviron &env)
	{
		Token tmp = **env.page;
		AST rtn = AST(e.name, tmp);
		while(env.page->pullOffset() < env.page->size())
		{
			ParsingEnviron tmpenv = env;
			AST tmpast = e.children[0]->parse(tmpenv);
			if (tmpast.containsErrors() == false)
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met, continueing...\n";
				rtn.add(tmpast);
				env = tmpenv;
			}
			else
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " NOT met, halting...\n";
				if (rtn.getChildrenSize() == 0)
				{
					rtn.error(tmpast.getErrors());
					env.error(e.name+" failed because of "+e.children[0]->name);
				}
				break;
			}
		}

		return rtn;
	}

	unsigned int handlerMinLengthManyUntil(Expectation &e)
	{
		if (e.children.empty())
			return 0;
		else if (e.children[0] != &e)
			return e.children[0]->minLength();
		else
			return 0;
	}

	AST handlerParseManyUntil(Expectation &e, ParsingEnviron &env)
	{
		AST rtn = AST(e.name, *(env.page->pullOffset()));
		while(1)
		{
			ParsingEnviron tmpenv = env;
			AST tmp = e.children[1]->parse(tmpenv);
			if (tmp.containsErrors() == false)
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met until condition, halting...\n";
				break;
			}

			tmpenv = env;
			tmp = e.children[0]->parse(tmpenv);
			if (tmp.containsErrors() == false)
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " met, continueing...\n";
				rtn.add(tmp);
				env = tmpenv;
			}
			else
			{
				if (e.isDebugging())
					cout << e.prefix(env) << " NOT met, halting...\n";
				if (rtn.getChildrenSize() == 0)
				{
					rtn.error(tmp.getErrors());
					env.error(e.name+" failed because of "+e.children[0]->name);
				}
				break;
			}
		}

		return rtn;
	}

	void Builder::createParser(string c, Expectation::callbackMinLength cml, Expectation::callbackParse cp)
	{
		content[c].cml = cml;
		content[c].cp = cp;
		content[c].ar = false;
	}

	void Builder::enableRankedAppending(string c)
	{
		content[c].ar = true;
	}

	void Builder::createDefaultParsers()
	{
		createParser("One", handlerMinLengthOne, handlerParseOne);
		createParser("Sequence", handlerMinLengthSequence, handlerParseSequence);
		createParser("Parallel", handlerMinLengthParallel, handlerParseParallel);
		enableRankedAppending("Parallel");
		createParser("Many", handlerMinLengthMany, handlerParseMany);
		createParser("ManyUntil", handlerMinLengthManyUntil, handlerParseManyUntil);
	}

	Expectation Builder::operator () (string s)
	{
		Expectation rtn = Expectation();
		rtn.cls = s;
		rtn.handlerMinLength = content[s].cml;
		rtn.handlerParse = content[s].cp;
		rtn.appendRanked = content[s].ar;
		return rtn;
	}

	Expectation Builder::operator () (string s, string n)
	{
		Expectation rtn = this->operator () (s);
		rtn.name = n;
		rtn.expecting = n;
		return rtn;
	}

	unsigned int Grammar::diff(string a, string b)
	{
		unsigned int l = a.size();
		if (b.size() < l)
			l = b.size();

		unsigned int rtn = 0;
		for (unsigned int i = 0; i < l; i++)
			if (a[i] != b[i])
				rtn++;
		return rtn+abs((int)(a.size()-b.size()));
	}

	Grammar &Grammar::createFromTokenizer(Builder &b, string as, Tokenizer &t)
	{
		vector<pair<string, Pattern> > cats = t.getCategorizers();

		for (vector<pair<string, Pattern> >::iterator i = cats.begin(); i != cats.end(); i++)
			this->operator << (b(as, i->first));
		
		return *this;
	}

	Grammar &Grammar::operator << (Expectation e)
	{
		for (map<string, Expectation>::iterator i = content.begin(); i != content.end(); i++)
		{
			if (i->second.compare(e) == true && i->first.compare(e.name) != 0)
			{
				cout << "[Mitten Grammar] Warning: expectation '" << e.name << "' may not be context-free alongside '" << i->first << "'\n";
			}
		}

		content[e.name] = e;
		return *this;
	}

	Expectation &Grammar::operator [] (string s)
	{
		if (content.find(s) == content.end())
		{
			cerr << "[Mitten Grammar] Errror: o such expectation: '" << s << "'\n";

			string close;
			unsigned int d = (unsigned int)-1;

			for (map<string, Expectation>::iterator i = content.begin(); i != content.end(); i++)
			{
				if (diff(s, i->first) < d)
				{
					d = diff(s, i->first);
					close = i->first;
				}
			}

			if (close.empty() == false)
				cout << "\tclosest match: '" << close << "'\n";

			throw runtime_error("No such expectation '"+s+"'");
		}
		return content[s];
	}

	void Grammar::enableDebuggingFiltered(string filter)
	{
		for (map<string, Expectation>::iterator i = content.begin(); i != content.end(); i++)
			if (i->first.find(filter) != string::npos)
				i->second.enableDebuggingNonRecursive();
	}

	unsigned int Grammar::getMemorySize()
	{
		unsigned int rtn = 0;
		for (map<string, Expectation>::iterator i = content.begin(); i != content.end(); i++)
			rtn += i->first.size()*sizeof(char)+i->second.getMemorySize();
		return rtn;
	}
}
