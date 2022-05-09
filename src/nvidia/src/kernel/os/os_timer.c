/*
 * SPDX-FileCopyrightText: Copyright (c) 2002-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief   This file contains platform-independent code for the 1 Hz OS timer.
 */

#include "objtmr.h"
#include "core/thread_state.h"
#include "core/locks.h"

static NvBool _os1HzCallbackIsOnList(OBJTMR *pTmr, OS1HZPROC callback, void *pData, NvU32 flags);
static NV_STATUS _os1HzCallback(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pTmrEvent);

/*!
 * @brief Initialize 1Hz callbacks
 *
 * Initialize the 1Hz callback list and create a timer event
 * (if using PTIMER events).
 *
 * @param[in,out]   pTmr        TMR object pointer
 */
NV_STATUS
osInit1HzCallbacks
(
    OBJTMR *pTmr
)
{
    NvU32   i;

    // Initialize the OS 1 Hz timer callback list.
    pTmr->pOs1HzCallbackList = NULL;
    pTmr->pOs1HzCallbackFreeList = pTmr->os1HzCallbackTable;

    // Fill in all the forward pointers in the callback table.
    for (i = 0; i < (TMR_NUM_CALLBACKS_OS - 1); i++)
    {
        pTmr->os1HzCallbackTable[i].next = &pTmr->os1HzCallbackTable[i+1];
    }
    pTmr->os1HzCallbackTable[i].next = NULL;

    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS))
    {
        NV_ASSERT_OK_OR_RETURN(tmrEventCreate(pTmr, &pTmr->pOs1HzEvent,
            _os1HzCallback, NULL, TMR_FLAG_RECUR));
    }

    return NV_OK;
}

/*!
 * @brief Destroy 1Hz callbacks
 *
 * Destroy the 1Hz callback list and free the timer event
 * (if using PTIMER events).
 *
 * @param[in,out]   pTmr        TMR object pointer
 */
NV_STATUS
osDestroy1HzCallbacks
(
    OBJTMR *pTmr
)
{
    if (pTmr->pOs1HzEvent != NULL)
    {
        tmrEventCancel(pTmr, pTmr->pOs1HzEvent);
        tmrEventDestroy(pTmr, pTmr->pOs1HzEvent);
        pTmr->pOs1HzEvent = NULL;
    }

    pTmr->pOs1HzCallbackList = NULL;
    pTmr->pOs1HzCallbackFreeList = NULL;
    return NV_OK;
}

/*!
 * @brief Timer function to insert 1Hz callback to the list.
 *
 * This function is used to insert/register the 1Hz callback to the callback list.
 *
 * @param[in,out]   pTmr        TMR object pointer
 * @param[in]       callback    OS1HZPROC callback function point
 * @param[in]       pData       Unique identifier for the callback
 * @param[in]       flags       Callback flags
 *
 * @return  NV_OK                   The callback has been added
 * @return  NV_ERR_INVALID_REQUEST  The callback has not been added
 */
NV_STATUS
osSchedule1SecondCallback
(
    OBJGPU     *pGpu,
    OS1HZPROC   callback,
    void       *pData,
    NvU32       flags
)
{
    OBJTMR             *pTmr = GPU_GET_TIMER(pGpu);
    OS1HZTIMERENTRY    *pEntry;
    NV_STATUS           nvStatus = NV_OK;

    // Grab the next free timer entry.
    if ((pTmr->pOs1HzCallbackFreeList != NULL) &&
        !_os1HzCallbackIsOnList(pTmr, callback, pData, flags))
    {
        if ((pTmr->pOs1HzCallbackList == NULL) && (pTmr->pOs1HzEvent != NULL))
        {
            // First one.  Add 1Hz callback to timer events.
            NV_ASSERT_OK(tmrEventScheduleRelSec(pTmr, pTmr->pOs1HzEvent, 1));
        }

        pEntry = pTmr->pOs1HzCallbackFreeList;
        pTmr->pOs1HzCallbackFreeList = pEntry->next;

        pEntry->callback = callback;
        pEntry->data     = pData;
        pEntry->flags    = flags;

        pEntry->next     = pTmr->pOs1HzCallbackList;
        pTmr->pOs1HzCallbackList = pEntry;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Callback registration FAILED!\n");
        nvStatus = NV_ERR_INVALID_REQUEST;
    }

    return nvStatus;
}

