/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0080/ctrl0080nvjpg.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/* NV01_DEVICE_XX/NV03_DEVICE NVJPG control commands and parameters */



/*
 * Size in bytes of NVJPG caps table.  This value should be one greater
 * than the largest byte_index value above.
 */
#define NV0080_CTRL_NVJPG_CAPS_TBL_SIZE   9

/*
 * NV0080_CTRL_CMD_NVJPG_GET_CAPS_V2
 *
 * This command returns the set of NVJPG capabilities for the device
 * in the form of an array of unsigned bytes. NVJPG capabilities
 * include supported features of the NVJPG engine(s) within the device,
 * each represented by a byte offset into the table and a bit position within
 * that byte.
 *
 *   [out] capsTbl
 *     This caps table array is where the NVJPG caps bits will be transferred
 *     by the RM. The caps table is an array of unsigned bytes.
 *   instanceId
 *     This parameter specifies the instance Id of NVDEC for which
 *     cap bits are requested. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_NVJPG_GET_CAPS_V2 (0x801f02) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_NVJPG_INTERFACE_ID << 8) | NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS {
    NvU8  capsTbl[NV0080_CTRL_NVJPG_CAPS_TBL_SIZE];
    NvU32 instanceId;
} NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS;

/* _ctrl0080NVJPG_h_ */

