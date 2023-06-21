/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0073/ctrl0073psr.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"



/*
 * NV0073_CTRL_CMD_PSR_GET_SR_PANEL_INFO
 *
 * displayId
 *    Display ID on which this information is being requested.
 * frameLockPin
 *    Returns the frame lock pin of the panel.
 * i2cAddress
 *    Returns the i2c address on which the SR panel is attached.
 *    NOTE: applies only to LVDS panels, otherwise this field
 *          should be ignored.
 * bSelfRefreshEnabled
 *    Returns whether SR is enabled in RM.
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_PSR_GET_SR_PANEL_INFO        (0x731602U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_PSR_INTERFACE_ID << 8) | NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS {
    NvU32  displayId;
    NvU32  frameLockPin;
    NvU8   i2cAddress;
    NvBool bSelfRefreshEnabled;
} NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS;



/* _ctrl0073psr_h_ */
