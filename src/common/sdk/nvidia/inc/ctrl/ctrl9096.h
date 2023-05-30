/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl9096.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV9096_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x9096, NV9096_CTRL_##cat, idx)

/* NV9096 command categories (6bits) */
#define NV9096_CTRL_RESERVED                                            (0x00U)
#define NV9096_CTRL_ZBC                                                 (0x01U)


/*
 * NV9096_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV9096_CTRL_CMD_NULL                                            (0x90960000U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_RESERVED_INTERFACE_ID << 8) | 0x0" */

/*
 * NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_
 * These are various possible CtFormats which 
 * the Client passes down to RM to set in the DS
 * Color Table.
 */



#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_INVALID             0x00000000U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_ZERO                0x00000001U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_UNORM_ONE           0x00000002U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_RF32_GF32_BF32_AF32 0x00000004U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_R16_G16_B16_A16     0x00000008U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_RN16_GN16_BN16_AN16 0x0000000cU
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_RS16_GS16_BS16_AS16 0x00000010U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_RU16_GU16_BU16_AU16 0x00000014U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_RF16_GF16_BF16_AF16 0x00000016U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A8R8G8B8            0x00000018U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A8RL8GL8BL8         0x0000001cU
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A2B10G10R10         0x00000020U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_AU2BU10GU10RU10     0x00000024U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A8B8G8R8            0x00000028U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A8BL8GL8RL8         0x0000002cU
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_AN8BN8GN8RN8        0x00000030U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_AS8BS8GS8RS8        0x00000034U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_AU8BU8GU8RU8        0x00000038U
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_A2R10G10B10         0x0000003cU
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL_BF10GF11RF11        0x00000040U

/*
 * NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR
 *
 * This command attempts to add a new entry to Color ZBC Tables.
 *
 *  colorFB
 *    This field sets the raw framebuffer values for the ZBC table entries. Prior to 
 *    GA10x, these values are written into the "L2" table.
 *    From GA10x and later, these values are written in "CROP" table.
 *  colorDS
 *    This field sets format-independent values for ZBC table entries. Prior to GA10X, 
 *    these values are written in the "DS" table and  matched with the format-independent 
 *    clear color sent in the 3D class. These values are ignored on GA10X and later 
 *  format
 *    This field specifies color format for ZBC table entries and should be one of the 
 *    NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT* enums. Prior to GA10X, these values 
 *    are written in the format field of the hardware ZBC table entry and are matched 
 *    against the color format sent in the 3D class. These values are ignored on GA10X and later.
 *  bSkipL2Table
 *    This parameter specifies if the L2 ZBC table should be updated or not. If
 *    this parameter is set to a non-zero value, the L2 ZBC table will not be
 *    updated. If this parameter is set to 0, the L2 ZBC table will be updated. This 
 *    parameter will skip programming DS table values in Pre-GA10x. From GA10x and later
 *    this parameter will skip programming CROP table entries. 
 *    Note: This parameter will only be supported in verification platforms. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_INSUFFICIENT_RESOURCES
 */
#define NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR        (0x90960101U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE 4U
#define NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS {
    NvU32  colorFB[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
    NvU32  colorDS[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
    NvU32  format;
    NvBool bSkipL2Table;
} NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS;


/*
 * NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT_VAL
 * These are various possible formats which the Client passes down to RM to set in the ZBC clear 
 * Table.
 * 
 * Though the "Depth" data written in both the DS as well as L2 Version of the 
 * depth table are of (only available!) FP32 Format. Still the format 
 * support is currently given with _CTRL_CMD_SET_ZBC_DEPTH_CLEAR as this format will
 * be used later on to disable/remove an entry from the table.
 * In future this field is going to be significant for
 * "Depth" entries too.
 */

#define NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT_VAL_INVALID 0x00000000U
// Fix me: Fix the name to FMT_VAL_FP32
#define NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT_FP32        0x00000001U

/*
 * NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR
 *
 * This command attempts to add a new entry to Depth ZBC Tables.
 *
 *  depth
 *    This field specifies the ZBC depth clear value to be set.
 *  format
 *    This field specifies the Depth format for the data send in  by the client. 
 *  bSkipL2Table
 *    This parameter specifies if the L2 ZBC table should be updated or not. If
 *    this parameter is set to a non-zero value, the L2 ZBC table will not be
 *    updated. If this parameter is set to 0, the L2 ZBC table will be updated.
 *    Note: This parameter will only be supported in verification platforms. 
 * 

 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_INSUFFICIENT_RESOURCES
 */
#define NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR                 (0x90960102U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS {
    NvU32  depth;
    NvU32  format;
    NvBool bSkipL2Table;
} NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS;

