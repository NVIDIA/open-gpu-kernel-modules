/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * The EVO modeset sequence is structured to minimize changes to the
 * hardware from one modeset to the next, and to minimize the number
 * of UPDATE methods that are programmed.
 *
 * Software state is tracked in three different structures:
 *
 * (1) NVDispEvo::headState - This is the NVKMS record of what has
 * been programmed in the hardware, for all heads on the disp.
 *
 * (2) NvKmsSetModeRequest - This is the NVKMS client's description of
 * what changes are requested.  Note that clients can just request to
 * change specific heads on specific disps.  Other heads/disps should
 * retain their current configuration across the modeset.
 *
 * (3) NVProposedModeSetHwState - This describes the hardware state
 * that is desired at the end of the modeset.  It is assigned by
 * considering the current state (NVDispEvo::headState) and applying
 * any client-requested changes (NvKmsSetModeRequest).
 *
 * The intended flow is:
 *
 * - Assign NVProposedModeSetHwState, given NVDispEvo::headState and
 *   NvKmsSetModeRequest, noting which heads are changing.
 * - Check whether the proposed state is valid, and fail the modeset
 *   if anything about the proposed configuration is invalid.
 *
 *  NOTE: Nothing before this point in the sequence should alter NVKMS
 *  software state, or program hardware.  Also, to the extent
 *  possible, we should avoid failing the modeset after this point in
 *  the sequence, because this is when we start altering software and
 *  hardware state.
 *
 * - Notify RM that the modeset is starting.
 * - Reset the EVO locking state machine.
 * - For each disp:
 *   - For each head:
 *     - Shut down newly unused heads
 *   - For each head:
 *     - Apply the requested changes.
 *   - Send evo UPDATE method
 *   - For each head:
 *     - Perform post-UPDATE work
 * - Update the EVO locking state machine.
 * - Notify RM that the modeset is complete.
 * - Populate the reply structure returned to the NVKMS client.
 *
 *
 * TODO:
 * - Would it be worthwhile to centralize SOR (re)assignment, disp-wide,
 *   in ApplyProposedModeSetHwStateOneDisp() between the calls to
 *   ApplyProposedModeSetStateOneApiHeadShutDown() and
 *   ApplyProposedModeSetHwStateOneHeadPreUpdate()?
 */

#include "nvkms-evo.h"
#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-rm.h"
#include "nvkms-hdmi.h"
#include "nvkms-hw-flip.h"
#include "nvkms-flip.h"
#include "nvkms-3dvision.h"
#include "nvkms-modepool.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-vrr.h"
#include "nvkms-lut.h"
#include "nvkms-dma.h"

#include "dp/nvdp-connector.h"
#include "dp/nvdp-device.h"

#include "nvkms-api.h"

#include "nvkms-modeset.h"
#include "nvkms-modeset-types.h"
#include "nvkms-modeset-workarea.h"
#include "nvkms-attributes.h"
#include "nvkms-headsurface-config.h"

static NvBool
GetColorSpaceAndColorRange(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    NVDpyAttributeColor *pDpyColor);

static void
ClearProposedModeSetHwState(const NVDevEvoRec *pDevEvo,
                            NVProposedModeSetHwState *pProposed,
                            const NvBool committed)
{
    const NVDispEvoRec *pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 head;

        if (!committed)  {
            /* Free new allocated RM display IDs for changed heads */
            for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                NVProposedModeSetStateOneApiHead *pProposedApiHead =
                    &pProposed->disp[dispIndex].apiHead[apiHead];

                if (!pProposedApiHead->changed ||
                        (pProposedApiHead->activeRmId == 0x0)) {
                    continue;
                }
                nvRmFreeDisplayId(pDispEvo, pProposedApiHead->activeRmId);
            }
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposed->disp[dispIndex].head[head];
            nvDPLibFreeModesetState(pProposedHead->pDpLibModesetState);
        }
    }

    nvkms_memset(pProposed, 0 , sizeof(*pProposed));
}

/*
 * Inherit the previous modeset state as part of this modeset if:
 * - The requesting client is not the internal NVKMS client (i.e., this is not
 * a console restore modeset).
 * - There is no modeset ownership change since the last modeset.
 */
static NvBool
InheritPreviousModesetState(const NVDevEvoRec *pDevEvo,
                            const struct NvKmsPerOpenDev *pCurrentModesetOpenDev)
{
    return (pCurrentModesetOpenDev != pDevEvo->pNvKmsOpenDev) &&
                !pDevEvo->modesetOwnerChanged;
}

/*!
 * Get the NVHwModeTimingsEvo for the mode requested by the client.
 *
 * NvKmsSetModeOneHeadRequest::mode specifies mode timings in a
 * hardware-neutral format, along with mode validation parameters and
 * the dpyIdList on which to set the mode.  Validate the requested
 * mode and compute NVHwModeTimingsEvo for it.
 *
 * \param[in]  pDispEvo     The disp of the dpyIdList and head.
 * \param[in]  pRequestHead The mode, mode validation parameters, dpyIdList,
 *                          and head requested by the client.
 * \param[out] pTimings     The mode timings to program in the hardware.
 *
 * \return     Return TRUE if the requested mode is valid and pTimings
 *             could be assigned.  Otherwise, return FALSE.
 */
NvBool
nvGetHwModeTimings(const NVDispEvoRec *pDispEvo,
                   const NvU32 apiHead,
                   const struct NvKmsSetModeOneHeadRequest *pRequestHead,
                   NVHwModeTimingsEvo *pTimings,
                   NVDpyAttributeColor *pDpyColor,
                   NVT_VIDEO_INFOFRAME_CTRL *pInfoFrameCtrl)
{
    NVDpyEvoPtr pDpyEvo;
    NVDpyAttributeColor dpyColor = { };

    if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
        return TRUE;
    }

    pDpyEvo = nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);

    if (pDpyEvo == NULL) {
        return FALSE;
    }

    if (!GetColorSpaceAndColorRange(pDispEvo, apiHead, pRequestHead,
                                    &dpyColor)) {
        return FALSE;
    }

    if (!nvValidateModeForModeset(pDpyEvo,
                                  &pRequestHead->modeValidationParams,
                                  &pRequestHead->mode,
                                  &pRequestHead->viewPortSizeIn,
                                  pRequestHead->viewPortOutSpecified ?
                                            &pRequestHead->viewPortOut : NULL,
                                  &dpyColor,
                                  pTimings,
                                  pInfoFrameCtrl)) {
        return FALSE;
    }

    if (pDpyColor != NULL) {
        *pDpyColor = dpyColor;
    }

    return TRUE;
}

static NvBool IsPreSyncptSpecified(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const struct NvKmsFlipCommonParams *pParams)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pParams->layer[layer].syncObjects.specified &&
            pParams->layer[layer].syncObjects.val.useSyncpt &&
            pParams->layer[layer].syncObjects.val.u.syncpts.pre.type !=
               NVKMS_SYNCPT_TYPE_NONE) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool
GetColorSpaceAndColorRange(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    NVDpyAttributeColor *pDpyColor)
{
    enum NvKmsOutputColorimetry colorimetry;
    enum NvKmsDpyAttributeColorRangeValue requestedColorRange;
    enum NvKmsDpyAttributeColorBpcValue requestedColorBpc;
    enum NvKmsDpyAttributeRequestedColorSpaceValue requestedColorSpace;
    NVDpyEvoRec *pOneArbitraryDpyEvo =
        nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);

    if (pRequestHead->colorSpaceSpecified) {
        const NVDpyEvoRec *pDpyEvo;

        /*
         * There could be multiple DPYs driven by this head. For each DPY,
         * validate that the requested colorspace and color range is valid.
         */
        FOR_ALL_EVO_DPYS(pDpyEvo, pRequestHead->dpyIdList, pDispEvo) {
            if (!nvDpyValidateColorSpace(pDpyEvo, pRequestHead->colorSpace)) {
                return FALSE;
            }
        }

        requestedColorSpace = pRequestHead->colorSpace;
    } else {
        requestedColorSpace = pOneArbitraryDpyEvo->requestedColorSpace;
    }

    if (pRequestHead->colorRangeSpecified) {
        requestedColorRange = pRequestHead->colorRange;
    } else {
        requestedColorRange = pOneArbitraryDpyEvo->requestedColorRange;
    }

    if (pRequestHead->colorBpcSpecified) {
        requestedColorBpc = pRequestHead->colorBpc;
    } else {
        requestedColorBpc = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
    }

    if (pRequestHead->flip.colorimetry.specified) {
        colorimetry = pRequestHead->flip.colorimetry.val;
    } else {
        colorimetry =
            pDispEvo->apiHeadState[apiHead].attributes.color.colorimetry;
    }

    /*
     * Choose current colorSpace and colorRange based on the current mode
     * timings and the requested color space and range.
     */
    if (!nvChooseCurrentColorSpaceAndRangeEvo(pOneArbitraryDpyEvo,
                                              pRequestHead->mode.timings.yuv420Mode,
                                              colorimetry,
                                              requestedColorSpace,
                                              requestedColorBpc,
                                              requestedColorRange,
                                              &pDpyColor->format,
                                              &pDpyColor->bpc,
                                              &pDpyColor->range)) {
        return FALSE;
    }
    pDpyColor->colorimetry = colorimetry;

    return TRUE;
}

static NvBool AssignProposedModeSetColorSpaceAndColorRangeSpecified(
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    NVProposedModeSetStateOneApiHead *pProposedApiHead)
{
    /*
     * When colorspace is specified in modeset request, it should
     * match the proposed colorspace.
     */
    if (pRequestHead->colorSpaceSpecified) {
        NvBool ret = FALSE;
        switch (pProposedApiHead->attributes.color.format) {
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
                ret = (pRequestHead->colorSpace ==
                        NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_RGB);
                break;
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
                ret = (pRequestHead->colorSpace ==
                        NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr422);
                break;
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
                ret = (pRequestHead->colorSpace ==
                        NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr444);
                 break;
             default:
                break;
        }
        if (!ret) {
            return ret;
        }
    }

    /*
     * When color bpc is specified in modeset request, it should
     * match the proposed color bpc.
     */
    if (pRequestHead->colorBpcSpecified &&
        (pProposedApiHead->attributes.color.bpc != pRequestHead->colorBpc)) {
        return FALSE;
    }

    /*
     * When color range is specified in modeset request, it should
     * match the proposed color range.
     */
    if (pRequestHead->colorRangeSpecified &&
        (pProposedApiHead->attributes.color.range != pRequestHead->colorRange)) {
        return FALSE;
    }

    pProposedApiHead->colorSpaceSpecified = pRequestHead->colorSpaceSpecified;
    pProposedApiHead->colorBpcSpecified = pRequestHead->colorBpcSpecified;
    pProposedApiHead->colorRangeSpecified = pRequestHead->colorRangeSpecified;
    return TRUE;
}

/* count existing unchanged and new vrr heads */
static NvU32 CountProposedVrrApiHeads(NVDevEvoPtr pDevEvo,
                                      const struct NvKmsSetModeRequest *pRequest)
{
    NvU32 sd;
    NVDispEvoPtr pDispEvo;
    NvU32 numVRRApiHeads = 0;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 apiHead;

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const struct NvKmsSetModeOneDispRequest *pRequestDisp =
                &pRequest->disp[sd];
            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];

            if (((pRequest->requestedDispsBitMask & (1 << sd)) == 0) ||
                    ((pRequestDisp->requestedHeadsBitMask & (1 << apiHead)) == 0)) {
                const NVDispApiHeadStateEvoRec *pApiHeadState =
                    &pDispEvo->apiHeadState[apiHead];

                if (pApiHeadState->timings.vrr.type != NVKMS_DPY_VRR_TYPE_NONE) {
                    numVRRApiHeads++;
                }
            } else {
                const NVDpyEvoRec *pDpyEvo =
                    nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);

                if (pDpyEvo == NULL) {
                    continue;
                }

                if (nvGetAllowedDpyVrrType(pDpyEvo,
                                           &pRequestHead->mode.timings,
                                           pRequestHead->modeValidationParams.stereoMode,
                                           pRequestHead->allowGsync,
                                           pRequestHead->allowAdaptiveSync) !=
                        NVKMS_DPY_VRR_TYPE_NONE) {
                    numVRRApiHeads++;
                }
            }
        }
    }

    return numVRRApiHeads;
}

static void AdjustHwModeTimingsForVrr(const NVDispEvoRec *pDispEvo,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    const NvU32 prohibitAdaptiveSync,
    NVHwModeTimingsEvo *pTimings)
{
    NVDpyEvoPtr pDpyEvo =
        nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);
    if (pDpyEvo == NULL) {
        return;
    }

    const NvBool allowGsync = pRequestHead->allowGsync;
    const enum NvKmsAllowAdaptiveSync allowAdaptiveSync =
        prohibitAdaptiveSync ? NVKMS_ALLOW_ADAPTIVE_SYNC_DISABLED :
            pRequestHead->allowAdaptiveSync;
    const NvU32 vrrOverrideMinRefreshRate = pRequestHead->vrrOverrideMinRefreshRate;
    const enum NvKmsDpyVRRType vrrType =
        nvGetAllowedDpyVrrType(pDpyEvo,
                               &pRequestHead->mode.timings,
                               pRequestHead->modeValidationParams.stereoMode,
                               allowGsync,
                               allowAdaptiveSync);

    nvAdjustHwModeTimingsForVrrEvo(pDpyEvo,
                                   vrrType,
                                   vrrOverrideMinRefreshRate,
                                   pTimings);
}

/*
 * Return whether headSurface is allowed. But, only honor the requestor's
 * setting if they have modeset owner permission. Otherwise, inherit the cached
 * value in pDevEvo.
 */
NvBool nvGetAllowHeadSurfaceInNvKms(const NVDevEvoRec *pDevEvo,
                                    const struct NvKmsPerOpenDev *pOpenDev,
                                    const struct NvKmsSetModeRequest *pRequest)
{
    if (nvKmsOpenDevHasSubOwnerPermissionOrBetter(pOpenDev)) {
        return pRequest->allowHeadSurfaceInNvKms;
    }

    return pDevEvo->allowHeadSurfaceInNvKms;
}

static void
InitNVProposedModeSetStateOneApiHead(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    NVProposedModeSetStateOneApiHead *pProposedApiHead)
{
#if defined(DEBUG)
    const NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pDispEvo->apiHeadState[apiHead].activeDpys,
                                pDispEvo);
#endif

    if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
        return;
    }

    nvAssert(pDispEvo->apiHeadState[apiHead].hwHeadsMask != 0x0);
    nvAssert(pDpyEvo != NULL);

    pProposedApiHead->hwHeadsMask =
        pDispEvo->apiHeadState[apiHead].hwHeadsMask;
    pProposedApiHead->timings =
        pDispEvo->apiHeadState[apiHead].timings;
    pProposedApiHead->dpyIdList =
        pDispEvo->apiHeadState[apiHead].activeDpys;
    pProposedApiHead->attributes =
        pDispEvo->apiHeadState[apiHead].attributes;
    pProposedApiHead->changed = FALSE;
    pProposedApiHead->hs10bpcHint =
        pDispEvo->apiHeadState[apiHead].hs10bpcHint;
    pProposedApiHead->infoFrame =
        pDispEvo->apiHeadState[apiHead].infoFrame;
    pProposedApiHead->tf = pDispEvo->apiHeadState[apiHead].tf;
    pProposedApiHead->hdrInfoFrameOverride =
        pDispEvo->apiHeadState[apiHead].hdrInfoFrameOverride;
    pProposedApiHead->hdrStaticMetadataLayerMask =
        pDispEvo->apiHeadState[apiHead].hdrStaticMetadataLayerMask;
    pProposedApiHead->viewPortPointIn =
        pDispEvo->apiHeadState[apiHead].viewPortPointIn;

    NvU32 head;
    NvU32 hwHeadCount = 0;
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask,
        head) {
        const NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];

        nvAssert(pDpyEvo->pConnectorEvo ==
                    pHeadState->pConnectorEvo);

        if (hwHeadCount == 0) {
            pProposedApiHead->dscInfo = pHeadState->dscInfo;
            pProposedApiHead->activeRmId = pHeadState->activeRmId;
            pProposedApiHead->modeValidationParams =
                pHeadState->modeValidationParams;
         } else {
            nvAssert(nvkms_memcmp(&pProposedApiHead->dscInfo,
                        &pHeadState->dscInfo,
                        sizeof(pProposedApiHead->dscInfo)) == 0x0);
            nvAssert(pProposedApiHead->activeRmId ==
                        pHeadState->activeRmId);
            nvAssert(nvkms_memcmp(&pProposedApiHead->modeValidationParams,
                &pHeadState->modeValidationParams,
                sizeof(pProposedApiHead->modeValidationParams)) == 0x0);
         }
    }
}

static void
InitProposedModeSetHwState(const NVDevEvoRec *pDevEvo,
                           const struct NvKmsPerOpenDev *pOpenDev,
                           NVProposedModeSetHwState *pProposed)
{
    NvU32 sd;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(pProposed, 0, sizeof(*pProposed));

    /*
     * If the previous modeset can not be inherited then initialize the
     * proposed modeset state to shutdown all heads.
     */
    if (!InheritPreviousModesetState(pDevEvo, pOpenDev)) {
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
                NvU32 layer;
                NVFlipEvoHwState *pFlip = &pProposed->sd[sd].head[head].flip;
                pFlip->dirty.tf = TRUE;
                pFlip->dirty.hdrStaticMetadata = TRUE;
                for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                    pFlip->dirty.layer[layer] = TRUE;
                }
            }

            for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                pProposed->disp[sd].apiHead[apiHead].changed = TRUE;
            }
        }
    } else {
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                NVProposedModeSetStateOneApiHead *pProposedApiHead =
                    &pProposed->disp[sd].apiHead[apiHead];;

                InitNVProposedModeSetStateOneApiHead(pDispEvo, apiHead,
                    pProposedApiHead);

                NvU32 head;
                FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask,
                    head) {
                    const NVDispHeadStateEvoRec *pHeadState =
                        &pDispEvo->headState[head];
                    NVProposedModeSetHwStateOneHead *pProposedHead =
                        &pProposed->disp[sd].head[head];

                    nvInitFlipEvoHwState(pDevEvo, sd, head,
                                         &pProposed->sd[sd].head[head].flip);

                    pProposedHead->mergeHeadSection =
                        pHeadState->mergeHeadSection;
                    pProposedHead->timings = pHeadState->timings;
                    pProposedHead->pConnectorEvo = pHeadState->pConnectorEvo;
                    pProposedHead->hdmiFrlConfig = pHeadState->hdmiFrlConfig;
                    pProposedHead->audio = pHeadState->audio;
                    pProposedHead->multiTileConfig =
                        pHeadState->multiTileConfig;
                }
            }
        }
    }
}

