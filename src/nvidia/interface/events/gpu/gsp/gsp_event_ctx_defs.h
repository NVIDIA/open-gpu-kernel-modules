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

#ifndef _GSP_EVENT_CTX_DEFS_H_
#define _GSP_EVENT_CTX_DEFS_H_

#include "events/gpu/gpu_event_ctx_defs.h"
#include "nvtypes.h"

typedef NvU16 GSP_EVENT_CTX_TYPE;
enum
{
    GSP_EVENT_CTX_TYPES_BASE = GPU_OPERATIONAL_EVENT_CTX_TYPES_GPU_GSP_BASE,
    GSP_EVENT_CTX_TYPE_RPC_TIMEOUT_DATA,    // 0xA101
};

/*
 * GSP RPC Timeout Data (Context Type GSP_EVENT_CTX_TYPE_RPC_TIMEOUT_DATA, Version 1.0)
 * Total size: variable (max 96 bytes)
 */
#define GSP_RPC_TIMEOUT_DATA_VERSION_MAJOR 1
#define GSP_RPC_TIMEOUT_DATA_VERSION_MINOR 0
#define GSP_RPC_TIMEOUT_DATA_VERSION       \
    ((GSP_RPC_TIMEOUT_DATA_VERSION_MINOR) | \
     (GSP_RPC_TIMEOUT_DATA_VERSION_MAJOR << 8))

typedef struct GSP_RPC_TIMEOUT_DATA
{
    NvU32 expectedFunc;
    NvU32 expectedSequence;
    NvU32 gpuInstance;
    NvU32 reserved;
    NvU64 historyData0;
    NvU64 historyData1;
    NvU64 waitedSeconds;

#define GSP_RPC_TIMEOUT_DATA_RPC_NAME_MAX_LEN 55
    EVENT_CTX_FLEXIBLE_ARRAY_MEMBER(char, rpcName,
        GSP_RPC_TIMEOUT_DATA_RPC_NAME_MAX_LEN + 1)
} GSP_RPC_TIMEOUT_DATA;

static inline NvLength GSP_RPC_TIMEOUT_DATA_SIZE(NvLength rpcNameSize)
{
    return NV_OFFSETOF(GSP_RPC_TIMEOUT_DATA, rpcName) + rpcNameSize;
}

#endif // _GSP_EVENT_CTX_DEFS_H_
