/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * File:      nvhdmipkt_C771.c
 *
 * Purpose:   Provides packet write functions for HDMI library  for ADA+ chips
 */

#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"

#include "class/clc771.h"

#define NVHDMIPKT_C771_MAX_PKT_BYTES_AVI    21  // 3 bytes header + 18 bytes payload

extern NVHDMIPKT_RESULT hdmiPacketWrite9171(NVHDMIPKT_CLASS*   pThis,
                                            NvU32              subDevice,
                                            NvU32              displayId,
                                            NvU32              head,
                                            NVHDMIPKT_TYPE     packetType,
                                            NVHDMIPKT_TC       transmitControl,
                                            NvU32              packetLen,
                                            NvU8 const *const  pPacket);

extern NVHDMIPKT_RESULT hdmiPacketCtrl9171(NVHDMIPKT_CLASS*  pThis,
                                            NvU32             subDevice,
                                            NvU32             displayId,
                                            NvU32             head,
                                            NVHDMIPKT_TYPE    packetType,
                                            NVHDMIPKT_TC      transmitControl);

extern void hdmiWriteAviPacket9171(NVHDMIPKT_CLASS*   pThis,
                                   NvU32*             pBaseReg,
                                   NvU32              head,
                                   NvU32              packetLen,
                                   NvU8 const *const  pPacket);

static NvU32
translateTransmitControlC771(NVHDMIPKT_CLASS*  pThis,
                             NVHDMIPKT_TC      transmitControl)
{
    NvU32 tc = 0;

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _ENABLE, _YES, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _SINGLE, _YES, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _CHECKSUM_HW, _YES, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _LOC, _VBLANK, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _LOC, _VBLANK, tc);
    }
    else if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _LOC, _VSYNC, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _LOC, _VSYNC, tc);
    }
    else if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _LOC, _LOADV, transmitControl))
    {
        tc = FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _LOC, _LOADV, tc);
    }

    return tc;
}

static NVHDMIPKT_RESULT
hdmiWritePacketCtrlC771(NVHDMIPKT_CLASS*  pThis,
                        NvU32*            pBaseReg,
                        NvU32             head,
                        NvU32             pktTypeC771,
                        NvU32             transmitControl,
                        NvBool            bDisable)
{
    NvU32 regOffset = 0;
    NvU32 hdmiCtrl  = 0;
    NvU32 sharedGenericIdx = pktTypeC771;

    // This function only handles shared generic infoframe ctrl.
    if (pBaseReg == 0 || head >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_1 || 
        sharedGenericIdx >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_2)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    regOffset = NVC771_SF_SHARED_GENERIC_CTRL(head, sharedGenericIdx);
    hdmiCtrl = REG_RD32(pBaseReg, regOffset);
    hdmiCtrl = (bDisable == NV_TRUE) ?
                (FLD_SET_DRF(C771, _SF_SHARED_GENERIC_CTRL, _ENABLE, _NO, hdmiCtrl)) :
                (transmitControl);
    REG_WR32(pBaseReg, regOffset, hdmiCtrl);

    return NVHDMIPKT_SUCCESS;
}

static NVHDMIPKT_RESULT
hdmiPacketCtrlC771(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32* pBaseReg    = (NvU32*)pThis->memMap[subDevice].pMemBase;

    if (pBaseReg == 0 || head >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_1
        || packetType >= NVHDMIPKT_INVALID_PKT_TYPE)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    NvU32 tc = 0;
    switch (packetType)
    {
        case NVHDMIPKT_TYPE_SHARED_GENERIC1:
        case NVHDMIPKT_TYPE_SHARED_GENERIC2:
        case NVHDMIPKT_TYPE_SHARED_GENERIC3:
        case NVHDMIPKT_TYPE_SHARED_GENERIC4:
        case NVHDMIPKT_TYPE_SHARED_GENERIC5:
        case NVHDMIPKT_TYPE_SHARED_GENERIC6:
            tc = translateTransmitControlC771(pThis, transmitControl);
            result = hdmiWritePacketCtrlC771(pThis, pBaseReg, head, packetType - NVHDMIPKT_TYPE_SHARED_GENERIC1, tc, NV_FALSE);
            break;
        default:
            result = hdmiPacketCtrl9171(pThis, subDevice, displayId, head, packetType, transmitControl);
            break;
    }
    return result;
}

static void 
hdmiWriteAviPacketC771(NVHDMIPKT_CLASS*   pThis,
                       NvU32*             pBaseReg,
                       NvU32              head,
                       NvU32              packetLen,
                       NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    if (packetLen > NVHDMIPKT_C771_MAX_PKT_BYTES_AVI)
    {
        NvHdmiPkt_Print(pThis, "ERROR - input AVI packet length incorrect. Write will be capped to max allowable bytes");
        NvHdmiPkt_Assert(0);
    }

    data = REG_RD32(pBaseReg, NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(head));
    data = FLD_SET_DRF_NUM(C771, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB14, pPacket[17], data);
    data = FLD_SET_DRF_NUM(C771, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB15, pPacket[18], data);
    data = FLD_SET_DRF_NUM(C771, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB16, pPacket[19], data);
    data = FLD_SET_DRF_NUM(C771, _SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW, _PB17, pPacket[20], data);
    REG_WR32(pBaseReg, NVC771_SF_HDMI_AVI_INFOFRAME_SUBPACK2_LOW(head), data);

    // the lower 17 bytes remain the same as in 9171 class, call 9171 packet write function to program them
    hdmiWriteAviPacket9171(pThis,
                           pBaseReg,
                           head,
                           17, // packetLen: HB0-2 and PB0-14
                           pPacket);

    return;
}