static NvBool
AssignProposedModeSetNVFlipEvoHwState(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsPerOpenDev *pOpenDev,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    NVFlipEvoHwState *pFlip,
    NVProposedModeSetHwStateOneHead *pProposedHead,
    const NvBool commit)
{
    /*
     * Clear the flipStates of all layers:
     *
     * The current flipState of main layer may still contain
     * old surfaces (e.g., headSurface) that are no longer
     * desirable or compatible with the new modeset
     * configuration.
     *
     * Function ApplyProposedModeSetHwStateOneHeadShutDown() clears
     * pSdHeadState and disables all layers. It is not possible to
     * re-apply the existing flipstates because hardware releases
     * sempahores when layers get disabled; this results in a stuck
     * channel if you re-apply the existing flipstate which has
     * the old semaphore values.
     */

    nvClearFlipEvoHwState(pFlip);

    if (commit) {
        NvU32 layer;

        pFlip->dirty.tf = TRUE;
        pFlip->dirty.hdrStaticMetadata = TRUE;
        pFlip->dirty.olut = TRUE;

        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            pFlip->dirty.layer[layer] = TRUE;
        }
    }

    /*!
     * Modeset path should not request pre-syncpt as it will
     * not progress because this will update all of the Core and
     * Window method state together, and wait for the Core
     * completion notifier to signal. If any of the Window
     * channels is waiting for a semaphore acquire, then this
     * will stall the Core notifier as well since the Core and
     * Window channels are interlocked.
     */
    if (pDevEvo->supportsSyncpts &&
            IsPreSyncptSpecified(pDevEvo, head, &pRequestHead->flip)) {
        return FALSE;
    }

    if (!nvUpdateFlipEvoHwState(pOpenDev,
                                pDevEvo,
                                sd,
                                head,
                                &pRequestHead->flip,
                                &pProposedHead->timings,
                                pProposedHead->mergeHeadSection,
                                pFlip,
                                FALSE /* allowVrr */)) {
        return FALSE;
    }

    /*
     * EVO3 hal simulates USE_CORE_LUT behavior.
     * NVDisplay window channel does allow to change the input LUT
     * on immediate flips, therefore force disable tearing
     * if LUT is specified.
     *
     * XXX NVKMS TODO: Implement separate input programming for
     * base and overlay layers and remove code block.
     */
    if ((pRequestHead->flip.lut.input.specified ||
         pRequestHead->flip.lut.output.specified) &&
        !pDevEvo->hal->caps.supportsCoreLut) {
        pFlip->layer[NVKMS_MAIN_LAYER].tearing = FALSE;
    }

    return TRUE;
}

static
NvBool AssignProposedHwHeadsForDsiConnector(
    const NVDispEvoRec *pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NvU32 *pFreeHwHeadsMask)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 freeHwHeadsMask = *pFreeHwHeadsMask;
    NvU32 ret = TRUE;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);

        if (!pProposedApiHead->changed ||
                nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList) ||
                (pDpyEvo->pConnectorEvo->signalFormat !=
                    NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI)) {
            continue;
        }

        if (pProposedApiHead->hwHeadsMask == 0x0) {
            /* DSI supports only HEAD0 assignment */
            if ((freeHwHeadsMask & NVBIT(0)) != 0x0) {
                nvAssert(pDevEvo->apiHead[apiHead].numLayers <=
                            pDevEvo->head[0].numLayers);
                nvAssignHwHeadsMaskProposedApiHead(pProposedApiHead, 0x1);
                freeHwHeadsMask &= ~pProposedApiHead->hwHeadsMask;
            } else {
                ret = FALSE;
            }
        } else {
            nvAssert(pProposedApiHead->hwHeadsMask == 0x1);
        }

        /* There can be only one DSI dpy */
        break;
    }

    *pFreeHwHeadsMask &= freeHwHeadsMask;
    return ret;
}

static NvBool HeadIsFree(const NVDevEvoRec *pDevEvo,
                         const NvU32 apiHead,
                         const NvU32 freeHwHeadsMask,
                         const NvU32 head)
{
    if ((NVBIT(head) & freeHwHeadsMask) == 0x0) {
        return FALSE;
    }

    if (pDevEvo->apiHead[apiHead].numLayers >
            pDevEvo->head[head].numLayers) {
        return FALSE;
    }

    return TRUE;
}

static NvU32 GetFreeHeads(const NVDevEvoRec *pDevEvo,
                          const NvU32 apiHead,
                          const NVDpyEvoRec *pDpyEvo,
                          const NvU32 freeHwHeadsMask)
{
    NvU32 foundHead = NV_INVALID_HEAD;

    for (NvS32 head = (pDevEvo->numHeads - 1); head >= 0; head--) {
        if (HeadIsFree(pDevEvo, apiHead, freeHwHeadsMask, head)) {
            if ((foundHead == NV_INVALID_HEAD) ||
                   (pDevEvo->head[head].numLayers <
                    pDevEvo->head[foundHead].numLayers)) {
                foundHead = head;
            }
        }
    }

    return foundHead;
}

static
NvBool AssignProposedHwHeadsForDPSerializer(
    const NVDispEvoRec *pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NvU32 *pFreeHwHeadsMask)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 freeHwHeadsMask = *pFreeHwHeadsMask;
    NvU32 ret = TRUE;
    NvU32 boundHead;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);

        if (!pProposedApiHead->changed ||
                nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList) ||
                !nvConnectorIsDPSerializer(pDpyEvo->pConnectorEvo)) {
            continue;
        }

        if (pDpyEvo->pConnectorEvo->dpSerializerCaps.supportsMST) {
            /* MST case: Use earlier assigned stream index */
            boundHead = pDpyEvo->dp.serializerStreamIndex;
        } else {
            /* SST case: find free heads if there are multiple DP Serializer in SST mode */
            boundHead = GetFreeHeads(pDevEvo, apiHead, pDpyEvo,
                                           freeHwHeadsMask);
        }

        if (pProposedApiHead->hwHeadsMask == 0x0) {
            if ((freeHwHeadsMask & NVBIT(boundHead)) != 0x0) {
                nvAssert(pDevEvo->apiHead[apiHead].numLayers <=
                            pDevEvo->head[boundHead].numLayers);
                nvAssignHwHeadsMaskProposedApiHead(
                    pProposedApiHead, NVBIT(boundHead));
                freeHwHeadsMask &= ~pProposedApiHead->hwHeadsMask;
            } else {
                ret = FALSE;
                break;
            }
        } else {
            nvAssert(pProposedApiHead->hwHeadsMask ==
                        NVBIT(boundHead));
        }
    }

    *pFreeHwHeadsMask &= freeHwHeadsMask;
    return ret;
}

static NvU32 GetFree2Heads1ORHeadsMask(const NVDevEvoRec *pDevEvo,
                                       const NvU32 apiHead,
                                       const NvU32 freeHwHeadsMask)
{
    /*
     * Hardware allows to setup 2Heads1OR mode
     * between head-0 and head-1, or head-2 and head-3 only.
     */

    if (HeadIsFree(pDevEvo, apiHead, freeHwHeadsMask, 0) &&
            HeadIsFree(pDevEvo, apiHead, freeHwHeadsMask, 1)) {
        return NVBIT(0) | NVBIT(1);
    }

    if (HeadIsFree(pDevEvo, apiHead, freeHwHeadsMask, 2) &&
            HeadIsFree(pDevEvo, apiHead, freeHwHeadsMask, 3)) {
        return NVBIT(2) | NVBIT(3);
    }

    return 0;
}

static NvBool AssignProposedHwHeadsGeneric(
    const NVDispEvoRec *pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NvU32 *pFreeHwHeadsMask,
    const NvBool b2Heads1Or)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 freeHwHeadsMask = *pFreeHwHeadsMask;
    NvU32 ret = TRUE;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);

        if (!pProposedApiHead->changed ||
                nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList) ||
                (pProposedApiHead->hwHeadsMask != 0x0)) {
            continue;
        }

        /*
         * The hardware-head assigment for the DSI and dp-serializer dpys
         * should be already done.
         */
        nvAssert((pDpyEvo->pConnectorEvo->signalFormat !=
                    NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) &&
                    !nvConnectorIsDPSerializer(pDpyEvo->pConnectorEvo));

        NvU32 foundHeadsMask = 0x0;

        if (b2Heads1Or) {
            if (!nvEvoUse2Heads1OR(
                    pDpyEvo,
                    &pProposedApiHead->timings,
                    &pProposedApiHead->modeValidationParams)) {
                continue;
            }
            foundHeadsMask = GetFree2Heads1ORHeadsMask(pDevEvo, apiHead,
                                                       freeHwHeadsMask);
        } else {
            nvAssert(!nvEvoUse2Heads1OR(
                        pDpyEvo,
                        &pProposedApiHead->timings,
                        &pProposedApiHead->modeValidationParams));

            NvU32 foundHead = GetFreeHeads(pDevEvo, apiHead, pDpyEvo,
                                           freeHwHeadsMask);
            if (foundHead != NV_INVALID_HEAD) {
                foundHeadsMask = NVBIT(foundHead);
            }
        }

        if (foundHeadsMask != 0x0) {
            nvAssignHwHeadsMaskProposedApiHead(
                pProposedApiHead, foundHeadsMask);
            freeHwHeadsMask &= ~pProposedApiHead->hwHeadsMask;
        } else {
            ret = FALSE;
            goto done;
        }
    }

done:
    *pFreeHwHeadsMask &= freeHwHeadsMask;
    return ret;
}

static void ClearHwHeadsMaskOneApiHead(const NVDispEvoRec *pDispEvo,
                                       const NvU32 apiHead,
                                       NVProposedModeSetHwState *pProposed)
{
    const NvU32 sd = pDispEvo->displayOwner;
    NVProposedModeSetHwStateOneDisp *pProposedDisp =
        &pProposed->disp[sd];
    NVProposedModeSetStateOneApiHead *pProposedApiHead =
        &pProposedDisp->apiHead[apiHead];
    NvU32 head;

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
        nvkms_memset(&pProposedDisp->head[head],
                     0, sizeof(pProposedDisp->head[head]));
        nvkms_memset(&pProposed->sd[sd].head[head],
                     0, sizeof(pProposed->sd[sd].head[head]));
    }
    nvAssignHwHeadsMaskProposedApiHead(pProposedApiHead, 0);
}

static
void ClearIncompatibleHwHeadsMaskOneDisp(const NVDispEvoRec *pDispEvo,
                                         NVProposedModeSetHwState *pProposed)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NVProposedModeSetHwStateOneDisp *pProposedDisp =
        &pProposed->disp[sd];

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
        if (!pProposedApiHead->changed || (pDpyEvo == NULL)) {
            continue;
        }

        NvBool current2Heads1OrState =
            (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1);

        const NvBool new2Heads1OrState =
            nvEvoUse2Heads1OR(pDpyEvo,
                              &pProposedApiHead->timings,
                              &pProposedApiHead->modeValidationParams);

        if (new2Heads1OrState != current2Heads1OrState) {
            ClearHwHeadsMaskOneApiHead(pDispEvo, apiHead, pProposed);
        }
    }
}

static
NvBool AssignProposedHwHeadsOneDisp(const NVDispEvoRec *pDispEvo,
                                    NVProposedModeSetHwState *pProposed)
{
    const NvU32 sd = pDispEvo->displayOwner;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVProposedModeSetHwStateOneDisp *pProposedDisp =
        &pProposed->disp[sd];
    NvU32 freeHwHeadsMask;
    /*
     * In first pass, keep the all existing api-head to
     * hardware-head(s) assignment unchanged.
     */
    NvU32 pass = 0;

    ClearIncompatibleHwHeadsMaskOneDisp(pDispEvo, pProposed);

repeatHwHeadsAssigment:
    freeHwHeadsMask = NVBIT(pDevEvo->numHeads) - 1;
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        const NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        nvAssert((pProposedApiHead->hwHeadsMask == 0) ||
                    !nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList));
        freeHwHeadsMask &= ~pProposedApiHead->hwHeadsMask;
    }

    if (!AssignProposedHwHeadsForDsiConnector(pDispEvo, pProposedDisp,
                                              &freeHwHeadsMask) ||
            !AssignProposedHwHeadsForDPSerializer(pDispEvo, pProposedDisp,
                                                  &freeHwHeadsMask) ||
            !AssignProposedHwHeadsGeneric(pDispEvo, pProposedDisp,
                                          &freeHwHeadsMask,
                                          TRUE /* b2Heads1Or */) ||
            !AssignProposedHwHeadsGeneric(pDispEvo, pProposedDisp,
                                          &freeHwHeadsMask,
                                          FALSE /* b2Heads1Or */)) {
        if (pass == 1) {
            return FALSE;
        }

        /*
         * In second pass, do the fresh hardware-head(s) assigment for
         * the all changed api-heads.
         */
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (pProposedDisp->apiHead[apiHead].changed) {
                ClearHwHeadsMaskOneApiHead(pDispEvo, apiHead, pProposed);
            }
        }
        pass = 1;
        goto repeatHwHeadsAssigment;
    }

    return TRUE;
}

/*!
 * Assign the NVProposedModeSetHwState structure.
 *
 * Use the current hardware state, and the requested changes in
 * pRequest, to determine what the desired resulting hardware
 * configuration for the device should be.
 *
 * \param[in]   pDevEvo    The device whose hardware state is to be changed.
 * \param[in]   pOpenDev   The pOpenDev of the client doing the modeset.
 * \param[in]   pRequest   The requested changes to apply to the hardware state.
 * \param[out]  pReply     The reply structure for the client; if we cannot
 *                         apply some portion of pRequest, set the
 *                         corresponding status field in pReply to a
 *                         non-SUCCESS value.
 * \param[out]  pProposed  The proposed resulting hardware state.
 *
 * \return If the requested changes could be applied to pProposed,
 *         return TRUE.  If the requested changes could not be applied
 *         to pProposed, set the corresponding status field in pReply
 *         to a non-SUCCESS value and return FALSE.
 */
