/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This module contains the multiprocessor synchronization
*       framework provided to the RM kernel.
*
******************************************************************************/

#include "core/core.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "diagnostics/tracer.h"
#include "objtmr.h"
#include <os/os.h>
#include <nv_ref.h>
#include <gpu/gpu.h>
#include "kernel/gpu/intr/intr.h"
#include <gpu/bif/kernel_bif.h>
#include "gpu/disp/kern_disp.h"

//
// GPU lock
//
// The GPU lock is used to synchronize access across all IRQ levels.
//
// Synchronization is done using a binary semaphore. See bug 1716608 for details
//
typedef struct
{
    PORT_SEMAPHORE *    pWaitSema; // Binary semaphore. See bug 1716608
    volatile NvS32      count;
    volatile NvBool     bRunning;
    volatile NvBool     bSignaled;
    OS_THREAD_HANDLE    threadId;
    NvU16               priority;
    NvU16               priorityPrev;
    NvU64               timestamp;
} GPULOCK;

//
// GPU lock info
//
// This structure contains all the state needed
// to manage the GPU locks.
//
// Access to this structure's fields is regulated by both pLock spinlock and
// the global RM API lock.
// The lock order is logically: API -> Spinlock -> Individual GPU locks,
//    however, it is possible to release and reacquire the spinlock while holding
//    a GPU lock. This is acceptable because no waits will be performed with
//    the spinlock held.
//
typedef struct
{
    //
    // Spinlock that protects access to everything in this structure.
    //
    PORT_SPINLOCK *     pLock;

    //
    // Mask of GPUs that can be locked.  Starts out as zero and
    // then modified as GPUs are attached to the system.
    // Requires holding API, pLock or any GPU to read, API+pLock+GPUs to write.
    //
    NvU32               gpusLockableMask;

    //
    // Mask of GPUs that have been "hidden" by the rmGpuLockHide routine.
    // Atomically read/written
    //
    NvU32 volatile      gpusHiddenMask;

    //
    // Mask of GPUs currently locked.
    // Requires holding pLock to read or write
    //
    NvU32               gpusLockedMask;

    //
    // Mask of GPU locks currently frozen.
    //
    NvU32               gpusFreezeMask;

    //
    // Tracks largest valid gpuInstance for which a lock has been allocated.
    // This is useful to pare down the # of loop iterations we need to make
    // when searching for lockable GPUs.
    // Requires holding API, pLock or any GPU to read, API+pLock+GPUs to write.
    //
    NvU32               maxLockableGpuInst;

    //
    // Array of per-GPU locks indexed by gpuInstance.
    //
    GPULOCK             gpuLocks[NV_MAX_DEVICES];

    //
    // Lock call trace info.
    //
    LOCK_TRACE_INFO     traceInfo;
} GPULOCKINFO;

static GPULOCKINFO rmGpuLockInfo;

static NV_STATUS _rmGpuLocksAcquire(NvU32, NvU32, NvU32, void *, NvU32 *);
static NvU32     _rmGpuLocksRelease(NvU32, NvU32, OBJGPU *, void *);
static NvBool    _rmGpuLockIsOwner(NvU32);

