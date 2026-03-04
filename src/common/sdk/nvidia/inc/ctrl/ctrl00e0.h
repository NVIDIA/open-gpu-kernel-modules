/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl00e0.finn
//

#include "ctrl/ctrlxxxx.h"
#include "class/cl00e0.h"

#define NV00E0_CTRL_CMD(cat,idx) NVXXXX_CTRL_CMD(0x00E0, NV00E0_CTRL_##cat, idx)

/* NV00E0 command categories (6bits) */
#define NV00E0_CTRL_RESERVED      (0x00U)
#define NV00E0_CTRL_MEMORY_EXPORT (0x01U)

/*
 * NV00E0_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV00E0_CTRL_CMD_NULL      (0xe00000U) /* finn: Evaluated from "(FINN_NV_MEMORY_EXPORT_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV00E0_CTRL_CMD_EXPORT_MEM
 *
 * Attaches memory handles to the export object.
 *
 *  index [IN]
 *    Index into the export object at which to start attaching the provided
 *    memory handles.
 *
 *  hParent [IN]
 *    Parent of memory handles to be attached (client or device or subdevice)
 *    If device or subdevice is used as parent, the related GPU is ref-counted
 *    by populating devDescriptor on behalf of the caller.
 *
 *  handles [IN]
 *    Memory handles to be attached.
 *
 *  numHandles [IN]
 *    Number of memory handles to be attached.
 *
 *  devDescriptor [OPAQUE]
 *    devDescriptor is a file descriptor for unix RM clients, but a void
 *    pointer for windows RM clients. It is transparent to RM clients i.e. RM's
 *    user-mode shim populates this field on behalf of clients.
 *
 *  Notes:
 *  - Imported fabric memory handles not supported.
 *  - No MIG support.
 *  - If 'numHandles' and 'index' overlap with a prior call, the newer call's
 *    RM handles will overwrite the attached handles from the previous call.
 *    This overlapping behavior can also be used to detach a handle by setting
 *    the appropriate object in 'handles' to 0.
 */
#define NV00E0_CTRL_CMD_EXPORT_MEM (0xe00101) /* finn: Evaluated from "(FINN_NV_MEMORY_EXPORT_INTERFACE_ID << 8) | NV00E0_CTRL_EXPORT_MEM_PARAMS_MESSAGE_ID" */

#define NV00E0_MAX_EXPORT_HANDLES  256

#define NV00E0_CTRL_EXPORT_MEM_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00E0_CTRL_EXPORT_MEM_PARAMS {
    NvU16    index;
    NvHandle hParent;
    NvHandle handles[NV00E0_MAX_EXPORT_HANDLES];
    NvU16    numHandles;
    NV_DECLARE_ALIGNED(NvU64 devDescriptor, 8);
} NV00E0_CTRL_EXPORT_MEM_PARAMS;

/*
 * NV00E0_CTRL_CMD_IMPORT_MEM
 *
 * Imports attached objects into the caller's RM client.
 *
 *  index [IN]
 *    Index into the export object from which start importing.
 *
 *  numHandles [IN]
 *    Number of memory handles to be imported.
 *
 *  handles [IN]
 *    Handles to be used for importing.
 *
 *  memInfos [OUT]
 *    Information of the imported objects
 */
#define NV00E0_CTRL_CMD_IMPORT_MEM       (0xe00102) /* finn: Evaluated from "(FINN_NV_MEMORY_EXPORT_INTERFACE_ID << 8) | NV00E0_CTRL_IMPORT_MEM_PARAMS_MESSAGE_ID" */

#define NV00E0_MAX_IMPORT_HANDLES        128

#define NV00E0_ADDR_SPACE_TYPE_SYSMEM    0x1
#define NV00E0_ADDR_SPACE_TYPE_VIDMEM    0x2
#define NV00E0_ADDR_SPACE_TYPE_FABRIC    0x3
#define NV00E0_ADDR_SPACE_TYPE_FABRIC_MC 0x4

typedef struct NV_IMPORT_MEM_INFO {
    NvHandle hParent;
    NvU8     addrSpace;
} NV_IMPORT_MEM_INFO;

#define NV00E0_CTRL_IMPORT_MEM_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00E0_CTRL_IMPORT_MEM_PARAMS {
    NvU16              index;
    NvU16              numHandles;
    NvHandle           handles[NV00E0_MAX_IMPORT_HANDLES];
    NV_IMPORT_MEM_INFO memInfos[NV00E0_MAX_IMPORT_HANDLES];
} NV00E0_CTRL_IMPORT_MEM_PARAMS;

/*
* NV00E0_CTRL_CMD_GET_INFO
*
*  Returns object information
*
*  info [OUT]
*    Same as NV00E0_ALLOCATION_PARAMETERS, see cl00e0.h for details.
*/
#define NV00E0_CTRL_CMD_GET_INFO (0xe00103) /* finn: Evaluated from "(FINN_NV_MEMORY_EXPORT_INTERFACE_ID << 8) | NV00E0_CTRL_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV00E0_CTRL_GET_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV00E0_CTRL_GET_INFO_PARAMS {
    NV00E0_ALLOCATION_PARAMETERS info;
} NV00E0_CTRL_GET_INFO_PARAMS;

/* _ctrl00e0_h_ */