static NvBool
AssignProposedModeSetHwState(NVDevEvoRec *pDevEvo,
                             const struct NvKmsPerOpenDev *pOpenDev,
                             const struct NvKmsSetModeRequest *pRequest,
                             struct NvKmsSetModeReply *pReply,
                             NVProposedModeSetHwState *pProposed)
{
    NvU32 sd;
    NVDispEvoPtr pDispEvo;
    NvBool ret = TRUE;
    /* If more than one head will enable VRR on Pascal, disallow Adaptive-Sync */
    const enum NvKmsAllowAdaptiveSync prohibitAdaptiveSync =
        (!pDevEvo->hal->caps.supportsDisplayRate &&
            (CountProposedVrrApiHeads(pDevEvo, pRequest) > 1));

    /* Initialize pProposed with the current hardware configuration. */
    InitProposedModeSetHwState(pDevEvo, pOpenDev, pProposed);

    /* Update pProposed with the requested changes from the client. */

    pProposed->allowHeadSurfaceInNvKms =
        nvGetAllowHeadSurfaceInNvKms(pDevEvo, pOpenDev, pRequest);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        const struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[sd];

        if ((pRequest->requestedDispsBitMask & (1 << sd)) == 0) {
            continue;
        }

        NVProposedModeSetHwStateOneDisp *pProposedDisp =
            &pProposed->disp[sd];

        pDispEvo = pDevEvo->pDispEvo[sd];

        /* Construct the per api head proposed modeset state */
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];
            NVProposedModeSetStateOneApiHead *pProposedApiHead =
                &pProposedDisp->apiHead[apiHead];
            const NVDpyEvoRec *pDpyEvo;
            NvU32 layer;

            if ((pRequestDisp->requestedHeadsBitMask & (1 << apiHead)) == 0) {
                /*
                 * Just leave the head alone so it keeps its current
                 * configuration.
                 */
                continue;
            }

            pDpyEvo = nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);
            if (pDpyEvo == NULL) {
                /*
                 * If newDpyIdList is empty or does not find a valid dpy in
                 * newDpyIdList, then the head should be disabled.
                 * Clear the pProposedHead, so that no state leaks to the new
                 * configuration.
                 */
                ClearHwHeadsMaskOneApiHead(pDispEvo, apiHead, pProposed);
                nvkms_memset(pProposedApiHead, 0, sizeof(*pProposedApiHead));
                pProposedApiHead->changed = TRUE;
                continue;
            }

            pProposedApiHead->changed = TRUE;
            pProposedApiHead->dpyIdList = pRequestHead->dpyIdList;
            pProposedApiHead->activeRmId =
                nvRmAllocDisplayId(pDispEvo, pProposedApiHead->dpyIdList);
            if (pProposedApiHead->activeRmId == 0x0) {
                /* XXX Need separate error code? */
                pReply->disp[sd].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY;
                ret = FALSE;
                continue;
            }

            if ((nvDpyGetPossibleApiHeadsMask(pDpyEvo) & NVBIT(apiHead)) == 0) {
                pReply->disp[sd].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY;
                ret = FALSE;
                continue;
            }

            /*
             * Get the requested modetimings for this head.  If that
             * fails, record in the reply that getting the mode
             * failed.  In the case of failure, continue to the next
             * head so that if additional heads fail, we can report
             * more complete failure information to the client.
             */
            if (!nvGetHwModeTimings(pDispEvo,
                                    apiHead,
                                    pRequestHead,
                                    &pProposedApiHead->timings,
                                    &pProposedApiHead->attributes.color,
                                    &pProposedApiHead->infoFrame.ctrl)) {
                pReply->disp[sd].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                ret = FALSE;
                continue;
            }

            if (!AssignProposedModeSetColorSpaceAndColorRangeSpecified(
                    pRequestHead, pProposedApiHead)) {
                pReply->disp[sd].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                ret = FALSE;
                continue;
            }

            AdjustHwModeTimingsForVrr(pDispEvo,
                                      pRequestHead,
                                      prohibitAdaptiveSync,
                                      &pProposedApiHead->timings);

            pProposedApiHead->stereo.mode =
                pRequestHead->modeValidationParams.stereoMode;
            pProposedApiHead->stereo.isAegis = pDpyEvo->stereo3DVision.isAegis;
            pProposedApiHead->infoFrame.hdTimings =
                nvEvoIsHDQualityVideoTimings(&pProposedApiHead->timings);

            pProposedApiHead->modeValidationParams =
                pRequestHead->modeValidationParams;

            pProposedApiHead->attributes.digitalSignal =
                nvGetDefaultDpyAttributeDigitalSignalValue(pDpyEvo->pConnectorEvo);
            if (pProposedApiHead->timings.protocol ==
                    NVKMS_PROTOCOL_SOR_HDMI_FRL) {
                nvAssert(pProposedApiHead->attributes.digitalSignal ==
                            NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_TMDS);
                pProposedApiHead->attributes.digitalSignal =
                    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_HDMI_FRL;
            }

            pProposedApiHead->attributes.dvc =
                pDpyEvo->currentAttributes.dvc;

            /* Image sharpening is available when scaling is enabled. */
            pProposedApiHead->attributes.imageSharpening.available =
                nvIsImageSharpeningAvailable(&pProposedApiHead->timings.viewPort);
            pProposedApiHead->attributes.imageSharpening.value =
                pDpyEvo->currentAttributes.imageSharpening.value;

            /*
             * If InheritPreviousModesetState() returns FALSE, it implies that
             * there was a modeset ownership change since the last modeset. If
             * input/output lut not specified by the new modeset owner then
             * keep them disabled by default.
             */
            if (!InheritPreviousModesetState(pDevEvo, pOpenDev)) {
                pProposedApiHead->lut = pRequestHead->flip.lut;

                if (!pRequestHead->flip.lut.input.specified) {
                    pProposedApiHead->lut.input.specified = TRUE;
                    pProposedApiHead->lut.input.end = 0;
                }

                if (!pRequestHead->flip.lut.output.specified) {
                    pProposedApiHead->lut.output.specified = TRUE;
                    pProposedApiHead->lut.output.enabled = FALSE;
                }
            } else if (pRequestHead->flip.lut.input.specified) {
                pProposedApiHead->lut = pRequestHead->flip.lut;
            } else {
                pProposedApiHead->lut.input.specified = FALSE;
            }

            if (pRequestHead->flip.tf.specified) {
                pProposedApiHead->tf = pRequestHead->flip.tf.val;

                // If enabling HDR TF...
                // XXX HDR TODO: Handle other transfer functions
                if (pProposedApiHead->tf == NVKMS_OUTPUT_TF_PQ) {
                    // Cannot be an SLI configuration.
                    // XXX HDR TODO: Test SLI Mosaic + HDR and remove this check
                    if (pDevEvo->numSubDevices > 1) {
                        ret = FALSE;
                        pReply->disp[sd].head[apiHead].status =
                            NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                        continue;
                    }

                    /* NVKMS_OUTPUT_TF_PQ requires the RGB color space */
                    if (pProposedApiHead->attributes.color.format !=
                            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) {
                        ret = FALSE;
                        pReply->disp[sd].head[apiHead].status =
                            NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                        continue;
                    }
                }
            }

            if (pRequestHead->flip.hdrInfoFrame.specified) {
                pProposedApiHead->hdrInfoFrameOverride =
                    pRequestHead->flip.hdrInfoFrame.enabled;
            }

            for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                if (pRequestHead->flip.layer[layer].hdr.specified) {
                    if (pRequestHead->flip.layer[layer].hdr.enabled) {
                        pProposedApiHead->hdrStaticMetadataLayerMask |=
                            1 << layer;
                    } else {
                        pProposedApiHead->hdrStaticMetadataLayerMask &=
                            ~(1 << layer);
                    }
                }
            }

            // If enabling HDR signaling...
            // XXX HDR TODO: Handle other colorimetries
            if (pProposedApiHead->hdrInfoFrameOverride ||
                (pProposedApiHead->hdrStaticMetadataLayerMask != 0) ||
                (pProposedApiHead->attributes.color.colorimetry ==
                    NVKMS_OUTPUT_COLORIMETRY_BT2100)) {
                const NVDpyEvoRec *pDpyEvo;

                // All dpys on apiHead must support HDR.
                FOR_ALL_EVO_DPYS(pDpyEvo,
                                 pProposedApiHead->dpyIdList,
                                 pDispEvo) {
                    if (!nvDpyIsHDRCapable(pDpyEvo)) {
                        ret = FALSE;
                        pReply->disp[sd].head[apiHead].status =
                            NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                        break;
                    }
                }

                if (ret == FALSE) {
                    continue;
                }
            }

            if (pRequestHead->flip.viewPortIn.specified) {
                pProposedApiHead->viewPortPointIn =
                    pRequestHead->flip.viewPortIn.point;
            }
        } /* apiHead */

        if (!AssignProposedHwHeadsOneDisp(pDispEvo, pProposed)) {
            pReply->disp[sd].status =
                NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_TO_ASSIGN_HARDWARE_HEADS;
            ret = FALSE;
            continue;
        }

        /*
         * Construct the per hardware head proposed modeset/flip state, and
         * assign the dependant per api head modeset parameters.
         */
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const struct NvKmsSetModeOneHeadRequest *pRequestHead;
            NVProposedModeSetStateOneApiHead *pProposedApiHead =
                &pProposedDisp->apiHead[apiHead];
            const NvU32 primaryHead =
                nvGetPrimaryHwHeadFromMask(pProposedApiHead->hwHeadsMask);
            const NvU32 numMergeHeadSections =
                nvPopCount32(pProposedApiHead->hwHeadsMask);
            const NVDpyEvoRec *pDpyEvo =
                nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
            NVProposedModeSetHwStateOneHead *pProposedPrimaryHead;
            NvU32 secondaryMergeHeadSection = 1;
            NvU32 head;

            nvAssert((pProposedApiHead->hwHeadsMask != 0x0) ||
                        (pDpyEvo == NULL));

            if (!pProposedApiHead->changed ||
                    (pProposedApiHead->hwHeadsMask == 0x0)) {
                continue;
            }

            nvAssert(primaryHead != NV_INVALID_HEAD);
            pProposedPrimaryHead = &pProposedDisp->head[primaryHead];

            nvAssert((pRequestDisp->requestedHeadsBitMask & (1 << apiHead)) != 0);
            pRequestHead = &pRequestDisp->head[apiHead];

            enum NvKmsSetModeOneHeadStatus status =
                NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS;

            FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
                NVProposedModeSetHwStateOneHead *pProposedHead =
                    &pProposedDisp->head[head];

                pProposedHead->mergeHeadSection =
                    (head == primaryHead) ? 0 : (secondaryMergeHeadSection++);

                if (!nvEvoGetSingleMergeHeadSectionHwModeTimings(
                        &pProposedApiHead->timings,
                        numMergeHeadSections,
                        &pProposedHead->timings)) {
                    status = NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                    break;
                }

                /*
                 * Broadcast the connector and the requested flip state across
                 * the corresponding hardware heads.
                 */
                pProposedHead->pConnectorEvo = pDpyEvo->pConnectorEvo;
                if (!AssignProposedModeSetNVFlipEvoHwState(
                        pDevEvo,
                        pOpenDev,
                        sd,
                        head,
                        &pRequest->disp[sd].head[apiHead],
                        &pProposed->sd[sd].head[head].flip,
                        pProposedHead,
                        pRequest->commit)) {
                    status = NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                    break;
                }
            }

            if (status != NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS) {
                pReply->disp[sd].head[apiHead].status = status;
                ret = FALSE;
                continue; /* next api head */
            }

            nvAssert(nvPopCount32(pProposedApiHead->hwHeadsMask) <= 2);

            /*
             * Query the per api head HDMI FRL configuration, and pass it to
             * the primary head. Save the dsc info into the per api head
             * proposed modeset state, to broadcast it onto all hardware heads
             * during modeset.
             */
            if (!nvHdmiFrlQueryConfig(pDpyEvo,
                                      &pRequestHead->mode.timings,
                                      &pProposedApiHead->timings,
                                      &pProposedApiHead->attributes.color,
                                      (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1)
                                        /* b2Heads1Or */,
                                      &pProposedApiHead->modeValidationParams,
                                      &pProposedPrimaryHead->hdmiFrlConfig,
                                      &pProposedApiHead->dscInfo)) {
                pReply->disp[sd].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                ret = FALSE;
                continue;
            }

            /*
             * Construct the api head audio state, and pass it
             * to the primary hardware head.
             */
            nvHdmiDpConstructHeadAudioState(pProposedApiHead->activeRmId,
                                            pDpyEvo,
                                            &pProposedPrimaryHead->audio);

            /*
             * If the modeset is flipping to a depth 30 surface, record this as
             * a hint to headSurface, so it can also allocate its surfaces at
             * depth 30.
             */
            {
                const NVFlipEvoHwState *pFlip =
                    &pProposed->sd[sd].head[primaryHead].flip;
                const NVSurfaceEvoRec *pSurfaceEvo =
                    pFlip->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];

                pProposedApiHead->hs10bpcHint =
                    (pSurfaceEvo != NULL) &&
                    ((pSurfaceEvo->format == NvKmsSurfaceMemoryFormatA2B10G10R10) ||
                     (pSurfaceEvo->format == NvKmsSurfaceMemoryFormatX2B10G10R10));
            }
        } /* apiHead */
    } /* pDispEvo */

    if (!ret) {
        ClearProposedModeSetHwState(pDevEvo, pProposed, FALSE /* committed */);
    }

    return ret;
}


/*!
 * Validate the proposed configuration on the specified disp using IMP.
 *
 * \param[in]   pDispEvo          The disp to which pProposedDisp is to be applied.
 * \param[in]   pProposed         The requested configuration.
 * \param[in]   pProposedDisp     The requested configuration for this disp.
 * \param[out]  pWorkArea         The scratch space for the current modeset request.
 *
 * \return      If pProposedDisp passes IMP, return TRUE.  Otherwise,
 *              return FALSE.
 */
static NvBool
ValidateProposedModeSetHwStateOneDispImp(NVDispEvoPtr pDispEvo,
                                         const NVProposedModeSetHwState
                                         *pProposed,
                                         NVProposedModeSetHwStateOneDisp
                                         *pProposedDisp,
                                         NVModeSetWorkArea *pWorkArea)
{
    NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP];
    NvBool skipImpCheck = TRUE, requireBootClocks = FALSE;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 modesetRequestedHeadsMask = 0;
    NVEvoReallocateBandwidthMode reallocBandwidth = pDevEvo->isSOCDisplay ?
        NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE :
        NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE;

    nvkms_memset(&timingsParams, 0, sizeof(timingsParams));

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        const NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];

        const NvBool skipImpCheckThisHead =
            (pProposedApiHead->modeValidationParams.overrides &
             NVKMS_MODE_VALIDATION_NO_EXTENDED_GPU_CAPABILITIES_CHECK) != 0;

        const NvBool requireBootClocksThisHead =
            (pProposedApiHead->modeValidationParams.overrides &
             NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS) != 0;

        NvU32 head;

        if (pProposedApiHead->hwHeadsMask == 0x0) {
            continue;
        }

        /*
         * Don't try to downgrade heads which are not marked as changed.
         * This could lead to unchanged/not-requested heads hogging all
         * the disp bandwidth and preventing otherwise possible modesets,
         * but it fixes the cases where we could have downgraded unchanged/
         * not-requested heads without NVKMS clients knowing about it.
         * Even if we add some mechanism through the modeset reply to notify
         * clients about such a change, not all clients might be in a position
         * to handle it. This seems to be a fair trade-off for Orin, as by
         * default all heads are initialized with minimal usage bounds.
         */
        if (pProposedApiHead->changed) {
            modesetRequestedHeadsMask |= pProposedApiHead->hwHeadsMask;
        }

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposedDisp->head[head];

            timingsParams[head].pConnectorEvo = pProposedHead->pConnectorEvo;
            timingsParams[head].activeRmId = pProposedApiHead->activeRmId;
            timingsParams[head].pixelDepth =
                nvEvoDpyColorToPixelDepth(&pProposedApiHead->attributes.color);
            timingsParams[head].pTimings = &pProposedHead->timings;
            timingsParams[head].enableDsc = (pProposedApiHead->dscInfo.type !=
                NV_DSC_INFO_EVO_TYPE_DISABLED);
            timingsParams[head].dscSliceCount =
                pProposedApiHead->dscInfo.sliceCount;
            timingsParams[head].possibleDscSliceCountMask =
                pProposedApiHead->dscInfo.possibleSliceCountMask;
            nvAssert(nvPopCount32(pProposedApiHead->hwHeadsMask) <= 2);
            timingsParams[head].b2Heads1Or =
                (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1);
            timingsParams[head].pUsage =
                &pProposedHead->timings.viewPort.guaranteedUsage;
            timingsParams[head].pMultiTileConfig =
                &pProposedHead->multiTileConfig;
        }

        skipImpCheck = skipImpCheck && skipImpCheckThisHead;
        requireBootClocks = requireBootClocks || requireBootClocksThisHead;
    }

    if (skipImpCheck &&
        reallocBandwidth == NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE) {
        return TRUE;
    }

    if (!nvValidateImpOneDispDowngrade(pDispEvo, timingsParams,
                                       requireBootClocks,
                                       reallocBandwidth,
                                       modesetRequestedHeadsMask)) {
        return FALSE;
    }

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        NvU32 primaryHead = nvGetPrimaryHwHeadFromMask(
            pProposedApiHead->hwHeadsMask);
        NvU32 head;

        if (!pProposedApiHead->changed ||
                (primaryHead == NV_INVALID_HEAD) ||
                (pProposedApiHead->dscInfo.type == NV_DSC_INFO_EVO_TYPE_DISABLED)) {
            continue;
        }

        pProposedApiHead->dscInfo.sliceCount =
            timingsParams[primaryHead].dscSliceCount;

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
            nvAssert(timingsParams[head].dscSliceCount ==
                        pProposedApiHead->dscInfo.sliceCount);
        }
    }

    if (pDevEvo->isSOCDisplay) {
        NvBool ret;
        struct NvKmsUsageBounds *guaranteedAndProposed =
            nvCalloc(1, sizeof(*guaranteedAndProposed) *
                            NVKMS_MAX_HEADS_PER_DISP);
        if (guaranteedAndProposed == NULL) {
            return FALSE;
        }

        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVProposedModeSetStateOneApiHead *pProposedApiHead =
                &pProposedDisp->apiHead[apiHead];
            NvU32 head;

            FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
                NVProposedModeSetHwStateOneHead *pProposedHead =
                    &pProposedDisp->head[head];
                const struct NvKmsUsageBounds *pProposedUsage;

                if (pProposedApiHead->changed) {
                    pProposedUsage = &pProposed->sd[0].head[head].flip.usage;
                } else {
                    pProposedUsage =
                        &pDevEvo->gpus[0].headState[head].preallocatedUsage;
                }

                nvUnionUsageBounds(&pProposedHead->timings.viewPort.guaranteedUsage,
                                   pProposedUsage, &guaranteedAndProposed[head]);
                timingsParams[head].pUsage = &guaranteedAndProposed[head];
            }
        }

        ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                                   requireBootClocks,
                                   reallocBandwidth,
                                   &pWorkArea->postModesetIsoBandwidthKBPS,
                                   &pWorkArea->postModesetDramFloorKBPS,
                                   0x0 /* changedHeadsMask */);

        nvFree(guaranteedAndProposed);

        if (!ret) {
            return FALSE;
        }

        nvScheduleLowerDispBandwidthTimer(pDevEvo);
    }

    return TRUE;
}

static NvBool SkipDisplayPortBandwidthCheck(
    const NVProposedModeSetStateOneApiHead *pProposedApiHead)
{
    return (pProposedApiHead->modeValidationParams.overrides &
            NVKMS_MODE_VALIDATION_NO_DISPLAYPORT_BANDWIDTH_CHECK) != 0;
}

static NvBool DowngradeColorSpaceAndBpcOneHead(
    const NVDispEvoRec *pDispEvo,
    NVProposedModeSetStateOneApiHead *pProposedApiHead)
{
    NVDpyAttributeColor dpyColor = pProposedApiHead->attributes.color;
    NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList,
                                pDispEvo);
    const NvKmsDpyOutputColorFormatInfo supportedColorFormats =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);

    if (!nvDowngradeColorSpaceAndBpc(pDpyEvo, &supportedColorFormats, &dpyColor)) {
        return FALSE;
    }

    if (pProposedApiHead->colorRangeSpecified &&
        (dpyColor.range != pProposedApiHead->attributes.color.range)) {
        return FALSE;
    }

    if (pProposedApiHead->colorBpcSpecified &&
        (dpyColor.bpc != pProposedApiHead->attributes.color.bpc)) {
        return FALSE;
    }

    if (pProposedApiHead->colorSpaceSpecified &&
        (dpyColor.format != pProposedApiHead->attributes.color.format)) {
        return FALSE;
    }

    pProposedApiHead->attributes.color = dpyColor;
    return TRUE;
}

static NvBool DowngradeColorSpaceAndBpcOneDisp(
    const NVDispEvoRec              *pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const NVConnectorEvoRec         *pConnectorEvo)
{
    NvBool ret = FALSE;
    NvU32 apiHead;

    /*
     * In DP-MST case, many heads can share same connector and dp-bandwidth
     * therefore its necessary to validate and downgrade dp-pixel-depth across
     * all head which are sharing same connector before retry.
     */
    for (apiHead = 0; apiHead < pDispEvo->pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);

        if (!pProposedApiHead->changed ||
                SkipDisplayPortBandwidthCheck(pProposedApiHead)) {
            continue;
        }

        if ((pDpyEvo != NULL) &&
            (pDpyEvo->pConnectorEvo == pConnectorEvo) &&
            DowngradeColorSpaceAndBpcOneHead(pDispEvo, pProposedApiHead)) {
            ret = TRUE;
        }
    }

    return ret;
}

