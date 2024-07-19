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
#include "nvkms-hw-flip.h"
#include "nvkms-utils-flip.h"
#include "nvkms-flip-workarea.h"
#include "nvkms-surface.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-rm.h"
#include "nvkms-vrr.h"
#include "nvkms-cursor.h"
#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-lut.h"
#include "nvkms-softfloat.h"
#include "nvkms-ctxdma.h"

#include "nvkms-sync.h"

#include "nvkms-difr.h"

static void SchedulePostFlipIMPTimer(NVDevEvoPtr pDevEvo);

// The EVO .mfs file defines the maximum minPresentInterval to be 8.
#define NV_MAX_SWAP_INTERVAL 8

static NvBool AssignPreSyncptEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject)
{
    NvBool ret;
    NvU32 id = 0;
    NvU32 value;
    enum NvKmsSyncptType preType;

    nvAssert(pChannelSyncObjects->useSyncpt);

    preType = pChannelSyncObjects->u.syncpts.pre.type;

    if (preType == NVKMS_SYNCPT_TYPE_NONE) {
        return TRUE;
    }

    if (preType == NVKMS_SYNCPT_TYPE_FD) {
        /*! Get id from fd using nvhost API */
        NvKmsSyncPtOpParams params = { };
        params.fd_to_id_and_thresh.fd =
            pChannelSyncObjects->u.syncpts.pre.u.fd;
        ret = nvkms_syncpt_op(NVKMS_SYNCPT_OP_FD_TO_ID_AND_THRESH,
                              &params);
        if (!ret) {
            return FALSE;
        }
        id = params.fd_to_id_and_thresh.id;
        value = params.fd_to_id_and_thresh.thresh;
    } else {
        id  = pChannelSyncObjects->u.syncpts.pre.u.raw.id;
        value = pChannelSyncObjects->u.syncpts.pre.u.raw.value;
    }
    if (id >= NV_SYNCPT_GLOBAL_TABLE_LENGTH) {
        return FALSE;
    }
    /*! Fill pre-syncpt related information in hardware state */
    pFlipSyncObject->u.syncpts.preSyncpt = id;
    pFlipSyncObject->u.syncpts.preValue = value;
    pFlipSyncObject->u.syncpts.isPreSyncptSpecified = TRUE;
    pFlipSyncObject->usingSyncpt = TRUE;

    return TRUE;
}

static NvBool AssignPostSyncptEvoHwState(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel,
    const struct NvKmsChannelSyncObjects *pChannelSyncObjects,
    NVFlipSyncObjectEvoHwState *pFlipSyncObject)
{
    enum NvKmsSyncptType postType;
    NvU32 threshold;

    nvAssert(pChannelSyncObjects->useSyncpt);

    postType = pChannelSyncObjects->u.syncpts.requestedPostType;

    /*!
     * It is possible that syncpt is mentioned but post-syncpt
     * is not specified (case where only pre-syncpt used)
     */
    if (postType == NVKMS_SYNCPT_TYPE_NONE) {
        return TRUE;
    }

    /*! return threshold to caller but increase only when programming hw */
    threshold = pChannel->postSyncpt.syncptMaxVal + 1;

    /*! each channel associated with one post-syncpt */
    pFlipSyncObject->u.syncpts.surfaceDesc =
        pChannel->postSyncpt.surfaceDesc;
    pFlipSyncObject->u.syncpts.postValue = threshold;

    /*
     * AllocPostSyncptPerChannel()->AllocSyncpt() sets allocated to TRUE
     * when postSyncpt is allocated/valid.
     */
    pFlipSyncObject->u.syncpts.isPostSyncptSpecified =
        pChannel->postSyncpt.allocated;

    pFlipSyncObject->usingSyncpt = TRUE;

    return TRUE;
}

void nvFillPostSyncptReplyOneChannel(
    NVEvoChannel *pChannel,
    enum NvKmsSyncptType postType,
    struct NvKmsSyncpt *postSyncpt,
    const NVFlipSyncObjectEvoHwState *pHwSyncObject)
{
    if (postType == NVKMS_SYNCPT_TYPE_RAW) {
        postSyncpt->u.raw.id = pChannel->postSyncpt.id;
        postSyncpt->u.raw.value = pHwSyncObject->u.syncpts.postValue;
        postSyncpt->type = NVKMS_SYNCPT_TYPE_RAW;
    } else if (postType == NVKMS_SYNCPT_TYPE_FD) {
        NvBool ret = TRUE;
        NvKmsSyncPtOpParams params = { };
        params.id_and_thresh_to_fd.id = pChannel->postSyncpt.id;
        params.id_and_thresh_to_fd.thresh =
            pHwSyncObject->u.syncpts.postValue;

        ret = nvkms_syncpt_op(NVKMS_SYNCPT_OP_ID_AND_THRESH_TO_FD, &params);
        if (!ret) {
            nvAssert(!"Failed syncpt op ID_AND_THRESH_TO_FD");
            return;
        }
        postSyncpt->u.fd = params.id_and_thresh_to_fd.fd;
        postSyncpt->type = NVKMS_SYNCPT_TYPE_FD;
    }
}

void nvClearFlipEvoHwState(
    NVFlipEvoHwState *pFlipState)
{
    NvU32 i;

    nvkms_memset(pFlipState, 0, sizeof(*pFlipState));

    for (i = 0; i < ARRAY_LEN(pFlipState->layer); i++) {
        pFlipState->layer[i].cscMatrix = NVKMS_IDENTITY_CSC_MATRIX;
    }
}

/*!
 * Initialize NVFlipEvoHwState with a current snapshot from headState.
 */
void nvInitFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState)
{
    NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVEvoSubDevHeadStateRec *pSdHeadState;
    const NVDispHeadStateEvoRec *pHeadState;

    NvU32 i;

    nvClearFlipEvoHwState(pFlipState);

    if (!nvHeadIsActive(pDispEvo, head)) {
        return;
    }

    pSdHeadState = &pDevEvo->gpus[sd].headState[head];
    pHeadState = &pDispEvo->headState[head];

    pFlipState->viewPortPointIn = pSdHeadState->viewPortPointIn;
    pFlipState->cursor = pSdHeadState->cursor;

    ct_assert(ARRAY_LEN(pFlipState->layer) == ARRAY_LEN(pSdHeadState->layer));

    for (i = 0; i < ARRAY_LEN(pFlipState->layer); i++) {
        pFlipState->layer[i] = pSdHeadState->layer[i];
    }

    // pFlipState->usage describes the usage bounds that will be necessary after
    // this flip is complete.  Initialize it using pSdHeadState->targetUsage,
    // which describes the usage bounds that will be required just before this
    // flip occurs, rather than pSdHeadState->usage, which describes the usage
    // bounds currently programmed into the hardware.
    //
    // pSdHeadState->usage may have higher bounds than pSdHeadState->targetUsage
    // if TryLoweringUsageBounds has not yet noticed that a satellite channel is
    // no longer in use, or a flip to NULL in a satellite channel is pending but
    // has not yet occurred.
    pFlipState->usage = pSdHeadState->targetUsage;

    pFlipState->disableMidFrameAndDWCFWatermark =
        pSdHeadState->targetDisableMidFrameAndDWCFWatermark;

    pFlipState->tf = pHeadState->tf;

    pFlipState->hdrInfoFrame.enabled = pHeadState->hdrInfoFrameOverride.enabled;
    pFlipState->hdrInfoFrame.eotf = pHeadState->hdrInfoFrameOverride.eotf;
    pFlipState->hdrInfoFrame.staticMetadata =
        pHeadState->hdrInfoFrameOverride.staticMetadata;
}


NvBool nvIsLayerDirty(const struct NvKmsFlipCommonParams *pParams,
                      const NvU32 layer)
{
    return pParams->layer[layer].surface.specified ||
           pParams->layer[layer].sizeIn.specified ||
           pParams->layer[layer].sizeOut.specified ||
           pParams->layer[layer].outputPosition.specified ||
           pParams->layer[layer].completionNotifier.specified ||
           pParams->layer[layer].syncObjects.specified ||
           pParams->layer[layer].compositionParams.specified ||
           pParams->layer[layer].csc.specified ||
           pParams->layer[layer].hdr.specified ||
           pParams->layer[layer].colorSpace.specified;
}

/*!
 * Determine whether a base channel flip requires a non-tearing present mode.
 *
 * EVO requires a non-tearing flip when certain parameters are changing.  See
 * NV_DISP_BASE_STATE_ERROR_052 in dispClass024XBaseUpdateErrorChecks.mfs.
 */
static NvBool FlipRequiresNonTearingMode(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVFlipChannelEvoHwState *pOld,
    const NVFlipChannelEvoHwState *pNew)
{
    // TODO: Do we need to care about the right eye here?  The error check
    // doesn't.
    const NVSurfaceEvoRec *pOldSurf = pOld->pSurfaceEvo[NVKMS_LEFT];
    const NVSurfaceEvoRec *pNewSurf = pNew->pSurfaceEvo[NVKMS_LEFT];
    NvU32 oldHwFormat = 0, newHwFormat = 0;

    if (pOldSurf == NULL || pNewSurf == NULL) {
        return TRUE;
    }

    // If these functions actually return FALSE at this point, then something is
    // really wrong...
    if (!pDevEvo->hal->ValidateWindowFormat(
            pOldSurf->format, NULL, &oldHwFormat)) {
        nvAssert(FALSE);
    }

    if (!pDevEvo->hal->ValidateWindowFormat(
            pNewSurf->format, NULL, &newHwFormat)) {
        nvAssert(FALSE);
    }

    // Commented entries are things checked in the .mfs that are not yet
    // supported in NVKMS.
    return // SuperSample
           oldHwFormat != newHwFormat ||
           // Gamma
           // Layout (i.e. frame, field1, or field2)
           pOldSurf->widthInPixels != pNewSurf->widthInPixels ||
           pOldSurf->heightInPixels != pNewSurf->heightInPixels ||
           pOldSurf->layout != pNewSurf->layout;
           // UseGainOfs
           // NewBaseLut -- USE_CORE_LUT is programmed in InitChannel*
           // NewOutputLut
}


/*!
 * Apply flip overrides if necessary.
 *
 * 1. Override swap intervals for VRR.
 * 2. If the flip is changing certain parameters, override the tearing mode.
 */
static NvBool ApplyBaseFlipOverrides(
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    NVFlipChannelEvoHwState *pNew,
    NvBool allowVrr)
{
    const NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVFlipChannelEvoHwState *pOld =
        &pDevEvo->gpus[sd].headState[head].layer[NVKMS_MAIN_LAYER];

    // Apply VRR swap interval overrides.
    //
    // Note that this applies the overrides whenever the client requests VRR and
    // VRR is enabled, regardless of whether actually activating it later
    // succeeds.
    if (allowVrr) {
        if (!nvHeadIsActive(pDispEvo, head)) {
            //
            // XXX If VRR is allowed then modeset should have happened before
            // base channel flip, currently we don't know how to do modeset
            // and program base channel for VRR at same time. This should be
            // revisited as part of bug 1731279.
            //
            return FALSE;
        }
        nvApplyVrrBaseFlipOverrides(pDevEvo->gpus[sd].pDispEvo, head,
                                    pOld, pNew);
    }

    if (!nvHeadIsActive(pDispEvo, head)) {
        //
        // This is possible when modeset and base flip happening at same time,
        // tearing parameter does not make sense in that case,
        // it should is disabled.
        //
        pNew->tearing = FALSE;
    } else {
        // Force non-tearing mode if EVO requires it.
        if (FlipRequiresNonTearingMode(pDevEvo, head, pOld, pNew)) {
            pNew->tearing = FALSE;
        }
    }

    return TRUE;
}

static NvBool ValidateScalingUsageBounds(
    const struct NvKmsScalingUsageBounds *pS,
    const struct NvKmsScalingUsageBounds *pMaxS)
{
    return (pS->maxVDownscaleFactor <= pMaxS->maxVDownscaleFactor) &&
           (pS->maxHDownscaleFactor <= pMaxS->maxHDownscaleFactor) &&
           (pS->vTaps <= pMaxS->vTaps) &&
           (!pS->vUpscalingAllowed || pMaxS->vUpscalingAllowed);
}

/*!
 * Validate the requested usage bounds against the specified maximums.
 */
