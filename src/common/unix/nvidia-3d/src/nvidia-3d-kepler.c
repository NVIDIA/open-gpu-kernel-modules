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

#include "nvidia-3d-kepler.h"
#include "nvidia-3d.h"
#include "nvidia-3d-constant-buffers.h"
#include "nvidia-3d-shader-constants.h"

#include <class/cla097.h>
#include <class/cla06fsubch.h>

void _nv3dSetSpaVersionKepler(Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr pPushChannel = p3dChannel->pPushChannel;
    const NvU16 major = p3dChannel->p3dDevice->spaVersion.major;
    const NvU16 minor = p3dChannel->p3dDevice->spaVersion.minor;

    // Tell AModel or fmodel what shader model version to use.  This has no
    // effect on real hardware.  The SM version (the "hardware revision" of the
    // SM block) does not always match the SPA version (the ISA version).
    nvPushMethod(pPushChannel, NVA06F_SUBCHANNEL_3D,
        NVA097_SET_SPA_VERSION, 1);
    nvPushSetMethodData(pPushChannel,
        NV3D_V(A097, SET_SPA_VERSION, MAJOR, major) |
        NV3D_V(A097, SET_SPA_VERSION, MINOR, minor));
}

void _nv3dInitChannelKepler(Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    // Configure constant buffer slot NV3D_CB_SLOT_BINDLESS_TEXTURE as the
    // place the texture binding table is stored. This is obsolete on Volta and
    // later, so don't run it there.
    if (p3dChannel->p3dDevice->caps.hasSetBindlessTexture) {
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
            NVA097_SET_BINDLESS_TEXTURE,
            NV3D_V(A097, SET_BINDLESS_TEXTURE, CONSTANT_BUFFER_SLOT_SELECT,
            NV3D_CB_SLOT_BINDLESS_TEXTURE));
    }

    // Disable shader exceptions.  This matches OpenGL driver behavior.
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_SHADER_EXCEPTIONS,
        NV3D_C(9097, SET_SHADER_EXCEPTIONS, ENABLE, FALSE));
}

/*!
 * Upload data using the INLINE_TO_MEMORY methods embedded in the KEPLER_A
 * class.
 *
 * The number of dwords pushed inline is limited by nvPushMaxMethodCount().
 * Push the data in multiple chunks, if necessary.
 */
void _nv3dUploadDataInlineKepler(
    Nv3dChannelRec *p3dChannel,
    NvU64 gpuBaseAddress,
    size_t offset,
    const void *data,
    size_t bytes)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    /*
     * Below we use '1 + dwordsThisChunk' as the method count, so subtract one
     * when computing chunkSizeDwords.
     */
    const NvU32 chunkSizeDwords = nvPushMaxMethodCount(p) - 1;
    const NvU32 chunkSize = chunkSizeDwords * 4; /* in bytes */
    size_t bytesSoFar;

    // Only allow uploading complete dwords.
    nvAssert((bytes & 3) == 0);

    for (bytesSoFar = 0; bytesSoFar < bytes; bytesSoFar += chunkSize) {

        const NvU32 bytesThisChunk = NV_MIN(bytes - bytesSoFar, chunkSize);
        const NvU32 dwordsThisChunk = bytesThisChunk / 4;

        nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NVA097_LINE_LENGTH_IN, 5);
        nvPushSetMethodData(p, bytesThisChunk);
        nvPushSetMethodData(p, 1); // NVA097_LINE_COUNT
        nvPushSetMethodDataU64(p, gpuBaseAddress + offset + bytesSoFar);
        nvPushSetMethodData(p, bytesThisChunk); // NVA097_PITCH_OUT

        nvPushMethodOneIncr(p, NVA06F_SUBCHANNEL_3D, NVA097_LAUNCH_DMA,
                            1 + dwordsThisChunk);
        nvPushSetMethodData(p,
            NV3D_C(A097, LAUNCH_DMA, DST_MEMORY_LAYOUT, PITCH) |
            // Disable flush -- As long as only 3D requires the data uploaded,
            // we don't need to incur the performance penalty of a sys-membar.
            NV3D_C(A097, LAUNCH_DMA, COMPLETION_TYPE, FLUSH_DISABLE) |
            NV3D_C(A097, LAUNCH_DMA, INTERRUPT_TYPE, NONE) |
            NV3D_C(A097, LAUNCH_DMA, SYSMEMBAR_DISABLE, TRUE));
        nvPushInlineData(p, (const NvU8 *)data + bytesSoFar, dwordsThisChunk);
    }
}

void _nv3dBindTexturesKepler(
    Nv3dChannelPtr p3dChannel,
    int programIndex,
    const int *textureBindingIndices)
{
    const NvU16 numTextureBindings = p3dChannel->numTextureBindings;
    NvPushChannelUnion *remappedBinding = NULL;
    NvU8 slot;
    const NvU64 gpuAddress =
        nv3dGetBindlessTextureConstantBufferGpuAddress(p3dChannel);

    nv3dSelectCbAddress(p3dChannel, gpuAddress, NV3D_CONSTANT_BUFFER_SIZE);
    nv3dBindCb(p3dChannel, NV3D_HW_BIND_GROUP_FRAGMENT,
               NV3D_CB_SLOT_BINDLESS_TEXTURE, TRUE);
    /*
     * Set up the header in the pushbuffer for the LOAD_CONSTANTS method.  The
     * below loop will write the data to upload directly into the pushbuffer.
     */
    remappedBinding = nv3dLoadConstantsHeader(p3dChannel, 0,
                                              numTextureBindings);

    for (slot = 0; slot < numTextureBindings; slot++) {
        int tex = textureBindingIndices[slot];

        /*
         * Bindless texture packed pointers.  Technically, these consist of
         * a header at bits 19:0 and a sampler in 32:20, but we don't need
         * to set a separate header because we enabled
         * SET_SAMPLER_BINDING_VIA_HEADER_BINDING.
         */
        remappedBinding[slot].u = tex * 2;
    }
}
