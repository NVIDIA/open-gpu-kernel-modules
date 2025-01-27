/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "dp/nvdp-connector.h"
#include "nvdp-timer.hpp"
#include "nvdp-connector-event-sink.hpp"
#include "dp/nvdp-connector-event-sink.h"
#include "dp/nvdp-timer.h"

#include "nvkms-evo.h"
#include "nvkms-types.h"
#include "nvkms-modeset.h"
#include "nvkms-utils.h"
#include "nvkms-rmapi.h"
#include "nvkms-prealloc.h"

#include <dp_connector.h>

// Loop over all display devices attached to a connector.
// Connector::enumDevices(NULL) returns the first device, and then
// enumDevices(previous) returns each subsequent device.
#define for_each_device(connector, dev) \
    for (DisplayPort::Device *(dev) = NULL; ((dev) = (connector)->enumDevices(dev)); )

NVDPLibConnectorPtr nvDPCreateConnector(NVConnectorEvoPtr pConnectorEvo)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    DisplayPort::Timer *pTimer = &pDevEvo->dpTimer->timer;
    NVDPLibConnectorPtr pNVDpLibConnector =
        (NVDPLibConnectorPtr) nvCalloc(1, sizeof(*pNVDpLibConnector));

    if (!pNVDpLibConnector) {
        return NULL;
    }

    pNVDpLibConnector->pConnectorEvo = pConnectorEvo;

    // Create the EVO interface object.
    pNVDpLibConnector->evoInterface =
        new nvkmsDisplayPort::EvoInterface(pConnectorEvo);
    if (!pNVDpLibConnector->evoInterface) {
        goto fail;
    }

    // Create the event sink object.
    pNVDpLibConnector->evtSink =
        new nvkmsDisplayPort::ConnectorEventSink(pConnectorEvo);
    if (!pNVDpLibConnector->evtSink) {
        goto fail;
    }

    // Create the MainLink object.
    pNVDpLibConnector->mainLink =
        DisplayPort::MakeEvoMainLink(pNVDpLibConnector->evoInterface, pTimer);
    if (!pNVDpLibConnector->mainLink) {
        goto fail;
    }

    // Create the AuxBus object.
    pNVDpLibConnector->auxBus =
        DisplayPort::MakeEvoAuxBus(pNVDpLibConnector->evoInterface, pTimer);
    if (!pNVDpLibConnector->auxBus) {
        goto fail;
    }

    pNVDpLibConnector->connector =
        DisplayPort::createConnector(pNVDpLibConnector->mainLink,
                                     pNVDpLibConnector->auxBus,
                                     pTimer,
                                     pNVDpLibConnector->evtSink);
    if (!pNVDpLibConnector->connector) {
        goto fail;
    }

    pNVDpLibConnector->connector->setPolicyAssessLinkSafely(TRUE);

    return pNVDpLibConnector;

 fail:
    nvDPDestroyConnector(pNVDpLibConnector);
    return NULL;
}

void nvDPNotifyLongPulse(NVConnectorEvoPtr pConnectorEvo,
                         NvBool connected)
{
    NVDPLibConnectorPtr pNVDpLibConnector = pConnectorEvo->pDpLibConnector;
    DisplayPort::Connector *c = pNVDpLibConnector->connector;

    pNVDpLibConnector->plugged = connected;

    if (!pNVDpLibConnector->linkHandoffEnabled &&
            connected && !nvAssignSOREvo(pConnectorEvo,
                                         nvDpyIdToNvU32(pConnectorEvo->displayId),
                                         FALSE /* b2Heads1Or */,
                                         0 /* sorExcludeMask */)) {
        // DPLib takes care of skipping LT on unassigned SOR Display. 
    }

    c->notifyLongPulse(connected);

}

void nvDPNotifyShortPulse(NVDPLibConnectorPtr pNVDpLibConnector)
{
    DisplayPort::Connector *c = pNVDpLibConnector->connector;

    c->notifyShortPulse();
}

void nvDPDestroyConnector(NVDPLibConnectorPtr pNVDpLibConnector)
{
    if (!pNVDpLibConnector) return;

    if (pNVDpLibConnector->connector) {
        pNVDpLibConnector->connector->destroy();
    }
    if (pNVDpLibConnector->auxBus) {
        delete pNVDpLibConnector->auxBus;
    }
    if (pNVDpLibConnector->mainLink) {
        delete pNVDpLibConnector->mainLink;
    }
    if (pNVDpLibConnector->evoInterface) {
        delete pNVDpLibConnector->evoInterface;
    }
    if (pNVDpLibConnector->evtSink) {
        delete pNVDpLibConnector->evtSink;
    }

    nvFree(pNVDpLibConnector);
}

NvBool nvDPIsLinkAwaitingTransition(NVConnectorEvoPtr pConnectorEvo)
{
    if (nvConnectorUsesDPLib(pConnectorEvo)) {
        DisplayPort::Connector *c = pConnectorEvo->pDpLibConnector->connector;
        return c->isLinkAwaitingTransition();
    }

    return FALSE;
}

/*!
 * Create a new DisplayPort group and populate it with the devices specified by
 * dpyIdList.  For MST groups, this allocates a dynamic RM display ID.
 * Otherwise, it uses the connector's display ID.
 */
