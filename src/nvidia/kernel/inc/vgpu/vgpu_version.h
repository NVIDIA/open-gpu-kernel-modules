/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define VGX_MAJOR_VERSION_NUMBER  0x1F
#define VGX_MINOR_VERSION_NUMBER  0x0F

// The NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL macros are auto-generated using the value from rpc-structures.def file.
#define AUTOGENERATE_RPC_MIN_SUPPORTED_VERSION_INFORMATION
#include "g_rpc-structures.h"
#undef AUTOGENERATE_RPC_MIN_SUPPORTED_VERSION_INFORMATION

#endif // __vgpu_vgpu_version_h__
