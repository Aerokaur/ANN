#-----------------------------------------------------------------------------
# Makefile for ANN library
#----------------------------------------------------------------------
# Copyright (c) 1997-2005 University of Maryland and Sunil Arya and
# David Mount.  All Rights Reserved.
# 
# This software and related documentation is part of the Approximate
# Nearest Neighbor Library (ANN).  This software is provided under
# the provisions of the Lesser GNU Public License (LGPL).  See the
# file ../ReadMe.txt for further information.
# 
# The University of Maryland (U.M.) and the authors make no
# representations about the suitability or fitness of this software for
# any purpose.  It is provided "as is" without express or implied
# warranty.
#----------------------------------------------------------------------
# History:
#	Revision 0.1  03/04/98
#		Initial release
#	Revision 1.0  04/01/05
#		Renamed files from .cc to .cpp for Microsoft Visual C++
#		Added kd_dump.cpp
#	Revision 1.1  05/03/05
#		Added kd_fix_rad_search.cpp and bd_fix_rad_search.cpp
#----------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Some basic definitions:
#		BASEDIR		where include, src, lib, ... are
#		INCLIB		include directory
#		LIBLIB		library directory
#-----------------------------------------------------------------------------
BASEDIR	= ..
INCDIR	= $(BASEDIR)/include
LIBDIR	= $(BASEDIR)/lib

CPP_SOURCES := $(wildcard *.cpp)
FF_SOURCES := $(wildcard *.F90)

CPP_OBJS := $(patsubst .%,%,$(CPP_SOURCES:.cpp=.o))
FF_OBJS += $(patsubst .%,%,$(FF_SOURCES:.F90=.o))


CPPHEADERS = kd_tree.h kd_split.h kd_util.h kd_search.h \
	kd_pr_search.h kd_fix_rad_search.h perf.h pr_queue.h pr_queue_k.h

OBJECTS := $(CPP_OBJS) $(FF_OBJS)
#-----------------------------------------------------------------------------
# Make the library
#-----------------------------------------------------------------------------

default:
	@echo "Specify a target configuration"

targets: $(LIBDIR)/$(ANNLIB)

$(LIBDIR)/$(ANNLIB): $(OBJECTS)
	$(MAKELIB) $(ANNLIB) $(OBJECTS)
	$(RANLIB) $(ANNLIB)
	mv $(ANNLIB) $(LIBDIR)
	cp *.mod $(INCDIR)
#-----------------------------------------------------------------------------
# Make object files
#-----------------------------------------------------------------------------
%.o: %.cpp Makefile
	$(C++) -c -I$(INCDIR) $(CFLAGS) -o $@ $<

%.o: %.F90 Makefile
	$(FF)  -c $(FFLAGS) -o $@ $<

#-----------------------------------------------------------------------------
# Configuration definitions
#-----------------------------------------------------------------------------

include ../Make-config

#-----------------------------------------------------------------------------
# Cleaning
#-----------------------------------------------------------------------------

clean:
	-rm -f *.o core *.mod;
	cd $(INCDIR); rm -f *.mod

realclean: clean

