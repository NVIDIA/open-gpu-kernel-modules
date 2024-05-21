/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

typedef enum NV00FE_CTRL_OPERATION_TYPE {
    NV00FE_CTRL_OPERATION_TYPE_NOP = 0, // operation is ignored, used internally
    NV00FE_CTRL_OPERATION_TYPE_MAP = 1,
    NV00FE_CTRL_OPERATION_TYPE_UNMAP = 2,
    NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_WAIT = 3,
    NV00FE_CTRL_OPERATION_TYPE_SEMAPHORE_SIGNAL = 4,
} NV00FE_CTRL_OPERATION_TYPE;

typedef struct NV00FE_CTRL_OPERATION_MAP {
    NvHandle hVirtualMemory;
    NV_DECLARE_ALIGNED(NvU64 virtualOffset, 8);
    NvHandle hPhysicalMemory;
    NV_DECLARE_ALIGNED(NvU64 physicalOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32    dmaFlags; // NVOS46_FLAGS
} NV00FE_CTRL_OPERATION_MAP;

typedef struct NV00FE_CTRL_OPERATION_UNMAP {
    NvHandle hVirtualMemory;
    NV_DECLARE_ALIGNED(NvU64 virtualOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32    dmaFlags; // NVOS47_FLAGS
} NV00FE_CTRL_OPERATION_UNMAP;

typedef struct NV00FE_CTRL_OPERATION_SEMAPHORE {
    NvU32 index;
    NV_DECLARE_ALIGNED(NvU64 value, 8);
} NV00FE_CTRL_OPERATION_SEMAPHORE;

typedef struct NV00FE_CTRL_OPERATION {
    NV00FE_CTRL_OPERATION_TYPE type;

    union {
        NV_DECLARE_ALIGNED(NV00FE_CTRL_OPERATION_MAP map, 8);
        NV_DECLARE_ALIGNED(NV00FE_CTRL_OPERATION_UNMAP unmap, 8);
        NV_DECLARE_ALIGNED(NV00FE_CTRL_OPERATION_SEMAPHORE semaphore, 8);
    } data;
} NV00FE_CTRL_OPERATION;

/*
 * NV00FE_CTRL_CMD_SUBMIT_OPERATIONS
 *
 * Execute a list of mapping/semaphore operations
 * Page size is determined by the virtual allocation
 * Offsets/sizes must respect the page size
 *
 */
#define NV00FE_CTRL_CMD_SUBMIT_OPERATIONS (0xfe0101U) /* finn: Evaluated from "(FINN_NV_MEMORY_MAPPER_INTERFACE_ID << 8) | NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS_MESSAGE_ID" */

#define NV00FE_MAX_OPERATIONS_COUNT       (0x00001000U)

#define NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS {
    NvU32 operationsCount;
    NV_DECLARE_ALIGNED(NV00FE_CTRL_OPERATION pOperations[NV00FE_MAX_OPERATIONS_COUNT], 8);
    NvU32 operationsProcessedCount;
} NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS;

/*
 * NV00FE_CTRL_CMD_RESIZE_QUEUE
 *
 * Resize the MemoryMapper command queue
 * All pending commands remain in queue
 *
 */
#define NV00FE_CTRL_CMD_RESIZE_QUEUE (0xfe0102U) /* finn: Evaluated from "(FINN_NV_MEMORY_MAPPER_INTERFACE_ID << 8) | NV00FE_CTRL_RESIZE_QUEUE_PARAMS_MESSAGE_ID" */

#define NV00FE_CTRL_RESIZE_QUEUE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00FE_CTRL_RESIZE_QUEUE_PARAMS {
    NvU32 maxQueueSize;
} NV00FE_CTRL_RESIZE_QUEUE_PARAMS;

