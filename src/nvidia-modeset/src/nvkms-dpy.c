/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "dp/nvdp-device.h"
#include "dp/nvdp-connector-event-sink.h"

#include "nvkms-evo.h"
#include "nvkms-dpy.h"
#include "nvkms-dpy-override.h"
#include "nvkms-hdmi.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-types.h"
#include "nvkms-attributes.h"
#include "nvkms-utils.h"
#include "nvkms-3dvision.h"

#include "nv_mode_timings_utils.h"

#include "nvkms-api.h"
#include "nvkms-private.h"

#include "nvos.h"
#include "timing/dpsdp.h"

#include "displayport/displayport.h"

#include <ctrl/ctrl0073/ctrl0073dfp.h> // NV0073_CTRL_DFP_FLAGS_*
#include <ctrl/ctrl0073/ctrl0073dp.h> // NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_*

#define TMDS_SINGLE_LINK_PCLK_MAX 165000
#define TMDS_DUAL_LINK_PCLK_MAX 330000

static void DpyGetDynamicDfpProperties(
    NVDpyEvoPtr pDpyEvo,
    const NvBool disableACPIBrightnessHotkeys);

static void
CreateParsedEdidFromNVT_TIMING(NVT_TIMING *pTimings,
                               NvU8 bpc,
                               NVParsedEdidEvoPtr pParsedEdid);

static NvBool ReadEdidFromDP              (const NVDpyEvoRec *pDpyEvo,
                                           NVEdidPtr pEdid);
static NvBool ReadEdidFromResman          (const NVDpyEvoRec *pDpyEvo,
                                           NVEdidPtr pEdid,
                                           NvKmsEdidReadMode readMode);
static NvBool ValidateEdid                (const NVDpyEvoRec *pDpyEvo,
                                           NVEdidPtr pEdid,
                                           NVEvoInfoStringPtr pInfoString,
                                           const NvBool ignoreEdidChecksum);
static void LogEdid                       (NVDpyEvoPtr pDpyEvo,
                                           NVEvoInfoStringPtr pInfoString);
static void ClearEdid                     (NVDpyEvoPtr pDpyEvo, const NvBool bSendHdmiCapsToRm);
static void ClearCustomEdid               (const NVDpyEvoRec *pDpyEvo);
static void WriteEdidToResman             (const NVDpyEvoRec *pDpyEvo,
                                           const NVEdidRec *pEdid);
static void PatchAndParseEdid             (const NVDpyEvoRec *pDpyEvo,
                                           NVEdidPtr pEdid,
                                           NVParsedEdidEvoPtr,
                                           NVEvoInfoStringPtr pInfoString);
static void ReadAndApplyEdidEvo           (NVDpyEvoPtr pDpyEvo,
                                           struct NvKmsQueryDpyDynamicDataParams *pParams);
static NvBool GetFixedModeTimings         (NVDpyEvoPtr pDpyEvo, struct NvKmsSuperframeInfo *pSuperframeInfo);
static NvBool ReadDSITimingsFromResman    (const NVDpyEvoRec *pDpyEvo,
                                           NVT_TIMING *pTimings,
                                           NvU8 *pBpc);
static void AssignDpyEvoName              (NVDpyEvoPtr pDpyEvo);

static NvBool IsConnectorTMDS             (NVConnectorEvoPtr);


static void DpyDisconnectEvo(NVDpyEvoPtr pDpyEvo, const NvBool bSendHdmiCapsToRm)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;

    pDispEvo->connectedDisplays =
        nvDpyIdListMinusDpyId(pDispEvo->connectedDisplays, pDpyEvo->id);

    ClearEdid(pDpyEvo, bSendHdmiCapsToRm);
}

static NvBool DpyConnectEvo(
    NVDpyEvoPtr pDpyEvo,
    struct NvKmsQueryDpyDynamicDataParams *pParams)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;

    pDispEvo->connectedDisplays =
        nvAddDpyIdToDpyIdList(pDpyEvo->id, pDispEvo->connectedDisplays);

    DpyGetDynamicDfpProperties(pDpyEvo, pParams->request.disableACPIBrightnessHotkeys);
    nvDPGetDpyGUID(pDpyEvo);

    if ((pDpyEvo->pConnectorEvo->signalFormat == NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) ||
        nvConnectorIsDPSerializer(pDpyEvo->pConnectorEvo)) {
        if (!GetFixedModeTimings(pDpyEvo, &pParams->reply.superframeInfo)) {
            return FALSE;
        }
    } else {
        ReadAndApplyEdidEvo(pDpyEvo, pParams);
    }

    nvUpdateInfoFrames(pDpyEvo);

    return TRUE;
}

/*
 * DpyAssignColorSpaceCaps() - parse both the CEA-861 extension block and 
 * the EDID 1.4 block to determine YCbCr422/444 capability.
 */
static void DpyAssignColorSpaceCaps(NVDpyEvoPtr pDpyEvo,
                                    NVEvoInfoStringPtr pInfoString)
{
    NvBool ycbr422_cap = FALSE;
    NvBool ycbr444_cap = FALSE;
    const NVParsedEdidEvoRec *pParsedEdid = &pDpyEvo->parsedEdid;

    /* check for edid YCbCr422/YCbCr444 capability */
    if (pParsedEdid->valid) {
        NvBool haveCEA861Block =
            (pParsedEdid->info.ext861.revision != NVT_CEA861_REV_NONE);
        if (haveCEA861Block) {
            ycbr422_cap = !!(pParsedEdid->info.ext861.basic_caps &
                             NVT_CEA861_CAP_YCbCr_422);
            ycbr444_cap = !!(pParsedEdid->info.ext861.basic_caps &
                             NVT_CEA861_CAP_YCbCr_444);
        }
        /* check EDID 1.4 base block */
        if (pParsedEdid->info.version == 0x104 &&
            pParsedEdid->info.input.isDigital) {
            NvBool edid14_ycbr422 =
                pParsedEdid->info.u.feature_ver_1_4_digital.support_ycrcb_422;
            NvBool edid14_ycbr444 =
                pParsedEdid->info.u.feature_ver_1_4_digital.support_ycrcb_444;
            if (haveCEA861Block && ycbr422_cap != edid14_ycbr422) {
                nvEvoLogInfoString(pInfoString,
                       "%s EDID inconsistency: the EDID 1.4 base block %s "
                       "YCbCr 4:2:2 support, but the CEA-861 extension block "
                       "%s. Assuming YCbCr 4:2:2 is supported.",
                       pDpyEvo->name,
                       edid14_ycbr422 ? "indicates" : "does not indicate",
                       ycbr422_cap ? "does" : "does not");
            }
            if (edid14_ycbr422) {
                ycbr422_cap = TRUE;
            }
            if (haveCEA861Block && ycbr444_cap != edid14_ycbr444) {
                nvEvoLogInfoString(pInfoString,
                       "%s EDID inconsistency: the EDID 1.4 base block %s "
                       "YCbCr 4:4:4 support, but the CEA-861 extension block "
                       "%s. Assuming YCbCr 4:4:4 is supported.",
                       pDpyEvo->name,
                       edid14_ycbr444 ? "indicates" : "does not indicate",
                       ycbr444_cap ? "does" : "does not");
            }
            if (edid14_ycbr444) {
                ycbr444_cap = TRUE;
            }
        }
    }
    pDpyEvo->colorSpaceCaps.ycbcr422Capable = ycbr422_cap;
    pDpyEvo->colorSpaceCaps.ycbcr444Capable = ycbr444_cap;
}



static NvBool GetEdidOverride(
    const struct NvKmsQueryDpyDynamicDataRequest *pRequest,
    NVEdidRec *pEdid)
{
    if ((pRequest == NULL) ||
        !pRequest->overrideEdid ||
        (pRequest->edid.bufferSize == 0) ||
        (pRequest->edid.bufferSize > sizeof(pRequest->edid.buffer))) {
        return FALSE;
    }

    pEdid->buffer = nvAlloc(pRequest->edid.bufferSize);

    if (pEdid->buffer == NULL) {
        return FALSE;
    }

    nvkms_memcpy(pEdid->buffer, pRequest->edid.buffer, pRequest->edid.bufferSize);

    pEdid->length = pRequest->edid.bufferSize;

    return TRUE;
}

/*!
 * Query resman for the EDID for the pDpyEvo, then parse the EDID into usable
 * data.  Do not modify the pDpyEvoRec.
 */

NvBool nvDpyReadAndParseEdidEvo(
    const NVDpyEvoRec *pDpyEvo,
    const struct NvKmsQueryDpyDynamicDataRequest *pRequest,
    NvKmsEdidReadMode readMode,
    NVEdidRec *pEdid,
    NVParsedEdidEvoPtr *ppParsedEdid,
    NVEvoInfoStringPtr pInfoString)
{
    NvBool ignoreEdid = FALSE;
    NvBool ignoreEdidChecksum = FALSE;

    if (pRequest != NULL) {
        ignoreEdid = pRequest->ignoreEdid;
        ignoreEdidChecksum = pRequest->ignoreEdidChecksum;
    }

    nvkms_memset(pEdid, 0, sizeof(*pEdid));

    /* Just return an empty EDID if requested. */
    if (ignoreEdid) {
        return TRUE;
    }

    /* Load any custom EDID, (or see if DP lib has EDID) */
    ClearCustomEdid(pDpyEvo);

    if ((pRequest && GetEdidOverride(pRequest, pEdid)) ||
        ReadEdidFromDP(pDpyEvo, pEdid)) {
        /* XXX [VSM] Write, clear and re-read the EDID to/from RM here to make
         * sure RM and X agree on the final EDID bits.  Once RM no longer
         * parses the EDID, we can avoid doing this for DP devices.
         *
         * If it's a DisplayPort 1.2 multistream device then don't bother trying
         * to ping-pong the EDID through RM.
         */
        if (nvDpyEvoIsDPMST(pDpyEvo)) {
            goto validateEdid;
        }

        WriteEdidToResman(pDpyEvo, pEdid);

        nvFree(pEdid->buffer);
        pEdid->buffer = NULL;
        pEdid->length = 0;
    }

    if (!ReadEdidFromResman(pDpyEvo, pEdid, readMode)) {
        goto fail;
    }

validateEdid:
    /* Validate the EDID */
    if (!ValidateEdid(pDpyEvo, pEdid, pInfoString, ignoreEdidChecksum)) {
        goto fail;
    }

    *ppParsedEdid = nvCalloc(1, sizeof(**ppParsedEdid));
    if (*ppParsedEdid == NULL) {
        goto fail;
    }
    /* Parse the EDID.  Note this may *change* the EDID bytes. */
    PatchAndParseEdid(pDpyEvo, pEdid, *ppParsedEdid, pInfoString);

    return TRUE;

fail:

    /* We failed to read a valid EDID.  Free any EDID buffer allocated above. */
    nvFree(pEdid->buffer);
    pEdid->buffer = NULL;
    pEdid->length = 0;

    return FALSE;
}

static void AssignIsVrHmd(NVDpyEvoRec *pDpyEvo)
{
    NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS params = { };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    pDpyEvo->isVrHmd = FALSE;

    if (!pDpyEvo->parsedEdid.valid) {
        return;
    }

    params.manufacturerID = pDpyEvo->parsedEdid.info.manuf_id;
    params.productID = pDpyEvo->parsedEdid.info.product_id;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_IS_DIRECTMODE_DISPLAY,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to query VR headset for %s", pDpyEvo->name);
        return;
    }

    /*
     * bIsDirectmode indicates any monitor that by default shouldn't be part of
     * a desktop (VR headset, touch panel, etc).  But, close enough for our
     * usage of isVrHmd.
     */
    pDpyEvo->isVrHmd = params.bIsDirectmode;
}

static NvBool EdidHasChanged(
    const NVDpyEvoRec *pDpyEvo,
    const NVEdidRec *pEdid,
    const NVParsedEdidEvoRec *pParsedEdid)
{
    /* Compare EDID bytes */
    if (pEdid->length != pDpyEvo->edid.length ||
        nvkms_memcmp(pEdid->buffer, pDpyEvo->edid.buffer, pEdid->length) != 0) {
        return TRUE;
    }

    /* Compare parsed data */
    if (pParsedEdid != NULL) {
        if (nvkms_memcmp(pParsedEdid, &pDpyEvo->parsedEdid,
                         sizeof(*pParsedEdid)) != 0) {
            return TRUE;
        }
    } else if (pDpyEvo->parsedEdid.valid) {
        return TRUE;
    }

    return FALSE;
}

static void ApplyNewEdid(
    NVDpyEvoPtr pDpyEvo,
    const NVEdidRec *pEdid,
    const NVParsedEdidEvoRec *pParsedEdid,
    const NvBool bSendHdmiCapsToRm,
    NVEvoInfoStringPtr pInfoString)
{
    if (pDpyEvo->edid.buffer != NULL) {
        nvFree(pDpyEvo->edid.buffer);
    }
    pDpyEvo->edid.buffer = pEdid->buffer;
    pDpyEvo->edid.length = pEdid->length;

    if (pParsedEdid != NULL) {
        nvkms_memcpy(&pDpyEvo->parsedEdid, pParsedEdid,
                     sizeof(pDpyEvo->parsedEdid));
    } else {
        nvkms_memset(&pDpyEvo->parsedEdid, 0, sizeof(pDpyEvo->parsedEdid));
    }

    /*
     * Regenerate the dpy's name, because the parsed EDID monitorName
     * may have changed.
     */
    AssignDpyEvoName(pDpyEvo);

    /* Write information about the parsed EDID to the infoString. */
    LogEdid(pDpyEvo, pInfoString);

    if (pDpyEvo->parsedEdid.valid) {
        /*
         * check 3D Vision capability
         */
        nvDpyCheck3DVisionCapsEvo(pDpyEvo);

        /*
         * Check HDMI VRR capability
         */
        nvDpyUpdateHdmiVRRCaps(pDpyEvo);
    }

    if (pDpyEvo->pConnectorEvo->legacyType ==
        NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {
        DpyAssignColorSpaceCaps(pDpyEvo, pInfoString);
    }

    if (bSendHdmiCapsToRm) {
        nvSendHdmiCapsToRm(pDpyEvo);
    }

    nvDpyProbeMaxPixelClock(pDpyEvo);

    AssignIsVrHmd(pDpyEvo);
}

/*
 * ReadDSITimingsFromResman() - Obtains modetimings for a DSI connector,
 *                              passing it into pTimings
 */
static NvBool ReadDSITimingsFromResman(
    const NVDpyEvoRec *pDpyEvo,
    NVT_TIMING *pTimings,
    NvU8 *pBpc)
{
    NvU32 ret;
    NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS dsiModeTimingParams = { 0 };

    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    dsiModeTimingParams.subDeviceInstance = pDispEvo->displayOwner;

    /*
     * Currently displayId must be hardcoded to 0 to receive timings from RM.
     * Once the corresponding DCB support is added for DSI, this hack will be
     * removed and NVKMS will use the actual displayId instead.
     */
    dsiModeTimingParams.displayId = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING,
                         &dsiModeTimingParams, sizeof(dsiModeTimingParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Unable to read DSI mode timings for display device %s",
                     pDpyEvo->name);
        return FALSE;
    }

    // Converts refresh (Hz) into appropriate units for rr1k (units of 0.001Hz)
    pTimings->etc.rrx1k = dsiModeTimingParams.refresh * 1000;
    pTimings->HVisible = dsiModeTimingParams.hActive;
    pTimings->HFrontPorch = dsiModeTimingParams.hFrontPorch;
    pTimings->HSyncWidth = dsiModeTimingParams.hSyncWidth;
    pTimings->HTotal = dsiModeTimingParams.hActive +
                       dsiModeTimingParams.hFrontPorch +
                       dsiModeTimingParams.hSyncWidth +
                       dsiModeTimingParams.hBackPorch;

    pTimings->VVisible = dsiModeTimingParams.vActive;
    pTimings->VFrontPorch = dsiModeTimingParams.vFrontPorch;
    pTimings->VSyncWidth = dsiModeTimingParams.vSyncWidth;
    pTimings->VTotal = dsiModeTimingParams.vActive +
                       dsiModeTimingParams.vFrontPorch +
                       dsiModeTimingParams.vSyncWidth +
                       dsiModeTimingParams.vBackPorch;

    pTimings->pclk = HzToKHz(dsiModeTimingParams.pclkHz) / 10;

    // DSI only supports RGB444
    *pBpc = dsiModeTimingParams.bpp / 3;

    return TRUE;
}