//
// rmGpuLockInfoInit
//
// Initialize GPU lock info state.
//
NV_STATUS
rmGpuLockInfoInit(void)
{
    portMemSet(&rmGpuLockInfo, 0, sizeof(rmGpuLockInfo));

    rmGpuLockInfo.pLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (rmGpuLockInfo.pLock == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    rmGpuLockInfo.maxLockableGpuInst = (NvU32)-1;

    return NV_OK;
}

//
// rmGpuLockInfoDestroy
//
// Initialize GPU lock info state.
//
void
rmGpuLockInfoDestroy(void)
{
    //
    // We expect all locks to have been freed by this point.
    //
    NV_ASSERT_OR_RETURN_VOID(rmGpuLockInfo.gpusLockableMask == 0);

    if (rmGpuLockInfo.pLock != NULL)
        portSyncSpinlockDestroy(rmGpuLockInfo.pLock);
}

//
// rmGpuLockAlloc
//
// Allocate GPU lock.
//
NV_STATUS
rmGpuLockAlloc(NvU32 gpuInst)
{
    GPULOCK *pGpuLock;
    NvU32 gpuMask, gpuLockedMask;
    NV_STATUS status;
    NvU64 threadId = ~0;
    NvU64 timestamp;

    // validate gpuInst argument
    NV_ASSERT_OR_RETURN((gpuInst < NV_MAX_DEVICES), NV_ERR_INVALID_ARGUMENT);

    pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

    // test to make sure lock hasn't already been allocated
    NV_ASSERT_OR_RETURN(((rmGpuLockInfo.gpusLockableMask & NVBIT(gpuInst)) == 0),
                      NV_ERR_INVALID_STATE);

    // TODO: RM-1492 MODS does not hold API lock when allocating GPUs.
    NV_ASSERT(rmapiLockIsOwner());

    // allocate intr mask lock
    status = rmIntrMaskLockAlloc(gpuInst);
    if (status != NV_OK)
        return status;

    // clear struct for good measure and then init everything
    portMemSet(pGpuLock, 0, sizeof(*pGpuLock));

    pGpuLock->pWaitSema = portSyncSemaphoreCreate(portMemAllocatorGetGlobalNonPaged(), 0);
    if (pGpuLock->pWaitSema == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pGpuLock->count = 1;
    pGpuLock->bRunning = NV_FALSE;
    pGpuLock->bSignaled = NV_FALSE;
    pGpuLock->threadId = ~(NvU64)0;

    //
    // Before updating the gpusLockableMask value we need to grab the
    // locks for all *other* GPUs.  This ensures that the gpusLockableMask
    // value cannot change in between acquire/release calls issued by
    // a different thread. Reading this is safe under API lock.
    //
    gpuMask = rmGpuLockInfo.gpusLockableMask;

    // LOCK: acquire GPU locks
    status = _rmGpuLocksAcquire(gpuMask, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT,
                                NV_RETURN_ADDRESS(), &gpuLockedMask);
    if (status == NV_WARN_NOTHING_TO_DO)
    {
        // Verify that this is a valid case - i.e. we're attaching first GPU.
        NV_ASSERT(gpuMask == 0);
        status = NV_OK;
    }
    if (status != NV_OK)
        goto done;

    portSyncSpinlockAcquire(rmGpuLockInfo.pLock);
    // add the GPU to the lockable mask
    rmGpuLockInfo.gpusLockableMask |= NVBIT(gpuInst);

    // save this gpuInst if it's the largest we've seen so far
    if (rmGpuLockInfo.maxLockableGpuInst == (NvU32)-1)
    {
        rmGpuLockInfo.maxLockableGpuInst = gpuInst;
    }
    else
    {
        if (gpuInst > rmGpuLockInfo.maxLockableGpuInst)
            rmGpuLockInfo.maxLockableGpuInst = gpuInst;
    }


    threadId = portThreadGetCurrentThreadId();
    osGetCurrentTick(&timestamp);
    INSERT_LOCK_TRACE(&rmGpuLockInfo.traceInfo,
                      NV_RETURN_ADDRESS(),
                      lockTraceAlloc,
                      gpuInst, 0,
                      threadId,
                      0, 0,
                      timestamp);

    portSyncSpinlockRelease(rmGpuLockInfo.pLock);

    // UNLOCK: release GPU locks
    _rmGpuLocksRelease(gpuLockedMask, GPUS_LOCK_FLAGS_NONE, NULL, NV_RETURN_ADDRESS());

done:
    if (status != NV_OK)
    {
        if (pGpuLock->pWaitSema)
            portSyncSemaphoreDestroy(pGpuLock->pWaitSema);

        // free intr mask lock
        rmIntrMaskLockFree(gpuInst);
    }

    return status;
}

//
// rmGpuLockFree
//
// We call this routine with the API lock held, but not the GPUs
//
// This routine must always free the lock.
//
void
rmGpuLockFree(NvU32 gpuInst)
{
    NvU32 i;
    GPULOCK *pGpuLock;
    NV_STATUS status;
    NvU32 gpuMask, gpuAttachMask, gpuLockedMask;
    NvU64 threadId = ~0;
    NvU64 timestamp;

    // validate gpuInst argument
    NV_ASSERT_OR_RETURN_VOID((gpuInst < NV_MAX_DEVICES));
    // TODO: RM-1492 MODS does not hold API lock when allocating GPUs.
    NV_ASSERT(rmapiLockIsOwner());

    pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

    //
    // Don't acquire/release gpu locks for the gpus which had been detached and
    // destroyed.
    // NOTE: The gpuInst GPU has already been detached at this point, so gpuMask
    // will not include it.
    //
    gpumgrGetGpuAttachInfo(NULL, &gpuAttachMask);
    // Reading rmGpuLockInfo.gpusLockableMask is safe under API lock
    gpuMask = (rmGpuLockInfo.gpusLockableMask & gpuAttachMask);

    // LOCK: acquire GPU locks
    status = _rmGpuLocksAcquire(gpuMask, GPUS_LOCK_FLAGS_NONE,
                                RM_LOCK_MODULES_DESTROY,
                                NV_RETURN_ADDRESS(), &gpuLockedMask);
    if (status != NV_OK && status != NV_WARN_NOTHING_TO_DO)
        return;

    portSyncSpinlockAcquire(rmGpuLockInfo.pLock);
    // remove the GPU from the lockable mask
    rmGpuLockInfo.gpusLockableMask &= ~NVBIT(gpuInst);

    threadId = portThreadGetCurrentThreadId();
    osGetCurrentTick(&timestamp);
    INSERT_LOCK_TRACE(&rmGpuLockInfo.traceInfo,
                      NV_RETURN_ADDRESS(),
                      lockTraceFree,
                      gpuInst, 0,
                      threadId,
                      0, 0,
                      timestamp);

    //
    // Reset max lockable gpuInstance if necessary.
    //
    if (gpuInst == rmGpuLockInfo.maxLockableGpuInst)
    {
        if (rmGpuLockInfo.gpusLockableMask != 0)
        {
            for (i = rmGpuLockInfo.maxLockableGpuInst; i != (NvU32)-1; i--)
            {
                if (rmGpuLockInfo.gpusLockableMask & NVBIT(i))
                    break;
            }

            rmGpuLockInfo.maxLockableGpuInst = i;
        }
        else
        {
            // no locks left so start over
            rmGpuLockInfo.maxLockableGpuInst = (NvU32)-1;
        }
    }
    portSyncSpinlockRelease(rmGpuLockInfo.pLock);

    // UNLOCK: release GPU locks
    _rmGpuLocksRelease(gpuLockedMask, GPUS_LOCK_FLAGS_NONE, NULL, NV_RETURN_ADDRESS());

    if (pGpuLock->pWaitSema)
    {
        //
        // At this point, we may still have threads waiting on the semaphore,
        // and possibly one thread holding the lock.
        // Wake up all threads that are waiting, and wait until the holding one
        // is done.
        //
        while (pGpuLock->count <= 0) // volatile read
        {
            portSyncSemaphoreRelease(pGpuLock->pWaitSema);
            osSchedule(); // Yield execution
            portSyncSemaphoreAcquire(pGpuLock->pWaitSema);
        }
        portSyncSemaphoreDestroy(pGpuLock->pWaitSema);
    }

    portMemSet(pGpuLock, 0, sizeof(*pGpuLock));

    // free intr mask lock
    rmIntrMaskLockFree(gpuInst);
}

//
// _gpuLocksAcquireDisableInterrupts
//
// Disable GPUs Interrupts thus blocking the ISR from
// entering.
//
static void _gpuLocksAcquireDisableInterrupts(NvU32 gpuInst, NvU32 flags)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInst);

    //
    // Handle case where we're asked to acquire a lock for a GPU that
    // has been removed from the ODB (e.g. from RmCleanupNvAdapter).
    //
    if (pGpu == NULL)
        return;

    // if hidden GPU then we skip out...
    if (rmGpuLockIsHidden(pGpu))
        return;

    if (osIsSwPreInitOnly(pGpu->pOsGpuInfo))
        return;

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        return;


    if (osLockShouldToggleInterrupts(pGpu))
    {
        Intr *pIntr = GPU_GET_INTR(pGpu);
        NvBool isIsr = !!(flags & GPU_LOCK_FLAGS_COND_ACQUIRE);
        NvBool bBcEnabled = gpumgrGetBcEnabledStatus(pGpu);

        // Always disable intrs for cond code
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

        // Note: SWRL is enabled only for vGPU, and is disabled otherwise.
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
        {
            // Disable the RM callback timer interrupt
            OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
            tmrRmCallbackIntrDisable(pTmr, pGpu);
        }

        osDisableInterrupts(pGpu, isIsr);

        if ((pIntr != NULL) && pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING) &&
             (isIsr == NV_FALSE) )
        {
            NvU64 oldIrql;
            NvU32 intrMaskFlags;

            oldIrql = rmIntrMaskLockAcquire(pGpu);

            intrMaskFlags = intrGetIntrMaskFlags(pIntr);
            intrMaskFlags &= ~INTR_MASK_FLAGS_ISR_SKIP_MASK_UPDATE;
            intrSetIntrMaskFlags(pIntr, intrMaskFlags);

            if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
            {
                // During non-cond RM code, allow some intrs to come in.
                if (pIntr->getProperty(pIntr, PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE))
                {
                    intrSetDisplayInterruptEnable_HAL(pGpu, pIntr, NV_TRUE,  NULL /* threadstate */);
                }
                else
                {
                    intrSetIntrMask_HAL(pGpu, pIntr, &pIntr->intrMask.engMaskUnblocked, NULL /* threadstate */);
                }
            }
            else
            {
                // Lazy case - we will lazily disable Intrs via the ISR as seen
            }

            intrSetIntrEnInHw_HAL(pGpu, pIntr, intrGetIntrEn(pIntr), NULL /* threadstate */);

            rmIntrMaskLockRelease(pGpu, oldIrql);
        }

        gpumgrSetBcEnabledStatus(pGpu, bBcEnabled);
    }
}

