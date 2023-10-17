/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 * File:      nvhdmipkt_0073.c
 *
 * Purpose:   Provides infoframe write functions for HDMI library  for Pre-KEPLER chips
 */

#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"

#include "hdmi_spec.h"
#include "ctrl/ctrl0073/ctrl0073specific.h"

NVHDMIPKT_RESULT
hdmiPacketCtrl0073(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl);

NVHDMIPKT_RESULT
hdmiPacketWrite0073(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacket);

/*
 * hdmiPacketCtrl0073
 */
NVHDMIPKT_RESULT
hdmiPacketCtrl0073(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS params = {0};

    NVMISC_MEMSET(&params, 0, sizeof(params));

    params.subDeviceInstance = subDevice;
    params.displayId = displayId;
    params.type  = pThis->translatePacketType(pThis, packetType);
    params.transmitControl = pThis->translateTransmitControl(pThis, transmitControl);

#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (NvRmControl(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET_CTRL,
                    &params,
                    sizeof(params)) != NVOS_STATUS_SUCCESS)

#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    NvBool bSuccess =  pThis->callback.rmDispControl2(pThis->cbHandle,
                                                      params.subDeviceInstance,
                                                      NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET_CTRL, 
                                                      &params, sizeof(params));
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        NvHdmiPkt_Print(pThis, "ERROR - RM call to hdmiPacketCtrl failed.");
        NvHdmiPkt_Assert(0);
        result = NVHDMIPKT_FAIL;
    }

    return result;
}

/*
 * hdmiPacketWrite0073
 */
NVHDMIPKT_RESULT
hdmiPacketWrite0073(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacket)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;

    if (packetLen > NV0073_CTRL_SET_OD_MAX_PACKET_SIZE)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS params = {0};

    NVMISC_MEMSET(&params, 0, sizeof(params));

    params.subDeviceInstance = subDevice;
    params.displayId = displayId;
    params.packetSize = packetLen;
    params.transmitControl = pThis->translateTransmitControl(pThis, transmitControl);

    // init the infoframe packet
    NVMISC_MEMSET(params.aPacket, 0, NV0073_CTRL_SET_OD_MAX_PACKET_SIZE);

    // copy the payload
    NVMISC_MEMCPY(params.aPacket, pPacket, packetLen);

#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (NvRmControl(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET,
                    &params,
                    sizeof(params)) != NVOS_STATUS_SUCCESS)

#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                     params.subDeviceInstance,
                                                     NV0073_CTRL_CMD_SPECIFIC_SET_OD_PACKET, 
                                                     &params, 
                                                     sizeof(params));
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        NvHdmiPkt_Print(pThis, "ERROR - RM call to hdmiPacketWrite failed.");
        NvHdmiPkt_Assert(0);
        result = NVHDMIPKT_FAIL;
    }

    return result;
}

/*
 * translatePacketType0073
 */
static NvU32
translatePacketType0073(NVHDMIPKT_CLASS*  pThis,
                        NVHDMIPKT_TYPE    packetType)
{
    NvU32 type0073 = 0;

    switch (packetType)
    {
    case NVHDMIPKT_TYPE_AVI_INFOFRAME:
        type0073 = pktType_AviInfoFrame;
        break;

    case NVHDMIPKT_TYPE_GENERIC:
        type0073 = pktType_GamutMetadata;
        break;

    case NVHDMIPKT_TYPE_GENERAL_CONTROL:
        type0073 = pktType_GeneralControl;
        break;

    case NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME:
        type0073 = pktType_VendorSpecInfoFrame;
        break;

    case NVHDMIPKT_TYPE_AUDIO_INFOFRAME:
        type0073 = pktType_AudioInfoFrame;
        break;

    default:
        NvHdmiPkt_Print(pThis, "ERROR - translatePacketType wrong packet type: %0x",
                         packetType);
        NvHdmiPkt_Assert(0);
        break;
    }

    return type0073;
}

/*
 * translateTransmitControl0073
 */
static NvU32
translateTransmitControl0073(NVHDMIPKT_CLASS*  pThis,
                             NVHDMIPKT_TC      transmitControl)
{
    NvU32 tc = 0;

    // TODO: tc validation
    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(0073, _CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL,
                         _ENABLE, _YES, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(0073, _CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL,
                         _OTHER_FRAME, _ENABLE, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(0073, _CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL,
                         _SINGLE_FRAME, _ENABLE, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _HBLANK, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(0073, _CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL,
                         _ON_HBLANK, _ENABLE, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _VIDEO_FMT, _HW_CTRL, transmitControl))
    {
        tc = FLD_SET_DRF(0073, _CTRL_SPECIFIC_SET_OD_PACKET_CTRL_TRANSMIT_CONTROL,
                         _VIDEO_FMT, _HW_CONTROLLED, tc);
    }

    return tc;
}

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructor0073
 */
NvBool
hdmiConstructor0073(NVHDMIPKT_CLASS* pThis)
{
    return NV_TRUE;
}

/*
 * hdmiUnDestructor0073
 */
void
hdmiDestructor0073(NVHDMIPKT_CLASS* pThis)

{
    return;
}

// Below are dummy functions for the HW functions not needed for a display class
/*
 * hdmiWriteDummyPacket
 */
void
hdmiWriteDummyPacket(NVHDMIPKT_CLASS*   pThis,
                     NvU32*             pBaseReg,
                     NvU32              head, 
                     NvU32              packetLen, 
                     NvU8 const *const  pPacket)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiWriteDummyPacket called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return;
}

/*
 * hdmiReadDummyPacketStatus
 */
static NvBool
hdmiReadDummyPacketStatus(NVHDMIPKT_CLASS*  pThis,
                          NvU32*            pBaseReg,
                          NvU32             head,
                          NvU32             pktType0073)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiReadDummyPacketStatus called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NV_TRUE;
}

