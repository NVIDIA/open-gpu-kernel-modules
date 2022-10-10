/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080gsp.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX GSP control commands and parameters */

/*
 * NV2080_CTRL_CMD_GSP_GET_FEATURES
 *
 * This command is used to determine which GSP features are
 * supported on this GPU.
 *
 *   gspFeatures
 *     Bit mask that specifies GSP features supported.
 *   bValid
 *     If this field is set to NV_TRUE, then above bit mask is
 *     considered valid. Otherwise, bit mask should be ignored
 *     as invalid. bValid will be set to NV_TRUE when RM is a
 *     GSP client with GPU support offloaded to GSP firmware.
 *   bDefaultGspRmGpu
 *     If this field is set to NV_TRUE, it indicates that the
 *     underlying GPU has GSP-RM enabled by default. If set to NV_FALSE,
 *     it indicates that the GPU has GSP-RM disabled by default.
 *   firmwareVersion
 *     This field contains the buffer into which the firmware build version
 *     should be returned, if GPU is offloaded. Otherwise, the buffer
 *     will remain untouched.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_GSP_GET_FEATURES    (0x20803601) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GSP_INTERFACE_ID << 8) | NV2080_CTRL_GSP_GET_FEATURES_PARAMS_MESSAGE_ID" */

#define NV2080_GSP_MAX_BUILD_VERSION_LENGTH (0x0000040)

#define NV2080_CTRL_GSP_GET_FEATURES_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_GSP_GET_FEATURES_PARAMS {
    NvU32  gspFeatures;
    NvBool bValid;
    NvBool bDefaultGspRmGpu;
    NvU8   firmwareVersion[NV2080_GSP_MAX_BUILD_VERSION_LENGTH];
} NV2080_CTRL_GSP_GET_FEATURES_PARAMS;

/* Valid feature values */
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED                    0:0
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED_FALSE (0x00000000)
#define NV2080_CTRL_GSP_GET_FEATURES_UVM_ENABLED_TRUE  (0x00000001)

// _ctrl2080gsp_h_
