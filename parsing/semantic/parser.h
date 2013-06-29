#ifndef __MITTEN_PARSING_PARSER_H
#define __MITTEN_PARSING_PARSER_H

#include <stdexcept>
#include <map>
#include "../lexical/tokenizer.h"
#include "ast.h"

namespace parsing
{
	using namespace std;

	class Expectation;

	class ParsingEnviron
	{
	public:
		typedef struct 
		{
			string cls;
			string name;
			TokenPage::iterator off;
		} stackElement;

		ParsingEnviron() {}
		ParsingEnviron(TokenPage &p) : page(&p) {}
		TokenPage *page;
		vector<stackElement> stack;
		string curname;
		string curexpecting;
		TokenPage::iterator alreadyDisplayed;
		vector<ASTError> errors;

		void error(string msg);
		void error(vector<ASTError> e);
		void errorUnexpected(Expectation &e);
		void dump();
		void dumpStack();
		string dumpErrors();
		string pad();
	};

	class Expectation
	{
	private:
		bool debug;

		void debugInput(ParsingEnviron &env);
		void debugOutput(AST rtn, ParsingEnviron &env);

	public:
		typedef AST (*callbackParse)(Expectation &, ParsingEnviron &);
		typedef unsigned int (*callbackMinLength)(Expectation &);

		unsigned int prec;
		string cls;
		string name;
		string expecting;
		bool keep;
		int sizeOverride;
		bool appendRanked;
		vector<Expectation *> children;
		callbackMinLength handlerMinLength;
		callbackParse handlerParse;

		Expectation() : debug(false), prec(0), keep(false), sizeOverride(-1), appendRanked(false), handlerMinLength(NULL), handlerParse(NULL) {}
		void enableDebuggingNonRecursive();
		void enableDebugging();
		bool isDebugging();
		string prefix(ParsingEnviron &env);
		string current(ParsingEnviron &env);
		void assumeSize(unsigned int s);
		void append(Expectation &e);
		bool compare(Expectation other);
		unsigned int minLength();
		AST parse(ParsingEnviron &env);

		Expectation expect(string e);
		Expectation enableKeep();

		unsigned int getMemorySize();
		bool isUninitialized();
	};

	unsigned int handlerMinLengthOne(Expectation &e);
	AST handlerParseOne(Expectation &e, ParsingEnviron &env);
	unsigned int handlerMinLengthSequence(Expectation &e);
	AST handlerParseSequence(Expectation &e, ParsingEnviron &env);
	unsigned int handlerMinLengthParallel(Expectation &e);
	AST handlerParseParallel(Expectation &e, ParsingEnviron &env);
	unsigned int handlerMinLengthMany(Expectation &e);
	AST handlerParseMany(Expectation &e, ParsingEnviron &env);
	unsigned int handlerMinLengthManyUntil(Expectation &e);
	AST handlerParseManyUntil(Expectation &e, ParsingEnviron &env);

	class Builder
	{
	public:
		typedef struct
		{
			Expectation::callbackMinLength cml;
			Expectation::callbackParse cp;
			bool ar;
		} expectationConfig;

	private:
		map<string, expectationConfig> content;

	public:
		Builder() {}
		void createParser(string c, Expectation::callbackMinLength cml, Expectation::callbackParse cp);
		void enableRankedAppending(string c);
		void createDefaultParsers();
		Expectation operator () (string s);
		Expectation operator () (string s, string n);
	};

	class Grammar
	{
	private:
		map<string, Expectation> content;
		unsigned int diff(string a, string b);

	public:
		Grammar() {}
		Grammar &createFromTokenizer(Builder &b, string as, Tokenizer &t);
		Grammar &operator << (Expectation e);
		Expectation &operator [] (string s);
		void enableDebuggingFiltered(string filter);
		unsigned int getMemorySize();
	};
}

#endif