static DisplayPort::Group* CreateGroup(
    const NVDPLibConnectorRec *pDpLibConnector,
    const NVDpyIdList dpyIdList)
{
    NVDpyEvoPtr pDpyEvo;
    DisplayPort::Group *pGroup = NULL;

    pGroup = pDpLibConnector->connector->newGroup();
    if (pGroup == NULL) {
        return NULL;
    }

    // Populate the group
    FOR_ALL_EVO_DPYS(pDpyEvo,
                     dpyIdList, pDpLibConnector->pConnectorEvo->pDispEvo) {
        if (pDpyEvo->dp.pDpLibDevice) {
            pGroup->insert(pDpyEvo->dp.pDpLibDevice->device);
        }
    }

    return pGroup;
}

static NvU32 GetColorDepth(
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc)
{
    switch (colorSpace) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            /*
             * In YUV420, HW is programmed with RGB color space and full color
             * range.  The color space conversion and color range compression
             * happen in a headSurface composite shader.
             */
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            /*
             * For RGB/YCbCr444, each pixel is always 3 components.  For
             * YCbCr/YUV420, we currently always scan out from the headSurface
             * as RGB.
             */
            return colorBpc * 3;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            return colorBpc * 2;
    }

    return 0;
}

static void SetDPMSATiming(const NVDispEvoRec *pDispEvo,
                           const NvU32 displayId,
                           NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS *msaParams,
                           const NVHwModeTimingsEvo *pTimings)
{
    NV0073_CTRL_DP_MSA_PROPERTIES_MASK *featureMask = &msaParams->featureMask;
    NV0073_CTRL_DP_MSA_PROPERTIES_VALUES *featureValues =
        &msaParams->featureValues;

    nvkms_memset(msaParams, 0, sizeof(*msaParams));

    /*
     * Fill in displayId and subDeviceInstance unconditionally.
     * From CL#27980662, dplib started passing the client provided displayId
     * to RM for setting MSA properties.
     * Default value of displayId is 0, leading to RMControl failure in
     * the displayport library.
     */
    msaParams->subDeviceInstance = pDispEvo->displayOwner;
    msaParams->displayId = displayId;

    if ((displayId == 0x0) ||
        ((pTimings->yuv420Mode != NV_YUV420_MODE_SW) &&
         !nvIsAdaptiveSyncDpyVrrType(pTimings->vrr.type))) {
        return;
    }

    msaParams->bEnableMSA = 1;
    msaParams->bCacheMsaOverrideForNextModeset = 1;

    if (pTimings->yuv420Mode == NV_YUV420_MODE_SW) {
        featureMask->bRasterTotalHorizontal   = true;
        featureMask->bActiveStartHorizontal   = true;
        featureMask->bSurfaceTotalHorizontal  = true;
        featureMask->bSyncWidthHorizontal     = true;
        featureValues->rasterTotalHorizontal  = 2 * pTimings->rasterSize.x;
        featureValues->activeStartHorizontal  = 2 * (pTimings->rasterBlankEnd.x + 1);
        featureValues->surfaceTotalHorizontal = 2 * nvEvoVisibleWidth(pTimings);
        featureValues->syncWidthHorizontal    = 2 * (pTimings->rasterSyncEnd.x + 1);
    }

    /*
     * In case of Adaptive-Sync VRR, override VTotal field of MSA (Main Stream
     * Attributes) to workaround bug 4164132.
     */
    if (nvIsAdaptiveSyncDpyVrrType(pTimings->vrr.type)) {
        featureMask->bRasterTotalVertical  = true;
        featureValues->rasterTotalVertical = pTimings->rasterSize.y;
    }
}

static void InitDpModesetParams(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvU32 displayId,
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc,
    DisplayPort::DpModesetParams *pParams)
{
    pParams->modesetInfo.pixelClockHz = pTimings->pixelClock * 1000;
    pParams->modesetInfo.rasterWidth  = pTimings->rasterSize.x;
    pParams->modesetInfo.rasterHeight = pTimings->rasterSize.y;
    pParams->modesetInfo.rasterBlankStartX = pTimings->rasterBlankStart.x;
    pParams->modesetInfo.rasterBlankEndX   = pTimings->rasterBlankEnd.x;
    pParams->modesetInfo.surfaceWidth  = nvEvoVisibleWidth(pTimings);
    pParams->modesetInfo.surfaceHeight = nvEvoVisibleHeight(pTimings);

    pParams->modesetInfo.depth =
        GetColorDepth(colorSpace, colorBpc);
    pParams->modesetInfo.bitsPerComponent = colorBpc;

    pParams->colorFormat = dpColorFormat_Unknown;
    switch (colorSpace) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            /* HW YUV420 mode is only supported for HDMI, not DP */
            nvAssert(pTimings->yuv420Mode == NV_YUV420_MODE_SW);
            pParams->modesetInfo.pixelClockHz *= 2;
            pParams->colorFormat = dpColorFormat_YCbCr420;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            pParams->colorFormat = dpColorFormat_YCbCr444;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            pParams->colorFormat = dpColorFormat_YCbCr422;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            pParams->colorFormat = dpColorFormat_RGB;
            break;
    }

    pParams->headIndex = head;

    SetDPMSATiming(pDispEvo, displayId, &pParams->msaparams, pTimings);
}

