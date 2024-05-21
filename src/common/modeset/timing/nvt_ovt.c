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
//  File:       nvt_ovt.c
//
//  Purpose:    calculate Optimized Video Timing (OVT) timing
//
//*****************************************************************************


#include "nvBinSegment.h"
#include "nvmisc.h"

#include "nvtiming_pvt.h"

PUSH_SEGMENTS

CONS_SEGMENT(PAGE_CONS)

const NvU32 NVT_OVT_PIXEL_CLOCK_GRANULARITY  = 1000;      // Resulting Pixel Clock will be a multiple of this
const NvU32 NVT_OVT_MIN_H_TOTAL_GRANULARITY  = 8;         // Resulting Htotal value will be a multiple of this
const NvU32 NVT_OVT_MIN_V_BLANK_MICROSEC     = 460;       // Minimum duration of Vblank (us)
const NvU32 NVT_OVT_MIN_V_SYNC_LEADING_EDGE  = 400;       // Minimum duration of Vsync + Vback (us)
const NvU32 NVT_OVT_MIN_CLOCK_RATE_420       = 590000000; // interface-specific minimum pixel rate for transport of 4:2:0 sample
const NvU32 NVT_OVT_PIXEL_FACTOR_420         = 2;         // Worst case of two pixels per link character for pixel rates of MinClockRate420 or more
const NvU32 NVT_OVT_MIN_H_BLANK_444          = 80;        // Minimum Hblank width for pixel rates below MinClockRate420  
const NvU32 NVT_OVT_MIN_H_BLANK_420          = 128;       // Minimum Hblank width for pixel rates of MinClockRate420 or more
const NvU32 NVT_OVT_MAX_CHUNK_RATE           = 650000000; // Maximum rate of chunks of pixels with a power-of-two size
const NvU32 NVT_OVT_AUDIO_PACKET_RATE        = 195000;    // 192k sample packets + 3k auxiliary data packets
const NvU32 NVT_OVT_AUDIO_PACKET_SIZE        = 32;        // each packet carries 8 audio sample
const NvU32 NVT_OVT_LINE_OVERHEAD            = 32;        // interface-specific overhead: 32 pixels/line

const NvU32 NVT_OVT_H_SYNC_PIXELS            = 32;
const NvU32 NVT_OVT_H_BACK_WIDTH             = 32;
const NvU32 NVT_OVT_V_SYNC_WIDTH             = 8;

CODE_SEGMENT(PAGE_DD_CODE)
static NvU32 nvFloorPow2_U32(NvU32 x)
{
    return x & ~(x - 1);
}

CODE_SEGMENT(PAGE_DD_CODE)
static NvU32 computeGCD(NvU32 a, NvU32 b)
{
    NvU32 temp;
    while (b != 0)
    {
        temp = a % b;
        if (temp == 0) return b;
        a = b;
        b = temp;
    }
    return a;
}

CODE_SEGMENT(PAGE_DD_CODE)
static NvU32 calculate_aspect_ratio(NVT_TIMING *pT)
{
    NvU32 gcd = computeGCD(pT->HVisible, pT->VVisible);

    if (gcd == 0) 
    { 
        pT->etc.aspect = (NvU32)0;
        return 0;
    }
    
    return (pT->HVisible / gcd) << 16 | (pT->VVisible / gcd);
}