//
// _rmGpuLocksAcquire
//
// Acquire the set of locks specified by gpuMask in ascending gpuInstance
// order.
//
static NV_STATUS
_rmGpuLocksAcquire(NvU32 gpuMask, NvU32 flags, NvU32 module, void *ra, NvU32 *pGpuLockedMask)
{
    NV_STATUS status = NV_OK;
    NvU32     gpuInst;
    NvU32     gpuMaskLocked = 0;
    GPULOCK   *pGpuLock;
    NvBool    bHighIrql, bCondAcquireCheck;
    NvU32     maxLockableGpuInst;
    NvU64     threadId = portThreadGetCurrentThreadId();
    NvU64     priority = 0;
    NvU64     priorityPrev = 0;
    NvU64     timestamp;
    NvBool    bLockAll = NV_FALSE;

    bHighIrql = (portSyncExSafeToSleep() == NV_FALSE);
    bCondAcquireCheck = ((flags & GPU_LOCK_FLAGS_COND_ACQUIRE) != 0);

    if (pGpuLockedMask)
        *pGpuLockedMask = 0;

    threadPriorityBoost(&priority, &priorityPrev);
    portSyncSpinlockAcquire(rmGpuLockInfo.pLock);

    //
    // If caller wishes to lock all GPUs then convert incoming  mask
    // to set that are actually lockable.
    //
    if (gpuMask == GPUS_LOCK_ALL)
    {
        gpuMask = rmGpuLockInfo.gpusLockableMask;
        bLockAll = NV_TRUE;
    }

    //
    // We may get a gpuMask of zero during setup of the first GPU attached.
    //
    if (gpuMask == 0)
    {
        status = NV_WARN_NOTHING_TO_DO;
        goto done;
    }

    //
    // If a read-only lock was requested, check to see if the module is allowed
    // to take read-only locks
    //
    if ((flags & GPU_LOCK_FLAGS_READ) && (module != RM_LOCK_MODULES_NONE))
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        if ((pSys->gpuLockModuleMask & NVBIT(module)) == 0)
        {
            flags &= ~RMAPI_LOCK_FLAGS_READ;
        }
    }

    if ((gpuMask & rmGpuLockInfo.gpusLockableMask) != gpuMask)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Attempting to lock GPUs (mask=%08x) that are not lockable (mask=%08x). Will skip non-lockables.\n",
                  gpuMask, rmGpuLockInfo.gpusLockableMask);
        gpuMask &= rmGpuLockInfo.gpusLockableMask;
        // Nothing to do if no requested GPUs are lockable
        if (gpuMask == 0)
        {
            status = NV_ERR_INVALID_REQUEST;
            goto done;
        }
    }
    // Cache global variable so it doesn't change in the middle of the loop.
    maxLockableGpuInst = rmGpuLockInfo.maxLockableGpuInst;
    if (maxLockableGpuInst >= NV_MAX_DEVICES)
    {
        DBG_BREAKPOINT();
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    if (flags & GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE)
    {
        NvU32 ownedMask = rmGpuLocksGetOwnedMask();

        // In safe mode, we never attempt to acquire locks we already own..
        gpuMask &= ~ownedMask;
        // If we already own everything we need, just bail early.
        if (gpuMask == 0)
        {
            status = NV_WARN_NOTHING_TO_DO;
            goto done;
        }

        // If we own a higher order lock than one of the needed ones, we are
        // violating the locking order and need to do a conditional acquire
        // clz32(0) == ctz(0) == 32:
        //    owned=0b00001100, needed=0b00110000: ((32-28) >  4), bCond=FALSE
        //    owned=0b00001100, needed=0b00110010: ((32-28) >  1), bCond=TRUE
        //    owned=0b11000011, needed=0b00010000: ((32-24) >  4), bCond=TRUE
        //    owned=0b00000000, needed=0b00000001: ((32-32) >  0), bCond=FALSE
        if ((32-portUtilCountLeadingZeros32(ownedMask)) > portUtilCountTrailingZeros32(gpuMask))
        {
            bCondAcquireCheck = NV_TRUE;
        }
    }

    //
    // There are two scenarios where we want to check to see if all of
    // the target locks are available before proceeding:
    //
    //  1- this is a conditional acquire call that must fail if any
    //     of the locks aren't currently available
    //  2- we are called at an elevated IRQL and cannot sleep waiting
    //     for a lock
    //
    if (bCondAcquireCheck || bHighIrql)
    {
        for (gpuInst = 0;
             gpuInst <= maxLockableGpuInst;
             gpuInst++)
        {
            if ((gpuMask & NVBIT(gpuInst)) == 0)
                continue;

            pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

            //
            // The conditional check takes precedence here.
            //
            if (bCondAcquireCheck)
            {
                if (pGpuLock->bRunning == NV_TRUE)
                {
                    status = NV_ERR_STATE_IN_USE;
                    goto done;
                }
            }
            else if (bHighIrql)
            {
                if (pGpuLock->count <= 0)
                {
                    status = NV_ERR_STATE_IN_USE;
                    goto done;
                }
            }
        }
    }

    //
    // Now (attempt) to acquire the locks...
    //
    for (gpuInst = 0;
         gpuInst <= maxLockableGpuInst;
         gpuInst++)
    {
        // skip any not in the mask
        if ((gpuMask & NVBIT(gpuInst)) == 0)
            continue;

        //
        // We might have released the spinlock while sleeping on a previous
        // semaphore, so check if current GPU wasn't deleted during that time
        //
        if ((NVBIT(gpuInst) & rmGpuLockInfo.gpusLockableMask) == 0)
        {
            NV_PRINTF(LEVEL_NOTICE,
                "GPU lock %d freed while we were waiting on a previous lock\n",
                gpuInst);
            continue;
        }

        pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

        //
        // Check to see if the lock is not free...we should only fall into this
        // case if we can actually tolerate waiting for it.
        //
        if (!bCondAcquireCheck && (pGpuLock->count <= 0))
        {
            //
            // Assert that this is not already the owner of the GpusLock
            // (the lock will cause a hang if acquired recursively)
            //
            // Note that conditional acquires of the semaphore return
            // IN_USE if the lock is already taken by this or another
            // thread (ie they don't hang).
            //
            // We have to place the assert after the conditional acquire
            // check, otherwise it could happen that:
            //
            // 1. We acquire the semaphore in one DPC function, but don't
            // release it before finishing (a later DPC will
            // rmGpuLockSetOwner and then release it).
            // 2. A DPC timer function sneaks in, tries to grab the lock
            // conditionally.
            // 3. On Vista, the timer DPC could be running under the same
            // threadid as the first DPC, so the code will believe that
            // the timer DPC is the owner, triggering the assert with
            // a false positive.
            //
            // (the scenario described above causing the false positive
            // assert, happens with the stack trace:
            // osTimerCallback->osRun1HzCallbacksNow->>rmGpuLocksAcquire)
            //
            if (_rmGpuLockIsOwner(NVBIT(gpuInst)))
            {
                NV_PRINTF(LEVEL_INFO,
                          "GPU lock is already acquired by this thread\n");
                NV_ASSERT(0);
                //
                // TODO: RM-1493 undo previous acquires
                //
                status = NV_ERR_STATE_IN_USE;
                goto done;
            }

            portAtomicDecrementS32(&pGpuLock->count);
            do
            {
                portSyncSpinlockRelease(rmGpuLockInfo.pLock);
                portSyncSemaphoreAcquire(pGpuLock->pWaitSema);
                portSyncSpinlockAcquire(rmGpuLockInfo.pLock);

                if ((rmGpuLockInfo.gpusLockableMask & NVBIT(gpuInst)) == 0)
                {
                    NV_PRINTF(LEVEL_WARNING,
                              "GPU lock %d freed while threads were still waiting.\n",
                              gpuInst);
                    // Can't release the semaphore while a spinlock is held
                    portSyncSpinlockRelease(rmGpuLockInfo.pLock);
                    portSyncSemaphoreRelease(pGpuLock->pWaitSema);
                    // Loop assumes spinlock is held, so reacquire
                    portSyncSpinlockAcquire(rmGpuLockInfo.pLock);
                    portAtomicIncrementS32(&pGpuLock->count);
                    // Skip this GPU, keep trying any others.
                    goto next_gpu_instance;
                }
                pGpuLock->bSignaled = NV_FALSE;
            }
            while (pGpuLock->bRunning);
        }
        else
        {
            portAtomicDecrementS32(&pGpuLock->count);
        }

        // indicate that we are running
        pGpuLock->bRunning = NV_TRUE;

        // save off thread that owns this GPUs lock
        osGetCurrentThread(&pGpuLock->threadId);

        // now disable interrupts
        _gpuLocksAcquireDisableInterrupts(gpuInst, flags);

        // mark this one as locked
        gpuMaskLocked |= NVBIT(gpuInst);

        // add acquire record to GPUs lock trace
        osGetCurrentTick(&timestamp);
        INSERT_LOCK_TRACE(&rmGpuLockInfo.traceInfo,
                          ra,
                          lockTraceAcquire,
                          gpuInst, gpuMask,
                          threadId,
                          bHighIrql,
                          (NvU16)priority,
                          timestamp);
        pGpuLock->priority = priority;
        pGpuLock->priorityPrev = priorityPrev;
        pGpuLock->timestamp = timestamp;

next_gpu_instance:
        ;
    }

    // update gpusLockedMask
    rmGpuLockInfo.gpusLockedMask |= gpuMaskLocked;

    // Log any changes to the GPU configuration due to race conditions
    if (maxLockableGpuInst != rmGpuLockInfo.maxLockableGpuInst)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "Max lockable instance changed from %d to %d\n",
                  maxLockableGpuInst, rmGpuLockInfo.maxLockableGpuInst);
    }
    if (gpuMaskLocked != gpuMask)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Locked a different GPU mask (0x%08x) than requested (0x%08x) @ %p.\n",
                  gpuMaskLocked, gpuMask, ra);
    }

    // Log any case where we wanted to but failed to lock ALL GPUs.
    if (bLockAll)
    {
        NV_ASSERT(gpuMaskLocked == rmGpuLockInfo.gpusLockedMask);
        NV_ASSERT(gpuMaskLocked == rmGpuLockInfo.gpusLockableMask);
    }

    if (pGpuLockedMask)
        *pGpuLockedMask = gpuMaskLocked;

    RMTRACE_RMLOCK(_GPUS_LOCK_ACQUIRE);

