##############################################################################
# Volume Rendering Application
# Copyright (C) 2006-2007 Nicholas Schwarz
#
# This software is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either Version 2.1 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser Public License along
# with this library; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##############################################################################

CXX_FLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -Wno-deprecated

CXX = g++

default: GenerateBrick

GenerateBrick: GenerateBrick.o \
               Makefile
	$(CXX) $(CXX_FLAGS) -o GenerateBrick GenerateBrick.o
	cp GenerateBrick ../../bin

GenerateBrick.o: GenerateBrick.cpp
	$(CXX) $(CXX_FLAGS) -c GenerateBrick.cpp

clean:
	rm -rf *.o *~ GenerateBrick
	rm -rf ../../bin/GenerateBrick

