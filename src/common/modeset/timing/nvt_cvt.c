//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//  File:       nvt_cvt.c
//
//  Purpose:    calculate CVT/CVT-RB timing
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "nvtiming_pvt.h"

PUSH_SEGMENTS

CONS_SEGMENT(PAGE_CONS)

const NvU32 NVT_MAX_NVU32= (NvU32)(-1);

const NvU32 NVT_CVT_CELL_GRAN        = 8;  // Character cell width.
const NvU32 NVT_CVT_MIN_VSYNCBP      = 11; // in 550us (!!) [1000000:550 = 20000:11]
const NvU32 NVT_CVT_V_PORCH          = 3;  // in pixels
const NvU32 NVT_CVT_C_PRIME          = 30; // value of (C' * 10)
const NvU32 NVT_CVT_M_PRIME_D_20     = 15; // value of (M' / 100)
const NvU32 NVT_CVT_CLOCK_STEP       = 25; // Pclk step, in 10kHz
const NvU32 NVT_CVT_H_SYNC_PER       = 8;  // HSYNC percentage (8%)

const NvU32 NVT_CVT_RB_HBLANK_CELLS  = 20; // 160 fixed hblank for RB
const NvU32 NVT_CVT_RB_HFPORCH_CELLS = 6;  // 48 fixed hfporch for RB
const NvU32 NVT_CVT_RB_HSYNCW_CELLS  = 4;  // 32 fixed hsyncwidth for RB
const NvU32 NVT_CVT_RB_MIN_VBLANK    = 23; // 460 line s (or 460 us?) [1000000:460 = 50000:23]
const NvU32 NVT_CVT_MIN_V_BPORCH     = 6;  // Minimum vertical back porch.


// VESA CVT spec ver1.2:
//
// Page 24 : Table 5-4 : Delta between Original Reduced Blank Timing and Reduced Blanking Timing V2
#define NVT_CVT_RB2_CLOCK_STEP_KHZ         1
#define NVT_CVT_RB2_H_BLANK_PIXELS         80
#define NVT_CVT_RB2_H_SYNC_PIXELS          32
#define NVT_CVT_RB2_MIN_VBLANK_MICROSEC    460
#define NVT_CVT_RB2_MIN_ALT_VBLANK_MICROSEC 300
#define NVT_CVT_RB2_MIN_V_FPORCH           1
#define NVT_CVT_RB2_MIN_V_BPORCH           6
// Page 16 : Table 3-2 : Vertical Sync Duration
#define NVT_CVT_RB2_V_SYNC_WIDTH           8
// Page 22: RB_MIN_VBI = RB_V_FPORCH + V_SYNC_RND + MIN_V_BPORCH
#define NVT_CVT_RB2_MIN_VBI  NVT_CVT_RB2_V_SYNC_WIDTH + NVT_CVT_RB2_MIN_V_FPORCH + NVT_CVT_RB2_MIN_V_BPORCH
// Page 15 : The Horizontal Sync Pulse duration will in all cases be 32 pixel clocks in duration, with the position
// set so that the trailing edge of the Horizontal Sync Pulse is located in the center of the Horizontal
// Blanking period.This implies that for a fixed blank of 80 pixel clocks, the Horizontal Back Porch is
// fixed to(80 / 2) 40 pixel clocks and the Horizontal Front Porch is fixed to(80 - 40 - 32) = 8 clock cycles.
#define NVT_CVT_RB2_H_FPORCH               8
#define NVT_CVT_RB2_H_BPORCH               40

// VESA CVT spec ver2.0:
//
// Page 15 : Table 3-2 Constants
#define NVT_CVT_RB3_CLOCK_STEP_KHZ        1000
#define NVT_CVT_RB3_H_BLANK_PIXELS        NVT_CVT_RB2_H_BLANK_PIXELS
#define NVT_CVT_RB3_H_SYNC_PIXELS         NVT_CVT_RB2_H_SYNC_PIXELS
#define NVT_CVT_RB3_H_FPORCH              NVT_CVT_RB2_H_FPORCH
#define NVT_CVT_RB3_MIN_VBLANK_MICROSEC   NVT_CVT_RB2_MIN_VBLANK_MICROSEC
#define NVT_CVT_RB3_MIN_ALT_VBLANK_MICROSEC    NVT_CVT_RB2_MIN_ALT_VBLANK_MICROSEC
#define NVT_CVT_RB3_V_FIELD_RATE_PPM_ADJ  350
#define NVT_CVT_RB3_V_SYNC_WIDTH          NVT_CVT_RB2_V_SYNC_WIDTH
#define NVT_CVT_RB3_MIN_V_FPORCH          NVT_CVT_RB2_MIN_V_FPORCH
#define NVT_CVT_RB3_MIN_V_BPROCH          NVT_CVT_RB2_MIN_V_BPORCH