NVDPLibModesetStatePtr nvDPLibCreateModesetState(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvU32 displayId,
    const NVDpyIdList dpyIdList,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc,
    const NVHwModeTimingsEvo *pTimings,
    const NVDscInfoEvoRec *pDscInfo)
{
    bool found = false;
    const NVDPLibConnectorRec *pDpLibConnector = NULL;
    const NVDpyEvoRec *pDpyEvo;
    NVDPLibModesetStatePtr pDpLibModesetState = NULL;

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {
        if (!found) {
            pDpLibConnector = pDpyEvo->pConnectorEvo->pDpLibConnector;
            found = true;
        } else if (pDpLibConnector != pDpyEvo->pConnectorEvo->pDpLibConnector) {
            /* All Dpys must belongs to same DP connector */
            return NULL;
        }
    }

    /* Do nothing if any of the display is not DP */
    if (pDpLibConnector == NULL) {
        return NULL;
    }

    pDpLibModesetState =
        (NVDPLibModesetStatePtr) nvCalloc(1, sizeof(*pDpLibModesetState));
    if (pDpLibModesetState == NULL) {
        return NULL;
    }

    InitDpModesetParams(pDispEvo,
                        head,
                        displayId,
                        pTimings,
                        colorSpace,
                        colorBpc,
                        &pDpLibModesetState->modesetParams);
    if (pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DP) {
        pDpLibModesetState->modesetParams.modesetInfo.bEnableDsc = true;

        /*
         * If DSC is enabled then override normal pixel depth with
         * target bpp rate of DSC encoder, the rate at which it is going to
         * output compressed stream.
         */
        pDpLibModesetState->modesetParams.modesetInfo.depth =
            pDscInfo->dp.bitsPerPixelX16;

        switch (pDscInfo->dp.dscMode) {
            case NV_DSC_EVO_MODE_SINGLE:
                pDpLibModesetState->modesetParams.modesetInfo.mode =
                    DSC_SINGLE;
                break;
            case NV_DSC_EVO_MODE_DUAL:
                pDpLibModesetState->modesetParams.modesetInfo.mode =
                    DSC_DUAL;
                break;
        }
    } else {
        nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DISABLED);
    }

    pDpLibModesetState->dpyIdList = dpyIdList;

    return pDpLibModesetState;
}

void nvDPLibFreeModesetState(NVDPLibModesetStatePtr pDpLibModesetState)
{
    nvFree(pDpLibModesetState);
}

static void DestructDpLibIsModesetPossibleParamsOneHead(
    const NvU32 head,
    DisplayPort::DpLinkIsModePossibleParams *pParams)
{
    nvFree(pParams->head[head].pModesetParams);

    if (pParams->head[head].pDscParams != NULL) {
        if (pParams->head[head].pDscParams->forcedParams != NULL) {
            nvFree(pParams->head[head].pDscParams->forcedParams);
        }
        nvFree(pParams->head[head].pDscParams->pDscOutParams);
    }
    nvFree(pParams->head[head].pDscParams);

    if (pParams->head[head].pTarget != NULL) {
        pParams->head[head].pTarget->destroy();
    }

    nvkms_memset(&pParams->head[head], 0, sizeof(pParams->head[head]));
}

static NvBool ConstructDpLibIsModesetPossibleParamsOneHead(
    const NVDPLibConnectorRec *pDpLibConnector,
    const NvU32 head,
    const NvU32 displayId,
    const NVDpyIdList dpyIdList,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    const NVHwModeTimingsEvo *pTimings,
    NVDscInfoEvoRec *pDscInfo,
    const NvBool b2Heads1Or,
    enum NVDpLibIsModePossibleQueryMode queryMode,
    DisplayPort::DP_IMP_ERROR *pErrorCode,
    DisplayPort::DpLinkIsModePossibleParams *pParams)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpLibConnector->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDpyEvoRec *pDpyEvo;

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {
        if (pDpyEvo->pConnectorEvo->pDpLibConnector != pDpLibConnector) {
            goto failed;
        }
    }

    pParams->head[head].pTarget = CreateGroup(pDpLibConnector, dpyIdList);
    if (pParams->head[head].pTarget == NULL) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to create a DisplayPort group");
        goto failed;
    }

    pParams->head[head].pDscParams = (DisplayPort::DscParams*)
        nvCalloc(1, sizeof(*pParams->head[head].pDscParams));
    if (pParams->head[head].pDscParams == NULL) {
        goto failed;
    }

    pParams->head[head].pDscParams->forcedParams = (DSC_INFO::FORCED_DSC_PARAMS*)
        nvCalloc(1, sizeof(*pParams->head[head].pDscParams->forcedParams));
    if (pParams->head[head].pDscParams->forcedParams == NULL) {
        goto failed;
    }

    pParams->head[head].pDscParams->pDscOutParams = (DisplayPort::DscOutParams*)
        nvCalloc(1, sizeof(*pParams->head[head].pDscParams->pDscOutParams));
    if (pParams->head[head].pDscParams->pDscOutParams == NULL) {
        goto failed;
    }

    pParams->head[head].pModesetParams = (DisplayPort::DpModesetParams*)
        nvCalloc(1, sizeof(*pParams->head[head].pModesetParams));
    if (pParams->head[head].pModesetParams == NULL) {
        goto failed;
    }

    InitDpModesetParams(pDispEvo,
                        head,
                        displayId,
                        pTimings,
                        colorSpace,
                        colorBpc,
                        pParams->head[head].pModesetParams);

    if (b2Heads1Or) {
        pParams->head[head].pModesetParams->modesetInfo.mode = DSC_DUAL;
    }

    pParams->head[head].pDscParams->bCheckWithDsc = true;
    switch (queryMode) {
        case NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_PRE_IMP:
            pParams->head[head].pDscParams->forceDsc =
                DisplayPort::DSC_DEFAULT;
            switch (pModeValidationParams->dscMode) {
                case NVKMS_DSC_MODE_FORCE_ENABLE:
                    pParams->head[head].pDscParams->forceDsc =
                        DisplayPort::DSC_FORCE_ENABLE;
                    break;
                case NVKMS_DSC_MODE_FORCE_DISABLE:
                    pParams->head[head].pDscParams->forceDsc =
                        DisplayPort::DSC_FORCE_DISABLE;
                    break;
                default:
                    pParams->head[head].pDscParams->forceDsc =
                        DisplayPort::DSC_DEFAULT;
                    break;
            }

            /*
             * 2Heads1Or requires either YUV420 or DSC; if b2Heads1Or is
             * enabled but YUV420 is not, force DSC.
             */
            if (b2Heads1Or && (pTimings->yuv420Mode != NV_YUV420_MODE_HW)) {
                if (pModeValidationParams->dscMode ==
                        NVKMS_DSC_MODE_FORCE_DISABLE) {
                    goto failed;
                }
                pParams->head[head].pDscParams->forceDsc =
                    DisplayPort::DSC_FORCE_ENABLE;
            }
            break;
        case NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP:
            if (pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DP) {
                pParams->head[head].pDscParams->forceDsc =
                    DisplayPort::DSC_FORCE_ENABLE;
                pParams->head[head].pDscParams->forcedParams->sliceCount =
                    pDscInfo->sliceCount;
            } else {
                nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DISABLED);
                pParams->head[head].pDscParams->forceDsc =
                    DisplayPort::DSC_FORCE_DISABLE;
            }
            break;
    }

    pParams->head[head].pDscParams->bitsPerPixelX16 =
        pModeValidationParams->dscOverrideBitsPerPixelX16;
    pParams->head[head].pErrorStatus = pErrorCode;

    return TRUE;

