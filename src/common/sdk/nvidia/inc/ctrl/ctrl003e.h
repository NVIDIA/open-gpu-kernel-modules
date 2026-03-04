/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl003e.finn
//

#include "ctrl0041.h"
#include "ctrl/ctrlxxxx.h"
/* NV01_MEMORY_SYSTEM control commands and parameters */

#define NV003E_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x003E, NV003E_CTRL_##cat, idx)

/* NV01_MEMORY_SYSTEM command categories (6bits) */
#define NV003E_CTRL_RESERVED (0x00)
#define NV003E_CTRL_MEMORY   (0x01)

/*
 * NV003E_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV003E_CTRL_CMD_NULL (0x3e0000) /* finn: Evaluated from "(FINN_NV01_MEMORY_SYSTEM_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV003E_CTRL_CMD_GET_SURFACE_PHYS_ATTR
 *
 * See NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR
 *
 */
#define NV003E_CTRL_CMD_GET_SURFACE_PHYS_ATTR NV0041_CTRL_CMD_GET_SURFACE_PHYS_ATTR

typedef NV0041_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS NV003E_CTRL_GET_SURFACE_PHYS_ATTR_PARAMS;


/* valid gpuCacheAttr return values */
#define NV003E_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED_UNKNOWN (0x00000000)
#define NV003E_CTRL_GET_SURFACE_PHYS_ATTR_GPU_CACHED         (0x00000001)
#define NV003E_CTRL_GET_SURFACE_PHYS_ATTR_GPU_UNCACHED       (0x00000002)

/* NV003E_CTRL_CMD_GET_SURFACE_NUM_PHYS_PAGES
 *
 * This command returns the number of physical pages associated with the
 * memory object.
 *
 * This call is currently only implemented on Linux and assumes that linux
 * kernel in which RM module will be loaded has same page size as defined
 * in linux kernel source with which RM module was built.
 *
 *   numPages
 *     This parameter returns total number of physical pages associated with
 *     the memory object.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV003E_CTRL_CMD_GET_SURFACE_NUM_PHYS_PAGES           (0x3e0102) /* finn: Evaluated from "(FINN_NV01_MEMORY_SYSTEM_MEMORY_INTERFACE_ID << 8) | NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS_MESSAGE_ID" */

#define NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS {
    NvU32 numPages;
} NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS;


/* NV003E_CTRL_CMD_GET_SURFACE_PHYS_PAGES
 *
 * This command returns physical pages associated with the memory object.
 *
 * This call is currently only implemented on Linux and assumes that linux
 * kernel in which RM module will be loaded has same page size as defined
 * in linux kernel source with which RM module was built.
 *
 *   pPages
 *     This parameter returns physical pages associated with the memory object.
 *
 *   numPages
 *     This parameter is both an input and an output. As an input parameter,
 *     it's value indicates maximum number of physical pages to be copied to
 *     pPages. As an output parameter, it's value indicates number of physical
 *     pages copied to pPages.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV003E_CTRL_CMD_GET_SURFACE_PHYS_PAGES (0x3e0103) /* finn: Evaluated from "(FINN_NV01_MEMORY_SYSTEM_MEMORY_INTERFACE_ID << 8) | NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS_MESSAGE_ID" */

#define NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pPages, 8);
    NvU32 numPages;
} NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS;

/* _ctrl003e_h_ */
