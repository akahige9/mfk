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


# The output objects dir
OBJDIR = ./out

# The plug-in .so file
OUTFILENAME = MantraRendererInfoPlugin.so
OUTFILEPATH = $(OBJDIR)/$(OUTFILENAME)

# Install path
INSTALLFILEPATH = ../../Resources/Libs/$(OUTFILENAME)

# Sources and includes
SOURCES = src/MantraRendererInfoPlugin.cpp

INCLUDES = -I./include

# Houdini compiler and linker flags
HOUDINI_CXX_FLAGS = $(shell hcustom -c)
HOUDINI_LINK_FLAGS = $(shell hcustom -m) -L$(HFS)/dsolib

# Third Party libraries
LIBS += -ldl -lHoudiniPRM -lHoudiniOPZ -lHAPI

# PLUGIN APIs sources and includes
PLUGIN_SRC = $(KATANA_HOME)/plugin_apis/src

# Base Foundry Plugin API
SOURCES += $(PLUGIN_SRC)/FnPluginSystem/FnPlugin.cpp

# Renderer Info API
SOURCES += $(PLUGIN_SRC)/RendererInfo/RendererInfoBase.cpp
SOURCES += $(PLUGIN_SRC)/RendererInfo/LiveRenderControlModule.cpp
SOURCES += $(PLUGIN_SRC)/RendererInfo/LiveRenderFilter.cpp
SOURCES += $(PLUGIN_SRC)/RendererInfo/RenderMethod.cpp
SOURCES += $(PLUGIN_SRC)/RendererInfo/ShaderInfoCache.cpp

# Asset API
SOURCES += $(PLUGIN_SRC)/FnAsset/FnAsset.cpp
SOURCES += $(PLUGIN_SRC)/FnAsset/FnFileSequence.cpp

# Attribute API
SOURCES += $(PLUGIN_SRC)/FnAttribute/FnAttribute.cpp
SOURCES += $(PLUGIN_SRC)/FnAttribute/FnDataBuilder.cpp

# Logging API
SOURCES += $(PLUGIN_SRC)/FnLogging/FnLogging.cpp

INCLUDES += -I$(KATANA_HOME)/plugin_apis/include

# Object files and flags
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

CXXFLAGS = -O2 -std=c++11 -Wall -pipe -m64 -fPIC -DPIC -fvisibility=hidden

# Targets:
all: $(OUTFILEPATH)

$(OUTFILEPATH): $(OBJS)
	@echo "  Compiling Mantra RendererInfo plugin..."
	$(CXX) $(CXXFLAGS) $(OBJS) $(LIBPATH) $(HOUDINI_LINK_FLAGS) $(LIBS) -shared -o $(OUTFILEPATH)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(HOUDINI_CXX_FLAGS) $(INCLUDES) -O0 -c $< -o $@

clean:
	@echo "  Cleaning Mantra RendererInfo plugin"
	@rm -rf $(OBJDIR)

install:
	cp $(OUTFILEPATH) $(INSTALLFILEPATH)
