CXX=c++
CXXFLAGS=-std=c++11 -stdlib=libc++ -g -O0

RM=rm
RMFLAGS=-rf

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

PARSING_OBJ=parsing/lexical/pattern.o parsing/lexical/tokenizer.o parsing/semantic/ast.o parsing/semantic/asterror.o parsing/semantic/scope.o
MC_OBJ=mc/parsing.o
TEST_OBJ=mc/test_parsing.o
TEST_BIN=mc/test_parsing.bin

%.bin : %.o
	$(CXX) $(CXXFLAGS) $(PARSING_OBJ) $(MC_OBJ) $< -o $@

build : $(PARSING_OBJ) $(MC_OBJ)

test : $(TEST_OBJ) $(TEST_BIN)

clean :
	$(RM) $(RMFLAGS) $(PARSING_OBJ) $(MC_OBJ) $(TEST_OBJ) $(TEST_BIN) $(shell for i in $(TEST_BIN); do echo $$i.dSYM; done)