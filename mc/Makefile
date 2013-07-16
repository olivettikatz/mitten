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

build : parsing.o

test : test_parsing.bin

test_parsing.bin : test_parsing.o parsing.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) -ltrace -lparsing

clean :
	$(RM) $(RMFLAGS) parsing.o test_parsing.o test_parsing.bin test_parsing.bin.dSYM doc