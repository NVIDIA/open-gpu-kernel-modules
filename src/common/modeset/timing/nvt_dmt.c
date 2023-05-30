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
//  File:       nvt_dmt.c
//
//  Purpose:    calculate DMT/DMT-RB timing
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "nvtiming_pvt.h"

PUSH_SEGMENTS

// DMT table 2-1
// Macro to declare a TIMING initializer for given parameters without border
#define DMT_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rr,pclk,id) \
{hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',NVT_PROGRESSIVE,pclk,{0,rr,set_rrx1k(pclk,ht,vt),0,0x1,{0},{0},{0},{0},NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT,id),"VESA DMT"}}

#define DMTRB_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rr,pclk,id) \
{hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',NVT_PROGRESSIVE,pclk,{0,rr,set_rrx1k(pclk,ht,vt),0,0x1,{0},{0},{0},{0},NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT_RB,id),"VESA DMT/RB"}}

#define DMTRB_2_TIMING(hv,hfp,hsw,ht,hsp,vv,vfp,vsw,vt,vsp,rr,pclk,id) \
{hv,0,hfp,hsw,ht,(hsp)=='-',vv,0,vfp,vsw,vt,(vsp)=='-',NVT_PROGRESSIVE,pclk,{0,rr,set_rrx1k(pclk,ht,vt),0,0x1,{0},{0},{0},{0},NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT_RB_2,id),"VESA DMT/RB2"}}

DATA_SEGMENT(PAGE_DATA)

static NVT_TIMING DMT[] =
{
    // VESA standard entries (ordered according to VESA DMT ID).
    //             hv,hfp,hsw,  ht,hsp,  vv,vfp,vsw,  vt,vsp, rr,pclk , id
    DMT_TIMING  ( 640, 32, 64, 832,'+', 350, 32,  3, 445,'-', 85, 3150, 0x01),
    DMT_TIMING  ( 640, 32, 64, 832,'-', 400,  1,  3, 445,'+', 85, 3150, 0x02),
    DMT_TIMING  ( 720, 36, 72, 936,'-', 400,  1,  3, 446,'+', 85, 3550, 0x03),
    DMT_TIMING  ( 640,  8, 96, 800,'-', 480,  2,  2, 525,'-', 60, 2518, 0x04),
    // 640x480x72Hz (VESA) - this entry have borders
    {640,8,16,40,832,NVT_H_SYNC_NEGATIVE,480,8,1,3,520,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,3150,{0,72,72000,0,1,{0},{0},{0},{0},NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT,5),"VESA DMT"}},
    DMT_TIMING  ( 640, 16, 64, 840,'-', 480,  1,  3, 500,'-', 75, 3150, 0x06),
    DMT_TIMING  ( 640, 56, 56, 832,'-', 480,  1,  3, 509,'-', 85, 3600, 0x07),
    DMT_TIMING  ( 800, 24, 72,1024,'+', 600,  1,  2, 625,'+', 56, 3600, 0x08),
    DMT_TIMING  ( 800, 40,128,1056,'+', 600,  1,  4, 628,'+', 60, 4000, 0x09),
    DMT_TIMING  ( 800, 56,120,1040,'+', 600, 37,  6, 666,'+', 72, 5000, 0x0A),
    DMT_TIMING  ( 800, 16, 80,1056,'+', 600,  1,  3, 625,'+', 75, 4950, 0x0B),
    DMT_TIMING  ( 800, 32, 64,1048,'+', 600,  1,  3, 631,'+', 85, 5625, 0x0C),
    DMTRB_TIMING( 800, 48, 32, 960,'+', 600,  3,  4, 636,'-',120, 7325, 0x0D),
    DMT_TIMING  ( 848, 16,112,1088,'+', 480,  6,  8, 517,'+', 60, 3375, 0x0E),
    DMT_TIMING  (1024,  8,176,1264,'+', 768,  0,  4, 817,'+', 43, 4490, 0x0F),
    DMT_TIMING  (1024, 24,136,1344,'-', 768,  3,  6, 806,'-', 60, 6500, 0x10),
    DMT_TIMING  (1024, 24,136,1328,'-', 768,  3,  6, 806,'-', 70, 7500, 0x11),
    DMT_TIMING  (1024, 16, 96,1312,'+', 768,  1,  3, 800,'+', 75, 7875, 0x12),
    DMT_TIMING  (1024, 48, 96,1376,'+', 768,  1,  3, 808,'+', 85, 9450, 0x13),
    DMTRB_TIMING(1024, 48, 32,1184,'+', 768,  3,  4, 813,'-',120,11550, 0x14),
    DMT_TIMING  (1152, 64,128,1600,'+', 864,  1,  3, 900,'+', 75,10800, 0x15),
    DMTRB_TIMING(1280, 48, 32,1440,'+', 768,  3,  7, 790,'-', 60, 6825, 0x16),
    DMT_TIMING  (1280, 64,128,1664,'-', 768,  3,  7, 798,'+', 60, 7950, 0x17),
    DMT_TIMING  (1280, 80,128,1696,'-', 768,  3,  7, 805,'+', 75,10225, 0x18),
    DMT_TIMING  (1280, 80,136,1712,'-', 768,  3,  7, 809,'+', 85,11750, 0x19),
    DMTRB_TIMING(1280, 48, 32,1440,'+', 768,  3,  7, 813,'-',120,14025, 0x1A),
    DMTRB_TIMING(1280, 48, 32,1440,'+', 800,  3,  6, 823,'-', 60, 7100, 0x1B),
    DMT_TIMING  (1280, 72,128,1680,'-', 800,  3,  6, 831,'+', 60, 8350, 0x1C),
    DMT_TIMING  (1280, 80,128,1696,'-', 800,  3,  6, 838,'+', 75,10650, 0x1D),
    DMT_TIMING  (1280, 80,136,1712,'-', 800,  3,  6, 843,'+', 85,12250, 0x1E),
    DMTRB_TIMING(1280, 48, 32,1440,'+', 800,  3,  6, 847,'-',120,14625, 0x1F),
    DMT_TIMING  (1280, 96,112,1800,'+', 960,  1,  3,1000,'+', 60,10800, 0x20),
    DMT_TIMING  (1280, 64,160,1728,'+', 960,  1,  3,1011,'+', 85,14850, 0x21),
    DMTRB_TIMING(1280, 48, 32,1440,'+', 960,  3,  4,1017,'-',120,17550, 0x22),
    DMT_TIMING  (1280, 48,112,1688,'+',1024,  1,  3,1066,'+', 60,10800, 0x23),
    DMT_TIMING  (1280, 16,144,1688,'+',1024,  1,  3,1066,'+', 75,13500, 0x24),
    DMT_TIMING  (1280, 64,160,1728,'+',1024,  1,  3,1072,'+', 85,15750, 0x25),
    DMTRB_TIMING(1280, 48, 32,1440,'+',1024,  3,  7,1084,'-',120,18725, 0x26),
    DMT_TIMING  (1360, 64,112,1792,'+', 768,  3,  6, 795,'+', 60, 8550, 0x27),
    DMTRB_TIMING(1360, 48, 32,1520,'+', 768,  3,  5, 813,'-',120,14825, 0x28),
    DMTRB_TIMING(1400, 48, 32,1560,'+',1050,  3,  4,1080,'-', 60,10100, 0x29),
    DMT_TIMING  (1400, 88,144,1864,'-',1050,  3,  4,1089,'+', 60,12175, 0x2A),
    DMT_TIMING  (1400,104,144,1896,'-',1050,  3,  4,1099,'+', 75,15600, 0x2B),
    DMT_TIMING  (1400,104,152,1912,'-',1050,  3,  4,1105,'+', 85,17950, 0x2C),
    DMTRB_TIMING(1400, 48, 32,1560,'+',1050,  3,  4,1050,'-',120,20800, 0x2D),
    DMTRB_TIMING(1440, 48, 32,1600,'+', 900,  3,  6, 926,'-', 60, 8875, 0x2E),
    DMT_TIMING  (1440, 80,152,1904,'-', 900,  3,  6, 934,'+', 60,10650, 0x2F),
    DMT_TIMING  (1440, 96,152,1936,'-', 900,  3,  6, 942,'+', 75,13675, 0x30),
    DMT_TIMING  (1440,104,152,1952,'-', 900,  3,  6, 948,'+', 85,15700, 0x31),
    DMTRB_TIMING(1440, 48, 32,1600,'+', 900,  3,  6, 953,'-',120,18275, 0x32),
    DMT_TIMING  (1600, 64,192,2160,'+',1200,  1,  3,1250,'+', 60,16200, 0x33),
    DMT_TIMING  (1600, 64,192,2160,'+',1200,  1,  3,1250,'+', 65,17550, 0x34),
    DMT_TIMING  (1600, 64,192,2160,'+',1200,  1,  3,1250,'+', 70,18900, 0x35),
    DMT_TIMING  (1600, 64,192,2160,'+',1200,  1,  3,1250,'+', 75,20250, 0x36),
    DMT_TIMING  (1600, 64,192,2160,'+',1200,  1,  3,1250,'+', 85,22950, 0x37),
    DMTRB_TIMING(1600, 48, 32,1760,'+',1200,  3,  4,1271,'-',120,26825, 0x38),
    DMTRB_TIMING(1680, 48, 32,1840,'+',1050,  3,  6,1080,'-', 60,11900, 0x39),
    DMT_TIMING  (1680,104,176,2240,'-',1050,  3,  6,1089,'+', 60,14625, 0x3A),
    DMT_TIMING  (1680,120,176,2272,'-',1050,  3,  6,1099,'+', 75,18700, 0x3B),
    DMT_TIMING  (1680,128,176,2288,'-',1050,  3,  6,1105,'+', 85,21475, 0x3C),
    DMTRB_TIMING(1680, 48, 32,1840,'+',1050,  3,  6,1112,'-',120,24550, 0x3D),
    DMT_TIMING  (1792,128,200,2448,'-',1344,  1,  3,1394,'+', 60,20475, 0x3E),
    DMT_TIMING  (1792, 96,216,2456,'-',1344,  1,  3,1417,'+', 75,26100, 0x3F),
    DMTRB_TIMING(1792, 48, 32,1952,'+',1344,  3,  4,1423,'-',120,33325, 0x40),
    DMT_TIMING  (1856, 96,224,2528,'-',1392,  1,  3,1439,'+', 60,21825, 0x41),
    DMT_TIMING  (1856,128,224,2560,'-',1392,  1,  3,1500,'+', 75,28800, 0x42),
    DMTRB_TIMING(1856, 48, 32,2016,'+',1392,  3,  4,1474,'-',120,35650, 0x43),
    DMTRB_TIMING(1920, 48, 32,2080,'+',1200,  3,  6,1235,'-', 60,15400, 0x44),
    DMT_TIMING  (1920,136,200,2592,'-',1200,  3,  6,1245,'+', 60,19325, 0x45),
    DMT_TIMING  (1920,136,208,2608,'-',1200,  3,  6,1255,'+', 75,24525, 0x46),
    DMT_TIMING  (1920,144,208,2624,'-',1200,  3,  6,1262,'+', 85,28125, 0x47),
    DMTRB_TIMING(1920, 48, 32,2080,'+',1200,  3,  6,1271,'-',120,31700, 0x48),
    DMT_TIMING  (1920,128,208,2600,'-',1440,  1,  3,1500,'+', 60,23400, 0x49),
    DMT_TIMING  (1920,144,224,2640,'-',1440,  1,  3,1500,'+', 75,29700, 0x4A),
    DMTRB_TIMING(1920, 48, 32,2080,'+',1440,  3,  4,1525,'-',120,38050, 0x4B),
    DMTRB_TIMING(2560, 48, 32,2720,'+',1600,  3,  6,1646,'-', 60,26850, 0x4C),
    DMT_TIMING  (2560,192,280,3504,'-',1600,  3,  6,1658,'+', 60,34850, 0x4D),
    DMT_TIMING  (2560,208,280,3536,'-',1600,  3,  6,1672,'+', 75,44325, 0x4E),
    DMT_TIMING  (2560,208,280,3536,'-',1600,  3,  6,1682,'+', 85,50525, 0x4F),
    DMTRB_TIMING(2560, 48, 32,2720,'+',1600,  3,  6,1694,'-',120,55275, 0x50),
    DMT_TIMING  (1366, 70,143,1792,'+',768 ,  3,  3, 798,'+', 60, 8550, 0x51),//1366 x 768  @60 (non-interlaced) DMT ID: 51h
    DMT_TIMING  (1920, 88, 44,2200,'+',1080,  4,  5,1125,'+', 60,14850, 0x52),//1920 x 1080 @60 (non-interlaced) DMT ID: 52h
    DMTRB_TIMING(1600, 24, 80,1800,'+', 900,  1,  3,1000,'+', 60,10800, 0x53),//1600 x 900  @60 (non-interlaced) DMT ID: 53h
    DMTRB_TIMING(2048, 26, 80,2250,'+',1152,  1,  3,1200,'+', 60,16200, 0x54),//2048 x 1152 @60 (non-interlaced) DMT ID: 54h
    DMT_TIMING  (1280,110, 40,1650,'+', 720,  5,  5, 750,'+', 60, 7425, 0x55),//1280 x 720  @60 (non-interlaced) DMT ID: 55h
    DMTRB_TIMING(1366, 14, 56,1500,'+', 768,  1,  3, 800,'+', 60, 7200, 0x56),//1366 x 768  @60 (non-interlaced) DMT ID: 56h

    // Added timing definitions in DMT 1.3 Version 1.0, Rev. 13 
    DMTRB_2_TIMING(4096, 8, 56,4176,'+', 2160, 48, 8, 2222,'-', 60,55674, 0x57),//4096 x 2160 @60 (non-interlaced) DMT ID: 57h
    DMTRB_2_TIMING(4096, 8, 56,4176,'+', 2160, 48, 8, 2222,'-', 59,55619, 0x58),//4096 x 2160 @60 (non-interlaced) DMT ID: 58h

    // ********************************
    // Additional non-standard entries.
    // ********************************

    // Settings for 640x400
    // GTF timing for 640x400x60Hz has too low HFreq, this is a
    // Specially constructed timing from 640x480, with extra blanking
    // on top and bottom of the screen

    DMT_TIMING(640,16,96,800,'-',400,50,2,525,'-',60,2518,0),
    DMT_TIMING(640,16,96,800,'+',400,12,2,449,'-',70,2518,0),

    // the end of table
    NVT_TIMING_SENTINEL
};
static NvU32 MAX_DMT_FORMAT = sizeof(DMT)/sizeof(DMT[0]) - 1;

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumDMT(NvU32 dmtId, NVT_TIMING *pT)
{
    if ((pT == NULL) || (dmtId == 0))
    {
        return NVT_STATUS_ERR;
    }

    // The last entry is not used.
    if (dmtId > MAX_DMT_FORMAT)
    {
        return NVT_STATUS_ERR;
    }

    // Make sure that the DMT ID matches according to the array index.
    if (NVT_GET_TIMING_STATUS_SEQ(DMT[dmtId - 1].etc.status) == dmtId)
    {
        *pT = DMT[dmtId - 1];

        pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk,
                                  (NvU32)10000*(NvU32)1000,
                                  (NvU32)pT->HTotal*(NvU32)pT->VTotal);
        NVT_SNPRINTF((char *)pT->etc.name, 40, "DMT:#%d:%dx%dx%dHz",
                     dmtId, pT->HVisible, pT->VVisible, pT->etc.rr);
        ((char *)pT->etc.name)[39] = '\0';

        return NVT_STATUS_SUCCESS;
    }

    return NVT_STATUS_ERR;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_EnumStdTwoBytesCode(NvU16 std2ByteCode, NVT_TIMING *pT)
{    
    NvU32 aspect, width, height, rr;

    if ((pT == NULL) || (std2ByteCode == 0))
    {
        return NVT_STATUS_ERR;
    }

    // The value in the EDID = (Horizontal active pixels/8) - 31
    width = (std2ByteCode & 0x0FF) + 31;
    width <<= 3;
    rr = ((std2ByteCode >> 8) & 0x3F) + 60; // bits 5->0

    // get the height
    aspect = ((std2ByteCode >> 8) & 0xC0); // aspect ratio at bit 7:6

         if (aspect == 0x00)    height = width * 5 / 8;  // 16:10
    else if (aspect == 0x40)    height = width * 3 / 4;  // 4:3
    else if (aspect == 0x80)    height = width * 4 / 5;  // 5:4
    else                        height = width * 9 / 16; // 16:9

    // try to get the timing from DMT or DMT_RB
    if (NvTiming_CalcDMT(width, height, rr, 0, pT) == NVT_STATUS_SUCCESS)
    {
        return NVT_STATUS_SUCCESS;
    }
    // try to get the timing from DMT_RB2
    else if (NvTiming_CalcDMT_RB2(width, height, rr, 0, pT) == NVT_STATUS_SUCCESS)
    {
        return NVT_STATUS_SUCCESS;
    }

    return NVT_STATUS_ERR;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcDMT(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NVT_TIMING *p = (NVT_TIMING *)DMT;

    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0 )
        return NVT_STATUS_ERR;

    // no interlaced DMT timing
    if ((flag & NVT_PVT_INTERLACED_MASK) != 0)
        return NVT_STATUS_ERR;

    while (p->HVisible != 0 && p->VVisible != 0)
    {
        if (NVT_GET_TIMING_STATUS_TYPE(p->etc.status) == NVT_TYPE_DMT)
        {
            if ((NvU32)p->HVisible == width &&
                (NvU32)p->VVisible == height &&
                (NvU32)p->etc.rr == rr)
            {
                NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
                *pT = *p;
                pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
                NVT_SNPRINTF((char *)pT->etc.name, 40, "DMT:%dx%dx%dHz",width, height, rr);
                pT->etc.name[39] = '\0';
                pT->etc.rgb444.bpc.bpc8 = 1;
                return NVT_STATUS_SUCCESS;
            }
        }   
        p ++;
    }

    // if we couldn't find a DMT with regular blanking, try the DMT with reduced blanking next
    return NvTiming_CalcDMT_RB(width, height, rr, flag, pT);
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcDMT_RB(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NVT_TIMING *p = (NVT_TIMING *)DMT;

    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0 )
        return NVT_STATUS_ERR;

    // no interlaced DMT timing
    if ((flag & NVT_PVT_INTERLACED_MASK) != 0)
        return NVT_STATUS_ERR;

    while (p->HVisible != 0 && p->VVisible != 0)
    {
        // select only reduced-bandwidth timing.
        if (NVT_GET_TIMING_STATUS_TYPE(p->etc.status) == NVT_TYPE_DMT_RB)
        {
            if ((NvU32)p->HVisible == width &&
                (NvU32)p->VVisible == height &&
                (NvU32)p->etc.rr == rr)
            {
                NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
                *pT = *p;
                pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
                NVT_SNPRINTF((char *)pT->etc.name, 40, "DMT-RB:%dx%dx%dHz",width, height, rr);
                pT->etc.name[39] = '\0';
                pT->etc.rgb444.bpc.bpc8 = 1;
                return NVT_STATUS_SUCCESS;
            }
        }
        p ++;
    }
    return NVT_STATUS_ERR;
}

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcDMT_RB2(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NVT_TIMING *p = (NVT_TIMING *)DMT;

    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0)
        return NVT_STATUS_ERR;

    // no interlaced DMT timing
    if ((flag & NVT_PVT_INTERLACED_MASK) != 0)
        return NVT_STATUS_ERR;

    while (p->HVisible != 0 && p->VVisible != 0)
    {
        // select only reduced-bandwidth timing.
        if (NVT_GET_TIMING_STATUS_TYPE(p->etc.status) == NVT_TYPE_DMT_RB_2)
        {
            if ((NvU32)p->HVisible == width &&
                (NvU32)p->VVisible == height &&
                (NvU32)p->etc.rr == rr)
            {
                NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
                *pT = *p;
                pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
                NVT_SNPRINTF((char *)pT->etc.name, 40, "DMT-RB2:%dx%dx%dHz",width, height, rr);
                pT->etc.name[39] = '\0';
                pT->etc.rgb444.bpc.bpc8 = 1;
                return NVT_STATUS_SUCCESS;
            }
        }
        p ++;
    }
    return NVT_STATUS_ERR;
}

POP_SEGMENTS