static NvBool ParseSuperframeInfo(
    NVDpyEvoRec *pDpyEvo,
    const NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pParams,
    struct NvKmsSuperframeInfo *pSuperframeInfo)
{
    NvU8 i;

    if (pParams->superframeInfo.numViews == 0) {
        return TRUE;
    }

    // Currently, we support only dual view superframe.
    if (pParams->superframeInfo.numViews != 2) {
        nvEvoLog(EVO_LOG_ERROR, "Invalid number of superframe views");
        return FALSE;
    }

    // Currently, we support only packed symmetrical side-by-side superframe.
    if ((pParams->superframeInfo.view[0].width * pParams->superframeInfo.numViews) !=
        pParams->hActive) {
        nvEvoLog(EVO_LOG_ERROR, "The width of Superframe view[0] is invalid");
        return FALSE;
    }

    if (pParams->superframeInfo.view[0].height != pParams->vActive) {
        nvEvoLog(EVO_LOG_ERROR, "The height of Superframe view[0] is invalid");
        return FALSE;
    }

    pSuperframeInfo->numViews = 0;

    for (i = 0; i < pParams->superframeInfo.numViews; i++) {
        // All superframe views must not have horizontal spacing in between them.
        if ((pParams->superframeInfo.view[0].width * i) !=
            pParams->superframeInfo.view[i].x) {
            nvEvoLog(EVO_LOG_ERROR, "The x offset of Superframe view[%u] is invalid", i);
            goto fail;
        }

        // All superframe views must have y offset as 0.
        if (pParams->superframeInfo.view[i].y != 0) {
            nvEvoLog(EVO_LOG_ERROR, "The y offset of Superframe view[%u] is invalid", i);
            goto fail;
        }

        // All superframe views must have the same width.
        if (pParams->superframeInfo.view[0].width !=
            pParams->superframeInfo.view[i].width) {
            nvEvoLog(EVO_LOG_ERROR, "The width of Superframe view[%u] is invalid", i);
            goto fail;
        }

        // All superframe views must have the same height.
        if (pParams->superframeInfo.view[0].height !=
            pParams->superframeInfo.view[i].height) {
            nvEvoLog(EVO_LOG_ERROR, "The height of Superframe view[%u] is invalid", i);
            goto fail;
        }

        pSuperframeInfo->view[i].x = pParams->superframeInfo.view[i].x;
        pSuperframeInfo->view[i].width = pParams->superframeInfo.view[i].width;
        pSuperframeInfo->view[i].y = pParams->superframeInfo.view[i].y;
        pSuperframeInfo->view[i].height = pParams->superframeInfo.view[i].height;
        pSuperframeInfo->numViews++;
    }

    return TRUE;

fail:
    nvkms_memset(pSuperframeInfo, 0, sizeof(*pSuperframeInfo));
    return FALSE;
}

static NvBool ReadDPSerializerTimings(
    NVDpyEvoRec *pDpyEvo,
    NVT_TIMING *pTimings,
    NvU8 *pBpc,
    struct NvKmsSuperframeInfo *pSuperframeInfo)
{
    NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS timingParams = { };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    timingParams.subDeviceInstance = pDispEvo->displayOwner;
    timingParams.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);
    timingParams.stream = pDpyEvo->dp.serializerStreamIndex;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_GET_FIXED_MODE_TIMING,
                         &timingParams, sizeof(timingParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Unable to read fixed mode timings for display device %s",
                     pDpyEvo->name);
        return FALSE;
    }

    if (!timingParams.valid) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Fixed mode timings are invalid for display device %s",
                     pDpyEvo->name);
        return FALSE;
    }

    if (!ParseSuperframeInfo(pDpyEvo, &timingParams, pSuperframeInfo)) {
        return FALSE;
    }

    nvkms_memset(pTimings, 0, sizeof(NVT_TIMING));

    pTimings->HVisible = timingParams.hActive;
    pTimings->HFrontPorch = timingParams.hFrontPorch;
    pTimings->HSyncWidth = timingParams.hSyncWidth;
    pTimings->HTotal = timingParams.hActive + timingParams.hFrontPorch +
                       timingParams.hSyncWidth + timingParams.hBackPorch;

    pTimings->VVisible = timingParams.vActive;
    pTimings->VFrontPorch = timingParams.vFrontPorch;
    pTimings->VSyncWidth = timingParams.vSyncWidth;
    pTimings->VTotal = timingParams.vActive + timingParams.vFrontPorch +
                       timingParams.vSyncWidth + timingParams.vBackPorch;

    pTimings->pclk = timingParams.pclkKHz / 10;
    pTimings->etc.rrx1k = timingParams.rrx1k;

    *pBpc = 0;

    return TRUE;
}

static NvBool GetFixedModeTimings(
    NVDpyEvoPtr pDpyEvo,
    struct NvKmsSuperframeInfo *pSuperframeInfo)
{
    NVT_TIMING timings = { };
    NvBool ret = FALSE;
    NvU8 bpc;

    if (pDpyEvo->pConnectorEvo->signalFormat == NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) {
        ret = ReadDSITimingsFromResman(pDpyEvo, &timings, &bpc);
    } else if (nvConnectorIsDPSerializer(pDpyEvo->pConnectorEvo)) {
        ret = ReadDPSerializerTimings(pDpyEvo, &timings, &bpc,
                                      pSuperframeInfo);
    }

    if (!ret) {
        return ret;
    }

    CreateParsedEdidFromNVT_TIMING(&timings, bpc, &pDpyEvo->parsedEdid);

    AssignDpyEvoName(pDpyEvo);
    nvDpyProbeMaxPixelClock(pDpyEvo);

    return TRUE;
}

static void ReadAndApplyEdidEvo(
    NVDpyEvoPtr pDpyEvo,
    struct NvKmsQueryDpyDynamicDataParams *pParams)
{
    const struct NvKmsQueryDpyDynamicDataRequest *pRequest = NULL;
    NVEdidRec edid = {NULL, 0};
    NVParsedEdidEvoPtr pParsedEdid = NULL;
    NVEvoInfoStringRec infoString;
    NvBool readSuccess;

    if (pParams != NULL) {
        nvInitInfoString(&infoString, pParams->reply.edid.infoString,
                         sizeof(pParams->reply.edid.infoString));
        pRequest = &pParams->request;
    } else {
        nvInitInfoString(&infoString, NULL, 0);
    }

    readSuccess = nvDpyReadAndParseEdidEvo(pDpyEvo, pRequest,
                                           NVKMS_EDID_READ_MODE_DEFAULT,
                                           &edid, &pParsedEdid, &infoString);

    if (pParams != NULL) {
        pParams->reply.edid.valid = readSuccess;
    }

    if (EdidHasChanged(pDpyEvo, &edid, pParsedEdid)) {
        /*
         * Do not plumb pRequest into ApplyNewEdid().  This helps ensure that
         * its operation is purely a function of the EDID and parsed EDID data,
         * which means that if we get into this function again with the same
         * EDID and parsed EDID data, we can safely skip ApplyNewEdid() without
         * worrying that this request has different parameters (like CustomEdid
         * or mode validation overrides).
         */
        ApplyNewEdid(pDpyEvo, &edid, pParsedEdid, TRUE /* bSendHdmiCapsToRm */,
                     &infoString);
    } else {
        nvFree(edid.buffer);
    }
    nvFree(pParsedEdid);
}

/*!
 * Get the maximum allowed pixel clock for pDpyEvo.
 *
 * This depends on the following conditions:
 *
 * - The RM's returned value is sufficient for non-TMDS connectors
 * - For HDMI, the SOR capabilities exceed the RM's returned value to allow
 *   for HDMI 1.4 modes that exceed 165MHz on a single link, or
 *   for HDMI 2.1 modes if the source and sink is capable of FRL
 * - For DVI, the user is allowed to set an option to exceed the 165MHz
 *   per-TMDS limit if the SOR capabilities allow it
 * - Contrary to the above, passive DP->DVI and DP->HDMI dongles have their
 *   own limits
 */
void nvDpyProbeMaxPixelClock(NVDpyEvoPtr pDpyEvo)
{
    NvU32 ret;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    NvU32 displayOwner = pDispEvo->displayOwner;
    NVEvoPassiveDpDongleType passiveDpDongleType;
    NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS params = { 0 };
    NvU32 passiveDpDongleMaxPclkKHz;

    /* First, get the RM-reported value. */

    params.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);
    params.subDeviceInstance = pDispEvo->displayOwner;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_GET_PCLK_LIMIT,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failure reading maximum pixel clock value "
                     "for display device %s.", pDpyEvo->name);
        pDpyEvo->maxPixelClockKHz = 100000;
        pDpyEvo->maxSingleLinkPixelClockKHz = pDpyEvo->maxPixelClockKHz;
        return;
    }

    pDpyEvo->maxPixelClockKHz = params.orPclkLimit;
    pDpyEvo->maxSingleLinkPixelClockKHz = pDpyEvo->maxPixelClockKHz;

    /*
     * The RM's returned max pclk value is sufficient for non-TMDS
     * connectors
     */
    if (!IsConnectorTMDS(pConnectorEvo)) {
        return;
    }

    /*
     * The RM returns a 165MHz max pclk for single link TMDS and 330MHz
     * max pclk for dual link TMDS.  We can exceed that in the
     * following cases:
     *
     * - HDMI 1.4a 4Kx2K and 1080p60hz frame packed stereo modes
     *   require a 297MHz single TMDS link pixel clock, and HDMI 2.0
     *   allows an even higher pixel clock.
     * - While the DVI spec mandates a limit of 165MHz per TMDS link,
     *   since certain GPUs and certain displays support DVI
     *   connections at higher pixel clocks, we allow users to
     *   override this limit to allow validation of higher maximum
     *   pixel clocks over DVI.
     */
    if (pDevEvo->gpus != NULL) {

        NVEvoSorCaps *sorCaps = pDevEvo->gpus[displayOwner].capabilities.sor;
        NvU32 orIndex = pConnectorEvo->or.primary;

        if (NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED)) {
            /*
             * With the SOR crossbar, pConnectorEvo->or.mask is unknown,
             * and may change at modeset time.  Use the caps of SOR 0
             * for validation.
             */
            orIndex = 0;
        }

        if (nvDpyIsHdmiEvo(pDpyEvo)) {
            pDpyEvo->maxPixelClockKHz =
                pDpyEvo->maxSingleLinkPixelClockKHz =
                sorCaps[orIndex].maxTMDSClkKHz;

            nvkms_memset(&pDpyEvo->hdmi.srcCaps, 0, sizeof(pDpyEvo->hdmi.srcCaps));
            nvkms_memset(&pDpyEvo->hdmi.sinkCaps, 0, sizeof(pDpyEvo->hdmi.sinkCaps));

            if (nvHdmiDpySupportsFrl(pDpyEvo)) {
                /*
                 * An SOR needs to be assigned temporarily to do FRL training.
                 *
                 * Since the only other SORs in use at the moment (if any) are
                 * those driving heads, we don't need to exclude RM from
                 * selecting any SOR, so an sorExcludeMask of 0 is appropriate.
                 */
                if (nvAssignSOREvo(pConnectorEvo,
                                   nvDpyIdToNvU32(pConnectorEvo->displayId),
                                   FALSE /* b2Heads1Or */,
                                   0 /* sorExcludeMask */) &&
                    nvHdmiFrlAssessLink(pDpyEvo)) {
                    /*
                     * Note that although we "assessed" the link above, the
                     * maximum pixel clock set here doesn't take that into
                     * account -- it's the maximum the GPU hardware is capable
                     * of on the most capable link, mostly for reporting
                     * purposes.
                     *
                     * The calculation for if a given mode can fit in the
                     * assessed FRL configuration is complex and depends on
                     * things like the amount of blanking, rather than a simple
                     * pclk cutoff.  So, we query the hdmi library when
                     * validating each individual mode, when we know actual
                     * timings.
                     */

                    /*
                     * This comes from the Windows display driver: (4 lanes *
                     * 12Gb per lane * FRL encoding i.e 16/18) / 1K
                     */
                    pDpyEvo->maxPixelClockKHz =
                        ((4 * 12 * 1000 * 1000 * 16) / 18);
                }
            }
        } else {
            /*
             * Connector and SOR both must be capable to drive dual-TMDS
             * resolutions.
             */
            NvBool bDualTMDS = sorCaps[orIndex].dualTMDS &&
                               FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _LINK, _DUAL,
                               pDpyEvo->pConnectorEvo->dfpInfo);

            pDpyEvo->maxPixelClockKHz = (bDualTMDS ?
                                         TMDS_DUAL_LINK_PCLK_MAX :
                                         TMDS_SINGLE_LINK_PCLK_MAX);

            pDpyEvo->maxSingleLinkPixelClockKHz = TMDS_SINGLE_LINK_PCLK_MAX;

            if (pDpyEvo->allowDVISpecPClkOverride) {
                pDpyEvo->maxPixelClockKHz = sorCaps[orIndex].maxTMDSClkKHz *
                    (bDualTMDS ? 2 : 1);
                pDpyEvo->maxSingleLinkPixelClockKHz =
                    sorCaps[orIndex].maxTMDSClkKHz;
            }
        }
    }

    /*
     * Passive DP->DVI and DP->HDMI dongles may have a limit more
     * restrictive than the one described above.  Check whether one of
     * these dongles is in use, and override the limit accordingly.
     */
    passiveDpDongleType =
        nvDpyGetPassiveDpDongleType(pDpyEvo, &passiveDpDongleMaxPclkKHz);

    if (passiveDpDongleType != NV_EVO_PASSIVE_DP_DONGLE_UNUSED) {
        pDpyEvo->maxPixelClockKHz = NV_MIN(passiveDpDongleMaxPclkKHz,
                                           pDpyEvo->maxPixelClockKHz);
        pDpyEvo->maxSingleLinkPixelClockKHz = pDpyEvo->maxPixelClockKHz;
    }
}

static void DpyGetDynamicDfpProperties(
    NVDpyEvoPtr pDpyEvo,
    const NvBool disableACPIBrightnessHotkeys)
{
    if (disableACPIBrightnessHotkeys) {
        return;
    }
    if (!disableACPIBrightnessHotkeys) {
        struct NvKmsGetDpyAttributeParams params;
        nvkms_memset(&params, 0, sizeof(params));
        params.request.attribute = NV_KMS_DPY_ATTRIBUTE_BACKLIGHT_BRIGHTNESS;

        pDpyEvo->hasBacklightBrightness =
            nvGetDpyAttributeEvo(pDpyEvo, &params);
    }
}
/*
 * DpyGetDfpProperties() - get DFP properties: reduced blanking flags
 * and general DFP flags
 */

