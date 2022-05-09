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
 *   ApplyProposedModeSetHwStateOneHeadShutDown() and
 *   ApplyProposedModeSetHwStateOneHeadPreUpdate()?
 */

#include "nvkms-evo.h"
#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-rm.h"
#include "nvkms-hdmi.h"
#include "nvkms-flip.h"
#include "nvkms-3dvision.h"
#include "nvkms-modepool.h"
#include "nvkms-prealloc.h"
#include "nvkms-private.h"
#include "nvkms-vrr.h"
#include "nvkms-lut.h"

#include "dp/nvdp-connector.h"

#include "nvkms-api.h"

#include "nvkms-modeset.h"
#include "nvkms-modeset-types.h"
#include "nvkms-modeset-workarea.h"
#include "nvkms-attributes.h"

/*!
 * Get an allowFlipLockGroup value that is not yet used by pProposed.
 *
 * Scan through pProposed to find all currently used
 * allowFlipLockGroup values, then pick the first allowFlipLockGroup
 * value that is not used.
 */
static NvU8 GetAvailableAllowFlipLockGroupValue(
    const NVProposedModeSetHwState *pProposed)
{
    NvU32 dispIndex;
    NvU64 allowFlipLockGroupMask = 0;
    NvU8 allowFlipLockGroup;

    /*
     * Find all of the used allowFlipLockGroup values.  Note that
     * allowFlipLockGroup is 1-based (so that 0 can represent no
     * assigned allowFlipLockGroup).  Shift to 0-based, to store in
     * allowFlipLockGroupMask.
    */
    for (dispIndex = 0; dispIndex < ARRAY_LEN(pProposed->disp); dispIndex++) {
        NvU32 head;
        for (head = 0;
             head < ARRAY_LEN(pProposed->disp[dispIndex].head);
             head++) {

            const NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposed->disp[dispIndex].head[head];

            if (pProposedHead->allowFlipLockGroup != 0) {
                nvAssert(pProposedHead->allowFlipLockGroup <= 64);
                allowFlipLockGroupMask |=
                    NVBIT64(pProposedHead->allowFlipLockGroup - 1);
            }
        }
    }

    /* Find the first available allowFlipLockGroup values. */
    allowFlipLockGroupMask = ~allowFlipLockGroupMask;
    if (allowFlipLockGroupMask == 0) {
        /*
         * For this to be zero, the pProposed would need to already
         * have 64 unique allowFlipLockGroup values; 64 unique
         * flipLock groups is highly unlikely.
         */
        nvAssert(!"allowFlipLockGroupMask is too small");
        return 0;
    }

    allowFlipLockGroup = BIT_IDX_64(LOWESTBIT(allowFlipLockGroupMask));

    /* Shift allowFlipLockGroup back to 1-based. */

    return allowFlipLockGroup + 1;
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
static NvBool
GetHwModeTimings(const NVDispEvoRec *pDispEvo,
                 const struct NvKmsSetModeOneHeadRequest *pRequestHead,
                 NVHwModeTimingsEvo *pTimings)
{
    NVDpyEvoPtr pDpyEvo;

    if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
        return TRUE;
    }

    pDpyEvo = nvGetOneArbitraryDpyEvo(pRequestHead->dpyIdList, pDispEvo);

    if (pDpyEvo == NULL) {
        return FALSE;
    }

    return nvValidateModeForModeset(pDpyEvo,
                                    &pRequestHead->modeValidationParams,
                                    &pRequestHead->mode,
                                    &pRequestHead->viewPortSizeIn,
                                    pRequestHead->viewPortOutSpecified ?
                                              &pRequestHead->viewPortOut : NULL,
                                    pTimings);
}

static NvBool ApplySyncptRegistration(
    NVDevEvoRec *pDevEvo,
    NvU32 head,
    const struct NvKmsFlipCommonParams *pParams,
    NVFlipEvoHwState *pFlipState)
{
    NvU32 layer;

    if (!pDevEvo->supportsSyncpts) {
        return TRUE;
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
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pParams->layer[layer].syncObjects.specified &&
            pParams->layer[layer].syncObjects.val.useSyncpt &&
            pParams->layer[layer].syncObjects.val.u.syncpts.pre.type !=
               NVKMS_SYNCPT_TYPE_NONE) {
            nvAssert(!"Failing as pre-syncpt requested in modeset!");
            return FALSE;
        }
    }

    return nvHandleSyncptRegistration(pDevEvo,
                                      head,
                                      pParams,
                                      pFlipState);
}

