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

/*
 * The nvidia-3d library provides utility code for programming a 3D
 * object.
 */

#ifndef __NVIDIA_3D_H__
#define __NVIDIA_3D_H__

#include "nvtypes.h"
#include "nvmisc.h" /* DRF_DEF, et al */
#include "nvlimits.h" /* NV_MAX_SUBDEVICES */

#include "nvidia-push-types.h"
#include "nvidia-3d-types.h"

#define NV3D_C(d, r, f, c) DRF_DEF( d, _ ## r, _ ## f, _ ## c)
#define NV3D_V(d, r, f, v) DRF_NUM( d, _ ## r, _ ## f, (NvU32)(v) )

/*
 * Allocate and free an Nv3dDeviceRec
 */
typedef struct _Nv3dAllocDeviceParams {
    NvPushDevicePtr pPushDevice;
} Nv3dAllocDeviceParams;

NvBool nv3dAllocDevice(
    const Nv3dAllocDeviceParams *pParams,
    Nv3dDevicePtr p3dDevice);

void nv3dFreeDevice(
    Nv3dDevicePtr p3dDevice);

/*
 * Allocate and free an Nv3dChannelRec data structure.
 *
 * Note that all pointers provided in this parameter structure are
 * cached in the Nv3dChannelRec.  They must remain valid from
 * nv3dAllocChannelState() until the corresponding
 * nv3dFreeChannelState() call.
 */
typedef struct _Nv3dAllocChannelStateParams {
    /*
     * The Nv3dDeviceRec to use with this channel.
     */
    Nv3dDevicePtr p3dDevice;

    /*
     * The number of texture headers/samplers nvidia-3d should
     * allocate.
     */
    NvU16 numTextures;

    /*
     * The number of general purpose constant buffers nvidia-3d should
     * allocate.
     */
    NvU16 numConstantBuffers;

    /*
     * The number of texture bindings.
     */
    NvU16 numTextureBindings;

    /*
     * Whether the host driver renders in terms of frames, or, like the X
     * driver, renders directly to the front buffer.  On >= Pascal, the pipe
     * needs to be explicitly flushed at the end of a frame.
     */
    NvBool hasFrameBoundaries;

} Nv3dAllocChannelStateParams;

NvBool nv3dAllocChannelState(
    const Nv3dAllocChannelStateParams *pParams,
    Nv3dChannelPtr p3dChannel);

void nv3dFreeChannelState(
    Nv3dChannelPtr p3dChannel);


/*
 * Allocate and free the RM object for an Nv3dChannelRec.
 */
typedef struct _Nv3dAllocChannelObjectParams {
    NvPushChannelPtr pPushChannel;
    NvU32 handle[NV_MAX_SUBDEVICES];
} Nv3dAllocChannelObjectParams;

NvBool nv3dAllocChannelObject(
    const Nv3dAllocChannelObjectParams *pParams,
    Nv3dChannelPtr p3dChannel);

void nv3dFreeChannelObject(
    Nv3dChannelPtr p3dChannel);


/*
 * Allocate and free the surface needed by the Nv3dChannelRec.
 */
NvBool nv3dAllocChannelSurface(Nv3dChannelPtr p3dChannel);

void nv3dFreeChannelSurface(Nv3dChannelPtr p3dChannel);


/*
 * Once the Nv3dChannelRec is allocated, and the objects and surface
 * for it are allocated, nv3dInitChannel() is used to initialize the
 * graphics engine and make it ready to use.
 */
NvBool nv3dInitChannel(Nv3dChannelPtr p3dChannel);


/*
 * Return the offset or GPU address of the specified item within the
 * Nv3dChannelRec's surface.
 */

static inline NvU64 nv3dGetTextureOffset(
    const Nv3dChannelRec *p3dChannel,
    NvU32 textureIndex)
{
    const NvU64 offset = p3dChannel->surface.textureOffset;

    return offset + (sizeof(Nv3dTexture) * textureIndex);
}

