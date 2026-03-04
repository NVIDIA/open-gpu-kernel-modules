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
#include "nvidia-3d-types-priv.h"
#include "nvos.h"
#include "nvidia-3d-fermi.h"
#include "nvidia-3d-kepler.h"
#include "nvidia-push-utils.h"

NvBool nv3dAllocChannelObject(
    const Nv3dAllocChannelObjectParams *pParams,
    Nv3dChannelPtr p3dChannel)
{
    NvPushChannelPtr pPushChannel = pParams->pPushChannel;
    NvPushDevicePtr pPushDevice = pPushChannel->pDevice;
    const NvU32 classNumber = p3dChannel->p3dDevice->classNumber;
    const NvU32 numChannels =
        pPushDevice->clientSli ? pPushDevice->numSubDevices : 1;
    int sd;

    /*
     * nv3dAllocChannel() should have been called to assign p3dDevice.
     */
    nvAssert(p3dChannel->p3dDevice != NULL);
    nvAssert(p3dChannel->p3dDevice->pPushDevice ==
             pParams->pPushChannel->pDevice);

    for (sd = 0; sd < numChannels; sd++) {

        if (nvPushIsAModel(pPushDevice)) {
            nvAssert(sd == 0);
        } else {
            const NvPushImports *pImports = pPushDevice->pImports;
            nvAssert(pPushChannel->channelHandle[sd] != 0);
            nvAssert(pParams->handle[sd] != 0);
            NvU32 ret = pImports->rmApiAlloc(pPushDevice,
                                             pPushChannel->channelHandle[sd],
                                             pParams->handle[sd],
                                             classNumber,
                                             NULL);
            if (ret != NVOS_STATUS_SUCCESS) {
                return FALSE;
            }
        }

        p3dChannel->handle[sd] = pParams->handle[sd];
    }

    p3dChannel->pPushChannel = pPushChannel;

    return TRUE;
}

void nv3dFreeChannelObject(Nv3dChannelPtr p3dChannel)
{
    int sd;

    p3dChannel->pPushChannel = NULL;

    // No need to actually free the object here.  It gets destroyed during
    // channel teardown.
    for (sd = 0; sd < ARRAY_LEN(p3dChannel->handle); sd++) {
        p3dChannel->handle[sd] = 0;
    }
}

void nv3dUploadDataInline(
    Nv3dChannelRec *p3dChannel,
    NvU64 gpuBaseAddress,
    size_t offset,
    const void *data,
    size_t bytes)
{
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;

    pHal->uploadDataInline(p3dChannel, gpuBaseAddress, offset, data, bytes);
}

void nv3dClearProgramCache(Nv3dChannelRec *p3dChannel)
{
    Nv3dShaderStage stage;

    for (stage = 0;
         stage < ARRAY_LEN(p3dChannel->currentProgramIndex);
         stage++) {
        p3dChannel->currentProgramIndex[stage] = -1;
    }
}

void nv3dLoadTextures(
    Nv3dChannelRec *p3dChannel,
    int firstTex,
    const Nv3dRenderTexInfo *texInfo,
    int numTex)
{
    /* Limit number of texture/samplers on the stack to 4 (256 bytes) */
#define MAX_TEX_CHUNK 4
    Nv3dTexture textures[MAX_TEX_CHUNK];
    const Nv3dHal *pHal = p3dChannel->p3dDevice->hal;
    const NvU64 gpuBaseAddress = nv3dGetTextureGpuAddress(p3dChannel, 0);

    nvAssert(numTex >= 1);

    // Invalidate the texture/sampler caches.  This will cause a wait for idle
    // if there's rendering still in progress.  This is necessary in case the
    // texture parameters we're about to overwrite are in use.
    _nv3dInvalidateTexturesFermi(p3dChannel);

    while (numTex) {
        const NvU32 chunkNumTex = NV_MIN(numTex, MAX_TEX_CHUNK);
        const size_t startOffset = sizeof(Nv3dTexture) * firstTex;
        const size_t bytes = sizeof(Nv3dTexture) * chunkNumTex;
        int i;

        NVMISC_MEMSET(textures, 0, sizeof(textures));

        nvAssert(firstTex + numTex <= p3dChannel->numTextures);

        // Write texture header to HW format
        for (i = 0; i < chunkNumTex; i++) {
            pHal->assignNv3dTexture(texInfo[i], &textures[i]);
        }

        nv3dUploadDataInline(p3dChannel, gpuBaseAddress, startOffset,
                             textures, bytes);

        numTex -= chunkNumTex;
        firstTex += chunkNumTex;
        texInfo += chunkNumTex;
    }
}

void nv3dBindTextures(
    Nv3dChannelPtr p3dChannel,
    int programIndex,
    const int *textureBindingIndices)
{
    nvAssert(programIndex < p3dChannel->programs.num);

    _nv3dBindTexturesKepler(p3dChannel, programIndex, textureBindingIndices);
}
