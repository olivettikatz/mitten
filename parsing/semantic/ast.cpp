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

	AST &AST::operator [] (unsigned int idx)
	{
		return children[idx];
	}

	string AST::display(unsigned int l)
	{
		stringstream ss;
		ss << string(l*2, ' ') << "(" << name << ":" << content.get();
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			ss << "\n" << string(l*2, ' ') << i->display(l+1);
		ss << ")";
		return ss.str();
	}

	string AST::display()
	{
		return display(0);
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
}

