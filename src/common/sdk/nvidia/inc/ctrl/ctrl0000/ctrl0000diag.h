/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000diag.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
/* NV01_ROOT (client) system control commands and parameters */

/*
 * NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_STATE
 *
 * This command returns the current lock meter logging state.
 *
 *   state
 *     This parameter returns the current lock meter logging state.
 *       NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_DISABLED
 *         This value indicates lock metering is disabled.
 *       NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_ENABLED
 *         This value indicates lock metering is enabled.
 *   count
 *     This parameter returns the total number of lock metering entries
 *     (NV0000_CTRL_DIAG_LOCK_METER_ENTRY) available.  This value will
 *     not exceed NV0000_CTRL_DIAG_LOCK_METER_MAX_ENTRIES.  When lock metering
 *     is enabled this parameter will return zero.
 *   missedCount
 *     This parameter returns the number of lock metering entries that had
 *     to be discarded due to a  full lock metering table.  This value will
 *     not exceed NV0000_CTRL_DIAG_LOCK_METER_MAX_TABLE_ENTRIES.  When lock
 *     metering is enabled this parameter will return zero.
 *   bCircularBuffer
 *     This parameter returns type of buffer.
 *       TRUE
 *         Buffer is circular
 *       FALSE
 *         Buffer is sequential 
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_STATE (0x480) /* finn: Evaluated from "(FINN_NV01_ROOT_DIAG_INTERFACE_ID << 8) | NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_PARAMS_MESSAGE_ID (0x80U)

typedef struct NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_PARAMS {
    NvU32  state;
    NvU32  count;
    NvU32  missedCount;
    NvBool bCircularBuffer;
} NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_PARAMS;

/* valid lock metering state values */
#define NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_DISABLED (0x00000000)
#define NV0000_CTRL_DIAG_GET_LOCK_METER_STATE_ENABLED  (0x00000001)

/* maximum possible number of lock metering entries stored internally */
#define NV0000_CTRL_DIAG_LOCK_METER_MAX_TABLE_ENTRIES  (0x20000)

/*
 * NV0000_CTRL_CMD_DIAG_SET_LOCK_METER_STATE
 *
 * This command sets the current lock meter logging state.
 *
 *   state
 *     This parameter specifies the new state of the lock metering mechanism.
 *     Legal state values are:
 *       NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_DISABLE
 *         This value disables lock metering.
 *       NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_ENABLE
 *         This value enables lock metering.
 *       NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_RESET
 *         This value resets, or clears, all lock metering state.  Lock
 *         metering must be disabled prior to attempting a reset.
 *   bCircularBuffer
 *     This parameter specifies type of buffer.
 *     Possible values are:
 *       TRUE
 *         For circular buffer.
 *       FALSE
 *         For sequential buffer.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0000_CTRL_CMD_DIAG_SET_LOCK_METER_STATE      (0x481) /* finn: Evaluated from "(FINN_NV01_ROOT_DIAG_INTERFACE_ID << 8) | NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_PARAMS {
    NvU32  state;
    NvBool bCircularBuffer;
} NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_PARAMS;

/* valid lock metering state values */
#define NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_DISABLE     (0x00000000)
#define NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_ENABLE      (0x00000001)
#define NV0000_CTRL_DIAG_SET_LOCK_METER_STATE_RESET       (0x00000002)

/*
 * NV0000_CTRL_DIAG_LOCK_METER_ENTRY
 *
 * This structure represents a single lock meter entry.
 *
 *   counter
 *     This field contains the number of nanonseconds elapsed since the
 *     the last system boot when the lock meter entry was generated.
 *   freq
 *     This field contains the CPU performance counter frequency in units
 *     of ticks per second.
 *   line
 *     This field contains the relevant line number.
 *   filename
 *     This field contains the relevant file name.
 *   tag
 *     This field contains a tag uniquely identifying the user of the metered
 *     lock operations.
 *   cpuNum
 *     This field contains the CPU number from which the metered operation
 *     was initiated.
 *   irql
 *     This field contains the IRQL at which the metered operation was
 *     initiated.
 *   data0
 *   data1
 *   data2
 *     These fields contain tag-specific data.
 */
#define NV0000_CTRL_DIAG_LOCK_METER_ENTRY_FILENAME_LENGTH (0xc)

typedef struct NV0000_CTRL_DIAG_LOCK_METER_ENTRY {
    NV_DECLARE_ALIGNED(NvU64 counter, 8);

    NvU32 line;
    NvU8  filename[NV0000_CTRL_DIAG_LOCK_METER_ENTRY_FILENAME_LENGTH];

    NvU16 tag;
    NvU8  cpuNum;
    NvU8  irql;

    NV_DECLARE_ALIGNED(NvU64 threadId, 8);

    NvU32 data0;
    NvU32 data1;
    NvU32 data2;
} NV0000_CTRL_DIAG_LOCK_METER_ENTRY;

