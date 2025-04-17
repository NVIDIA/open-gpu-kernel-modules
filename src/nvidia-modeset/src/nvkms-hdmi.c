/*
 * SPDX-FileCopyrightText: Copyright (c) 2007 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This source file contains codes for enabling HDMI audio.
 */


#include "nvkms-dpy.h"
#include "nvkms-hdmi.h"
#include "nvkms-evo.h"
#include "nvkms-modepool.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h"
#include "nvkms-vrr.h"
#include "dp/nvdp-connector.h"

#include "hdmi_spec.h"
#include "nvos.h"

#include <ctrl/ctrl0073/ctrl0073dfp.h> // NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS
#include <ctrl/ctrl0073/ctrl0073dp.h> // NV0073_CTRL_CMD_DP_SET_AUDIO_MUTESTREAM
#include <ctrl/ctrl0073/ctrl0073specific.h> // NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS
#include <ctrl/ctrl2080/ctrl2080unix.h> // NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER

#include <hdmipacket/nvhdmipkt.h>

#define CAP_HDMI_SUPPORT_GPU             0x00000001
#define CAP_HDMI_SUPPORT_MONITOR         0x00000002
#define CAP_HDMI_SUPPORT_MONITOR_48_BPP  0x00000004
#define CAP_HDMI_SUPPORT_MONITOR_36_BPP  0x00000008
#define CAP_HDMI_SUPPORT_MONITOR_30_BPP  0x00000010

static inline const NVT_EDID_CEA861_INFO *GetExt861(const NVParsedEdidEvoRec *pParsedEdid,
                                                    int extIndex)
{
    if (!pParsedEdid->valid || extIndex > 1) {
        return NULL;
    }

    return (extIndex == 0) ? &pParsedEdid->info.ext861 :
                             &pParsedEdid->info.ext861_2;
}

/*
 * CalculateVideoInfoFrameColorFormat() - calculate colorspace,
 * colorimetry and colorrange for video infoframe.
 */
static void CalculateVideoInfoFrameColorFormat(
    const NVDpyAttributeColor *pDpyColor,
    const NvU32 hdTimings,
    const NVT_EDID_INFO *pEdidInfo,
    NVT_VIDEO_INFOFRAME_CTRL *pCtrl)
{
    NvBool    sinkSupportsRGBQuantizationOverride = FALSE;

    // sets video infoframe colorspace (RGB/YUV).
    switch (pDpyColor->format) {
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
        pCtrl->color_space = NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_RGB;
        break;
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
        pCtrl->color_space = NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr422;
        break;
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
        pCtrl->color_space = NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr444;
        break;
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
        pCtrl->color_space = NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr420;
        break;
    default:
        nvAssert(!"Invalid colorSpace value");
        break;
    }

    // sets video infoframe colorimetry.
    switch (pDpyColor->format) {
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
        switch (pDpyColor->colorimetry) {
        case NVKMS_OUTPUT_COLORIMETRY_BT2100:
            pCtrl->colorimetry = NVT_VIDEO_INFOFRAME_BYTE2_C1C0_EXT_COLORIMETRY;
            pCtrl->extended_colorimetry =
                NVT_VIDEO_INFOFRAME_BYTE3_EC_BT2020RGBYCC;
            break;
        case NVKMS_OUTPUT_COLORIMETRY_DEFAULT:
            pCtrl->colorimetry = NVT_VIDEO_INFOFRAME_BYTE2_C1C0_NO_DATA;
            break;
        }
        break;
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
        switch (pDpyColor->colorimetry) {
        case NVKMS_OUTPUT_COLORIMETRY_BT2100:
            pCtrl->colorimetry = NVT_VIDEO_INFOFRAME_BYTE2_C1C0_EXT_COLORIMETRY;
            pCtrl->extended_colorimetry =
                NVT_VIDEO_INFOFRAME_BYTE3_EC_BT2020RGBYCC;
            break;
        case NVKMS_OUTPUT_COLORIMETRY_DEFAULT:
            pCtrl->colorimetry =
                (hdTimings ? NVT_VIDEO_INFOFRAME_BYTE2_C1C0_ITU709 :
                             NVT_VIDEO_INFOFRAME_BYTE2_C1C0_SMPTE170M_ITU601);
            break;
        }
        break;
    case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
        // XXX HDR TODO: Support YUV420 + HDR
        nvAssert(pDpyColor->colorimetry != NVKMS_OUTPUT_COLORIMETRY_BT2100);
        pCtrl->colorimetry = NVT_VIDEO_INFOFRAME_BYTE2_C1C0_ITU709;
        break;
    default:
        nvAssert(!"Invalid colorSpace value");
        break;
    }

    // sets video infoframe colorrange.
    switch (pDpyColor->range) {
    case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL:
        pCtrl->rgb_quantization_range =
            NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_FULL_RANGE;
        break;
    case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED:
        pCtrl->rgb_quantization_range =
            NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_LIMITED_RANGE;
        break;
    default:
        nvAssert(!"Invalid colorRange value");
        break;
    }

    if (pEdidInfo != NULL) {
        sinkSupportsRGBQuantizationOverride = (pEdidInfo->ext861.valid.VCDB &&
            ((pEdidInfo->ext861.video_capability & NVT_CEA861_VCDB_QS_MASK) >>
             NVT_CEA861_VCDB_QS_SHIFT) != 0);
    }

    if ((pDpyColor->format == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) &&
            !sinkSupportsRGBQuantizationOverride) {
        pCtrl->rgb_quantization_range = NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_DEFAULT;
    }

    /*
     * Only limited color range is allowed with YUV444 and YUV422 color spaces.
     */
    nvAssert(!(((pCtrl->color_space == NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr422) ||
                (pCtrl->color_space == NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr444)) &&
               (pCtrl->rgb_quantization_range !=
                    NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_LIMITED_RANGE)));
}

/*
 * GetHDMISupportCap() - find the HDMI capabilities of 
 * the gpu and the display device.
 */

static NvU32 GetHDMISupportCap(const NVDpyEvoRec *pDpyEvo)
{
    NvU32 hdmiCap = 0;
    int extIndex;

    if (pDpyEvo->hdmiCapable) {
        hdmiCap |= CAP_HDMI_SUPPORT_GPU;
    }

    for (extIndex = 0; TRUE; extIndex++) {

        int vsdbIndex;
        const NVT_EDID_CEA861_INFO *pExt861 =
            GetExt861(&pDpyEvo->parsedEdid, extIndex);

        if (pExt861 == NULL) {
            break;
        }

        if (pExt861->revision <= NVT_CEA861_REV_ORIGINAL) {
            continue;
        }

        for (vsdbIndex = 0; vsdbIndex < pExt861->total_vsdb; vsdbIndex++) {
            if (pExt861->vsdb[vsdbIndex].ieee_id == NVT_CEA861_HDMI_IEEE_ID) {
                const NVT_HDMI_LLC_VSDB_PAYLOAD *payload =
                    (const NVT_HDMI_LLC_VSDB_PAYLOAD *)
                        &pExt861->vsdb[vsdbIndex].vendor_data;

                hdmiCap |= CAP_HDMI_SUPPORT_MONITOR;

                if (payload->DC_48bit) {
                    hdmiCap |= CAP_HDMI_SUPPORT_MONITOR_48_BPP;
                }

                if (payload->DC_36bit) {
                    hdmiCap |= CAP_HDMI_SUPPORT_MONITOR_36_BPP;
                }

                if (payload->DC_30bit) {
                    hdmiCap |= CAP_HDMI_SUPPORT_MONITOR_30_BPP;
                }

                return hdmiCap;
            }
        }
    }

    return hdmiCap;
}

/*!
 * Return whether the GPU supports HDMI and the display is connected
 * via HDMI.
 */
NvBool nvDpyIsHdmiEvo(const NVDpyEvoRec *pDpyEvo)
{
    NvU32 hdmiCap;

    hdmiCap = GetHDMISupportCap(pDpyEvo);

    return ((hdmiCap & CAP_HDMI_SUPPORT_GPU) &&
            (hdmiCap & CAP_HDMI_SUPPORT_MONITOR));
}

/*!
 * Returns whether the GPU and the display both support HDMI depth 30.
 */
NvBool nvDpyIsHdmiDepth30Evo(const NVDpyEvoRec *pDpyEvo)
{
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    NvU32 hdmiCap = GetHDMISupportCap(pDpyEvo);

    return nvkms_hdmi_deepcolor() &&
           nvDpyIsHdmiEvo(pDpyEvo) &&
           pDevEvo->hal->caps.supportsHDMI10BPC &&
           (hdmiCap & CAP_HDMI_SUPPORT_MONITOR_30_BPP);
}

/*!
 * Updates the display's HDMI 2.0 capabilities to the RM.
 */
void nvSendHdmiCapsToRm(NVDpyEvoPtr pDpyEvo)
{
    NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS params = { 0 };
    NVParsedEdidEvoPtr pParsedEdid = &pDpyEvo->parsedEdid;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (!pDevEvo->caps.supportsHDMI20 ||
        nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return;
    }

    params.subDeviceInstance    = pDispEvo->displayOwner;
    params.displayId            = nvDpyEvoGetConnectorId(pDpyEvo);
    params.caps = 0;

    /*
     * nvSendHdmiCapsToRm() gets called on dpy's connect/disconnect events
     * to set/clear capabilities, clear capabilities if parsed edid
     * is not valid.
     */
    if (pParsedEdid->valid) {
        const NVT_HDMI_FORUM_INFO *pHdmiInfo = &pParsedEdid->info.hdmiForumInfo;
        if (pHdmiInfo->scdc_present) {
            params.caps |= DRF_DEF(0073, _CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS,
                                   _SCDC_SUPPORTED, _TRUE);
        }

        if (pHdmiInfo->max_TMDS_char_rate > 0)  {
            params.caps |= DRF_DEF(0073, _CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS,
                                   _GT_340MHZ_CLOCK_SUPPORTED, _TRUE);
        }

        if (pHdmiInfo->lte_340Mcsc_scramble) {
            if (!pHdmiInfo->scdc_present) {
                nvEvoLogDisp(pDispEvo,
                             EVO_LOG_WARN,
                             "EDID inconsistency: SCDC is not present in EDID, but EDID requests 340Mcsc scrambling.");
            }

            params.caps |= DRF_DEF(0073, _CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS,
                                   _LTE_340MHZ_SCRAMBLING_SUPPORTED, _TRUE);
        }

        /* HDMI Fixed-rate link information */
        if (pDevEvo->hal->caps.supportsHDMIFRL) {
            nvAssert((pHdmiInfo->max_FRL_Rate &
                      ~DRF_MASK(NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_MAX_FRL_RATE_SUPPORTED)) == 0);
            params.caps |= DRF_NUM(0073_CTRL_CMD_SPECIFIC, _SET_HDMI_SINK_CAPS, _MAX_FRL_RATE_SUPPORTED,
                                   pHdmiInfo->max_FRL_Rate);

            if (pHdmiInfo->dsc_1p2) {
                nvAssert((pHdmiInfo->dsc_1p2 &
                          ~DRF_MASK(NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS_DSC_MAX_FRL_RATE_SUPPORTED)) == 0);
                params.caps |= DRF_NUM(0073_CTRL_CMD_SPECIFIC, _SET_HDMI_SINK_CAPS, _DSC_MAX_FRL_RATE_SUPPORTED,
                                       pHdmiInfo->dsc_1p2);
                params.caps |= DRF_DEF(0073_CTRL_CMD_SPECIFIC, _SET_HDMI_SINK_CAPS, _DSC_12_SUPPORTED, _TRUE);
            }
        }
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_SINK_CAPS failed");
    }
}

