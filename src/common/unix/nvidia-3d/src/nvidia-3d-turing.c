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

#include "nvidia-3d-turing.h"
#include "nvidia-3d-pascal.h"
#include "nvidia-3d.h"

#include "class/clc597.h"
#include <class/cla06fsubch.h>

void _nv3dInitChannelTuring(Nv3dChannelRec *p3dChannel)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    _nv3dInitChannelPascal(p3dChannel);

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D, NVC597_SET_SPH_VERSION, 2);
    nvPushSetMethodData(p,
        NV3D_V(C597, SET_SPH_VERSION, CURRENT, 4) |
        NV3D_V(C597, SET_SPH_VERSION, OLDEST_SUPPORTED, 4));
    nvPushSetMethodData(p,
        NV3D_V(C597, CHECK_SPH_VERSION, CURRENT, 4) |
        NV3D_V(C597, CHECK_SPH_VERSION, OLDEST_SUPPORTED, 4));
}

void _nv3dSetVertexStreamEndTuring(
    Nv3dChannelPtr p3dChannel,
    enum Nv3dVertexAttributeStreamType stream,
    const Nv3dVertexAttributeStreamRec *pStream)
{
    NvPushChannelPtr p = p3dChannel->pPushChannel;

    nvPushMethod(p, NVA06F_SUBCHANNEL_3D,
        NVC597_SET_VERTEX_STREAM_SIZE_A(stream), 2);
    nvPushSetMethodDataU64(p, pStream->end - pStream->current);
}
