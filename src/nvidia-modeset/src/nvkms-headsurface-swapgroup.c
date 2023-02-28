/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-headsurface-swapgroup.h"
#include "nvkms-headsurface-config.h"
#include "nvkms-headsurface.h"
#include "nvkms-utils.h"
#include "nvkms-private.h"

/*
 * When headSurface is used to implement a SwapGroup, there are several surfaces
 * involved:
 *
 *                      +------+
 *                      |      |        Client Surface
 *                      +------+
 *                        |  |
 *                    +---+  +---+      screen-aligned blits
 *                    v          v
 *                 +------+  +------+
 *                 |(0)   |  |(1)   |   Staging Surfaces
 *                 +------+  +------+
 *                    |          |      headSurface transformation
 *                    v          v
 *                 +------+  +------+
 *                 |(0)   |  |(1)   |
 *                 |......|  |......|   Double-height headSurface surfaces.
 *                 |      |  |      |
 *                 +------+  +------+
 *
 * For each frame of headSurface:
 *
 * - All non-SwapGroup content from the Client Surface is copied to Staging
 *   Surface (0) (merging new non-SwapGroup content with old SwapGroup content).
 *
 * - All content is copied from the Client Surface to Staging Surface (1).
 *
 * - HeadSurface transformation (applying rotation, yuv420, etc) textures from
 *   Staging Surface (0) and renders to one half of double-height headSurface
 *   surface (0).
 *
 * - HeadSurface transformation (applying rotation, yuv420, etc) textures from
 *   Staging Surface (1) and renders to one half of double-height headSurface
 *   surface (1).
 *
 * - We use ViewPortIn to flip between halves of the double-height headSurface
 *   surfaces.
 *
 * - We use flip-locked flips to flip between the headSurface surfaces (0) and
 *   (1).
 *
 * For performance reasons the following optimizations are in place to
 * reduce blitting overhead:
 *
 * - If Swapgroup content is full-screen we skip the Staging Surface
 *   entirely. There are no regions of Swapgroup and non-Swapgroup content
 *   that need to be glued together in the Staging Surface: we will only
 *   ever source updated content from the swapgroup client. We can also
 *   render transformed frames without the Staging Surface in this case.
 *
 * - With full-screen Swapgroup content we can also skip REQUEST_TYPE_VBLANK
 *   frames as there can't possibly be any non-Swapgroup content to be
 *   updated. We do have to keep rendering continuously though to update the
 *   mouse cursor, overlays, and other individual artifacts. We thus skip
 *   REQUEST_TYPE_VBLANK frames if the client keeps actively generating
 *   REQUEST_TYPE_SWAP_GROUP_READY frames. However, if the client pauses
 *   itself we fall back to rendering REQUEST_TYPE_VBLANK frames for cursor
 *   and other updates at vsync rate.
 */

static void FlipSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup);

/*!
 * Update whether this pSwapGroup needs headSurface.
 *
 * \param[in]  pDevEvo     The device the SwapGroup is on.
 * \param[in]  pSwapGroup  The SwapGroup to update.
 * \param[in]  needed      Whether headSurface is needed for this SwapGroup.
 *
 * The usage here is similar to what is done at modeset time: create an
 * NVHsConfig and initialize it to describe the headSurface configuration
 * required across the device.  Allocate the resources for the NVHsConfig and
 * apply them to the pDevEvo.
 *
 * \return     Return whether the headSurface needed state could be successfully
 *             updated.
 */
static NvBool HsSwapGroupUpdateHeadSurfaceNeeded(
    NVDevEvoPtr pDevEvo,
    const NVSwapGroupRec *pSwapGroup,
    const NvBool needed)
{
    NvBool ret = FALSE;
    NVHsConfig *pHsConfig = nvCalloc(1, sizeof(*pHsConfig));

    if (pHsConfig == NULL) {
        goto done;
    }

    nvHsConfigInitSwapGroup(pDevEvo, pSwapGroup, needed, pHsConfig);

    if (!nvHsConfigAllocResources(pDevEvo, pHsConfig)) {
        goto done;
    }

    /*
     * XXX NVKMS HEADSURFACE TODO: validate the pHsConfig.  If we fail
     * validation, downgrade and try again.  Repeat until we either pass
     * validation or downgrading fails and we need to give up.
     */

    /* we cannot fail beyond this point */

    nvHsConfigStop(pDevEvo, pHsConfig);

    nvHsConfigStart(pDevEvo, pHsConfig);

    nvHsConfigFreeResources(pDevEvo, pHsConfig);

    ret = TRUE;

done:
    nvFree(pHsConfig);

    return ret;
}


