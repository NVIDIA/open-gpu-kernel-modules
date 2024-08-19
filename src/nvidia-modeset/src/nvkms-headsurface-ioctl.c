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

#include "nvkms-types.h"
#include "nvkms-private.h"
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-ioctl.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-ioctl.h"
#include "nvkms-cursor.h"
#include "nvkms-utils.h"
#include "nvkms-utils-flip.h"
#include "nvkms-flip.h"

/*
 * This source file contains functions that intercept portions of the NVKMS API.
 *
 * If the API request changes headSurface state, we update headSurface
 * accordingly.  If the API request does not touch headSurface-ful heads, we
 * "call down" to the normal NVKMS implementation.
 *
 * Note that some NVKMS requests can touch multiple heads at once, where a
 * subset of the heads are driven by headSurface.  In those cases, we:
 *
 * - Identify if all requested heads are headSurface-less; if so, call down and
 *   return.
 *
 * - Validate the request for the headSurface-ful heads.
 *
 * - If there are both headSurface-less *and* headSurface-ful heads in the
 *   request, build a temporary request structure with the headSurface-ful heads
 *   removed.  Call down with that temporary structure.
 *
 * - Apply the request to the headSurface-ful heads.
 */

typedef struct _NVHsRequestedFlipState {
    struct NvKmsPoint viewPortPointIn;
    NVFlipCursorEvoHwState cursor;
    NVHsLayerRequestedFlipState layer[NVKMS_MAX_LAYERS_PER_HEAD];

    struct {
        NvBool viewPortPointIn   : 1;
        NvBool cursorSurface     : 1;
        NvBool cursorPosition    : 1;

        NvBool layer[NVKMS_MAX_LAYERS_PER_HEAD];
    } dirty;

} NVHsRequestedFlipState;

/*!
 * Validate the NvKmsMoveCursorCommonParams for headSurface.
 */
static NvBool HsIoctlValidateMoveCursor(
    const NVHsChannelEvoRec *pHsChannel,
    const struct NvKmsMoveCursorCommonParams *pParams)
{
    /* XXX NVKMS HEADSURFACE TODO: validate x,y against headSurface config */

    return TRUE;
}

/*!
 * Apply x,y to headSurface.
 *
 * This configuration should first be validated by HsIoctlValidateMoveCursor().
 * We cannot fail from here.
 */
static void HsIoctlMoveCursor(
    NVHsChannelEvoRec *pHsChannel,
    const NvS16 x, const NvS16 y)
{
    pHsChannel->config.cursor.x = x;
    pHsChannel->config.cursor.y = y;

    /*
     * XXX NVKMS HEADSURFACE TODO: record that this head is dirty, so
     * headSurface knows it needs to rerender the frame.
     */
}

/*!
 * Update headSurface for NVKMS_IOCTL_MOVE_CURSOR.
 */
NvBool nvHsIoctlMoveCursor(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead,
    const struct NvKmsMoveCursorCommonParams *pParams)
{
    NVHsChannelEvoRec *pHsChannel;

    if (apiHead >= ARRAY_LEN(pDispEvo->pHsChannel)) {
        return FALSE;
    }

    pHsChannel = pDispEvo->pHsChannel[apiHead];

    /* If headSurface is not used on this head, call down. */

    if (pHsChannel == NULL) {
        nvMoveCursor(pDispEvo, apiHead, pParams);
        return TRUE;
    }

    if (!HsIoctlValidateMoveCursor(pHsChannel, pParams)) {
        return FALSE;
    }

    HsIoctlMoveCursor(pHsChannel, pParams->x, pParams->y);

    return TRUE;
}

/*!
 * Validate the NvKmsSetCursorImageCommonParams for headSurface.
 *
 * If successfully validated, also assign ppSurfaceEvo.
 */
static NvBool HsIoctlValidateSetCursorImage(
    const NVHsChannelEvoRec *pHsChannel,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    const struct NvKmsSetCursorImageCommonParams *pParams,
    NVSurfaceEvoRec **ppSurfaceEvo)
{
    NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES];

    if (!nvGetCursorImageSurfaces(pHsChannel->pDispEvo->pDevEvo,
                                  pOpenDevSurfaceHandles,
                                  pParams,
                                  pSurfaceEvos)) {
        return FALSE;
    }

    /* XXX NVKMS HEADSURFACE TODO: make cursor stereo-aware. */

    *ppSurfaceEvo = pSurfaceEvos[NVKMS_LEFT];

    return TRUE;
}

