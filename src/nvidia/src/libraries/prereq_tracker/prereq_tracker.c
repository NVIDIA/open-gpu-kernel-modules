/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/* ------------------------ Includes --------------------------------------- */
#include "prereq_tracker/prereq_tracker.h"
#include "os/os.h"
#include "gpu/gpu_access.h"

/* ------------------------ Static Function Prototypes --------------------- */
static NvBool    _prereqValid(PrereqTracker *pTracker, PREREQ_ENTRY *pPrereq);

static NV_STATUS  _prereqComposeEntryHelper(PrereqTracker *pTracker,
    GpuPrereqCallback *callback, PREREQ_ID_BIT_VECTOR *pDepends,
    PREREQ_ENTRY **ppPrereq, NvBool bDeferrable);

static void _prereqTracker_WORKITEM(NvU32 gpuInstance, void *pArgs);

static NV_STATUS _prereqDeferCallback(PrereqTracker *pTracker, PREREQ_ENTRY  *pPrereqEntry);

/* ------------------------ Datatypes -------------------------------------- */
/*!
 * @brief Arguments for @ref _prereqTracker_WORKITEM
 *
 * @param[in]     pTracker    PrereqTracker object pointer
 * @param[in/out] pPrereqEntry Prerequisite entry object pointer
 */
typedef struct
{
    PrereqTracker *pTracker;
    PREREQ_ENTRY *pPrereqEntry;
} PrereqDeferredWorkItemArgs;

/* ------------------------ Public Functions  ------------------------------ */

/*!
 * @brief   Construct the prereq tracker object
 *
 * @param[in]   pTracker    PrereqTracker object to be constructed
 * @param[in]   pParent     Parent GPU passed into the first parameter of callbacks
 *
 * @return  NV_OK   Successfully constructed tracker
 * @return  NV_ERR_INVALID_STATE If already constructed
 */
