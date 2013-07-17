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

CXXFLAGS+=-I../..

ifeq ($(SYSTEM),Darwin)
build : libparsing.dylib
else
build : libparsing.so
endif

OBJ=lexical/pattern.o lexical/tokenizer.o semantic/ast.o semantic/asterror.o semantic/scope.o semantic/rdp.o

libparsing.dylib : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@ $(LDFLAGS) -ltrace

libparsing.so : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) -Wl,-soname,libparsing.so$^ -o $@ $(LDFLAGS) -ltrace

install :
	$(MKDIR) $(MKDIRFLAGS) /usr/include/mitten/parsing/lexical
	$(MKDIR) $(MKDIRFLAGS) /usr/include/mitten/parsing/semantic
	$(CP) $(CPFLAGS) lexical/pattern.h lexical/tokenizer.h /usr/include/mitten/parsing/lexical
	$(CP) $(CPFLAGS) semantic/ast.h semantic/asterror.h semantic/scope.h semantic/rdp.h /usr/include/mitten/parsing/semantic
ifeq ($(SYSTEM),Darwin)
	$(CP) $(CPFLAGS) libparsing.dylib /usr/lib/libparsing.dylib
else
	$(CP) $(CPFLAGS) libparsing.so /usr/lib/libparsing.so
endif

uninstall :
ifeq ($(SYSTEM),Darwin)
	$(RM) $(RMFLAGS) /usr/include/mitten/parsing /usr/lib/libparsing.dylib
else
	$(RM) $(RMFLAGS) /usr/include/mitten/parsing /usr/lib/libparsing.so
endif

doc :
	doxygen Doxyfile

clean :
	$(RM) $(RMFLAGS) $(OBJ) libparsing.dylib libparsing.so doc
