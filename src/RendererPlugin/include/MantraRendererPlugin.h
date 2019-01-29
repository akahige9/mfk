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

#ifndef MANTRARENDERERPLUGIN_H_
#define MANTRARENDERERPLUGIN_H_

#include <OpenEXR/ImathMatrix.h>

#include <Render/RenderBase.h>
#include "MantraWrapper.h"

namespace FnKat = Foundry::Katana;

namespace ds_mfk {

class MantraRendererPlugin : public FnKat::Render::RenderBase
{
public:
    MantraRendererPlugin(FnKat::FnScenegraphIterator rootIterator,
                         FnKat::GroupAttribute arguments);
    ~MantraRendererPlugin();

    // Render Control
    int start() override;
    int pause() override;
    int resume() override;
    int stop() override;

    // Interactive live updates
    int startLiveEditing() override;
    int stopLiveEditing() override;
    int processControlCommand(const std::string& command) override;
    int queueDataUpdates(FnKat::GroupAttribute updateAttribute) override;
    int applyPendingDataUpdates() override;
    bool hasPendingDataUpdates() const override;

    // Disk Render
    void configureDiskRenderOutputProcess(
        FnKat::Render::DiskRenderOutputProcess& diskRenderOutputProcess,
        const std::string& outputName,
        const std::string& outputPath,
        const std::string& renderMethodName,
        const float& frameTime) const override;

    // Plug-in Interface
    static Foundry::Katana::Render::RenderBase* create(
        FnKat::FnScenegraphIterator rootIterator, FnKat::GroupAttribute args)
    {
        return new MantraRendererPlugin(rootIterator, args);
    }

    static void flush() {}

private:
    bool initMantra(FnKat::FnScenegraphIterator rootIterator);
    void setupRender(FnKat::FnScenegraphIterator rootIterator);

    bool initScriptFile();
    bool buildHeader(FnKat::FnScenegraphIterator rootIterator);
    bool buildRenderCamera(FnKat::FnScenegraphIterator rootIteratorm,
                           FnKat::Render::RenderSettings& settings);
    void buildMainProcedural();

    template <typename T>
    std::string buildPropertyString(T attr) const;
    void parseGlobalProperties(FnKat::FnScenegraphIterator rootIterator);

    std::string _scriptFilePath;
    MantraWrapper _mantra;
};

} // namespace ds_mfk

#endif // MANTRARENDERERPLUGIN_H_