/*!
 * @brief Timer function to remove 1Hz callback from the list.
 *
 * This function is used to remove/unregister the 1Hz callback from
 * the callback list.
 *
 * @param[in,out]   pTmr        TMR object pointer
 * @param[in]       callback    OS1HZPROC callback function point
 * @param[in]       pData       Unique identifier for the callback
 */
void
osRemove1SecondRepeatingCallback
(
    OBJGPU     *pGpu,
    OS1HZPROC   callback,
    void       *pData
)
{
    OBJTMR             *pTmr = GPU_GET_TIMER(pGpu);
    OS1HZTIMERENTRY    *pEntry;
    OS1HZTIMERENTRY   **ppEntryPtr;

    ppEntryPtr = &pTmr->pOs1HzCallbackList;
    while ((*ppEntryPtr) != NULL)
    {
        pEntry = *ppEntryPtr;
        if ((pEntry->callback == callback) &&
            (pEntry->data == pData) &&
            (pEntry->flags & NV_OS_1HZ_REPEAT))
        {
            *ppEntryPtr                  = pEntry->next;
            pEntry->next                 = pTmr->pOs1HzCallbackFreeList;
            pEntry->data                 = NULL;
            pEntry->callback             = NULL;
            pEntry->flags                = NV_OS_1HZ_REPEAT;
            pTmr->pOs1HzCallbackFreeList = pEntry;
            break;
        }
        ppEntryPtr = &pEntry->next;
    }

    if ((pTmr->pOs1HzCallbackList == NULL) && (pTmr->pOs1HzEvent != NULL))
    {
        // Last one.  Remove 1Hz callback from timer events.
        tmrEventCancel(pTmr, pTmr->pOs1HzEvent);
    }
}

