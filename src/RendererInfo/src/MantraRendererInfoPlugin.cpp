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

#include "MantraRendererInfoPlugin.h"

#include <algorithm>

#include <MGR/MGR_Node.h>
#include <MOT/MOT_Director.h>
#include <OP/OP_OTLLibrary.h>
#include <PRM/DS_Stream.h>
#include <PRM/PRM_ScriptPage.h>
#include <PRM/PRM_ScriptParm.h>

#include "MantraRendererInfoPlugin.h"

namespace ds_mfk {

MantraRendererInfoPlugin::MantraRendererInfoPlugin()
    : _nodeManager(nullptr)
{
    _director.reset(new MOT_Director("katana"));
    OPsetDirector(_director.get());
    _nodeManager = _director->getObjectManager();

    loadShaders();
}

MantraRendererInfoPlugin::~MantraRendererInfoPlugin()
{
}

void MantraRendererInfoPlugin::fillRenderMethods(
    std::vector<FnKat::RendererInfo::RenderMethod*>& renderMethods) const
{
    renderMethods.clear();

    // Katana will take ownership of the pointers.
    renderMethods.push_back(new FnKat::RendererInfo::DiskRenderMethod());
    renderMethods.push_back(new FnKat::RendererInfo::PreviewRenderMethod());
    renderMethods.push_back(new FnKat::RendererInfo::LiveRenderMethod());
}

void MantraRendererInfoPlugin::fillRendererObjectNames(
    std::vector<std::string>& rendererObjectNames,
    const std::string& type,
    const std::vector<std::string>& typeTags) const
{
    rendererObjectNames.clear();

    if (type == kFnRendererObjectTypeShader)
    {
        ShaderInfoCacheIterator it;
        for (it = _shaderInfoCache.begin(); it != _shaderInfoCache.end(); ++it)
        {
            rendererObjectNames.push_back((*it).first);
        }
    }
}

void MantraRendererInfoPlugin::fillRendererObjectTypes(
    std::vector<std::string>& renderObjectTypes,
    const std::string& type) const
{
    renderObjectTypes.clear();

    // We only support surface shader for now.
    if (type == kFnRendererObjectTypeShader)
    {
        renderObjectTypes.push_back("surface");
    }
}

void MantraRendererInfoPlugin::configureBatchRenderMethod(
    Foundry::Katana::RendererInfo::DiskRenderMethod& batchRenderMethod) const
{
    // e.g.
    // batchRenderMethod.setDebugOutputFileType("txt");
}

std::string MantraRendererInfoPlugin::getRegisteredRendererName() const
{
    return "mantra13";
}

std::string MantraRendererInfoPlugin::getRegisteredRendererVersion() const
{
    return "0.1v1";
}

std::string MantraRendererInfoPlugin::getRendererObjectDefaultType(
    const std::string& type) const
{
    // e.g.
    // if (type == kFnRendererObjectTypeRenderOutput)
    // {
    //     return kFnRendererOutputTypeColor;
    // }

    return FnKat::RendererInfo::RendererInfoBase::getRendererObjectDefaultType(
        type);
}

bool MantraRendererInfoPlugin::isPresetLocalFileNeeded(
    const std::string &outputType) const
{
    // e.g.
    // if (outputType == kSomeOutputTypeWhichRequiresPresetLocalFile)
    // {
    //     return true;
    // }

    return FnKat::RendererInfo::RendererInfoBase::isPresetLocalFileNeeded(
        outputType);
}

bool MantraRendererInfoPlugin::isNodeTypeSupported(
    const std::string &nodeType) const
{
    // e.g.
    // if (nodeType == "ShadingNode" || nodeType == "OutputChannelDefine")
    // {
    //     return true;
    // }

    return FnKat::RendererInfo::RendererInfoBase::isNodeTypeSupported(nodeType);
}

bool MantraRendererInfoPlugin::isPolymeshFacesetSplittingEnabled() const
{
    // If true is returned, Katana will split geometry automatically before
    // calling the Render plug-in.

    return FnKat::RendererInfo::RendererInfoBase::
        isPolymeshFacesetSplittingEnabled();
}

void MantraRendererInfoPlugin::fillShaderInputNames(
    std::vector<std::string>& shaderInputNames,
    const std::string& shaderName) const
{
    shaderInputNames.clear();

    // TODO: Fill out the names of inputs supported by the shader specified in
    // 'shaderName'. These are used to connect shading nodes.

    // e.g.
    // if (shaderName == "myWoodShader")
    // {
    //     shaderInputNames.push_back("colour");
    // }
}

void MantraRendererInfoPlugin::fillShaderInputTags(
    std::vector<std::string>& shaderInputTags,
    const std::string& shaderName,
    const std::string& inputName) const
{
    shaderInputTags.clear();

    // TODO: Fill out the type tags of inputs supported by the shader specified
    // in 'shaderName'. These are used to constrain connections between shading
    // nodes.
    //
    // Type tags are specified using a simple Python-like syntax
    // that supports boolean operations.

    // e.g.
    // if (shaderName == "myWoodShader")
    // {
    //     shaderInputTags.push_back("rgba or float");
    // }
}

void MantraRendererInfoPlugin::fillShaderOutputNames(
    std::vector<std::string>& shaderOutputNames,
    const std::string& shaderName) const
{
    shaderOutputNames.clear();

    // TODO: Fill out the names of inputs supported by the
    // shader specified in 'shaderName'. These are used to
    // connect shading nodes.

    // e.g.
    // if (shaderName == "myWoodShader")
    // {
    //     shaderOutputNames.push_back("out");
    // }
}

void MantraRendererInfoPlugin::fillShaderOutputTags(
    std::vector<std::string>& shaderOutputTags,
    const std::string& shaderName,
    const std::string& outputName) const
{
    shaderOutputTags.clear();

    // TODO: Fill out the type tags of outputs supported by the
    // shader specified in 'shaderName'. These are used to
    // constrain connections between shading nodes.
    //
    // Type tags are specified using a simple Python-like syntax
    // that supports boolean operations.

    // e.g.
    // if (shaderName == "myWoodShader")
    // {
    //     shaderOutputTags.push_back("rgba or float");
    // }
}

void MantraRendererInfoPlugin::fillRendererShaderTypeTags(
    std::vector<std::string>& shaderTypeTags,
    const std::string& shaderType) const
{
    shaderTypeTags.clear();

    // TODO: Fill out any type tags that are compatible with the
    // shader specified in 'shaderType'.  These are used to
    // constrain the connection of shading nodes to Network
    // Material node input terminals.

    // e.g.
    // Allowing only float and rgba to connect to a surface
    // terminal on a NetworkMaterial node:
    //
    // if (shaderType == "surface")
    // {
    //     shaderTypeTags.push_back("float or rgba");
    // }
}

std::string MantraRendererInfoPlugin::getRendererCoshaderType() const
{
    // e.g.
    // return "class";

    // Default:
    return FnKat::RendererInfo::RendererInfoBase::getRendererCoshaderType();
}

void MantraRendererInfoPlugin::buildLiveRenderControlModules(
    FnKat::GroupBuilder& liveRenderControlModules) const
{
    // e.g.
    // Construct the policy
    // std::string startLiveRenderArgumentsPolicy = " \
    //     { \
    //         'progressive':1, \
    //         '__childOrder':['progressive'], \
    //         '__childHints': \
    //         { \
    //             'progressive': \
    //             { \
    //                 'widget':'checkBox', \
    //                 'label':'Progressive Refinement' \
    //             }, \
    //         } \
    //     }";

    // // Create a render configuration live render policy module
    // FnKat::RendererInfo::RenderConfigurationPolicyLRCM liveRenderPolicy(
    //   "LiveRenderOptions", startLiveRenderArgumentsPolicy);
    // liveRenderPolicy.setLabel( "Live Render Options" );
    // liveRenderPolicy.buildAttribute( liveRenderControlModules );
}

bool MantraRendererInfoPlugin::buildRendererObjectInfo(
    FnKat::GroupBuilder& rendererObjectInfo,
    const std::string& name,
    const std::string& type,
    const FnKat::GroupAttribute inputAttr /*= 0x0*/) const
{
    if (type == kFnRendererObjectTypeShader)
    {
        const ShaderInfo& info = _shaderInfoCache.getShaderInfo(name);

        if (!info.lib)
        {
            return false;
        }

        // Note: Only Shop/surface ops are considerer, see parseOtlFile()
        std::set<std::string> typeTags;
        FnKat::Attribute containerHintsAttr;

        configureBasicRenderObjectInfo(
            rendererObjectInfo,
            kFnRendererObjectTypeShader,
            std::vector<std::string>(typeTags.begin(), typeTags.end()),
            info.name,
            info.lib->getSource().toStdString(),
            kFnRendererObjectValueTypeUnknown,
            containerHintsAttr);

        fillParametersForShader(rendererObjectInfo, name);
        return true;
    }

    return false;
}

void MantraRendererInfoPlugin::initialiseCaches()
{
}

void MantraRendererInfoPlugin::flushCaches()
{
    _shaderInfoCache.flush();
    loadShaders();
}

void MantraRendererInfoPlugin::parseOtlFile(const std::string& filename)
{
    OP_OTLManager& otlMan = _director->getOTLManager();
    UT_WorkBuffer wb;
    OP_OTLLibrary* lib = otlMan.addLibrary(
        filename.c_str(), "Current HIP", true, false, wb);

    if (!lib)
        return;

    // Get the number of definitions in this Otl
    const auto numDefs = lib->getNumDefinitions();

    for (auto i = 0; i < numDefs; ++i)
    {
        OP_OTLDefinition def;
        lib->getDefinition(i, def);

        const UT_String& defName = def.getName();
        const UT_String& defOpTableName = def.getOpTableName();
        const UT_String& extraInfo = def.getExtraInfo();

        // TODO: parse also other object types
        if (defOpTableName == "Shop"
            && (extraInfo == "vopmaterial" || extraInfo == "surface"))
        {
            ShaderInfo info;
            info.name = defName.toStdString();
            info.opTable = defOpTableName.toStdString();
            info.lib = lib;

            _shaderInfoCache.addShaderInfo(defName.toStdString(), info);
        }
    }
}

void MantraRendererInfoPlugin::loadShaders()
{
    const UT_PathSearch* otlPath =
        UT_PathSearch::getInstance(UT_HOUDINI_OTL_PATH);

    UT_StringArray files;
    otlPath->matchAllFiles(".otl", true, files, true);

    for (auto i = 0; i < files.entries(); ++i)
    {
        parseOtlFile(files(i).toStdString());
    }
}

void MantraRendererInfoPlugin::parseScriptPage(
    FnKat::GroupBuilder& paramsGb, PRM_ScriptPage* scriptPage,
    const std::string& pageName) const
{
    assert(scriptPage);

    // Skip OpenGL parameters
    if (strcmp(scriptPage->getLabel(), "OpenGL") == 0)
    {
        return;
    }

    // Check page visibility
    if (scriptPage->getFlag(PRM_ScriptPage::PRM_SCRIPT_INVISIBLE) != 0 ||
        scriptPage->getFlag(PRM_ScriptPage::PRM_SCRIPT_INVISIBLETAB) != 0)
    {
        return;
    }

    const int numParm = scriptPage->getParmEntries();
    for (int i = 0; i < numParm; ++i)
    {
        PRM_ScriptParm* scriptParm = scriptPage->getParm(i);
        assert(scriptParm);

        const int templateSize = scriptParm->computeTemplateSize();

        // Check if the parameter is a group
        PRM_ScriptGroup* grp = scriptParm->getGroup();
        if (grp)
        {
            for (int p = 0; p < grp->getPageCount(); ++p)
            {
                PRM_ScriptPage* page = grp->getPage(p);
                std::string newPageName;
                if (!pageName.empty())
                {
                    newPageName = pageName;
                    newPageName += ".";
                }
                newPageName += page->getLabel();
                parseScriptPage(paramsGb, page, newPageName);
            }
        }
        else
        {
            if (templateSize == 1)
            {
                int idx = 0;
                PRM_ScriptImports* imports = nullptr;
                PRM_Template paramTemplate;
                scriptParm->fillTemplate(&paramTemplate, idx, imports, 0);

                if (!paramTemplate.getInvisible())
                {
                    fillParameterFromTemplate(paramsGb, paramTemplate,
                                              pageName);
                }
            }
            else
            {
                // Un-handled type
                // TODO: implement or add warning message
            }
        }
    }
}

void MantraRendererInfoPlugin::fillParameterFromTemplate(
    FnKat::GroupBuilder& paramsGb, const PRM_Template& paramTemplate,
    const std::string& page) const
{
    // TODO: not all parameter types are currently handled !
    const PRM_Name* paramName = paramTemplate.getNamePtr();
    const char* name = paramName->getToken();

    int type = kFnRendererObjectValueTypeUnknown;

    const int vectorSize = paramTemplate.getVectorSize();
    const PRM_Type& paramType = paramTemplate.getType();

    FnKat::Attribute defaultAttr;
    FnKat::GroupBuilder hintsGb;

    hintsGb.set("label", FnKat::StringAttribute(paramName->getLabel()));

    if (paramType.isFloatType())
    {
        PRM_Default* defaults = paramTemplate.getFactoryDefaults();

        switch (vectorSize)
        {
            case 1:
            {
                type = kFnRendererObjectValueTypeFloat;
                float defaultValue = 0.0f;
                if (defaults)
                {
                    defaultValue = defaults[0].getFloat();
                }
                defaultAttr = FnKat::FloatAttribute(defaultValue);
            }
            break;

            case 2:
            {
                type = kFnRendererObjectValueTypeVector2;
                float defaultValue[2] = { 0.0f, 0.0f };
                if (defaults)
                {
                    defaultValue[0] = defaults[0].getFloat();
                    defaultValue[1] = defaults[1].getFloat();
                }
                defaultAttr = FnKat::FloatAttribute(defaultValue, 2, 2);
            }
            break;

            case 3:
            {
                type = kFnRendererObjectValueTypeVector3;
                float defaultValue[3] = { 0.0f, 0.0f, 0.0f };
                if (defaults)
                {
                    defaultValue[0] = defaults[0].getFloat();
                    defaultValue[1] = defaults[1].getFloat();
                    defaultValue[2] = defaults[2].getFloat();
                }
                defaultAttr = FnKat::FloatAttribute(defaultValue, 3, 3);

                if (paramType.hasFloatType(PRM_Type::PRM_FLOAT_RGBA))
                {
                    hintsGb.set("widget", FnKat::StringAttribute("color"));
                }
            }
            break;

            case 4:
                type = kFnRendererObjectValueTypeVector4;
                defaultAttr = FnKat::FloatAttribute(0.0f);
                break;

            case 16:
                type = kFnRendererObjectValueTypeMatrix;
                defaultAttr = FnKat::FloatAttribute(0.0f);
                break;

            default:
                defaultAttr = FnKat::FloatAttribute(0.0f);
                break;
        }
    }
    else if (paramType.isStringType())
    {
        type = kFnRendererObjectValueTypeString;
        defaultAttr = FnKat::StringAttribute("");
    }
    else
    {
        defaultAttr = FnKat::FloatAttribute(0.0f);
    }

    if (!page.empty())
    {
        hintsGb.set("page", FnKat::StringAttribute(page));
    }

    EnumPairVector enums;
    addRenderObjectParam(paramsGb, name, type, 1, defaultAttr, hintsGb.build(),
                         enums);
}

void MantraRendererInfoPlugin::fillParametersForShader(
    FnKat::GroupBuilder& paramsGb, const std::string& shaderName) const
{
    if (shaderName.empty())
        return;

    const ShaderInfo& info = _shaderInfoCache.getShaderInfo(shaderName);
    if (!info.lib)
        return;

    // Get the index file for the current definition
    // FS_IndexFile*
    auto fs = info.lib->getDefinitionIndexFile(info.opTable.c_str(),
                                                        info.name.c_str());
    if (!fs)
        return;

    // Extract the stream for the DialogScript section
    FS_ReaderStream* rd = fs->getSectionStream(OTL_DS_SECTION);
    // TODO: add proper handling
    assert(rd);

    UT_IStream* st = rd->getStream();
    // TODO: add proper handling
    assert(st);

    // Parse the DialogScript
    DS_Stream dsStream(*st);

    PRM_ScriptPage scriptPage;
    scriptPage.parse(dsStream, true, nullptr, true);

    parseScriptPage(paramsGb, &scriptPage, "");
}


DEFINE_RENDERERINFO_PLUGIN(MantraRendererInfoPlugin)

}

__attribute__ ((visibility("default")))
void registerPlugins()
{
    REGISTER_PLUGIN(ds_mfk::MantraRendererInfoPlugin,
                    "MantraRendererInfoPlugin", 0, 1);
}
