/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef THREAD_STATE_H
#define THREAD_STATE_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Defines and structures used for Thread State management             *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "os/os.h"
#include "tls/tls.h"
#include "containers/map.h"
#include "containers/list.h"

typedef struct OBJGPU OBJGPU;

//
// Thread State Tracking structures and defines
//
typedef struct THREAD_TIMEOUT_STATE
{
    NvU64   enterTime;
    NvU64   nonComputeTime;
    NvU64   computeTime;
    NvU64   nextCpuYieldTime;
    NvU64   overrideTimeoutMsecs;

} THREAD_TIMEOUT_STATE, *PTHREAD_TIMEOUT_STATE;

typedef struct THREAD_STATE_FREE_CALLBACK
{
    void *pCbData;
    void (*pCb)(void *pCbData);
} THREAD_STATE_FREE_CALLBACK;

MAKE_LIST(THREAD_STATE_FREE_CB_LIST, THREAD_STATE_FREE_CALLBACK);

typedef struct THREAD_STATE_NODE THREAD_STATE_NODE;

struct THREAD_STATE_NODE
{
    OS_THREAD_HANDLE     threadId;
    /*!
     * Thread sequencer id.  This is a unique identifier for a given thread
     * entry into the RM.  This is separate from @ref threadId, as the threadId
     * is really the OS's thread handle/pointer.  In cases where the same
     * physical thread is re-used (e.g. WORK_ITEMs are scheduled from a
     * pre-allocated pool of worker threads), different RM threads will have the
     * same threadId.
     *
     * This value is set by @ref threadStateInitXYZ() based off the global @ref
     * THREAD_STATE_DB::threadSeqCntr.
     */
    NvU32                threadSeqId;
    NvBool               bValid;
    THREAD_TIMEOUT_STATE timeout;
    NvU32                cpuNum;
    NvU32                flags;
    MapNode              node;

    /*!
     * If a callback is installed, threadStateFree() may block on it.
     *
     * The installed callbacks will be processed in FIFO order only.
     *
     * Only supported on non-ISR CPU RM paths.
     */
    THREAD_STATE_FREE_CB_LIST cbList;
};

MAKE_INTRUSIVE_MAP(ThreadStateNodeMap, THREAD_STATE_NODE, node);

typedef struct THREAD_STATE_DB_TIMEOUT
{
    NvU64    nonComputeTimeoutMsecs;
    NvU64    computeTimeoutMsecs;
    NvU32    computeGpuMask;
    NvU32    flags;

} THREAD_STATE_DB_TIMEOUT, *PTHREAD_STATE_DB_TIMEOUT;

#define THREAD_STATE_TRACE_MAX_ENTRIES      8

typedef struct THREAD_STATE_TRACE_ENTRY
{
    NvU64 callerRA;
    NvU32 flags;

} THREAD_STATE_TRACE_ENTRY;

typedef struct THREAD_STATE_TRACE_INFO
{
    NvU32 index;
    THREAD_STATE_TRACE_ENTRY entries[THREAD_STATE_TRACE_MAX_ENTRIES];

} THREAD_STATE_TRACE_INFO;

typedef struct THREAD_STATE_ISR_LOCKLESS
{
    THREAD_STATE_NODE **ppIsrThreadStateGpu;
} THREAD_STATE_ISR_LOCKLESS, *PTHREAD_STATE_ISR_LOCKLESS, **PPTHREAD_STATE_ISR_LOCKLESS;

typedef struct THREAD_STATE_DB
{
    NvU32   setupFlags;
    NvU32   maxCPUs;
    /*!
     * Thread state sequencer id counter.  The last allocated thread state
     * sequencer id via @ref threadStateInitXYZ().
     */
    NvU32   threadSeqCntr;
    /*!
     * Thread state sequencer id counter for only GSP task_interrupt.
     */
    NvU32   gspIsrThreadSeqCntr;
    PORT_SPINLOCK *spinlock;
    ThreadStateNodeMap  dbRoot;
    THREAD_STATE_NODE **ppISRDeferredIntHandlerThreadNode;
    PTHREAD_STATE_ISR_LOCKLESS pIsrlocklessThreadNode;
    THREAD_STATE_DB_TIMEOUT timeout;
    THREAD_STATE_TRACE_INFO traceInfo;
} THREAD_STATE_DB, *PTHREAD_STATE_DB;

//
// This is the same for all OSes. This value was chosen because it is
// the minimum found on any OS at the time of this writing (May, 2008).
//
#define TIMEOUT_DEFAULT_OS_RESCHEDULE_INTERVAL_SECS     2

//
// The normal power transition requirement for Windows is 4 seconds.
// Use longer time to let OS fire timeout and ask recovery.
//
#define TIMEOUT_WDDM_POWER_TRANSITION_INTERVAL_MS       9800

