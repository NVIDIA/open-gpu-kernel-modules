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
// Source file:      ctrl/ctrl208f/ctrl208fmmu.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_MMU_ECC_INJECT_ERROR
 *
 * This ctrl call injects MMU ECC errors.  Please see the confluence
 * page "ECC" for more information on ECC and ECC injection:
 *
 * Parameters:
 *
 * location
 *   Only used for HSHUBMMU.
 *
 * sublocation
 *   Only used for HSHUBMMU.
 *
 * unit
 *   Specifies the MMU HW unit where the injection will occur.
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
#define NV208F_CTRL_CMD_MMU_ECC_INJECT_ERROR (0x208f0b01) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_MMU_INTERFACE_ID << 8) | NV208F_CTRL_MMU_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_MMU_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_MMU_ECC_INJECT_ERROR_PARAMS {
    NvU32 location;
    NvU32 sublocation;
    NvU8  unit;
    NvU8  errorType;
    NvU8  instance;
} NV208F_CTRL_MMU_ECC_INJECT_ERROR_PARAMS;



/*
 * NV208F_CTRL_CMD_MMU_ECC_INJECTION_SUPPORTED
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
 *      NV_ERR_INVALID_ARGUMENT if the requested location is invalid.
 *      NV_ERR_INSUFFICIENT_PERMISSIONS if priv write not enabled.
 *      NV_ERR_NOT_SUPPORTED otherwise
 *
 */
#define NV208F_CTRL_CMD_MMU_ECC_INJECTION_SUPPORTED                 (0x208f0b02) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_MMU_INTERFACE_ID << 8) | NV208F_CTRL_MMU_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID" */



#define NV208F_CTRL_MMU_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV208F_CTRL_MMU_ECC_INJECTION_SUPPORTED_PARAMS {
    NvU8   unit;
    NvBool bCorrectableSupported;
    NvBool bUncorrectableSupported;
} NV208F_CTRL_MMU_ECC_INJECTION_SUPPORTED_PARAMS;

/*
 * NV208F_CTRL_CMD_MMU_GET_NUM_HSHUBMMUS
 *
 * Returns the number of ECC Capable HSHUBMMUS.
 *
 * numHshubmmus [out]
 *   Number of Hshubmmus
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV208F_CTRL_CMD_MMU_GET_NUM_HSHUBMMUS (0x208f0b03) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_MMU_INTERFACE_ID << 8) | NV208F_CTRL_MMU_GET_NUM_HSHUBMMUS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_MMU_GET_NUM_HSHUBMMUS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_MMU_GET_NUM_HSHUBMMUS_PARAMS {
    NvU32 numHshubmmus;
} NV208F_CTRL_MMU_GET_NUM_HSHUBMMUS_PARAMS;

/*
* NV208F_CTRL_CMD_MMU_GET_NUM_HUBMMUS
*
* Returns the number of ECC Capable HUBMMUS.
*
* numHubmmus [out]
*   Number of Hubmmus
*
* Possible status values returned are:
*   NV_OK
*   NV_ERR_NOT_SUPPORTED
*/
#define NV208F_CTRL_CMD_MMU_GET_NUM_HUBMMUS (0x208f0b04) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_MMU_INTERFACE_ID << 8) | NV208F_CTRL_MMU_GET_NUM_HUBMMUS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_MMU_GET_NUM_HUBMMUS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV208F_CTRL_MMU_GET_NUM_HUBMMUS_PARAMS {
    NvU32 numHubmmus;
} NV208F_CTRL_MMU_GET_NUM_HUBMMUS_PARAMS;

/* _ctrl208fmmu_h_ */
