/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * File:      nvhdmipkt_C871.c
 *
 */

#include "nvhdmipkt_common.h"
/*
 * Purpose:   Provides packet write functions for HDMI library  for T23+ chips
 */
#include "nvhdmipkt_class.h"
#include "nvhdmipkt_internal.h"
#include "hdmi_spec.h"
#include "class/clc871.h"
#include "ctrl/ctrl0073/ctrl0073specific.h"

#define NVHDMIPKT_C871_MAX_PKT_BYTES_AVI    21  // 3 bytes header + 18 bytes payload

extern void hdmiWriteAviPacket9171(NVHDMIPKT_CLASS*   pThis,
                                   NvU32*             pBaseReg,
                                   NvU32              head,
                                   NvU32              packetLen,
                                   NvU8 const *const  pPacket);

/*
 * translatePacketTypeC871
 */
static NvU32 
translatePacketTypeC871(NVHDMIPKT_CLASS*  pThis,
                        NVHDMIPKT_TYPE    packetType)
{
    NvU32 typeC871 = NVHDMIPKT_INVALID_PKT_TYPE; // initialize to an invalid type enum

    switch (packetType)
    {
    case NVHDMIPKT_TYPE_AVI_INFOFRAME:
        typeC871 = NVC871_SF_HDMI_INFO_IDX_AVI_INFOFRAME;
        break;
    case NVHDMIPKT_TYPE_GENERAL_CONTROL:
        typeC871 = NVC871_SF_HDMI_INFO_IDX_GCP;
        break;
    case NVHDMIPKT_TYPE_AUDIO_INFOFRAME:
    default:
        NvHdmiPkt_Print(pThis, "ERROR - translatePacketType wrong packet type for class C871: %0x.",
                        packetType);
        NvHdmiPkt_Assert(0);
        break;
    }

    return typeC871;
}

/*
 * hdmiWriteAviPacketC871
 */
static void 
hdmiWriteAviPacketC871(NVHDMIPKT_CLASS*   pThis,
                       NvU32*             pBaseReg,
                       NvU32              head,
                       NvU32              packetLen,
                       NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    if (packetLen > NVHDMIPKT_C871_MAX_PKT_BYTES_AVI)
    {
        NvHdmiPkt_Print(pThis, "ERROR - input AVI packet length incorrect. Write will be capped to max allowable bytes");
        NvHdmiPkt_Assert(0);
    }

    data = REG_RD32(pBaseReg, NVC871_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(head));
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB14, pPacket[17], data);
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB15, pPacket[18], data);
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB16, pPacket[19], data);
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB17, pPacket[20], data);
    REG_WR32(pBaseReg, NVC871_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(head), data);

    // the lower 17 bytes remain the same as in 9171 class, call 9171 packet write function to program them
    hdmiWriteAviPacket9171(pThis,
                           pBaseReg,
                           head,
                           17, // HB0-2 and PB0-14
                           pPacket);

    return;
}

/*
 * hdmiWriteGeneralCtrlPacketC871
 */
static void 
hdmiWriteGeneralCtrlPacketC871(NVHDMIPKT_CLASS*   pThis,
                               NvU32*             pBaseReg,
                               NvU32              head,
                               NvU32              packetLen,
                               NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    // orIndexer info is ignored.
    data = REG_RD32(pBaseReg, NVC871_SF_HDMI_GCP_SUBPACK(head));
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_GCP_SUBPACK, _SB0, pPacket[3], data);
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_GCP_SUBPACK, _SB1, pPacket[4], data);
    data = FLD_SET_DRF_NUM(C871, _SF_HDMI_GCP_SUBPACK, _SB2, pPacket[5], data);
    REG_WR32(pBaseReg, NVC871_SF_HDMI_GCP_SUBPACK(head), data);

    return;
}

/*
 * hdmiPacketWriteC871
 */