static NvU32 SetDpLibImpParamsOneConnectorEvo(
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const NVConnectorEvoRec *pConnectorEvo,
    NVDpLibIsModePossibleParamsRec *pParams)
{
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 attachedHeadsMask = 0x0;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NvU32 head =
            nvGetPrimaryHwHeadFromMask(pProposedApiHead->hwHeadsMask);
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
        const NvBool b2Heads1Or =
            nvPopCount32(pProposedApiHead->hwHeadsMask) > 1;

        nvAssert(nvPopCount32(pProposedApiHead->hwHeadsMask) <= 2);

        if ((pDpyEvo == NULL) || (pDpyEvo->pConnectorEvo != pConnectorEvo) ||
            SkipDisplayPortBandwidthCheck(pProposedApiHead)) {
            continue;
        }

        /*
         * We know that `head` is valid since we have a non-null pDpyEvo;
         * this means that either the client did not change this apiHead and the
         * existing configuration already had a valid HW head assignment,
         * or the client did change this apiHead with a requested dpy,
         * in which case AssignProposedHwHeadsOneDisp() guarantees a valid 
         * assignment.
         *
         * Assert this so Coverity doesn't complain about NVBIT(head).
         */

        nvAssert(head != NV_INVALID_HEAD);
        nvAssert((NVBIT(head) & attachedHeadsMask) == 0x0);

        pParams->head[head].displayId = pProposedApiHead->activeRmId;
        pParams->head[head].dpyIdList = pProposedApiHead->dpyIdList;
        pParams->head[head].colorSpace = pProposedApiHead->attributes.color.format;
        pParams->head[head].colorBpc = pProposedApiHead->attributes.color.bpc;
        pParams->head[head].pModeValidationParams =
            &pProposedApiHead->modeValidationParams;
        pParams->head[head].pTimings = &pProposedApiHead->timings;
        pParams->head[head].b2Heads1Or = b2Heads1Or;
        pParams->head[head].pDscInfo = &pProposedApiHead->dscInfo;

        attachedHeadsMask |= NVBIT(head);
    }

    return attachedHeadsMask;
}

static NvBool DowngradeColorSpaceAndBpcOneConnectorEvo(
    const NVConnectorEvoRec *pConnectorEvo,
    const NvU32 failedHeadMask,
    NVProposedModeSetHwStateOneDisp *pProposedDisp)
{
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    /*
     * First, try to downgrade the pixel depth for the failed heads.
     * If the pixel depth for the failed current heads is not possible
     * to downgrade further then try to downgrade the pixel depth of
     * other changed heads which are sharing same connector and
     * dp-bandwidth.
     */

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];

        if (((pProposedApiHead->hwHeadsMask & failedHeadMask) == 0x0) ||
                !pProposedApiHead->changed) {
            continue;
        }

        if (DowngradeColorSpaceAndBpcOneHead(pDispEvo, pProposedApiHead)) {
            return TRUE;
        }
    }


    if (DowngradeColorSpaceAndBpcOneDisp(pDispEvo,
                                         pProposedDisp,
                                         pConnectorEvo)) {
        return TRUE;
    }

    return FALSE;
}

static NvBool ValidateProposedModeSetHwStateOneConnectorDPlib(
    const NVConnectorEvoRec *pConnectorEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const enum NVDpLibIsModePossibleQueryMode queryMode)
{
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 attachedHeadsMask = 0x0, failedHeadMask = 0x0;
    NvBool bResult = TRUE;
    NVDpLibIsModePossibleParamsRec *pDpLibImpParams =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_DPLIB_IS_MODE_POSSIBLE_PARAMS,
                      sizeof(*pDpLibImpParams));

    nvAssert(pDpLibImpParams != NULL);

    if (!nvConnectorUsesDPLib(pConnectorEvo)) {
        goto done;
    }

tryAgain:
    nvkms_memset(pDpLibImpParams, 0, sizeof(*pDpLibImpParams));

    attachedHeadsMask = 0x0;
    failedHeadMask = 0x0;
    bResult = TRUE;

    attachedHeadsMask = SetDpLibImpParamsOneConnectorEvo(pProposedDisp,
                                                         pConnectorEvo,
                                                         pDpLibImpParams);
    if (attachedHeadsMask == 0x0) {
        goto done;
    }

    pDpLibImpParams->queryMode = queryMode;

    bResult = nvDPLibIsModePossible(pConnectorEvo->pDpLibConnector,
                                    pDpLibImpParams,
                                    &failedHeadMask);

    if (!bResult) {
        if (DowngradeColorSpaceAndBpcOneConnectorEvo(pConnectorEvo,
                                                     failedHeadMask,
                                                     pProposedDisp)) {
            goto tryAgain;
        }

        /*
         * Cannot downgrade pixelDepth further --
         *   This proposed mode-set is not possible on this DP link, so fail.
         */
    }

done:
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_DPLIB_IS_MODE_POSSIBLE_PARAMS);
    return bResult;
}

/*!
 * Validate the DisplayPort bandwidth of the proposed disp configuration.
 *
 * \param[in]   pDispEvo       The disp to which pProposedDisp is to be applied.
 * \param[in]   pProposedDisp  The requested configuration.
 *
 * \return      If pProposedDisp passes the DP bandwidth check, return
 *              TRUE.  Otherwise, return FALSE.
 */
static NvBool ValidateProposedModeSetHwStateOneDispDPlib(
    NVDispEvoPtr                     pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const enum NVDpLibIsModePossibleQueryMode queryMode)
{
    NvBool dpIsModePossible = TRUE;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVConnectorEvoRec *pConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (!ValidateProposedModeSetHwStateOneConnectorDPlib(pConnectorEvo,
                                                             pProposedDisp,
                                                             queryMode)) {
            /*
             * The Dp link bandwidth check fails for an unchanged head --
             *   This proposed mode-set is not possible on this DP link, so fail.
             */
            dpIsModePossible = FALSE;
            break;
        }
    }

    if (dpIsModePossible &&
            (queryMode == NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP)) {
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const NVProposedModeSetStateOneApiHead *pProposedApiHead =
                &pProposedDisp->apiHead[apiHead];
            const NvU32 primaryHead =
                nvGetPrimaryHwHeadFromMask(pProposedApiHead->hwHeadsMask);
            const NVDpyEvoRec *pDpyEvo =
                nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
            NVProposedModeSetHwStateOneHead *pProposedPrimaryHead;

            nvAssert((pProposedApiHead->hwHeadsMask != 0x0) ||
                        (pDpyEvo == NULL));

            if ((pProposedApiHead->hwHeadsMask == 0x0)) {
                continue;
            }

            nvAssert(primaryHead != NV_INVALID_HEAD);
            pProposedPrimaryHead = &pProposedDisp->head[primaryHead];

            if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
                pProposedPrimaryHead->pDpLibModesetState = NULL;
                continue;
            }

            pProposedPrimaryHead->pDpLibModesetState =
                nvDPLibCreateModesetState(pDispEvo,
                                          primaryHead,
                                          pProposedApiHead->activeRmId,
                                          pProposedApiHead->dpyIdList,
                                          pProposedApiHead->attributes.color.format,
                                          pProposedApiHead->attributes.color.bpc,
                                          &pProposedApiHead->timings,
                                          &pProposedApiHead->dscInfo);
            if (pProposedPrimaryHead->pDpLibModesetState == NULL) {
                dpIsModePossible = FALSE;
                goto done;
            }
        }
    }

done:
    return dpIsModePossible;
}

static NvBool VblankCallbackListsAreEmpty(
    const NVDispVblankApiHeadState *pVblankApiHeadState)
{
    ct_assert(ARRAY_LEN(pVblankApiHeadState->vblankCallbackList) == 2);

    return (nvListIsEmpty(&pVblankApiHeadState->vblankCallbackList[0]) &&
            nvListIsEmpty(&pVblankApiHeadState->vblankCallbackList[1]));
}

static void VBlankCallbackDeferredWork(void *dataPtr, NvU32 data32)
{
    NVDispVblankApiHeadState *pVblankApiHeadState = NULL;
    NVVBlankCallbackPtr pVBlankCallbackTmp = NULL;
    NVVBlankCallbackPtr pVBlankCallback = NULL;
    NVDispEvoPtr pDispEvo = dataPtr;
    NvU32 apiHead = data32;

    if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
        return;
    }

    pVblankApiHeadState = &pDispEvo->vblankApiHeadState[apiHead];

    /*
     * Increment the vblankCount here, so that any callbacks in the list can
     * rely on the same value.
     */
    pVblankApiHeadState->vblankCount++;

    for (NvU32 i = 0; i < ARRAY_LEN(pVblankApiHeadState->vblankCallbackList); i++) {

        nvListForEachEntry_safe(pVBlankCallback,
                                pVBlankCallbackTmp,
                                &pVblankApiHeadState->vblankCallbackList[i],
                                vblankCallbackListEntry) {
            pVBlankCallback->pCallback(pDispEvo, pVBlankCallback);
        }
    }
}

static void VBlankCallback(void *pParam1, void *pParam2)
{
    const NvU32 apiHead = (NvU32)(NvUPtr)pParam2;

    (void) nvkms_alloc_timer_with_ref_ptr(
               VBlankCallbackDeferredWork,
               pParam1, /* ref_ptr to pDispEvo */
               apiHead,    /* dataU32 */
               0);      /* timeout: schedule the work immediately */
}


static void DisableVBlankCallbacks(const NVDevEvoRec *pDevEvo)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        NvU32 apiHead;

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];

            if (pApiHeadState->rmVBlankCallbackHandle != 0) {
                nvRmRemoveVBlankCallback(pDispEvo,
                                         pApiHeadState->rmVBlankCallbackHandle);
                pApiHeadState->rmVBlankCallbackHandle = 0;
            }
        }
    }
}

static void EnableVBlankCallbacks(const NVDevEvoRec *pDevEvo)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        NvU32 apiHead;

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];

            const NVDispVblankApiHeadState *pVblankApiHeadState =
                &pDispEvo->vblankApiHeadState[apiHead];

            nvAssert(pApiHeadState->rmVBlankCallbackHandle == 0);

            if (VblankCallbackListsAreEmpty(pVblankApiHeadState)) {
                continue;
            }

            const NvU32 hwHead =
                nvGetPrimaryHwHeadFromMask(pApiHeadState->hwHeadsMask);

            if (hwHead == NV_INVALID_HEAD) {
                continue;
            }

            pApiHeadState->rmVBlankCallbackHandle =
                nvRmAddVBlankCallback(pDispEvo, hwHead,
                                      VBlankCallback, (void *)(NvUPtr)apiHead);
        }
    }
}

/*!
 * Validate the proposed configuration on the specified disp.
 *
 * \param[in]   pDispEvo       The disp to which pProposedDisp is to be applied.
 * \param[in]   pProposedDisp  The requested configuration.
 * \param[out]  pReplyDisp     The reply structure for the client.
 * \param[out]  pWorkArea      The scratch space for the current modeset request.
 *
 * \return      If pProposedDisp is valid, return TRUE.  Otherwise, set the
 *              appropriate status fields in pReplyDisp to non-SUCCESS,
 *              and return FALSE.
 */
static NvBool
ValidateProposedModeSetHwStateOneDisp(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwState *pProposed,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    struct NvKmsSetModeOneDispReply *pReplyDisp,
    NVModeSetWorkArea *pWorkArea)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVDpyIdList dpyIdList;

    /*
     * Check that the requested configuration of connectors can be
     * driven simultaneously.
     */
    dpyIdList = nvEmptyDpyIdList();
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVDpyEvoPtr pDpyEvo;
        FOR_ALL_EVO_DPYS(pDpyEvo,
                         pProposedDisp->apiHead[apiHead].dpyIdList, pDispEvo) {
            dpyIdList = nvAddDpyIdToDpyIdList(pDpyEvo->pConnectorEvo->displayId,
                                              dpyIdList);
        }
    }

    if (!nvRmIsPossibleToActivateDpyIdList(pDispEvo, dpyIdList)) {
        pReplyDisp->status = NVKMS_SET_MODE_ONE_DISP_STATUS_INCOMPATIBLE_DPYS;
        return FALSE;
    }

    /*
     * Check that no dpyId is used by multiple heads.
     */
    dpyIdList = nvEmptyDpyIdList();
    for (NvU32 apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        /*
         * Intersect the proposed dpys for this head with the
         * accumulated list of dpys for this disp; if the intersection
         * is not empty, a dpy is proposed to be used on multiple
         * api heads.
         */
        NVDpyIdList proposedDpyIdList =
            pProposedDisp->apiHead[apiHead].dpyIdList;
        NVDpyIdList intersectedDpyIdList =
            nvIntersectDpyIdListAndDpyIdList(dpyIdList, proposedDpyIdList);

        if (!nvDpyIdListIsEmpty(intersectedDpyIdList)) {
            pReplyDisp->status = NVKMS_SET_MODE_ONE_DISP_STATUS_DUPLICATE_DPYS;
            return FALSE;
        }

        dpyIdList = nvAddDpyIdListToDpyIdList(dpyIdList, proposedDpyIdList);
    }

    /*
     * Check ViewPortIn dimensions and ensure valid h/vTaps can be assigned.
     */
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NvU32 head;
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
            /* XXX assume the gpus have equal capabilities */
            const NVEvoScalerCaps *pScalerCaps =
                &pDevEvo->gpus[0].capabilities.head[head].scalerCaps;
            const NVHwModeTimingsEvoPtr pTimings = &pProposedDisp->head[head].timings;

            if (!nvValidateHwModeTimingsViewPort(pDevEvo, pScalerCaps, pTimings,
                                                 &dummyInfoString)) {
                pReplyDisp->head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                return FALSE;
            }
        }
    }

    /*
     * Check that the configuration fits DisplayPort bandwidth constraints.
     */
    if (!ValidateProposedModeSetHwStateOneDispDPlib(pDispEvo, pProposedDisp,
            NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_PRE_IMP)) {
        pReplyDisp->status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_DISPLAY_PORT_BANDWIDTH_CHECK;
        return FALSE;
    }

    /*
     * The pixelDepth value, which required to choose the dithering
     * configuration, gets finalized as part of the DisplayPort bandwidth
     * validation.
     */
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);

        if (!pProposedApiHead->changed || (pDpyEvo == NULL)) {
            continue;
        }

        nvChooseDitheringEvo(pDpyEvo->pConnectorEvo,
                             pProposedApiHead->attributes.color.bpc,
                             pProposedApiHead->attributes.color.colorimetry,
                             &pDpyEvo->requestedDithering,
                             &pProposedApiHead->attributes.dithering);
    }

    /*
     * Check that the configuration passes IMP.
     */
    if (!ValidateProposedModeSetHwStateOneDispImp(pDispEvo, pProposed,
                                                  pProposedDisp, pWorkArea)) {
        pReplyDisp->status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_EXTENDED_GPU_CAPABILITIES_CHECK;
        return FALSE;
    }

    if (!ValidateProposedModeSetHwStateOneDispDPlib(pDispEvo, pProposedDisp,
            NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP)) {
        pReplyDisp->status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_DISPLAY_PORT_BANDWIDTH_CHECK;
        return FALSE;
    }

    return TRUE;
}

/*!
 * Validate the proposed flip configuration on the specified sub device.
 *
 * \param[in]   pDispEvo       The disp to which pProposedDisp is to be applied.
 * \param[in]   pProposed      The requested configuration.
 * \param[out]  pProposedSd    The requested flip configuration.
 *
 * \return      If pProposedDisp is valid, return TRUE.  Otherwise, set the
 *              appropriate status fields in pReplyDisp to non-SUCCESS,
 *              and return FALSE.
 */
static NvBool
ValidateProposedFlipHwStateOneSubDev(
    const NVDevEvoRec *pDevEvo,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVProposedModeSetHwStateOneSubDev *pProposedSd,
    struct NvKmsSetModeOneDispReply *pReplyDisp)
{
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NvU32 head;
        const NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];

        if (!pProposedApiHead->changed ||
                nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList)) {
            continue;
        }

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
            nvOverrideScalingUsageBounds(
                pDevEvo,
                head,
                &pProposedSd->head[head].flip,
                &pProposedDisp->head[head].timings.viewPort.possibleUsage);

            if (!nvValidateFlipEvoHwState(pDevEvo,
                                          head,
                                          &pProposedDisp->head[head].timings,
                                          &pProposedSd->head[head].flip)) {
                pReplyDisp->head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*!
 * Validate the proposed configuration.
 *
 * \param[in]   pDevEvo    The device to which pProposed is to be applied.
 * \param[in]   pProposed  The requested configuration.
 * \param[out]  pReply     The reply structure for the client.
 * \param[out]  pWorkArea  The scratch space for the current modeset request.
 *
 * \return      If pProposed is valid, return TRUE.  Otherwise, set the
 *              appropriate status fields in pReply to non-SUCCESS,
 *              and return FALSE.
 */
static NvBool
ValidateProposedModeSetHwState(NVDevEvoPtr pDevEvo,
                               NVProposedModeSetHwState *pProposed,
                               struct NvKmsSetModeReply *pReply,
                               NVModeSetWorkArea *pWorkArea)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;
    NvBool ret = FALSE;
    NVProposedModeSetHwState *pActual =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_VALIDATE_PROPOSED_MODESET_HW_STATE,
                      sizeof(*pActual));

    /*
     * Copy the proposed modeset to a scratch area.  During the process below,
     * we may modify some parts of the timings.  If all of validation succeeds,
     * then we'll copy the modified version back out; if not, we don't want to
     * touch the input.
     */
    nvkms_memcpy(pActual, pProposed, sizeof(*pProposed));

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        NVProposedModeSetHwStateOneDisp *pProposedDisp =
            &pActual->disp[dispIndex];
        NVProposedModeSetHwStateOneSubDev *pProposedSd =
            &pActual->sd[pDispEvo->displayOwner];
        struct NvKmsSetModeOneDispReply *pReplyDisp;

        pReplyDisp = &pReply->disp[dispIndex];

        if (!ValidateProposedModeSetHwStateOneDisp(pDispEvo,
                                                   pActual,
                                                   pProposedDisp,
                                                   pReplyDisp,
                                                   pWorkArea)) {
            goto done;
        }

        if (!ValidateProposedFlipHwStateOneSubDev(pDevEvo,
                                                  pProposedDisp,
                                                  pProposedSd,
                                                  pReplyDisp)) {
            goto done;
        }
    }

    nvkms_memcpy(pProposed, pActual, sizeof(*pProposed));
    ret = TRUE;

