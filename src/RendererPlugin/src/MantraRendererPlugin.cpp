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

#include <sys/types.h>
#include <unistd.h>

#include <sstream>

#include <OpenEXR/ImathMatrix.h>

#include <RendererInfo/RenderMethod.h>
#include <RenderOutputUtils/RenderOutputUtils.h>

#include <FnScenegraphIterator/FnScenegraphIterator.h>

#include "MantraRendererPlugin.h"

namespace ds_mfk
{

MantraRendererPlugin::MantraRendererPlugin(
    FnKat::FnScenegraphIterator rootIterator,
    FnKat::GroupAttribute arguments)
        : FnKat::Render::RenderBase(rootIterator, arguments)
{
}

MantraRendererPlugin::~MantraRendererPlugin()
{
    _mantra.close();
}

int MantraRendererPlugin::start()
{
    initScriptFile();

    FnKat::FnScenegraphIterator rootIterator = getRootIterator();

    FnKat::Render::RenderSettings renderSettings(rootIterator);

    if (!initMantra(rootIterator))
    {
        std::cerr << "Unable to initialize Mantra wrapper." << std::endl;
        return -1;
    }

    if (!buildHeader(rootIterator))
    {
        std::cerr << "Unable to initialize Mantra render." << std::endl;
        return -1;
    }

    parseGlobalProperties(rootIterator);

    if (!buildRenderCamera(rootIterator, renderSettings))
    {
        std::cerr << "Unable to initialize Mantra render." << std::endl;
        return -1;
    }

    buildMainProcedural();

    // Start the render
    _mantra.sendCommand("ray_raytrace");

    // End the Mantra session
    _mantra.sendCommand("ray_quit");

    return 0;
}

int MantraRendererPlugin::pause()
{
    // Default:
    return FnKat::Render::RenderBase::pause();
}

int MantraRendererPlugin::resume()
{
    // Default:
    return FnKat::Render::RenderBase::resume();
}

int MantraRendererPlugin::stop()
{
    // Default:
    return FnKat::Render::RenderBase::stop();
}

int MantraRendererPlugin::startLiveEditing()
{
    // Default:
    return FnKat::Render::RenderBase::startLiveEditing();
}

int MantraRendererPlugin::stopLiveEditing()
{
    // Default:
    return FnKat::Render::RenderBase::stopLiveEditing();
}

int MantraRendererPlugin::processControlCommand(const std::string& command)
{
    // Default:
    return FnKat::Render::RenderBase::processControlCommand(command);
}

int MantraRendererPlugin::queueDataUpdates(
    FnKat::GroupAttribute updateAttribute)
{
    // Default:
    return FnKat::Render::RenderBase::queueDataUpdates(updateAttribute);
}

int MantraRendererPlugin::applyPendingDataUpdates()
{
    // Default:
    return FnKat::Render::RenderBase::applyPendingDataUpdates();
}

bool MantraRendererPlugin::hasPendingDataUpdates() const
{
    // Default:
    return FnKat::Render::RenderBase::hasPendingDataUpdates();
}

// Disk Render

void MantraRendererPlugin::configureDiskRenderOutputProcess(
    FnKat::Render::DiskRenderOutputProcess& diskRenderOutputProcess,
    const std::string& outputName,
    const std::string& outputPath,
    const std::string& renderMethodName,
    const float& frameTime) const
{
    std::auto_ptr<FnKat::Render::RenderAction>
        renderAction(new FnKat::Render::NoOutputRenderAction());

    diskRenderOutputProcess.setRenderAction(renderAction);
}

bool MantraRendererPlugin::initMantra(FnKat::FnScenegraphIterator rootIterator)
{
    return _mantra.init();
}

bool MantraRendererPlugin::initScriptFile()
{
    // Build the script file path
    std::ostringstream ss;

    ss << "/tmp/mantra_katana_script_file_" << getpid() << "_.py";

    _scriptFilePath = ss.str();

    std::string cmd = "cp ";
    cmd += getFilterScriptFilename();
    cmd += " ";
    cmd += _scriptFilePath;

    system(cmd.c_str());

    return true;
}

bool MantraRendererPlugin::buildHeader(FnKat::FnScenegraphIterator rootIterator)
{
    // Frame number
    int frameNumber = static_cast<int>(getRenderTime());
    _mantra << "ray_time " << frameNumber << MantraWrapper::endl;

    // Define display driver and image planes
    // FIXME: Add support for custom display drivers and image planes

    // NOTE: Only MPlay is support a.t.m.
    _mantra.sendCommand("ray_image \"ip\"");

    _mantra.sendCommand("ray_declare plane string IPlay.s3dleftplane \"\"");
    _mantra.sendCommand("ray_declare plane string IPlay.s3drightplane \"\"");

    _mantra.sendCommand("ray_declare plane string IPlay.rendermode \"append\"");
    _mantra.sendCommand("ray_declare plane string IPlay.framerange \"1 1\"");
    _mantra.sendCommand("ray_declare plane float IPlay.currentframe 1");

    _mantra.sendCommand("ray_start plane");
    _mantra.sendCommand("ray_property plane variable \"Cf+Af\"");
    _mantra.sendCommand("ray_property plane vextype \"vector4\"");
    _mantra.sendCommand("ray_property plane channel \"C\"");
    _mantra.sendCommand("ray_end");

    // Thread count
    // FIXME: Remove when 'threadcount' will be added to MantraGlobalSettings.xml
    _mantra.sendCommand("ray_property global threadcount 1");

    return true;
}

bool MantraRendererPlugin::buildRenderCamera(
    FnKat::FnScenegraphIterator rootIterator,
    FnKat::Render::RenderSettings& settings)
{
    const FnKat::Render::CameraSettings* cameraSettings =
        settings.getCameraSettings();

    const std::string cameraPath = settings.getCameraName();
    if (cameraPath.empty())
    {
        std::cerr << "[Error] Unable to find main render camera. "
                  << "Render aborted." << std::endl;
        return false;
    }

    FnKat::FnScenegraphIterator cameraIterator =
        rootIterator.getByPath(cameraPath);
    if (!cameraIterator.isValid())
    {
        std::cerr << "[Error] Unable to find main render camera. "
                  << "Render aborted.\n";
        return false;
    }

    int dataWindowSize[2];
    settings.getDataWindowSize(dataWindowSize);
    _mantra << "ray_property image resolution " << dataWindowSize[0]
            << " " << dataWindowSize[1] << MantraWrapper::endl;

    // FIXME: fixed pixel aspect ratio
    _mantra.sendCommand("ray_property image pixelaspect 1");

    FnKat::DoubleAttribute orthoWidthAttr =
        cameraIterator.getAttribute("geometry.orthographicWidth");
    const double orthoWidth = orthoWidthAttr.getValue(false, 30.0);
    _mantra << "ray_property camera orthowidth " << orthoWidth
            << MantraWrapper::endl;

    FnKat::DoubleAttribute fovAttr =
        cameraIterator.getAttribute("geometry.fov");
    const double fov = fovAttr.getValue(false, 70.0);
    _mantra << "ray_property camera zoom " << (180.0 / fov / M_PI)
            << MantraWrapper::endl;

    // FIXME: support for perspective projection only
    _mantra.sendCommand("ray_property camera projection \"perspective\"");

    float clipping[2];
    cameraSettings->getClipping(clipping);
    _mantra << "ray_property camera clip " << clipping[0]
            << " " << clipping[1] << MantraWrapper::endl;

    // FIXME: fixed window and crop sizes
    _mantra.sendCommand("ray_property image window 0 1 0 1");
    _mantra.sendCommand("ray_property image crop 0 1 0 1");

    // Build camera transform
    FnKat::GroupAttribute xformAttr =
        FnKat::RenderOutputUtils::getCollapsedXFormAttr(cameraIterator);

    // FIXME: no motion blur a.t.m.
    std::vector<float> relevantSampleTimes;
    relevantSampleTimes.push_back(0.0f);

    bool isAbsolute;
    FnKat::RenderOutputUtils::XFormMatrixVector xforms;
    FnKat::RenderOutputUtils::calcXFormsFromAttr(
        xforms, isAbsolute, xformAttr,
        relevantSampleTimes,
        FnKat::RenderOutputUtils::kAttributeInterpolation_Linear);

    const double* xformValues = xforms[0].getValues();
    Imath::M44d mat((double(*)[4])xformValues);
    mat.invert();

    const double* elems = mat.getValue();
    std::ostringstream ssXform;
    ssXform << "ray_transform ";
    for (size_t j = 0; j < 16; ++j)
    {
        ssXform << elems[j] << " ";
    }
    _mantra.sendCommand(ssXform.str());

    // Create a dummy head-light
    _mantra.sendCommand("ray_start light");

    std::ostringstream ssLight;
    ssLight << "ray_transform 1 0 0 0   0 1 0 0   0 0 1 0 ";
    ssLight << xformValues[12] << " ";
    ssLight << xformValues[13] << " ";
    ssLight << xformValues[14] << " ";
    ssLight << xformValues[15] << " ";
    _mantra.sendCommand(ssLight.str());

    _mantra.sendCommand(
        "ray_property object name \"soho_autoheadlight_light\"");
    _mantra.sendCommand("ray_property light projection \"perspective\"");
    _mantra.sendCommand("ray_property light zoom 1.20710550585 1.20710550585");
    _mantra.sendCommand("ray_end");

    // Mantra pixel samples
    FnKat::FloatAttribute pixelSamplesAttr =
        rootIterator.getAttribute("mantraGlobalStatements.image.pixelSamples");
    if (pixelSamplesAttr.isValid())
    {
        FnKat::FloatConstVector pixelSamples =
            pixelSamplesAttr.getNearestSample(0.f);
        _mantra << "ray_property image samples " << pixelSamples[0]
                << " " << pixelSamples[1] << MantraWrapper::endl;
    }

    return true;
}

void MantraRendererPlugin::buildMainProcedural()
{
    std::string procCommand = "ray_procedural KatanaProc ";
    procCommand += "producerFilename \"";
    procCommand += _scriptFilePath;
    procCommand += "\" ";

    _mantra.sendCommand("ray_start object");
    _mantra.sendCommand(procCommand);
    _mantra.sendCommand("ray_property object name \"katana_procedural\"");
    _mantra.sendCommand("ray_end");
}

template <typename T>
std::string MantraRendererPlugin::buildPropertyString(T attr) const
{
    assert(attr.isValid());

    std::ostringstream ss;
    FnKat::ConstVector<typename T::value_type> vec =
        attr.getNearestSample(0.0f);

    for (size_t i = 0; i < vec.size(); ++i)
    {
        ss << " " << vec.at(i);
    }

    return ss.str();
}

void MantraRendererPlugin::parseGlobalProperties(
    FnKat::FnScenegraphIterator rootIterator)
{
    FnKat::GroupAttribute mantraGlobals =
        rootIterator.getAttribute("mantra13GlobalStatements");
    if (!mantraGlobals.isValid())
    {
        return;
    }

    const int numChildren = mantraGlobals.getNumberOfChildren();
    for (int i = 0; i < numChildren; ++i)
    {
        const std::string attrName = mantraGlobals.getChildName(i);
        FnKat::DataAttribute attr = mantraGlobals.getChildByIndex(i);

        if (attr.isValid())
        {
            std::string attrStr;
            switch (attr.getType())
            {
                case kFnKatAttributeTypeInt:
                    attrStr = buildPropertyString<FnKat::IntAttribute>(attr);
                    break;

                case kFnKatAttributeTypeFloat:
                    attrStr = buildPropertyString<FnKat::FloatAttribute>(attr);
                    break;

                case kFnKatAttributeTypeDouble:
                    attrStr = buildPropertyString<FnKat::DoubleAttribute>(attr);
                    break;

                case kFnKatAttributeTypeString:
                    attrStr = buildPropertyString<FnKat::StringAttribute>(attr);
                    break;

                default:
                    // TODO: Add warning message
                    continue;
            }

            _mantra << "ray_property global " << attrName
                    << " " << attrStr
                    << MantraWrapper::endl;
        }
    }
}

DEFINE_RENDER_PLUGIN(MantraRendererPlugin)

} // namespace ds_mfk

__attribute__ ((visibility("default")))
void registerPlugins()
{
    REGISTER_PLUGIN(ds_mfk::MantraRendererPlugin, "mantra13", 0, 1);
}