/*!
 * Apply the cursor pSurfaceEvo to headSurface.
 *
 * This configuration should first be validated by
 * HsIoctlValidateSetCursorImage().  We cannot fail from here.
 */
static void HsIoctlSetCursorImage(
    NVHsChannelEvoRec *pHsChannel,
    NVSurfaceEvoRec *pSurfaceEvo)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;

    /*
     * Increment the refcnt of the new surface, and
     * decrement the refcnt of the old surface.
     *
     * XXX NVKMS HEADSURFACE TODO: wait until graphics channel is done using old
     * surface before decrementing its refcnt?
     */

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pSurfaceEvo, TRUE /* increase */);

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pHsChannel->config.cursor.pSurfaceEvo, FALSE /* increase */);

    pHsChannel->config.cursor.pSurfaceEvo = pSurfaceEvo;

    /*
     * XXX NVKMS HEADSURFACE TODO: record that this head is dirty, so
     * headSurface knows it needs to rerender the frame.
     */
}

/*!
 * Update headSurface for NVKMS_IOCTL_SET_CURSOR_IMAGE.
 */
NvBool nvHsIoctlSetCursorImage(
    NVDispEvoPtr pDispEvo,
    const struct NvKmsPerOpenDev *pOpenDevice,
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles,
    NvU32 apiHead,
    const struct NvKmsSetCursorImageCommonParams *pParams)
{
    NVHsChannelEvoRec *pHsChannel;
    NVSurfaceEvoRec *pSurfaceEvo = NULL;

    if (apiHead >= ARRAY_LEN(pDispEvo->pHsChannel)) {
        return FALSE;
    }

    pHsChannel = pDispEvo->pHsChannel[apiHead];

    /* If headSurface is not used on this head, call down. */

    if (pHsChannel == NULL) {
        return nvSetCursorImage(pDispEvo,
                                pOpenDevice,
                                pOpenDevSurfaceHandles,
                                apiHead,
                                pParams);
    }

    if (!HsIoctlValidateSetCursorImage(pHsChannel,
                                       pOpenDevSurfaceHandles,
                                       pParams,
                                       &pSurfaceEvo)) {
        return FALSE;
    }

    HsIoctlSetCursorImage(pHsChannel, pSurfaceEvo);

    return TRUE;
}

/*!
 * Apply NvKmsPoint to headSurface.
 */
static void HsIoctlPan(
    NVHsChannelEvoRec *pHsChannel,
    const struct NvKmsPoint *pViewPortPointIn)
{
    pHsChannel->config.viewPortIn.x = pViewPortPointIn->x;
    pHsChannel->config.viewPortIn.y = pViewPortPointIn->y;

    /*
     * XXX NVKMS HEADSURFACE TODO: record that this head is dirty, so
     * headSurface knows it needs to rerender the frame.
     */
}

/*!
 * Create a copy of pFlipHead[] array with the headSurface-ful heads removed.
 */
static struct NvKmsFlipRequestOneHead *HsIoctlRemoveHsHeadsFromNvKmsFlipHead(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsFlipRequestOneHead *pFlipHeadOriginal,
    const NvU32 numFlipHeadsOriginal,
    const NvU32 numFlipHeads,
    const NvU8 hsMask[NV_MAX_FLIP_REQUEST_HEADS / 8])
{
    struct NvKmsFlipRequestOneHead *pFlipHead = NULL;
    NvU32 i, j;

    pFlipHead = nvAlloc(sizeof(*pFlipHead) * numFlipHeads);
    if (pFlipHead == NULL) {
        return NULL;
    }

    j = 0;
    for (i = 0; i < numFlipHeadsOriginal; i++) {
        if ((hsMask[i / 8] & NVBIT(i % 8)) == 0) {
            pFlipHead[j++] = pFlipHeadOriginal[i];
        }
    }
    nvAssert(j == numFlipHeads);

    return pFlipHead;
}

static void HsIoctlAssignSurfacesMaxEyes(
    NVSurfaceEvoPtr pSurfaceEvoDst[],
    NVSurfaceEvoRec *const pSurfaceEvoSrc[])
{
    NvU8 eye;

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
        pSurfaceEvoDst[eye] = pSurfaceEvoSrc[eye];
    }
}