/*!
 * Return whether there is a non-zero number of SwapGroup members for the
 * specified device, disp, head.
 */
NvBool nvHsSwapGroupIsHeadSurfaceNeeded(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead)
{
    const NVSwapGroupRec *pSwapGroup = pDispEvo->pSwapGroup[apiHead];

    if (pSwapGroup == NULL) {
        return FALSE;
    }

    return pSwapGroup->nMembers > 0;
}


/*
 * We should only write to the NvKmsDeferredRequestFifo::semaphore[] element
 * that the client requested.  When the NVDeferredRequestFifoRec is not ready,
 * store an invalid semaphore index value, so that bad indexing is obvious.
 */
#define INVALID_SEMAPHORE_INDEX 0xFFFFFFFF


/*!
 * Return whether all current members of the SwapGroup are ready to swap.
 */
static NvBool SwapGroupIsReady(const NVSwapGroupRec *pSwapGroup)
{
    /*
     * As an optimization, we maintain nMembers and nMembersReady so that we can
     * quickly check if all members are ready.
     *
     * If a member joins while a swapgroup has a pending flip, it is put in a
     * pendingJoined state, added to nMembersPendingJoined, and not counted
     * here.
     */
    NvBool ret = (pSwapGroup->nMembers - pSwapGroup->nMembersPendingJoined) ==
                 pSwapGroup->nMembersReady;

    /* This should only be called if a swap group hasn't been zombified. */
    nvAssert(!pSwapGroup->zombie);

#if defined(DEBUG)
    /*
     * Assert that bookkeeping matches between
     * pDeferredRequestFifo->swapGroup.ready,
     * pDeferredRequestFifo->swapGroup.pendingJoined, and nMembersReady.
     */
    NvBool assertRet = TRUE;

    const NVDeferredRequestFifoRec *pDeferredRequestFifo;

    FOR_ALL_DEFERRED_REQUEST_FIFOS_IN_SWAP_GROUP(pSwapGroup,
                                                 pDeferredRequestFifo) {

        nvAssert(pDeferredRequestFifo->swapGroup.pSwapGroup == pSwapGroup);
        if (!pDeferredRequestFifo->swapGroup.ready &&
            !pDeferredRequestFifo->swapGroup.pendingJoined) {
            assertRet = FALSE;
            break;
        }
    }

    nvAssert(assertRet == ret);
#endif

    return ret;
}


/*!
 * Release the pDeferredRequestFifo member of the SwapGroup.
 */
