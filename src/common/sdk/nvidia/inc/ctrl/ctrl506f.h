/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl506f.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV50_GPFIFO control commands and parameters */

#define NV506F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x506F, NV506F_CTRL_##cat, idx)

/* NV50_GPFIFO command categories (6bits) */
#define NV506F_CTRL_RESERVED (0x00)
#define NV506F_CTRL_GPFIFO   (0x01)

/*
 * NV506F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV506F_CTRL_CMD_NULL (0x506f0000) /* finn: Evaluated from "(FINN_NV50_CHANNEL_GPFIFO_RESERVED_INTERFACE_ID << 8) | 0x0" */

/*
 * NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL
 *
 * This command resets a channel which was isolated previously by RC recovery.
 *
 *   exceptType
 *      This input parameter specifies the type of RC error that occurred. See the
 *      description of the ROBUST_CHANNEL_* values in nverror.h for valid exceptType
 *      values. info32 field of the error notifier is set with the exceptType when 
 *      the error notifier is signaled.
 *
 *   engineID
 *      This input parameter specifies the engine to be reset.  See the description 
 *      of the NV2080_ENGINE_TYPE values in cl2080.h for valid engineID values.  info16 
 *      field of the error notifier is set with the engineID when the error notifier is 
 *      signaled.    
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */


#define NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL (0x506f0105) /* finn: Evaluated from "(FINN_NV50_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS {
    NvU32 exceptType;
    NvU32 engineID;
} NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS;

/*
 * NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL
 *
 * This command is an internal command sent from Kernel RM to Physical RM
 * to perform the channel reset operations in hardware on an isolated channel
 *
 * Please see description of NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL for more information.
 *
 */
#define NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL (0x506f0106) /* finn: Evaluated from "(FINN_NV50_CHANNEL_GPFIFO_GPFIFO_INTERFACE_ID << 8) | NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS_MESSAGE_ID" */

#define NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS_MESSAGE_ID (0x6U)

typedef NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS NV506F_CTRL_CMD_INTERNAL_RESET_ISOLATED_CHANNEL_PARAMS;

/* _ctrl506f.h_ */
