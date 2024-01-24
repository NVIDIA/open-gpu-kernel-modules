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
// Source file:      ctrl/ctrlc637.finn
//

#include "ctrl/ctrlxxxx.h"
/* AMPERE_SMC_PARTITION_REF commands and parameters */

#define NVC637_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0xC637, NVC637_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVC637_CTRL_RESERVED        (0x00)
#define NVC637_CTRL_EXEC_PARTITIONS (0x01)


/*!
 * NVC637_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC637_CTRL_CMD_NULL        (0xc6370000) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_RESERVED_INTERFACE_ID << 8) | 0x0" */

/*!
 * NVC637_CTRL_EXEC_PARTITIONS_INFO
 *
 * This structure specifies resources in an execution partition
 *
 *  gpcCount[IN/OUT]
 *      - Total Number of GPCs in this partition (including GFX Supported GPCs)
 *
 *  gfxGpcCount[IN/OUT]
 *      - Number of GFX GPCs in this partition. This should be a subset of gpcs inlcuded in gpcCount
 *
 *  VeidCount[OUT]
 *      - Number of VEIDs available in this partition.
 *
 *  ceCount[IN/OUT]
 *      - Copy Engines in this partition
 *
 *  nvEncCount[IN/OUT]
 *      - Encoder Engines in this partition
 *
 *  nvDecCount[IN/OUT]
 *      - Decoder Engines in this partition
 *
 *  nvJpgCount[IN/OUT]
 *      - Jpg Engines in this partition
 *
 *  nvOfaCount[IN/OUT]
 *      - Ofa engines in this partition
 *
 *  sharedEngFlags[IN/OUT]
 *      - Flags determining whether above engines are shared with other execution partitions
 *
 *  veidStartOffset[OUT]
 *      - VEID start offset within GPU partition
 *
 *  smCount[IN/OUT]
 *      - Number of active SMs in this partition
 *
 *  spanStart[IN/OUT]
 *      - First slot in the span for an execution partition placement
 *
 *  computeSize[IN/OUT]
 *      - Flag corresponding to the compute profile used 
 *        
 */
typedef struct NVC637_CTRL_EXEC_PARTITIONS_INFO {
    NvU32 gpcCount;
    NvU32 gfxGpcCount;
    NvU32 veidCount;
    NvU32 ceCount;
    NvU32 nvEncCount;
    NvU32 nvDecCount;
    NvU32 nvJpgCount;
    NvU32 ofaCount;
    NvU32 sharedEngFlag;
    NvU32 veidStartOffset;
    NvU32 smCount;
    NvU32 spanStart;
    NvU32 computeSize;
} NVC637_CTRL_EXEC_PARTITIONS_INFO;

#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG         31:0
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NONE                      0x0
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_CE      NVBIT(0)
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVDEC   NVBIT(1)
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVENC   NVBIT(2)
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_OFA     NVBIT(3)
#define NVC637_CTRL_EXEC_PARTITIONS_SHARED_FLAG_NVJPG   NVBIT(4)

#define NVC637_CTRL_MAX_EXEC_PARTITIONS                                   8
#define NVC637_CTRL_EXEC_PARTITIONS_ID_INVALID                            0xFFFFFFFF

/*!
 * NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS
 *
 * This command will create requested execution partitions under the subscribed
 * memory partition. The memory partition is expected to be configured before
 * execution partition creation.
 *
 * bQuery[IN]
 *      - If NV_TRUE, execution partitions will not be created, but return
 *      status of NV_OK will indicate that the request is valid and can
 *      currently be fulfilled
 * flag [IN]
 *      REQUEST_WITH_PART_ID
 *      - If set, RM will try to assign execution partition id requested by clients.
 *      This flag is only supported on vGPU enabled RM build and will be removed
 *      when vgpu plugin implements virtualized execution partition ID support.
 *      (bug 2938187)
 *
 *      REQUEST_AT_SPAN
 *      - If set, RM will try to assign execution partition resources at the specified span.
 *      This flag currently is only useful for chips in which CTS IDs are mandatory in RM,
 *      as it allows the requester to position compute instances without using RM best fit
 *      allocation.
 *
 * execPartCount[IN]
 *      - Number of execution partitions requested
 *
 * execPartInfo[IN]
 *      - Requested execution partition resources for each requested partition
 *
 * execPartId[OUT]
 *      - ID of each requested execution partition
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_WITH_PART_ID                   0:0
#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_WITH_PART_ID_FALSE (0x00000000)
#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_WITH_PART_ID_TRUE  (0x00000001)
#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_AT_SPAN                        1:1
#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_AT_SPAN_FALSE      (0x00000000)
#define NVC637_CTRL_DMA_EXEC_PARTITIONS_CREATE_REQUEST_AT_SPAN_TRUE       (0x00000001)

#define NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE (0xc6370101) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS_MESSAGE_ID" */

