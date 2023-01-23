/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-3d.h"
#include "nvidia-3d-vertex-arrays.h"
#include "nvidia-3d-types-priv.h"
#include "nvidia-3d-constant-buffers.h"
#include "nvidia-3d-utils.h"

#include <class/cl9097.h>
#include <class/cla06fsubch.h>

static void InitializeStreamFromSurf(
    const Nv3dStreamSurfaceRec *pSurf,
    Nv3dVertexAttributeStreamRec *pStream)
{
    pStream->current = pSurf->gpuAddress;
    pStream->end = pSurf->gpuAddress + pSurf->size;
    pStream->stride = 0;
    pStream->nextLaunch = 0;
}

static void InitializeStream(
    Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream,
    Nv3dVertexAttributeStreamRec *pStream)
{
    const Nv3dStreamSurfaceRec tmpSurf = {
        .gpuAddress =
            nv3dGetVertexAttributestreamGpuAddress(p3dChannel, stream),
        .size = NV3D_VERTEX_ATTRIBUTE_STREAM_SIZE,
    };
    InitializeStreamFromSurf(&tmpSurf, pStream);
}

void _nv3dInitializeStreams(
    Nv3dChannelRec *p3dChannel)
{
    enum Nv3dVertexAttributeStreamType stream;
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    // Disable vertex attribute vectors 16 through 31 (scalars 64 through 127).
    // We don't use them.
    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_B(0), 2);
    nvPushSetMethodData(p, ~0);
    nvPushSetMethodData(p, ~0);

    for (stream = NV3D_VERTEX_ATTRIBUTE_STREAM_FIRST;
         stream < NV3D_VERTEX_ATTRIBUTE_STREAM_COUNT;
         stream++) {

        Nv3dVertexAttributeStreamRec *pStream =
            &p3dChannel->vertexStreams[stream];

        InitializeStream(p3dChannel, stream, pStream);
    }
}

static void AdvanceStream(
    Nv3dVertexAttributeStreamRec *pStream)
{
    pStream->current += pStream->stride * pStream->nextLaunch;
    nvAssert(pStream->current <= pStream->end);
    pStream->nextLaunch = 0;
}

/*!
 * Configure a vertex attribute stream to fetch from a surface.
 *
 * \param[in]   p3dChannel  The channel
 * \param[in]   stream      The vertex attribute stream
 * \param[in]   pStream     The vertex attribute stream tracking structure
 */
static void
SetVertexStreamSurface(
    Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream,
    const Nv3dVertexAttributeStreamRec *pStream)
{
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_VERTEX_STREAM_A_FORMAT(stream), 3);
    nvPushSetMethodData(p,
        NV3D_V(9097, SET_VERTEX_STREAM_A_FORMAT, STRIDE, pStream->stride) |
        NV3D_C(9097, SET_VERTEX_STREAM_A_FORMAT, ENABLE, TRUE));
    nvPushSetMethodDataU64(p, pStream->current);

    pHal->setVertexStreamEnd(p3dChannel, stream, pStream);
}

/*!
 * Reset a vertex attribute stream to the specified offset, while leaving its
 * stride and limit alone.
 */
static void
SetVertexStreamOffset(
    Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream,
    NvU64 offset)
{
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    Nv3dVertexAttributeStreamRec *pStream = &p3dChannel->vertexStreams[stream];

    pStream->current = offset;
    pStream->nextLaunch = 0;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_VERTEX_STREAM_A_LOCATION_A(stream), 2);
    nvPushSetMethodDataU64(p, offset);

    pHal->setVertexStreamEnd(p3dChannel, stream, pStream);
}

/*!
 * Point the constant buffer selector at the next location for data in the
 * given stream.
 */
static void SelectCbForStream(
    Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream)
{
    Nv3dVertexAttributeStreamRec *pStream = &p3dChannel->vertexStreams[stream];
    const NvU64 gpuAddress =
        nv3dGetVertexAttributestreamGpuAddress(p3dChannel, stream);
    int startOffset = pStream->current + pStream->stride * pStream->nextLaunch -
                      gpuAddress;

    nv3dSelectCbAddress(p3dChannel, gpuAddress,
                        NV3D_VERTEX_ATTRIBUTE_STREAM_SIZE);
    nv3dSetConstantBufferOffset(p3dChannel, startOffset);
}

