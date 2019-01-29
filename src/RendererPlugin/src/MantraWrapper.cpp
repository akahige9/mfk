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

#include <cstdio>

#include "MantraWrapper.h"

namespace ds_mfk {

MantraWrapper::MantraWrapper()
    : _pipe(nullptr)
{
}

MantraWrapper::~MantraWrapper()
{
}

bool MantraWrapper::init()
{
    if (_pipe)
    {
        std::cerr << "Mantra wrapper is already initialized\n";
        return false;
    }

    _pipe = popen("mantra", "w");

    if (!_pipe)
    {
        std::cerr << "Failed to initialize mantra wrapper\n";
        return false;
    }

    return true;
}

bool MantraWrapper::close()
{
    if (_pipe)
    {
        pclose(_pipe);
    }
    return true;
}

void MantraWrapper::flushStream()
{
    if (!_pipe)
    {
        return;
    }

    const std::string str = _strstream.str();
    fprintf(_pipe, "%s\n", str.c_str());

    // Reset the stream
    _strstream.str(std::string());
}

void MantraWrapper::sendCommand(const std::string& cmd)
{
    fprintf(_pipe, "%s\n", cmd.c_str());
}

} // namespace ds_mfk
