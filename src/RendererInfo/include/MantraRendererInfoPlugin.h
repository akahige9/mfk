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

#ifndef MANTRA_RENDERERINFOPLUGIN_H
#define MANTRA_RENDERERINFOPLUGIN_H

#include <memory>

#include <PRM/PRM_Template.h>

#include <RendererInfo/RendererInfoBase.h>
#include <RendererInfo/ShaderInfoCache.h>

namespace FnKat = Foundry::Katana;

class MGR_Node;
class MOT_Director;
class OP_OTLLibrary;
class PRM_ScriptPage;

namespace ds_mfk {

class MantraRendererInfoPlugin : public FnKat::RendererInfo::RendererInfoBase
{
public:
    MantraRendererInfoPlugin();
    virtual ~MantraRendererInfoPlugin();

    void fillRenderMethods(std::vector<FnKat::RendererInfo::RenderMethod*>&
                               renderMethods) const override;
    void fillRendererObjectNames(
        std::vector<std::string>& rendererObjectNames, const std::string& type,
        const std::vector<std::string>& typeTags) const override;

    void fillRendererObjectTypes(std::vector<std::string>& renderObjectTypes,
                                 const std::string& type) const override;
    void configureBatchRenderMethod(FnKat::RendererInfo::DiskRenderMethod&
                                        batchRenderMethod) const override;

    std::string getRegisteredRendererName() const override;
    std::string getRegisteredRendererVersion() const override;
    std::string getRendererObjectDefaultType(
        const std::string& type) const override;
    bool isPresetLocalFileNeeded(const std::string& outputType) const override;
    bool isNodeTypeSupported(const std::string& nodeType) const override;
    bool isPolymeshFacesetSplittingEnabled() const override;
    void fillShaderInputNames(std::vector<std::string>& shaderInputNames,
                              const std::string& shaderName) const override;
    void fillShaderInputTags(std::vector<std::string>& shaderInputTags,
                             const std::string& shaderName,
                             const std::string& inputName) const override;

    void fillShaderOutputNames(std::vector<std::string>& shaderOutputNames,
                               const std::string& shaderName) const override;

    void fillShaderOutputTags(std::vector<std::string>& shaderOutputTags,
                              const std::string& shaderName,
                              const std::string& outputName) const override;

    void fillRendererShaderTypeTags(
        std::vector<std::string>& shaderTypeTags,
        const std::string& shaderType) const override;

    std::string getRendererCoshaderType() const override;

    void buildLiveRenderControlModules(
        FnKat::GroupBuilder& liveRenderControlModules) const override;

    bool buildRendererObjectInfo(
        FnKat::GroupBuilder& rendererObjectInfo,
        const std::string& name,
        const std::string& type,
        const FnKat::GroupAttribute inputAttr) const override;

    void initialiseCaches() override;
    void flushCaches() override;

    static FnKat::RendererInfo::RendererInfoBase* create()
    {
        return new MantraRendererInfoPlugin();
    }

    static void flush() {}

private:

    struct ShaderInfo
    {
        std::string name;
        std::string opTable;
        OP_OTLLibrary* lib;
    };

    using ShaderInfoCacheIterator =
        FnKat::RendererInfo::ShaderInfoCache<ShaderInfo>::Iterator;

    void parseOtlFile(const std::string& filename);
    void loadShaders();

    void parseScriptPage(FnKat::GroupBuilder& paramsGb,
                         PRM_ScriptPage* scriptPage,
                         const std::string& page) const;

    void fillParameterFromTemplate(FnKat::GroupBuilder& paramsGb,
                                   const PRM_Template& paramTemplate,
                                   const std::string& page) const;

    void fillParametersForShader(FnKat::GroupBuilder& paramsGb,
                                 const std::string& shaderName) const;

    std::unique_ptr<MOT_Director> _director;
    MGR_Node* _nodeManager;
    FnKat::RendererInfo::ShaderInfoCache<ShaderInfo> _shaderInfoCache;
};

} // namespace ds_mfk

#endif // MANTRA_RENDERERINFOPLUGIN_H