done:
    portSyncSpinlockRelease(rmGpuLockInfo.pLock);

    if (status != NV_OK)
    {
        threadPriorityRestore();
        return status;
    }

    return (gpuMaskLocked == 0) ? NV_WARN_NOTHING_TO_DO : NV_OK;
}

//
// rmGpuLocksAcquire
//
NV_STATUS
rmGpuLocksAcquire(NvU32 flags, NvU32 module)
{
    NV_STATUS status;
    NvU32 gpusLockedMask = 0;

    status = _rmGpuLocksAcquire(GPUS_LOCK_ALL, flags, module,
                                NV_RETURN_ADDRESS(), &gpusLockedMask);
    //
    // Since the request was to acquire locks for all GPUs, if there are none
    // we consider the request fulfilled.
    // Set to NV_OK since most callers check for NV_OK explicitly.
    //
    if (status == NV_WARN_NOTHING_TO_DO)
        status = NV_OK;
    //
    // If we successfully locked all GPUs but there's still more not locked
    // it means that an additional GPU was added in the meantime somehow.
    // Release everything and try again (once only to prevent infinite loops)
    //
    if (status == NV_OK && gpusLockedMask != rmGpuLockInfo.gpusLockableMask)
    {
        //
        // On Windows, at high IRQL we can't signal the semaphore. So we
        // use a second pGpu to schedule a DPC to do that. Pick one that
        // we've already locked for that purpose.
        //
        OBJGPU *pDpcGpu = gpumgrGetGpu(portUtilCountTrailingZeros32(gpusLockedMask));

        if (_rmGpuLocksRelease(gpusLockedMask, flags, pDpcGpu, NV_RETURN_ADDRESS()) == NV_SEMA_RELEASE_SUCCEED)
        {
            // All locks successfully released without a DPC scheduled, can re-attempt.
            status = _rmGpuLocksAcquire(GPUS_LOCK_ALL, flags, module, NV_RETURN_ADDRESS(), &gpusLockedMask);
            // If it happened again, just release and return
            if (status == NV_OK && gpusLockedMask != rmGpuLockInfo.gpusLockableMask)
            {
                _rmGpuLocksRelease(gpusLockedMask, flags, pDpcGpu, NV_RETURN_ADDRESS());
                status = NV_ERR_INVALID_LOCK_STATE;
            }
        }
        else
        {
            status = NV_ERR_INVALID_LOCK_STATE;
        }
    }

    if (status == NV_OK)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        if (pCallContext != NULL)
        {
            NV_ASSERT(pCallContext->pLockInfo != NULL);
            if (pCallContext->pLockInfo != NULL)
                pCallContext->pLockInfo->state |= RM_LOCK_STATES_GPUS_LOCK_ACQUIRED;
        }
    }

    return status;
}

//
// rmGpuGroupLockAcquire: Takes lock for only those gpus specified by the gpuGrpId
//
NV_STATUS
rmGpuGroupLockAcquire
(
    NvU32 gpuInst,
    GPU_LOCK_GRP_ID gpuGrpId,
    NvU32 flags,
    NvU32 module,
    GPU_MASK* pGpuMask
)
{
    NvU32 status = NV_OK;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    //
    // QuadroSync (previously known as GSync) is a cross GPU feature that
    // synchronizes display across multiple GPUs.  See changelist 16809243.  If
    // GSync is enabled, acquire locks for all GPUs.
    //
    const NvBool bGSync = pSys->getProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED);

    if (pGpuMask == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (bGSync)
    {
        *pGpuMask = GPUS_LOCK_ALL;
    }
    else
    {
        status = rmGpuGroupLockGetMask(gpuInst, gpuGrpId, pGpuMask);
    }
    if (status != NV_OK)
        return status;

    status = _rmGpuLocksAcquire(*pGpuMask, flags, module, NV_RETURN_ADDRESS(), pGpuMask);
    if (status == NV_WARN_NOTHING_TO_DO)
    {
        //
        // Callers using SAFE_LOCK_UPGRADE will often consider this normal,
        // so silence the print if the flag is passed.
        //
        NV_PRINTF_COND((flags & GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE), LEVEL_INFO, LEVEL_NOTICE,
                  "Nothing to lock for gpuInst=%d, gpuGrpId=%d, gpuMask=0x%08x\n",
                  gpuInst, gpuGrpId, *pGpuMask);
        status = NV_OK;
    }

    if ((status == NV_OK) && bGSync)
    {
        GPU_MASK deviceGpuMask = 0;
        rmGpuGroupLockGetMask(gpuInst, GPU_LOCK_GRP_DEVICE, &deviceGpuMask);
        //
        // Verify that we actually locked *this* device, not all others.
        // Check *held* locks not *acquired* locks in case of SAFE_LOCK_UPGRADE.
        //
        if ((rmGpuLocksGetOwnedMask() & deviceGpuMask) != deviceGpuMask)
        {
            //
            // On Windows, at high IRQL we can't signal the semaphore. So we
            // use a second pGpu to schedule a DPC to do that. Pick one that
            // we've already locked for that purpose.
            //
            OBJGPU *pDpcGpu = gpumgrGetGpu(portUtilCountTrailingZeros32(*pGpuMask));

            _rmGpuLocksRelease(*pGpuMask, flags, pDpcGpu, NV_RETURN_ADDRESS());
            // Notify that pGpu is gone and finish
            status = NV_ERR_INVALID_DEVICE;
        }
    }

    return status;
}