/*
 * NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE
 *
 *
 * This command is used to get the ZBC Clear Color/Depth/Stencil table data i.e. color 
 * values and the status "Use-satatus" of each value in the table from RM. 
 *
 *   colorFB
 *      This value returns raw framebuffer values for color ZBC table entries. Pre-GA10x, these values
 *      are taken from the "L2" table. From GA10x+, these values are taken from CROP table. This value is
 *      set only when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_COLOR"
 *   colorDS
 *      Pre-GA10x, returns the DS color value set for ZBC. From GA10x+, returns zeroes since this format is
 *      deprecated in HW. This value is set only when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_COLOR"
 *   depth
 *     This field returns the ZBC depth clear value set, when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_DEPTH".
 *   stencil
 *     This field returns the ZBC stencil clear value set, when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_STENCIL"
 *   format
 *      This field returns the format of color, depth, or stencil ZBC table entries, using the
 *      NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT,
 *      NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT,
 *      NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT* enums depending on the table identified by valType.
 *      On GA10X and later, color ZBC table entries do not have a format in hardware and this query 
 *      returns a format of "INVALID".
 *   valType
 *     This filed specifies the type of the Table  data to be fetched.
 *     Possible Types are :
 *       NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_COLOR
 *       NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_DEPTH
 *       NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_STENCIL
 *   indexUsed
 *     This boolean parameter indicates if a particular index of the table is valid or not.
 *
 *   Note: The following parameters are deprecated after Tegra interface is also changed. Clients need 
 *   to use NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_SIZE to query the (start, end) indexes of respective ZBC tables
 *
 *   indexSize
 *     This parameter is used to fetch the table size when "valType" parameter
 *     is specified as " 0 ".(INVALID TYPE). It is also used to pass in the
 *     index of the ZBC table for which we want the COLOR/DEPTH info.
 *   indexStart
 *      This parameter is used to return the valid starting index of ZBC table, when
 *      "valType" parameter is specified as "INVALID_TYPE". It will also be used
 *       as input index to query the ZBC table for COLOR/QUERY/STENCIL Info. 
 *   indexEnd
 *       This parameter is used to return the valid ending index of ZBC table, when
 *       "valType" parameter is specified as "INVALID_TYPE".
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE       (0x90960103U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_INVALID 0U
#define NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_COLOR   1U
#define NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_DEPTH   2U
#define NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_STENCIL 3U
#define NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS {
    struct {
        NvU32 colorFB[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
        NvU32 colorDS[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
        NvU32 depth;
        NvU32 stencil;
    } value;
    NvU32 indexSize;
    NvU32 indexStart;
    NvU32 indexEnd;
    NvU32 indexUsed; /* TODO: Change to NvBool, need to fix the tegra interface too */
    NvU32 format;
    NvU32 valType;
} NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS;

/*
 * Note: This ctrl call is deprecated. To program ZBC table entries, please use 
 *  NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR
 *  NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR
 *  NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR
 *  which will update a single entry in single table at a time. 
 *  
 *  
 * NV9096_CTRL_CMD_SET_ZBC_CLEAR_TABLE
 *
 * This command is used to set the ZBC Clear Color/Depth/Stencil table data at a specified
 * index. The parameters to this command are described below.
 *
 *  colorFB
 *    This array field specifies the L2 color value to be written to the ZBC table.
 *  colorDS
 *    This array field specifies the DS color value to be written to the ZBC table.
 *  colorFormat
 *    This field specifies the ZBC color format to be set. This field must be set
 *    to one of the valid NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL* defines.
 *  depth
 *    This field specifies the ZBC depth clear value to be set.
 *  depthFormat
 *    This field specifies the ZBC depth format to be set. This field must be set
 *    to one of the valid NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT_VAL* defines.
 *  stencil
 *    This field specifies the ZBC stencil clear value to be set.
 *  stencilFormat
 *    This field specifies the ZBC stencil format to be set. This field must be set
 *    to one of the valid NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT_VAL* defines.
 *  index
 *    This field specifies the index at which the color/depth data is to be
 *    written into the ZBC table. Legal values for this field must lie between
 *    1 and the value returned in the indexSize parameter of the
 *    NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE control call when called with the
 *    parameter valType set to NV9096_CTRL_ZBC_CLEAR_OBJECT_TYPE_INVALID.
 *  bSkipL2Table
 *    This parameter specifies if the L2 ZBC table should be updated or not. If
 *    this parameter is set to a non-zero value, the L2 ZBC table will not be
 *    updated. If this parameter is set to 0, the L2 ZBC table will be updated.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_INSUFFICIENT_RESOURCES
 */