failed:
    DestructDpLibIsModesetPossibleParamsOneHead(head, pParams);
    return FALSE;
}

static NvBool DPLibNeedPostIMPDpIsModePossible(
    const NVDevEvoRec *pDevEvo,
    const NVDpLibIsModePossibleParamsRec *pParams)
{
    if (pDevEvo->hal->SetMultiTileConfig == NULL) {
        goto done;
    }

    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        NVDscInfoEvoRec *pDscInfo = pParams->head[head].pDscInfo;

        if (nvDpyIdListIsEmpty(pParams->head[head].dpyIdList) ||
                (pDscInfo->type != NV_DSC_INFO_EVO_TYPE_DP)) {
            continue;
        }

        return TRUE;
    }

done:
    return FALSE;
}

/*
 * Validate the DP link for all specified NVHwModeTimingsEvos + dpyIdLists + heads.
 *
 * If validation fails, this function returns FALSE and the mask of heads for
 * which validation is failed.
 *
 * If validation succeeds, the DSC fields within the per head mode parameters
 * are updated with what is returned by dpLinkIsModePossible().
 */
NvBool nvDPLibIsModePossible(const NVDPLibConnectorRec *pDpLibConnector,
                             const NVDpLibIsModePossibleParamsRec *pParams,
                             NvU32 *pFailedHeadMask)
{
    DisplayPort::DpLinkIsModePossibleParams dpImpParams = { };
    DisplayPort::DP_IMP_ERROR dpErrorCode[NV_MAX_HEADS] = { };
    const NVConnectorEvoRec *pConnectorEvo = pDpLibConnector->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvBool ret = FALSE;
    NvU32 head;

    if ((pParams->queryMode ==
            NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP) &&
            !DPLibNeedPostIMPDpIsModePossible(pDevEvo, pParams)) {
        return TRUE;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        if (nvDpyIdListIsEmpty(pParams->head[head].dpyIdList)) {
            continue;
        }

        if (!ConstructDpLibIsModesetPossibleParamsOneHead(
                pDpLibConnector,
                head,
                pParams->head[head].displayId,
                pParams->head[head].dpyIdList,
                pParams->head[head].colorSpace,
                pParams->head[head].colorBpc,
                pParams->head[head].pModeValidationParams,
                pParams->head[head].pTimings,
                pParams->head[head].pDscInfo,
                pParams->head[head].b2Heads1Or,
                pParams->queryMode,
                &dpErrorCode[head],
                &dpImpParams)) {
            goto done;
        }
    }

    ret = pDpLibConnector->connector->dpLinkIsModePossible(dpImpParams);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        DisplayPort::DscParams *pDpDscParams =
            dpImpParams.head[head].pDscParams;
        NVDscInfoEvoRec *pDscInfo = pParams->head[head].pDscInfo;
        const NvBool b2Heads1Or = pParams->head[head].b2Heads1Or;
#if defined(DEBUG)
        const NVHwModeTimingsEvo *pTimings = pParams->head[head].pTimings;
#endif

        if (nvDpyIdListIsEmpty(pParams->head[head].dpyIdList)) {
            continue;
        }

        if (ret) {
            if (b2Heads1Or) {
                /*
                 * 2Heads1OR requires either YUV420 or DSC;
                 * dpDscParams.bEnableDsc is assigned by compoundQueryAttach().
                 */
                nvAssert(pDpDscParams->bEnableDsc ||
                             (pTimings->yuv420Mode == NV_YUV420_MODE_HW));
            }

            if (pDscInfo != NULL) {
                switch (pParams->queryMode) {
                    case NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_PRE_IMP:
                        nvkms_memset(pDscInfo, 0, sizeof(*pDscInfo));

                        if (pDpDscParams->bEnableDsc) {
                            pDscInfo->type = NV_DSC_INFO_EVO_TYPE_DP;
                            pDscInfo->possibleSliceCountMask =
                                pDpDscParams->sliceCountMask;

                            pDscInfo->dp.dscMode = b2Heads1Or ?
                                NV_DSC_EVO_MODE_DUAL : NV_DSC_EVO_MODE_SINGLE;
                            pDscInfo->dp.bitsPerPixelX16 =
                                pDpDscParams->bitsPerPixelX16;
                            ct_assert(sizeof(pDscInfo->dp.pps) ==
                                      sizeof(pDpDscParams->pDscOutParams->PPS));
                            nvkms_memcpy(pDscInfo->dp.pps,
                                         pDpDscParams->pDscOutParams->PPS,
                                         sizeof(pDscInfo->dp.pps));
                        } else {
                            pDscInfo->type = NV_DSC_INFO_EVO_TYPE_DISABLED;
                        }
                        break;
                    case NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP:
                        if (pDpDscParams->bEnableDsc) {
                            nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DP);
                            nvAssert(pDscInfo->possibleSliceCountMask != 0x0);
                            nvAssert(pDscInfo->sliceCount != 0x0);

                            pDscInfo->dp.bitsPerPixelX16 =
                                pDpDscParams->bitsPerPixelX16;
                            ct_assert(sizeof(pDscInfo->dp.pps) ==
                                      sizeof(pDpDscParams->pDscOutParams->PPS));
                            nvkms_memcpy(pDscInfo->dp.pps,
                                         pDpDscParams->pDscOutParams->PPS,
                                         sizeof(pDscInfo->dp.pps));
                        } else {
                            nvAssert(pDscInfo->type ==
                                        NV_DSC_INFO_EVO_TYPE_DISABLED);
                        }
                        break;
                }
            }
        } else if (dpErrorCode[head] != DisplayPort::DP_IMP_ERROR_NONE) {
            *pFailedHeadMask |= NVBIT(head);
        }
    }

    nvAssert(ret || (*pFailedHeadMask != 0x0));