//
// rmGpuGroupLockIsOwner
//
// Checks if current thread already owns locks for a given gpu group.
// If NV_TRUE, it returns the current group mask
//
NvBool
rmGpuGroupLockIsOwner(NvU32 gpuInst, GPU_LOCK_GRP_ID gpuGrpId, GPU_MASK* pGpuMask)
{
    NvBool bIsOwner = NV_FALSE;
    NvBool bReleaseSpinlock = NV_FALSE;

    if (rmGpuGroupLockGetMask(gpuInst, gpuGrpId, pGpuMask) != NV_OK)
        return NV_FALSE;

    if ((gpuGrpId == GPU_LOCK_GRP_ALL) && (*pGpuMask == GPUS_LOCK_ALL))
    {
        bReleaseSpinlock = NV_TRUE;
        portSyncSpinlockAcquire(rmGpuLockInfo.pLock);
        *pGpuMask = rmGpuLockInfo.gpusLockableMask;
    }

    bIsOwner = _rmGpuLockIsOwner(*pGpuMask);

    if (bReleaseSpinlock)
        portSyncSpinlockRelease(rmGpuLockInfo.pLock);

    return bIsOwner;
}

//
// rmDeviceGpuLocksAcquire
//
NV_STATUS
rmDeviceGpuLocksAcquire(OBJGPU *pGpu, NvU32 flags, NvU32 module)
{
    NvU32 gpuMask;
    NvU32 gpuLockedMask = 0;
    NV_STATUS status;
    OBJSYS    *pSys = SYS_GET_INSTANCE();

    //
    // Make sure that the GPU is in usable condition before continuing further.
    // If pGpu is invalid or NULL, using pGpu structure members can cause a crash.
    // It is possible that another thread has teared-down the GPU as part of TDR recovery procedure.
    // gpumgrGetGpuMask function called immediately after the check accesses pGpu->deviceInstance.
    // If pGpu is invalid, this will cause a crash.
    // See bugs 200183282, 200118671
    //
    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        return NV_ERR_INVALID_DEVICE;

    // XXX: TOCTTOU issue here, but nothing to be done. Use rmGpuGroupLockXxx API instead.
    // TODO: RM-1140 - Migrate to the other API and delete this one.
    gpuMask = gpumgrGetGpuMask(pGpu);

    //
    // QuadroSync (previously known as GSync) is a cross GPU feature that
    // synchronizes display across multiple GPUs.  See changelist 16809243.  If
    // GSync is enabled, acquire locks for all GPUs.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED))
    {
        status = _rmGpuLocksAcquire(GPUS_LOCK_ALL, flags, module,
                                    NV_RETURN_ADDRESS(), &gpuLockedMask);

        // Verify that we actually locked *this* pGpu, not all others.
        if (status == NV_OK && ((gpuLockedMask & gpuMask) != gpuMask))
        {
            //
            // On Windows, at high IRQL we can't signal the semaphore. So we
            // use a second pGpu to schedule a DPC to do that. Pick one that
            // we've already locked for that purpose.
            //
            OBJGPU *pDpcGpu = gpumgrGetGpu(portUtilCountTrailingZeros32(gpuLockedMask));

            _rmGpuLocksRelease(gpuLockedMask, flags, pDpcGpu, NV_RETURN_ADDRESS());
            // Notify that pGpu is gone and finish
            status = NV_ERR_INVALID_DEVICE;
        }
    }
    else
    {
        status = _rmGpuLocksAcquire(gpuMask, flags, module,
                                    NV_RETURN_ADDRESS(), &gpuLockedMask);
        //
        // Verify that the SLI configuration did not change for this pGpu
        //
        if (status == NV_OK)
        {
            if (gpuMask != gpumgrGetGpuMask(pGpu))
            {
                _rmGpuLocksRelease(gpuLockedMask, flags, pGpu, NV_RETURN_ADDRESS());
                status = NV_ERR_INVALID_DEVICE;
            }
        }
    }

    // Even if we get NV_OK, there are a couple of edge cases to handle
    if (status == NV_OK)
    {
        //
        // Currently, release-and-free sequence for GPU locks is not atomic, so
        // we could theoretically acquire a perfectly valid lock for a non-existent
        // device. In this case, just release and return error.
        //
        if (!gpumgrIsGpuPointerValid(pGpu))
        {
            // We don't need a pDpcGpu here as this can't happen at DIRQL.
            _rmGpuLocksRelease(gpuLockedMask, flags, NULL, NV_RETURN_ADDRESS());
            status = NV_ERR_INVALID_DEVICE;
        }
        else
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            if (pCallContext != NULL)
            {
                NV_ASSERT(pCallContext->pLockInfo != NULL);
                if (pCallContext->pLockInfo != NULL)
                    pCallContext->pLockInfo->state |= RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED;
            }
        }
    }
    //
    // We're trying to acquire the lock for a specific device. If we get
    // NOTHING_TO_DO, that means the device has been detached and pGpu is no
    // longer valid.
    //
    if (status == NV_WARN_NOTHING_TO_DO)
        status = NV_ERR_INVALID_DEVICE;

    return status;
}

//
// _gpuLocksReleaseEnableInterrupts
//
// Enable GPUs Interrupts thus allowing the ISR to
// enter.
//
static void _gpuLocksReleaseEnableInterrupts(NvU32 gpuInst, NvU32 flags)
{
    OBJGPU *pGpu;

    pGpu = gpumgrGetGpu(gpuInst);

    //
    // Handle case where we're asked to release a lock for a GPU
    // after it has been removed from the GpuMgr (e.g. from RmCleanUpNvAdapter).
    //
    if (pGpu == NULL)
        return;

    // if hidden GPU then we skip out...
    if (rmGpuLockIsHidden(pGpu))
        return;

    if (osIsSwPreInitOnly(pGpu->pOsGpuInfo))
        return;

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
        return;

    if (osLockShouldToggleInterrupts(pGpu))
    {
        Intr *pIntr = GPU_GET_INTR(pGpu);

        // Make sure we only enable interrupts unicast
        NvBool bBcEnabled = gpumgrGetBcEnabledStatus(pGpu);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
        if ((pIntr != NULL) &&
            pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING) )
        {
            NvU64 oldIrql;
            NvU32 intrMaskFlags;
            MC_ENGINE_BITVECTOR engines;

            oldIrql = rmIntrMaskLockAcquire(pGpu);

            intrMaskFlags = intrGetIntrMaskFlags(pIntr);
            intrMaskFlags |= INTR_MASK_FLAGS_ISR_SKIP_MASK_UPDATE;
            intrSetIntrMaskFlags(pIntr, intrMaskFlags);

            if (pIntr->getProperty(pIntr, PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING))
            {
                // Allow all intrs to be reflected and come in.
                bitVectorSetAll(&engines);
                intrSetIntrMask_HAL(pGpu, pIntr, &engines, NULL /* threadstate */);
            }
            else
            {
                // Lazy case - Enable all engine intrs that may have been disabled via the ISR
                bitVectorClrAll(&pIntr->intrMask.engMaskIntrsDisabled);
            }

            rmIntrMaskLockRelease(pGpu, oldIrql);
        }
        else
        {
            if ((pGpu->bIsSOC == NV_FALSE) && !IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
            {
                NV_ASSERT(intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL) != INTERRUPT_TYPE_HARDWARE);
            }
        }
        osEnableInterrupts(pGpu);

        // Note: SWRL is enabled only for vGPU, and is disabled otherwise.
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
        {
            // Enable the alarm interrupt. Rearm MSI when timer interrupt is pending.
            OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
            NvU32   retVal;

            tmrRmCallbackIntrEnable(pTmr, pGpu);
            tmrGetIntrStatus_HAL(pGpu, pTmr, &retVal, NULL);
            if (retVal != 0)
            {
                KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
                kbifCheckAndRearmMSI(pGpu, pKernelBif);
            }
        }

        gpumgrSetBcEnabledStatus(pGpu, bBcEnabled);
    }
}