/*
 * hdmiWriteDummyPacketCtrl
 */
static NVHDMIPKT_RESULT
hdmiWriteDummyPacketCtrl(NVHDMIPKT_CLASS*  pThis,
                         NvU32*            pBaseReg,
                         NvU32             head,
                         NvU32             pktType0073,
                         NvU32             transmitControl,
                         NvBool            bDisable)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiWriteDummyPacketCtrl called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NVHDMIPKT_SUCCESS;
}

NVHDMIPKT_RESULT
hdmiAssessLinkCapabilitiesDummy(NVHDMIPKT_CLASS             *pThis,
                                NvU32                        subDevice,
                                NvU32                        displayId,
                                NVT_EDID_INFO         const * const pSinkEdid,
                                HDMI_SRC_CAPS               *pSrcCaps,
                                HDMI_SINK_CAPS              *pSinkCaps)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiAssessLinkCapabilitiesDummy called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NVHDMIPKT_SUCCESS;
}

NVHDMIPKT_RESULT
hdmiQueryFRLConfigDummy(NVHDMIPKT_CLASS                     *pThis,
                        HDMI_VIDEO_TRANSPORT_INFO     const * const pVidTransInfo,
                        HDMI_QUERY_FRL_CLIENT_CONTROL const * const pClientCtrl,
                        HDMI_SRC_CAPS                 const * const pSrcCaps,
                        HDMI_SINK_CAPS                const * const pSinkCaps,
                        HDMI_FRL_CONFIG                     *pFRLConfig)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiQueryFRLConfigDummy called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NVHDMIPKT_SUCCESS;
}

NVHDMIPKT_RESULT
hdmiSetFRLConfigDummy(NVHDMIPKT_CLASS               *pThis,
                      NvU32                         subDevice,
                      NvU32                         displayId,
                      NvBool                        bFakeLt,
                      HDMI_FRL_CONFIG              *pFRLConfig)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiSetFRLConfigDummy called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NVHDMIPKT_SUCCESS;
}

NVHDMIPKT_RESULT
hdmiClearFRLConfigDummy(NVHDMIPKT_CLASS                   *pThis,
                        NvU32                              subDevice,
                        NvU32                              displayId)
{
    NvHdmiPkt_Print(pThis, "ERROR - Dummy function hdmiClearFRLConfigDummy called. "
                           "Should never be called.");
    NvHdmiPkt_Assert(0);
    return NVHDMIPKT_SUCCESS;
}

/*
 * initializeHdmiPktInterface0073
 */
void
initializeHdmiPktInterface0073(NVHDMIPKT_CLASS* pClass)
{
    pClass->hdmiPacketCtrl               = hdmiPacketCtrl0073;
    pClass->hdmiPacketWrite              = hdmiPacketWrite0073;
    pClass->translatePacketType          = translatePacketType0073;
    pClass->translateTransmitControl     = translateTransmitControl0073;

    // Functions below are mapped to dummy functions, as not needed for HW before GK104
    pClass->hdmiReadPacketStatus         = hdmiReadDummyPacketStatus;
    pClass->hdmiWritePacketCtrl          = hdmiWriteDummyPacketCtrl;
    pClass->hdmiWriteAviPacket           = hdmiWriteDummyPacket;
    pClass->hdmiWriteAudioPacket         = hdmiWriteDummyPacket;
    pClass->hdmiWriteGenericPacket       = hdmiWriteDummyPacket;
    pClass->hdmiWriteGeneralCtrlPacket   = hdmiWriteDummyPacket;
    pClass->hdmiWriteVendorPacket        = hdmiWriteDummyPacket;

    // Update SF_USER data
    pClass->dispSfUserClassId = 0;
    pClass->dispSfUserSize    = 0;

    // Functions below are used by HDMI FRL and will be available for Ampere+.
    pClass->hdmiAssessLinkCapabilities  = hdmiAssessLinkCapabilitiesDummy;
    pClass->hdmiQueryFRLConfig          = hdmiQueryFRLConfigDummy;
    pClass->hdmiSetFRLConfig            = hdmiSetFRLConfigDummy;
    pClass->hdmiClearFRLConfig          = hdmiClearFRLConfigDummy;

}
