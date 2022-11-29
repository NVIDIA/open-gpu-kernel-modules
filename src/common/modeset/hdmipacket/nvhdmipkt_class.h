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
 * File:      nvhdmipkt_class.h
 *
 * Purpose:   This file contains hdmipkt class definition. Which defines class interfaces.
 */

#ifndef _NVHDMIPKT_CLASS_H_
#define _NVHDMIPKT_CLASS_H_

#include "nvlimits.h"
#include "nvhdmi_frlInterface.h"

/*************************************************************************************************
 *            NOTE * This header file to be used only inside this (Hdmi Packet) library.         *
 ************************************************************************************************/
// NVHDMIPKT_CLASS_ID: HDMI packet class version
// NOTE: Anytime a new class comes with upgrades, it needs to be added here.
//       Consult resman\kernel\inc\classhal.h, before adding a class.
typedef enum
{
    NVHDMIPKT_0073_CLASS = 0, // pre GK104
    NVHDMIPKT_9171_CLASS = 1, // GK104
    NVHDMIPKT_9271_CLASS = 2, // GK110
    NVHDMIPKT_9471_CLASS = 3, // GM10X
    NVHDMIPKT_9571_CLASS = 4, // GM20X
    NVHDMIPKT_C371_CLASS = 5, // GV100
    NVHDMIPKT_C571_CLASS = 6, // TU102
    NVHDMIPKT_C671_CLASS = 7, // GA102, T234D
    NVHDMIPKT_C771_CLASS = 8, // AD10X
    NVHDMIPKT_INVALID_CLASS   // Not to be used by client, and always the last entry here.
} NVHDMIPKT_CLASS_ID;

// Hdmi packet class
struct tagNVHDMIPKT_CLASS
{
    // data
    NvU32                        dispSfUserClassId; // Id from nvidia/class definition
    NvU32                        dispSfUserSize;
    NvU32                        numSubDevices;
    NvU32                        sfUserHandle;
    NVHDMIPKT_RM_CLIENT_HANDLES  clientHandles;
    NVHDMIPKT_MEM_MAP            memMap[NV_MAX_SUBDEVICES];
    NvHdmiPkt_CBHandle           cbHandle;
    NVHDMIPKT_CALLBACK           callback;
    NVHDMIPKT_CLASS_ID           thisId;
    NvBool                       isRMCallInternal;
   
    // functions
    NVHDMIPKT_RESULT
    (*hdmiPacketCtrl)            (NVHDMIPKT_CLASS*   pThis,
                                  NvU32              subDevice,
                                  NvU32              displayId,
                                  NvU32              head,
                                  NVHDMIPKT_TYPE     packetType,
                                  NVHDMIPKT_TC       transmitControl);

    NVHDMIPKT_RESULT
    (*hdmiPacketWrite)           (NVHDMIPKT_CLASS*   pThis,
                                  NvU32              subDevice,
                                  NvU32              displayId,
                                  NvU32              head,
                                  NVHDMIPKT_TYPE     packetType,
                                  NVHDMIPKT_TC       transmitControl,
                                  NvU32              packetLen,
                                  NvU8 const *const  pPacket);

    // HW functions - that read/write registers
    NvBool
    (*hdmiReadPacketStatus)      (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head,
                                  NvU32              pktTypeNative);

    NVHDMIPKT_RESULT
    (*hdmiWritePacketCtrl)       (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head,
                                  NvU32              pktTypeNative,
                                  NvU32              transmitControl,
                                  NvBool             bDisable);
    
    void
    (*hdmiWriteAviPacket)        (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head, 
                                  NvU32              packetLen, 
                                  NvU8 const *const  pPacket);

    void
    (*hdmiWriteAudioPacket)      (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head, 
                                  NvU32              packetLen, 
                                  NvU8 const *const  pPacket);

    void
    (*hdmiWriteGenericPacket)    (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head, 
                                  NvU32              packetLen, 
                                  NvU8 const *const  pPacket);

    void
    (*hdmiWriteGeneralCtrlPacket)(NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head, 
                                  NvU32              packetLen, 
                                  NvU8 const *const  pPacket);

    void
    (*hdmiWriteVendorPacket)     (NVHDMIPKT_CLASS*   pThis,
                                  NvU32*             pBaseReg,
                                  NvU32              head, 
                                  NvU32              packetLen, 
                                  NvU8 const *const  pPacket);

    // utility functions to translate the generic packet type and transmit control
    // to corresponding rm ctrl or hw define types.
    NvU32
    (*translatePacketType)       (NVHDMIPKT_CLASS*   pThis,
                                  NVHDMIPKT_TYPE     packetType);

    NvU32
    (*translateTransmitControl)  (NVHDMIPKT_CLASS*   pThis,
                                  NVHDMIPKT_TC       transmitControl);

    //
    // HDMI FRL functions to enable/disable HDMI FRL and calculate the bandwidth
    // capacity required for target timing.
    //
    NVHDMIPKT_RESULT
    (*hdmiAssessLinkCapabilities)  (NVHDMIPKT_CLASS             *pThis,
                                    NvU32                        subDevice,
                                    NvU32                        displayId,
                                    NVT_EDID_INFO         const * const pSinkEdid,
                                    HDMI_SRC_CAPS               *pSrcCaps,
                                    HDMI_SINK_CAPS              *pSinkCaps);
    NVHDMIPKT_RESULT
    (*hdmiQueryFRLConfig)       (NVHDMIPKT_CLASS                     *pThis,
                                 HDMI_VIDEO_TRANSPORT_INFO     const * const pVidTransInfo,
                                 HDMI_QUERY_FRL_CLIENT_CONTROL const * const pClientCtrl,
                                 HDMI_SRC_CAPS                 const * const pSrcCaps,
                                 HDMI_SINK_CAPS                const * const pSinkCaps,
                                 HDMI_FRL_CONFIG                     *pFRLConfig);

    NVHDMIPKT_RESULT
    (*hdmiSetFRLConfig)         (NVHDMIPKT_CLASS             *pThis,
                                 NvU32                        subDevice,
                                 NvU32                        displayId,
                                 NvBool                       bFakeLt,
                                 HDMI_FRL_CONFIG             *pFRLConfig);

    NVHDMIPKT_RESULT
    (*hdmiClearFRLConfig)       (NVHDMIPKT_CLASS*       pThis,
                                 NvU32                  subDevice,
                                 NvU32                  displayId);
};

#endif //_NVHDMIPKT_CLASS_H_
