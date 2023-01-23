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

#ifndef __NVIDIA_3D_UTILS_H__
#define __NVIDIA_3D_UTILS_H__

#include "nvidia-3d.h"

#include <class/cl9097.h>
#include <class/cla06fsubch.h>

static inline void nv3dSetSurfaceClip(
    Nv3dChannelRec *p3dChannel,
    NvS16 x,
    NvS16 y,
    NvU16 w,
    NvU16 h)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NV9097_SET_SURFACE_CLIP_HORIZONTAL, 2);
    nvPushSetMethodData(p,
        NV3D_V(9097, SET_SURFACE_CLIP_HORIZONTAL, X, x) |
        NV3D_V(9097, SET_SURFACE_CLIP_HORIZONTAL, WIDTH, w));
    nvPushSetMethodData(p,
        NV3D_V(9097, SET_SURFACE_CLIP_VERTICAL, Y, y) |
        NV3D_V(9097, SET_SURFACE_CLIP_VERTICAL, HEIGHT, h));
}

static inline void nv3dClearSurface(
    Nv3dChannelRec *p3dChannel,
    const NvU32 clearColor[4],
    NvU16 x,
    NvU16 y,
    NvU16 w,
    NvU16 h)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_COLOR_CLEAR_VALUE(0), 4);
    nvPushSetMethodData(p, clearColor[0]);
    nvPushSetMethodData(p, clearColor[1]);
    nvPushSetMethodData(p, clearColor[2]);
    nvPushSetMethodData(p, clearColor[3]);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NV9097_SET_CLEAR_RECT_HORIZONTAL, 2);
    nvPushSetMethodData(p,
        NV3D_V(9097, SET_CLEAR_RECT_HORIZONTAL, XMIN, x) |
        NV3D_V(9097, SET_CLEAR_RECT_HORIZONTAL, XMAX, x + w));
    nvPushSetMethodData(p,
        NV3D_V(9097, SET_CLEAR_RECT_VERTICAL, YMIN, y) |
        NV3D_V(9097, SET_CLEAR_RECT_VERTICAL, YMAX, y + h));
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_CLEAR_SURFACE,
        NV3D_C(9097, CLEAR_SURFACE, R_ENABLE, TRUE) |
        NV3D_C(9097, CLEAR_SURFACE, G_ENABLE, TRUE) |
        NV3D_C(9097, CLEAR_SURFACE, B_ENABLE, TRUE) |
        NV3D_C(9097, CLEAR_SURFACE, A_ENABLE, TRUE));
}

static inline void nv3dVasBegin(
    Nv3dChannelRec *p3dChannel,
    NvU32 mode)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvAssert(p3dChannel->currentPrimitiveMode == ~0);

    p3dChannel->currentPrimitiveMode = mode;
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_BEGIN, mode);
}

static inline void nv3dVasEnd(
    Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvAssert(p3dChannel->currentPrimitiveMode != ~0);

    p3dChannel->currentPrimitiveMode = ~0;
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_3D, NV9097_END, 0);
}

#endif /* __NVIDIA_3D_UTILS_H__ */
