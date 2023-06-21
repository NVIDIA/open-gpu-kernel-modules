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
// Source file:      ctrl/ctrl00fe.finn
//

#define NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_FLAG_PAGE_KIND_SOURCE_ALLOCATION 0:0
#define NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_FLAG_PAGE_KIND_SOURCE_ALLOCATION_PHYSICAL (0x00000000U)
#define NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_FLAG_PAGE_KIND_SOURCE_ALLOCATION_VIRTUAL  (0x00000001U)

typedef struct NV00FE_CTRL_PAGING_OPERATION {
    NvHandle hVirtualMemory;
    NV_DECLARE_ALIGNED(NvU64 virtualOffset, 8);
    NvHandle hPhysicalMemory;
    NV_DECLARE_ALIGNED(NvU64 physicalOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32    flags;
} NV00FE_CTRL_PAGING_OPERATION;

/*
 * NV00FE_CTRL_CMD_SUBMIT_PAGING_OPERATIONS
 *
 * Execute a list of paging operations
 * Page size is determined by the virtual allocation
 * Offsets/sizes must respect the page size
 *
 */
#define NV00FE_CTRL_CMD_SUBMIT_PAGING_OPERATIONS       (0xfe0002U) /* finn: Evaluated from "(FINN_NV_MEMORY_MAPPER_INTERFACE_ID << 8) | NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS_MESSAGE_ID" */

#define NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_MAX_COUNT (0x00010000U)

#define NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS {
    NvU32 pagingOpsCount;
    NV_DECLARE_ALIGNED(NvP64 pagingOps, 8);
} NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS;

