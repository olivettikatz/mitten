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
