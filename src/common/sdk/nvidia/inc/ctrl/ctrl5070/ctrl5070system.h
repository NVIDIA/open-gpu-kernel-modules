/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl5070/ctrl5070system.finn
//

#include "ctrl/ctrl5070/ctrl5070base.h"
#include "ctrl/ctrl5070/ctrl5070common.h" // NV5070_CTRL_CMD_MAX_HEADS

/* extract cap bit setting from tbl */
#define NV5070_CTRL_SYSTEM_GET_CAP(tbl,c)         (((NvU8)tbl[(1?c)]) & (0?c)) 

/* caps format is byte_index:bit_mask */
#define NV5070_CTRL_SYSTEM_CAPS_BUG_237734_REQUIRES_DMI_WAR                 0:0x01 // Deprecated
#define NV5070_CTRL_SYSTEM_CAPS_STEREO_DIN_AVAILABLE                        0:0x02
#define NV5070_CTRL_SYSTEM_CAPS_BUG_381003_MULTIWAY_AFR_WAR                 0:0x04
#define NV5070_CTRL_SYSTEM_CAPS_BUG_538079_COLOR_COMPRESSION_SUPPORTED      0:0x08 // Deprecated
#define NV5070_CTRL_SYSTEM_CAPS_BUG_2052012_GLITCHY_MCLK_SWITCH             0:0x10
#define NV5070_CTRL_SYSTEM_CAPS_DEEP_COLOR_SUPPORT                          0:0x20
#define NV5070_CTRL_SYSTEM_CAPS_BUG_644815_DNISO_VIDMEM_ONLY                0:0x40


/* size in bytes of display caps table */
#define NV5070_CTRL_SYSTEM_CAPS_TBL_SIZE   1

/*
 * NV5070_CTRL_CMD_SYSTEM_GET_CAPS_V2
 *
 * This command returns the set of display capabilities for the parent device
 * in the form of an array of unsigned bytes.  Display capabilities
 * include supported features and required workarounds for the display
 * engine(s) within the device, each represented by a byte offset into the
 * table and a bit position within that byte.  The set of display capabilities
 * will be normalized across all GPUs within the device (a feature capability
 * will be set only if it's supported on all GPUs while a required workaround
 * capability will be set if any of the GPUs require it).
 *
 *   [out] capsTbl
 *     This caps table array is where the display cap bits will be transferred
 *     by the RM. The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV5070_CTRL_CMD_SYSTEM_GET_CAPS_V2 (0x50700709) /* finn: Evaluated from "(FINN_NV50_DISPLAY_SYSTEM_INTERFACE_ID << 8) | NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;

    NvU8                        capsTbl[NV5070_CTRL_SYSTEM_CAPS_TBL_SIZE];
} NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS;

/* _ctrl5070system_h_ */