/**
 *                                         OVT Algorithm Calculations Formula
 * 
 * @brief Sinks can indicate supported video formats with VFD in a VFDB that are not represented by a CTA VIC.
 *        The timing parameters of those Video Formats are determined by the Optimized Video Timing(OVT) algorithm
 * 
 * @param width  : resolution width from RID
 * @param height : resolution height from RID
 * @param refreshRate : refresh rate x fraction rate
 * @param pT    : output all the parameters in NVT_TIMING 
 * 
 * @return NVT_STATUS_SUCCESS
  * 
 */
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcOVT(NvU32 width, NvU32 height, NvU32 refreshRate, NVT_TIMING *pT)
{
    NvU32 hTotal = 0;
    NvU32 vTotal = 0;
    NvU32 maxVRate = refreshRate;
    NvU32 vTotalGranularity = 1;
    NvU32 resolutionGranularity = 0;
    NvU32 minVBlank, minVTotal, minLineRate, minHBlank, minHTotal, vBlank, vSyncPosition;
    NvU32 hTotalGranularityChunk, hTotalGranularity, maxAudioPacketsPerLine;
    
    NvU64 minPixelClockRate = 0LL;
    NvU64 pixelClockRate = 0LL;
    NvU64 maxActiveTime  = 0LL;
    NvU64 minLineTime    = 0LL;
    NvU64 minResolution  = 0LL;
    NvU32 V = 0;
    NvU32 H = 0;
    NvU64 R = 0;

    // parameter sanity check 
    if (width % 8 != 0)
        return NVT_STATUS_ERR;

    // ** Preparation **
    // 1. Determine maximum Vrate of frame rate group (see Table 13) and V-Total granularity:
    //    Currently client doesn't have customize refresh rate value as it only from VFDB
    switch (refreshRate)
    {
        case 24: case 25: case 30:
            maxVRate          = 30;
            vTotalGranularity = 20;
        break;
        case 48: case 50: case 60:
            maxVRate          = 60;
            vTotalGranularity = 20;
        break;
        case 100: case 120:
            maxVRate          = 120;
            vTotalGranularity = 5;
        break;
        case 144:
            maxVRate          = 144;
            vTotalGranularity = 1;
        break;
        case 200: case 240:
            maxVRate          = 240;
            vTotalGranularity = 5;
        break;
        case 300: case 360:
            maxVRate          = 360;
            vTotalGranularity = 5;
        break;
        case 400: case 480:
            maxVRate          = 480;
            vTotalGranularity = 5;
        break;
        default:
            vTotalGranularity = 1;
            maxVRate          = refreshRate;
        break;
    }

    // 2. Minimum Vtotal is found from highest frame rate of Vrate group, Vactive and the minimum Vblank time of 460 μSec:
    // 2.1 the minimum number of determine the maximum active time. For the sake of precision, it is multiplied by 1,000,000.
    maxActiveTime = ((NvU64)1000000000000 / (NvU64)maxVRate) - (NvU64)NVT_OVT_MIN_V_BLANK_MICROSEC * 1000000;
    // 2.2 get the minimum line time 
    minLineTime = maxActiveTime / (NvU64)height;
    // 2.3 get the minimum number of VBlank lines. The multiplicand 1000000 is for accuracy, because we multiply it at 2.1 
    minVBlank = (NvU32)(NV_UNSIGNED_DIV_CEIL((NvU64)NVT_OVT_MIN_V_BLANK_MICROSEC * (NvU64)1000000, (NvU64)minLineTime));
    // 2.4 get the minimum total number of lines
    minVTotal = height + minVBlank;
    if (minVTotal % vTotalGranularity !=0)
        minVTotal += (vTotalGranularity - (minVTotal % vTotalGranularity));

    // 3. Find the audio packet rate and use it to determine the required audio packets per line:
    // 3.1 determine a minimum line rate 
    minLineRate = maxVRate * minVTotal; // Hz
    // 3.2 The maximum number of audio packets
    maxAudioPacketsPerLine = NV_UNSIGNED_DIV_CEIL(NVT_OVT_AUDIO_PACKET_RATE, minLineRate);
    
    // 4. Find initial minimum horizontal total size, based on audio requirements (1 pixel = 1 character):
    minHBlank = NVT_OVT_LINE_OVERHEAD + NVT_OVT_AUDIO_PACKET_SIZE * maxAudioPacketsPerLine;
    // 4.1 determine a minimum Horizontal Total pixel (MinHtotal)
    minHTotal = width + NV_MAX(NVT_OVT_MIN_H_BLANK_444, minHBlank);

    // 5. Find hTotal and vTotal so that the pixelClockRate is divisible by the pixelClockGranularity, and 
    //    hTotal is divisible by an appropriate processing chunk size:
    minPixelClockRate = (NvU64)maxVRate * (NvU64)minHTotal * (NvU64)minVTotal; // Hz
    // 5.1 determinate new granularity and minHtotal based on the new granularity
    hTotalGranularityChunk = nvNextPow2_U32((NvU32)NV_UNSIGNED_DIV_CEIL(minPixelClockRate, (NvU64)NVT_OVT_MAX_CHUNK_RATE));
    // 5.2 If this value is greater than the 8, it becomes the new horizontal granularity
    hTotalGranularity = NV_MAX((NvU64)NVT_OVT_MIN_H_TOTAL_GRANULARITY, hTotalGranularityChunk);
    if (minHTotal % hTotalGranularity != 0)
    {
        minHTotal += (hTotalGranularity - (minHTotal % hTotalGranularity));
    }
    // 5.3 optimized by iterating on resolution totals without multiplying by the max refresh rate.
    resolutionGranularity = NVT_OVT_PIXEL_CLOCK_GRANULARITY / computeGCD(NVT_OVT_PIXEL_CLOCK_GRANULARITY, maxVRate);

    // ** OVT Timing Search **
    // 5.4 it will repeat until the found pixel clock is greater than the divisible pixel clock of the search at hte previous vTotal value, 
    //     the hTotal and vTotal values of that preceding search are chosen for the video timing
    for(;;)
    {
        minResolution = 0;
        V = minVTotal;

        for(;;)
        {
            H = minHTotal;
            R = (NvU64)H * (NvU64)V;

            if (minResolution && R > minResolution)
                break;
            
            while (R % resolutionGranularity || maxVRate * R / nvFloorPow2_U32(H) > NVT_OVT_MAX_CHUNK_RATE)
            {
                H += hTotalGranularity;
                R = (NvU64)H * (NvU64)V;
            }

            if (minResolution == 0 || R < minResolution)
            {
                hTotal = H;
                vTotal = V;
                minResolution = R;
            }
            V += vTotalGranularity;
        }

        pixelClockRate = maxVRate * minResolution;

        // 6. Check if timing requires adjustments for 4:2:0:
        // 6.a Re-calculate minHTotal, in pixels, adjusted for 4:2:0 requirements. (2 pixels = 1 character):
        minHTotal = width + NV_MAX(NVT_OVT_MIN_H_BLANK_420, NVT_OVT_PIXEL_FACTOR_420 * minHBlank);
        // 6.b If the resulting PixelClockRate allows for 4:2:0, assure that the new Hblank requirement is met, or repeat calculation with new MinHtotal:
        if (pixelClockRate >= NVT_OVT_MIN_CLOCK_RATE_420 && hTotal < minHTotal)
        {
            continue;
        }
        break;
    }

    // ** post-processing **
    // 7. Adjust Vtotal, in lines, to achieve (integer) target Vrate:
    vTotal = vTotal * maxVRate / refreshRate;
    
    // 8. Find Vsync leading edge:
    vBlank = vTotal - height;    
    vSyncPosition = (NvU32)NV_UNSIGNED_DIV_CEIL(((NvU64)NVT_OVT_MIN_V_SYNC_LEADING_EDGE * (NvU64)pixelClockRate), ((NvU64)1000000 * (NvU64)hTotal));
    
    // 10. fill in the essential timing info for output
    pT->HVisible    = (NvU16)width;
    pT->HTotal      = (NvU16)hTotal;
    pT->HFrontPorch = (NvU16)(hTotal - width - NVT_OVT_H_SYNC_PIXELS - NVT_OVT_H_BACK_WIDTH); 
    pT->HSyncWidth  = (NvU16)NVT_OVT_H_SYNC_PIXELS;
    pT->VVisible    = (NvU16)height;
    pT->VTotal      = (NvU16)vTotal;
    pT->VSyncWidth  = (NvU16)NVT_OVT_V_SYNC_WIDTH;
    pT->VFrontPorch = (NvU16)(vBlank - vSyncPosition);
    pT->pclk        = (NvU32)(pixelClockRate /*Hz*/ / 1000 + 5) / 10; //convert to 10Khz
    pT->HSyncPol    = NVT_H_SYNC_POSITIVE;
    pT->VSyncPol    = NVT_V_SYNC_POSITIVE;
    pT->HBorder     = pT->VBorder = 0;  // not supported
    pT->interlaced  = 0;                // not supported
    // fill in the extra timing info
    pT->etc.flag    = 0;
    pT->etc.rr      = (NvU16)refreshRate;    
    pT->etc.rrx1k   = (NvU32)axb_div_c_64((NvU64)pT->pclk, (NvU64)10000 * (NvU64)1000, (NvU64)pT->HTotal*(NvU64)pT->VTotal);
    pT->etc.aspect  = calculate_aspect_ratio(pT);
    pT->etc.rep     = 0x1;
    NVT_SNPRINTF((char *)pT->etc.name, 40, "CTA861-OVT:%dx%dx%dHz", width, height, refreshRate);
    pT->etc.name[39] = '\0';

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvBool NvTiming_IsTimingOVT(const NVT_TIMING *pTiming)
{
    // Check from the Timing Type
    if (pTiming->etc.flag & NVT_FLAG_CTA_OVT_TIMING)
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}

POP_SEGMENTS