done:
    for (head = 0; head < pDevEvo->numHeads; head++) {
        DestructDpLibIsModesetPossibleParamsOneHead(head, &dpImpParams);
    }

    return ret;
}

NvBool nvDPValidateModeForDpyEvo(
    const NVDpyEvoRec *pDpyEvo,
    const NVDpyAttributeColor *pDpyColor,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    const NVHwModeTimingsEvo *pTimings,
    const NvBool b2Heads1Or,
    NVDscInfoEvoRec *pDscInfo)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 failedHeadMask = 0x0;
    const NvU32 head = 0;
    NvBool ret;

    NVDpLibIsModePossibleParamsRec *pParams = (NVDpLibIsModePossibleParamsRec*)
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_DPLIB_IS_MODE_POSSIBLE_PARAMS,
                      sizeof(*pParams));
    nvAssert(pParams != NULL);

    nvkms_memset(pParams, 0, sizeof(*pParams));

    nvAssert(nvConnectorUsesDPLib(pConnectorEvo));

    pParams->queryMode = NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_NONE;

    pParams->head[head].displayId = 0;
    pParams->head[head].dpyIdList = nvAddDpyIdToEmptyDpyIdList(pDpyEvo->id);
    pParams->head[head].colorSpace = pDpyColor->format;
    pParams->head[head].colorBpc = pDpyColor->bpc;
    pParams->head[head].pModeValidationParams = pModeValidationParams;
    pParams->head[head].pTimings = pTimings;
    pParams->head[head].b2Heads1Or = b2Heads1Or;
    pParams->head[head].pDscInfo = pDscInfo;

    ret = nvDPLibIsModePossible(pConnectorEvo->pDpLibConnector, pParams,
                                &failedHeadMask);

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_DPLIB_IS_MODE_POSSIBLE_PARAMS);

    return ret;
}

static
void DPAttachBeginOneHead(NVDPLibConnectorPtr pDpLibConnector,
                          const NvU32 head,
                          const NVDPLibModesetStateRec *pDpLibModesetState,
                          DisplayPort::DpPreModesetParams *pPreModesetParams)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpLibConnector->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const DisplayPort::DpModesetParams *pParams =
        &pDpLibModesetState->modesetParams;
    const NVDpyEvoRec *pDpyEvo = NULL;

    /* Insert active dpys into group */
    pDpLibConnector->dpyIdList[head] = pDpLibModesetState->dpyIdList;
    FOR_ALL_EVO_DPYS(pDpyEvo, pDpLibConnector->dpyIdList[head], pDispEvo) {
        if (pDpyEvo->dp.pDpLibDevice) {
            pDpLibConnector->pGroup[head]->insert(
                    pDpyEvo->dp.pDpLibDevice->device);
        }
    }

    pPreModesetParams->head[head].pTarget = pDpLibConnector->pGroup[head];
    pPreModesetParams->head[head].pModesetParams = pParams;

    pPreModesetParams->headMask |= NVBIT(head);
}

