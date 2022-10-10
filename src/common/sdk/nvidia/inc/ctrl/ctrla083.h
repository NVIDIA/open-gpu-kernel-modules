/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrla083.finn
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



#define NVA083_CTRL_CMD_VIRTUAL_DISPLAY_GET_MAX_PIXELS (0xa0830106) /* finn: Evaluated from "(FINN_NVA083_GRID_DISPLAYLESS_VIRTUAL_DISPLAY_INTERFACE_ID << 8) | NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS_MESSAGE_ID" */

#define NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 maxPixels, 8);
} NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS;

/* _ctrla083_h_ */
