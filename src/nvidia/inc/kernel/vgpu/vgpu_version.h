/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NV_RPC_VERSION_NUMBER_MAJOR 31:24 /* R---D */
#define NV_RPC_VERSION_NUMBER_MINOR 23:16 /* R---D */

#define RPC_VERSION_FROM_VGX_VERSION(major, minor) (DRF_NUM(_RPC, _VERSION_NUMBER, _MAJOR, major) | \
                                                    DRF_NUM(_RPC, _VERSION_NUMBER, _MINOR, minor))
#define VGX_MAJOR_VERSION_NUMBER 0x2A
#define VGX_MINOR_VERSION_NUMBER 0x09

#define VGX_MAJOR_VERSION_NUMBER_VGPU_12_0 0x1A
#define VGX_MINOR_VERSION_NUMBER_VGPU_12_0 0x18
#define VGX_MAJOR_VERSION_NUMBER_VGPU_13_0 0x1C
#define VGX_MINOR_VERSION_NUMBER_VGPU_13_0 0x0A
#define VGX_MAJOR_VERSION_NUMBER_VGPU_16_0 0x23
#define VGX_MAJOR_VERSION_NUMBER_VGPU_17_0 0x25
#define VGX_MAJOR_VERSION_NUMBER_VGPU_18_0 0x29
#define VGX_MINOR_VERSION_NUMBER_VGPU_18_0 0x0B

#define VGX_MAJOR_VERSION_WITH_FB_COPY_LARGE_BLOCKSZ_SUPPORT 0x28

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
    {{0x2A, 0x00}, {0x2A, 0x09}, {0x19, 0x01}},             \
    {{0x29, 0x00}, {0x29, 0x0B}, {0x18, 0x01}},             \
    {{0x29, 0x0C}, {0x29, 0x0C}, {0x18, 0x02}},             \
    {{0x28, 0x00}, {0x28, 0x09}, {0x17, 0x01}},             \
    {{0x27, 0x00}, {0x27, 0x06}, {0x16, 0x01}},             \
    {{0x26, 0x00}, {0x26, 0x05}, {0x15, 0x01}},             \
    {{0x25, 0x00}, {0x25, 0x1B}, {0x14, 0x01}},             \
    {{0x24, 0x00}, {0x24, 0x0A}, {0x13, 0x01}},             \
    {{0x23, 0x00}, {0x23, 0x05}, {0x12, 0x01}},             \
    {{0x22, 0x00}, {0x22, 0x02}, {0x11, 0x01}},             \
    {{0x21, 0x00}, {0x21, 0x0C}, {0x10, 0x01}},             \
    {{0x20, 0x00}, {0x20, 0x04}, {0x0F, 0x01}},             \
    {{0x1F, 0x00}, {0x1F, 0x0F}, {0x0E, 0x01}},             \
    {{0x1E, 0x00}, {0x1E, 0x0E}, {0x0D, 0x01}},             \
    {{0x1D, 0x00}, {0x1D, 0x06}, {0x0C, 0x01}},             \
    {{0x1C, 0x00}, {0x1C, 0x0A}, {0x0B, 0x01}},             \
    {{0x1C, 0x0B}, {0x1C, 0x0C}, {0x0B, 0x02}},             \
    {{0x1B, 0x00}, {0x1B, 0x05}, {0x0A, 0x01}},             \
    {{0x1A, 0x00}, {0x1A, 0x18}, {0x09, 0x01}},             \
    {{0x1A, 0x19}, {0x1A, 0x24}, {0x09, 0x02}},             \
    {{0x19, 0x00}, {0x19, 0x01}, {0x08, 0x01}},             \
    {{0x18, 0x00}, {0x18, 0x14}, {0x07, 0x01}},             \
    {{0x18, 0x15}, {0x18, 0x16}, {0x07, 0x02}},             \
    {{0x17, 0x00}, {0x17, 0x06}, {0x06, 0x01}},             \
    {{0x16, 0x00}, {0x16, 0x06}, {0x05, 0x01}},             \
    {{0x16, 0x07}, {0x16, 0x07}, {0x05, 0x02}}

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
#define NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR 0x19
#define NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR 0x1

/* WARNING: Should be updated with each vGPU release, if minimum supported
 * version change on the host.
 */
#define NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR 0x7
#define NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR 0x1

#endif // __vgpu_vgpu_version_h__
