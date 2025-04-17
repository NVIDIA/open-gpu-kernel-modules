/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//*****************************************************************************
//
//      This file contains code used for Thread State management
//
//  Terminology:
//
//      ISR: First level interrupt handler, acknowledge function (VMK)
//
//      Deferred INT handler: DPC (Windows), Bottom-half (*nux), Interrupt handler (VMK)
//
//*****************************************************************************

#include "core/core.h"
#include "core/thread_state.h"
#include "core/locks.h"
#include "os/os.h"
#include "containers/map.h"
#include "nvrm_registry.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/gpu_timeout.h"

#include "virtualization/hypervisor/hypervisor.h"
#include "diagnostics/journal.h"

THREAD_STATE_DB threadStateDatabase;

static void _threadStatePrintInfo(THREAD_STATE_NODE *pThreadNode)
{
    if ((threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_PRINT_INFO_ENABLED) == 0)
        return;

    if (pThreadNode != NULL)
    {
        NV_PRINTF(LEVEL_NOTICE, "Thread state:\n");
        NV_PRINTF(LEVEL_NOTICE,
                "threadId: 0x%llx flags: 0x0%x\n",
                pThreadNode->threadId,
                pThreadNode->flags);

        NV_PRINTF(LEVEL_NOTICE,
                "enterTime: 0x%llx Limits: nonComputeTime: 0x%llx computeTime: 0x%llx\n",
                pThreadNode->timeout.enterTime,
                pThreadNode->timeout.nonComputeTime,
                pThreadNode->timeout.computeTime);
    }
}

static void _threadStateFreeProcessWork(THREAD_STATE_NODE *pThreadNode)
{
    PORT_UNREFERENCED_VARIABLE(pThreadNode);
}

/**
 * @brief allocate threadState which is per-cpu and per-GPU, only supporting lockless ISR
 *
 * @param[in/out] ppIsrlocklessThreadNode
 *
 * @return NV_OK if success, error otherwise
 *
 */
static NV_STATUS _threadStateAllocPerCpuPerGpu(PPTHREAD_STATE_ISR_LOCKLESS ppIsrlocklessThreadNode)
{
    NvU32 allocSize;
    PTHREAD_STATE_ISR_LOCKLESS pIsrlocklessThreadNode;
    NvS32 i;
    NvU32 coreCount = osGetMaximumCoreCount();

    // Bug 789767
    threadStateDatabase.maxCPUs = 32;
    if (coreCount > threadStateDatabase.maxCPUs)
        threadStateDatabase.maxCPUs = coreCount;

    allocSize = threadStateDatabase.maxCPUs * sizeof(PTHREAD_STATE_ISR_LOCKLESS);

    pIsrlocklessThreadNode = portMemAllocNonPaged(allocSize);
    if (pIsrlocklessThreadNode == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pIsrlocklessThreadNode, 0, allocSize);
    allocSize = NV_MAX_DEVICES * sizeof(THREAD_STATE_NODE *);

    // Allocate thread node for each gpu per cpu.
    for (i = 0; i < (NvS32)threadStateDatabase.maxCPUs; i++)
    {
        pIsrlocklessThreadNode[i].ppIsrThreadStateGpu = portMemAllocNonPaged(allocSize);
        if (pIsrlocklessThreadNode[i].ppIsrThreadStateGpu == NULL)
        {
            for (--i; i >= 0; --i)
                portMemFree(pIsrlocklessThreadNode[i].ppIsrThreadStateGpu);

            portMemFree(pIsrlocklessThreadNode);
            return NV_ERR_NO_MEMORY;
        }
        else
        {
            portMemSet(pIsrlocklessThreadNode[i].ppIsrThreadStateGpu, 0, allocSize);
        }
    }
    *ppIsrlocklessThreadNode = pIsrlocklessThreadNode;
    return NV_OK;
}

/**
 * @brief free threadState which is per-cpu and per-GPU, only working for lockless ISR
 *
 * @param[in/out] pIsrlocklessThreadNode
 *
 */
static void _threadStateFreePerCpuPerGpu(PTHREAD_STATE_ISR_LOCKLESS pIsrlocklessThreadNode)
{
    NvU32 i;
    // Free any memory we allocated
    if (pIsrlocklessThreadNode)
    {
        for (i = 0; i < threadStateDatabase.maxCPUs; i++)
            portMemFree(pIsrlocklessThreadNode[i].ppIsrThreadStateGpu);
        portMemFree(pIsrlocklessThreadNode);
    }
}

/**
 * @brief the main function to allocate the threadState
 *
 * @return NV_OK if the entire global threadState is created successfully,
 *         and an appropriate ERROR otherwise.
 *
 */
