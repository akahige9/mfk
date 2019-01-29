# ******************************************************************************
#
# Copyright (c) 2014-2019, Davide Selmo.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of Davide Selmo nor the names of
#   its contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ------------------------------------------------------------------------------
#
# This software is provided "as is", and is entirely unconnected to any
# development work done by The Foundry or Side Effects.
#
# Please don't use the usual The Foundry or Side Effects support channels
# for any questions or issues relating to this software.
# Email ds_gfx@zoho.com instead.
#
# All trademarks are the properties of their respective holders.
#
# ******************************************************************************

# Check if the needed environment variables are set
ifndef KATANA_HOME
$(error KATANA_HOME is not set)
endif

ifndef HFS
$(error HFS is not set)
endif

ifndef HIH
$(error HIH is not set)
endif


# Output objects dir
OBJDIR = ./out

# Output file name and path
OUTFILENAME = KatanaProcedural.so
OUTFILEPATH = $(OBJDIR)/$(OUTFILENAME)

# Install path
INSTALLFILEPATH = $(HIH)/dso/$(OUTFILENAME)

# Sources and includes
SOURCES =	src/KatanaProcedural.cpp
SOURCES +=	src/ProceduralIterator.cpp

INCLUDES = -I./include

# Houdini compiler and linker flags
HOUDINI_CXX_FLAGS = $(shell hcustom -c)
HOUDINI_LINK_FLAGS = $(shell hcustom -m)

# Katana APIs sources and includes
PLUGIN_SRC = $(KATANA_HOME)/plugin_apis/src
SOURCES += $(PLUGIN_SRC)/FnAttribute/FnAttribute.cpp
SOURCES += $(PLUGIN_SRC)/FnScenegraphIterator/FnScenegraphIterator.cpp
SOURCES += $(PLUGIN_SRC)/RenderOutputUtils/RenderOutputUtils.cpp
SOURCES += $(PLUGIN_SRC)/RenderOutputUtils/XFormMatrix.cpp

INCLUDES += -I$(KATANA_HOME)/plugin_apis/include

# Object files and flags
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Compiler flags
CXXFLAGS = -Wall -pipe -m64 -fPIC -DPIC -fvisibility=hidden

# Targets:
all: $(OUTFILEPATH)

$(OUTFILEPATH): $(OBJS)
	@echo "  Compiling KatanaProcedural plug-in..."
	$(CXX) -g $(CXXFLAGS) $(OBJS) $(LIBPATH) $(LIBS) $(HOUDINI_LINK_FLAGS) -shared -o $(OUTFILEPATH) -Wl,-soname,$(OUTFILENAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(HOUDINI_CXX_FLAGS) $(INCLUDES) -c $< -o $@


clean:
	@echo "  Cleaning KatanaProcedural plug-in..."
	@rm -rf $(OBJDIR)

install:
	cp $(OUTFILEPATH) $(INSTALLFILEPATH)
