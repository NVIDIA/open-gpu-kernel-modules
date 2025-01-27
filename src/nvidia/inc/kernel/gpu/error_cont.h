/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file    error_cont.h
 * @brief   Holds data structures, defines and policy table required by the
 *          Ampere Error Containment feature / code.
 */

#ifndef _ERROR_CONT_H_
#define _ERROR_CONT_H_

/* ------------------------ Includes ---------------------------------------- */
#include "core/core.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "nverror.h"

/* ------------------------ Forward Definitions ----------------------------- */

typedef struct Device Device;

/* ------------------------ Datatypes --------------------------------------- */

/*!
 * Error Containment error ID enum
 */
typedef enum _NV_ERROR_CONT_ERR_ID
{
    NV_ERROR_CONT_ERR_ID_E01_FB_ECC_DED                        =  0,   // FD Error ID: E01: FB ECC DED
    NV_ERROR_CONT_ERR_ID_E02_FB_ECC_DED_IN_CBC_STORE           =  1,   // FD Error ID: E02: FB ECC DED in CBC
    NV_ERROR_CONT_ERR_ID_E05_LTC_ECC_DSTG                      =  2,   // FD Error ID: E05: LTC ECC in data region
    NV_ERROR_CONT_ERR_ID_E06_LTC_UNSUPPORTED_CLIENT_POISON     =  3,   // FD Error ID: E06: LTC unsupported client poison
    NV_ERROR_CONT_ERR_ID_E07_LTC_ECC_TSTG                      =  4,   // FD Error ID: E07: LTC Tag Parity error
    NV_ERROR_CONT_ERR_ID_E08_LTC_ECC_RSTG                      =  5,   // FD Error ID: E08: LTC CBC Parity error
    NV_ERROR_CONT_ERR_ID_E09_FBHUB_POISON                      =  6,   // FD Error ID: E09: FBHUB poison error
    NV_ERROR_CONT_ERR_ID_E10_SM_POISON                         =  7,   // FD Error ID: E10: SM poison error
    NV_ERROR_CONT_ERR_ID_E12A_CE_POISON_IN_USER_CHANNEL        =  8,   // FD Error ID: E12: CE poison error in user channel
    NV_ERROR_CONT_ERR_ID_E12B_CE_POISON_IN_KERNEL_CHANNEL      =  9,   // FD Error ID: E12: CE poison error in kernel channel
    NV_ERROR_CONT_ERR_ID_E13_MMU_POISON                        = 10,   // FD Error ID: E13: MMU poison error
    NV_ERROR_CONT_ERR_ID_E16_GCC_POISON                        = 11,   // FD Error ID: E16: GCC poison error
    NV_ERROR_CONT_ERR_ID_E17_CTXSW_POISON                      = 12,   // FD Error ID: E17: FECS/GPCCS/TPCCS poison error
    NV_ERROR_CONT_ERR_ID_E20_XALEP_EGRESS_POISON               = 13,   // FD Error ID: E20: XALEP EGRESS poison error
    NV_ERROR_CONT_ERR_ID_E21A_XALEP_INGRESS_CONTAINED_POISON   = 14,   // FD Error ID: E21: XALEP INGRESS contained poison error
    NV_ERROR_CONT_ERR_ID_E21B_XALEP_INGRESS_UNCONTAINED_POISON = 15,   // FD Error ID: E21: XALEP INGRESS contained poison error
    NV_ERROR_CONT_ERR_ID_E22_PMU_POISON                        = 16,   // FD Error ID: E22: PMU poison error
    NV_ERROR_CONT_ERR_ID_E23_SEC2_POISON                       = 17,   // FD Error ID: E23: SEC2 poison error
    NV_ERROR_CONT_ERR_ID_E24_GSP_POISON                        = 18,   // FD Error ID: E24: GSP poison error
    NV_ERROR_CONT_ERR_ID_E25_FBFALCON_POISON                   = 19,   // FD Error ID: E25: FBFalcon poison error
    NV_ERROR_CONT_ERR_ID_E26_NVDEC_POISON                      = 20,   // FD Error ID: E26: NVDEC poison error
    NV_ERROR_CONT_ERR_ID_E27_NVJPG_POISON                      = 21,   // FD Error ID: E27: NVJPG poison error
    NV_ERROR_CONT_ERR_ID_E28_OFA_POISON                        = 22    // FD Error ID: E28: OFA poison error
} NV_ERROR_CONT_ERR_ID;

/*!
 * Error Containment settings per error ID, when SMC memory partitioning is disable or enabled.
 */
