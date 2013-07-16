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
build : libparsing.dylib
else
build : libparsing.so
endif

OBJ=lexical/pattern.o lexical/tokenizer.o semantic/ast.o semantic/asterror.o semantic/scope.o semantic/rdp.o

libparsing.dylib : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@ -ltrace

libparsing.so : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@ -ltrace

install :
	mkdir -p /usr/include/mitten/parsing/lexical
	mkdir -p /usr/include/mitten/parsing/semantic
	mkdir -p /usr/lib/mitten
	cp lexical/pattern.h lexical/tokenizer.h /usr/include/mitten/parsing/lexical
	cp semantic/ast.h semantic/asterror.h semantic/scope.h semantic/rdp.h /usr/include/mitten/parsing/semantic
	[ -f libmittenparsing.dylib ] && cp libmittenparsing.dylib /usr/lib/mitten/libparsing.dylib
	[ -f libmittenparsing.so ] && cp libmittenparsing.dylib /usr/lib/mitten/libparsing.so

uninstall :
	rm -r /usr/include/mitten /usr/lib/mitten

doc :
	doxygen Doxyfile

clean :
	$(RM) $(RMFLAGS) $(OBJ) libparsing.dylib libparsing.so doc
