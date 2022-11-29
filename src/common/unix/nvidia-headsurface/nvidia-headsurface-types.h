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

#ifndef __NVIDIA_HEADSURFACE_TYPES_H__
#define __NVIDIA_HEADSURFACE_TYPES_H__

#include "nvtypes.h"
#include "nvidia-3d-types.h"

typedef struct _NvHsVertexUniforms {
    Nv3dVertexAttrib2U vertexScale;
    Nv3dVertexAttrib2U primaryTextureScale;
    Nv3dVertexAttrib2U primaryTextureBias;
    Nv3dVertexAttrib2S cursorPosition;
} __attribute__((packed)) NvHsVertexUniforms;

typedef struct _NvHsFragmentUniforms  {          // Byte offsets
    Nv3dVertexAttrib2U vertexScale;                                    // 0
    Nv3dVertexAttrib3U numLutEntries         NV_ALIGN_BYTES(16);       // 16
    Nv3dVertexAttrib2U primaryTextureBias    NV_ALIGN_BYTES(8);        // 32
    Nv3dVertexAttrib2S cursorPosition;                                 // 40
    // Although this is really a 3x3 matrix, GLSL std140 uniform block
    // layout says that the column stride is equal to a vec4.
    Nv3dFloat transform[3][4];                                         // 48
    Nv3dVertexAttrib2F pixelShiftOffset;                               // 96
    Nv3dVertexAttrib3F luminanceCoefficient  NV_ALIGN_BYTES(16);       // 112
    Nv3dVertexAttrib2F chromaCoefficient     NV_ALIGN_BYTES(8);        // 128
    Nv3dFloat luminanceScale;                                          // 136
    Nv3dFloat luminanceBlackLevel;                                     // 140
    Nv3dFloat chrominanceScale;                                        // 144
    Nv3dFloat chrominanceBlackLevel;                                   // 148
    NvU32 useSatHue;                                                   // 152
    Nv3dFloat satCos;                                                  // 156
    int resamplingMethod;                                              // 160
} __attribute__((packed)) NvHsFragmentUniforms;

/*
 * The static warp mesh consists of four vertices, each vertex has six
 * components: (XY, UVRQ).
 */
typedef struct {
    struct {
        Nv3dFloat x, y, u, v, r, q;
    } vertex[4];
} NvHsStaticWarpMesh;

#endif /* __NVIDIA_HEADSURFACE_TYPES_H__ */
