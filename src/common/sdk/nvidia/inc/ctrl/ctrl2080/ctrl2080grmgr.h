/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080grmgr.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX grmgr control commands and parameters */

//
// NV2080_CTRL_CMD_GRMGR_GET_GR_FS_INFO
//
// This control call works as a batched query interface where we
// have multiple different queries that can be passed in
// and RM will return the associated data and status type
// If there is any error in NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS,
// we will immediately fail the call.
// However, if there is an error in the query-specific calls, we will
// log the error and march on.
//
// NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS
//  numQueries[IN]
//      - Specifies the number of valid queries that the caller will be passing in
//
// Possible status values returned are:
//   NV_OK
//   NV_ERR_INVALID_ARGUMENT
//   NV_ERR_INVALID_STATE
//
#define NV2080_CTRL_CMD_GRMGR_GET_GR_FS_INFO        (0x20803801) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_GRMGR_INTERFACE_ID << 8) | NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_MESSAGE_ID" */

// Max number of queries that can be batched in a single call to NV2080_CTRL_CMD_GRMGR_GET_GR_FS_INFO
#define NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES    96

//
// Preference is to keep max.size of union at 24 bytes (i.e. 6 32-bit members)
// so that the size of entire query struct is maintained at 32 bytes, to ensure
// that overall params struct does not exceed 4kB
//
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_MAX_SIZE 32
#define NV2080_CTRL_GRMGR_MAX_SMC_IDS               8

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_GPC_COUNT_PARAMS
 *  gpcCount[OUT]
 *      - No. of logical/local GPCs which client can use to create the
 *        logical/local mask respectively
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_GPC_COUNT_PARAMS {
    NvU32 gpcCount;         // param[out] - logical/local GPC mask
} NV2080_CTRL_GRMGR_GR_FS_INFO_GPC_COUNT_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_GPC_MAP_PARAMS
 *  gpcId[IN]
 *      - Logical/local GPC ID
 *  chipletGpcMap[OUT]
 *      - Returns chiplet GPC ID for legacy case and device monitoring client
 *      - Returns local GPC ID (== input gpcId) for SMC client
 *      - Does not support DM attribution case
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_GPC_MAP_PARAMS {
    NvU32 gpcId;            // param[in] - logical/local GPC ID
    NvU32 chipletGpcMap;    // param[out] - chiplet GPC ID
} NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_GPC_MAP_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_TPC_MASK_PARAMS
 *  gpcId[IN]
 *      - Logical/local GPC ID
 *  tpcMask[OUT]
 *      - Returns physical TPC mask for legacy, DM client and SMC cases
 *      - Does not support DM attribution case
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_TPC_MASK_PARAMS {
    NvU32 gpcId;            // param[in] - logical/local GPC ID
    NvU32 tpcMask;          // param[out] - physical TPC mask
} NV2080_CTRL_GRMGR_GR_FS_INFO_TPC_MASK_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_PPC_MASK_PARAMS
 *  gpcId[IN]
 *      - Logical/local GPC ID
 *  ppcMask[OUT]
 *      - Returns physical PPC mask for legacy, DM client and SMC cases
 *      - Does not support DM attribution case
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_PPC_MASK_PARAMS {
    NvU32 gpcId;            // param[in] - logical/local GPC ID
    NvU32 ppcMask;          // param[out] - physical PPC mask
} NV2080_CTRL_GRMGR_GR_FS_INFO_PPC_MASK_PARAMS;

