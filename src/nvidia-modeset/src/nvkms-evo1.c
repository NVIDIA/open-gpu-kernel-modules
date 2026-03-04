/*
 * SPDX-FileCopyrightText: Copyright (c) 2010 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * This file contains implementations of the EVO HAL methods for display class
 * 1.x, found in the Tesla and Fermi 1 (GF10x) chips.
 */

#include "nvkms-types.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-evo1.h"
#include "nvkms-prealloc.h"
#include "nvkms-utils.h"

#include "hdmi_spec.h"

/*!
 * Return the value to use for HEAD_SET_STORAGE_PITCH.
 *
 * Per dispClass_02.mfs, the HEAD_SET_STORAGE_PITCH "units are blocks
 * if the layout is BLOCKLINEAR, the units are multiples of 256 bytes
 * if the layout is PITCH."
 *
 * \return   Returns 0 if the pitch is invalid.  Otherwise returns the
 *           HEAD_SET_STORAGE_PITCH value.
 */
NvU32 nvEvoGetHeadSetStoragePitchValue(const NVDevEvoRec *pDevEvo,
                                       enum NvKmsSurfaceMemoryLayout layout,
                                       NvU32 pitch)
{
    if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        /* pitch is already in units of blocks; nothing else needed. */
    } else {
        /* pitch is in units of bytes, and must be aligned to 0x100. */
        if ((pitch & 0xFF) != 0) {
            return 0;
        }

        pitch >>= 8;
    }

    if (pitch > pDevEvo->caps.maxPitchValue) {
        return 0;
    }

    return pitch;
}

/*
 * The 'type' the timing library writes into the NVT_INFOFRAME_HEADER
 * structure is not the same as the protocol values that hardware
 * expects to see in the real packet header; those are defined in the
 * HDMI_PACKET_TYPE enums (hdmi_pktType_*) from hdmi_spec.h; use those
 * to fill in the first byte of the packet.
 */
NvBool nvEvo1NvtToHdmiInfoFramePacketType(const NvU32 srcType, NvU8 *pDstType)
{
    NvU8 hdmiPacketType;

    switch (srcType) {
        default:
            nvAssert(!"unsupported packet type");
            return FALSE;
        case NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET:
            hdmiPacketType = hdmi_pktType_ExtendedMetadata;
            break;
        case NVT_INFOFRAME_TYPE_VIDEO:
            hdmiPacketType = hdmi_pktType_AviInfoFrame;
            break;
        case NVT_INFOFRAME_TYPE_VENDOR_SPECIFIC:
            hdmiPacketType = hdmi_pktType_VendorSpecInfoFrame;
            break;
        case NVT_INFOFRAME_TYPE_DYNAMIC_RANGE_MASTERING:
            hdmiPacketType = hdmi_pktType_DynamicRangeMasteringInfoFrame;
            break;
    }

    *pDstType = hdmiPacketType;

    return TRUE;
}

static NVHDMIPKT_TC EvoInfoFrameToHdmiLibTransmitCtrl(
    NvEvoInfoFrameTransmitControl src,
    NvBool needChecksum)
{
    NVHDMIPKT_TC hdmiLibTransmitCtrl =
        NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_EVERY_FRAME;

    switch (src) {
        case NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME:
            hdmiLibTransmitCtrl = NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_SINGLE_FRAME;
            break;
        case NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME:
            hdmiLibTransmitCtrl = NVHDMIPKT_TRANSMIT_CONTROL_ENABLE_EVERY_FRAME;
            break;
    }

    if (!needChecksum) {
        hdmiLibTransmitCtrl &=
            ~DRF_DEF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN);
    }

    return hdmiLibTransmitCtrl;
}