/*
 * HdmiSendEnable() - Used to signal RM to enable various hdmi components
 * such as audio engine.
 */

static void HdmiSendEnable(NVDpyEvoPtr pDpyEvo, NvBool hdmiEnable)
{
    NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance    = pDpyEvo->pDispEvo->displayOwner;
    params.displayId            = nvDpyEvoGetConnectorId(pDpyEvo);
    params.enable               = hdmiEnable;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_ENABLE,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_ENABLE failed");
    }
}

/*!
 * Sends General Control Packet to the HDMI sink.
 */
static void SendHdmiGcp(const NVDispEvoRec *pDispEvo,
                        const NvU32 head, NvBool avmute)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVHDMIPKT_RESULT ret;

    NvU8 sb0 = avmute ? HDMI_GENCTRL_PACKET_MUTE_ENABLE :
        HDMI_GENCTRL_PACKET_MUTE_DISABLE;

    NvU8 sb1 = 0;

    NvU8 sb2 = NVT_HDMI_RESET_DEFAULT_PIXELPACKING_PHASE;

    NvU8 gcp[] = {
        pktType_GeneralControl, 0, 0, sb0, sb1, sb2, 0, 0, 0, 0
    };

    ret = NvHdmiPkt_PacketWrite(pDevEvo->hdmiLib.handle,
                                pDispEvo->displayOwner,
                                pHeadState->activeRmId,
                                head,
                                NVHDMIPKT_TYPE_GENERAL_CONTROL,
                                NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_EVERY_FRAME,
                                sizeof(gcp),
                                gcp);

    if (ret != NVHDMIPKT_SUCCESS) {
        nvAssert(ret == NVHDMIPKT_SUCCESS);
    }
}

/*
 * SendVideoInfoFrame() - Construct video infoframe using provided EDID and call
 * ->SendHdmiInfoFrame() to send it to RM.
 */
static void SendVideoInfoFrame(const NVDispEvoRec *pDispEvo,
                               const NvU32 head,
                               const NVDpyAttributeColor *pDpyColor,
                               const NVDispHeadInfoFrameStateEvoRec *pInfoFrameState,
                               NVT_EDID_INFO *pEdidInfo)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvBool hdTimings = pInfoFrameState->hdTimings;
    NVT_VIDEO_INFOFRAME_CTRL videoCtrl = pInfoFrameState->ctrl;
    NVT_VIDEO_INFOFRAME VideoInfoFrame;
    NVT_STATUS status;

    CalculateVideoInfoFrameColorFormat(pDpyColor, hdTimings, pEdidInfo, &videoCtrl);

    status = NvTiming_ConstructVideoInfoframe(pEdidInfo,
                                              &videoCtrl,
                                              NULL, &VideoInfoFrame);
    if (status != NVT_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                          "Error in constructing Video InfoFrame");
        return;
    }

    pDevEvo->hal->SendHdmiInfoFrame(
        pDispEvo,
        head,
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME,
        (NVT_INFOFRAME_HEADER *) &VideoInfoFrame,
        (/* header length */ sizeof(NVT_INFOFRAME_HEADER) +
         /* payload length */ VideoInfoFrame.length),
        TRUE /* needChecksum */);
}

/*
 * SendHDMI3DVendorSpecificInfoFrame() - Construct vendor specific infoframe
 * using provided EDID and call ->SendHdmiInfoFrame() to send it to RM. Currently
 * hardcoded to send the infoframe necessary for HDMI 3D.
 */

static void
SendHDMI3DVendorSpecificInfoFrame(const NVDispEvoRec *pDispEvo,
                                  const NvU32 head, NVT_EDID_INFO *pEdidInfo)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState =
                                 &pDispEvo->headState[head];
    NVT_VENDOR_SPECIFIC_INFOFRAME_CTRL vendorCtrl = {
        .Enable          = 1,
        .HDMIFormat      = NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_3D,
        .HDMI_VIC        = NVT_HDMI_VS_BYTE5_HDMI_VIC_NA,
        .ThreeDStruc     = NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK,
        .ThreeDDetail    = NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_NA,
        .MetadataPresent = 0,
        .MetadataType    = NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_NA,
    };
    NVT_VENDOR_SPECIFIC_INFOFRAME vendorInfoFrame;
    NVT_STATUS status;

    if (!pEdidInfo->HDMI3DSupported) {
        // Only send the HDMI 3D infoframe if the display supports HDMI 3D
        return;
    }

    // Send the infoframe with HDMI 3D configured if we're setting an HDMI 3D
    // mode.
    if (!pHeadState->timings.hdmi3D) {
        pDevEvo->hal->DisableHdmiInfoFrame(pDispEvo, head,
                                           NVT_INFOFRAME_TYPE_VENDOR_SPECIFIC);
        return;
    }

    status = NvTiming_ConstructVendorSpecificInfoframe(pEdidInfo,
                                                       &vendorCtrl,
                                                       &vendorInfoFrame);
    if (status != NVT_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                "Error in constructing Vendor Specific InfoFrame");
        return;
    }

    pDevEvo->hal->SendHdmiInfoFrame(
        pDispEvo,
        head,
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME,
        &vendorInfoFrame.Header,
        (/* header length */ sizeof(vendorInfoFrame.Header) +
         /* payload length */ vendorInfoFrame.Header.length),
        TRUE /* needChecksum */);
}

static void
SendHDRInfoFrame(const NVDispEvoRec *pDispEvo, const NvU32 head,
                 NVT_EDID_INFO *pEdidInfo)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState =
                                 &pDispEvo->headState[head];
    NVT_HDR_INFOFRAME hdrInfoFrame = { 0 };
    const NVT_HDR_STATIC_METADATA *pHdrInfo =
        &pEdidInfo->hdr_static_metadata_info;
    NvEvoInfoFrameTransmitControl transmitCtrl =
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_INIT;

    // Only send the HDMI HDR infoframe if the display supports HDR
    if (!pHdrInfo->supported_eotf.smpte_st_2084_eotf ||
        (pHdrInfo->static_metadata_type != 1)) {
        return;
    }

    // XXX HDR is not supported with HDMI 3D due to both using VSI infoframes.
    if (pEdidInfo->HDMI3DSupported) {
        return;
    }

    hdrInfoFrame.header.type = NVT_INFOFRAME_TYPE_DYNAMIC_RANGE_MASTERING;
    hdrInfoFrame.header.version = NVT_VIDEO_INFOFRAME_VERSION_1;
    hdrInfoFrame.header.length = sizeof(NVT_HDR_INFOFRAME) -
                                 sizeof(NVT_INFOFRAME_HEADER);

    if (pHeadState->hdrInfoFrame.state == NVKMS_HDR_INFOFRAME_STATE_ENABLED) {
        hdrInfoFrame.payload.eotf = pHeadState->hdrInfoFrame.eotf;
        hdrInfoFrame.payload.static_metadata_desc_id =
            NVT_CEA861_STATIC_METADATA_SM0;

        // payload->type1 = static metadata
        ct_assert(sizeof(NVT_HDR_INFOFRAME_MASTERING_DATA) ==
                  sizeof(struct NvKmsHDRStaticMetadata));
        nvkms_memcpy(&hdrInfoFrame.payload.type1,
                     (const NvU16 *) &pHeadState->hdrInfoFrame.staticMetadata,
                     sizeof(NVT_HDR_INFOFRAME_MASTERING_DATA));

        transmitCtrl = NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME;
    } else if (pHeadState->hdrInfoFrame.state ==
               NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING) {
        nvDpyAssignSDRInfoFramePayload(&hdrInfoFrame.payload);

        transmitCtrl = NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME;
    } else {
        nvAssert(pHeadState->hdrInfoFrame.state == NVKMS_HDR_INFOFRAME_STATE_DISABLED);

        nvDpyAssignSDRInfoFramePayload(&hdrInfoFrame.payload);

        transmitCtrl = NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME;
    }

    pDevEvo->hal->SendHdmiInfoFrame(
        pDispEvo,
        head,
        transmitCtrl,
        (NVT_INFOFRAME_HEADER *) &hdrInfoFrame.header,
        (/* header length */ sizeof(hdrInfoFrame.header) +
         /* payload length */ hdrInfoFrame.header.length),
        TRUE /* needChecksum */);
}


/*
 * Send video and 3D InfoFrames for HDMI.
 */
void nvUpdateHdmiInfoFrames(const NVDispEvoRec *pDispEvo,
                            const NvU32 head,
                            const NVDpyAttributeColor *pDpyColor,
                            const NVDispHeadInfoFrameStateEvoRec *pInfoFrameState,
                            NVDpyEvoRec *pDpyEvo)
{
    if (!nvDpyIsHdmiEvo(pDpyEvo)) {
        return;
    }

    if (!pDpyEvo->parsedEdid.valid) {
        nvEvoLogDispDebug(
            pDispEvo, EVO_LOG_WARN,
            "No EDID: cannot construct video/vendor-specific info frame");
        return;
    }

    SendVideoInfoFrame(pDispEvo,
                       head,
                       pDpyColor,
                       pInfoFrameState,
                       &pDpyEvo->parsedEdid.info);

    SendHDMI3DVendorSpecificInfoFrame(pDispEvo,
                                      head,
                                      &pDpyEvo->parsedEdid.info);

    SendHDRInfoFrame(pDispEvo,
                     head,
                     &pDpyEvo->parsedEdid.info);
}

