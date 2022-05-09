/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl00f8.finn
//




#include "ctrl/ctrlxxxx.h"

#define NV00F8_CTRL_CMD(cat,idx)       NVXXXX_CTRL_CMD(0x00f8, NV00F8_CTRL_##cat, idx)

/* NV00F8 command categories (6bits) */
#define NV00F8_CTRL_RESERVED (0x00U)
#define NV00F8_CTRL_FABRIC   (0x01U)

/*
 * NV00F8_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00F8_CTRL_CMD_NULL (0xf80000U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV00F8_CTRL_CMD_GET_INFO
 *
 * Queries memory allocation attributes.
 *
 *  size [OUT]
 *    Size of the allocation.
 *
 *  pageSize [OUT]
 *    Page size of the allocation.
 *
 *  allocFlags [OUT]
 *    Flags passed during the allocation.
 */
#define NV00F8_CTRL_CMD_GET_INFO (0xf80101U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV00F8_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00F8_CTRL_GET_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32 pageSize;
    NvU32 allocFlags;
} NV00F8_CTRL_GET_INFO_PARAMS;

/*
 * NV00F8_CTRL_CMD_DESCRIBE
 *
 * Queries the physical attributes of the fabric memory allocation.
 *
 *  offset [IN]
 *    Offset into memory allocation to query physical addresses for.
 *
 *  totalPfns [OUT]
 *    Number of PFNs in memory allocation.
 *
 *  pfnArray [OUT]
 *    Array of PFNs in memory allocation (2MB page size shifted).
 *
 *  numPfns [OUT]
 *    Number of valid entries in pfnArray.
 *
 * Note: This ctrl call is only available for kerenl mode client in vGPU platforms.
 */

#define NV00F8_CTRL_CMD_DESCRIBE            (0xf80102) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_FABRIC_INTERFACE_ID << 8) | NV00F8_CTRL_DESCRIBE_PARAMS_MESSAGE_ID" */

#define NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE 512

#define NV00F8_CTRL_DESCRIBE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00F8_CTRL_DESCRIBE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 totalPfns, 8);
    NvU32 pfnArray[NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE];
    NvU32 numPfns;
} NV00F8_CTRL_DESCRIBE_PARAMS;

/* _ctrl00f8_h_ */
