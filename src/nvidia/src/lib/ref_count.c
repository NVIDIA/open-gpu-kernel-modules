/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/prelude.h"
#include "lib/ref_count.h"
#include "os/os.h"

static NV_STATUS _refcntReleaseReferences        (OBJREFCNT *, NvU64, NvBool,
                                                  NvBool, NvBool*);
static NV_STATUS _refcntInvokeStateChangeCallback(OBJREFCNT *, REFCNT_STATE,
                                                  REFCNT_STATE);
static void      _refcntInvokeResetCallback      (OBJREFCNT *, NvU64);

/*!
 * @brief Construct the reference counter according to the parameters.
 *
 * @param[in] pRefcnt               - REFCNT object pointer
 * @param[in] pParent               - The parent ODB object of the reference
 *                                    counter
 * @param[in] tag                   - An identifier the caller can use to
 *                                    distinguish multiple REFCNTs - the
 *                                    implementation does not use this value
 * @param[in] pStateChangeCallback  - A callback function to be called when
 *                                    the state of the reference-counted
 *                                    setting changes. Optional.
 * @param[in] pResetCallback        - A callback function to be called for
 *                                    each requester when the counter is
 *                                    reset. Optional.
 *
 * @returns NV_OK if the reference counter is constructed successfully
 */
NV_STATUS
refcntConstruct_IMPL
(
    OBJREFCNT                 *pRefcnt,
    Dynamic                   *pParent,
    NvU32                      tag,
    RefcntStateChangeCallback *pStateChangeCallback,
    RefcntResetCallback       *pResetCallback
)
{
    pRefcnt->pParent                     = pParent;
    pRefcnt->tag                         = tag;
    pRefcnt->refcntStateChangeCallback   = pStateChangeCallback;
    pRefcnt->refcntResetCallback         = pResetCallback;
    pRefcnt->state                       = REFCNT_STATE_DEFAULT;
    pRefcnt->count                       = 0;

    mapInit(&pRefcnt->requesterTree, portMemAllocatorGetGlobalNonPaged());

    return NV_OK;
}

/*!
 * @brief Destructor
 */
void
refcntDestruct_IMPL
(
    OBJREFCNT *pRefcnt
)
{

    // If there are any requesters left, remove them now
    mapDestroy(&pRefcnt->requesterTree);
}

/*!
 * @brief Request a state for the reference-counted setting on behalf of the
 *        requester.
 *
 * @param[in] pRefcnt         - REFCNT object pointer
 * @param[in] requesterId     - Unique identifier representing the requester
 * @param[in] requestedState  - The state of the setting that the requester
 *                              wants; should be either REFCNT_STATE_ENABLED
 *                              or REFCNT_STATE_DISABLED
 * @param[in] bForce          - The setting should be forced to the requested
 *                              state; if this state is different from the
 *                              current state, this will force a reset of the
 *                              counter
 *
 * @returns NV_OK if the reference-counted setting is in the requested state
 *              upon exit
 *          NV_ERR_NOT_SUPPORTED if the REFCNT engine is not enabled
 *          NV_ERR_STATE_IN_USE if the reference-counted setting could not be
 *              put in the requested state due to conflicting requests
 *          NV_ERR_INVALID_ARGUMENT if the requested state is not one of the
 *              valid values
 *          NV_ERR_INVALID_STATE if the reference counter's bookkeeping has
 *              become unreliable
 *          NV_ERR_ILLEGAL_ACTION if the requester is attempting to request a
 *              state while it already has a reference and the
 *              reference-counter is configured not to allow recursive
 *              requests
 *          Other errors from _refcntInvokeStateChangeCallback(),
 *              refcntReset(), btreeInsert(), and btreeUnlink()
 */
