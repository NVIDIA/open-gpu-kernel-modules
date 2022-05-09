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
//  File:       nvt_gtf.c
//
//  Purpose:    calculate gtf timing
//
//*****************************************************************************

#include "nvBinSegment.h"
#include "nvmisc.h"

#include "nvtiming_pvt.h"

// calculate GTF timing

PUSH_SEGMENTS

CONS_SEGMENT(PAGE_CONS)

const NvU32 NVT_GTF_CELL_GRAN=8;
const NvU32 NVT_GTF_MIN_VSYNCBP=11;     // in 550us (!!) [1000000:550 = 20000:11]
const NvU32 NVT_GTF_MIN_VPORCH=1;

const NvU32 NVT_GTF_C_PRIME=30;         // (gtf_C-gtf_J)*gtf_K/256+gtf_J;
const NvU32 NVT_GTF_M_PRIME=300;        // NVT_GTFK/256*gtf_M;
const NvU32 NVT_GTF_VSYNC_RQD=3;
const NvU32 NVT_GTF_HSYNC_PERCENTAGE=8; // 8% HSync for GTF

CODE_SEGMENT(PAGE_DD_CODE)
NVT_STATUS NvTiming_CalcGTF(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT)
{
    NvU32 dwXCells, dwVSyncBP, dwVTotal, dwIdN, dwIdD, dwHBlank, dwHTCells, dwHSync, dwHFrontPorch, dwRefreshRate;

    // parameter check 
    if (pT == NULL)
        return NVT_STATUS_ERR;

    if (width == 0 || height == 0 || rr == 0 )
        return NVT_STATUS_ERR;
    
    dwRefreshRate = rr;
    dwXCells = a_div_b(width, NVT_GTF_CELL_GRAN);

    if(dwRefreshRate * NVT_GTF_MIN_VSYNCBP >= 20000)
        return NVT_STATUS_ERR;//NVT_STATUS_ERR_OUTOFRANGE; // H period estimate less than 0

    dwVSyncBP = a_div_b((height + NVT_GTF_MIN_VPORCH) * NVT_GTF_MIN_VSYNCBP * dwRefreshRate, 
                        (20000 - NVT_GTF_MIN_VSYNCBP * dwRefreshRate));
    dwVTotal = dwVSyncBP + height + NVT_GTF_MIN_VPORCH;

    // Calculate the numerator and denominator of Ideal Duty Cycle
    // NOTE: here dwIdN/dwIdN = IdealDutyCycle/GTF_C_Prime
    dwIdD = dwVTotal * dwRefreshRate;
    
    if(dwIdD <= NVT_GTF_M_PRIME * 1000 / NVT_GTF_C_PRIME)
        return NVT_STATUS_ERR;//NVT_STATUS_ERR_OUTOFRANGE; // Ideal duty cycle less than 0

    dwIdN = dwIdD - NVT_GTF_M_PRIME * 1000 / NVT_GTF_C_PRIME;
    
    // A proper way to calculate dwXCells*dwIdN/(100*dwIdD/GTF_C_PRIME-dwIdN)
    dwHBlank = axb_div_c(dwIdN*3, dwXCells, 2*(300*dwIdD/NVT_GTF_C_PRIME - dwIdN*3));
    dwHBlank = ( dwHBlank ) * 2 * NVT_GTF_CELL_GRAN;
    dwHTCells = dwXCells + dwHBlank / NVT_GTF_CELL_GRAN;
    dwHSync = a_div_b(dwHTCells * NVT_GTF_HSYNC_PERCENTAGE, 100) * NVT_GTF_CELL_GRAN;
    if((dwHSync == 0) || (dwHSync*2 > dwHBlank))
        return NVT_STATUS_ERR;//NVT_STATUS_ERR_OUTOFRANGE; // HSync too small or too big.

    dwHFrontPorch = dwHBlank/2-dwHSync;

    NVMISC_MEMSET(pT, 0, sizeof(NVT_TIMING));

    pT->HVisible = (NvU16)(dwXCells*NVT_GTF_CELL_GRAN);
    pT->VVisible = (NvU16)height;

    pT->HTotal = (NvU16)(dwHTCells*NVT_GTF_CELL_GRAN);
    pT->HFrontPorch = (NvU16)dwHFrontPorch;
    pT->HSyncWidth = (NvU16)dwHSync;

    pT->VTotal = (NvU16)dwVTotal;
    pT->VFrontPorch = (NvU16)NVT_GTF_MIN_VPORCH;
    pT->VSyncWidth = (NvU16)NVT_GTF_VSYNC_RQD;

    // A proper way to calculate fixed HTotal*VTotal*Rr/10000
    pT->pclk = axb_div_c(dwHTCells*dwVTotal, dwRefreshRate, 10000/NVT_GTF_CELL_GRAN);

    pT->HSyncPol = NVT_H_SYNC_NEGATIVE;
    pT->VSyncPol = NVT_V_SYNC_POSITIVE;
    pT->interlaced = 0;

    // fill in the extra timing info
    pT->etc.flag = 0;
    pT->etc.rr = (NvU16)rr;
    pT->etc.rrx1k = axb_div_c((NvU32)pT->pclk, (NvU32)10000*(NvU32)1000, (NvU32)pT->HTotal*(NvU32)pT->VTotal);
    pT->etc.aspect = 0;
    pT->etc.rep = 0x1; 
    pT->etc.status = NVT_STATUS_GTF;   
    NVT_SNPRINTF((char *)pT->etc.name, 40, "GTF:%dx%dx%dHz",width, height, rr);
    pT->etc.name[39] = '\0';
    pT->etc.rgb444.bpc.bpc8 = 1;
    
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

POP_SEGMENTS
