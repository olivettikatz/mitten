#ifndef __MITTEN_PARSING_TAGGER_H
#define __MITTEN_PARSING_TAGGER_H

#include "scope.h"
#include <stdarg.h>

namespace parsing
{
	using namespace std;

	class Tagger
	{
	public:
		const bool expectationToken = false;
		const bool expectationScope = true;
		typedef pair<bool, string> parsingElementExpectation;

		typedef struct parsingElement
		{
			string name;
			vector<parsingElementExpectation> expecting;
			parsingElement &operator << (parsingElementExpectation e);
		} parsingElement;

	private:
		vector<parsingElement> content;
		bool debug;

	public:
		Tagger() : debug(false) {}
		void enableDebugging();
		parsingElementExpectation expectToken(string type);
		parsingElementExpectation expectScope(string name);
		parsingElementExpectation expectNull();
		parsingElement createParsingElement(string n);
		void addParsingElement(parsingElement e);
		AST parse(AST ast, unsigned int l);
		AST parse(AST ast);
	};
}

#endif