NV_STATUS
refcntRequestReference_IMPL
(
    OBJREFCNT *pRefcnt,
    NvU64      requesterId,
    NvU32      requestedState,
    NvBool     bForce
)
{
    NV_STATUS               status;
    PREFCNT_REQUESTER_ENTRY pRequesterInfo = NULL;
    REFCNT_REQUESTER_ENTRY  requesterInfo;
    NvBool                  bInfoInTree    = NV_FALSE;

    // Releasing requests should go through refcntReleaseReferences()
    if (requestedState != REFCNT_STATE_ENABLED &&
        requestedState != REFCNT_STATE_DISABLED)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    // Do not allow any operations on a reference counter in an error state.
    if (pRefcnt->state == REFCNT_STATE_ERROR)
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    //
    // If there are any outstanding references to the setting, we need to
    // make sure that the requested state is the current state, or forcibly
    // reset the reference counter.
    //
    if ((pRefcnt->count > 0) && (requestedState != pRefcnt->state))
    {
        if (bForce)
        {
            //
            // Reset the counter, but don't do a state transition to the
            // default state.
            //
            status = refcntReset(pRefcnt, NV_FALSE);
            if (status != NV_OK)
            {
                goto done;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "Cannot enter requested state %d (current state: %d, count: %d)\n",
                      requestedState, pRefcnt->state,
                      pRefcnt->count);
            status = NV_ERR_STATE_IN_USE;
            goto done;
        }
    }

    // Find out if the requester has existing references to the current setting
    pRequesterInfo = mapFind(&pRefcnt->requesterTree, requesterId);
    if (pRequesterInfo != NULL)
    {
        bInfoInTree = NV_TRUE;
        //
        // Recursive requests are only legal if the reference counter
        // allows it.
        //
        if (pRequesterInfo->numReferences > 0 &&
            !pRefcnt->getProperty(pRefcnt,
                                  PDB_PROP_REFCNT_ALLOW_RECURSIVE_REQUESTS))
        {
            NV_PRINTF(LEVEL_NOTICE,
                        "Requester 0x%016llx already has a reference to this setting (current state: %d, count: %d)\n",
                        requesterId, pRefcnt->state,
                        pRefcnt->count);
            status = NV_ERR_ILLEGAL_ACTION;
            goto done;
        }
    }
    else
    {
        // We need to create a new requester entry
        requesterInfo.numReferences = 0;
    }

    //
    // Determine if we should transition into a new state. If the transition
    // fails, we don't update the counts so we don't get out of sync with the
    // setting.
    //
    if (pRefcnt->count == 0)
    {
        status = _refcntInvokeStateChangeCallback(pRefcnt, pRefcnt->state,
                                                  requestedState);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to enter state %d (current state: %d, status: 0x%08x)\n",
                      requestedState, pRefcnt->state, status);
            goto done;
        }
    }

    // Only insert new entries if we could make the state change (if any).
    if (!bInfoInTree)
    {
        pRequesterInfo = mapInsertValue(&pRefcnt->requesterTree, requesterId, &requesterInfo);
        if (pRequesterInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to insert entry for requester 0x%016llx (status: 0x%08x)\n",
                      requesterId, status);
            pRefcnt->state = REFCNT_STATE_ERROR;
            goto done;
        }

        bInfoInTree = NV_TRUE;
    }

    //
    // The state change (if any) was successful, so record the new reference
    // now.
    //
    pRequesterInfo->numReferences++;
    pRefcnt->count++;
    pRefcnt->state = requestedState;
    status = NV_OK;

done:
    // Clean up the requester info if it's not being used.
    if (pRequesterInfo && pRequesterInfo->numReferences == 0)
    {
        if (bInfoInTree)
        {
            mapRemove(&pRefcnt->requesterTree, pRequesterInfo);
        }
    }

    return status;
}

/*!
 * @brief Release an outstanding request for the reference-counted setting on
 *        behalf of the requester.
 *
 * The request must have been created via a successful call to
 * refcntRequestReference().
 *
 * @param[in] pRefcnt             - REFCNT object pointer
 * @param[in] requesterId         - Unique identifier representing the
 *                                  requester
 * @param[in] bReleaseAllRequests - Indicates whether all requests made by the
 *                                  requester for this setting should be
 *                                  released; this parameter only matters for
 *                                  REFCNTs with the ALLOW_RECURSIVE_REQUESTS
 *                                  PDB property enabled
 *
 * @returns The status of _refcntReleaseReferences()
 */
NV_STATUS
refcntReleaseReferences_IMPL
(
    OBJREFCNT *pRefcnt,
    NvU64      requesterId,
    NvBool     bReleaseAllRequests
)
{
    NvBool bNodeIsRemoved;
    return _refcntReleaseReferences(pRefcnt, requesterId, bReleaseAllRequests,
                                    NV_FALSE, &bNodeIsRemoved);
}

/*!
 * @brief Reset the reference counter to the default state.
 *
 * This will release all existing requests for the reference-counted setting,
 * invoking the reset callback for each requester.
 *
 * @param[in] pRefcnt        - REFCNT object pointer
 * @param[in] bForceCallback - If NV_TRUE, indicates that the counter should
 *                             invoke the state change callback to the default
 *                             state once it has been reset, regardless of
 *                             whether the counter was already in the default
 *                             state; if NV_FALSE, indicates that the counter
 *                             should not invoke the state change callback at
 *                             all (it will remain in its current state with
 *                             0 requesters)
 *
 * @returns NV_OK if the counter is reset successfully or was already in the
 *              DEFAULT state
 *          NV_ERR_INVALID_STATE if there are still outstanding requesters
 *              with references upon exit
 *          Other errors from refcntReleaseReferences() and
 *              _refcntInvokeStateChangeCallback()
 */
