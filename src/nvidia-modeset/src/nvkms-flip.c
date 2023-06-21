/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-dma.h"
#include "nvkms-evo.h"
#include "nvkms-flip.h"
#include "nvkms-hw-flip.h"
#include "nvkms-utils-flip.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-vrr.h"
#include "nvkms-dpy.h"
#include "nvkms-rm.h"

/*!
 * Check whether the flipPermissions for pOpenDev allow the flipping
 * requested by NvKmsFlipCommonParams.
 */
NvBool nvCheckFlipPermissions(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 apiHead,
    const struct NvKmsFlipCommonParams *pParams)
{
    const int dispIndex = pDevEvo->gpus[sd].pDispEvo->displayOwner;
    const struct NvKmsFlipPermissions *pFlipPermissions =
        nvGetFlipPermissionsFromOpenDev(pOpenDev);
    const struct NvKmsModesetPermissions *pModesetPermissions =
        nvGetModesetPermissionsFromOpenDev(pOpenDev);
    const NvU8 allLayersMask = NVBIT(pDevEvo->apiHead[apiHead].numLayers) - 1;
    NvU8 layerMask = 0;
    NvU32 layer;

    nvAssert(pOpenDev != NULL);
    nvAssert(pFlipPermissions != NULL);
    nvAssert(pModesetPermissions != NULL);

    layerMask = pFlipPermissions->disp[dispIndex].head[apiHead].layerMask;

    /*
     * If the client has modeset permissions for this disp+head, allow
     * the client to also perform flips on any layer.
     */
    if (!nvDpyIdListIsEmpty(pModesetPermissions->disp[dispIndex].
                            head[apiHead].dpyIdList)) {
        layerMask = allLayersMask;
    }

    /* Changing viewPortIn requires permission to alter all layers. */

    if (pParams->viewPortIn.specified && (layerMask != allLayersMask)) {
        return FALSE;
    }

    for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
        if (nvIsLayerDirty(pParams, layer) && ((layerMask & NVBIT(layer)) == 0)) {
            return FALSE;
        }
    }

    return TRUE;
}

static void FillPostSyncptReplyOneApiHead(
    NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 apiHead,
    const struct NvKmsFlipCommonParams *pRequestParams,
    struct NvKmsFlipCommonReplyOneHead *pReplyParams,
    const struct NvKmsFlipWorkArea *pWorkArea)
{
    /* XXX[2Heads1OR] Return per hardware-head post syncpt */
    const NvU32 head = nvGetPrimaryHwHead(pDevEvo->gpus[sd].pDispEvo, apiHead);
    NvU32 layer;

    /*! check for valid config */
    if ((head == NV_INVALID_HEAD) || !pDevEvo->supportsSyncpts) {
        return;
    }

    for (layer = 0; layer < ARRAY_LEN(pRequestParams->layer); layer++) {
        const NVFlipEvoHwState *pFlipState =
            &pWorkArea->sd[sd].head[head].newState;

        if (!pRequestParams->layer[layer].syncObjects.specified ||
            !pRequestParams->layer[layer].syncObjects.val.useSyncpt) {
            continue;
        }

        nvFillPostSyncptReplyOneChannel(
            pDevEvo->head[head].layer[layer],
            pRequestParams->layer[layer].syncObjects.val.u.syncpts.requestedPostType,
            &pReplyParams->layer[layer].postSyncpt,
            &pFlipState->layer[layer].syncObject);
    }
}


static NvBool UpdateProposedFlipStateOneApiHead(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    const struct NvKmsFlipCommonParams *pParams,
    NVProposedFlipStateOneApiHead *pProposedApiHead)
{
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];

    if (pParams->tf.specified) {
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);

        pProposedApiHead->dirty.hdr = TRUE;
        pProposedApiHead->hdr.tf = pParams->tf.val;

        // If enabling HDR...
        // XXX HDR TODO: Handle other transfer functions
        if (pParams->tf.val == NVKMS_OUTPUT_TF_PQ) {
            // Cannot be an SLI configuration.
            // XXX HDR TODO: Test SLI Mosaic + HDR and remove this check
            if (pDispEvo->pDevEvo->numSubDevices > 1) {
                return FALSE;
            }

            // Sink must support HDR.
            if (!nvIsHDRCapableHead(pDispEvo, apiHead)) {
                return FALSE;
            }

            /* NVKMS_OUTPUT_TF_PQ requires the RGB color space */
            if (pProposedApiHead->hdr.colorSpace !=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) {
                return FALSE;
            }
        }

        if (!nvChooseColorRangeEvo(pParams->tf.val,
                                   pDpyEvo->requestedColorRange,
                                   pProposedApiHead->hdr.colorSpace,
                                   pProposedApiHead->hdr.colorBpc,
                                   &pProposedApiHead->hdr.colorRange)) {
            return FALSE;
        }
    }

    if (pParams->viewPortIn.specified) {
        pProposedApiHead->dirty.viewPortPointIn = TRUE;
        pProposedApiHead->viewPortPointIn = pParams->viewPortIn.point;
    }

    return TRUE;
}