static NvBool
GetColorSpaceAndColorRange(
    const NVDispEvoPtr pDispEvo,
    const struct NvKmsSetModeOneHeadRequest *pRequestHead,
    NVProposedModeSetHwStateOneHead *pProposedHead)
{
    enum NvKmsDpyAttributeColorRangeValue requestedColorRange;
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

    /*
     * Choose current colorSpace and colorRange based on the current mode
     * timings and the requested color space and range.
     */
    nvChooseCurrentColorSpaceAndRangeEvo(&pProposedHead->timings,
                                         requestedColorSpace,
                                         requestedColorRange,
                                         &pProposedHead->colorSpace,
                                         &pProposedHead->colorRange);
    /*
     * When colorspace is specified in modeset request, it should
     * match the proposed colorspace.
     */
    if (pRequestHead->colorSpaceSpecified) {
        NvBool ret = FALSE;
        switch (pProposedHead->colorSpace) {
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
     * When color range is specified in modeset request, it should
     * match the proposed color range.
     */
    if (pRequestHead->colorRangeSpecified &&
        (pProposedHead->colorRange != pRequestHead->colorRange)) {
        return FALSE;
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
                             NVProposedModeSetHwState *pProposed,
                             NvBool modesetOwnerChanged)
{
    NvU32 sd;
    NVDispEvoPtr pDispEvo;
    NvBool ret = TRUE;
    NvU8 allowFlipLockGroup = 0;

    /* Initialize pProposed with the current hardware configuration. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 head;
        for (head = 0; head < pDevEvo->numHeads; head++) {

            const NVDispHeadStateEvoRec *pHeadState;
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposed->disp[sd].head[head];

            /*
             * Case of invalid hardware head is handled inside
             * nvInitFlipEvoHwState().
             */
            nvInitFlipEvoHwState(pDevEvo, sd, head,
                                 &pProposed->sd[sd].head[head].flip);

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            pHeadState = &pDispEvo->headState[head];

            pProposedHead->timings = pHeadState->timings;
            pProposedHead->dpyIdList = pHeadState->activeDpys;
            pProposedHead->pConnectorEvo = pHeadState->pConnectorEvo;
            pProposedHead->activeRmId = pHeadState->activeRmId;
            pProposedHead->allowFlipLockGroup = pHeadState->allowFlipLockGroup;
            pProposedHead->modeValidationParams =
                pHeadState->modeValidationParams;
            pProposedHead->colorSpace = pHeadState->attributes.colorSpace;
            pProposedHead->colorRange = pHeadState->attributes.colorRange;
            pProposedHead->changed = FALSE;
            pProposedHead->hs10bpcHint = pHeadState->hs10bpcHint;
            pProposedHead->audio = pHeadState->audio;
        }
    }

    /* Update pProposed with the requested changes from the client. */

    if (pOpenDev == pDevEvo->modesetOwner || pOpenDev == pDevEvo->pNvKmsOpenDev) {
        pProposed->allowHeadSurfaceInNvKms = pRequest->allowHeadSurfaceInNvKms;
    } else {
        pProposed->allowHeadSurfaceInNvKms = pDevEvo->allowHeadSurfaceInNvKms;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        const struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[sd];
        NvBool shutDownAllHeads = FALSE;
        NvU32 head;

        if ((pRequest->requestedDispsBitMask & (1 << sd)) == 0) {
            if (modesetOwnerChanged) {
                shutDownAllHeads = TRUE;
            } else {
                continue;
            }
        }

        NVProposedModeSetHwStateOneDisp *pProposedDisp =
            &pProposed->disp[sd];

        pDispEvo = pDevEvo->pDispEvo[sd];

        for (head = 0; head < pDevEvo->numHeads; head++) {

            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[head];
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposedDisp->head[head];
            NVDpyIdList newDpyIdList;
            NvBool clearAndContinue = FALSE;

            if ((pRequestDisp->requestedHeadsBitMask & (1 << head)) == 0 ||
                shutDownAllHeads) {
                if (modesetOwnerChanged) {
                    /*
                     * If the modeset owner is changing, implicitly shut down
                     * other heads not included in requestedHeadsBitMask.
                     */
                    newDpyIdList = nvEmptyDpyIdList();
                } else {
                    /*
                     * Otherwise, just leave the head alone so it keeps its
                     * current configuration.
                     */
                    continue;
                }
            } else {
                newDpyIdList = pRequestHead->dpyIdList;
            }

            /*
             * If newDpyIdList is empty or do not find the valid dpy in
             * newDpyIdList, then the head should be disabled.
             * Clear the pProposedHead, so that no state leaks to the new
             * configuration.
             */
            if (nvDpyIdListIsEmpty(newDpyIdList)) {
                clearAndContinue = TRUE;
            } else {
                const NVDpyEvoRec *pDpyEvo =
                    nvGetOneArbitraryDpyEvo(newDpyIdList, pDispEvo);
                if (pDpyEvo != NULL) {
                    pProposedHead->pConnectorEvo = pDpyEvo->pConnectorEvo;
                    pProposedHead->changed = TRUE;
                } else {
                    clearAndContinue = TRUE;
                }
            }


            if (clearAndContinue) {
                nvkms_memset(pProposedHead, 0, sizeof(*pProposedHead));
                pProposedHead->changed = TRUE;
                continue;
            }

            pProposedHead->dpyIdList = newDpyIdList;
            pProposedHead->activeRmId =
                nvRmAllocDisplayId(pDispEvo, pProposedHead->dpyIdList);
            if (pProposedHead->activeRmId == 0x0) {
                /* XXX Need separate error code? */
                pReply->disp[sd].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY;
                ret = FALSE;
                continue;
            }

            /* Verify that the requested dpys are valid on this head. */
            if ((pProposedHead->pConnectorEvo->validHeadMask & NVBIT(head)) == 0) {
                pReply->disp[sd].head[head].status =
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
            if (!GetHwModeTimings(pDispEvo, pRequestHead, &pProposedHead->timings)) {
                pReply->disp[sd].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                ret = FALSE;
                continue;
            }

            pProposedHead->allowFlipLockGroup = 0;
            pProposedHead->modeValidationParams =
                pRequestHead->modeValidationParams;
            pProposedHead->allowGsync = pRequestHead->allowGsync;
            pProposedHead->allowAdaptiveSync = pRequestHead->allowAdaptiveSync;
            pProposedHead->vrrOverrideMinRefreshRate =
                pRequestHead->vrrOverrideMinRefreshRate;

            if (!GetColorSpaceAndColorRange(pDispEvo, pRequestHead, pProposedHead)) {
                pReply->disp[sd].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
                ret = FALSE;
                continue;
            }

            /*
             * modesetOwnerChanged implies that there was a modeset
             * ownership change since the last modeset. If input/output lut not
             * specified by the new modeset owner then keep them disabled by
             * default.
             */
            if (modesetOwnerChanged) {
                pProposedHead->lut = pRequestHead->lut;

                if (!pRequestHead->lut.input.specified) {
                    pProposedHead->lut.input.specified = TRUE;
                    pProposedHead->lut.input.end = 0;
                }

                if (!pRequestHead->lut.output.specified) {
                    pProposedHead->lut.output.specified = TRUE;
                    pProposedHead->lut.output.enabled = FALSE;
                }
            } else if (pRequestHead->lut.input.specified) {
                pProposedHead->lut = pRequestHead->lut;
            } else {
                pProposedHead->lut.input.specified = FALSE;
            }

            NVFlipEvoHwState *pFlip =
                &pProposed->sd[sd].head[head].flip;

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

            if (pRequest->commit) {
                NvU32 layer;

                for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                    pFlip->dirty.layer[layer] = TRUE;
                }
            }

            if (!ApplySyncptRegistration(
                    pDevEvo,
                    head,
                    &pRequest->disp[sd].head[head].flip,
                    pFlip)) {
                pReply->disp[sd].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                ret = FALSE;
                continue; /* next head */
            }
            if (!nvUpdateFlipEvoHwState(pOpenDev,
                                        pDevEvo,
                                        sd,
                                        head,
                                        &pRequestHead->flip,
                                        pFlip,
                                        FALSE /* allowVrr */,
                                        &pProposedHead->timings.viewPort.possibleUsage)) {
                pReply->disp[sd].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
                ret = FALSE;
                continue; /* next head */
            }

            /*
             * If the modeset is flipping to a depth 30 surface, record this as
             * a hint to headSurface, so it can also allocate its surfaces at
             * depth 30.
             */
            {
                const NVSurfaceEvoRec *pSurfaceEvo =
                    pFlip->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];

                pProposedHead->hs10bpcHint =
                    (pSurfaceEvo != NULL) &&
                    (pSurfaceEvo->format == NvKmsSurfaceMemoryFormatA2B10G10R10 ||
                     pSurfaceEvo->format == NvKmsSurfaceMemoryFormatX2B10G10R10);
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
            if ((pRequestHead->lut.input.specified ||
                 pRequestHead->lut.output.specified) &&
                !pDevEvo->hal->caps.supportsCoreLut) {
                pFlip->layer[NVKMS_MAIN_LAYER].tearing = FALSE;
            }

            /* Construct audio state */
            {
                NVDpyEvoRec *pDpyEvo =
                    nvGetOneArbitraryDpyEvo(pProposedHead->dpyIdList,
                                            pDispEvo);

                nvHdmiDpConstructHeadAudioState(pProposedHead->activeRmId,
                                                pDpyEvo, &pProposedHead->audio);
            }
        } /* head */
    } /* pDispEvo */

    /* Assign allowFlipLockGroup for the heads specified in the request. */

    allowFlipLockGroup = GetAvailableAllowFlipLockGroupValue(pProposed);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        NvU32 head;

        if ((pRequest->requestedDispsBitMask & NVBIT(sd)) == 0) {
            continue;
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {

            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequest->disp[sd].head[head];
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposed->disp[sd].head[head];

            if ((pRequest->disp[sd].requestedHeadsBitMask &
                 NVBIT(head)) == 0) {
                continue;
            }

            if (pRequestHead->allowFlipLock) {
                pProposedHead->allowFlipLockGroup = allowFlipLockGroup;
            }
        }
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
    NvU32 head, downgradePossibleHeadsBitMask = 0;
    NVEvoReallocateBandwidthMode reallocBandwidth = pDevEvo->isSOCDisplay ?
        NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE :
        NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE;

    nvkms_memset(&timingsParams, 0, sizeof(timingsParams));

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];

        const NvBool skipImpCheckThisHead =
            (pProposedHead->modeValidationParams.overrides &
             NVKMS_MODE_VALIDATION_NO_EXTENDED_GPU_CAPABILITIES_CHECK) != 0;

        const NvBool requireBootClocksThisHead =
            (pProposedHead->modeValidationParams.overrides &
             NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS) != 0;

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
        if (pProposedHead->changed) {
            downgradePossibleHeadsBitMask |= NVBIT(head);
        }

        if (pProposedHead->pConnectorEvo == NULL) {
            continue;
        }

        timingsParams[head].pConnectorEvo = pProposedHead->pConnectorEvo;
        timingsParams[head].activeRmId = pProposedHead->activeRmId;
        timingsParams[head].pTimings = &pProposedHead->timings;
        timingsParams[head].pUsage =
            &pProposedHead->timings.viewPort.guaranteedUsage;

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
                                       downgradePossibleHeadsBitMask)) {
        return FALSE;
    }

    if (pDevEvo->isSOCDisplay) {
        NvBool ret;
        struct NvKmsUsageBounds *guaranteedAndProposed =
            nvCalloc(1, sizeof(*guaranteedAndProposed) *
                            NVKMS_MAX_HEADS_PER_DISP);
        if (guaranteedAndProposed == NULL) {
            return FALSE;
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposedDisp->head[head];
            const struct NvKmsUsageBounds *pProposedUsage;

            if (pProposedHead->pConnectorEvo == NULL) {
                continue;
            }

            if (pProposedHead->changed) {
                pProposedUsage = &pProposed->sd[0].head[head].flip.usage;
            } else {
                pProposedUsage =
                    &pDevEvo->gpus[0].headState[head].preallocatedUsage;
            }

            guaranteedAndProposed[head] = nvUnionUsageBounds(
                &pProposedHead->timings.viewPort.guaranteedUsage,
                pProposedUsage);
            timingsParams[head].pUsage = &guaranteedAndProposed[head];
        }

        ret = nvValidateImpOneDisp(pDispEvo, timingsParams,
                                   requireBootClocks,
                                   reallocBandwidth,
                                   &pWorkArea->postModesetIsoBandwidthKBPS,
                                   &pWorkArea->postModesetDramFloorKBPS);

        nvFree(guaranteedAndProposed);

        if (!ret) {
            return FALSE;
        }

        nvScheduleLowerDispBandwidthTimer(pDevEvo);
    }

    return TRUE;
}