NV_STATUS
refcntReset_IMPL
(
    OBJREFCNT *pRefcnt,
    NvBool     bForceCallback
)
{
    NvU64                          requesterId;
    NV_STATUS                      status;
    NV_STATUS                      errStatus = NV_OK;
    REFCNT_REQUESTER_ENTRY_MAPIter requesterMapIter = mapIterAll(&pRefcnt->requesterTree);

    //
    // For every requester that has an outstanding reference, release its
    // references and invoke the reset callback for it.
    //
    while (mapIterNext(&requesterMapIter))
    {
        NvBool bNodeIsRemoved = NV_FALSE;
        requesterId = mapKey(&pRefcnt->requesterTree, requesterMapIter.pValue);

        //
        // Suppress the potential state change - we can decide later if we
        // need to invoke it explicitly.
        //
        status = _refcntReleaseReferences(pRefcnt, requesterId, NV_TRUE,
                                          NV_TRUE, &bNodeIsRemoved);

        if (bNodeIsRemoved)
        {
            //
            // pNode is removed from the map and freed in
            // _refcntReleaseReferences(), so re-initialize requesterMapIter.
            // The iteration should start from the smallest key which is larger
            // than requesterId now.
            //
            requesterMapIter = mapIterRange(&pRefcnt->requesterTree,
                                            mapFindGEQ(&pRefcnt->requesterTree, requesterId),
                                            mapFindLEQ(&pRefcnt->requesterTree, NV_U64_MAX));
        }

        if (status != NV_OK)
        {
            errStatus = (errStatus == NV_OK) ? status : errStatus;
            NV_PRINTF(LEVEL_WARNING,
                      "Failed to release references on behalf of requester 0x%016llx prior to a forced reset (status: 0x%08x)\n",
                      requesterId, status);
        }

        // Call the reset callback for the requester, if any
        _refcntInvokeResetCallback(pRefcnt, requesterId);
    }

    //
    // After a forced reset, the reference counter should be cleared
    // out (but not in the default state yet)
    //
    if (pRefcnt->count != 0)
    {
        NV_ASSERT(pRefcnt->count == 0);
        pRefcnt->state = REFCNT_STATE_ERROR;
        errStatus = NV_ERR_INVALID_STATE;
    }

    if (bForceCallback && errStatus == NV_OK)
    {
        errStatus = _refcntInvokeStateChangeCallback(pRefcnt,
                                                     pRefcnt->state,
                                                     REFCNT_STATE_DEFAULT);
        if (errStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to reset setting (status: 0x%x)\n",
                      errStatus);
            DBG_BREAKPOINT();
            pRefcnt->state = REFCNT_STATE_ERROR;
        }
        else
        {
            //
            // Only mark the counter as in the default state if we're actually
            // there
            //
            pRefcnt->state = REFCNT_STATE_DEFAULT;
        }
    }

    return errStatus;
}

/*!
 * @brief Perform the actual reference release on behalf of the requester.
 *
 * @param[in] pRefcnt              - REFCNT object pointer
 * @param[in] requesterId          - Unique identifier representing the
 *                                   requester
 * @param[in] bReleaseAllRequests  - Indicates whether all requests made by the
 *                                   requester for this setting should be
 *                                   released; this parameter only matters for
 *                                   REFCNTs with the ALLOW_RECURSIVE_REQUESTS
 *                                   PDB property enabled
 * @param[in] bSuppressStateChange - Indicates whether the state change (and
 *                                   its callback) should be suppressed
 * @param[out] bNodeIsRemoved      - Whether the requester is removed from
 *                                   requesterTree.
 *
 * @returns NV_OK if the requester's references are successfully removed from
 *              the reference counter
 *          NV_ERR_INVALID_STATE if the reference counter's bookkeeping has
 *              become unreliable
 *          NV_ERR_ILLEGAL_ACTION if the requester does not own any references
 *              to the reference-counted setting
 *          Other errors from _refcntInvokeStateChangeCallback()
 */
