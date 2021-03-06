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

#include "ast.h"

namespace parsing
{
	/*ASTError AST::error(string msg)
	{
		return ASTError(content, msg);
		//errors.push_back(ASTError(content, msg));
	}*/

	/*void AST::error(ASTError e)
	{
		errors.push_back(e);
	}

	void AST::error(vector<ASTError> e)
	{
		errors.insert(errors.end(), e.begin(), e.end());
	}

	void AST::pullUpErrors()
	{
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
		{
			i->pullUpErrors();
			error(i->getErrors());
		}
	}

	bool AST::containsErrors()
	{
		return !errors.empty();
	}

	vector<ASTError> AST::getErrors()
	{
		return errors;
	}*/

	bool AST::empty()
	{
		return (content.empty() && name.empty()) && children.empty();
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

	AST &AST::addAtBeginning(AST a)
	{
		if (a.empty() == false)
		{
			children.insert(children.begin(), a);
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

	void AST::remove(unsigned int idx)
	{
		children.erase(children.begin()+idx);
	}

	void AST::removeAfter(unsigned int idx)
	{
		vector<AST> tmp(children.begin(), children.begin()+idx);
		children = tmp;
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

	string AST::getFile()
	{
		if (content.getFile().empty())
		{
			for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			{
				string tmp = i->getFile();
				if (tmp.empty() == false)
				{
					return tmp;
				}
			}

			throw runtime_error("fileless AST");
		}
		
		return content.getFile();
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

	int AST::findLeafContent(string c)
	{
		for (int i = 0; i < size(); i++)
		{
			if (children[i].size() == 0 && children[i].getContent().get().compare(c) == 0)
			{
				return i;
			}
		}

		return npos;
	}

	int AST::findLeafContent(string c, int after)
	{
		for (int i = after; i < size(); i++)
		{
			if (children[i].size() == 0 && children[i].getContent().get().compare(c) == 0)
			{
				return i;
			}
		}

		return npos;
	}

	int AST::findLeafType(string t)
	{
		for (int i = 0; i < size(); i++)
		{
			if (children[i].size() == 0 && children[i].getContent().getType().compare(t) == 0)
			{
				return i;
			}
		}

		return npos;
	}

	int AST::findLeafType(string t, int after)
	{
		for (int i = after; i < size(); i++)
		{
			if (children[i].size() == 0 && children[i].getContent().getType().compare(t) == 0)
			{
				return i;
			}
		}

		return npos;
	}

	AST AST::subtree(int start)
	{
		AST rtn = AST(name, content);
		for (int i = start; i < size(); i++)
			rtn.add(children[i]);
		return rtn;
	}

	AST AST::subtree(int start, int length)
	{
		AST rtn = AST(name, content);
		for (int i = start; i < start+length; i++)
			rtn.add(children[i]);
		return rtn;
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

	/*string AST::dumpErrors()
	{
		stringstream ss;
		
		for (vector<AST>::iterator i = children.begin(); i != children.end(); i++)
			ss << i->dumpErrors();

		for (vector<ASTError>::iterator i = errors.begin(); i != errors.end(); i++)
			ss << i->display();

		return ss.str();
	}*/

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

	void ASTE::replace(unsigned int i, ASTE e)
	{
		children.erase(children.begin()+i);
		children.insert(children.begin()+i, e.children.begin(), e.children.end());
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
		else if (type == _tag)
			ss << "tag";
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