NV_STATUS threadStateGlobalAlloc(void)
{
    NV_STATUS rmStatus;
    NvU32 allocSize;

    NV_ASSERT(tlsInitialize() == NV_OK);

    // Init the thread sequencer id counter to 0.
    threadStateDatabase.threadSeqCntr = 0;
    threadStateDatabase.gspIsrThreadSeqCntr = 0;

    threadStateDatabase.spinlock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (threadStateDatabase.spinlock == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    allocSize = NV_MAX_DEVICES * sizeof(THREAD_STATE_NODE *);
    threadStateDatabase.ppISRDeferredIntHandlerThreadNode = portMemAllocNonPaged(allocSize);
    if (threadStateDatabase.ppISRDeferredIntHandlerThreadNode == NULL)
    {
        portSyncSpinlockDestroy(threadStateDatabase.spinlock);
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(threadStateDatabase.ppISRDeferredIntHandlerThreadNode, 0, allocSize);

    rmStatus = _threadStateAllocPerCpuPerGpu(&threadStateDatabase.pIsrlocklessThreadNode);
    if (rmStatus != NV_OK)
    {
        portMemFree(threadStateDatabase.ppISRDeferredIntHandlerThreadNode);
        portSyncSpinlockDestroy(threadStateDatabase.spinlock);
        return rmStatus;
    }

    mapInitIntrusive(&threadStateDatabase.dbRoot);

    return rmStatus;
}

void threadStateGlobalFree(void)
{
    // Disable all threadState usage once the spinlock is freed
    threadStateDatabase.setupFlags = THREAD_STATE_SETUP_FLAGS_NONE;

    // Free any memory we allocated
    _threadStateFreePerCpuPerGpu(threadStateDatabase.pIsrlocklessThreadNode);
    threadStateDatabase.pIsrlocklessThreadNode = NULL;

    portMemFree(threadStateDatabase.ppISRDeferredIntHandlerThreadNode);
    threadStateDatabase.ppISRDeferredIntHandlerThreadNode = NULL;

    if (threadStateDatabase.spinlock != NULL)
    {
        portSyncSpinlockDestroy(threadStateDatabase.spinlock);
        threadStateDatabase.spinlock = NULL;
    }

    mapDestroy(&threadStateDatabase.dbRoot);

    tlsShutdown();
}

void threadStateInitRegistryOverrides(OBJGPU *pGpu)
{
    NvU32 flags;

    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_THREAD_STATE_SETUP_FLAGS, &flags) == NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Overriding threadStateDatabase.setupFlags from 0x%x to 0x%x\n",
                  threadStateDatabase.setupFlags, flags);
        threadStateDatabase.setupFlags = flags;
    }
}

void threadStateInitSetupFlags(NvU32 flags)
{
    threadStateDatabase.timeout.nonComputeTimeoutMsecs = 0;
    threadStateDatabase.timeout.computeTimeoutMsecs = 0;
    threadStateDatabase.timeout.computeGpuMask = 0;
    threadStateDatabase.setupFlags = flags;
}

NvU32 threadStateGetSetupFlags(void)
{
    return threadStateDatabase.setupFlags;
}

//
// Sets the nextCpuYieldTime field to a value that corresponds to a
// short time in the future. This value represents the next time that
// the osScheduler may be invoked, during long waits.
//
static void _threadStateSetNextCpuYieldTime(THREAD_STATE_NODE *pThreadNode)
{
    NvU64 timeInNs;
    timeInNs = osGetCurrentTick();

    pThreadNode->timeout.nextCpuYieldTime = timeInNs +
        (TIMEOUT_DEFAULT_OS_RESCHEDULE_INTERVAL_SECS) * 1000000 * 1000;
}

void threadStateYieldCpuIfNecessary(OBJGPU *pGpu, NvBool bQuiet)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE *pThreadNode = NULL;
    NvU64 timeInNs;

    rmStatus = threadStateGetCurrent(&pThreadNode, pGpu);
    if ((rmStatus == NV_OK) && pThreadNode )
    {
        timeInNs = osGetCurrentTick();
        if (timeInNs >= pThreadNode->timeout.nextCpuYieldTime)
        {
            if (NV_OK == osSchedule())
            {
                NV_PRINTF_COND(bQuiet, LEVEL_INFO, LEVEL_WARNING, "Yielding\n");
            }

            _threadStateSetNextCpuYieldTime(pThreadNode);
        }
    }
}

static NV_STATUS _threadNodeInitTime(THREAD_STATE_NODE *pThreadNode)
{
    NV_STATUS rmStatus = NV_OK;
    NvU64 timeInNs;
    NvBool firstInit;
    NvU64 computeTimeoutMsecs;
    NvU64 nonComputeTimeoutMsecs;
    NvBool bIsDpcOrIsr = !!(pThreadNode->flags &
                            (THREAD_STATE_FLAGS_IS_ISR |
                             THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING |
                             THREAD_STATE_FLAGS_IS_ISR_LOCKLESS));

    //
    // _threadNodeInitTime() is used both for the first init and
    // threadStateResetTimeout(). We can tell the two apart by checking whether
    // enterTime has been initialized already.
    //
    firstInit = (pThreadNode->timeout.enterTime == 0);

    computeTimeoutMsecs = threadStateDatabase.timeout.computeTimeoutMsecs;
    nonComputeTimeoutMsecs = threadStateDatabase.timeout.nonComputeTimeoutMsecs;

    //
    // If we are in DPC or ISR contexts, we need to timeout the driver before OS
    // mechanisms kick in and panic the kernel
    //
    if (bIsDpcOrIsr)
    {
        //
        // Note that MODS does not have interrupt timeout requirements and there are
        // existing code paths that violates the timeout
        //
        computeTimeoutMsecs = TIMEOUT_DPC_ISR_INTERVAL_MS;
        nonComputeTimeoutMsecs = TIMEOUT_DPC_ISR_INTERVAL_MS;
    }

    timeInNs = osGetCurrentTick();

    if (firstInit)
    {
        //
        // Save off the time we first entered the RM.  We do not
        // want to reset this if we call threadStateResetTimeout()
        //
        pThreadNode->timeout.enterTime = timeInNs;
    }

    if (pThreadNode->timeout.overrideTimeoutMsecs)
    {
        nonComputeTimeoutMsecs = pThreadNode->timeout.overrideTimeoutMsecs;
        computeTimeoutMsecs = pThreadNode->timeout.overrideTimeoutMsecs;
    }

    if ((pThreadNode->flags & THREAD_STATE_FLAGS_DEVICE_INIT) != 0)
    {
        //
        // Even on platforms with strict timing requirements (e.g. WDDM) there
        // is an exception for initialization. While init time is an important
        // performance metric, we do not want to functionally fail because of
        // an arbitrary deadline. Thus, we set the timeout to give plenty of
        // buffer room for some of the slower platforms:
        // - P40 can take ~30 seconds when booting in passthrough due to
        //   Hyper-V intercepting all MMIO accesses (bug 1900927)
        // - Hopper+ can take 3+ seconds due to memory link initialization
        //
        const NvU32 DEVICE_INIT_TIMEOUT_MS = 60 * 1000;

        computeTimeoutMsecs = NV_MAX(computeTimeoutMsecs, DEVICE_INIT_TIMEOUT_MS);
        nonComputeTimeoutMsecs = NV_MAX(nonComputeTimeoutMsecs, DEVICE_INIT_TIMEOUT_MS);
    }

    _threadStateSetNextCpuYieldTime(pThreadNode);

    if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSTIMER)
    {
        pThreadNode->timeout.nonComputeTime = timeInNs + (nonComputeTimeoutMsecs * 1000 * 1000);
        pThreadNode->timeout.computeTime = timeInNs + (computeTimeoutMsecs * 1000 * 1000);
    }
    else if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSDELAY)
    {
        // Convert from msecs (1,000) to usecs (1,000,000)
        pThreadNode->timeout.nonComputeTime = nonComputeTimeoutMsecs * 1000;
        pThreadNode->timeout.computeTime = computeTimeoutMsecs * 1000;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Bad threadStateDatabase.timeout.flags: 0x%x!\n",
                  threadStateDatabase.timeout.flags);

        rmStatus = NV_ERR_INVALID_STATE;
    }

    return rmStatus;
}

