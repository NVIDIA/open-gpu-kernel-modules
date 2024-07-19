/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlb0cc/ctrlb0ccprofiler.finn
//

#include "ctrl/ctrlb0cc/ctrlb0ccbase.h"


/*!
 * NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY
 *
 * Reserves the HWPM legacy PM system for use by the calling client.
 * This PM system will only be accessible if this reservation is
 * taken.
 *
 * If a device level reservation is held by another client, then this command
 * will fail regardless of reservation scope.
 *
 * This reservation can be released with @ref NVB0CC_CTRL_CMD_RELEASE_HWPM_LEGACY.
 *
 */


#define NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY (0xb0cc0101) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS {
    /*!
     * [in] Enable ctxsw for HWPM.
     */
    NvBool ctxsw;
} NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_RELEASE_HWPM_LEGACY
 *
 * Releases the reservation taken with @ref NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY.
 *
 * This command does not take any parameters.
 *
 */
#define NVB0CC_CTRL_CMD_RELEASE_HWPM_LEGACY  (0xb0cc0102) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0x2" */

/*!
 * NVB0CC_CTRL_CMD_RESERVE_PM_AREA_SMPC
 *
 * Reserves the SMPC PM system for use by the calling client.
 * This PM system will only be accessible if this reservation is
 * taken.
 *
 * Reservation scope and rules are same as for @ref NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY.
 *
 * This reservation can be released with @ref NVB0CC_CTRL_CMD_RELEASE_PM_AREA_SMPC.
 *
 */
#define NVB0CC_CTRL_CMD_RESERVE_PM_AREA_SMPC (0xb0cc0103) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS {
    /*!
     * [in] Enable ctxsw for SMPC.
     */
    NvBool ctxsw;
} NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_RELEASE_PM_AREA_SMPC
 *
 * Releases the reservation taken with @ref NVB0CC_CTRL_CMD_RESERVE_PM_AREA_SMPC.
 *
 * This command does not take any parameters.
 *
 */
#define NVB0CC_CTRL_CMD_RELEASE_PM_AREA_SMPC (0xb0cc0104) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0x4" */

/*!
 * NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM
 *
 * Allocates PMA VA and map it to the buffers for streaming records and for
 * for streaming the updated bytes available in the buffer.
 *
 */
#define NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM     (0xb0cc0105) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_MESSAGE_ID" */

/*!
 * Defines the maximum size of PMA buffer for streamout. It can be up to 4GB minus one page
 * reserved for streaming mem_bytes (see @ref NVB0CC_PMA_BYTES_AVAILABLE_SIZE).
 */
#define NVB0CC_PMA_BUFFER_SIZE_MAX           (0xffe00000ULL) /* finn: Evaluated from "(4 * 1024 * 1024 * 1024 - 2 * 1024 * 1024)" */
#define NVB0CC_PMA_BYTES_AVAILABLE_SIZE      (0x1000) /* finn: Evaluated from "(4 * 1024)" */

#define NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS {
    /*!
     * [in] Memory handle (RW memory) for streaming records.
     * Size of this must be >= @ref pmaBufferOffset + @ref pmaBufferSize.
     */
    NvHandle hMemPmaBuffer;

    /*!
     * [in] Start offset of PMA buffer (offset in @ref hMemPmaBuffer).
     */
    NV_DECLARE_ALIGNED(NvU64 pmaBufferOffset, 8);

    /*!
     * [in] size of the buffer. This must be <= NVB0CC_PMA_BUFFER_SIZE_MAX.
     */
    NV_DECLARE_ALIGNED(NvU64 pmaBufferSize, 8);

    /*!
     * [in] Memory handle (RO memory) for streaming number of bytes available.
     * Size of this must be of at least @ref pmaBytesAvailableOffset +
     * @ref NVB0CC_PMA_BYTES_AVAILABLE_SIZE.
     */
    NvHandle hMemPmaBytesAvailable;

    /*!
     * [in] Start offset of PMA bytes available buffer (offset in @ref hMemPmaBytesAvailable).
     */
    NV_DECLARE_ALIGNED(NvU64 pmaBytesAvailableOffset, 8);

    /*!
     * [in] Enable ctxsw for PMA stream.
     */
    NvBool   ctxsw;

    /*!
     * [out] The PMA Channel Index associated with a given PMA stream.
     */
    NvU32    pmaChannelIdx;

    /*!
     * [out] PMA buffer VA. Note that this is a HWPM Virtual address.
     */
    NV_DECLARE_ALIGNED(NvU64 pmaBufferVA, 8);
} NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_FREE_PMA_STREAM
 *
 * Releases (unmap and free) PMA stream allocated through
 * @ref NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM.
 *
 */
