/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrla0bd.finn
//

#include "ctrl/ctrlxxxx.h"
/* NVFBC_SW_SESSION control commands and parameters */
#define NVA0BD_CTRL_CMD(cat,idx)                            NVXXXX_CTRL_CMD(0xA0BD, NVA0BD_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVA0BD_CTRL_RESERVED                         (0x00)
#define NVA0BD_CTRL_NVFBC_SW_SESSION                 (0x01)

#define NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES  60

/*
 * NVA0BD_CTRL_CMD_NVFBC_SW_SESSION_UPDATE_INFO
 *
 * This command is used to let RM know about runtime information about
 * NVFBC session on given GPU.
 *  *
  *   hResolution
 *     This parameter specifies the current horizontal resolution of NVFBC session.
 *   vResolution
 *     This parameter specifies the current vertical resolution of NVFBC session.
 *   captureCallFlags
 *     This field specifies the flags associated with the capture call and the session.
 *     One of the flags specifies whether the user made the capture with wait or not.
 *   totalGrabCalls
 *     This field specifies the total number of grab calls made by the user.
 *   averageLatency
 *     This field specifies the average capture latency over last 1 second.
 *   averageFPS
 *     This field specifies the average frames captured.
  *   timestampEntryCount
 *     This field specifies the number of entries in the timestampEntry array.
 *     It should not be greater than NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES.
 *     When this field is zero, RM will assume that client has calculated averageFBCFps
 *     and averageFBCLatency, thus ignore timestampEntry array.
 *   timestampEntry
 *     This field specifies a array holding capture timestamps in microseconds.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA0BD_CTRL_CMD_NVFBC_SW_SESSION_UPDATE_INFO (0xa0bd0101) /* finn: Evaluated from "(FINN_NVFBC_SW_SESSION_NVFBC_SW_SESSION_INTERFACE_ID << 8) | NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS_MESSAGE_ID" */

typedef struct NVA0BD_CTRL_NVFBC_TIMESTAMP {
    NV_DECLARE_ALIGNED(NvU64 startTime, 8);
    NV_DECLARE_ALIGNED(NvU64 endTime, 8);
} NVA0BD_CTRL_NVFBC_TIMESTAMP;

#define NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS {
    NvU32 hResolution;
    NvU32 vResolution;
    NvU32 captureCallFlags;
    NvU32 totalGrabCalls;
    NvU32 averageLatency;
    NvU32 averageFPS;
    NvU32 timestampEntryCount;
    NV_DECLARE_ALIGNED(NVA0BD_CTRL_NVFBC_TIMESTAMP timestampEntry[NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES], 8);
} NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS;

#define NVA0BD_NVFBC_WITH_WAIT                    1:0
#define NVA0BD_NVFBC_WITH_WAIT_FALSE    (0x00000000)
#define NVA0BD_NVFBC_WITH_WAIT_INFINITE (0x00000001)
#define NVA0BD_NVFBC_WITH_WAIT_TIMEOUT  (0x00000010)

/* _ctrla0bd_h_ */