static void _getTimeoutDataFromGpuMode(
    OBJGPU *pGpu,
    THREAD_STATE_NODE *pThreadNode,
    NvU64 **ppThreadNodeTime,
    NvU64 *pThreadStateDatabaseTimeoutMsecs)
{
    if (pGpu)
    {
        if (threadStateDatabase.timeout.computeGpuMask & NVBIT(pGpu->gpuInstance))
        {
            *ppThreadNodeTime = &pThreadNode->timeout.computeTime;
        }
        else
        {
            *ppThreadNodeTime = &pThreadNode->timeout.nonComputeTime;
        }

        *pThreadStateDatabaseTimeoutMsecs =
            NV_MAX(threadStateDatabase.timeout.computeTimeoutMsecs, threadStateDatabase.timeout.nonComputeTimeoutMsecs);
    }
}

//
// The logic in _threadNodeCheckTimeout() should closely resemble
// that of _gpuCheckTimeout().
//
static NV_STATUS _threadNodeCheckTimeout(OBJGPU *pGpu, THREAD_STATE_NODE *pThreadNode, NvU64 *pElapsedTimeUs)
{
    NV_STATUS rmStatus = NV_OK;
    NvU64 threadStateDatabaseTimeoutMsecs = 0;
    NvU64 *pThreadNodeTime = NULL;
    NvU64 timeInNs;

    if (pGpu)
    {
        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        {
            NV_PRINTF(LEVEL_ERROR, "API_GPU_ATTACHED_SANITY_CHECK failed!\n");
            return NV_ERR_TIMEOUT;
        }
    }

    _getTimeoutDataFromGpuMode(pGpu, pThreadNode, &pThreadNodeTime,
                               &threadStateDatabaseTimeoutMsecs);
    if ((threadStateDatabaseTimeoutMsecs == 0) ||
         (pThreadNodeTime == NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "threadStateDatabaseTimeoutMsecs or pThreadNodeTime was NULL!\n");
        return NV_ERR_INVALID_STATE;
    }

    timeInNs = osGetCurrentTick();
    if (pElapsedTimeUs)
    {
        *pElapsedTimeUs = (timeInNs - pThreadNode->timeout.enterTime) / 1000;
    }

    if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSTIMER)
    {
        if (timeInNs >= *pThreadNodeTime)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "_threadNodeCheckTimeout: currentTime: %llx >= %llx\n",
                      timeInNs, *pThreadNodeTime);

            rmStatus = NV_ERR_TIMEOUT;
        }
    }
    else if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSDELAY)
    {
        osDelayUs(100);
        *pThreadNodeTime -= NV_MIN(100, *pThreadNodeTime);
        if (*pThreadNodeTime == 0)
        {
            rmStatus = NV_ERR_TIMEOUT;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "_threadNodeCheckTimeout: Unsupported timeout.flags: 0x%x!\n",
                  threadStateDatabase.timeout.flags);

        rmStatus = NV_ERR_INVALID_STATE;
    }

    if (rmStatus == NV_ERR_TIMEOUT)
    {
        // Report the time this Thread entered the RM
        _threadStatePrintInfo(pThreadNode);

        // This is set via osGetTimeoutParams per platform
        NV_PRINTF(LEVEL_ERROR,
                  "_threadNodeCheckTimeout: Timeout was set to: %lld msecs!\n",
                  threadStateDatabaseTimeoutMsecs);

        if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ASSERT_ON_TIMEOUT_ENABLED)
        {
            NV_ASSERT(0);
        }

        if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_RESET_ON_TIMEOUT_ENABLED)
        {
            threadStateResetTimeout(pGpu);
        }
    }

    return rmStatus;
}

static void _threadStateFreeInvokeCallbacks
(
    THREAD_STATE_NODE *pThreadNode
)
{
    THREAD_STATE_FREE_CALLBACK *pCbListNode;

    NV_ASSERT_OR_RETURN_VOID(pThreadNode->flags &
                             THREAD_STATE_FLAGS_STATE_FREE_CB_ENABLED);

    // Start from head to maintain FIFO semantics.
    while ((pCbListNode = listHead(&pThreadNode->cbList)) != NULL)
    {
        (*pCbListNode->pCb)(pCbListNode->pCbData);
        listRemove(&pThreadNode->cbList, pCbListNode);
    }
}

static void _threadStateLogInitCaller(THREAD_STATE_NODE *pThreadNode, NvU64 funcAddr)
{
    threadStateDatabase.traceInfo.entries[threadStateDatabase.traceInfo.index].callerRA = funcAddr;
    threadStateDatabase.traceInfo.entries[threadStateDatabase.traceInfo.index].flags = pThreadNode->flags;
    threadStateDatabase.traceInfo.index =
        (threadStateDatabase.traceInfo.index + 1) % THREAD_STATE_TRACE_MAX_ENTRIES;
}