void nvCancelSDRTransitionTimer(NVDispApiHeadStateEvoRec *pApiHeadState)
{
    nvkms_free_timer(pApiHeadState->hdrToSdrTransitionTimer);
    pApiHeadState->hdrToSdrTransitionTimer = NULL;
}

static void SDRTransition(void *dataPtr, NvU32 apiHead)
{
    NvU32 head;
    NVDispEvoRec *pDispEvo = dataPtr;
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NVDpyEvoPtr pDpyEvo =
        nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);

    nvCancelSDRTransitionTimer(pApiHeadState);

    if (pDpyEvo == NULL) {
        return;
    }

    nvAssert(pApiHeadState->hwHeadsMask != 0);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        nvAssert(pHeadState->hdr.outputState ==
                 NVKMS_HDR_OUTPUT_STATE_TRANSITIONING_TO_SDR);
        pHeadState->hdr.outputState = NVKMS_HDR_OUTPUT_STATE_SDR;
    }

    nvUpdateInfoFrames(pDpyEvo);
}

static
void ScheduleSDRTransitionTimer(NVDispEvoRec *pDispEvo, const NvU32 apiHead)
{
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];

    nvAssert(!pApiHeadState->hdrToSdrTransitionTimer);

    pApiHeadState->hdrToSdrTransitionTimer =
        nvkms_alloc_timer(SDRTransition,
                          pDispEvo,
                          apiHead,
                          2000000 /* 2 seconds */);
}

static NvBool GetAllowVrr(const NVDevEvoRec *pDevEvo,
                          const struct NvKmsFlipRequestOneHead *pFlipHead,
                          NvU32 numFlipHeads,
                          NvBool allowVrr,
                          NvBool *pApplyAllowVrr)
{
    NvU32 sd, i;
    const NVDispEvoRec *pDispEvo;
    const NvU32 requestedApiHeadCount = numFlipHeads;
    NvU32 activeApiHeadCount, dirtyMainLayerCount;

    *pApplyAllowVrr = FALSE;

    /*!
     * Count active heads so we can make a decision about VRR
     * and register syncpts if specified.
     */
    activeApiHeadCount = dirtyMainLayerCount = 0;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (nvApiHeadIsActive(pDispEvo, apiHead)) {
                activeApiHeadCount++;
            }
        }
    }

    for (i = 0; i < numFlipHeads; i++) {
        if (nvIsLayerDirty(&pFlipHead[i].flip, NVKMS_MAIN_LAYER)) {
            dirtyMainLayerCount++;
        }
    }

    /*
     * Deactivate VRR if only a subset of the heads are requested or
     * only a subset of the heads are being flipped.
     */
    if ((activeApiHeadCount != requestedApiHeadCount) ||
            (activeApiHeadCount != dirtyMainLayerCount)) {
        allowVrr = FALSE;
    }

    /*
     * Apply NvKmsFlipRequest::allowVrr
     * only if at least one main layer is became dirty.
     */
    if (dirtyMainLayerCount > 0) {
        *pApplyAllowVrr = TRUE;
    }

    return allowVrr;
}

static void FillNvKmsFlipReply(NVDevEvoRec *pDevEvo,
                               struct NvKmsFlipWorkArea *pWorkArea,
                               const NvBool applyAllowVrr,
                               const NvS32 vrrSemaphoreIndex,
                               const struct NvKmsFlipRequestOneHead *pFlipHead,
                               NvU32 numFlipHeads,
                               struct NvKmsFlipReply *reply)
{
    NvU32 i;