#define NVB0CC_CTRL_CMD_FREE_PMA_STREAM (0xb0cc0106) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS {
    /*!
     * [in] The PMA channel index associated with a given PMA stream.
     */
    NvU32 pmaChannelIdx;
} NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS;



/*!
 * NVB0CC_CTRL_CMD_BIND_PM_RESOURCES
 *
 * Binds all PM resources reserved through @ref NVB0CC_CTRL_CMD_RESERVE_*
 * and with @ref NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM with PMA engine.
 * After this call, interface is ready for programming a collection
 * of counters.
 * @Note: Any new PM resource reservation via NVB0CC_CTRL_CMD_RESERVE_* or
 * @ref NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM request after this call will fail,
 * clients need to unbind (see @ref NVB0CC_CTRL_CMD_UNBIND_PM_RESOURCES) to
 * reserve more resources.
 *
 * This can be unbound with @ref NVB0CC_CTRL_CMD_UNBIND_PM_RESOURCES.
 *
 */
#define NVB0CC_CTRL_CMD_BIND_PM_RESOURCES         (0xb0cc0107) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0x7" */

/*!
 * NVB0CC_CTRL_CMD_UNBIND_PM_RESOURCES
 *
 * Unbinds PM resources that were bound with @ref NVB0CC_CTRL_CMD_BIND_PM_RESOURCES
 *
 */
#define NVB0CC_CTRL_CMD_UNBIND_PM_RESOURCES       (0xb0cc0108) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0x8" */

/*!
 * NVB0CC_CTRL_CMD_PMA_STREAM_UPDATE_GET_PUT
 *
 * This command updates bytes consumed by the SW and optionally gets the
 * current available bytes in the buffer.
 *
 */
#define NVB0CC_CTRL_CMD_PMA_STREAM_UPDATE_GET_PUT (0xb0cc0109) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_MESSAGE_ID" */

#define NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE      0xFFFFFFFF
#define NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_MESSAGE_ID (0x9U)

typedef struct NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS {
    /*!
     * [in] Total bytes consumed by SW since last update.
     */
    NV_DECLARE_ALIGNED(NvU64 bytesConsumed, 8);

    /*!
     * [in] Initiate streaming of the bytes available (see @ref hMemPmaBytesAvailable).
     * RM will set the memory for streaming (see @ref hMemPmaBytesAvailable) to NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE and
     * client can optionally wait (see @ref bWait) for it to change from this value.
     */
    NvBool bUpdateAvailableBytes;

    /*!
     * [in] Waits for available bytes to get updated
     */
    NvBool bWait;

    /*!
     * [out] Bytes available in the PMA buffer (see @ref hMemPmaBuffer) for SW to consume.
     * This will only be populated if both bUpdateAvailableBytes and bWait are set
     * to TRUE.
     */
    NV_DECLARE_ALIGNED(NvU64 bytesAvailable, 8);

    /*!
     * [in] If set to TRUE, current put pointer will be returned in @ref putPtr.
     */
    NvBool bReturnPut;

    /*!
     * [out] Current PUT pointer (MEM_HEAD).
     * This will only be populated if bReturnPut is set to TRUE.
     */
    NV_DECLARE_ALIGNED(NvU64 putPtr, 8);

    /*!
     * [in] The PMA Channel Index associated with a given PMA stream.
     */
    NvU32  pmaChannelIdx;

    /*!
     * [out] Set to TRUE if PMA buffer has overflowed.
     */
    NvBool bOverflowStatus;
} NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS;

