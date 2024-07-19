/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __vgpu_vgpu_version_h__
#define __vgpu_vgpu_version_h__

/* VGX interface version */
#define NV_RPC_VERSION_NUMBER_MAJOR                           31:24 /* R---D */
#define NV_RPC_VERSION_NUMBER_MINOR                           23:16 /* R---D */

#define RPC_VERSION_FROM_VGX_VERSION(major, minor) ( DRF_NUM(_RPC, _VERSION_NUMBER, _MAJOR, major) | \
                                                     DRF_NUM(_RPC, _VERSION_NUMBER, _MINOR, minor))
#define VGX_MAJOR_VERSION_NUMBER  0x27
#define VGX_MINOR_VERSION_NUMBER  0x06

#define VGX_MAJOR_VERSION_NUMBER_VGPU_12_0 0x1A
#define VGX_MINOR_VERSION_NUMBER_VGPU_12_0 0x18
#define VGX_MAJOR_VERSION_NUMBER_VGPU_13_0 0x1C
#define VGX_MINOR_VERSION_NUMBER_VGPU_13_0 0x0A
#define VGX_MAJOR_VERSION_NUMBER_VGPU_16_0 0x23
#define VGX_MAJOR_VERSION_NUMBER_VGPU_17_0 0x25

/**
 * This macro have the mapping between internal (RPC) and external version
 * and is required to be updated appropriately with every new internal version.
 *
 * In case a new external version is added, a new entry representing the mapping
 * for the external version should be appended. Please note that the external
 * version should be updated when both of the following are true:
 * 1. The new RPC version update cause a break in migration compatibility.
 * 2. This is the first break in migration compatibility after a release.
 */
#define NV_VGPU_GRIDSW_INTERNAL_TO_EXTERNAL_VERSION_MAPPING \
    {{0x27, 0x0}, {0x27, 0x06}, {0x16, 0x1}},               \
    {{0x26, 0x0}, {0x26, 0x05}, {0x15, 0x1}},               \
    {{0x25, 0x0}, {0x25, 0x1B}, {0x14, 0x1}},               \
    {{0x24, 0x0}, {0x24, 0x0A}, {0x13, 0x1}},               \
    {{0x23, 0x0}, {0x23, 0x05}, {0x12, 0x1}},               \
    {{0x22, 0x0}, {0x22, 0x02}, {0x11, 0x1}},               \
    {{0x21, 0x0}, {0x21, 0x0C}, {0x10, 0x1}},               \
    {{0x20, 0x0}, {0x20, 0x04}, {0xF, 0x1}},                \
    {{0x1F, 0x0}, {0x1F, 0xF}, {0xE, 0x1}},                 \
    {{0x1E, 0x0}, {0x1E, 0xE}, {0xD, 0x1}},                 \
    {{0x1D, 0x0}, {0x1D, 0x6}, {0xC, 0x1}},                 \
    {{0x1C, 0x0}, {0x1C, 0xA}, {0xB, 0x1}},                 \
    {{0x1C, 0xB}, {0x1C, 0xC}, {0xB, 0x2}},                 \
    {{0x1B, 0x0}, {0x1B, 0x5}, {0xA, 0x1}},                 \
    {{0x1A, 0x0}, {0x1A, 0x18}, {0x9, 0x1}},                \
    {{0x1A, 0x19}, {0x1A, 0x24}, {0x9, 0x2}},               \
    {{0x19, 0x0}, {0x19, 0x1}, {0x8, 0x1}},                 \
    {{0x18, 0x0}, {0x18, 0x14},{0x7, 0x1}},                 \
    {{0x18, 0x15}, {0x18, 0x16},{0x7, 0x2}},                \
    {{0x17, 0x0}, {0x17, 0x6}, {0x6, 0x1}},                 \
    {{0x16, 0x0}, {0x16, 0x6}, {0x5, 0x1}},                 \
    {{0x16, 0x7}, {0x16, 0x7}, {0x5, 0x2}}

/*
 * Internal Versioning
 */

#define NV_VGPU_GRIDSW_NUMBER_INTERNAL_MAJOR                           63:32
#define NV_VGPU_GRIDSW_NUMBER_INTERNAL_MINOR                           31:0

#define GRIDSW_VERSION_INTERNAL(major, minor) (DRF_NUM64(_VGPU, _GRIDSW_NUMBER_INTERNAL, _MAJOR, major) | \
                                               DRF_NUM64(_VGPU, _GRIDSW_NUMBER_INTERNAL, _MINOR, minor))


// The NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL macros are auto-generated using the value from rpc-structures.def file.
#define AUTOGENERATE_RPC_MIN_SUPPORTED_VERSION_INFORMATION
#include "g_rpc-structures.h"
#undef AUTOGENERATE_RPC_MIN_SUPPORTED_VERSION_INFORMATION

/*
 * Versioning exposed externally
 */
#define NV_VGPU_GRIDSW_NUMBER_EXTERNAL_MAJOR                           31:16
#define NV_VGPU_GRIDSW_NUMBER_EXTERNAL_MINOR                           15:0

#define GRIDSW_VERSION_EXTERNAL(major, minor) (DRF_NUM(_VGPU, _GRIDSW_NUMBER_EXTERNAL, _MAJOR, major) | \
                                               DRF_NUM(_VGPU, _GRIDSW_NUMBER_EXTERNAL, _MINOR, minor))

/* WARNING: Should be updated with each vGPU release, if there is a break in
 * migration compatibility during the development of that release. */
#define NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR 0x16
#define NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR 0x1

/* WARNING: Should be updated with each vGPU release, if minimum supported
 * version change on the host.
 */
#define NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR 0x7
#define NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR 0x1

#endif // __vgpu_vgpu_version_h__