static NvBool SkipDisplayPortBandwidthCheck(
    const NVProposedModeSetHwStateOneHead *pProposedHead)
{
    return (pProposedHead->modeValidationParams.overrides &
            NVKMS_MODE_VALIDATION_NO_DISPLAYPORT_BANDWIDTH_CHECK) != 0;
}

static NvBool DowngradeDpPixelDepth(
    NVDispEvoPtr                     pDispEvo,
    NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const NVConnectorEvoRec         *pConnectorEvo)
{
    NvU32 head;

    /*
     * In DP-MST case, many heads can share same connector and dp-bandwidth
     * therefore its necessary to validate and downgrade dp-pixel-depth across
     * all head which are sharing same connector before retry.
     */
    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];
        NVHwModeTimingsEvoPtr pTimings = &pProposedHead->timings;

        if (SkipDisplayPortBandwidthCheck(pProposedHead)) {
            continue;
        }

        if ((pProposedHead->pConnectorEvo == pConnectorEvo) &&
            nvDowngradeHwModeTimingsDpPixelDepthEvo(
                pTimings,
                pProposedHead->colorSpace)) {
                return TRUE;
        }
    }

    return FALSE;
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
    NVProposedModeSetHwStateOneDisp *pProposedDisp)
{
    NvU32 head;
    NvBool bResult = TRUE, bTryAgain = FALSE;


tryAgain:

    bTryAgain = FALSE;
    bResult = TRUE;

    nvDPBeginValidation(pDispEvo);

    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];
        NVHwModeTimingsEvoPtr pTimings = &pProposedHead->timings;

        if ((pProposedHead->pConnectorEvo == NULL) ||
            SkipDisplayPortBandwidthCheck(pProposedHead)) {
            continue;
        }

        bResult = !!nvDPLibValidateTimings(pDispEvo,
                                           head,
                                           pProposedHead->activeRmId,
                                           pProposedHead->dpyIdList,
                                           pProposedHead->colorSpace,
                                           &pProposedHead->modeValidationParams,
                                           pTimings);

        if (!bResult) {
            if (DowngradeDpPixelDepth(pDispEvo,
                                      pProposedDisp,
                                      pProposedHead->pConnectorEvo)) {
                bTryAgain = TRUE;
            }

            /*
             * Cannot downgrade pixelDepth further --
             *   This proposed mode-set is not possible on this DP link, so fail.
             */

            break;
        }
    }

    bResult = !!nvDPEndValidation(pDispEvo) && bResult;

    if (bTryAgain) {
        goto tryAgain;
    }

    if (bResult) {
        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposedDisp->head[head];

            if ((pProposedHead->pConnectorEvo == NULL) ||
                    !nvConnectorUsesDPLib(pProposedHead->pConnectorEvo)) {
                pProposedHead->pDpLibModesetState = NULL;
                continue;
            }

            pProposedHead->pDpLibModesetState =
                nvDPLibCreateModesetState(pDispEvo,
                                          head,
                                          pProposedHead->activeRmId,
                                          pProposedHead->dpyIdList,
                                          pProposedHead->colorSpace,
                                          &pProposedHead->timings);
            if (pProposedHead->pDpLibModesetState == NULL) {
                return FALSE;
            }
        }
    }

    return bResult;
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
    NvU32 head;

    /*
     * Check that the requested configuration of connectors can be
     * driven simultaneously.
     */
    dpyIdList = nvEmptyDpyIdList();
    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVDpyEvoPtr pDpyEvo;
        FOR_ALL_EVO_DPYS(pDpyEvo,
                         pProposedDisp->head[head].dpyIdList, pDispEvo) {
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
    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        /*
         * Intersect the proposed dpys for this head with the
         * accumulated list of dpys for this disp; if the intersection
         * is not empty, a dpy is proposed to be used on multiple
         * heads.
         */
        NVDpyIdList proposedDpyIdList =
            pProposedDisp->head[head].dpyIdList;
        NVDpyIdList intersectedDpyIdList =
            nvIntersectDpyIdListAndDpyIdList(dpyIdList, proposedDpyIdList);

        if (!nvDpyIdListIsEmpty(intersectedDpyIdList)) {
            pReplyDisp->status = NVKMS_SET_MODE_ONE_DISP_STATUS_DUPLICATE_DPYS;
            return FALSE;
        }

        dpyIdList = nvAddDpyIdListToDpyIdList(dpyIdList, proposedDpyIdList);
    }

    /*
     * Check that the requested flipping state is valid.
     */

    for (head = 0; head < pDevEvo->numHeads; head++) {

        if (!pProposedDisp->head[head].changed) {
            continue;
        }

        if (nvDpyIdListIsEmpty(pProposedDisp->head[head].dpyIdList)) {
            continue;
        }

        if (!nvValidateFlipEvoHwState(
                pDevEvo,
                head,
                &pProposedDisp->head[head].timings,
                &pProposed->sd[pDispEvo->displayOwner].head[head].flip)) {
            pReplyDisp->head[head].status =
                NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP;
            return FALSE;
        }
    }

    /*
     * Check ViewPortIn dimensions and ensure valid h/vTaps can be assigned.
     */
    for (head = 0; head < pDevEvo->numHeads; head++) {

        /* XXX assume the gpus have equal capabilities */
        const NVEvoScalerCaps *pScalerCaps =
            &pDevEvo->gpus[0].capabilities.head[head].scalerCaps;
        const NVHwModeTimingsEvoPtr pTimings = &pProposedDisp->head[head].timings;

        if (!nvValidateHwModeTimingsViewPort(pDevEvo, pScalerCaps, pTimings,
                                             &dummyInfoString)) {
            pReplyDisp->head[head].status =
                NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE;
            return FALSE;
        }
    }

    /*
     * Check that the configuration fits DisplayPort bandwidth constraints.
     */
    if (!ValidateProposedModeSetHwStateOneDispDPlib(pDispEvo, pProposedDisp)) {
        pReplyDisp->status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_DISPLAY_PORT_BANDWIDTH_CHECK;
        return FALSE;
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
        struct NvKmsSetModeOneDispReply *pReplyDisp;

        pReplyDisp = &pReply->disp[dispIndex];

        if (!ValidateProposedModeSetHwStateOneDisp(pDispEvo,
                                                   pActual,
                                                   pProposedDisp,
                                                   pReplyDisp,
                                                   pWorkArea)) {
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
static void AssignSor(NVModeSetWorkArea *pWorkArea,
                      NVConnectorEvoPtr pConnectorEvo)
{
    const NvU32 sd = pConnectorEvo->pDispEvo->displayOwner;

    if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
        // Nothing to do!
        return;
    }

    /* If an OR has already been assigned for this connector, we are done. */
    if (nvDpyIdIsInDpyIdList(
            pConnectorEvo->displayId,
            pWorkArea->sd[sd].sorAssignedConnectorsList)) {
        nvAssert(pConnectorEvo->or.mask != 0x0);
        return;
    }

    /*
     * We keep a record all the SORs assigned for this modeset, so that
     * it can be used as the sorExcludeMask argument to
     * nvAssignSOREvo().
     */
    if (nvAssignSOREvo(
            pConnectorEvo,
            pWorkArea->sd[sd].assignedSorMask /* sorExcludeMask */)) {
        nvAssert(pConnectorEvo->or.mask != 0x0);

        pWorkArea->sd[sd].sorAssignedConnectorsList =
            nvAddDpyIdToDpyIdList(
                pConnectorEvo->displayId,
                pWorkArea->sd[sd].sorAssignedConnectorsList);
        pWorkArea->sd[sd].assignedSorMask |= pConnectorEvo->or.mask;
    } else {
        nvAssert(!"Failed to assign SOR, this failure might cause hang!");
    }
}

static void AssignProposedUsageOneHead(
    NVDevEvoPtr pDevEvo,
    const NVProposedModeSetHwState *pProposed,
    NvU32 head)
{
    const NVProposedModeSetHwStateOneHead *pProposedHead =
        &pProposed->disp[0].head[head];
    const NVProposedModeSetHwStateOneSubDev *pProposedSd =
        &pProposed->sd[0];

    if (!pDevEvo->isSOCDisplay || (pProposedHead->pConnectorEvo == NULL)) {
        return;
    }

    pDevEvo->gpus[0].headState[head].preallocatedUsage =
        pProposedSd->head[head].flip.usage;
}

static NvBool IsProposedModeSetHwStateOneDispIncompatibleWithDpy
(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    const NVConnectorEvoRec *pConnectorEvo
)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {

        const NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];

        if (!pProposedHead->changed) {
            continue;
        }

        /*
         * DDC partners incompatible with each other, only one should be active
         * at a time.
         */
        if ((pProposedHead->pConnectorEvo != NULL) &&
            nvDpyIdIsInDpyIdList(pProposedHead->pConnectorEvo->displayId,
                                 pConnectorEvo->ddcPartnerDpyIdsList)) {
            return TRUE;
        }
    }

    return FALSE;
}