static void SetDpAudioMute(const NVDispEvoRec *pDispEvo,
                           const NvU32 displayId, const NvBool mute)
{
    NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = displayId;
    params.mute = mute;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DP_SET_AUDIO_MUTESTREAM,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR, "NvRmControl"
                          "(NV0073_CTRL_CMD_DP_SET_AUDIO_MUTESTREAM) failed"
                          "return status = %d...", ret);
    }
}

static void SetDpAudioEnable(const NVDispEvoRec *pDispEvo,
                             const NvU32 head, const NvBool enable)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVConnectorEvoRec *pConnectorEvo = pHeadState->pConnectorEvo;
    NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    /* Mute audio stream before disabling it */
    if (!enable) {
        SetDpAudioMute(pDispEvo, pHeadState->activeRmId, TRUE);
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;
    params.enable = enable;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_SET_AUDIO_ENABLE,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "%s: Failed to %s DisplayPort audio stream-%u",
                     pConnectorEvo->name,
                     enable ? "enable" : "disable",
                     head);
    }

    /* Unmute audio stream after enabling it */
    if (enable) {
        SetDpAudioMute(pDispEvo, pHeadState->activeRmId, FALSE);
    }
}

/*
 * Uses RM control to mute HDMI audio stream at source side.
 */
static void SetHdmiAudioMute(const NVDispEvoRec *pDispEvo,
                             const NvU32 head, const NvBool mute)
{
    NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS params = { };
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;
    params.mute = (mute ? NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_TRUE :
        NV0073_CTRL_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_FALSE);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM failed");
    }
}

static void EnableHdmiAudio(const NVDispEvoRec *pDispEvo,
                            const NvU32 head, const NvBool enable)
{
    /*
     * XXX Is it correct to use pktType_GeneralControl to mute/unmute
     * the audio? pktType_GeneralControl controls both the audio and video data.
     */
    static const NvU8 InfoframeMutePacket[] = {
        pktType_GeneralControl, 0, 0, HDMI_GENCTRL_PACKET_MUTE_ENABLE, 0, 0, 0, 0,
        0, 0
    };
    static const NvU8 InfoframeUnMutePacket[] = {
        pktType_GeneralControl, 0, 0, HDMI_GENCTRL_PACKET_MUTE_DISABLE, 0, 0, 0, 0,
        0, 0
    };
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;
    params.transmitControl =
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ENABLE, _YES) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _OTHER_FRAME, _DISABLE) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _SINGLE_FRAME, _DISABLE) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ON_HBLANK, _DISABLE) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _VIDEO_FMT, _SW_CONTROLLED) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _RESERVED_LEGACY_MODE, _NO);

    params.packetSize = sizeof(InfoframeMutePacket);

    nvAssert(sizeof(InfoframeMutePacket) == sizeof(InfoframeUnMutePacket));

    nvkms_memcpy(params.aPacket,
                 enable ? InfoframeUnMutePacket : InfoframeMutePacket,
                 params.packetSize);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET failed");
    }
}

static const NVT_EDID_CEA861_INFO *GetMaxSampleRateExtBlock(
    const NVDpyEvoRec *pDpyEvo,
    const NVParsedEdidEvoRec *pParsedEdid,
    NvU32 *pMaxFreqSupported)
{
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    const NVT_EDID_CEA861_INFO *pExt861 = NULL;
    int extIndex;
    int i;

   *pMaxFreqSupported = 0;

    for (extIndex = 0; TRUE; extIndex++) {

        NvU8 sampleRateMask = 0;
        const NVT_EDID_CEA861_INFO *pTmpExt861 =
                                    GetExt861(pParsedEdid, extIndex);
        NvU32 maxFreqSupported = 0;

        if (pTmpExt861 == NULL) {
            break;
        }

        if (pTmpExt861->revision == NVT_CEA861_REV_NONE) {
            continue;
        }

        /* loop through all SAD to find out the max supported rate */
        for (i = 0; i < NVT_CEA861_AUDIO_MAX_DESCRIPTOR; i++) {

            const NvU8 byte1 = pTmpExt861->audio[i].byte1;
            const NvU8 byte2 = pTmpExt861->audio[i].byte2;

            if (byte1 == 0) {
                break;
            }

            if ((byte2 & NVT_CEA861_AUDIO_SAMPLE_RATE_MASK) > sampleRateMask) {
                sampleRateMask = byte2 & NVT_CEA861_AUDIO_SAMPLE_RATE_MASK;
            }
        }

        if (sampleRateMask != 0) {
            /* get the highest bit index */
            for (i = 7; i >= 1; i--) {
                if ((1<<(i-1)) & sampleRateMask) {
                    maxFreqSupported = i;
                    break;
                }
            }
        } else if (pTmpExt861->basic_caps & NVT_CEA861_CAP_BASIC_AUDIO) {
            /*
             * no short audio descriptor found, try the basic cap
             * Uncompressed, two channel, digital audio. Exact parameters are
             * determined by the interface specification used with CEA-861-D
             * (e.g., 2 channel IEC 60958 LPCM, 32, 44.1, and 48 kHz
             * sampling rates, 16 bits/sample).
             */
            maxFreqSupported =
                NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0480KHZ;
        }

        /* Cap DP audio to 48 KHz unless device supports 192 KHz */
        if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
            !pDevEvo->hal->caps.supportsDPAudio192KHz &&
            (maxFreqSupported >
             NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0480KHZ)) {
            maxFreqSupported =
                NV0073_CTRL_DFP_ELD_AUDIO_CAPS_MAX_FREQ_SUPPORTED_0480KHZ;
        }

        if (maxFreqSupported > *pMaxFreqSupported) {
            *pMaxFreqSupported = maxFreqSupported;
            pExt861 = pTmpExt861;
        }
    }

    return pExt861;
}

/*
 * Search a CEA-861 block for a Vendor Specific Data Block
 * with an IEEE "HDMI Licensing, LLC" OUI.
 *
 * If  found, returns VSDB_DATA * to Vendor Specific Data Block
 * If !found, returns NULL
 */
static const VSDB_DATA *GetVsdb(const NVT_EDID_CEA861_INFO *pExt861)
{
    const VSDB_DATA *pVsdb = NULL;

    for (int i = 0; i < pExt861->total_vsdb; i++) {
        if (pExt861->vsdb[i].ieee_id == NVT_CEA861_HDMI_IEEE_ID) {
            pVsdb = &pExt861->vsdb[i];
            break;
        }
    }
    return pVsdb;
}

static NvBool FillELDBuffer(const NVDpyEvoRec *pDpyEvo,
                            const NvU32 displayId,
                            const NVParsedEdidEvoRec *pParsedEdid,
                            NVEldEvoRec *pEld,
                            NvU32 *pMaxFreqSupported)
{
    const NVT_EDID_CEA861_INFO *pExt861;
    NvU32 SADCount, monitorNameLen;
    NvU8 name[NVT_EDID_LDD_PAYLOAD_SIZE + 1];
    NVT_STATUS status;
    NvU32 i;
    NvU8 EldSAI = 0;
    NvU8 EldAudSynchDelay = 0;
    const VSDB_DATA *pVsdb;
    NvBool isDisplayPort = nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo);

    pExt861 = GetMaxSampleRateExtBlock(pDpyEvo, pParsedEdid, pMaxFreqSupported);

    if (pExt861 == NULL) {
        return FALSE;
    }

    /* ELD header block: offset 0: ELD_Ver */
    pEld->buffer[0] = NVT_ELD_VER_2 << 3;

    /* Baseline block: offset 4: CEA_EDID_Ver */
    pEld->buffer[4] = pExt861->revision << 5;

    /* offset 5: SAD_Count */
    SADCount = 0;
    while (SADCount < NVT_CEA861_AUDIO_MAX_DESCRIPTOR &&
           pExt861->audio[SADCount].byte1 != 0) {
        SADCount++;
    }
    pEld->buffer[5] = SADCount << 4;

    /* offset 5: Conn_Type */
    if (isDisplayPort) {
        pEld->buffer[5] |= NVT_ELD_CONN_TYPE_DP << 2;
    } else {
        pEld->buffer[5] |= NVT_ELD_CONN_TYPE_HDMI << 2;
    }

    /* offset 5 b0: HDCP; always 0 for now */

    pVsdb = GetVsdb(pExt861);
    /* offset 5 b1=1 if Supports_AI; always 0 for DP */
    if ((!isDisplayPort) &&
        (pVsdb != NULL) &&
        (pVsdb->vendor_data_size > 2)) {
        EldSAI = pVsdb->vendor_data[2];
        EldSAI >>= 7;
    }
    pEld->buffer[5] |= EldSAI << 1;

    /* offset 6: Aud_Synch_delay in units of 2 msec */
    if ((pVsdb != NULL) &&
        (pVsdb->vendor_data_size > 6)) {
        EldAudSynchDelay = pVsdb->vendor_data[6];
    }
    pEld->buffer[6] = EldAudSynchDelay;

    /* offset 7: speaker allocation multiple allocation is not supported in ELD */
    pEld->buffer[7] = pExt861->speaker[0].byte1;

    /*
     * offset 8 ~ 15: port ID; nobody knows what port ID is, so far DD/RM/Audio
     * all agree to fill it with display Id.
     */
    pEld->buffer[8]  = displayId & 0xff;
    pEld->buffer[9]  = (displayId >> 8) & 0xff;
    pEld->buffer[10] = (displayId >> 16) & 0xff;
    pEld->buffer[11] = (displayId >> 24) & 0xff;

    /* offset 16 ~ 17: manufacturer name */
    pEld->buffer[16] = pParsedEdid->info.manuf_id & 0xff;
    pEld->buffer[17] = pParsedEdid->info.manuf_id >> 8;
    /* offset 18 ~ 19: product code */
    pEld->buffer[18] = pParsedEdid->info.product_id & 0xff;
    pEld->buffer[19] = (pParsedEdid->info.product_id >> 8) & 0xff;

    /*
     * offset 20 ~ 20 + MNL - 1: monitor name string (MNL - Monitor Name
     * Length)
     */

    monitorNameLen = 0;

    status = NvTiming_GetProductName(&pParsedEdid->info, name, sizeof(name));

    if (status == NVT_STATUS_SUCCESS) {
        /*
         * NvTiming_GetProductName returns a nul-terminated string. Figure out
         * how long it is and copy the bytes up to, but not including, the nul
         * terminator.
         */
        monitorNameLen = nvkms_strlen((char *)name);
        pEld->buffer[4] |= monitorNameLen;
        nvkms_memcpy(&pEld->buffer[20], name, monitorNameLen);
    }

    /* offset 20 + MNL ~ 20 + MNL + (3 * SAD_Count) - 1 : CEA_SADs */
    if (SADCount) {
        const size_t sadSize = SADCount * sizeof(NVT_3BYTES);
        const size_t bufferSize = sizeof(pEld->buffer) - monitorNameLen - 20;
        const size_t copySize = NV_MIN(bufferSize, sadSize);
        nvAssert(copySize == sadSize);

        nvkms_memcpy(&pEld->buffer[20 + monitorNameLen],
                     &pExt861->audio[0], copySize);
    }

    /*
     * The reserved section is not used yet.
     * offset 20 + MNL + (3 * SAD_Count) ~ 4 + Baseline_ELD_Len * 4 - 1;
     */

    /* Baseline block size in DWORD */
    i = (16 + monitorNameLen + SADCount * sizeof(NVT_3BYTES) +
        sizeof(NvU32) - 1) / sizeof(NvU32);
    pEld->buffer[2] = (NvU8)i;

    /* Update the entire ELD space */
    pEld->size = NV0073_CTRL_DFP_ELD_AUDIO_CAPS_ELD_BUFFER;

    return TRUE;
}