    if (reply == NULL) {
        return;
    }

    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 sd = pFlipHead[i].sd;
        const NvU32 apiHead = pFlipHead[i].head;

        FillPostSyncptReplyOneApiHead(pDevEvo,
                                      sd,
                                      apiHead,
                                      &pFlipHead[i].flip,
                                      &reply->flipHead[i],
                                      pWorkArea);
    }

    if (applyAllowVrr) {
        reply->vrrFlipType = nvGetActiveVrrType(pDevEvo);
        reply->vrrSemaphoreIndex = vrrSemaphoreIndex;
    } else {
        reply->vrrFlipType = NV_KMS_VRR_FLIP_NON_VRR;
        reply->vrrSemaphoreIndex = -1;
    }
}

static void InitNvKmsFlipWorkArea(const NVDevEvoRec *pDevEvo,
                                  struct NvKmsFlipWorkArea *pWorkArea)
{
    const NVDispEvoRec *pDispEvo;
    NvU32 sd, head, apiHead;

    nvkms_memset(pWorkArea, 0, sizeof(*pWorkArea));

    /*
     * Initialize the work area. Note we take two snapshots of the
     * current headState: newState and oldState.  newState will
     * describe the new configuration.  After that is applied, we will
     * refer to oldState to identify any surfaces that are no longer
     * in use.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (head = 0; head < ARRAY_LEN(pWorkArea->sd[sd].head); head++) {
            nvInitFlipEvoHwState(pDevEvo, sd, head,
                                 &pWorkArea->sd[sd].head[head].newState);
            nvInitFlipEvoHwState(pDevEvo, sd, head,
                                 &pWorkArea->sd[sd].head[head].oldState);
        }

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            NVProposedFlipStateOneApiHead *pProposedApiHead =
                &pWorkArea->disp[sd].apiHead[apiHead].proposedFlipState;
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];

            pProposedApiHead->hdr.tf = pApiHeadState->tf;
            pProposedApiHead->hdr.colorSpace =
                pApiHeadState->attributes.colorSpace;
            pProposedApiHead->hdr.colorBpc =
                pApiHeadState->attributes.colorBpc;
            pProposedApiHead->hdr.colorRange =
                pApiHeadState->attributes.colorRange;

            pProposedApiHead->viewPortPointIn =
                pApiHeadState->viewPortPointIn;
        }
    }
}

static void FlipEvoOneApiHead(NVDispEvoRec *pDispEvo,
                              const NvU32 apiHead,
                              const struct NvKmsFlipWorkArea *pWorkArea,
                              const NvBool allowFlipLock,
                              NVEvoUpdateState *pUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NvU32 head;
    const NVProposedFlipStateOneApiHead *pProposedApiHead =
        &pWorkArea->disp[sd].apiHead[apiHead].proposedFlipState;
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);
    const NVT_EDID_INFO *pInfo = &pDpyEvo->parsedEdid.info;
    const NVT_HDR_STATIC_METADATA *pHdrInfo =
        &pInfo->hdr_static_metadata_info;

    nvAssert(nvApiHeadIsActive(pDispEvo, apiHead));

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvFlipEvoOneHead(pDevEvo, sd, head, pHdrInfo,
                         &pWorkArea->sd[sd].head[head].newState,
                         allowFlipLock,
                         pUpdateState);

        if (pProposedApiHead->dirty.hdr) {
            /* Update hardware's current colorSpace and colorRange */
            nvUpdateCurrentHardwareColorSpaceAndRangeEvo(
                pDispEvo,
                head,
                pProposedApiHead->hdr.colorSpace,
                pProposedApiHead->hdr.colorRange,
                pUpdateState);
        }
    }

    if (pProposedApiHead->dirty.hdr) {
        pApiHeadState->attributes.colorSpace =
            pProposedApiHead->hdr.colorSpace;
        pApiHeadState->attributes.colorBpc =
            pProposedApiHead->hdr.colorBpc;
        pApiHeadState->attributes.colorRange =
            pProposedApiHead->hdr.colorRange;

        if (pProposedApiHead->hdr.tf == NVKMS_OUTPUT_TF_PQ) {
            nvCancelSDRTransitionTimer(pApiHeadState);
        } else if (pApiHeadState->tf == NVKMS_OUTPUT_TF_PQ) {
            ScheduleSDRTransitionTimer(pDispEvo, apiHead);
        }

        pApiHeadState->tf = pProposedApiHead->hdr.tf;

        nvUpdateInfoFrames(pDpyEvo);
    }

    if (pProposedApiHead->dirty.viewPortPointIn) {
        pApiHeadState->viewPortPointIn =
            pProposedApiHead->viewPortPointIn;
    }
}

