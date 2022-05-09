/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 */

#include "nv_mode_timings_utils.h"

/*
 * Check if this NVT_TIMING is the 640x480@60Hz Industry standard
 * mode; but due to the lack of precision of the pclk field in the
 * NVT_TIMING data structure, is not quite correct: pclk should be
 * 2517.5, and rrx1k should be 59940.  See bug 263631.
 *
 * Note that we check if rrx1k is either 60000 or 59940 because we may
 * use this function immediately after receiving the NVT_TIMINGs from
 * the EDID and patch rrx1k, or we may use this function later from
 * NVT_TIMINGtoNvModeTimings(), at which point we'll have already
 * patched rrx1k.
 */

NvBool IsEdid640x480_60_NVT_TIMING(const NVT_TIMING *pTiming)
{
    return ((pTiming->pclk        == 2518)                &&
            (pTiming->HVisible    == 640)                 &&
            (pTiming->VVisible    == 480)                 &&
            (pTiming->HTotal      == 800)                 &&
            (pTiming->HFrontPorch == 16)                  &&
            (pTiming->HSyncWidth  == 96)                  &&
            (pTiming->VTotal      == 525)                 &&
            (pTiming->VFrontPorch == 10)                  &&
            (pTiming->VSyncWidth  == 2)                   &&
            (pTiming->HBorder     == 0)                   &&
            (pTiming->VBorder     == 0)                   &&
            (pTiming->HSyncPol    == NVT_H_SYNC_NEGATIVE) &&
            (pTiming->VSyncPol    == NVT_V_SYNC_NEGATIVE) &&
            (pTiming->interlaced  == 0)                   &&
            ((pTiming->etc.flag &
              NVT_FLAG_NV_DOUBLE_SCAN_TIMING) == 0)       &&
            ((pTiming->etc.rrx1k  == 60000)               ||
             (pTiming->etc.rrx1k  == 59940)));
}

/*
 * Convert from NVT_TIMING to NvModeTimings; this is a safe operation
 * to perform because NvModeTimings has higher precision (pixelclockHz
 * in Hz, and vertical values doubled for interlaced) than NVT_TIMING
 */

void NVT_TIMINGtoNvModeTimings(const NVT_TIMING *pTiming,
                               NvModeTimingsPtr pModeTimings)
{
    char *bytePtr = (char *)pModeTimings;
    size_t i;

    for (i = 0; i < sizeof(*pModeTimings); i++) {
        bytePtr[i] = 0;
    }

    pModeTimings->RRx1k = pTiming->etc.rrx1k;

    /* pTiming->pclk is in 10*kHz; pModeTimings->pixelClockHz is in Hz */

    pModeTimings->pixelClockHz = KHzToHz(pTiming->pclk) * 10;

    pModeTimings->hVisible = pTiming->HVisible;
    pModeTimings->hSyncStart = pTiming->HFrontPorch + pTiming->HVisible;
    pModeTimings->hSyncEnd =
        pTiming->HFrontPorch + pTiming->HVisible + pTiming->HSyncWidth;
    pModeTimings->hTotal = pTiming->HTotal;

    pModeTimings->vVisible = pTiming->VVisible;
    pModeTimings->vSyncStart = pTiming->VFrontPorch + pTiming->VVisible;
    pModeTimings->vSyncEnd =
        pTiming->VFrontPorch + pTiming->VVisible + pTiming->VSyncWidth;
    pModeTimings->vTotal = pTiming->VTotal;

    pModeTimings->interlaced = pTiming->interlaced;
    pModeTimings->doubleScan =
        !!(pTiming->etc.flag & NVT_FLAG_NV_DOUBLE_SCAN_TIMING);

    /*
     * pTiming stores vertical values divided by two when interlaced; so
     * double the vertical values in pModeTimings
     */

    if (pModeTimings->interlaced) {
        pModeTimings->vVisible *= 2;
        pModeTimings->vSyncStart *= 2;
        pModeTimings->vSyncEnd *= 2;
        pModeTimings->vTotal *= 2;
    }

    /*
     * pTiming: 0 is positive, 1 is negative
     * pModeTimings: FALSE is positive, TRUE is negative
     */

    if (pTiming->HSyncPol == NVT_H_SYNC_POSITIVE) {
        pModeTimings->hSyncNeg = NV_FALSE;
    } else {
        pModeTimings->hSyncNeg = NV_TRUE;
    }

    if (pTiming->VSyncPol == NVT_V_SYNC_POSITIVE) {
        pModeTimings->vSyncNeg = NV_FALSE;
    } else {
        pModeTimings->vSyncNeg = NV_TRUE;
    }

    pModeTimings->hSyncPos = !pModeTimings->hSyncNeg;
    pModeTimings->vSyncPos = !pModeTimings->vSyncNeg;

    /*
     * Save any physical size information for this mode from the
     * Detailed Timing Definition of the EDID.
     */
    if (NV_NVT_TIMING_HAS_IMAGE_SIZE(pTiming)) {
        pModeTimings->sizeMM.w = NV_NVT_TIMING_IMAGE_SIZE_WIDTH(pTiming);
        pModeTimings->sizeMM.h = NV_NVT_TIMING_IMAGE_SIZE_HEIGHT(pTiming);
    }

    /*
     * XXX work around lack of precision in NVT_TIMING: catch the
     * 640x480@60Hz EDID mode and patch pixelClockHz and RRx1k.
     */

    if (IsEdid640x480_60_NVT_TIMING(pTiming)) {
        pModeTimings->RRx1k = 59940;
        pModeTimings->pixelClockHz = 25175000;
    }
}


/*!
 * Build a mode name, of the format 'WIDTHxHEIGHT'.
 */
void nvBuildModeName(NvU16 width, NvU16 height, char *name, size_t nameLen)
{
    nvBuildModeNameSnprintf(name, nameLen, "%dx%d", width, height);
    name[nameLen - 1] = '\0';
}