static NVHDMIPKT_RESULT 
hdmiPacketWriteC871(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacketIn)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32* pBaseReg    = (NvU32*)pThis->memMap[subDevice].pMemBase;
    NvU32  pktTypeC871 = pThis->translatePacketType(pThis, packetType);
    NvU32  tc          = pThis->translateTransmitControl(pThis, transmitControl);
    NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS params = {0};

    // packetIn can be of varying size. Use a fixed max size buffer for programing hw units to prevent out of bounds access
    NvU8   pPacket[NVHDMIPKT_CTAIF_MAX_PKT_BYTES] = {0};

    if (pBaseReg == 0  || head >= NVC871_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1 ||
        packetLen == 0 || pPacketIn == 0 || pktTypeC871 == NVHDMIPKT_INVALID_PKT_TYPE)
    {
        result = NVHDMIPKT_INVALID_ARG;
        NvHdmiPkt_Print(pThis, "Invalid arg");
        goto hdmiPacketWriteC871_exit;
    }

    if (packetLen > NVHDMIPKT_CTAIF_MAX_PKT_BYTES)
    {
        NvHdmiPkt_Print(pThis, "ERROR - input packet length incorrect %d Packet write will be capped to max allowable bytes", packetLen);
        packetLen = NVHDMIPKT_CTAIF_MAX_PKT_BYTES;
        NvHdmiPkt_Assert(0);
    }

    // input packet looks ok to use, copy over the bytes
    NVMISC_MEMCPY(pPacket, pPacketIn, packetLen);

    // acquire mutex
    pThis->callback.acquireMutex(pThis->cbHandle);

    // Disable this packet type.
    pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktTypeC871, tc, NV_TRUE);

    // write the packet
    switch (pktTypeC871)
    {
    case NVC871_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
        pThis->hdmiWriteAviPacket(pThis, pBaseReg, head, packetLen, pPacket);
        break;

    case NVC871_SF_HDMI_INFO_IDX_GCP:
        // Check whether the GCP packet is AVMute DISABLE or AvMute ENABLE
        // Enable HDMI only on GCP unmute i.e. AVMUTE DISABLE
        if (pPacket[HDMI_PKT_HDR_SIZE] == HDMI_GENCTRL_PACKET_MUTE_DISABLE)
        {
            // Enable HDMI.
            NVMISC_MEMSET(&params, 0, sizeof(params));
            params.subDeviceInstance = (NvU8)subDevice;
            params.displayId         = displayId;
            params.bEnable           = NV0073_CTRL_SPECIFIC_CTRL_HDMI_ENABLE;

#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_CTRL_HDMI,
                            &params,
                            sizeof(params)) != NVOS_STATUS_SUCCESS)

#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                             params.subDeviceInstance,
                                                             NV0073_CTRL_CMD_SPECIFIC_CTRL_HDMI, 
                                                             &params, 
                                                             sizeof(params));
            if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                NvHdmiPkt_Print(pThis, "ERROR - RM call to enable hdmi ctrl failed.");
                NvHdmiPkt_Assert(0);
                result = NVHDMIPKT_FAIL;
            }
        }
        pThis->hdmiWriteGeneralCtrlPacket(pThis, pBaseReg, head, packetLen, pPacket);
        break;

    default:
        result = NVHDMIPKT_INVALID_ARG;
        break;
    }

    // Enable this infoframe.
    pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktTypeC871, tc, NV_FALSE);

    // release mutex
    pThis->callback.releaseMutex(pThis->cbHandle);
hdmiPacketWriteC871_exit:
    return result;
}


NvBool 
isInfoframeOffsetAvailable(NvU32*     pBaseReg,
                           NvU32      head,
                           NvU32      requestedInfoframe)
{
    NvU32 regAddr, regData = 0;
    NvU32 ifIndex, size;
    NvBool bResult = NV_TRUE;

    for (ifIndex = 0; ifIndex < NVC871_SF_GENERIC_INFOFRAME_CTRL__SIZE_2; ifIndex++)
    {
        regAddr = NVC871_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
        regData = REG_RD32(pBaseReg, regAddr);
        size    = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_CTRL, _SIZE, regData);

        // if an infoframe is enabled and it's occupying the offset the requested infoframe would use, 
        // we cannot allow programming this requested infoframe
        if (FLD_TEST_DRF(C871, _SF_GENERIC_INFOFRAME_CTRL, _ENABLE, _YES, regData) && (size > 0))
        {
            if ((ifIndex + size) > requestedInfoframe)
            {
                bResult = NV_FALSE;
                break;
            }
        }
    }

    return bResult;
}