static void DPAttachEndOneHead(NVDPLibConnectorPtr pDpLibConnector, NvU32 head)
{
    pDpLibConnector->headMask |= NVBIT(head);
}

static void DPDetachBeginOneHead(NVDPLibConnectorPtr pDpLibConnector,
                                 const NvU32 head,
                                 DisplayPort::DpPreModesetParams *pPreModesetParams)
{
    nvAssert((NVBIT(head) & pDpLibConnector->headMask) != 0x0);

    pPreModesetParams->head[head].pTarget = NULL;
    pPreModesetParams->headMask |= NVBIT(head);
}

static void DPDetachEndOneHead(NVDPLibConnectorPtr pDpLibConnector, const NvU32 head)
{
    if (!pDpLibConnector->headInFirmware) {
        const NVConnectorEvoRec *pConnectorEvo =
                                 pDpLibConnector->pConnectorEvo;
        const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
        const NVDpyEvoRec *pDpyEvo;


        /* Empty inactive group */
        FOR_ALL_EVO_DPYS(pDpyEvo, pDpLibConnector->dpyIdList[head], pDispEvo) {
            if (pDpyEvo->dp.pDpLibDevice) {
                pDpLibConnector->pGroup[head]->remove(
                        pDpyEvo->dp.pDpLibDevice->device);
            }
        }
        pDpLibConnector->dpyIdList[head] = nvEmptyDpyIdList();
    } else {
        nvAssert(pDpLibConnector->pGroup[head]->enumDevices(0) == NULL);
        pDpLibConnector->headInFirmware = false;
    }

    pDpLibConnector->headMask &= ~NVBIT(head);
}

/*
 * Handles DP stream programming requires to be done before committing MODESET
 * update. The function should be called for each of affected(change in
 * head-connector attachment) DpLib connectors, before commit.
 */
void nvDPPreSetMode(NVDPLibConnectorPtr pDpLibConnector,
                    const NVEvoModesetUpdateState *pModesetUpdateState)
{
    const NVConnectorEvoRec *pConnectorEvo =
                             pDpLibConnector->pConnectorEvo;
    DisplayPort::Connector *connector = pDpLibConnector->connector;
    NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NvU32 oldHeadMask = pDpLibConnector->headMask;
    const NvU32 newHeadMask =
        nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);
    DisplayPort::DpPreModesetParams preModesetParams = { };

    for (NvU32 head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {

        if ((newHeadMask & NVBIT(head)) != 0x0 &&
            (oldHeadMask & NVBIT(head)) == 0x0) {

            if (pModesetUpdateState->pDpLibModesetState[head] != NULL) {
                DPAttachBeginOneHead(pDpLibConnector,
                                    head,
                                    pModesetUpdateState->pDpLibModesetState[head],
                                    &preModesetParams);
            }
        } else if ((newHeadMask & NVBIT(head)) == 0x0 &&
                   (oldHeadMask & NVBIT(head)) != 0x0) {

            DPDetachBeginOneHead(pDpLibConnector, head, &preModesetParams);

        }
    }

    connector->dpPreModeset(preModesetParams);
}

/*
 * Handles DP stream programming requires to be done before committing MODESET
 * update. The function should be called for each of affected(change in
 * head-connector attachment) DpLib connectors, before commit.
 */
void nvDPPostSetMode(NVDPLibConnectorPtr pDpLibConnector,
                     const NVEvoModesetUpdateState *pModesetUpdateState)
{
    const NVConnectorEvoRec *pConnectorEvo =
                             pDpLibConnector->pConnectorEvo;
    DisplayPort::Connector *connector = pDpLibConnector->connector;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NvU32 oldHeadMask = pDpLibConnector->headMask;
    const NvU32 newHeadMask =
        nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);

    connector->dpPostModeset();

    for (NvU32 head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {

        if ((newHeadMask & NVBIT(head)) != 0x0 &&
            (oldHeadMask & NVBIT(head)) == 0x0) {

            if (pModesetUpdateState->pDpLibModesetState[head] != NULL) {
                DPAttachEndOneHead(pDpLibConnector, head);
            }
        } else if ((newHeadMask & NVBIT(head)) == 0x0 &&
                   (oldHeadMask & NVBIT(head)) != 0x0) {

            DPDetachEndOneHead(pDpLibConnector, head);

        }
    }

    /*
     * Update DisplayPort link information for all displays on DpLib connector
     */
    if (newHeadMask != oldHeadMask) {
        NVDpyEvoPtr pDpyEvo;

        FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->validDisplays, pDispEvo) {
            if (pDpyEvo->pConnectorEvo->pDpLibConnector == pDpLibConnector) {
                nvDPLibUpdateDpyLinkConfiguration(pDpyEvo);
            }
        }
    }
}

