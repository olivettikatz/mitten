#ifndef __PARSING_ERROR_H
#define __PARSING_ERROR_H

#include "../lexical/tokenizer.h"
#include <sstream>
#include <memory>

namespace parsing
{
	using namespace std;

	class Expectation;

	class ASTError
	{
	private:
		Token src;
		Expectation *exp;
		string msg;

	public:
		ASTError() : exp(NULL) {}
		ASTError(Token s, string m) : src(s), exp(NULL), msg(m) {}
		ASTError(Token s, Expectation &e, string m);
		string format(string fmt);
		string display(string fmt);
		string display();
	};
}

#endif