/**
 * @brief Initialize a threadState for regular threads (non-interrupt context)
 *
 * @param[in/out] pThreadNode
 * @param[in] flags
 *
 */
void threadStateInit(THREAD_STATE_NODE *pThreadNode, NvU32 flags)
{
    NV_STATUS rmStatus;
    NvU64 funcAddr;

    // Isrs should be using threadStateIsrInit().
    NV_ASSERT((flags & (THREAD_STATE_FLAGS_IS_ISR_LOCKLESS |
                        THREAD_STATE_FLAGS_IS_ISR |
                        THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING)) == 0);

    // Check to see if ThreadState is enabled
    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    portMemSet(pThreadNode, 0, sizeof(*pThreadNode));
    pThreadNode->threadSeqId = portAtomicIncrementU32(&threadStateDatabase.threadSeqCntr);
    pThreadNode->cpuNum = osGetCurrentProcessorNumber();
    pThreadNode->flags = flags;

    //
    // The thread state free callbacks are only supported in the non-ISR paths
    // as they invoke memory allocation routines.
    //
    listInit(&pThreadNode->cbList, portMemAllocatorGetGlobalNonPaged());
    pThreadNode->flags |= THREAD_STATE_FLAGS_STATE_FREE_CB_ENABLED;

    rmStatus = _threadNodeInitTime(pThreadNode);
    if (rmStatus == NV_OK)
        pThreadNode->flags |= THREAD_STATE_FLAGS_TIMEOUT_INITED;

    rmStatus = osGetCurrentThread(&pThreadNode->threadId);
    if (rmStatus != NV_OK)
        return;

    NV_ASSERT_OR_RETURN_VOID(pThreadNode->cpuNum < threadStateDatabase.maxCPUs);

    funcAddr = (NvU64) (NV_RETURN_ADDRESS());

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    if (!mapInsertExisting(&threadStateDatabase.dbRoot, (NvU64)pThreadNode->threadId, pThreadNode))
    {
        // Reset the threadId as insertion failed. bValid is already NV_FALSE
        pThreadNode->threadId = 0;
        portSyncSpinlockRelease(threadStateDatabase.spinlock);
        return;
    }
    else
    {
        pThreadNode->bValid = NV_TRUE;
        rmStatus = NV_OK;
    }

    _threadStateLogInitCaller(pThreadNode, funcAddr);

    portSyncSpinlockRelease(threadStateDatabase.spinlock);

    _threadStatePrintInfo(pThreadNode);

    NV_ASSERT(rmStatus == NV_OK);
    threadPriorityStateAlloc();

    if (TLS_MIRROR_THREADSTATE)
    {
        THREAD_STATE_NODE **pTls = (THREAD_STATE_NODE **)tlsEntryAcquire(TLS_ENTRY_ID_THREADSTATE);
        NV_ASSERT_OR_RETURN_VOID(pTls != NULL);
        if (*pTls != NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: Nested threadState inits detected. Previous threadState node is %p, new is %p\n",
                      *pTls, pThreadNode);
        }
        *pTls = pThreadNode;
    }
}

/**
 * @brief Initialize a threadState for locked ISR and Bottom-half
 *
 * @param[in/out] pThreadNode
 * @param[in] pGpu
 * @param[in] flags THREAD_STATE_FLAGS_IS_ISR or THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING
 *
 */
void threadStateInitISRAndDeferredIntHandler
(
    THREAD_STATE_NODE *pThreadNode,
    OBJGPU *pGpu,
    NvU32 flags
)
{
    NV_STATUS rmStatus;

    NV_ASSERT(pGpu);

    // should be using threadStateIsrInit().
    NV_ASSERT(flags & (THREAD_STATE_FLAGS_IS_ISR | THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING));

    portMemSet(pThreadNode, 0, sizeof(*pThreadNode));
    pThreadNode->threadSeqId = portAtomicIncrementU32(&threadStateDatabase.threadSeqCntr);
    pThreadNode->cpuNum = osGetCurrentProcessorNumber();
    pThreadNode->flags = flags;

    rmStatus = _threadNodeInitTime(pThreadNode);

    if (rmStatus == NV_OK)
        pThreadNode->flags |= THREAD_STATE_FLAGS_TIMEOUT_INITED;

    if (TLS_MIRROR_THREADSTATE)
    {
        THREAD_STATE_NODE **pTls = (THREAD_STATE_NODE **)tlsEntryAcquire(TLS_ENTRY_ID_THREADSTATE);
        NV_ASSERT_OR_GOTO(pTls != NULL, TlsMirror_Exit);
        if (*pTls != NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: Nested threadState inits detected. Previous threadState node is %p, new is %p\n",
                      *pTls, pThreadNode);
        }
        *pTls = pThreadNode;
    }
TlsMirror_Exit:

    rmStatus = osGetCurrentThread(&pThreadNode->threadId);
    if (rmStatus != NV_OK)
        return;

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    threadStateDatabase.ppISRDeferredIntHandlerThreadNode[pGpu->gpuInstance] = pThreadNode;
    portSyncSpinlockRelease(threadStateDatabase.spinlock);
}

/**
 * @brief Initialize a threadState for lockless ISR
 *
 * @param[in/out] pThreadNode
 * @param[in] pGpu
 * @param[in] flags THREAD_STATE_FLAGS_IS_ISR_LOCKLESS
 *
 */
