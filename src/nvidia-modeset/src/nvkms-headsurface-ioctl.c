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
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-ioctl.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-cursor.h"
#include "nvkms-utils.h"
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

/*!
 * Assign headSurface bitmasks.
 *
 * Given requestedHeadsBitMask and a disp, assign a head bitmasks of the
 * headSurface-ful heads and headSurface-less heads.
 *
 * Fail if requestedHeadsBitMask contains invalid heads.
 */
static NvBool HsIoctlGetBitMasks(
    const NVDispEvoRec *pDispEvo,
    const NvU32 requestedHeadsBitMask,
    NvU32 *pHsMask,
    NvU32 *pNonHsMask)
{
    NvU32 hsMask = 0;
    NvU32 nonHsMask = 0;
    NvU32 head;

    if (nvHasBitAboveMax(requestedHeadsBitMask, pDispEvo->pDevEvo->numHeads)) {
        return FALSE;
    }

    FOR_EACH_INDEX_IN_MASK(32, head, requestedHeadsBitMask) {
        const NvU32 apiHead = nvHardwareHeadToApiHead(head);
        if (pDispEvo->pHsChannel[apiHead] == NULL) {
            nonHsMask |= NVBIT(head);
        } else {
            hsMask |= NVBIT(head);
        }

    } FOR_EACH_INDEX_IN_MASK_END;

    /*
     * Each bit from the original mask should be in exactly one of hsMask or
     * nonHsMask.
     */
    nvAssert((hsMask | nonHsMask) == requestedHeadsBitMask);
    nvAssert((hsMask & nonHsMask) == 0);

    *pHsMask = hsMask;
    *pNonHsMask = nonHsMask;

    return TRUE;
}

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
    NvU32 head,
    const struct NvKmsMoveCursorCommonParams *pParams)
{
    const NvU32 apiHead = nvHardwareHeadToApiHead(head);
    NVHsChannelEvoRec *pHsChannel;

    if (apiHead > ARRAY_LEN(pDispEvo->pHsChannel)) {
        return FALSE;
    }

    pHsChannel = pDispEvo->pHsChannel[apiHead];

    /* If headSurface is not used on this head, call down. */

    if (pHsChannel == NULL) {
        nvEvoMoveCursor(pDispEvo, head, pParams);
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
    /*
     * Increment the refcnt of the new surface, and
     * decrement the refcnt of the old surface.
     *
     * XXX NVKMS HEADSURFACE TODO: wait until graphics channel is done using old
     * surface before decrementing its refcnt?
     */

    HsChangeSurfaceFlipRefCount(
        pSurfaceEvo, TRUE /* increase */);

    HsChangeSurfaceFlipRefCount(
        pHsChannel->config.cursor.pSurfaceEvo, FALSE /* increase */);

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
    NvU32 head,
    const struct NvKmsSetCursorImageCommonParams *pParams)
{
    const NvU32 apiHead = nvHardwareHeadToApiHead(head);
    NVHsChannelEvoRec *pHsChannel;
    NVSurfaceEvoRec *pSurfaceEvo = NULL;

    if (apiHead > ARRAY_LEN(pDispEvo->pHsChannel)) {
        return FALSE;
    }

    pHsChannel = pDispEvo->pHsChannel[apiHead];

    /* If headSurface is not used on this head, call down. */

    if (pHsChannel == NULL) {
        return nvSetCursorImage(pDispEvo,
                                pOpenDevice,
                                pOpenDevSurfaceHandles,
                                head,
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
 * Create a copy of NvKmsFlipRequest with the headSurface-ful heads removed.
 */
static struct NvKmsFlipRequest *HsIoctlRemoveHsHeadsFromNvKmsFlipRequest(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsFlipRequest *pRequestOriginal,
    const NvU32 hsMask[NVKMS_MAX_SUBDEVICES])
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, sd;

    struct NvKmsFlipRequest *pRequest = nvAlloc(sizeof(*pRequest));

    if (pRequest == NULL) {
        return FALSE;
    }

    *pRequest = *pRequestOriginal;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        FOR_EACH_INDEX_IN_MASK(32, head, hsMask[sd]) {

            nvkms_memset(&pRequest->sd[sd].head[head], 0,
                         sizeof(pRequest->sd[sd].head[head]));

            pRequest->sd[sd].requestedHeadsBitMask &= ~NVBIT(head);

        } FOR_EACH_INDEX_IN_MASK_END;
    }

    return pRequest;
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
    const NVFlipChannelEvoHwState *pHwState,
    const NvU32 sd)
{
    /* HeadSurface does not support completion notifiers, yet. */

    if ((pHwState->completionNotifier.surface.pSurfaceEvo != NULL) ||
        (pHwState->completionNotifier.awaken)) {
        return FALSE;
    }

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

    /* HeadSurface does not support timeStamp flips, yet. */

    if (pHwState->timeStamp != 0) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Assign NVFlipEvoHwState.
 *
 * Return TRUE if the NVFlipEvoHwState could be assigned and is valid for use by
 * headSurface.
 */
static NvBool HsIoctlFlipAssignHwStateOneHead(
    NVHsChannelEvoRec *pHsChannel,
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsPerOpenDev *pOpenDev,
    const struct NvKmsFlipCommonParams *pRequestOneHead,
    NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;
    const struct NvKmsUsageBounds *pPossibleUsage =
        &pDevEvo->gpus[sd].pDispEvo->headState[head].timings.viewPort.possibleUsage;

    nvAssert(pHsChannel != NULL);

    /* Init pFlipState using current pHsChannel state. */

    nvClearFlipEvoHwState(pFlipState);

    pFlipState->cursor = pHsChannel->config.cursor;

    pFlipState->viewPortPointIn.x = pHsChannel->config.viewPortIn.x;
    pFlipState->viewPortPointIn.y = pHsChannel->config.viewPortIn.y;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        pFlipState->layer[layer] = *HsGetLastFlipQueueEntry(pHsChannel, layer);
    }

    /* Apply pRequestOneHead to pFlipState. */

    if (!nvUpdateFlipEvoHwState(pOpenDev, pDevEvo, sd, head, pRequestOneHead,
                                pFlipState, FALSE /* allowVrr */)) {
        return FALSE;
    }

    nvOverrideScalingUsageBounds(pDevEvo, head, pFlipState, pPossibleUsage);

    /* Validate that the requested changes can be performed by headSurface. */
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
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
    const struct NvKmsFlipRequest *pRequest,
    struct NvKmsFlipReply *pReply)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;
    NvU32 nHsHeads = 0;
    NvU32 nNonHsHeads = 0;
    NvBool ret = FALSE;

    struct {
        NvU32 hsMask[NVKMS_MAX_SUBDEVICES];
        NvU32 nonHsMask[NVKMS_MAX_SUBDEVICES];

        NVFlipEvoHwState flipState
            [NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP];

    } *pWorkArea = nvCalloc(1, sizeof(*pWorkArea));

    if (pWorkArea == NULL) {
        goto done;
    }

    /* Take inventory of which heads are touched by the request. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        if (!HsIoctlGetBitMasks(pDispEvo,
                                pRequest->sd[sd].requestedHeadsBitMask,
                                &pWorkArea->hsMask[sd],
                                &pWorkArea->nonHsMask[sd])) {
            goto done;
        }

        nHsHeads += nvPopCount32(pWorkArea->hsMask[sd]);
        nNonHsHeads += nvPopCount32(pWorkArea->nonHsMask[sd]);
    }

    /*
     * Handle the common case: if there are no headSurface-ful heads touched by
     * the request, call down and return.
     */
    if (nHsHeads == 0) {
        ret = nvFlipEvo(pDevEvo,
                        pOpenDev,
                        pRequest,
                        pReply,
                        FALSE /* skipUpdate */,
                        TRUE /* allowFlipLock */);
        goto done;
    }

    /*
     * Assign and validate flipState for any headSurface heads in the
     * request.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        FOR_EACH_INDEX_IN_MASK(32, head, pWorkArea->hsMask[sd]) {
            const NvU32 apiHead = nvHardwareHeadToApiHead(head);
            if (!HsIoctlFlipAssignHwStateOneHead(
                    pDispEvo->pHsChannel[apiHead],
                    pDevEvo,
                    sd,
                    head,
                    pOpenDev,
                    &pRequest->sd[sd].head[head],
                    &pWorkArea->flipState[sd][head])) {
                goto done;
            }

        } FOR_EACH_INDEX_IN_MASK_END;
    }

    /*
     * If we got this far, we know there are headSurface-ful heads.  If there
     * are also headSurface-less heads, build a new request structure with the
     * headSurface-ful heads removed and call down.
     */

    if (nNonHsHeads != 0) {

        NvBool tmp;
        struct NvKmsFlipRequest *pRequestLocal =
            HsIoctlRemoveHsHeadsFromNvKmsFlipRequest(
                pDevEvo, pRequest, pWorkArea->hsMask);

        if (pRequestLocal == NULL) {
            goto done;
        }

        tmp = nvFlipEvo(pDevEvo,
                        pOpenDev,
                        pRequestLocal,
                        pReply,
                        FALSE /* skipUpdate */,
                        TRUE /* allowFlipLock */);

        nvFree(pRequestLocal);

        if (!tmp) {
            goto done;
        }
    }

    /* We cannot fail beyond this point. */

    ret = TRUE;


    /* If this is a validation-only request, we are done. */

    if (!pRequest->commit) {
        goto done;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        FOR_EACH_INDEX_IN_MASK(32, head, pWorkArea->hsMask[sd]) {
            const NvU32 apiHead = nvHardwareHeadToApiHead(head);
            NVHsChannelEvoRec *pHsChannel = pDispEvo->pHsChannel[apiHead];
            const struct NvKmsFlipCommonParams *pParams =
                &pRequest->sd[sd].head[head];
            NVFlipEvoHwState *pFlipState =
                &pWorkArea->flipState[sd][head];

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            if (pParams->layer[NVKMS_MAIN_LAYER].skipPendingFlips &&
                pFlipState->dirty.layer[NVKMS_MAIN_LAYER]) {
                nvHsIdleFlipQueue(pHsChannel, TRUE /* force */);
            }
        } FOR_EACH_INDEX_IN_MASK_END;
    }

    /* Finally, update the headSurface-ful heads in the request. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        FOR_EACH_INDEX_IN_MASK(32, head, pWorkArea->hsMask[sd]) {
            const NvU32 apiHead = nvHardwareHeadToApiHead(head);
            const NVFlipEvoHwState *pFlipState =
                &pWorkArea->flipState[sd][head];

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
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
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
        } FOR_EACH_INDEX_IN_MASK_END;
    }

done:

    nvFree(pWorkArea);

    return ret;
}