void nvDPPause(NVDPLibConnectorPtr pNVDpLibConnector)
{
    DisplayPort::Connector *connector = pNVDpLibConnector->connector;
    const NVConnectorEvoRec *pConnectorEvo = pNVDpLibConnector->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    if (!pNVDpLibConnector->isActive) {
        return;
    }

    if (pDevEvo->skipConsoleRestore && pNVDpLibConnector->headMask != 0) {
        /* Clear vbios DisplayPort RAD scratch registers, see bug 200471345 */

        nvAssert(nvPopCount32(pNVDpLibConnector->headMask) == 1);
        nvAssert(connector->isDp11ProtocolForced());

        NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS params = {0};

        params.subDeviceInstance = pDispEvo->displayOwner;
        params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

        nvAssert(pConnectorEvo->or.protocol ==
                 NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A ||
                 pConnectorEvo->or.protocol ==
                 NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B);

        params.dpLink = pConnectorEvo->or.protocol ==
                        NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A ? 0 : 1;
        params.sorIndex = pConnectorEvo->or.primary;

        NvU32 ret = nvRmApiControl(
            nvEvoGlobal.clientHandle,
            pDevEvo->displayCommonHandle,
            NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG,
            &params,
            sizeof(params));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDispDebug(
                pDispEvo,
                EVO_LOG_ERROR,
                "NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG "
                "failed, error code 0x%x",
                ret);
        }
    }

    /* Before pausing DpLib, destroy group and clear head bitmask */
    for (NvU32 head = 0; head < ARRAY_LEN(pNVDpLibConnector->pGroup); head++) {
        pNVDpLibConnector->pGroup[head]->destroy();
    }
    pNVDpLibConnector->headMask = 0x0;

    connector->pause();

    pNVDpLibConnector->isActive = false;
}

/*!
 * Determine which head, if any, is driving this connector.
 */
static NvU32 GetFirmwareHead(NVConnectorEvoPtr pConnectorEvo)
{
    NvU32 orIndex = pConnectorEvo->or.primary;
    NvU32 ret;

    if (orIndex == NV_INVALID_OR ||
        pConnectorEvo->or.ownerHeadMask[orIndex] == 0) {
        return NV_INVALID_HEAD;
    }

    ret = BIT_IDX_32(pConnectorEvo->or.ownerHeadMask[orIndex]);
    nvAssert(ret < NV_MAX_HEADS);

    return ret;
}

/*!
 * Determine whether an active connector shares an OR with this connector.
 */
static bool IsDDCPartnerActive(NVDPLibConnectorPtr pNVDpLibConnector)
{
    NVConnectorEvoRec *pConnectorEvo =
                       pNVDpLibConnector->pConnectorEvo;
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVConnectorEvoPtr pOtherConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pOtherConnectorEvo, pDispEvo) {
        if (pOtherConnectorEvo != pConnectorEvo &&
            nvIsConnectorActiveEvo(pOtherConnectorEvo) &&
            nvDpyIdIsInDpyIdList(pOtherConnectorEvo->displayId,
                pConnectorEvo->ddcPartnerDpyIdsList)) {
            return true;
        }
    }

    return false;
}

NvBool nvDPResume(NVDPLibConnectorPtr pNVDpLibConnector, NvBool plugged)
{
    NVConnectorEvoRec *pConnectorEvo =
                       pNVDpLibConnector->pConnectorEvo;
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    DisplayPort::Connector *c = pNVDpLibConnector->connector;
    const unsigned int firmwareHead = GetFirmwareHead(pConnectorEvo);
    bool dpyIdIsDynamic = false;
    /* By default allow MST */
    bool allowMST = true;

    pNVDpLibConnector->linkHandoffEnabled =
                            IsDDCPartnerActive(pNVDpLibConnector);

    if (firmwareHead != NV_INVALID_HEAD) {
        NVDpyId firmwareDpyId = nvInvalidDpyId();

        pNVDpLibConnector->headInFirmware = true;
        pNVDpLibConnector->headMask = NVBIT(firmwareHead);

        // Use the first displayId in the boot display list.
        //
        // TODO: What should we do if more than one dpy ID is listed for a boot
        // display?
        nvAssert(nvCountDpyIdsInDpyIdList(pDispEvo->vbiosDpyConfig[firmwareHead]) == 1);
        firmwareDpyId =
            nvNextDpyIdInDpyIdListUnsorted(nvInvalidDpyId(),
                                           pDispEvo->vbiosDpyConfig[firmwareHead]);

        dpyIdIsDynamic = !nvDpyIdsAreEqual(firmwareDpyId,
                                           pConnectorEvo->displayId);

        /* Do not allow MST if firmware driving DP connector in SST mode */
        if (!dpyIdIsDynamic) {
            allowMST = false;
        }
    }

    pConnectorEvo->detectComplete = FALSE;

    pNVDpLibConnector->plugged = plugged;
    if (plugged && !pNVDpLibConnector->headInFirmware) {
        NvBool ret = nvAssignSOREvo(pConnectorEvo,
                                    nvDpyIdToNvU32(pConnectorEvo->displayId),
                                    FALSE /* b2Heads1Or */,
                                    0 /* sorExcludeMask */);

        nvAssert(ret);
        if (!ret) {
            // DP lib skips LT for unassigned SOR.
        }
    }

    c->resume(pNVDpLibConnector->linkHandoffEnabled,
              pNVDpLibConnector->headInFirmware,
              plugged,
              false /* isUefiSystem */,
              firmwareHead,
              dpyIdIsDynamic /* bFirmwareLinkUseMultistream */,
              true /* bDisableVbiosScratchRegisterUpdate, bug 200471345 */,
              allowMST);

    for (NvU32 head = 0; head < ARRAY_LEN(pNVDpLibConnector->pGroup); head++) {
        pNVDpLibConnector->pGroup[head] =
            pNVDpLibConnector->connector->newGroup();

        if (pNVDpLibConnector->pGroup[head] == NULL) {
            for (NvU32 i = 0; i < head; i++) {
                pNVDpLibConnector->pGroup[i]->destroy();
            }
            goto failed;
        }
    }

    pNVDpLibConnector->isActive = true;
    return TRUE;

failed:
    pNVDpLibConnector->connector->pause();
    return FALSE;
}