static NvBool HsIoctlFlipValidateOneHwState(
    const NVHsLayerRequestedFlipState *pHwState,
    const NvU32 sd)
{
    /* The semaphore surface must have a CPU mapping. */

    if (!pHwState->syncObject.usingSyncpt) {
        if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo != NULL) {
            if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo->cpuAddress[sd] == NULL) {
                return FALSE;
            }
        }

        if (pHwState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo != NULL) {
            if (pHwState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo->cpuAddress[sd] == NULL) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*!
 * Assign NVHsRequestedFlipState.
 *
 * Return TRUE if the NVHsRequestedFlipState could be assigned and is valid for use by
 * headSurface.
 */
static NvBool HsIoctlFlipAssignHwStateOneHead(
    NVHsChannelEvoRec *pHsChannel,
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 apiHead,
    const struct NvKmsPerOpenDev *pOpenDev,
    const struct NvKmsFlipCommonParams *pParams,
    NVHsRequestedFlipState *pFlipState)
{
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);
    NvU32 layer;

    nvAssert(pHsChannel != NULL);

    /* Init pFlipState using current pHsChannel state. */

    nvkms_memset(pFlipState, 0, sizeof(*pFlipState));

    pFlipState->cursor = pHsChannel->config.cursor;

    pFlipState->viewPortPointIn.x = pHsChannel->config.viewPortIn.x;
    pFlipState->viewPortPointIn.y = pHsChannel->config.viewPortIn.y;

    for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
        pFlipState->layer[layer] = *HsGetLastFlipQueueEntry(pHsChannel, layer);
    }

    /* Apply pParams to pFlipState. */
    if (!nvCheckFlipPermissions(pOpenDev, pDevEvo, sd, apiHead, pParams)) {
        return FALSE;
    }

    if (pParams->viewPortIn.specified) {
        pFlipState->dirty.viewPortPointIn = TRUE;
        pFlipState->viewPortPointIn = pParams->viewPortIn.point;
    }

    if (pParams->cursor.imageSpecified) {
        if (!nvAssignCursorSurface(pOpenDev, pDevEvo, &pParams->cursor.image,
                                   &pFlipState->cursor.pSurfaceEvo)) {
            return FALSE;
        }

        if (pFlipState->cursor.pSurfaceEvo != NULL) {
            pFlipState->cursor.cursorCompParams =
                pParams->cursor.image.cursorCompParams;
        }

        pFlipState->dirty.cursorSurface = TRUE;
    }

    if (pParams->cursor.positionSpecified) {
        pFlipState->cursor.x = pParams->cursor.position.x;
        pFlipState->cursor.y = pParams->cursor.position.y;

        pFlipState->dirty.cursorPosition = TRUE;
    }

    for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
        if (pParams->layer[layer].surface.specified) {
            NvBool ret =
                nvAssignSurfaceArray(pDevEvo,
                                     pOpenDevSurfaceHandles,
                                     pParams->layer[layer].surface.handle,
                                     FALSE /* isUsedByCursorChannel */,
                                     TRUE /* isUsedByLayerChannel */,
                                     pFlipState->layer[layer].pSurfaceEvo);
            if (!ret) {
                return FALSE;
            }

            pFlipState->dirty.layer[layer] = TRUE;
        }

        if (pParams->layer[layer].syncObjects.specified) {
            NvBool ret;

            if (pParams->layer[layer].syncObjects.val.useSyncpt) {
                return FALSE;
            }

            nvkms_memset(&pFlipState->layer[layer].syncObject,
                         0,
                         sizeof(pFlipState->layer[layer].syncObject));

            ret = nvAssignSemaphoreEvoHwState(pDevEvo,
                                              pOpenDevSurfaceHandles,
                                              layer,
                                              sd,
                                              &pParams->layer[layer].syncObjects.val,
                                              &pFlipState->layer[layer].syncObject);
            if (!ret) {
                return FALSE;
            }

            pFlipState->dirty.layer[layer] = TRUE;
        }

        if (pParams->layer[layer].completionNotifier.specified &&
            (pParams->layer[layer].completionNotifier.val.surface.surfaceHandle != 0)) {

            /*
             * HeadSurface only supports client notifiers when running in
             * swapgroup mode where each flip IOCTL will result in a real
             * flip in HW.
             */
            if (!pHsChannel->config.neededForSwapGroup) {
                return FALSE;
            }

            NvBool ret = nvAssignCompletionNotifierEvoHwState(
                      pDevEvo,
                      pOpenDevSurfaceHandles,
                      &pParams->layer[layer].completionNotifier.val,
                      layer,
                      &pFlipState->layer[layer].completionNotifier);
            if (!ret) {
                return FALSE;
            }

            pFlipState->dirty.layer[layer] = TRUE;
        }

        /* HeadSurface does not support timeStamp flips, yet. */
        if (pParams->layer[layer].timeStamp != 0) {
            return FALSE;
        }
    }

    /* XXX Reject all unhandled flip parameters */

    /* Validate that the requested changes can be performed by headSurface. */
    for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
        if (!pFlipState->dirty.layer[layer]) {
            continue;
        }

        if (!HsIoctlFlipValidateOneHwState(&pFlipState->layer[layer], sd)) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Update headSurface for NVKMS_IOCTL_FLIP.
 *
 * XXX NVKMS HEADSURFACE TODO: handle/validate the rest of the flip request
 * structure.
 */