void nvHdmiDpConstructHeadAudioState(const NvU32 displayId,
                                     const NVDpyEvoRec *pDpyEvo,
                                     NVDispHeadAudioStateEvoRec *pAudioState)
{
    nvkms_memset(pAudioState, 0, sizeof(*pAudioState));

    /*
     * CRT and the DSI digital flat panel does not support audio. If (supported
     * == FALSE) the nvHdmiDpEnableDisableAudio does nothing.
     */
    if (pDpyEvo->pConnectorEvo->legacyType !=
            NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP ||
        pDpyEvo->pConnectorEvo->signalFormat ==
            NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) {
        return;
    }

    /*
     * The DP/TMDS digital flat panels supports audio, but do not enable audio
     * on the eDP and DVI displays. Some eDP panels goes blank when audio is
     * enabled, and DVI monitors do not support audio.
     *
     * If (supported == TRUE) and (enabled == FALSE) then
     * nvHdmiDpEnableDisableAudio() makes sure to keep audio disabled for
     * a given head.
     */
    pAudioState->supported = TRUE;

    if ((nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
            pDpyEvo->internal) ||
        (!nvDpyIsHdmiEvo(pDpyEvo) &&
            !nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo))) {
        return;
    }

    pAudioState->isAudioOverHdmi = nvDpyIsHdmiEvo(pDpyEvo);

    if (FillELDBuffer(pDpyEvo,
                      displayId,
                      &pDpyEvo->parsedEdid,
                      &pAudioState->eld,
                      &pAudioState->maxFreqSupported)) {
        pAudioState->enabled = TRUE;
    }
}

/*
 * Returns audio device entry of connector, which should
 * be attached to given head. Returns NONE if head is inactive.
 *
 * Each connector(SOR) supports four audio device entries, from 0 to 3,
 * which can drive four independent audio streams. Any head can be attached to
 * any audio device entry.
 *
 * Before audio-over-dp-mst support, by default the 0th device entry was
 * used when a given head was driving a DP-SST/HDMI/DVI display. This
 * function preserves that behavior. In the case of DP-MST, multiple heads
 * are attached to a single connector. In that case this functions returns
 * a device entry equal to the given head index.
 */
static NvU32 GetAudioDeviceEntry(const NVDispEvoRec *pDispEvo, const NvU32 head)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVConnectorEvoRec *pConnectorEvo =
        pHeadState->pConnectorEvo;

    if (pConnectorEvo == NULL) {
        return NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_NONE;
    }

    ct_assert(NV_MAX_AUDIO_DEVICE_ENTRIES == NVKMS_MAX_HEADS_PER_DISP);

    if (nvConnectorUsesDPLib(pConnectorEvo) &&
            (nvDPGetActiveLinkMode(pConnectorEvo->pDpLibConnector) ==
             NV_DP_LINK_MODE_MST)) {
        return NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_0 + head;
    }

    return NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_0;
}

/*!
 * Send EDID-Like-Data (ELD) to RM.
 *
 * ELD should be updated under the following situations:
 *
 * 1. Power on reset
 * 2. Pre modeset
 * 3. HotPlug / Post modeset
 *
 * Apart from ELD, also update the following control flags:
 *
 * isPD       - Present Detect, indicates if the monitor is attached
 * isELDV     - indicates if the ELD is Valid
 *
 * The values of iSPD and isELDV should be:
 *
 * NV_ELD_POWER_ON_RESET : isPD = 0, isELDV = 0
 * NV_ELD_PRE_MODESET    : isPD = 1, isELDV = 0
 * NV_ELD_POST_MODESET   : isPD = 1, isELDV = 1
 *
 * The initial ELD case of each audio device entry in hardware is unknown.
 * Fortunately, NVConnectorEvoRec::audioDevEldCase[] is zero-initialized,
 * which means each audioDevEldCase[] array element will have initial
 * value NV_ELD_PRE_MODESET=0.
 *
 * That ensures that nvRemoveUnusedHdmiDpAudioDevice(), during
 * the first modeset, will reset all unused audio device entries to
 * NV_ELD_POWER_ON_RESET.
 *
 * \param[in]  pDispEvo       The disp of the displayId
 * \param[in]  displayId      The display device whose ELD should be updated.
 *                            This should be NVDispHeadStateEvoRec::activeRmId
 *                            in case of NV_ELD_PRE_MODESET and
 *                            NV_ELD_POST_MODESET, otherwise it should be
 *                            NVConnectorEvoRec::displayId.
 * \param[in]  deviceEntry    The device entry of connector.
 * \param[in[  isDP           The DisplayPort display device.
 * \param[in]  pParsedEdid    The parsed edid from which ELD should be
 *                            extracted.
 * \param[in]  eldCase        The condition that requires updating the ELD.
 */

static void RmSetELDAudioCaps(
    const NVDispEvoRec *pDispEvo,
    NVConnectorEvoRec *pConnectorEvo,
    const NvU32 displayId,
    const NvU32 deviceEntry,
    const NvU32 maxFreqSupported, const NVEldEvoRec *pEld,
    const NvEldCase eldCase)
{
    NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS params = { 0 };
    NV2080_CTRL_OS_UNIX_AUDIO_DYNAMIC_POWER_PARAMS audio_power_params = { 0 };
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvBool isPD, isELDV;
    NvU32 ret;

    pConnectorEvo->audioDevEldCase[deviceEntry] = eldCase;

    /* setup the ctrl flag */
    switch(eldCase) {
        case NV_ELD_POWER_ON_RESET :
            isPD = isELDV = FALSE;
            break;
        case NV_ELD_PRE_MODESET :
            isPD = TRUE;
            isELDV = FALSE;
            break;
        case NV_ELD_POST_MODESET :
            isPD = isELDV = TRUE;
            break;
        default :
            return;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.deviceEntry = deviceEntry;
    params.displayId = displayId;

    if (isELDV) {
        if (pEld->size == 0) {
            isPD = isELDV = FALSE;
        } else {
            ct_assert(sizeof(params.bufferELD) == sizeof(pEld->buffer));

            nvkms_memcpy(params.bufferELD, pEld->buffer, sizeof(pEld->buffer));
            params.numELDSize = pEld->size;

            params.maxFreqSupported = maxFreqSupported;
        }
    } else {
        params.numELDSize = 0;
    }

    params.ctrl =
        DRF_NUM(0073_CTRL, _DFP_ELD_AUDIO_CAPS, _CTRL_PD, isPD)|
        DRF_NUM(0073_CTRL, _DFP_ELD_AUDIO_CAPS, _CTRL_ELDV, isELDV);

    /*
     * ELD information won't be populated to GPU HDA controller driver if
     * HDA controller is in suspended state.
     * Issue NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER RM control call for
     * bringing the HDA controller in active state before writing ELD. Once ELD
     * data is written, then HDA controller can again go into suspended state.
     */
    audio_power_params.bEnter = FALSE;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[pDispEvo->displayOwner]->handle,
                         NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER,
                         &audio_power_params, sizeof(audio_power_params));

    if (ret != NVOS_STATUS_SUCCESS)
        nvAssert(!"NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER failed");

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR, "NvRmControl"
            "(NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS) failed"
            "return status = %d...", ret);
    }

    audio_power_params.bEnter = TRUE;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[pDispEvo->displayOwner]->handle,
                         NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER,
                         &audio_power_params, sizeof(audio_power_params));

    if (ret != NVOS_STATUS_SUCCESS)
        nvAssert(!"NV2080_CTRL_CMD_OS_UNIX_AUDIO_DYNAMIC_POWER failed");

}

void nvHdmiDpEnableDisableAudio(const NVDispEvoRec *pDispEvo,
                                const NvU32 head, const NvBool enable)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NVConnectorEvoRec *pConnectorEvo = pHeadState->pConnectorEvo;
    const NvU32 deviceEntry = GetAudioDeviceEntry(pDispEvo, head);

    /*
     * We should only reach this function for active heads, and therefore
     * pConnectorEvo and deviceEntry are valid.
     */
    nvAssert((pHeadState->pConnectorEvo != NULL) &&
             (deviceEntry != NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_NONE));

    if (!pHeadState->audio.supported) {
        return;
    }

    if (!enable) {
        /*
         * This is pre modeset code path. If audio device is enabled
         * (pHeadState->audio.enabled == TRUE) then invalidate ELD buffer
         * before disabling audio.
         */
        if (pHeadState->audio.enabled) {
            RmSetELDAudioCaps(pDispEvo,
                              pConnectorEvo,
                              pHeadState->activeRmId,
                              deviceEntry,
                              0 /* maxFreqSupported */,
                              NULL /* pEld */,
                              NV_ELD_PRE_MODESET);

            if (nvConnectorUsesDPLib(pConnectorEvo)) {
                SetDpAudioEnable(pDispEvo, head, FALSE /* enable */);
            }
        }
    }

    if (pHeadState->audio.isAudioOverHdmi) {
        EnableHdmiAudio(pDispEvo, head, enable);
        SetHdmiAudioMute(pDispEvo, head, !enable /* mute */);
        SendHdmiGcp(pDispEvo, head, !enable /* avmute */);
    }

    if (enable) {
        /*
         * This is post modeset code path. If audio device is enabled
         * (pHeadState->audio.enabled == TRUE) then populate ELD buffer after
         * enabling audio, otherwise make sure to remove corresponding audio
         * device.
         */
        if (pHeadState->audio.enabled) {
            if (nvConnectorUsesDPLib(pConnectorEvo)) {
                SetDpAudioEnable(pDispEvo, head, TRUE /* enable */);
            }

            RmSetELDAudioCaps(pDispEvo,
                              pConnectorEvo,
                              pHeadState->activeRmId,
                              deviceEntry,
                              pHeadState->audio.maxFreqSupported,
                              &pHeadState->audio.eld,
                              NV_ELD_POST_MODESET);
        } else {
            RmSetELDAudioCaps(pDispEvo,
                              pConnectorEvo,
                              nvDpyIdToNvU32(pConnectorEvo->displayId),
                              deviceEntry,
                              0 /* maxFreqSupported */,
                              NULL /* pEld */,
                              NV_ELD_POWER_ON_RESET);
        }
    }
}

