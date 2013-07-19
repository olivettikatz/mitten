SYSTEM=$(shell uname -s)

ifeq ($(SYSTEM),Darwin)
CXX=c++
CXXFLAGS=-std=c++11 -stdlib=libc++ -ggdb -O0 -I/usr/include/mitten -I.. -D_MITTEN_PLATFORM_DARWIN
CXXSHAREDFLAGS=-dynamiclib
LDFLAGS=-L/usr/lib/mitten -L../trace -L../parsing -L../runcfg
RM=rm
RMFLAGS=-rf
CP=cp
CPFLAGS=-f
MKDIR=mkdir
MKDIRFLAGS=-p
LN=ln
LNFLAGS=-sf
else
CXX=g++
CXXFLAGS=-std=c++11 -ggdb -O0 -I/usr/include/mitten -I.. -fPIC -D_MITTEN_PLATFORM_OTHER
CXXSHAREDFLAGS=-shared
LDFLAGS=-L/usr/lib/mitten -L../trace -L../parsing -L../runcfg
RM=rm
RMFLAGS=-rf
CP=cp
CPFLAGS=-f
MKDIR=mkdir
MKDIRFLAGS=-p
LN=ln
LNFLAGS=-sf
endif

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
