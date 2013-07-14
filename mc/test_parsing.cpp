#include <unistd.h>
#include "parsing.h"

using namespace mc;

int main(int argc, char *argv[])
{
	TRACE_INIT(argc, argv);
	TRACE_MODULE_MAP("parsing/semantic/scope.cpp", "Scope");

	cout << "Creating tokenizer...\n";
	Tokenizer t = createTokenizer();
	cout << "\tdone.\n";

	cout << "Creating scope parser...\n";
	ScopeParser s = createScopeParser();
	cout << "\tdone.\n";

	cout << "Creating recursive descent parser...\n";
	RDP r = createRDP();
	cout << "\tdone.\n";

	cout << "Tokenizing test 'expr'...\n";	
	vector<Token> exprToks = t.tokenize("int main(@vector(string) args)\n{\tprint(\"hello, world\\n\");\n}\n", "--");
	
	cout << "Scope parsing test 'expr'...\n";
	AST exprAst = s.parse(exprToks);
	
	cout << "Recursive-descent parsing test 'expr'...\n";
	exprAst = r.parse(exprAst);
	
	cout << "Parsing done.\n";
	if (exprAst.containsErrors())
		cout << exprAst.dumpErrors() << "\n";

	cout << "Tokenizing test 'arith'...\n";
	vector<Token> arithToks = t.tokenize("2+3*1\n", "--");

	cout << "Scope parsing test 'arith'...\n";
	AST arithAst = s.parse(arithToks);

	TRACE_ENABLE_DEBUG();
	cout << "Recursive-descent parsing test 'arith'...\n";
	arithAst = r.parse(arithAst);
	TRACE_DISABLE_DEBUG();

	cout << "Parsing done.\n";
	if (arithAst.containsErrors())
		cout << arithAst.dumpErrors() << "\n";

	return 0;
}
