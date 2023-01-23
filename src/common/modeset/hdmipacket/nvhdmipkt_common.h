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
 * File:      nvhdmipkt_common.h
 *
 * Purpose:   This file contains defines and structures used across hdmipkt library. All the
 *            common stuff goes here.
 */

#ifndef _NVHDMIPKT_COMMON_H_
#define _NVHDMIPKT_COMMON_H_

/*************************************************************************************************
 *            NOTE * This header file to be used only inside this (Hdmi Packet) library.         *
 ************************************************************************************************/

#include "nvhdmipkt.h"
#include "nvhdmi_frlInterface.h"
#if NVHDMIPKT_RM_CALLS_INTERNAL
#include "nvRmApi.h"
#define CALL_DISP_RM(x) x

#endif

/**************************** HDMI Library defines, enums and structs ***************************/
// typedefs
typedef struct tagNVHDMIPKT_CLASS   NVHDMIPKT_CLASS;
typedef struct tagNVHDMIPKT_MEM_MAP NVHDMIPKT_MEM_MAP;

// Register read/write defines
#define REG_RD32(reg, offset)       (*(((volatile NvU32*)(reg)) + ((offset)/4)))
#define REG_WR32(reg, offset, data) ((*(((volatile NvU32*)(reg)) + ((offset)/4))) = (data))

#define NVHDMIPKT_INVALID_SUBDEV              (0xFFFFFFFF)
#define NVHDMIPKT_DONT_USE_TIMER
#define NVHDMIPKT_STATUS_READ_TIMEOUT_IN_us   (1*1000*1000) /* us - micro second */

// Disp SF User memory map and handle structure
struct tagNVHDMIPKT_MEM_MAP
{
    NvU32 subDevice;
    NvU32 memHandle;
    void* pMemBase;
};

// HDMIPKT print define
#if defined (DEBUG)
    #define NvHdmiPkt_Print(_p, ...)                                                     \
        do {                                                                             \
            if ((_p)->callback.print)                                                    \
            {                                                                            \
                (_p)->callback.print((_p)->cbHandle, "HdmiPacketLibrary: " __VA_ARGS__); \
            }                                                                            \
        } while(0)
#else
    #define NvHdmiPkt_Print(_p, ...) /* nothing */
#endif


// HDMIPKT assert define
#if defined (DEBUG)
    #define NvHdmiPkt_AssertP(p, expr)  ((p)->callback.assert ?                           \
                                         (p)->callback.assert((p)->cbHandle, !!(expr)) : 0)
    #define NvHdmiPkt_Assert(expr)      NvHdmiPkt_AssertP(pThis, expr)
#else
    #define NvHdmiPkt_AssertP(p, expr)
    #define NvHdmiPkt_Assert(expr)
#endif


// Prototypes for common functions shared across implementations.
extern void hdmiWriteDummyPacket(NVHDMIPKT_CLASS*, NvU32*, NvU32, NvU32, NvU8 const *const);
extern NVHDMIPKT_RESULT hdmiAssessLinkCapabilitiesDummy(NVHDMIPKT_CLASS             *pThis,
                                                        NvU32                        subDevice,
                                                        NvU32                        displayId,
                                                        NVT_EDID_INFO         const * const pSinkEdid,
                                                        HDMI_SRC_CAPS               *pSrcCaps,
                                                        HDMI_SINK_CAPS              *pSinkCaps);
extern NVHDMIPKT_RESULT hdmiQueryFRLConfigDummy(NVHDMIPKT_CLASS                     *pThis,
                                                HDMI_VIDEO_TRANSPORT_INFO     const * const pVidTransInfo,
                                                HDMI_QUERY_FRL_CLIENT_CONTROL const * const pClientCtrl,
                                                HDMI_SRC_CAPS                 const * const pSrcCaps,
                                                HDMI_SINK_CAPS                const * const pSinkCaps,
                                                HDMI_FRL_CONFIG                   *pFRLConfig);
extern NVHDMIPKT_RESULT hdmiSetFRLConfigDummy(NVHDMIPKT_CLASS             *pThis,
                                              NvU32                        subDevice,
                                              NvU32                        displayId,
                                              NvBool                       bFakeLt,
                                              HDMI_FRL_CONFIG             *pFRLConfig);
extern NVHDMIPKT_RESULT hdmiClearFRLConfigDummy(NVHDMIPKT_CLASS    *pThis,
                                                NvU32               subDevice,
                                                NvU32               displayId);

#endif //_NVHDMIPKT_COMMON_H_