/*!
 * Maximum number of register operations allowed in a single request.
 * @NOTE: @ref NVB0CC_REGOPS_MAX_COUNT is chosen to keep struct size
 * of @ref NVB0CC_CTRL_EXEC_REG_OPS_PARAMS under 4KB.
 */
#define NVB0CC_REGOPS_MAX_COUNT      (124)

/*!
 * NVB0CC_CTRL_CMD_EXEC_REG_OPS
 *
 * This command is used to submit an array containing one or more
 * register operations for processing.  Each entry in the
 * array specifies a single read or write operation. Each entry is
 * checked for validity in the initial pass: Only registers from PM area
 * are allowed using this interface and only register from PM systems for
 * which user has a valid reservation are allowed (see @ref NVB0CC_CTRL_CMD_RESERVE_*).
 * Operation type (@ref  NV2080_CTRL_GPU_REG_OP_TYPE_*) is not required to be passed in.
 */
#define NVB0CC_CTRL_CMD_EXEC_REG_OPS (0xb0cc010a) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_MESSAGE_ID" */

/*!
 * Structure definition for register operation. See @ref NV2080_CTRL_GPU_REG_OP.
 */
typedef NV2080_CTRL_GPU_REG_OP NVB0CC_GPU_REG_OP;

/*!
 * Enumeration of different REG_OPS modes. This mode determines how a failure
 * of a regop is handled in a batch of regops.
 */
typedef enum NVB0CC_REGOPS_MODE {

    /*!
     * Either all regops will be executed or none of them will be executed.
     * Failing regop will have the appropriate status (see @ref NVB0CC_GPU_REG_OP::regStatus).
     */
    NVB0CC_REGOPS_MODE_ALL_OR_NONE = 0,
    /*!
     * All regops will be attempted and the ones that failed will have the
     * the appropriate status (see @ref NVB0CC_GPU_REG_OP::regStatus).
     */
    NVB0CC_REGOPS_MODE_CONTINUE_ON_ERROR = 1,
} NVB0CC_REGOPS_MODE;

#define NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_MESSAGE_ID (0xAU)

typedef struct NVB0CC_CTRL_EXEC_REG_OPS_PARAMS {
    /*!
     * [in] Number of valid entries in the regOps array. This value cannot
     * exceed NVB0CC_REGOPS_MAX_COUNT.
     */
    NvU32              regOpCount;

    /*!
     * [in] Specifies the mode for the entire operation see @ref NVB0CC_REGOPS_MODE.
     */
    NVB0CC_REGOPS_MODE mode;

    /*!
     * [out] Provides status for the entire operation. This is only valid for
     * mode @ref NVB0CC_REGOPS_MODE_CONTINUE_ON_ERROR.
     */
    NvBool             bPassed;

    /*!
     * [out] This is currently not populated.
     */
    NvBool             bDirect;

    /*!
     * [in/out] An array (of fixed size NVB0CC_REGOPS_MAX_COUNT) of register read or write
     * operations (see @ref NVB0CC_GPU_REG_OP)
     *
     */
    NVB0CC_GPU_REG_OP  regOps[NVB0CC_REGOPS_MAX_COUNT];
} NVB0CC_CTRL_EXEC_REG_OPS_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_RESERVE_PM_AREA_PC_SAMPLER
 *
 * Reserves the PC sampler system for use by the calling client.
 *
 * This reservation can be released with @ref NVB0CC_CTRL_CMD_RELEASE_PM_AREA_PC_SAMPLER.
 *
 * This command does not take any parameters.
 *
 * PC sampler is always context switched with a GR context, so reservation scope is
 * always context. This requires that profiler object is instantiated with a valid GR
 * context. See @ref NVB2CC_ALLOC_PARAMETERS.
 */