static void DpyGetStaticDfpProperties(NVDpyEvoPtr pDpyEvo)
{
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;

    if (pConnectorEvo->legacyType != NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {
        return;
    }

    if (nvDpyEvoIsDPMST(pDpyEvo)) {
        // None of this stuff can be queried directly for dynamic DP MST
        // displays.
        // XXX DP MST: Should we fill in these fields somehow anyway?
        return;
    }

    pDpyEvo->internal = FALSE;
    pDpyEvo->hdmiCapable = FALSE;

    if (pConnectorEvo->dfpInfo == 0x0) {
        return;
    }
    /* Check if the connected DFP is HDMI capable */

    if (FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _HDMI_CAPABLE, _TRUE,
                     pConnectorEvo->dfpInfo)) {
        pDpyEvo->hdmiCapable = TRUE;
    }

    pDpyEvo->internal = nvConnectorIsInternal(pDpyEvo->pConnectorEvo);
}

/*!
 * Return true if the connector is single or dual link TMDS (not CRT, not DP).
 */
static NvBool IsConnectorTMDS(NVConnectorEvoPtr pConnectorEvo)
{
    NvU32 protocol = pConnectorEvo->or.protocol;
    return ((pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) &&
            ((protocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A) ||
             (protocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B) ||
             (protocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS)));
}

/*!
 * Query RM for the passive Displayport dongle type; this can influence
 * the maximum pixel clock allowed on that display.
 */
NVEvoPassiveDpDongleType
nvDpyGetPassiveDpDongleType(const NVDpyEvoRec *pDpyEvo,
                            NvU32 *passiveDpDongleMaxPclkKHz)
{
    NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS params = { 0 };
    NvU32 ret;
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    NVEvoPassiveDpDongleType passiveDpDongleType =
        NV_EVO_PASSIVE_DP_DONGLE_UNUSED;

    // The rmcontrol below fails if we try querying the dongle info on
    // non-TMDS connectors.
    if (!IsConnectorTMDS(pConnectorEvo)) {
        return passiveDpDongleType;
    }

    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);
    params.subDeviceInstance = pDispEvo->displayOwner;
    params.flags = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_GET_DISPLAYPORT_DONGLE_INFO,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failure reading DP dongle info "
                     "for display device %s.", pDpyEvo->name);
        return passiveDpDongleType;
    }

    if (FLD_TEST_DRF(0073_CTRL_DFP,
                     _GET_DISPLAYPORT_DONGLE_INFO_FLAGS,
                     _ATTACHED, _TRUE, params.flags))
    {
        if (FLD_TEST_DRF(0073_CTRL_DFP,
                         _GET_DISPLAYPORT_DONGLE_INFO_FLAGS, _TYPE, _DP2DVI,
                         params.flags)) {

            passiveDpDongleType = NV_EVO_PASSIVE_DP_DONGLE_DP2DVI;

            if (passiveDpDongleMaxPclkKHz) {
                *passiveDpDongleMaxPclkKHz = TMDS_SINGLE_LINK_PCLK_MAX;
            }
        } else if (FLD_TEST_DRF(0073_CTRL_DFP,
                                _GET_DISPLAYPORT_DONGLE_INFO_FLAGS, _TYPE, _DP2HDMI,
                                params.flags)) {
            if (FLD_TEST_DRF(0073_CTRL_DFP,
                             _GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE, _TYPE, _1,
                             params.flags)) {

                passiveDpDongleType = NV_EVO_PASSIVE_DP_DONGLE_DP2HDMI_TYPE_1;

                if (passiveDpDongleMaxPclkKHz) {
                    *passiveDpDongleMaxPclkKHz = params.maxTmdsClkRateHz / 1000;
                }
            } else if (FLD_TEST_DRF(0073_CTRL_DFP,
                                    _GET_DISPLAYPORT_DONGLE_INFO_FLAGS_DP2TMDS_DONGLE, _TYPE, _2,
                                    params.flags)) {

                passiveDpDongleType = NV_EVO_PASSIVE_DP_DONGLE_DP2HDMI_TYPE_2;

                if (passiveDpDongleMaxPclkKHz) {
                    *passiveDpDongleMaxPclkKHz = params.maxTmdsClkRateHz / 1000;
                }
            }
            // For other dongle types: LFH_DVI (DMS59-DVI) and LFH_VGA (DMS59-VGA) breakout dongles,
            // We consider them as native connection, hence we don't track passiveDpDongleType here
        }
    }

    return passiveDpDongleType;
}


/*!
 * Validate an NVKMS client-specified NvKmsModeValidationFrequencyRanges.
 */
static NvBool ValidateFrequencyRanges(
    const struct NvKmsModeValidationFrequencyRanges *pRanges)
{
    NvU32 i;

    if (pRanges->numRanges >= ARRAY_LEN(pRanges->range)) {
        return FALSE;
    }

    for (i = 0; i < pRanges->numRanges; i++) {
        if (pRanges->range[i].high < pRanges->range[i].low) {
            return FALSE;
        }
        if (pRanges->range[i].high == 0) {
            return FALSE;
        }
    }

    return TRUE;
}


static void DpySetValidSyncsHelper(
    struct NvKmsModeValidationFrequencyRanges *pRanges,
    const NVParsedEdidEvoRec *pParsedEdid,
    NvBool isHorizSync, NvBool ignoreEdidSource)
{
    NvBool found = FALSE;
    NvU32 edidMin = 0, edidMax = 0;

    if (pParsedEdid->valid) {
        if (isHorizSync) {
            edidMin = pParsedEdid->limits.min_h_rate_hz;
            edidMax = pParsedEdid->limits.max_h_rate_hz;
        } else {
            edidMin = pParsedEdid->limits.min_v_rate_hzx1k;
            edidMax = pParsedEdid->limits.max_v_rate_hzx1k;
        }
    }

    /* If the client-specified ranges are invalid, clear them. */

    if ((pRanges->source ==
         NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_BEFORE_EDID) ||
        (pRanges->source ==
         NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_AFTER_EDID)) {

        if (!ValidateFrequencyRanges(pRanges)) {
            nvkms_memset(pRanges, 0, sizeof(*pRanges));
            pRanges->source = NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_NONE;
        }
    }

    /* Use CLIENT_BEFORE_EDID, if provided. */

    if (pRanges->source ==
        NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_BEFORE_EDID) {
        found = TRUE;
    }

    /*
     * Otherwise, if EDID-reported sync ranges are available, use
     * those.
     */
    if (!found &&
        !ignoreEdidSource &&
        (edidMin != 0) && (edidMax != 0)) {

        pRanges->numRanges = 1;
        pRanges->range[0].low = edidMin;
        pRanges->range[0].high = edidMax;
        pRanges->source = NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_EDID;
        found = TRUE;
    }

    /*
     * Otherwise, use CLIENT_AFTER_EDID, if available.
     */
    if (!found &&
        (pRanges->source ==
         NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_AFTER_EDID)) {
        found = TRUE;
    }

    /*
     * Finally, fall back to conservative defaults if we could not
     * find anything else; this will validate 1024x768 @ 60Hz.
     */
    if (!found) {

        pRanges->numRanges = 1;

        if (isHorizSync) {
            pRanges->range[0].low = 28000;
            pRanges->range[0].high = 55000;
        } else {
            pRanges->range[0].low = 43000;
            pRanges->range[0].high = 72000;
        }

        pRanges->source =
            NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CONSERVATIVE_DEFAULTS;
    }
}


/*!
 * Assign NvKmsModeValidationValidSyncs
 *
 * Assign the HorizSync and VertRefresh ranges in
 * NvKmsModeValidationValidSyncs.  The priority order is:
 *
 * (1) Any HorizSync and VertRefresh provided by the client that
 *     overrides the EDID (CLIENT_BEFORE_EDID).
 * (2) Valid range information from the EDID.
 * (3) Any HorizSync and VertRefresh specified by the client as a
 *     fallback for the EDID (CLIENT_AFTER_EDID).
 * (4) Conservative builtin defaults.
 *
 * HorizSync and VertRefresh can come from different sources.  (1) and
 * (3) are provided through pValidSyncs.  (2) and (4) get written to
 * pValidSyncs.
 *
 * \param[in]      pDpy         The dpy whose EDID will be used.
 * \param[in,out]  pValidSyncs  This is initialized by the client, and
 *                              will be updated based on the frequency
 *                              range priority described above.
 */
void nvDpySetValidSyncsEvo(const NVDpyEvoRec *pDpyEvo,
                           struct NvKmsModeValidationValidSyncs *pValidSyncs)
{
    const NVParsedEdidEvoRec *pParsedEdid = &pDpyEvo->parsedEdid;

    DpySetValidSyncsHelper(&pValidSyncs->horizSyncHz,
                           pParsedEdid,
                           TRUE, /* isHorizSync */
                           pValidSyncs->ignoreEdidSource);

    DpySetValidSyncsHelper(&pValidSyncs->vertRefreshHz1k,
                           pParsedEdid,
                           FALSE, /* isHorizSync */
                           pValidSyncs->ignoreEdidSource);
}


/*
 * ReadEdidFromDP() - query the EDID for the specified display device from the
 * DP lib.
 */

static NvBool ReadEdidFromDP(const NVDpyEvoRec *pDpyEvo, NVEdidPtr pEdid)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NvU8 *pNewEdid = NULL;
    int newEdidLength;

    if (!nvDpyUsesDPLib(pDpyEvo)) {
        return FALSE;
    }

    /* get size and allocate space for the EDID data */
    newEdidLength = nvDPGetEDIDSize(pDpyEvo);
    if (newEdidLength == 0) {
        goto fail;
    }

    pNewEdid = nvCalloc(newEdidLength, 1);

    if (pNewEdid == NULL) {
        goto fail;
    }

    if (!nvDPGetEDID(pDpyEvo, pNewEdid, newEdidLength)) {
        goto fail;
    }

    pEdid->buffer = pNewEdid;
    pEdid->length = newEdidLength;

    return TRUE;

 fail:

    nvFree(pNewEdid);

    nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                 "Unable to read EDID for display device %s",
                 pDpyEvo->name);
    return FALSE;

} // ReadEdidFromDP()



/*
 * ReadEdidFromResman() - query the EDID for the specified display device
 */

static NvBool ReadEdidFromResman(const NVDpyEvoRec *pDpyEvo, NVEdidPtr pEdid,
                                 NvKmsEdidReadMode readMode)
{
    NvU32 ret;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *getEdidParams;
    int retryEdidReadCount = 0;
    NvBool success = FALSE;

    if (nvDpyEvoIsDPMST(pDpyEvo)) {
        // RM doesn't track this device, so leave the EDID alone.
        return TRUE;
    }

    getEdidParams = nvCalloc(sizeof(*getEdidParams), 1);
    if (getEdidParams == NULL) {
        goto done;
    }

 query_edid:

    getEdidParams->subDeviceInstance = pDispEvo->displayOwner;
    getEdidParams->displayId = nvDpyEvoGetConnectorId(pDpyEvo);
    getEdidParams->flags = NV0073_CTRL_SPECIFIC_GET_EDID_FLAGS_COPY_CACHE_NO;

    if (readMode == NVKMS_EDID_READ_MODE_ACPI) {
        getEdidParams->flags |= DRF_DEF(0073_CTRL_SPECIFIC, _GET_EDID_FLAGS,
            _DISPMUX_READ_MODE, _ACPI);
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_GET_EDID_V2,
                         getEdidParams, sizeof(*getEdidParams));

    if ((ret != NVOS_STATUS_SUCCESS) || (getEdidParams->bufferSize <= 0)) {
        /* WAR for Bug 777646: retry reading the EDID on error for DP
         * devices to avoid possible TDR assertion in the RM.
         *
         * XXX This should be moved to the DP library.
         */
        if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
            (retryEdidReadCount < NV_DP_READ_EDID_RETRIES)) {
            retryEdidReadCount++;

            nvkms_usleep(NV_DP_REREAD_EDID_DELAY_USEC);

            goto query_edid;
        }
        goto done;
    }

    pEdid->buffer = nvCalloc(getEdidParams->bufferSize, 1);

    if (pEdid->buffer == NULL) {
        goto done;
    }

    nvkms_memcpy(pEdid->buffer, &getEdidParams->edidBuffer,
                 getEdidParams->bufferSize);
    pEdid->length = getEdidParams->bufferSize;

    success = TRUE;

 done:

    nvFree(getEdidParams);

    if (!success) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Unable to read EDID for display device %s",
                     pDpyEvo->name);
    }

    return success;
} // ReadEdidFromResman()


/*
 * Check if the EDID meets basic validation criteria.
 */
static NvBool ValidateEdid(const NVDpyEvoRec *pDpyEvo, NVEdidPtr pEdid,
                           NVEvoInfoStringPtr pInfoString,
                           const NvBool ignoreEdidChecksum)
{
    NvU32 status, tmpStatus;

    status = NvTiming_EDIDValidationMask(pEdid->buffer, pEdid->length, TRUE);
    tmpStatus = status;

    if (status == 0) {
        return TRUE;
    }

    nvEvoLogInfoString(pInfoString,
                       "The EDID read for display device %s is invalid:",
                       pDpyEvo->name);

    /*
     * Warn about every error we know about, masking it out of tmpStatus, then
     * warn about an unknown error if there are still any bits remaining in
     * tmpStatus.
     */
    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_VERSION)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has an unrecognized version.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_VERSION);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_SIZE)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID is too short.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_SIZE);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM)) {
        /*
         * XXX NVKMS TODO: massage wording to not reference X
         * configuration option.
         */
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has a bad checksum. %s",
                     ignoreEdidChecksum ? "This error will be ignored. Note "
                     "that an EDID with a bad checksum could indicate a "
                     "corrupt EDID. A corrupt EDID may have mode timings "
                     "beyond the capabilities of your display, and could "
                     "damage your hardware. Please use with care." :
                     "The \"IgnoreEDIDChecksum\" X configuration option may "
                     "be used to attempt using mode timings in this EDID in "
                     "spite of this error. A corrupt EDID may have mode "
                     "timings beyond the capabilities of your display, and "
                     "could damage your hardware. Please use with care.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_RANGE_LIMIT)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has a bad range limit.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_RANGE_LIMIT);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_DTD)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has a bad detailed timing descriptor.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_DTD);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT_DTD)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has an extension block with a bad detailed "
                     "timing descriptor.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT_DTD);
    }

    if (status &
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT)) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID extension block is invalid.");
        tmpStatus &= ~NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT);
    }

    if (tmpStatus) {
        nvEvoLogInfoString(pInfoString,
                     "- The EDID has an unrecognized error.");
    }

    /*
     * Unset the bits for errors we don't care about (invalid DTDs in the
     * extension block, or checksum errors if ignoreEdidChecksum is in use)
     * then return true if there are any remaining errors we do care about.
     */
    if (ignoreEdidChecksum) {
        status &= ~(NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_CHECKSUM));
    }

    if (status ==
        NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT_DTD)) {
        /*
         * If the only problem with the EDID is invalid DTDs in the extension
         * block, don't reject the EDID; those timings can be safely skipped in
         * NvTiming_ParseEDIDInfo()/parse861ExtDetailedTiming()
         */
        nvEvoLogInfoString(pInfoString,
                           "This bad detailed timing descriptor will be ignored.");
    }

    status &= ~(NVT_EDID_VALIDATION_ERR_MASK(NVT_EDID_VALIDATION_ERR_EXT_DTD));

    return (status == 0);
}

