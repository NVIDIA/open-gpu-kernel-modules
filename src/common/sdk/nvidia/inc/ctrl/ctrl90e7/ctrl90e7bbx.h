/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl90e7/ctrl90e7bbx.finn
//

#include "nvfixedtypes.h"
#include "ctrl/ctrl90e7/ctrl90e7base.h"



/*
 * NV90E7_CTRL_CMD_BBX_GET_LAST_FLUSH_TIME
 *
 * This command is used to query the last BBX flush timestamp and duration. If BBX has not yet
 * been flushed, the status returned is NV_ERR_NOT_READY.
 *
 *   timestamp
 *     This parameter specifies the start timestamp of the last BBX flush.
 *
 *   durationUs
 *     This parameter specifies the duration (us) of the last BBX flush.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_READY
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV90E7_CTRL_CMD_BBX_GET_LAST_FLUSH_TIME (0x90e70113) /* finn: Evaluated from "(FINN_GF100_SUBDEVICE_INFOROM_BBX_INTERFACE_ID << 8) | NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS_MESSAGE_ID" */

#define NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 timestamp, 8);
    NvU32 durationUs;
} NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS;