static NvU32 FlipEvo2Head1OrOneDisp(NVDispEvoRec *pDispEvo,
                                    struct NvKmsFlipWorkArea *pWorkArea,
                                    const NvBool skipUpdate)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 flip2Heads1OrApiHeadsMask = 0x0;

    for (NvU32 apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[apiHead];
        const NvBool b2Heads1Or =
            (nvPopCount32(pApiHeadState->hwHeadsMask) >= 2);

        if (!nvApiHeadIsActive(pDispEvo, apiHead) || !b2Heads1Or) {
            continue;
        }

        nvkms_memset(&pWorkArea->updateState, 0,
                     sizeof(pWorkArea->updateState));

        FlipEvoOneApiHead(pDispEvo, apiHead, pWorkArea,
                          TRUE /* allowFlipLock */, &pWorkArea->updateState);

        /*
         * If api-head is using 2Heads1OR mode then it can not be flip with
         * other ap-heads in a single update; because each api-head, which is
         * using 2Heads1OR mode, uses different fliplock group and kicking off
         * multiple fliplock groups as part of a single update call is not
         * supported yet.
         */
        pDevEvo->hal->Update(pDevEvo, &pWorkArea->updateState,
                             TRUE /* releaseElv */);
        nvAssert(!skipUpdate);

        flip2Heads1OrApiHeadsMask |= NVBIT(apiHead);
    }

    return flip2Heads1OrApiHeadsMask;
}

/*!
 * Program a flip on all requested layers on all requested heads on
 * all requested disps in NvKmsFlipRequest.
 *
 * /param[in]   skipUpdate      Update software state tracking, but don't kick
 *                              off or perform an UPDATE.
 *
 *                              Note that this should be used only when the
 *                              satellite channels (including the cursor) are
 *                              disabled -- only the core channel should be
 *                              displaying anything, and only the core surface
 *                              should be specified in a skipUpdate flip.
 * /param[in]   allowFlipLock    Whether this update should use fliplocked base
 *                               flips. This is used on nvdisplay to set the
 *                               interlock mask to include all fliplocked
 *                               channels if necessary.  This should currently
 *                               only be set when this flip was initiated
 *                               through NVKMS_IOCTL_FLIP.
 */
