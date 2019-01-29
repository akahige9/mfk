// *****************************************************************************
//
// Copyright (c) 2014-2019, Davide Selmo.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of Davide Selmo nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------
//
// This software is provided "as is", and is entirely unconnected to any
// development work done by The Foundry or Side Effects.
//
// Please don't use the usual The Foundry or Side Effects support channels
// for any questions or issues relating to this software.
// Email ds_gfx@zoho.com instead.
//
// All trademarks are the properties of their respective holders.
//
// *****************************************************************************

#include <GU/GU_Detail.h>
#include <RenderOutputUtils/RenderOutputUtils.h>

#include "ProceduralIterator.h"
#include "KatanaProcedural.h"

namespace  ds_mfk {

const VRAY_ProceduralArg g_proceduralArgs[] =
{
    VRAY_ProceduralArg("producerFilename", "string", ""),
    VRAY_ProceduralArg()
};

const char* KatanaProcedural::getClassName()
{
    return className();
}

const char* KatanaProcedural::className() const
{
    return "KatanaProcedural";
}

int KatanaProcedural::initialize(const UT_BoundingBox* bbox)
{
    UT_String producerFilename;

    import("producerFilename", producerFilename);
    _producerFilepath = producerFilename.toStdString();

    if (_producerFilepath.empty())
    {
        std::cerr << "Procedural initialization failed: "
                  << "'producerFilename' no set.\n";
        return 0;
    }

    const char* katanaRoot = getenv("KATANA_ROOT");
    if (!katanaRoot)
    {
        std::cerr << "Procedural initialization failed: "
                  << "KATANA_ROOT not set, unable to bootstrap Geolib.\n";
        return 0;
    }

    if (!FnKat::RenderOutputUtils::bootstrapGEOLIB(katanaRoot))
    {
        std::cerr << "Procedural initialization failed: "
                  << "Failed to bootstrap Geolib.\n";
        return 0;
    }

    _rootIterator = FnKat::RenderOutputUtils::readScript(_producerFilepath);

    if (!_rootIterator.isValid())
    {
        std::cerr << "Procedural initialization failed: "
                  << "Invalid root iterator\n";
        return 0;
    }

    return 1;
}

void KatanaProcedural::getBoundingBox(UT_BoundingBox& bbox)
{
    bbox.initMaxBounds();
}

void KatanaProcedural::render()
{
    openProceduralObject();

    ProceduralIterator* proc = new ProceduralIterator(_rootIterator);
    addProcedural(proc);

    closeObject();
}

} // namespace ds_mfk

__attribute__ ((visibility("default")))
VRAY_Procedural* allocProcedural(const char*)
{
    return new ds_mfk::KatanaProcedural();
}

__attribute__ ((visibility("default")))
const VRAY_ProceduralArg* getProceduralArgs(const char*)
{
    return ds_mfk::g_proceduralArgs;
}