static const char *GetColorDepthBpc(NVT_COLORDEPTH colorDepth)
{
    static char buffer[32];
    NVEvoInfoStringRec infoString;
    NvBool first = TRUE;
    int i;

    struct {
        NvBool val;
        int bpc;
    } table[] = {
        { colorDepth.bpc.bpc6,  6  },
        { colorDepth.bpc.bpc8,  8  },
        { colorDepth.bpc.bpc10, 10 },
        { colorDepth.bpc.bpc12, 12 },
        { colorDepth.bpc.bpc14, 14 },
        { colorDepth.bpc.bpc16, 16 },
    };

    nvInitInfoString(&infoString, buffer, sizeof(buffer));

    buffer[0] = '\0';

    for (i = 0; i < ARRAY_LEN(table); i++) {
        if (table[i].val) {
            nvEvoLogInfoStringRaw(&infoString, "%s%d",
                                  first ? "" : ", ",
                                  table[i].bpc);
            first = FALSE;
        }
    }

    return buffer;
}


/*
 * Log information about the EDID.
 */

static void LogEdid(NVDpyEvoPtr pDpyEvo, NVEvoInfoStringPtr pInfoString)
{
    int k;
    NVParsedEdidEvoPtr pParsedEdid;

    static const struct {
        NVT_TIMING_TYPE type;
        const char *name;
    } mode_type_table[] = {
        { NVT_TYPE_DMT,              "Display Monitor Timings" },
        { NVT_TYPE_GTF,              "Generalized Timing Formula Timings" },
        { NVT_TYPE_ASPR,             "ASPR Timings"},
        { NVT_TYPE_NTSC_TV,          "NTSC Timings" },
        { NVT_TYPE_PAL_TV,           "PAL Timings" },
        { NVT_TYPE_CVT,              "Coordinated Video Timings"},
        { NVT_TYPE_CVT_RB,      "Reduced Blanking Coordinated Video Timings" },
        { NVT_TYPE_CUST,             "Customized Timings" },
        { NVT_TYPE_EDID_STD,         "Standard Timings" },
        { NVT_TYPE_EDID_DTD,         "Detailed Timings" },
        { NVT_TYPE_EDID_CVT,         "Coordinated Video Timings" },
        { NVT_TYPE_EDID_EST,         "Established Timings"},
        { NVT_TYPE_EDID_861ST,       "CEA-861B Timings" },
        { NVT_TYPE_NV_PREDEFINED,    "Predefined Timings" },
        { NVT_TYPE_DMT_RB,        "Reduced Blanking Display Monitor Timings" },
        { NVT_TYPE_EDID_EXT_DTD,     "Extension Block Detailed Timings" },
        { NVT_TYPE_SDTV,             "SDTV Timings "},
        { NVT_TYPE_HDTV,             "HDTV Timings" },
        { NVT_TYPE_SMPTE,            "SMPTE Timings" },
        { NVT_TYPE_EDID_VTB_EXT,     "VTB Extension Timings" },
        { NVT_TYPE_EDID_VTB_EXT_STD, "VTB Extension Detailed Timings" },
        { NVT_TYPE_EDID_VTB_EXT_DTD, "VTB Extension Standard Timings" },
        { NVT_TYPE_EDID_VTB_EXT_CVT, "VTB Extension CVT Timings" },
        { NVT_TYPE_HDMI_STEREO,      "HDMI Stereo Timings" },
        { NVT_TYPE_DISPLAYID_1,      "DisplayID Type 1 Timings" },
        { NVT_TYPE_DISPLAYID_2,      "DisplayID Type 2 Timings" },
        { NVT_TYPE_HDMI_EXT,         "HDMI Extended Resolution Timings" },
        { NVT_TYPE_CUST_AUTO,        "Customized Auto Timings" },
        { NVT_TYPE_CUST_MANUAL,      "Customized Manual Timings" },
        { NVT_TYPE_CVT_RB_2,"Reduced Blanking Coordinated Video Timings, v2" },
        { NVT_TYPE_DMT_RB_2,         "Display Monitor Timings, V2" },
        { NVT_TYPE_DISPLAYID_7,      "DisplayID Type 7 Timings" },
        { NVT_TYPE_DISPLAYID_8,      "DisplayID Type 8 Timings" },
        { NVT_TYPE_DISPLAYID_9,      "DisplayID Type 9 Timings" },
        { NVT_TYPE_DISPLAYID_10,     "DisplayID Type 10 Timings" },
        { NVT_TYPE_CVT_RB_3,         "Reduced Blanking Coordinated Video Timings, v3" },
    };

    /*
     * Trigger a warning if new NVT_TIMING_TYPE values are added
     * without updating this function.
     *
     * If a warning is produced about unhandled enum in the below
     * switch statement, please update both the switch statement and
     * mode_type_table[], or contact the sw-nvkms email alias.
     */
    if (pDpyEvo->parsedEdid.valid) {
        for (k = 0; k < pDpyEvo->parsedEdid.info.total_timings; k++) {
            NvU32 status = pDpyEvo->parsedEdid.info.timing[k].etc.status;
            NVT_TIMING_TYPE type = NVT_GET_TIMING_STATUS_TYPE(status);

            switch (type) {
                case NVT_TYPE_DMT:
                case NVT_TYPE_GTF:
                case NVT_TYPE_ASPR:
                case NVT_TYPE_NTSC_TV:
                case NVT_TYPE_PAL_TV:
                case NVT_TYPE_CVT:
                case NVT_TYPE_CVT_RB:
                case NVT_TYPE_CUST:
                case NVT_TYPE_EDID_DTD:
                case NVT_TYPE_EDID_STD:
                case NVT_TYPE_EDID_EST:
                case NVT_TYPE_EDID_CVT:
                case NVT_TYPE_EDID_861ST:
                case NVT_TYPE_NV_PREDEFINED:
                case NVT_TYPE_DMT_RB:
                case NVT_TYPE_EDID_EXT_DTD:
                case NVT_TYPE_SDTV:
                case NVT_TYPE_HDTV:
                case NVT_TYPE_SMPTE:
                case NVT_TYPE_EDID_VTB_EXT:
                case NVT_TYPE_EDID_VTB_EXT_STD:
                case NVT_TYPE_EDID_VTB_EXT_DTD:
                case NVT_TYPE_EDID_VTB_EXT_CVT:
                case NVT_TYPE_HDMI_STEREO:
                case NVT_TYPE_DISPLAYID_1:
                case NVT_TYPE_DISPLAYID_2:
                case NVT_TYPE_HDMI_EXT:
                case NVT_TYPE_CUST_AUTO:
                case NVT_TYPE_CUST_MANUAL:
                case NVT_TYPE_CVT_RB_2:
                case NVT_TYPE_DMT_RB_2:
                case NVT_TYPE_DISPLAYID_7:
                case NVT_TYPE_DISPLAYID_8:
                case NVT_TYPE_DISPLAYID_9:
                case NVT_TYPE_DISPLAYID_10:
                case NVT_TYPE_CVT_RB_3:
                    /*
                     * XXX temporarily disable the warning so that additional
                     * NVT_TYPEs_ can be added to nvtiming.h.  Bug 3849339.
                     */
                default:
                    break;
            }
            break;
        }
    }

    nvEvoLogInfoString(pInfoString, "");
    nvEvoLogInfoString(pInfoString,
                       "--- EDID for %s ---", pDpyEvo->name);

    if (!pDpyEvo->parsedEdid.valid) {
        nvEvoLogInfoString(pInfoString, "");
        nvEvoLogInfoString(pInfoString, "No EDID Available.");
        nvEvoLogInfoString(pInfoString, "");
        goto done;
    }

    pParsedEdid = &pDpyEvo->parsedEdid;

    nvEvoLogInfoString(pInfoString,
                       "EDID Version                 : %d.%d",
                       pParsedEdid->info.version >> 8,
                       pParsedEdid->info.version & 0xff);

    nvEvoLogInfoString(pInfoString,
                       "Manufacturer                 : %s",
                       pParsedEdid->info.manuf_name);

    nvEvoLogInfoString(pInfoString,
                       "Monitor Name                 : %s",
                       pParsedEdid->monitorName);

    nvEvoLogInfoString(pInfoString,
                       "Product ID                   : 0x%04x",
                       pParsedEdid->info.product_id);

    nvEvoLogInfoString(pInfoString,
                       "32-bit Serial Number         : 0x%08x",
                       pParsedEdid->info.serial_number);

    nvEvoLogInfoString(pInfoString,
                       "Serial Number String         : %s",
                       pParsedEdid->serialNumberString);

    nvEvoLogInfoString(pInfoString,
                       "Manufacture Date             : %d, week %d",
                       pParsedEdid->info.year,
                       pParsedEdid->info.week);

    /*
     * despite the name feature_ver_1_3, the below features are
     * reported on all EDID versions
     */
    nvEvoLogInfoString(pInfoString,
                       "DPMS Capabilities            :%s%s%s",
                       pParsedEdid->info.u.feature_ver_1_3.support_standby ?
                       " Standby" : "",
                       pParsedEdid->info.u.feature_ver_1_3.support_suspend ?
                       " Suspend" : "",
                       pParsedEdid->info.u.feature_ver_1_3.support_active_off ?
                       " Active Off" : "");

    nvEvoLogInfoString(pInfoString,
                       "Input Type                   : %s",
                       pParsedEdid->info.input.isDigital ?
                       "Digital" : "Analog");

    nvEvoLogInfoString(pInfoString,
                       "Prefer first detailed timing : %s",
                       pParsedEdid->info.u.feature_ver_1_3.preferred_timing_is_native ?
                       "Yes" : "No");

    if (pParsedEdid->info.version == NVT_EDID_VER_1_3) {
        nvEvoLogInfoString(pInfoString,
                           "Supports GTF                 : %s",
                           pParsedEdid->info.u.feature_ver_1_3.support_gtf ?
                           "Yes" : "No");
    }

    if (pParsedEdid->info.version >= NVT_EDID_VER_1_4) {
        NvBool continuousFrequency = FALSE;
        if (pParsedEdid->info.input.isDigital) {
            continuousFrequency =
                pParsedEdid->info.u.feature_ver_1_4_digital.continuous_frequency;
        } else {
            continuousFrequency =
                pParsedEdid->info.u.feature_ver_1_4_analog.continuous_frequency;
        }

        nvEvoLogInfoString(pInfoString,
                           "Supports Continuous Frequency: %s",
                           continuousFrequency ? "Yes" : "No");

        nvEvoLogInfoString(pInfoString,
                           "EDID 1.4 YCbCr 422 support   : %s",
                           pParsedEdid->info.u.feature_ver_1_4_digital.support_ycrcb_422
                           ? "Yes" : "No");

        nvEvoLogInfoString(pInfoString,
                           "EDID 1.4 YCbCr 444 support   : %s",
                           pParsedEdid->info.u.feature_ver_1_4_digital.support_ycrcb_444
                           ? "Yes" : "No");
    }

    nvEvoLogInfoString(pInfoString,
                       "Maximum Image Size           : %d mm x %d mm",
                       pParsedEdid->info.screen_size_x * 10, /* screen_size_* is in cm */
                       pParsedEdid->info.screen_size_y * 10);

    nvEvoLogInfoString(pInfoString,
                       "Valid HSync Range            : "
                       NV_FMT_DIV_1000_POINT_1
                       " kHz - " NV_FMT_DIV_1000_POINT_1 " kHz",
                       NV_VA_DIV_1000_POINT_1(pParsedEdid->limits.min_h_rate_hz),
                       NV_VA_DIV_1000_POINT_1(pParsedEdid->limits.max_h_rate_hz));

    nvEvoLogInfoString(pInfoString,
                       "Valid VRefresh Range         : "
                       NV_FMT_DIV_1000_POINT_1 " Hz - "
                       NV_FMT_DIV_1000_POINT_1 " Hz",
                       NV_VA_DIV_1000_POINT_1(pParsedEdid->limits.min_v_rate_hzx1k),
                       NV_VA_DIV_1000_POINT_1(pParsedEdid->limits.max_v_rate_hzx1k));

    nvEvoLogInfoString(pInfoString,
                       "EDID maximum pixel clock     : "
                       NV_FMT_DIV_1000_POINT_1 " MHz",
                       NV_VA_DIV_1000_POINT_1(pParsedEdid->limits.max_pclk_10khz * 10));

    if (pParsedEdid->info.nvdaVsdbInfo.valid) {
        nvEvoLogInfoString(pInfoString,
                           "G-Sync capable               : %s",
                           pParsedEdid->info.nvdaVsdbInfo.vrrData.v1.supportsVrr
                           ? "Yes" : "No");
        nvEvoLogInfoString(pInfoString,
                           "G-Sync minimum refresh rate  : %d Hz",
                           pParsedEdid->info.nvdaVsdbInfo.vrrData.v1.minRefreshRate);
    }

    if (pParsedEdid->info.ext_displayid.version) {
        nvEvoLogInfoString(pInfoString,
                           "DisplayID vfreq_min          : %d Hz",
                           pParsedEdid->info.ext_displayid.range_limits[0].vfreq_min);
    }

    if (pParsedEdid->info.ext_displayid20.version &&
        pParsedEdid->info.ext_displayid20.range_limits.seamless_dynamic_video_timing_change) {
        nvEvoLogInfoString(pInfoString,
                           "DisplayID 2.0 vfreq_min      : %d Hz",
                           pParsedEdid->info.ext_displayid20.range_limits.vfreq_min);
    }

    if (pParsedEdid->info.ext_displayid20.version &&
        pParsedEdid->info.ext_displayid20.total_adaptive_sync_descriptor != 0) {
        for (k = 0;
             k < pParsedEdid->info.ext_displayid20.total_adaptive_sync_descriptor &&
             k < ARRAY_LEN(pParsedEdid->info.ext_displayid20.adaptive_sync_descriptor);
             k++) {
            nvEvoLogInfoString(pInfoString,
                               "DisplayID 2.0 adaptive sync  : %d Hz (max), %d Hz (min)",
                               pParsedEdid->info.ext_displayid20.adaptive_sync_descriptor[k].max_rr,
                               pParsedEdid->info.ext_displayid20.adaptive_sync_descriptor[k].min_rr);
        }
    }

    for (k = 0; k < ARRAY_LEN(pParsedEdid->info.ldd); k++) {
        if (pParsedEdid->info.ldd[k].tag == NVT_EDID_DISPLAY_DESCRIPTOR_DRL) {
            nvEvoLogInfoString(pInfoString,
                               "min_v_rate                   : %d Hz",
                               pParsedEdid->info.ldd[k].u.range_limit.min_v_rate);
        }
    }

    if (pParsedEdid->info.hdmiForumInfo.vrr_min != 0) {
        nvEvoLogInfoString(pInfoString,
                           "HDMI Forum vrr_min           : %d Hz",
                           pParsedEdid->info.hdmiForumInfo.vrr_min);
    }

    nvLogEdidCea861InfoEvo(pDpyEvo, pInfoString);

    if (pParsedEdid->info.input.isDigital &&
        pParsedEdid->info.version >= NVT_EDID_VER_1_4) {
        nvEvoLogInfoString(pInfoString,
                           "EDID bits per component      : %d",
                           pParsedEdid->info.input.u.digital.bpc);
    }

    /* print the tiled display extension block, if present */
    if (pParsedEdid->info.ext_displayid.tile_topology_id.vendor_id) {
        const NVT_DISPLAYID_INFO *tile = &pParsedEdid->info.ext_displayid;
        const char *tmp;

        nvEvoLogInfoString(pInfoString,
                           "Tiled display information    :");
        nvEvoLogInfoString(pInfoString,
                           "  Revision                   : %d",
                           tile->tiled_display_revision);
        nvEvoLogInfoString(pInfoString,
                           "  Single Enclosure           : %s",
                           tile->tile_capability.bSingleEnclosure ?
                           "Yes" : "No");

        tmp = "Unknown";
        switch (tile->tile_capability.multi_tile_behavior) {
            case NVT_MULTI_TILE_BEHAVIOR_OTHER:
                tmp = "Other";
                break;
            case NVT_MULTI_TILE_BEHAVIOR_SOURCE_DRIVEN:
                tmp = "Source-driven";
                break;
        }
        nvEvoLogInfoString(pInfoString,
                           "  Multi-tile Behavior        : %s", tmp);

        tmp = "Unknown";
        switch (tile->tile_capability.single_tile_behavior) {
            case NVT_SINGLE_TILE_BEHAVIOR_OTHER:
                tmp = "Other";
                break;
            case NVT_SINGLE_TILE_BEHAVIOR_SOURCE_DRIVEN:
                tmp = "Source-driven";
                break;
            case NVT_SINGLE_TILE_BEHAVIOR_SCALE:
                tmp = "Scale";
                break;
            case NVT_SINGLE_TILE_BEHAVIOR_CLONE:
                tmp = "Clone";
                break;
        }
        nvEvoLogInfoString(pInfoString,
                           "  Single-tile Behavior       : %s", tmp);
        nvEvoLogInfoString(pInfoString,
                           "  Topology                   : %d row%s, %d column%s",
                           tile->tile_topology.row,
                           (tile->tile_topology.row == 1) ? "" : "s",
                           tile->tile_topology.col,
                           (tile->tile_topology.col == 1) ? "" : "s");
        nvEvoLogInfoString(pInfoString,
                           "  Location                   : (%d,%d)",
                           tile->tile_location.x, tile->tile_location.y);
        nvEvoLogInfoString(pInfoString,
                           "  Native Resolution          : %dx%d",
                           tile->native_resolution.width,
                           tile->native_resolution.height);
        if (tile->tile_capability.bHasBezelInfo) {
            nvEvoLogInfoString(pInfoString,
                               "  Bezel Information          :");
            nvEvoLogInfoString(pInfoString,
                               "    Pixel Density            : %d",
                               tile->bezel_info.pixel_density);
            nvEvoLogInfoString(pInfoString,
                               "    Top                      : %d",
                               tile->bezel_info.top);
            nvEvoLogInfoString(pInfoString,
                               "    Bottom                   : %d",
                               tile->bezel_info.bottom);
            nvEvoLogInfoString(pInfoString,
                               "    Left                     : %d",
                               tile->bezel_info.right);
            nvEvoLogInfoString(pInfoString,
                               "    Right                    : %d",
                               tile->bezel_info.left);
        }
        nvEvoLogInfoString(pInfoString,
                           "  Vendor ID                  : 0x%x",
                           tile->tile_topology_id.vendor_id);
        nvEvoLogInfoString(pInfoString,
                           "  Product ID                 : 0x%x",
                           tile->tile_topology_id.product_id);
        nvEvoLogInfoString(pInfoString,
                           "  Serial Number              : 0x%x",
                           tile->tile_topology_id.serial_number);
    }

    for (k = 0; k < ARRAY_LEN(mode_type_table); k++) {

        int i;

        /* scan through the ModeList to find a mode of the current type */

        for (i = 0; i < pParsedEdid->info.total_timings; i++) {
            NVT_TIMING *pTiming = &pParsedEdid->info.timing[i];
            if (mode_type_table[k].type ==
                NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status)) {
                break;
            }
        }

        /* if there are none of this type, skip to the next mode type */

        if (i == pParsedEdid->info.total_timings) {
            continue;
        }

        nvEvoLogInfoString(pInfoString, "");
        nvEvoLogInfoString(pInfoString, "%s:", mode_type_table[k].name);

        for (i = 0; i < pParsedEdid->info.total_timings; i++) {

            NVT_TIMING *pTiming = &pParsedEdid->info.timing[i];
            NVT_TIMING_TYPE type =
                NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status);
            int vScale = 1;

            if (mode_type_table[k].type != type) {
                continue;
            }

            if ((type == NVT_TYPE_EDID_EST) ||
                (type == NVT_TYPE_EDID_STD)) {

                nvEvoLogInfoString(pInfoString,
                                   "  %-4d x %-4d @ %d Hz",
                                   NV_NVT_TIMING_HVISIBLE(pTiming),
                                   NV_NVT_TIMING_VVISIBLE(pTiming),
                                   pTiming->etc.rr);
                continue;
            }

            if (pTiming->interlaced) {
                vScale = 2;
            }

            nvEvoLogInfoString(pInfoString,
                               "  %-4d x %-4d @ %d Hz",
                               NV_NVT_TIMING_HVISIBLE(pTiming),
                               NV_NVT_TIMING_VVISIBLE(pTiming),
                               pTiming->etc.rr);

            nvEvoLogInfoString(pInfoString,
                               "    Pixel Clock      : "
                               NV_FMT_DIV_1000_POINT_2 " MHz",
                               NV_VA_DIV_1000_POINT_2(pTiming->pclk
                                                              * 10));

            nvEvoLogInfoString(pInfoString,
                               "    HRes, HSyncStart : %d, %d",
                               pTiming->HVisible,
                               pTiming->HVisible +
                               pTiming->HFrontPorch);

            nvEvoLogInfoString(pInfoString,
                               "    HSyncEnd, HTotal : %d, %d",
                               pTiming->HVisible +
                               pTiming->HFrontPorch +
                               pTiming->HSyncWidth,
                               pTiming->HTotal);

            nvEvoLogInfoString(pInfoString,
                               "    VRes, VSyncStart : %d, %d",
                               pTiming->VVisible * vScale,
                               (pTiming->VVisible +
                                pTiming->VFrontPorch) * vScale);

            nvEvoLogInfoString(pInfoString,
                               "    VSyncEnd, VTotal : %d, %d",
                               (pTiming->VVisible +
                                pTiming->VFrontPorch +
                                pTiming->VSyncWidth) * vScale,
                               pTiming->VTotal * vScale);

            nvEvoLogInfoString(pInfoString,
                               "    H/V Polarity     : %s/%s",
                               (pTiming->HSyncPol == NVT_H_SYNC_NEGATIVE) ?
                               "-" : "+",
                               (pTiming->VSyncPol == NVT_V_SYNC_NEGATIVE) ?
                               "-" : "+");

            if (pTiming->interlaced) {
                nvEvoLogInfoString(pInfoString,
                                   "    Interlaced       : yes");
            }
            if (pTiming->etc.flag & NVT_FLAG_NV_DOUBLE_SCAN_TIMING) {
                nvEvoLogInfoString(pInfoString,
                                   "    Double Scanned   : yes");
            }

            if (type == NVT_TYPE_EDID_861ST) {
                nvEvoLogInfoString(pInfoString,
                                   "    CEA Format       : %d",
                                   NVT_GET_CEA_FORMAT(pTiming->etc.status));
            }

            if (NV_NVT_TIMING_HAS_ASPECT_RATIO(pTiming)) {
                nvEvoLogInfoString(pInfoString,
                                   "    Aspect Ratio     : %d:%d",
                                   NV_NVT_TIMING_IMAGE_SIZE_WIDTH(pTiming),
                                   NV_NVT_TIMING_IMAGE_SIZE_HEIGHT(pTiming));
            }

            if (NV_NVT_TIMING_HAS_IMAGE_SIZE(pTiming)) {
                nvEvoLogInfoString(pInfoString,
                                   "    Image Size       : %d mm x %d mm",
                                   NV_NVT_TIMING_IMAGE_SIZE_WIDTH(pTiming),
                                   NV_NVT_TIMING_IMAGE_SIZE_HEIGHT(pTiming));
            }

            if (IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.rgb444.bpcs)) {
                nvEvoLogInfoString(pInfoString,
                                   "    RGB 444 bpcs     : %s",
                                   GetColorDepthBpc(pTiming->etc.rgb444));
            }

            if (IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.yuv444.bpcs)) {
                nvEvoLogInfoString(pInfoString,
                                   "    YUV 444 bpcs     : %s",
                                   GetColorDepthBpc(pTiming->etc.yuv444));
            }

            if (IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.yuv422.bpcs)) {
                nvEvoLogInfoString(pInfoString,
                                   "    YUV 422 bpcs     : %s",
                                   GetColorDepthBpc(pTiming->etc.yuv422));
            }

            if (IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.yuv420.bpcs)) {
                nvEvoLogInfoString(pInfoString,
                                   "    YUV 420 bpcs     : %s",
                                   GetColorDepthBpc(pTiming->etc.yuv420));
            }
        } // i
    } // k

    nvEvoLogInfoString(pInfoString, "");

 done:
    nvEvoLogInfoString(pInfoString,
                       "--- End of EDID for %s ---", pDpyEvo->name);
    nvEvoLogInfoString(pInfoString, "");
}



