include config.mk

PARSING_OBJ=parsing/lexical/pattern.o parsing/lexical/tokenizer.o parsing/semantic/ast.o parsing/semantic/asterror.o parsing/semantic/scope.o parsing/semantic/tagger.o parsing/semantic/rdp.o
MC_OBJ=mc/parsing.o
TEST_OBJ=mc/test_parsing.o
TEST_BIN=mc/test_parsing.bin

build : $(PARSING_OBJ) $(MC_OBJ)

test : $(TEST_OBJ) $(TEST_BIN)

mc/test_parsing.bin : mc/test_parsing.o $(PARSING_OBJ) $(MC_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean :
	$(RM) $(RMFLAGS) $(PARSING_OBJ) $(MC_OBJ) $(TEST_OBJ) $(TEST_BIN) $(shell for i in $(TEST_BIN); do echo $$i.dSYM; done)