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

#ifndef MANTRAWRAPPER_H_
#define MANTRAWRAPPER_H_

#include <cstdlib>
#include <iostream>
#include <sstream>

namespace ds_mfk {

// Implements a basic wrapper around mantra pipe-based interface.
// This simplifies the communication with the renderer as Mantra does not
// currently expose render APIs in the same way PRMan or Arnold do, so we have
// to feed it with text commands.
class MantraWrapper
{
public:
    typedef MantraWrapper& (*MantraWrapperManipulator)(MantraWrapper&);

    // Define a type for std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);

    MantraWrapper();
    virtual ~MantraWrapper();

    bool init();
    bool close();

    void sendCommand(const std::string& cmd);

    // Support for stream-like usage of this class
    template <class T>
    MantraWrapper& operator <<(const T& x)
    {
        _strstream << x;
        return *this;
    }

    MantraWrapper& operator <<(MantraWrapperManipulator manip)
    {
        return manip(*this);
    }

    // Define a custom 'endl'-like token for the stream
    static MantraWrapper& endl(MantraWrapper& stream)
    {
        stream.flushStream();
        return stream;
    }

    // Custom operator << to handle std::endl
    MantraWrapper& operator <<(StandardEndLine manip)
    {
        manip(std::cout);
        return *this;
    }

private:
    void flushStream();

private:
    FILE* _pipe;
    std::ostringstream _strstream;
};

} // namespace ds_mfk

#endif // MANTRAWRAPPER_H_