static void ReleaseDeferredRequestFifo(
    NVDeferredRequestFifoRec *pDeferredRequestFifo)
{
    const NvGpuSemaphore semReadyValue = {
        .data[0] = NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_READY,
    };

    const NvU32 semIndex = pDeferredRequestFifo->swapGroup.semaphoreIndex;
    struct NvKmsDeferredRequestFifo *pFifo = pDeferredRequestFifo->fifo;
    NVSwapGroupRec *pSwapGroup = pDeferredRequestFifo->swapGroup.pSwapGroup;

    /* This should only be called if the member is ready. */
    nvAssert(pDeferredRequestFifo->swapGroup.ready);

    /*
     * This shouldn't be called if a new member has joined and submitted
     * swapready while the swap group had an outstanding flip (i.e.
     * pendingJoined is true).  In that case:
     *
     * - nvHsSwapGroupReady() will mark this member as pendingReady
     * - When the flip completes, nvHsSwapGroupRelease will release all members
     *   that were present when the flip was kicked off, and promote
     *   pendingJoined/pendingReady members to joined/ready
     * - When the original members submit another ready (nvHsSwapGroupReady),
     *   the last non-ready original member leaves (nvHsLeaveSwapGroup), or
     *   the last non-pendingReady member leaves between kicking off a flip
     *   and the first vblank after the flip completes (nvHsSwapGroupRelease),
     *   a new flip is kicked off.
     */
    nvAssert(!pDeferredRequestFifo->swapGroup.pendingJoined &&
             !pDeferredRequestFifo->swapGroup.pendingReady);

    if (semIndex < ARRAY_LEN(pFifo->semaphore)) {
        pFifo->semaphore[semIndex] = semReadyValue;

        nvSendUnicastEvent(pDeferredRequestFifo->swapGroup.pOpenUnicastEvent);

    } else {

        /*
         * The semaphoreIndex is validated in nvHsSwapGroupReady() before
         * assignment, so it should always be valid by the time we get here.
         */
        nvAssert(!"Invalid semIndex");
    }

    pDeferredRequestFifo->swapGroup.ready = FALSE;
    pDeferredRequestFifo->swapGroup.semaphoreIndex = INVALID_SEMAPHORE_INDEX;

    /*
     * This may be called if a deferred request fifo entry is being processed
     * after its associated swap group has been freed, in which case,
     * pDeferredRequestFifo->swapGroup.pSwapGroup will be NULL.
     */
    if (pSwapGroup) {
        nvAssert(!pSwapGroup->zombie);
        nvAssert(pSwapGroup->nMembersReady > 0);
        pSwapGroup->nMembersReady--;
    }
}


/*!
 * Release all members of the SwapGroup.
 */
void nvHsSwapGroupRelease(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup)
{
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    NvBool readiedFifos = FALSE;

    /* This should only be called if a swap group hasn't been zombified. */
    nvAssert(!pSwapGroup->zombie);

    FOR_ALL_DEFERRED_REQUEST_FIFOS_IN_SWAP_GROUP(pSwapGroup,
                                                 pDeferredRequestFifo) {
        if (!pDeferredRequestFifo->swapGroup.pendingJoined) {
            ReleaseDeferredRequestFifo(pDeferredRequestFifo);
        }
    }

    nvAssert(pSwapGroup->nMembersReady == 0);

    pSwapGroup->pendingFlip = FALSE;

    /*
     * If a new member joins or submits swap ready between FlipSwapGroup()
     * and nvHsSwapGroupRelease, it enters the pendingJoined and pendingReady
     * state to avoid changing swap group state while a flip is in flight.
     *
     * Now that the flip has completed, promote these members to fully joined/
     * fully ready.
     */
    if (pSwapGroup->nMembersPendingJoined != 0) {
        FOR_ALL_DEFERRED_REQUEST_FIFOS_IN_SWAP_GROUP(pSwapGroup,
                                                     pDeferredRequestFifo) {
            if (pDeferredRequestFifo->swapGroup.pendingJoined) {
                pDeferredRequestFifo->swapGroup.pendingJoined = FALSE;
                nvAssert(pSwapGroup->nMembersPendingJoined > 0);
                pSwapGroup->nMembersPendingJoined--;
            }

            if (pDeferredRequestFifo->swapGroup.pendingReady) {
                pDeferredRequestFifo->swapGroup.pendingReady = FALSE;
                pDeferredRequestFifo->swapGroup.ready = TRUE;
                pSwapGroup->nMembersReady++;
                readiedFifos = TRUE;
            }
        }

        nvAssert(pSwapGroup->nMembersPendingJoined == 0);
    }

    /*
     * If any pending joined fifos submitted SWAP_READY while a flip was in
     * flight, they were promoted from pending ready to ready above; if the
     * fifos that were previously joined to the swap group left while their
     * last flip was in flight, then we need to kick off a new flip for the
     * previously pending ready fifos now.
     */
    if (readiedFifos && SwapGroupIsReady(pSwapGroup)) {
        FlipSwapGroup(pDevEvo, pSwapGroup);
    }
}