done:
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_PROPOSED_MODESET_HW_STATE);

    return ret;
}

/*!
 * Ensure there is an SOR assigned for this pConnectorEvo, for use by
 * the pending modeset.
 *
 * In DP-MST, multiple heads may use the same pConnectorEvo, and they
 * should use the same SOR.
 *
 * When we call nvAssignSOREvo(), we have to tell RM which SORs have
 * already been assigned and need to be excluded from consideration for
 * the new SOR assignment request.
 */
static void AssignSor(const NVDispEvoRec *pDispEvo,
                      const NVProposedModeSetStateOneApiHead *pProposedApiHead,
                      NVModeSetWorkArea *pWorkArea)
{
    const NvU32 sd = pDispEvo->displayOwner;
    const NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;

    const NvBool b2Heads1Or =
        (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1);
    /*
     * In 2Heads1OR DP-MST case, NV0073_CTRL_CMD_DFP_ASSIGN_SOR needs
     * to be called with the dynamic displayId.
     */
    const NvU32 displayId = (nvDpyEvoIsDPMST(pDpyEvo) && b2Heads1Or) ?
        pProposedApiHead->activeRmId : nvDpyIdToNvU32(pConnectorEvo->displayId);
    /*
     * There are two usecases:
     *  1. Two 2Heads1OR DP-MST streams over a same connector.
     *  2. One legacy and one 2Heads1OR DP-MST stream over a same connector.
     * For both of these usecases, NV0073_CTRL_CMD_DFP_ASSIGN_SOR will get
     * called over the same connector multiple times,
     *
     * Two DP-MST streams over a same connector shares a same primary SOR index.
     *
     * Remove the already assigned assigned primary SOR index from
     * sorExcludeMask, so that NV0073_CTRL_CMD_DFP_ASSIGN_SOR
     * re-use it.
     */
    const NvU32 sorExcludeMask =
        (pConnectorEvo->or.primary != NV_INVALID_OR) ?
        (pWorkArea->sd[sd].assignedSorMask & ~NVBIT(pConnectorEvo->or.primary)) :
        (pWorkArea->sd[sd].assignedSorMask);

    if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
        // Nothing to do!
        return;
    }

    if (nvAssignSOREvo(pDpyEvo->pConnectorEvo, displayId, b2Heads1Or, sorExcludeMask)) {
        nvAssert(pConnectorEvo->or.primary != NV_INVALID_OR);
        pWorkArea->sd[sd].assignedSorMask |= nvConnectorGetORMaskEvo(pConnectorEvo);
    } else {
        nvAssert(!"Failed to assign SOR, this failure might cause hang!");
    }
}

static void
SetLinkHandOffOnDpDdcPartners(NVConnectorEvoRec *pConnectorEvo, NVDispEvoPtr pDispEvo, NvBool enable)
{
    NVConnectorEvoRec *pTmpConnectorEvo;
    FOR_ALL_EVO_CONNECTORS(pTmpConnectorEvo, pDispEvo) {
        if (nvDpyIdIsInDpyIdList(pTmpConnectorEvo->displayId,
                     pConnectorEvo->ddcPartnerDpyIdsList)) {
            if (nvConnectorUsesDPLib(pTmpConnectorEvo)) {
                nvDPSetLinkHandoff(pTmpConnectorEvo->pDpLibConnector, enable);
            }
        }
    }
}

static void
KickoffModesetUpdateState(
    NVDispEvoPtr pDispEvo,
    NVEvoModesetUpdateState *modesetUpdateState)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        nvEvoPreModesetRegisterFlipOccurredEvent(pDispEvo, head,
            modesetUpdateState);
    }

    if (!nvDpyIdListIsEmpty(modesetUpdateState->connectorIds)) {
        NVConnectorEvoRec *pConnectorEvo;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            if (!nvDpyIdIsInDpyIdList(pConnectorEvo->displayId,
                                      modesetUpdateState->connectorIds)) {
                continue;
            }

            if (pConnectorEvo->pDpLibConnector != NULL) {
                nvDPPreSetMode(pConnectorEvo->pDpLibConnector,
                               modesetUpdateState);
            } else if (nvConnectorIsDPSerializer(pConnectorEvo)) {
                nvDPSerializerPreSetMode(pDispEvo, pConnectorEvo);
            } else {
                if (nvIsConnectorActiveEvo(pConnectorEvo)) {
                    SetLinkHandOffOnDpDdcPartners(pConnectorEvo, pDispEvo, TRUE);
                }
            }
        }
    }

    nvDoIMPUpdateEvo(pDispEvo,
                     &modesetUpdateState->updateState);

    if (!nvDpyIdListIsEmpty(modesetUpdateState->connectorIds)) {
        NVConnectorEvoRec *pConnectorEvo;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            if (!nvDpyIdIsInDpyIdList(pConnectorEvo->displayId,
                                      modesetUpdateState->connectorIds)) {
                continue;
            }

            if (pConnectorEvo->pDpLibConnector != NULL) {
                nvDPPostSetMode(pConnectorEvo->pDpLibConnector,
                                modesetUpdateState);
            }  else if (nvConnectorIsDPSerializer(pConnectorEvo)) {
                nvDPSerializerPostSetMode(pDispEvo, pConnectorEvo);
            } else {
                if (!nvIsConnectorActiveEvo(pConnectorEvo)) {
                    SetLinkHandOffOnDpDdcPartners(pConnectorEvo, pDispEvo, FALSE);
                }
            }
        }
    }

    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        nvEvoPostModesetUnregisterFlipOccurredEvent(pDispEvo, head,
            modesetUpdateState);
    }

    nvkms_memset(modesetUpdateState, 0, sizeof(*modesetUpdateState));
}

/*
 * The hardware does not allow changing the tile/phywin ownership in a single
 * update. NVKMS first needs to detach the tile/phywin from its existing owner
 * and then attach it to another owner in a separate update.
 *
 * For the given apiHead, if tiles currently attached to its heads are moved to a
 * different head, mark that apiHead as incompatible to ensure it gets shut
 * down before modeset. This will ensure that the tiles attached to its heads
 * are detached before moving them to different heads during modeset.
 */
static NvBool IsCurrentMultiTileConfigOneApiHeadIncompatible(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head;

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVHwHeadMultiTileConfigRec *pMultiTileConfig =
            &pDispEvo->headState[head].multiTileConfig;
        const NvU32 tilesMask = pMultiTileConfig->tilesMask;
        NvU32 phywinsMask = 0x0;

        for (NvU32 layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            phywinsMask |= pMultiTileConfig->phywinsMask[layer];
        }

        for (NvU32 tmpHead = 0; tmpHead < pDevEvo->numHeads; tmpHead++) {
            const NVHwHeadMultiTileConfigRec *pTmpMultiTileConfig =
                &pProposedDisp->head[tmpHead].multiTileConfig;

            if (tmpHead == head) {
                continue;
            }

            if ((pTmpMultiTileConfig->tilesMask & tilesMask) != 0x0) {
                return TRUE;
            }

            for (NvU32 layer = 0;
                    layer < pDevEvo->head[tmpHead].numLayers; layer++) {
                if ((pTmpMultiTileConfig->phywinsMask[layer] &
                        phywinsMask) != 0x0) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/*!
 * Determine if display devices driven by head are incompatible with newly
 * activated display devices.
 */
static NvBool
IsProposedModeSetStateOneApiHeadIncompatible(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead,
    const
    NVProposedModeSetHwStateOneDisp *pProposedDisp)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVProposedModeSetStateOneApiHead *pProposedApiHead =
        &pProposedDisp->apiHead[apiHead];
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    const NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);
    const NvBool bCurrent2Heads1Or =
        (nvPopCount32(pApiHeadState->hwHeadsMask) > 1);
    const NvBool bProposed2Heads1Or =
        (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1);
    nvAssert(pDpyEvo != NULL);

    /*
     * DisplayPort devices require an EVO update when detaching the head
     * from the SOR, because DPlib performs link-training to powerdown
     * the link.  So, always consider DisplayPort as incompatible.
     *
     * Consider this api-head incompatible if there is change in the api-head
     * to hardware-head(s) mapping.
     *
     * Mark api-head incompatible if its current or proposed modeset state is
     * using 2Heads1OR configuration:
     * Even if there is no change in the hardware heads or modetimings, it is
     * not possible to do modeset on the active 2Heads1OR api-head without
     * shutting it down first. The modeset code path is ready to handle the
     * glitchless 2Heads1OR modeset, for example NV0073_CTRL_CMD_DFP_ASSIGN_SOR
     * does not handles the assignment of secondary SORs if display is already
     * active and returns incorrect information which leads to segfault in
     * NVKMS.
     */

    if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) ||
            ((pProposedApiHead->hwHeadsMask != 0x0) &&
             (pProposedApiHead->hwHeadsMask != pApiHeadState->hwHeadsMask)) ||
            bCurrent2Heads1Or || bProposed2Heads1Or) {
        return TRUE;
    }

    for (NvU32 tmpApiHead = 0; tmpApiHead < pDevEvo->numApiHeads; tmpApiHead++) {
        const NVProposedModeSetStateOneApiHead *pTmpProposedApiHead =
            &pProposedDisp->apiHead[tmpApiHead];
        const NVDpyEvoRec *pDpyEvoTmp =
            nvGetOneArbitraryDpyEvo(pTmpProposedApiHead->dpyIdList, pDispEvo);
        NVDpyIdList dpyIdList;

        if (!pTmpProposedApiHead->changed) {
            continue;
        }

        /*
         * DDC partners incompatible with each other, only one should be active
         * at a time.
         */
        if ((pDpyEvoTmp != NULL) &&
            nvDpyIdIsInDpyIdList(pDpyEvoTmp->pConnectorEvo->displayId,
                                 pDpyEvo->pConnectorEvo->ddcPartnerDpyIdsList)) {
            return TRUE;
        }

        /*
         * For the remaining tests, we compare apiHead against all other heads
         * in the tmpApiHead loop.
         */
        if (tmpApiHead == apiHead) {
            continue;
        }

        /*
         * Consider this api-head incompatible if its current hardware heads
         * are proposed to map onto the different api-head.
         */
        if ((pTmpProposedApiHead->hwHeadsMask &
             pApiHeadState->hwHeadsMask) != 0x0) {
            return TRUE;
        }

        /*
         * Consider this api-head incompatible if its current
         * dpy(s) are proposed to attach to a different api-head.
         */
        dpyIdList = nvIntersectDpyIdListAndDpyIdList(pTmpProposedApiHead->dpyIdList,
                                                     pApiHeadState->activeDpys);
        if (!nvDpyIdListIsEmpty(dpyIdList)) {
            return TRUE;
        }
    }

    if (IsCurrentMultiTileConfigOneApiHeadIncompatible(pDispEvo,
                                                       apiHead,
                                                       pProposedDisp)) {
        return TRUE;
    }

    return FALSE;
}

static void DisableActiveCoreRGSyncObjects(NVDispEvoRec *pDispEvo,
                                           const NvU32 apiHead,
                                           NVEvoUpdateState *pUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];

    for (int i = 0; i < pApiHeadState->numVblankSyncObjectsCreated; i++) {
        if (pApiHeadState->vblankSyncObjects[i].enabled) {
            NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
            nvAssert(head != NV_INVALID_HEAD);

            /* hCtxDma of 0 indicates Disable. */
            pDevEvo->hal->ConfigureVblankSyncObject(
                    pDevEvo,
                    0, /* rasterLine */
                    head,
                    i,
                    NULL, /* pSurfaceDesc */
                    pUpdateState);
            pApiHeadState->vblankSyncObjects[i].enabled = FALSE;
        }
    }
}

static void
ApplyProposedModeSetHwStateOneHeadShutDown(
    NVDispEvoPtr pDispEvo,
    NvU32 head,
    const
    NVProposedModeSetHwStateOneDisp
    *pProposedDisp,
    NVModeSetWorkArea *pWorkArea)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NVDispHeadStateEvoPtr pHeadState = &pDispEvo->headState[head];

    nvHdmiDpEnableDisableAudio(pDispEvo, head, FALSE /* enable */);

    nvEvoDetachConnector(pHeadState->pConnectorEvo, head,
                         &pWorkArea->modesetUpdateState);

    nvEvoDisableHwYUV420Packer(pDispEvo, head,
        &pWorkArea->modesetUpdateState.updateState);

    pHeadState->pConnectorEvo = NULL;

    pHeadState->bypassComposition = FALSE;
    pHeadState->mergeHeadSection = 0;
    nvkms_memset(&pHeadState->timings, 0, sizeof(pHeadState->timings));
    pHeadState->activeRmId = 0;

    nvkms_memset(&pHeadState->audio, 0, sizeof(pHeadState->audio));

    nvkms_memset(&pHeadState->modeValidationParams, 0,
                 sizeof(pHeadState->modeValidationParams));

    nvkms_memset(&pDevEvo->gpus[sd].headState[head], 0,
                 sizeof(pDevEvo->gpus[sd].headState[head]));

    pDevEvo->gpus[sd].headState[head].cursor.cursorCompParams =
        nvDefaultCursorCompositionParams(pDevEvo);

    nvkms_memset(&pHeadState->multiTileConfig, 0,
                 sizeof(pHeadState->multiTileConfig));
    if (pDevEvo->hal->SetMultiTileConfig != NULL) {
        pDevEvo->hal->SetMultiTileConfig(pDispEvo,
                                         head,
                                         NULL /* pTimings */,
                                         NULL /* pDscInfo */,
                                         &pHeadState->multiTileConfig,
                                         &pWorkArea->modesetUpdateState);
    }

    for (NvU32 layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        nvEvoSetFlipOccurredEvent(pDispEvo,
                                  head,
                                  layer,
                                  NULL,
                                  &pWorkArea->modesetUpdateState);
    }
}

/*!
 * Send methods to shut down a api-head
 *
 * \param[in,out]  pDispEvo       The disp of the head.
 * \param[in]      apiHead        The api head to consider.
 * \param[in]      pProposedDisp  The requested configuration of the display
 * \param[in/out]  modesetUpdateState Structure tracking channels which need to
 *                                    be updated/kicked off
 */
static void
ApplyProposedModeSetStateOneApiHeadShutDown(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead,
    const
    NVProposedModeSetHwStateOneDisp
    *pProposedDisp,
    NVModeSetWorkArea *pWorkArea)
{
    NVDpyEvoPtr pDpyEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    NvU32 head;

    /*
     * If nothing changed about this head's configuration, then we
     * should not shut it down.
     */
    if (!pProposedDisp->apiHead[apiHead].changed) {
        return;
    }

    /*
     * Otherwise, go through the shutdown process for any head that
     * changed.  If NVProposedModeSetStateOneApiHead::dpyIdList is
     * empty, then we'll leave it shut down.  If it is non-empty, then
     * ApplyProposedModeSetHwStateOneHead{Pre,Post}Update() will
     * update the head with its new configuration.
     */

    if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
        return;
    }

    nvAssert(pWorkArea->sd[pDispEvo->displayOwner].
                        apiHead[apiHead].oldActiveRmId == 0);

    if (nvPopCount32(pApiHeadState->hwHeadsMask) > 1) {
        nvEvoDisableMergeMode(pDispEvo, pApiHeadState->hwHeadsMask,
                              &pWorkArea->modesetUpdateState.updateState);
    }

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];

        /* Track old activeRmId and free it after end modeset */
        if (pWorkArea->sd[pDispEvo->displayOwner].apiHead[apiHead].oldActiveRmId == 0) {
            pWorkArea->sd[pDispEvo->displayOwner].
                apiHead[apiHead].oldActiveRmId = pHeadState->activeRmId;
        } else {
            nvAssert(pWorkArea->sd[pDispEvo->displayOwner].
                        apiHead[apiHead].oldActiveRmId == pHeadState->activeRmId);
        }

        ApplyProposedModeSetHwStateOneHeadShutDown(pDispEvo, head,
                                                   pProposedDisp, pWorkArea);
    }

    pDpyEvo = nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);

    nvCancelSDRTransitionTimer(pDpyEvo);

    /*
     * Identify and disable any active core RG sync objects.
     *
     * Note: the disable occurs at the hardware level; this intentionally does
     * not clear the software state tracking the existence of these sync
     * objects, which will be re-enabled at the hardware level in
     * ApplyProposedModeSetStateOneApiHeadPreUpdate(), if the given head will be
     * active after the modeset.
     */
    DisableActiveCoreRGSyncObjects(pDispEvo, apiHead,
                                   &pWorkArea->modesetUpdateState.updateState);

    nvDisable3DVisionAegis(pDpyEvo);

    /* Cancel any pending LUT updates. */
    nvCancelLutUpdateEvo(pDispEvo, apiHead);

    /* Clear software shadow state. */
    pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList =
        nvAddDpyIdListToDpyIdList(
            pApiHeadState->activeDpys,
            pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList);
    pApiHeadState->activeDpys = nvEmptyDpyIdList();
    nvkms_memset(&pApiHeadState->timings, 0, sizeof(pApiHeadState->timings));
    nvkms_memset(&pApiHeadState->stereo, 0, sizeof(pApiHeadState->stereo));

    pDpyEvo->apiHead = NV_INVALID_HEAD;
    nvAssignHwHeadsMaskApiHeadState(pApiHeadState, 0x0);
}