static NV_STATUS
_refcntReleaseReferences
(
    OBJREFCNT *pRefcnt,
    NvU64      requesterId,
    NvBool     bReleaseAllRequests,
    NvBool     bSuppressStateChange,
    NvBool    *bNodeIsRemoved
)
{
    NV_STATUS               status         = NV_OK;
    NV_STATUS               tmpStatus      = NV_OK;
    PREFCNT_REQUESTER_ENTRY pRequesterInfo = NULL;
    NvU32                   nAdjust        = 0;

    (*bNodeIsRemoved) = NV_FALSE;

    // Do not allow any operations on a reference counter in an error state.
    if (pRefcnt->state == REFCNT_STATE_ERROR)
    {
        return NV_ERR_INVALID_STATE;
    }

    pRequesterInfo = mapFind(&pRefcnt->requesterTree, requesterId);
    if (pRequesterInfo != NULL)
    {
        if (bReleaseAllRequests)
        {
            nAdjust = pRequesterInfo->numReferences;
        }
        else if (pRequesterInfo->numReferences > 0)
        {
            nAdjust = 1;
        }

        if (pRefcnt->count >= nAdjust)
        {
            pRefcnt->count -= nAdjust;
            pRequesterInfo->numReferences -= nAdjust;
        }
        else
        {
            //
            // The global refcount should always be at least the number of
            // references from a single requester.
            //
            NV_ASSERT(0);
            pRefcnt->state = REFCNT_STATE_ERROR;
            status = NV_ERR_INVALID_STATE;
        }

        // Remove the entry from the reference counter, if necessary
        if (pRequesterInfo == NULL || pRequesterInfo->numReferences == 0)
        {
            mapRemove(&pRefcnt->requesterTree, pRequesterInfo);
            (*bNodeIsRemoved) = NV_TRUE;
        }

        //
        // Transition into the default state if necessary - only change the
        // counter state if the transition is successful, but leave the count
        // at 0 to inidicate it's not in use. Do not do the state transition
        // while in reset - refcntReset() will handle this if needed.
        //
        if (pRefcnt->state != REFCNT_STATE_DEFAULT && pRefcnt->count == 0 &&
            !bSuppressStateChange)
        {
            tmpStatus = _refcntInvokeStateChangeCallback(pRefcnt,
                                                         pRefcnt->state,
                                                         REFCNT_STATE_DEFAULT);
            if (tmpStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to enter default state (current state: %d, status: 0x%08x)\n",
                          pRefcnt->state, tmpStatus);
                if (status == NV_OK)
                {
                    status = tmpStatus;
                }
            }
            else
            {
                pRefcnt->state = REFCNT_STATE_DEFAULT;
            }
        }
    }
    else
    {
        status = NV_ERR_ILLEGAL_ACTION;
    }

    // Sanity check - the count should be 0 when we're in the _DEFAULT state.
    if (pRefcnt->state == REFCNT_STATE_DEFAULT)
    {
        NV_ASSERT(pRefcnt->count == 0);
    }

    return status;
}

/*!
 * @brief Invoke the state-change callback to trigger the actual change in the
 *        reference-counted setting.
 *
 * If there is no state-change callback, the state change simply succeeds.
 *
 * @param[in] pRefcnt   - REFCNT object pointer
 * @param[in] prevState - The state of the reference-counted setting prior to
 *                        the state change
 * @param[in] newState  - The intended new state of the reference-counted
 *                        setting after the state change
 *
 * @returns NV_OK if there is no state change callback or the state change is
 *              successful
 *          Other errors from the state change callback wired up to
 *              pRefcnt->refcntStateChangeCallback
 */
static NV_STATUS
_refcntInvokeStateChangeCallback
(
    OBJREFCNT   *pRefcnt,
    REFCNT_STATE prevState,
    REFCNT_STATE newState
)
{
    if (pRefcnt->refcntStateChangeCallback != NULL)
    {
        return pRefcnt->refcntStateChangeCallback(pRefcnt, pRefcnt->pParent,
                                                  prevState, newState);
    }

    // No state change callback is hooked up, assume that's okay
    return NV_OK;
}

/*!
 * @brief Invoke the reset callback to handle the reference-counted setting
 *        being reset underneath the requester.
 *
 * @param[in] pRefcnt     - REFCNT object pointer
 * @param[in] requesterId - Unique identifier representing the requester
 */
static void
_refcntInvokeResetCallback
(
    OBJREFCNT *pRefcnt,
    NvU64      requesterId
)
{
    if (pRefcnt->refcntResetCallback != NULL)
    {
        pRefcnt->refcntResetCallback(pRefcnt, pRefcnt->pParent, requesterId);
    }
}