/*
 * Clear the EDID and related fields in the display device data
 * structure.
 */

static void ClearEdid(NVDpyEvoPtr pDpyEvo, const NvBool bSendHdmiCapsToRm)
{
    NVEdidRec edid = { };
    NVEvoInfoStringRec infoString;
    nvInitInfoString(&infoString, NULL, 0);

    if (EdidHasChanged(pDpyEvo, &edid, NULL)) {
        ApplyNewEdid(pDpyEvo, &edid, NULL,
                     bSendHdmiCapsToRm, &infoString);
    }
}



/*
 * ClearCustomEdid() - send an empty custom EDID to RM; this is to
 * clear out any stale state in RM about custom EDIDs that we may have
 * told RM about previous runs of X.
 */

static void ClearCustomEdid(const NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *setEdidParams;

    if (nvDpyEvoIsDPMST(pDpyEvo)) {
        // RM doesn't track this device, so leave the EDID alone.
        return;
    }

    setEdidParams = nvCalloc(sizeof(*setEdidParams), 1);
    if (setEdidParams == NULL) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Unable to clear custom EDID for display device %s",
                     pDpyEvo->name);
        return;
    }

    setEdidParams->subDeviceInstance = pDispEvo->displayOwner;
    setEdidParams->displayId = nvDpyEvoGetConnectorId(pDpyEvo);
    setEdidParams->bufferSize = 0;

    /* ignore the NvRmControl() return value */

    (void) nvRmApiControl(nvEvoGlobal.clientHandle,
                          pDevEvo->displayCommonHandle,
                          NV0073_CTRL_CMD_SPECIFIC_SET_EDID_V2,
                          setEdidParams, sizeof(*setEdidParams));

    nvFree(setEdidParams);
} // ClearCustomEdid()



/*
 * WriteEdidToResman() - send a custom EDID to RM.
 */

static void WriteEdidToResman(const NVDpyEvoRec *pDpyEvo,
                              const NVEdidRec *pEdid)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *setEdidParams = NULL;
    NvU32 status = NVOS_STATUS_ERROR_OPERATING_SYSTEM;

    if (pEdid->length > sizeof(setEdidParams->edidBuffer)) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
            "EDID for display device %s is too long for NV0073_CTRL_CMD_SPECIFIC_SET_EDID_V2",
            pDpyEvo->name);
        goto done;
    }

    setEdidParams = nvCalloc(sizeof(*setEdidParams), 1);
    if (setEdidParams == NULL) {
        goto done;
    }

    setEdidParams->subDeviceInstance = pDispEvo->displayOwner;
    setEdidParams->displayId = nvDpyEvoGetConnectorId(pDpyEvo);
    nvkms_memcpy(&setEdidParams->edidBuffer, pEdid->buffer, pEdid->length);
    setEdidParams->bufferSize = pEdid->length;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            pDevEvo->displayCommonHandle,
                            NV0073_CTRL_CMD_SPECIFIC_SET_EDID_V2,
                            setEdidParams, sizeof(*setEdidParams));

done:
    if (status != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Failure processing EDID for display device "
                     "%s.", pDpyEvo->name);
    }

    nvFree(setEdidParams);
} // WriteEdidToResman()


/*
 * NvTiming_ParseEDIDInfo() will ignore some modes that are blatantly
 * wrong, so we need to apply any patching to the EDID bytes before
 * parsing the EDID.
 */
static void PrePatchEdid(const NVDpyEvoRec *pDpyEvo, NVEdidPtr pEdid,
                         NVEvoInfoStringPtr pInfoString)
{
    NvU8 *pEdidData = pEdid->buffer;

    if (pEdid->buffer == NULL || pEdid->length < 128) {
        return;
    }

    /*
     * Work around bug 628240: some AUO flat panels have invalid
     * native modes where HSyncEnd is larger than HTotal, putting the
     * end of the sync pulse several columns into the active region of
     * the next frame.  AUO confirmed these corrected timings:
     *
     *     "1366x768" 69.30 1366 1398 1422 1432 768 771 775 806 -hsync -vsync
     */
    if (pDpyEvo->pConnectorEvo->legacyType ==
        NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP &&
        pEdidData[0x36] == 0x26 &&
        pEdidData[0x37] == 0x1b &&
        pEdidData[0x38] == 0x56 &&
        pEdidData[0x39] == 0x47 &&
        pEdidData[0x3a] == 0x50 &&
        pEdidData[0x3b] == 0x00 &&
        pEdidData[0x3c] == 0x26 &&
        pEdidData[0x3d] == 0x30 &&
        pEdidData[0x3e] == 0x30 &&
        pEdidData[0x3f] == 0x20 &&
        pEdidData[0x40] == 0x34 &&
        pEdidData[0x41] == 0x00 &&
        pEdidData[0x42] == 0x58 &&
        pEdidData[0x43] == 0xc1 &&
        pEdidData[0x44] == 0x10 &&
        pEdidData[0x45] == 0x00 &&
        pEdidData[0x46] == 0x00 &&
        pEdidData[0x47] == 0x18 &&
        pEdidData[0x7f] == 0x2e) {

        pEdidData[0x36] = 0x12;
        pEdidData[0x37] = 0x1b;
        pEdidData[0x38] = 0x56;
        pEdidData[0x39] = 0x42;
        pEdidData[0x3a] = 0x50;
        pEdidData[0x3b] = 0x00;
        pEdidData[0x3c] = 0x26;
        pEdidData[0x3d] = 0x30;
        pEdidData[0x3e] = 0x20;
        pEdidData[0x3f] = 0x18;
        pEdidData[0x40] = 0x34;
        pEdidData[0x41] = 0x00;
        pEdidData[0x42] = 0x58;
        pEdidData[0x43] = 0xc1;
        pEdidData[0x44] = 0x10;
        pEdidData[0x45] = 0x00;
        pEdidData[0x46] = 0x00;
        pEdidData[0x47] = 0x18;
        pEdidData[0x7f] = 0x5f;

        nvEvoLogInfoString(pInfoString, "Fixed invalid mode for 1366x768");
    }
}

/*
 * CreateParsedEdidFromNVT_TIMING() - Puts modetiming data from RM into an EDID format
 */
static void CreateParsedEdidFromNVT_TIMING(
    NVT_TIMING *pTimings,
    NvU8 bpc,
    NVParsedEdidEvoPtr pParsedEdid)
{
    nvkms_memset(pParsedEdid, 0, sizeof(*pParsedEdid));
    pParsedEdid->info.total_timings = 1;
    nvkms_memcpy(&pParsedEdid->info.timing[0], pTimings, sizeof(*pTimings));
    pParsedEdid->info.timing[0].etc.status = NVT_STATUS_CUST;
    pParsedEdid->info.u.feature_ver_1_4_digital.continuous_frequency = FALSE;
    pParsedEdid->info.version = NVT_EDID_VER_1_4;
    pParsedEdid->info.input.isDigital = TRUE;
    pParsedEdid->info.input.u.digital.bpc = bpc;
    pParsedEdid->limits.min_h_rate_hz = 1;
    pParsedEdid->limits.min_v_rate_hzx1k = 1;
    pParsedEdid->limits.max_h_rate_hz = NV_U32_MAX;
    pParsedEdid->limits.max_v_rate_hzx1k = NV_U32_MAX;
    pParsedEdid->valid = TRUE;
}