void threadStateInitISRLockless(THREAD_STATE_NODE *pThreadNode, OBJGPU *pGpu, NvU32 flags)
{
    NV_STATUS rmStatus;
    PTHREAD_STATE_ISR_LOCKLESS pThreadStateIsrLockless;

    NV_ASSERT(flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS);

    // Check to see if ThreadState is enabled
    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    portMemSet(pThreadNode, 0, sizeof(*pThreadNode));
    pThreadNode->threadSeqId = portAtomicIncrementU32(&threadStateDatabase.threadSeqCntr);
    pThreadNode->cpuNum = osGetCurrentProcessorNumber();
    pThreadNode->flags = flags;

    rmStatus = _threadNodeInitTime(pThreadNode);
    if (rmStatus == NV_OK)
        pThreadNode->flags |= THREAD_STATE_FLAGS_TIMEOUT_INITED;

    if (TLS_MIRROR_THREADSTATE)
    {
        THREAD_STATE_NODE **pTls = (THREAD_STATE_NODE **)tlsEntryAcquire(TLS_ENTRY_ID_THREADSTATE);
        NV_ASSERT_OR_GOTO(pTls != NULL, TlsMirror_Exit);
        if (*pTls != NULL)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: Nested threadState inits detected. Previous threadState node is %p, new is %p\n",
                      *pTls, pThreadNode);
        }
        *pTls = pThreadNode;
    }
TlsMirror_Exit:

    rmStatus = osGetCurrentThread(&pThreadNode->threadId);
    if (rmStatus != NV_OK)
        return;

    NV_ASSERT_OR_RETURN_VOID(pThreadNode->cpuNum < threadStateDatabase.maxCPUs);

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);

    //
    // We use a cpu/gpu indexed structure to store the threadNode pointer
    // instead of a tree indexed by threadId because threadId is no longer
    // unique in an isr. We also need to index by both cpu num and gpu instance
    // because isrs can prempt one another, and run on the same processor
    // at the same time.
    //
    pThreadStateIsrLockless = &threadStateDatabase.pIsrlocklessThreadNode[pThreadNode->cpuNum];
    NV_ASSERT(pThreadStateIsrLockless->ppIsrThreadStateGpu[pGpu->gpuInstance] == NULL);
    pThreadStateIsrLockless->ppIsrThreadStateGpu[pGpu->gpuInstance] = pThreadNode;
    portSyncSpinlockRelease(threadStateDatabase.spinlock);
}

void threadStateOnlyProcessWorkISRAndDeferredIntHandler
(
    THREAD_STATE_NODE *pThreadNode,
    OBJGPU *pGpu,
    NvU32 flags
)
{
    NV_ASSERT_OR_RETURN_VOID(pGpu &&
        (flags & (THREAD_STATE_FLAGS_IS_ISR | THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING)));

    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    // Process any work needed before exiting.
    _threadStateFreeProcessWork(pThreadNode);
}

void threadStateOnlyFreeISRAndDeferredIntHandler
(
    THREAD_STATE_NODE *pThreadNode,
    OBJGPU *pGpu,
    NvU32 flags
)
{
    NV_STATUS rmStatus;

    NV_ASSERT_OR_RETURN_VOID(pGpu &&
        (flags & (THREAD_STATE_FLAGS_IS_ISR | THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING)));

    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_CHECK_TIMEOUT_AT_FREE_ENABLED)
    {
        rmStatus = _threadNodeCheckTimeout(NULL /*pGpu*/, pThreadNode, NULL /*pElapsedTimeUs*/);
        NV_ASSERT(rmStatus == NV_OK);
    }

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    threadStateDatabase.ppISRDeferredIntHandlerThreadNode[pGpu->gpuInstance] = NULL;
    portSyncSpinlockRelease(threadStateDatabase.spinlock);

    if (TLS_MIRROR_THREADSTATE)
    {
        NvU32 r;
        THREAD_STATE_NODE *pTlsNode = NvP64_VALUE(tlsEntryGet(TLS_ENTRY_ID_THREADSTATE));
        NV_ASSERT(pTlsNode);
        if (pTlsNode != pThreadNode)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: TLS / threadState mismatch: pTlsNode=%p, pThreadNode=%p\n",
                      pTlsNode, pThreadNode);
        }
        r = tlsEntryRelease(TLS_ENTRY_ID_THREADSTATE);
        if (r != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                     "TLS: tlsEntryRelease returned %d (this is likely due to nested threadStateInit() calls)\n",
                     r);
        }
    }
}

/**
 * @brief Free the thread state for locked ISR and bottom-half
 *
 * @param[in/out] pThreadNode
 * @param[in] pGpu
 * @param[in] flags THREAD_STATE_FLAGS_IS_ISR or THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING
 *
 */
void threadStateFreeISRAndDeferredIntHandler
(
    THREAD_STATE_NODE *pThreadNode,
    OBJGPU *pGpu,
    NvU32 flags
)
{
    threadStateOnlyProcessWorkISRAndDeferredIntHandler(pThreadNode, pGpu, flags);

    threadStateOnlyFreeISRAndDeferredIntHandler(pThreadNode, pGpu, flags);
}

/**
 * @brief Free the thread state for a regular thread
 *
 * @param[in/out] pThreadNode
 * @param[in] flags
 *
 */