/*!
 * Enqueue a headSurface flip on all heads of the pSwapGroup.
 *
 * Now that the specified pSwapGroup is ready, call
 * nvHsNextFrame(SWAP_GROUP_READY) for all active heads in the pSwapGroup.  This
 * will render a new frame of headSurface, including the now complete SwapGroup
 * content, and kick off a flip to the new frame.  The flip will only complete
 * once the entire SwapBarrier is ready.
 *
 * Once headSurface sees that these flips completed, it will call
 * nvHsSwapGroupRelease() to release all SwapGroup members.
 */
static void FlipSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup)
{
    NVHsDeviceEvoPtr pHsDevice = pDevEvo->pHsDevice;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;
    NvBool waitForFlip = FALSE;

    /*
     * We should never kick off a new flip before the previous flip has
     * completed and the swap group has been released.
     */
    nvAssert(!pSwapGroup->pendingFlip);

    pSwapGroup->pendingFlip = TRUE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->pSwapGroup); apiHead++) {
            if (pDispEvo->pSwapGroup[apiHead] == pSwapGroup) {
                NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

                if (pHsChannel == NULL) {
                    continue;
                }

                nvHsNextFrame(pHsDevice, pHsChannel,
                              NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY);
                waitForFlip = TRUE;
            }
        }
    }

    /*
     * If there are no active heads in the pSwapGroup, then there are no flips
     * to wait for: release the SwapGroup now.
     */
    if (!waitForFlip) {
        nvHsSwapGroupRelease(pDevEvo, pSwapGroup);
    }
}


/*!
 * Check that the NvKmsSwapGroupConfig is valid for the given pDevEvo.
 */
static NvBool HsSwapGroupValidateConfig(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsSwapGroupConfig *pConfig)
{
    const NvU32 validHeadMask = NVBIT(pDevEvo->numApiHeads) - 1;
    NvU32 dispIndex;

    for (dispIndex = 0; dispIndex < ARRAY_LEN(pConfig->disp); dispIndex++) {

        if (pConfig->disp[dispIndex].headMask == 0) {
            continue;
        }

        /* Fail if the config describes disps not present on the pDevEvo. */

        if (dispIndex >= pDevEvo->nDispEvo) {
            return FALSE;
        }

        /* Fail if the config describes heads not present on the disp. */

        if ((pConfig->disp[dispIndex].headMask & ~validHeadMask) != 0) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Allocate a SwapGroup.
 *
 * This validates that the requested configuration is valid, and adds the
 * SwapGroup to pDevEvo's list of SwapGroups.
 */
NVSwapGroupRec* nvHsAllocSwapGroup(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsAllocSwapGroupRequest *pRequest)
{
    NVSwapGroupRec *pSwapGroup;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    if (!HsSwapGroupValidateConfig(pDevEvo, &pRequest->config)) {
        return NULL;
    }

    /* Are there heads requested that already belong to another SwapGroup? */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        FOR_ALL_HEADS(apiHead, pRequest->config.disp[dispIndex].headMask) {
            if (pDispEvo->pSwapGroup[apiHead] != NULL) {
                return NULL;
            }
        }
    }

    pSwapGroup = nvCalloc(1, sizeof(*pSwapGroup));

    if (pSwapGroup == NULL) {
        return NULL;
    }

    if (!nvHsIncrementSwapGroupRefCnt(pSwapGroup)) {
        nvFree(pSwapGroup);
        return NULL;
    }

    nvListInit(&pSwapGroup->deferredRequestFifoList);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        FOR_ALL_HEADS(apiHead, pRequest->config.disp[dispIndex].headMask) {
            pDispEvo->pSwapGroup[apiHead] = pSwapGroup;
        }
    }

    return pSwapGroup;
}


/*!
 * Returns a swap group with the given handle from the given set of swap group
 * handles.
 *
 * Does not return NULL if pSwapGroup->zombie is TRUE; the only operation we
 * want to perform on a swap group that has already been freed but hasn't lost
 * all of its references yet is NVKMS_IOCTL_RELEASE_SWAP_GROUP; all other
 * callers should use nvHsGetSwapGroup.
 */