static NvBool ValidateUsageBounds(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const struct NvKmsUsageBounds *pUsage,
    const struct NvKmsUsageBounds *pGuaranteedUsage)
{
    NvU32 i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const NvU64 supportedSurfaceFormatsUnion =
            pUsage->layer[i].supportedSurfaceMemoryFormats |
            pGuaranteedUsage->layer[i].supportedSurfaceMemoryFormats;

        if ((pUsage->layer[i].usable && !pGuaranteedUsage->layer[i].usable) ||
            (supportedSurfaceFormatsUnion !=
             pGuaranteedUsage->layer[i].supportedSurfaceMemoryFormats) ||
            !ValidateScalingUsageBounds(&pUsage->layer[i].scaling,
                                        &pGuaranteedUsage->layer[i].scaling)) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Assign pFlipState->usage.
 */
static NvBool AssignUsageBounds(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState)
{
    struct NvKmsUsageBounds *pUsage = &pFlipState->usage;
    int i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const NVFlipChannelEvoHwState *pLayerFlipState = &pFlipState->layer[i];

        nvInitScalingUsageBounds(pDevEvo, &pUsage->layer[i].scaling);

        if (pLayerFlipState->pSurfaceEvo[NVKMS_LEFT]) {
            pUsage->layer[i].usable = TRUE;
            pUsage->layer[i].supportedSurfaceMemoryFormats =
                nvEvoGetFormatsWithEqualOrLowerUsageBound(
                    pLayerFlipState->pSurfaceEvo[NVKMS_LEFT]->format,
                    pDevEvo->caps.layerCaps[i].supportedSurfaceMemoryFormats);

            if (pDevEvo->hal->GetWindowScalingCaps) {
                const NVEvoScalerCaps *pScalerCaps =
                    pDevEvo->hal->GetWindowScalingCaps(pDevEvo);

                if (!nvComputeScalingUsageBounds(pScalerCaps,
                                                 pLayerFlipState->sizeIn.width,
                                                 pLayerFlipState->sizeIn.height,
                                                 pLayerFlipState->sizeOut.width,
                                                 pLayerFlipState->sizeOut.height,
                                                 pLayerFlipState->hTaps,
                                                 pLayerFlipState->vTaps,
                                                 &pUsage->layer[i].scaling)) {
                    return FALSE;
                }
            }

            if (pLayerFlipState->maxDownscaleFactors.specified) {
                struct NvKmsScalingUsageBounds *pTargetScaling =
                    &pFlipState->usage.layer[i].scaling;

                if ((pLayerFlipState->maxDownscaleFactors.vertical <
                        pTargetScaling->maxVDownscaleFactor) ||
                        (pLayerFlipState->maxDownscaleFactors.horizontal <
                            pTargetScaling->maxHDownscaleFactor)) {
                    return FALSE;
                }

                pTargetScaling->maxVDownscaleFactor =
                    pLayerFlipState->maxDownscaleFactors.vertical;
                pTargetScaling->maxHDownscaleFactor =
                    pLayerFlipState->maxDownscaleFactors.horizontal;
            }

        } else {
            pUsage->layer[i].usable = FALSE;
            pUsage->layer[i].supportedSurfaceMemoryFormats = 0;
        }
    }

    return TRUE;
}

void
nvOverrideScalingUsageBounds(const NVDevEvoRec *pDevEvo,
                             NvU32 head,
                             NVFlipEvoHwState *pFlipState,
                             const struct NvKmsUsageBounds *pPossibleUsage)
{
    NvU32 i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        const NVFlipChannelEvoHwState *pLayerFlipState = &pFlipState->layer[i];
        const struct NvKmsScalingUsageBounds *pPossibleScaling =
           &pPossibleUsage->layer[i].scaling;
        struct NvKmsScalingUsageBounds *pTargetScaling =
                &pFlipState->usage.layer[i].scaling;

        if (!pFlipState->usage.layer[i].usable) {
            continue;
        }

        if (!pLayerFlipState->maxDownscaleFactors.specified) {
            const NvU16 possibleV = pPossibleScaling->maxVDownscaleFactor;
            const NvU16 possibleH = pPossibleScaling->maxHDownscaleFactor;
            NvU16 targetV = pTargetScaling->maxVDownscaleFactor;
            NvU16 targetH = pTargetScaling->maxHDownscaleFactor;

            /*
             * Calculate max H/V downscale factor by quantizing the range.
             *
             * E.g.,
             * max H/V downscale factor supported by HW is 4x for 5-tap and 2x
             * for 2-tap mode. If 5-tap mode is required, the target usage bound
             * that nvkms will attempt to program will either allow up to 2x
             * downscaling, or up to 4x downscaling. If 2-tap mode is required,
             * the target usage bound that NVKMS will attempt to program will
             * allow up to 2x downscaling. Example: to downscale from 4096x2160
             * -> 2731x864 in 5-tap mode, NVKMS would specify up to 2x for the
             * H downscale bound (required is 1.5x), and up to 4x for the V
             * downscale bound (required is 2.5x).
             */
            if (targetV > NV_EVO_SCALE_FACTOR_1X) {
                const NvU16 possibleMid =
                    NV_EVO_SCALE_FACTOR_1X + ((possibleV - NV_EVO_SCALE_FACTOR_1X) / 2);

                if (targetV <= possibleMid) {
                    pTargetScaling->maxVDownscaleFactor = possibleMid;
                } else {
                    pTargetScaling->maxVDownscaleFactor = possibleV;
                }
            }

            if (targetH > NV_EVO_SCALE_FACTOR_1X) {
                const NvU16 possibleMid =
                    NV_EVO_SCALE_FACTOR_1X + ((possibleH - NV_EVO_SCALE_FACTOR_1X) / 2);

                if (targetH <= possibleMid) {
                    pTargetScaling->maxHDownscaleFactor = possibleMid;
                } else {
                    pTargetScaling->maxHDownscaleFactor = possibleH;
                }
            }
        }

        pTargetScaling->vTaps = pPossibleScaling->vTaps;
        pTargetScaling->vUpscalingAllowed = pPossibleScaling->vUpscalingAllowed;
    }
}

static NvBool FlipTimeStampValidForChannel(
    const NVEvoChannel *pChannel,
    NvU64 timeStamp)
{
    if (pChannel->caps.validTimeStampBits < 64) {
        const NvU64 validTimeStampMask =
            NVBIT64(pChannel->caps.validTimeStampBits) - 1;
        if ((timeStamp & ~validTimeStampMask) != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

static NvBool UpdateLayerFlipEvoHwStateHDRStaticMetadata(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState,
    NVFlipChannelEvoHwState *pHwState,
    const NvU32 head,
    const NvU32 layer)
{
    if (pParams->layer[layer].hdr.specified) {
        if (pParams->layer[layer].hdr.enabled) {
            // Don't allow enabling HDR on a layer that doesn't support ICtCp.
            if (!pDevEvo->caps.layerCaps[layer].supportsICtCp) {
                return FALSE;
            }

            pHwState->hdrStaticMetadata.val =
                pParams->layer[layer].hdr.staticMetadata;
        }
        pHwState->hdrStaticMetadata.enabled = pParams->layer[layer].hdr.enabled;

        // Only mark dirty if layer supports ICtCp, otherwise this is a no-op.
        if (pDevEvo->caps.layerCaps[layer].supportsICtCp) {
            pFlipState->dirty.hdrStaticMetadata = TRUE;
        }
    }

    return TRUE;
}

static NvBool UpdateLayerFlipEvoHwStateCommon(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NvU32 layer,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    const NVEvoChannel *pChannel = pDevEvo->head[head].layer[layer];
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[layer];
    const NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pOpenDev);
    NvBool ret;

    if (pParams->layer[layer].surface.specified) {
        ret = nvAssignSurfaceArray(pDevEvo,
                                   pOpenDevSurfaceHandles,
                                   pParams->layer[layer].surface.handle,
                                   FALSE /* isUsedByCursorChannel */,
                                   TRUE /* isUsedByLayerChannel */,
                                   pHwState->pSurfaceEvo);
        if (!ret) {
            return FALSE;
        }

        /*
         * Verify the (rotation, reflectionX, reflectionY) is a
         * combination currently supported.
         */
        if ((NVBIT(NvKmsRRParamsToCapBit(&pParams->layer[layer].surface.rrParams)) &
             pDevEvo->caps.validLayerRRTransforms) == 0) {
            return FALSE;
        }
        pHwState->rrParams = pParams->layer[layer].surface.rrParams;

    }

    /* Verify the timestamp is in the valid range for this channel. */
    if (!FlipTimeStampValidForChannel(pChannel,
                                      pParams->layer[layer].timeStamp)) {
        return FALSE;
    }
    pHwState->timeStamp = pParams->layer[layer].timeStamp;

    if (pParams->layer[layer].syncObjects.specified) {
        if (!pDevEvo->supportsSyncpts &&
                pParams->layer[layer].syncObjects.val.useSyncpt) {
            return FALSE;
        }

        nvkms_memset(&pFlipState->layer[layer].syncObject,
                     0,
                     sizeof(pFlipState->layer[layer].syncObject));

        if (pParams->layer[layer].syncObjects.val.useSyncpt) {
            ret = AssignPreSyncptEvoHwState(pDevEvo,
                                            &pParams->layer[layer].syncObjects.val,
                                            &pHwState->syncObject);
            if (!ret) {
                return FALSE;
            }
            pFlipState->dirty.layerSyncObjects[layer] = TRUE;

            ret = AssignPostSyncptEvoHwState(pDevEvo,
                                             pDevEvo->head[head].layer[layer],
                                             &pParams->layer[layer].syncObjects.val,
                                             &pHwState->syncObject);
            if (!ret) {
                return FALSE;
            }
        } else {
            if (pParams->layer[layer].syncObjects.val.u.semaphores.acquire.surface.surfaceHandle != 0 ||
                pParams->layer[layer].syncObjects.val.u.semaphores.release.surface.surfaceHandle != 0) {
                if (pParams->layer[layer].skipPendingFlips) {
                    return FALSE;
                }
            }

            ret = nvAssignSemaphoreEvoHwState(pDevEvo,
                                              pOpenDevSurfaceHandles,
                                              layer,
                                              sd,
                                              &pParams->layer[layer].syncObjects.val,
                                              &pHwState->syncObject);
            if (!ret) {
                return FALSE;
            }
        }
    }

    if (pHwState->pSurfaceEvo[NVKMS_LEFT]) {
        pHwState->minPresentInterval =
            pParams->layer[layer].minPresentInterval;
    } else {
        /* The hardware requires that MPI be 0 when disabled. */
        pHwState->minPresentInterval = 0;
    }

    if (pParams->layer[layer].sizeIn.specified) {
        pHwState->sizeIn = pParams->layer[layer].sizeIn.val;
    }

    if (pParams->layer[layer].sizeOut.specified) {
        pHwState->sizeOut = pParams->layer[layer].sizeOut.val;
    }

    /*
     * If supportsWindowMode = TRUE, the sizeIn/sizeOut dimensions can be
     * any arbitrary (valid) values.
     *
     * If supportsWindowMode = FALSE (legacy EVO main layer), the sizeIn
     * /sizeOut dimensions must match the size of the surface for that layer.
     *
     * Note that if sizeIn/Out dimensions are invalid i.e. with a width or
     * height of zero, this will be rejected by a call to
     * ValidateFlipChannelEvoHwState() later in the code path.
     *
     * Note that if scaling is unsupported, i.e. that sizeIn cannot differ
     * from sizeOut, then any unsupported configurations will be caught by the
     * ComputeWindowScalingTaps() call later on in this function.
     */
    if (!pDevEvo->caps.layerCaps[layer].supportsWindowMode &&
        (pHwState->pSurfaceEvo[NVKMS_LEFT] != NULL)) {
        const NVSurfaceEvoRec *pSurfaceEvo =
            pHwState->pSurfaceEvo[NVKMS_LEFT];

        if ((pHwState->sizeIn.width != pSurfaceEvo->widthInPixels) ||
            (pHwState->sizeIn.height != pSurfaceEvo->heightInPixels)) {
            return FALSE;
        }

        if ((pHwState->sizeOut.width != pSurfaceEvo->widthInPixels) ||
            (pHwState->sizeOut.height != pSurfaceEvo->heightInPixels)) {
            return FALSE;
        }
    }

    /*
     * Allow the client to specify non-origin outputPosition only if the
     * layer supports window mode.
     *
     * If window mode is unsupported but the client specifies non-origin
     * outputPosition, return FALSE.
     */
    if (pDevEvo->caps.layerCaps[layer].supportsWindowMode) {
        if (pParams->layer[layer].outputPosition.specified) {
            const NvS16 x = pParams->layer[layer].outputPosition.val.x;
            const NvS16 y = pParams->layer[layer].outputPosition.val.y;
            if ((pHwState->outputPosition.x != x) ||
                (pHwState->outputPosition.y != y)) {
                pHwState->outputPosition.x = x;
                pHwState->outputPosition.y = y;
                pFlipState->dirty.layerPosition[layer] = TRUE;
            }
        }
    } else if (pParams->layer[layer].outputPosition.specified &&
               ((pParams->layer[layer].outputPosition.val.x != 0) ||
                (pParams->layer[layer].outputPosition.val.y != 0))) {
        return FALSE;
    }

    if (pParams->layer[layer].compositionParams.specified) {
        pHwState->composition =
            pParams->layer[layer].compositionParams.val;
    }

    if (!UpdateLayerFlipEvoHwStateHDRStaticMetadata(
            pDevEvo, pParams, pFlipState,
            pHwState, head, layer)) {
        return FALSE;
    }

    if (pParams->layer[layer].colorRange.specified) {
        pHwState->colorRange = pParams->layer[layer].colorRange.val;
    }

    if (pParams->layer[layer].colorSpace.specified) {
        pHwState->colorSpace =
            pParams->layer[layer].colorSpace.val;
    }

    if (pHwState->composition.depth == 0) {
        pHwState->composition.depth =
            NVKMS_MAX_LAYERS_PER_HEAD - layer;
    }

    /* XXX Move ValidatePerLayerCompParams() call to nvValidateFlipEvoHwState() */
    if (!nvValidatePerLayerCompParams(
            &pHwState->composition,
            &pDevEvo->caps.layerCaps[layer].composition,
            pHwState->pSurfaceEvo[NVKMS_LEFT])) {
        return FALSE;
    }

    if (!pDevEvo->hal->ComputeWindowScalingTaps(pDevEvo,
                                                pChannel,
                                                pHwState)) {
        return FALSE;
    }

    if (pParams->layer[layer].completionNotifier.specified) {
        ret = nvAssignCompletionNotifierEvoHwState(
                    pDevEvo,
                    pOpenDevSurfaceHandles,
                    &pParams->layer[layer].completionNotifier.val,
                    layer,
                    &pFlipState->layer[layer].completionNotifier);
        if (!ret) {
            return FALSE;
        }
    }

    if (pParams->layer[layer].maxDownscaleFactors.specified) {
        pFlipState->layer[layer].maxDownscaleFactors.vertical =
            pParams->layer[layer].maxDownscaleFactors.vertical;
        pFlipState->layer[layer].maxDownscaleFactors.horizontal =
            pParams->layer[layer].maxDownscaleFactors.horizontal;
        pFlipState->layer[layer].maxDownscaleFactors.specified = TRUE;
    } else {
        pFlipState->layer[layer].maxDownscaleFactors.vertical = 0;
        pFlipState->layer[layer].maxDownscaleFactors.horizontal = 0;
        pFlipState->layer[layer].maxDownscaleFactors.specified = FALSE;
    }

    pFlipState->dirty.layer[layer] = TRUE;

    return TRUE;
}

static NvBool UpdateMainLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState,
    NvBool allowVrr)
{
    const NVEvoChannel *pChannel =
        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[NVKMS_MAIN_LAYER];

    if (!nvIsLayerDirty(pParams, NVKMS_MAIN_LAYER)) {
        return TRUE;
    }

    if (!UpdateLayerFlipEvoHwStateCommon(pOpenDev, pDevEvo, sd, head,
                                         NVKMS_MAIN_LAYER,
                                         pParams, pFlipState)) {
        return FALSE;
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].csc.specified) {
        if (pParams->layer[NVKMS_MAIN_LAYER].csc.useMain) {
            return FALSE;
        } else {
            pHwState->cscMatrix = pParams->layer[NVKMS_MAIN_LAYER].csc.matrix;
        }
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].surface.specified) {
        if (pParams->layer[NVKMS_MAIN_LAYER].perEyeStereoFlip &&
            !pChannel->caps.perEyeStereoFlips) {
            return FALSE;
        }

        pHwState->perEyeStereoFlip =
            pParams->layer[NVKMS_MAIN_LAYER].perEyeStereoFlip;
    }

    if (pParams->layer[NVKMS_MAIN_LAYER].tearing && !pChannel->caps.tearingFlips) {
        return FALSE;
    }

    // EVO will throw an invalid argument exception if
    // minPresentInterval is too large, or if tearing is enabled and
    // it's not zero.
    if (pParams->layer[NVKMS_MAIN_LAYER].minPresentInterval > NV_MAX_SWAP_INTERVAL ||
        (pParams->layer[NVKMS_MAIN_LAYER].tearing &&
         pParams->layer[NVKMS_MAIN_LAYER].minPresentInterval != 0)) {
        return FALSE;
    }

    pHwState->tearing = pParams->layer[NVKMS_MAIN_LAYER].tearing;

    /*
     * XXX: Cludge. Ideally, this would be handled by FlipRequiresNonTearingMode
     * recognizing a difference in HW state. However, the new HW LUT state is
     * not computed until later, when nvEvoSetLut() and nvEvoSetLUTContextDma()
     * are called. See bug 4054546.
     */
    if ((pParams->lut.input.specified ||
        pParams->lut.output.specified) &&
        !pDevEvo->hal->caps.supportsCoreLut) {

        pHwState->tearing = FALSE;
    }

    if (!ApplyBaseFlipOverrides(pDevEvo,
                                sd, head, &pFlipState->layer[NVKMS_MAIN_LAYER],
                                allowVrr)) {
        return FALSE;
    }

    pFlipState->skipLayerPendingFlips[NVKMS_MAIN_LAYER] =
        pParams->layer[NVKMS_MAIN_LAYER].skipPendingFlips;

    return TRUE;
}