static void
ApplyProposedModeSetStateOneDispFlip(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwState *pProposed,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        const NVProposedModeSetStateOneApiHead *pProposedApiHead =
            &pProposedDisp->apiHead[apiHead];
        const NVDpyEvoRec *pDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
        const NvU32 sd = pDispEvo->displayOwner;
        NvU32 head;

        /*
         * If nothing changed about this api-head's configuration or this
         * api-head is disabled, then there is nothing to do.
         */
        if (!pProposedApiHead->changed ||
                (pProposedApiHead->hwHeadsMask == 0)) {
            continue;
        }

        nvAssert(pDpyEvo != NULL);

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
            nvSetUsageBoundsEvo(pDevEvo, sd, head,
                                &pProposed->sd[sd].head[head].flip.usage,
                                pUpdateState);

            nvFlipEvoOneHead(pDevEvo, sd, head,
                             &pDpyEvo->parsedEdid.info.hdr_static_metadata_info,
                             &pProposed->sd[sd].head[head].flip,
                             FALSE /* allowFlipLock */,
                             pUpdateState);

            if (pDevEvo->isSOCDisplay) {
                pDevEvo->gpus[0].headState[head].preallocatedUsage =
                    pProposed->sd[sd].head[head].flip.usage;
            }
        }

        pDispEvo->apiHeadState[apiHead].tf =
            pProposedApiHead->tf;

        pDispEvo->apiHeadState[apiHead].hdrInfoFrameOverride =
            pProposedApiHead->hdrInfoFrameOverride;

        pDispEvo->apiHeadState[apiHead].hdrStaticMetadataLayerMask =
            pProposedApiHead->hdrStaticMetadataLayerMask;

        pDispEvo->apiHeadState[apiHead].viewPortPointIn =
            pProposedApiHead->viewPortPointIn;
    }
}

static void ReEnableActiveCoreRGSyncObjects(NVDispEvoRec *pDispEvo,
                                            const NvU32 apiHead,
                                            NVEvoUpdateState *pUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];

    nvAssert(head != NV_INVALID_HEAD);

    for (int i = 0; i < pApiHeadState->numVblankSyncObjectsCreated; i++) {
        if (pApiHeadState->vblankSyncObjects[i].inUse) {
            pDevEvo->hal->ConfigureVblankSyncObject(
                    pDevEvo,
                    pDispEvo->headState[head].timings.rasterBlankStart.y,
                    head,
                    i,
                    &pApiHeadState->vblankSyncObjects[i].evoSyncpt.surfaceDesc,
                    pUpdateState);

            pApiHeadState->vblankSyncObjects[i].enabled = TRUE;
        }
    }
}

static void
ApplyProposedModeSetHwStateOneHeadPreUpdate(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetStateOneApiHead *pProposedApiHead,
    const NvU32 head,
    const NvU32 isPrimaryHead,
    const NVProposedModeSetHwStateOneHead *pProposedHead,
    NvBool bypassComposition,
    NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVDispHeadStateEvoPtr pHeadState = &pDispEvo->headState[head];
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;

    pHeadState->modeValidationParams = pProposedApiHead->modeValidationParams;
    pHeadState->bypassComposition = bypassComposition;
    pHeadState->activeRmId = pProposedApiHead->activeRmId;
    pHeadState->pConnectorEvo = pProposedHead->pConnectorEvo;
    pHeadState->mergeHeadSection = pProposedHead->mergeHeadSection;
    pHeadState->timings = pProposedHead->timings;
    pHeadState->dscInfo = pProposedApiHead->dscInfo;
    pHeadState->hdmiFrlConfig = pProposedHead->hdmiFrlConfig;
    pHeadState->pixelDepth =
        nvEvoDpyColorToPixelDepth(&pProposedApiHead->attributes.color);
    pHeadState->audio = pProposedHead->audio;

    nvEvoSetTimings(pDispEvo, head, updateState);

    pHeadState->multiTileConfig = pProposedHead->multiTileConfig;
    if (pDispEvo->pDevEvo->hal->SetMultiTileConfig != NULL) {
        pDispEvo->pDevEvo->hal->SetMultiTileConfig(pDispEvo,
                                                   head,
                                                   &pProposedHead->timings,
                                                   &pHeadState->dscInfo,
                                                   &pHeadState->multiTileConfig,
                                                   pModesetUpdateState);
    }
    nvEvoSetDpVscSdp(pDispEvo, head, &pProposedApiHead->infoFrame,
                     &pProposedApiHead->attributes.color, updateState);

    nvSetDitheringEvo(pDispEvo,
                      head,
                      &pProposedApiHead->attributes.dithering,
                      updateState);

    nvEvoHeadSetControlOR(pDispEvo,
                          head,
                          &pProposedApiHead->attributes.color,
                          updateState);

    /* Update hardware's current colorSpace and colorRange */
    nvUpdateCurrentHardwareColorSpaceAndRangeEvo(pDispEvo,
                                                 head,
                                                 &pProposedApiHead->attributes.color,
                                                 updateState);

    nvEvoAttachConnector(pProposedHead->pConnectorEvo,
                         head,
                         isPrimaryHead,
                         pProposedHead->pDpLibModesetState,
                         pModesetUpdateState);

    nvSetViewPortsEvo(pDispEvo, head, updateState);

    nvSetImageSharpeningEvo(
        pDispEvo,
        head,
        pProposedApiHead->attributes.imageSharpening.value,
        updateState);


    nvSetDVCEvo(pDispEvo, head,
                pProposedApiHead->attributes.dvc,
                updateState);


    nvHdmiFrlSetConfig(pDispEvo, head);
}

/*!
 * Update the api heads to be modified on this disp.
 *
 * This should update the ASSY state of the head, but not trigger an
 * UPDATE method.
 *
 * \param[in,out]  pDispEvo       The disp of the head.
 * \param[in]      apiHead        The api head to consider.
 * \param[in]      pProposedHead  The requested configuration of the head.
 * \param[in,out]  updateState    Indicates which channels require UPDATEs
 * \param[in]      bypassComposition
 *                                On Turing and newer, enable display
 *                                composition pipeline bypass mode.
 */
static void
ApplyProposedModeSetStateOneApiHeadPreUpdate(
    NVDispEvoPtr pDispEvo,
    const NvU32 apiHead,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVModeSetWorkArea *pWorkArea,
    NvBool bypassComposition)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoModesetUpdateState *pModesetUpdateState = &pWorkArea->modesetUpdateState;
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];
    const NVProposedModeSetStateOneApiHead *pProposedApiHead =
        &pProposedDisp->apiHead[apiHead];
    const NvU32 proposedPrimaryHead =
        nvGetPrimaryHwHeadFromMask(pProposedApiHead->hwHeadsMask);
    NVDpyEvoPtr pDpyEvo =
        nvGetOneArbitraryDpyEvo(pProposedApiHead->dpyIdList, pDispEvo);
    NvU32 head;

    /*
     * If nothing changed about this head's configuration, then there
     * is nothing to do.
     */
    if (!pProposedApiHead->changed) {
        return;
    }

    /* Check for disabled heads. */

    if (nvDpyIdListIsEmpty(pProposedApiHead->dpyIdList)) {
        /*
         * ApplyProposedModeSetStateOneApiHeadShutDown() should have
         * already been called for this head.
         */
        nvAssert(!nvApiHeadIsActive(pDispEvo, apiHead));
        return;
    }

    if (pDpyEvo == NULL) {
        nvAssert(!"Invalid pDpyEvo");
        return;
    }

    /*
     * pDevEvo->apiHead[apiHead].numLayers is the number of layers which are
     * visible to nvkms clients, and that number should be less than or equal
     * to pDevEvo->head[proposedPrimaryHead].numLayers.
     *
     * If (pDevEvo->head[proposedPrimaryHead].numLayers >
     * pDevEvo->apiHead[apiHead].numLayers) then the extra per hardware-head
     * layers remain unused and there is no need to register the completion
     * notifier callback for those extra per hardware-head layers.
     */

    nvAssert(pDevEvo->head[proposedPrimaryHead].numLayers >=
                 pDevEvo->apiHead[apiHead].numLayers);

    for (NvU32 layer = 0; layer <
            pDevEvo->apiHead[apiHead].numLayers; layer++) {
        nvEvoSetFlipOccurredEvent(pDispEvo,
                                  proposedPrimaryHead,
                                  layer,
                                  pApiHeadState->flipOccurredEvent[layer].
                                    ref_ptr,
                                  &pWorkArea->modesetUpdateState);
    }
    pApiHeadState->hwHeadsMask = pProposedApiHead->hwHeadsMask;
    pDpyEvo->apiHead = apiHead;

    AssignSor(pDispEvo, pProposedApiHead, pWorkArea);

    nvDpyUpdateHdmiPreModesetEvo(pDpyEvo);

    /*
     * Cache the list of active pDpys for this head, as well as the
     * mode timings.
     */
    pApiHeadState->activeDpys = pProposedApiHead->dpyIdList;
    pApiHeadState->timings = pProposedApiHead->timings;
    pApiHeadState->stereo = pProposedApiHead->stereo;
    pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList =
        nvAddDpyIdListToDpyIdList(
            pApiHeadState->activeDpys,
            pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList);

    pApiHeadState->infoFrame = pProposedApiHead->infoFrame;

    nvSendHwModeTimingsToAegisEvo(pDispEvo, apiHead);

    /* Set LUT settings
     *
     * Don't set the LUT notifier because we're already waiting on a core
     * notifier for the update.
     */
    nvEvoSetLut(pDispEvo, apiHead, FALSE /* kickoff */, &pProposedApiHead->lut);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pProposedApiHead->hwHeadsMask, head) {
        const NvBool isPrimaryHead = (head == proposedPrimaryHead);

        nvAssert(pDpyEvo->pConnectorEvo ==
                    pProposedDisp->head[head].pConnectorEvo);

        ApplyProposedModeSetHwStateOneHeadPreUpdate(pDispEvo, pProposedApiHead,
            head, isPrimaryHead, &pProposedDisp->head[head],
            bypassComposition, pModesetUpdateState);
    }

    /*
     * Re-enable any active sync objects, configuring them in accordance with
     * the new timings.
     */
    ReEnableActiveCoreRGSyncObjects(pDispEvo, apiHead, updateState);

    pApiHeadState->attributes = pProposedApiHead->attributes;
    pApiHeadState->tf = pProposedApiHead->tf;
    pApiHeadState->hdrInfoFrameOverride =
        pProposedApiHead->hdrInfoFrameOverride;
    pApiHeadState->hdrStaticMetadataLayerMask =
        pProposedApiHead->hdrStaticMetadataLayerMask;
    pApiHeadState->hs10bpcHint = pProposedApiHead->hs10bpcHint;

    if (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1) {
        nvEvoEnableMergeModePreModeset(pDispEvo,
                                       pProposedApiHead->hwHeadsMask,
                                       &pModesetUpdateState->updateState);
    }
}


/*!
 * Update the heads to be modified on this disp.
 *
 * PreUpdate() will have already been called on this head, and an
 * UPDATE method sent.
 *
 * \param[in,out]  pDispEvo          The disp of the head.
 * \param[in]      apihead           The api head to consider.
 * \param[in]      pProposedApiHead  The requested configuration of the api head.
 */
static void
ApplyProposedModeSetStateOneApiHeadPostModesetUpdate(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead,
    const NVProposedModeSetStateOneApiHead *pProposedApiHead,
    NVEvoUpdateState *pUpdateState)
{
    NvU32 head;
    NVDpyEvoRec *pDpyEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];

    /*
     * If nothing changed about this head's configuration, then there
     * is nothing to do.
     */
    if (!pProposedApiHead->changed) {
        return;
    }

    if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
        return;
    }

    if (nvPopCount32(pProposedApiHead->hwHeadsMask) > 1) {
        nvEvoEnableMergeModePostModeset(pDispEvo,
                                        pProposedApiHead->hwHeadsMask,
                                        pUpdateState);
    }

    pDpyEvo = nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);
    nvAssert(pDpyEvo != NULL);

    nvUpdateInfoFrames(pDpyEvo);

    /* Perform 3D vision authentication */
    nv3DVisionAuthenticationEvo(pDispEvo, apiHead);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvHdmiDpEnableDisableAudio(pDispEvo, head, TRUE /* enable */);
    }
}

/*
 * Shut down all api-heads that are incompatible with pProposedDisp. This
 * requires doing an update immediately.
 */
static void
KickoffProposedModeSetStateIncompatibleApiHeadsShutDown(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVModeSetWorkArea *pWorkArea)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvBool foundIncompatibleHead = FALSE;
    NvU32 clearHdmiFrlActiveRmId[NVKMS_MAX_HEADS_PER_DISP] = { };
    NVDpyIdList proposedActiveConnectorsList = nvEmptyDpyIdList();
    NVDpyIdList currActiveConnectorsList = nvEmptyDpyIdList();
    NVDpyIdList proposedInactiveConnectorList, unionOfActiveConnectorList;

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        const NVDpyEvoRec *pActiveDpyEvo =
            nvGetOneArbitraryDpyEvo(pDispEvo->apiHeadState[apiHead].activeDpys,
                pDispEvo);
        const NVDpyEvoRec *pProposedDpyEvo =
            nvGetOneArbitraryDpyEvo(pProposedDisp->apiHead[apiHead].dpyIdList,
                pDispEvo);
        NVDpyId activeConnectorId = (pActiveDpyEvo != NULL) ?
                pActiveDpyEvo->pConnectorEvo->displayId :
                nvInvalidDpyId();
        NVDpyId proposedConnectorId = (pProposedDpyEvo != NULL) ?
                pProposedDpyEvo->pConnectorEvo->displayId :
                nvInvalidDpyId();

        currActiveConnectorsList =
            nvAddDpyIdToDpyIdList(activeConnectorId,
                                  currActiveConnectorsList);

        proposedActiveConnectorsList =
            nvAddDpyIdToDpyIdList(proposedConnectorId,
                                  proposedActiveConnectorsList);
    }

    proposedInactiveConnectorList =
        nvDpyIdListMinusDpyIdList(currActiveConnectorsList,
                                  proposedActiveConnectorsList);
    unionOfActiveConnectorList =
        nvAddDpyIdListToDpyIdList(proposedActiveConnectorsList,
                                  currActiveConnectorsList);

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NvBool thisHeadIncompatible = FALSE;

        if (!pProposedDisp->apiHead[apiHead].changed ||
                !nvApiHeadIsActive(pDispEvo, apiHead)) {
            continue;
        }

        const NVDpyEvoRec *pCurrDpyEvo =
            nvGetOneArbitraryDpyEvo(pDispEvo->apiHeadState[apiHead].activeDpys,
                pDispEvo);
        const NVConnectorEvoRec *pCurrConnectorEvo =
            pCurrDpyEvo->pConnectorEvo;


        /*
         * If the number of current active connectors + proposed active
         * connectors is greater than number of heads then modeset is under
         * risk to run out of SORs. This is because the number of connectors >
         * the number of SORs >= the number of heads.
         *
         * The sor assignment failure during modeset causes display engine
         * and/or kernel panics.
         *
         * In this situation, all the connectors which are not going active
         * after modeset, mark them incompatible and shut down them before
         * triggering modeset on all the active connectors.
         *
         * [2Heads1OR] XXX This incompatibility check will not work because in
         * the 2Heads1OR configuration two heads gets attached to a single
         * connector.
         */
        if (nvCountDpyIdsInDpyIdList(unionOfActiveConnectorList) >
            pDispEvo->pDevEvo->numHeads &&
            nvDpyIdIsInDpyIdList(pCurrConnectorEvo->displayId,
                                 proposedInactiveConnectorList)) {
            thisHeadIncompatible = TRUE;
        }

        /* if the *new* timings are FRL, then we need to shut down the head. */
        if (pProposedDisp->apiHead[apiHead].timings.protocol ==
                NVKMS_PROTOCOL_SOR_HDMI_FRL) {
            thisHeadIncompatible = TRUE;
        }

        /* if the *old* timings are FRL, then we need to shut down the head and
         * clear the FRL config. */
        if (pDispEvo->apiHeadState[apiHead].timings.protocol ==
                NVKMS_PROTOCOL_SOR_HDMI_FRL) {
            NvU32 head;
            thisHeadIncompatible = TRUE;
            /* cache the activeRmId since it will be cleared below, but
             * we don't want to actually call into the HDMI library until
             * afterwards. */
            FOR_EACH_EVO_HW_HEAD_IN_MASK(
                pDispEvo->apiHeadState[apiHead].hwHeadsMask,
                head) {
                if (clearHdmiFrlActiveRmId[apiHead] == 0) {
                    clearHdmiFrlActiveRmId[apiHead] =
                        pDispEvo->headState[head].activeRmId;
                } else {
                    nvAssert(clearHdmiFrlActiveRmId[apiHead] ==
                                pDispEvo->headState[head].activeRmId);
                }
            }
        }

        if (IsProposedModeSetStateOneApiHeadIncompatible(pDispEvo,
                                                         apiHead,
                                                         pProposedDisp)) {
            thisHeadIncompatible = TRUE;
        }

        if (!thisHeadIncompatible) {
            continue;
        }

        ApplyProposedModeSetStateOneApiHeadShutDown(
            pDispEvo,
            apiHead,
            pProposedDisp,
            pWorkArea);

        foundIncompatibleHead = TRUE;
    }

    /* Submit UPDATE method and kick off, to shut down incompatible heads. */
    if (foundIncompatibleHead) {
        KickoffModesetUpdateState(pDispEvo, &pWorkArea->modesetUpdateState);
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numHeads; apiHead++) {
            if (clearHdmiFrlActiveRmId[apiHead] == 0) {
                continue;
            }
            nvHdmiFrlClearConfig(pDispEvo, clearHdmiFrlActiveRmId[apiHead]);
        }
    }
}