static inline NvU64 nv3dGetTextureGpuAddress(
    const Nv3dChannelRec *p3dChannel,
    NvU32 textureIndex)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetTextureOffset(p3dChannel, textureIndex);
}

static inline NvU64 nv3dGetConstantBufferOffset(
    const Nv3dChannelRec *p3dChannel,
    NvU32 constantBufferIndex)
{
    const NvU64 offset = p3dChannel->surface.constantBufferOffset;

    return offset + (NV3D_CONSTANT_BUFFER_SIZE * constantBufferIndex);
}

static inline NvU64 nv3dGetConstantBufferGpuAddress(
    const Nv3dChannelRec *p3dChannel,
    NvU32 constantBufferIndex)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetConstantBufferOffset(p3dChannel, constantBufferIndex);
}

static inline NvU64 nv3dGetProgramOffset(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.programOffset;
}

static inline NvU64 nv3dGetProgramGpuAddress(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.gpuAddress + nv3dGetProgramOffset(p3dChannel);
}

static inline NvU64 nv3dGetProgramConstantsOffset(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.programConstantsOffset;
}

static inline NvU64 nv3dGetProgramConstantsGpuAddress(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetProgramConstantsOffset(p3dChannel);
}

static inline NvU64 nv3dGetProgramLocalMemoryOffset(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.programLocalMemoryOffset;
}

static inline NvU64 nv3dGetProgramLocalMemoryGpuAddress(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetProgramLocalMemoryOffset(p3dChannel);
}

static inline NvU64 nv3dGetBindlessTextureConstantBufferOffset(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.bindlessTextureConstantBufferOffset;
}

static inline NvU64 nv3dGetBindlessTextureConstantBufferGpuAddress(
    const Nv3dChannelRec *p3dChannel)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetBindlessTextureConstantBufferOffset(p3dChannel);
}

static inline NvU64 nv3dGetVertexAttributestreamOffset(
    const Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream)
{
    return p3dChannel->surface.vertexStreamOffset[stream];
}

static inline NvU64 nv3dGetVertexAttributestreamGpuAddress(
    const Nv3dChannelRec *p3dChannel,
    enum Nv3dVertexAttributeStreamType stream)
{
    return p3dChannel->surface.gpuAddress +
        nv3dGetVertexAttributestreamOffset(p3dChannel, stream);
}

void nv3dUploadDataInline(
    Nv3dChannelRec *p3dChannel,
    NvU64 gpuBaseAddress,
    size_t offset,
    const void *data,
    size_t bytes);

void nv3dClearProgramCache(
    Nv3dChannelRec *p3dChannel);

void nv3dLoadProgram(
    Nv3dChannelRec *p3dChannel,
    int programIndex);

void nv3dLoadTextures(
    Nv3dChannelRec *p3dChannel,
    int firstTextureIndex,
    const Nv3dRenderTexInfo *texInfo,
    int numTexures);

void nv3dBindTextures(
    Nv3dChannelPtr p3dChannel,
    int programIndex,
    const int *textureBindingIndices);

void nv3dSetBlendColorCoefficients(
    Nv3dChannelPtr p3dChannel,
    enum Nv3dBlendOperation op,
    NvBool forceNoDstAlphaBits,
    NvBool dualSourceBlending);

void nv3dSetBlend(
    Nv3dChannelPtr p3dChannel,
    const Nv3dBlendState *blendStateColor,
    const Nv3dBlendState *blendStateAlpha,
    const Nv3dColor *blendColor);

int nv3dVasSetup(
    Nv3dChannelRec *p3dChannel,
    const Nv3dVertexAttributeInfoRec *attribs,
    const Nv3dStreamSurfaceRec *pSurf);

void nv3dVasSelectCbForVertexData(
    Nv3dChannelRec *p3dChannel);

void nv3dVasDrawInlineVerts(
    Nv3dChannelRec *p3dChannel,
    const void *data,
    int numVerts);

NvBool nv3dVasMakeRoom(
    Nv3dChannelRec *p3dChannel,
    NvU32 pendingVerts,
    NvU32 moreVerts);

#endif /* __NVIDIA_3D_H__ */