NvBool nvFlipEvo(NVDevEvoPtr pDevEvo,
                 const struct NvKmsPerOpenDev *pOpenDev,
                 const struct NvKmsFlipRequestOneHead *pFlipHead,
                 NvU32 numFlipHeads,
                 NvBool commit,
                 NvBool requestAllowVrr,
                 struct NvKmsFlipReply *reply,
                 NvBool skipUpdate,
                 NvBool allowFlipLock)
{
    NvS32 vrrSemaphoreIndex = -1;
    NvU32 apiHead, sd;
    NvBool applyAllowVrr = FALSE;
    NvBool ret = FALSE;
    NvBool changed = FALSE;
    NVDispEvoPtr pDispEvo;
    const NvBool allowVrr =
        GetAllowVrr(pDevEvo, pFlipHead, numFlipHeads,
                    requestAllowVrr, &applyAllowVrr);
    struct NvKmsFlipWorkArea *pWorkArea =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_FLIP_WORK_AREA,
                      sizeof(*pWorkArea));
    NvU32 i;

    /*
     * Do not execute NVKMS_IOCTL_FLIP if the display channel yet has not
     * been transitioned from vbios to driver. A modeset requires, to make
     * display channel transition from vbios to driver.
     *
     * The NVKMS client should do modeset before initiating
     * NVKMS_IOCTL_FLIP requests.
     */
    if (pDevEvo->coreInitMethodsPending) {
        goto done;
    }

    InitNvKmsFlipWorkArea(pDevEvo, pWorkArea);

    /* Validate the flip parameters and update the work area. */
    for (i = 0; i < numFlipHeads; i++) {
        const NvU32 apiHead = pFlipHead[i].head;
        const NvU32 sd = pFlipHead[i].sd;
        NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[sd];
        NvU32 head;
        const NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[apiHead];

        if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
            goto done;
        }

        if (!nvCheckFlipPermissions(pOpenDev, pDevEvo, sd, apiHead,
                                    &pFlipHead[i].flip)) {
            goto done;
        }

        if (!UpdateProposedFlipStateOneApiHead(
                pDispEvo,
                apiHead,
                &pFlipHead[i].flip,
                &pWorkArea->disp[sd].apiHead[apiHead].proposedFlipState)) {
            goto done;
        }

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
            if (!nvAssignNVFlipEvoHwState(pDevEvo,
                                          pOpenDev,
                                          sd,
                                          head,
                                          &pFlipHead[i].flip,
                                          allowVrr,
                                          &pWorkArea->sd[sd].head[head].newState)) {
                goto done;
            }
        }

        pWorkArea->sd[sd].changed = TRUE;
        changed = TRUE;
    }

    /* If nothing changed, fail. */

    if (!changed) {
        goto done;
    }

    ret = nvAllocatePreFlipBandwidth(pDevEvo, pWorkArea);
    if (!ret) {
        goto done;
    }

    if (!commit) {
        ret = NV_TRUE;
        goto done;
    }

    if (!nvPrepareToDoPreFlip(pDevEvo, pWorkArea)) {
        goto done;
    }

    /*
     * At this point, something changed on at least one head of one
     * subdevice, and has been validated.  Apply the request to our
     * hardware and software state.  We must not fail beyond this
     * point.
     */

    ret = TRUE;

    nvPreFlip(pDevEvo, pWorkArea, applyAllowVrr, allowVrr, skipUpdate);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 flip2Heads1OrApiHeadsMask = 0x0;

        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        pDispEvo = pDevEvo->gpus[sd].pDispEvo;

        flip2Heads1OrApiHeadsMask =
            FlipEvo2Head1OrOneDisp(pDispEvo, pWorkArea, skipUpdate);

        nvkms_memset(&pWorkArea->updateState, 0,
                     sizeof(pWorkArea->updateState));

        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            if (!nvApiHeadIsActive(pDispEvo, apiHead) ||
                    ((NVBIT(apiHead) & flip2Heads1OrApiHeadsMask) != 0x0)) {
                continue;
            }

            FlipEvoOneApiHead(pDispEvo, apiHead, pWorkArea, allowFlipLock,
                              &pWorkArea->updateState);
        }

        if (!skipUpdate) {
            pDevEvo->hal->Update(pDevEvo, &pWorkArea->updateState,
                                 TRUE /* releaseElv */);
        }
    }

    nvPostFlip(pDevEvo, pWorkArea, skipUpdate, applyAllowVrr, &vrrSemaphoreIndex);

    FillNvKmsFlipReply(pDevEvo, pWorkArea, applyAllowVrr, vrrSemaphoreIndex,
                       pFlipHead, numFlipHeads, reply);

    /* fall through */

done:

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_FLIP_WORK_AREA);

    return ret;
}

void nvApiHeadGetLayerSurfaceArray(const NVDispEvoRec *pDispEvo,
                                   const NvU32 apiHead,
                                   const NvU32 layer,
                                   NVSurfaceEvoPtr pSurfaceEvos[NVKMS_MAX_EYES])
{

    const NvU32 sd = pDispEvo->displayOwner;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head, headCount;

    nvAssert(apiHead != NV_INVALID_HEAD);

    headCount = 0;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVEvoSubDevHeadStateRec *pSdHeadState =
            &pDispEvo->pDevEvo->gpus[sd].headState[head];
        NvU8 eye;

        if (headCount == 0) {
            for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
                pSurfaceEvos[eye] =
                    pSdHeadState->layer[layer].pSurfaceEvo[eye];
            }
        } else {
            for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {
                nvAssert(pSurfaceEvos[eye] ==
                    pSdHeadState->layer[layer].pSurfaceEvo[eye]);
            }
        }

        headCount++;
    }
}

