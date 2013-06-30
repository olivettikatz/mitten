#include <unistd.h>
#include "parsing.h"

using namespace mc;

int main()
{
	cout << "Creating tokenizer...\n";
	Tokenizer t = createTokenizer();
	cout << "\tdone.\n";

	cout << "Creating scope parser...\n";
	ScopeParser s = createScopeParser();
	cout << "\tdone.\n";

	//t.enableDebugging();
	//s.enableDebugging();

	cout << "Tokenizing test 'expr'...\n";	
	vector<Token> exprToks = t.tokenize("int main(@vector(string) args)\n{\tprint(\"hello, world\\n\");\n}\n", "--");
	
	cout << "Scope parsing test 'expr'...\n";
	AST exprAst = s.parse(exprToks);

	cout << "Parsing done.\n";

	return 0;
}
