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

LLVM_DIR=$(shell llvm-config --includedir)
LLVM_INC=-I$(LLVM_DIR)

CXXFLAGS+=-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS $(LLVM_INC)

OBJ=core/vm.o parsing.o

build : $(OBJ)

doc :
	doxygen Doxyfile

clean :
	$(RM) $(RMFLAGS) doc $(OBJ)