void nvApiHeadGetCursorInfo(const NVDispEvoRec *pDispEvo,
                            const NvU32 apiHead,
                            NVSurfaceEvoPtr *ppSurfaceEvo,
                            NvS16 *x, NvS16 *y)
{

    const NvU32 sd = pDispEvo->displayOwner;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head, headCount;

    nvAssert(apiHead != NV_INVALID_HEAD);

    headCount = 0;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVEvoSubDevHeadStateRec *pSdHeadState =
            &pDispEvo->pDevEvo->gpus[sd].headState[head];

        if (headCount == 0) {
            *ppSurfaceEvo = pSdHeadState->cursor.pSurfaceEvo;
            *x = pSdHeadState->cursor.x;
            *y = pSdHeadState->cursor.y;
        } else {
            nvAssert(*ppSurfaceEvo == pSdHeadState->cursor.pSurfaceEvo);
            nvAssert(*x == pSdHeadState->cursor.x);
            nvAssert(*y == pSdHeadState->cursor.y);
        }

        headCount++;
    }
}

void nvApiHeadSetViewportPointIn(const NVDispEvoRec *pDispEvo,
                                 const NvU32 apiHead,
                                 const NvU16 x,
                                 const NvU16 y)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoUpdateState updateState = { };
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU16 hwViewportInWidth;
    NvU32 head, headCount;

    nvAssert(apiHead != NV_INVALID_HEAD);

    headCount = 0;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        const NVHwModeTimingsEvo *pTimings =
            &pHeadState->timings;

        if (headCount == 0) {
            hwViewportInWidth = pTimings->viewPort.in.width;
        } else {
            nvAssert(hwViewportInWidth == pTimings->viewPort.in.width);
        }

        nvPushEvoSubDevMaskDisp(pDispEvo);
        pDevEvo->hal->SetViewportPointIn(pDevEvo, head,
            x + (hwViewportInWidth * pHeadState->tilePosition), y,
            &updateState);
        nvPopEvoSubDevMask(pDevEvo);

        headCount++;
    }

    if (headCount != 0) {
        nvEvoUpdateAndKickOff(pDispEvo, FALSE /* sync */, &updateState,
                              TRUE /* releaseElv */);
    }
}

NvU32 nvApiHeadGetActiveViewportOffset(NVDispEvoRec *pDispEvo,
                                       NvU32 apiHead)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head, headCount;
    NvU32 offset = 0;

    nvAssert(apiHead != NV_INVALID_HEAD);

    headCount = 0;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        if (headCount == 0) {
            offset = pDevEvo->hal->GetActiveViewportOffset(pDispEvo, head);
        } else {
            nvAssert(offset == pDevEvo->hal->GetActiveViewportOffset(pDispEvo, head));
        }
        headCount++;
    }

    return offset;
}

void nvApiHeadIdleMainLayerChannels(NVDevEvoRec *pDevEvo,
    const NvU32 apiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES])
{
    NVEvoChannelMask idleChannelMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };
    const NVDispEvoRec *pDispEvo;
    NvU32 dispIndex, apiHead;
    NvBool found = FALSE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];
            NvU32 head;

            if ((apiHeadMaskPerSd[pDispEvo->displayOwner] &
                    NVBIT(apiHead)) == 0x0) {
                continue;
            }

            FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
                NVEvoChannelPtr pMainLayerChannel =
                    pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
                idleChannelMaskPerSd[pDispEvo->displayOwner] |=
                    pMainLayerChannel->channelMask;
                found = TRUE;
            }
        }
    }

    if (!found) {
        return;
    }

    nvIdleMainLayerChannels(pDevEvo, idleChannelMaskPerSd,
                            FALSE /* allowForceIdle */);
}