/*!
 * NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS
 *
 * This command will delete requested execution partitions.
 *
 * execPartCount[IN]
 *      - Number of execution partitions to delete.
 *
 * execPartId[IN]
 *      - Execution partition IDs to delete
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_OBJECT_NOT_FOUND
 */
#define NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS {
    NvBool                           bQuery;
    NvU32                            flags;
    NvU32                            execPartCount;
    // C form: NVC637_CTRL_EXEC_PARTITIONS_INFO execPartInfo[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NVC637_CTRL_EXEC_PARTITIONS_INFO execPartInfo[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    // C form: NvU32 execPartId[NVC637_CTRL_MAX_EXECUTION_PARTITIONS];
    NvU32                            execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];
} NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS;
#define NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS {
    NvU32 execPartCount;
    NvU32 execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];
} NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS;

#define NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE (0xc6370102) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS_MESSAGE_ID" */

/*!
 * NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS
 *
 * This command will return information about execution partitions which
 * currently exist within the subscribed memory partition.
 *
 * execPartCount[OUT]
 *      - Number of existing execution partitions
 *
 * execPartId[OUT]
 *      - ID of existing execution partitions
 *
 * execPartInfo[OUT]
 *      - Resources within each existing execution partition
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS {
    NvU32                            execPartCount;
    // C form: NvU32 execPartId[NVC637_CTRL_MAX_EXECUTION_PARTITIONS];
    NvU32                            execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    // C form: NVC637_CTRL_EXEC_PARTITIONS_INFO execPartInfo[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NVC637_CTRL_EXEC_PARTITIONS_INFO execPartInfo[NVC637_CTRL_MAX_EXEC_PARTITIONS];
} NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS;

#define NVC637_CTRL_CMD_EXEC_PARTITIONS_GET (0xc6370103) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS_MESSAGE_ID" */

/*!
 * NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS
 *
 * This command will return IDs of all active execution partitions in a memory
 * partition
 *
 * execPartCount[OUT]
 *      - Number of existing execution partitions
 *
 * execPartId[OUT]
 *      - ID of existing execution partitions
 *
 * execPartUuid[OUT]
 *      - ASCII UUID string of existing execution partitions
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

/* 'M' 'I' 'G' '-'(x5), '\0x0', extra = 9 */
#define NVC637_UUID_LEN                     16
#define NVC637_UUID_STR_LEN                 (0x29) /* finn: Evaluated from "((NVC637_UUID_LEN << 1) + 9)" */

typedef struct NVC637_EXEC_PARTITION_UUID {
    // C form: char str[NVC638_UUID_STR_LEN];
    char str[NVC637_UUID_STR_LEN];
} NVC637_EXEC_PARTITION_UUID;

#define NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS {
    NvU32                      execPartCount;

    // C form: NvU32 execPartId[NVC637_CTRL_MAX_EXECUTION_PARTITIONS];
    NvU32                      execPartId[NVC637_CTRL_MAX_EXEC_PARTITIONS];

    // C form: NVC637_EXEC_PARTITION_UUID execPartUuid[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NVC637_EXEC_PARTITION_UUID execPartUuid[NVC637_CTRL_MAX_EXEC_PARTITIONS];
} NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS;

#define NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS               (0xc6370104) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS_MESSAGE_ID" */

/*
 * NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT
 *
 * Export the resource and placement information about an exec partition such
 * that a similar partition can be recreated from scratch in the same position.
 */
#define NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT                   (0xc6370105) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | 0x5" */

/*
 * NVC637_CTRL_CMD_EXEC_PARTITIONS_IMPORT
 *
 * Create an exec partition resembling the exported partition info. The imported
 * partition should behave identically with respect to fragmentation.
 */
#define NVC637_CTRL_CMD_EXEC_PARTITIONS_IMPORT                   (0xc6370106) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | 0x6" */

