# This file is part of Mitten.
# 
# Mitten is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Mitten is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Mitten.  If not, see <http://www.gnu.org/licenses/>.
#
# Copyright 2013 Oliver Katz

include config.mk

ifeq ($(SYSTEM),Darwin)
LLVM_DIR=/opt/local/libexec/llvm-3.2/include
LLVM_LIB=-L/opt/local/libexec/llvm-3.2/lib -lLLVM-3.2svn
else
LLVM_DIR=$(shell llvm-config --includedir)
LLVM_LIB=$(shell llvm-config --ldflags --libs)
endif
LLVM_INC=-I$(LLVM_DIR)

CXXFLAGS+=-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS $(LLVM_INC)

OBJ=core/vm.o parsing.o

build : $(OBJ) main.o
	$(CXX) $(CXXFLAGS) $(OBJ) main.o -o mc $(LDFLAGS) -ltrace -lparsing -lruncfg $(LLVM_LIB) -ldl -lc++
ifeq ($(SYSTEM),Darwin)
	$(LN) $(LNFLAGS) ../trace/libtrace.dylib .
	$(LN) $(LNFLAGS) ../parsing/libparsing.dylib .
	$(LN) $(LNFLAGS) ../runcfg/libruncfg.dylib .
else
	$(LN) $(LNFLAGS) ../trace/libtrace.so .
	$(LN) $(LNFLAGS) ../parsing/libparsing.so .
	$(LN) $(LNFLAGS) ../runcfg/libruncfg.so .
endif

install : 
	$(CP) $(CPFLAGS) mc /usr/bin

uninstall : 
	$(RM) $(RMFLAGS) /usr/bin/mc

doc :
	doxygen Doxyfile

clean :
	$(RM) $(RMFLAGS) doc $(OBJ) main.o mc