NVSwapGroupRec *nvHsGetSwapGroupStruct(
    const NVEvoApiHandlesRec *pEvoApiHandles,
    NvKmsSwapGroupHandle handle)
{
    return nvEvoGetPointerFromApiHandle(pEvoApiHandles, handle);
}


/*!
 * Returns a swap group with the given handle from the given set of swap group
 * handles, or NULL if the swap group is in the "zombie" state.
 *
 * A swap group is in the "zombie" state if nvHsFreeSwapGroup() has removed it
 * from pDevEvo->swapGroupList, removed its deferred request fifos from
 * pSwapGroup->deferredRequestFifoList, and freed its clip list, but its
 * reference count is nonzero; in that case, most operations on that swap group
 * should call this function and behave as if that swap group no longer exists.
 */
NVSwapGroupRec *nvHsGetSwapGroup(
    const NVEvoApiHandlesRec *pEvoApiHandles,
    NvKmsSwapGroupHandle handle)
{
    NVSwapGroupRec *pSwapGroup = nvHsGetSwapGroupStruct(pEvoApiHandles, handle);

    if (pSwapGroup == NULL) {
        return NULL;
    }

    if (pSwapGroup->zombie) {
        return NULL;
    }

    return pSwapGroup;
}


/*!
 * Increment the swap group's reference count, failing if refCnt is already
 * NV_U64_MAX.
 */
NvBool nvHsIncrementSwapGroupRefCnt(NVSwapGroupPtr pSwapGroup)
{
    if (pSwapGroup->refCnt == NV_U64_MAX) {
        return FALSE;
    }

    pSwapGroup->refCnt++;

    return TRUE;
}


/*!
 * Decrement the swap group's reference count, and free it if there are no more
 * references to it.
 */
void nvHsDecrementSwapGroupRefCnt(NVSwapGroupPtr pSwapGroup)
{
    nvAssert(pSwapGroup->refCnt >= 1);
    pSwapGroup->refCnt--;

    if (pSwapGroup->refCnt == 0) {
        nvFree(pSwapGroup);
    }
}


/*!
 * Free the SwapGroup.
 *
 * - Make any NVDeferredRequestFifoRec members implicitly leave the SwapGroup.
 *
 * - Remove the SwapGroup from pDevEvo's list of SwapGroups.
 *
 * - Free the SwapGroup's pClipList
 *
 * - Mark the SwapGroup as a "zombie" and decrement its reference count.  If
 *   this removes the last reference to the swap group, free it immediately.
 *   Otherwise, any remaining references to the swap group need to handle
 *   the "zombie" swap group correctly.
 */
void nvHsFreeSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup)
{
    NVDeferredRequestFifoRec *pDeferredRequestFifo;
    NVDeferredRequestFifoRec *pDeferredRequestFifoTmp;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    /*
     * Be careful to use the "_safe" loop macro, because nvHsLeaveSwapGroup()
     * will remove pDeferredRequestFifo from
     * pSwapGroup->deferredRequestFifoList.
     */
    nvListForEachEntry_safe(pDeferredRequestFifo,
                            pDeferredRequestFifoTmp,
                            &pSwapGroup->deferredRequestFifoList,
                            swapGroup.deferredRequestFifoListEntry) {

        nvHsLeaveSwapGroup(pDevEvo, pDeferredRequestFifo, TRUE /* teardown */);
    }

    nvAssert(pSwapGroup->nMembers == 0);
    nvAssert(pSwapGroup->nMembersReady == 0);

    /*
     * XXX NVKMS HEADSURFACE TODO: Shutdown headSurface, if this SwapGroup was
     * forcing headSurface on.
     */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->pSwapGroup); apiHead++) {
            if (pDispEvo->pSwapGroup[apiHead] == pSwapGroup) {
                pDispEvo->pSwapGroup[apiHead] = NULL;
            }
        }
    }

    nvFree(pSwapGroup->pClipList);
    pSwapGroup->pClipList = NULL;

    pSwapGroup->zombie = TRUE;
    nvHsDecrementSwapGroupRefCnt(pSwapGroup);
}