#define NVC637_CTRL_EXEC_PARTITIONS_EXPORT_MAX_ENGINES_MASK_SIZE 4
typedef struct NVC637_CTRL_EXEC_PARTITIONS_EXPORTED_INFO {
    NV_DECLARE_ALIGNED(NvU64 enginesMask[NVC637_CTRL_EXEC_PARTITIONS_EXPORT_MAX_ENGINES_MASK_SIZE], 8);
    NvU8  uuid[NVC637_UUID_LEN];
    NvU32 sharedEngFlags;
    NvU32 gpcMask;
    NvU32 gfxGpcCount;
    NvU32 veidOffset;
    NvU32 veidCount;
    NvU32 smCount;
    NvU32 spanStart;
    NvU32 computeSize;
} NVC637_CTRL_EXEC_PARTITIONS_EXPORTED_INFO;

typedef struct NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS {
    NvU32  id;
    NV_DECLARE_ALIGNED(NVC637_CTRL_EXEC_PARTITIONS_EXPORTED_INFO info, 8);
    NvBool bCreateCap;
} NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS;

/*
 * NVC637_CTRL_EXEC_PARTITION_PARTITION_SPAN
 *
 * This struct represents the span of a compute instance, which represents the
 * resource slots a given partition occupies (or may occupy) within a fixed range which
 * is defined per-chip. A partition containing more resources will cover more
 * resource slots and therefore cover a larger span.
 *
 *   lo
 *      - The starting unit of this span, inclusive
 *
 *   hi
 *      - The ending unit of this span, inclusive
 *
 */
typedef struct NVC637_CTRL_EXEC_PARTITION_PARTITION_SPAN {
    NV_DECLARE_ALIGNED(NvU64 lo, 8);
    NV_DECLARE_ALIGNED(NvU64 hi, 8);
} NVC637_CTRL_EXEC_PARTITION_PARTITION_SPAN;

/*
 * NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY
 *
 * This command returns the count of compute instances which can be created 
 * of the given commpute profile size (represented by the computeSize field
 * within a profile) which can be requested via NV2080_CTRL_CMD_GPU_GET_COMPUTE_PROFILES
 * Note that this API does not "reserve" any partitions, and there is no
 * guarantee that the reported count of available partitions of a given size
 * will remain consistent following creation of partitions of different size
 * through NV2080_CTRL_GPU_SET_PARTITIONS.
 * Note that this API is unsupported if SMC is feature-disabled.
 * Note that the caller of this CTRL must be subscribed to a valid GPU instance
 *
 *   computeSize[IN]
 *      - Partition flag indicating size of requested profile
 *
 *   profileCount[OUT]
 *      - Available number of profiles of the given size which can currently be created.
 *
 *   availableSpans[OUT]
 *      - For each profile able to be created of the specified size, the span
 *        it could occupy.
 *
 *   availableSpansCount[OUT]
 *      - Number of valid entries in availableSpans.
 *
 *   totalProfileCount[OUT]
 *      - Total number of profiles of the given size which can be created.
 *
 *   totalSpans[OUT]
 *      - List of spans which can possibly be occupied by profiles of the
 *        given type.
 *
 *   totalSpansCount[OUT]
 *      - Number of valid entries in totalSpans.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NVC637_CTRL_CMD_EXEC_PARTITIONS_GET_PROFILE_CAPACITY (0xc63701a9U) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS_MESSAGE_ID" */

#define NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS_MESSAGE_ID (0xA9U)

typedef struct NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS {
    NvU32 computeSize;
    NvU32 profileCount;
    NV_DECLARE_ALIGNED(NVC637_CTRL_EXEC_PARTITION_PARTITION_SPAN availableSpans[NVC637_CTRL_MAX_EXEC_PARTITIONS], 8);
    NvU32 availableSpansCount;
    NvU32 totalProfileCount;
    NV_DECLARE_ALIGNED(NVC637_CTRL_EXEC_PARTITION_PARTITION_SPAN totalSpans[NVC637_CTRL_MAX_EXEC_PARTITIONS], 8);
    NvU32 totalSpansCount;
} NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS;

/*
 * NVC637_CTRL_CMD_GET_UUID
 */
#define NVC637_CTRL_CMD_GET_UUID (0xc63701aaU) /* finn: Evaluated from "(FINN_AMPERE_SMC_PARTITION_REF_EXEC_PARTITIONS_INTERFACE_ID << 8) | NVC637_CTRL_GET_UUID_PARAMS_MESSAGE_ID" */

#define NVC637_CTRL_GET_UUID_PARAMS_MESSAGE_ID (0xAAU)

typedef struct NVC637_CTRL_GET_UUID_PARAMS {
    NvU8 uuid[NVC637_UUID_LEN];

    char uuidStr[NVC637_UUID_STR_LEN];
} NVC637_CTRL_GET_UUID_PARAMS;

//  _ctrlc637_h_
