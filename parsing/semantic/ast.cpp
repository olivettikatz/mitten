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

	bool AST::good()
	{
		return (eid.empty() == false);
	}

	bool AST::empty()
	{
		return (content.empty() && eid.empty()) && (children.empty() && errors.empty());
	}

	AST &AST::add(AST a)
	{
		if (a.empty() == false)
			children.push_back(a);
		return *this;
	}

	AST &AST::addAtBeginning(AST a)
	{
		children.insert(children.begin(), a);
		return *this;
	}

	string AST::getExpectationID()
	{
		return eid;
	}

	string AST::setExpectationID(string id)
	{
		return (eid = id);
	}

	Token AST::getContent()
	{
		return content;
	}

	unsigned int AST::getChildrenSize()
	{
		return children.size();
	}

	AST AST::getChild(unsigned int idx)
	{
		return children[idx];
	}

	AST AST::setChild(unsigned int idx, AST c)
	{
		return (children[idx] = c);
	}

	string AST::display(unsigned int l)
	{
		stringstream ss;
		ss << string(l*2, ' ') << "(" << eid << ":" << content.get();
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