#define NVB0CC_CTRL_CMD_RESERVE_PM_AREA_PC_SAMPLER (0xb0cc010b) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0xB" */

/*!
 * NVB0CC_CTRL_CMD_RELEASE_PM_AREA_PC_SAMPLER
 *
 * Releases the reservation taken with @ref NVB0CC_CTRL_CMD_RESERVE_PM_AREA_PC_SAMPLER.
 *
 * This command does not take any parameters.
 *
 */
#define NVB0CC_CTRL_CMD_RELEASE_PM_AREA_PC_SAMPLER (0xb0cc010c) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | 0xC" */

/*!
 * NVB0CC_CTRL_CMD_GET_TOTAL_HS_CREDITS
 *
 * Gets the total high speed streaming credits available for the client.
 *
 * This command can only be performed after a bind using NVB0CC_CTRL_CMD_BIND_PM_RESOURCES.
 *
 */
#define NVB0CC_CTRL_CMD_GET_TOTAL_HS_CREDITS       (0xb0cc010d) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_MESSAGE_ID (0xDU)

typedef struct NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS {
    NvU32 numCredits;
} NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_SET_HS_CREDITS_CHIPLET
 *
 * Sets per chiplet (pmm router) credits for high speed streaming for a pma channel.
 *
 * @note: This command resets the current credits to 0 before setting the new values also
 *        if programming fails, it will reset credits to 0 for all the chiplets.
 *
 */
#define NVB0CC_CTRL_CMD_SET_HS_CREDITS (0xb0cc010e) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_MESSAGE_ID" */

typedef enum NVB0CC_CHIPLET_TYPE {
    NVB0CC_CHIPLET_TYPE_INVALID = 0,
    NVB0CC_CHIPLET_TYPE_FBP = 1,
    NVB0CC_CHIPLET_TYPE_GPC = 2,
    NVB0CC_CHIPLET_TYPE_SYS = 3,
} NVB0CC_CHIPLET_TYPE;

typedef enum NVB0CC_HS_CREDITS_CMD_STATUS {
    NVB0CC_HS_CREDITS_CMD_STATUS_OK = 0,
    /*!
     * More credits are requested than the total credits. Total credits can be queried using @ref NVB0CC_CTRL_CMD_GET_TOTAL_HS_CREDITS
     */
    NVB0CC_HS_CREDITS_CMD_STATUS_INVALID_CREDITS = 1,
    /*!
     * Chiplet index is invalid.
     */
    NVB0CC_HS_CREDITS_CMD_STATUS_INVALID_CHIPLET = 2,
} NVB0CC_HS_CREDITS_CMD_STATUS;

typedef struct NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_INFO {
    /*!
     * Specifies the chiplet type @ref NVB0CC_CHIPLET_TYPE.
     */
    NvU8  chipletType;

    /*!
     * Specifies the logical index of the chiplet.
     */
    NvU8  chipletIndex;

    /*!
     * Specifies the number of credits for the chiplet.
     */
    NvU16 numCredits;
} NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_INFO;

typedef struct NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_STATUS {
    /*!
     * Status for the command @ref NVB0CC_HS_CREDITS_CMD_STATUS.
     */
    NvU8 status;

    /*!
     * Index of the failing @ref NVB0CC_CTRL_SET_HS_CREDITS_PARAMS::creditInfo entry. This
     * is only relevant if status is NVB0CC_HS_CREDITS_CMD_STATUS_INVALID_CHIPLET.
     */
    NvU8 entryIndex;
} NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_STATUS;

#define NVB0CC_MAX_CREDIT_INFO_ENTRIES (63)

typedef struct NVB0CC_CTRL_HS_CREDITS_PARAMS {
    /*!
     * [in] The PMA Channel Index associated with a given PMA stream.
     */
    NvU8                                     pmaChannelIdx;

    /*!
     * [in] Number of valid entries in creditInfo.
     */
    NvU8                                     numEntries;

    /*!
     * [out] Provides status for the entire operation.
     */
    NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_STATUS statusInfo;

    /*!
     * [in] Credit programming per chiplet
     */
    NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_INFO   creditInfo[NVB0CC_MAX_CREDIT_INFO_ENTRIES];
} NVB0CC_CTRL_HS_CREDITS_PARAMS;

