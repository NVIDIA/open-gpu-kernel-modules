/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080bios.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX bios-related control commands and parameters */



typedef struct NV2080_CTRL_BIOS_INFO {
    NvU32 index;
    NvU32 data;
} NV2080_CTRL_BIOS_INFO;

/* Maximum number of bios infos that can be queried at once */
#define NV2080_CTRL_BIOS_INFO_MAX_SIZE                              (0x0000000F)

#define NV2080_CTRL_BIOS_INFO_INDEX_REVISION                        (0x00000000)
#define NV2080_CTRL_BIOS_INFO_INDEX_OEM_REVISION                    (0x00000001)



/*
 * NV2080_CTRL_CMD_BIOS_GET_INFO
 *
 * This command returns bios information for the associated GPU.
 * Requests to retrieve bios information use a list of one or more
 * NV2080_CTRL_BIOS_INFO structures.
 *
 *   biosInfoListSize
 *     This field specifies the number of entries on the caller's
 *     biosInfoList.
 *   biosInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the bios information is to be returned.
 *     This buffer must be at least as big as biosInfoListSize multiplied
 *     by the size of the NV2080_CTRL_BIOS_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_BIOS_GET_INFO                               (0x20800802) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | 0x2" */

typedef struct NV2080_CTRL_BIOS_GET_INFO_PARAMS {
    NvU32 biosInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 biosInfoList, 8);
} NV2080_CTRL_BIOS_GET_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_BIOS_GET_INFO_V2
 *
 * This command returns bios information for the associated GPU.
 * Requests to retrieve bios information use a list of one or more
 * NV2080_CTRL_BIOS_INFO structures.
 *
 *   biosInfoListSize
 *     This field specifies the number of entries on the caller's
 *     biosInfoList.
 *   biosInfoList
 *     Bios information to be returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_BIOS_GET_INFO_V2 (0x20800810) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS_MESSAGE_ID (0x10U)

typedef struct NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS {
    NvU32                 biosInfoListSize;
    NV2080_CTRL_BIOS_INFO biosInfoList[NV2080_CTRL_BIOS_INFO_MAX_SIZE];
} NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS;



/*
 * NV2080_CTRL_CMD_BIOS_GET_SKU_INFO
 *
 * This command returns information about the current board SKU.
 * NV_ERR_INVALID_OWNER will be returned if the call
 * isn't made with the OS as the administrator.
 *
 *  chipSKU
 *    This field returns the sku for the current chip.
 *  chipSKUMod
 *    This field returns the SKU modifier.
 *  project
 *    This field returns the Project (Board) number.
 *  projectSKU
 *    This field returns the Project (Board) SKU number.
 *  CDP
 *    This field returns the Collaborative Design Project Number.
 *  projectSKUMod
 *    This field returns the Project (Board) SKU Modifier.
 *  businessCycle
 *    This field returns the business cycle the board is associated with.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OWNER
 */
#define NV2080_CTRL_CMD_BIOS_GET_SKU_INFO (0x20800808) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_MESSAGE_ID" */

/* maximum length of parameter strings */


#define NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS {
    NvU32 BoardID;
    char  chipSKU[4];
    char  chipSKUMod[2];
    char  project[5];
    char  projectSKU[5];
    char  CDP[6];
    char  projectSKUMod[2];
    NvU32 businessCycle;
} NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_BIOS_GET_POST_TIME

 * This command is used to get the GPU POST time (in milliseconds).
 * If the associated GPU is the master GPU this value will be recorded
 * by the VBIOS and retrieved from the KDA buffer.  If the associated
 * GPU is a secondaryGPU then this value will reflect the devinit
 * processing time.
 *
 * vbiosPostTime
 *   This parameter returns the vbios post time in msec.
 *
 * Possible return status values are
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV2080_CTRL_CMD_BIOS_GET_POST_TIME (0x20800809) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 vbiosPostTime, 8);
} NV2080_CTRL_CMD_BIOS_GET_POST_TIME_PARAMS;



/*
 * NV2080_CTRL_CMD_BIOS_GET_UEFI_SUPPORT
 *
 * This function is used to give out the UEFI version, UEFI image presence and
 * Graphics Firmware Mode i.e. whether system is running in UEFI or not.
 *
 *   version
 *     This parameter returns the UEFI version.
 *
 *   flags
 *     This parameter indicates UEFI image presence and Graphics Firmware mode.
 *       NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE
 *         This field returns UEFI presence value. Legal values for this
 *         field include:
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_NO
 *             This value indicates that UEFI image is not present.
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_YES
 *             This value indicates that UEFI image is present.
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_PLACEHOLDER
 *             This value indicates that there is a dummy UEFI placeholder,
 *             which can later be updated with a valid UEFI image.
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_HIDDEN
 *             This value indicates that UEFI image is hidden.
 *       NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING
 *         This field indicates the UEFI running value. Legal values for
 *         this parameter include:
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING_FALSE
 *             This value indicates that UEFI is not running.
 *           NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING_TRUE
 *             This value indicates that UEFI is running.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_READY
 *   NV_ERR_INVALID_STATE
 */

#define NV2080_CTRL_CMD_BIOS_GET_UEFI_SUPPORT (0x2080080b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS {
    NvU32 version;
    NvU32 flags;
} NV2080_CTRL_BIOS_GET_UEFI_SUPPORT_PARAMS;

/* Legal values for flags parameter */
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE               1:0
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_NO          (0x00000000)
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_YES         (0x00000001)
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_PLACEHOLDER (0x00000002)
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_PRESENCE_HIDDEN      (0x00000003)
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING                2:2
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING_FALSE        (0x00000000)
#define NV2080_CTRL_BIOS_UEFI_SUPPORT_FLAGS_RUNNING_TRUE         (0x00000001)



/* _ctrl2080bios_h_ */
