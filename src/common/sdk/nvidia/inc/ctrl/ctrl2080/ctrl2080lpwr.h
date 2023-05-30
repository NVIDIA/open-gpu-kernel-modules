/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080lpwr.finn
//



/*!
 * NV2080_CTRL_CMD_LPWR_DIFR_CTRL
 *
 * This command is used to control the DIFR
 * feature behavior.
 *
 */
#define NV2080_CTRL_CMD_LPWR_DIFR_CTRL            (0x20802801) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_LPWR_INTERFACE_ID << 8) | NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS_MESSAGE_ID" */

/*!
 * @brief Various Values for control
 */
// Disable the DIFR
#define NV2080_CTRL_LPWR_DIFR_CTRL_DISABLE        (0x00000001)
// Enable the DIFR
#define NV2080_CTRL_LPWR_DIFR_CTRL_ENABLE         (0x00000002)

// Support status for DIFR
#define NV2080_CTRL_LPWR_DIFR_CTRL_SUPPORT_STATUS (0x00000003)

/*!
 * Structure containing DIFR control call Parameters
 */
#define NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS {
    NvU32 ctrlParamVal;
} NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS;

// Values for the SUPPORT Control Status
#define NV2080_CTRL_LPWR_DIFR_SUPPORTED                          (0x00000001)
#define NV2080_CTRL_LPWR_DIFR_NOT_SUPPORTED                      (0x00000002)

/*!
 * NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE
 *
 * This control call is used to send the prefetch response
 *
 */
#define NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE              (0x20802802) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_LPWR_INTERFACE_ID << 8) | NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS_MESSAGE_ID" */

/*!
 * @brief Various Values of Reponses for Prefetch Status
 */

// Prefetch is successfull.
#define NV2080_CTRL_LPWR_DIFR_PREFETCH_SUCCESS                   (0x00000001)
// OS Filps are enabled, so prefetch can not be done.
#define NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_OS_FLIPS_ENABLED     (0x00000002)
// Current Display surface can not fit in L2
#define NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_INSUFFICIENT_L2_SIZE (0x00000003)
// Fatal and un recoverable Error
#define NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_CE_HW_ERROR          (0x00000004)

/*!
 * Structure containing DIFR prefetch response control call Parameters
 */
#define NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS {
    NvU32 responseVal;
} NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS;


// _ctrl2080lpwr_h_