/*
 * PatchAndParseEdid() - use the nvtiming library to parse the EDID data.  The
 * EDID data provided in the 'pEdid' argument may be patched or modified.
 */

static void PatchAndParseEdid(
    const NVDpyEvoRec *pDpyEvo,
    NVEdidPtr pEdid,
    NVParsedEdidEvoPtr pParsedEdid,
    NVEvoInfoStringPtr pInfoString)
{
    int i;
    NVT_STATUS status;
    NvU32 edidSize;

    if (pEdid->buffer == NULL || pEdid->length == 0) {
        return;
    }

    nvkms_memset(pParsedEdid, 0, sizeof(*pParsedEdid));

    PrePatchEdid(pDpyEvo, pEdid, pInfoString);

    /* parse the majority of information from the EDID */

    status = NvTiming_ParseEDIDInfo(pEdid->buffer, pEdid->length,
                                    &pParsedEdid->info);

    if (status != NVT_STATUS_SUCCESS) {
        return;
    }

    /* interpret the frequency range limits from the EDID */

    NvTiming_CalculateEDIDLimits(&pParsedEdid->info, &pParsedEdid->limits);

    /* get the user-friendly monitor name */

    NvTiming_GetMonitorName(&pParsedEdid->info,
                            (NvU8 *) &pParsedEdid->monitorName);
    nvAssert(pParsedEdid->monitorName[0] != '\0');

    /* find the serial number string */

    pParsedEdid->serialNumberString[0] = '\0';

    for (i = 0; i < NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR; i++) {
        if (pParsedEdid->info.ldd[i].tag == NVT_EDID_DISPLAY_DESCRIPTOR_DPSN) {
            nvkms_strncpy(
                pParsedEdid->serialNumberString,
                (const char *)pParsedEdid->info.ldd[i].u.serial_number.str,
                sizeof(pParsedEdid->serialNumberString));
            pParsedEdid->serialNumberString[
                sizeof(pParsedEdid->serialNumberString) - 1] = '\0';
            break;
        }
    }


    for (i = 0; i < pParsedEdid->info.total_timings; i++) {
        NVT_TIMING *pTiming = &pParsedEdid->info.timing[i];

        /* patch up RRx1k for 640x480@60Hz */

        if (IsEdid640x480_60_NVT_TIMING(pTiming)) {
            pTiming->etc.rrx1k = 59940;
        }

        /*
         * Invalidate modes that require pixel repetition (i.e., modes
         * that don't support Pixel Repetition 0).  See bug 1459376.
         */

        nvAssert(pTiming->etc.rep != 0);

        if ((pTiming->etc.rep & NVBIT(0)) == 0) {
            pTiming->etc.status = 0;
        }
    }

    pParsedEdid->valid = TRUE;

    /* resize the EDID buffer, if necessary */

    edidSize = NVT_EDID_ACTUAL_SIZE(&pParsedEdid->info);

    if (edidSize < pEdid->length) {
        NvU8 *pEdidData = nvAlloc(edidSize);

        if (pEdidData != NULL) {
            nvkms_memcpy(pEdidData, pEdid->buffer, edidSize);

            nvFree(pEdid->buffer);

            pEdid->buffer = pEdidData;
            pEdid->length = edidSize;
        }
    }
}


/*!
 * Assign NVDpyEvoRec::name.
 *
 * The name has the form:
 *
 *  "edidName (typeName-N.dpAddress)"
 *
 * If edidName is unavailable, then it, and the parentheses are omitted:
 *
 *  "typeName-N.dpAddress"
 *  "typeName-N"
 *
 * if dpAddress is unavailable, then the ".dpAddress" is omitted:
 *
 *  "edidName (typeName-N)"
 *  "typeName-N"
 */
static void AssignDpyEvoName(NVDpyEvoPtr pDpyEvo)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;
    const char *edidName = "";
    const char *openParen = "";
    const char *closeParen = "";
    const char *dpAddress = "";
    const char *dpAddressSeparator = "";

    if (pDpyEvo->parsedEdid.valid &&
        pDpyEvo->parsedEdid.monitorName[0] != '\0') {
        edidName = pDpyEvo->parsedEdid.monitorName;
        openParen = " (";
        closeParen = ")";
    }

    if (pDpyEvo->dp.addressString != NULL) {
        dpAddress = pDpyEvo->dp.addressString;
        dpAddressSeparator = ".";
    }

    nvkms_snprintf(pDpyEvo->name, sizeof(pDpyEvo->name),
                   "%s%s%s%s%s%s",
                   edidName,
                   openParen,
                   pConnectorEvo->name,
                   dpAddressSeparator,
                   dpAddress,
                   closeParen);

    pDpyEvo->name[sizeof(pDpyEvo->name) - 1] = '\0';
}

enum NvKmsDpyAttributeDigitalSignalValue
nvGetDefaultDpyAttributeDigitalSignalValue(const NVConnectorEvoRec *pConnectorEvo)
{
    enum NvKmsDpyAttributeDigitalSignalValue signal =
        NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_LVDS;

    if (pConnectorEvo->legacyType == NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {
        if (nvConnectorUsesDPLib(pConnectorEvo)) {
            signal = NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_DISPLAYPORT;
        } else {
            nvAssert((pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) ||
                     (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_DSI));

            if (pConnectorEvo->or.protocol ==
                NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM) {
                signal = NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_LVDS;
            } else if (pConnectorEvo->or.protocol ==
                       NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DSI) {
                signal = NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_DSI;
            } else {
                // May be later changed to HDMI_FRL at modeset time.
                signal = NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_TMDS;
            }
        }
    }

    return signal;
}

NVDpyEvoPtr nvAllocDpyEvo(NVDispEvoPtr pDispEvo,
                          NVConnectorEvoPtr pConnectorEvo,
                          NVDpyId dpyId, const char *dpAddress)
{
    NVDpyEvoPtr pDpyEvo;

    pDpyEvo = nvCalloc(1, sizeof(*pDpyEvo));

    if (pDpyEvo == NULL) {
        return NULL;
    }

    pDpyEvo->pDispEvo = pDispEvo;
    pDpyEvo->pConnectorEvo = pConnectorEvo;
    pDpyEvo->apiHead = NV_INVALID_HEAD;
    pDpyEvo->id = dpyId;

    nvListAdd(&pDpyEvo->dpyListEntry, &pDispEvo->dpyList);

    if (dpAddress) {
        pDpyEvo->dp.addressString = nvStrDup(dpAddress);
        pDispEvo->displayPortMSTIds =
            nvAddDpyIdToDpyIdList(dpyId, pDispEvo->displayPortMSTIds);

        if (!nvConnectorIsDPSerializer(pConnectorEvo)) {
            pDispEvo->dynamicDpyIds =
                nvAddDpyIdToDpyIdList(dpyId, pDispEvo->dynamicDpyIds);
	}
    }

    AssignDpyEvoName(pDpyEvo);

    nvDpyProbeMaxPixelClock(pDpyEvo);

    pDpyEvo->requestedDithering.state =
        NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_AUTO;
    pDpyEvo->requestedDithering.mode =
        NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO;
    pDpyEvo->requestedDithering.depth =
        NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_AUTO;

    // Initialize DP link rate and lane count to sane values.
    // This is normally done in nvDPLibUpdateDpyLinkConfiguration,
    // but do it here as well in case we query flat panel properties for
    // screenless DP devices.
    if (nvConnectorUsesDPLib(pConnectorEvo)) {
        pDpyEvo->dp.linkRate = 0;
        pDpyEvo->dp.laneCount = NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_1;
        pDpyEvo->dp.connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_UNKNOWN;
        pDpyEvo->dp.sinkIsAudioCapable = FALSE;
    }

    pDpyEvo->requestedColorSpace =
        NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_RGB;
    pDpyEvo->requestedColorRange =
        NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;

    pDpyEvo->currentAttributes = NV_EVO_DEFAULT_ATTRIBUTES_SET;
    pDpyEvo->currentAttributes.digitalSignal =
        nvGetDefaultDpyAttributeDigitalSignalValue(pConnectorEvo);

    DpyGetStaticDfpProperties(pDpyEvo);

    return pDpyEvo;
}


void nvFreeDpyEvo(NVDispEvoPtr pDispEvo, NVDpyEvoPtr pDpyEvo)
{
    nvCancelSDRTransitionTimer(pDpyEvo);

    DpyDisconnectEvo(pDpyEvo, FALSE /* bSendHdmiCapsToRm */);

    // Let the DP library host implementation handle deleting a pDpy as if the
    // library had notified it of a lost device.
    nvDPDpyFree(pDpyEvo);
    nvAssert(!pDpyEvo->dp.pDpLibDevice);

    pDispEvo->validDisplays =
        nvDpyIdListMinusDpyId(pDispEvo->validDisplays, pDpyEvo->id);

    pDispEvo->displayPortMSTIds =
        nvDpyIdListMinusDpyId(pDispEvo->displayPortMSTIds, pDpyEvo->id);
    pDispEvo->dynamicDpyIds =
        nvDpyIdListMinusDpyId(pDispEvo->dynamicDpyIds, pDpyEvo->id);

    nvListDel(&pDpyEvo->dpyListEntry);

    nvFree(pDpyEvo->dp.addressString);
    nvFree(pDpyEvo);
}


/*!
 * Return the pConnectorEvo associated with the given (static) display ID.
 *
 * XXX[DP] not valid for DP monitors, the connector will be known before
 *         initialization so this will not be needed.
 *
 * \param[in]  pDisp    The pDisp on which to search for the pConnector.
 * \param[in]  dpyId    The ID of the connector to search for.
 *
 * \return  The pConnectorEvo from pDisp that matches the ID, or NULL if
 *          no connector is found.
 */
NVConnectorEvoPtr nvGetConnectorFromDisp(NVDispEvoPtr pDispEvo, NVDpyId dpyId)
{
    NVConnectorEvoPtr pConnectorEvo;

    nvAssert(nvDpyIdIsInDpyIdList(dpyId, pDispEvo->connectorIds));

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (nvDpyIdsAreEqual(dpyId, pConnectorEvo->displayId)) {
            return pConnectorEvo;
        }
    }

    nvAssert(!"Failed to find pDpy's connector!");
    return NULL;
}

void nvDpyAssignSDRInfoFramePayload(NVT_HDR_INFOFRAME_PAYLOAD *pPayload)
{
    nvkms_memset(pPayload, 0, sizeof(*pPayload));
    pPayload->eotf = NVT_CEA861_HDR_INFOFRAME_EOTF_SDR_GAMMA;
    pPayload->static_metadata_desc_id = NVT_CEA861_STATIC_METADATA_SM0;
}

static void ConstructHdrInfoFrameSdp(const NVDispEvoRec *pDispEvo,
                                     const NvU32 head,
                                     DPSDP_DESCRIPTOR *sdp)
{
    const NVDispHeadStateEvoRec *pHeadState =
                                &pDispEvo->headState[head];

    sdp->hb.hb0 = 0;
    sdp->hb.hb1 = dp_pktType_DynamicRangeMasteringInfoFrame;
    sdp->hb.hb2 = DP_INFOFRAME_SDP_V1_3_NON_AUDIO_SIZE - 1;
    sdp->hb.hb3 = DP_INFOFRAME_SDP_V1_3_VERSION <<
                      DP_INFOFRAME_SDP_V1_3_HB3_VERSION_SHIFT;

    sdp->db.db0 = NVT_VIDEO_INFOFRAME_VERSION_1;
    sdp->db.db1 = sizeof(NVT_HDR_INFOFRAME_PAYLOAD);

    nvAssert(sizeof(NVT_HDR_INFOFRAME_PAYLOAD) <= (sizeof(sdp->db) - 2));

    if (pHeadState->hdrInfoFrame.state == NVKMS_HDR_INFOFRAME_STATE_ENABLED) {
        NVT_HDR_INFOFRAME_PAYLOAD *payload =
            (NVT_HDR_INFOFRAME_PAYLOAD *) &sdp->db.db2;

        payload->eotf = pHeadState->hdrInfoFrame.eotf;

        payload->static_metadata_desc_id = NVT_CEA861_STATIC_METADATA_SM0;

        // payload->type1 = static metadata
        nvAssert(sizeof(NVT_HDR_INFOFRAME_MASTERING_DATA) ==
                 (sizeof(struct NvKmsHDRStaticMetadata)));
        nvkms_memcpy(&payload->type1,
                     &pHeadState->hdrInfoFrame.staticMetadata,
                     sizeof(NVT_HDR_INFOFRAME_MASTERING_DATA));
    } else if (pHeadState->hdrInfoFrame.state ==
               NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING) {
        nvDpyAssignSDRInfoFramePayload((NVT_HDR_INFOFRAME_PAYLOAD *) &sdp->db.db2);
    } else {
        nvAssert(pHeadState->hdrInfoFrame.state == NVKMS_HDR_INFOFRAME_STATE_DISABLED);

        nvDpyAssignSDRInfoFramePayload((NVT_HDR_INFOFRAME_PAYLOAD *) &sdp->db.db2);
    }

    sdp->dataSize = sizeof(NVT_HDR_INFOFRAME_PAYLOAD) + 2;
}

static void UpdateDpHDRInfoFrame(const NVDispEvoRec *pDispEvo, const NvU32 head)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState =
                                &pDispEvo->headState[head];
    DPSDP_DESCRIPTOR sdp = { };
    NvEvoInfoFrameTransmitControl transmitCtrl =
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_INIT;

    ConstructHdrInfoFrameSdp(pDispEvo, head, &sdp);

    switch (pHeadState->hdrInfoFrame.state) {
        case NVKMS_HDR_INFOFRAME_STATE_DISABLED:
            transmitCtrl = NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME;
            break;
        case NVKMS_HDR_INFOFRAME_STATE_ENABLED:
        case NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING:
            transmitCtrl = NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME;
            break;
    }

    pDevEvo->hal->SendDpInfoFrameSdp(pDispEvo, head, transmitCtrl, &sdp);
}