static void _gpuLocksReleaseHandleDeferredWork(NvU32 gpuMask)
{
    OBJGPU    *pGpu;
    NvU32      i = 0;
    NvU32      gpuInstance = 0;

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (GPU_GET_INTR(pGpu) == NULL)
            continue;

        if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
            continue;

        for (i = 0; i < MAX_DEFERRED_CMDS; i++)
        {
            if (pGpu->pRmCtrlDeferredCmd[i].pending == RMCTRL_DEFERRED_READY)
            {
                // ignore failure here since caller won't be able to receive it
                if (rmControl_Deferred(&pGpu->pRmCtrlDeferredCmd[i]) != NV_OK)
                {
                    NV_ASSERT(0);
                }
            }
        }

        // Delay reset until we have the lock
        osHandleDeferredRecovery(pGpu);

        //
        // Bug 1798647: Defer osIsr until semaphore release when contention is high
        // osIsr does not perform its task if it is unable to acquire its locks
        // immediately. In Multi-GPU environment where contention on locks is high,
        // interrupts are getting starved. Ensure that osIsr is not starved by
        // deferring it to lock release time if lock is not available.
        // This WAR should be removed once per-GPU locks are implemented.
        //
        osDeferredIsr(pGpu);
    }
}

//
// _rmGpuLocksRelease
//
// Release the locks specified by gpuMask in descending gpuInstance order.
//
static NvU32
_rmGpuLocksRelease(NvU32 gpuMask, NvU32 flags, OBJGPU *pDpcGpu, void *ra)
{
    static volatile NvU32 bug200413011_WAR_WakeUpMask;
    GPULOCK *pGpuLock;
    NvU32   gpuMaskWakeup = 0;
    NvU32   gpuInst;
    NvU32   highestInstanceInGpuMask;
    NvBool  bSemaCanWake = portSyncExSafeToWake();
    NvBool  bHighIrql = (portSyncExSafeToSleep() == NV_FALSE);
    NvU64   threadId = portThreadGetCurrentThreadId();
    NvU64   priority = 0;
    NvU64   priorityPrev = 0;
    NvU64   timestamp;
    NV_STATUS status;

    //
    // We may get a gpuMask of zero during setup of the first GPU attached.
    //
    if (gpuMask == 0)
        return NV_OK;

    //
    // The lock(s) being released must not be frozen.
    // Log all attempts to do so, but don't bail early to enable recovery paths.
    //
    NV_ASSERT((rmGpuLockInfo.gpusFreezeMask & gpuMask) == 0);

    //
    // Important test.  Caller must own the lock(s) it is attempting to
    // release.
    // In some error recovery cases we want to be able to force release a lock.
    // Log all such issues, but don't bail early to enable recovery paths.
    //
    NV_ASSERT(_rmGpuLockIsOwner(gpuMask));

    _gpuLocksReleaseHandleDeferredWork(gpuMask);

    threadPriorityBoost(&priority, &priorityPrev);
    portSyncSpinlockAcquire(rmGpuLockInfo.pLock);

    if ((gpuMask & rmGpuLockInfo.gpusLockableMask) != gpuMask)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Releasing nonlockable GPU (already went through teardown). gpuMask = 0x%08x, gpusLockableMask = 0x%08x.\n",
                  gpuMask, rmGpuLockInfo.gpusLockableMask);
    }
    if ((gpuMask & rmGpuLockInfo.gpusLockedMask) != gpuMask)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Attempting to release unlocked GPUs. gpuMask = 0x%08x, gpusLockedMask = 0x%08x. Will skip them.\n",
                  gpuMask, rmGpuLockInfo.gpusLockedMask);
        gpuMask &= rmGpuLockInfo.gpusLockedMask;

        if (gpuMask == 0)
        {
            NV_PRINTF(LEVEL_WARNING, "No more GPUs to release after skipping");
            portSyncSpinlockRelease(rmGpuLockInfo.pLock);
            status = NV_OK;
            goto done;
        }
    }

    // Find the highest GPU instance that's locked, to be used for loop bounds
    highestInstanceInGpuMask = 31 - portUtilCountLeadingZeros32(gpuMask);
    if (highestInstanceInGpuMask > rmGpuLockInfo.maxLockableGpuInst)
    {
        NV_PRINTF(LEVEL_WARNING, "GPU mask for release (0x%08x) has higher instance that maxLockableGpuIns (%d)\n",
            highestInstanceInGpuMask, rmGpuLockInfo.maxLockableGpuInst);
    }

    //
    // When we release locks we do it in reverse order.
    //
    // In this pass we check to see how many of the locks we are to release
    // have something waiting.  If any of them do, then we queue up a DPC
    // to handle the release of all of them.
    //
    for (gpuInst = highestInstanceInGpuMask;
         gpuInst != (NvU32)-1;
         gpuInst--)
    {
        if ((gpuMask & NVBIT(gpuInst)) == 0)
            continue;

        pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

        if (pGpuLock->count < 0)
        {
            if (!pGpuLock->bSignaled)
            {
                gpuMaskWakeup |= NVBIT(gpuInst);
                if (bSemaCanWake)
                    pGpuLock->bSignaled = NV_TRUE;
            }
        }
    }

    //
    // Check to see if we can safely release the locks.
    //
    // We can do this if there are no threads waiting on any of them
    // or if we're running at a processor level that will allow us to
    // wake any such threads.
    //
    // Put another way, the only time we *cannot* do this is when something
    // is waiting and we are running at an elevated processor level (i.e.
    // we're here from a call in our ISR).
    //
    if (gpuMaskWakeup == 0 || bSemaCanWake)
    {
        for (gpuInst = highestInstanceInGpuMask;
             gpuInst != (NvU32)-1;
             gpuInst--)
        {
            if ((gpuMask & NVBIT(gpuInst)) == 0)
                continue;

            pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

            // now enable interrupts
            _gpuLocksReleaseEnableInterrupts(gpuInst, flags);

            // indicate that the API is not running
            NV_ASSERT(pGpuLock->threadId == threadId);
            pGpuLock->bRunning = NV_FALSE;
            pGpuLock->threadId = ~(NvU64)0;

            portAtomicIncrementS32(&pGpuLock->count);
            NV_ASSERT(pGpuLock->count <= 1);

            // update gpusLockedMask
            rmGpuLockInfo.gpusLockedMask &= ~NVBIT(gpuInst);

            // add release record to GPUs lock trace
            osGetCurrentTick(&timestamp);
            INSERT_LOCK_TRACE(&rmGpuLockInfo.traceInfo,
                              ra,
                              lockTraceRelease,
                              gpuInst, gpuMask,
                              threadId,
                              bHighIrql,
                              (NvU8)priority,
                              timestamp);
        }
    }

    RMTRACE_RMLOCK(_GPUS_LOCK_RELEASE);

    portSyncSpinlockRelease(rmGpuLockInfo.pLock);

    if (bSemaCanWake)
    {
        NvU32 extraWakeUp;
        do { extraWakeUp = bug200413011_WAR_WakeUpMask; }
        while (!portAtomicCompareAndSwapU32(&bug200413011_WAR_WakeUpMask, 0, extraWakeUp));
        gpuMaskWakeup |= extraWakeUp;
    }

    //
    // Handle wake up(s).
    //
    if (gpuMaskWakeup != 0)
    {
        if (bSemaCanWake)
        {
            for (gpuInst = highestInstanceInGpuMask;
                 gpuInst != (NvU32)-1;
                 gpuInst--)
            {
                if ((gpuMaskWakeup & NVBIT(gpuInst)) == 0)
                    continue;

                pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];
                if (pGpuLock->pWaitSema)
                    portSyncSemaphoreRelease(pGpuLock->pWaitSema);
                else
                    DBG_BREAKPOINT();
            }
            status = NV_SEMA_RELEASE_NOTIFIED;
            goto done;
        }
        else
        {
            if (pDpcGpu == NULL)
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Releasing GPU locks (mask:0x%08x) at raised IRQL without a DPC GPU at %p. Attempting to recover..\n",
                    gpuMask, ra);
                portAtomicOrU32(&bug200413011_WAR_WakeUpMask, gpuMaskWakeup);
                status = NV_SEMA_RELEASE_FAILED;
                goto done;
            }
            // Use a dpc to release the locks.
            NV_ASSERT((gpuMask == gpumgrGetGpuMask(pDpcGpu)) ||
                      (gpuMask == rmGpuLockInfo.gpusLockedMask));
            if (gpuMask == gpumgrGetGpuMask(pDpcGpu))
            {
                status = osGpuLocksQueueRelease(pDpcGpu, DPC_RELEASE_SINGLE_GPU_LOCK);
                goto done;
            }
            else
            {
                status = osGpuLocksQueueRelease(pDpcGpu, DPC_RELEASE_ALL_GPU_LOCKS);
                goto done;
            }
        }
    }

    status = NV_SEMA_RELEASE_SUCCEED;