void threadStateFree(THREAD_STATE_NODE *pThreadNode, NvU32 flags)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE *pNode;
    ThreadStateNodeMap *pMap;

    NV_ASSERT((flags & (THREAD_STATE_FLAGS_IS_ISR_LOCKLESS |
                        THREAD_STATE_FLAGS_IS_ISR          |
                        THREAD_STATE_FLAGS_DEFERRED_INT_HANDLER_RUNNING)) == 0);

    // Check to see if ThreadState is enabled
    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    if (!(flags & THREAD_STATE_FLAGS_EXCLUSIVE_RUNNING))
    {
        //
        // Do not do this for exclusive running threads as all the info
        // is not filled in.
        //
        if (!pThreadNode->bValid && pThreadNode->threadId == 0)
            return;
    }

    _threadStateFreeInvokeCallbacks(pThreadNode);

    listDestroy(&pThreadNode->cbList);

    // Process any work needed before exiting.
    _threadStateFreeProcessWork(pThreadNode);

    if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_CHECK_TIMEOUT_AT_FREE_ENABLED)
    {
        rmStatus = _threadNodeCheckTimeout(NULL /*pGpu*/, pThreadNode, NULL /*pElapsedTimeUs*/);
        NV_ASSERT(rmStatus == NV_OK);
    }

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    pMap = &threadStateDatabase.dbRoot;

    pNode = mapFind(pMap, (NvU64)pThreadNode->threadId);

    if (pNode != NULL)
    {
        mapRemove(pMap, pThreadNode);
        pThreadNode->bValid = NV_FALSE;
        rmStatus = NV_OK;
    }
    else
    {
        rmStatus = NV_ERR_OBJECT_NOT_FOUND;
    }

    portSyncSpinlockRelease(threadStateDatabase.spinlock);

    _threadStatePrintInfo(pThreadNode);

    NV_ASSERT(rmStatus == NV_OK);

    threadPriorityStateFree();

    if (TLS_MIRROR_THREADSTATE)
    {
        NvU32 r;
        THREAD_STATE_NODE *pTlsNode = NvP64_VALUE(tlsEntryGet(TLS_ENTRY_ID_THREADSTATE));
        NV_ASSERT(pTlsNode);
        if (pTlsNode != pThreadNode)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: TLS / threadState mismatch: pTlsNode=%p, pThreadNode=%p\n",
                      pTlsNode, pThreadNode);
        }
        r = tlsEntryRelease(TLS_ENTRY_ID_THREADSTATE);
        if (r != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                     "TLS: tlsEntryRelease returned %d (this is likely due to nested threadStateInit() calls)\n",
                     r);
        }
    }
}

/**
 * @brief Free thread state for lockless ISR
 *
 * @param[in/out] pThreadNode
 * @param[in] pGpu
 * @param[in] flags
 *
 */
void threadStateFreeISRLockless(THREAD_STATE_NODE *pThreadNode, OBJGPU *pGpu, NvU32 flags)
{
    NV_STATUS rmStatus = NV_OK;
    PTHREAD_STATE_ISR_LOCKLESS pThreadStateIsrlockless;

    NV_ASSERT(flags & (THREAD_STATE_FLAGS_IS_ISR_LOCKLESS | THREAD_STATE_FLAGS_IS_ISR));
    NV_ASSERT(pThreadNode->cpuNum == osGetCurrentProcessorNumber());

    // Check to see if ThreadState is enabled
    if (!(threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED))
        return;

    // Process any work needed before exiting.
    _threadStateFreeProcessWork(pThreadNode);

    if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_CHECK_TIMEOUT_AT_FREE_ENABLED)
    {
        rmStatus = _threadNodeCheckTimeout(NULL /*pGpu*/, pThreadNode, NULL /*pElapsedTimeUs*/);
        NV_ASSERT(rmStatus == NV_OK);
    }

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    pThreadStateIsrlockless = &threadStateDatabase.pIsrlocklessThreadNode[pThreadNode->cpuNum];
    NV_ASSERT(pThreadStateIsrlockless->ppIsrThreadStateGpu[pGpu->gpuInstance] != NULL);
    pThreadStateIsrlockless->ppIsrThreadStateGpu[pGpu->gpuInstance] = NULL;
    portSyncSpinlockRelease(threadStateDatabase.spinlock);

    if (TLS_MIRROR_THREADSTATE)
    {
        NvU32 r;
        THREAD_STATE_NODE *pTlsNode = NvP64_VALUE(tlsEntryGet(TLS_ENTRY_ID_THREADSTATE));
        NV_ASSERT(pTlsNode);
        if (pTlsNode != pThreadNode)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: TLS / threadState mismatch: pTlsNode=%p, pThreadNode=%p\n",
                      pTlsNode, pThreadNode);
        }
        r = tlsEntryRelease(TLS_ENTRY_ID_THREADSTATE);
        if (r != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                     "TLS: tlsEntryRelease returned %d (this is likely due to nested threadStateInit() calls)\n",
                     r);
        }
    }
}

/**
 * @brief Get the thread state with given <thread_id, pGpu>
 *
 * @param[in] threadId
 * @param[in] pGpu
 * @param[out] ppThreadNode
 *
 * @return NV_OK if we are able to locate the thread state with <thread_id, pGpu>,
 *         NV_ERR_OBJECT_NOT_FOUND if we can't find inside map
 *         NV_ERR_INVALID_STATE if the thread state is not enabled or the CPU has
 *                              been hotpluged.
 */
static NV_STATUS _threadStateGet
(
    OS_THREAD_HANDLE threadId,
    OBJGPU *pGpu,
    THREAD_STATE_NODE **ppThreadNode
)
{
    THREAD_STATE_NODE *pNode;

    // Check to see if ThreadState is enabled
    if ((threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED) == NV_FALSE)
    {
        *ppThreadNode = NULL;
        return NV_ERR_INVALID_STATE;
    }
    else
    {
        NvU32 cpuNum = osGetCurrentProcessorNumber();
        THREAD_STATE_NODE *pIsrlocklessThreadNode;
        THREAD_STATE_NODE *pISRDeferredIntHandlerNode;

        if (cpuNum >= threadStateDatabase.maxCPUs)
        {
            NV_ASSERT(0);
            *ppThreadNode = NULL;
            return NV_ERR_INVALID_STATE;
        }

        //
        // Several threadState call sites will not pass a pGpu b/c it is not
        // easily available, and they are not running in interrupt context.
        // _threadStateGet() only needs to pGpu for getting the thread node
        // when called for an isr, so that site has assumed it will never
        // be in interrupt context.
        //
        if (pGpu)
        {
            // Check to see if the this is an lockless ISR running thread.
            pIsrlocklessThreadNode = threadStateDatabase.pIsrlocklessThreadNode[cpuNum].ppIsrThreadStateGpu[pGpu->gpuInstance];
            if (pIsrlocklessThreadNode && (pIsrlocklessThreadNode->threadId == threadId))
            {
                *ppThreadNode = pIsrlocklessThreadNode;
                return NV_OK;
            }

            // Check to see if the this is an ISR or bottom-half thread
            pISRDeferredIntHandlerNode = threadStateDatabase.ppISRDeferredIntHandlerThreadNode[pGpu->gpuInstance];
            if  (pISRDeferredIntHandlerNode && (pISRDeferredIntHandlerNode->threadId == threadId))
            {
                *ppThreadNode = pISRDeferredIntHandlerNode;
                return NV_OK;
            }
        }
    }

    portSyncSpinlockAcquire(threadStateDatabase.spinlock);
    pNode = mapFind(&threadStateDatabase.dbRoot, (NvU64) threadId);
    portSyncSpinlockRelease(threadStateDatabase.spinlock);

    *ppThreadNode = pNode;
    if (pNode != NULL)
    {
        NV_ASSERT((*ppThreadNode)->threadId == threadId);
        return NV_OK;
    }
    else
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
}