#define NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_MESSAGE_ID (0xEU)

typedef NVB0CC_CTRL_HS_CREDITS_PARAMS NVB0CC_CTRL_SET_HS_CREDITS_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_GET_HS_CREDITS
 *
 * Gets per chiplet (pmm router) high speed streaming credits for a pma channel.
 *
 */
#define NVB0CC_CTRL_CMD_GET_HS_CREDITS (0xb0cc010f) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_MESSAGE_ID (0xFU)

typedef NVB0CC_CTRL_HS_CREDITS_PARAMS NVB0CC_CTRL_GET_HS_CREDITS_PARAMS;



typedef enum NVB0CC_CTRL_HES_TYPE {
    NVB0CC_CTRL_HES_INVALID = 0,
    NVB0CC_CTRL_HES_CWD = 1,
} NVB0CC_CTRL_HES_TYPE;

typedef struct NVB0CC_CTRL_RESERVE_HES_CWD_PARAMS {
    /*!
     * [in] Enable ctxsw for HES_CWD.
     */
    NvBool ctxsw;
} NVB0CC_CTRL_RESERVE_HES_CWD_PARAMS;

/*
 * NVB0CC_CTRL_HES_RESERVATION_UNION
 *
 * Union of all HES reservation params
 *
 */
typedef union NVB0CC_CTRL_HES_RESERVATION_UNION {
    NVB0CC_CTRL_RESERVE_HES_CWD_PARAMS cwd;
} NVB0CC_CTRL_HES_RESERVATION_UNION;

/*!
 * NVB0CC_CTRL_CMD_RESERVE_HES
 *
 * Reserves HES for use by the calling client.
 * This PM system will only be accessible if this reservation is
 * taken.
 *
 * This reservation can be released with @ref NVB0CC_CTRL_CMD_RELEASE_HES.
 *
 * Reservation scope and rules are same as for @ref NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY.
 *
 */
#define NVB0CC_CTRL_CMD_RESERVE_HES (0xb0cc0113) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_RESERVE_HES_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_RESERVE_HES_PARAMS_MESSAGE_ID (0x13U)

typedef struct NVB0CC_CTRL_RESERVE_HES_PARAMS {
    /*!
     * [in] Denotes the HES reservation type. Choose from @NVB0CC_CTRL_HES_TYPE.
     */
    NvU32                             type;

    /*!
     * [in] Union of all possible reserve param structs. Initialize the reserveParams corresponding to the chosen type.
     */
    NVB0CC_CTRL_HES_RESERVATION_UNION reserveParams;
} NVB0CC_CTRL_RESERVE_HES_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_RELEASE_HES
 *
 * Releases the reservation taken with @ref NVB0CC_CTRL_CMD_RESERVE_HES.
 *
 */
#define NVB0CC_CTRL_CMD_RELEASE_HES (0xb0cc0114) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_RELEASE_HES_PARAMS_MESSAGE_ID" */

#define NVB0CC_CTRL_RELEASE_HES_PARAMS_MESSAGE_ID (0x14U)

typedef struct NVB0CC_CTRL_RELEASE_HES_PARAMS {
    /*!
     * [in] type of @NVB0CC_CTRL_HES_TYPE needs to be released.
     */
    NVB0CC_CTRL_HES_TYPE type;
} NVB0CC_CTRL_RELEASE_HES_PARAMS;

/*!
 * Defines the maximum count of output credit pools.
 * 30 is estimate based on the # of PMAs (2) and chiplet types(3), 
 * which should be big enough to accommodate the required number of credit pools
 */
#define NVB0CC_CREDIT_POOL_MAX_COUNT 30

