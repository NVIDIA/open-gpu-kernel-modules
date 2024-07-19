/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fbif.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
* NV208F_CTRL_CMD_BIF_PBI_WRITE_COMMAND
*
* Control command to send a write command to the Post Box Interface
*
* Parameters:
*
*cmdFuncId
*   this specifies the function that needs to be performed on pbi
*data
*   the data to be set in the data in register
* status
*   this corresponds to pbi status register
* sysNotify
*   this corresponds to system notify event, i.e. whether system
*   needs to be notified of command completion
* drvNotify
*   this corresponds to driver notify event, i.e. whether driver
*   needs to be notified of command completion
*
* For the possible values of the above parameters refer rmpbicmdif.h
*/
#define NV208F_CTRL_CMD_BIF_PBI_WRITE_COMMAND (0x208f0701) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS {
    NvU8   cmdFuncId;
    NvU32  data;
    NvU8   status;
    NvBool sysNotify;
    NvBool drvNotify;
} NV208F_CTRL_BIF_PBI_WRITE_COMMAND_PARAMS;

/*
* NV208F_CTRL_CMD_BIF_CONFIG_REG_READ
*   This command is used to read any of the PBI registers in the config space
*
* Parameters:
*
* RegIndex
*   Defines the index of the PBI register
* data
*   Data that is read
*/
#define NV208F_CTRL_CMD_BIF_CONFIG_REG_READ (0x208f0702) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS {
    NvU8  RegIndex;
    NvU32 data;
} NV208F_CTRL_BIF_CONFIG_REG_READ_PARAMS;

/*
* NV208F_CTRL_CMD_BIF_CONFIG_REG_WRITE
*   This command is used to write any of the PBI registers in the config space
*
* Parameters:
*
* RegIndex
*   Defines the index of the PBI register
* data
*   Data that is to be written
*/
#define NV208F_CTRL_CMD_BIF_CONFIG_REG_WRITE (0x208f0703) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS {
    NvU8  RegIndex;
    NvU32 data;
} NV208F_CTRL_BIF_CONFIG_REG_WRITE_PARAMS;

/*
* NV208F_CTRL_CMD_BIF_INFO
*   This command is used to read a bif property
*
* Parameters:
*
* index
*   Defines the index of the property to read
* data
*   Data that is read
*/
#define NV208F_CTRL_CMD_BIF_INFO (0x208f0704) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_INFO_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_BIF_INFO_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV208F_CTRL_BIF_INFO_PARAMS {
    NvU32 index;
    NvU32 data;
} NV208F_CTRL_BIF_INFO_PARAMS;

/* valid bif info index values */
#define NV208F_CTRL_BIF_INFO_INDEX_L0S_ENABLED   (0x00000000)
#define NV208F_CTRL_BIF_INFO_INDEX_L1_ENABLED    (0x00000001)

/*
 * NV208F_CTRL_CMD_BIF_ECC_INJECT_ERROR
 *
 * This ctrl call injects PCI-E XPL-EP ECC errors.  Please see the confluence
 * page "ECC" for more information on ECC and ECC injection:
 *
 * Parameters:
 *
 * errorUnit
 *   Specifies the HW unit where the injection will occur.
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
#define NV208F_CTRL_CMD_BIF_ECC_INJECT_ERROR     (0x208f0705) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID" */



#define NV208F_CTRL_BIF_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV208F_CTRL_BIF_ECC_INJECT_ERROR_PARAMS {
    NvU8 errorType;
    NvU8 errorUnit;
} NV208F_CTRL_BIF_ECC_INJECT_ERROR_PARAMS;

/*
 * NV208F_CTRL_CMD_BIF_ECC_INJECTION_SUPPORTED
 *
 * Reports if error injection is supported for XPL-EP
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
#define NV208F_CTRL_CMD_BIF_ECC_INJECTION_SUPPORTED (0x208f0706) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_BIF_INTERFACE_ID << 8) | NV208F_CTRL_BIF_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_BIF_ECC_INJECTION_SUPPORTED_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV208F_CTRL_BIF_ECC_INJECTION_SUPPORTED_PARAMS {
    NvU8   errorUnit;
    NvBool bCorrectableSupported;
    NvBool bUncorrectableSupported;
} NV208F_CTRL_BIF_ECC_INJECTION_SUPPORTED_PARAMS;

/* _ctrl208fbif_h_ */

