/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0050.finn
//

#include "ctrl/ctrlxxxx.h"

#define NV0050_CTRL_CMD(cat, idx) NVXXXX_CTRL_CMD(0x0050, NV0050_CTRL_##cat, idx)

#define NV0050_CTRL_RESERVED (0x00U)
#define NV0050_CTRL_MEMORY   (0x01U)

#define NV0050_CTRL_CMD_NULL (0x5000U) /* finn: Evaluated from "(FINN_NV_CE_UTILS_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV0050_CTRL_CMD_MEMSET
 *
 * Memsets a memory allocation and releases a semaphore on completion.
 *
 * hMemory  [IN]
 *    Memory handle of the memory descriptor that needs to be memset.
 *    This is only available for verification purposes.
 *
 * offset   [IN]
 *    Offset into the memory descriptor.
 *
 * length   [IN]
 *    Length of physical memory to be memset.
 *    Must be less than or equal to memory size.
 *
 * pattern  [IN]
 *    The pattern to memset to
 *
 * flags    [IN]
 *    Can be any of the NV0050_CTRL_MEMSET_FLAGS_*
 *        DEFAULT
 *            By default, the memcopy operation will be synchronous and using
 *            physical copies
 *        ASYNC
 *            This flag forces this memset to be asynchronous.
 *        VIRTUAL
 *            This flag forces the memset to use Virtual addresses which are
 *            identity mapped. To use this feature, users need to pass in the
 *            hVaspace with identity mapped addresses for the entire memory during
 *            construct.
 *        PIPELINED
 *            This flag allows the copy/memset operation to be pipelined with previous dma operations on the same channel
 *            It means that its reads/writes are allowed happen before writes of preceding operations are tlb-acked
 *            The flag can be useful when dealing with non-inersecting async operations,
 *            but it can result in races when 2 async CE operations target the same allocation, and the second operation uses the flag
 *            Race example:
 *            1. async copy A -> B
 *            2. pipelined copy B -> C
 *            Here copy 2 can read B before copy finishes writing it, which will result in C containing invalid data
 *            Technical details:
 *            By default, first _LAUNCH_DMA method of a CE operation is marked has _TRANSFER_TYPE_NON_PIPELINED, which the flag overrides
 *            Subsequent _LAUNCH_DMA methods belonging to the same operation use _TRANSFER_TYPE_PIPELINED, as each of these methods should
 *            target different addresses
 *
 * submittedWorkId [OUT]
 *    The work submission token users can poll on to wait for work
 *    completed by CE. Only valid in case of ASYNC mode.
 */

#define NV0050_CTRL_MEMSET_FLAGS_DEFAULT 0
#define NV0050_CTRL_MEMSET_FLAGS_ASYNC      NVBIT(0)
#define NV0050_CTRL_MEMSET_FLAGS_VIRTUAL    NVBIT(1)
#define NV0050_CTRL_MEMSET_FLAGS_PIPELINED  NVBIT(2)

#define NV0050_CTRL_CMD_MEMSET           (0x500101U) /* finn: Evaluated from "(FINN_NV_CE_UTILS_UTILS_INTERFACE_ID << 8) | NV0050_CTRL_MEMSET_PARAMS_MESSAGE_ID" */

#define NV0050_CTRL_MEMSET_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0050_CTRL_MEMSET_PARAMS {
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 length, 8);
    NvU32    pattern;
    NV_DECLARE_ALIGNED(NvU64 flags, 8);
    NV_DECLARE_ALIGNED(NvU64 submittedWorkId, 8);
} NV0050_CTRL_MEMSET_PARAMS;

