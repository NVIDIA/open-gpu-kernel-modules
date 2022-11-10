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

#ifndef __NVIDIA_3D_CONSTANT_BUFFERS_H__
#define __NVIDIA_3D_CONSTANT_BUFFERS_H__

#include "nvidia-3d.h"

#include <class/cl9097.h>
#include <class/cla06fsubch.h>

/*
 * This header file defines static inline functions to manage 3D class
 * constant buffers.
 */


static inline void nv3dSelectCbAddress(
    Nv3dChannelRec *p3dChannel,
    NvU64 offset,
    NvU32 size)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvAssert(size > 0);
    nvAssert(NV_IS_ALIGNED(size, NV3D_MIN_CONSTBUF_ALIGNMENT));
    nvAssert(size <= 65536);
    nvAssert(NV_IS_ALIGNED(offset, NV3D_MIN_CONSTBUF_ALIGNMENT));

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_CONSTANT_BUFFER_SELECTOR_A, 3);
    nvPushSetMethodData(p, size);
    nvPushSetMethodDataU64(p, offset);
}

/*!
 * Select a constant buffer for binding or updating.
 */
static inline void nv3dSelectCb(
    Nv3dChannelRec *p3dChannel,
    int constantBufferIndex)
{
    const NvU64 gpuAddress =
        nv3dGetConstantBufferGpuAddress(p3dChannel, constantBufferIndex);

    nv3dSelectCbAddress(p3dChannel, gpuAddress, NV3D_CONSTANT_BUFFER_SIZE);
}

/*!
 * Bind the selected Cb to a given slot (or invalidate that slot).
 */
static inline void nv3dBindCb(
    Nv3dChannelRec *p3dChannel,
    int bindGroup, // XXX TODO: this type should be NVShaderBindGroup
    int slot,
    NvBool valid)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    ASSERT_DRF_NUM(9097, _BIND_GROUP_CONSTANT_BUFFER, _SHADER_SLOT, slot);

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
        NV9097_BIND_GROUP_CONSTANT_BUFFER(bindGroup),
        NV3D_V(9097, BIND_GROUP_CONSTANT_BUFFER, VALID, !!valid) |
        NV3D_V(9097, BIND_GROUP_CONSTANT_BUFFER, SHADER_SLOT, slot));
}

/*!
 * Push *only the header* to tell the GPU to "load" constants from the
 * pushbuffer.
 *
 * \param[in]       p3dChannel          The nvidia-3d channel.
 * \param[in]       offset              The offset in bytes of the start of the
 *                                      updates.
 * \param[in]       dwords              Count of dwords to be loaded (after the
 *                                      header).
 *
 * \return          An NvPushChannelUnion pointing immediately after the
 *                  header, with enough contiguous space to copy 'dwords' of
 *                  data.
 */
static inline NvPushChannelUnion *nv3dLoadConstantsHeader(
    Nv3dChannelRec *p3dChannel,
    NvU32 offset,
    size_t dwords)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    NvPushChannelUnion *buffer;

    nvAssert((dwords + 1) <= nvPushMaxMethodCount(p));

    nvPushMethodOneIncr(p, NVA06F_SUBCHANNEL_3D,
        NV9097_LOAD_CONSTANT_BUFFER_OFFSET, dwords + 1);
    nvPushSetMethodData(p, offset);

    buffer = p->main.buffer;
    p->main.buffer += dwords;

    return buffer;
}

/*!
 * Load an array of bytes into a constant buffer at a specified location.
 *
 * The count must be a multiple of 4 bytes.
 *
 * \param[in]       p3dChannel          The nvidia-3d channel.
 * \param[in]       offset              The offset in bytes of the start of the
 *                                      updates.
 * \param[in]       bytes               Count of bytes to write.  Must be a
 *                                      multiple of 4.
 * \param[in]       values              Data to be written.
 */
static inline void nv3dLoadConstants(
    Nv3dChannelRec *p3dChannel,
    NvU32 offset,
    size_t bytes,
    const void *values)
{
    const size_t dwords = bytes / 4;
    NvPushChannelUnion *buffer;

    nvAssert((bytes & 3) == 0);

    buffer = nv3dLoadConstantsHeader(p3dChannel, offset, dwords);

    nvDmaMoveDWORDS(buffer, values, dwords);
}

/*!
 * Set the current constant buffer's current byte offset, for use with
 * nv3dPushConstants().
 */
static inline void nv3dSetConstantBufferOffset(
    Nv3dChannelRec *p3dChannel,
    NvU32 offset)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_LOAD_CONSTANT_BUFFER_OFFSET, 1);
    nvPushSetMethodData(p, offset);
}

// Load an array of dwords into a constant buffer at the current location.  This
// also advances the constant buffer load offset, so that multiple calls to
// nv3dPushConstants will write to sequential memory addresses.
static inline void nv3dPushConstants(
    Nv3dChannelRec *p3dChannel,
    size_t bytes,
    const void *values)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;
    const size_t dwords = bytes / 4;
    nvAssert((bytes & 3) == 0);
    nvAssert(dwords <= nvPushMaxMethodCount(p));

    nvPushMethodNoIncr(p, NVA06F_SUBCHANNEL_3D,
        NV9097_LOAD_CONSTANT_BUFFER(0), dwords);
    nvPushInlineData(p, values, dwords);
}

static inline void nv3dLoadSingleConstant(
    Nv3dChannelRec *p3dChannel,
    NvU32 offset,
    NvU32 value)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_LOAD_CONSTANT_BUFFER_OFFSET, 2);
    nvPushSetMethodData(p, offset);
    nvPushSetMethodData(p, value);
}

#endif /* __NVIDIA_3D_CONSTANT_BUFFERS_H__ */
