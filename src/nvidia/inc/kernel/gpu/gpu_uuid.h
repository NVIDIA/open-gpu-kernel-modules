/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _GPUUUID_H_
#define _GPUUUID_H_

#include "core/core.h"
#include "nvCpuUuid.h"

//
// GPU unique ID sizes.  RM_SHA1_GID_SIZE uses the first 16 bytes of
// the SHA-1 digest (this is consistent with the way canonical UUIDs are
// constructed)
//
#define RM_SHA1_GID_SIZE         16

// UUID conversion routine:
NV_STATUS transformGidToUserFriendlyString(const NvU8 *pGidData, NvU32 gidSize, NvU8 **ppGidString,
                                           NvU32 *pGidStrlen, NvU32 gidFlags, NvU8 prefix);

NV_STATUS nvGenerateGpuUuid(NvU16 chipId, NvU64 pdi, NvUuid *pUuid);

NV_STATUS nvGenerateSmcUuid(NvU16 chipId, NvU64 pdi,
                            NvU32 swizzId, NvU32 syspipeId, NvUuid *pUuid);

// 'G' 'P' 'U' '-'(x5), '\0x0', extra = 9
#define NV_UUID_STR_LEN ((NV_UUID_LEN << 1) + 9)

#define RM_UUID_PREFIX_GPU         0U
#define RM_UUID_PREFIX_MIG         1U
#define RM_UUID_PREFIX_DLA         2U

void nvGetUuidString(const NvUuid *pUuid, NvU8 prefix, char *pUuidStr);

#endif // _GPUUUID_H_