//
// Return Value(TRUE) is used by Vista to determine if we were able to acquire the lock
// If we cannot acquire the lock this means the API or ISR/DPC has it
//
NvBool
osRun1HzCallbacksNow
(
    OBJGPU *pGpu
)
{
    OBJSYS             *pSys = SYS_GET_INSTANCE();
    OBJTMR             *pTmr = GPU_GET_TIMER(pGpu);
    OS1HZTIMERENTRY   **ppEntryPtr;
    OS1HZPROC           pProc;
    THREAD_STATE_NODE   threadState;
    void               *pData;
    NvBool              bAcquired = NV_TRUE;
    GPU_MASK            lockedGpus = 0;
#if !TLS_DPC_HAVE_UNIQUE_ID
    NvU8 stackAllocator[TLS_ISR_ALLOCATOR_SIZE]; // ISR allocations come from this buffer
    PORT_MEM_ALLOCATOR *pDpcAllocator;
    pDpcAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
    tlsIsrInit(pDpcAllocator);
#endif

    //
    // LOCK:
    //
    // What irql are we at here?  Should we acquire the API lock in addition to
    // or instead of the GPUs lock?
    //

    // LOCK: try to acquire GPU lock
    if (rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_TMR,
                &lockedGpus) != NV_OK)
    {
        // Out of conflicting thread
        bAcquired = NV_FALSE;
        goto exit;
    }

    if (osCondAcquireRmSema(pSys->pSema) != NV_OK)
    {
        // UNLOCK: release GPU lock
        rmGpuGroupLockRelease(lockedGpus, GPUS_LOCK_FLAGS_NONE);
        // Out of conflicting thread
        bAcquired = NV_FALSE;
        goto exit;
    }

    threadStateInitISRAndDeferredIntHandler(&threadState, pGpu,
        THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

    if (!gpuIsGpuFullPower(pGpu))
    {
        goto exit;
    }

    ppEntryPtr = &pTmr->pOs1HzCallbackList;
    for (;;)
    {
        POS1HZTIMERENTRY entry;

        // Be paranoid.
        entry = *ppEntryPtr;

        // End of list?
        if (entry == NULL)
            break;

        // Run the callback.
        if (entry->callback != NULL)
        {
            pProc = entry->callback;
            pData = entry->data;
            pProc(pGpu, pData);
        }

        //
        // The proc call above can add new entries to the list.
        // When new entries are added, they are added at the
        // beginning of the list.  That means that our *entryPtr
        // might no longer point to our current entry.  If that is
        // the case, then we need to search the list again to find
        // our entry.  Or inside this code, we need to find the
        // entryPtr over again.
        //
        if (entry != *ppEntryPtr)
        {
            POS1HZTIMERENTRY item;

            ppEntryPtr = &pTmr->pOs1HzCallbackList;
            for (;;)
            {
                // Be paranoid.
                item = *ppEntryPtr;

                // End of list?
                if (item == NULL)
                    break;

                if (item == entry)
                {
                    break;
                }

                ppEntryPtr = &item->next;
            }

            if (item != entry)
            {
                //
                // The entry was removed from the list inside the proc.
                // So, we don't need to do anything below.  Use
                // ppEntryPtr = NULL to indicate that for now.
                //
                ppEntryPtr = NULL;
            }

        }

        //
        // If this timer is supposed to repeat, leave it in place.
        // Otherwise, move it to the free list.
        //
        if ( (ppEntryPtr != NULL) &&
            !(entry->flags & NV_OS_1HZ_REPEAT))
        {
            *ppEntryPtr = entry->next;
            entry->next = pTmr->pOs1HzCallbackFreeList;
            pTmr->pOs1HzCallbackFreeList = entry;
        }
        else
        {
            ppEntryPtr = &entry->next;
        }
    }

exit:
    if (bAcquired)
    {
        // Out of conflicting thread
        threadStateFreeISRAndDeferredIntHandler(&threadState,
            pGpu, THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);
        osReleaseRmSema(pSys->pSema, NULL);
        // UNLOCK: release GPU lock
        rmGpuGroupLockRelease(lockedGpus, GPUS_LOCK_FLAGS_NONE);
    }

#if !TLS_DPC_HAVE_UNIQUE_ID
    tlsIsrDestroy(pDpcAllocator);
    portMemAllocatorRelease(pDpcAllocator);
#endif

    return bAcquired;
}

/*!
 * @brief Timer function to check the duplicate callback on the list.
 *
 * This function is used to check if there's any duplicate repeat callback has
 * been registered to the list, walk through the list and find if there's any
 * registered callback matched with flags NV_OS_1HZ_REPEAT.
 *
 * @param[in,out]   pTmr        TMR object pointer
 * @param[in]       callback    OS1HZPROC callback function point
 * @param[in]       pData       Unique identifier for the callback
 * @param[in]       flags       Callback flags
 *
 * @return          NV_TRUE     The callback has been registered
 * @return          NV_FALSE    The callback has not been registered
 */
static NvBool
_os1HzCallbackIsOnList
(
    OBJTMR    *pTmr,
    OS1HZPROC  callback,
    void      *pData,
    NvU32      flags
)
{
    POS1HZTIMERENTRY    pScan;

    for (pScan = pTmr->pOs1HzCallbackList; pScan != NULL; pScan = pScan->next)
    {
        if ((pScan->callback == callback) &&
            (pScan->data == pData) &&
            (pScan->flags & NV_OS_1HZ_REPEAT))
        {
            break;
        }
    }

    return pScan != NULL;
}

/*!
 * @brief Os 1Hz callback function
 *
 * Calls all callbacks on the 1Hz list and reschedules callback
 * (if using PTIMER events).
 *
 * @param[in,out]   pGpu        GPU object pointer
 * @param[in,out]   pTmr        TMR object pointer
 * @param[in]       pTmrEvent   pointer to the timer event
 *
 * @return  NV_OK                   The callback was rescheduled successfully.
 * @return  NV_ERR_INVALID_ARGUMENT The callback was not rescheduled.
 */
static NV_STATUS
_os1HzCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    osRun1HzCallbacksNow(pGpu);

    // TMR_FLAG_RECUR does not work, so reschedule it here.
    return tmrEventScheduleRelSec(pTmr, pTmrEvent, 1);
}
