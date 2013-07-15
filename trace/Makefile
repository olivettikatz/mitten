include config.mk

ifeq ($(SYSTEM),Darwin)
build : libtrace.dylib
else
build : libtrace.so
endif

libtrace.dylib : pattern.o tokenizer.o trace.o
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@

libtrace.so : pattern.o tokenizer.o trace.o
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@

clean :
	$(RM) $(RMFLAGS) pattern.o tokenizer.o trace.o libtrace.dylib libtrace.so