static NvBool UpdateCursorLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsFlipCommonParams *pParams,
    const NVHwModeTimingsEvo *pTimings,
    const NvU8 tilePosition,
    NVFlipEvoHwState *pFlipState)
{
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
        pFlipState->cursor.x = (pParams->cursor.position.x -
                                (pTimings->viewPort.in.width * tilePosition));
        pFlipState->cursor.y = pParams->cursor.position.y;

        pFlipState->dirty.cursorPosition = TRUE;
    }

    return TRUE;
}

static NvBool UpdateOverlayLayerFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NvU32 layer,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    NVFlipChannelEvoHwState *pHwState = &pFlipState->layer[layer];

    nvAssert(layer != NVKMS_MAIN_LAYER);

    if (!nvIsLayerDirty(pParams, layer)) {
        return TRUE;
    }

    if (pParams->layer[layer].skipPendingFlips ||
        pParams->layer[layer].perEyeStereoFlip) {
        return FALSE;
    }

    if (!UpdateLayerFlipEvoHwStateCommon(pOpenDev, pDevEvo, sd, head, layer,
                                         pParams, pFlipState)) {
        return FALSE;
    }

    if (pParams->layer[layer].csc.specified) {
        if (pParams->layer[layer].csc.useMain) {
            if (pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT]) {
                pHwState->cscMatrix =
                    pFlipState->layer[NVKMS_MAIN_LAYER].cscMatrix;
            }
        } else {
            pHwState->cscMatrix = pParams->layer[layer].csc.matrix;
        }
    }

    return TRUE;
}

/*!
 * Update the NVFlipEvoHwState, using NvKmsFlipCommonParams.
 *
 * Propagate the requested configuration from NvKmsFlipCommonParams to
 * NVFlipEvoHwState, performing steps such as translating from
 * NvKmsSurfaceHandle to NVSurfaceEvoRecs.  Validate the NvKmsFlipCommonParams
 * parameters, but defer more general validation of the resulting
 * NVFlipEvoHwState until nvValidateFlipEvoHwState(), which callers must call
 * separately.
 *
 * The NVFlipEvoHwState should first be initialized by calling
 * nvInitFlipEvoHwState().
 *
 * No NVKMS hardware or software state should be altered here, because
 * this function is used before we have decided to commit the proposed
 * NVFlipEvoHwState to hardware.
 *
 * \param[in]  pOpenDev       The pOpenDev of the client doing the flip.
 * \param[in]  pDevEvo        The device on which the surface image will be set.
 * \param[in]  sd             The subdevice for the flip, as specified by the
 *                            client.
 * \param[in]  head           The head for the flip, as specified by the client.
 * \param[in]  pParams        The requested flip, NvKmsFlipCommonParams.
 * \param[in]  pTimings       The mode timings for the flip.
 * \param[in,out] pFlipState  The resulting NVFlipEvoHwState.
 * \param[in]  allowVrr       Whether VRR flipping should be allowed.
 * \param[in]  pPossibleUsage Possible usage.
 *
 * \return  If pFlipState could be updated, return TRUE.
 *          Otherwise, return FALSE.
 */
