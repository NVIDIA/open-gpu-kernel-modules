//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//  File:       nvt_tv.c
//
//  Purpose:    calculate tv based timing timing
//
//*****************************************************************************

#include "nvBinSegment.h"

#include "nvtiming_pvt.h"

PUSH_SEGMENTS

CONS_SEGMENT(PAGE_CONS)

static const NVT_TIMING TV_TIMING[] = 
{
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,240, 0,10,6,262, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1407, {0,60,59940,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_NTSC_M, "SDTV:NTSC_M"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,240, 0,10,6,262, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1407, {0,60,59940,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_NTSC_J, "SDTV:NTSC_J"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,288, 0,10,8,312, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1397, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_PAL_M,  "SDTV:PAL_M"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,288, 0,10,8,312, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1397, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_PAL_A,  "SDTV:PAL_A"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,288, 0,10,8,312, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1397, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_PAL_N,  "SDTV:PAL_N"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,288, 0,10,8,312, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1397, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_SDTV_PAL_NC, "SDTV:PAL_NC"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,240, 0,10,6,262, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1407, {0,60,59940,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_480I,   "HDTV(analog):480i"}},
    {720, 0,15,8, 858, NVT_H_SYNC_NEGATIVE,480, 0,10,4,525, NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,2700, {0,60,59940,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_480P,   "HDTV(analog):480p"}},
    {720, 0,21,66,894, NVT_H_SYNC_POSITIVE,288, 0,10,8,312, NVT_V_SYNC_POSITIVE,NVT_INTERLACED, 1397, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_576I,   "HDTV(analog):576i"}},
    {720, 0,10,8, 864, NVT_H_SYNC_NEGATIVE,576, 0,5, 4,625, NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,2700, {0,50,50000,0x0403,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_576P,   "HDTV(analog):576p"}},
    {1280,0,70,80, 1650,NVT_H_SYNC_NEGATIVE,720,0,5, 5,750, NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,7418, {0,60,59940,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_720P,    "HDTV(analog):720p"}},
    {1920,0,44,88,2200,NVT_H_SYNC_NEGATIVE,540, 0,2, 5,562, NVT_V_SYNC_NEGATIVE,NVT_INTERLACED, 7418, {0,60,59940,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_1080I,  "HDTV(analog):1080i"}},
    {1920,0,44,88,2200,NVT_H_SYNC_NEGATIVE,1080,0,4, 5,1125,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,14835,{0,60,59940,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_1080P,  "HDTV(analog):1080p"}},
    {1280,0,400,80,1980,NVT_H_SYNC_NEGATIVE,720,0,5, 5,750, NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,7425, {0,50,50000,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_720P50, "HDTV(analog):720p50"}},
    {1920,0,594,88,2750,NVT_H_SYNC_NEGATIVE,1080,0,4, 5,1125,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,7425,{0,24,24000,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_1080P24,"HDTV(analog):1080p24"}},
    {1920,0,484,88,2640,NVT_H_SYNC_NEGATIVE,540, 0,4, 5,562, NVT_V_SYNC_NEGATIVE,NVT_INTERLACED, 7425,{0,50,50000,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_1080I50,"HDTV(analog):1080i50"}},
    {1920,0,484,88,2640,NVT_H_SYNC_NEGATIVE,1080,0,4, 5,1125,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,14850,{0,50,50000,0x1009,0x1,{0},{0},{0},{0},NVT_STATUS_HDTV_1080P50,"HDTV(analog):1080p50"}},
    {0,0,0,0,0,NVT_H_SYNC_NEGATIVE,0,0,0,0,0,NVT_V_SYNC_NEGATIVE,NVT_PROGRESSIVE,0,{0,0,0,0,0,{0},{0},{0},{0},0,""}}
};

//***********************************************
//** Wrapper Structure to store Fake EDID data **
//***********************************************
typedef struct tagFAKE_TV_EDID
{
    NvU32 EdidType;
    NvU32 EdidSize;
    const NvU8* FakeEdid;
} FAKE_TV_EDID;

