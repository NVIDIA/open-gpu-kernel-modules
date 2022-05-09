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
#include "nvkms-modeset-types.h"
#include "nvkms-utils.h"
#include "nvkms-rmapi.h"

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

    if (connected && !nvAssignSOREvo(pConnectorEvo, 0 /* sorExcludeMask */)) {
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

/*
 * Start DisplayPort mode validation on all connectors on a disp.
 */
void nvDPBeginValidation(NVDispEvoPtr pDispEvo)
{
    NVConnectorEvoPtr pConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            pConnectorEvo->pDpLibConnector->connector->beginCompoundQuery();
        }
    }
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

/*!
 * Returns the bits per pixel for the pixel depth value given
 *
 * \param[in]   pixelDepth   nvKmsPixelDepth value
 *
 * \return      The pixel depth configured by this enum value
 */
static NvU32 GetSORBpp(
    const enum nvKmsPixelDepth pixelDepth,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace)
{
    NvU32 bpc = nvPixelDepthToBitsPerComponent(pixelDepth);
    if (bpc == 0) {
        nvAssert(!"Unrecognized SOR pixel depth");
        /* XXX Assume lowest ? */
        bpc = 6;
    }

    /*
     * In YUV420, HW is programmed with RGB color space and full color range.
     * The color space conversion and color range compression happen in a
     * headSurface composite shader.
     *
     * XXX Add support for
     * NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422 over DP.
     */
    nvAssert(colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420 ||
             colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444 ||
             colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB);

    /* For RGB/YCbCr444, each pixel is always 3 components.  For YCbCr/YUV420,
     * we currently always scan out from the headSurface as RGB. */
    return bpc * 3;
}

/* XXX Instead of tracking pixelDepth, you should track bpc and calculate bpp
 * from bpc + colorSpace. */
static NvU32 GetBpc(
    const enum nvKmsPixelDepth pixelDepth,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace)
{
    NvU32 bpc = nvPixelDepthToBitsPerComponent(pixelDepth);
    if (bpc == 0) {
        nvAssert(!"Unrecognized SOR pixel depth");
        /* XXX Assume lowest ? */
        return 6;
    }

    /*
     * In YUV420, HW is programmed with RGB color space and full color range.
     * The color space conversion and color range compression happen in a
     * headSurface composite shader.
     *
     * XXX Add support for
     * NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422 over DP.
     */
    nvAssert(colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420 ||
             colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444 ||
             colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB);

    return bpc;
}

static void SetDPMSATiming(const NVDispEvoRec *pDispEvo,
                           const NvU32 displayId,
                           NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS *msaParams,
                           const NVHwModeTimingsEvo *pTimings)
{
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

    if ((pTimings->yuv420Mode == NV_YUV420_MODE_SW) && displayId != 0) {
        NV0073_CTRL_DP_MSA_PROPERTIES_MASK *featureMask = &msaParams->featureMask;
        NV0073_CTRL_DP_MSA_PROPERTIES_VALUES *featureValues = &msaParams->featureValues;

        msaParams->bEnableMSA = 1;
        msaParams->bCacheMsaOverrideForNextModeset = 1;
        featureMask->bRasterTotalHorizontal   = true;
        featureMask->bActiveStartHorizontal   = true;
        featureMask->bSurfaceTotalHorizontal  = true;
        featureMask->bSyncWidthHorizontal     = true;
        featureValues->rasterTotalHorizontal  = 2 * pTimings->rasterSize.x;
        featureValues->activeStartHorizontal  = 2 * (pTimings->rasterBlankEnd.x + 1);
        featureValues->surfaceTotalHorizontal = 2 * nvEvoVisibleWidth(pTimings);
        featureValues->syncWidthHorizontal    = 2 * (pTimings->rasterSyncEnd.x + 1);
    }
}

static void InitDpModesetParams(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvU32 displayId,
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
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
        GetSORBpp(pTimings->pixelDepth, colorSpace);

    pParams->modesetInfo.bitsPerComponent =
        GetBpc(pTimings->pixelDepth, colorSpace);

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
    NVHwModeTimingsEvo *pTimings)
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
                        &pDpLibModesetState->modesetParams);
    if (pTimings->dpDsc.enable) {
        pDpLibModesetState->modesetParams.modesetInfo.bEnableDsc = true;

        /*
         * If DSC is enabled then override normal pixel depth with
         * target bpp rate of DSC encoder, the rate at which it is going to
         * output compressed stream.
         */
        pDpLibModesetState->modesetParams.modesetInfo.depth =
            pTimings->dpDsc.bitsPerPixelX16;
    }
    pDpLibModesetState->dpyIdList = dpyIdList;

    return pDpLibModesetState;
}

void nvDPLibFreeModesetState(NVDPLibModesetStatePtr pDpLibModesetState)
{
    nvFree(pDpLibModesetState);
}

/*
 * Validate the mode for a given NVHwModeTimingsEvo + dpyIdList.  This
 * function should be called for each head, and must be called between
 * nvDPBeginValidation and nvDPEndValidation.
 *
 * If validation fails, this function returns FALSE.  You must still call
 * nvDPEndValidation even if an individual head fails.
 *
 * If validation succeeds, the DSC fields within pTimings are updated with what
 * is returned by compoundQueryAttach().
 */
