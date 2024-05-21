/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fucodecoverage.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/* Numerical ID for each ucode */
#define NV208F_UCODE_COVERAGE_SEC2          1
#define NV208F_UCODE_COVERAGE_PMU           2
#define NV208F_UCODE_COVERAGE_GSP_TASK_RM   3
#define NV208F_UCODE_COVERAGE_GSP_TASK_VGPU 4


/*
 * NV208F_CTRL_UCODE_COVERAGE_STATE_PARAMS
 *
 * Parameters struct shared by the control calls
 * NV208F_CTRL_CMD_UCODE_COVERAGE_GET_STATE and
 * NV208F_CTRL_CMD_UCODE_COVERAGE_SET_STATE.
 */
typedef struct NV208F_CTRL_UCODE_COVERAGE_STATE_PARAMS {
    NvU32  ucode;
    NvU32  gfid;
    NvBool bEnabled;
    NvBool bClear;
} NV208F_CTRL_UCODE_COVERAGE_STATE_PARAMS;

/*
 * NV208F_CTRL_CMD_UCODE_COVERAGE_GET_STATE
 *
 * Retrieves the status of the target ucode's coverage
 *
 *   ucode     numeric id of the desired ucode to target
 *   gfid      specifies which partition to send the command to
 *             (applies to ucode=NV208F_UCODE_COVERAGE_GSP_TASK_VGPU only)
 *   bEnabled  whether the ucode's SanitizerCoverage run-time data gathering
 *             is enabled (NV_TRUE) or not (NV_FALSE)
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV208F_CTRL_CMD_UCODE_COVERAGE_GET_STATE (0x208f1901) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_UCODE_COVERAGE_INTERFACE_ID << 8) | NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS_MESSAGE_ID (0x1U)

typedef NV208F_CTRL_UCODE_COVERAGE_STATE_PARAMS NV208F_CTRL_UCODE_COVERAGE_GET_STATE_PARAMS;

/*
 * NV208F_CTRL_CMD_UCODE_COVERAGE_SET_STATE
 *
 * Enables/disables and optionally clears coverage data of the target ucode
 *
 *   ucode     numeric id of the desired ucode to target
 *   gfid      specifies which partition to send the command to
 *             (applies to ucode=NV208F_UCODE_COVERAGE_GSP_TASK_VGPU only)
 *   bEnabled  whether to enable (NV_TRUE) or disable (NV_FALSE) coverage for the given ucode
 *   bClear    whether to clear coverage prior to enabling enabling coverage collection
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV208F_CTRL_CMD_UCODE_COVERAGE_SET_STATE (0x208f1902) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_UCODE_COVERAGE_INTERFACE_ID << 8) | NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS_MESSAGE_ID (0x2U)

typedef NV208F_CTRL_UCODE_COVERAGE_STATE_PARAMS NV208F_CTRL_UCODE_COVERAGE_SET_STATE_PARAMS;

/*
 * NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_*
 *
 * Maximum number of bytes that can be retrieved by one RPC call
 * (potentially differs per-ucode). These are used to size the buffer in the
 * respective ucode RPC parameters.
 */
#define NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_PMU   128
#define NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_GSPRM 65536

/*
 * NV208F_CTRL_CMD_UCODE_COVERAGE_GET_DATA
 *
 * Retrieves coverage data of the target ucode
 *
 *   data         buffer to retrieve data into
 *   ucode        numeric id of the desired ucode to retrieve data from
 *   gfid         specifies which partition to send the command to
 *                (applies to ucode=NV208F_UCODE_FUZZER_GSP_TASK_VGPU only)
 *   offset       offset of internal buffer to copy from
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV208F_CTRL_CMD_UCODE_COVERAGE_GET_DATA   (0x208f1903) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_UCODE_COVERAGE_INTERFACE_ID << 8) | NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS {
    NvU8  data[NV208F_UCODE_COVERAGE_RPC_MAX_BYTES_GSPRM];
    NvU32 ucode;
    NvU32 gfid;
    NvU32 offset;
} NV208F_CTRL_UCODE_COVERAGE_GET_DATA_PARAMS;

/* _ctrl208fucodecoverage_h_ */