/*
 * disableInfoframeC871
 */

NVHDMIPKT_RESULT
disableInfoframeC871(NVHDMIPKT_CLASS*  pThis,
                     NvU32*            pBaseReg,
                     NvU32             head,
                     NvU32             ifIndex)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_TIMEOUT;
    NvU32  regAddr, regData;

    regAddr = NVC871_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
    regData = REG_RD32(pBaseReg, regAddr);

    // if infoframe is already disabled nothing to do
    if (FLD_TEST_DRF(C871, _SF_GENERIC_INFOFRAME_CTRL, _ENABLE, _NO, regData))
    {
        return NVHDMIPKT_SUCCESS;
    }

    // engage timer callbacks to wait for HW register status change if timer callbacks are provided
    NvBool bWaitForIdle = NV_FALSE;
    if ((pThis->callback.setTimeout != 0) && (pThis->callback.checkTimeout != 0))
    {
        // wait until BUSY _NO if timer could be engaged successfully
        bWaitForIdle = (pThis->callback.setTimeout(pThis->cbHandle, NVHDMIPKT_STATUS_READ_TIMEOUT_IN_us) == NV_TRUE);
    }

    // write ENABLE_NO
    regData = FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CTRL, _ENABLE, _NO, regData);
    REG_WR32(pBaseReg, regAddr, regData);

    // if timer callbacks are available poll for  disable done
    if (bWaitForIdle)
    {
        regData = REG_RD32(pBaseReg, regAddr);
        while(FLD_TEST_DRF(C871, _SF_GENERIC_INFOFRAME_CTRL, _BUSY, _YES, regData))
        {
            if (pThis->callback.checkTimeout(pThis->cbHandle) == NV_TRUE)
            {
                // timeout waiting for infoframe to get disabled
                NvHdmiPkt_Print(pThis, "MoreInfoframe: timeout waiting for infoframe to get disabled");
                goto disableInfoframe_exit;
            }
            regData = REG_RD32(pBaseReg, regAddr);
        }

        NvHdmiPkt_Assert(FLD_TEST_DRF(C871, _SF_GENERIC_INFOFRAME_CTRL, _BUSY, _NO, regData));
    }
    else
    {
        NvHdmiPkt_Print(pThis, "MoreInfoframe: Clients must ideally provide timer callbacks to wait for enable/disable infoframes");
        NvHdmiPkt_Assert(0);
    }

    result = NVHDMIPKT_SUCCESS;

disableInfoframe_exit:
    return result;
}

/*
 * programAdvancedInfoframeC871
 */