NV_STATUS threadStateGetCurrentUnchecked(THREAD_STATE_NODE **ppThreadNode, OBJGPU *pGpu)
{
    NV_STATUS rmStatus;
    OS_THREAD_HANDLE threadId;

    // Check to see if ThreadState is enabled
    if ((threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ENABLED) == NV_FALSE)
    {
        *ppThreadNode = NULL;
        return NV_ERR_INVALID_STATE;
    }

    rmStatus = osGetCurrentThread(&threadId);
    if (rmStatus == NV_OK)
    {
        rmStatus = _threadStateGet(threadId, pGpu, ppThreadNode);
    }

    // Assert if the current lookup failed - Please add the stack from this assert to bug 690089.
    if (threadStateDatabase.setupFlags & THREAD_STATE_SETUP_FLAGS_ASSERT_ON_FAILED_LOOKUP_ENABLED)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "threadState[Init,Free] call may be missing from this RM entry point!\n");
        NV_ASSERT(rmStatus == NV_OK);
    }

    return rmStatus;
}

NV_STATUS threadStateGetCurrent(THREAD_STATE_NODE **ppThreadNode, OBJGPU *pGpu)
{
    NV_STATUS status = threadStateGetCurrentUnchecked(ppThreadNode, pGpu);

    if (TLS_MIRROR_THREADSTATE)
    {
        THREAD_STATE_NODE *pTlsNode = NvP64_VALUE(tlsEntryGet(TLS_ENTRY_ID_THREADSTATE));

        if ((status == NV_OK) && (pTlsNode != *ppThreadNode))
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: TLS / threadState mismatch: pTlsNode=%p, *ppThreadNode=%p; ThreadID = %llx (NvPort:%llx), sp=%p\n",
                      pTlsNode, *ppThreadNode,
                      (NvU64)(*ppThreadNode)->threadId,
                      portThreadGetCurrentThreadId(), &status);

        }
        else if ((status != NV_OK) && (pTlsNode != NULL))
        {
            NV_PRINTF(LEVEL_WARNING,
                      "TLS: TLS / threadState mismatch: ThreadNode not found (status=0x%08x), but found in TLS:%p (tid=%llx;sp=%p)\n",
                      status, pTlsNode,
                      portThreadGetCurrentThreadId(), &status);
        }
    }

    return status;
}

//
// Sets the timeout value and method of timeout
//
NV_STATUS threadStateInitTimeout(OBJGPU *pGpu, NvU32 timeoutUs, NvU32 flags)
{
    NvU32 timeoutMsecs = (timeoutUs / 1000);
    NvU32 gpuMode = gpuGetMode(pGpu);
    NvU32 scaleIgnored = 0;
    NvU32 flagsIgnored = 0;
    NvU32 perOSTimeoutUs = 999; // What we'll see if osGetTimeoutParams ever fails

    if (gpuMode == NV_GPU_MODE_GRAPHICS_MODE)
    {
        threadStateDatabase.timeout.nonComputeTimeoutMsecs = timeoutMsecs;
        threadStateDatabase.timeout.computeGpuMask &= ~NVBIT(pGpu->gpuInstance);
    }
    else
    {
        threadStateDatabase.timeout.computeGpuMask |= NVBIT(pGpu->gpuInstance);
    }
    //
    // Initializing the compute timeout limits in all cases, but use
    // per-OS values:
    //
    osGetTimeoutParams(pGpu, &perOSTimeoutUs, &scaleIgnored, &flagsIgnored);
    timeoutMsecs = (perOSTimeoutUs / 1000);
    timeoutMsecs = gpuScaleTimeout(pGpu, timeoutMsecs);

    threadStateDatabase.timeout.computeTimeoutMsecs = timeoutMsecs;
    threadStateDatabase.timeout.flags = flags;

    return NV_OK;
}

//
// Resets the current threadId time
//
NV_STATUS threadStateResetTimeout(OBJGPU *pGpu)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE *pThreadNode = NULL;

    // Check to see if ThreadState Timeout is enabled
    if ((threadStateDatabase.setupFlags &
          THREAD_STATE_SETUP_FLAGS_TIMEOUT_ENABLED) == NV_FALSE)
    {
        return NV_ERR_INVALID_STATE;
    }

    rmStatus = threadStateGetCurrent(&pThreadNode, pGpu);
    if ((rmStatus == NV_OK) && pThreadNode )
    {
        // Reset the timeout
        rmStatus = _threadNodeInitTime(pThreadNode);
        if (rmStatus == NV_OK)
        {
            pThreadNode->flags |= THREAD_STATE_FLAGS_TIMEOUT_INITED;
            _threadStatePrintInfo(pThreadNode);
        }
    }

    return rmStatus;
}