void nvApiHeadUpdateFlipLock(NVDevEvoRec *pDevEvo,
                             const NvU32 apiHeadMaskPerSd[NVKMS_MAX_SUBDEVICES],
                             const NvBool enable)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;
    NvU32 headMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };
    NVEvoChannelMask channelMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };
    NvBool found = FALSE;

    /* Determine which channels need to enable or disable fliplock. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];
            NvU32 head;

            if ((apiHeadMaskPerSd[pDispEvo->displayOwner] &
                    NVBIT(apiHead)) == 0x0) {
                continue;
            }

            FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
                NVEvoChannelPtr pMainLayerChannel =
                    pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];

                if (!nvNeedToToggleFlipLock(pDispEvo, head, enable)) {
                    continue;
                }

                if (enable) {
                    /*
                     * Override the prohibition of fliplock on pDispEvos with
                     * headsurface enabled (calculated earlier in
                     * HsConfigAllowFlipLock) to allow enabling fliplock for
                     * headSurface swapgroups.
                     */
                    nvAllowFlipLockEvo(pDispEvo, TRUE /* allowFlipLock */);
                }

                headMaskPerSd[pDispEvo->displayOwner] |= NVBIT(head);
                channelMaskPerSd[pDispEvo->displayOwner] |=
                    pMainLayerChannel->channelMask;
                found = TRUE;
            }
        }
    }

    if (!found) {
        return;
    }

    /*
     * Wait for all base channels that are enabling/disabling fliplock to be
     * idle.  This shouldn't timeout if we're enabling fliplock while bringing
     * up swapgroups on a new head.
     */
    nvIdleMainLayerChannels(pDevEvo, channelMaskPerSd, !enable /* forceIdle */);

    /* Now that all channels are idle, update fliplock. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvToggleFlipLockPerDisp(pDispEvo,
                                headMaskPerSd[pDispEvo->displayOwner],
                                enable);
    }
}

NvBool nvIdleMainLayerChannelCheckIdleOneApiHead(NVDispEvoPtr pDispEvo,
                                                 NvU32 apiHead)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head;

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        NVEvoChannelPtr pMainLayerChannel =
            pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
        NvBool isMethodPending = FALSE;
        NvBool ret;

        ret = pDevEvo->hal->IsChannelMethodPending(pDevEvo, pMainLayerChannel,
            pDispEvo->displayOwner, &isMethodPending);

        if (ret && isMethodPending) {
            return FALSE;
        }
    }

    return TRUE;
}

NvU32 nvCollectSurfaceUsageMaskOneApiHead(const NVDispEvoRec *pDispEvo,
                                          const NvU32 apiHead,
                                          NVSurfaceEvoPtr pSurfaceEvo)
{
    NvU32 usageMaskOneHead = 0x0;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head;

    if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
        return 0;
    }

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVEvoSubDevHeadStateRec *pSdHeadState =
            &pDevEvo->gpus[sd].headState[head];
        NvU32 layer;

        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            const NVFlipChannelEvoHwState *pLayerFlipState =
                &pSdHeadState->layer[layer];
            const NVFlipSyncObjectEvoHwState *pSyncObject =
                &pLayerFlipState->syncObject;
            NvU32 usageMaskOneLayer = 0x0;

            if ((pSurfaceEvo == pLayerFlipState->pSurfaceEvo[NVKMS_LEFT]) ||
                    (pSurfaceEvo == pLayerFlipState->pSurfaceEvo[NVKMS_RIGHT])) {
                usageMaskOneLayer = FLD_SET_DRF(_SURFACE, _USAGE_MASK_LAYER,
                    _SCANOUT, _ENABLE, usageMaskOneLayer);
            }

            if(pSurfaceEvo ==
                    pLayerFlipState->completionNotifier.surface.pSurfaceEvo) {
                usageMaskOneLayer = FLD_SET_DRF(_SURFACE, _USAGE_MASK_LAYER,
                    _NOTIFIER, _ENABLE, usageMaskOneLayer);
            }

            if ((!pLayerFlipState->syncObject.usingSyncpt) &&
                    (pSurfaceEvo ==
                     pSyncObject->u.semaphores.acquireSurface.pSurfaceEvo) &&
                    (pSurfaceEvo ==
                     pSyncObject->u.semaphores.releaseSurface.pSurfaceEvo)) {
                usageMaskOneLayer = FLD_SET_DRF(_SURFACE, _USAGE_MASK_LAYER,
                    _SEMAPHORE, _ENABLE, usageMaskOneLayer);
            }

            usageMaskOneHead = FLD_IDX_SET_DRF_NUM(_SURFACE, _USAGE_MASK,
                _LAYER, layer, usageMaskOneLayer, usageMaskOneHead);
        }

        if (pSurfaceEvo == pSdHeadState->cursor.pSurfaceEvo) {
            usageMaskOneHead = FLD_SET_DRF(_SURFACE, _USAGE_MASK,
                _CURSOR, _ENABLE, usageMaskOneHead);
        }
    }

    return usageMaskOneHead;
}

void nvIdleLayerChannels(NVDevEvoRec *pDevEvo,
    NvU32 layerMaskPerSdApiHead[NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP])
{
    NVEvoChannelMask channelMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };
    const NVDispEvoRec *pDispEvo;
    NvU32 sd;
    NvU64 startTime = 0;
    const NvU32 timeout = 500000; // .5 seconds
    NvBool allIdle;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];
            NvU32 head;

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
                for (NvU32 layer = 0;
                     layer < pDevEvo->head[head].numLayers; layer++) {
                    if ((NVBIT(layer) &
                            layerMaskPerSdApiHead[sd][apiHead]) != 0x0) {
                        channelMaskPerSd[sd] |=
                            pDevEvo->head[head].layer[layer]->channelMask;
                    }
                }
            }
        }
    }

    do {
        allIdle = TRUE;
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

            for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
                NvU32 layer;

                if (!nvHeadIsActive(pDispEvo, head)) {
                    continue;
                }

                for (layer = 0;
                     layer < pDevEvo->head[head].numLayers; layer++) {
                    NVEvoChannelPtr pLayerChannel =
                        pDevEvo->head[head].layer[layer];
                    NvBool isMethodPending;

                    if ((pLayerChannel->channelMask &
                            channelMaskPerSd[sd]) == 0x0) {
                        continue;
                    }

                    if (pDevEvo->hal->IsChannelMethodPending(pDevEvo,
                            pLayerChannel, sd, &isMethodPending) &&
                            isMethodPending) {

                        allIdle = FALSE;
                    } else {
                        /* This has been completed, no need to keep trying */
                        channelMaskPerSd[sd] &= ~pLayerChannel->channelMask;
                    }
                }
            }
        }

        if (!allIdle) {
            if (nvExceedsTimeoutUSec(&startTime, timeout)) {
                break;
            }
            nvkms_yield();
        }
    } while (!allIdle);

    /* If we timed out above, force things to be idle. */
    if (!allIdle) {
        NVEvoIdleChannelState idleChannelState = { };
        NvBool tryToForceIdle = FALSE;

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

            for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
                NvU32 layer;

                if (!nvHeadIsActive(pDispEvo, head)) {
                    continue;
                }

                for (layer = 0;
                     layer < pDevEvo->head[head].numLayers; layer++) {
                    NVEvoChannelPtr pLayerChannel =
                        pDevEvo->head[head].layer[layer];

                    if ((pLayerChannel->channelMask &
                            channelMaskPerSd[sd]) != 0x0) {
                        idleChannelState.subdev[sd].channelMask |=
                            pLayerChannel->channelMask;
                        tryToForceIdle = TRUE;
                    }
                }
            }
        }

        if (tryToForceIdle) {
            NvBool ret = pDevEvo->hal->ForceIdleSatelliteChannel(pDevEvo,
                &idleChannelState);
            if (!ret) {
                nvAssert(ret);
            }
        }
    }
}