static void
KickoffModesetUpdateState(
    NVDispEvoPtr pDispEvo,
    NVEvoModesetUpdateState *modesetUpdateState)
{
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
                nvDPPostSetMode(pConnectorEvo->pDpLibConnector);
            }  else if (nvConnectorIsDPSerializer(pConnectorEvo)) {
                nvDPSerializerPostSetMode(pDispEvo, pConnectorEvo);
            }
        }
    }

    *modesetUpdateState =
        (NVEvoModesetUpdateState) {  };
}

/*!
 * Determine if display devices driven by head are incompatible with newly
 * activated display devices.
 */
static NvBool
IsProposedModeSetHwStateOneHeadIncompatible(
    NVDispEvoPtr pDispEvo,
    NvU32 head,
    const
    NVProposedModeSetHwStateOneDisp *pProposedDisp)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvBool isIncompatible;

    /*
     * DisplayPort devices require an EVO update when detaching the head
     * from the SOR, because DPlib performs link-training to powerdown
     * the link.  So, always consider DisplayPort as incompatible.
     */

    isIncompatible =
            nvConnectorUsesDPLib(pHeadState->pConnectorEvo) ||
            IsProposedModeSetHwStateOneDispIncompatibleWithDpy(pDispEvo,
                                                               pProposedDisp,
                                                               pHeadState->pConnectorEvo);

    return isIncompatible;
}