#define NVT_CVT_RB3_MIN_VBI               NVT_CVT_RB2_MIN_VBI

CODE_SEGMENT(PAGE_DD_CODE)
static NvU16 getCVTVSync(NvU32 XRes, NvU32 YRes)
{
    // 4:3 modes
    if(XRes * 3 == YRes * 4)
        return 4;

    // 16:9 modes
    //if((XRes * 9 == YRes * 16) ||
    //   (XRes == 848 && YRes == 480) ||      // 53:30   = 1.76666
    //   (XRes == 1064 && YRes == 600) ||     // 133:75  = 1.77333
    //   (XRes == 1360 && YRes == 768) ||     // 85:48   = 1.77083
    //   (XRes == 1704 && YRes == 960) ||     // 71:40   = 1.775
    //   (XRes == 1864 && YRes == 1050) ||    // 832:525 = 1.77523809
    //   (XRes == 2128 && YRes == 1200) ||    // 133:75
    //   (XRes == 2728 && YRes == 1536) ||    // 341:192 = 1.7760416
    //   (XRes == 3408 && YRes == 1920) ||    // 71:40
    //   (XRes == 4264 && YRes == 2400))      // 533:300 = 1.77666
    //    return 5;
    // NOTE: Because 16:9 modes are really a collection of mode of
    //       aspect ratio between 16:9 and 53:30, we will include
    //       all generic mode within this aspect ration range
    if((XRes * 9 <= YRes * 16) && (XRes * 30 >= YRes * 53))
        return 5;

    // 16:10 modes
    if((XRes * 5 == YRes * 8) ||
       (XRes == 1224 && YRes == 768) ||
       (XRes == 2456 && YRes == 1536))
        return 6;

    // Special 1280 modes
    if((XRes == 1280 && YRes == 1024) ||
       (XRes == 1280 && YRes == 768))
        return 7;

    // Failure value, for identification
    return 10;
}


CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcCVT(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NvU32 dwXCells, dwVSyncBP, dwHBlankCells, dwPClk, dwHSyncCells, dwVSyncWidth;

    NvU32 dwHPeriodEstimate_NUM, dwHPeroidEstimate_DEN;
    NvU32 dwIdealDutyCycle_NUM, dwIdealDutyCycle_DEN;

    // parameter check
    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0 )
        return NVT_STATUS_ERR;

    // Check for valid input parameter
    if (width < 300 || height < 200 || rr < 10)
        return NVT_STATUS_ERR;//return NVT_STATUS_ERR_BACKOFF | NVT_STATUS_ERR_OUTOFRANGE;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));

    pT->etc.status = NVT_STATUS_CVT;

    // For 1, 2, 3, 4 in Computation of common parameters
    // H_PIXELS_RND = ROUNDDOWN(H_PIXELS / CELL_GRAN_RND,0) * CELL_GRAN_RND
    if ((width % NVT_CVT_CELL_GRAN)!=0)
    {
        width = (width / NVT_CVT_CELL_GRAN) * NVT_CVT_CELL_GRAN;
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_ALIGNMENT);
    }

    // Calculate timing
    dwXCells = width / NVT_CVT_CELL_GRAN; // Convert to number of cells
    dwVSyncWidth = getCVTVSync(dwXCells * NVT_CVT_CELL_GRAN, height);

    dwHPeriodEstimate_NUM = 20000 - NVT_CVT_MIN_VSYNCBP * rr;
    dwHPeroidEstimate_DEN = rr * (height + NVT_CVT_V_PORCH);

    dwVSyncBP = NVT_CVT_MIN_VSYNCBP * dwHPeroidEstimate_DEN / dwHPeriodEstimate_NUM +1;
    if(dwVSyncBP < dwVSyncWidth + NVT_CVT_MIN_V_BPORCH)
        dwVSyncBP = dwVSyncWidth + NVT_CVT_MIN_V_BPORCH;

    // Check for overflow
    //DBG_ASSERT(NVT_MAX_NVU32 / NVT_CVT_C_PRIME > dwHPeroidEstimate_DEN);

    dwIdealDutyCycle_DEN = dwHPeroidEstimate_DEN;
    dwIdealDutyCycle_NUM = NVT_CVT_C_PRIME * dwHPeroidEstimate_DEN - NVT_CVT_M_PRIME_D_20 * dwHPeriodEstimate_NUM;

    if (dwIdealDutyCycle_NUM < dwIdealDutyCycle_DEN * 20 || 
        (NVT_CVT_C_PRIME * dwHPeroidEstimate_DEN < NVT_CVT_M_PRIME_D_20 * dwHPeriodEstimate_NUM))
    {
        dwIdealDutyCycle_NUM=20;
        dwIdealDutyCycle_DEN=1;
    }

    // Check for overflow
    if (NVT_MAX_NVU32 / dwXCells <= dwIdealDutyCycle_NUM)
    {
        dwIdealDutyCycle_NUM /= 10;
        dwIdealDutyCycle_DEN /= 10;
    }

    dwHBlankCells = ((dwXCells * dwIdealDutyCycle_NUM)/(200*dwIdealDutyCycle_DEN - 2*dwIdealDutyCycle_NUM))*2;

    // Check for overflow
    //DBG_ASSERT(MAX_NVU32 / dwHPeroidEstimate_DEN > (dwXCells + dwHBlankCells)*CVT_CELL_GRAN);
    dwPClk = ((dwXCells + dwHBlankCells) * NVT_CVT_CELL_GRAN * dwHPeroidEstimate_DEN * 2 / dwHPeriodEstimate_NUM / NVT_CVT_CLOCK_STEP) * NVT_CVT_CLOCK_STEP;

    dwHSyncCells = (dwXCells + dwHBlankCells) * NVT_CVT_H_SYNC_PER / 100;


    pT->HVisible = (NvU16)(dwXCells * NVT_CVT_CELL_GRAN);
    pT->VVisible = (NvU16)height;

    pT->HTotal = (NvU16)((dwXCells + dwHBlankCells) * NVT_CVT_CELL_GRAN);
    pT->HFrontPorch = (NvU16)((dwHBlankCells/2 - dwHSyncCells) * NVT_CVT_CELL_GRAN);
    pT->HSyncWidth = (NvU16)(dwHSyncCells * NVT_CVT_CELL_GRAN);

    pT->VTotal = (NvU16)(height + dwVSyncBP + NVT_CVT_V_PORCH);
    pT->VFrontPorch = (NvU16)(NVT_CVT_V_PORCH);
    pT->VSyncWidth = getCVTVSync(dwXCells * NVT_CVT_CELL_GRAN, height);

    pT->pclk     = dwPClk;
    pT->pclk1khz = (dwPClk << 3) + (dwPClk << 1); // *10

    pT->HSyncPol = NVT_H_SYNC_NEGATIVE;
    pT->VSyncPol = NVT_V_SYNC_POSITIVE;

    // Clear unused fields
    pT->HBorder = pT->VBorder = 0;
    pT->interlaced = NVT_PROGRESSIVE;

    pT->etc.flag = 0;
    pT->etc.rr = (NvU16)rr;
    pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
    pT->etc.aspect = 0;
    pT->etc.rep = 0x1;
    NVT_SNPRINTF((char *)pT->etc.name, 40, "CVT:%dx%dx%dHz",width, height, rr);
    pT->etc.name[39] = '\0';

    // interlaced adjustment
    if ((flag & NVT_PVT_INTERLACED_MASK) != 0)
    {
        if ((pT->VTotal & 0x1) != 0)
            pT->interlaced = NVT_INTERLACED_EXTRA_VBLANK_ON_FIELD2;
        else
            pT->interlaced = NVT_INTERLACED_NO_EXTRA_VBLANK_ON_FIELD2;

        pT->pclk   >>= 1;
        pT->VTotal >>= 1;
        pT->VVisible = (pT->VVisible + 1) / 2;
    }
    pT->etc.rgb444.bpc.bpc8 = 1;

    return NVT_STATUS_SUCCESS;
}