static NVHDMIPKT_RESULT 
programAdvancedInfoframeC871(NVHDMIPKT_CLASS           *pThis,
                             NvU32                      subDevice,
                             NvU32                      head,
                             NVHDMIPKT_TYPE             packetReg,
                             const ADVANCED_INFOFRAME  *pInfoframe)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32* pBaseReg = (NvU32*)pThis->memMap[subDevice].pMemBase;

    if ((packetReg < NVHDMIPKT_TYPE_SHARED_GENERIC1) || (packetReg >= NVHDMIPKT_INVALID_PKT_TYPE))
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    NvU32 ifIndex = packetReg - NVHDMIPKT_TYPE_SHARED_GENERIC1;
    NvU32 ifNum, dwordNum;

    NvU32 regAddr         = 0;
    NvU32 regData         = 0;
    NvU32 numOfInfoframes = pInfoframe->isLargeInfoframe ? (pInfoframe->numAdditionalInfoframes + 1) : 1;

    if (NV_FALSE == isInfoframeOffsetAvailable(pBaseReg, head, ifIndex))
    {
        NvHdmiPkt_Print(pThis, "MoreInfoframe: Client requested overwriting an active infoframe");
    }

    // acquire mutex
    pThis->callback.acquireMutex(pThis->cbHandle);

    // disable and wait for infoframe HW unit to be ready
    result = disableInfoframeC871(pThis, pBaseReg, head, ifIndex);
    if (result != NVHDMIPKT_SUCCESS)
    {
        return result;
    }

    // write DATA_CTRL
    regData = 0;
    regAddr = NVC871_SF_GENERIC_INFOFRAME_DATA_CTRL(head);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA_CTRL, _OFFSET, ifIndex, regData);
    REG_WR32(pBaseReg, regAddr, regData);

    // send header + payload
    NvHdmiPkt_Assert(pInfoframe->packetLen == (9 * sizeof(NvU32) * numOfInfoframes));

    for (ifNum = 0; ifNum < numOfInfoframes; ifNum++)
    {
        const NvU8 *pPayload = pInfoframe->pPacket + (ifNum * 9 * sizeof(NvU32));

        for (dwordNum = 0; dwordNum < 9; dwordNum++) // each infoframe is 9 DWORDs including the header
        {
            regData = 0;
            regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE0, pPayload[4*dwordNum + 0], regData);
            regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE1, pPayload[4*dwordNum + 1], regData);
            regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE2, pPayload[4*dwordNum + 2], regData);
            regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE3, pPayload[4*dwordNum + 3], regData);

            REG_WR32(pBaseReg, NVC871_SF_GENERIC_INFOFRAME_DATA(head), regData);
        }
    }

    // write GENERIC_CONFIG
    regData = 0;
    regAddr = NVC871_SF_GENERIC_INFOFRAME_CONFIG(head, ifIndex);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CONFIG, _FID, pInfoframe->flipId, regData);
    if (pInfoframe->location == INFOFRAME_CTRL_LOC_LINE)
    {
        regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID, pInfoframe->lineNum, regData);
        regData = (pInfoframe->lineIdReversed) ? 
                        FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID_REVERSED, _YES, regData) :
                        FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID_REVERSED, _NO,  regData);
    }

    regData = (pInfoframe->crcOverride) ? 
                    FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _CRC_OVERRIDE, _YES, regData) :
                    FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _CRC_OVERRIDE, _NO,  regData);

    regData = (pInfoframe->matchFidMethodArmState) ? 
                    FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _MTD_STATE_CTRL, _ARM, regData) : // send Infoframe at LOC when matching FID found at channel's FID method's ARM state
                    FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_CONFIG, _MTD_STATE_CTRL, _ACT,  regData); // default is when FID method is at ACTIVE state

    // write reg
    REG_WR32(pBaseReg, regAddr, regData);

    // ENABLE_YES to GENERIC_CTRL
    regData = 0;
    regAddr = NVC871_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CTRL, _RUN_MODE, pInfoframe->runMode,            regData);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CTRL, _LOC,      pInfoframe->location,           regData);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CTRL, _OFFSET,   ifIndex,                        regData);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_CTRL, _SIZE,     pInfoframe->numAdditionalInfoframes, regData);
    regData = FLD_SET_DRF    (C871, _SF_GENERIC_INFOFRAME_CTRL, _ENABLE,   _YES,                           regData);

    // write reg
    REG_WR32(pBaseReg, regAddr, regData);

    NvHdmiPkt_Print(pThis, "MoreInfoframe: Sent infoframe of length %d bytes, transmit ctrl 0x%x at offset %d head=%x subdev=%d", 
                                                                pInfoframe->packetLen, regData, ifIndex, head, subDevice);

    // setup MSC_CTRL
    regData = 0;
    regAddr = NVC871_SF_GENERIC_INFOFRAME_MISC_CTRL(head);
    regData = pInfoframe->winMethodCyaBroadcast ? 
                                             FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_MISC_CTRL, _WIN_CHN_SEL,    _PUBLIC, regData) : 
                                             FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_MISC_CTRL, _WIN_CHN_SEL,    _PRIVATE, regData) ;
    regData = pInfoframe->highAudioPriority ?
                                             FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_MISC_CTRL, _AUDIO_PRIORITY, _HIGH,   regData) :
                                             FLD_SET_DRF(C871, _SF_GENERIC_INFOFRAME_MISC_CTRL, _AUDIO_PRIORITY, _LOW,    regData);
    // write reg
    REG_WR32(pBaseReg, regAddr, regData);


    // release mutex
    pThis->callback.releaseMutex(pThis->cbHandle);

    return result;
}

