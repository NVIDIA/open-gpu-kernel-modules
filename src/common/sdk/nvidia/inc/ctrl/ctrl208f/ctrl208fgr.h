/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fgr.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

#include "ctrl/ctrl2080/ctrl2080gr.h"



/*
 * NV208F_CTRL_CMD_GR_ECC_INJECT_ERROR
 *
 * Control command to inject a gr ecc error
 *
 * Parameters:
 *
 * location
 *   location index
 * sublocation
 *   sublocation index
 * unit
 *   unit to inject error into
 * errorType
 *   SBE or DBE
 * grRouteInfo
 *   Routing info for SMC
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_GR_ECC_INJECT_ERROR (0x208f1203) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GR_INTERFACE_ID << 8) | NV208F_CTRL_GR_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GR_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_GR_ECC_INJECT_ERROR_PARAMS {
    NvU32 location;
    NvU32 sublocation;
    NvU8  unit;
    NvU8  errorType;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV208F_CTRL_GR_ECC_INJECT_ERROR_PARAMS;



/*
 * NV208F_CTRL_CMD_GR_ECC_INJECTION_SUPPORTED
 *
 * Reports if error injection is supported for a given HW unit
 *
 * unit [in]:
 *      The ECC protected unit for which ECC injection support is being checked.
 *
 * bCorrectableSupported [out]:
 *      Boolean value that shows if correcatable errors can be injected.
 *
 * bUncorrectableSupported [out]:
 *      Boolean value that shows if uncorrecatable errors can be injected.
 *
 * Return values:
 *      NV_OK on success
 *      NV_ERR_INVALID_ARGUMENT if the requested unit is invalid.
 *      NV_ERR_INSUFFICIENT_PERMISSIONS if priv write not enabled.
 *      NV_ERR_NOT_SUPPORTED otherwise
 *
 *
 */
#define NV208F_CTRL_CMD_GR_ECC_INJECTION_SUPPORTED   (0x208f1204) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GR_INTERFACE_ID << 8) | NV208F_CTRL_GR_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GR_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV208F_CTRL_GR_ECC_INJECTION_SUPPORTED_PARAMS {
    NvU8   unit;
    NvBool bCorrectableSupported;
    NvBool bUncorrectableSupported;
    NV_DECLARE_ALIGNED(NV2080_CTRL_GR_ROUTE_INFO grRouteInfo, 8);
} NV208F_CTRL_GR_ECC_INJECTION_SUPPORTED_PARAMS;

/*
 * NV208F_CTRL_CMD_GR_ECC_SET_TRANSIENT_CLEARING_POLICY
 *
 * Control command to determine whether or not the actions to clear potential transient
 * errors in the SM should be taken
 *
 * Parameters:
 *
 * policy
 *   NV208F_CTRL_GR_ECC_TRANSIENT_CLEARING_DISABLED
 *      Don't attempt to clear a transient error in the SM
 *   NV208F_CTRL_GR_ECC_TRANSIENT_CLEARING_ENABLED
 *      Attempt to clear a transient error in the SM
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_GR_ECC_TRANSIENT_CLEARING_DISABLED       (0x00000000)
#define NV208F_CTRL_GR_ECC_TRANSIENT_CLEARING_ENABLED        (0x00000001)

#define NV208F_CTRL_CMD_GR_ECC_SET_TRANSIENT_CLEARING_POLICY (0x208f1205) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GR_INTERFACE_ID << 8) | NV208F_CTRL_GR_ECC_SET_TRANSIENT_CLEARING_POLICY_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GR_ECC_SET_TRANSIENT_CLEARING_POLICY_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV208F_CTRL_GR_ECC_SET_TRANSIENT_CLEARING_POLICY_PARAMS {
    NvU32 policy;
} NV208F_CTRL_GR_ECC_SET_TRANSIENT_CLEARING_POLICY_PARAMS;

/*
 * NV208F_CTRL_CMD_GR_INJECT_CTXSW_UCODE_ERROR
 *
 * Control command to inject a CTXSW ucode error
 *
 * Parameters:
 *
 * errorType
 *   Warning, Partial GR Reset, Full GR Reset, or GPU Reset
 *
 * errorMethod
 *   Main, Firmware, or Sideband error
 *
 * bInjectGpccs
 *   TRUE  - inject into GPCCS
 *   FALSE - inject into FECS
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_GR_INJECT_CTXSW_UCODE_ERROR (0x208f1206) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GR_INTERFACE_ID << 8) | NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS {
    NvU32  errorType;
    NvU32  errorMethod;
    NvBool bInjectGpccs;
} NV208F_CTRL_GR_INJECT_CTXSW_UCODE_ERROR_PARAMS;


/* _ctrl208fgr_h_ */
