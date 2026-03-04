/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-22 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl00f9.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl00f8.h"

#define NV00F9_CTRL_CMD(cat,idx)       NVXXXX_CTRL_CMD(0x00f9, NV00F9_CTRL_##cat, idx)

/* NV00F9 command categories (6bits) */
#define NV00F9_CTRL_RESERVED (0x00U)
#define NV00F9_CTRL_IMPORT   (0x01U)

/*
 * NV00F9_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00F9_CTRL_CMD_NULL (0xf90000U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_IMPORT_V2_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV00F9_CTRL_CMD_GET_INFO
 *
 * Queries memory allocation attributes.
 *
 *  size [OUT]
 *    Size of the allocation.
 *
 *  pageSize [OUT]
 *    Page size of the allocation
 *
 *  memFlags [OUT]
 *    Flags associated with memory allocation.
 *
 *  physAttrs [OUT]
 *    Physical attributes associated with memory allocation.
 *    For flexible mappings, it is not possible to retrieve this information,
 *    behavior is undefined (returns all zeros).
 */
#define NV00F9_CTRL_CMD_GET_INFO (0xf90101U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_IMPORT_V2_IMPORT_INTERFACE_ID << 8) | NV00F9_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV00F9_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00F9_CTRL_GET_INFO_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NV_DECLARE_ALIGNED(NvU64 pageSize, 8);
    NvU32                    memFlags;
    NV_PHYSICAL_MEMORY_ATTRS physAttrs;
} NV00F9_CTRL_GET_INFO_PARAMS;

/* _ctrl00f9_h_ */