void nvConstructDpVscSdp(const NVDispHeadInfoFrameStateEvoRec *pInfoFrame,
                         const NVDpyAttributeColor *pDpyColor,
                         DPSDP_DP_VSC_SDP_DESCRIPTOR *sdp)
{
    nvkms_memset(sdp, 0, sizeof(*sdp));

    // Header
    // Per DP1.3 spec
    sdp->hb.hb0 = 0;
    sdp->hb.hb1 = SDP_PACKET_TYPE_VSC;
    sdp->hb.revisionNumber = SDP_VSC_REVNUM_STEREO_PSR2_COLOR;
    sdp->hb.numValidDataBytes = SDP_VSC_VALID_DATA_BYTES_PSR2_COLOR;

    sdp->db.stereoInterface = 0;
    sdp->db.psrState = 0;
    sdp->db.contentType = SDP_VSC_CONTENT_TYPE_GRAPHICS;
    switch (pDpyColor->format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            sdp->db.pixEncoding = SDP_VSC_PIX_ENC_RGB;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            sdp->db.pixEncoding = SDP_VSC_PIX_ENC_YCBCR444;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            sdp->db.pixEncoding = SDP_VSC_PIX_ENC_YCBCR422;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            sdp->db.pixEncoding = SDP_VSC_PIX_ENC_YCBCR420;
            break;
        default:
            nvAssert(!"unrecognized color format");
            break;
    }

    switch (pDpyColor->format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            switch (pDpyColor->colorimetry) {
                case NVKMS_OUTPUT_COLORIMETRY_BT2100:
                    sdp->db.colorimetryFormat =
                        SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_ITU_R_BT2020_RGB;
                    break;
                case NVKMS_OUTPUT_COLORIMETRY_DEFAULT:
                    sdp->db.colorimetryFormat =
                        SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_SRGB;
                    break;
            }

            switch (pDpyColor->bpc) {
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
                    sdp->db.bitDepth = SDP_VSC_BIT_DEPTH_RGB_10BPC;
                    break;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
                    sdp->db.bitDepth = SDP_VSC_BIT_DEPTH_RGB_8BPC;
                    break;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6:
                    sdp->db.bitDepth = SDP_VSC_BIT_DEPTH_RGB_6BPC;
                    break;
                default:
                    nvAssert(!"Invalid bpc value for RBG format");
                    break;
            }
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            switch (pDpyColor->colorimetry) {
                case NVKMS_OUTPUT_COLORIMETRY_BT2100:
                    sdp->db.colorimetryFormat =
                        SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT2020_YCBCR;
                    break;
                case NVKMS_OUTPUT_COLORIMETRY_DEFAULT:
                    sdp->db.colorimetryFormat =
                        (pInfoFrame->hdTimings ?
                            SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT709 :
                            SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT601);
                    break;
            }

            switch (pDpyColor->bpc) {
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
                    sdp->db.bitDepth = SDP_VSC_BIT_DEPTH_YCBCR_10BPC;
                    break;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
                    sdp->db.bitDepth = SDP_VSC_BIT_DEPTH_YCBCR_8BPC;
                    break;
                default:
                    nvAssert(!"Invalid bpc value for YUV color format");
                    break;
            }
            break;
        default:
            nvAssert(!"unrecognized color format");
            break;
    }

    switch (pDpyColor->range) {
        case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL:
            sdp->db.dynamicRange = SDP_VSC_DYNAMIC_RANGE_VESA;
            break;
        case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED:
            sdp->db.dynamicRange = SDP_VSC_DYNAMIC_RANGE_CEA;
            break;
        default:
            nvAssert(!"Invalid colorRange value");
            break;
    }
}

/*
 * Construct the DP 1.3 VSC SDP infoframe, and toggle it on or off based on
 * whether or not YUV420 mode or BT2100 colorimetry is in use.
 */
static void UpdateDpVscSdpInfoFrame(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NVDpyAttributeColor *pDpyColor,
    const NVDispHeadInfoFrameStateEvoRec *pInfoFrame)
{
    const NVDispHeadStateEvoRec *pHeadState =
                                &pDispEvo->headState[head];
    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    /*
     * If the hardware supports the VSC SDP programming using the core
     * channel, the VSC SDP is already programmed during modeset.
     */
    if (pDevEvo->hal->SetDpVscSdp != NULL) {
        return;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;

    if ((pDpyColor->format == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420) ||
        (pDpyColor->colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100)) {

        // DPSDP_DP_VSC_SDP_DESCRIPTOR has a (dataSize, hb, db) layout, while
        // NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS.aPacket needs to contain
        // (hb, db) without dataSize, so this makes sdp->hb align with aPacket.
        DPSDP_DP_VSC_SDP_DESCRIPTOR *sdp =
            (DPSDP_DP_VSC_SDP_DESCRIPTOR *)(params.aPacket -
            offsetof(DPSDP_DP_VSC_SDP_DESCRIPTOR, hb));

        nvAssert((void *)&sdp->hb == (void *)params.aPacket);

        nvConstructDpVscSdp(pInfoFrame, pDpyColor, sdp);

        params.packetSize = sizeof(sdp->hb) + sdp->hb.numValidDataBytes;

        params.transmitControl =
            DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ENABLE, _YES) |
            DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _OTHER_FRAME, _DISABLE) |
            DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _SINGLE_FRAME, _DISABLE) |
            DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ON_HBLANK, _DISABLE);
    } else {
        params.transmitControl =
            DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ENABLE, _NO);
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET failed");
    }
}

static void UpdateDpInfoFrames(const NVDispEvoRec *pDispEvo,
                               const NvU32 head,
                               const NVDpyAttributeColor *pDpyColor,
                               const NVDispHeadInfoFrameStateEvoRec *pInfoFrame)
{
    UpdateDpHDRInfoFrame(pDispEvo, head);

    UpdateDpVscSdpInfoFrame(pDispEvo, head, pDpyColor, pInfoFrame);
}

void nvCancelSDRTransitionTimer(NVDpyEvoRec *pDpyEvo)
{
    nvkms_free_timer(pDpyEvo->hdrToSdrTransitionTimer);
    pDpyEvo->hdrToSdrTransitionTimer = NULL;
}

static void SDRTransition(void *dataPtr, NvU32 dataU32)
{
    NvU32 head;
    NVDpyEvoRec *pDpyEvo = dataPtr;
    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvCancelSDRTransitionTimer(pDpyEvo);

    nvAssert(pApiHeadState->hwHeadsMask != 0);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];
        nvAssert(pHeadState->hdrInfoFrame.state ==
                 NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING);
        pHeadState->hdrInfoFrame.state = NVKMS_HDR_INFOFRAME_STATE_DISABLED;
    }

    nvUpdateInfoFrames(pDpyEvo);
}

static
void ScheduleSDRTransitionTimer(NVDpyEvoRec *pDpyEvo)
{
    if (pDpyEvo->hdrToSdrTransitionTimer) {
        return;
    }

    pDpyEvo->hdrToSdrTransitionTimer =
        nvkms_alloc_timer(SDRTransition,
                          pDpyEvo,
                          0,
                          2000000 /* 2 seconds */);
}

void nvUpdateInfoFrames(NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState;
    const NVDispHeadStateEvoRec *pHeadState;
    NvU32 head;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return;
    }
    pApiHeadState = &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvAssert((pApiHeadState->hwHeadsMask) != 0x0 &&
             (nvDpyIdIsInDpyIdList(pDpyEvo->id, pApiHeadState->activeDpys)));

    head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);

    nvAssert(head != NV_INVALID_HEAD);

    pHeadState = &pDispEvo->headState[head];

    if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        UpdateDpInfoFrames(pDispEvo,
                           head,
                           &pApiHeadState->attributes.color,
                           &pApiHeadState->infoFrame);
    } else {
        nvUpdateHdmiInfoFrames(pDispEvo,
                               head,
                               &pApiHeadState->attributes.color,
                               &pApiHeadState->infoFrame,
                               pDpyEvo);
    }

    if (pHeadState->hdrInfoFrame.state == NVKMS_HDR_INFOFRAME_STATE_ENABLED) {
        nvCancelSDRTransitionTimer(pDpyEvo);
    } else if (pHeadState->hdrInfoFrame.state ==
               NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING) {
        ScheduleSDRTransitionTimer(pDpyEvo);
    }
}

/*!
 * nvDpyRequiresDualLinkEvo() - Returns whether or not the given mode exceeds
 * the maximum single TMDS link pixel clock.
 *
 * \param[in] pDpyEvo display to check the maximum single link pixel clock
 *
 * \param[in] pTimings mode timings to check pixel clock
 *
 * \return TRUE if pixel clock exceeds display's maximum single link pixel
 * clock
 */
NvBool nvDpyRequiresDualLinkEvo(const NVDpyEvoRec *pDpyEvo,
                                const NVHwModeTimingsEvo *pTimings)
{
    const NvU32 pixelClock = (pTimings->yuv420Mode == NV_YUV420_MODE_HW) ?
        (pTimings->pixelClock / 2) : pTimings->pixelClock;

    // Dual link HDMI is not possible.
    nvAssert(!(nvDpyIsHdmiEvo(pDpyEvo) &&
               (pixelClock > pDpyEvo->maxSingleLinkPixelClockKHz)));
    return (pixelClock > pDpyEvo->maxSingleLinkPixelClockKHz);
}

/*!
 * Return the NVDpyEvoPtr that corresponds to the given dpyId, on the
 * given NVDispEvoPtr, or NULL if no matching NVDpyEvoPtr can be
 * found.
 */
NVDpyEvoPtr nvGetDpyEvoFromDispEvo(const NVDispEvoRec *pDispEvo, NVDpyId dpyId)
{
    NVDpyEvoPtr pDpyEvo;

    FOR_ALL_EVO_DPYS(pDpyEvo, nvAddDpyIdToEmptyDpyIdList(dpyId), pDispEvo) {
        return pDpyEvo;
    }

    return NULL;
}

/*
 * Find or create a pDpy with a given root connector and topology path.
 */
NVDpyEvoPtr nvGetDPMSTDpyEvo(NVConnectorEvoPtr pConnectorEvo,
                             const char *address, NvBool *pDynamicDpyCreated)
{
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVDpyEvoPtr pDpyEvo = NULL, pTmpDpyEvo;
    NVDpyId dpyId;

    // Look for a pDpyEvo on pConnectorEvo whose dp address matches.
    FOR_ALL_EVO_DPYS(pTmpDpyEvo, pDispEvo->validDisplays, pDispEvo) {
        if (pTmpDpyEvo->pConnectorEvo != pConnectorEvo) {
            continue;
        }
        if (pTmpDpyEvo->dp.addressString == NULL) {
            continue;
        }
        if (nvkms_strcmp(pTmpDpyEvo->dp.addressString, address) == 0) {
            pDpyEvo = pTmpDpyEvo;
            goto done;
        }
    }

    // Find a display ID that is not used on this GPU.
    dpyId = nvNewDpyId(pDispEvo->validDisplays);
    if (nvDpyIdIsInvalid(dpyId)) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to allocate a display ID for device %s.%s",
                     pConnectorEvo->name,
                     address);
        goto done;
    }

    // Create a new pDpy for this address.
    pDpyEvo = nvAllocDpyEvo(pDispEvo, pConnectorEvo, dpyId, address);
    if (pDpyEvo == NULL) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to create a display device object for %s-%u.%s",
                     NvKmsConnectorTypeString(pConnectorEvo->type),
                     pConnectorEvo->typeIndex,
                     address);
        goto done;
    }

    pDispEvo->validDisplays =
        nvAddDpyIdToDpyIdList(dpyId, pDispEvo->validDisplays);

    *pDynamicDpyCreated = TRUE;

done:
    return pDpyEvo;
}

/*!
 * Return a string with a comma-separated list of dpy names, for all
 * dpys in dpyIdList.
 *
 * If there are no dpys in the dpyIdList, return "none".
 *
 * The string is dynamically allocated and should be freed by the caller.
 *
 * Return NULL if an allocation failure occurs.
 */
char *nvGetDpyIdListStringEvo(NVDispEvoPtr pDispEvo,
                              const NVDpyIdList dpyIdList)
{
    NVDpyEvoPtr pDpyEvo;
    char *listString = NULL;
    NvU32 lengths[NV_DPY_ID_MAX_DPYS_IN_LIST];
    NvU32 totalLength = 0;
    NvU32 currentOffset;
    NvU32 index;

    index = 0;
    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {

        nvAssert(index < ARRAY_LEN(lengths));

        lengths[index] = nvkms_strlen(pDpyEvo->name);

        totalLength += lengths[index];

        if (index != 0) {
            totalLength += 2; /* nvkms_strlen(", ") */
        }

        index++;
    }

    totalLength += 1; /* for nul terminator */

    if (index == 0) {
        return nvStrDup("none");
    }

    listString = nvAlloc(totalLength);

    if (listString == NULL) {
        return NULL;
    }

    index = 0;
    currentOffset = 0;

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {

        if (index != 0) {
            listString[currentOffset] = ',';
            listString[currentOffset+1] = ' ';
            currentOffset += 2;
        }

        nvkms_memcpy(listString + currentOffset, pDpyEvo->name, lengths[index]);

        currentOffset += lengths[index];

        index++;
    }

    listString[currentOffset] = '\0';
    currentOffset += 1;

    nvAssert(currentOffset == totalLength);

    return listString;
}

NvBool nvDpyGetDynamicData(
    NVDpyEvoPtr pDpyEvo,
    struct NvKmsQueryDpyDynamicDataParams *pParams)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    struct NvKmsQueryDpyDynamicDataRequest *pRequest = &pParams->request;
    struct NvKmsQueryDpyDynamicDataReply *pReply = &pParams->reply;
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    NVDpyIdList connectedList;
    NVDpyIdList oneDpyIdList = nvAddDpyIdToEmptyDpyIdList(pDpyEvo->id);
    NVDpyOverridePtr pDpyOverride = nvDpyEvoGetOverride(pDpyEvo);

    nvkms_memset(pReply, 0, sizeof(*pReply));

    if (pDpyOverride != NULL) {
        if (pDpyOverride->connected && !pRequest->forceDisconnected) {
            /*
             * If display is overridden as connected, treat the request as if it
             * had both forceConnected and overrideEdid set, unless the request
             * had forceDisconnected set.
             *
             * If the request already had an EDID override, honor that EDID instead
             * of the display override EDID.
             */
            NvBool old = pRequest->forceConnected;
            pRequest->forceConnected = TRUE;

            if (!pRequest->overrideEdid) {
                size_t len = nvReadDpyOverrideEdid(pDpyOverride,
                                                   pRequest->edid.buffer,
                                                   ARRAY_LEN(pRequest->edid.buffer));

                if (len != 0) {
                    pRequest->overrideEdid = TRUE;
                    pRequest->edid.bufferSize = len;
                } else {
                    pRequest->forceConnected = old;
                }
            }
        } else if (!pDpyOverride->connected && !pRequest->forceConnected) {
            /*
             * If display is overriden as disconnected, treat the request as if it
             * had forceDisconnected set, unless the request had forceConnected set.
             */
            pRequest->forceDisconnected = TRUE;
        }
    }

    /*
     * Check for the connection state of the dpy.
     *
     * For DP MST, we need to honor the current DPlib state; if a DP
     * MST monitor is physically connected but forceDisconnected, its
     * hotplug events won't get serviced and DPlib will complain
     * loudly. This doesn't apply to DP serializer (which is not managed
     * by DPLib) since we don't need to do any topology/branch detection,
     * and we can honor force{Connected,Disconnected} in MST & SST mode.
     *
     * Otherwise, allow the client to override detection.
     *
     * Otherwise, honor the current DPlib state.
     *
     * If we're using a DP serializer connector in MST mode, don't expose any
     * SST displays as connected. In all other cases, assume that everything
     * is connected since the serializer connector has a fixed topology.
     *
     * Lastly, call RM to check if the dpy is connected.
     */

    if (nvDpyEvoIsDPMST(pDpyEvo) &&
        nvConnectorUsesDPLib(pConnectorEvo)) {
        /* honor DP MST connectedness */
        connectedList = nvDPLibDpyIsConnected(pDpyEvo) ?
            oneDpyIdList : nvEmptyDpyIdList();
    } else if (pRequest->forceConnected) {
        connectedList = oneDpyIdList;
    } else if (pRequest->forceDisconnected) {
        connectedList = nvEmptyDpyIdList();
    } else if (nvConnectorUsesDPLib(pConnectorEvo)) {
        connectedList = nvDPLibDpyIsConnected(pDpyEvo) ?
            oneDpyIdList : nvEmptyDpyIdList();
    } else if (nvConnectorIsDPSerializer(pConnectorEvo)) {
        if (pConnectorEvo->dpSerializerCaps.supportsMST &&
            !nvDpyEvoIsDPMST(pDpyEvo)) {
            connectedList = nvEmptyDpyIdList();
        } else {
            connectedList = oneDpyIdList;
        }
    } else {
        connectedList = nvRmGetConnectedDpys(pDispEvo, oneDpyIdList);
    }

    pDpyEvo->dp.inbandStereoSignaling = pRequest->dpInbandStereoSignaling;

    /*
     * XXX NVKMS TODO: once NVKMS is in the kernel and
     * nvAllocCoreChannelEvo() is guaranteed to happen before
     * nvDpyGetDynamicData(), pass allowDVISpecPClkOverride through to
     * nvDpyProbeMaxPixelClock() rather than cache it.
     */
    pDpyEvo->allowDVISpecPClkOverride = pRequest->allowDVISpecPClkOverride;

    if (nvDpyIdIsInDpyIdList(pDpyEvo->id, connectedList)) {
        if (!DpyConnectEvo(pDpyEvo, pParams)) {
            return FALSE;
        }
    } else {
        DpyDisconnectEvo(pDpyEvo, TRUE /* bSendHdmiCapsToRm */);
    }

    if (nvConnectorUsesDPLib(pConnectorEvo)) {
        nvDPLibUpdateDpyLinkConfiguration(pDpyEvo);
    }

    ct_assert(sizeof(pDpyEvo->name) == sizeof(pReply->name));

    nvkms_memcpy(pReply->name, pDpyEvo->name, sizeof(pDpyEvo->name));

    if (pDpyEvo->parsedEdid.valid) {
        pReply->physicalDimensions.heightInCM =
            pDpyEvo->parsedEdid.info.screen_size_y;
        pReply->physicalDimensions.widthInCM =
            pDpyEvo->parsedEdid.info.screen_size_x;
    }

    /*
     * XXX NVKMS until NVKMS is in the kernel and
     * nvAllocCoreChannelEvo() is guaranteed to happen before
     * nvDpyGetDynamicData(), pDpyEvo->maxPixelClockKHz could change
     * later after the assignment here.
     */
    pReply->maxPixelClockKHz = pDpyEvo->maxPixelClockKHz;

    pReply->connected =
        nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->connectedDisplays);

    pReply->isVirtualRealityHeadMountedDisplay = pDpyEvo->isVrHmd;

    pReply->vrrType = pDpyEvo->vrr.type;
    pReply->supportsHDR = nvDpyIsHDRCapable(pDpyEvo);

    pReply->stereo3DVision.supported = pDpyEvo->stereo3DVision.supported;
    pReply->stereo3DVision.isDLP = pDpyEvo->stereo3DVision.isDLP;
    pReply->stereo3DVision.isAegis = pDpyEvo->stereo3DVision.isAegis;
    pReply->stereo3DVision.subType = pDpyEvo->stereo3DVision.subType;

    pReply->dp.guid.valid = pDpyEvo->dp.guid.valid;

    ct_assert(sizeof(pReply->dp.guid.buffer) ==
              sizeof(pDpyEvo->dp.guid.buffer));
    nvkms_memcpy(pReply->dp.guid.buffer, pDpyEvo->dp.guid.buffer,
                 sizeof(pDpyEvo->dp.guid.buffer));

    ct_assert(sizeof(pReply->dp.guid.str) == sizeof(pDpyEvo->dp.guid.str));
    nvkms_memcpy(pReply->dp.guid.str, pDpyEvo->dp.guid.str,
                 sizeof(pDpyEvo->dp.guid.str));

    if (pDpyEvo->edid.length > sizeof(pReply->edid.buffer)) {
        nvAssert(!"EDID larger than can be returned in NVKMS API");
        return FALSE;
    }

    if (pDpyEvo->edid.length > 0) {
        pReply->edid.bufferSize = pDpyEvo->edid.length;
        nvkms_memcpy(pReply->edid.buffer, pDpyEvo->edid.buffer, pDpyEvo->edid.length);
    }

    pReply->supportedOutputColorFormats =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);

    return TRUE;
}