/*
 * hdmiWritePacketCtrlC871
 */
static NVHDMIPKT_RESULT 
hdmiWritePacketCtrlLegacyPktsC871(NVHDMIPKT_CLASS*  pThis,
                                  NvU32*            pBaseReg,
                                  NvU32             head,
                                  NvU32             pktTypeC871,
                                  NvU32             transmitControl,
                                  NvBool            bDisable)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_INVALID_ARG;
    NvU32 regOffset = 0;
    NvU32 hdmiCtrl  = 0;

    if (pBaseReg == 0 || head >= NVC871_SF_HDMI_INFO_CTRL__SIZE_1)
    {
        return result;
    }

    switch (pktTypeC871)
    {
    case NVC871_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
    case NVC871_SF_HDMI_INFO_IDX_GCP:
        regOffset = NVC871_SF_HDMI_INFO_CTRL(head, pktTypeC871);
        hdmiCtrl = REG_RD32(pBaseReg, regOffset);
        hdmiCtrl = (bDisable == NV_TRUE) ?
                   (FLD_SET_DRF(C871, _SF_HDMI_INFO_CTRL, _ENABLE, _DIS, hdmiCtrl)) :
                   (transmitControl);
        REG_WR32(pBaseReg, regOffset, hdmiCtrl);
        result = NVHDMIPKT_SUCCESS;
        break;
    default:
        NvHdmiPkt_Assert(0 && "Invalid pkt type!");
        break;
    }

    return result;
}

/*
 * hdmiPacketReadC871
 */
static NVHDMIPKT_RESULT 
hdmiPacketReadC871(NVHDMIPKT_CLASS*   pThis,
                   NvU32              subDevice,
                   NvU32              head,
                   NVHDMIPKT_TYPE     packetReg,
                   NvU32              bufferLen,
                   NvU8 *const        pOutPktBuffer)
{
    NvU32 ifIndex, ifNum, dw, regAddr, regData, numOfInfoframes;
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32* pBaseReg = (NvU32*)pThis->memMap[subDevice].pMemBase;

    if ((packetReg < NVHDMIPKT_TYPE_SHARED_GENERIC1) || (packetReg >= NVHDMIPKT_INVALID_PKT_TYPE))
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    ifIndex = packetReg - NVHDMIPKT_TYPE_SHARED_GENERIC1;

    // write infoframe Offset to DATA_CTRL
    regData = 0;
    regAddr = NVC871_SF_GENERIC_INFOFRAME_DATA_CTRL(head);
    regData = FLD_SET_DRF_NUM(C871, _SF_GENERIC_INFOFRAME_DATA_CTRL, _OFFSET, ifIndex, regData);
    REG_WR32(pBaseReg, regAddr, regData);

    // read size of infoframe programmed at this Offset
    regAddr = NVC871_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
    regData = REG_RD32(pBaseReg, regAddr);
    numOfInfoframes = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_CTRL, _SIZE, regData) + 1; // total size is 1 more than SIZE field

    NvU32 remainingBufSize = bufferLen;
    NvU8 *pBuffer = pOutPktBuffer;

    for (ifNum = 0; ifNum < numOfInfoframes; ifNum++)
    {
        if (remainingBufSize == 0)
        {
            NvHdmiPkt_Assert(0 && "MoreInfoframe: Buffer size insufficient to copy read packet data");
            result = NVHDMIPKT_INSUFFICIENT_BUFFER;
            break;
        }

        // a temporary buffer to read a 36 byte chunk of this infoframe
        NvU8 pktBytes[9 * sizeof(NvU32)];

        for (dw = 0; dw < 9; dw++) // each infoframe is 9 DWORDs including the header
        {
            regData = REG_RD32(pBaseReg, NVC871_SF_GENERIC_INFOFRAME_DATA(head));

            pktBytes[dw*4 + 0] = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE0, regData);
            pktBytes[dw*4 + 1] = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE1, regData);
            pktBytes[dw*4 + 2] = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE2, regData);
            pktBytes[dw*4 + 3] = DRF_VAL(C871, _SF_GENERIC_INFOFRAME_DATA, _BYTE3, regData);
        }

        NvU32 bytesCopied = (remainingBufSize > 36) ? 36 : remainingBufSize;
        NVMISC_MEMCPY(pBuffer, &pktBytes, bytesCopied);

        // move out buffer ptr by the copied bytes
        pBuffer += bytesCopied;
        // reduce remaining buffer size by the amount we copied
        remainingBufSize = remainingBufSize - bytesCopied;
    }

    return result;
}

