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

#ifndef __NVIDIA_3D_TYPES_PRIV_H__
#define __NVIDIA_3D_TYPES_PRIV_H__

#include "nvidia-3d-types.h"

struct _Nv3dHal {
    void   (*setSpaVersion)         (Nv3dChannelRec *p3dChannel);
    void   (*initChannel)           (Nv3dChannelRec *p3dChannel);
    void   (*uploadDataInline)      (Nv3dChannelRec *p3dChannel,
                                     NvU64 gpuBaseAddress,
                                     size_t offset,
                                     const void *data,
                                     size_t bytes);
    void   (*setProgramOffset)      (Nv3dChannelRec *p3dChannel,
                                     NvU32 stage,
                                     NvU32 offset);
    void   (*assignNv3dTexture)     (Nv3dRenderTexInfo info,
                                     Nv3dTexture *tex);
    void   (*setVertexStreamEnd)    (Nv3dChannelPtr p3dChannel,
                                     enum Nv3dVertexAttributeStreamType stream,
                                     const Nv3dVertexAttributeStreamRec *pStream);
};

#endif /* __NVIDIA_3D_TYPES_PRIV_H__ */