static NVHDMIPKT_RESULT
hdmiWriteSharedGenericPacketC771(NVHDMIPKT_CLASS*   pThis,
                                 NvU32              subDevice,
                                 NvU32              displayId,
                                 NvU32              head,
                                 NvU32              pktTypeC771,
                                 NVHDMIPKT_TC       transmitControl,
                                 NvU32              packetLen,
                                 NvU8 const *const  pPacketIn)
{
    NvU32 sharedGenericIdx = pktTypeC771;
    NvU32 data = 0;
    NvU32* pBaseReg = (NvU32*)pThis->memMap[subDevice].pMemBase;
    NvU32  tc          = translateTransmitControlC771(pThis, transmitControl);
    NvU8 pPacket[36] = {0};

    if (pBaseReg == 0  || head >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_1 ||
        packetLen == 0 || (packetLen > 36) || pPacketIn == 0 || sharedGenericIdx >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_2)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    // In case client doesn't guarantee input pkt length to be 9 DWORDs, create a local buffer to avoid out of bound access of input
    NVMISC_MEMCPY(pPacket, pPacketIn, packetLen);

    // Disable this packet type.
    hdmiWritePacketCtrlC771(pThis, pBaseReg, head, sharedGenericIdx, tc, NV_TRUE);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_HEADER, _HB0,         pPacket[0],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_HEADER, _HB1,         pPacket[1],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_HEADER, _HB2,         pPacket[2],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_HEADER, _HB3,         pPacket[3],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK0, _DB0,   pPacket[4],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK0, _DB1,   pPacket[5],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK0, _DB2,   pPacket[6],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK0, _DB3,   pPacket[7],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK0(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK1, _DB4,   pPacket[8],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK1, _DB5,   pPacket[9],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK1, _DB6,   pPacket[10],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK1, _DB7,   pPacket[11],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK1(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK2, _DB8,   pPacket[12],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK2, _DB9,   pPacket[13],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK2, _DB10,   pPacket[14],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK2, _DB11,   pPacket[15],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK2(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK3, _DB12,   pPacket[16],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK3, _DB13,   pPacket[17],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK3, _DB14,   pPacket[18],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK3, _DB15,   pPacket[19],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK3(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK4, _DB16,   pPacket[20],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK4, _DB17,   pPacket[21],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK4, _DB18,   pPacket[22],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK4, _DB19,   pPacket[23],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK4(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK5, _DB20,   pPacket[24],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK5, _DB21,   pPacket[25],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK5, _DB22,   pPacket[26],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK5, _DB23,   pPacket[27],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK5(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK6, _DB24,   pPacket[28],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK6, _DB25,   pPacket[29],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK6, _DB26,   pPacket[30],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK6, _DB27,   pPacket[31],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK6(head, sharedGenericIdx), data);

    data = REG_RD32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7(head, sharedGenericIdx));
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK7, _DB28,   pPacket[32],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK7, _DB29,   pPacket[33],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK7, _DB30,   pPacket[34],  data);
    data = FLD_SET_DRF_NUM(C771, _SF_SHARED_GENERIC_INFOFRAME_SUBPACK7, _DB31,   pPacket[35],  data);
    REG_WR32(pBaseReg, NVC771_SF_SHARED_GENERIC_INFOFRAME_SUBPACK7(head, sharedGenericIdx), data);

    // Enable this infoframe.
    hdmiWritePacketCtrlC771(pThis, pBaseReg, head, sharedGenericIdx, tc, NV_FALSE);

    return NVHDMIPKT_SUCCESS;
}

static NVHDMIPKT_RESULT
hdmiPacketWriteC771(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacketIn)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;

    if (head >= NVC771_SF_SHARED_GENERIC_INFOFRAME_HEADER__SIZE_1 ||
        packetLen == 0                                            || 
        pPacketIn == 0                                            ||
        packetType >= NVHDMIPKT_INVALID_PKT_TYPE)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    if (packetType >= NVHDMIPKT_TYPE_SHARED_GENERIC1 && packetType <= NVHDMIPKT_TYPE_SHARED_GENERIC6)
    {
        result = hdmiWriteSharedGenericPacketC771(pThis, subDevice, displayId, head, packetType - NVHDMIPKT_TYPE_SHARED_GENERIC1, transmitControl, packetLen, pPacketIn);
    }
    else
    {
        result = hdmiPacketWrite9171(pThis, subDevice, displayId, head, packetType, transmitControl, packetLen, pPacketIn);
    }

    return result;
}

// non-HW - class utility/maintenance functions
NvBool
hdmiConstructorC771(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}

void
hdmiDestructorC771(NVHDMIPKT_CLASS* pThis)
{
    return;
}

void
initializeHdmiPktInterfaceC771(NVHDMIPKT_CLASS* pClass)
{
    // Update SF_USER data
    pClass->dispSfUserClassId = NVC771_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(NvC771DispSfUserMap);

    pClass->hdmiPacketCtrl    = hdmiPacketCtrlC771;
    pClass->hdmiPacketWrite   = hdmiPacketWriteC771;
    pClass->hdmiWriteAviPacket = hdmiWriteAviPacketC771;
}