/*!
 * Configure the DA and VAF to fetch from vertex attribute streams.
 *
 * This function configures the Data Assembler (DA) and Vertex Attribute Fetch
 * (VAF) units to fetch vertex attributes from pSurf using a format configured
 * by the 'attribs' array.
 *
 * It configures two streams: NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC and
 * NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC.  The static stream contains attributes
 * that are the same across all vertices.  The dynamic stream contains
 * attributes that are different for each vertex.  The static stream sources
 * from the next available location in the static vertex data surface and uses a
 * stride of 0, so that all vertices in an array fetch the same values for those
 * attributes.  Then, it configures the dynamic stream to fetch starting at
 * offset 0 of pSurf, unless pSurf is NULL in which case it starts at the
 * appropriate offset in the dynamic vertex data surface.
 *
 * The 'attribs' array stores Nv3dVertexAttributeInfoRecs, terminated with an
 * element where attributeType is NV3D_VERTEX_ATTRIBUTE_END.  Each element
 * contains:
 *
 * (a) An enum Nv3dVertexAttributeType indicating which vertex attribute this
 * array element describes.
 *
 * (b) An enum Nv3dVertexAttributeDataType indicating the data type to use for
 * the attribute.
 *
 * (c) An enum Nv3dVertexAttributeStreamType indicating which stream should use
 * the attribute.
 *
 * If any attributes are enabled as static, this function selects the static
 * stream surface as the current constant buffer.  The caller should push the
 * appropriate vertex data.
 *
 * Note that if you launch rendering using vertex attributes from a surface, you
 * must wait for idle before changing those attributes later.  Otherwise, the
 * VAF unit may fetch the new data instead of the old data, causing corruption.
 *
 * \param[in]   p3dChannel  The 3d channel to program
 * \param[in]   attribs     Description of vertex attributes (see above)
 * \param[in]   pSurf       Surface that dynamic attributes will be fetched from
 *
 * \return      The size in bytes of the static attribute data
 */
