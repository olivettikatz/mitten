include config.mk

MC_OBJ=mc/parsing.o
TEST_OBJ=mc/test_parsing.o
TEST_BIN=mc/test_parsing.bin

build : $(MC_OBJ)

test : $(TEST_OBJ) $(TEST_BIN)

mc/test_parsing.bin : mc/test_parsing.o $(MC_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ -Lmitten -ltrace -lparsing

clean :
	$(RM) $(RMFLAGS) $(MC_OBJ) $(TEST_OBJ) $(TEST_BIN) $(shell for i in $(TEST_BIN); do echo $$i.dSYM; done)