/*!
 * Given an array of {deferred request fifo, swapgroup} tuples, join each
 * deferred request fifo to its corresponding swapgroup.  If any join causes
 * a headsurface transition which fails, clean up by undoing any headsurface
 * transitions which succeeded.
 */
NvBool nvHsJoinSwapGroup(
    NVHsJoinSwapGroupWorkArea *joinSwapGroupWorkArea,
    NvU32 numHandles,
    NvBool pendingJoin)
{
    NvU32 i;

    for (i = 0; i < numHandles; i++) {
        NVDevEvoPtr pDevEvo = joinSwapGroupWorkArea[i].pDevEvo;
        NVSwapGroupRec *pSwapGroup = joinSwapGroupWorkArea[i].pSwapGroup;
        NVDeferredRequestFifoRec *pDeferredRequestFifo =
            joinSwapGroupWorkArea[i].pDeferredRequestFifo;

        /*
         * If we are transitioning from 0 to 1 nMembers, change the "needed" state
         * of headSurface.
         */
        if (pSwapGroup->nMembers == 0) {
            /*
             * pendingJoin should only be true if a client joins a
             * pDeferredRequestFifo to a SwapGroup while a SwapGroup flip is
             * pending on any SwapGroup that is being joined as part of this
             * collective join; in that case, there must already be at least
             * one fifo joined to each SwapGroup this client is joining, or we
             * may do a headsurface transition with a SwapGroup flip pending.
             */
            nvAssert(!pendingJoin);

            if (!HsSwapGroupUpdateHeadSurfaceNeeded(pDevEvo, pSwapGroup, TRUE)) {
                goto fail;
            }

            /*
             * Keep track of all the swapgroups which have successfully caused
             * a headsurface transition, so we can disable headsurface on them
             * if we fail to enable headsurface on a subsequent entry.
             */
            joinSwapGroupWorkArea[i].enabledHeadSurface = TRUE;
        }

        /* This should only be called if a swap group hasn't been zombified. */
        nvAssert(!pSwapGroup->zombie);

        pSwapGroup->nMembers++;

        nvkms_memset(&pDeferredRequestFifo->swapGroup, 0,
                     sizeof(pDeferredRequestFifo->swapGroup));

        if (pendingJoin) {
            pDeferredRequestFifo->swapGroup.pendingJoined = TRUE;
            pSwapGroup->nMembersPendingJoined++;
        }

        pDeferredRequestFifo->swapGroup.pSwapGroup = pSwapGroup;
        pDeferredRequestFifo->swapGroup.semaphoreIndex = INVALID_SEMAPHORE_INDEX;

        nvListAppend(&pDeferredRequestFifo->swapGroup.deferredRequestFifoListEntry,
                     &pSwapGroup->deferredRequestFifoList);
    }

    return TRUE;

fail:
    /*
     * Enabling headsurface for one of the swapgroups in this request failed;
     * undo any successful headsurface enablements that happened earlier.
     */
    for (i = 0; i < numHandles; i++) {
        if (joinSwapGroupWorkArea[i].enabledHeadSurface) {
            NVDevEvoPtr pDevEvo = joinSwapGroupWorkArea[i].pDevEvo;
            NVSwapGroupRec *pSwapGroup = joinSwapGroupWorkArea[i].pSwapGroup;
            if (!HsSwapGroupUpdateHeadSurfaceNeeded(pDevEvo, pSwapGroup, FALSE)) {
                nvAssert(!"Failed nvHsJoinSwapGroup cleanup.");
            }
        }
    }

    return FALSE;
}


/*!
 * Remove the pDeferredRequestFifo from the SwapGroup.
 */
