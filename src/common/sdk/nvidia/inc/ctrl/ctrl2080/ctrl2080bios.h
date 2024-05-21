/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080bios.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX bios-related control commands and parameters */



typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_BIOS_INFO;

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
 * NV2080_CTRL_BIOS_NBSI
 *
 * NV2080_CTRL_BIOS_NBSI_MAX_REG_STRING_LENGTH
 *   This is the maximum length of a given registry string input (in characters).
 *
 * NV2080_CTRL_BIOS_NBSI_STRING_TYPE_ASCII
 *   This is a value indicating the format of a registry string is ascii.
 * NV2080_CTRL_BIOS_NBSI_STRING_TYPE_UNICODE
 *   This is a value indicating the format of a registry string is unicode.
 * NV2080_CTRL_BIOS_NBSI_STRING_TYPE_HASH
 *   This is a value indicating a registry string is actually a pre-hashed value.
 *
 * NV2080_CTRL_BIOS_NBSI_REG_STRING
 *   This is a structure used to store a registry string object.
 *   The members are as follows:
 *
 *   size
 *     This is the size (in bytes) of the data contained in the string. If this
 *     is greater than the maximum registry string length, an error will be
 *     returned.
 *   type
 *     This is the type of data contained in the registry string. It can be either
 *     ascii, unicode or a pre-hashed value.
 *   value
 *     This is the value of the string. Depending on the type, a different object
 *     will be used to access the data.
 */
#define NV2080_CTRL_BIOS_NBSI_MAX_REG_STRING_LENGTH (0x00000100)

#define NV2080_CTRL_BIOS_NBSI_STRING_TYPE_ASCII     (0x00000000)
#define NV2080_CTRL_BIOS_NBSI_STRING_TYPE_UNICODE   (0x00000001)
#define NV2080_CTRL_BIOS_NBSI_STRING_TYPE_HASH      (0x00000002)

#define NV2080_CTRL_BIOS_NBSI_MODULE_ROOT           (0x00000000)
#define NV2080_CTRL_BIOS_NBSI_MODULE_RM             (0x00000001)
#define NV2080_CTRL_BIOS_NBSI_MODULE_DISPLAYDRIVER  (0x00000002)
#define NV2080_CTRL_BIOS_NBSI_MODULE_VIDEO          (0x00000003)
#define NV2080_CTRL_BIOS_NBSI_MODULE_CPL            (0x00000004)
#define NV2080_CTRL_BIOS_NBSI_MODULE_D3D            (0x00000005)
#define NV2080_CTRL_BIOS_NBSI_MODULE_OGL            (0x00000006)
#define NV2080_CTRL_BIOS_NBSI_MODULE_PMU            (0x00000007)
#define NV2080_CTRL_BIOS_NBSI_MODULE_MODE           (0x00000008)
// this should equal the last NBSI_MODULE plus 1.
#define NV2080_CTRL_BIOS_NBSI_NUM_MODULES           (0x00000009)

//
// Never use this value! It's needed for DD/Video modules, but does not correspond
// to a valid NBSI hive!
//
#define NV2080_CTRL_BIOS_NBSI_MODULE_UNKNOWN        (0x80000000)

typedef struct NV2080_CTRL_BIOS_NBSI_REG_STRING {
    NvU32 size;
    NvU32 type;

    union {
        NvU8  ascii[NV2080_CTRL_BIOS_NBSI_MAX_REG_STRING_LENGTH];
        NvU16 unicode[NV2080_CTRL_BIOS_NBSI_MAX_REG_STRING_LENGTH];
        NvU16 hash;
    } value;
} NV2080_CTRL_BIOS_NBSI_REG_STRING;


/*
 * NV2080_CTRL_CMD_BIOS_GET_NBSI
 *
 * module
 *   This field specifies the given module per the MODULE_TYPES enum.
 * path
 *   This field specifies the full path and registry node name for a
 *   given NBSI object. This is a maximum of 255 unicode characters,
 *   but may be provided as ascii or a pre-formed hash per the type
 *   member. The size (in bytes) of the given string/hash should be
 *   provided in the size member.
 *
 *   NOTE: In the case of an incomplete path such as HKR, one may pass
 *   in simply the root node. E.g.:
 *   1.) Normal case: HKLM\Path\Subpath
 *   2.) Unknown case: HKR
 *   It is expected that all unknown/incomplete paths will be determined
 *   prior to NBSI programming! There is otherwise NO WAY to match
 *   the hash given by an incomplete path to that stored in NBSI!
 *
 * valueName
 *   This field specifies the registry name for a given NBSI object.
 *   This is a maximum of 255 unicode characters, but may be provided
 *   in ascii or a pre-formed hash per the type member. The size (in bytes)
 *   of the given string/hash should be provided in the size member.
 * retBuf
 *   This field provides a pointer to a buffer into which the value
 *   retrieved from NBSI may be returned
 * retSize
 *   This field is an input/output. It specifies the maximum size of the
 *   return buffer as an input, and the size of the returned data as an
 *   output.
 * errorCode
 *   This field is a return value. It gives an error code representing
 *   failure to return a value (as opposed to failure of the call).
 *   This obeys the following:
 *
 *   NV2080_CTRL_BIOS_GET_NBSI_SUCCESS
 *     The call has returned complete and valid data.
 *   NV2080_CTRL_BIOS_GET_NBSI_OVERRIDE
 *     The call returned complete and valid data which is expected to override
 *     any stored registry settings.
 *   NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE
 *     The call returned data, but the size of the return buffer was
 *     insufficient to contain it. The value returned in retSize represents
 *     the total size necessary (in bytes) to contain the data.
 *     if the size was non-0, the buffer is filled with the object contents up
 *     to that size. Can be used with retBufOffset to use multiple calls to get
 *     tables of very large size.
 *   NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND
 *     The call did not find a valid NBSI object for this key. This indicates
 *     NBSI has no opinion and, more importantly, any data returned is identical
 *     to data passed in.
 *
 * Possible return values are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_BIOS_GET_NBSI_SUCCESS         (0x00000000)
#define NV2080_CTRL_BIOS_GET_NBSI_OVERRIDE        (0x00000001)
#define NV2080_CTRL_BIOS_GET_NBSI_BAD_HASH        (0xFFFFFFFA)
#define NV2080_CTRL_BIOS_GET_NBSI_APITEST_SUCCESS (0xFFFFFFFB)
#define NV2080_CTRL_BIOS_GET_NBSI_BAD_TABLE       (0xFFFFFFFC)
#define NV2080_CTRL_BIOS_GET_NBSI_NO_TABLE        (0xFFFFFFFD)
#define NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE      (0xFFFFFFFE)
#define NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND       (0xFFFFFFFF)

#define NV2080_CTRL_CMD_BIOS_GET_NBSI             (0x20800803) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_NBSI_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BIOS_GET_NBSI_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_BIOS_GET_NBSI_PARAMS {
    NvU32                            module;
    NV2080_CTRL_BIOS_NBSI_REG_STRING path;
    NV2080_CTRL_BIOS_NBSI_REG_STRING valueName;
    NV_DECLARE_ALIGNED(NvP64 retBuf, 8);
    NvU32                            retSize;
    NvU32                            errorCode;
} NV2080_CTRL_BIOS_GET_NBSI_PARAMS;

#define NV2080_CTRL_CMD_BIOS_GET_NBSI_V2  (0x2080080e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS_MESSAGE_ID" */