/*
 * Report HDMI capabilities to RM before modeset.
 */
void nvDpyUpdateHdmiPreModesetEvo(NVDpyEvoPtr pDpyEvo)
{
    if (!nvDpyIsHdmiEvo(pDpyEvo)) {
        pDpyEvo->pConnectorEvo->isHdmiEnabled = FALSE;
        return;
    }

    HdmiSendEnable(pDpyEvo, TRUE);
    pDpyEvo->pConnectorEvo->isHdmiEnabled = TRUE;
}

/*
 * Parse HDMI 2.1 VRR capabilities from the EDID and GPU.
 */
void nvDpyUpdateHdmiVRRCaps(NVDpyEvoPtr pDpyEvo)
{

    const NVParsedEdidEvoRec *pParsedEdid = &pDpyEvo->parsedEdid;
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    const NvBool gpuSupportsHDMIVRR = pDevEvo->hal->caps.supportsHDMIVRR;

    const NvBool dispSupportsVrr = nvDispSupportsVrr(pDispEvo) && 
                                       !nvkms_conceal_vrr_caps();

    const NvU32 edidVrrMin = pParsedEdid->info.hdmiForumInfo.vrr_min;

    nvAssert(pParsedEdid->valid);

    if (dispSupportsVrr && gpuSupportsHDMIVRR && (edidVrrMin > 0)) {
        if (nvDpyIsAdaptiveSyncDefaultlisted(pDpyEvo)) {
            pDpyEvo->vrr.type =
                NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_DEFAULTLISTED;
        } else {
            pDpyEvo->vrr.type =
                NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_NON_DEFAULTLISTED;
        }
    }
}

void nvRemoveUnusedHdmiDpAudioDevice(const NVDispEvoRec *pDispEvo)
{
    NVConnectorEvoRec *pConnectorEvo;
    const NvU32 activeSorMask = nvGetActiveSorMask(pDispEvo);

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        NvU32 deviceEntry;

        // Only connectors with assigned SORs can have audio.
        if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR ||
            pConnectorEvo->or.primary == NV_INVALID_OR) {
            continue;
        }

        // Check whether an active pConnectorEvo shares an SOR with this one.
        //
        // This is a workaround for the fact that
        // NV0073_CTRL_CMD_DFP_SET_ELD_AUDIO_CAPS takes a displayId rather than
        // an SOR index. See bug 1953489.
        if (nvIsConnectorActiveEvo(pConnectorEvo) &&
            (NVBIT(pConnectorEvo->or.primary) & activeSorMask) != 0x0) {
            continue;
        }

        for (deviceEntry = 0;
             deviceEntry < NV_MAX_AUDIO_DEVICE_ENTRIES;
             deviceEntry++) {

            /*
             * Skip if the audio device is enabled (ELD case is set to
             * NV_ELD_POST_MODESET by nvHdmiDpEnableDisableAudio()), or if the
             * audio device is already disabled (ELD case is set to
             * NV_ELD_POWER_ON_RESET).
             */
            if ((pConnectorEvo->audioDevEldCase[deviceEntry] ==
                        NV_ELD_POST_MODESET) ||
                    (pConnectorEvo->audioDevEldCase[deviceEntry] ==
                     NV_ELD_POWER_ON_RESET)) {
                continue;
            }

            RmSetELDAudioCaps(pDispEvo,
                              pConnectorEvo,
                              nvDpyIdToNvU32(pConnectorEvo->displayId),
                              deviceEntry,
                              0 /* maxFreqSupported */,
                              NULL /* pEld */,
                              NV_ELD_POWER_ON_RESET);
        }
    }
}

/*
 * Find the name of the given audio format, as described in the
 * CEA-861 specification's description of byte 1 in the Audio
 * Descriptor Block.  hasSampleSize and hasMaxBitRate (i.e., how to
 * interpret byte 3 of the Audio Descriptor Block) are a function of
 * audio format, so set them as a side effect of interpreting the
 * audio format.
 *
 * Note the return value is a const char * and should not be freed.
 */
static const char *GetCea861AudioFormatInfo(NvU8 format,
                                            NvBool *hasSampleSize,
                                            NvBool *hasMaxBitRate)
{
    static const struct {
        NvU8 format;
        NvBool hasSampleSize : 1;
        NvBool hasMaxBitRate : 1;
        const char *name;
    } audioFormatTable[] = {
        { NVT_CEA861_AUDIO_FORMAT_LINEAR_PCM, TRUE, FALSE,  "PCM"     },
        { NVT_CEA861_AUDIO_FORMAT_AC3,        FALSE, TRUE,  "AC-3"    },
        { NVT_CEA861_AUDIO_FORMAT_MPEG1,      FALSE, TRUE,  "MPEG-1"  },
        { NVT_CEA861_AUDIO_FORMAT_MP3,        FALSE, TRUE,  "MP3"     },
        { NVT_CEA861_AUDIO_FORMAT_MPEG2,      FALSE, TRUE,  "MPEG-2"  },
        { NVT_CEA861_AUDIO_FORMAT_AAC,        FALSE, TRUE,  "AAC"     },
        { NVT_CEA861_AUDIO_FORMAT_DTS,        FALSE, TRUE,  "DTS"     },
        { NVT_CEA861_AUDIO_FORMAT_ATRAC,      FALSE, TRUE,  "ATRAC"   },
        { NVT_CEA861_AUDIO_FORMAT_ONE_BIT,    FALSE, FALSE, "DSD"     },
        { NVT_CEA861_AUDIO_FORMAT_DDP,        FALSE, FALSE, "E-AC-3"  },
        { NVT_CEA861_AUDIO_FORMAT_DTS_HD,     FALSE, FALSE, "DTS-HD"  },
        { NVT_CEA861_AUDIO_FORMAT_MAT,        FALSE, FALSE, "MLP"     },
        { NVT_CEA861_AUDIO_FORMAT_DST,        FALSE, FALSE, "DSP"     },
        { NVT_CEA861_AUDIO_FORMAT_WMA_PRO,    FALSE, FALSE, "WMA Pro" },
    };

    int i;

    *hasSampleSize = FALSE;
    *hasMaxBitRate = FALSE;

    for (i = 0; i < ARRAY_LEN(audioFormatTable); i++) {
        if (format != audioFormatTable[i].format) {
            continue;
        }

        *hasSampleSize = audioFormatTable[i].hasSampleSize;
        *hasMaxBitRate = audioFormatTable[i].hasMaxBitRate;

        return audioFormatTable[i].name;
    }

    return "";
}


/*
 * Build a string description of the list of sample Rates, as
 * described in the CEA-861 specification's description of byte 2 in
 * the Audio Descriptor Block.
 *
 * Note the return value is a static char * and will be overwritten in
 * subsequent calls to this function.
 */
static const char *GetCea861AudioSampleRateString(NvU8 sampleRates)
{
    static const struct {
        NvU8 rate;
        const char *name;
    } sampleRateTable[] = {
        { NVT_CEA861_AUDIO_SAMPLE_RATE_32KHZ, "32KHz"  },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_44KHZ, "44KHz"  },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_48KHZ, "48KHz"  },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_88KHZ, "88KHz"  },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_96KHZ, "96KHz"  },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_176KHZ,"176KHz" },
        { NVT_CEA861_AUDIO_SAMPLE_RATE_192KHZ,"192KHz" },
    };

    static char sampleRateString[64];

    NvBool first = TRUE;
    int i;
    char *s;
    int ret, bytesLeft = sizeof(sampleRateString);

    sampleRateString[0] = '\0';
    s = sampleRateString;

    for (i = 0; i < ARRAY_LEN(sampleRateTable); i++) {
        if (sampleRates & sampleRateTable[i].rate) {
            if (first) {
                first = FALSE;
            } else {
                ret = nvkms_snprintf(s, bytesLeft, ", ");
                s += ret;
                bytesLeft -= ret;
            }
            ret = nvkms_snprintf(s, bytesLeft, "%s", sampleRateTable[i].name);
            s += ret;
            bytesLeft -= ret;
        }
    }

    nvAssert(bytesLeft >= 0);

    return sampleRateString;
}


/*
 * Build a string description of the list of sample sizes, as
 * described in the CEA-861 specification's description of byte 3 in
 * the Audio Descriptor Block.
 *
 * Note the return value is a static char * and will be overwritten in
 * subsequent calls to this function.
 */
static const char *GetCea861AudioSampleSizeString(NvU8 sampleSizes)
{
    static const struct {
        NvU8 bit;
        const char *name;
    } sampleSizeTable[] = {
        { NVT_CEA861_AUDIO_SAMPLE_SIZE_16BIT, "16-bits" },
        { NVT_CEA861_AUDIO_SAMPLE_SIZE_20BIT, "20-bits" },
        { NVT_CEA861_AUDIO_SAMPLE_SIZE_24BIT, "24-bits" },
    };

    static char sampleSizeString[64];

    NvBool first = TRUE;
    int i;
    char *s;
    int ret, bytesLeft = sizeof(sampleSizeString);

    sampleSizeString[0] = '\0';
    s = sampleSizeString;

    for (i = 0; i < ARRAY_LEN(sampleSizeTable); i++) {
        if (sampleSizes & sampleSizeTable[i].bit) {
            if (first) {
                first = FALSE;
            } else {
                ret = nvkms_snprintf(s, bytesLeft, ", ");
                s += ret;
                bytesLeft -= ret;
            }
            ret = nvkms_snprintf(s, bytesLeft, "%s", sampleSizeTable[i].name);
            s += ret;
            bytesLeft -= ret;
        }
    }

    nvAssert(bytesLeft >= 0);

    return sampleSizeString;
}


/*
 * Log the speaker allocation data block, as described in the CEA-861
 * specification.
 */