void nvEvo1SendHdmiInfoFrame(const NVDispEvoRec *pDispEvo,
                             const NvU32 head,
                             const NvEvoInfoFrameTransmitControl transmitCtrl,
                             const NVT_INFOFRAME_HEADER *pInfoFrameHeader,
                             const NvU32 infoframeSize,
                             NvBool needChecksum)
{
    NVHDMIPKT_TC hdmiLibTransmitCtrl =
        EvoInfoFrameToHdmiLibTransmitCtrl(transmitCtrl, needChecksum);
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVHDMIPKT_TYPE hdmiLibType;
    NVHDMIPKT_RESULT ret;
    NvU8 *infoframe = NULL;
    NvU8 hdmiPacketType, checksum;
    NvU32 i;
    const NvU8 *pPayload;
    size_t headerSize;

    /*
     * The 'type' the timing library is not the type that the HDMI
     * library expects to see in its NvHdmiPkt_PacketWrite call; those
     * are NVHDMIPKT_TYPE_*.  Determine both below.
     */
    switch (pInfoFrameHeader->type) {
        default:
            nvAssert(0);
            return;
        case NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET:
            hdmiLibType = NVHDMIPKT_TYPE_GENERIC;
            break;
        case NVT_INFOFRAME_TYPE_VIDEO:
            hdmiLibType = NVHDMIPKT_TYPE_AVI_INFOFRAME;
            break;
        case NVT_INFOFRAME_TYPE_VENDOR_SPECIFIC:
            hdmiLibType = NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME;
            break;
        case NVT_INFOFRAME_TYPE_DYNAMIC_RANGE_MASTERING:
            hdmiLibType = NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME;
            break;
    }

    if (!nvEvo1NvtToHdmiInfoFramePacketType(pInfoFrameHeader->type,
                                            &hdmiPacketType)) {
        return;
    }

    /*
     * These structures are weird. The NVT_VIDEO_INFOFRAME,
     * NVT_VENDOR_SPECIFIC_INFOFRAME, NVT_EXTENDED_METADATA_PACKET_INFOFRAME,
     * etc structures are *kind of* what we want to send to the hdmipkt library,
     * except the type in the header is different, and a single checksum byte
     * may need to be inserted *between* the header and the payload (requiring
     * us to allocate a buffer one byte larger).
     */
    infoframe = nvAlloc(infoframeSize + (needChecksum ? sizeof(checksum) : 0));
    if (infoframe == NULL) {
        return;
    }

    /*
     * The fields and size of NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER
     * match with those of NVT_INFOFRAME_HEADER at the time of writing, but
     * nvtiming.h declares them separately. To be safe, special case
     * NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET.
     */
    if (pInfoFrameHeader->type == NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET) {
        const NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER *pExtMetadataHeader =
            (const NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER *) pInfoFrameHeader;

        pPayload = (const NvU8 *)(pExtMetadataHeader + 1);
        headerSize = sizeof(NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER);
    } else {
        pPayload = (const NvU8 *)(pInfoFrameHeader + 1);
        headerSize = sizeof(NVT_INFOFRAME_HEADER);
    }

    infoframe[0] = hdmiPacketType;
    nvkms_memcpy(&infoframe[1], &((const NvU8*) pInfoFrameHeader)[1],
        headerSize - 1);

    /* copy the payload, starting after the 3-byte header and checksum */
    nvkms_memcpy(&infoframe[headerSize + (needChecksum ? sizeof(checksum) : 0)],
                 pPayload, infoframeSize - headerSize /* payload size */);

    /*
     * XXX Redundant since needsChecksum implies
     * _HDMI_PKT_TRANSMIT_CTRL_CHKSUM_HW_EN via
     * EvoInfoFrameToHdmiLibTransmitCtrl()?
     */
    if (needChecksum) {
        /* PB0: checksum */
        checksum = 0;
        infoframe[headerSize] = 0;
        for (i = 0; i < infoframeSize + sizeof(checksum); i++) {
            checksum += infoframe[i];
        }
        infoframe[headerSize] = ~checksum + 1;
    }

    ret = NvHdmiPkt_PacketWrite(pDevEvo->hdmiLib.handle,
                                pDispEvo->displayOwner,
                                pHeadState->activeRmId,
                                head,
                                hdmiLibType,
                                hdmiLibTransmitCtrl,
                                infoframeSize,
                                infoframe);

    if (ret != NVHDMIPKT_SUCCESS) {
        nvAssert(ret == NVHDMIPKT_SUCCESS);
    }

    nvFree(infoframe);
}

void nvEvo1DisableHdmiInfoFrame(const NVDispEvoRec *pDispEvo,
                                const NvU32 head,
                                const NvU8 nvtInfoFrameType)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU8 hdmiPacketType;
    NvU32 ret;

    if (!nvEvo1NvtToHdmiInfoFramePacketType(nvtInfoFrameType,
                                            &hdmiPacketType)) {
        return;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;
    params.type = hdmiPacketType;
    params.transmitControl = DRF_DEF(0073_CTRL_SPECIFIC,
        _SET_OD_PACKET_CTRL_TRANSMIT_CONTROL, _ENABLE, _NO);


    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET_CTRL,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET_CTRL failed");
    }
}

void nvEvo1SendDpInfoFrameSdp(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NvEvoInfoFrameTransmitControl transmitCtrl,
                              const DPSDP_DESCRIPTOR *sdp)
{
    NvU32 ret;
    const NVDispHeadStateEvoRec *pHeadState =
                                &pDispEvo->headState[head];
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS params = {
        .subDeviceInstance = pDispEvo->displayOwner,
        .displayId = pHeadState->activeRmId,
    };

    switch (transmitCtrl) {
        case NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME:
            params.transmitControl =
                DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL,
                        _SINGLE_FRAME, _DISABLE);
            break;
        case NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME:
            params.transmitControl =
                DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL,
                        _SINGLE_FRAME, _ENABLE);
            break;
    }

    params.transmitControl |=
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _OTHER_FRAME,  _DISABLE) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ENABLE,       _YES)     |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _GEN_INFOFRAME_MODE, _INFOFRAME1) |
        DRF_DEF(0073_CTRL_SPECIFIC, _SET_OD_PACKET_TRANSMIT_CONTROL, _ON_HBLANK,    _DISABLE);

    nvAssert((sizeof(sdp->hb) + sdp->dataSize) <= sizeof(params.aPacket));

    params.packetSize = NV_MIN((sizeof(sdp->hb) + sdp->dataSize),
        sizeof(params.aPacket));
    nvkms_memcpy(params.aPacket, &sdp->hb, params.packetSize);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET failed");
    }
}