void nvDPSetAllowMultiStreamingOneConnector(
    NVDPLibConnectorPtr pDpLibConnector,
    NvBool allowMST)
{
    NVConnectorEvoRec *pConnectorEvo =
                       pDpLibConnector->pConnectorEvo;

    if (pDpLibConnector->connector->getAllowMultiStreaming() == allowMST) {
        return;
    }

    /*
     * If there is change in MST capability and DPlib re-runs device detection
     * routine for plugged sink. Reset 'pConnectorEvo->detectComplete' only for
     * MST capable sinks, in order to track completion of that fresh detection
     * routine.
     */
    if (pDpLibConnector->plugged &&
        pDpLibConnector->connector->getSinkMultiStreamCap()) {
        pConnectorEvo->detectComplete = FALSE;
    }
    pDpLibConnector->connector->setAllowMultiStreaming(allowMST);
}

static NvBool IsDpSinkMstCapableForceSst(const NVDispEvoRec *pDispEvo,
                                         const NvU32 apiHead,
                                         void *pData)
{
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    const NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);
    const NVConnectorEvoRec *pConnectorEvo = (pDpyEvo != NULL) ?
        pDpyEvo->pConnectorEvo : NULL;

    if ((pConnectorEvo == NULL) ||
        (pConnectorEvo->pDpLibConnector == NULL)) {
        return FALSE;
    }

    DisplayPort::Connector *c =
        pConnectorEvo->pDpLibConnector->connector;

    return (c->getSinkMultiStreamCap() && !c->getAllowMultiStreaming());
}

static NvBool IsDpLinkTransitionWaitingForHeadShutDown(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead,
    void *pData)
{
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    const NVDpyEvoRec *pDpyEvo =
        nvGetOneArbitraryDpyEvo(pApiHeadState->activeDpys, pDispEvo);

    return (pDpyEvo != NULL) &&
           nvDPIsLinkAwaitingTransition(pDpyEvo->pConnectorEvo);
}

void nvDPSetAllowMultiStreaming(NVDevEvoPtr pDevEvo, NvBool allowMST)
{
    NvBool needUpdate = FALSE;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NVConnectorEvoPtr pConnectorEvo;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            NVDPLibConnectorPtr pDpLibConnector =
                                pConnectorEvo->pDpLibConnector;
            if (pDpLibConnector &&
                pDpLibConnector->connector->getAllowMultiStreaming()
                    != allowMST) {
                needUpdate = TRUE;
            }
        }
    }

    if (!needUpdate) {
        return;
    }

    nvShutDownApiHeads(pDevEvo, pDevEvo->pNvKmsOpenDev,
                       IsDpSinkMstCapableForceSst, NULL /* pData */,
                       TRUE /* doRasterLock */);

    /*
     * Heads driving MST capable sinks in force SST mode, are shut down. Now you
     * can allow MST on all DisplayPort Connector, safely in compliance
     * of DP 1.2 specification.
     *
     * The section 5.4 and table 2-75 (of section 2.9.3.1) of DisplayPort 1.2
     * specification, does not allow to enable/disable MST mode of sink while
     * transmitting active stream (see description of CL#25551338).
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NVConnectorEvoPtr pConnectorEvo;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            if (!pConnectorEvo->pDpLibConnector) {
                continue;
            }
            nvDPSetAllowMultiStreamingOneConnector(
                pConnectorEvo->pDpLibConnector,
                allowMST);
        }
    }

    /* Shut down all DisplayPort heads that need to transition to/from SST. */
    nvShutDownApiHeads(pDevEvo, pDevEvo->pNvKmsOpenDev,
                       IsDpLinkTransitionWaitingForHeadShutDown,
                       NULL /* pData */,
                       TRUE /* doRasterLock */);

    /*
     * Handle any pending timers the DP library scheduled to notify us
     * about changes in the connected device list.
     */
    nvDPFireExpiredTimers(pDevEvo);
}

enum NVDpLinkMode nvDPGetActiveLinkMode(NVDPLibConnectorPtr pDpLibConnector)
{
    DisplayPort::LinkConfiguration linkConfig =
        pDpLibConnector->connector->getActiveLinkConfig();
    if (linkConfig.lanes == 0) {
        return NV_DP_LINK_MODE_OFF;
    }
    return linkConfig.multistream ? NV_DP_LINK_MODE_MST :
                NV_DP_LINK_MODE_SST;
}

void nvDPSetLinkHandoff(NVDPLibConnectorPtr pDpLibConnector, NvBool enable)
{
    if (enable) {
        pDpLibConnector->connector->enableLinkHandsOff();
        pDpLibConnector->linkHandoffEnabled = TRUE;
    } else {
        pDpLibConnector->linkHandoffEnabled = FALSE;
        pDpLibConnector->connector->releaseLinkHandsOff();
    }
}
