/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0080/ctrl0080msenc.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/* NV01_DEVICE_XX/NV03_DEVICE MSENC control commands and parameters */

/*
 * NV0080_CTRL_CMD_MSENC_GET_CAPS
 *
 * This command returns the set of MSENC capabilities for the device
 * in the form of an array of unsigned bytes. MSENC capabilities
 * include supported features and required workarounds for the MSENC-related
 * engine(s) within the device, each represented by a byte offset into
 * the table and a bit position within that byte.
 *
 *   capsTblSize
 *     This parameter specifies the size in bytes of the caps table.
 *     This value should be set to NV0080_CTRL_MSENC_CAPS_TBL_SIZE.
 *   capsTbl
 *     This parameter specifies a pointer to the client's caps table buffer
 *     into which the MSENC caps bits will be transferred by the RM.
 *     The caps table is an array of unsigned bytes.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0080_CTRL_CMD_MSENC_GET_CAPS (0x801b01) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_MSENC_INTERFACE_ID << 8) | NV0080_CTRL_MSENC_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_MSENC_GET_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0080_CTRL_MSENC_GET_CAPS_PARAMS {
    NvU32 capsTblSize;
    NV_DECLARE_ALIGNED(NvP64 capsTbl, 8);
} NV0080_CTRL_MSENC_GET_CAPS_PARAMS;



/* size in bytes of MSENC caps table */
#define NV0080_CTRL_MSENC_CAPS_TBL_SIZE 4

/* _ctrl0080msenc_h_ */