//
// Thread State flags used for threadStateInitSetupFlags
//
#define THREAD_STATE_SETUP_FLAGS_NONE                               0
#define THREAD_STATE_SETUP_FLAGS_ENABLED                            NVBIT(0)
#define THREAD_STATE_SETUP_FLAGS_TIMEOUT_ENABLED                    NVBIT(1)
#define THREAD_STATE_SETUP_FLAGS_SLI_LOGIC_ENABLED                  NVBIT(2)
#define THREAD_STATE_SETUP_FLAGS_CHECK_TIMEOUT_AT_FREE_ENABLED      NVBIT(3)
#define THREAD_STATE_SETUP_FLAGS_ASSERT_ON_TIMEOUT_ENABLED          NVBIT(4)
#define THREAD_STATE_SETUP_FLAGS_ASSERT_ON_FAILED_LOOKUP_ENABLED    NVBIT(5)
#define THREAD_STATE_SETUP_FLAGS_RESET_ON_TIMEOUT_ENABLED           NVBIT(6)
#define THREAD_STATE_SETUP_FLAGS_DO_NOT_INCLUDE_SLEEP_TIME_ENABLED  NVBIT(7)
#define THREAD_STATE_SETUP_FLAGS_PRINT_INFO_ENABLED                 NVBIT(31)

//
// Thread State flags used for threadState[Init,Free]
//
#define THREAD_STATE_FLAGS_NONE                             0
#define THREAD_STATE_FLAGS_IS_ISR                       NVBIT(0)
#define THREAD_STATE_FLAGS_IS_ISR_DEFERRED_INT_HANDLER  NVBIT(1)
#define THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER      NVBIT(2)
#define THREAD_STATE_FLAGS_IS_ISR_LOCKLESS              NVBIT(3)
#define THREAD_STATE_FLAGS_TIMEOUT_INITED               NVBIT(5)
#define THREAD_STATE_FLAGS_DEVICE_INIT                  NVBIT(7)
#define THREAD_STATE_FLAGS_STATE_FREE_CB_ENABLED        NVBIT(8)

// These Threads run exclusively between a conditional acquire
#define THREAD_STATE_FLAGS_EXCLUSIVE_RUNNING   (THREAD_STATE_FLAGS_IS_ISR                       | \
                                                THREAD_STATE_FLAGS_IS_ISR_DEFERRED_INT_HANDLER  | \
                                                THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER)

#define THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING (THREAD_STATE_FLAGS_IS_ISR_DEFERRED_INT_HANDLER | \
                                       THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER)

NV_STATUS   threadStateGlobalAlloc(void);
void        threadStateGlobalFree(void);
void        threadStateInitRegistryOverrides(OBJGPU *pGpu);
void        threadStateInitSetupFlags(NvU32 flags);
NvU32       threadStateGetSetupFlags(void);

void        threadStateInitISRLockless(THREAD_STATE_NODE *, OBJGPU*, NvU32);
void        threadStateFreeISRLockless(THREAD_STATE_NODE *, OBJGPU*, NvU32);
void        threadStateInitISRAndDeferredIntHandler(THREAD_STATE_NODE *, OBJGPU*, NvU32);
void        threadStateFreeISRAndDeferredIntHandler(THREAD_STATE_NODE *, OBJGPU*, NvU32);
void        threadStateInit(THREAD_STATE_NODE *pThreadNode, NvU32 flags);
void        threadStateFree(THREAD_STATE_NODE *pThreadNode, NvU32 flags);

NV_STATUS   threadStateGetCurrent(THREAD_STATE_NODE **ppThreadNode, OBJGPU *pGpu);
NV_STATUS   threadStateGetCurrentUnchecked(THREAD_STATE_NODE **ppThreadNode, OBJGPU *pGpu);
NV_STATUS   threadStateInitTimeout(OBJGPU *pGpu, NvU32 timeoutUs, NvU32 flags);
NV_STATUS   threadStateCheckTimeout(OBJGPU *pGpu, NvU64 *pElapsedTimeUs);
NV_STATUS   threadStateResetTimeout(OBJGPU *pGpu);
void        threadStateLogTimeout(OBJGPU *pGpu, NvU64 funcAddr, NvU32 lineNum);
void        threadStateYieldCpuIfNecessary(OBJGPU *pGpu, NvBool bQuiet);
void        threadStateSetTimeoutOverride(THREAD_STATE_NODE *, NvU64);

NV_STATUS   threadStateEnqueueCallbackOnFree(THREAD_STATE_NODE *pThreadNode,
                                             THREAD_STATE_FREE_CALLBACK *pCallback);
void        threadStateRemoveCallbackOnFree(THREAD_STATE_NODE *pThreadNode,
                                            THREAD_STATE_FREE_CALLBACK *pCallback);
#endif // THREAD_STATE_H