void threadStateLogTimeout(OBJGPU *pGpu, NvU64 funcAddr, NvU32 lineNum)
{
        // Log the Timeout in the RM Journal
        RmRC2GpuTimeout3_RECORD* pRec = NULL;

        rcdbAddAssertJournalRecWithLine(pGpu, lineNum, (void**)&pRec,
                                RmGroup, RmRC2GpuTimeout_V3,
                                sizeof(RmRC2GpuTimeout3_RECORD),
                                DRF_DEF(_RM, _ASSERT, _TYPE, _INFO),
                                funcAddr);

        // If this is release and we have RmBreakOnRC on -- Stop
#ifndef DEBUG
        OBJSYS    *pSys = SYS_GET_INSTANCE();
        if (DRF_VAL(_DEBUG, _BREAK_FLAGS, _GPU_TIMEOUT, pSys->debugFlags) ==
            NV_DEBUG_BREAK_FLAGS_GPU_TIMEOUT_ENABLE)
        {
            DBG_BREAKPOINT();
        }
#endif
}

//
// Checks the current threadId time against a set timeout period
//
NV_STATUS threadStateCheckTimeout(OBJGPU *pGpu, NvU64 *pElapsedTimeUs)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE *pThreadNode = NULL;

    if (pElapsedTimeUs)
        *pElapsedTimeUs = 0;

    //
    // Make sure the DB has been initialized, we have a valid threadId,
    // and that the Timeout logic is enabled
    //
    if ((threadStateDatabase.setupFlags &
          THREAD_STATE_SETUP_FLAGS_TIMEOUT_ENABLED) == NV_FALSE)
    {
        return NV_ERR_INVALID_STATE;
    }
    if  (threadStateDatabase.timeout.flags == 0)
    {
        return NV_ERR_INVALID_STATE;
    }

    rmStatus = threadStateGetCurrent(&pThreadNode, pGpu);
    if ((rmStatus == NV_OK) && pThreadNode )
    {
        if (pThreadNode->flags & THREAD_STATE_FLAGS_TIMEOUT_INITED)
        {
            rmStatus = _threadNodeCheckTimeout(pGpu, pThreadNode, pElapsedTimeUs);
        }
        else
        {
            rmStatus = NV_ERR_INVALID_STATE;
        }
    }

    return rmStatus;
}

static void _threadStateSetTimeoutOverride(THREAD_STATE_NODE *pThreadNode, NvU64 newTimeoutMs)
{
    NvU64 timeInNs = osGetCurrentTick();

    _threadStateSetNextCpuYieldTime(pThreadNode);

    if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSTIMER)
    {
        pThreadNode->timeout.nonComputeTime = timeInNs + (newTimeoutMs * 1000 * 1000);
        pThreadNode->timeout.computeTime = timeInNs + (newTimeoutMs * 1000 * 1000);
    }
    else if (threadStateDatabase.timeout.flags & GPU_TIMEOUT_FLAGS_OSDELAY)
    {
        // Convert from msecs (1,000) to usecs (1,000,000)
        pThreadNode->timeout.nonComputeTime = newTimeoutMs * 1000;
        pThreadNode->timeout.computeTime = newTimeoutMs * 1000;
    }
}

//
// Set override timeout value for specified thread
//
void threadStateSetTimeoutOverride(THREAD_STATE_NODE *pThreadNode, NvU64 newTimeoutMs)
{
    pThreadNode->timeout.overrideTimeoutMsecs = newTimeoutMs;
    _threadStateSetTimeoutOverride(pThreadNode, newTimeoutMs);
}

//
// One-time override timeout for specified thread; does not apply across timeout resets
//
void threadStateSetTimeoutSingleOverride(THREAD_STATE_NODE *pThreadNode, NvU64 newTimeoutMs)
{
    // Does not cache override in overrideTimeoutMsecs, so it is not re-applied upon reset.
    _threadStateSetTimeoutOverride(pThreadNode, newTimeoutMs);
}

NV_STATUS threadStateEnqueueCallbackOnFree
(
    THREAD_STATE_NODE          *pThreadNode,
    THREAD_STATE_FREE_CALLBACK *pCallback
)
{
    THREAD_STATE_FREE_CALLBACK *pCbListNode;

    if ((pThreadNode == NULL) || (pCallback == NULL) ||
        (pCallback->pCb == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    if (!(pThreadNode->flags & THREAD_STATE_FLAGS_STATE_FREE_CB_ENABLED))
        return NV_ERR_INVALID_OPERATION;

    // Add from tail to maintain FIFO semantics.
    pCbListNode = listAppendNew(&pThreadNode->cbList);
    if (pCbListNode == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pCbListNode->pCb = pCallback->pCb;
    pCbListNode->pCbData = pCallback->pCbData;

    return NV_OK;
}

void threadStateRemoveCallbackOnFree
(
    THREAD_STATE_NODE          *pThreadNode,
    THREAD_STATE_FREE_CALLBACK *pCallback
)
{
    THREAD_STATE_FREE_CALLBACK *pCbListNode;

    NV_ASSERT_OR_RETURN_VOID(pThreadNode->flags &
                             THREAD_STATE_FLAGS_STATE_FREE_CB_ENABLED);

    //
    // Remove doesn't need to obey FIFO semantics.
    //
    // Must remove only one entry per call to be symmetric with
    // threadStateEnqueueCallbackOnFree(). It is caller's responsibility to
    // invoke this API repeatedly as needed.
    //
    for (pCbListNode = listHead(&pThreadNode->cbList);
         pCbListNode != NULL;
         pCbListNode = listNext(&pThreadNode->cbList, pCbListNode))
    {
        if ((pCbListNode->pCb == pCallback->pCb) &&
            (pCbListNode->pCbData = pCallback->pCbData))
        {
            listRemove(&pThreadNode->cbList, pCbListNode);
            return;
        }
    }
}