#define NV2080_BIOS_GET_NBSI_MAX_RET_SIZE (0x100)

#define NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS {
    NvU32                            module;
    NV2080_CTRL_BIOS_NBSI_REG_STRING path;
    NV2080_CTRL_BIOS_NBSI_REG_STRING valueName;
    NvU8                             retBuf[NV2080_BIOS_GET_NBSI_MAX_RET_SIZE];
    NvU32                            retSize;
    NvU32                            errorCode;
} NV2080_CTRL_BIOS_GET_NBSI_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_BIOS_GET_NBSI_OBJ
 *
 * globType
 *   This field specifies the glob type wanted
 *   0xffff: APItest... returns NV2080_CTRL_BIOS_GET_NBSI_APITEST_SUCCESS
 * globIndex
 *   Index for globType desired
 *      0 = best fit
 *      1..255 = actual index
 * globSource
 *   Index to nbsi directory sources used when getting entire directory
 *      0 = registry
 *      1 = VBIOS
 *      2 = SBIOS
 *      3 = ACPI
 * retBufOffset
 *   When making multiple calls to get the object (if retSize is too small)
 *   offset into real object (0=start of object)
 * retBuf
 *   This field provides a pointer to a buffer into which the object
 *   retrieved from NBSI may be returned
 * retSize
 *   This field is an input/output. It specifies the maximum size of the
 *   return buffer as an input, and the size of the returned data as an
 *   output.
 * totalObjSize
 *   This field is an output, where the total size of the object being
 *   retrieved is returned.
 * errorCode
 *   This field is a return value. It gives an error code representing
 *   failure to return a value (as opposed to failure of the call).
 *   This obeys the following:
 *
 *   NV2080_CTRL_BIOS_GET_NBSI_SUCCESS
 *     The call has returned complete and valid data.
 *   NV2080_CTRL_BIOS_GET_NBSI_OVERRIDE
 *     The call returned complete and valid data which is expected to override
 *     any stored registry settings.
 *   NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE
 *     The call returned data, but the size of the return buffer was
 *     insufficient to contain it. The value returned in retSize represents
 *     the total size necessary (in bytes) to contain the data.
 *   NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND
 *     The call did not find a valid NBSI object for this key. This indicates
 *     NBSI has no opinion and, more importantly, any data returned is identical
 *     to data passed in.
 *
 * Possible return values are:
 *   NV2080_CTRL_BIOS_GET_NBSI_SUCCESS
 *   NV2080_CTRL_BIOS_GET_NBSI_APITEST_NODIRACCESS
 *   NV2080_CTRL_BIOS_GET_NBSI_APITEST_SUCCESS
 *   NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE
 *   NV2080_CTRL_BIOS_GET_NBSI_BAD_TABLE
 *   NV2080_CTRL_BIOS_GET_NBSI_NO_TABLE
 *   NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND
 */
#define NV2080_CTRL_CMD_BIOS_GET_NBSI_OBJ (0x20800806) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BIOS_INTERFACE_ID << 8) | NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS {
    NvU16 globType;
    NvU8  globIndex;
    NvU16 globSource;
    NvU32 retBufOffset;
    NV_DECLARE_ALIGNED(NvP64 retBuf, 8);
    NvU32 retSize;
    NvU32 totalObjSize;
    NvU32 errorCode;
} NV2080_CTRL_BIOS_GET_NBSI_OBJ_PARAMS;

#define GLOB_TYPE_GET_NBSI_DIR                       0xfffe
#define GLOB_TYPE_APITEST                            0xffff
#define GLOB_TYPE_GET_NBSI_ACPI_RAW                  0xfffd



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
 *  skuConfigVersion
 *    Version number for the SKU configuration detailing pstate, thermal, VF curve and so on.
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

#define NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS_MESSAGE_ID (0x8U)

typedef struct NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS {
    NvU32 BoardID;
    char  chipSKU[9];
    char  chipSKUMod[5];
    NvU32 skuConfigVersion;
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