static void LogEdidCea861SpeakerAllocationData(NVEvoInfoStringPtr pInfoString,
                                               NvU8 speaker)
{
    if ((speaker & NVT_CEA861_SPEAKER_ALLOC_MASK) == 0) {
        return;
    }

    nvEvoLogInfoString(pInfoString,
                       "  Speaker Allocation Data    :");

    if (speaker & NVT_CEA861_SPEAKER_ALLOC_FL_FR) {
        nvEvoLogInfoString(pInfoString,
                           "   Front Left + Front Right");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_LFE) {
        nvEvoLogInfoString(pInfoString,
                           "   Low Frequency Effect");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_FC) {
        nvEvoLogInfoString(pInfoString,
                           "   Front Center");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_RL_RR) {
        nvEvoLogInfoString(pInfoString,
                           "   Rear Left + Rear Right");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_RC) {
        nvEvoLogInfoString(pInfoString,
                           "   Rear Center");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_FLC_FRC) {
        nvEvoLogInfoString(pInfoString,
                           "   Front Left Center + Front Right Center");
    }
    if (speaker & NVT_CEA861_SPEAKER_ALLOC_RLC_RRC) {
        nvEvoLogInfoString(pInfoString,
                           "   Rear Left Center + Rear Right Center");
    }
}


static void LogEdidCea861Info(NVEvoInfoStringPtr pInfoString,
                              const NVT_EDID_CEA861_INFO *pExt861)
{
    int vsdbIndex;
    int audioIndex;

    nvEvoLogInfoString(pInfoString,
                       " CEA-861 revision            : %d\n",
                       pExt861->revision);

    /*
     * IEEE vendor registration IDs are tracked here:
     * https://standards.ieee.org/develop/regauth/oui/oui.txt
     */
    for (vsdbIndex = 0; vsdbIndex < pExt861->total_vsdb; vsdbIndex++) {
        const NvU32 ieeeId = pExt861->vsdb[vsdbIndex].ieee_id;
        nvEvoLogInfoString(pInfoString,
                           "  IEEE Vendor Registration ID: %02x-%02x-%02x",
                           (ieeeId >> 16) & 0xFF,
                           (ieeeId >> 8) & 0xFF,
                           ieeeId & 0xFF);
    }

    nvEvoLogInfoString(pInfoString,
                       "  Supports YCbCr 4:4:4       : %s",
                       (pExt861->basic_caps & NVT_CEA861_CAP_YCbCr_444) ?
                       "Yes" : "No");

    nvEvoLogInfoString(pInfoString,
                       "  Supports YCbCr 4:2:2       : %s",
                       (pExt861->basic_caps & NVT_CEA861_CAP_YCbCr_422) ?
                       "Yes" : "No");

    nvEvoLogInfoString(pInfoString,
                       "  Supports Basic Audio       : %s",
                       (pExt861->basic_caps & NVT_CEA861_CAP_BASIC_AUDIO) ?
                       "Yes" : "No");

    for (audioIndex = 0; audioIndex < ARRAY_LEN(pExt861->audio); audioIndex++) {

        NvU32 byte1, byte2, byte3;
        NvU8 format;
        NvU8 maxChannels;
        NvU8 sampleRates;
        const char *formatString;
        NvBool hasSampleSize;
        NvBool hasMaxBitRate;

        byte1 = pExt861->audio[audioIndex].byte1;
        byte2 = pExt861->audio[audioIndex].byte2;
        byte3 = pExt861->audio[audioIndex].byte3;

        if (byte1 == 0) {
            break;
        }

        nvEvoLogInfoString(pInfoString,
                           "  Audio Descriptor           : %d", audioIndex);

        /*
         * byte 1 contains the Audio Format and the maximum number
         * of channels
         */

        format = ((byte1 & NVT_CEA861_AUDIO_FORMAT_MASK) >>
                  NVT_CEA861_AUDIO_FORMAT_SHIFT);

        formatString = GetCea861AudioFormatInfo(format,
                                                &hasSampleSize,
                                                &hasMaxBitRate);

        maxChannels = (byte1 & NVT_CEA861_AUDIO_MAX_CHANNEL_MASK) + 1;

        /* byte 2 contains the sample rates */

        sampleRates = (byte2 & NVT_CEA861_AUDIO_SAMPLE_RATE_MASK);

        /*
         * byte 3 varies, depending on Audio Format; interpret
         * using hasSampleSize and hasMaxBitRate
         */

        nvEvoLogInfoString(pInfoString,
                           "   Audio Format              : %s", formatString);
        nvEvoLogInfoString(pInfoString,
                           "   Maximum Channels          : %d", maxChannels);
        nvEvoLogInfoString(pInfoString,
                           "   Sample Rates              : %s",
                           GetCea861AudioSampleRateString(sampleRates));
        if (hasSampleSize) {
            nvEvoLogInfoString(pInfoString,
                               "   Sample Sizes              : %s",
                               GetCea861AudioSampleSizeString(byte3));
        }
        if (hasMaxBitRate) {
            nvEvoLogInfoString(pInfoString,
                               "   Maximum Bit Rate          : %d kHz",
                               byte3 * 8);
        }
    }

    LogEdidCea861SpeakerAllocationData(pInfoString, pExt861->speaker[0].byte1);
}

void nvLogEdidCea861InfoEvo(NVDpyEvoPtr pDpyEvo,
                            NVEvoInfoStringPtr pInfoString)
{
    int extIndex;

    for (extIndex = 0; TRUE; extIndex++) {
        const NVT_EDID_CEA861_INFO *pExt861 =
            GetExt861(&pDpyEvo->parsedEdid, extIndex);

        if (pExt861 == NULL) {
            break;
        }

        if (pExt861->revision == NVT_CEA861_REV_NONE) {
            continue;
        }

        nvEvoLogInfoString(pInfoString,
                           "CEA-861 extension block #    : %d\n", extIndex);

        LogEdidCea861Info(pInfoString, pExt861);
    }
}

/*
 * HDMI 2.0 4K@60hz uncompressed RGB 4:4:4 (6G mode) is allowed if:
 *
 * - The GPU supports it.
 * - The EDID and NVT_TIMING indicate the monitor supports it, or
 *   this check is overridden.
 */
NvBool nvHdmi204k60HzRGB444Allowed(const NVDpyEvoRec *pDpyEvo,
                                   const struct NvKmsModeValidationParams *pParams,
                                   const NVT_TIMING *pTiming)
{
    const NVParsedEdidEvoRec *pParsedEdid = &pDpyEvo->parsedEdid;
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;

    const NvBool gpuSupports444 = pDevEvo->caps.supportsHDMI20;

    const NvBool overrideMonitorCheck = ((pParams->overrides &
        NVKMS_MODE_VALIDATION_NO_HDMI2_CHECK) != 0);

    const NvBool monitorSupports444 =
        (IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.rgb444.bpcs) &&
         (pParsedEdid->info.hdmiForumInfo.max_TMDS_char_rate > 0));

    nvAssert(pParsedEdid->valid);

    return (gpuSupports444 &&
            (overrideMonitorCheck || monitorSupports444));
}

/*
 * Enable or disable HDMI 2.1 VRR infoframes.  The HDMI 2.1 VRR infoframe must
 * be enabled before the first extended vblank after enabling VRR, or the
 * display will blank.
 */
void nvHdmiSetVRR(NVDispEvoPtr pDispEvo, NvU32 head, NvBool enable)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME empInfoFrame;
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME_CTRL empCtrl = { 0 };
    NvEvoInfoFrameTransmitControl transmitCtrl = enable ?
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME :
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME;
    NVT_STATUS status;

    empCtrl.EnableVRR = enable;

    status = NvTiming_ConstructExtendedMetadataPacketInfoframe(&empCtrl,
                                                               &empInfoFrame);

    if (status != NVT_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                "Error in constructing Extended Metadata Packet InfoFrame");
        return;
    }

    // XXX Extended metadata infoframes do not contain a length header field.
    pDevEvo->hal->SendHdmiInfoFrame(
        pDispEvo,
        head,
        transmitCtrl,
        (NVT_INFOFRAME_HEADER *) &empInfoFrame,
        sizeof(empInfoFrame),
        FALSE /* needChecksum */);
}

/*
 * The HDMI library calls this function during initialization to ask the
 * implementation to allocate and map a NV*71_DISP_SF_USER object.  The
 * appropriate class, mapping size, and subdevice ID are provided.  A handle is
 * generated here and passed back to the library; the same handle is provided
 * in the symmetric HdmiLibRmFreeMemoryMap() function so we don't have to save
 * a copy of it in nvkms's data structures.
 */
static NvBool HdmiLibRmGetMemoryMap(
    NvHdmiPkt_CBHandle handle,
    NvU32 dispSfUserClassId,
    NvU32 dispSfUserSize,
    NvU32 sd,
    NvU32 *pDispSfHandle,
    void **pPtr)
{
    NVDevEvoRec *pDevEvo = handle;
    void *ptr = NULL;
    NvU32 ret;
    NvU32 dispSfHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (dispSfHandle == 0) {
        return FALSE;
    }

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->pSubDevices[sd]->handle,
                       dispSfHandle,
                       dispSfUserClassId,
                       NULL);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           dispSfHandle);
        return FALSE;
    }

    ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                           pDevEvo->pSubDevices[sd]->handle,
                           dispSfHandle,
                           0,
                           dispSfUserSize,
                           &ptr,
                           0);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->pSubDevices[sd]->handle,
                    dispSfHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           dispSfHandle);
        return FALSE;
    }

    *pDispSfHandle = dispSfHandle;
    *pPtr = ptr;

    return TRUE;
}

static void HdmiLibRmFreeMemoryMap(
    NvHdmiPkt_CBHandle handle,
    NvU32 sd,
    NvU32 dispSfHandle,
    void *ptr)
{
    NVDevEvoRec *pDevEvo = handle;
    NvU32 ret;

    if (ptr != NULL) {
        nvAssert(dispSfHandle != 0);
        ret = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                 pDevEvo->pSubDevices[sd]->handle,
                                 dispSfHandle,
                                 ptr,
                                 0);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(ret == NVOS_STATUS_SUCCESS);
        }
    }

    if (dispSfHandle) {
        ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                          pDevEvo->pSubDevices[sd]->handle,
                          dispSfHandle);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(ret == NVOS_STATUS_SUCCESS);
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           dispSfHandle);
    }
}

/* Wrapper around RmControl for 0073 (NV04_DISPLAY_COMMON) object. */
static NvBool HdmiLibRmDispControl(
    NvHdmiPkt_CBHandle handle,
    NvU32 subDevice,
    NvU32 cmd,
    void *pParams,
    NvU32 paramSize)
{
    NVDevEvoRec *pDevEvo = handle;
    NvU32 ret;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         cmd,
                         pParams,
                         paramSize);

    return ret == NVOS_STATUS_SUCCESS;
}