done:
    threadPriorityRestore();

    return status;
}

//
// rmGpuLocksRelease
// NOTE: This function assumes ALL GPUs were previously locked by rmGpuLocksAcquire
// Under this assumption, it is accessing gpusLockedMask and gpusLockableMask, as
// these cannot changed if all GPUs are already locked.
// If any GPU is not locked (due to API misuse most likely), this function is
// open to various race conditions.
//
NvU32
rmGpuLocksRelease(NvU32 flags, OBJGPU *pDpcGpu)
{
    NvU32 gpuMask;
    NvU32 rc;
    CALL_CONTEXT *pCallContext;

    //
    // It's possible that we're asked to release locks when there are none.
    // This can happen when a release attempt happens and the GPU(s) in the
    // mask have been hidden with rmGpuLockHide().  In such cases we won't
    // even bother trying to do the release.
    //
    if (rmGpuLockInfo.gpusLockedMask == 0)
        return NV_SEMA_RELEASE_SUCCEED;

    // Only attempt to release the ones that are both locked and lockable
    gpuMask = rmGpuLockInfo.gpusLockedMask & rmGpuLockInfo.gpusLockableMask;

    if (gpuMask == 0)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Attempting to release nonlockable GPUs. gpuMask = 0x%08x, gpusLockableMask = 0x%08x\n",
                  gpuMask, rmGpuLockInfo.gpusLockableMask);
        return NV_SEMA_RELEASE_FAILED;
    }
    rc = _rmGpuLocksRelease(gpuMask, flags, pDpcGpu, NV_RETURN_ADDRESS());

    pCallContext = resservGetTlsCallContext();
    if (pCallContext != NULL)
    {
        NV_ASSERT(pCallContext->pLockInfo != NULL);
        if (pCallContext->pLockInfo != NULL)
            pCallContext->pLockInfo->state &= ~RM_LOCK_STATES_GPUS_LOCK_ACQUIRED;
    }

    return rc;
}

//
// rmGpuLocksFreeze: Freezes locks for those GPUs specified in the mask
//
void
rmGpuLocksFreeze(GPU_MASK gpuMask)
{
    rmGpuLockInfo.gpusFreezeMask |= gpuMask;
}

//
// rmGpuLocksUnfreeze: Unfreezes locks for those GPUs specified in the mask
//
void
rmGpuLocksUnfreeze(GPU_MASK gpuMask)
{
    rmGpuLockInfo.gpusFreezeMask &= ~gpuMask;
}

//
// rmGpuGroupLockRelease: Releases lock for only those gpus specified in the mask
//
NV_STATUS
rmGpuGroupLockRelease(GPU_MASK gpuMask, NvU32 flags)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pDpcGpu = NULL;

    if (gpuMask == 0)
        return NV_SEMA_RELEASE_SUCCEED;

    //
    // QuadroSync (previously known as GSync) is a cross GPU feature that
    // synchronizes display across multiple GPUs.  See changelist 16809243.  If
    // GSync is enabled, acquire locks for all GPUs.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED))
    {
        // See note for rmGpuLocksRelease() - assumes locks are actually held.
        gpuMask = rmGpuLockInfo.gpusLockedMask;

        pDpcGpu = gpumgrGetGpu(portUtilCountTrailingZeros32(gpuMask));
    }

    return _rmGpuLocksRelease(gpuMask, flags, pDpcGpu, NV_RETURN_ADDRESS());
}

//
// rmDeviceGpuLocksRelease
//
NvU32
rmDeviceGpuLocksRelease(OBJGPU *pGpu, NvU32 flags, OBJGPU *pDpcGpu)
{
    NvU32 gpuMask;
    NvU32 rc;
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    CALL_CONTEXT *pCallContext;

    //
    // QuadroSync (previously known as GSync) is a cross GPU feature that
    // synchronizes display across multiple GPUs.  See changelist 16809243.  If
    // GSync is enabled, acquire locks for all GPUs.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED))
    {
        // See note for rmGpuLocksRelease() - assumes locks are actually held.
        gpuMask = rmGpuLockInfo.gpusLockedMask;
    }
    else
    {
        gpuMask = gpumgrGetGpuMask(pGpu);
    }

    if (gpuMask == 0)
    {
        return NV_SEMA_RELEASE_SUCCEED;
    }

    rc = _rmGpuLocksRelease(gpuMask, flags, pDpcGpu, NV_RETURN_ADDRESS());

    pCallContext = resservGetTlsCallContext();
    if (pCallContext != NULL)
    {
        NV_ASSERT(pCallContext->pLockInfo != NULL);
        if (pCallContext->pLockInfo != NULL)
            pCallContext->pLockInfo->state &= ~RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED;
    }

    return rc;
}

//
// rmGpuLockHide
//
// Hide the given gpuMask from the GPU lock acquire and release
//
NV_STATUS
rmGpuLockHide(NvU32 gpuMask)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuInst = 0;
    Intr *pIntr;

    // We should not be getting intrs on a Hidden GPU
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInst)) != NULL)
    {
        // XXX - return failure for this?
        pIntr = GPU_GET_INTR(pGpu);
        NV_ASSERT(intrGetIntrEnFromHw_HAL(pGpu, pIntr, NULL) == INTERRUPT_TYPE_DISABLED);
    }

    portAtomicOrU32(&rmGpuLockInfo.gpusHiddenMask, gpuMask);

    return NV_OK;
}

//
// rmGpuLockShow
//
// Allow the given gpuMask to be acted upon by the GPU lock acquire and release
//
void
rmGpuLockShow(NvU32 gpuMask)
{
    portAtomicAndU32(&rmGpuLockInfo.gpusHiddenMask, ~gpuMask);
}

NvBool rmGpuLockIsHidden(OBJGPU *pGpu)
{
    return ((rmGpuLockInfo.gpusHiddenMask & NVBIT(pGpu->gpuInstance)) != 0);
}