NvBool nvDPLibValidateTimings(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvU32 displayId,
    const NVDpyIdList dpyIdList,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    NVHwModeTimingsEvo *pTimings)
{
    const NVDpyEvoRec *pDpyEvo;
    const NVDPLibConnectorRec *pDpLibConnector = NULL;
    bool found = false;

    DisplayPort::Group *pGroup = NULL;
    DisplayPort::DscOutParams *pDscOutParams = NULL;
    DisplayPort::DpModesetParams *pModesetParams = NULL;
    DisplayPort::DscParams dpDscParams;
    NvBool ret = FALSE;

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {
        if (!found) {
            pDpLibConnector = pDpyEvo->pConnectorEvo->pDpLibConnector;
            found = true;
        } else if (pDpLibConnector != pDpyEvo->pConnectorEvo->pDpLibConnector) {
            /* All Dpys must belongs to same DP connector */
            return FALSE;
        }
    }

    /* Do nothing if any of the display is not DP */
    if (pDpLibConnector == NULL) {
        return TRUE;
    }

    pGroup = CreateGroup(pDpLibConnector, dpyIdList);
    if (pGroup == NULL) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to create a DisplayPort group");
        goto done;
    }

    pDscOutParams =
        (DisplayPort::DscOutParams*) nvCalloc(1, sizeof(*pDscOutParams));
    if (pDscOutParams == NULL) {
        goto done;
    }


    pModesetParams =
        (DisplayPort::DpModesetParams*) nvCalloc(1, sizeof(*pModesetParams));
    if (pModesetParams == NULL) {
        goto done;
    }

    InitDpModesetParams(pDispEvo,
                        head,
                        displayId,
                        pTimings,
                        colorSpace,
                        pModesetParams);

    dpDscParams.bCheckWithDsc = true;
    dpDscParams.forceDsc = pModeValidationParams->forceDsc ?
        DisplayPort::DSC_FORCE_ENABLE :
        DisplayPort::DSC_DEFAULT;
    dpDscParams.bitsPerPixelX16 =
        pModeValidationParams->dscOverrideBitsPerPixelX16;
    dpDscParams.pDscOutParams = pDscOutParams;

    ret = pDpLibConnector->connector->compoundQueryAttach(
            pGroup, *pModesetParams,
            &dpDscParams);

    if (ret) {
        pTimings->dpDsc.enable = dpDscParams.bEnableDsc;
        pTimings->dpDsc.bitsPerPixelX16 = dpDscParams.bitsPerPixelX16;

        ct_assert(sizeof(pTimings->dpDsc.pps) == sizeof(pDscOutParams->PPS));

        nvkms_memcpy(pTimings->dpDsc.pps,
                     pDscOutParams->PPS, sizeof(pTimings->dpDsc.pps));
    }

done:
    nvFree(pDscOutParams);
    nvFree(pModesetParams);
    if (pGroup != NULL) {
        pGroup->destroy();
    }
    return ret;
}

/*
 * Finishes DisplayPort mode validation.  Returns TRUE if the complete
 * configuration is possible, and FALSE if it can't be achieved.
 */
NvBool nvDPEndValidation(NVDispEvoPtr pDispEvo)
{
    NvBool ret = TRUE;
    NVConnectorEvoPtr pConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            DisplayPort::Connector *connector =
                pConnectorEvo->pDpLibConnector->connector;

             /* endCompoundQuery() must be called for all dp connectors */
            ret = connector->endCompoundQuery() && ret;
        }
    }

    return ret;
}

NvBool nvDPValidateModeForDpyEvo(
    const NVDpyEvoRec *pDpyEvo,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const struct NvKmsModeValidationParams *pModeValidationParams,
    NVHwModeTimingsEvo *pTimings)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;

    nvAssert(nvConnectorUsesDPLib(pConnectorEvo));

    DisplayPort::Connector *connector =
        pConnectorEvo->pDpLibConnector->connector;

    connector->beginCompoundQuery();
    NvBool ret = nvDPLibValidateTimings(pDpyEvo->pDispEvo,
                                        0 /* head */,
                                        0 /* displayId */,
                                        nvAddDpyIdToEmptyDpyIdList(pDpyEvo->id),
                                        colorSpace,
                                        pModeValidationParams,
                                        pTimings);
    connector->endCompoundQuery();

    return ret;
}

/*
 * Notify the DisplayPort library that a given mode is about to be set on a
 * given head.  The configuration for this head must have previously been
 * validated by a call to nvDPLibValidateTimings.
 */
static
void NotifyAttachBegin(NVDPLibConnectorPtr pDpLibConnector,
                       const NvU32 head,
                       const NVDPLibModesetStateRec *pDpLibModesetState)
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

    pDpLibConnector->connector->notifyAttachBegin(
        pDpLibConnector->pGroup[head],
        *pParams);
}

/*
 * Notify the DisplayPort library that a modeset on a head begun by
 * nvDPNotifyAttachBegin is finished.
 */