NvBool nvUpdateFlipEvoHwState(
    const struct NvKmsPerOpenDev *pOpenDev,
    const NVDevEvoRec *pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    const NVHwModeTimingsEvo *pTimings,
    const NvU8 tilePosition,
    NVFlipEvoHwState *pFlipState,
    NvBool allowVrr)
{
    NvU32 layer;

    if (pParams->viewPortIn.specified) {
        pFlipState->dirty.viewPortPointIn = TRUE;
        pFlipState->viewPortPointIn.x = pParams->viewPortIn.point.x +
            (pTimings->viewPort.in.width * tilePosition);
        pFlipState->viewPortPointIn.y = pParams->viewPortIn.point.y;
    }

    if (!UpdateCursorLayerFlipEvoHwState(pOpenDev, pDevEvo, pParams, pTimings,
                                         tilePosition, pFlipState)) {
        return FALSE;
    }

    if (pParams->tf.specified) {
        pFlipState->dirty.tf = TRUE;
        pFlipState->tf = pParams->tf.val;
    }

    if (pParams->hdrInfoFrame.specified) {
        pFlipState->dirty.hdrStaticMetadata = TRUE;

        if (pParams->hdrInfoFrame.enabled) {
            pFlipState->hdrInfoFrame.eotf =
                pParams->hdrInfoFrame.eotf;
            pFlipState->hdrInfoFrame.staticMetadata =
                pParams->hdrInfoFrame.staticMetadata;
        }
        pFlipState->hdrInfoFrame.enabled =
            pParams->hdrInfoFrame.enabled;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (layer == NVKMS_MAIN_LAYER) {
            if (!UpdateMainLayerFlipEvoHwState(pOpenDev, pDevEvo, sd, head,
                                               pParams, pFlipState, allowVrr)) {
                return FALSE;
            }
            continue;
        }

        if (!UpdateOverlayLayerFlipEvoHwState(pOpenDev, pDevEvo, sd, head,
                                              layer, pParams, pFlipState)) {
            return FALSE;
        }
    }

    if (!AssignUsageBounds(pDevEvo, head, pFlipState)) {
        return FALSE;
    }

    /*
     * If there is active cursor/cropped-window(overlay) without full screen
     * window(base/core) then NVKMS is supposed to disable MidFrame/DWCF
     * watermark.
     */

    pFlipState->disableMidFrameAndDWCFWatermark = FALSE;

    if (NV5070_CTRL_SYSTEM_GET_CAP(
            pDevEvo->capsBits,
            NV5070_CTRL_SYSTEM_CAPS_BUG_2052012_GLITCHY_MCLK_SWITCH) &&
        !pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT]) {

        if (pFlipState->cursor.pSurfaceEvo != NULL) {
            pFlipState->disableMidFrameAndDWCFWatermark = TRUE;
        } else {
            NvU32 layer;

            /*
             * XXX TODO: Check the output size of the overlay in order
             * to determine if it will be fullscreen or not.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (layer != NVKMS_MAIN_LAYER &&
                    pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL) {
                    pFlipState->disableMidFrameAndDWCFWatermark = TRUE;
                    break;
                }
            }
        }
    }

    return TRUE;
}

/*
 * Checks that if the surface is NULL (i.e. no image will be shown), various
 * other elements must be NULL as well. If the surface is not NULL, verifies
 * that the sizeIn/Out have nonzero values.
 */
inline static NvBool ValidateFlipChannelEvoHwState(
    const NVFlipChannelEvoHwState *pState)
{
    if (pState->pSurfaceEvo[NVKMS_LEFT] != NULL) {
        /* Verify sizes are valid. */
        if ((pState->sizeIn.width == 0) || (pState->sizeIn.height == 0) ||
            (pState->sizeOut.width == 0) || (pState->sizeOut.height == 0)) {
            return FALSE;
        }

        return TRUE;
    }

    if (pState->completionNotifier.surface.pSurfaceEvo != NULL) {
        return FALSE;
    }

    if (!pState->syncObject.usingSyncpt) {
        if (pState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo != NULL) {
            return FALSE;
        }

        if (pState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo != NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

static NvBool ValidateSurfaceSize(
    const NVDevEvoRec *pDevEvo,
    const NVSurfaceEvoRec *pSurfaceEvo,
    const struct NvKmsRect *sourceFetchRect)
{
    NvU8 planeIndex;

    if ((pSurfaceEvo->widthInPixels > pDevEvo->caps.maxWidthInPixels) ||
        (pSurfaceEvo->heightInPixels > pDevEvo->caps.maxHeight)) {
        return FALSE;
    }

    FOR_ALL_VALID_PLANES(planeIndex, pSurfaceEvo) {

        NvU64 planePitch = pSurfaceEvo->planes[planeIndex].pitch;

        /*
         * Convert planePitch to units of bytes if it's currently specified in
         * units of blocks. Each block is 64-bytes wide.
         */
        if (pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
            planePitch <<= NVKMS_BLOCK_LINEAR_LOG_GOB_WIDTH;
        }

        if (planePitch > pDevEvo->caps.maxWidthInBytes) {
            return FALSE;
        }
    }

    if (!pDevEvo->hal->ValidateWindowFormat(pSurfaceEvo->format,
                                            sourceFetchRect,
                                            NULL)) {
        return FALSE;
    }

    return TRUE;
}

static NvBool
ValidateMainFlipChannelEvoHwState(const NVDevEvoRec *pDevEvo,
                                  const NVFlipChannelEvoHwState *pHwState,
                                  const NVHwModeTimingsEvo *pTimings,
                                  struct NvKmsPoint viewPortPointIn)
{
    NvU32 eye;
    const NVSurfaceEvoRec *pFirstSurfaceEvo = NULL;

    /*
     * This struct represents the source fetch rectangle for a given surface,
     * and will be populated later as such. This function doesn't explicitly set
     * sourceFetchRect.{x,y} because NVKMS currently doesn't support programming
     * source fetch offsets, so the init value of 0 should be fine for both of
     * these fields.
     */
    struct NvKmsRect sourceFetchRect = {0};

    if (!ValidateFlipChannelEvoHwState(pHwState)) {
        return FALSE;
    }

    for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {
        const NVSurfaceEvoRec *pSurfaceEvo = pHwState->pSurfaceEvo[eye];

        if (pSurfaceEvo == NULL) {
            continue;
        }

        if (pFirstSurfaceEvo == NULL) {
            pFirstSurfaceEvo = pSurfaceEvo;
        } else if (pSurfaceEvo->widthInPixels !=
                   pFirstSurfaceEvo->widthInPixels ||
                   pSurfaceEvo->heightInPixels !=
                   pFirstSurfaceEvo->heightInPixels) {
            return FALSE;
        }

        sourceFetchRect.width = pHwState->sizeIn.width;
        sourceFetchRect.height = pHwState->sizeIn.height;

        if (!ValidateSurfaceSize(pDevEvo, pSurfaceEvo, &sourceFetchRect)) {
            return FALSE;
        }

        /* The use of A_plus_B_greater_than_C_U16 is only valid if these
         * fit in a U16 */
        nvAssert(pSurfaceEvo->widthInPixels <= NV_U16_MAX);
        nvAssert(pSurfaceEvo->heightInPixels <= NV_U16_MAX);
        /* And the checks above in ValidateSurfaceSize should have
         * guaranteed that. */
        nvAssert(pDevEvo->caps.maxWidthInPixels <= NV_U16_MAX);
        nvAssert(pDevEvo->caps.maxHeight <= NV_U16_MAX);

        /*
         * Validate that the requested viewport parameters fit within the
         * specified surface, unless the main layer is allowed to be smaller
         * than the viewport.
         */
        if (!pDevEvo->caps.layerCaps[NVKMS_MAIN_LAYER].supportsWindowMode) {
            if (A_plus_B_greater_than_C_U16(viewPortPointIn.x,
                                            pTimings->viewPort.in.width,
                                            pSurfaceEvo->widthInPixels)) {
                return FALSE;
            }

            if (A_plus_B_greater_than_C_U16(viewPortPointIn.y,
                                            pTimings->viewPort.in.height,
                                            pSurfaceEvo->heightInPixels)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static NvBool
ValidateOverlayFlipChannelEvoHwState(const NVDevEvoRec *pDevEvo,
                                     const NVFlipChannelEvoHwState *pHwState)
{
    const NVSurfaceEvoRec *pSurfaceEvo = pHwState->pSurfaceEvo[NVKMS_LEFT];

    /*
     * This struct represents the source fetch rectangle for a given surface,
     * and will be populated later as such. This function doesn't explicitly set
     * sourceFetchRect.{x,y} because NVKMS currently doesn't support programming
     * source fetch offsets, so the init value of 0 should be fine for both of
     * these fields.
     */
    struct NvKmsRect sourceFetchRect = {0};

    if (!ValidateFlipChannelEvoHwState(pHwState)) {
        return FALSE;
    }

    if (pSurfaceEvo == NULL) {
        return TRUE;
    }

    sourceFetchRect.width = pHwState->sizeIn.width;
    sourceFetchRect.height = pHwState->sizeIn.height;

    if (!ValidateSurfaceSize(pDevEvo, pSurfaceEvo, &sourceFetchRect)) {
        return FALSE;
    }

    /* Validate input size against surface size. */
    if (pHwState->sizeIn.width > pSurfaceEvo->widthInPixels ||
        pHwState->sizeIn.height > pSurfaceEvo->heightInPixels) {
        return FALSE;
    }

    return TRUE;
}

static NvBool
ValidateHDR(const NVDevEvoRec *pDevEvo,
            const NvU32 head,
            const NVFlipEvoHwState *pFlipState)
{
    NvU32 layerStaticMetadataCount = 0;
    NvU32 layerSupportedCount = 0;

    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pDevEvo->caps.layerCaps[layer].supportsICtCp) {
            layerSupportedCount++;
        }

        if (pFlipState->layer[layer].hdrStaticMetadata.enabled) {
            layerStaticMetadataCount++;

            /*
             * If HDR static metadata is enabled, we may need TMO. CSC11 will be
             * used by NVKMS to convert from linear FP16 LMS to linear FP16 RGB.
             * As such, the user-supplied precomp CSC can't be programmed into
             * CSC11 in this case.
             */
            if (!nvIsCscMatrixIdentity(&pFlipState->layer[layer].cscMatrix)) {
                return FALSE;
            }

            // Already checked in UpdateLayerFlipEvoHwStateHDRStaticMetadata()
            nvAssert(pDevEvo->caps.layerCaps[layer].supportsICtCp);
        }
    }

    // If enabling HDR TF...
    // XXX HDR TODO: Handle other transfer functions
    if (pFlipState->tf == NVKMS_OUTPUT_TF_PQ) {
        // At least one layer must support HDR.
        if (layerSupportedCount == 0) {
            return FALSE;
        }

        // If HDR metadata is not overridden for the head...
        if (!pFlipState->hdrInfoFrame.enabled) {
            // At least one layer must have static metadata.
            if (layerStaticMetadataCount == 0) {
                return FALSE;
            }
        }
    }

    // Only one layer can specify HDR static metadata.
    // XXX HDR TODO: Support multiple layers with HDR static metadata
    if (layerStaticMetadataCount > 1) {
        return FALSE;
    }

    return TRUE;
}

static NvBool
ValidateColorspace(const NVDevEvoRec *pDevEvo,
                   const NvU32 head,
                   const NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if ((pFlipState->layer[layer].colorSpace !=
             NVKMS_INPUT_COLORSPACE_NONE)) {

            NVSurfaceEvoPtr pSurfaceEvo =
                pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT];
            const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
                (pSurfaceEvo != NULL) ?
                    nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvo->format) : NULL;

            // XXX HDR TODO: Support YUV.
            if ((pFormatInfo == NULL) || pFormatInfo->isYUV) {
                return FALSE;
            }

            // FP16 is only for use with scRGB.
            if ((pFlipState->layer[layer].colorSpace !=
                 NVKMS_INPUT_COLORSPACE_SCRGB_LINEAR) &&
                ((pSurfaceEvo->format ==
                  NvKmsSurfaceMemoryFormatRF16GF16BF16AF16) ||
                 (pSurfaceEvo->format ==
                  NvKmsSurfaceMemoryFormatRF16GF16BF16XF16))) {
                return FALSE;
            }

            // scRGB is only compatible with FP16.
            if ((pFlipState->layer[layer].colorSpace ==
                 NVKMS_INPUT_COLORSPACE_SCRGB_LINEAR) &&
                !((pSurfaceEvo->format ==
                   NvKmsSurfaceMemoryFormatRF16GF16BF16AF16) ||
                  (pSurfaceEvo->format ==
                   NvKmsSurfaceMemoryFormatRF16GF16BF16XF16))) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static NvU32 ValidateCompositionDepth(const NVFlipEvoHwState *pFlipState,
                                      const NvU32 layer)
{
    NvU32 tmpLayer;

    if (pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] == NULL) {
        return TRUE;
    }

    /* Depth should be different for each of the layers owned by the head */
    for (tmpLayer = 0; tmpLayer < ARRAY_LEN(pFlipState->layer); tmpLayer++) {
        if (pFlipState->layer[tmpLayer].pSurfaceEvo[NVKMS_LEFT] == NULL) {
            continue;
        }

        if ((tmpLayer != layer) &&
                (pFlipState->layer[tmpLayer].composition.depth ==
                     pFlipState->layer[layer].composition.depth)) {
            return FALSE;
        }
    }

    /* Depth of the main layer should be the greatest one */
    if (pFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT] != NULL) {
        if (pFlipState->layer[NVKMS_MAIN_LAYER].composition.depth <
                pFlipState->layer[layer].composition.depth) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * Perform validation of the the given NVFlipEvoHwState.
 */
NvBool nvValidateFlipEvoHwState(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVHwModeTimingsEvo *pTimings,
    const NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (!ValidateCompositionDepth(pFlipState, layer)) {
            return FALSE;
        }

        if (layer == NVKMS_MAIN_LAYER) {
            if (!ValidateMainFlipChannelEvoHwState(pDevEvo,
                                                   &pFlipState->layer[layer],
                                                   pTimings,
                                                   pFlipState->viewPortPointIn)) {
                return FALSE;
            }
        } else {
            const NVFlipChannelEvoHwState *pMainLayerState =
                &pFlipState->layer[NVKMS_MAIN_LAYER];

            /*
             * No overlay layer should be enabled if the main
             * layer is disabled.
             */
            if ((pMainLayerState->pSurfaceEvo[NVKMS_LEFT] == NULL) &&
                (pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL)) {
                return FALSE;
            }

            if (!pFlipState->dirty.layer[layer]) {
                continue;
            }

            if (!ValidateOverlayFlipChannelEvoHwState(pDevEvo,
                                                      &pFlipState->layer[layer])) {
                return FALSE;
            }
        }
    }

    if (!ValidateHDR(pDevEvo, head, pFlipState)) {
        return FALSE;
    }

    if (!ValidateColorspace(pDevEvo, head, pFlipState)) {
        return FALSE;
    }

    /* XXX NVKMS TODO: validate cursor x,y against current viewport in? */

    return ValidateUsageBounds(pDevEvo,
                               head,
                               &pFlipState->usage,
                               &pTimings->viewPort.possibleUsage);
}

/*
 * Record in the updateState that the given channel needs interlocked
 * window immediate updates.
 */
static void UpdateWinImmInterlockState(NVDevEvoPtr pDevEvo,
                                       NVEvoUpdateState *updateState,
                                       const NVEvoChannel *pChannel)
{
    const NvU32 subDevMask = nvPeekEvoSubDevMask(pDevEvo);
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (subDevMask & (1 << sd)) {
            updateState->subdev[sd].winImmInterlockMask |=
                pChannel->channelMask;
        }
    }
}

/*!
 * Record in the updateState that the given channel's method are eligible for
 * flip locking.
 */
static void UpdateUpdateFlipLockState(NVDevEvoPtr pDevEvo,
                                      NVEvoUpdateState *updateState,
                                      const NVEvoChannel *pChannel)
{
    const NvU32 subDevMask = nvPeekEvoSubDevMask(pDevEvo);
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (subDevMask & (1 << sd)) {
            updateState->subdev[sd].flipLockQualifyingMask |=
                pChannel->channelMask;
        }
    }
}

// Adjust from EDID-encoded maxCLL/maxFALL to actual values in units of 1 cd/m2
static inline NvU32 MaxCvToVal(NvU32 cv)
{
    if (cv == 0) {
        return 0;
    }

    // 50*2^(cv/32)
    return f64_to_ui32(
        f64_mul(ui32_to_f64(50),
            nvKmsPow(ui32_to_f64(2),
                f64_div(ui32_to_f64(cv),
                        ui32_to_f64(32)))), softfloat_round_near_even, FALSE);
}

// Adjust from EDID-encoded minCLL to actual value in units of 0.0001 cd/m2
static inline NvU32 MinCvToVal(NvU32 cv, NvU32 maxCLL)
{
    // 10,000 * (minCLL = (maxCLL * ((cv/255)^2 / 100)))
    return f64_to_ui32(
        f64_mul(ui32_to_f64(10000),
                f64_mul(ui32_to_f64(maxCLL),
                        f64_div(nvKmsPow(f64_div(ui32_to_f64(cv),
                                                 ui32_to_f64(255)),
                                         ui32_to_f64(2)),
                                ui32_to_f64(100)))),
                                softfloat_round_near_even, FALSE);
}

static void UpdateHDR(NVDevEvoPtr pDevEvo,
                      const NVFlipEvoHwState *pFlipState,
                      const NvU32 sd,
                      const NvU32 head,
                      const NVT_HDR_STATIC_METADATA *pHdrInfo,
                      NVEvoUpdateState *updateState)
{
    NVDispEvoPtr pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvBool dirty = FALSE;

    if (pFlipState->dirty.tf) {
        pHeadState->tf = pFlipState->tf;

        dirty = TRUE;
    }

    if (pFlipState->dirty.hdrStaticMetadata) {
        NvBool found = FALSE;

        /*
         * Track if HDR static metadata is overridden for the head in order to
         * initialize subsequent instances of NVFlipEvoHwState.
         */
        pHeadState->hdrInfoFrameOverride.enabled =
            pFlipState->hdrInfoFrame.enabled;
        pHeadState->hdrInfoFrameOverride.eotf =
            pFlipState->hdrInfoFrame.eotf;
        pHeadState->hdrInfoFrameOverride.staticMetadata =
            pFlipState->hdrInfoFrame.staticMetadata;

        // Populate head with updated static metadata.

        if (pFlipState->hdrInfoFrame.enabled) {
            // If HDR static metadata is overridden for the head, use that.
            pHeadState->hdrInfoFrame.staticMetadata =
                pFlipState->hdrInfoFrame.staticMetadata;
            pHeadState->hdrInfoFrame.eotf =
                pFlipState->hdrInfoFrame.eotf;

            pHeadState->hdrInfoFrame.state = NVKMS_HDR_INFOFRAME_STATE_ENABLED;
            found = TRUE;
        } else {
            NvU32 layer;

            /*
             * If HDR static metadata is specified for layer(s), construct the
             * head's HDR static metadata using those.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (pFlipState->layer[layer].hdrStaticMetadata.enabled) {
                    NvU32 targetMaxCLL = MaxCvToVal(pHdrInfo->max_cll);

                    /*
                     * Only one layer can currently specify static metadata,
                     * verified by ValidateHDR().
                     *
                     * XXX HDR TODO: Combine metadata from multiple layers.
                     */
                    nvAssert(!found);

                    // Send this layer's metadata to the display.
                    pHeadState->hdrInfoFrame.staticMetadata =
                        pFlipState->layer[layer].hdrStaticMetadata.val;

                    // Infer metadata eotf from output tf
                    // XXX HDR TODO: Handle other transfer functions
                    switch (pHeadState->tf) {
                        default:
                            nvAssert(!"Unrecognized output TF");
                            // Fall through
                        case NVKMS_OUTPUT_TF_TRADITIONAL_GAMMA_SDR:
                        case NVKMS_OUTPUT_TF_NONE:
                            pHeadState->hdrInfoFrame.eotf =
                                NVKMS_INFOFRAME_EOTF_SDR_GAMMA;
                            break;
                        case NVKMS_OUTPUT_TF_PQ:
                            pHeadState->hdrInfoFrame.eotf =
                                NVKMS_INFOFRAME_EOTF_ST2084;
                            break;
                    }

                    pHeadState->hdrInfoFrame.state =
                        NVKMS_HDR_INFOFRAME_STATE_ENABLED;
                    found = TRUE;

                    /*
                     * Prepare for tone mapping. If we expect to tone map and
                     * the EDID has valid lum values, mirror EDID lum values to
                     * prevent redundant tone mapping by the display. We will
                     * tone map to the specified maxCLL.
                     */
                    if (nvNeedsTmoLut(pDevEvo, pDevEvo->head[head].layer[layer],
                                      &pFlipState->layer[layer],
                                      nvGetHDRSrcMaxLum(
                                          &pFlipState->layer[layer]),
                                      targetMaxCLL)) {
                        NvU32 targetMaxFALL = MaxCvToVal(pHdrInfo->max_fall);
                        if ((targetMaxCLL > 0) &&
                            (targetMaxCLL <= 10000) &&
                            (targetMaxCLL >= targetMaxFALL)) {

                            NvU32 targetMinCLL = MinCvToVal(pHdrInfo->min_cll,
                                                            targetMaxCLL);

                            pHeadState->hdrInfoFrame.staticMetadata.
                                maxDisplayMasteringLuminance = targetMaxCLL;
                            pHeadState->hdrInfoFrame.staticMetadata.
                                minDisplayMasteringLuminance = targetMinCLL;
                            pHeadState->hdrInfoFrame.staticMetadata.maxCLL =
                                targetMaxCLL;
                            pHeadState->hdrInfoFrame.staticMetadata.maxFALL =
                                targetMaxFALL;
                        }
                    }
                }
            }
        }

        if (!found) {
            nvkms_memset(&pHeadState->hdrInfoFrame.staticMetadata, 0,
                         sizeof(struct NvKmsHDRStaticMetadata));
            if (pHeadState->hdrInfoFrame.state ==
                    NVKMS_HDR_INFOFRAME_STATE_ENABLED) {
                pHeadState->hdrInfoFrame.state =
                    NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING;
            }
        }

        dirty = TRUE;
    }

    if (dirty) {
        // Update OCSC / OLUT
        nvEvoSetLUTContextDma(pDispEvo, head, updateState);
    }
}

/*!
 * Program a flip on all requested layers on the specified head.
 *
 * This also updates pDispEvo->headState[head], caching what was programmed.

 * \param[in,out] pDispEvo    The disp on which the flip should be performed.
 * \param[in]     head        The head on which the flip should be performed.
 * \param[in]     pFlipState  The description of how to update each layer.
 * \param[in,out] updateState Indicates which channels require UPDATEs
 */
void nvFlipEvoOneHead(
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const NVT_HDR_STATIC_METADATA *pHdrInfo,
    const NVFlipEvoHwState *pFlipState,
    NvBool allowFlipLock,
    NVEvoUpdateState *updateState)
{
    const NvU32 subDeviceMask = NVBIT(sd);
    const NVDispHeadStateEvoRec *pHeadState =
        &pDevEvo->gpus[sd].pDispEvo->headState[head];
    NvBool bypassComposition = pHeadState->bypassComposition;
    NVEvoSubDevHeadStateRec *pSdHeadState =
        &pDevEvo->gpus[sd].headState[head];
    NvU32 layer;

    /*
     * Provide the pre-update hardware state (in pSdHeadState) and the new
     * target state (pFlipState) to the HAL implementation so that it has the
     * information it needs to implement the workaround for hardware bug
     * 2193096, which requires special logic on transitions between NULL and
     * non-NULL ctxdmas (and vice versa).
     */
    pDevEvo->hal->FlipTransitionWAR(pDevEvo, sd, head,
                                    pSdHeadState, pFlipState,
                                    updateState);

    /*
     * Promote the software state first, such that the hardware programming
     * paths below see the new state atomically.
     */
    if (pFlipState->dirty.viewPortPointIn) {
        pSdHeadState->viewPortPointIn = pFlipState->viewPortPointIn;
    }

    if (pFlipState->dirty.cursorSurface || pFlipState->dirty.cursorPosition) {
        pSdHeadState->cursor = pFlipState->cursor;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->dirty.layer[layer]) {
            pSdHeadState->layer[layer] = pFlipState->layer[layer];
        }
    }

    if (pFlipState->dirty.viewPortPointIn) {
        nvSetViewPortPointInEvo(pDevEvo->gpus[sd].pDispEvo,
                                head,
                                pFlipState->viewPortPointIn.x,
                                pFlipState->viewPortPointIn.y,
                                updateState);
    }

    if (pFlipState->dirty.cursorSurface) {
        nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
        pDevEvo->hal->SetCursorImage(pDevEvo,
                                     head,
                                     pSdHeadState->cursor.pSurfaceEvo,
                                     updateState,
                                     &pSdHeadState->cursor.cursorCompParams);
        nvPopEvoSubDevMask(pDevEvo);
    }

    if (pFlipState->dirty.cursorPosition) {
        nvEvoMoveCursorInternal(pDevEvo->gpus[sd].pDispEvo,
                                head,
                                pFlipState->cursor.x,
                                pFlipState->cursor.y);
    }

    UpdateHDR(pDevEvo, pFlipState, sd, head, pHdrInfo, updateState);

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (!pFlipState->dirty.layer[layer]) {
            continue;
        }

        nvPushEvoSubDevMask(pDevEvo, subDeviceMask);

        if (pFlipState->dirty.layerPosition[layer]) {
            /* Ensure position updates are supported on this layer. */
            nvAssert(pDevEvo->caps.layerCaps[layer].supportsWindowMode);

            pDevEvo->hal->SetImmPointOut(pDevEvo,
                                         pDevEvo->head[head].layer[layer],
                                         sd,
                                         updateState,
                                         pFlipState->layer[layer].outputPosition.x,
                                         pFlipState->layer[layer].outputPosition.y);

            if (pDevEvo->hal->caps.supportsSynchronizedOverlayPositionUpdate) {
                UpdateWinImmInterlockState(pDevEvo, updateState,
                                           pDevEvo->head[head].layer[layer]);
            }
        }

        /* Inform DIFR about the upcoming flip. */
        if (pDevEvo->pDifrState) {
            nvDIFRNotifyFlip(pDevEvo->pDifrState);
        }

        pDevEvo->hal->Flip(pDevEvo,
                           pDevEvo->head[head].layer[layer],
                           &pFlipState->layer[layer],
                           updateState,
                           bypassComposition);
        if (layer == NVKMS_MAIN_LAYER && allowFlipLock) {
            UpdateUpdateFlipLockState(pDevEvo, updateState,
                                      pDevEvo->head[head].layer[layer]);
        }
        nvPopEvoSubDevMask(pDevEvo);
    }

    pSdHeadState->targetUsage = pFlipState->usage;

    pSdHeadState->targetDisableMidFrameAndDWCFWatermark =
        pFlipState->disableMidFrameAndDWCFWatermark;
}

