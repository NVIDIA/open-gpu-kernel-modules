//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//  SPDX-License-Identifier: MIT
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//  File:       nvtiming_pvt.h
//
//  Purpose:    the private functions/structures which are only used inside
//              the nv timing library.
//
//*****************************************************************************

#ifndef __NVTIMING_PVT_H_
#define __NVTIMING_PVT_H_

#include "nvtiming.h"

#if defined(NVT_USE_NVKMS)
  #include "nvidia-modeset-os-interface.h"
  #define NVT_SNPRINTF nvkms_snprintf
#else
  #include <string.h>
  #include <stdio.h>
    #define NVT_SNPRINTF snprintf
#endif

#define nvt_assert(p) ((void)0)

#ifdef DD_UNITTEST
#undef nvt_assert(p)
#define nvt_assert(p) ((void)0)
#endif // DD_UNITTEST

#include <stddef.h> // NULL

#ifdef __cplusplus
extern "C" {
#endif

// EDID related private functions
NvU32      getEdidVersion(NvU8 *pData, NvU32 *pVer);
NvBool     assignNextAvailableTiming(NVT_EDID_INFO *pInfo, const NVT_TIMING *pTiming);
void       parseEdidCvtTiming(NVT_EDID_INFO *pInfo);
void       parseEdidEstablishedTiming(NVT_EDID_INFO *pInfo);
void       parseEdidStandardTiming(NVT_EDID_INFO *pInfo);
void       parseEdidDetailedTiming(NvU8 *pEdid, NVT_EDID_INFO *pInfo);
NVT_STATUS parseEdidDetailedTimingDescriptor(NvU8 *pDTD, NVT_TIMING *pT);
void       parseEdidCvt3ByteDescriptor(NvU8 *p, NVT_EDID_INFO *pInfo, NvU32 *vtbCount);
void       parseEdidStandardTimingDescriptor(NvU16 timing, NVT_EDID_INFO *pInfo, NvU32 count, NVT_TIMING * pT);
void       parseVTBExtension(NvU8 *pEdidExt, NVT_EDID_INFO *pInfo);
void       updateHDMILLCDeepColorForTiming(NVT_EDID_INFO *pInfo, NvU32 index);
void       updateBpcForTiming(NVT_EDID_INFO *pInfo, NvU32 index);
void       updateColorFormatAndBpcTiming(NVT_EDID_INFO *pInfo);
// End EDID

// CTA861 related private functions
NVT_STATUS get861ExtInfo(NvU8 *pEdid, NvU32 edidSize, NVT_EDID_CEA861_INFO *p);
NVT_STATUS parseCta861DataBlockInfo(NvU8 *pEdid, NvU32 size, NVT_EDID_CEA861_INFO *p);
void       parse861ExtDetailedTiming(NvU8 *pEdidExt, NvU8 basicCaps, NVT_EDID_INFO *pInfo);
void       parse861bShortTiming(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parse861bShortYuv420Timing(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861VideoFormatDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo);
void       parseCta861NativeOrPreferredTiming(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861VsdbBlocks(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861VsvdbBlocks(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861HfScdb(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861HfEeodb(NVT_EDID_CEA861_INFO *pExt861, NvU32 *pTotalEdidExtensions);
void       parseEdidMsftVsdbBlock(VSDB_DATA *pVsdb, MSFT_VSDB_PARSED_INFO *vsdbInfo);
void       parseEdidHdmiLlcBasicInfo(VSDB_DATA *pVsdb, NVT_HDMI_LLC_INFO *pHdmiLlc);
void       parseEdidHdmiForumVSDB(VSDB_DATA *pVsdb, NVT_HDMI_FORUM_INFO *pHdmiInfo);
void       getEdidHDM1_4bVsdbTiming(NVT_EDID_INFO *pInfo);
void       parseEdidHDMILLCTiming(NVT_EDID_INFO *pInfo, VSDB_DATA *pVsdb, NvU32 *pSupported, HDMI3DSUPPORTMAP * pM);
void       parseEdidNvidiaVSDBBlock(VSDB_DATA *pVsdb, NVDA_VSDB_PARSED_INFO *vsdbInfo);
void       parseCta861HdrStaticMetadataDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo, NVT_CTA861_ORIGIN flag);
void       parseCta861DvStaticMetadataDataBlock(VSVDB_DATA* pVsvdb, NVT_DV_STATIC_METADATA* pDvInfo);
void       parseCta861Hdr10PlusDataBlock(VSVDB_DATA* pVsvdb, NVT_HDR10PLUS_INFO* pHdr10PlusInfo);
void       parseCta861DIDType7VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo);
void       parseCta861DIDType8VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo);
void       parseCta861DIDType10VideoTimingDataBlock(NVT_EDID_CEA861_INFO *pExt861, void *pRawInfo);
NvBool     isMatchedCTA861Timing(NVT_EDID_INFO *pInfo, NVT_TIMING *pT);
NvBool     isMatchedStandardTiming(NVT_EDID_INFO *pInfo, NVT_TIMING *pT);
NvBool     isMatchedEstablishedTiming(NVT_EDID_INFO *pInfo, NVT_TIMING *pT);
NvU32      isHdmi3DStereoType(NvU8 StereoStructureType);
NvU32      getCEA861TimingAspectRatio(NvU32 vic);
NvU8       getHighestPrioritySVRIdx(const NVT_EDID_CEA861_INFO *pExt861);
void       SetActiveSpaceForHDMI3DStereo(const NVT_TIMING *pTiming, NVT_EXT_TIMING *pExtTiming);
void       AddModeToSupportMap(HDMI3DSUPPORTMAP * pMap, NvU8 vic, NvU8 structure, NvU8 Detail);
void       getMonitorDescriptorString(NvU8 *pEdid, NvU8 tag, char *str, int onceOnly);
// End CTA861

// DispalyID base / extension related functions
NvU32      getDID2Version(NvU8 *pData, NvU32 *pVer);
NVT_STATUS getDisplayIdEDIDExtInfo(NvU8* pEdid, NvU32 edidSize, NVT_EDID_INFO* pEdidInfo);
NVT_STATUS parseDisplayIdBlock(NvU8* pBlock, NvU8 max_length, NvU8* pLength, NVT_EDID_INFO* pEdidInfo);
NVT_STATUS getDisplayId20EDIDExtInfo(NvU8* pDisplayid, NvU32 edidSize, NVT_EDID_INFO* pEdidInfo);
NVT_STATUS parseDisplayId20EDIDExtDataBlocks(NvU8* pDataBlock, NvU8 remainSectionLength, NvU8* pCurrentDBLength, NVT_EDID_INFO* pEdidInfo);
NVT_STATUS parseDisplayId20Timing7Descriptor(const void *pDescriptor, NVT_TIMING *pTiming, NvU8 count);
NVT_STATUS parseDisplayId20Timing8Descriptor(const void *pDescriptor, NVT_TIMING *pTiming, NvU8 codeType, NvU8 codeSize, NvU8 index, NvU8 count);
NVT_STATUS parseDisplayId20Timing9Descriptor(const void *pDescriptor, NVT_TIMING *pTiming, NvU8 count);
NVT_STATUS parseDisplayId20Timing10Descriptor(const void *pDescriptor, NVT_TIMING *pTiming, NvU8 payloadBytes, NvU8 count);
void       updateColorFormatForDisplayIdExtnTimings(NVT_EDID_INFO* pInfo, NvU32 timingIdx);
void       updateColorFormatForDisplayId20ExtnTimings(NVT_EDID_INFO* pInfo, NvU32 timingIdx);
NvBool     assignNextAvailableDisplayId20Timing(NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, const NVT_TIMING *pTiming);
void       updateColorFormatForDisplayId20Timings(NVT_DISPLAYID_2_0_INFO* pDisplayId2Info, NvU32 timingIdx);
// End DisplayID
#ifdef __cplusplus
}
#endif

NvU32 axb_div_c_old(NvU32 a, NvU32 b, NvU32 c);

#define NVT_EDID_BLOCK_SIZE             128

#define NVT_PVT_INTERLACED_MASK         0xF
#define NVT_PVT_DOUBLESCAN_MASK         0x10
#define NVT_PVT_RB_MASK                 0x20

#define NVT_PVT_DOUBLE_SCAN_HEIGHT      384
#define NVT_PVT_DOUBLE_SCAN_HEIGHT_VGA  600
#define NVT_PVT_DOUBLE_SCAN_PCLK_MIN    1200     //in 10KHz

#define abs(a) ((a)>0?(a):-(a))
#define set_rrx1k(a,b,c) ((b)*(c)==0?(0):(NvU32)(((NvU64)(a)*10000*1000+(b)*(c)/2)/((b)*(c))))
#define frame_height(a) ((NvU32)((a).VVisible * ((a).interlaced!=0?2:1)))
#define nvt_is_wideaspect(width,height) ((width)*5 >= (height)*8)

#ifndef MIN
#define MIN(x, y) ((x)>(y) ? (y) : (x))
#endif
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif


#ifndef COUNT
#define COUNT(a) (sizeof(a)/sizeof(a[0]))
#endif
#ifndef offsetof
#define offsetof(st, m) ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))
#endif
#define nvt_nvu8_set_bits(d, s, m, shift) {(d)&=(NvU8)((NvU8)(m)^0xFFU);(d)|=((s)<<(shift))&(m);}
#define nvt_get_bits(d, m, shift) (((d)&(m))>>shift)
#define nvt_lowest_bit(n) ((n)&(~((n)-1)))
#define nvt_aspect_x(n) ((n)>>16)
#define nvt_aspect_y(n) ((n)&0xFFFF)

// Sentinel values for NVT_TIMING
#define NVT_TIMINGEXT_SENTINEL {0,0,0,0,0,{0},{0},{0},{0},0,""}
#define NVT_TIMING_SENTINEL {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,NVT_TIMINGEXT_SENTINEL}

#endif //__NVTIMING_PVT_H_