static void
KickoffProposedModeSetHwState(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwState *pProposed,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const NvBool bypassComposition,
    NVModeSetWorkArea *pWorkArea)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoModesetUpdateState *pModesetUpdateState = &pWorkArea->modesetUpdateState;
    /*
     * If there is a change in window ownership, decouple window channel flips
     * and the core channel update that performs a modeset.
     *
     * This allows window channel flips to be instead interlocked with the core
     * channel update that sets the window usage bounds, avoiding window
     * invalid usage exceptions.
     *
     * See comment about NVDisplay error code 37, in
     * function EvoInitWindowMapping3().
     */
    const NvBool decoupleFlipUpdates =
        pModesetUpdateState->windowMappingChanged;

    /* Send methods to shut down any other unused heads, but don't update yet. */
    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        ApplyProposedModeSetStateOneApiHeadShutDown(
            pDispEvo,
            apiHead,
            pProposedDisp,
            pWorkArea);
    }

    /* Apply pre-UPDATE modifications for any enabled heads. */
    for (NvU32 apiHead = 0;
            apiHead < pDispEvo->pDevEvo->numApiHeads; apiHead++) {
        ApplyProposedModeSetStateOneApiHeadPreUpdate(
            pDispEvo,
            apiHead,
            pProposedDisp,
            pWorkArea,
            bypassComposition);
    }

    if (!decoupleFlipUpdates) {
        /* Merge modeset and flip state updates together */
        ApplyProposedModeSetStateOneDispFlip(
            pDispEvo,
            pProposed,
            pProposedDisp,
            &pModesetUpdateState->updateState);
    }

    /* Submit UPDATE method and kick off. */
    KickoffModesetUpdateState(pDispEvo,
                              pModesetUpdateState);

    if (decoupleFlipUpdates) {
        NVEvoUpdateState flipUpdateState = { };

        ApplyProposedModeSetStateOneDispFlip(
            pDispEvo,
            pProposed,
            pProposedDisp,
            &flipUpdateState);

        pDevEvo->hal->Update(pDevEvo,
                             &flipUpdateState,
                             TRUE /* releaseElv */);
    }

    nvRemoveUnusedHdmiDpAudioDevice(pDispEvo);

    {
        NVEvoUpdateState updateState = { };

        /* Apply post-MODESET-UPDATE modifications for any enabled api-heads. */
        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            ApplyProposedModeSetStateOneApiHeadPostModesetUpdate(
                pDispEvo,
                apiHead,
                &pProposedDisp->apiHead[apiHead],
                &updateState);
        }

        if (!nvIsUpdateStateEmpty(pDevEvo, &updateState)) {
            pDevEvo->hal->Update(pDevEvo, &updateState,
                                 TRUE /* releaseElv */);
        }
    }
}

static void AllocatePostModesetDispBandwidth(NVDispEvoPtr pDispEvo,
                                             NVModeSetWorkArea *pWorkArea)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU64 maxFrameTimeUsec = 0ULL;
    NvU32 head;

    if (!pDevEvo->isSOCDisplay) {
        return;
    }

    if ((pDispEvo->isoBandwidthKBPS == pWorkArea->postModesetIsoBandwidthKBPS) &&
        (pDispEvo->dramFloorKBPS == pWorkArea->postModesetDramFloorKBPS)) {
        return;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NvU64 curFrameTimeUsec = 0ULL;

        if (!nvHeadIsActive(pDispEvo, head)) {
            continue;
        }

        curFrameTimeUsec = nvEvoFrametimeUsFromTimings(&pDispEvo->headState[head].timings);
        maxFrameTimeUsec = NV_MAX(maxFrameTimeUsec, curFrameTimeUsec);
    }

    nvkms_usleep(maxFrameTimeUsec * 2);

    if (!nvAllocateDisplayBandwidth(pDispEvo,
                                    pWorkArea->postModesetIsoBandwidthKBPS,
                                    pWorkArea->postModesetDramFloorKBPS)) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Unexpectedly failed to program post-modeset bandwidth!");
    }
}

/*!
 * Update the disp with the modifications described in pProposedDisp.
 *
 * \param[in]   pDispEvo         The disp to be modified.
 * \param[in]   pProposedDisp    The requested configuration of the disp.
 * \param[in]   pWorkArea        Preallocated scratch memory.
 * \param[in]   updateCoreFirst  If true, avoid interlock with core: kick off
 *                               the core channel and wait for a notifier
 *                               before the rest of the channels for this update.
 * \param[in]      bypassComposition
 *                               On Turing and newer, enable display composition
 *                               pipeline bypass mode.
 *
 * This function is not allowed to fail.
 */
static void
ApplyProposedModeSetHwStateOneDisp(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwState *pProposed,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVModeSetWorkArea *pWorkArea,
    NvBool updateCoreFirst,
    NvBool bypassComposition)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head;
    const NvU32 sd = pDispEvo->displayOwner;

    nvkms_memset(&pWorkArea->modesetUpdateState, 0,
                 sizeof(pWorkArea->modesetUpdateState));

    /* Record the current flip state. */
    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        nvInitFlipEvoHwState(pDevEvo, sd, head,
                             &pWorkArea->sd[sd].head[head].oldState);
    }

    if (updateCoreFirst) {
        /* If this is the first core update, initialize the window -> head
         * mapping.
         *
         * Hal ->InitWindowMapping() sets
         * NVModesetUpdateState::windowMappingChanged true, if there is
         * any change in window ownerships/assignment. This is necessary on
         * GV100+ because of a series of unfortunate requirements.
         *
         * NVDisplay has two requirements that we need to honor:
         *
         *  1. You can't move a window from one head to another while the head
         *  is active.
         *  2. You can't change window assignments in an update that's
         *  interlocked with the corresponding window channel.
         *
         * In addition, GV100 has an additional requirement:
         *
         *  3. You can't change window assignment for a head while it is
         *  active, but it's okay to assign windows in the same update that
         *  activates a head.
         *
         * If there is a change in window assignment, the task of not
         * interlocking core and respective window channels will be handled by
         * NVEvoUpdateState::subdev[]::noCoreInterlockMask.
         * ->InitWindowMapping() will set 'noCoreInterlockMask' and ->Update()
         * will take care not to interlock window channels specified in mask
         * with core channel.
         *
         * The GOP driver and NVKMS assign window channels in the same way. The
         * window channels channels 2n and 2n+1 are guaranteed to get assigned
         * to head n.
         */
        pDevEvo->hal->InitWindowMapping(pDispEvo, &pWorkArea->modesetUpdateState);
    }

    /*
     * Temporarily lock to the max DRAM frequency to prevent mclk switch events
     * from being requested. Display can't tolerate mclk switch events during
     * modeset transitions. This max DRAM floor will be released after the Core
     * notifier signals post-modeset in the AllocatePostModesetDispBandwidth()
     * call below. This only needs to be done for Orin SOC display.
     */
    if (!nvAllocateDisplayBandwidth(pDispEvo,
                                    pDispEvo->isoBandwidthKBPS,
                                    NV_U32_MAX)) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Unexpectedly failed to lock to max DRAM pre-modeset!");
    }

    KickoffProposedModeSetStateIncompatibleApiHeadsShutDown(
        pDispEvo,
        pProposedDisp,
        pWorkArea);

    KickoffProposedModeSetHwState(
        pDispEvo,
        pProposed,
        pProposedDisp,
        bypassComposition,
        pWorkArea);

    /*
     * This function waits for 2 frames to make sure that the final IMP
     * arbitration settings have been programmed by the post-SV3 worker thread
     * in RM. Once these settings have taken effect, it's safe to release the
     * max DRAM floor that was previously requested, and to program the ISO
     * bandwidth that's required for the new mode. This only needs to be done
     * for Orin SOC display.
     */
    AllocatePostModesetDispBandwidth(pDispEvo, pWorkArea);

    /*
     * Record the new flip state, then generate any flip events, and update
     * surface reference counts.
     */
    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        nvInitFlipEvoHwState(
            pDevEvo, sd, head,
            &pWorkArea->sd[sd].head[head].newState);
        nvUpdateSurfacesFlipRefCount(
            pDevEvo,
            head,
            &pWorkArea->sd[sd].head[head].newState,
            NV_TRUE);
        nvUpdateSurfacesFlipRefCount(
            pDevEvo,
            head,
            &pWorkArea->sd[sd].head[head].oldState,
            NV_FALSE);
    }
}


/*!
 * Initialize the pReply structure.
 *
 * Mark all of the heads and disps as successful.  During the process
 * of assigning and validating the proposed configuration, heads with
 * invalid requested configuration will have their reply status field
 * changed to a non-success value.
 *
 * \param[in]   pRequest  The client's requested configuration.  This
 *                        indicates which heads on which disps the
 *                        client requested changes on.
 * \param[out]  pReply    The reply to the client.
 */
static void
InitializeReply(const NVDevEvoRec *pDevEvo,
                const struct NvKmsSetModeRequest *pRequest,
                struct NvKmsSetModeReply *pReply)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(pReply, 0, sizeof(*pReply));

    pReply->status = NVKMS_SET_MODE_STATUS_SUCCESS;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        NvU32 apiHead;

        pReply->disp[dispIndex].status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_SUCCESS;

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {

            pReply->disp[dispIndex].head[apiHead].status =
                NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS;
        }
    }
}


/*!
 * Validate the client-provided NvKmsSetModeRequest.
 *
 * Check basic validity of NvKmsSetModeRequest: e.g., that
 * requestedDispsBitMask and requestedHeadsBitMask do not exceed the
 * disps or heads of the pDevEvo.
 *
 * \param[in]   pDevEvo   The device that is to be modified.
 * \param[in]   pOpenDev  The pOpenDev of the client doing the modeset.
 * \param[in]   pRequest  The client's requested configuration.  This
 *                        indicates which heads on which disps the
 *                        client requested changes on.
 * \param[out]  pReply    The reply to the client.

 * \return      If pRequest is valid, return TRUE.  Otherwise, set the
 *              appropriate status fields in pReply to non-SUCCESS,
 *              and return FALSE.
 */
static NvBool
ValidateRequest(const NVDevEvoRec *pDevEvo,
                const struct NvKmsPerOpenDev *pOpenDev,
                const struct NvKmsSetModeRequest *pRequest,
                struct NvKmsSetModeReply *pReply)
{
    NvU32 dispIndex, apiHead;
    NvBool ret = TRUE;

    const struct NvKmsModesetPermissions *pPermissions =
        nvGetModesetPermissionsFromOpenDev(pOpenDev);

    nvAssert(pOpenDev != NULL);
    nvAssert(pPermissions != NULL);

    /* Check for invalid disps in requestedDispsBitMask. */
    if (nvHasBitAboveMax(pRequest->requestedDispsBitMask,
                         NVKMS_MAX_SUBDEVICES)) {
        pReply->status = NVKMS_SET_MODE_STATUS_INVALID_REQUESTED_DISPS_BITMASK;
        ret = FALSE;
    }

    for (dispIndex = 0; dispIndex < NVKMS_MAX_SUBDEVICES; dispIndex++) {

        if ((pRequest->requestedDispsBitMask & (1 << dispIndex)) == 0) {
            continue;
        }

        if (dispIndex >= pDevEvo->nDispEvo) {
            pReply->status =
                NVKMS_SET_MODE_STATUS_INVALID_REQUESTED_DISPS_BITMASK;
            ret = FALSE;
            continue;
        }

        const struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[dispIndex];

        /* Check for invalid heads in requestedHeadsBitMask. */
        if (nvHasBitAboveMax(pRequestDisp->requestedHeadsBitMask,
                             NVKMS_MAX_HEADS_PER_DISP)) {
            pReply->disp[dispIndex].status =
                NVKMS_SET_MODE_ONE_DISP_STATUS_INVALID_REQUESTED_HEADS_BITMASK;
            ret = FALSE;
        }

        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {

            if ((pRequestDisp->requestedHeadsBitMask & (1 << apiHead)) == 0) {
                continue;
            }

            if (apiHead >= pDevEvo->numApiHeads) {
                pReply->disp[dispIndex].status =
                    NVKMS_SET_MODE_ONE_DISP_STATUS_INVALID_REQUESTED_HEADS_BITMASK;
                ret = FALSE;
                continue;
            }

            const NVDpyIdList permDpyIdList =
                pPermissions->disp[dispIndex].head[apiHead].dpyIdList;

            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];

            /*
             * Does the client have permission to touch this head at
             * all?
             */
            if (pRequest->commit && nvDpyIdListIsEmpty(permDpyIdList)) {
                pReply->disp[dispIndex].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_PERMISSIONS;
                ret = FALSE;
                continue;
            }

            /*
             * pRequestHead->dpyIdList == EMPTY means the head is
             * being shut down: no more to do for validation.
             */
            if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
                continue;
            }

            /*
             * Does the client have permission to drive this dpyIdList
             * with this head?
             */
            if (pRequest->commit &&
                !nvDpyIdListIsASubSetofDpyIdList(pRequestHead->dpyIdList,
                                                 permDpyIdList)) {
                pReply->disp[dispIndex].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_PERMISSIONS;
                ret = FALSE;
                continue;
            }

            /*
             * Are all requested dpys in the list of valid dpys for this disp?
             */
            if (!nvDpyIdListIsASubSetofDpyIdList(
                    pRequestHead->dpyIdList,
                    pDevEvo->pDispEvo[dispIndex]->validDisplays)) {
                pReply->disp[dispIndex].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY;
                ret = FALSE;
                continue;
            }

            if (!nvValidateSetLutCommonParams(pDevEvo, &pRequestHead->flip.lut)) {
                pReply->disp[dispIndex].head[apiHead].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_LUT;
                ret = FALSE;
                continue;
            }
        }
    }

    return ret;
}

static void FillPostSyncptReplyForModeset(
    const NVDevEvoRec *pDevEvo,
    NvU32 head,
    const struct NvKmsFlipCommonParams *pFlipRequest,
    struct NvKmsFlipCommonReplyOneHead *pFlipReply,
    const NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipRequest->layer[layer].syncObjects.specified &&
            pFlipRequest->layer[layer].syncObjects.val.useSyncpt) {
            nvFillPostSyncptReplyOneChannel(
                pDevEvo->head[head].layer[layer],
                pFlipRequest->layer[layer].syncObjects.val.u.syncpts.requestedPostType,
                &pFlipReply->layer[layer].postSyncpt,
                &pFlipState->layer[layer].syncObject);
        }
    }
}

/*!
 * Assign the NvKmsSetModeReply structure.
 *
 * After a modeset was successfully completed, update the pReply with
 * information about the modeset that the client may need.
 *
 * \param[in]   pDevEvo   The device that was modified.
 * \param[in]   pRequest  The client's requested configuration.  This
 *                        indicates which heads on which disps the
 *                        client requested changes on.
 * \param[out]  pReply    The reply to the client.
 */
static void
AssignReplySuccess(NVDevEvoRec *pDevEvo,
                   const struct NvKmsSetModeRequest *pRequest,
                   struct NvKmsSetModeReply *pReply,
                   const NVModeSetWorkArea *pWorkArea)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(pReply, 0, sizeof(*pReply));

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        const struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[dispIndex];

        if ((pRequest->requestedDispsBitMask & (1 << dispIndex)) == 0) {
            continue;
        }

        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[apiHead];
            struct NvKmsSetModeOneHeadReply *pReplyHead =
                &pReply->disp[dispIndex].head[apiHead];

            if ((pRequestDisp->requestedHeadsBitMask & (1 << apiHead)) == 0) {
                continue;
            }

            pReplyHead->status = NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS;

            if (!nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
                const NVDispApiHeadStateEvoRec *pApiHeadState =
                    &pDispEvo->apiHeadState[apiHead];
                const NvU32 primaryHwHead =
                    nvGetPrimaryHwHeadFromMask(pApiHeadState->hwHeadsMask);

                nvAssert((pApiHeadState->hwHeadsMask != 0x0) &&
                            (primaryHwHead != NV_INVALID_HEAD));

                struct NvKmsUsageBounds *pTmpUsageBounds =
                    nvPreallocGet(pDevEvo,
                                  PREALLOC_TYPE_MODE_SET_REPLY_TMP_USAGE_BOUNDS,
                                  sizeof(*pTmpUsageBounds));
                NvU32 head;

                pReplyHead->possibleUsage = pDispEvo->headState[primaryHwHead].
                    timings.viewPort.possibleUsage;
                pReplyHead->guaranteedUsage = pDispEvo->headState[primaryHwHead].
                    timings.viewPort.guaranteedUsage;
                FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
                    const NVDispHeadStateEvoRec *pHeadState =
                        &pDispEvo->headState[head];

                    nvIntersectUsageBounds(&pReplyHead->possibleUsage,
                                           &pHeadState->timings.viewPort.possibleUsage,
                                           pTmpUsageBounds);
                    pReplyHead->possibleUsage = *pTmpUsageBounds;

                    nvIntersectUsageBounds(&pReplyHead->guaranteedUsage,
                                           &pHeadState->timings.viewPort.guaranteedUsage,
                                           pTmpUsageBounds);
                    pReplyHead->guaranteedUsage = *pTmpUsageBounds;
                }

                nvPreallocRelease(pDevEvo,
                                  PREALLOC_TYPE_MODE_SET_REPLY_TMP_USAGE_BOUNDS);

                pReplyHead->usingHeadSurface =
                    (pDispEvo->pHsChannel[apiHead] != NULL);
                pReplyHead->vrrEnabled =
                    (pApiHeadState->timings.vrr.type !=
                     NVKMS_DPY_VRR_TYPE_NONE);
                pReplyHead->hwHead = primaryHwHead;

                FillPostSyncptReplyForModeset(
                    pDevEvo,
                    primaryHwHead,
                    &pRequestHead->flip,
                    &pReplyHead->flipReply,
                    &pWorkArea->sd[dispIndex].head[primaryHwHead].newState);
            } else {
                pReplyHead->hwHead = NV_INVALID_HEAD;
            }
        }
    }
}


/*!
 * Call RM to notify that a modeset is impending, or that the modeset has
 * completed.
 *
 * \param[in]  pDevEvo    The device to modify.
 * \param[in]  pProposed  The proposed resulting hardware state.
 * \param[in]  beginOrEnd Whether this is a begin call or an end call.
 */
static void
BeginEndModeset(NVDevEvoPtr pDevEvo,
                const NVProposedModeSetHwState *pProposed,
                enum NvKmsBeginEndModeset beginOrEnd)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        const NVProposedModeSetHwStateOneDisp *pProposedDisp =
            &pProposed->disp[dispIndex];
        NvU32 apiHead, dpyMask = 0;

        /* Compute dpyMask: take all the dpyIds on this dispIndex. */
        for (apiHead = 0; apiHead <
                ARRAY_LEN(pProposedDisp->apiHead); apiHead++) {
            const NVProposedModeSetStateOneApiHead *pProposedApiHead =
                &pProposedDisp->apiHead[apiHead];
            dpyMask |= nvDpyIdListToNvU32(pProposedApiHead->dpyIdList);
        }

        nvRmBeginEndModeset(pDispEvo, beginOrEnd, dpyMask);
    }
}

