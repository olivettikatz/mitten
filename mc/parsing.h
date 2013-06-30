#ifndef __MC_PARSING_H
#define __MC_PARSING_H

#include <iostream>
#include "../parsing/parsing.h"

namespace mc
{
	using namespace std;
	using namespace parsing;

	Tokenizer createTokenizer();
	ScopeParser createScopeParser();
}

#endif