// CVT-RB timing calculation
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcCVT_RB(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NvU32 dwXCells, dwPClk, dwVBILines, dwVSyncWidth;

    // parameter check
    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0 )
        return NVT_STATUS_ERR;

    // Check for valid input parameter
    if (width < 300 || height < 200 || rr < 10)
        return NVT_STATUS_ERR;//NVT_STATUS_ERR_BACKOFF | NVT_STATUS_ERR_OUTOFRANGE;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
    pT->etc.status = NVT_STATUS_CVT_RB;

    // H_PIXELS_RND = ROUNDDOWN(H_PIXELS / CELL_GRAN_RND,0) * CELL_GRAN_RND
    if ((width % NVT_CVT_CELL_GRAN)!=0)
    {
        width = (width / NVT_CVT_CELL_GRAN) * NVT_CVT_CELL_GRAN;
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_ALIGNMENT);
    }

    // Calculate timing
    dwXCells = width / NVT_CVT_CELL_GRAN; // Convert to number of cells
    dwVSyncWidth = getCVTVSync(dwXCells * NVT_CVT_CELL_GRAN, height);

    dwVBILines = (NVT_CVT_RB_MIN_VBLANK * height * rr) / (50000 - NVT_CVT_RB_MIN_VBLANK * rr) + 1;

    if(dwVBILines < NVT_CVT_V_PORCH + dwVSyncWidth + NVT_CVT_MIN_V_BPORCH)
        dwVBILines = NVT_CVT_V_PORCH + dwVSyncWidth + NVT_CVT_MIN_V_BPORCH;

    dwPClk = rr * (height + dwVBILines) * (dwXCells + NVT_CVT_RB_HBLANK_CELLS) / (10000 / NVT_CVT_CELL_GRAN) / NVT_CVT_CLOCK_STEP;
    dwPClk *= NVT_CVT_CLOCK_STEP;

    pT->HVisible = (NvU16)(dwXCells * NVT_CVT_CELL_GRAN);
    pT->VVisible = (NvU16)height;

    pT->HTotal = (NvU16)((dwXCells + NVT_CVT_RB_HBLANK_CELLS) * NVT_CVT_CELL_GRAN);
    pT->HFrontPorch = (NvU16)(NVT_CVT_RB_HFPORCH_CELLS * NVT_CVT_CELL_GRAN);
    pT->HSyncWidth = (NvU16)(NVT_CVT_RB_HSYNCW_CELLS * NVT_CVT_CELL_GRAN);

    pT->VTotal = (NvU16)(height + dwVBILines);
    pT->VFrontPorch = (NvU16)(NVT_CVT_V_PORCH);
    pT->VSyncWidth = (NvU16)dwVSyncWidth;

    pT->pclk = dwPClk;

    pT->HSyncPol = NVT_H_SYNC_POSITIVE;
    pT->VSyncPol = NVT_V_SYNC_NEGATIVE;

    // Clear unused fields
    pT->HBorder = pT->VBorder = 0;
    pT->interlaced = 0;

    // fill in the extra timing info
    pT->etc.flag = 0;
    pT->etc.rr = (NvU16)rr;
    pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
    pT->etc.aspect = 0;
    pT->etc.rep = 0x1;
    NVT_SNPRINTF((char *)pT->etc.name, 40, "CVT-RB:%dx%dx%dHz",width, height, rr);
    pT->etc.name[39] = '\0';

    // interlaced adjustment
    if ((flag & NVT_PVT_INTERLACED_MASK) != 0)
    {
        if ((pT->VTotal & 0x1) != 0)
            pT->interlaced = NVT_INTERLACED_EXTRA_VBLANK_ON_FIELD2;
        else
            pT->interlaced = NVT_INTERLACED_NO_EXTRA_VBLANK_ON_FIELD2;

        pT->pclk   >>= 1;
        pT->VTotal >>= 1;
        pT->VVisible = (pT->VVisible + 1) / 2;
    }

    return NVT_STATUS_SUCCESS;
}

