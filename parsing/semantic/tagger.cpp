#include "tagger.h"

namespace parsing
{
	Tagger::parsingElement &Tagger::parsingElement::operator << (Tagger::parsingElementExpectation e)
	{
		expecting.push_back(e);
		return *this;
	}

	void Tagger::enableDebugging()
	{
		debug = true;
	}

	Tagger::parsingElementExpectation Tagger::expectToken(string type)
	{
		parsingElementExpectation rtn;
		rtn.first = expectationToken;
		rtn.second = type;
		return rtn;
	}

	Tagger::parsingElementExpectation Tagger::expectScope(string name)
	{
		parsingElementExpectation rtn;
		rtn.first = expectationScope;
		rtn.second = name;
		return rtn;
	}

	Tagger::parsingElementExpectation Tagger::expectNull()
	{
		parsingElementExpectation rtn;
		rtn.first = expectationToken;
		rtn.second = "";
		return rtn;
	}

	Tagger::parsingElement Tagger::createParsingElement(string n)
	{
		parsingElement rtn;
		rtn.name = n;
		return rtn;
	}

	void Tagger::addParsingElement(Tagger::parsingElement e)
	{
		if (content.empty())
		{
			content.push_back(e);
		}
		else
		{
			for (vector<parsingElement>::iterator i = content.begin(); i != content.end(); i++)
			{
				if (i->expecting.size() < e.expecting.size())
				{
					content.insert(i, e);
					break;
				}
			}
		}
	}

	AST Tagger::parse(AST ast, unsigned int l)
	{
		for (vector<parsingElement>::iterator i = content.begin(); i != content.end(); i++)
		{
			bool matching = true;

			if (ast.size() != i->expecting.size())
				matching = false;


			for (unsigned int j = 0; j < ast.size(); j++)
			{
				if (i->expecting[j].first == expectationToken)
				{
					if (i->expecting[j].second.compare(ast[j].getContent().getType()) != 0)
					{
						matching = false;
						break;
					}
				}
				else
				{
					if (ast[j].getName().empty())
					{
						ast[j] = parse(ast[j], l+1);
					}

					if (i->expecting[j].second.compare(ast[j].getName()) != 0)
					{
						matching = false;
						break;
					}
				}
			}

			if (matching == true)
			{
				ast.setName(i->name);
				break;
			}
		}

		return ast;
	}

	AST Tagger::parse(AST ast)
	{
		return parse(ast, 0);
	}
}