// calculate the backend TV timing
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_GetTvTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NvU32 tvFormat, NVT_TIMING *pT)
{
    NvU32 i, j, k;

    // input check
    if (pT == NULL)
        return NVT_STATUS_ERR;

    if ((width == 0 || height == 0 || rr == 0) && tvFormat >= NVT_MAX_TV_FORMAT)
        return NVT_STATUS_ERR;

    // handle double scan
    if (height <= NVT_PVT_DOUBLE_SCAN_HEIGHT)
    {
        width <<= 1;
        height <<= 1;
    }

    // try the exact match first
    if (tvFormat != NVT_AUTO_HDTV_FORMAT)
    {
        i = 0;
        while (TV_TIMING[i].HVisible != 0)
        {
            if (NVT_GET_TIMING_STATUS_SEQ(TV_TIMING[i].etc.status) == tvFormat)
            {
                // find the match
                *pT = TV_TIMING[i];
                return NVT_STATUS_SUCCESS;
            }

            // move to the next entry
            i++;
        }

        // unknown TV format, return failure here
        *pT = TV_TIMING[0];
        return NVT_STATUS_ERR;
    }

    // we are doing auto HDTV format binding here
    i = 0;
    j = k = sizeof(TV_TIMING)/sizeof(TV_TIMING[0]) - 1;
    while (TV_TIMING[i].HVisible != 0)
    {
        // #1: try the exact resolution/refreshrate/interlaced match
        if (width  == TV_TIMING[i].HVisible &&
            height == frame_height(TV_TIMING[i])&&
            rr     == TV_TIMING[i].etc.rr &&
            !!(flag & NVT_PVT_INTERLACED_MASK) == !!TV_TIMING[i].interlaced &&
            NVT_GET_TIMING_STATUS_TYPE(TV_TIMING[i].etc.status) == NVT_TYPE_HDTV)
        {
            // exact match, return from here
            *pT = TV_TIMING[i];
            return NVT_STATUS_SUCCESS;
        }

        // #2: try to closest match with interlaced check ON
        if (!!(flag & NVT_PVT_INTERLACED_MASK) == !!TV_TIMING[i].interlaced &&
            NVT_GET_TIMING_STATUS_TYPE(TV_TIMING[i].etc.status) == NVT_TYPE_HDTV)
        {
            if (abs_delta(width,  TV_TIMING[i].HVisible)      <= abs_delta(width,  TV_TIMING[j].HVisible) &&
                abs_delta(height, frame_height(TV_TIMING[i])) <= abs_delta(height, frame_height(TV_TIMING[j])) &&
                abs_delta(rr,     TV_TIMING[i].etc.rr)        <= abs_delta(rr,     TV_TIMING[j].etc.rr) &&
                width  <= TV_TIMING[i].HVisible &&
                height <= frame_height(TV_TIMING[i]))
            {
                j = i;
            }
        }

        // #3: try to closest match with interlaced check OFF
        if (NVT_GET_TIMING_STATUS_TYPE(TV_TIMING[i].etc.status) == NVT_TYPE_HDTV)
        {
            if (abs_delta(width,  TV_TIMING[i].HVisible)      <= abs_delta(width,  TV_TIMING[k].HVisible) &&
                abs_delta(height, frame_height(TV_TIMING[i])) <= abs_delta(height, frame_height(TV_TIMING[k])) &&
                abs_delta(rr,     TV_TIMING[i].etc.rr)        <= abs_delta(rr,     TV_TIMING[j].etc.rr) &&
                width  <= TV_TIMING[i].HVisible &&
                height <= frame_height(TV_TIMING[i]))
            {
                k = i;
            }
        }

        // move to the next entry
        i++;
    }

    // return the closest matched timing here
    if (TV_TIMING[j].HVisible != 0)
    {
        *pT = TV_TIMING[j];
    }
    else if (TV_TIMING[k].HVisible != 0)
    {
        *pT = TV_TIMING[k];
    }
    else
    {
        *pT = TV_TIMING[0];
    }

    // set the mismatch status
    if (pT->HVisible != width || frame_height(*pT) != height)
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_SIZE);
    }
    if (pT->etc.rr != rr)
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_RR);
    }
    if (!!pT->interlaced != !!(flag & NVT_PVT_INTERLACED_MASK))
    {
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_FORMAT);
    }

    return NVT_STATUS_SUCCESS;

}

POP_SEGMENTS
