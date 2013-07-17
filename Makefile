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

MAKE=make
MAKEFLAGS=

build :
	$(MAKE) $(MAKEFLAGS) -C trace build
	$(MAKE) $(MAKEFLAGS) -C parsing build
	$(MAKE) $(MAKEFLAGS) -C runcfg build
	$(MAKE) $(MAKEFLAGS) -C mc build

installbuild :
	cp config.mk trace
	cp config.mk parsing
	cp config.mk runcfg
	cp config.mk mc

install : 
	$(MAKE) $(MAKEFLAGS) -C trace install
	$(MAKE) $(MAKEFLAGS) -C parsing install
	$(MAKE) $(MAKEFLAGS) -C runcfg install
	$(MAKE) $(MAKEFLAGS) -C mc install

uninstall :
	$(MAKE) $(MAKEFLAGS) -C trace uninstall
	$(MAKE) $(MAKEFLAGS) -C parsing uninstall
	$(MAKE) $(MAKEFLAGS) -C runcfg uninstall
	$(MAKE) $(MAKEFLAGS) -C mc uninstall

doc :
	$(MAKE) $(MAKEFLAGS) -C trace doc
	$(MAKE) $(MAKEFLAGS) -C parsing doc
	$(MAKE) $(MAKEFLAGS) -C runcfg doc
	$(MAKE) $(MAKEFLAGS) -C mc doc

clean :
	$(MAKE) $(MAKEFLAGS) -C trace clean
	$(MAKE) $(MAKEFLAGS) -C parsing clean
	$(MAKE) $(MAKEFLAGS) -C runcfg clean
	$(MAKE) $(MAKEFLAGS) -C mc clean