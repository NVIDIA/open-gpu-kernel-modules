/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080pmgr.finn
//



#define NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_SUPPORTED  0U
#define NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_SUPPORTED      1U
#define NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_APPLICABLE 2U

/*!
 * NV2080_CTRL_PMGR_MODULE_INFO_PARAMS
 *
 * This provides information about different module properties
 *
 * moduleId[OUT]
 *     - This is a static HW identifier that is unique for each module on a given baseboard.
 *       For non-baseboard products this would always be 0.
 * nvswitchSupport[OUT]
 *     - NVSwitch present or not. Possible values are
 *        NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_SUPPORTED
 *        NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_SUPPORTED
 *        NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_APPLICABLE
 */
#define NV2080_CTRL_PMGR_MODULE_INFO_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_PMGR_MODULE_INFO_PARAMS {
    NvU32 moduleId;
    NvU8  nvswitchSupport;
} NV2080_CTRL_PMGR_MODULE_INFO_PARAMS;

/*!
 * NV2080_CTRL_CMD_PMGR_GET_MODULE_INFO
 *
 * Control call to query the subdevice module INFO.
 *
 */
#define NV2080_CTRL_CMD_PMGR_GET_MODULE_INFO (0x20802609) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PMGR_INTERFACE_ID << 8) | NV2080_CTRL_PMGR_MODULE_INFO_PARAMS_MESSAGE_ID" */

