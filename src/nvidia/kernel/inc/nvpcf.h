/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVPCF_H
#define NVPCF_H

#include "ctrl/ctrl0000/ctrl0000system.h"

/*
 * Definitions for the dynamic params table.
 */
#define NVPCF0100_CTRL_DYNAMIC_TABLE_1X_VERSION                           (0x10)
#define NVPCF0100_CTRL_DYNAMIC_TABLE_1X_ENTRY_SIZE                          (4U)
//
// This is set to 32UL in windows NVPCF driver. Set it to 2UL which is good
// enough for now to save space
//
#define NVPCF0100_CTRL_DYNAMIC_TABLE_1X_ENTRY_MAX                          (2UL)
#define NVPCF0100_CTRL_DYNAMIC_TABLE_1X_INPUT_CMD_GET_TPP                 (0x04)

/*
 * Dynamic Params Table Header, v1.x.
 */
typedef struct
{
    NvU8 version;

    NvU8 size;

    //
    // Number of entries in the entire table.
    //
    NvU8 entryCnt;

    NvU8 reserved;

} NVPCF0100_CTRL_DYNAMIC_TABLE_1X_HEADER,
*PNVPCF0100_CTRL_DYNAMIC_TABLE_1X_HEADER;

/*
 * Define the dynamic params table header and entries used by the ACPI call.
 */
typedef struct
{
    NVPCF0100_CTRL_DYNAMIC_TABLE_1X_HEADER header;
    NvU32                                  entries[NVPCF0100_CTRL_DYNAMIC_TABLE_1X_ENTRY_MAX];
} CONTROLLER_DYNAMIC_TABLE_1X_ACPI,
*PCONTROLLER_DYNAMIC_TABLE_1X_ACPI;

/*!
 * Config DSM NVPCF 2x version specific defines
 */
#define NVPCF_DYNAMIC_PARAMS_20_VERSION                                   (0x20)
#define NVPCF_DYNAMIC_PARAMS_21_VERSION                                   (0x21)
#define NVPCF_DYNAMIC_PARAMS_2X_HEADER_SIZE_05                           (0x05U)
#define NVPCF_DYNAMIC_PARAMS_2X_HEADER_FMT_SIZE_05                        ("5b")
#define NVPCF_DYNAMIC_PARAMS_2X_COMMON_SIZE_10                           (0x10U)
#define NVPCF_DYNAMIC_PARAMS_2X_COMMON_FMT_SIZE_10                        ("4d")
#define NVPCF_DYNAMIC_PARAMS_2X_ENTRY_SIZE_1C                            (0x1CU)
#define NVPCF_DYNAMIC_PARAMS_2X_ENTRY_FMT_SIZE_1C                         ("7d")
#define NVPCF_DYNAMIC_PARAMS_2X_ENTRY_MAX                                    (8)

// Power unit used, 125 milli-watts
#define NVPCF_DYNAMIC_PARAMS_2X_POWER_UNIT_MW                              (125)

/*!
 * Dynamic params header, unpacked.
 */
typedef struct
{
    /*
     * Dynamic params table Version.
     */
    NvU32   version;

    /*
     * Size of dynamic params table header in bytes.
     */
    NvU32   headerSize;

    /*
     * Size of global/common entry in bytes.
     */
    NvU32   commonSize;

    /*
     * Size of each controller entry in bytes.
     */
    NvU32   entrySize;

    /*
     * Number of controller entries.
     */
    NvU32   entryCount;
} DYNAMIC_PARAMS_HEADER_2X;

/*!
 * Dynamic params table global/common, unpacked.
 */
typedef struct
{
    NvU32 param0;
    NvU32 param1;
    NvU32 param2;
    NvU32 param3;
} DYNAMIC_PARAMS_COMMON_2X;

/*!
 * Dynamic params table controller entry, unpacked.
 */
typedef struct
{
    NvU32 param0;
    NvU32 param1;
    NvU32 param2;
    NvU32 param3;
    NvU32 param4;
    NvU32 param5;
    NvU32 param6;
} DYNAMIC_PARAMS_ENTRY_2X;

/*!
 * Dynamic params table header, packed.
 */
typedef struct
{
    NvU8 version;
    NvU8 headerSize;
    NvU8 commonSize;
    NvU8 entrySize;
    NvU8 entryCount;
} DYNAMIC_PARAMS_HEADER_2X_PACKED;

/*!
 * Dynamic params table global/common, packed.
 */
typedef struct
{
    NvU32 param0;
    NvU32 param1;
    NvU32 param2;
    NvU32 param3;
} DYNAMIC_PARAMS_COMMON_2X_PACKED;

/*!
 * Dynamic params table controller entry, packed.
 */
typedef struct
{
    NvU32 param0;
    NvU32 param1;
    NvU32 param2;
    NvU32 param3;
    NvU32 param4;
    NvU32 param5;
    NvU32 param6;
} DYNAMIC_PARAMS_ENTRY_2X_PACKED;

// Input Commands (Input Param0)
#define NVPCF_DYNAMIC_PARAMS_COMMON_2X_INPUT_PARAM0_CMD                      1:0
#define NVPCF_DYNAMIC_PARAMS_COMMON_2X_INPUT_PARAM0_CMD_GET                  (0)
#define NVPCF_DYNAMIC_PARAMS_COMMON_2X_INPUT_PARAM0_CMD_SET                  (1)

//
// Input Command 0 (Get Controller Parameters)
//
// Global/Common Entry, Output Param0
#define NVPCF_DYNAMIC_PARAMS_COMMON_2X_OUTPUT_PARAM0_CMD0_CTGP_AC_OFFSET    15:0
#define NVPCF_DYNAMIC_PARAMS_COMMON_2X_OUTPUT_PARAM0_CMD0_CTGP_DC_OFFSET   31:16
// Controller Entry, Output Param0
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM0_CMD0_IDX                 7:0
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM0_CMD0_DISABLE_AC          8:8
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM0_CMD0_DISABLE_DC          9:9
// Controller Entry, Output Params1
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM1_CMD0_SIGNED0            15:0
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM1_CMD0_SIGNED1           31:16
// Controller Entry, Output Params2
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM2_CMD0_SIGNED0            15:0
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM2_CMD0_SIGNED1           31:16
// Controller Entry, Output Params3
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM3_CMD0_SIGNED0            15:0
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM3_CMD0_SIGNED1           31:16

#endif // NVPCF_H

