/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fpmu.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_PMU_ECC_INJECT_ERROR
 *
 * This ctrl call injects PMU ECC errors.  Please see the confluence
 * page "ECC" for more information on ECC and ECC injection.
 *
 * Parameters:
 *
 * location
 *   Specifies the PMU HW unit where the injection will occur.
 *
 * errorType
 *   Specifies whether the injected error will be correctable or uncorrectable.
 *   Correctable errors have no effect on running programs while uncorrectable
 *   errors will cause all channels to be torn down.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV208F_CTRL_CMD_PMU_ECC_INJECT_ERROR (0x208f0c01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_PMU_INTERFACE_ID << 8) | NV208F_CTRL_PMU_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_PMU_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_PMU_ECC_INJECT_ERROR_PARAMS {
    NvU8 errorType;
} NV208F_CTRL_PMU_ECC_INJECT_ERROR_PARAMS;

#define NV208F_CTRL_PMU_ECC_INJECT_ERROR_TYPE                 0:0
#define NV208F_CTRL_PMU_ECC_INJECT_ERROR_TYPE_CORRECTABLE   (0x00000000)
#define NV208F_CTRL_PMU_ECC_INJECT_ERROR_TYPE_UNCORRECTABLE (0x00000001)

/*
 * NV208F_CTRL_CMD_PMU_ECC_INJECTION_SUPPORTED
 *
 * Reports if error injection is supported for the PMU
 *
 * bCorrectableSupported [out]:
 *      Boolean value that shows if correcatable errors can be injected.
 *
 * bUncorrectableSupported [out]:
 *      Boolean value that shows if uncorrecatable errors can be injected.
 *
 * Return values:
 *      NV_OK on success
 *      NV_ERR_INSUFFICIENT_PERMISSIONS if priv write not enabled.
 *      NV_ERR_NOT_SUPPORTED otherwise
 *
 *
 */
#define NV208F_CTRL_CMD_PMU_ECC_INJECTION_SUPPORTED         (0x208f0c02) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_PMU_INTERFACE_ID << 8) | NV208F_CTRL_PMU_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_PMU_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV208F_CTRL_PMU_ECC_INJECTION_SUPPORTED_PARAMS {
    NvBool bCorrectableSupported;
    NvBool bUncorrectableSupported;
} NV208F_CTRL_PMU_ECC_INJECTION_SUPPORTED_PARAMS;

/* _ctrl208fpmu_h_ */
