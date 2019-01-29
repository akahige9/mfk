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

#include <sstream>

#include <GU/GU_PrimPoly.h>
#include <GU/GU_PrimSphere.h>
#include <RenderOutputUtils/RenderOutputUtils.h>

#include "ProceduralIterator.h"

namespace ds_mfk {

const char* ProceduralIterator::getClassName()
{
    return className();
}

const char* ProceduralIterator::className() const
{
    return "ProceduralIterator";
}

int ProceduralIterator::initialize(const UT_BoundingBox* bbox)
{
    return 0;
}

void ProceduralIterator::getBoundingBox(UT_BoundingBox& bbox)
{
    bbox.initMaxBounds();

    FnKat::DoubleAttribute boundAttr = _sgIterator.getAttribute("bound");
    if (!boundAttr.isValid())
        return;

    const FnKat::DoubleConstVector boundValues =
        boundAttr.getNearestSample(0.0f);
    if (6 == boundValues.size())
    {
        bbox.setBounds(
            boundValues[0], boundValues[2], boundValues[4],
            boundValues[1], boundValues[3], boundValues[5]);
    }
}

void ProceduralIterator::render()
{
    renderLocation(_sgIterator);
}

void ProceduralIterator::renderLocation(FnKat::FnScenegraphIterator sgIterator)
{
    while (sgIterator.isValid())
    {
        processLocation(sgIterator);
        sgIterator = sgIterator.getNextSibling();
    }
}

void ProceduralIterator::processLocation(FnKat::FnScenegraphIterator sgIterator)
{
    if (!sgIterator.isValid())
    {
        return;
    }

    const std::string& type = sgIterator.getType();

    if (type == "group")
    {
        processTransform(sgIterator);
        processGroup(sgIterator);
    }
    // TODO: polymesh and subdmesh should be handled separately.
    else if (type == "polymesh" || type == "subdmesh")
    {
        GU_Detail* gdp = processPoly(sgIterator, 0x0);
        openGeometryObject();
            processTransform(sgIterator);
            processMaterial(sgIterator);
            addGeometry(gdp, 0);
        closeObject();
    }
    else
    {
        processGroup(sgIterator);
    }
}

void ProceduralIterator::processGroup(FnKat::FnScenegraphIterator sgIterator)
{
    FnKat::FnScenegraphIterator childSgIterator = sgIterator.getFirstChild();
    if (!childSgIterator.isValid())
    {
        return;
    }

    // TODO: code to enable creation of recursive procedurals
    // DoubleAttribute boundAttr = childSgIterator.getAttribute("bound");
    // if (boundAttr.isValid())
    // {
    //    openProceduralObject();
    //        addProcedural(new ProceduralIterator(childSgIterator));
    //    closeObject();
    //    return;
    //}

    renderLocation(childSgIterator);
}

GU_Detail* ProceduralIterator::processPoly(
    FnKat::FnScenegraphIterator iterator, void* optionalInput)
{
    if (!iterator.isValid())
    {
        std::cerr << "Procedural failed: invalid iterator\n";
        return nullptr;
    }

    FnKat::GroupAttribute geometryGroupAttr = iterator.getAttribute("geometry");
    if (!geometryGroupAttr.isValid())
    {
        std::cerr << "Procedural failed: 'geometry' group not found\n";
        return nullptr;
    }

    FnKat::GroupAttribute polyGroupAttr =
        geometryGroupAttr.getChildByName("poly");
    FnKat::GroupAttribute pointGroupAttr =
        geometryGroupAttr.getChildByName("point");
    FnKat::GroupAttribute vertexGroupAttr =
        geometryGroupAttr.getChildByName("vertex");

    if (!polyGroupAttr.isValid() || !pointGroupAttr.isValid())
    {
        std::cerr << "Procedural failed: invalid geometry'\n";
       return nullptr;
    }

    FnKat::FloatAttribute pointAttr = pointGroupAttr.getChildByName("P");
    if (!pointAttr.isValid())
    {
        std::cerr << "Procedural failed: invalid geometry'\n";
        return nullptr;
    }

    FnKat::IntAttribute polyStartIndexAttr =
        polyGroupAttr.getChildByName("startIndex");
    FnKat::IntAttribute vertexListAttr =
        polyGroupAttr.getChildByName("vertexList");
    if (!polyStartIndexAttr.isValid() || !vertexListAttr.isValid())
    {
        std::cerr << "Procedural failed: invalid geometry'\n";
        return nullptr;
    }

    const int numPoly = polyStartIndexAttr.getNumberOfTuples();
    const int numVert = vertexListAttr.getNumberOfTuples();
    std::vector<int> vertCount(numPoly, 0);
    std::vector<int> vertices(numVert, 0);

    const FnKat::IntConstVector polyStartIndexAttrValue =
        polyStartIndexAttr.getNearestSample(0.0f);
    unsigned int srcIndex = 0;

    int n_poly = numPoly;
    for (size_t i = 0; srcIndex < numPoly; ++i, ++srcIndex)
    {
        if (srcIndex + 1 < numPoly)
        {
            vertCount[i] = polyStartIndexAttrValue[srcIndex + 1] -
                              polyStartIndexAttrValue[srcIndex];
        }
        else
        {
            vertCount[i] = numVert - polyStartIndexAttrValue[srcIndex];
        }

        if (vertCount[i] <= 0)
        {
            --i;
            --n_poly;
        }
    }

    FnKat::IntConstVector::const_iterator vertexIt =
        vertexListAttr.getNearestSample(0).begin();
    FnKat::IntConstVector::const_iterator vertexItEnd =
        vertexListAttr.getNearestSample(0).end();
    for (int i = 0; vertexIt != vertexItEnd; ++vertexIt, ++i)
    {
        vertices[i] = *vertexIt;
    }

    // Allocate geometry for the procedural
    GU_Detail* gdp = allocateGeometry();

    const unsigned int numPoints = pointAttr.getNumberOfTuples();
    FnKat::FloatConstVector vec = pointAttr.getNearestSample(0.0f);
    const float* data = vec.data();

    for (unsigned int i = 0; i < numPoints; ++i)
    {
        const float* curr = data;

        GA_Offset ptoff = gdp->appendPointOffset();
        gdp->setPos3(ptoff, *curr, *(curr + 1), *(curr + 2));
        data += 3;
    }

    int fInd = 0;
    for (size_t i = 0; i < numPoly; ++i )
    {
        const int numPts = vertCount[i];
        GU_PrimPoly* poly = GU_PrimPoly::build(gdp, numPts, GU_POLY_CLOSED, 0);

        for (size_t j = 0; j < numPts; ++j)
        {
            GA_Offset ptoff = vertices[fInd];
            poly->setVertexPoint(j, ptoff);
            ++fInd;
        }
    }

    return gdp;
}

void ProceduralIterator::processTransform(FnKat::FnScenegraphIterator iterator)
{
    FnKat::GroupAttribute xformAttr =
        FnKat::RenderOutputUtils::getCollapsedXFormAttr(iterator);
    if (!xformAttr.isValid())
    {
        return;
    }

    bool isAbsolute;
    std::vector<float> relevantSampleTimes;
    relevantSampleTimes.push_back(0.0f);
    FnKat::RenderOutputUtils::XFormMatrixVector xforms;
    FnKat::RenderOutputUtils::calcXFormsFromAttr(
        xforms, isAbsolute, xformAttr, relevantSampleTimes,
        FnKat::RenderOutputUtils::kAttributeInterpolation_Linear);

    const double* m = xforms[0].getValues();
    UT_Matrix4D mat((double(*)[4])m);
    setPreTransform(mat, 0.0f);
}

// TODO: Avoid code duplication! See RendererPlugin/src/MantraRendererPlugin.cpp
template <typename T>
std::string ProceduralIterator::buildPropertyString(T attr) const
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

void ProceduralIterator::processMaterial(FnKat::FnScenegraphIterator iterator)
{
    FnKat::GroupAttribute matAttr = iterator.getAttribute("material", true);

    if (!matAttr.isValid())
    {
        return;
    }

    FnKat::StringAttribute matNameAttr = matAttr.getChildByName(
        "mantra13SurfaceShader");
    const std::string matName = matNameAttr.getValue("", false);

    if (!matName.empty())
    {
        std::ostringstream ss;
        ss << "opdef:/Shop/" << matName;

        // Build the shader's parameters string
        FnKat::GroupAttribute params = matAttr.getChildByName(
            "mantra13SurfaceParams");
        for (int i = 0; i < params.getNumberOfChildren(); ++i)
        {
            const std::string attrName = params.getChildName(i);
            FnKat::DataAttribute attr = params.getChildByIndex(i);

            if (attr.isValid())
            {
                std::string attrStr;
                switch (attr.getType())
                {
                    case kFnKatAttributeTypeInt:
                        attrStr =
                            buildPropertyString<FnKat::IntAttribute>(attr);
                        break;

                    case kFnKatAttributeTypeFloat:
                        attrStr =
                            buildPropertyString<FnKat::FloatAttribute>(attr);
                        break;

                    case kFnKatAttributeTypeDouble:
                        attrStr =
                            buildPropertyString<FnKat::DoubleAttribute>(attr);
                        break;

                    case kFnKatAttributeTypeString:
                        attrStr =
                            buildPropertyString<FnKat::StringAttribute>(attr);
                        break;

                    default:
                        std::cerr << "Warning: unknown attribute type for '"
                                  << attrName << "'\n";
                        continue;
                }
                ss << " " << attrName << " " << attrStr << "\n";
            }
        }
        changeSetting("surface", ss.str().c_str(), "object");
    }
}

} // namespace ds_mfk
