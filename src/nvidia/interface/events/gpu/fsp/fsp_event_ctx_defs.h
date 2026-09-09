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

#ifndef _FSP_EVENT_CTX_DEFS_H_
#define _FSP_EVENT_CTX_DEFS_H_

#include "events/gpu/gpu_event_ctx_defs.h"
#include "nvtypes.h"

typedef NvU16 FSP_EVENT_CTX_TYPE;
enum
{
    FSP_EVENT_CTX_TYPES_BASE = GPU_OPERATIONAL_EVENT_CTX_TYPES_GPU_FSP_BASE,
    FSP_EVENT_CTX_TYPE_BOOT_TIMEOUT_DATA, // 0xA201
    FSP_EVENT_CTX_TYPE_FUSE_ERROR_DATA,   // 0xA202
};

/*
 * FSP Boot Timeout Data (Context Type FSP_EVENT_CTX_TYPE_BOOT_TIMEOUT_DATA, Version 1.0)
 * Total size: 24 bytes
 */
#define FSP_BOOT_TIMEOUT_DATA_VERSION_MAJOR 1
#define FSP_BOOT_TIMEOUT_DATA_VERSION_MINOR 0
#define FSP_BOOT_TIMEOUT_DATA_VERSION       \
    ((FSP_BOOT_TIMEOUT_DATA_VERSION_MINOR) | \
     (FSP_BOOT_TIMEOUT_DATA_VERSION_MAJOR << 8))

typedef struct FSP_BOOT_TIMEOUT_DATA
{
    NvU32 errorStatus;
    NvU32 fspBootComplete;
    NvU32 scratch0;
    NvU32 scratch1;
    NvU32 scratch2;
    NvU32 scratch3;
} FSP_BOOT_TIMEOUT_DATA;

/*
 * FSP Fuse Error Data (Context Type FSP_EVENT_CTX_TYPE_FUSE_ERROR_DATA, Version 1.0)
 * Total size: 4 bytes
 */
#define FSP_FUSE_ERROR_DATA_VERSION_MAJOR 1
#define FSP_FUSE_ERROR_DATA_VERSION_MINOR 0
#define FSP_FUSE_ERROR_DATA_VERSION       \
    ((FSP_FUSE_ERROR_DATA_VERSION_MINOR) | \
     (FSP_FUSE_ERROR_DATA_VERSION_MAJOR << 8))

typedef struct FSP_FUSE_ERROR_DATA
{
    NvU32 fuseStatus;
} FSP_FUSE_ERROR_DATA;

#endif // _FSP_EVENT_CTX_DEFS_H_