static void ChangeSurfaceFlipRefCount(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo,
    NvBool increase)
{
    if (pSurfaceEvo != NULL) {
        if (increase) {
            nvEvoIncrementSurfaceRefCnts(pSurfaceEvo);
        } else {
            nvEvoDecrementSurfaceRefCnts(pDevEvo, pSurfaceEvo);
        }
    }
}

void nvUpdateSurfacesFlipRefCount(
    NVDevEvoPtr pDevEvo,
    const NvU32 head,
    NVFlipEvoHwState *pFlipState,
    NvBool increase)
{
    NvU32 i;

    ChangeSurfaceFlipRefCount(
        pDevEvo,
        pFlipState->cursor.pSurfaceEvo,
        increase);

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        NVFlipChannelEvoHwState *pLayerFlipState = &pFlipState->layer[i];

        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->pSurfaceEvo[NVKMS_LEFT],
            increase);
        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->pSurfaceEvo[NVKMS_RIGHT],
            increase);
        ChangeSurfaceFlipRefCount(
            pDevEvo,
            pLayerFlipState->completionNotifier.surface.pSurfaceEvo,
            increase);

        if (!pLayerFlipState->syncObject.usingSyncpt) {
            ChangeSurfaceFlipRefCount(
                pDevEvo,
                pLayerFlipState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo,
                increase);
            ChangeSurfaceFlipRefCount(
                pDevEvo,
                pLayerFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo,
                increase);
        }
    }
}