static void HdmiLibAcquireMutex(
    NvHdmiPkt_CBHandle handle)
{
    /* The HDMI library only executes when nvkms calls it, and nvkms will only
     * call it while holding the nvkms lock.  So there is no concurrency to
     * protect against with this mutex. */
}

static void HdmiLibReleaseMutex(
    NvHdmiPkt_CBHandle handle)
{
}

static void *HdmiLibMalloc(NvHdmiPkt_CBHandle handle, NvLength len)
{
    return nvAlloc(len);
}

static void HdmiLibFree(NvHdmiPkt_CBHandle handle, void *p)
{
    nvFree(p);
}

static void HdmiLibPrint(
    NvHdmiPkt_CBHandle handle,
    const char *format, ...)
{
    NVDevEvoRec *pDevEvo = handle;

    va_list ap;
    va_start(ap, format);
    /* The HDMI library doesn't have log levels, but currently only logs in
     * debug builds.  It's pretty chatty (e.g., it prints "Initialize Success"
     * when it inits), so hardcode it to INFO level for now. */
    nvVEvoLog(EVO_LOG_INFO, pDevEvo->gpuLogIndex, format, ap);
    va_end(ap);
}

static void HdmiLibAssert(
    const char *expr,
    const char *filename,
    const char *function,
    unsigned int line)
{
#ifdef DEBUG
    nvDebugAssert(expr, filename, function, line);
#endif
}

static NvU64 hdmiLibTimerStartTime = 0;
static NvU64 hdmiLibTimerTimeout = 0;

static NvBool HdmiLibSetTimeout(NvHdmiPkt_CBHandle handle,
                                NvU32 timeoutUs)
{
    hdmiLibTimerTimeout = timeoutUs;
    hdmiLibTimerStartTime = nvkms_get_usec();
    return TRUE;
}

static NvBool HdmiLibCheckTimeout(NvHdmiPkt_CBHandle handle)
{
    const NvU64 currentTime = nvkms_get_usec();
    if (currentTime < hdmiLibTimerStartTime) {
        return TRUE;
    }
    return (currentTime - hdmiLibTimerStartTime) > hdmiLibTimerTimeout;
}

static const NVHDMIPKT_CALLBACK HdmiLibCallbacks =
{
    .rmGetMemoryMap = HdmiLibRmGetMemoryMap,
    .rmFreeMemoryMap = HdmiLibRmFreeMemoryMap,
    .rmDispControl2 = HdmiLibRmDispControl,
    .acquireMutex = HdmiLibAcquireMutex,
    .releaseMutex = HdmiLibReleaseMutex,
    .setTimeout = HdmiLibSetTimeout,
    .checkTimeout = HdmiLibCheckTimeout,
    .malloc = HdmiLibMalloc,
    .free = HdmiLibFree,
    .print = HdmiLibPrint,
    .assert = HdmiLibAssert,
};

void nvTeardownHdmiLibrary(NVDevEvoRec *pDevEvo)
{
    NvHdmiPkt_DestroyLibrary(pDevEvo->hdmiLib.handle);
}

NvBool nvInitHdmiLibrary(NVDevEvoRec *pDevEvo)
{
    pDevEvo->hdmiLib.handle =
        NvHdmiPkt_InitializeLibrary(pDevEvo->dispClass,
                                    pDevEvo->numSubDevices,
                                    pDevEvo, // callback handle
                                    &HdmiLibCallbacks,
                                    0,     // not used because we set
                                    NULL); // NVHDMIPKT_RM_CALLS_INTERNAL=0

    if (pDevEvo->hdmiLib.handle == NVHDMIPKT_INVALID_HANDLE) {
        pDevEvo->hdmiLib.handle = NULL;
        return FALSE;
    }

    return TRUE;
}

/*
 * Call the HDMI library to "assess" the link.  This basically does link
 * training to see what the maximum lane configuration is.  We do this when the
 * monitor is connected after reading the EDID, so we can validate modes
 * against the link capabilities.
 *
 * Returns true if the link was assessed to be capable of FRL, and false
 * otherwise.
 */
NvBool nvHdmiFrlAssessLink(NVDpyEvoPtr pDpyEvo)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVHDMIPKT_RESULT ret;
    const NvU32 displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);

    nvAssert(nvDpyIsHdmiEvo(pDpyEvo));

    /* HDMI dpys not dynamic dpy so its connector should have a dpyId. */
    nvAssert(displayId != 0);
    nvAssert(pDpyEvo->parsedEdid.valid);

    ret = NvHdmi_AssessLinkCapabilities(pDevEvo->hdmiLib.handle,
                                        pDispEvo->displayOwner,
                                        displayId,
                                        &pDpyEvo->parsedEdid.info,
                                        &pDpyEvo->hdmi.srcCaps,
                                        &pDpyEvo->hdmi.sinkCaps);
    if (ret != NVHDMIPKT_SUCCESS) {
        nvAssert(ret == NVHDMIPKT_SUCCESS);
        return FALSE;
    }

    return pDpyEvo->hdmi.sinkCaps.linkMaxFRLRate != HDMI_FRL_DATA_RATE_NONE;
}

/*
 * Determine if the given HDMI dpy supports FRL.
 *
 * Returns TRUE if the dpy supports FRL, or FALSE otherwise.
 */
NvBool nvHdmiDpySupportsFrl(const NVDpyEvoRec *pDpyEvo)
{
    NvU32 passiveDpDongleMaxPclkKHz;
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;

    nvAssert(nvDpyIsHdmiEvo(pDpyEvo));

    /* Can't use FRL if disabled by kernel module param. */
    if (nvkms_disable_hdmi_frl()) {
        return FALSE;
    }

    /* Can't use FRL if the display hardware doesn't support it. */
    if (!pDevEvo->hal->caps.supportsHDMIFRL) {
        return FALSE;
    }

    /* Can't use FRL if the HDMI sink doesn't support it. */
    if (!pDpyEvo->parsedEdid.valid ||
        !pDpyEvo->parsedEdid.info.hdmiForumInfo.max_FRL_Rate) {
        return FALSE;
    }

    /* Can't use FRL if we are using a passive DP to HDMI dongle. */
    if (nvDpyGetPassiveDpDongleType(pDpyEvo, &passiveDpDongleMaxPclkKHz) !=
        NV_EVO_PASSIVE_DP_DONGLE_UNUSED) {
        return FALSE;
    }

    return TRUE;
}

NvU32 nvHdmiGetEffectivePixelClockKHz(const NVDpyEvoRec *pDpyEvo,
                                       const NVHwModeTimingsEvo *pHwTimings,
                                       const NVDpyAttributeColor *pDpyColor)
{
    const NvU32 pixelClock = (pHwTimings->yuv420Mode == NV_YUV420_MODE_HW) ?
        (pHwTimings->pixelClock / 2) : pHwTimings->pixelClock;

    nvAssert((pHwTimings->yuv420Mode == NV_YUV420_MODE_NONE) ||
                (pDpyColor->format ==
                 NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420));
    nvAssert(nvDpyIsHdmiEvo(pDpyEvo));
    nvAssert(pDpyColor->bpc >= NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8);

    /* YCbCr422 does not change the effective pixel clock. */
    if (pDpyColor->format ==
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422) {
        return pixelClock;
    }

    /*
     * For > 8 BPC, the effective pixel clock is adjusted upwards according to
     * the ratio of the given BPC and 8 BPC.
     */
    return ((pixelClock * pDpyColor->bpc) / 8ULL);
}

static NvU64 GetHdmiFrlLinkRate(HDMI_FRL_DATA_RATE frlRate)
{
    const NvU64 giga = 1000000000ULL;
    NvU64 hdmiLinkRate = 0;
    switch(frlRate )
    {
        case HDMI_FRL_DATA_RATE_NONE:
            hdmiLinkRate = 0;
            break;
        case HDMI_FRL_DATA_RATE_3LANES_3GBPS:
            hdmiLinkRate =  3 * giga;
            break;
        case HDMI_FRL_DATA_RATE_3LANES_6GBPS:
        case HDMI_FRL_DATA_RATE_4LANES_6GBPS:
            hdmiLinkRate =  6 * giga;
            break;
        case HDMI_FRL_DATA_RATE_4LANES_8GBPS:
            hdmiLinkRate =  8 * giga;
            break;
        case HDMI_FRL_DATA_RATE_4LANES_10GBPS:
            hdmiLinkRate = 10 * giga;
            break;
        case HDMI_FRL_DATA_RATE_4LANES_12GBPS:
            hdmiLinkRate = 12 * giga;
            break;
        case HDMI_FRL_DATA_RATE_UNSPECIFIED:
            nvAssert(!"Unspecified FRL data rate");
            break;
    };

    return hdmiLinkRate;
}

