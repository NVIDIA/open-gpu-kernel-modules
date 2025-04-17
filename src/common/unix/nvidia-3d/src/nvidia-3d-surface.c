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
#include "nvidia-3d-surface.h"
#include "nvidia-push-utils.h" /* nvPushIsAmodel() */

#include <nvos.h>

static void FreeSurface(
    Nv3dChannelRec *p3dChannel)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    int sd;

    for (sd = ARRAY_LEN(pPushDevice->subDevice) - 1;
         sd >= 0;
         sd--) {
        if (p3dChannel->surface.handle[sd]) {
            NvU32 ret = pPushDevice->pImports->rmApiFree(
                     pPushDevice,
                     pPushDevice->subDevice[sd].deviceHandle,
                     p3dChannel->surface.handle[sd]);
            nvAssert(ret == NVOS_STATUS_SUCCESS);
            (void)ret;
            p3dChannel->surface.handle[sd] = 0;
        }
    }
}

static NvBool AllocSurface(
    Nv3dChannelRec *p3dChannel,
    NvU64 size)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    int sd;

    for (sd = 0;
         sd < ARRAY_LEN(pPushDevice->subDevice) &&
            pPushDevice->subDevice[sd].deviceHandle != 0;
         sd++) {

        NVOS32_PARAMETERS params = {
            .hRoot = pPushDevice->clientHandle,
            .hObjectParent = pPushDevice->subDevice[sd].deviceHandle,
            .function = NVOS32_FUNCTION_ALLOC_SIZE,
            .data.AllocSize.owner = pPushDevice->clientHandle,
            .data.AllocSize.type = NVOS32_TYPE_SHADER_PROGRAM,
            .data.AllocSize.size = size,
            .data.AllocSize.attr =
                (pPushDevice->hasFb ?
                    DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) :
                    DRF_DEF(OS32, _ATTR, _LOCATION, _PCI)) |
                DRF_DEF(OS32, _ATTR, _PHYSICALITY, _ALLOW_NONCONTIGUOUS) |
                DRF_DEF(OS32, _ATTR, _COHERENCY, _WRITE_COMBINE),
            .data.AllocSize.attr2 =
                DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _YES),
            .data.AllocSize.flags = 0,
            .data.AllocSize.alignment = 4096,
        };

        NvU32 ret = pImports->rmApiVidHeapControl(pPushDevice, &params);

        if (ret != NVOS_STATUS_SUCCESS) {
            FreeSurface(p3dChannel);
            return FALSE;
        }

        p3dChannel->surface.handle[sd] = params.data.AllocSize.hMemory;
    }

    return TRUE;
}

static void UnmapSurface(
    const Nv3dChannelRec *p3dChannel,
    NvU64 gpuAddress)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    int sd;

    for (sd = ARRAY_LEN(p3dChannel->surface.handle) - 1; sd >= 0; sd--) {
        if (p3dChannel->surface.handle[sd]) {
            NvU32 ret = pImports->rmApiUnmapMemoryDma(
                            pPushDevice,
                            pPushDevice->subDevice[sd].deviceHandle,
                            pPushDevice->subDevice[sd].gpuVASpaceCtxDma,
                            p3dChannel->surface.handle[sd],
                            0,
                            gpuAddress);
            nvAssert(ret == NVOS_STATUS_SUCCESS);
            (void)ret;
        }
    }
}

static NvU64 MapSurface(
    const Nv3dChannelRec *p3dChannel,
    NvU64 size)
{
    NvPushDevicePtr pPushDevice = p3dChannel->p3dDevice->pPushDevice;
    const NvPushImports *pImports = pPushDevice->pImports;
    NvU64 gpuAddress = 0;
    int sd;

    for (sd = 0;
         sd < ARRAY_LEN(p3dChannel->surface.handle) &&
            p3dChannel->surface.handle[sd] != 0;
         sd++) {
        NvU32 flags = DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);
        NvU64 thisGpuAddress;

        if (sd == 0) {
            /* For the first device, RM assigns a virtual address. */
            thisGpuAddress = 0;
        } else {
            /* For subsequent devices, use the same virtual address. */
            flags = FLD_SET_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags);
            nvAssert(gpuAddress != 0);
            thisGpuAddress = gpuAddress;
        }

        NvU32 ret = pImports->rmApiMapMemoryDma(pPushDevice,
                                                pPushDevice->subDevice[sd].deviceHandle,
                                                pPushDevice->subDevice[sd].gpuVASpaceCtxDma,
                                                p3dChannel->surface.handle[sd],
                                                0,
                                                size,
                                                flags,
                                                &thisGpuAddress);
        if (ret != NVOS_STATUS_SUCCESS) {
            if (sd != 0) {
                /* Clean up earlier successful mappings */
                UnmapSurface(p3dChannel, gpuAddress);
            }
            return 0;
        }

        if (sd == 0) {
            gpuAddress = thisGpuAddress;
        } else {
            nvAssert(gpuAddress == thisGpuAddress);
        }
    }

    return gpuAddress;
}