/*
 * NV0050_CTRL_CMD_MEMCOPY
 *
 * Copies from a source memoryto ssdestination  memory and releases a semaphore
 * on completion
 *
 * hDstMemory  [IN]
 *    Memory handle of the memory descriptor to which data will be copied.
 *    This is only available for verification purposes.
 *
 * hSrcMemory  [IN]
 *    Memory handle of the memory descriptor from which data will be copied.
 *    This is only available for verification purposes.
 *
 * dstOfffset   [IN]
 *    Offset into the destination memory descriptor.
 *
 * srcOffset   [IN]
 *    Offset into the source memory descriptor.
 *
 * length   [IN]
 *    Length of physical memory to be copied.
 *    Must be less than or equal to both destination and source memory size.
 *
 * flags    [IN]
 *    Can be any of the NV0050_CTRL_MEMCOPY_FLAGS_*
 *        DEFAULT
 *            By default, the memcopy operation will be synchronous and using
 *            physical copies
 *        ASYNC
 *            This flag forces this memset to be asynchronous.
 *        VIRTUAL
 *            This flag forces the memset to use Virtual addresses which are
 *            identity mapped. To use this feature, users need to pass in the
 *            hVaspace with identity mapped addresses for the entire memory during
 *            construct.
 *        PIPELINED
 *            This flag allows the copy/memset operation to be pipelined with previous dma operations on the same channel
 *            It means that its reads/writes are allowed happen before writes of preceding operations are tlb-acked
 *            The flag can be useful when dealing with non-inersecting async operations,
 *            but it can result in races when 2 async CE operations target the same allocation, and the second operation uses the flag
 *            Race example:
 *            1. async copy A -> B
 *            2. pipelined copy B -> C
 *            Here copy 2 can read B before copy finishes writing it, which will result in C containing invalid data
 *            Technical details:
 *            By default, first _LAUNCH_DMA method of a CE operation is marked has _TRANSFER_TYPE_NON_PIPELINED, which the flag overrides
 *            Subsequent _LAUNCH_DMA methods belonging to the same operation use _TRANSFER_TYPE_PIPELINED, as each of these methods should
 *            target different addresses
 *
 * submittedWorkId [OUT]
 *    The work submission token users can poll on to wait for work
 *    completed by CE. Only valid in case of ASYNC mode.
 */

#define NV0050_CTRL_MEMCOPY_FLAGS_DEFAULT 0
#define NV0050_CTRL_MEMCOPY_FLAGS_ASYNC      NVBIT(0)
#define NV0050_CTRL_MEMCOPY_FLAGS_VIRTUAL    NVBIT(1)
#define NV0050_CTRL_MEMCOPY_FLAGS_PIPELINED  NVBIT(2)

#define NV0050_CTRL_CMD_MEMCOPY           (0x500102U) /* finn: Evaluated from "(FINN_NV_CE_UTILS_UTILS_INTERFACE_ID << 8 | NV0050_CTRL_MEMCOPY_PARAMS_MESSAGE_ID)" */

#define NV0050_CTRL_MEMCOPY_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0050_CTRL_MEMCOPY_PARAMS {
    NvHandle hDstMemory;
    NvHandle hSrcMemory;
    NV_DECLARE_ALIGNED(NvU64 dstOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 srcOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 length, 8);
    NV_DECLARE_ALIGNED(NvU64 flags, 8);
    NV_DECLARE_ALIGNED(NvU64 submittedWorkId, 8);
} NV0050_CTRL_MEMCOPY_PARAMS;

/*
 * NV0050_CTRL_CMD_CHECK_PROGRESS
 *
 * Check if a previously submitted work item has been completed by HW.
 *
 * submittedWorkId  [IN]
 *    The work submission token users can poll on to wait for work
 *    completed by CE.
 *
 */
#define NV0050_CTRL_CHECK_PROGRESS_RESULT_DEFAULT 0
#define NV0050_CTRL_CHECK_PROGRESS_RESULT_FINISHED    NVBIT(1)

#define NV0050_CTRL_CMD_CHECK_PROGRESS            (0x500103U) /* finn: Evaluated from "(FINN_NV_CE_UTILS_UTILS_INTERFACE_ID << 8 | NV0050_CTRL_CHECK_PROGRESS_PARAMS_MESSAGE_ID)" */

#define NV0050_CTRL_CHECK_PROGRESS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0050_CTRL_CHECK_PROGRESS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 submittedWorkId, 8);
    NvU32 result;
} NV0050_CTRL_CHECK_PROGRESS_PARAMS;

/* _ctrl0050_h_ */
