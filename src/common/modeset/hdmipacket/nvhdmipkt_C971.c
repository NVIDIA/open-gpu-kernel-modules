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
 * File:      nvhdmipkt_C971.c
 *
 */

#include "nvhdmipkt_common.h"
/*
 * Purpose:   Provides packet write functions for HDMI library  for NVD5.0 chips
 */
#include "nvhdmipkt_class.h"
#include "nvhdmipkt_internal.h"
#include "class/clc971.h"

/*
 * programAdvancedInfoframeC971
 */
static NVHDMIPKT_RESULT 
programAdvancedInfoframeC971(NVHDMIPKT_CLASS           *pThis,
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

    NvHdmiPkt_Assert((pInfoframe->crcOverride == 0) && 
                     "CRC Override bit not supported in Nvd 5.0");
    NvHdmiPkt_Assert((pInfoframe->winMethodCyaBroadcast == 0) &&
                     "window channel priv reg control not supported in Nvd5.0");
    NvHdmiPkt_Assert((pInfoframe->location != INFOFRAME_CTRL_LOC_LOADV) &&
                     "LoadV location not supported in Nvd5.0");

    // acquire mutex
    pThis->callback.acquireMutex(pThis->cbHandle);

    // disable and wait for infoframe HW unit to be ready
    // Note, C971 HW provides Clear option for SENT field, so we don't have to disable
    // just to make sure new data sending is successful. But we disable Infoframe 
    // before reprogramming to avoid corrupting a payload that is actively being sent
    result = disableInfoframeC871(pThis, pBaseReg, head, ifIndex);
    if (result != NVHDMIPKT_SUCCESS)
    {
        return result;
    }

    // write SENT bit to clear the SENT field
    regAddr = NVC971_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
    regData = REG_RD32(pBaseReg, regAddr);
    regData = FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CTRL, _SENT, _CLEAR, regData);
    REG_WR32(pBaseReg, regAddr, regData);

    // write DATA_CTRL
    regData = 0;
    regAddr = NVC971_SF_GENERIC_INFOFRAME_DATA_CTRL(head);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_DATA_CTRL, _OFFSET, ifIndex, regData);
    REG_WR32(pBaseReg, regAddr, regData);

    // send header + payload
    NvHdmiPkt_Assert(pInfoframe->packetLen == (9 * sizeof(NvU32) * numOfInfoframes));

    for (ifNum = 0; ifNum < numOfInfoframes; ifNum++)
    {
        const NvU8 *pPayload = pInfoframe->pPacket + (ifNum * 9 * sizeof(NvU32));

        for (dwordNum = 0; dwordNum < 9; dwordNum++) // each infoframe is 9 DWORDs including the header
        {
            regData = 0;
            regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_DATA, _BYTE0, pPayload[4*dwordNum + 0], regData);
            regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_DATA, _BYTE1, pPayload[4*dwordNum + 1], regData);
            regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_DATA, _BYTE2, pPayload[4*dwordNum + 2], regData);
            regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_DATA, _BYTE3, pPayload[4*dwordNum + 3], regData);

            REG_WR32(pBaseReg, NVC971_SF_GENERIC_INFOFRAME_DATA(head), regData);
        }
    }

    // write GENERIC_CONFIG
    regData = 0;
    regAddr = NVC971_SF_GENERIC_INFOFRAME_CONFIG(head, ifIndex);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CONFIG, _FID, pInfoframe->flipId, regData);
    if (pInfoframe->location == INFOFRAME_CTRL_LOC_LINE)
    {
        regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID, pInfoframe->lineNum, regData);
        regData = (pInfoframe->lineIdReversed) ? 
                        FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID_REVERSED, _YES, regData) :
                        FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _LINE_ID_REVERSED, _NO,  regData);
    }

    regData = (pInfoframe->asSdpOverride) ? 
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _AS_SDP_OVERRIDE_EN, _YES, regData) :
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _AS_SDP_OVERRIDE_EN, _NO,  regData);

    regData = (pInfoframe->matchFidMethodArmState) ? 
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _MTD_STATE_CTRL, _ARM, regData) : 
                                        // send Infoframe at LOC when matching FID found at channel's FID method's ARM state
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_CONFIG, _MTD_STATE_CTRL, _ACT,  regData); 
                                        // default is when FID method is at ACTIVE state

    // write reg
    REG_WR32(pBaseReg, regAddr, regData);

    // ENABLE_YES to GENERIC_CTRL
    regData = 0;
    regAddr = NVC971_SF_GENERIC_INFOFRAME_CTRL(head, ifIndex);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CTRL, _RUN_MODE, pInfoframe->runMode,  regData);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CTRL, _LOC,      pInfoframe->location, regData);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CTRL, _OFFSET,   ifIndex,              regData);
    regData = FLD_SET_DRF_NUM(C971, _SF_GENERIC_INFOFRAME_CTRL, _SIZE,     pInfoframe->numAdditionalInfoframes, regData);
    regData = FLD_SET_DRF    (C971, _SF_GENERIC_INFOFRAME_CTRL, _ENABLE,   _YES,                 regData);

    // write reg
    REG_WR32(pBaseReg, regAddr, regData);

    NvHdmiPkt_Print(pThis, "MoreInfoframe: Sent infoframe of length %d bytes, transmit ctrl 0x%x at offset %d head=%x subdev=%d", 
                                                                pInfoframe->packetLen, regData, ifIndex, head, subDevice);

    // setup MSC_CTRL
    regData = 0;
    regAddr = NVC971_SF_GENERIC_INFOFRAME_MISC_CTRL(head);
    regData = pInfoframe->highAudioPriority ?
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_MISC_CTRL, _AUDIO_PRIORITY, _HIGH,   regData) :
                    FLD_SET_DRF(C971, _SF_GENERIC_INFOFRAME_MISC_CTRL, _AUDIO_PRIORITY, _LOW,    regData);
    // write reg
    REG_WR32(pBaseReg, regAddr, regData);

    // release mutex
    pThis->callback.releaseMutex(pThis->cbHandle);

    return result;
}

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructorC971
 */
NvBool 
hdmiConstructorC971(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}

/*
 * hdmiDestructorC971
 */
void 
hdmiDestructorC971(NVHDMIPKT_CLASS* pThis)
{
    return;
}

/*
 * initializeHdmiPktInterfaceC971
 */
void
initializeHdmiPktInterfaceC971(NVHDMIPKT_CLASS* pClass)
{
    // Update SF_USER data
    pClass->dispSfUserClassId = NVC971_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(NvC971DispSfUserMap);

    // generic infoframe (shareable by DP and HDMI)
    pClass->programAdvancedInfoframe = programAdvancedInfoframeC971;
}
