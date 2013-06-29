#ifndef __PARSING_ERROR_H
#define __PARSING_ERROR_H

#include "../lexical/tokenizer.h"
#include <sstream>
#include <memory>

namespace parsing
{
	using namespace std;

	class ASTError
	{
	private:
		Token src;
		string msg;

	public:
		ASTError() {}
		ASTError(Token s, string m) : src(s), msg(m) {}
		string format(string fmt);
		string display(string fmt);
		string display();
	};
}

#endif