static void UnionScalingUsageBounds(
    const struct NvKmsScalingUsageBounds *a,
    const struct NvKmsScalingUsageBounds *b,
    struct NvKmsScalingUsageBounds *ret)
{
    ret->maxVDownscaleFactor = NV_MAX(a->maxVDownscaleFactor,
                                      b->maxVDownscaleFactor);
    ret->maxHDownscaleFactor = NV_MAX(a->maxHDownscaleFactor,
                                      b->maxHDownscaleFactor);
    ret->vTaps = NV_MAX(a->vTaps, b->vTaps);
    ret->vUpscalingAllowed = a->vUpscalingAllowed || b->vUpscalingAllowed;
}

void nvUnionUsageBounds(const struct NvKmsUsageBounds *a,
                        const struct NvKmsUsageBounds *b,
                        struct NvKmsUsageBounds *ret)
{
    NvU32 i;

    nvkms_memset(ret, 0, sizeof(*ret));

    for (i = 0; i < ARRAY_LEN(a->layer); i++) {
        nvAssert(a->layer[i].usable ==
                 !!a->layer[i].supportedSurfaceMemoryFormats);
        nvAssert(b->layer[i].usable ==
                 !!b->layer[i].supportedSurfaceMemoryFormats);

        ret->layer[i].usable = a->layer[i].usable || b->layer[i].usable;

        ret->layer[i].supportedSurfaceMemoryFormats =
            a->layer[i].supportedSurfaceMemoryFormats |
            b->layer[i].supportedSurfaceMemoryFormats;

        UnionScalingUsageBounds(&a->layer[i].scaling,
                                &b->layer[i].scaling,
                                &ret->layer[i].scaling);
    }
}

static void IntersectScalingUsageBounds(
    const struct NvKmsScalingUsageBounds *a,
    const struct NvKmsScalingUsageBounds *b,
    struct NvKmsScalingUsageBounds *ret)
{
    ret->maxVDownscaleFactor = NV_MIN(a->maxVDownscaleFactor,
                                      b->maxVDownscaleFactor);
    ret->maxHDownscaleFactor = NV_MIN(a->maxHDownscaleFactor,
                                      b->maxHDownscaleFactor);
    ret->vTaps = NV_MIN(a->vTaps, b->vTaps);
    ret->vUpscalingAllowed = a->vUpscalingAllowed && b->vUpscalingAllowed;
}

void nvIntersectUsageBounds(const struct NvKmsUsageBounds *a,
                            const struct NvKmsUsageBounds *b,
                            struct NvKmsUsageBounds *ret)
{
    NvU32 i;

    nvkms_memset(ret, 0, sizeof(*ret));

    for (i = 0; i < ARRAY_LEN(a->layer); i++) {
        nvAssert(a->layer[i].usable ==
                 !!a->layer[i].supportedSurfaceMemoryFormats);
        nvAssert(b->layer[i].usable ==
                 !!b->layer[i].supportedSurfaceMemoryFormats);

        ret->layer[i].usable = a->layer[i].usable && b->layer[i].usable;

        ret->layer[i].supportedSurfaceMemoryFormats =
            a->layer[i].supportedSurfaceMemoryFormats &
            b->layer[i].supportedSurfaceMemoryFormats;

        IntersectScalingUsageBounds(&a->layer[i].scaling,
                                    &b->layer[i].scaling,
                                    &ret->layer[i].scaling);
    }
}
NvBool UsageBoundsEqual(
    const struct NvKmsUsageBounds *a,
    const struct NvKmsUsageBounds *b)
{
    NvU32 layer;

    for (layer = 0; layer < ARRAY_LEN(a->layer); layer++) {
        if (!nvEvoLayerUsageBoundsEqual(a, b, layer)) {
            return FALSE;
        }
    }

    return TRUE;
}

NvBool nvAllocatePreFlipBandwidth(NVDevEvoPtr pDevEvo,
                                  struct NvKmsFlipWorkArea *pWorkArea)
{
    NVValidateImpOneDispHeadParamsRec *timingsParams = NULL;
    struct NvKmsUsageBounds *currentAndNew = NULL;
    struct NvKmsUsageBounds *guaranteedAndCurrent = NULL;
    NVDispEvoPtr pDispEvo;
    NvU32 head;
    NvBool recheckIMP = FALSE;
    NvBool ret = TRUE;

    if (!pDevEvo->isSOCDisplay) {
        return TRUE;
    }

    timingsParams =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*timingsParams));
    if (timingsParams == NULL) {
        return FALSE;
    }

    currentAndNew =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*currentAndNew));
    if (currentAndNew == NULL) {
        nvFree(timingsParams);
        return FALSE;
    }

    guaranteedAndCurrent =
        nvCalloc(NVKMS_MAX_HEADS_PER_DISP, sizeof(*guaranteedAndCurrent));
    if (guaranteedAndCurrent == NULL) {
        nvFree(timingsParams);
        nvFree(currentAndNew);
        return FALSE;
    }

    pDispEvo = pDevEvo->pDispEvo[0];

    // SOC Display never has more than one disp
    nvAssert(pDevEvo->nDispEvo == 1);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        const struct NvKmsUsageBounds *pCurrent =
            &pDevEvo->gpus[0].headState[head].preallocatedUsage;
        const struct NvKmsUsageBounds *pNew =
            &pWorkArea->sd[0].head[head].newState.usage;

        if (pHeadState->activeRmId == 0) {
            continue;
        }

        timingsParams[head].pConnectorEvo = pHeadState->pConnectorEvo;
        timingsParams[head].activeRmId = pHeadState->activeRmId;
        timingsParams[head].pixelDepth = pHeadState->pixelDepth;
        timingsParams[head].pTimings = &pHeadState->timings;
        timingsParams[head].enableDsc = (pHeadState->dscInfo.type !=
            NV_DSC_INFO_EVO_TYPE_DISABLED);
        timingsParams[head].b2Heads1Or =
            (pHeadState->mergeMode != NV_EVO_MERGE_MODE_DISABLED);

        nvUnionUsageBounds(pCurrent, pNew, &currentAndNew[head]);
        nvUnionUsageBounds(&pHeadState->timings.viewPort.guaranteedUsage,
                           pCurrent, &guaranteedAndCurrent[head]);

        if (!ValidateUsageBounds(pDevEvo,
                                 head,
                                 pNew,
                                 &guaranteedAndCurrent[head])) {
            recheckIMP = TRUE;
        }

        nvUnionUsageBounds(&guaranteedAndCurrent[head], pNew,
                           &guaranteedAndCurrent[head]);
        timingsParams[head].pUsage = &guaranteedAndCurrent[head];
    }

    if (recheckIMP) {
        ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                                   FALSE /* requireBootClocks */,
                                   NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE,
                                   NULL /* pMinIsoBandwidthKBPS */,
                                   NULL /* pMinDramFloorKBPS */);
        if (ret) {
            for (head = 0; head < pDevEvo->numHeads; head++) {
                pDevEvo->gpus[0].headState[head].preallocatedUsage =
                    currentAndNew[head];
            }
        }
    }

    nvFree(timingsParams);
    nvFree(currentAndNew);
    nvFree(guaranteedAndCurrent);

    if (ret) {
        nvScheduleLowerDispBandwidthTimer(pDevEvo);
    }

    return ret;
}

/*!
 * If the satellite channel is active then pre-NVDisplay hardware does not allow
 * to change its usage bounds in non-interlock update. The nvSetUsageBoundsEvo()
 * code path for pre-NVDisplay hardware, interlocks the satellite channels with
 * the usage bounds update. This makes it essential to poll for
 * NO_METHOD_PENDING state of the satellite channels, otherwise blocking
 * pre-flip IMP update will also get stuck.
 *
 * It is not possible to interlock flip-locked satellite channels with the core
 * channel usage bounds update; in that case, reject the flip.  Do not allow
 * client to make any change in surface usage bounds parameters without
 * deactivating channel first, if channel is flip-locked.
 */
static NvBool PrepareToDoPreFlipIMP(NVDevEvoPtr pDevEvo,
                                    struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU64 startTime = 0;
    NvU32 timeout = 2000000; /* 2 seconds */
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVEvoHeadControlPtr pHC =
                &pEvoSubDev->headControl[head];
            const NVEvoSubDevHeadStateRec *pCurrentFlipState =
                &pDevEvo->gpus[sd].headState[head];
            const NVSurfaceEvoRec *pCurrentBaseSurf =
                pCurrentFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            const struct NvKmsUsageBounds *pCurrentUsage =
                &pCurrentFlipState->usage;

            NVFlipEvoHwState *pNewFlipState =
                &pWorkArea->sd[sd].head[head].newState;
            const NVSurfaceEvoRec *pNewBaseSurf =
                pNewFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
            struct NvKmsUsageBounds *pNewUsage =
                &pNewFlipState->usage;

            struct NvKmsUsageBounds *pPreFlipUsage =
                &pWorkArea->sd[sd].head[head].preFlipUsage;

            NvU32 layer;

            nvUnionUsageBounds(pNewUsage, pCurrentUsage, pPreFlipUsage);

            if (pDevEvo->hal->caps.supportsNonInterlockedUsageBoundsUpdate) {
                /*
                 * NVDisplay does not interlock the satellite channel
                 * with its usage bounds update.
                 */
                continue;
            }

            /*
             * If head is flip-locked then do not change usage
             * bounds while base channel is active.
             */
            if (pHC->flipLock &&
                 /* If the base channel is active before and after flip then
                  * current and new base usage bounds should be same. */
                ((pNewBaseSurf != NULL &&
                  pCurrentBaseSurf != NULL &&
                  !nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                              pNewUsage, NVKMS_MAIN_LAYER)) ||
                  /* If the base channel is active before flip then current and
                   * preflip base usage bounds should be same. */
                 (pCurrentBaseSurf != NULL &&
                  !nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                              pPreFlipUsage, NVKMS_MAIN_LAYER)))) {
                return FALSE;
            }

            /*
             * Poll for NO_METHOD_PENDING state if usage
             * bounds of the channel are changed.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (!nvEvoLayerUsageBoundsEqual(pCurrentUsage,
                                                pPreFlipUsage, layer) &&
                    !nvEvoPollForNoMethodPending(pDevEvo,
                                                 sd,
                                                 pDevEvo->head[head].layer[layer],
                                                 &startTime,
                                                 timeout)) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*!
 * Tasks need to perform before triggering flip, they all should be done here.
 *
 * If necessary, raise usage bounds and/or disable MidFrameAndDWCFWatermark
 * (bug 200508242) in the core channel and do an IMP update.
 *
 * Note that this function only raises usage bounds and/or disables
 * MidFrameAndDWCFWatermark, never lowers usage bounds and/or enables
 * MidFrameAndDWCFWatermark. This allows it to run before queuing a flip even
 * if there are still pending flips in a base channel.
 */
static void PreFlipIMP(NVDevEvoPtr pDevEvo,
                       const struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NVEvoUpdateState updateState = { };
        NvBool update = FALSE;

        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            const NVFlipEvoHwState *pNewState =
                &pWorkArea->sd[sd].head[head].newState;
            const struct NvKmsUsageBounds *pPreFlipUsage =
                &pWorkArea->sd[sd].head[head].preFlipUsage;
            struct NvKmsUsageBounds *pCurrentUsage =
                &pDevEvo->gpus[sd].headState[head].usage;

            if (!UsageBoundsEqual(pCurrentUsage, pPreFlipUsage)) {
                update |= nvSetUsageBoundsEvo(pDevEvo, sd, head,
                                              pPreFlipUsage, &updateState);
            }

            if (!pDevEvo->gpus[sd].
                headState[head].disableMidFrameAndDWCFWatermark &&
                pNewState->disableMidFrameAndDWCFWatermark) {

                nvEnableMidFrameAndDWCFWatermark(pDevEvo,
                                                 sd,
                                                 head,
                                                 FALSE /* enable */,
                                                 &updateState);
                update = TRUE;
            }
        }

        if (update) {
            nvDoIMPUpdateEvo(pDispEvo, &updateState);
        }
    }
}