NV_STATUS
prereqConstruct_IMPL
(
    PrereqTracker *pTracker,
    OBJGPU        *pParent,
    GpuPrereqDeferralCheckCallback *pCallback
)
{
    NV_ASSERT_OR_RETURN(!pTracker->bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    bitVectorClrAll(&pTracker->satisfied);

    listInit(&pTracker->prereqList, portMemAllocatorGetGlobalNonPaged());
    pTracker->bInitialized = NV_TRUE;
    pTracker->pParent = pParent;
    pTracker->pDeferralCheckCallback = pCallback;

    return NV_OK;
}

/*!
 * @brief   Destroys the prerequisite tracker object
 *
 * @param[in]   pTracker    PrereqTracker object to be destroyed
 */
void
prereqDestruct_IMPL
(
    PrereqTracker *pTracker
)
{
    NV_ASSERT_OR_RETURN_VOID(pTracker->bInitialized);

    listDestroy(&pTracker->prereqList);
    pTracker->bInitialized = NV_FALSE;
}

/*!
 * @brief Arms a tracking structure to fire the callback when all prerequisites
 * are satisfied.  May only be called after all prerequisites are specified.  No
 * more prerequisites may be specified after arming.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   pPrereq     PREREQ_ENTRY object pointer
 *
 * @return  NV_OK   Prerequisite successfully armed.
 * @return  error   Errors propagated up from functions called.
 */
static NV_STATUS
_prereqArm
(
    PrereqTracker *pTracker,
    PREREQ_ENTRY  *pPrereq
)
{
    PREREQ_ID_BIT_VECTOR requestedAndSatisfied;

    NV_ASSERT_OR_RETURN(pTracker->bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(_prereqValid(pTracker, pPrereq), NV_ERR_INVALID_OBJECT);
    NV_ASSERT_OR_RETURN(!pPrereq->bArmed, NV_ERR_INVALID_STATE);

    //
    // Set the PREREQ_ENTRY state to bArmed.  No more PREREQ_IDs may be added
    // after this point.
    //
    pPrereq->bArmed = NV_TRUE;

    //
    // Put together a mask of PREREQ_IDs which are both satisfied and requested
    // We do not keep track of satisfied prereqs until armed, so we have no existing
    // state to worry about here.
    //
    NV_ASSERT_OK_OR_RETURN(bitVectorAnd(&requestedAndSatisfied,
                                        &pPrereq->requested,
                                        &pTracker->satisfied));

    pPrereq->countSatisfied = bitVectorCountSetBits(&requestedAndSatisfied);

    if (PREREQ_IS_SATISFIED(pPrereq))
    {
        if (pTracker->pDeferralCheckCallback(pTracker, pPrereq))
        {
            NV_PRINTF(LEVEL_INFO, "Deferring callback while arming! pCallback: %p \n",
                pPrereq->callback);
            NV_ASSERT_OK_OR_RETURN(_prereqDeferCallback(pTracker, pPrereq));
        }
        else
        {
            NV_ASSERT_OK_OR_RETURN(pPrereq->callback(pTracker->pParent, NV_TRUE));
        }
    }

    return NV_OK;
}

/*!
 * @brief Creates, adds IDs to, and Arms a prereq tracking structure into the list.
 * Simple wrapper around _prereqComposeEntryHelper that sets bDeferrable to NV_FALSE.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   callback    Callback function pointer
 *                          First parameter passed will be NVOC parent of pTracker
 * @param[in]   pDepends    Bitvector of prerequisite IDs to add as requirement
 * @param[out]  ppPrereq    PREREQ_ENTRY object pointer created, or NULL if not desired
 *
 * @return  NV_OK   Prerequisite successfully armed.
 * @return  error   Errors propagated up from functions called.
 */
NV_STATUS
prereqComposeEntry_IMPL
(
    PrereqTracker        *pTracker,
    GpuPrereqCallback    *callback,
    PREREQ_ID_BIT_VECTOR *pDepends,
    PREREQ_ENTRY        **ppPrereq
)
{
    NV_ASSERT_OK_OR_RETURN(_prereqComposeEntryHelper(pTracker, callback,
        pDepends, ppPrereq, NV_FALSE));

    return NV_OK;
}

/*!
 * @brief Creates, adds IDs to, and Arms a prereq tracking structure into the list.
 * Simple wrapper around _prereqComposeEntryHelper that sets bDeferrable to NV_TRUE.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   callback    Callback function pointer
 *                          First parameter passed will be NVOC parent of pTracker
 * @param[in]   pDepends    Bitvector of prerequisite IDs to add as requirement
 * @param[out]  ppPrereq    PREREQ_ENTRY object pointer created, or NULL if not desired
 *
 * @return  NV_OK   Prerequisite successfully armed.
 * @return  error   Errors propagated up from functions called.
 */
NV_STATUS
prereqComposeEntryDeferrable_IMPL
(
    PrereqTracker        *pTracker,
    GpuPrereqCallback    *callback,
    PREREQ_ID_BIT_VECTOR *pDepends,
    PREREQ_ENTRY        **ppPrereq
)
{
    NV_ASSERT_OK_OR_RETURN(_prereqComposeEntryHelper(pTracker, callback,
        pDepends, ppPrereq, NV_TRUE));

    return NV_OK;
}

/*!
 * @brief   Notifies that prerequisite was satisfied.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   prereqId    Prerequisite ID to add as requirement
 *
 * @return  NV_OK   Prerequisite successfully satisfied & all callbacks passed.
 * @return  error   Errors propagated up from functions called.
 */
NV_STATUS
prereqSatisfy_IMPL
(
    PrereqTracker *pTracker,
    PREREQ_ID      prereqId
)
{
    PREREQ_ENTRY *pPrereq;
    PrereqListIter it;

    NV_ASSERT_OR_RETURN(pTracker->bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((prereqId < PREREQ_ID_VECTOR_SIZE),
                        NV_ERR_INVALID_REQUEST);

    //
    // The prerequisite can be satisfied only once. An attempt to satisfy
    // the prerequisite multiple times should indicate bad code design.
    //
    NV_ASSERT_OR_RETURN(!bitVectorTest(&pTracker->satisfied, prereqId),
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(bitVectorSet(&pTracker->satisfied, prereqId));

    // Broadcast satisfaction of this PREREQ_ID to all armed PREREQ_ENTRY.
    it = listIterAll(&pTracker->prereqList);
    while (listIterNext(&it))
    {
        pPrereq = it.pValue;
        if (pPrereq->bArmed &&
            bitVectorTest(&pPrereq->requested, prereqId))
        {
            pPrereq->countSatisfied++;
            NV_ASSERT_OR_RETURN(pPrereq->countSatisfied <= pPrereq->countRequested,
                                NV_ERR_INVALID_STATE);

            if (PREREQ_IS_SATISFIED(pPrereq))
            {
                if (pTracker->pDeferralCheckCallback(pTracker, pPrereq))
                {
                    NV_PRINTF(LEVEL_INFO, "Deferring callback on satisfy! pCallback: %p \n",
                              pPrereq->callback);
                    NV_ASSERT_OK_OR_RETURN(_prereqDeferCallback(pTracker, pPrereq));
                }
                else
                {
                    NV_ASSERT_OK_OR_RETURN(pPrereq->callback(pTracker->pParent, NV_TRUE));
                }
            }
        }
    }

    return NV_OK;
}

/*!
 * @brief   Notifies that prerequisite will be retracted.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   prereqId    Prerequisite ID to add as requirement
 *
 * @return  NV_OK   Prerequisite successfully retracted & all callbacks passed.
 * @return  error   Errors propagated up from functions called.
 */
NV_STATUS
prereqRetract_IMPL
(
    PrereqTracker *pTracker,
    PREREQ_ID      prereqId
)
{
    PREREQ_ENTRY *pNode;
    PrereqListIter it;
    NV_STATUS   status = NV_OK;

    NV_ASSERT_OR_RETURN(pTracker != NULL,
                      NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pTracker->bInitialized,
                      NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((prereqId < PREREQ_ID_VECTOR_SIZE),
                      NV_ERR_INVALID_REQUEST);

    //
    // The prerequisite can be retracted even if it was not satisfied. This
    // simplifies client code since it no longer need to track if prerequisite
    // was satisfied (or not) and allows us avoiding isSatisfied() interface.
    //
    if (!bitVectorTest(&pTracker->satisfied, prereqId))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(bitVectorClr(&pTracker->satisfied, prereqId));

    it = listIterAll(&pTracker->prereqList);
    while (listIterNext(&it))
    {
        pNode = it.pValue;

        if (pNode->bArmed &&
            bitVectorTest(&pNode->requested, prereqId))
        {
            if (PREREQ_IS_SATISFIED(pNode))
            {
                if (pTracker->pDeferralCheckCallback(pTracker, pNode))
                {
                    NV_PRINTF(LEVEL_ERROR, "Deferring callback for retract! pCallback: %p\n",
                        pNode->callback);
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status, _prereqDeferCallback(pTracker, pNode));
                }
                else
                {
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status, pNode->callback(pTracker->pParent, NV_FALSE));
                }
            }

            pNode->countSatisfied--;
            if (pNode->countSatisfied < 0)
            {
                NV_ASSERT(0);
                if (status == NV_OK)
                {
                    status = NV_ERR_INVALID_STATE;
                }
            }
        }
    }

    return status;
}

/*!
 * @brief   Indicates if a prerequisite ID is currently satisfied.
 *
 * @param[in]   pTracker    PrereqTracker object pointer
 * @param[in]   prereqId    Prerequisite ID to check
 *
 * @return  NV_TRUE Prerequisite ID is in the satisfied mask.
 *          NV_FALSE otherwise
 */
NvBool
prereqIdIsSatisfied_IMPL
(
    PrereqTracker  *pTracker,
    PREREQ_ID       prereqId
)
{
    NvBool bIsSatisfied;

    if ((pTracker->bInitialized) &&
        (prereqId < PREREQ_ID_VECTOR_SIZE))
    {
        bIsSatisfied = bitVectorTest(&pTracker->satisfied, prereqId);
    }
    else
    {
        bIsSatisfied = NV_FALSE;
    }

    return bIsSatisfied;
}

/* ---------------------- Private Static Functions -------------------------- */
/*!
 * Helper function which determines whether a given PREREQ_ENTRY tracking
 * structure is valid (i.e. is in the tracker's list at @ref
 * PrereqTracker::prereqList).
 *
 * @param[in]   pTracker    PrereqTracker object pointer
 * @param[in]   pPrereq     PREREQ_ENTRY object pointer
 *
 * @return NV_TRUE    pPrereq is valid.
 * @return NV_FALSE   pPrereq is invalid.
 */
static NvBool
_prereqValid
(
    PrereqTracker  *pTracker,
    PREREQ_ENTRY   *pPrereq
)
{
    PrereqListIter it = listIterAll(&pTracker->prereqList);
    while (listIterNext(&it))
    {
        // pPrereq is valid if found in the list.
        if (it.pValue == pPrereq)
            return NV_TRUE;
    }

    return NV_FALSE;
}

/*!
 * @brief Creates, adds IDs to, and Arms a prereq tracking structure into the list.
 * Caller gives up all control of the prereq structure to the prereq tracker, which
 * will take care of storing the completed, final struct and freeing it once done.
 *
 * @param[in]   pTracker    PrereqTracker object
 * @param[in]   callback    Callback function pointer
 *                          First parameter passed will be NVOC parent of pTracker
 * @param[in]   pDepends    Bitvector of prerequisite IDs to add as requirement
 * @param[out]  ppPrereq    PREREQ_ENTRY object pointer created, or NULL if not desired
 * @param[in]   bDeferrable Boolean indicating whether the callback can be deferred
 *
 * @return  NV_OK   Prerequisite successfully armed.
 * @return  error   Errors propagated up from functions called.
 */
NV_STATUS
_prereqComposeEntryHelper
(
    PrereqTracker        *pTracker,
    GpuPrereqCallback    *callback,
    PREREQ_ID_BIT_VECTOR *pDepends,
    PREREQ_ENTRY        **ppPrereq,
    NvBool                bDeferrable
)
{
    PREREQ_ENTRY *pPrereq;

    NV_ASSERT_OR_RETURN(pTracker->bInitialized, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(callback != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pDepends != NULL, NV_ERR_INVALID_POINTER);

    pPrereq = listAppendNew(&pTracker->prereqList);
    NV_ASSERT_OR_RETURN(pPrereq != NULL, NV_ERR_NO_MEMORY);

    NV_ASSERT_OK_OR_RETURN(bitVectorCopy(&pPrereq->requested, pDepends));

    pPrereq->countRequested = bitVectorCountSetBits(&pPrereq->requested);
    pPrereq->countSatisfied = 0;
    pPrereq->callback       = callback;

    //
    // bLastCallbackWasSatisfy gets initialized to NV_FALSE, since the first
    // action we can take on a PREREQ_ENTRY is to satisfy it.
    //
    pPrereq->bLastCallbackWasSatisfy = NV_FALSE;
    pPrereq->bDeferrable    = bDeferrable;
    pPrereq->bWorkItemScheduled = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(_prereqArm(pTracker, pPrereq));

    if (ppPrereq != NULL)
        *ppPrereq = pPrereq;

    return NV_OK;
}

/*!
 * @brief Defers a callback by scheduling a work-item to execute it.
 *
 * @param[in]   pTracker    PrereqTracker object pointer
 * @param[in]   pPrereqEntry PREREQ_ENTRY object pointer
 *
 * @return NV_OK   Callback successfully deferred.
 * @return error   Errors propagated up from functions called.
 */
static NV_STATUS
_prereqDeferCallback
(
    PrereqTracker *pTracker,
    PREREQ_ENTRY  *pPrereqEntry
)
{
    NV_STATUS status;
    PrereqDeferredWorkItemArgs *pWorkItemArgs = NULL;

    if (pPrereqEntry->bWorkItemScheduled)
    {
        status = NV_OK;
        goto _prereqDeferCallback_exit;
    }

    pWorkItemArgs = portMemAllocNonPaged(sizeof(*pWorkItemArgs));
    NV_ASSERT_TRUE_OR_GOTO(status,
        pWorkItemArgs != NULL,
        NV_ERR_NO_MEMORY,
        _prereqDeferCallback_exit);

    pWorkItemArgs->pTracker = pTracker;
    pWorkItemArgs->pPrereqEntry = pPrereqEntry;

    NV_ASSERT_OK_OR_GOTO(status,
        osQueueWorkItem(pTracker->pParent,
                        _prereqTracker_WORKITEM,
                        pWorkItemArgs,
                        (OsQueueWorkItemFlags){
                            .bLockSema = NV_TRUE,
                            .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY,
                            .bLockGpuGroupSubdevice = NV_TRUE,
                            .bFullGpuSanity = NV_TRUE }),
        _prereqDeferCallback_exit);

    pPrereqEntry->bWorkItemScheduled = NV_TRUE;

_prereqDeferCallback_exit:
    if ((status != NV_OK) && (pWorkItemArgs != NULL))
    {
        portMemFree(pWorkItemArgs);
    }
    return status;
}

/*!
 * @brief Executes a deferred callback.
 * First checks if the conditions which led to scheduling the callback are still met.
 * If they aren't, this is a nop beyond resetting the bWorkItemScheduled flag.
 *
 * @param[in]   gpuInstance GPU instance
 * @param[in]   pArgs       Pointer to args @ref PrereqDeferredWorkItemArgs
 */
static void
_prereqTracker_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    PrereqDeferredWorkItemArgs *pWorkItemArgs = (PrereqDeferredWorkItemArgs *)pArgs;
    PrereqTracker *pTracker;
    PREREQ_ENTRY *pPrereq;

    NV_ASSERT_OR_RETURN_VOID(pWorkItemArgs != NULL);
    pTracker = pWorkItemArgs->pTracker;
    pPrereq = pWorkItemArgs->pPrereqEntry;

    pPrereq->bWorkItemScheduled = NV_FALSE;
    if (!pPrereq->bLastCallbackWasSatisfy)
    {
        // Work-item must have been scheduled to handle satisfy, double check that we still want that
        if (PREREQ_IS_SATISFIED(pPrereq))
        {
            pPrereq->bLastCallbackWasSatisfy = NV_TRUE;
            NV_PRINTF(LEVEL_INFO, "Running deferred callback (%p) for satisfy!\n",
                      pPrereq->callback);

            NV_ASSERT_OR_RETURN_VOID(pPrereq->callback(pTracker->pParent, NV_TRUE) == NV_OK);
        }
        else
        {
            NV_BITVECTOR_PRINT(
                NV_PRINTF(LEVEL_INFO, "Deferred callback (%p) no longer meets satisfy condition! countRequested: %d countSatisfied: %d Dependencies: \n",
                          pPrereq->callback, pPrereq->countRequested, pPrereq->countSatisfied),
                &pPrereq->requested);
        }
    }
    else
    {
        // Work-item must have been scheduled to retract, double check that we still want that
        if (!PREREQ_IS_SATISFIED(pPrereq))
        {
            NV_PRINTF(LEVEL_INFO, "Running deferred callback (%p) for retract!\n",
                      pPrereq->callback);
            pPrereq->bLastCallbackWasSatisfy = NV_FALSE;
            NV_ASSERT_OR_RETURN_VOID(pPrereq->callback(pTracker->pParent, NV_FALSE) == NV_OK);
        }
        else
        {
            NV_BITVECTOR_PRINT(
                NV_PRINTF(LEVEL_INFO, "Deferred callback (%p) no longer meets retract condition! countRequested: %d countSatisfied: %d Dependencies: \n",
                          pPrereq->callback, pPrereq->countRequested, pPrereq->countSatisfied),
                &pPrereq->requested);
        }
    }
}