/*
 * hdmiPacketCtrlC871
 */
static NVHDMIPKT_RESULT 
hdmiPacketCtrlC871(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl)
{
    NvU32* pBaseReg    = (NvU32*)pThis->memMap[subDevice].pMemBase;
    NvU32  tc          = pThis->translateTransmitControl(pThis, transmitControl);

    if ((pBaseReg == 0) || (head >= NVC871_SF_GENERIC_INFOFRAME_CTRL__SIZE_1) ||
        (packetType >= NVHDMIPKT_INVALID_PKT_TYPE))
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    if ((packetType == NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME) ||
        (packetType == NVHDMIPKT_TYPE_GENERIC))
    {
        NvHdmiPkt_Print(pThis, "Generic and VSI registers removed in C871 HW. Call NvHdmiPkt_SetupAdvancedInfoframe to use one of the generic registers!");
        NvHdmiPkt_Assert(0);
        return NVHDMIPKT_INVALID_ARG;
    }

    if (packetType >= NVHDMIPKT_TYPE_SHARED_GENERIC1 && packetType <= NVHDMIPKT_TYPE_SHARED_GENERIC10)
    {
        // client is only expected to call packet ctrl interface to disable the infoframe, no support for other packet ctrl options
        // to reprogram/change run mode of a packet, call NvHdmiPkt_SetupAdvancedInfoframe interface
        NvHdmiPkt_Assert(FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE, _DIS, transmitControl));

        return disableInfoframeC871(pThis, pBaseReg, head, (packetType - NVHDMIPKT_TYPE_SHARED_GENERIC1));
    }

    NvU32 pktTypeC871 = pThis->translatePacketType(pThis, packetType);
    return pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktTypeC871, tc, NV_FALSE);
}

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructorC871
 */
NvBool 
hdmiConstructorC871(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}


/*
 * hdmiDestructorC871
 */
void 
hdmiDestructorC871(NVHDMIPKT_CLASS* pThis)
{
    return;
}

/*
 * initializeHdmiPktInterfaceC871
 */
void
initializeHdmiPktInterfaceC871(NVHDMIPKT_CLASS* pClass)
{
    // Update SF_USER data
    pClass->dispSfUserClassId = NVC871_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(NvC871DispSfUserMap);

    pClass->translatePacketType = translatePacketTypeC871;
    pClass->hdmiWriteAviPacket  = hdmiWriteAviPacketC871;
    pClass->hdmiWriteGeneralCtrlPacket = hdmiWriteGeneralCtrlPacketC871;
    pClass->hdmiPacketWrite     = hdmiPacketWriteC871;
    pClass->hdmiPacketCtrl      = hdmiPacketCtrlC871;
    pClass->hdmiWritePacketCtrl = hdmiWritePacketCtrlLegacyPktsC871;

    // generic infoframe (shareable by DP and HDMI)
    pClass->hdmiPacketRead           = hdmiPacketReadC871;
    pClass->programAdvancedInfoframe = programAdvancedInfoframeC871;
}
