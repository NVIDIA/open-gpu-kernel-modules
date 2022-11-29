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

#ifndef __NVIDIA_3D_COLOR_TARGETS_H__
#define __NVIDIA_3D_COLOR_TARGETS_H__

#include "nvidia-3d.h"

#include <class/cl9097.h>
#include <class/cla06fsubch.h>

/*
 * This header file defines static inline functions to manage 3D class
 * color targets.
 */

static inline void nv3dSelectColorTarget(
    Nv3dChannelPtr p3dChannel,
    NvU8 colorTargetIndex)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_CT_SELECT,
        NV3D_V(9097, SET_CT_SELECT, TARGET_COUNT, 1)           |
        NV3D_V(9097, SET_CT_SELECT, TARGET0, colorTargetIndex) |
        NV3D_V(9097, SET_CT_SELECT, TARGET1, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET2, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET3, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET4, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET5, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET6, 0)                |
        NV3D_V(9097, SET_CT_SELECT, TARGET7, 0));
}

static inline void nv3dSetColorTarget(
    Nv3dChannelPtr p3dChannel,
    NvU8 colorTargetIndex,
    NvU32 surfaceFormat,
    NvU64 surfaceGpuAddress,
    NvBool blockLinear,
    Nv3dBlockLinearLog2GobsPerBlock gobsPerBlock,
    NvU32 surfaceWidth,
    NvU32 surfaceHeight)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    const NvU32 memoryInfo =
        blockLinear ?
        (NV3D_V(9097, SET_COLOR_TARGET_MEMORY, BLOCK_WIDTH,  gobsPerBlock.x) |
         NV3D_V(9097, SET_COLOR_TARGET_MEMORY, BLOCK_HEIGHT, gobsPerBlock.y) |
         NV3D_V(9097, SET_COLOR_TARGET_MEMORY, BLOCK_DEPTH,  gobsPerBlock.z) |
         NV3D_C(9097, SET_COLOR_TARGET_MEMORY, LAYOUT, BLOCKLINEAR)) :
        NV3D_C(9097, SET_COLOR_TARGET_MEMORY, LAYOUT, PITCH);

    if (surfaceFormat == NV9097_SET_COLOR_TARGET_FORMAT_V_DISABLED) {
        // Disable this color target.
        nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D,
            NV9097_SET_COLOR_TARGET_FORMAT(colorTargetIndex),
            NV9097_SET_COLOR_TARGET_FORMAT_V_DISABLED);
        return;
    }

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_COLOR_TARGET_A(colorTargetIndex), 6);

    nvPushSetMethodDataU64(p, surfaceGpuAddress);
    nvPushSetMethodData(p, surfaceWidth);
    nvPushSetMethodData(p, surfaceHeight);
    nvPushSetMethodData(p, surfaceFormat);
    nvPushSetMethodData(p, memoryInfo);
}

#endif /* __NVIDIA_3D_COLOR_TARGETS_H__ */
