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
// Source file:      ctrl/ctrla0bc.finn
//

#include "ctrl/ctrlxxxx.h"
/* NVENC_SW_SESSION control commands and parameters */
#define NVA0BC_CTRL_CMD(cat,idx)                            NVXXXX_CTRL_CMD(0xA0BC, NVA0BC_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVA0BC_CTRL_RESERVED                         (0x00)
#define NVA0BC_CTRL_NVENC_SW_SESSION                 (0x01)

#define NVA0BC_CTRL_CMD_NVENC_MAX_BUFFER_ENTRIES     60

/*
 * NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO
 *
 * This command is used to let RM know about runtime information about
 * running NVENC session on given GPU.
 *  *
 *   hResolution
 *     This parameter specifies the current horizontal resolution of NVENC session.
 *   vResolution
 *     This parameter specifies the current vertical resolution of NVENC session.
 *   averageEncodeLatency
 *     This field specifies the average encode latency over last 1 second.
 *   averageEncodeFps
 *     This field specifies the average encode FPS over last 1 second.
 *   timestampBufferSize
 *     This field specifies the number of entries in the caller's timestampBuffer.
 *     It should not be greater than NVA0BC_CTRL_CMD_NVENC_MAX_BUFFER_ENTRIES.
 *     When this field is zero, RM will assume that client has calculated averageEncodeFps
 *     and averageEncodeLatency, thus ignore timestampBuffer.
 *   timestampBuffer
 *     This field specifies a pointer in the caller's address space
 *     to the buffer holding encode timestamps in microseconds.
 *     This buffer must be at least as big as timestampBufferSize multiplied
 *     by the size of the NVA0BC_CTRL_NVENC_TIMESTAMP structure.
 *     e.g. if there are 10 fps, buffer will contain only 10 entries and rest of
 *     entries should be 0x00. However if there are more than 60 fps, buffer will
 *     contain last/latest 60 entries of frame encoding start-end timestamps. Caller
 *     should make sure timestamps won't wrap around. RM assume that for each
 *     frame timestamp value endTime would be greater than startTime.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO (0xa0bc0101) /* finn: Evaluated from "(FINN_NVENC_SW_SESSION_NVENC_SW_SESSION_INTERFACE_ID << 8) | NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_MESSAGE_ID" */

typedef struct NVA0BC_CTRL_NVENC_TIMESTAMP {
    NV_DECLARE_ALIGNED(NvU64 startTime, 8);
    NV_DECLARE_ALIGNED(NvU64 endTime, 8);
} NVA0BC_CTRL_NVENC_TIMESTAMP;

#define NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS {
    NvU32 hResolution;
    NvU32 vResolution;
    NvU32 averageEncodeLatency;
    NvU32 averageEncodeFps;
    NvU32 timestampBufferSize;
    NV_DECLARE_ALIGNED(NvP64 timestampBuffer, 8);
} NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS;

/*
* NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO_V2
*
* This command updates the NVENC software sessions information for the associate GPU.
* This command is similar to NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO but doesn't have
* embedded pointers.
*
* Check NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO for detailed information.
*/

#define NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO_V2 (0xa0bc0102) /* finn: Evaluated from "(FINN_NVENC_SW_SESSION_NVENC_SW_SESSION_INTERFACE_ID << 8) | NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS_MESSAGE_ID" */

#define NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS {
    NvU32 hResolution;
    NvU32 vResolution;
    NvU32 averageEncodeLatency;
    NvU32 averageEncodeFps;
    NvU32 timestampBufferSize;
    NV_DECLARE_ALIGNED(NVA0BC_CTRL_NVENC_TIMESTAMP timestampBuffer[NVA0BC_CTRL_CMD_NVENC_MAX_BUFFER_ENTRIES], 8);
} NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS;

/* _ctrla0bc_h_ */