int nv3dVasSetup(
    Nv3dChannelRec *p3dChannel,
    const Nv3dVertexAttributeInfoRec *attribs,
    const Nv3dStreamSurfaceRec *pSurf)
{
    /* This table is indexed by enum Nv3dVertexAttributeDataType. */
    static const struct {
        NvU32 size;
        NvU32 setVertexAttributeA;
    } attribTypeTable[] = {

        [NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_2_32_FLOAT] = {
            sizeof(float) * 2,
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A,
                    COMPONENT_BIT_WIDTHS, R32_G32) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, NUMERICAL_TYPE, NUM_FLOAT),
        },

        [NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_32_FLOAT] = {
            sizeof(float) * 4,
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A,
                    COMPONENT_BIT_WIDTHS, R32_G32_B32_A32) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, NUMERICAL_TYPE, NUM_FLOAT),
        },

        [NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_16_UNORM] = {
            sizeof(NvU16) * 4,
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A,
                    COMPONENT_BIT_WIDTHS, R16_G16_B16_A16) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, NUMERICAL_TYPE, NUM_UNORM),
        },

        [NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_8_UNORM] = {
            sizeof(NvU8) * 4,
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A,
                    COMPONENT_BIT_WIDTHS, A8B8G8R8) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, NUMERICAL_TYPE, NUM_UNORM),
        },

        [NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_2_16_SSCALED] = {
            sizeof(NvU32),
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A,
                    COMPONENT_BIT_WIDTHS, R16_G16) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, NUMERICAL_TYPE, NUM_SSCALED),
        },

    };

    NvPushChannelPtr p = p3dChannel->pPushChannel;
    Nv3dVertexAttributeStreamRec *pStatic =
        &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC];
    Nv3dVertexAttributeStreamRec *pDynamic =
        &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC];
    int staticOffset = 0, dynamicOffset = 0;
    Nv3dVertexAttributeStreamRec tmpStreamRec;
    NvU32 stride = 0;
    NvU64 daEnableMask = 0, daSkipMask;
    NvBool hasStaticAttribs = FALSE;
    NvBool hasPositionAttrib = FALSE;
    int i;

    // POSITION must be specified and must be a dynamic attribute.
    for (i = 0; attribs[i].attributeType != NV3D_VERTEX_ATTRIBUTE_END; i++) {
        if (attribs[i].attributeType != NV3D_VERTEX_ATTRIBUTE_POSITION) {
            continue;
        }
        hasPositionAttrib = TRUE;
        nvAssert(attribs[i].streamType == NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC);
    }
    if (!hasPositionAttrib) {
        nvAssert(!"POSITION vertex attribute not specified.");
    }

    // Configure the DA output skip mask so that it only fetches attributes for
    // enabled streams.
    for (i = 0; attribs[i].attributeType != NV3D_VERTEX_ATTRIBUTE_END; i++) {
        const enum Nv3dVertexAttributeType attrib = attribs[i].attributeType;
        // Always enable all four components of the value.  This causes the
        // DA to generate default values if there are not enough components
        // in the pulled vertex data.  This sets W=1 if W is missing.
        //
        // Otherwise, the value would come from the default the hardware
        // generates as input to the vertex shader when that attribute is
        // skipped in the DA, which is specified in the .mfs file as, "a
        // default value is inserted".
        //
        // Note all attribute values are expected to be less than 16 (i.e., fit
        // in MASK_A; attributes 16 through 31 would go in MASK_B).
        nvAssert(attrib < 16);
        daEnableMask |= 0xfULL << (4 * attrib);
    }
    daSkipMask = ~daEnableMask;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_DA_OUTPUT_ATTRIBUTE_SKIP_MASK_A(0), 2);
    nvPushSetMethodData(p, NvU64_LO32(daSkipMask));
    nvPushSetMethodData(p, NvU64_HI32(daSkipMask));

    // Configure the attributes to fetch from the streams.
    for (i = 0; attribs[i].attributeType != NV3D_VERTEX_ATTRIBUTE_END; i++) {

        const enum Nv3dVertexAttributeType attrib = attribs[i].attributeType;
        const enum Nv3dVertexAttributeDataType dataType = attribs[i].dataType;
        const enum Nv3dVertexAttributeStreamType stream = attribs[i].streamType;
        const NvU32 size = attribTypeTable[dataType].size;
        const NvU32 setVertexAttributeA =
            attribTypeTable[dataType].setVertexAttributeA;

        int offset;

        if (stream == NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC) {
            offset = staticOffset;
            staticOffset += size;
            hasStaticAttribs = TRUE;
        } else {
            nvAssert(stream == NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC);
            offset = dynamicOffset;
            dynamicOffset += size;
            stride += size;
        }

        nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
            NV9097_SET_VERTEX_ATTRIBUTE_A(attrib), 1);
        nvPushSetMethodData(p,
            NV3D_V(9097, SET_VERTEX_ATTRIBUTE_A, STREAM, stream) |
            NV3D_C(9097, SET_VERTEX_ATTRIBUTE_A, SOURCE, ACTIVE) |
            NV3D_V(9097, SET_VERTEX_ATTRIBUTE_A, OFFSET, offset) |
            setVertexAttributeA);
    }


    // Advance the stream past any attribs used previously.
    AdvanceStream(pStatic);
    // Although we may have set a non-zero stride on a previous call to this
    // function (mostly so the bookkeeping above works out), as far as the GPU
    // is concerned we should program a stride of 0.
    pStatic->stride = 0;

    // See if we need to wrap the static stream.
    if (pStatic->current + staticOffset >= pStatic->end) {
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_WAIT_FOR_IDLE, 0);

        // Reset both the static and dynamic streams, since we know the GPU is
        // done reading from both.
        InitializeStream(p3dChannel,
                         NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC, pStatic);
        InitializeStream(p3dChannel,
                         NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC, pDynamic);
    } else if (!pSurf) {
        // Advance the dynamic stream past any attribs used previously (unless
        // we just reset the stream).
        AdvanceStream(pDynamic);
    }

    /* override dynamic stream with pSurf */
    if (pSurf) {
        pDynamic = &tmpStreamRec;
        InitializeStreamFromSurf(pSurf, pDynamic);
    }

    // Configure the streams.  A stride of 0 makes it read the same attribute
    // each time.
    nvAssert(pStatic->stride == 0);
    SetVertexStreamSurface(p3dChannel,
                           NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC,
                           pStatic);
    nvAssert(stride != 0);
    pDynamic->stride = stride;
    SetVertexStreamSurface(p3dChannel,
                           NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC,
                           pDynamic);

    // If there are static attributes, set up the constant buffer selector.
    if (hasStaticAttribs) {
        SelectCbForStream(p3dChannel, NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC);

        // Override the static stream's "stride" so that the next time this
        // function is called it will set staticStartOffset to right after the
        // static data here.
        pStatic->stride = staticOffset;
        pStatic->nextLaunch = 1;
    }

    return staticOffset;
}

/*!
 * Check if uploading the specified number of vertices will write past the end
 * of the given vertex stream.
 */