#define NV9096_CTRL_CMD_SET_ZBC_CLEAR_TABLE (0x90960104U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS {
    NvU32  colorFB[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
    NvU32  colorDS[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
    NvU32  colorFormat;
    NvU32  depth;
    NvU32  depthFormat;
    NvU32  stencil;
    NvU32  stencilFormat;
    NvU32  index;
    NvBool bSkipL2Table;
} NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS;

/*
 * NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT_VAL_
 * These are various possible Formats which the Client passes down to RM to set in the 
 * ZBC clear(DSS) Table.
 */

#define NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT_VAL_INVALID 0x00000000U
// Fix me: Change it to CLEAR_FMT_VAL_U8
#define NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT_U8          0x00000001U

/*
 * NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR
 *
 * This command attempts to add a new entry to Stencil ZBC Tables.
 *
 *  stencil
 *    This field specifies the ZBC stencil clear value to be set.
 *  format
 *    This field specifies the stencil format for the data send in  by the client.
 *  bSkipL2Table
 *    This parameter specifies if the L2 ZBC table should be updated or not. If
 *    this parameter is set to a non-zero value, the L2 ZBC table will not be
 *    updated. If this parameter is set to 0, the L2 ZBC table will be updated.
 *    Note: This parameter will only be supported in verification platforms.
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_INSUFFICIENT_RESOURCES
 */
#define NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR                 (0x90960105U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS {
    NvU32  stencil;
    NvU32  format;
    NvBool bSkipL2Table;
} NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS;

/*
 * NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_SIZE
 *   This command returns the range of valid indices in the color, depth, or stencil ZBC tables.
 * 
 *   indexStart
 *      This parameter is used to return the first valid index in the color, depth, or stencil ZBC table,
 *      depending on the value passed in the tableType 
 *   indexEnd
 *      This parameter is used to return the last valid index in the color, depth, or stencil ZBC table,
 *      depending on the value passed in the tableType
 *   tableType
 *     This field specifies the type of the Table  data to be fetched.
 *     Possible Types are :
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COLOR
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_DEPTH
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_STENCIL
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

typedef enum NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE {
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_INVALID = 0,
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COLOR = 1,
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_DEPTH = 2,
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_STENCIL = 3,
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COUNT = 4,
} NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE;

#define NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_SIZE (0x90960106U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS {
    NvU32                            indexStart;
    NvU32                            indexEnd;
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE tableType;
} NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS;

/*
 * NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_ENTRY
 *   This command returns the ZBC entry stored in the color, depth or stencil ZBC tables
 *
 *   colorFB[out]
 *      This value returns raw framebuffer values for color ZBC table entries. Pre-GA10x, these values
 *      are taken from the "L2" table. From GA10x+, these values are taken from CROP table. This value is
 *      set only when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COLOR"
 *   colorDS[out]
 *      Pre-GA10x, returns the DS color value set for ZBC. From GA10x+, returns zeroes since this format is
 *      deprecated in HW. This value is set only when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COLOR"
 *   depth[out]
 *      This field specifies the ZBC depth clear value set, when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_DEPTH".
 *   stencil[out]
 *      This field specifies the ZBC stencil clear value set, when valType is chosen as "NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_STENCIL"
 *   format
 *      This field returns the format of color, depth, or stencil ZBC table entries, using the
 *      NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR_FMT_VAL*,
 *      NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR_FMT*,
 *      NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR_FMT* enums depending on the table identified by valType.
 *      On GA10X and later, color ZBC table entries do not have a format in hardware and this query 
 *      returns a format of "INVALID".
 *   index[in]
 *      This field specifies table index for which the ZBC entry information needs to be fetched. 
 *   bIndexValid[out]
 *      This field specifies whether the entry is valid or not. 
 *   tableType[in]
 *      This field specifies the type of the Table  data to be fetched.
 *      Possible Types are :
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_COLOR
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_DEPTH
 *       NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE_STENCIL
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_ENTRY (0x90960107U) /* finn: Evaluated from "(FINN_GF100_ZBC_CLEAR_ZBC_INTERFACE_ID << 8) | NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_MESSAGE_ID" */

#define NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS {
    struct {
        NvU32 colorFB[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
        NvU32 colorDS[NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE];
        NvU32 depth;
        NvU32 stencil;
    } value;
    NvU32                            format;
    NvU32                            index;
    NvBool                           bIndexValid;
    NV9096_CTRL_ZBC_CLEAR_TABLE_TYPE tableType;
} NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS;

/* _ctrl9096_h_ */