static void DisableActiveCoreRGSyncObjects(NVDevEvoPtr pDevEvo,
                                           NVDispHeadStateEvoPtr pHeadState,
                                           NvU32 head,
                                           NVEvoUpdateState *pUpdateState)
{
    for (int i = 0; i < pHeadState->numVblankSyncObjectsCreated; i++) {
        if (pHeadState->vblankSyncObjects[i].enabled) {
            /* hCtxDma of 0 indicates Disable. */
            pDevEvo->hal->ConfigureVblankSyncObject(
                    pDevEvo,
                    0, /* rasterLine */
                    head,
                    i,
                    0, /* hCtxDma */
                    pUpdateState);
            pHeadState->vblankSyncObjects[i].enabled = FALSE;
        }
    }
}

/*!
 * Send methods to shut down a head
 *
 * \param[in,out]  pDispEvo       The disp of the head.
 * \param[in]      head           The head to consider.
 * \param[in]      pProposedDisp  The requested configuration of the display
 * \param[in/out]  modesetUpdateState Structure tracking channels which need to
 *                                    be updated/kicked off
 */
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
    NVDispHeadStateEvoPtr pHeadState;
    NVDpyEvoPtr pDpyEvo;
    const NvU32 sd = pDispEvo->displayOwner;

    /*
     * If nothing changed about this head's configuration, then we
     * should not shut it down.
     */
    if (!pProposedDisp->head[head].changed) {
        return;
    }

    /*
     * Otherwise, go through the shutdown process for any head that
     * changed.  If NVProposedModeSetHwStateOneHead::dpyIdList is
     * empty, then we'll leave it shut down.  If it is non-empty, then
     * ApplyProposedModeSetHwStateOneHead{Pre,Post}Update() will
     * update the head with its new configuration.
     */

    if (!nvHeadIsActive(pDispEvo, head)) {
        return;
    }

    pHeadState = &pDispEvo->headState[head];
    pDpyEvo = nvGetOneArbitraryDpyEvo(pHeadState->activeDpys, pDispEvo);

    /*
     * Identify and disable any active core RG sync objects.
     *
     * Note: the disable occurs at the hardware level; this intentionally does
     * not clear the software state tracking the existence of these sync
     * objects, which will be re-enabled at the hardware level in
     * ApplyProposedModeSetHwStateOneHeadPreUpdate(), if the given head will be
     * active after the modeset.
     */
    DisableActiveCoreRGSyncObjects(pDevEvo, pHeadState, head,
                                   &pWorkArea->modesetUpdateState.updateState);

    nvDisable3DVisionAegis(pDpyEvo);

    nvHdmiDpEnableDisableAudio(pDispEvo, head, FALSE /* enable */);

    /* Cancel any pending LUT updates. */
    nvCancelLutUpdateEvo(pDispEvo, head);

    nvEvoDetachConnector(pHeadState->pConnectorEvo, head, &pWorkArea->modesetUpdateState);

    /* Clear software shadow state. */

    pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList =
        nvAddDpyIdListToDpyIdList(
            pHeadState->activeDpys,
            pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList);
    pHeadState->activeDpys = nvEmptyDpyIdList();
    pHeadState->pConnectorEvo = NULL;

    pHeadState->bypassComposition = FALSE;
    nvkms_memset(&pHeadState->timings, 0, sizeof(pHeadState->timings));

    /* Track old activeRmId and free it after end modeset */
    pWorkArea->sd[pDispEvo->displayOwner].head[head].oldActiveRmId =
        pHeadState->activeRmId;
    pHeadState->activeRmId = 0;

    pHeadState->allowFlipLockGroup = 0;

    nvkms_memset(&pHeadState->audio, 0, sizeof(pHeadState->audio));

    nvkms_memset(&pHeadState->modeValidationParams, 0,
                 sizeof(pHeadState->modeValidationParams));

    nvkms_memset(&pDevEvo->gpus[sd].headState[head], 0,
                 sizeof(pDevEvo->gpus[sd].headState[head]));

    pDpyEvo->head = NV_INVALID_HEAD;
}

static void
ApplyProposedModeSetHwStateOneDispFlip(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwState *pProposed,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head;

    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        const NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];
        const NvU32 sd = pDispEvo->displayOwner;

        /*
         * If nothing changed about this head's configuration, then there
         * is nothing to do.
         */
        if (!pProposedHead->changed) {
            continue;
        }

        /* Check for disabled heads. */
        if (pProposedHead->pConnectorEvo == NULL) {
            continue;
        }

        nvSetUsageBoundsEvo(pDevEvo, sd, head,
                            &pProposed->sd[sd].head[head].flip.usage,
                            pUpdateState);

        nvFlipEvoOneHead(pDevEvo, sd, head,
                         &pProposed->sd[sd].head[head].flip,
                         FALSE /* allowFlipLock */,
                         pUpdateState);
    }
}

static void ReenableActiveCoreRGSyncObjects(NVDevEvoPtr pDevEvo,
                                            NVDispHeadStateEvoPtr pHeadState,
                                            NvU32 head,
                                            NVEvoUpdateState *pUpdateState)
{
    for (int i = 0; i < pHeadState->numVblankSyncObjectsCreated; i++) {
        if (pHeadState->vblankSyncObjects[i].inUse) {
            pDevEvo->hal->ConfigureVblankSyncObject(
                    pDevEvo,
                    pHeadState->timings.rasterBlankStart.y,
                    head,
                    i,
                    pHeadState->vblankSyncObjects[i].evoSyncpt.hCtxDma,
                    pUpdateState);

            pHeadState->vblankSyncObjects[i].enabled = TRUE;
        }
    }
}

/*!
 * Update the heads to be modified on this disp.
 *
 * This should update the ASSY state of the head, but not trigger an
 * UPDATE method.
 *
 * \param[in,out]  pDispEvo       The disp of the head.
 * \param[in]      head           The head to consider.
 * \param[in]      pProposedHead  The requested configuration of the head.
 * \param[in,out]  updateState    Indicates which channels require UPDATEs
 * \param[in]      bypassComposition
 *                                On Turing and newer, enable display
 *                                composition pipeline bypass mode.
 */
