/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000gsync.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
#include "class/cl30f1.h"
/* NV01_ROOT (client) system controller control commands and parameters */

/*
 * NV0000_CTRL_CMD_GSYNC_GET_ATTACHED_IDS
 *
 * This command returns a table of attached gsyncId values.
 * The table is NV0000_CTRL_GSYNC_MAX_ATTACHED_GSYNCS entries in size.
 *
 *   gsyncIds[]
 *     This parameter returns the table of attached gsync IDs.
 *     The gsync ID is an opaque platform-dependent value that
 *     can be used with the NV0000_CTRL_CMD_GSYNC_GET_ID_INFO command to
 *     retrieve additional information about the gsync device.
 *     The valid entries in gsyncIds[] are contiguous, with a value
 *     of NV0000_CTRL_GSYNC_INVALID_ID indicating the invalid entries.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV0000_CTRL_CMD_GSYNC_GET_ATTACHED_IDS (0x301) /* finn: Evaluated from "(FINN_NV01_ROOT_GSYNC_INTERFACE_ID << 8) | NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS {
    NvU32 gsyncIds[NV30F1_MAX_GSYNCS];
} NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS;

/* this value marks entries in gsyncIds[] as invalid */
#define NV0000_CTRL_GSYNC_INVALID_ID      (0xffffffff)

/*
 * NV0000_CTRL_CMD_GSYNC_GET_ID_INFO
 *
 * This command returns gsync instance information for the
 * specified gsync device.
 *
 *   gsyncId
 *     This parameter should specify a valid gsync ID value.
 *     If there is no gsync present with the specified ID, a
 *     status of NV_ERR_INVALID_ARGUMENT is returned.
 *   gsyncFlags
 *     This parameter returns the current state of the gsync device. 
 *   gsyncInstance
 *     This parameter returns the instance number associated with the
 *     specified gsync.  This value can be used to instantiate
 *     a reference to the gsync using one of the NV30_GSYNC
 *     classes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_GSYNC_GET_ID_INFO (0x302) /* finn: Evaluated from "(FINN_NV01_ROOT_GSYNC_INTERFACE_ID << 8) | NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS {
    NvU32 gsyncId;
    NvU32 gsyncFlags;
    NvU32 gsyncInstance;
} NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS;

/* _ctrl0000gsync_h_ */