// CVT-RB2 timing calculation
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcCVT_RB2(NvU32 width, NvU32 height, NvU32 rr, NvBool is1000div1001, NvBool isAltMiniVblankTiming, NVT_TIMING *pT)
{
    NvU32 vbi, act_vbi_lines, total_v_lines, total_pixels, act_pixel_freq_khz;

    // parameter check
    if (pT == NULL || width == 0 || height == 0 || rr == 0)
        return NVT_STATUS_ERR;

    // Check for valid input parameter
    if (width < 300 || height < 200 || rr < 10)
        return NVT_STATUS_ERR;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
    pT->etc.status = NVT_STATUS_CVT_RB_2;

    // CVT spec1.2 - page 21 : 5.4 Computation of Reduced Blanking Timing Parameters
    // 8. Estimate the Horizontal Period (kHz):
    //    H_PERIOD_EST = ((1000000 / (V_FIELD_RATE_RQD)) - RB_MIN_V_BLANK) / (V_LINES_RND +
    //                    TOP_MARGIN + BOT_MARGIN)
    // h_period_est = (1000000 / rr - NVT_CVT_RB2_MIN_VBLANK) / height;

    // 9. Determine the number of lines in the vertical blanking interval :
    //    VBI_LINES = ROUNDDOWN(RB_MIN_V_BLANK / H_PERIOD_EST, 0) + 1
    // vbi = NVT_CVT_RB2_MIN_VBLANK / h_period_est + 1;

    // combining step 8, 9,
    if (isAltMiniVblankTiming)
    {
       // CVT spec2.1 - page 11 6. VBlank Period
       // an alternate minimum VBlank duration of 300 us may be used under conditions defined by the interface specification on which the timing is used
        vbi = height * NVT_CVT_RB2_MIN_ALT_VBLANK_MICROSEC * rr / (1000000 - NVT_CVT_RB2_MIN_ALT_VBLANK_MICROSEC * rr) + 1;
    }
    else
    {
        vbi = height * NVT_CVT_RB2_MIN_VBLANK_MICROSEC * rr / (1000000 - NVT_CVT_RB2_MIN_VBLANK_MICROSEC * rr) + 1;
    }

    // 10. Check Vertical Blanking is Sufficient :
    // RB_MIN_VBI = RB_V_FPORCH + V_SYNC_RND + MIN_V_BPORCH
    //   ACT_VBI_LINES = IF(VBI_LINES < RB_MIN_VBI, RB_MIN_VBI, VBI_LINES)
    act_vbi_lines = MAX(vbi, NVT_CVT_RB2_MIN_VBI);

    // 11. Find total number of vertical lines :
    // TOTAL_V_LINES = ACT_VBI_LINES + V_LINES_RND + TOP_MARGIN + BOT_MARGIN
    // + INTERLACE
    total_v_lines = act_vbi_lines + height; //+0.5 if interlaced

    // 12. Find total number of pixel clocks per line :
    // TOTAL_PIXELS = RB_H_BLANK + TOTAL_ACTIVE_PIXELS
    total_pixels = NVT_CVT_RB2_H_BLANK_PIXELS + width;

    // sanity check just in case of bad edid where the timing value could exceed the limit of NVT_TIMING structure which unfortunately is defined in NvU16
    if (total_pixels > (NvU16)-1 || total_v_lines > (NvU16)-1)
        return NVT_STATUS_INVALID_PARAMETER;

    // 13. Calculate Pixel Clock Frequency to nearest CLOCK_STEP MHz :
    // ACT_PIXEL_FREQ = CLOCK_STEP * ROUNDDOWN((V_FIELD_RATE_RQD * TOTAL_V_LINES *
    // TOTAL_PIXELS / 1000000 * REFRESH_MULTIPLIER) / CLOCK_STEP, 0)
    if (is1000div1001)
        act_pixel_freq_khz = NVT_CVT_RB2_CLOCK_STEP_KHZ * (rr * total_v_lines * total_pixels / 1001 / NVT_CVT_RB2_CLOCK_STEP_KHZ);
    else
        act_pixel_freq_khz = NVT_CVT_RB2_CLOCK_STEP_KHZ * (rr * total_v_lines * total_pixels / 1000 / NVT_CVT_RB2_CLOCK_STEP_KHZ);

    // 14. Find actual Horizontal Frequency(kHz) :
    // ACT_H_FREQ = 1000 * ACT_PIXEL_FREQ / TOTAL_PIXELS
    // 15. Find Actual Field Rate(Hz) :
    // ACT_FIELD_RATE = 1000 * ACT_H_FREQ / TOTAL_V_LINES
    // 16. Find actual Vertical Refresh Rate(Hz) :
    // ACT_FRAME_RATE = IF(INT_RQD ? = "y", ACT_FIELD_RATE / 2, ACT_FIELD_RATE

    // fill in the essential timing info for output
    pT->HVisible    = (NvU16)width;
    pT->HTotal      = (NvU16)(total_pixels);
    pT->HFrontPorch = NVT_CVT_RB2_H_FPORCH;
    pT->HSyncWidth  = NVT_CVT_RB2_H_SYNC_PIXELS;
    pT->VVisible    = (NvU16)height;
    pT->VTotal      = (NvU16)total_v_lines;
    pT->VSyncWidth  = NVT_CVT_RB2_V_SYNC_WIDTH;
    pT->VFrontPorch = (NvU16)(act_vbi_lines - NVT_CVT_RB2_V_SYNC_WIDTH - NVT_CVT_RB2_MIN_V_BPORCH);
    pT->pclk        = (act_pixel_freq_khz + 5) / 10; //convert to 10Khz
    pT->pclk1khz    = act_pixel_freq_khz;
    pT->HSyncPol    = NVT_H_SYNC_POSITIVE;
    pT->VSyncPol    = NVT_V_SYNC_NEGATIVE;
    pT->HBorder     = pT->VBorder = 0;  // not supported
    pT->interlaced  = 0;                // not supported yet

    // fill in the extra timing info
    pT->etc.flag   = 0;
    pT->etc.rr     = (NvU16)rr;
    pT->etc.rrx1k  = (NvU32)axb_div_c_64((NvU64)act_pixel_freq_khz, (NvU64)1000 * (NvU64)1000, (NvU64)pT->HTotal*(NvU64)pT->VTotal);
    pT->etc.aspect = 0;
    pT->etc.rep    = 0x1;
    NVT_SNPRINTF((char *)pT->etc.name, 40, "CVT-RB2:%dx%dx%dHz", width, height, rr);
    pT->etc.name[39] = '\0';

    return NVT_STATUS_SUCCESS;
}