NvBool nv3dAllocChannelSurface(Nv3dChannelPtr p3dChannel)
{
    const NvU64 size = p3dChannel->surface.totalSize;
    NvU64 gpuAddress;

    if (!AllocSurface(p3dChannel, size)) {
        return FALSE;
    }

    gpuAddress = MapSurface(p3dChannel, size);

    if (gpuAddress == 0) {
        FreeSurface(p3dChannel);
        return FALSE;
    }

    p3dChannel->surface.gpuAddress = gpuAddress;

    return TRUE;
}

void nv3dFreeChannelSurface(Nv3dChannelPtr p3dChannel)
{
    if (p3dChannel->p3dDevice == NULL) {
        return;
    }

    if (p3dChannel->surface.gpuAddress != 0) {
        /*
         * If the surface is mapped into our channel, we need to ensure
         * that any methods in the channel that might reference the
         * gpuAddress have idled before we unmap the address.
         */
        nvPushIdleChannel(p3dChannel->pPushChannel);

        UnmapSurface(p3dChannel,
                     p3dChannel->surface.gpuAddress);
        p3dChannel->surface.gpuAddress = 0;
    }

    FreeSurface(p3dChannel);
}

/*
 * The Nv3dChannelRec's surface contains:
 *
 *   programLocalMemory
 *   programCode
 *   programConstants
 *   Nv3dTexture[numTextures]
 *   bindlessTextureConstantBuffer (optionally)
 *   Nv3dConstantBuffer[numConstantBuffers]
 *   vertexStreams
 *
 * Where all items are aligned to NV3D_TEXTURE_PITCH_ALIGNMENT.
 *
 * Compute all the offsets into the surface, and the total surface
 * size.
 *
 * XXX TODO: use correct alignment for all items, rather than
 * NV3D_TEXTURE_PITCH_ALIGNMENT.
 */
void _nv3dAssignSurfaceOffsets(
    const Nv3dAllocChannelStateParams *pParams,
    Nv3dChannelPtr p3dChannel)
{
    const NvU32 programPrefetchPadding = 2048;

    NvU64 offset = 0;
    enum Nv3dVertexAttributeStreamType stream;

    /*
     * Program local memory requires at least 4k alignment.  So, place
     * it at the start of the surface.
     */
    p3dChannel->surface.programLocalMemoryOffset = offset;

    offset += p3dChannel->programLocalMemorySize;
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    p3dChannel->surface.programOffset = offset;

    offset += p3dChannel->programs.code.decompressedSize;
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    p3dChannel->surface.programConstantsOffset = offset;

    offset += p3dChannel->programs.constants.size;
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    p3dChannel->surface.textureOffset = offset;

    offset += (sizeof(Nv3dTexture) * pParams->numTextures);
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    p3dChannel->surface.bindlessTextureConstantBufferOffset = offset;
    offset += NV3D_CONSTANT_BUFFER_SIZE;
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    p3dChannel->surface.constantBufferOffset = offset;

    offset += (NV3D_CONSTANT_BUFFER_SIZE * pParams->numConstantBuffers);
    offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);

    /*
     * TODO: not all nvidia-3d host drivers will require the vertex stream
     * memory; maybe host drivers should opt in?
     */
    for (stream = NV3D_VERTEX_ATTRIBUTE_STREAM_FIRST;
         stream < NV3D_VERTEX_ATTRIBUTE_STREAM_COUNT;
         stream++) {

        p3dChannel->surface.vertexStreamOffset[stream] = offset;

        offset += NV3D_VERTEX_ATTRIBUTE_STREAM_SIZE;
        offset = NV_ALIGN_UP(offset, NV3D_TEXTURE_PITCH_ALIGNMENT);
    }

    /*
     * Make sure the total surface size is large enough to cover any
     * potential prefetch region.
     */
    p3dChannel->surface.totalSize =
        NV_MAX(p3dChannel->surface.programOffset + programPrefetchPadding,
               offset);
}
