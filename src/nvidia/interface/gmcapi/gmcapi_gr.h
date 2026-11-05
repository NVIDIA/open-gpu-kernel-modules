/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_GR_H
#define GMCAPI_GR_H

#include "gmcapi/gmcapi_memory.h"

#define GMCAPI_GR_GOLDEN_CTX_IMAGE_FLAGS_NONE                  0x00000000U

/*
 * Ephemeral state keys for GMCAPI_CMD_GR_COPY_GOLDEN_CTX_IMAGE.
 *
 * These keys set state that persists across a single
 * GMCAPI_CMD_GR_COPY_GOLDEN_CTX_IMAGE call. The index field of these keys
 * must be zero. The default values are reset at the beginning of each
 * GMCAPI_CMD_GR_COPY_GOLDEN_CTX_IMAGE call. All defaults are zero.
 *
 * Use with: #define NVKV_PREFIX NVGMC_GR.
 */

/* 32-bit state keys */
#define NVGMC_GR_GOLDEN_CTX_COPY_DST_APERTURE                  0x0001
#define NVGMC_GR_GOLDEN_CTX_COPY_DST_APERTURE_SYSMEM           GMCAPI_MEMORY_APERTURE_SYSMEM
#define NVGMC_GR_GOLDEN_CTX_COPY_DST_APERTURE_FBMEM            GMCAPI_MEMORY_APERTURE_FBMEM

#define NVGMC_GR_GOLDEN_CTX_COPY_PAGE_SIZE                     0x0002

#define NVGMC_GR_GOLDEN_CTX_COPY_FLAGS                         0x0003
#define NVGMC_GR_GOLDEN_CTX_COPY_FLAGS_NONE                    GMCAPI_GR_GOLDEN_CTX_IMAGE_FLAGS_NONE

#define NVGMC_GR_GOLDEN_CTX_COPY_IMAGE_INDEX                   0x0004

/* 64-bit array keys */
#define NVGMC_GR_GOLDEN_CTX_COPY_PAGE_ADDRESSES                0x1001
#define NVGMC_GR_GOLDEN_CTX_COPY_PAGE_ADDRESSES_MAX_COUNT      4096U

typedef struct GmcApiGrGoldenCtxImageInfo
{
    NvU32 imageSize;
    NvU32 pageSize;
    NvU32 flags;
    NvU32 imageCount;
    NvU32 reserved[2];
} GmcApiGrGoldenCtxImageInfo;

#endif // GMCAPI_GR_H
