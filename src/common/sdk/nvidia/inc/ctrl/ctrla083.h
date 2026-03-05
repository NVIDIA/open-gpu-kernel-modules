/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla083.finn
//

#include "ctrl/ctrlxxxx.h"
/* NVA083_GRID_DISPLAYLESS control commands and parameters */

#define NVA083_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0xA083, NVA083_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVA083_CTRL_RESERVED        (0x00)
#define NVA083_CTRL_VIRTUAL_DISPLAY (0x01)

/*
 * NVA083_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVA083_CTRL_CMD_NULL        (0xa0830000) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_NUM_HEADS
 *
 * This command returns the number of heads supported on a GPU with
 * Displayless path enabled OR on a virtual GPU.
 *
 *   numHeads
 *     This parameter returns total number of heads supported.
 *
 *   maxNumHeads
 *     This parameter returns the MAX number of heads supported on this
 *     Displayless GRID GPU without any overrides of GPU capabilities.
 *     On Windows/Linux, we cannot dynamically change number of display
 *     head/source exposed to OS. Hence DD is required to report total
 *     number of display heads during init.
 *     DD registers during initialization the MAX number of heads supported and
 *     post that ensure only fixed allowed number of heads (reported via
 *     numHeads) can be made active (usable).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_NUM_HEADS (0xa0830101) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS {
    NvU32 numHeads;
    NvU32 maxNumHeads;
} NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS;

/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION
 * 
 * This command returns the maximum resolution supported on a GPU with
 * Displayless path enabled OR on a virtual GPU.
 *
 *   headIndex
 *     This parameter specifies the head for which the maximum resolution is to be 
 *     retrieved.
 *
 *   maxHResolution
 *     This parameter returns the maximum horizontal resolution.
 *
 *   maxVResolution
 *      This parameter returns the maximum vertical resolution.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION (0xa0830102) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS {
    NvU32 headIndex;
    NvU32 maxHResolution;
    NvU32 maxVResolution;
} NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS;

/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_DEFAULT_EDID
 * 
 * This command returns the default EDID for GRID
 * displayless path.
 *
 *   pEdidBuffer
 *     This field provides a pointer to a buffer into which the
 *     default EDID for GRID displayless is retrieved.
 *   edidSize
 *     This field is used as an input/output. It specifies the maximum size of
 *     pEdidBuffer. If edidSize is set to zero on input, the actual required
 *     size of buffer will be updated in this field before returning.
 *     If however, edidSize is non-zero and smaller than that required, an 
 *     error will be returned indicating the buffer was too small.
 *   connectorType
 *     This field is used as an input. It takes the connector information
 *     for which EDID for GRID displayless is queried.
 *     for DVI_D, Digital EDID is returned
 *     for VGA_15_PIN, Analog EDID is returned
 *     Both Digital and Analog EDIDs have same set of resolutions
 * 
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NVOS_ERR_BUFFER_TOO_SMALL
 */

#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_DEFAULT_EDID (0xa0830103) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pEdidBuffer, 8);
    NvU32 edidSize;
    NvU8  connectorType;
} NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS;

/* valid connector type values. Keeping DVI as the default one for displayless */
#define NVA083_CTRL_CMD_CONNECTOR_INFO_TYPE_DVI_D      (0x00)
#define NVA083_CTRL_CMD_CONNECTOR_INFO_TYPE_VGA_15_PIN (0x01)

/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_IS_ACTIVE
 *
 * This command to query if any display is active or not
 *
 * Parameters:
 *
 *   isDisplayActive
 *     Indicates if display is currently active or not.
 *
 *   Possible status values returned are:
 *     NVOS_STATUS_SUCCESS
 */
#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_IS_ACTIVE      (0xa0830104) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS {
    NvBool isDisplayActive;
} NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS;

/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_IS_CONNECTED
 *
 * This command to query if any display is connected or not
 *
 * Parameters:
 *
 *   isDisplayConnected
 *     Indicates if display is currently connected or not.
 *
 *   Possible status values returned are:
 *     NVOS_STATUS_SUCCESS
 */
#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_IS_CONNECTED (0xa0830105) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS {
    NvU32 isDisplayConnected;
} NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS;


/*
 * NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_MAX_PIXELS
 *
 * This command returns the maximum pixel count for on a GPU with
 * Displayless path enabled OR on a virtual GPU.
 *
 *   maxPixels
 *     This parameter returns the max pixel limit for a displayless GPU
 *     OR a vGPU
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_MAX_PIXELS (0xa0830106) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 maxPixels, 8);
} NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS;

/* _ctrla083_h_ */