/*!
 *  !!! DEPRECATED - This query will return NV_ERR_NOT_SUPPORTED since deleting
 *               it would break driver compatibility !!!
 *
 *  NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_GPC_MAP_PARAMS
 *  swizzId[IN]
 *      - Swizz ID of partition
 *      - A DM client with an invalid swizz ID, will fail this call
 *      - This parameter is not compulsory for an SMC client; the subscription
 *        itself will do the necessary validation.
 *  gpcId[IN]
 *      - Logical/local GPC ID
 *  chipletGpcMap[OUT]
 *      - Returns chiplet GPC ID for legacy case and device monitoring client
 *      - Returns local GPC ID (== input gpcId) for SMC client
 *      - Does not support non-attribution case for DM client
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_GPC_MAP_PARAMS {
    NvU32 swizzId;          // param[in]  - swizz ID of partition
    NvU32 gpcId;            // param[in]  - logical/local GPC ID
    NvU32 chipletGpcMap;    // param[out] - chiplet GPC ID
} NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_GPC_MAP_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_ROP_MASK_PARAMS
 *  gpcId[IN]
 *      - Logical/local GPC ID
 *  ropMask[OUT]
 *      - Returns physical ROP mask for legacy, DM client
 *      - Returns logical ROP mask for SMC
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_ROP_MASK_PARAMS {
    NvU32 gpcId;            // param[in] - logical/local GPC ID
    NvU32 ropMask;          // param[out] - physical ROP mask
} NV2080_CTRL_GRMGR_GR_FS_INFO_ROP_MASK_PARAMS;

/*!
 *  NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_SYSPIPE_MASK_PARAMS
 *  chipletSyspipeMask [OUT]
 *      - Mask of chiplet SMC-IDs for DM client attribution case
 *      - Mask of local SMC-IDs for SMC client
 *      - Legacy case returns 1 GR
 *      - Does not support attribution case for DM client
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_SYSPIPE_MASK_PARAMS {
    NvU32 chipletSyspipeMask;   // param[out] - Mask of chiplet SMC IDs
} NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_SYSPIPE_MASK_PARAMS;

/*!
 *  NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_SYSPIPE_IDS_PARAMS
 *  swizzId[IN]
 *      - Swizz ID of partition
 *      - A DM client with an invalid swizz ID, will fail this call
 *  physSyspipeId[GRMGR_MAX_SMC_IDS] [OUT]
 *      - Physical SMC-IDs mapped to partition local idx for DM client attribution case
 *      - Does not support non-attribution case for DM client, SMC clients, legacy case
 *  physSyspipeIdCount[OUT]
 *      - Valid count of physSmcIds which has been populated in above array.
 *      - Failure case will return 0
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_SYSPIPE_IDS_PARAMS {
    NvU16 swizzId;                                          // param[in]  - swizz ID of partition
    NvU16 physSyspipeIdCount;                               // param[out] - Count of physSmcIds in above array
    NvU8  physSyspipeId[NV2080_CTRL_GRMGR_MAX_SMC_IDS];      // param[out] - physical/local SMC IDs
} NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_SYSPIPE_IDS_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_PROFILER_MON_GPC_MASK_PARAMS
 * swizzId[IN]
 *      - Swizz ID of partition
 *      - Mandatory parameter
 *      - A DM client with an invalid swizz ID, will fail this call
 * grIdx[IN]
 *      - Local grIdx for a partition
 *      - Mandatory parameter
 * gpcEnMask[OUT]
 *      - Logical enabled GPC mask associated with requested grIdx of the partition i.e swizzid->engineId->gpcMask
 *      - These Ids should be used as input further
 *      - Does not support non-attribution case for DM client, SMC clients, legacy case
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_PROFILER_MON_GPC_MASK_PARAMS {
    NvU32 swizzId;      // param[in]  - swizz ID of partition
    NvU32 grIdx;        // param[in]  - partition local GR ID
    NvU32 gpcEnMask;    // param[out] - logical enabled GPC mask
} NV2080_CTRL_GRMGR_GR_FS_INFO_PROFILER_MON_GPC_MASK_PARAMS;

/*!
 * NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_SYSPIPE_ID
 * syspipeId[OUT]
 *      - Partition-local GR idx for client subscribed to exec partition
 *      - Does not support legacy case, DM client, or SMC client subscribed only to partition
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_SYSPIPE_ID_PARAMS {
    NvU32 syspipeId;    // param[out] - partition-local Gr idx
} NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_SYSPIPE_ID_PARAMS;

/*!
 * queryType[IN]
 *  - Use queryType defines to specify what information is being requested
 * status[OUT]
 *  - Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_INVALID_STATE
 */
typedef struct NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARAMS {
    NvU16 queryType;
    NvU8  reserved[2]; // To keep the struct aligned for now and available for future use (if needed)
    NvU32 status;
    union {
        NV2080_CTRL_GRMGR_GR_FS_INFO_GPC_COUNT_PARAMS                     gpcCountData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_GPC_MAP_PARAMS               chipletGpcMapData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_TPC_MASK_PARAMS                      tpcMaskData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_PPC_MASK_PARAMS                      ppcMaskData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_GPC_MAP_PARAMS     partitionGpcMapData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_CHIPLET_SYSPIPE_MASK_PARAMS          syspipeMaskData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_CHIPLET_SYSPIPE_IDS_PARAMS partitionChipletSyspipeData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_PROFILER_MON_GPC_MASK_PARAMS         dmGpcMaskData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_PARTITION_SYSPIPE_ID_PARAMS          partitionSyspipeIdData;
        NV2080_CTRL_GRMGR_GR_FS_INFO_ROP_MASK_PARAMS                      ropMaskData;
    } queryData;
} NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARAMS;

#define NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS {
    NvU16                                     numQueries;
    NvU8                                      reserved[6]; // To keep the struct aligned for now and available for future use (if needed)
    NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARAMS queries[NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES];
} NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS;

#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_INVALID                       0
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_GPC_COUNT                     1
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_GPC_MAP               2
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_TPC_MASK                      3
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PPC_MASK                      4
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_GPC_MAP     5   /* deprecated */
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_SYSPIPE_MASK          6
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_SYSPIPE_IDS 7
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PROFILER_MON_GPC_MASK         8
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_SYSPIPE_ID          9
#define NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_ROP_MASK                      10

/* _ctrl2080grmgr_h_ */