void nvDpyUpdateCurrentAttributes(NVDpyEvoRec *pDpyEvo)
{
    NVAttributesSetEvoRec newAttributes = pDpyEvo->currentAttributes;

    if (pDpyEvo->apiHead != NV_INVALID_HEAD) {
        newAttributes =
            pDpyEvo->pDispEvo->apiHeadState[pDpyEvo->apiHead].attributes;
    } else {
        newAttributes.dithering.enabled = FALSE;
        newAttributes.dithering.depth   = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE;
        newAttributes.dithering.mode    = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE;
        newAttributes.digitalSignal =
            nvGetDefaultDpyAttributeDigitalSignalValue(pDpyEvo->pConnectorEvo);
        newAttributes.numberOfHardwareHeadsUsed = 0;
    }

    if (newAttributes.color.format !=
        pDpyEvo->currentAttributes.color.format) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE,
            newAttributes.color.format);
    }

    if (newAttributes.color.range !=
        pDpyEvo->currentAttributes.color.range) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_RANGE,
            newAttributes.color.range);
    }

    if (newAttributes.dithering.enabled !=
        pDpyEvo->currentAttributes.dithering.enabled) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING,
            newAttributes.dithering.enabled);
    }

    if (newAttributes.dithering.depth !=
        pDpyEvo->currentAttributes.dithering.depth) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH,
            newAttributes.dithering.depth);
    }

    if (newAttributes.dithering.mode !=
        pDpyEvo->currentAttributes.dithering.mode) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE,
            newAttributes.dithering.mode);
    }

    if (newAttributes.imageSharpening.available !=
        pDpyEvo->currentAttributes.imageSharpening.available) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING_AVAILABLE,
            newAttributes.imageSharpening.available);
    }

    if (newAttributes.digitalSignal !=
            pDpyEvo->currentAttributes.digitalSignal) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL,
            newAttributes.digitalSignal);
    }

    if (newAttributes.numberOfHardwareHeadsUsed !=
        pDpyEvo->currentAttributes.numberOfHardwareHeadsUsed) {
        nvSendDpyAttributeChangedEventEvo(
            pDpyEvo,
            NV_KMS_DPY_ATTRIBUTE_NUMBER_OF_HARDWARE_HEADS_USED,
            newAttributes.numberOfHardwareHeadsUsed);
    }

    pDpyEvo->currentAttributes = newAttributes;
}

// Returns TRUE if this display is capable of Adaptive-Sync
NvBool nvDpyIsAdaptiveSync(const NVDpyEvoRec *pDpyEvo)
{
    return nvIsAdaptiveSyncDpyVrrType(pDpyEvo->vrr.type);
}

// Returns TRUE if this display is in the Adaptive-Sync defaultlist
NvBool nvDpyIsAdaptiveSyncDefaultlisted(const NVDpyEvoRec *pDpyEvo)
{
    NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS params = { };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (!pDpyEvo->parsedEdid.valid) {
        return FALSE;
    }

    params.manufacturerID = pDpyEvo->parsedEdid.info.manuf_id;
    params.productID = pDpyEvo->parsedEdid.info.product_id;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to query default adaptivesync listing for %s", pDpyEvo->name);
        return FALSE;
    }

    return params.bDefaultAdaptivesync;
}

static enum NvKmsDpyAttributeColorBpcValue GetYuv422MaxBpc(
    const NVDpyEvoRec *pDpyEvo)
{
    const NVT_EDID_CEA861_INFO *p861Info =
        &pDpyEvo->parsedEdid.info.ext861;

    nvAssert(nvDpyIsHdmiEvo(pDpyEvo) ||
             nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo));

    if (!pDpyEvo->parsedEdid.valid ||
        !pDpyEvo->parsedEdid.info.input.isDigital) {
        return NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
    }

    if (pDpyEvo->parsedEdid.info.version >= NVT_EDID_VER_1_4) {
        if (pDpyEvo->parsedEdid.info.u.feature_ver_1_4_digital.support_ycrcb_422) {
            if (pDpyEvo->parsedEdid.info.input.u.digital.bpc >= 10) {
                return NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
            } else if (pDpyEvo->parsedEdid.info.input.u.digital.bpc >= 8) {
                return NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
            }
        }
    } else {
        nvAssert(!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo));

        if (p861Info->revision >= NVT_CEA861_REV_A &&
                !!(p861Info->basic_caps & NVT_CEA861_CAP_YCbCr_422)) {
            return NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
        }
    }

    return NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
}

NvKmsDpyOutputColorFormatInfo nvDpyGetOutputColorFormatInfo(
    const NVDpyEvoRec *pDpyEvo)
{
    const NVConnectorEvoRec *pConnectorEvo =
            pDpyEvo->pConnectorEvo;
    NvKmsDpyOutputColorFormatInfo colorFormatsInfo = { };

    if (pConnectorEvo->legacyType ==
            NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT) {

        colorFormatsInfo.rgb444.maxBpc =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
        colorFormatsInfo.rgb444.minBpc =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;

    } else if (pConnectorEvo->legacyType ==
                   NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {

        if (pConnectorEvo->signalFormat ==
                NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) {

            if (pDpyEvo->parsedEdid.valid) {
                switch (pDpyEvo->parsedEdid.info.input.u.digital.bpc) {
                    case 10:
                        colorFormatsInfo.rgb444.maxBpc =
                            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
                        break;
                    case 6:
                        colorFormatsInfo.rgb444.maxBpc =
                            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6;
                        break;
                    default:
                        nvAssert(!"Unsupported bpc for DSI");
                        // fall through
                    case 8:
                        colorFormatsInfo.rgb444.maxBpc =
                            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                        break;
                }

                colorFormatsInfo.rgb444.minBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6;
            } else {
                colorFormatsInfo.rgb444.maxBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                colorFormatsInfo.rgb444.minBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
            }
        } else if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {

            if (pDpyEvo->parsedEdid.valid &&
                pDpyEvo->parsedEdid.info.input.isDigital &&
                pDpyEvo->parsedEdid.info.version >= NVT_EDID_VER_1_4) {
                if (pDpyEvo->parsedEdid.info.input.u.digital.bpc >= 10) {
                    colorFormatsInfo.rgb444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
                    colorFormatsInfo.yuv444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
                } else if (pDpyEvo->parsedEdid.info.input.u.digital.bpc < 8) {
                    colorFormatsInfo.rgb444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6;
                    colorFormatsInfo.yuv444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
                } else {
                    colorFormatsInfo.rgb444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                    colorFormatsInfo.yuv444.maxBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                }

                colorFormatsInfo.rgb444.minBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6;
                if (colorFormatsInfo.yuv444.maxBpc !=
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
                     colorFormatsInfo.yuv444.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                } else {
                    colorFormatsInfo.yuv444.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
                }

                colorFormatsInfo.yuv422.maxBpc = GetYuv422MaxBpc(pDpyEvo);
                if (colorFormatsInfo.yuv422.maxBpc !=
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
                    colorFormatsInfo.yuv422.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                } else {
                    colorFormatsInfo.yuv422.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
                }
            } else {
                colorFormatsInfo.rgb444.maxBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                colorFormatsInfo.rgb444.minBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
            }
        } else {
            colorFormatsInfo.rgb444.maxBpc =
                nvDpyIsHdmiDepth30Evo(pDpyEvo) ?
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10 :
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
            colorFormatsInfo.rgb444.minBpc =
                NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;

            if (nvDpyIsHdmiEvo(pDpyEvo)) {
                colorFormatsInfo.yuv444.maxBpc =
                    nvDpyIsHdmiDepth30Evo(pDpyEvo) ?
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10 :
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                colorFormatsInfo.yuv444.minBpc =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;

                colorFormatsInfo.yuv422.maxBpc = GetYuv422MaxBpc(pDpyEvo);
                if (colorFormatsInfo.yuv422.maxBpc !=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
                    colorFormatsInfo.yuv422.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
                } else {
                    colorFormatsInfo.yuv422.minBpc =
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
                }
            }
        }
    }

    switch (nvkms_debug_force_color_space()) {
    case NVKMS_DEBUG_FORCE_COLOR_SPACE_RGB:
        colorFormatsInfo.yuv444.maxBpc =
        colorFormatsInfo.yuv444.minBpc =
        colorFormatsInfo.yuv422.maxBpc =
        colorFormatsInfo.yuv422.minBpc =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
        break;
    case NVKMS_DEBUG_FORCE_COLOR_SPACE_YUV444:
        colorFormatsInfo.rgb444.maxBpc =
        colorFormatsInfo.rgb444.minBpc =
        colorFormatsInfo.yuv422.maxBpc =
        colorFormatsInfo.yuv422.minBpc =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
        break;
    case NVKMS_DEBUG_FORCE_COLOR_SPACE_YUV422:
        colorFormatsInfo.rgb444.maxBpc =
        colorFormatsInfo.rgb444.minBpc =
        colorFormatsInfo.yuv444.maxBpc =
        colorFormatsInfo.yuv444.minBpc =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN;
        break;
    default:
        nvAssert(!"Unrecognzed debug_force_color_space value");
        // fallthrough
    case NVKMS_DEBUG_FORCE_COLOR_SPACE_NONE:
        break;
    }

    return colorFormatsInfo;
}

NvU32 nvDpyGetPossibleApiHeadsMask(const NVDpyEvoRec *pDpyEvo)
{
    NvU32 possibleApiHeadMask = 0x0;
    NvU32 possibleNumLayers = NVKMS_MAX_LAYERS_PER_HEAD;
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;

    /*
     * DSI supports only the hardware head-0 assigment, and the
     * dp-serializer dpys are bound to the specific hardware head;
     * the modeset client can be allowed to choose only those
     * api-heads to drive these dpys which has the number of layers
     * less than or equal to the number of layers supported by the
     * bound hardware heads.
     */
    if (pDpyEvo->pConnectorEvo->signalFormat ==
            NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) {
        possibleNumLayers = pDevEvo->head[0].numLayers;
    } else if (nvConnectorIsDPSerializer(pDpyEvo->pConnectorEvo)) {
        const NvU32 boundHead = pDpyEvo->dp.serializerStreamIndex;
        possibleNumLayers = pDevEvo->head[boundHead].numLayers;
    }

    for (NvU32 apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        if (pDevEvo->apiHead[apiHead].numLayers <= possibleNumLayers) {
            possibleApiHeadMask |= NVBIT(apiHead);
        }
    }

    return possibleApiHeadMask;
}

NvBool nvDpyIsHDRCapable(const NVDpyEvoRec *pDpyEvo)
{
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    const NVT_EDID_INFO *pInfo = &pDpyEvo->parsedEdid.info;
    const NVT_HDR_STATIC_METADATA *pHdrInfo =
        &pInfo->hdr_static_metadata_info;

    // Only supported on DP 1.3+ or HDMI
    if (nvDpyUsesDPLib(pDpyEvo)) {
        unsigned int major;
        unsigned int minor;

        if(!pDevEvo->caps.supportsDP13) {
            return FALSE;
        }

        if (!nvDPDpyGetDpcdRevision(pDpyEvo, &major, &minor)) {
            return FALSE;
        }

        nvAssert(major >= 1);
        if ((major == 1) && (minor < 3)) {
            return FALSE;
        }
    } else if (!nvDpyIsHdmiEvo(pDpyEvo)) {
        return FALSE;
    }

    if (!pDpyEvo->parsedEdid.valid) {
        return FALSE;
    }

    /*
     * XXX HDR is not supported with HDMI 3D due to both using VSI
     * infoframes.
     */
    if (pInfo->HDMI3DSupported) {
        return FALSE;
    }

    // Sink should support ST2084 EOTF.
    if (!pHdrInfo->supported_eotf.smpte_st_2084_eotf) {
        return FALSE;
    }

    /*
     * Sink should support static metadata type1. Nvtiming sets
     * static_metadata_type to 1 if the sink supports static metadata type1.
     */
    if (pHdrInfo->static_metadata_type != 1) {
        return FALSE;
    }

    return TRUE;
}
