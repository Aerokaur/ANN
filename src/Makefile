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
INTERFACE_DIR = F_Interface

# hdfc path
FF = h5pfc

CPP_SOURCES := $(wildcard *.cpp)
CINT_SOURCES := $(wildcard $(INTERFACE_DIR)/*.cpp)
FINT_SOURCES := $(wildcard $(INTERFACE_DIR)/*.F90)

CPP_OBJS := $(patsubst .%,$(INTERFACE_DIR)/%,$(CPP_SOURCES:.cpp=.o))
INT_OBJS := $(patsubst $(INTERFACE_DIR)/%,$(INTERFACE_DIR)/%,$(CINT_SOURCES:.cpp=.o))
INT_OBJS += $(patsubst $(INTERFACE_DIR)/%,$(INTERFACE_DIR)/%,$(FINT_SOURCES:.F90=.o))


CPPHEADERS = kd_tree.h kd_split.h kd_util.h kd_search.h \
	kd_pr_search.h kd_fix_rad_search.h perf.h pr_queue.h pr_queue_k.h

OBJECTS := $(CPP_OBJS) $(INT_OBJS)
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
#-----------------------------------------------------------------------------
# Make object files
#-----------------------------------------------------------------------------
%.o: %.cpp Makefile
	$(C++) -c -I$(INCDIR) $(CFLAGS) -o $@ $<

$(INTERFACE_DIR)/%.o: $(INTERFACE_DIR)/%.cpp Makefile
	$(C++) -c -I$(INCDIR) $(CFLAGS) -o $@ $<

$(INTERFACE_DIR)/%.o: $(INTERFACE_DIR)/%.F90 Makefile
	$(FF)  -c -o $@ $<

#-----------------------------------------------------------------------------
# Configuration definitions
#-----------------------------------------------------------------------------

include ../Make-config

install: 
	mv *.mod $(INCDIR)

#-----------------------------------------------------------------------------
# Cleaning
#-----------------------------------------------------------------------------

clean:
	-rm -f *.o core; cd $(INTERFACE_DIR); rm -f *.o
	cd $(INCDIR); rm -f *.mod

realclean: clean

