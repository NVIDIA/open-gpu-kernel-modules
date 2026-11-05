/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RM_STATE_MONITOR_H
#define RM_STATE_MONITOR_H

#include "nvctassert.h"

// run every 100ms to prevent excessive CPU usage
// as RM dump task runs as a higher priority task
// than RM main task
#define RM_STATE_MONITOR_WAIT_PERIOD (100000000) // 100ms
 
typedef enum {
    RM_STATE_MONITOR_HEADER_REQUEST_EXIT,
    RM_STATE_MONITOR_HEADER_REQUEST_RM_STATE,
    RM_STATE_MONITOR_HEADER_RESPONSE
} RM_STATE_MONITOR_HEADER;
 
typedef struct {
    RM_STATE_MONITOR_HEADER header;
} RM_STATE_MONITOR_REQUEST_MESSAGE; 
 
typedef struct {
    NV_STATUS status;
} RM_STATE_MONITOR_RESPONSE_MESSAGE;
 
typedef union {
    RM_STATE_MONITOR_REQUEST_MESSAGE request;
    RM_STATE_MONITOR_RESPONSE_MESSAGE response;
} RM_STATE_MONITOR_MESSAGE;
 
#define RM_STATE_MONITOR_MAX_DPC_QUEUE_ITEMS 16
#define RM_STATE_MONITOR_MAX_WORK_ITEMS 32
#define RM_STATE_MONITOR_MAX_INTERRUPTS 32
#define RM_STATE_MONITOR_MAX_ENINGE_BYTE_ARRAY (4)
#define RM_STATE_MONITOR_BUFFER_SIZE 4096U
#define RM_STATE_MONITOR_NVLINK_INVALID_VALUE NV_U32_MAX
#define RM_STATE_MONITOR_NVLINK_MAX_SRAM_ACCESS_REGS 320

#define RM_STATE_MONITOR_NVLINK_RPC_OP_NONE              0U
#define RM_STATE_MONITOR_NVLINK_RPC_OP_MSETP_POLL        1U
#define RM_STATE_MONITOR_NVLINK_RPC_OP_MNOC_SEND         2U
#define RM_STATE_MONITOR_NVLINK_RPC_OP_MNOC_RECV         3U
#define RM_STATE_MONITOR_NVLINK_RPC_OP_MINION_COMMAND    4U
#define RM_STATE_MONITOR_NVLINK_RPC_OP_MSE_DEGRADATION   6U

#define KNVLINK_TIMEOUT_KIND_NONE          0U
#define KNVLINK_TIMEOUT_KIND_MSETP         1U
#define KNVLINK_TIMEOUT_KIND_MNOC          2U
#define KNVLINK_TIMEOUT_KIND_MINION_CMD    3U
#define KNVLINK_TIMEOUT_KIND_MSE_FATAL     4U

typedef struct {
    NvU32 inFlightRpcCmd;
    NvU64 inFlightRpcLinkMask;
    NvU64 rpcStartTimestampNs;
    NvU32 totalMSETPTimeouts;
    NvU32 totalMNOCTimeouts;
    NvU32 totalMinionCmdTimeouts;
    NvU32 totalMSEFatalEvents;
    NvU32 lastTimeoutKind;
    NvU32 lastTimeoutLinkId;
    NvU64 lastTimeoutTimestampNs;
} KNVLINK_RPC_STATS;

typedef struct {
    KNVLINK_RPC_STATS rpcStats;
    NvU64 enabledLinkMask;
    NvU32 sramAccessTotalCount;
    NvU32 sramAccessCount;
    NvU32 sramAccess[RM_STATE_MONITOR_NVLINK_MAX_SRAM_ACCESS_REGS];
} KNVLINK_DUMP;

typedef struct {
    NvU64 dpcQueue[RM_STATE_MONITOR_MAX_DPC_QUEUE_ITEMS][RM_STATE_MONITOR_MAX_ENINGE_BYTE_ARRAY];
    NvU64 workItemsQueue[RM_STATE_MONITOR_MAX_WORK_ITEMS];
    NvU64 lastDpcExecutionTime; 
    NvU64 totalDpcExecutionTime;
    NvU64 lastWorkItemsExecutionTime;
    NvU64 totalWorkItemsExecutionTime;
    NvU64 interrupts[RM_STATE_MONITOR_MAX_INTERRUPTS]; //TODO: should this get set? will affect perf
    NvBool bNvlinkDumpPresent;
    KNVLINK_DUMP nvlinkDump;
} RM_STATE_MONITOR_DATA;
 
typedef struct {
    NvU64 requestSequenceNumber;
    NvU64 responseSequenceNumber; 
    RM_STATE_MONITOR_DATA data;
} RM_STATE_MONITOR_BUFFER_LAYOUT;

ct_assert(sizeof(RM_STATE_MONITOR_BUFFER_LAYOUT) <= RM_STATE_MONITOR_BUFFER_SIZE);

#ifndef RMCFG_IS_PLATFORM
#define RM_STATE_MONITOR_NVLINK_RPC_HOOKS_EXTERN 1
#else
#define RM_STATE_MONITOR_NVLINK_RPC_HOOKS_EXTERN 0
#endif

#if RM_STATE_MONITOR_NVLINK_RPC_HOOKS_EXTERN

void rmStateMonitorNvlinkRpcStart(NvU32 rmctrlCmd, NvU64 linkMask);

void rmStateMonitorNvlinkRpcComplete(void);

void rmStateMonitorNvlinkRpcOpStart(NvU32 linkId);

void rmStateMonitorNvlinkRpcOpEnd(NvU32 op, NvU32 linkId, NV_STATUS status);

#else

static inline void
rmStateMonitorNvlinkRpcStart
(NvU32 rmctrlCmd, NvU64 linkMask) {}

static inline void
rmStateMonitorNvlinkRpcComplete(void) {}

static inline void
rmStateMonitorNvlinkRpcOpStart(NvU32 linkId) {}

static inline void
rmStateMonitorNvlinkRpcOpEnd(NvU32 op, NvU32 linkId, NV_STATUS status) {}

#endif

#undef RM_STATE_MONITOR_NVLINK_RPC_HOOKS_EXTERN
 
#endif // RM_STATE_MONITOR_H