static void LowerDispBandwidth(void *dataPtr, NvU32 dataU32)
{
    NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP];
    struct NvKmsUsageBounds *guaranteedAndCurrent;
    NVDevEvoPtr pDevEvo = dataPtr;
    NVDispEvoPtr pDispEvo;
    NvU32 head;
    NvBool ret;

    guaranteedAndCurrent =
        nvCalloc(1, sizeof(*guaranteedAndCurrent) * NVKMS_MAX_HEADS_PER_DISP);
    if (guaranteedAndCurrent == NULL) {
        nvAssert(guaranteedAndCurrent != NULL);
        return;
    }

    nvkms_memset(&timingsParams, 0, sizeof(timingsParams));

    pDispEvo = pDevEvo->pDispEvo[0];

    // SOC Display never has more than one disp
    nvAssert(pDevEvo->nDispEvo == 1);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        const struct NvKmsUsageBounds *pGuaranteed =
            &pHeadState->timings.viewPort.guaranteedUsage;
        const struct NvKmsUsageBounds *pCurrent =
            &pDevEvo->gpus[0].headState[head].usage;

        if (pHeadState->activeRmId == 0) {
            continue;
        }

        timingsParams[head].pConnectorEvo = pHeadState->pConnectorEvo;
        timingsParams[head].activeRmId = pHeadState->activeRmId;
        timingsParams[head].pixelDepth = pHeadState->pixelDepth;
        timingsParams[head].pTimings = &pHeadState->timings;
        timingsParams[head].enableDsc = (pHeadState->dscInfo.type !=
            NV_DSC_INFO_EVO_TYPE_DISABLED);
        timingsParams[head].b2Heads1Or =
            (pHeadState->mergeMode != NV_EVO_MERGE_MODE_DISABLED);

        nvUnionUsageBounds(pGuaranteed, pCurrent, &guaranteedAndCurrent[head]);
        timingsParams[head].pUsage = &guaranteedAndCurrent[head];
    }

    ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                               FALSE /* requireBootClocks */,
                               NV_EVO_REALLOCATE_BANDWIDTH_MODE_POST,
                               NULL /* pMinIsoBandwidthKBPS */,
                               NULL /* pMinDramFloorKBPS */);
    if (ret) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            pDevEvo->gpus[0].headState[head].preallocatedUsage =
                pDevEvo->gpus[0].headState[head].usage;
        }
    }

    nvAssert(ret);

    nvFree(guaranteedAndCurrent);
}

void nvCancelLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo)
{
    nvkms_free_timer(pDevEvo->lowerDispBandwidthTimer);
    pDevEvo->lowerDispBandwidthTimer = NULL;
}

void nvScheduleLowerDispBandwidthTimer(NVDevEvoPtr pDevEvo)
{
    nvAssert(pDevEvo->isSOCDisplay);

    nvCancelLowerDispBandwidthTimer(pDevEvo);

    pDevEvo->lowerDispBandwidthTimer =
        nvkms_alloc_timer(LowerDispBandwidth,
                          pDevEvo,
                          0, /* dataU32 */
                          30000000 /* 30 seconds */);
}

/*!
 * Check whether the core, base, and overlay channels are idle (i.e. no methods
 * pending in the corresponding pushbuffer) and lower the usage bounds if
 * possible.
 */
static NvBool TryLoweringUsageBoundsOneHead(NVDevEvoPtr pDevEvo, NvU32 sd,
                                            NvU32 head,
                                            NVEvoUpdateState *updateState)
{
    const NVEvoSubDevHeadStateRec *pHeadState =
        &pDevEvo->gpus[sd].headState[head];
    const struct NvKmsUsageBounds *pCurrent = &pHeadState->usage;
    const struct NvKmsUsageBounds *pTarget = &pHeadState->targetUsage;
    struct NvKmsUsageBounds newUsage = *pCurrent;
    NvBool changed = FALSE;
    NvBool scheduleLater = FALSE;
    int i;

    for (i = 0; i < pDevEvo->head[head].numLayers; i++) {
        if (pCurrent->layer[i].usable && !pTarget->layer[i].usable) {
            NvBool isMethodPending;

            if (pDevEvo->hal->IsChannelMethodPending(
                                        pDevEvo,
                                        pDevEvo->head[head].layer[i],
                                        sd,
                                        &isMethodPending) && !isMethodPending) {
                newUsage.layer[i] = pTarget->layer[i];
                changed = TRUE;
            } else {
                scheduleLater = TRUE;
            }
        } else if ((pCurrent->layer[i].usable && pTarget->layer[i].usable) &&
                   ((pCurrent->layer[i].supportedSurfaceMemoryFormats !=
                     pTarget->layer[i].supportedSurfaceMemoryFormats) ||
                    (!nvEvoScalingUsageBoundsEqual(&pCurrent->layer[i].scaling,
                                                   &pTarget->layer[i].scaling)))) {
            NvBool isMethodPending;

            if (pDevEvo->hal->IsChannelMethodPending(
                                        pDevEvo,
                                        pDevEvo->head[head].layer[i],
                                        sd,
                                        &isMethodPending) && !isMethodPending) {
                newUsage.layer[i] = pTarget->layer[i];
                changed = TRUE;
            } else {
                scheduleLater = TRUE;
            }
        }
    }

    if (scheduleLater) {
        SchedulePostFlipIMPTimer(pDevEvo);
    }

    if (changed) {
        changed = nvSetUsageBoundsEvo(pDevEvo, sd, head, &newUsage,
                                      updateState);
    }

    return changed;
}

static NvBool
TryEnablingMidFrameAndDWCFWatermarkOneHead(NVDevEvoPtr pDevEvo,
                                           NvU32 sd,
                                           NvU32 head,
                                           NVEvoUpdateState *updateState)
{
    const NVEvoSubDevHeadStateRec *pHeadState =
        &pDevEvo->gpus[sd].headState[head];
    NvBool changed = FALSE;

    if (pHeadState->disableMidFrameAndDWCFWatermark &&
        !pHeadState->targetDisableMidFrameAndDWCFWatermark) {

        NvBool isIdle;

        if (pDevEvo->hal->IsChannelIdle(pDevEvo,
                                        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                        sd,
                                        &isIdle) && isIdle) {
            nvEnableMidFrameAndDWCFWatermark(pDevEvo,
                                             sd,
                                             head,
                                             TRUE /* enable */,
                                             updateState);
            changed = TRUE;
        } else {
            // Schedule another timer to try again later.
            SchedulePostFlipIMPTimer(pDevEvo);
        }
    }

    return changed;
}

static void
TryToDoPostFlipIMP(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    pDevEvo->postFlipIMPTimer = NULL;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NVEvoUpdateState updateState = { };
        NvBool update = FALSE;

        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            if (TryLoweringUsageBoundsOneHead(pDevEvo, sd, head,
                                              &updateState)) {
                update = TRUE;
            }

            if (TryEnablingMidFrameAndDWCFWatermarkOneHead(
                    pDevEvo,
                    sd,
                    head,
                    &updateState)) {
                update = TRUE;
            }
        }

        if (update) {
            nvDoIMPUpdateEvo(pDispEvo, &updateState);
        }
    }
}

static void SchedulePostFlipIMPTimer(NVDevEvoPtr pDevEvo)
{
    if (!pDevEvo->postFlipIMPTimer) {
        pDevEvo->postFlipIMPTimer =
            nvkms_alloc_timer(
                TryToDoPostFlipIMP,
                pDevEvo,
                0, /* dataU32 */
                10000000 /* 10 seconds */);
    }
}

void nvEvoCancelPostFlipIMPTimer(NVDevEvoPtr pDevEvo)
{
    nvkms_free_timer(pDevEvo->postFlipIMPTimer);
    pDevEvo->postFlipIMPTimer = NULL;
}

/*!
 * If necessary, schedule a timer to see if usage bounds can be lowered.
 */
static void SchedulePostFlipIMP(NVDevEvoPtr pDevEvo)
{
    NvU32 head, sd;
    NVDispEvoPtr pDispEvo;

    // If a timer is already scheduled, do nothing.
    if (pDevEvo->postFlipIMPTimer) {
        return;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            const NVEvoSubDevHeadStateRec *pHeadState =
                &pDevEvo->gpus[sd].headState[head];

            if (!UsageBoundsEqual(&pHeadState->usage,
                                  &pHeadState->targetUsage) ||
                (pHeadState->disableMidFrameAndDWCFWatermark !=
                 pHeadState->targetDisableMidFrameAndDWCFWatermark)) {

                SchedulePostFlipIMPTimer(pDevEvo);
                return;
            }
        }
    }
}

static void SkipLayerPendingFlips(NVDevEvoRec *pDevEvo,
                                  const NvBool trashPendingMethods,
                                  const NvBool unblockMethodsInExecutation,
                                  struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU64 startTime = 0;
    const NvU32 timeout = 2000000; /* 2 seconds */
    struct {
        struct {
            struct {
                NvU32 oldAccelMask;
            } head[NVKMS_MAX_HEADS_PER_DISP];
        } sd[NVKMS_MAX_SUBDEVICES];
    } accelState = { };
    NvU32 sd, head;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            const NVFlipEvoHwState *pFlipState =
                &pWorkArea->sd[sd].head[head].newState;

            if (!pFlipState->skipLayerPendingFlips[NVKMS_MAIN_LAYER]||
                !pFlipState->dirty.layer[NVKMS_MAIN_LAYER]) {
                continue;
            }

            pDevEvo->hal->AccelerateChannel(
                pDevEvo,
                pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                sd,
                trashPendingMethods,
                unblockMethodsInExecutation,
                &accelState.sd[sd].head[head].oldAccelMask);
        }
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            const NVFlipEvoHwState *pFlipState =
                &pWorkArea->sd[sd].head[head].newState;

            if (!pFlipState->skipLayerPendingFlips[NVKMS_MAIN_LAYER] ||
                !pFlipState->dirty.layer[NVKMS_MAIN_LAYER]) {
                continue;
            }

            if (unblockMethodsInExecutation) {
                if (!nvEvoPollForNoMethodPending(pDevEvo,
                                                 sd,
                                                 pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                                 &startTime,
                                                 timeout)) {
                    nvAssert(!"Failed to idle the main layer channel");
                }
            } else {
                if (!nvEvoPollForEmptyChannel(pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                              sd,
                                              &startTime,
                                              timeout)) {
                    nvAssert(!"Failed to empty the main layer channel");
                }
            }

            pDevEvo->hal->ResetChannelAccelerators(
                pDevEvo,
                pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                sd,
                trashPendingMethods,
                unblockMethodsInExecutation,
                accelState.sd[sd].head[head].oldAccelMask);
        }
    }
}

void nvPreFlip(NVDevEvoRec *pDevEvo,
               struct NvKmsFlipWorkArea *pWorkArea,
               const NvBool applyAllowVrr,
               const NvBool allowVrr,
               const NvBool skipUpdate)
{
    NvU32 sd, head;
    NVDispEvoRec *pDispEvo;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            // Increase refCnt of surfaces used AFTER flip
            nvUpdateSurfacesFlipRefCount(
                pDevEvo,
                head,
                &pWorkArea->sd[sd].head[head].newState,
                NV_TRUE);

            nvRefTmoLutSurfacesEvo(
                pDevEvo,
                &pWorkArea->sd[sd].head[head].newState,
                head);
        }
    }

    PreFlipIMP(pDevEvo, pWorkArea);

    if (!skipUpdate) {
        /* Trash flips pending in channel which are not yet in execution */
        SkipLayerPendingFlips(pDevEvo, TRUE /* trashPendingMethods */,
                              FALSE /* unblockMethodsInExecutation */,
                              pWorkArea);
    }

    if (applyAllowVrr) {
        nvSetVrrActive(pDevEvo, allowVrr);
    }

    /*
     * Update flip metering for Frame pacing smoothing/frame splitting for direct
     * drive and adaptive sync VRR, and override the flip timestamp if
     * necessary.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (NvU32 inputHead = 0; inputHead < pDevEvo->numHeads; inputHead++) {
            const NVDispHeadStateEvoRec *pInputHeadState =
                &pDispEvo->headState[inputHead];
            const struct NvKmsVrrFramePacingInfo *pInputVrrFramePacingInfo =
                &pInputHeadState->vrrFramePacingInfo;
            const NvU32 headsMask = pInputHeadState->mergeModeVrrSecondaryHeadMask |
                NVBIT(inputHead);

            /*
             * XXX[2Heads1OR] Implement per api-head frame pacing and remove this
             * mergeMode check and NVDispEvoRec::mergeModeVrrSecondaryHeadMask.
             */
            if (pInputHeadState->mergeMode == NV_EVO_MERGE_MODE_SECONDARY) {
                continue;
            }

