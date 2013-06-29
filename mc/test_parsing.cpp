#include <unistd.h>
#include "parsing.h"

using namespace mc;

int main()
{
	cout << "Creating tokenizer...\n";
	Tokenizer t = createTokenizer();
	cout << "\tdone.\nCreating parser...\n";
	Grammar g = createParser(t);
	cout << "\tdone.\nCreating parsing environment...\n";
	ParsingEnviron env;
	cout << "\tdone.\n";

	g.enableDebuggingFiltered("Expression");
	g["Page"].enableDebugging();
	//t.enableDebugging();

	cout << "Memory size of tokenizer: " << t.getMemorySize()/1000.0 << "Kb\n";
	cout << "Memory size of grammar: " << g.getMemorySize()/1000.0 << "Kb\n";

	/*
	vector<Token> iasm_toks = t.tokenize("asm\n{\n\tmovi eax 2;\n}\n");
	for (vector<Token>::iterator i = iasm_toks.begin(); i != iasm_toks.end(); i++)
		cout << "'" << i->get() << "'\n";

	AST iasm_ast = p.parse(iasm_toks);
	displayErrors(p.getErrors());
	cout << "result: " << iasm_ast.display() << "\n";*/

	TokenPage expr_toks = t.tokenize("int main(@vector(string) args)\n{\tprint(\"hello, world\\n\");\n}\n", "--");

	env = ParsingEnviron(expr_toks);
	AST expr_ast = g["Page"].parse(env);
	cout << expr_ast.dumpErrors() << "\n";
	cout << "result: " << expr_ast.display() << "\n";

	return 0;
}