static void NotifyAttachEnd(NVDPLibConnectorPtr pDpLibConnector, NvU32 head)
{
    pDpLibConnector->connector->notifyAttachEnd(false);
    pDpLibConnector->headMask |= NVBIT(head);
}

/*
 * Notify the DisplayPort library that the given head driving displays on this
 * connector is about to be shut down.
 */
static void NotifyDetachBegin(NVDPLibConnectorPtr pDpLibConnector, const NvU32 head)
{
    /*
     * The firmware group is the VBIOS monitor group the DP Library manages
     * internally. In notifyDetachBegin(NULL), the NULL defaults to firmware
     * group.
     */
    pDpLibConnector->connector->notifyDetachBegin(
        pDpLibConnector->headInFirmware ?
            NULL : pDpLibConnector->pGroup[head]);
}

/*
 * Notify the DisplayPort library that the driver has finished shutting down a
 * head that was previously driving this connector.
 */
static void NotifyDetachEnd(NVDPLibConnectorPtr pDpLibConnector, const NvU32 head)
{
    pDpLibConnector->connector->notifyDetachEnd();

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
    NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NvU32 oldHeadMask = pDpLibConnector->headMask;
    const NvU32 newHeadMask =
        nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);

    for (NvU32 head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {

        if ((newHeadMask & NVBIT(head)) != 0x0 &&
            (oldHeadMask & NVBIT(head)) == 0x0) {

            NotifyAttachBegin(pDpLibConnector,
                              head,
                              pModesetUpdateState->pDpLibModesetState[head]);

        } else if ((newHeadMask & NVBIT(head)) == 0x0 &&
                   (oldHeadMask & NVBIT(head)) != 0x0) {

            NotifyDetachBegin(pDpLibConnector, head);

        }
    }
}

/*
 * Handles DP stream programming requires to be done before committing MODESET
 * update. The function should be called for each of affected(change in
 * head-connector attachment) DpLib connectors, before commit.
 */
void nvDPPostSetMode(NVDPLibConnectorPtr pDpLibConnector)
{
    const NVConnectorEvoRec *pConnectorEvo =
                             pDpLibConnector->pConnectorEvo;
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NvU32 oldHeadMask = pDpLibConnector->headMask;
    const NvU32 newHeadMask =
        nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);

    for (NvU32 head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {

        if ((newHeadMask & NVBIT(head)) != 0x0 &&
            (oldHeadMask & NVBIT(head)) == 0x0) {

            NotifyAttachEnd(pDpLibConnector, head);

        } else if ((newHeadMask & NVBIT(head)) == 0x0 &&
                   (oldHeadMask & NVBIT(head)) != 0x0) {

            NotifyDetachEnd(pDpLibConnector, head);

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
        params.sorIndex = nvEvoConnectorGetPrimaryOr(pConnectorEvo);

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
    NvU32 orIndex = nvEvoConnectorGetPrimaryOr(pConnectorEvo);

    if (orIndex == NV_INVALID_OR ||
        pConnectorEvo->or.ownerHeadMask[orIndex] == 0) {
        return NV_INVALID_HEAD;
    }

    return BIT_IDX_32(pConnectorEvo->or.ownerHeadMask[orIndex]);
}

/*!
 * Determine whether an active connector shares an OR with this connector.
 */
static bool ConnectorIsSharedWithActiveOR(NVConnectorEvoPtr pConnectorEvo)
{
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVConnectorEvoPtr pOtherConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pOtherConnectorEvo, pDispEvo) {
        if (pOtherConnectorEvo != pConnectorEvo &&
            nvIsConnectorActiveEvo(pOtherConnectorEvo) &&
            (pOtherConnectorEvo->or.mask & pConnectorEvo->or.mask) != 0x0) {
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
    const bool firmwareLinkHandsOff = ConnectorIsSharedWithActiveOR(pConnectorEvo);
    bool dpyIdIsDynamic = false;
    /* By default allow MST */
    bool allowMST = true;

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
        NvBool ret = nvAssignSOREvo(pConnectorEvo, 0 /* sorExcludeMask */);

        nvAssert(ret);
        if (!ret) {
            // DP lib skips LT for unassigned SOR.
        }
    }

    c->resume(firmwareLinkHandsOff,
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
                                         const NvU32 head)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVConnectorEvoRec *pConnectorEvo = pHeadState->pConnectorEvo;

    if (pConnectorEvo == NULL ||
        pConnectorEvo->pDpLibConnector == NULL) {
        return FALSE;
    }

    DisplayPort::Connector *c =
        pConnectorEvo->pDpLibConnector->connector;

    return (c->getSinkMultiStreamCap() && !c->getAllowMultiStreaming());
}

static NvBool IsDpLinkTransitionWaitingForHeadShutDown(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    return pHeadState->pConnectorEvo &&
           nvDPIsLinkAwaitingTransition(pHeadState->pConnectorEvo);
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

    nvShutDownHeads(pDevEvo, IsDpSinkMstCapableForceSst);

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
    nvShutDownHeads(pDevEvo,
                    IsDpLinkTransitionWaitingForHeadShutDown);

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
