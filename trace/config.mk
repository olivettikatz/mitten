SYSTEM=$(shell uname -s)

ifeq ($(SYSTEM),Darwin)
CXX=c++
CXXFLAGS=-std=c++11 -stdlib=libc++ -ggdb -O0
CXXSHAREDFLAGS=-dynamiclib
else
CXX=g++
CXXFLAGS=-std=c++11 -ggdb -O0
endif

RM=rm
RMFLAGS=-rf

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@