/* valid lock meter entry tag values */
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ACQUIRE_SEMA        (0x00000001)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ACQUIRE_SEMA_FORCED (0x00000002)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ACQUIRE_SEMA_COND   (0x00000003)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_RELEASE_SEMA        (0x00000004)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ACQUIRE_API         (0x00000010)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_RELEASE_API         (0x00000011)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ACQUIRE_GPUS        (0x00000020)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_RELEASE_GPUS        (0x00000021)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_DATA                (0x00000100)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_RMCTRL              (0x00001000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_CFG_GETEX           (0x00002002)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_CFG_SETEX           (0x00002003)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_VIDHEAP             (0x00003000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_MAPMEM              (0x00003001)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_UNMAPMEM            (0x00003002)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_MAPMEM_DMA          (0x00003003)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_UNMAPMEM_DMA        (0x00003004)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ALLOC               (0x00004000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ALLOC_MEM           (0x00004001)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_DUP_OBJECT          (0x00004010)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_CLIENT         (0x00005000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_DEVICE         (0x00005001)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_SUBDEVICE      (0x00005002)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_SUBDEVICE_DIAG (0x00005003)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_DISP           (0x00005004)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_DISP_CMN       (0x00005005)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_CHANNEL        (0x00005006)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_CHANNEL_MPEG   (0x00005007)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_CHANNEL_DISP   (0x00005008)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_MEMORY         (0x00005009)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_FBMEM          (0x0000500A)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_OBJECT         (0x0000500B)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_FREE_EVENT          (0x0000500C)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_IDLE_CHANNELS       (0x00006000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_BIND_CTXDMA         (0x00007000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ALLOC_CTXDMA        (0x00007001)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_ISR                 (0x0000F000)
#define NV0000_CTRL_DIAG_LOCK_METER_TAG_DPC                 (0x0000F00F)

/*
 * NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_ENTRIES
 *
 * This command returns lock metering data in a fixed-sized array of entries.
 * Each request will return up NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_MAX_ENTRIES
 * entries.
 *
 * It is up to the caller to repeat these requests to retrieve the total number
 * of entries reported by NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_STATE.
 *
 *   entryCount
 *     This parameter returns the total number of valid entries returned
 *     in the entries array.  This value will not exceed
 *     NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_MAX but may be less.
 *     A value of zero indicates there are no more valid entries.
 *   entries
 *     This parameter contains the storage into which lock metering entry
 *     data is returned.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV0000_CTRL_CMD_DIAG_GET_LOCK_METER_ENTRIES         (0x485) /* finn: Evaluated from "(FINN_NV01_ROOT_DIAG_INTERFACE_ID << 8) | NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_PARAMS_MESSAGE_ID" */

/* total number of entries returned */
#define NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_MAX         (0x40)

#define NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_PARAMS_MESSAGE_ID (0x85U)

typedef struct NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_PARAMS {
    NvU32 entryCount;
    NV_DECLARE_ALIGNED(NV0000_CTRL_DIAG_LOCK_METER_ENTRY entries[NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_MAX], 8);
} NV0000_CTRL_DIAG_GET_LOCK_METER_ENTRIES_PARAMS;

/*
 * NV0000_CTRL_CMD_DIAG_PROFILE_RPC
 *
 * This command returns the RPC runtime information, and
 * will only return valid when it is running inside VGX mode.
 *
 *   rpcProfileCmd:
 *      RPC profiler command issued by rpc profiler utility
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_DIAG_PROFILE_RPC (0x488) /* finn: Evaluated from "(FINN_NV01_ROOT_DIAG_INTERFACE_ID << 8) | NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS_MESSAGE_ID" */

typedef struct RPC_METER_ENTRY {
    NV_DECLARE_ALIGNED(NvU64 startTimeInNs, 8);
    NV_DECLARE_ALIGNED(NvU64 endTimeInNs, 8);
    NV_DECLARE_ALIGNED(NvU64 rpcDataTag, 8);
    NV_DECLARE_ALIGNED(NvU64 rpcExtraData, 8);
} RPC_METER_ENTRY;

#define NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS_MESSAGE_ID (0x88U)

typedef struct NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS {
    NvU32 rpcProfileCmd;
} NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS;

#define NV0000_CTRL_PROFILE_RPC_CMD_DISABLE (0x00000000)
#define NV0000_CTRL_PROFILE_RPC_CMD_ENABLE  (0x00000001)
#define NV0000_CTRL_PROFILE_RPC_CMD_RESET   (0x00000002)

/*
 * NV0000_CTRL_CMD_DIAG_DUMP_RPC
 *
 * This command returns the RPC runtime information, which
 * will be logged by NV0000_CTRL_CMD_DIAG_PROFILE_RPC command
 * when running inside VGX mode.
 *
 * When issuing this command, the RPC profiler has to be disabled.
 *
 *   firstEntryOffset:
 *     [IN] offset for first entry.
 *
 *   outputEntryCout:
 *     [OUT] number of entries returned in rpcProfilerBuffer.
 *
 *   remainingEntryCount:
 *     [OUT] number of entries remaining. 
 *
 *   elapsedTimeInNs:
 *     [OUT] runtime for the RPC profiler tool. 
 *
 *   rpcProfilerBuffer:
 *     [OUT] buffer to store the RPC entries
 */

#define NV0000_CTRL_CMD_DIAG_DUMP_RPC       (0x489) /* finn: Evaluated from "(FINN_NV01_ROOT_DIAG_INTERFACE_ID << 8) | NV0000_CTRL_DIAG_DUMP_RPC_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_DIAG_RPC_MAX_ENTRIES    (100)

#define NV0000_CTRL_DIAG_DUMP_RPC_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV0000_CTRL_DIAG_DUMP_RPC_PARAMS {
    NvU32 firstEntryOffset;
    NvU32 outputEntryCount;
    NvU32 remainingEntryCount;
    NV_DECLARE_ALIGNED(NvU64 elapsedTimeInNs, 8);
    NV_DECLARE_ALIGNED(RPC_METER_ENTRY rpcProfilerBuffer[NV0000_CTRL_DIAG_RPC_MAX_ENTRIES], 8);
} NV0000_CTRL_DIAG_DUMP_RPC_PARAMS;

/* _ctrl0000diag_h_ */