static void
ApplyProposedModeSetHwStateOneHeadPreUpdate(
    NVDispEvoPtr pDispEvo,
    NvU32 head,
    const NVProposedModeSetHwState *pProposed,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVModeSetWorkArea *pWorkArea,
    NvBool bypassComposition)
{
    const NVProposedModeSetHwStateOneHead *pProposedHead =
        &pProposedDisp->head[head];
    NVEvoModesetUpdateState *pModesetUpdateState = &pWorkArea->modesetUpdateState;
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVDispHeadStateEvoPtr pHeadState;
    NVDpyEvoPtr pDpyEvo =
        nvGetOneArbitraryDpyEvo(pProposedHead->dpyIdList, pDispEvo);

    /*
     * If nothing changed about this head's configuration, then there
     * is nothing to do.
     */
    if (!pProposedHead->changed) {
        return;
    }

    /* Check for disabled heads. */

    if (pProposedHead->pConnectorEvo == NULL) {
        /*
         * ApplyProposedModeSetHwStateOneHeadShutDown() should have
         * already been called for this head.
         */
        nvAssert(!nvHeadIsActive(pDispEvo, head));
        return;
    }

    if (pDpyEvo == NULL) {
        nvAssert(!"Invalid pDpyEvo");
        return;
    }

    pDpyEvo->head = head;

    AssignSor(pWorkArea, pProposedHead->pConnectorEvo);

    nvDpyUpdateHdmiPreModesetEvo(pDpyEvo);

    pHeadState = &pDispEvo->headState[head];

    pHeadState->bypassComposition = bypassComposition;

    pHeadState->activeRmId = pProposedHead->activeRmId;

    /*
     * Cache the list of active pDpys for this head, as well as the
     * mode timings.
     */
    pHeadState->activeDpys = pProposedHead->dpyIdList;
    pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList =
        nvAddDpyIdListToDpyIdList(
            pHeadState->activeDpys,
            pWorkArea->sd[pDispEvo->displayOwner].changedDpyIdList);

    nvAssert(pDpyEvo->pConnectorEvo == pProposedHead->pConnectorEvo);
    pHeadState->pConnectorEvo = pProposedHead->pConnectorEvo;

    pHeadState->timings = pProposedHead->timings;

    pHeadState->audio = pProposedHead->audio;

    AssignProposedUsageOneHead(pDispEvo->pDevEvo, pProposed, head);

    nvSendHwModeTimingsToAegisEvo(pDispEvo, head);

    /* Set LUT settings */
    nvEvoSetLut(pDispEvo, head, FALSE /* kickoff */, &pProposedHead->lut);

    /* Update current LUT to hardware */
    nvEvoUpdateCurrentPalette(pDispEvo, head, FALSE /* kickoff */);

    nvEvoSetTimings(pDispEvo, head, updateState);

    // Set the dither type & mode
    nvSetDitheringEvo(pDispEvo, head,
                      pDpyEvo->requestedDithering.state,
                      pDpyEvo->requestedDithering.depth,
                      pDpyEvo->requestedDithering.mode,
                      updateState);

    nvEvoHeadSetControlOR(pDispEvo, head, updateState);

    /* Update hardware's current colorSpace and colorRange */
    pHeadState->attributes.colorSpace = pProposedHead->colorSpace;
    pHeadState->attributes.colorRange = pProposedHead->colorRange;
    nvUpdateCurrentHardwareColorSpaceAndRangeEvo(pDispEvo, head, updateState);

    nvEvoAttachConnector(pProposedHead->pConnectorEvo,
                         head,
                         pProposedHead->pDpLibModesetState,
                         pModesetUpdateState);

    nvSetViewPortsEvo(pDispEvo, head, updateState);

    nvSetImageSharpeningEvo(
        pDispEvo,
        head,
        pDpyEvo->currentAttributes.imageSharpening.value,
        updateState);


    nvSetDVCEvo(pDispEvo, head,
                pDpyEvo->currentAttributes.dvc,
                updateState);

    pHeadState->attributes.digitalSignal =
        nvGetDefaultDpyAttributeDigitalSignalValue(pDpyEvo->pConnectorEvo);

    /* If required, nvHdmiFrlSetConfig() overrides attributes.digitalSignal */
    nvHdmiFrlSetConfig(pDispEvo, head);

    /*
     * Re-enable any active sync objects, configuring them in accordance with
     * the new timings.
     */
    ReenableActiveCoreRGSyncObjects(pDispEvo->pDevEvo, pHeadState, head,
                                    updateState);
}


/*!
 * Update the heads to be modified on this disp.
 *
 * PreUpdate() will have already been called on this head, and an
 * UPDATE method sent.
 *
 * \param[in,out]  pDispEvo       The disp of the head.
 * \param[in]      head           The head to consider.
 * \param[in]      pProposedHead  The requested configuration of the head.
 */
static void
ApplyProposedModeSetHwStateOneHeadPostUpdate(NVDispEvoPtr pDispEvo,
                                             NvU32 head,
                                             const
                                             NVProposedModeSetHwStateOneHead
                                             *pProposedHead)
{
    NVDispHeadStateEvoRec *pHeadState;

    /*
     * If nothing changed about this head's configuration, then there
     * is nothing to do.
     */
    if (!pProposedHead->changed) {
        return;
    }

    if (!nvHeadIsActive(pDispEvo, head)) {
        return;
    }

    /*
     * Cache configuration state in the headState, so that
     * AssignProposedModeSetHwState() can preserve the configuration
     * if this head is left alone in the next NvKmsSetModeRequest.
     */
    pHeadState = &pDispEvo->headState[head];
    pHeadState->allowFlipLockGroup = pProposedHead->allowFlipLockGroup;
    pHeadState->modeValidationParams = pProposedHead->modeValidationParams;
    pHeadState->hs10bpcHint = pProposedHead->hs10bpcHint;

    nvUpdateInfoFrames(pDispEvo, head);

    /* Perform 3D vision authentication */
    nv3DVisionAuthenticationEvo(pDispEvo, head);

    nvHdmiDpEnableDisableAudio(pDispEvo, head, TRUE /* enable */);
}

/*
 * Shut down all heads that are incompatible with pProposedDisp. This
 * requires doing an update immediately.
 */
