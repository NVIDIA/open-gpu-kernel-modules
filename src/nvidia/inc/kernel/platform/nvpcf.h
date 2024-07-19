/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Definitions for the static  params table.
 */

/*!
 *  Layout of SysDev 2x data used for static config
 */
#define NVPCF_SYSDEV_STATIC_TABLE_VERSION_2X                              (0x20)
#define NVPCF_SYSDEV_STATIC_TABLE_HEADER_2X_SIZE_03                      (0x03U)
#define NVPCF_SYSDEV_STATIC_TABLE_HEADER_2X_FMT_SIZE_03                   ("3b")
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_SIZE_01                      (0x01U)
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_FMT_SIZE_01                   ("1b")

/*!
 * Static system dev header table, unpacked
 */
typedef struct
{
    /*
     * System device Table Version.
     */
    NvU32   version;

    /*
     * Size of device Table Header in bytes .
     */
    NvU32   headerSize;

    /*
     * Size of common entry in bytes.
     */
    NvU32   commonSize;
} SYSDEV_STATIC_TABLE_HEADER_2X;

/*!
 * Static system dev common entry
 */
typedef struct
{
    NvU32   param0;
} SYSDEV_STATIC_TABLE_COMMON_2X;

#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_CPU_TYPE                  3:0
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_CPU_TYPE_INTEL   (0x00000000)
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_CPU_TYPE_AMD     (0x00000001)
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_CPU_TYPE_NVIDIA  (0x00000002)
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_GPU_TYPE                  7:4
#define NVPCF_SYSDEV_STATIC_TABLE_COMMON_2X_PARAM0_GPU_TYPE_NVIDIA  (0x00000000)

/*!
 * Layout of Controller 2x data used for static config
 */
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_20                          (0x20)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_21                          (0x21)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_22                          (0x22)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_23                          (0x23)
#define NVPCF_CONTROLLER_STATIC_TABLE_VERSION_24                          (0x24)
#define NVPCF_CONTROLLER_STATIC_TABLE_MAX_ENTRIES                         (8)

// format for 2.0 and 2.1
#define NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V20_SIZE_05                 (0x05U)
#define NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V20_FMT_SIZE_05              ("5b")
#define NVPCF_CONTROLLER_STATIC_TABLE_COMMON_V20_SIZE_02                 (0x02U)
#define NVPCF_CONTROLLER_STATIC_TABLE_COMMON_V20_FMT_SIZE_02              ("1w")
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_SIZE_0F                  (0x0FU)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FMT_SIZE_0F           ("1b1w3d")

/*!
 * Static system controller header table v2.0/2.1, unpacked
 */
typedef struct
{
    /*
     * System controller Table Version.
     */
    NvU32   version;

    /*
     * Size of controller Table Header in bytes .
     */
    NvU32   headerSize;

    /*
     * Size of controller Table Common/Global Entry in bytes.
     */
    NvU32   commonSize;

    /*
     * Size of controller Table Entry in bytes.
     */
    NvU32   entrySize;

    /*
     * Number of controller Entries
     */
    NvU32   entryCount;
} CONTROLLER_STATIC_TABLE_HEADER_V20;

/*!
 * Static system controller common/global entry v2.0/2.1, unpacked
 */
typedef struct
{
    /*
     * Base sampling period in ms
     */
    NvU32  samplingPeriodms;
} CONTROLLER_STATIC_TABLE_COMMON_V20;

/*!
 * Static system controller entry v2.0/2.1, unpacked
 */
typedef struct
{
    /*
     * System controller entry type specific flag  (Flags0).
     */
    NvU32   flags0;

    /*
     * Sampling Multiplier.
     */
    NvU32  samplingMulti;

    /*
     * System controller entry filter parameters.
     */
    NvU32  filterParams;

    /*
     * System controller entry Usage-Specific Parameter (Param0).
     */
    NvU32  param0;

    /*
     * System controller entry Usage-Specific Parameter (Param1).
     */
    NvU32  param1;

} CONTROLLER_STATIC_TABLE_ENTRY_V20;

// FLAGS0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS                         3:0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_DISABLED       (0x00000000)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_PPAB           (0x00000001)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FLAGS0_CLASS_CTGP           (0x00000002)

// Filter
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FILTER_TYPE                         7:0

// filterType = EWMA
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FILTERPARAM_EWMA_WEIGHT            15:8
// filterType = MAX, others
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FILTERPARAM_WINDOW_SIZE            15:8

#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_FILTER_RESERVED                   31:16

// Param0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_PARAM0_QBOOST_INCREASE_GAIN        15:0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_PARAM0_QBOOST_DECREASE_GAIN       31:16

// Param1
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V20_PARAM1_QBOOST_DC_SUPPORT            0:0

// format for 2.2
#define NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V22_SIZE_04                 (0x04U)
#define NVPCF_CONTROLLER_STATIC_TABLE_HEADER_V22_FMT_SIZE_04              ("4b")
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_SIZE_05                  (0x05U)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FMT_SIZE_05             ("1b1d")

/*!
 * Static system controller header table v2.2, unpacked
 */
typedef struct
{
    /*
     * System controller Table Version.
     */
    NvU32   version;

    /*
     * Size of controller Table Header in bytes .
     */
    NvU32   headerSize;

    /*
     * Size of controller Table Entry in bytes.
     */
    NvU32   entrySize;

    /*
     * Number of controller Entries
     */
    NvU32   entryCount;
} CONTROLLER_STATIC_TABLE_HEADER_V22;

/*!
 * Static system controller entry v2.2, unpacked
 */
typedef struct
{
    /*
     * System controller entry type specific flag  (Flags0).
     */
    NvU32   flags0;

    /*
     * System controller entry Usage-Specific Parameter (Param0).
     */
    NvU32  param0;

} CONTROLLER_STATIC_TABLE_ENTRY_V22;

// FLAGS0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS                        3:0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_DISABLED      (0x00000000)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_PPAB          (0x00000001)
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_FLAGS0_CLASS_CTGP          (0x00000002)

// Param0
#define NVPCF_CONTROLLER_STATIC_TABLE_ENTRY_V22_PARAM0_QBOOST_DC_SUPPORT            0:0

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

/*
 * Definitions for the dynamic params table.
 */
#define NVPCF_DYNAMIC_PARAMS_20_VERSION                                   (0x20)
#define NVPCF_DYNAMIC_PARAMS_21_VERSION                                   (0x21)
#define NVPCF_DYNAMIC_PARAMS_22_VERSION                                   (0x22)
#define NVPCF_DYNAMIC_PARAMS_23_VERSION                                   (0x23)
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
// Controller Entry, Output Params4
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM4_CMD0_UNSIGNED           31:0
// Controller Entry, Output Params5
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM5_CMD0_UNSIGNED           31:0
// Controller Entry, Output Params6
#define NVPCF_DYNAMIC_PARAMS_ENTRY_2X_OUTPUT_PARAM6_CMD0_UNSIGNED           31:0

#endif // NVPCF_H