/*
 * XXX NVKMS TODO
 * Make the sync more efficient: we only need to sync if the
 * in-flight methods flip away from this surface.
 */
void nvEvoClearSurfaceUsage(NVDevEvoRec *pDevEvo,
                            NVSurfaceEvoPtr pSurfaceEvo)
{
    NvU32 head;

    /*
     * If the core channel is no longer allocated, we don't need to
     * sync. This assumes the channels are allocated/deallocated
     * together.
     */
    if (pDevEvo->core) {

        if (pDevEvo->hal->ClearSurfaceUsage != NULL) {
            pDevEvo->hal->ClearSurfaceUsage(pDevEvo, pSurfaceEvo);
        }

        nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvU32 layer;

            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                NVEvoChannelPtr pChannel =
                    pDevEvo->head[head].layer[layer];

                nvRMSyncEvoChannel(pDevEvo, pChannel, __LINE__);
            }
        }
    }
}

NvBool nvIdleBaseChannelOneApiHead(NVDispEvoRec *pDispEvo, NvU32 apiHead,
                                   NvBool *pStoppedBase)
{
    NvBool ret = TRUE;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head;

    *pStoppedBase = FALSE;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        NvBool stoppedBase = FALSE;
        if (!nvRMIdleBaseChannel(pDevEvo, head,
                pDispEvo->displayOwner, &stoppedBase)) {
            ret = FALSE;
        } else if (stoppedBase) {
            *pStoppedBase = TRUE;
        }
    }

    return ret;
}
