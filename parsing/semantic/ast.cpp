#include "ast.h"

namespace parsing
{
	void AST::error(ASTError e)
	{
		errors.push_back(e);
	}

	void AST::error(vector<ASTError> e)
	{
		errors.insert(errors.end(), e.begin(), e.end());
	}

	bool AST::containsErrors()
	{
		return !errors.empty();
	}

	vector<ASTError> AST::getErrors()
	{
		return errors;
	}

	bool AST::empty()
	{
		return (content.empty() && name.empty()) && (children.empty() && errors.empty());
	}

	AST &AST::add(AST a)
	{
		if (a.empty() == false)
			children.push_back(a);
		return *this;
	}

	AST &AST::add(Token t)
	{
		return add(AST(t));
	}

	AST &AST::add(string n)
	{
		return add(AST(n));
	}

	AST &AST::addLowerPrecedence(AST a)
	{
		if (a.empty() == false)
		{
			AST tmp = a;
			tmp.children.insert(tmp.children.begin(), children.back());
			children.pop_back();
			children.push_back(tmp);
		}
		return *this;
	}

	AST &AST::operator << (AST a)
	{
		return add(a);
	}

	AST &AST::operator << (Token t)
	{
		return add(t);
	}

	AST &AST::operator << (string n)
	{
		return add(n);
	}

	string AST::getName()
	{
		return name;
	}

	string AST::setName(string n)
	{
		return (name = n);
	}

	Token AST::getContent()
	{
		return content;
	}

	Token AST::setContent(Token t)
	{
		return (content = t);
	}

	unsigned int AST::size()
	{
		return children.size();
	}

	unsigned int AST::sizeRecursive()
	{
		unsigned int n = 1;
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			n += i->sizeRecursive();
		return n;
	}

	AST &AST::operator [] (unsigned int idx)
	{
		return children[idx];
	}

	string AST::display(unsigned int l)
	{
		string c;
		if (status == statusRaw)
			c = "";
		else if (status == statusScope)
			c = "\033[0;34m";
		else if (status == statusRDP)
			c = "\033[0;32m";
		else
			c = "\033[0;31m";
		stringstream ss;
		ss << string(l*2, ' ') << c << "(\033[0;0m" << name << c << ":\033[0;0m" << content.get();
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			ss << "\n" << string(l*2, ' ') << i->display(l+1);
		ss << c << ")\033[0;0m";
		return ss.str();
	}

	string AST::display()
	{
		return display(0);
	}

	string AST::displaySome(unsigned int l, unsigned int limit)
	{
		string c;
		if (status == statusRaw)
			c = "";
		else if (status == statusScope)
			c = "\033[0;34m";
		else if (status == statusRDP)
			c = "\033[0;32m";
		else
			c = "\033[0;31m";
		stringstream ss;
		ss << string(l*2, ' ') << c << "(\033[0;0m" << name << c << ":\033[0;0m" << content.get();
		if (limit == 0)
		{
			ss << " ...";
		}
		else
		{
			for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			{
				ss << "\n" << string(l*2, ' ') << i->displaySome(l+1, limit-1);
			}
		}
		ss << c << ")\033[0;0m";
		return ss.str();
	}

	string AST::displaySome(unsigned int limit)
	{
		return displaySome(0, limit);
	}	

	string AST::dumpErrors()
	{
		stringstream ss;
		
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			ss << i->dumpErrors();

		for (vector<ASTError>::iterator i = errors.begin(); i != errors.end(); i++)
			ss << i->display();

		return ss.str();
	}

	AST AST::flatten(vector<AST> &rtn)
	{
		rtn.push_back(AST(name, content));
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			i->flatten();
		return AST();
	}

	AST AST::flatten()
	{
		vector<AST> tmp;
		flatten(tmp);
		AST rtn = AST();
		rtn.children = tmp;
		return rtn;
	}

	bool AST::containsNamedBranch()
	{
		if (name.empty() == false)
			return true;

		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			if (i->containsNamedBranch())
				return true;

		return false;
	}

	bool AST::containsStatusBranch(int s)
	{
		if (status >= s)
			return true;

		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			if (i->containsStatusBranch(s))
				return true;

		return false;
	}

	int AST::setStatus(int s)
	{
		return (status = s);
	}

	int AST::setStatusRecursive(int s)
	{
		status = s;
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			i->setStatusRecursive(s);
		return s;
	}

	int AST::getStatus()
	{
		return status;
	}

	bool ASTE::empty()
	{
		return name.empty() && (argument.empty() && children.empty());
	}

	ASTE &ASTE::add(ASTE a)
	{
		children.push_back(a);
		return *this;
	}

	ASTE &ASTE::add(string n, string t)
	{
		children.push_back(ASTE(n, t));
		return *this;
	}

	ASTE &ASTE::add(string n)
	{
		children.push_back(ASTE(n));
		return *this;
	}

	ASTE &ASTE::operator << (ASTE a)
	{
		return add(a);
	}

	ASTE &ASTE::operator << (string n)
	{
		return add(n);
	}

	string ASTE::getName()
	{
		return name;
	}

	string ASTE::setName(string n)
	{
		return (name = n);
	}

	ASTE::expectationType ASTE::getExpectationType()
	{
		return type;
	}

	ASTE::expectationType ASTE::setExpectationType(ASTE::expectationType t)
	{
		return (type = t);
	}

	string ASTE::getArgument()
	{
		return argument;
	}

	string ASTE::setArgument(string a)
	{
		return (argument = a);
	}

	unsigned int ASTE::size()
	{
		return children.size();
	}

	ASTE &ASTE::operator [] (unsigned int l)
	{
		return children[l];
	}

	string ASTE::display(unsigned int l)
	{
		stringstream ss;
		ss << string(l*2, ' ') << "(" << name << " <= ";
		if (type == _name)
			ss << "name";
		else if (type == _type)
			ss << "type";
		else if (type == _scope)
			ss << "scope";
		ss << ":" << argument;
		for (vector<ASTE>::iterator i = children.begin(); i != children.end(); i++)
			ss << "\n" << string(l*2, ' ') << i->display(l+1);
		ss << ")";
		return ss.str();
	}

	string ASTE::display()
	{
		return display(0);
	}
}
