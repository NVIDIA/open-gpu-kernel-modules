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
 * File:      nvhdmipkt_9171.c
 *
 * Purpose:   Provides packet write functions for HDMI library  for KEPLER + chips
 */

#include "nvlimits.h"
#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"

#include "hdmi_spec.h"
#include "class/cl9171.h"
#include "ctrl/ctrl0073/ctrl0073specific.h"

#define NVHDMIPKT_9171_INVALID_PKT_TYPE  ((NV9171_SF_HDMI_INFO_IDX_VSI) + 1)
NVHDMIPKT_RESULT 
hdmiPacketWrite9171(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacket);

NVHDMIPKT_RESULT 
hdmiPacketCtrl9171(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl);
/*
 * hdmiReadPacketStatus9171
 */
static NvBool
hdmiReadPacketStatus9171(NVHDMIPKT_CLASS*  pThis,
                         NvU32*            pBaseReg,
                         NvU32             head,
                         NvU32             pktType9171)
{
    NvBool bResult   = NV_FALSE;
    NvU32  regOffset = 0;
    NvU32  status    = 0;

    if (pBaseReg == 0 || head >= NV9171_SF_HDMI_INFO_STATUS__SIZE_1)
    {
        return bResult;
    }

    switch (pktType9171)
    {
    case NV9171_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
    case NV9171_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME:
    case NV9171_SF_HDMI_INFO_IDX_GCP:
    case NV9171_SF_HDMI_INFO_IDX_VSI:
        regOffset = NV9171_SF_HDMI_INFO_STATUS(head, pktType9171);
        status = REG_RD32(pBaseReg, regOffset);
        bResult = FLD_TEST_DRF(9171, _SF_HDMI_INFO_STATUS, _SENT, _DONE, status);
        break;

    default:
        break; 
    }

    return bResult;
}

/*
 * hdmiWritePacketCtrl9171
 */
static NVHDMIPKT_RESULT 
hdmiWritePacketCtrl9171(NVHDMIPKT_CLASS*  pThis,
                        NvU32*            pBaseReg,
                        NvU32             head,
                        NvU32             pktType9171,
                        NvU32             transmitControl,
                        NvBool            bDisable)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_INVALID_ARG;
    NvU32 regOffset = 0;
    NvU32 hdmiCtrl  = 0;

    if (pBaseReg == 0 || head >= NV9171_SF_HDMI_INFO_CTRL__SIZE_1)
    {
        return result;
    }

    switch (pktType9171)
    {
    case NV9171_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
    case NV9171_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME:
    case NV9171_SF_HDMI_INFO_IDX_GCP:
    case NV9171_SF_HDMI_INFO_IDX_VSI:
        regOffset = NV9171_SF_HDMI_INFO_CTRL(head, pktType9171);
        hdmiCtrl = REG_RD32(pBaseReg, regOffset);
        hdmiCtrl = (bDisable == NV_TRUE) ?
                   (FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _ENABLE, _DIS, hdmiCtrl)) :
                   (transmitControl);
        REG_WR32(pBaseReg, regOffset, hdmiCtrl);

        result = NVHDMIPKT_SUCCESS;
        break;

    default:
        break; 
    }

    return result;
}

/*
 * hdmiWriteAviPacket9171
 */
static void 
hdmiWriteAviPacket9171(NVHDMIPKT_CLASS*   pThis,
                       NvU32*             pBaseReg,
                       NvU32              head,
                       NvU32              packetLen,
                       NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_HEADER(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_HEADER, _HB0,         pPacket[0],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_HEADER, _HB1,         pPacket[1],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_HEADER, _HB2,         pPacket[2],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_HEADER(head), data);
    
    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW, _PB0,   pPacket[3],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW, _PB1,   pPacket[4],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW, _PB2,   pPacket[5],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW, _PB3,   pPacket[6],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK0_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH, _PB4,  pPacket[7],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH, _PB5,  pPacket[8],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH, _PB6,  pPacket[9],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK0_HIGH(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW, _PB7,   pPacket[10], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW, _PB8,   pPacket[11], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW, _PB9,   pPacket[12], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW, _PB10,  pPacket[13], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK1_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH, _PB11, pPacket[14], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH, _PB12, pPacket[15], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH, _PB13, pPacket[16], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_AVI_INFOFRAME_SUBPACK1_HIGH(head), data);

    return;
}