static NvBool nvHdmiFrlQueryConfigOneBpc(
    const NVDpyEvoRec *pDpyEvo,
    const NvModeTimings *pModeTimings,
    const NVHwModeTimingsEvo *pHwTimings,
    const NVDpyAttributeColor *pDpyColor,
    const NvBool b2Heads1Or,
    const struct NvKmsModeValidationParams *pValidationParams,
    HDMI_FRL_CONFIG *pConfig,
    NVDscInfoEvoRec *pDscInfo)
{
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    HDMI_VIDEO_TRANSPORT_INFO videoTransportInfo = { };
    HDMI_QUERY_FRL_CLIENT_CONTROL clientControl = { };
    const NVT_TIMING *pNvtTiming;
    NVT_TIMING nvtTiming = { };
    NVHDMIPKT_RESULT ret;

    if (pHwTimings->protocol != NVKMS_PROTOCOL_SOR_HDMI_FRL) {
        nvkms_memset(pDscInfo, 0, sizeof(*pDscInfo));
        nvkms_memset(pConfig, 0, sizeof(*pConfig));
        return TRUE;
    }

    nvAssert(nvDpyIsHdmiEvo(pDpyEvo));
    nvAssert(nvHdmiDpySupportsFrl(pDpyEvo));
    nvAssert(nvHdmiGetEffectivePixelClockKHz(pDpyEvo, pHwTimings, pDpyColor) >
                pDpyEvo->maxSingleLinkPixelClockKHz);

    /* See if we can find an NVT_TIMING for this mode from the EDID. */
    pNvtTiming = nvFindEdidNVT_TIMING(pDpyEvo, pModeTimings, pValidationParams);

    if (pNvtTiming == NULL) {
        /*
         * No luck finding this mode in the EDID.
         *
         * Construct enough of an NVT_TIMING for the hdmi library, based on the
         * pHwTimings mode.
         *
         * The HDMI library's hdmiQueryFRLConfigC671 uses:
         * - pVidTransInfo->pTiming->pclk
         * - pVidTransInfo->pTiming->HTotal
         * - pVidTransInfo->pTiming->HVisible
         * - pVidTransInfo->pTiming->VVisible
         *
         * This is also used, although we don't have a CEA format so we just
         * set it to 0:
         * - NVT_GET_CEA_FORMAT(pVidTransInfo->pTiming->etc.status)
         */

        /* Convert from KHz to 10KHz; round up for the purposes of determining a
         * minimum FRL rate. */
        nvtTiming.pclk = (pHwTimings->pixelClock + 9) / 10;
        nvtTiming.HVisible = pHwTimings->rasterBlankStart.x -
                             pHwTimings->rasterBlankEnd.x;
        nvtTiming.HTotal = pHwTimings->rasterSize.x;
        nvtTiming.VVisible = pHwTimings->rasterBlankStart.y -
                             pHwTimings->rasterBlankEnd.y;
        nvtTiming.etc.status = 0;

        pNvtTiming = &nvtTiming;
    }

    videoTransportInfo.pTiming = pNvtTiming;
    /*
     * pTimings->pixelDepth isn't assigned yet at this point in mode
     * validation, so we can't use that.
     * This matches the non-DP default assigned later in
     * nvConstructHwModeTimingsEvo().
     */
    switch(pDpyColor->bpc) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
            videoTransportInfo.bpc = HDMI_BPC10;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
            videoTransportInfo.bpc = HDMI_BPC8;
            break;
        default:
            return FALSE;
    }

    /* TODO: support YUV/YCbCr 444 and 422 packing modes. */
    switch (pModeTimings->yuv420Mode) {
        case NV_YUV420_MODE_NONE:
            videoTransportInfo.packing = HDMI_PIXEL_PACKING_RGB;
            break;
        case NV_YUV420_MODE_SW:
            /*
             * Don't bother implementing this with FRL.
             * HDMI FRL and HW YUV420 support were both added in nvdisplay 4.0
             * hardware, so if the hardware supports FRL it should support
             * YUV420_MODE_HW.
             */
            return FALSE;
        case NV_YUV420_MODE_HW:
            videoTransportInfo.packing = HDMI_PIXEL_PACKING_YCbCr420;
            break;
    }

    videoTransportInfo.bDualHeadMode = b2Heads1Or;

    clientControl.option = HDMI_QUERY_FRL_HIGHEST_PIXEL_QUALITY;

    if (pValidationParams->dscMode == NVKMS_DSC_MODE_FORCE_ENABLE) {
        clientControl.enableDSC = TRUE;
    }

    /*
     * 2Heads1Or requires either YUV420 or DSC; if b2Heads1Or is enabled
     * but YUV420 is not, force DSC.
     */
    if (b2Heads1Or && (pHwTimings->yuv420Mode != NV_YUV420_MODE_HW)) {
        if (pValidationParams->dscMode == NVKMS_DSC_MODE_FORCE_DISABLE) {
            return FALSE;
        }
        clientControl.enableDSC = TRUE;
    }

    if (pValidationParams->dscOverrideBitsPerPixelX16 != 0) {
        clientControl.forceBppx16 = TRUE;
        clientControl.bitsPerPixelX16 =
            pValidationParams->dscOverrideBitsPerPixelX16;
    }

    ret = NvHdmi_QueryFRLConfig(pDevEvo->hdmiLib.handle,
                                &videoTransportInfo,
                                &clientControl,
                                &pDpyEvo->hdmi.srcCaps,
                                &pDpyEvo->hdmi.sinkCaps,
                                pConfig);

    if ((ret == NVHDMIPKT_SUCCESS) && b2Heads1Or) {
        /*
         * 2Heads1Or requires either YUV420 or DSC; pConfig->dscInfo.bEnableDSC
         * is assigned by NvHdmi_QueryFRLConfig().
         */
        nvAssert(pConfig->dscInfo.bEnableDSC ||
                    (pHwTimings->yuv420Mode == NV_YUV420_MODE_HW));
    }

    if (ret == NVHDMIPKT_SUCCESS) {
        if (pDscInfo != NULL) {
            const NvU64 hdmiLinkRate = GetHdmiFrlLinkRate(pConfig->frlRate);

            nvAssert((hdmiLinkRate != 0) ||
                        (pConfig->frlRate == HDMI_FRL_DATA_RATE_NONE));

            nvkms_memset(pDscInfo, 0, sizeof(*pDscInfo));

            if ((pConfig->frlRate != HDMI_FRL_DATA_RATE_NONE) &&
                    pConfig->dscInfo.bEnableDSC &&
                    (hdmiLinkRate != 0)) {

                if (pValidationParams->dscMode ==
                        NVKMS_DSC_MODE_FORCE_DISABLE) {
                    ret = NVHDMIPKT_FAIL;
                    goto done;
                }

                pDscInfo->type = NV_DSC_INFO_EVO_TYPE_HDMI;
                pDscInfo->sliceCount = pConfig->dscInfo.sliceCount;
                /*
                 * XXX NvHdmi_QueryFRLConfig() might get updated in future, but
                 * today it does not return the possible DSC slice counts.
                 * NvHdmi_QueryFRLConfig() returns the slice count which it has
                 * been used to calculate DSC PPS, populate
                 * 'possibleSliceCountMask' using that slice count.
                 */
                pDscInfo->possibleSliceCountMask =
                    NVBIT(pDscInfo->sliceCount - 1);
                pDscInfo->hdmi.dscMode = b2Heads1Or ?
                    NV_DSC_EVO_MODE_DUAL : NV_DSC_EVO_MODE_SINGLE;
                pDscInfo->hdmi.bitsPerPixelX16 =
                    pConfig->dscInfo.bitsPerPixelX16;
                ct_assert(sizeof(pDscInfo->hdmi.pps) ==
                          sizeof(pConfig->dscInfo.pps));
                nvkms_memcpy(pDscInfo->hdmi.pps,
                             pConfig->dscInfo.pps,
                             sizeof(pDscInfo->hdmi.pps));
                pDscInfo->hdmi.dscHActiveBytes =
                    pConfig->dscInfo.dscHActiveBytes;
                pDscInfo->hdmi.dscHActiveTriBytes =
                    pConfig->dscInfo.dscHActiveTriBytes;
                pDscInfo->hdmi.dscHBlankTriBytes =
                    pConfig->dscInfo.dscHBlankTriBytes;
                pDscInfo->hdmi.dscTBlankToTTotalRatioX1k =
                    pConfig->dscInfo.dscTBlankToTTotalRatioX1k;
                pDscInfo->hdmi.hblankMin =
                    NV_UNSIGNED_DIV_CEIL(((pHwTimings->pixelClock * 1000) *
                                              pConfig->dscInfo.dscHBlankTriBytes),
                                         (hdmiLinkRate / 6));
            } else {
                pDscInfo->type = NV_DSC_INFO_EVO_TYPE_DISABLED;
            }
        }
    }

done:
    return ret == NVHDMIPKT_SUCCESS;
}

void nvHdmiFrlClearConfig(NVDispEvoRec *pDispEvo, NvU32 activeRmId)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    NVHDMIPKT_RESULT ret;
    ret = NvHdmi_ClearFRLConfig(pDevEvo->hdmiLib.handle,
                                pDispEvo->displayOwner, activeRmId);
    if (ret != NVHDMIPKT_SUCCESS) {
        nvAssert(ret == NVHDMIPKT_SUCCESS);
    }
}

NvBool nvHdmiFrlQueryConfig(
    const NVDpyEvoRec *pDpyEvo,
    const NvModeTimings *pModeTimings,
    const NVHwModeTimingsEvo *pHwTimings,
    NVDpyAttributeColor *pDpyColor,
    const NvBool b2Heads1Or,
    const struct NvKmsModeValidationParams *pValidationParams,
    HDMI_FRL_CONFIG *pConfig,
    NVDscInfoEvoRec *pDscInfo)
{
    const NvKmsDpyOutputColorFormatInfo supportedColorFormats =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);
    NVDpyAttributeColor dpyColor = *pDpyColor;
    do {
        if (nvHdmiFrlQueryConfigOneBpc(pDpyEvo,
                                       pModeTimings,
                                       pHwTimings,
                                       &dpyColor,
                                       b2Heads1Or,
                                       pValidationParams,
                                       pConfig,
                                       pDscInfo)) {
            *pDpyColor = dpyColor;
            return TRUE;
        }
    } while(nvDowngradeColorBpc(&supportedColorFormats, &dpyColor) &&
                (dpyColor.bpc >= NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8));
    return FALSE;
}

void nvHdmiFrlSetConfig(NVDispEvoRec *pDispEvo, NvU32 head)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVDispHeadStateEvoRec *pHeadState =
        &pDispEvo->headState[head];
    HDMI_FRL_CONFIG *pFrlConfig = &pHeadState->hdmiFrlConfig;
    NVHDMIPKT_RESULT ret;
    NvU32 retries = 0;
    const NvU32 MAX_RETRIES = 5;

    if (pFrlConfig->frlRate == HDMI_FRL_DATA_RATE_NONE) {
        return;
    }

    nvAssert(pHeadState->activeRmId != 0);

    do {
        ret = NvHdmi_SetFRLConfig(pDevEvo->hdmiLib.handle,
                                  pDispEvo->displayOwner,
                                  pHeadState->activeRmId,
                                  NV_FALSE /* bFakeLt */,
                                  pFrlConfig);
    } while (ret != NVHDMIPKT_SUCCESS && retries++ < MAX_RETRIES);

    if (ret != NVHDMIPKT_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "HDMI FRL link training failed.");

        /*
         * Link training failed even after retrying.  Since link training
         * happens after we've already committed to a modeset and failing is
         * not an option, try one last time with the 'bFakeLt' parameter
         * set, which should enable enough of the display hardware to
         * prevent hangs when we attempt to drive the OR with
         * PROTOCOL_HDMI_FRL.
         */
        ret = NvHdmi_SetFRLConfig(pDevEvo->hdmiLib.handle,
                                  pDispEvo->displayOwner,
                                  pHeadState->activeRmId,
                                  NV_TRUE /* bFakeLt */,
                                  pFrlConfig);

        if (ret != NVHDMIPKT_SUCCESS) {
            nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                         "HDMI FRL fallback link training failed.");
        }
    }

    if (retries != 0) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_WARN,
                          "HDMI FRL link training retried %d times.",
                          retries);
    }
}