NvBool nvHsIoctlFlip(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsPerOpenDev *pOpenDev,
    const struct NvKmsFlipRequestOneHead *pFlipHead,
    NvU32 numFlipHeads,
    NvBool commit,
    NvBool allowVrr,
    struct NvKmsFlipReply *pReply)
{
    NvU32 i;
    ct_assert((NV_MAX_FLIP_REQUEST_HEADS % 8) == 0);
    NvU8 hsMask[NV_MAX_FLIP_REQUEST_HEADS / 8] = { };
    NvU32 nHsApiHeads = 0;
    NvU32 nNonHsApiHeads = 0;
    NvBool ret = FALSE;

    struct {

        NVHsRequestedFlipState flipState
            [NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP];

    } *pWorkArea = NULL;

    /* Take inventory of which heads are touched by the request. */

    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 apiHead = pFlipHead[i].head;
        const NvU32 sd = pFlipHead[i].sd;
        NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[sd];

        if (pDispEvo->pHsChannel[apiHead] == NULL) {
            nNonHsApiHeads++;
        } else {
            hsMask[i / 8] |= NVBIT(i % 8);
            nHsApiHeads++;
        }
    }
    nvAssert(numFlipHeads == nNonHsApiHeads + nHsApiHeads);

    /*
     * Handle the common case: if there are no headSurface-ful heads touched by
     * the request, call down and return.
     */
    if (nHsApiHeads == 0) {
        ret = nvFlipEvo(pDevEvo,
                        pOpenDev,
                        pFlipHead, numFlipHeads, commit, allowVrr,
                        pReply,
                        FALSE /* skipUpdate */,
                        TRUE /* allowFlipLock */);
        goto done;
    }

    pWorkArea = nvCalloc(1, sizeof(*pWorkArea));
    if (pWorkArea == NULL) {
        goto done;
    }

    /*
     * Assign and validate flipState for any headSurface heads in the
     * request.
     */
    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 apiHead = pFlipHead[i].head;
        const NvU32 sd = pFlipHead[i].sd;
        NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[sd];

        if ((hsMask[i / 8] & NVBIT(i % 8)) == 0) {
            continue;
        }

        if (!HsIoctlFlipAssignHwStateOneHead(
                pDispEvo->pHsChannel[apiHead],
                pDevEvo,
                sd,
                apiHead,
                pOpenDev,
                &pFlipHead[i].flip,
                &pWorkArea->flipState[sd][apiHead])) {
            goto done;
        }
    }

    /*
     * If we got this far, we know there are headSurface-ful heads.  If there
     * are also headSurface-less heads, build a new request structure with the
     * headSurface-ful heads removed and call down.
     */

    if (nNonHsApiHeads != 0) {

        NvBool tmp;
        struct NvKmsFlipRequestOneHead *pFlipHeadLocal =
            HsIoctlRemoveHsHeadsFromNvKmsFlipHead(
                pDevEvo, pFlipHead, numFlipHeads, nNonHsApiHeads, hsMask);

        if (pFlipHeadLocal == NULL) {
            goto done;
        }

        tmp = nvFlipEvo(pDevEvo,
                        pOpenDev,
                        pFlipHeadLocal, nNonHsApiHeads, commit, allowVrr,
                        pReply,
                        FALSE /* skipUpdate */,
                        TRUE /* allowFlipLock */);

        // nvFlipEvo filled in pReply for the heads in pFlipHeadLocal.
        // Move those replies to the right location for pFlipHead.
        //
        // Due to how HsIoctlRemoveHsHeadsFromNvKmsFlipHead() created
        // pFlipHeadLocal, the entries will be in the same order as the
        // original pFlipHead request, but some of the entries have been
        // removed so the original array is longer.
        //
        // Iterate backwards through the local array (headLocal), which points
        // to where the reply data was filled in by nvFlipEvo().
        // Keep an index into the original array (headOriginal) which points to
        // the entry where the reply *should* be.  This should always be >=
        // headLocal.
        // If the expected location for the reply is not the same as the local
        // index, copy the reply to the right location and clear the local data
        // (which was in the wrong place).
        {
            NvS32 headOriginal = numFlipHeads - 1;
            NvS32 headLocal;
            for (headLocal = nNonHsApiHeads - 1; headLocal >= 0; headLocal--) {
                while (pFlipHead[headOriginal].sd != pFlipHeadLocal[headLocal].sd ||
                       pFlipHead[headOriginal].head != pFlipHeadLocal[headLocal].head) {
                    headOriginal--;
                    nvAssert(headOriginal >= 0);
                }
                if (headOriginal != headLocal) {
                    nvAssert(headOriginal > headLocal);
                    pReply->flipHead[headOriginal] = pReply->flipHead[headLocal];
                    nvkms_memset(&pReply->flipHead[headLocal], 0,
                                 sizeof(pReply->flipHead[headLocal]));
                }
            }
        }

        nvFree(pFlipHeadLocal);

        if (!tmp) {
            goto done;
        }
    }

    /* We cannot fail beyond this point. */

    ret = TRUE;


    /* If this is a validation-only request, we are done. */

    if (!commit) {
        goto done;
    }

    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 apiHead = pFlipHead[i].head;
        const NvU32 sd = pFlipHead[i].sd;
        NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[sd];

        if ((hsMask[i / 8] & NVBIT(i % 8)) == 0) {
            continue;
        }

        NVHsChannelEvoRec *pHsChannel = pDispEvo->pHsChannel[apiHead];
        const struct NvKmsFlipCommonParams *pParams =
            &pFlipHead[i].flip;
        NVHsRequestedFlipState *pFlipState =
            &pWorkArea->flipState[sd][apiHead];

        if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
            continue;
        }

        if (pParams->layer[NVKMS_MAIN_LAYER].skipPendingFlips &&
            pFlipState->dirty.layer[NVKMS_MAIN_LAYER]) {
            nvHsIdleFlipQueue(pHsChannel, TRUE /* force */);
        }
    }

    /* Finally, update the headSurface-ful heads in the request. */

    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 apiHead = pFlipHead[i].head;
        const NvU32 sd = pFlipHead[i].sd;
        NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[sd];

        if ((hsMask[i / 8] & NVBIT(i % 8)) == 0) {
            continue;
        }

        const NVHsRequestedFlipState *pFlipState =
            &pWorkArea->flipState[sd][apiHead];

        NVHsChannelEvoRec *pHsChannel = pDispEvo->pHsChannel[apiHead];
        NvU32 layer;

        nvAssert(pHsChannel != NULL);

        if (pFlipState->dirty.cursorPosition) {
            HsIoctlMoveCursor(
                pHsChannel,
                pFlipState->cursor.x,
                pFlipState->cursor.y);
        }

        if (pFlipState->dirty.cursorSurface) {
            HsIoctlSetCursorImage(
                pHsChannel,
                pFlipState->cursor.pSurfaceEvo);
        }

        if (pFlipState->dirty.viewPortPointIn) {
            HsIoctlPan(pHsChannel, &pFlipState->viewPortPointIn);
        }

        /*
         * XXX NVKMS HEADSURFACE TODO: Layers that are specified as part
         * of the same NVKMS_IOCTL_FLIP request should be flipped
         * atomically.  But, layers that are specified separately should
         * be allowed to flip separately.  Update the headSurface flip
         * queue handling to coordinate multi-layer atomic flips.
         */
        for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
            if (!pFlipState->dirty.layer[layer]) {
                continue;
            }

            if (layer == NVKMS_MAIN_LAYER) {
                HsIoctlAssignSurfacesMaxEyes(
                    pHsChannel->flipQueueMainLayerState.pSurfaceEvo,
                    pFlipState->layer[layer].pSurfaceEvo);
            }

            nvHsPushFlipQueueEntry(pHsChannel, layer, &pFlipState->layer[layer]);

            if (pHsChannel->config.neededForSwapGroup) {
                pHsChannel->swapGroupFlipping = NV_TRUE;
            }
        }
    }

done:

    nvFree(pWorkArea);

    return ret;
}