// CVT-RB3 timing calculation
// This is intended to work in conjunction with VESA Adaptive-Sync operation (or other similar VRR methodology)
CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcCVT_RB3(NvU32  width,
                                NvU32  height,
                                NvU32  rr,
                                NvU32  deltaHBlank,
                                NvU32  vBlankMicroSec,
                                NvBool isAltMiniVblankTiming,
                                NvBool isEarlyVSync,
                                NVT_TIMING *pT)
{
    NvU32 vbi, act_v_blank_time, act_v_blank_lines, v_back_porch_est, v_back_porch, total_v_lines, total_pixels, adj_rr_x1M, act_pixel_freq_khz;
    NvU64 act_pixel_freq_hz = 0xFFFFFFFFFFFFFFFFULL;

    // parameter check
    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0)
        return NVT_STATUS_ERR;

    // Check for valid input parameter
    if ((height % 8 != 0) || (deltaHBlank % 8 != 0) || deltaHBlank > 120)
        return NVT_STATUS_INVALID_PARAMETER;

    if ((isAltMiniVblankTiming && vBlankMicroSec > 140) || (!isAltMiniVblankTiming && vBlankMicroSec > 245))
        return NVT_STATUS_INVALID_PARAMETER;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));
    pT->etc.status = NVT_STATUS_CVT_RB_3;

    // 1 Calculate the required field refresh rate (Hz):
    // V_FIELD_RATE_RQD = I_IP_FREQ_RQD * (1 + C_V_FIELD_RATE_PPM_ADJ / 1000000)

    // Parameters mapping:
    // - V_FIELD_RATE_RQD == "adj_rr_x1M"
    // - I_IP_FREQ_RQD == "rr"
    // - C_V_FIELD_RATE_PPM_ADJ == "NVT_CVT_RB3_V_FIELD_RATE_PPM_ADJ"
    adj_rr_x1M = rr * (1000000 + NVT_CVT_RB3_V_FIELD_RATE_PPM_ADJ);

    // 2 Round the desired number of horizontal pixels down to the nearest character cell boundary:
    // TOTAL_ACTIVE_PIXELS = ROUNDDOWN(I_H_PIXELS / C_CELL_GRAN_RND, 0) * C_CELL_GRAN_RND

    // Parameters mapping:
    // - TOTAL_ACTIVE_PIXELS and I_H_PIXELS == "width"
    // - C_CELL_GRAN_RND == "NVT_CVT_CELL_GRAN"
    if ((width % NVT_CVT_CELL_GRAN) != 0)
    {
        // ROUNDDOWN
        width = (width / NVT_CVT_CELL_GRAN) * NVT_CVT_CELL_GRAN;
        NVT_SET_TIMING_STATUS_MISMATCH(pT->etc.status, NVT_STATUS_TIMING_MISMATCH_ALIGNMENT);
    }

    // 3 Round the number of vertical lines down to the nearest integer:
    // V_LINES_RND = ROUNDDOWN(I_V_LINES, 0)

    // Parameters mapping:
    // - V_LINES_RND == "height"

    // 4 Calculate the minimum allowed VBlank duration:
    // MIN_V_BLANK = IF(I_ALTERNATE_VBLANK_MIN?="N", C_RB_MIN_V_BLANK, C_RB_ALT_MIN_V_BLANK)

    // 5 Calculate the estimated Horizontal Period (kHz):
    // H_PERIOD_EST = ((1,000,000 / (V_FIELD_RATE_RQD)) – MIN_V_BLANK) / V_LINES_RND

    // Parameters mapping:
    // - H_PERIOD_EST     == "h_period_est"
    // - C_RB_MIN_V_BLANK == "NVT_CVT_RB3_MIN_VBLANK_MICROSEC" == 460
    // - C_RB_ALT_MIN_V_BLANK == "NVT_CVT_RB3_MIN_ALT_VBLANK_MICROSEC" == 300
    // h_period_est = ((1000000000000 / adj_rr_x1M) - MIN_V_BLANK) / height

    // 6 Calculate the total VBlank time:
    // ACT_V_BLANK_TIME = IF(I_VBLANK < MIN_V_BLANK, MIN_V_BLANK, I_VBLANK)

    // Parameters mapping:
    // - ACT_V_BLANK_TIME == "act_v_blank_time"
    // - I_VBLANK         == "vBlankMicroSec"
    if (isAltMiniVblankTiming)
        act_v_blank_time = MAX(vBlankMicroSec + NVT_CVT_RB3_MIN_ALT_VBLANK_MICROSEC, NVT_CVT_RB3_MIN_ALT_VBLANK_MICROSEC);
    else
        act_v_blank_time = MAX(vBlankMicroSec + NVT_CVT_RB3_MIN_VBLANK_MICROSEC, NVT_CVT_RB3_MIN_VBLANK_MICROSEC);

    // 7 Calculate the number of idealized lines in the VBlank interval:
    // VBI_LINES = ROUNDUP(ACT_V_BLANK_TIME / H_PERIOD_EST, 0)

    // Parameters mapping:
    // - VBI_LINES == "vbi"
    // below formula are combining step 4, 5, 6 together. i.e. both numerator and denominator multiple by height and addj_rr_x1M.
    vbi = (NvU32)(((NvU64)height * (NvU64)act_v_blank_time * (NvU64)adj_rr_x1M) / ((NvU64)1000000000000 - (NvU64)act_v_blank_time * (NvU64)adj_rr_x1M));
    // ROUNDUP
    if (((NvU64)height * (NvU64)act_v_blank_time * (NvU64)adj_rr_x1M) % ((NvU64)1000000000000 - (NvU64)act_v_blank_time * (NvU64)adj_rr_x1M) !=0)
        vbi += 1;

    // 8 Determine whether idealized VBlank is sufficient and calculate the actual number of lines in the VBlank period:
    // RB_MIN_VBI = C_RB_MIN_V_FPORCH + C_V_SYNC_RND + C_MIN_V_BPORCH
    // V_BLANK = IF(VBI_LINES < RB_MIN_VBI, RB_MIN_VBI, VBI_LINES)

    // Parameters mapping:
    // - C_RB_MIN_V_FPORCH == 1
    // - C_V_SYNC_RND      == 8
    // - C_MIN_V_BPORCH    == 6
    // - V_BLANK           == "act_v_blank_lines"
    // NVT_CVT_RB3_MIN_VBI ==  1 + 8 + 6 = 15
    act_v_blank_lines = MAX(vbi, NVT_CVT_RB3_MIN_VBI);

    // 9 Calculate the total number of vertical lines:
    // TOTAL_V_LINES = V_BLANK + V_LINES_RND
    total_v_lines = act_v_blank_lines + height;

    // 10 Calculate the estimated vertical back porch, determine whether the remaining vertical front porch is greater than the
    // minimum vertical front porch (C_RB_MIN_V_FPORCH), and if not, reduce the estimated vertical back porch to reserve VBlank lines for the vertical front porch:
    // V_BACK_PORCH_EST = IF(AND(I_RED_BLANK_VER=3,I_EARLY_VSYNC_RQD?="Y"), ROUNDDOWN(VBI_LINES / 2, 0), C_MIN_V_BPORCH)
    // V_BACK_PORCH = IF(AND(I_RED_BLANK_VER=3, I_EARLY_VSYNC_RQD?="Y"),
    //                   IF(VBLANK – V_BACK_PORCH_EST – C_V_SYNC_RND < C_RB_MIN_V_FPORCH, V_BLANK – C_V_SYNC_RND – C_RB_MIN_V_FPORCH, V_BACK_PORCH_EST),
    //                   V_BACK_PORCH_EST)

    // Parameters mapping:
    // - V_BACK_PORCH_EST  == "(VBI_LINES / 2)
    // - V_BACK_PORCH      == "v_back_porch"
    // - I_RED_BLANK_VER   == "3" this is for RB3 function so the value is 3
    // - I_EARLY_VSYNC_RQD == "isEarlyVSync"
    // - C_V_SYNC_RND      == NVT_CVT_RB3_V_SYNC_WIDTH == 8
    // - C_RB_MIN_V_FPORCH == NVT_CVT_RB3_MIN_V_FPORCH == 1
    // - C_MIN_V_BPORCH    == NVT_CVT_RB3_MIN_V_BPROCH == 6
    if (isEarlyVSync)
    {
        v_back_porch_est = vbi / 2;
        if ((act_v_blank_lines - v_back_porch_est - NVT_CVT_RB3_V_SYNC_WIDTH) < NVT_CVT_RB3_MIN_V_FPORCH)
            v_back_porch = act_v_blank_lines - NVT_CVT_RB3_V_SYNC_WIDTH - NVT_CVT_RB3_MIN_V_FPORCH;
        else
            v_back_porch = v_back_porch_est;
    }
    else
        v_back_porch = NVT_CVT_RB3_MIN_V_BPROCH;

    // 11 Calculate the vertical front porch:
    // V_FRONT_PORCH = V_BLANK – V_BACK_PORCH – C_V_SYNC_RND
    // pT->VFrontPorch  = (NvU16)(act_v_blank_lines - NVT_CVT_RB3_V_SYNC_WIDTH - v_back_porch);

    // 12 Calculate the total number of pixels per line:
    // TOTAL_PIXELS = TOTAL_ACTIVE_PIXELS + C_RB_MIN_H_BLANK + IF(I_RED_BLANK_VER=3, I_ADDITIONAL_HBLANK, 0)

    // Parameters mapping:
    // - C_RB_MIN_H_BLANK    == NVT_CVT_RB3_H_BLANK_PIXELS == 80
    // - I_ADDITIONAL_HBLANK == deltaHBlank scopes are defined in the TypeX in displayId2.1a
    // 80 <= HBlank <= 200 is a valid scope
    total_pixels = width + NVT_CVT_RB3_H_BLANK_PIXELS + deltaHBlank;

    // 13 Calculate the horizontal back porch:
    // H_BACK_PORCH = C_RB_MIN_H_BLANK + IF(I_RED_BLANK_VER=3, I_ADDITIONAL_HBLANK, 0) – C_H_FRONT_PORCH – C_RB_H_SYNC
    // NVT_TIMING did not need to store H_BACK_PORCH

    // sanity check just in case of bad edid where the timing value could exceed the limit of NVT_TIMING structure which unfortunately is defined in NvU16
    if (total_pixels > (NvU16)-1 || total_v_lines > (NvU16)-1)
        return NVT_STATUS_INVALID_PARAMETER;

    // 14 Calculate the pixel clock frequency to the nearest C_CLOCK_STEP (MHz):
    // ACT_PIXEL_FREQ = C_CLOCK_STEP * ROUNDUP((V_FIELD_RATE_RQD * TOTAL_V_LINES * TOTAL_PIXELS / 1000000 * 1) / C_CLOCK_STEP, 0))

    // Parameters mapping:
    // - ACT_PIXEL_FREQ == "act_pixel_freq_hz"
    // - C_CLOCK_STEP   == "NVT_CVT_RB3_CLOCK_STEP_KHZ" == 1000
    act_pixel_freq_hz = ((NvU64)adj_rr_x1M * (NvU64)total_v_lines * (NvU64)total_pixels / (NvU64)1000000);

    // Here we need to divide extra 1000 because adj_rr_x1M extends to Million, i.e 1Mhz / 1000 = 1kHz
    act_pixel_freq_khz = (NvU32)(act_pixel_freq_hz / NVT_CVT_RB3_CLOCK_STEP_KHZ);

    // kHz ROUNDUP
    if ((act_pixel_freq_hz  % 1000) != 0)
        act_pixel_freq_khz += 1;

    pT->HVisible     = (NvU16)width;
    pT->HTotal       = (NvU16)total_pixels;
    pT->HFrontPorch  = NVT_CVT_RB3_H_FPORCH;
    pT->HSyncWidth   = NVT_CVT_RB3_H_SYNC_PIXELS;
    pT->VVisible     = (NvU16)height;
    pT->VTotal       = (NvU16)total_v_lines;
    pT->VSyncWidth   = NVT_CVT_RB3_V_SYNC_WIDTH;
    pT->VFrontPorch  = (NvU16)(act_v_blank_lines - NVT_CVT_RB3_V_SYNC_WIDTH - v_back_porch);
    pT->pclk         = ((NvU32)act_pixel_freq_khz + 5) / 10; //convert to 10Khz
    pT->pclk1khz     = act_pixel_freq_khz;
    pT->HSyncPol     = NVT_H_SYNC_POSITIVE;
    pT->VSyncPol     = NVT_V_SYNC_NEGATIVE;
    pT->HBorder      = pT->VBorder = 0;  // not supported
    pT->interlaced   = 0;                // not supported yet

    // fill in the extra timing info
    pT->etc.flag     = 0;
    pT->etc.rr       = (NvU16)rr;
    pT->etc.rrx1k    = (NvU32)axb_div_c_64((NvU64)act_pixel_freq_khz, (NvU64)1000 * (NvU64)1000, (NvU64)pT->HTotal*(NvU64)pT->VTotal);
    pT->etc.aspect   = 0;
    pT->etc.rep      = 0x1;
    NVT_SNPRINTF((char *)pT->etc.name, 40, "CVT-RB3:%dx%dx%dHz", width, height, rr);
    pT->etc.name[39] = '\0';

    return NVT_STATUS_SUCCESS;
}