//
// _rmGpuLockIsOwner
//
// Returns NV_TRUE if calling thread currently owns specified set of locks.
//
static NvBool
_rmGpuLockIsOwner(NvU32 gpuMask)
{
    NvU32 gpuInst;
    OS_THREAD_HANDLE threadId;
    OS_THREAD_HANDLE lockedThreadId;
    NvU32 highestInstanceInGpuMask;

    if (gpuMask == 0)
        return NV_FALSE;

    osGetCurrentThread(&threadId);

    // Can't use rmGpuLockInfo.maxLockableGpuInst since we may not hold the lock
    highestInstanceInGpuMask = 31 - portUtilCountLeadingZeros32(gpuMask);
    for (gpuInst = portUtilCountTrailingZeros32(gpuMask);
         gpuInst <= highestInstanceInGpuMask;
         gpuInst++)
    {
        // skip any not lockable
        if ((gpuMask & NVBIT(gpuInst)) == 0)
            continue;

        lockedThreadId = rmGpuLockInfo.gpuLocks[gpuInst].threadId;
        if (lockedThreadId != threadId)
            return NV_FALSE;
    }

    return NV_TRUE;
}

//
// rmGpuLockIsOwner
//
// Returns NV_TRUE if calling thread currently owns all lockable GPUs.
//
NvBool
rmGpuLockIsOwner(void)
{
    return (_rmGpuLockIsOwner(rmGpuLockInfo.gpusLockableMask));
}

//
// rmGpuLocksGetOwnedMask
//
// Returns mask of locks currently owned by the calling thread.
//
NvU32
rmGpuLocksGetOwnedMask(void)
{
    NvU32 gpuMask = 0;
    NvU32 gpuInst;
    OS_THREAD_HANDLE threadId;
    OS_THREAD_HANDLE lockedThreadId;
    NvU32 lockableMask;
    NvU32 highestInstanceInGpuMask;

    lockableMask = rmGpuLockInfo.gpusLockableMask;
    if (lockableMask == 0)
        return 0;

    osGetCurrentThread(&threadId);

    // Can't use rmGpuLockInfo.maxLockableGpuInst since we may not hold the lock
    highestInstanceInGpuMask = 31 - portUtilCountLeadingZeros32(lockableMask);
    for (gpuInst = portUtilCountTrailingZeros32(lockableMask);
         gpuInst <= highestInstanceInGpuMask;
         gpuInst++)
    {
        // skip any not lockable
        if ((lockableMask & NVBIT(gpuInst)) == 0)
            continue;

        lockedThreadId = rmGpuLockInfo.gpuLocks[gpuInst].threadId;
        if (lockedThreadId == threadId)
            gpuMask |= NVBIT(gpuInst);
    }
    return gpuMask;
}

//
// rmDeviceGpuLockIsOwner
//

NvBool
rmDeviceGpuLockIsOwner(NvU32 gpuInst)
{
    return (_rmGpuLockIsOwner(gpumgrGetGrpMaskFromGpuInst(gpuInst)));
}

//
// rmGpuLockSetOwner
//
// The GPUs lock is acquired in the ISR, then released in the DPC.
// Since the threadId changes between the ISR and DPC, we need to
// refresh the owning thread in the DPC to be accurate.
//
// NOTE: Assumes all GPU locks are owned by a thread that is no longer using them
// (or no longer exists).
//
NV_STATUS
rmGpuLockSetOwner(OS_THREAD_HANDLE threadId)
{
    GPULOCK *pGpuLock;
    NvU32 gpuInst;
    NvU32 maxLockableGpuInst;

    maxLockableGpuInst = rmGpuLockInfo.maxLockableGpuInst;
    if (maxLockableGpuInst >= NV_MAX_DEVICES)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    for (gpuInst = 0; gpuInst <= maxLockableGpuInst; gpuInst++)
    {
        // skip any not lockable
        if ((rmGpuLockInfo.gpusLockableMask & NVBIT(gpuInst)) == 0)
            continue;

        pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

        if (threadId != GPUS_LOCK_OWNER_PENDING_DPC_REFRESH)
        {
            NV_ASSERT_OR_RETURN(pGpuLock->threadId == GPUS_LOCK_OWNER_PENDING_DPC_REFRESH, NV_ERR_INVALID_STATE);
        }
        pGpuLock->threadId = threadId;
    }

    return NV_OK;
}

//
// rmDeviceGpuLockSetOwner
//
// NOTE: Assumes the locks in question are owned by a thread that is no longer
// using them (or no longer exists). Also assumes that at least one lock belongs
// to pGpu.

NV_STATUS
rmDeviceGpuLockSetOwner(OBJGPU *pGpu, OS_THREAD_HANDLE threadId)
{
    GPULOCK *pGpuLock;
    NvU32 gpuInst;
    NvU32 gpuMask;
    NvU32 maxLockableGpuInst;
    OBJSYS    *pSys = SYS_GET_INSTANCE();

    if (pSys->getProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED))
    {
        gpuMask = rmGpuLockInfo.gpusLockedMask;
    }
    else
    {
        gpuMask = gpumgrGetGpuMask(pGpu);
    }

    maxLockableGpuInst = rmGpuLockInfo.maxLockableGpuInst;
    if (maxLockableGpuInst >= NV_MAX_DEVICES)
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    for (gpuInst = 0; gpuInst <= maxLockableGpuInst; gpuInst++)
    {
        // skip any not lockable
        if ((gpuMask & NVBIT(gpuInst)) == 0)
            continue;

        pGpuLock = &rmGpuLockInfo.gpuLocks[gpuInst];

        if (threadId != GPUS_LOCK_OWNER_PENDING_DPC_REFRESH)
        {
            NV_ASSERT_OR_RETURN(pGpuLock->threadId == GPUS_LOCK_OWNER_PENDING_DPC_REFRESH, NV_ERR_INVALID_STATE);
        }
        pGpuLock->threadId = threadId;
    }

    return NV_OK;
}

//
// WAR for bug 200288016: In some cases due to GSYNC updates a worker thread
// does not release all the locks it acquired. This is an attempt at recovery.
//
void bug200288016_WAR_ReleaseAllOwnedLocked(void)
{
    NvU32 gpuInst;
    NvU32 gpuMask = 0;
    OS_THREAD_HANDLE threadId;

    osGetCurrentThread(&threadId);
    for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
    {
        if (rmGpuLockInfo.gpuLocks[gpuInst].threadId == threadId)
            gpuMask |= NVBIT(gpuInst);
    }

    if (gpuMask != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Worker thread finished without releasing all locks. gpuMask=%x\n",
                  gpuMask);
        _rmGpuLocksRelease(gpuMask, GPUS_LOCK_FLAGS_NONE, NULL, NV_RETURN_ADDRESS());
    }
}



//
// IntrMask Locking Support
//
typedef struct INTR_MASK_LOCK
{
    PORT_SPINLOCK *pLock;
} INTR_MASK_LOCK;

static INTR_MASK_LOCK intrMaskLock[NV_MAX_DEVICES];

NV_STATUS rmIntrMaskLockAlloc(NvU32 gpuInst)
{
    intrMaskLock[gpuInst].pLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    return (intrMaskLock[gpuInst].pLock == NULL) ? NV_ERR_INSUFFICIENT_RESOURCES : NV_OK;
}

void rmIntrMaskLockFree(NvU32 gpuInst)
{
    if (intrMaskLock[gpuInst].pLock != NULL)
        portSyncSpinlockDestroy(intrMaskLock[gpuInst].pLock);
}

NvU64 rmIntrMaskLockAcquire(OBJGPU *pGpu)
{
    if ((pGpu != NULL) && (intrMaskLock[pGpu->gpuInstance].pLock != NULL))
        portSyncSpinlockAcquire(intrMaskLock[pGpu->gpuInstance].pLock);
    return 0;
}

void rmIntrMaskLockRelease(OBJGPU *pGpu, NvU64 oldIrql)
{
    if ((pGpu != NULL) && (intrMaskLock[pGpu->gpuInstance].pLock != NULL))
        portSyncSpinlockRelease(intrMaskLock[pGpu->gpuInstance].pLock);
}