void nvHsLeaveSwapGroup(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo,
    NvBool teardown)
{
    NVSwapGroupRec *pSwapGroup = pDeferredRequestFifo->swapGroup.pSwapGroup;
    NvBool removingReadyFifo = FALSE;

    if (pSwapGroup == NULL) {
        return;
    }

    /* This should only be called if a swap group hasn't been zombified. */
    nvAssert(!pSwapGroup->zombie);

    /*
     * If the last member of the SwapGroup is leaving, change the "needed" state
     * of headSurface.
     */
    if (pSwapGroup->nMembers == 1) {
        if (!HsSwapGroupUpdateHeadSurfaceNeeded(pDevEvo, pSwapGroup, FALSE)) {
            nvAssert(!"Failed to transition out of headSurface");
            /* XXX NVKMS HEADSURFACE TODO: we need to do something here... */
        }
    }

    nvListDel(&pDeferredRequestFifo->swapGroup.deferredRequestFifoListEntry);
    pDeferredRequestFifo->swapGroup.pSwapGroup = NULL;

    nvRemoveUnicastEvent(pDeferredRequestFifo->swapGroup.pOpenUnicastEvent);

    if (pDeferredRequestFifo->swapGroup.ready) {
        nvAssert(pSwapGroup->nMembersReady > 0);
        removingReadyFifo = TRUE;
        pSwapGroup->nMembersReady--;
    }

    nvAssert(pSwapGroup->nMembers > 0);
    pSwapGroup->nMembers--;

    /*
     * Release the SwapGroup if this member was the only unready member of the
     * SwapGroup.
     *
     * We only want to do this if we're not in the process of removing
     * every member from the swap group (i.e. while the swap group is being
     * freed), since this may trigger a flip depending on the order in which
     * the deferred request fifos leave the swap group if some members are
     * ready and some aren't.
     *
     * In addition, we only want to do this if the member we're removing was
     * the last member preventing the SwapGroup from flipping; otherwise, we
     * may kick off a redundant flip here if a member leaves between the swap
     * group kicking off a flip and a subsequent vblank or headsurface
     * transition releasing the swapgroup.
     */
    if (!teardown && !removingReadyFifo && (pSwapGroup->nMembers != 0)) {
        if (SwapGroupIsReady(pSwapGroup)) {
            FlipSwapGroup(pDevEvo, pSwapGroup);
        }
    }
}


/*!
 * Update the clip list of the SwapGroup.
 *
 * \param[in]  pDevEvo     The device the SwapGroup is on.
 * \param[in]  pSwapGroup  The SwapGroup to modify.
 * \param[in]  nClips      The number of NvKmsRects in pClipList.
 * \param[in]  pClipList   The array of NvKmsRects.  This is dynamically
 *                         allocated by the caller.
 *
 * \return  Return whether the clip list was updated.  If this returns TRUE,
 *          then pSwapGroup takes responsibility for freeing pClipList (either
 *          when the next clip list update occurs, or when freeing the
 *          pSwapGroup).
 */
NvBool nvHsSetSwapGroupClipList(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup,
    const NvU16 nClips,
    struct NvKmsRect *pClipList)
{
    NvU16 i;

    /*
     * TODO:
     *
     * - If clip list is transitioning from empty to non-empty, allocate
     *   headSurface SwapGroup resources.
     *
     * - If clip list is transitioning from non-empty to empty, free headSurface
     *   SwapGroup resources.
     */

    /* Reject the clip list if any of rects overflow NvU16. */

    for (i = 0; i < nClips; i++) {

        if (A_plus_B_greater_than_C_U16(pClipList[i].x,
                                        pClipList[i].width,
                                        NV_U16_MAX)) {
            return FALSE;
        }

        if (A_plus_B_greater_than_C_U16(pClipList[i].y,
                                        pClipList[i].height,
                                        NV_U16_MAX)) {
            return FALSE;
        }
    }

    nvFree(pSwapGroup->pClipList);

    pSwapGroup->pClipList = pClipList;
    pSwapGroup->nClips = nClips;

