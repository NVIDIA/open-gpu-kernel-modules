/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVIDIA_3D_SHADERS_H__
#define __NVIDIA_3D_SHADERS_H__

#include <nvtypes.h>
#include <class/cl9097.h>

// These are used in the "shader type" field below
#define NV3D_SHADER_TYPE_VERTEX   NV9097_SET_PIPELINE_SHADER_TYPE_VERTEX
#define NV3D_SHADER_TYPE_PIXEL    NV9097_SET_PIPELINE_SHADER_TYPE_PIXEL

typedef enum {
    NV3D_HW_SHADER_STAGE_VERTEX_A = 0,
    NV3D_HW_SHADER_STAGE_VERTEX_B,
    NV3D_HW_SHADER_STAGE_TESS_CONTROL,
    NV3D_HW_SHADER_STAGE_TESS_EVAL,
    NV3D_HW_SHADER_STAGE_GEOMETRY,
    NV3D_HW_SHADER_STAGE_PIXEL,
    NV3D_HW_SHADER_STAGE_COUNT,
} __attribute__ ((__packed__)) Nv3dShaderStage;

typedef enum {
    NV3D_HW_BIND_GROUP_VERTEX = 0,
    NV3D_HW_BIND_GROUP_TESS_CONTROL,
    NV3D_HW_BIND_GROUP_TESS_EVAL,
    NV3D_HW_BIND_GROUP_GEOMETRY,
    NV3D_HW_BIND_GROUP_FRAGMENT,
    NV3D_HW_BIND_GROUP_LAST = NV3D_HW_BIND_GROUP_FRAGMENT
} __attribute__ ((__packed__)) Nv3dShaderBindGroup;

typedef struct _nv_program_info {
    NvU32                       offset;         // Start offset relative to program heap
    NvU8                        registerCount;  // From '#.MAX_REG n'+1
    NvU8                        type;           // Shader type
    NvS8                        constIndex;     // Index into the compiler-generated constant buffer table

    Nv3dShaderStage             stage;          // Pipeline stage
    Nv3dShaderBindGroup         bindGroup;      // NV3D_HW_BIND_GROUP
} Nv3dProgramInfo;

typedef struct _nv_shader_const_buf_info {
    const NvU32 *data;
    NvU32 offset;
    NvU32 size;
} Nv3dShaderConstBufInfo;

#endif // __NVIDIA_3D_SHADERS_H__
