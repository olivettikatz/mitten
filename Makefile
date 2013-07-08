SYSTEM=$(shell uname -s)

ifeq ($(SYSTEM),"Darwin")
CXX=c++
CXXFLAGS=-std=c++11 -stdlib=libc++ -g -O0
else
CXX=g++
CXXFLAGS=-std=c++11 -g -O0
endif

RM=rm
RMFLAGS=-rf

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

TRACE_OBJ=trace/trace.o
PARSING_OBJ=parsing/lexical/pattern.o parsing/lexical/tokenizer.o parsing/semantic/ast.o parsing/semantic/asterror.o parsing/semantic/scope.o parsing/semantic/tagger.o parsing/semantic/rdp.o
MC_OBJ=mc/parsing.o
TEST_OBJ=mc/test_parsing.o
TEST_BIN=mc/test_parsing.bin

build : $(TRACE_OBJ) $(PARSING_OBJ) $(MC_OBJ)

test : $(TEST_OBJ) $(TEST_BIN)

mc/test_parsing.bin : mc/test_parsing.o $(TRACE_OBJ) $(PARSING_OBJ) $(MC_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean :
	$(RM) $(RMFLAGS) $(TRACE_OBJ) $(PARSING_OBJ) $(MC_OBJ) $(TEST_OBJ) $(TEST_BIN) $(shell for i in $(TEST_BIN); do echo $$i.dSYM; done)