CODE_SEGMENT(PAGE_DD_CODE)
NvBool NvTiming_IsTimingCVTRB(const NVT_TIMING *pTiming)
{
    // Check from the Timing Type
    NvU32 reducedType = 0;
    NvU32 hblank      = 0;
    reducedType = NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status);

    if (reducedType == NVT_TYPE_CVT_RB || reducedType == NVT_TYPE_CVT_RB_2 || reducedType == NVT_TYPE_CVT_RB_3)
    {
        return NV_TRUE;
    }

    hblank = pTiming->HTotal - pTiming->HVisible;

    // Manually Check for RB 1 and 2
    // RB1 - HBlank = 160, and HSync = 32, HFrontPorch = 48, HBackPorch = 80
    if ((hblank == 160) && (pTiming->HSyncWidth == 32) && (pTiming->HFrontPorch == 48))
    {
        return NV_TRUE;
    }

    // RB2 - HBlank = 80, HSync = 32, HFrontPorch = 8, HBackPorch = 40
    if ((hblank == 80) && (pTiming->HSyncWidth == 32) && (pTiming->HFrontPorch == 8))
    {
        return NV_TRUE;
    }

    // RB3 - HBlank is any integer multiple of 8 [80-200], HSync = 32, HFrontPorch = 8
    if (((hblank > 80) && (hblank <= 200) && (hblank % 8 == 0)) && (pTiming->HSyncWidth == 32) && (pTiming->HFrontPorch == 8))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

POP_SEGMENTS