typedef struct _NV_ERROR_CONT_SMC_DIS_EN_SETTING
{
    NvU32  rcErrorCode;
    NvBool bGpuResetReqd;
    NvBool bGpuDrainAndResetReqd;
    NvBool bPrintSmcPartitionInfo;
    NvU32  nv2080Notifier;
} NV_ERROR_CONT_SMC_DIS_EN_SETTING;

/*!
 * Error Containment state table tracking policy settings for each error ID
 */
typedef struct _NV_ERROR_CONT_STATE_TABLE
{
    NV_ERROR_CONT_ERR_ID             errorCode;
    NV_ERROR_CONT_SMC_DIS_EN_SETTING smcDisEnSetting[2]; // 0: SMC memory partitioning disabled,
                                                         // 1: SMC memory partitioning enabled
} NV_ERROR_CONT_STATE_TABLE;

/*!
 * Struct for LTC location
 */
typedef struct _NV_ERROR_CONT_LOCATION_LTC
{
    NvU32 partition;
    NvU32 slice;
} NV_ERROR_CONT_LOCATION_LTC;

/*!
 * Struct for DRAM location
 */
typedef struct _NV_ERROR_CONT_LOCATION_DRAM
{
    NvU32 partition;
    NvU32 subPartition;
    NvU64 physicalAddress;
} NV_ERROR_CONT_LOCATION_DRAM;

/*!
 * Struct for Engine id
 */
typedef struct _NV_ERROR_CONT_LOCATION_ENG_ID
{
    RM_ENGINE_TYPE  rmEngineId;
    Device         *pDevice;
} NV_ERROR_CONT_LOCATION_ENG_ID;

/*!
 * Error Containment location type
 */
typedef enum _NV_ERROR_CONT_LOCATION_TYPE
{
    NV_ERROR_CONT_LOCATION_TYPE_NONE   = 0, // No location information available
    NV_ERROR_CONT_LOCATION_TYPE_DRAM   = 1, // DRAM location
    NV_ERROR_CONT_LOCATION_TYPE_LTC    = 2, // LTC location
    NV_ERROR_CONT_LOCATION_TYPE_ENGINE = 3, // Engine location
    NV_ERROR_CONT_LOCATION_TYPE_VF     = 4  // VF location
} NV_ERROR_CONT_LOCATION_TYPE;

/*!
 * Union for Error Containment location information
 */
typedef union _NV_ERROR_CONT_LOCATION_INFO
{
    NV_ERROR_CONT_LOCATION_DRAM   dramLoc;   // DRAM location
    NV_ERROR_CONT_LOCATION_LTC    ltcLoc;    // LTC location
    NV_ERROR_CONT_LOCATION_ENG_ID engineLoc; // Engine location
    NvU32                         vfGfid;    // VF location
} NV_ERROR_CONT_LOCATION_INFO;

typedef struct _NV_ERROR_CONT_LOCATION
{
    NV_ERROR_CONT_LOCATION_TYPE locType;
    NV_ERROR_CONT_LOCATION_INFO locInfo;
} NV_ERROR_CONT_LOCATION;

/* ------------------------ Macros ------------------------------------------ */

#define ROBUST_CHANNEL_CONTAINED_ERROR_STR   "Contained"
#define ROBUST_CHANNEL_UNCONTAINED_ERROR_STR "Uncontained"
#define NO_XID                               NV_U32_MAX
#define NO_NV2080_NOTIFIER                   NV2080_NOTIFIERS_MAXCOUNT
#define NV_ERR_CONT_LOCATION_STRING_SIZE_MAX 64

/*!
 * Error Containment error types string.
 * The order of this list must match the NV_ERROR_CONT_ERR_ID enums.
 */
#define NV_ERROR_CONT_ERR_ID_STRING_PUBLIC {"FB DED",            \
                                            "DED CBC",           \
                                            "LTC Data",          \
                                            "LTC GPC",           \
                                            "LTC TAG",           \
                                            "LTC CBC",           \
                                            "FBHUB",             \
                                            "SM",                \
                                            "CE User Channel",   \
                                            "CE Kernel Channel", \
                                            "MMU",               \
                                            "GCC",               \
                                            "CTXSW",             \
                                            "PCIE",              \
                                            "PCIE",              \
                                            "PCIE",              \
                                            "PMU",               \
                                            "SEC2",              \
                                            "GSP",               \
                                            "FB Falcon",         \
                                            "NVDEC",             \
                                            "NVJPG",             \
                                            "OFA"}

#endif // _ERROR_CONT_H_