static NvBool WillVertexDataWrap(
    Nv3dVertexAttributeStreamRec *pStream,
    int n)
{
    // >= here is intentional: It's illegal to set the constant buffer selector
    // past the end of the constant buffer, which could happen if the last
    // primitive drawn exactly fills the dynamic data stream and another
    // primitive is drawn.  Then the next call to nv3dVasSelectCbForVertexData()
    // would cause a channel error.
    //
    // Instead of trying to detect that case there, just disallow completely
    // filling the stream so it wraps slightly earlier.
    return pStream->current + pStream->stride * (pStream->nextLaunch + n) >=
           pStream->end;
}

/*!
 * Launch vertices and update tracked vertex array state.
 */
static void DrawVertexArray(Nv3dChannelRec *p3dChannel, int numVerts)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    Nv3dVertexAttributeStreamRec *pDynamic =
        &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC];

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_VERTEX_ARRAY_START, 2);
    nvPushSetMethodData(p, pDynamic->nextLaunch);
    nvPushSetMethodData(p, numVerts); // NV9097_DRAW_VERTEX_ARRAY

    pDynamic->nextLaunch += numVerts;
}

/*!
 * Reset both the static and dynamic vertex array streams to the base of the
 * corresponding surfaces.
 */
static void WrapVertexStreams(Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    Nv3dVertexAttributeStreamRec *pStatic =
        &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC];
    const NvU64 gpuAddress =
        nv3dGetVertexAttributestreamGpuAddress(p3dChannel,
            NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC);
    const NvU32 primMode = p3dChannel->currentPrimitiveMode;

    // Set the software tracking for the static stream so it starts over at the
    // beginning next time nv3dVasSetup() is called, but leave the hardware
    // configured to read the data that's already there, in case vertices
    // submitted later still need it.
    pStatic->current = pStatic->end;
    pStatic->nextLaunch = 0;

    // The hardware can't handle changing the vertex stream offset inside a
    // BEGIN / END block, so temporarily end now.
    nv3dVasEnd(p3dChannel);

    // Wrap the dynamic vertex stream.
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_WAIT_FOR_IDLE, 0);
    SetVertexStreamOffset(p3dChannel, NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC,
                          gpuAddress);

    nv3dVasBegin(p3dChannel, primMode);
}

/*!
 * Point the constant buffer selector at the next location for vertex data in
 * the dynamic data surface.
 */
void nv3dVasSelectCbForVertexData(Nv3dChannelRec *p3dChannel)
{
    SelectCbForStream(p3dChannel, NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC);
}

/*!
 * Upload and draw vertices using the dynamic vertex data surface
 *
 * This function uploads data to the dynamic vertex attribute stream surface
 * using inline constant buffer updates starting at the next free space in that
 * surface, and then launches rendering.  The number of vertices rendered is
 * specified by 'numVerts'.
 *
 * Static data should have already been written to the static vertex attribute
 * stream surface by the caller.
 *
 * If not enough space is available in the dynamic data surface, this function
 * waits for idle before wrapping to the beginning of the surface to avoid
 * conflicting with earlier rendering that might be in flight.
 *
 * It is up to the caller to send BEGIN and END methods around calls to this
 * function.
 *
 * \param[in]   p3dChannel  The channel
 * \param[in]   data        Data to upload
 * \param[in]   numVerts    Number of vertices rendered
 */
void nv3dVasDrawInlineVerts(
    Nv3dChannelRec *p3dChannel,
    const void *data,
    int numVerts)
{
    if (data != NULL) {
        Nv3dVertexAttributeStreamRec *pDynamic =
            &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC];

        // See if we need to wrap the dynamic stream.
        if (WillVertexDataWrap(pDynamic, numVerts)) {
            WrapVertexStreams(p3dChannel);
        }

        nv3dVasSelectCbForVertexData(p3dChannel);
        nv3dPushConstants(p3dChannel, pDynamic->stride * numVerts, data);
    }

    DrawVertexArray(p3dChannel, numVerts);
}

NvBool nv3dVasMakeRoom(
    Nv3dChannelRec *p3dChannel,
    NvU32 pendingVerts,
    NvU32 moreVerts)
{
    Nv3dVertexAttributeStreamRec *pDynamic =
        &p3dChannel->vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC];

    const NvBool wrap = WillVertexDataWrap(pDynamic, pendingVerts + moreVerts);

    // If pendingVerts + moreVerts would exceed the dynamic vertex array buffer,
    // flush it now and start over at the beginning.
    if (wrap) {
        DrawVertexArray(p3dChannel, pendingVerts);
        WrapVertexStreams(p3dChannel);

        // Reset the constant buffer update pointer to the beginning of the
        // dynamic vertex data buffer.
        nv3dSetConstantBufferOffset(p3dChannel, 0);
    }

    return wrap;
}