#if defined(DEBUG)
            FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
                const NVFlipEvoHwState *pInputHeadNewState =
                    &pWorkArea->sd[sd].head[inputHead].newState;
                const NVFlipEvoHwState *pNewState =
                    &pWorkArea->sd[sd].head[head].newState;

                nvAssert(pNewState->dirty.layer[NVKMS_MAIN_LAYER] ==
                            pInputHeadNewState->dirty.layer[NVKMS_MAIN_LAYER]);
            }
#endif

            FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
                NVFlipEvoHwState *pNewState =
                    &pWorkArea->sd[sd].head[head].newState;
                if (pNewState->dirty.layer[NVKMS_MAIN_LAYER]) {
                    nvTrackAndDelayFlipForVrrSwFramePacing(pDispEvo,
                        pInputVrrFramePacingInfo,
                        &pNewState->layer[NVKMS_MAIN_LAYER]);
                }
            }
        }
    }
}

void nvPostFlip(NVDevEvoRec *pDevEvo,
                struct NvKmsFlipWorkArea *pWorkArea,
                const NvBool skipUpdate,
                const NvBool applyAllowVrr,
                NvS32 *pVrrSemaphoreIndex)
{
    NvU32 sd, head;

    if (!skipUpdate) {
        /* Unblock flips which are stuck in execution */
        SkipLayerPendingFlips(pDevEvo, FALSE /* trashPendingMethods */,
                              TRUE /* unblockMethodsInExecutation */,
                              pWorkArea);
    }

    if (applyAllowVrr) {
        *pVrrSemaphoreIndex = nvIncVrrSemaphoreIndex(pDevEvo);
    } else {
        // TODO Schedule vrr unstall; per-disp/per-device?
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (!pWorkArea->sd[sd].changed) {
            continue;
        }

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            // Decrease refCnt of surfaces used BEFORE the flip
            nvUpdateSurfacesFlipRefCount(
                pDevEvo,
                head,
                &pWorkArea->sd[sd].head[head].oldState,
                NV_FALSE);

            nvUnrefTmoLutSurfacesEvo(
                pDevEvo,
                &pWorkArea->sd[sd].head[head].oldState,
                head);
        }
    }

    if (!skipUpdate) {
        // Note that usage bounds are not lowered here, because the flip
        // queued by this function may not occur until later. Instead, schedule
        // a timer for later to check if the usage bounds can be lowered.
        SchedulePostFlipIMP(pDevEvo);

        pDevEvo->skipConsoleRestore = FALSE;
    }
}

static NvBool AllocPreSyncpt(NVDevEvoRec *pDevEvo,
                             NVEvoChannel *pChannel, const NvU32 id)
{
    NVSurfaceDescriptor surfaceDesc;

    /*! use id value to check the global table */
    if (!pDevEvo->preSyncptTable[id].allocated) {
        /*! Register - allocate and bind surface descriptor for syncpt*/
        if (!nvRmEvoAllocAndBindSyncpt(pDevEvo,
                                       pChannel,
                                       id,
                                       &surfaceDesc,
                                       &pDevEvo->preSyncptTable[id])) {
            nvAssert(!"Failed to register pre-syncpt");
            return FALSE;
        }

        /*! Fill the Entry in Global Table */
        pDevEvo->preSyncptTable[id].channelMask |= pChannel->channelMask;
    } else {
        /*!
         * syncpt found, just bind the surface descriptor of this syncpt
         * to the window if it is not already.
         */
        if ((pDevEvo->preSyncptTable[id].channelMask &
             pChannel->channelMask) == 0) {

            NvU32 ret =
                pDevEvo->hal->BindSurfaceDescriptor(pDevEvo,
                    pChannel, &pDevEvo->preSyncptTable[id].surfaceDesc);
            if (ret != NVOS_STATUS_SUCCESS) {
                nvAssert(!"Failed to bind surface descriptor for pre-syncpt");
            }

            pDevEvo->preSyncptTable[id].channelMask |= pChannel->channelMask;
            /*! hSyncpt already allocated for id*/
        }
    }

    return TRUE;
}

static NvBool RegisterPreSyncpt(NVDevEvoRec *pDevEvo,
                                struct NvKmsFlipWorkArea *pWorkArea)
{
    NvU32 sd;
    NvU32 ret = TRUE;
    const NVDispEvoRec *pDispEvo;

    pDevEvo->pAllSyncptUsedInCurrentFlip =
        nvCalloc(1, sizeof(NvBool) * NV_SYNCPT_GLOBAL_TABLE_LENGTH);
    if (pDevEvo->pAllSyncptUsedInCurrentFlip == NULL) {
        ret = FALSE;
        goto done;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 head;
        for (head = 0; head < ARRAY_LEN(pWorkArea->sd[sd].head); head++) {
            NVFlipEvoHwState *pFlipState =
                &pWorkArea->sd[sd].head[head].newState;
            NvU32 layer;

            for (layer = 0; layer < ARRAY_LEN(pFlipState->layer); layer++) {
                NVFlipSyncObjectEvoHwState *pFlipSyncObject =
                    &pFlipState->layer[layer].syncObject;
                NvU32 preSyncpt = pFlipSyncObject->u.syncpts.preSyncpt;

                if (!pFlipState->dirty.layerSyncObjects[layer] ||
                    !pFlipSyncObject->usingSyncpt ||
                    !pFlipSyncObject->u.syncpts.isPreSyncptSpecified) {
                    continue;
                }

                if (!AllocPreSyncpt(pDevEvo, pDevEvo->head[head].layer[layer],
                                    preSyncpt)) {
                    ret = FALSE;
                    goto done;
                }

                pDevEvo->pAllSyncptUsedInCurrentFlip[preSyncpt] = NV_TRUE;
            }
        }
    }

done:
    nvFree(pDevEvo->pAllSyncptUsedInCurrentFlip);
    pDevEvo->pAllSyncptUsedInCurrentFlip = NULL;

    return ret;
}

NvBool nvPrepareToDoPreFlip(NVDevEvoRec *pDevEvo,
                            struct NvKmsFlipWorkArea *pWorkArea)
{
    if (!RegisterPreSyncpt(pDevEvo, pWorkArea)) {
        return FALSE;
    }

    if (!PrepareToDoPreFlipIMP(pDevEvo, pWorkArea)) {
        return FALSE;
    }

    return TRUE;
}

NvBool nvAssignNVFlipEvoHwState(NVDevEvoRec *pDevEvo,
                                const struct NvKmsPerOpenDev *pOpenDev,
                                const NvU32 sd,
                                const NvU32 head,
                                const struct NvKmsFlipCommonParams *pParams,
                                const NvBool allowVrr,
                                NVFlipEvoHwState *pFlipHwState)
{
    const NVDispEvoRec *pDispEvo =  pDevEvo->gpus[sd].pDispEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const struct NvKmsUsageBounds *pPossibleUsage =
        &pHeadState->timings.viewPort.possibleUsage;

    if (!nvUpdateFlipEvoHwState(pOpenDev, pDevEvo, sd, head, pParams,
                                &pHeadState->timings, pHeadState->tilePosition,
                                pFlipHwState, allowVrr)) {
        return FALSE;
    }

    nvOverrideScalingUsageBounds(pDevEvo, head, pFlipHwState, pPossibleUsage);

    if (!nvValidateFlipEvoHwState(pDevEvo, head, &pHeadState->timings,
                                  pFlipHwState)) {
        return FALSE;
    }

    if (!nvSetTmoLutSurfacesEvo(pDevEvo, pFlipHwState, head)) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Wait for idle on a set of the main layer channels.
 *
 * \param[in,out]  pDevEvo               The device.
 * \param[in]      idleChannelMaskPerSd  The channel masks per subdevice that
 *                                       we should wait to be idle.
 * \param[in]      allowForceIdle        Whether we should force idle a channel
 *                                       or just assert if the idle times out.
 */
void nvIdleMainLayerChannels(
    NVDevEvoPtr pDevEvo,
    const NVEvoChannelMask *idleChannelMaskPerSd,
    NvBool allowStopBase)
{
    NvU64 startTime = 0;
    NvBool allChannelsIdle = FALSE;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex, head;
    NVEvoChannelMask busyChannelMaskPerSd[NVKMS_MAX_SUBDEVICES] = { };

    /*
     * Wait up to 2 seconds for all channels to be idle, and gather a list of
     * all busy channels.
     */
    while (!allChannelsIdle) {

        const NvU32 timeout = 2000000; /* 2 seconds */
        NvBool anyChannelBusy = FALSE;

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            for (head = 0; head < pDevEvo->numHeads; head++) {
                NVEvoChannelPtr pMainLayerChannel =
                    pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
                if (idleChannelMaskPerSd[pDispEvo->displayOwner] &
                    pMainLayerChannel->channelMask) {

                    NvBool isMethodPending = FALSE;
                    if (!pDevEvo->hal->IsChannelMethodPending(
                            pDevEvo,
                            pMainLayerChannel,
                            pDispEvo->displayOwner,
                            &isMethodPending)
                        || isMethodPending) {

                        /* Mark this channel as busy. */
                        busyChannelMaskPerSd[pDispEvo->displayOwner] |=
                            pMainLayerChannel->channelMask;
                        anyChannelBusy = TRUE;
                    } else {
                        /*
                         * Mark this channel as no longer busy, in case its
                         * flip completed while we were waiting on another
                         * channel.
                         */
                        busyChannelMaskPerSd[pDispEvo->displayOwner] &=
                            ~pMainLayerChannel->channelMask;
                    }
                }
            }
        }

        if (!anyChannelBusy) {
            allChannelsIdle = TRUE;
            break;
        }

        /* Break out of the loop if we exceed the timeout. */
        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            break;
        }

        nvkms_yield();
    }

    if (!allChannelsIdle) {
        /*
         * At least one channel was still idle after the 2 second timeout
         * above.
         */
        if (!allowStopBase) {
            /*
             * The caller of this function expected this wait for idle not to
             * time out.
             */
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Timeout while waiting for idle.");
        } else {
            /*
             * Idle all base channels that were still busy when the wait above
             * timed out.
             */
            NVEvoIdleChannelState idleChannelState = { };

            FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
                idleChannelState.subdev[pDispEvo->displayOwner].channelMask =
                    busyChannelMaskPerSd[pDispEvo->displayOwner];
            }

            pDevEvo->hal->ForceIdleSatelliteChannelIgnoreLock(
                pDevEvo, &idleChannelState);
        }
    }
}

NvBool nvNeedToToggleFlipLock(const NVDispEvoRec *pDispEvo,
                              const NvU32 head, const NvBool enable)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    const NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
    NvBool needToToggle = FALSE;

    if (!enable && pHC->flipLock) {
        /*
         * This channel is currently using fliplock in the config that
         * is being torn down; idle its base channel and disable
         * fliplock.
         */
        needToToggle = TRUE;
    }

    if (enable && ((pHC->serverLock != NV_EVO_NO_LOCK) ||
                   (pHC->clientLock != NV_EVO_NO_LOCK))) {
        /*
         * This channel will be using fliplock for swap groups in the
         * new config; idle its base channel and enable fliplock.
         */
        nvAssert(!HEAD_MASK_QUERY(pEvoSubDev->flipLockProhibitedHeadMask,
                                  head));
        needToToggle = TRUE;
    }

    return needToToggle;
}

void nvToggleFlipLockPerDisp(NVDispEvoRec *pDispEvo, const NvU32 headMask,
                             const NvBool enable)
{
    NvU32 head;
    NVEvoUpdateState updateState = { };
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        if ((headMask & NVBIT(head)) != 0x0) {
            NvU32 setEnable = enable;

            if (!nvUpdateFlipLockEvoOneHead(pDispEvo, head, &setEnable,
                                            TRUE /* set */,
                                            &updateState)) {
                nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                    "Failed to toggle fliplock for swapgroups.");
            }
        }
    }

    if (!nvIsUpdateStateEmpty(pDevEvo, &updateState)) {
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);
    }
}
