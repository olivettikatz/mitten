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
build : libruncfg.dylib
else
build : libruncfg.so
endif

OBJ=runcfg.o

libruncfg.dylib : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) $^ -o $@ $(LDFLAGS)

libruncfg.so : $(OBJ)
	$(CXX) $(CXXFLAGS) $(CXXSHAREDFLAGS) -Wl,-soname,libruncfg.so $^ -o $@ $(LDFLAGS)

install : 
	$(MKDIR) $(MKDIRFLAGS) /usr/include/mitten/runcfg
	$(MKDIR) $(MKDIRFLAGS) /usr/lib/mitten
	$(CP) $(CPFLAGS) runcfg.h /usr/include/mitten/runcfg
ifeq ($(SYSTEM),Darwin)
	$(CP) $(CPFLAGS) libruncfg.dylib /usr/lib/mitten
else
	$(CP) $(CPFLAGS) libruncfg.so /usr/lib/mitten
endif

uninstall :
ifeq ($(SYSTEM),Darwin)
	$(RM) $(RMFLAGS) /usr/include/mitten/runcfg /usr/lib/mitten/libruncfg.dylib
else
	$(RM) $(RMFLAGS) /usr/include/mitten/runcfg /usr/lib/mitten/libruncfg.so
endif

doc :
	doxygen Doxyfile

clean :
	$(RM) $(RMFLAGS) $(OBJ) libruncfg.dylib libruncfg.so doc