static void
KickoffProposedModeSetHwStateIncompatibleHeadsShutDown(
    NVDispEvoPtr pDispEvo,
    const NVProposedModeSetHwStateOneDisp *pProposedDisp,
    NVModeSetWorkArea *pWorkArea)
{
    NvU32 head;
    NvBool foundIncompatibleHead = FALSE;
    NvU32 clearHdmiFrlActiveRmId[NVKMS_MAX_HEADS_PER_DISP] = { };
    NVDpyIdList proposedActiveConnectorsList = nvEmptyDpyIdList();
    NVDpyIdList currActiveConnectorsList = nvEmptyDpyIdList();
    NVDpyIdList proposedInactiveConnectorList, unionOfActiveConnectorList;

    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        NVDpyId activeConnectorId =
            (pDispEvo->headState[head].pConnectorEvo != NULL) ?
                pDispEvo->headState[head].pConnectorEvo->displayId :
                nvInvalidDpyId();
        NVDpyId proposedConnectorId =
            (pProposedDisp->head[head].pConnectorEvo != NULL) ?
                pProposedDisp->head[head].pConnectorEvo->displayId :
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

    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        NvBool thisHeadIncompatible = FALSE;
        const NVConnectorEvoRec *pCurrConnectorEvo =
            pDispEvo->headState[head].pConnectorEvo;

        if (!pProposedDisp->head[head].changed || !nvHeadIsActive(pDispEvo, head)) {
            continue;
        }

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
         */
        if (nvCountDpyIdsInDpyIdList(unionOfActiveConnectorList) >
            pDispEvo->pDevEvo->numHeads &&
            nvDpyIdIsInDpyIdList(pCurrConnectorEvo->displayId,
                                 proposedInactiveConnectorList)) {
            thisHeadIncompatible = TRUE;
        }

        /* if the *new* timings are FRL, then we need to shut down the head. */
        if (pProposedDisp->head[head].timings.hdmiFrlConfig.frlRate !=
            HDMI_FRL_DATA_RATE_NONE) {
            thisHeadIncompatible = TRUE;
        }

        /* if the *old* timings are FRL, then we need to shut down the head and
         * clear the FRL config. */
        if (pDispEvo->headState[head].timings.hdmiFrlConfig.frlRate !=
            HDMI_FRL_DATA_RATE_NONE) {
            thisHeadIncompatible = TRUE;
            /* cache the activeRmId since it will be cleared below, but
             * we don't want to actually call into the HDMI library until
             * afterwards. */
            clearHdmiFrlActiveRmId[head] = pDispEvo->headState[head].activeRmId;
        }

        if (IsProposedModeSetHwStateOneHeadIncompatible(pDispEvo,
                                                        head,
                                                        pProposedDisp)) {
            thisHeadIncompatible = TRUE;
        }

        if (!thisHeadIncompatible) {
            continue;
        }

        ApplyProposedModeSetHwStateOneHeadShutDown(
            pDispEvo,
            head,
            pProposedDisp,
            pWorkArea);

        foundIncompatibleHead = TRUE;
    }

    /* Submit UPDATE method and kick off, to shut down incompatible heads. */
    if (foundIncompatibleHead) {
        KickoffModesetUpdateState(pDispEvo, &pWorkArea->modesetUpdateState);
        for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
            if (clearHdmiFrlActiveRmId[head] == 0) {
                continue;
            }
            nvHdmiFrlClearConfig(pDispEvo, clearHdmiFrlActiveRmId[head]);
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
    NvU32 head;

    /* Send methods to shut down any other unused heads, but don't update yet. */
    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        ApplyProposedModeSetHwStateOneHeadShutDown(
            pDispEvo,
            head,
            pProposedDisp,
            pWorkArea);
    }

    /* Apply pre-UPDATE modifications for any enabled heads. */
    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        ApplyProposedModeSetHwStateOneHeadPreUpdate(
            pDispEvo,
            head,
            pProposed,
            pProposedDisp,
            pWorkArea,
            bypassComposition);
    }

    if (!decoupleFlipUpdates) {
        /* Merge modeset and flip state updates together */
        ApplyProposedModeSetHwStateOneDispFlip(
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

        ApplyProposedModeSetHwStateOneDispFlip(
            pDispEvo,
            pProposed,
            pProposedDisp,
            &flipUpdateState);

        pDevEvo->hal->Update(pDevEvo,
                             &flipUpdateState,
                             TRUE /* releaseElv */);
    }

    nvRemoveUnusedHdmiDpAudioDevice(pDispEvo);

    /* Apply post-UPDATE modifications for any enabled heads. */
    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {

        const NVProposedModeSetHwStateOneHead *pProposedHead =
            &pProposedDisp->head[head];

        ApplyProposedModeSetHwStateOneHeadPostUpdate(
            pDispEvo,
            head,
            pProposedHead);
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

    KickoffProposedModeSetHwStateIncompatibleHeadsShutDown(
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

        NvU32 head;

        pReply->disp[dispIndex].status =
            NVKMS_SET_MODE_ONE_DISP_STATUS_SUCCESS;

        for (head = 0; head < pDevEvo->numHeads; head++) {

            pReply->disp[dispIndex].head[head].status =
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
    NvU32 dispIndex, head;
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

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {

            if ((pRequestDisp->requestedHeadsBitMask & (1 << head)) == 0) {
                continue;
            }

            if (head >= pDevEvo->numHeads) {
                pReply->disp[dispIndex].status =
                    NVKMS_SET_MODE_ONE_DISP_STATUS_INVALID_REQUESTED_HEADS_BITMASK;
                ret = FALSE;
                continue;
            }

            const NVDpyIdList permDpyIdList =
                pPermissions->disp[dispIndex].head[head].dpyIdList;

            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[head];

            /*
             * Does the client have permission to touch this head at
             * all?
             */
            if (pRequest->commit && nvDpyIdListIsEmpty(permDpyIdList)) {
                pReply->disp[dispIndex].head[head].status =
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
                pReply->disp[dispIndex].head[head].status =
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
                pReply->disp[dispIndex].head[head].status =
                    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY;
                ret = FALSE;
                continue;
            }

            if (!nvValidateSetLutCommonParams(pDevEvo, &pRequestHead->lut)) {
                pReply->disp[dispIndex].head[head].status =
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
AssignReplySuccess(const NVDevEvoRec *pDevEvo,
                   const struct NvKmsSetModeRequest *pRequest,
                   struct NvKmsSetModeReply *pReply,
                   const NVModeSetWorkArea *pWorkArea)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(pReply, 0, sizeof(*pReply));

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        NvU32 head;
        const struct NvKmsSetModeOneDispRequest *pRequestDisp =
            &pRequest->disp[dispIndex];

        if ((pRequest->requestedDispsBitMask & (1 << dispIndex)) == 0) {
            continue;
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const struct NvKmsSetModeOneHeadRequest *pRequestHead =
                &pRequestDisp->head[head];
            struct NvKmsSetModeOneHeadReply *pReplyHead =
                &pReply->disp[dispIndex].head[head];

            if ((pRequestDisp->requestedHeadsBitMask & (1 << head)) == 0) {
                continue;
            }

            pReplyHead->status = NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS;

            if (nvDpyIdListIsEmpty(pRequestHead->dpyIdList)) {
                pReplyHead->activeRmId = 0;
            } else {
                const NVDispHeadStateEvoRec *pHeadState =
                    &pDispEvo->headState[head];
                pReplyHead->activeRmId = pHeadState->activeRmId;
                pReplyHead->possibleUsage = pHeadState->timings.viewPort.possibleUsage;
                pReplyHead->guaranteedUsage = pHeadState->timings.viewPort.guaranteedUsage;
                pReplyHead->usingHeadSurface =
                    (pDispEvo->pHsChannel[head] != NULL);
                pReplyHead->vrrEnabled =
                    (pDispEvo->headState[head].timings.vrr.type !=
                     NVKMS_DPY_VRR_TYPE_NONE);
            }
            FillPostSyncptReplyForModeset(
                pDevEvo,
                head,
                &pRequestHead->flip,
                &pReplyHead->flipReply,
                &pWorkArea->sd[dispIndex].head[head].newState);
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
        NvU32 head, dpyMask = 0;

        /* Compute dpyMask: take all the dpyIds on this dispIndex. */
        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            dpyMask |=
                nvDpyIdListToNvU32(pProposed->disp[dispIndex].head[head].dpyIdList);
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
 * Helper function to validate the proposed mode
 */
static NvBool
IsProposedModeSetValid(NVDevEvoPtr pDevEvo,
                       struct NvKmsSetModeReply *pReply,
                       const struct NvKmsPerOpenDev *pOpenDev,
                       NVProposedModeSetHwState *pProposed,
                       const struct NvKmsSetModeRequest *pRequest,
                       NVModeSetWorkArea *pWorkArea)
{
    return ValidateProposedModeSetHwState(pDevEvo, pProposed, pReply,
                                          pWorkArea);
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
                        const struct NvKmsPerOpenDev *pOpenDev,
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
    NvU32 dispNeedsEarlyUpdate;
    NvBool updateCoreFirst = FALSE;

    /*
     * We should shutdown unused heads and do not inherit the previous modeset
     * state as part of this modeset if:
     * - The requesting client is the internal NVKMS client (i.e.,
     *   this is a console restore modeset), or
     * - 'modesetOwnerChanged' is recorded in the device;
     *   i.e., there was a modeset ownership change since the last
     *   modeset.
     */
    const NvBool modesetOwnerChanged =
        (pOpenDev == pDevEvo->pNvKmsOpenDev) ? TRUE :
        pDevEvo->modesetOwnerChanged;

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

    if (!AssignProposedModeSetHwState(pDevEvo, pOpenDev,
                                      pRequest, pReply, pProposed,
                                      modesetOwnerChanged)) {
        goto done;
    }

    if (!IsProposedModeSetValid(pDevEvo, pReply, pOpenDev, pProposed,
                                pRequest, pWorkArea)) {
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
    if (modesetOwnerChanged) {
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

    nvEvoCancelPostFlipIMPTimer(pDevEvo);

    BeginEndModeset(pDevEvo, pProposed, BEGIN_MODESET);

    nvEvoLockStatePreModeset(pDevEvo, &dispNeedsEarlyUpdate, &pWorkArea->earlyUpdateState);

    nvDisableVrr(pDevEvo);

    updateCoreFirst = pDevEvo->coreInitMethodsPending;
    pDevEvo->coreInitMethodsPending = FALSE;

    /*
     * If the core channel has assembly state we need to be committed
     * before proceeding through the rest of the modeset, kickoff here.
     * This is used to disable fliplock before issuing base flips
     * in ApplyProposedModeSetHwStateOneDisp.
     *
     * XXX This violates the assumption (guarded by
     * pDevEvo->coreInitMethodsPending) that we aren't kicking
     * off until after the assembly core channel state (which we don't
     * want to commit) has already been overwritten below and made safe
     * for kickoff.  Because of this, needsEarlyUpdate should only be set
     * when it is safe to kickoff the existing core channel assembly
     * state immediately.  Currently it is only set when the call
     * to nvEvoLockStatePreModeset() above disabled fliplock, at which
     * point there should be no invalid state remaining in the
     * core channel assembly.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        if (dispNeedsEarlyUpdate & (1 << dispIndex)) {
            nvEvoUpdateAndKickOff(pDispEvo, TRUE, &pWorkArea->earlyUpdateState,
                                  TRUE /* releaseElv */);
        }
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        ApplyProposedModeSetHwStateOneDisp(pDispEvo,
                                           pProposed,
                                           &pProposed->disp[dispIndex],
                                           pWorkArea,
                                           updateCoreFirst,
                                           bypassComposition);
    }

    nvEnableVrr(pDevEvo, pRequest);

    /*
     * Cache whether HS in NVKMS is allowed, so we can make consistent
     * decisions for future partial updates from non-modeset owners.
     */
    pDevEvo->allowHeadSurfaceInNvKms = pProposed->allowHeadSurfaceInNvKms;

    nvEvoLockStatePostModeset(pDevEvo, doRasterLock);

    BeginEndModeset(pDevEvo, pProposed, END_MODESET);

    AssignReplySuccess(pDevEvo, pRequest, pReply, pWorkArea);

    pDevEvo->skipConsoleRestore = FALSE;

    /*
     * If this was a pNvKmsOpenDev-initiated modeset, force the next modeset to
     * shut down all unused heads and not to inherit any state from this
     * modeset. That will prevent a regular client from inheriting
     * pNvKmsOpenDev modeset state.
     */
    pDevEvo->modesetOwnerChanged =
        (pOpenDev == pDevEvo->pNvKmsOpenDev) ? TRUE : FALSE;

    /* fall through */
done:

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 head;

        /*
         * In case of successful commit, update current attribute values and
         * free old display IDs.
         */
        if (pRequest->commit && ret) {
            NVDpyEvoRec *pDpyEvo;

            FOR_ALL_EVO_DPYS(pDpyEvo,
                             pWorkArea->sd[dispIndex].changedDpyIdList,
                             pDispEvo) {
                nvDpyUpdateCurrentAttributes(pDpyEvo);
            }

            for (head = 0; head < pDevEvo->numHeads; head++) {
                if (pWorkArea->sd[dispIndex].head[head].oldActiveRmId != 0x0) {
                    nvRmFreeDisplayId(
                        pDispEvo,
                        pWorkArea->sd[dispIndex].head[head].oldActiveRmId);
                }
            }
        } else {
            /* Otherwise, free new allocated RM display IDs for changed heads */
            for (head = 0; head < pDevEvo->numHeads; head++) {
                NVProposedModeSetHwStateOneHead *pProposedHead =
                    &pProposed->disp[dispIndex].head[head];

                if (!pProposedHead->changed || pProposedHead->activeRmId == 0x0) {
                    continue;
                }
                nvRmFreeDisplayId(pDispEvo, pProposedHead->activeRmId);
            }
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVProposedModeSetHwStateOneHead *pProposedHead =
                &pProposed->disp[dispIndex].head[head];
            nvDPLibFreeModesetState(pProposedHead->pDpLibModesetState);
        }
    }

    /* If all heads are shut down, allow GC6. */
    if (nvAllHeadsInactive(pDevEvo)) {
        nvRmSetGc6Allowed(pDevEvo, TRUE);
    }

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_MODE_SET_WORK_AREA);
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_PROPOSED_MODESET_HW_STATE);
    return ret;
}

/*!
 * Perform a modeset that disables some or all heads.
 *
 * \param[in]      pDevEvo     The device to shut down.
 * \param[in]      pTestFunc   The pointer to test function, identifying heads
 *                             targeted to shut down. If NULL then shut down
 *                             all heads.
 */
void nvShutDownHeads(NVDevEvoPtr pDevEvo, NVShutDownHeadsTestFunc pTestFunc)
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
            NvU32 head;

            req->requestedDispsBitMask |= NVBIT(dispIndex);
            for (head = 0; head < pDevEvo->numHeads; head++) {
                /*
                 * XXX pTestFunc isn't honored by nvSetDispModeEvo()'s
                 * modesetOwnerChanged logic.
                 */
                if (pTestFunc && !pTestFunc(pDispEvo, head)) {
                    continue;
                }

                dirty = TRUE;
                req->disp[dispIndex].requestedHeadsBitMask |= NVBIT(head);
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
                        NVBIT(pDevEvo->numHeads) - 1;
                }
            }

            nvSetDispModeEvo(pDevEvo, pDevEvo->pNvKmsOpenDev, req,
                             &params->reply, FALSE /* bypassComposition */,
                             TRUE /* doRastertLock */);
        }

        nvPreallocRelease(pDevEvo, PREALLOC_TYPE_SHUT_DOWN_HEADS_SET_MODE);
    }

    if (pTestFunc == NULL) {
        nvAssertAllDpysAreInactive(pDevEvo);
    }
}