/*!
 * NVB0CC_CTRL_CMD_GET_CHIPLET_HS_CREDIT_POOL
 *
 * Gets the total high speed streaming credits available for the client 
 * in each chiplet pool.
 *
 * This command is similar to @ref NVB0CC_CTRL_CMD_GET_TOTAL_HS_CREDITS but
 * supports multiple chiplet credit pools.
 *
 */
typedef struct NVB0CC_CTRL_CREDIT_POOL_INFO {
    /*!
     * [out] number of credits.
     */
    NvU16 numCredits;

    /*!
     * [out] index of credit pool.
     */
    NvU8  poolIndex;

    /*!
     * [out] chiplet type of credit pool.
     */
    NvU8  chipletType;
} NVB0CC_CTRL_CREDIT_POOL_INFO;
#define NVB0CC_CTRL_CMD_GET_CHIPLET_HS_CREDIT_POOL (0xb0cc0115) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL_MESSAGE_ID" */

#define NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL_MESSAGE_ID (0x15U)

typedef struct NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL {
    /*!
     * [out] chiplet-level credit pool.
     */
    NVB0CC_CTRL_CREDIT_POOL_INFO poolInfos[NVB0CC_CREDIT_POOL_MAX_COUNT];

    /*!
     * [out] number of credit pools.
     */
    NvU32                        poolInfosCount;
} NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL;

typedef struct NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_MAPPING_INFO {
    /*! 
     * [in] Specifies the chiplet type @ref NVB0CC_CHIPLET_TYPE. 
     */
    NvU8 chipletType;

    /*! 
     * [in] Specifies the logical index of the chiplet. 
     */
    NvU8 chipletIndex;

    /*! 
     * [out] Specifies the index of credits pool for the chiplet. 
     */
    NvU8 poolIndex;
} NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_MAPPING_INFO;

/*!
 * NVB0CC_CTRL_CMD_GET_HS_CREDITS_MAPPING
 * 
 * Query the associated PMA credit pool index for given chiplet.
 *
 */
#define NVB0CC_CTRL_CMD_GET_HS_CREDITS_MAPPING (0xb0cc0116) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_MESSAGE_ID" */
#define NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_MESSAGE_ID (0x16U)

typedef struct NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS {
    /*!
     * [in]: number of input queries.
     */
    NvU16                                          numQueries;

    /*!
     * [out] Provides status for the entire operation.
     */
    NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_STATUS       statusInfo;

    /*!
     * [in/out]: Used to query the PMA credit pool index of specified chiplet.
     */
    NVB0CC_CTRL_PMA_STREAM_HS_CREDITS_MAPPING_INFO queries[NVB0CC_MAX_CREDIT_INFO_ENTRIES];
} NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS;

 /* End of extension construct */


/*!
 * NVB0CC_CTRL_CMD_DISABLE_DYNAMIC_MMA_BOOST
 * 
 * Disable the Dynamic MMA clock boost during profiler lifetime.
 *
 */
#define NVB0CC_CTRL_CMD_DISABLE_DYNAMIC_MMA_BOOST (0xb0cc0117) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS_MESSAGE_ID" */
#define NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS_MESSAGE_ID (0x17U)

typedef struct NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS {
    /*!
     * [in]: En/Disable Dynamic MMA Boost.  True = disable Boost; False = re-enable Boost.
     */
    NvBool disable;
} NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS;

/*!
 * NVB0CC_CTRL_CMD_GET_DYNAMIC_MMA_BOOST_STATUS
 * 
 * Request the Dynamic MMA clock boost feature enablement status.
 *
 */
#define NVB0CC_CTRL_CMD_GET_DYNAMIC_MMA_BOOST_STATUS (0xb0cc0118) /* finn: Evaluated from "(FINN_MAXWELL_PROFILER_PROFILER_INTERFACE_ID << 8) | NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS_MESSAGE_ID" */
#define NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS_MESSAGE_ID (0x18U)

typedef struct NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS {
    /*!
     * [out]: Dynamic MMA Boost status: true = boost enabled/available; False = Boost disabled/unavailable.
     */
    NvBool enabled;
} NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS;



/* _ctrlb0ccprofiler_h_ */