/*
 * hdmiWriteGenericPacket9171
 */
static void 
hdmiWriteGenericPacket9171(NVHDMIPKT_CLASS*   pThis,
                           NvU32*             pBaseReg,
                           NvU32              head,
                           NvU32              packetLen,
                           NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_HEADER(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_HEADER, _HB0,         pPacket[0],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_HEADER, _HB1,         pPacket[1],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_HEADER, _HB2,         pPacket[2],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_HEADER(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK0_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_LOW, _PB0,   pPacket[3],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_LOW, _PB1,   pPacket[4],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_LOW, _PB2,   pPacket[5],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_LOW, _PB3,   pPacket[6],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK0_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK0_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_HIGH, _PB4,  pPacket[7],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_HIGH, _PB5,  pPacket[8],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK0_HIGH, _PB6,  pPacket[9],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK0_HIGH(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK1_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_LOW, _PB7,   pPacket[10], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_LOW, _PB8,   pPacket[11], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_LOW, _PB9,   pPacket[12], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_LOW, _PB10,  pPacket[13], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK1_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK1_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_HIGH, _PB11, pPacket[14], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_HIGH, _PB12, pPacket[15], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK1_HIGH, _PB13, pPacket[16], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK1_HIGH(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK2_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_LOW, _PB14,  pPacket[17], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_LOW, _PB15,  pPacket[18], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_LOW, _PB16,  pPacket[19], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_LOW, _PB17,  pPacket[20], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK2_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK2_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_HIGH, _PB18, pPacket[21], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_HIGH, _PB19, pPacket[22], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK2_HIGH, _PB20, pPacket[23], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK2_HIGH(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK3_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_LOW, _PB21,  pPacket[24], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_LOW, _PB22,  pPacket[25], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_LOW, _PB23,  pPacket[26], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_LOW, _PB24,  pPacket[27], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK3_LOW(head), data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK3_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_HIGH, _PB25, pPacket[28], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_HIGH, _PB26, pPacket[29], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GENERIC_SUBPACK3_HIGH, _PB27, pPacket[30], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK3_HIGH(head), data);

    return;
}

/*
 * hdmiWriteGeneralCtrlPacket9171
 */
static void 
hdmiWriteGeneralCtrlPacket9171(NVHDMIPKT_CLASS*   pThis,
                               NvU32*             pBaseReg,
                               NvU32              head,
                               NvU32              packetLen,
                               NvU8 const *const  pPacket)
{
    NvU32 data = 0;

    // orIndexer info is ignored.
    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GCP_SUBPACK(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GCP_SUBPACK, _SB0, pPacket[3], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GCP_SUBPACK, _SB1, pPacket[4], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_GCP_SUBPACK, _SB2, pPacket[5], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_GCP_SUBPACK(head), data);

    return;
}

/*
 * hdmiWriteVendorPacket9171
 */
static void 
hdmiWriteVendorPacket9171(NVHDMIPKT_CLASS*   pThis,
                          NvU32*             pBaseReg,
                          NvU32              head,
                          NvU32              packetLen,
                          NvU8 const *const  pPacketIn)
{
    NvU32 data = 0;
    NvU8  pPacket[31] = {0};

    NVMISC_MEMCPY(pPacket, pPacketIn, packetLen);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_HEADER(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_HEADER, _HB0,         pPacket[0],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_HEADER, _HB1,         pPacket[1],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_HEADER, _HB2,         pPacket[2],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_HEADER(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK0_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_LOW, _PB0,   pPacket[3],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_LOW, _PB1,   pPacket[4],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_LOW, _PB2,   pPacket[5],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_LOW, _PB3,   pPacket[6],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK0_LOW(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK0_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_HIGH, _PB4,  pPacket[7],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_HIGH, _PB5,  pPacket[8],  data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK0_HIGH, _PB6,  pPacket[9],  data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK0_HIGH(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK1_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_LOW, _PB7,   pPacket[10], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_LOW, _PB8,   pPacket[11], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_LOW, _PB9,   pPacket[12], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_LOW, _PB10,  pPacket[13], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK1_LOW(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK1_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_HIGH, _PB11, pPacket[14], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_HIGH, _PB12, pPacket[15], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK1_HIGH, _PB13, pPacket[16], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK1_HIGH(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK2_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_LOW, _PB14,  pPacket[17], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_LOW, _PB15,  pPacket[18], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_LOW, _PB16,  pPacket[19], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_LOW, _PB17,  pPacket[20], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK2_LOW(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK2_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_HIGH, _PB18, pPacket[21], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_HIGH, _PB19, pPacket[22], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK2_HIGH, _PB20, pPacket[23], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK2_HIGH(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_GENERIC_SUBPACK3_LOW(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_LOW, _PB21,  pPacket[24], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_LOW, _PB22,  pPacket[25], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_LOW, _PB23,  pPacket[26], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_LOW, _PB24,  pPacket[27], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK3_LOW(head),  data);

    data = REG_RD32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK3_HIGH(head));
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_HIGH, _PB25, pPacket[28], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_HIGH, _PB26, pPacket[29], data);
    data = FLD_SET_DRF_NUM(9171, _SF_HDMI_VSI_SUBPACK3_HIGH, _PB27, pPacket[30], data);
    REG_WR32(pBaseReg, NV9171_SF_HDMI_VSI_SUBPACK3_HIGH(head),  data);

    return;
}

/*
 * translatePacketType9171
 */
static NvU32 
translatePacketType9171(NVHDMIPKT_CLASS*  pThis,
                        NVHDMIPKT_TYPE    packetType)
{
    NvU32 type9171 = NVHDMIPKT_9171_INVALID_PKT_TYPE;

    switch (packetType)
    {
    case NVHDMIPKT_TYPE_AVI_INFOFRAME:
        type9171 = NV9171_SF_HDMI_INFO_IDX_AVI_INFOFRAME;
        break;

    case NVHDMIPKT_TYPE_GENERIC:
        type9171 = NV9171_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME;
        break;

    case NVHDMIPKT_TYPE_GENERAL_CONTROL:
        type9171 = NV9171_SF_HDMI_INFO_IDX_GCP;
        break;

    case NVHDMIPKT_TYPE_VENDOR_SPECIFIC_INFOFRAME:
        type9171 = NV9171_SF_HDMI_INFO_IDX_VSI;
        break;

    case NVHDMIPKT_TYPE_AUDIO_INFOFRAME:
    default:
        NvHdmiPkt_Print(pThis, "ERROR - translatePacketType wrong packet type: %0x.",
                        packetType);
        NvHdmiPkt_Assert(0);
        break;
    }

    return type9171;
}

/*
 * translateTransmitControl9171
 */
static NvU32 
translateTransmitControl9171(NVHDMIPKT_CLASS*  pThis,
                             NVHDMIPKT_TC      transmitControl)
{
    NvU32 tc = 0;

    // TODO: tc validation
    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _ENABLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _ENABLE, _EN, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _OTHER, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _OTHER, _EN, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _SINGLE, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _SINGLE, _EN, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _CHKSUM_HW, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _CHKSUM_HW, _EN, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _HBLANK, _EN, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _HBLANK, _EN, tc);
    }

    if (FLD_TEST_DRF(_HDMI_PKT, _TRANSMIT_CTRL, _VIDEO_FMT, _HW_CTRL, transmitControl))
    {
        tc = FLD_SET_DRF(9171, _SF_HDMI_INFO_CTRL, _VIDEO_FMT, _HW_CONTROLLED, tc);
    }

    return tc;
}

/*
 * hdmiPacketCtrl9171
 */
NVHDMIPKT_RESULT 
hdmiPacketCtrl9171(NVHDMIPKT_CLASS*  pThis,
                   NvU32             subDevice,
                   NvU32             displayId,
                   NvU32             head,
                   NVHDMIPKT_TYPE    packetType,
                   NVHDMIPKT_TC      transmitControl)
{
    NvU32* pBaseReg    = (NvU32*)pThis->memMap[subDevice].pMemBase;
    NvU32  pktType9171 = pThis->translatePacketType(pThis, packetType);
    NvU32  tc          = pThis->translateTransmitControl(pThis, transmitControl);

    if (pBaseReg == 0 || head >= NV9171_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1 ||
        pktType9171 == NVHDMIPKT_9171_INVALID_PKT_TYPE)
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    return pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktType9171, tc, NV_FALSE);
}

/*
 * internal utility function
 * checkPacketStatus
 */
static NVHDMIPKT_RESULT
checkPacketStatus(NVHDMIPKT_CLASS*  pThis,
                  NvU32*            pBaseReg,
                  NvU32             head,
                  NvU32             pktType9171)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvBool bCheckPacketStatus = NV_TRUE;
    NvU32  regOffset = 0;
    NvU32  status    = 0;

    // check to see if timer callbacks are provided
    if (pThis->callback.setTimeout == 0 || pThis->callback.checkTimeout == 0)
    {
        goto checkPacketStatus_exit;
    }

    // Mark packets that don't need status check
    switch (pktType9171)
    {
    case NV9171_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
    case NV9171_SF_HDMI_INFO_IDX_GCP:
        regOffset          = NV9171_SF_HDMI_INFO_STATUS(head, pktType9171);
        status             = REG_RD32(pBaseReg, regOffset);
        bCheckPacketStatus = FLD_TEST_DRF(9171, _SF_HDMI_INFO_CTRL, _SINGLE, _EN, status);
        break;

    default:
        bCheckPacketStatus = NV_FALSE;
        break;
    }

    if (bCheckPacketStatus == NV_TRUE)
    {
        if (pThis->callback.setTimeout(pThis->cbHandle, NVHDMIPKT_STATUS_READ_TIMEOUT_IN_us)
            == NV_FALSE)
        {
            // Timer set failed
            goto checkPacketStatus_exit;
        }

        while(pThis->hdmiReadPacketStatus(pThis, pBaseReg, head, pktType9171) == NV_FALSE)
        {
            if (pThis->callback.checkTimeout(pThis->cbHandle) == NV_TRUE)
            {
                // status check operation timed out
                result = NVHDMIPKT_TIMEOUT;
                goto checkPacketStatus_exit;
            }
        }
    }

checkPacketStatus_exit:
    return result;
}

/*
 * hdmiPacketWrite9171
 */
NVHDMIPKT_RESULT 
hdmiPacketWrite9171(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacket)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32* pBaseReg    = (NvU32*)pThis->memMap[subDevice].pMemBase;
    NvU32  pktType9171 = pThis->translatePacketType(pThis, packetType);
    NvU32  tc          = pThis->translateTransmitControl(pThis, transmitControl);
    NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS params = {0};

    if (pBaseReg == 0 || head >= NV9171_SF_HDMI_AVI_INFOFRAME_CTRL__SIZE_1 ||
        packetLen == 0 || pPacket == 0 || pktType9171 == NVHDMIPKT_9171_INVALID_PKT_TYPE)
    {
        result = NVHDMIPKT_INVALID_ARG;
        goto hdmiPacketWrite9171_exit;
    }

    // acquire mutex
    pThis->callback.acquireMutex(pThis->cbHandle);

    // Check status if last infoframe was sent out or not

    if ((result = checkPacketStatus(pThis, pBaseReg, head, pktType9171)) ==
        NVHDMIPKT_TIMEOUT)
    {
        NvHdmiPkt_Print(pThis, "ERROR - Packet status check timed out.");
        NvHdmiPkt_Assert(0);
        goto hdmiPacketWrite9171_release_mutex_exit;
    }
    
    // Disable this packet type.
    pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktType9171, tc, NV_TRUE);

    // write the packet
    switch (pktType9171)
    {
    case NV9171_SF_HDMI_INFO_IDX_AVI_INFOFRAME:
        pThis->hdmiWriteAviPacket(pThis, pBaseReg, head, packetLen, pPacket);
        break;

    case NV9171_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME:
        pThis->hdmiWriteGenericPacket(pThis, pBaseReg, head, packetLen, pPacket);
        break;

    case NV9171_SF_HDMI_INFO_IDX_GCP:
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

    case NV9171_SF_HDMI_INFO_IDX_VSI:
        pThis->hdmiWriteVendorPacket(pThis, pBaseReg, head, packetLen, pPacket);
        break;

    default:
        result = NVHDMIPKT_INVALID_ARG;
        break;
    }

    // Enable this infoframe.
    pThis->hdmiWritePacketCtrl(pThis, pBaseReg, head, pktType9171, tc, NV_FALSE);

hdmiPacketWrite9171_release_mutex_exit:
    // release mutex
    pThis->callback.releaseMutex(pThis->cbHandle);
hdmiPacketWrite9171_exit:
    return result;
}

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructor9171
 */
NvBool 
hdmiConstructor9171(NVHDMIPKT_CLASS* pThis)
{
    NvU32  i = 0;
    NvBool result = NV_TRUE;

#if NVHDMIPKT_RM_CALLS_INTERNAL
    for (i = 0; i < pThis->numSubDevices; i++)
    {
        if (CALL_DISP_RM(NvRmAlloc)(pThis->clientHandles.hClient,
                      pThis->clientHandles.hSubDevices[i],
                      pThis->sfUserHandle + i,
                      pThis->dispSfUserClassId,
                      (void*)0) != NVOS_STATUS_SUCCESS)
        {
            NvHdmiPkt_Print(pThis, "ERROR - Init failed. "
                                   "Failed to alloc SF_USER handle");
            NvHdmiPkt_Assert(0);
            break;
        }

        pThis->memMap[i].memHandle = pThis->sfUserHandle + i;

        if (CALL_DISP_RM(NvRmMapMemory)(pThis->clientHandles.hClient,
                          pThis->clientHandles.hSubDevices[i],
                          pThis->memMap[i].memHandle,
                          0,
                          pThis->dispSfUserSize,
                          &pThis->memMap[i].pMemBase,
                          0) != NVOS_STATUS_SUCCESS)
        {
            NvHdmiPkt_Print(pThis, "ERROR - Init failed. "
                                   "Failed to map SF_USER memory.");
            NvHdmiPkt_Assert(0);
            break;
        }

        if (pThis->memMap[i].pMemBase == 0)
        {
            NvHdmiPkt_Print(pThis, "ERROR - Init failed. "
                                   "SF_USER memory returned is NULL.");
            NvHdmiPkt_Assert(0);
            break;
        }

        pThis->memMap[i].subDevice = i;
    }

    // coudln't complete the loop above
    if (i < pThis->numSubDevices)
    {
        result = NV_FALSE;
        goto hdmiConstructor9171_exit;
    }
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    for (i = 0; i < pThis->numSubDevices; i++)
    {
        result = pThis->callback.rmGetMemoryMap(pThis->cbHandle,
                                                pThis->dispSfUserClassId,
                                                pThis->dispSfUserSize,
                                                i,
                                                &pThis->memMap[i].memHandle,
                                                &pThis->memMap[i].pMemBase);
        if (result == NV_TRUE)
        {
            pThis->memMap[i].subDevice = i;
        }
        else
        {
            NvHdmiPkt_Print(pThis, "ERROR - Init failed. "
                                   "Failed to map SF_USER memory.");
            NvHdmiPkt_Assert(0);
            result = NV_FALSE;
            goto hdmiConstructor9171_exit;
        }
    }
#endif // NVHDMIPKT_RM_CALLS_INTERNAL

hdmiConstructor9171_exit:
    return result;
}

/*
 * hdmiDestructor9171
 */
void 
hdmiDestructor9171(NVHDMIPKT_CLASS* pThis)

{
    NvU32 i = 0;

#if NVHDMIPKT_RM_CALLS_INTERNAL
    for (i = 0; i < NV_MAX_SUBDEVICES; i++)
    {
        // free memory
        if (pThis->memMap[i].pMemBase)
        {
            if (CALL_DISP_RM(NvRmUnmapMemory)(pThis->clientHandles.hClient,
                                pThis->clientHandles.hSubDevices[i],
                                pThis->memMap[i].memHandle,
                                pThis->memMap[i].pMemBase,
                                0) != NVOS_STATUS_SUCCESS)
            {
                NvHdmiPkt_Print(pThis, "ERROR - unInit failed. "
                                       "SF_USER memory unMap failed.");
                NvHdmiPkt_Assert(0);
            }
        }

        // free handle
        if (pThis->memMap[i].memHandle)
        {
            if (CALL_DISP_RM(NvRmFree)(pThis->clientHandles.hClient,
                         pThis->clientHandles.hSubDevices[i],
                         pThis->memMap[i].memHandle) != NVOS_STATUS_SUCCESS)
            {
                NvHdmiPkt_Print(pThis, "ERROR - unInit failed. "
                                       "Freeing SF_USER memory handle failed.");
                NvHdmiPkt_Assert(0);
            }
        }

        pThis->memMap[i].subDevice = NVHDMIPKT_INVALID_SUBDEV;
        pThis->memMap[i].memHandle = 0;
        pThis->memMap[i].pMemBase  = 0;
    }
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    for (i = 0; i < NV_MAX_SUBDEVICES; i++)
    {
        if (pThis->memMap[i].memHandle)
        {
            pThis->callback.rmFreeMemoryMap(pThis->cbHandle,
                                            i,
                                            pThis->memMap[i].memHandle,
                                            pThis->memMap[i].pMemBase);

            pThis->memMap[i].subDevice = NVHDMIPKT_INVALID_SUBDEV;
            pThis->memMap[i].memHandle = 0;
            pThis->memMap[i].pMemBase  = 0;
        }
    }
#endif // NVHDMIPKT_RM_CALLS_INTERNAL

    return;
}

/*
 * initializeHdmiPktInterface9171
 */
void
initializeHdmiPktInterface9171(NVHDMIPKT_CLASS* pClass)
{
    pClass->hdmiPacketCtrl             = hdmiPacketCtrl9171;
    pClass->hdmiPacketWrite            = hdmiPacketWrite9171;
    pClass->translatePacketType        = translatePacketType9171;
    pClass->translateTransmitControl   = translateTransmitControl9171;

    // HW register write functions
    pClass->hdmiReadPacketStatus       = hdmiReadPacketStatus9171;
    pClass->hdmiWritePacketCtrl        = hdmiWritePacketCtrl9171;
    pClass->hdmiWriteAviPacket         = hdmiWriteAviPacket9171;
    pClass->hdmiWriteAudioPacket       = hdmiWriteDummyPacket;
    pClass->hdmiWriteGenericPacket     = hdmiWriteGenericPacket9171;
    pClass->hdmiWriteGeneralCtrlPacket = hdmiWriteGeneralCtrlPacket9171;
    pClass->hdmiWriteVendorPacket      = hdmiWriteVendorPacket9171;

    // Update SF_USER data
    pClass->dispSfUserClassId = NV9171_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(Nv9171DispSfUserMap);

    // Functions below are used by HDMI FRL and will be available for Ampere+.
    pClass->hdmiAssessLinkCapabilities  = hdmiAssessLinkCapabilitiesDummy;
    pClass->hdmiQueryFRLConfig          = hdmiQueryFRLConfigDummy;
    pClass->hdmiSetFRLConfig            = hdmiSetFRLConfigDummy;
    pClass->hdmiClearFRLConfig          = hdmiClearFRLConfigDummy;
}
