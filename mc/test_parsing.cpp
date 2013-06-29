#include <unistd.h>
#include "parsing.h"

using namespace mc;

int main()
{
	cout << "Creating tokenizer...\n";
	Tokenizer t = createTokenizer();
	cout << "\tdone.\n";

	t.enableDebugging();
	
	vector<Token> expr_toks = t.tokenize("int main(@vector(string) args)\n{\tprint(\"hello, world\\n\");\n}\n", "--");

	return 0;
}
