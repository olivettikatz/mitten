SYSTEM=$(shell uname -s)

ifeq ($(SYSTEM),Darwin)
CXX=c++
CXXFLAGS=-std=c++11 -stdlib=libc++ -ggdb -O0 -I/usr/include/mitten
CXXSHAREDFLAGS=-dynamiclib
LDFLAGS=-L/usr/include/mitten
else
CXX=g++
CXXFLAGS=-std=c++11 -ggdb -O0 -I/usr/include/mitten
LDFLAGS=-L/usr/include/mitten
endif

RM=rm
RMFLAGS=-rf

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@