    /* The cliplists we receive here originate straight from
     * UpdateSwapGroupClipList() in nvx_clip.c. The clips are exclusive in
     * nature, i.e. they describe areas without swapgroup content. The
     * cliplists come constructed depending on the screen content as
     * follows:
     *
     * 1) No swapgroup content at all: NULL cliplist
     * 2) Swapgroup and non-swapgroup content: cliplist contains regions
     *    covering all the non-swapgroup areas
     * 3) Only swapgroup content: a cliplist of length 1 containing an empty
     *    clip region. This is what we detect and cache here.
     */
    pSwapGroup->swapGroupIsFullscreen = (pSwapGroup->nClips == 1 &&
                                         pSwapGroup->pClipList[0].x == 0 &&
                                         pSwapGroup->pClipList[0].y == 0 &&
                                         pSwapGroup->pClipList[0].width == 0 &&
                                         pSwapGroup->pClipList[0].height == 0);

    return TRUE;
}


/*!
 * Mark the pDeferredRequestFifo SwapGroup member as ready.
 */
void nvHsSwapGroupReady(
    NVDevEvoPtr pDevEvo,
    NVDeferredRequestFifoRec *pDeferredRequestFifo,
    const NvU32 request)
{
    const NvU32 semaphoreIndex =
        DRF_VAL(KMS, _DEFERRED_REQUEST, _SEMAPHORE_INDEX, request);
    const NvU32 perEyeStereo =
        DRF_VAL(KMS, _DEFERRED_REQUEST,
                _SWAP_GROUP_READY_PER_EYE_STEREO, request);

    NVSwapGroupRec *pSwapGroup = pDeferredRequestFifo->swapGroup.pSwapGroup;

    if (semaphoreIndex >= NVKMS_MAX_DEFERRED_REQUESTS) {
        return;
    }

    /*
     * Duplicate execution of a deferred request fifo entry likely indicates
     * a poorly behaved client, so assert that this semaphore index hasn't
     * transitioned yet.
     */
    nvAssert(pDeferredRequestFifo->fifo->semaphore[semaphoreIndex].data[0] !=
             NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_READY);

    pDeferredRequestFifo->swapGroup.semaphoreIndex = semaphoreIndex;

    if (pDeferredRequestFifo->swapGroup.pendingJoined) {
        /*
         * This deferred request fifo joined and was marked ready between its
         * swap group kicking off a flip and that flip completing.  Mark it
         * as pendingReady now, and it will be promoted to ready once the
         * swap group is released.
         */
        pDeferredRequestFifo->swapGroup.pendingReady = TRUE;
        return;
    }

    pDeferredRequestFifo->swapGroup.ready = TRUE;
    pDeferredRequestFifo->swapGroup.perEyeStereo =
        perEyeStereo == NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO_PER_EYE;

    /*
     * This may be called if a deferred request fifo entry is being processed
     * after its associated swap group has been freed, in which case,
     * pDeferredRequestFifo->swapGroup.pSwapGroup will be NULL and we should
     * release the deferred request fifo.
     */
    if (pSwapGroup == NULL) {
        ReleaseDeferredRequestFifo(pDeferredRequestFifo);
        return;
    }

    nvAssert(pSwapGroup->nMembersReady < NV_U32_MAX);
    pSwapGroup->nMembersReady++;
    nvAssert(pSwapGroup->nMembersReady <= pSwapGroup->nMembers);

    /* Kick off a SwapGroup flip when all members are ready. */
    if (SwapGroupIsReady(pSwapGroup)) {
        FlipSwapGroup(pDevEvo, pSwapGroup);
    }
}

/*!
 * Return the reconciled perEyeStereo setting across all deferred request fifos
 * joined to this SwapGroup.
 *
 * If any deferred request fifo wants per-eye presentation (perEyeStereo ==
 * TRUE), return TRUE for the entire SwapGroup.  Otherwise, return FALSE (i.e.,
 * per-pair presentation).
 */
NvBool nvHsSwapGroupGetPerEyeStereo(
    const NVSwapGroupRec *pSwapGroup)
{
    const NVDeferredRequestFifoRec *pDeferredRequestFifo;

    FOR_ALL_DEFERRED_REQUEST_FIFOS_IN_SWAP_GROUP(pSwapGroup,
                                                 pDeferredRequestFifo) {
        if (pDeferredRequestFifo->swapGroup.perEyeStereo) {
            return TRUE;
        }
    }

    return FALSE;
}
