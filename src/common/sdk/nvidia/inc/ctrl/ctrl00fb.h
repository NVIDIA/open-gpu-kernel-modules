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
// Source file:      ctrl/ctrl00fb.finn
//

#include "nvtypes.h"
#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl00f8.h"

#define NV00FB_CTRL_CMD(cat, idx) \
    NVXXXX_CTRL_CMD(0x00fb, NV00FB_CTRL_##cat, idx)

/* NV00FB command categories (6bits) */
#define NV00FB_CTRL_RESERVED   (0x00U)
#define NV00FB_CTRL_IMPORT_REF (0x01U)

/*
 * NV00FB_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00FB_CTRL_CMD_NULL   (0xfb0000U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_IMPORTED_REF_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV00FB_CTRL_CMD_VALIDATE
 *
 * Populate the imported memory, making it ready to map locally.
 *
 *  attrs [IN]
 *   Attributes associated with memory allocation. 
 *
 *  physAttrs [IN]
 *    Physical attributes associated with memory allocation.
 *    For flexible mappings, it is not possible to retrieve this information,
 *    behavior is undefined (returns all zeros).
 *
 *  offset [IN]
 *    Offset into memory allocation to query physical addresses for.
 *
 *  totalPfns [IN]
 *    Number of PFNs in memory allocation.
 *
 *  pfnArray [IN]
 *    Array of PFNs in memory allocation (2MB page size shifted).
 *
 *  numPfns [IN]
 *    Number of valid entries in pfnArray.
 *
 *  memFlags [IN]
 *    Flags associated with memory allocation.
 *
 *  flags [IN]
 *    Flags to notify RM about errors during import.
 *
 *  bDone [out]
 *    Whether the RM is expecting additional calls to _VALIDATE.
 */
#define NV00FB_CTRL_CMD_VALIDATE            (0xfb0101U) /* finn: Evaluated from "(FINN_NV_MEMORY_FABRIC_IMPORTED_REF_IMPORT_REF_INTERFACE_ID << 8) | NV00FB_CTRL_VALIDATE_PARAMS_MESSAGE_ID" */

#define NV00FB_CTRL_VALIDATE_PFN_ARRAY_SIZE 512U

#define NV00FB_CTRL_FLAGS_NONE              0U
#define NV00FB_CTRL_FLAGS_IMPORT_FAILED     1U

#define NV00FB_CTRL_VALIDATE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00FB_CTRL_VALIDATE_PARAMS {
    NV_DECLARE_ALIGNED(NV_FABRIC_MEMORY_ATTRS attrs, 8);
    NV_PHYSICAL_MEMORY_ATTRS physAttrs;
    NV_DECLARE_ALIGNED(NvU64 offset, 8);
    NV_DECLARE_ALIGNED(NvU64 totalPfns, 8);
    NvU32                    pfnArray[NV00FB_CTRL_VALIDATE_PFN_ARRAY_SIZE];
    NvU32                    numPfns;
    NvU32                    memFlags;
    NvU32                    flags;
    NvBool                   bDone;
} NV00FB_CTRL_VALIDATE_PARAMS;

/* _ctrl00fb_h_ */