/*!
 * Idle all of the satellite channels.
 *
 * XXX NVKMS: use interlocked UPDATEs, instead, so that we don't
 * have to busy-wait on the CPU.
 *
 * XXX NVKMS: we should idle all channels, not just base.
 */
static NvBool IdleAllSatelliteChannels(NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvBool unused;
            if (!nvRMIdleBaseChannel(pDevEvo, head, sd, &unused)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*!
 * Helper function to assign and validate the proposed mode
 */
static NvBool
AssignAndValidateProposedModeSet(NVDevEvoPtr pDevEvo,
                                 struct NvKmsPerOpenDev *pOpenDev,
                                 const struct NvKmsSetModeRequest *pRequest,
                                 struct NvKmsSetModeReply *pReply,
                                 NVProposedModeSetHwState *pProposed,
                                 NVModeSetWorkArea *pWorkArea)
{
    NvBool ret = FALSE;
    struct NvKmsSetModeRequest *pPatchedRequest = nvPreallocGet(pDevEvo,
        PREALLOC_TYPE_HS_PATCHED_MODESET_REQUEST,
        sizeof(*pPatchedRequest));

    /* clear pPatchedRequest */
    nvkms_memset(pPatchedRequest, 0, sizeof(*pPatchedRequest));

    if (!nvHsConfigInitModeset(pDevEvo, pRequest, pReply, pOpenDev,
                               &pWorkArea->hsConfig)) {
        goto done;
    }

tryHsAgain:
    {
        NvU32 patchedApiHeadsMask[NVKMS_MAX_SUBDEVICES] = { };

        if (!nvHsConfigAllocResources(pDevEvo, &pWorkArea->hsConfig)) {
            goto done;
        }

        /* copy pRequest -> pPatchedRequest */
        *pPatchedRequest = *pRequest;

        /* modify pPatchedRequest for a headsurface config */
        if (!nvHsConfigPatchSetModeRequest(pDevEvo, &pWorkArea->hsConfig,
                                           pOpenDev, pPatchedRequest,
                                           patchedApiHeadsMask)) {

            nvHsConfigFreeResources(pDevEvo, &pWorkArea->hsConfig);

            goto done;
        }

        /* assign pProposed from pPatchedRequest */
        if (!AssignProposedModeSetHwState(pDevEvo, pOpenDev, pPatchedRequest,
                                          pReply, pProposed)) {

            nvHsConfigClearPatchedSetModeRequest(pDevEvo,
                                                 pOpenDev,
                                                 pPatchedRequest,
                                                 patchedApiHeadsMask);
            nvHsConfigFreeResources(pDevEvo, &pWorkArea->hsConfig);

            goto done;
        }

        /* validate pProposed */
        if (!ValidateProposedModeSetHwState(pDevEvo, pProposed, pReply,
                                            pWorkArea)) {

            ClearProposedModeSetHwState(pDevEvo, pProposed, FALSE);
            nvHsConfigClearPatchedSetModeRequest(pDevEvo,
                                                 pOpenDev,
                                                 pPatchedRequest,
                                                 patchedApiHeadsMask);
            nvHsConfigFreeResources(pDevEvo, &pWorkArea->hsConfig);

            /*
             * If the pProposed assigned from the patched modeset request
             * failed validation, downgrade the headSurface configuration and
             * try again.
             */
            if (nvHsConfigDowngrade(pDevEvo, pRequest, &pWorkArea->hsConfig)) {
                goto tryHsAgain;
            }
            goto done;
        }

        nvHsConfigClearPatchedSetModeRequest(pDevEvo, pOpenDev, pPatchedRequest,
                                             patchedApiHeadsMask);
        ret = TRUE;
    }

done:
    nvPreallocRelease(pDevEvo,
        PREALLOC_TYPE_HS_PATCHED_MODESET_REQUEST);

    return ret;
}

/*!
 * Perform a modeset across the heads on the disps of the device.
 *
 * See the comments at the top of this source file for a description
 * of the flow performed by this function.
 *
 * \param[in,out]  pDevEvo   The device to be modified.
 * \param[in]      pOpenDev  The pOpenDev of the client doing the modeset.
 * \param[in]      pOpenDevSurfaceHandles
 *                           The table mapping client handles to surfaces.
 * \param[in]      pRequest  The client's requested configuration changes.
 * \param[out]     pReply    The reply to the client.
 * \param[in]      bypassComposition
 *                           On Turing and higher, enable composition pipeline
 *                           bypass mode.
 * \param[in]      doRasterLock
 *                           Rasterlock heads in the post-modeset routine.
 *
 * \return         Return TRUE if the modeset was successful.  Otherwise,
 *                 return FALSE.  If the modeset was not successful,
 *                 the state of the hardware and software should not
 *                 have been changed.
 */
NvBool nvSetDispModeEvo(NVDevEvoPtr pDevEvo,
                        struct NvKmsPerOpenDev *pOpenDev,
                        const struct NvKmsSetModeRequest *pRequest,
                        struct NvKmsSetModeReply *pReply,
                        NvBool bypassComposition,
                        NvBool doRasterLock)
{
    NvBool ret = FALSE;
    NVProposedModeSetHwState *pProposed =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_PROPOSED_MODESET_HW_STATE,
                      sizeof(*pProposed));
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;
    NvBool updateCoreFirst = FALSE;
    NvBool committed = FALSE;

    NVModeSetWorkArea *pWorkArea =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_MODE_SET_WORK_AREA,
                      sizeof(*pWorkArea));

    nvkms_memset(pProposed, 0, sizeof(*pProposed));
    nvkms_memset(pWorkArea, 0, sizeof(*pWorkArea));

    nvAssert(pOpenDev != NULL);

    InitializeReply(pDevEvo, pRequest, pReply);

    if (!ValidateRequest(pDevEvo, pOpenDev, pRequest, pReply)) {
        goto done;
    }

    /* Disallow GC6 in anticipation of touching GPU/displays. */
    if (!nvRmSetGc6Allowed(pDevEvo, FALSE)) {
        goto done;
    }

    if (!AssignAndValidateProposedModeSet(pDevEvo, pOpenDev, pRequest, pReply,
                                          pProposed, pWorkArea)) {
        goto done;
    }

    /* The requested configuration is valid. */

    ret = TRUE;

    if (!pRequest->commit) {
        goto done;
    }

    /* All satellite channels must be idle. */

    if (!IdleAllSatelliteChannels(pDevEvo)) {
        ret = FALSE;
        goto done;
    }

    /* From this point, we should not fail. */

    /*
     * Disable stereo pin during console restore or modeset owner changes.
     */
    if (!InheritPreviousModesetState(pDevEvo, pOpenDev)) {
        NvU32 sd;
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NvU32 head;
            for (head = 0; head < pDevEvo->numHeads; head++) {
                if (!nvHeadIsActive(pDispEvo, head)) {
                    continue;
                }
                nvSetStereoEvo(pDispEvo, head, FALSE);
            }
        }
    }

    /*
     * Tear down any existing headSurface config, restoring the pre-headSurface
     * config.  This must be done before fliplock is potentially re-enabled
     * during nvEvoLockStatePostModeset below.
     */

    nvHsConfigStop(pDevEvo, &pWorkArea->hsConfig);

    nvEvoCancelPostFlipIMPTimer(pDevEvo);

    BeginEndModeset(pDevEvo, pProposed, BEGIN_MODESET);

    DisableVBlankCallbacks(pDevEvo);

    nvEvoLockStatePreModeset(pDevEvo);

    nvEvoRemoveOverlappingFlipLockRequestGroupsForModeset(pDevEvo, pRequest);

    nvDisableVrr(pDevEvo);

    updateCoreFirst = pDevEvo->coreInitMethodsPending;
    pDevEvo->coreInitMethodsPending = FALSE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        ApplyProposedModeSetHwStateOneDisp(pDispEvo,
                                           pProposed,
                                           &pProposed->disp[dispIndex],
                                           pWorkArea,
                                           updateCoreFirst,
                                           bypassComposition);
    }

    nvEnableVrr(pDevEvo);

    /*
     * Cache whether HS in NVKMS is allowed, so we can make consistent
     * decisions for future partial updates from non-modeset owners.
     */
    pDevEvo->allowHeadSurfaceInNvKms = pProposed->allowHeadSurfaceInNvKms;

    nvEvoLockStatePostModeset(pDevEvo, doRasterLock);

    EnableVBlankCallbacks(pDevEvo);

    /*
     * The modeset was successful: if headSurface was used as part of this
     * modeset, record that in the pDevEvo.
     */
    nvHsConfigStart(pDevEvo, &pWorkArea->hsConfig);

    BeginEndModeset(pDevEvo, pProposed, END_MODESET);

    AssignReplySuccess(pDevEvo, pRequest, pReply, pWorkArea);

    pDevEvo->skipConsoleRestore = FALSE;

    pDevEvo->modesetOwnerChanged = FALSE;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        /*
         * In case of successful commit, update current attribute values and
         * free old display IDs.
         */
        NVDpyEvoRec *pDpyEvo;

        FOR_ALL_EVO_DPYS(pDpyEvo,
                         pWorkArea->sd[dispIndex].changedDpyIdList,
                         pDispEvo) {
            nvDpyUpdateCurrentAttributes(pDpyEvo);
        }

        for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (pWorkArea->sd[dispIndex].apiHead[apiHead].oldActiveRmId != 0x0) {
                nvRmFreeDisplayId(
                    pDispEvo,
                    pWorkArea->sd[dispIndex].apiHead[apiHead].oldActiveRmId);
            }
        }
    }

    committed = TRUE;

    /* fall through */
done:
    ClearProposedModeSetHwState(pDevEvo, pProposed, committed);

    /* If all heads are shut down, allow GC6. */
    if (nvAllHeadsInactive(pDevEvo)) {
        nvRmSetGc6Allowed(pDevEvo, TRUE);
    }

    /*
     * nvHsConfigFreeResources() frees any headSurface resources no longer
     * needed.  On a successful modeset, nvHsConfigApply() will move resources
     * from the hsConfig to the pDevEvo, and nvHsConfigFreeResources() will be a
     * noop.
     */
    nvHsConfigFreeResources(pDevEvo, &pWorkArea->hsConfig);

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_MODE_SET_WORK_AREA);
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_PROPOSED_MODESET_HW_STATE);
    return ret;
}

/*!
 * Register a callback to activate when vblank is reached on a given head.
 *
 * \param[in,out]  pDispEvo  The display engine to register the callback on.
 * \param[in]      apiHead   The api head to register the callback on.
 * \param[in]      pCallback The function to call when vblank is reached on the
 *                           provided pDispEvo+head combination.
 * \param[in]      pUserData A pointer to caller-provided custom data.
 * \param[in]      listIndex Which vblankCallbackList[] array to add this callback into.
 *
 * \return         Returns a pointer to a NVVBlankCallbackRec structure if the
 *                 registration was successful.  Otherwise, return NULL.
 */
NVVBlankCallbackPtr
nvApiHeadRegisterVBlankCallback(NVDispEvoPtr pDispEvo,
                                const NvU32 apiHead,
                                NVVBlankCallbackProc pCallback,
                                void *pUserData,
                                NvU8 listIndex)
{
    /*
     * All the hardware heads mapped on the input api head should be
     * rasterlocked, and should trigger vblank callback exactly at same time;
     * therefore it is sufficient to register vblank callback only with the
     * primary hardware head.
     */
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];
    NVDispVblankApiHeadState *pVblankApiHeadState =
        &pDispEvo->vblankApiHeadState[apiHead];
    NVVBlankCallbackPtr pVBlankCallback = NULL;

    pVBlankCallback = nvCalloc(1, sizeof(*pVBlankCallback));
    if (pVBlankCallback == NULL) {
        return NULL;
    }

    pVBlankCallback->pCallback = pCallback;
    pVBlankCallback->pUserData = pUserData;
    pVBlankCallback->apiHead = apiHead;

    /* append to the tail of the list */
    nvListAppend(&pVBlankCallback->vblankCallbackListEntry,
                 &pVblankApiHeadState->vblankCallbackList[listIndex]);

    // If this is the first entry in the list, register the vblank callback
    if ((head != NV_INVALID_HEAD) &&
            (pApiHeadState->rmVBlankCallbackHandle == 0)) {
        pApiHeadState->rmVBlankCallbackHandle =
            nvRmAddVBlankCallback(pDispEvo, head, VBlankCallback,
                                  (void *)(NvUPtr)apiHead);
    }

    return pVBlankCallback;
}

/*!
 * Un-register a vblank callback for a given api head.
 *
 * \param[in,out]  pDispEvo  The display engine to register the callback on.
 * \param[in]      pCallback A pointer to the NVVBlankCallbackRec to un-register.
 *
 */
void nvApiHeadUnregisterVBlankCallback(NVDispEvoPtr pDispEvo,
                                       NVVBlankCallbackPtr pCallback)
{
    const NvU32 apiHead = pCallback->apiHead;
    NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[apiHead];
    const NVDispVblankApiHeadState *pVblankApiHeadState =
        &pDispEvo->vblankApiHeadState[apiHead];
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);

    nvListDel(&pCallback->vblankCallbackListEntry);
    nvFree(pCallback);

    nvAssert((head != NV_INVALID_HEAD) ||
                (pApiHeadState->rmVBlankCallbackHandle == 0));

    // If there are no more callbacks, disable the RM-level callback
    if (VblankCallbackListsAreEmpty(pVblankApiHeadState) &&
            (head != NV_INVALID_HEAD) &&
            (pApiHeadState->rmVBlankCallbackHandle != 0)) {
        nvRmRemoveVBlankCallback(pDispEvo,
                                 pApiHeadState->rmVBlankCallbackHandle);
        pApiHeadState->rmVBlankCallbackHandle = 0;
    }
}

/*!
 * Perform a modeset that disables some or all api heads.
 *
 * \param[in]      pDevEvo     The device to shut down.
 * \param[in]      pOpenDev    The pOpenDev of the client doing the disabling.
 * \param[in]      pTestFunc   The pointer to test function, identifying heads
 *                             targeted to shut down. If NULL then shut down
 *                             all heads.
 * \param[in]      pData       Data passed to the test function.
 * \param[in]      doRasterLock Allow rasterlock to be implicitly enabled
 */
void nvShutDownApiHeads(NVDevEvoPtr pDevEvo,
                        struct NvKmsPerOpenDev *pOpenDev,
                        NVShutDownApiHeadsTestFunc pTestFunc,
                        void *pData,
                        NvBool doRasterLock)
{
    if (pDevEvo->displayHandle != 0) {
        struct NvKmsSetModeParams *params =
            nvPreallocGet(pDevEvo, PREALLOC_TYPE_SHUT_DOWN_HEADS_SET_MODE,
                          sizeof(*params));
        struct NvKmsSetModeRequest *req = NULL;
        NVDispEvoPtr pDispEvo;
        NvU32 dispIndex;
        NvBool dirty = FALSE;

        nvkms_memset(params, 0, sizeof(*params));
        req = &params->request;

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            NvU32 apiHead;

            req->requestedDispsBitMask |= NVBIT(dispIndex);
            for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                /*
                 * XXX pTestFunc isn't honored by nvSetDispModeEvo()'s
                 * InheritPreviousModesetState() logic.
                 */
                if (pTestFunc && !pTestFunc(pDispEvo, apiHead, pData)) {
                    continue;
                }

                dirty = TRUE;
                req->disp[dispIndex].requestedHeadsBitMask |= NVBIT(apiHead);
            }
        }

        if (dirty) {
            req->commit = TRUE;

            /*
             * XXX TODO: The coreInitMethodsPending flag indicates that the
             * init_no_update methods which were pushed by the hardware during
             * core channel allocation are still pending, it means this is
             * first modeset after boot and the boot display/heads are still
             * active. In theory, we could only shut down heads which satisfies
             * pTestFunc() test but this fails because other heads active at
             * boot do not have mode timing information populated during
             * MarkConnectorBootHeadActive(), so nvSetDispMode() tries to
             * program invalid modes on those heads.
             *
             * For now, just shut down all heads if any head satisfies
             * pTestFunc() test.
             */
            if (pDevEvo->coreInitMethodsPending) {
                FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
                    req->disp[dispIndex].requestedHeadsBitMask |=
                        NVBIT(pDevEvo->numApiHeads) - 1;
                }
            }

            nvSetDispModeEvo(pDevEvo, pOpenDev, req,
                             &params->reply, FALSE /* bypassComposition */,
                             doRasterLock);
        }

        nvPreallocRelease(pDevEvo, PREALLOC_TYPE_SHUT_DOWN_HEADS_SET_MODE);
    }

    if (pTestFunc == NULL) {
        nvAssertAllDpysAreInactive(pDevEvo);
    }
}

NVRgLine1CallbackPtr
nvApiHeadAddRgLine1Callback(NVDispEvoRec *pDispEvo,
                            const NvU32 apiHead,
                            NVRgLine1CallbackProc pCallbackProc,
                            void *pUserData)
{
    /*
     * All the hardware heads mapped on the input api head should be
     * rasterlocked, and should trigger RgLine1 callback exactly at same time;
     * therefore it is sufficient to register RgLine1 callback only with the
     * primary hardware head.
     */
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
    if (head == NV_INVALID_HEAD) {
        return NULL;
    }
    return nvRmAddRgLine1Callback(pDispEvo, head, pCallbackProc, pUserData);
}


void nvApiHeadGetScanLine(const NVDispEvoRec *pDispEvo,
                          const NvU32 apiHead,
                          NvU16 *pScanLine,
                          NvBool *pInBlankingPeriod)
{
    /*
     * All the hardware heads mapped on the input api head should be
     * rasterlocked; therefore it is sufficient to get scanline only for the
     * primary hardware head.
     */
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);

    nvAssert(head != NV_INVALID_HEAD);
    pDispEvo->pDevEvo->hal->GetScanLine(pDispEvo, head, pScanLine,
                                        pInBlankingPeriod);
}

