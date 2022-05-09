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

#ifndef __NV_MODE_TIMINGS_H__
#define __NV_MODE_TIMINGS_H__

#include "nvtypes.h"

/*
 * NvModeTimings: hardware-independent modetiming structure.
 *
 * For interlaced modes, the vertical values are stored in frame size,
 * rather than field size (e.g., for 1080i modes, vVisible is 1080,
 * not 540); similarly, for doublescan modes, the vertical values are
 * stored in normal framesize (not doubled vertically).
 *
 * RRx1k should be field rate for interlaced modes, and should be
 * frame rate for doubleScan modes; e.g., 1920x1080@60i and
 * 640x480@60d, not 1920x1080@30i or 640x480@120d.
 *
 * RRx1k is also the "real" refresh rate (time spent displaying one eye)
 * for HDMI 3D frame packed modes, e.g. 47940 (2x24hz) for 1920x1080@24
 * HDMI 3D mode. This needs to be halved again for all user-visible reported
 * refresh rates (which needs to report time spent between each vblank, or
 * each pair of eyes).
 *
 * pixelClock is doubled for doubleScan and HDMI 3D frame packed modes.
 *
 * The intent is that this structure match the X configuration file
 * ModeLine.
 *
 * hdmi3D reflects whether this mode is a HDMI 3D frame packed mode. True only
 * if the user selected HDMI 3D stereo mode and the GPU supports it. If true,
 * then pixelClock is doubled.
 *
 * yuv420Mode reflects whether this mode requires YUV 4:2:0 decimation into a
 * half-width output through headsurface (SW YUV420) or >=nvdisplay 4.0 HW CSC
 * (HW YUV420).
 *
 * If a mode requires SW YUV 4:2:0 emulation, the pixelClock and width values
 * in NvModeTimings will still be the full width values specified by the mode
 * parsed from the EDID (e.g. 3840x2160@60), but the pixelClock and width values
 * in NVHwModeTimingsEvo will be the "real" half width values programmed in HW
 * and rendered to through a headSurface transform (e.g. 1920x2160@60).  If a
 * mode requires HW YUV 4:2:0 CSC, the pixelClock and width values in both
 * NvModeTimings and NVHwModeTimingsEvo will be full width, and the decimation
 * to the half width scanout surface is performed in HW.  In both cases, only
 * the full width values should ever be reported to the client.
 */

enum NvYuv420Mode {
    NV_YUV420_MODE_NONE = 0,
    NV_YUV420_MODE_SW,
    NV_YUV420_MODE_HW,
};

typedef struct _NvModeTimings {
    NvU32 RRx1k;
    NvU32 pixelClockHz; /* in Hz units */
    NvU16 hVisible;
    NvU16 hSyncStart;
    NvU16 hSyncEnd;
    NvU16 hTotal;
    NvU16 hSkew;    /* Just placeholder for XRRModeInfo.hSkew */
    NvU16 vVisible;
    NvU16 vSyncStart;
    NvU16 vSyncEnd;
    NvU16 vTotal;
    struct {
        NvU16 w;
        NvU16 h;
    } sizeMM;
    NvBool interlaced;
    NvBool doubleScan;
    /*
     * Note: hSyncPos and vSyncPos are ignored, and the polarity is positive if
     * [hv]SyncNeg is false.  However, X.Org has separate flags for each, and
     * treats modes with positive, negative, both, and neither as separate
     * modes.
     */
    NvBool hSyncPos;
    NvBool hSyncNeg;
    NvBool vSyncPos;
    NvBool vSyncNeg;
    NvBool hdmi3D;
    enum NvYuv420Mode yuv420Mode;
} NvModeTimings, *NvModeTimingsPtr;

static inline NvBool NvModeTimingsMatch(const NvModeTimings *pA,
                                        const NvModeTimings *pB,
                                        NvBool ignoreSizeMM,
                                        NvBool ignoreRRx1k)
{
    /*
     * Ignore sizeMM and/or RRx1k, if requested.  The sizeMM and RRx1k fields
     * don't impact hardware modetiming values, so it is reasonable that some
     * callers may choose to ignore them when comparing NvModeTimings.
     */
    NvBool sizeMMmatches = ignoreSizeMM || ((pA->sizeMM.w == pB->sizeMM.w) &&
                                            (pA->sizeMM.h == pB->sizeMM.h));

    NvBool rrx1kMatches = ignoreRRx1k || (pA->RRx1k == pB->RRx1k);

    return (sizeMMmatches && rrx1kMatches &&
            (pA->pixelClockHz       == pB->pixelClockHz) &&
            (pA->hVisible           == pB->hVisible) &&
            (pA->hSyncStart         == pB->hSyncStart) &&
            (pA->hSyncEnd           == pB->hSyncEnd) &&
            (pA->hTotal             == pB->hTotal) &&
            (pA->hSkew              == pB->hSkew) &&
            (pA->vVisible           == pB->vVisible) &&
            (pA->vSyncStart         == pB->vSyncStart) &&
            (pA->vSyncEnd           == pB->vSyncEnd) &&
            (pA->vTotal             == pB->vTotal) &&
            (pA->interlaced         == pB->interlaced) &&
            (pA->doubleScan         == pB->doubleScan) &&
            (pA->hSyncPos           == pB->hSyncPos) &&
            (pA->hSyncNeg           == pB->hSyncNeg) &&
            (pA->vSyncPos           == pB->vSyncPos) &&
            (pA->vSyncNeg           == pB->vSyncNeg) &&
            (pA->hdmi3D             == pB->hdmi3D) &&
            (pA->yuv420Mode         == pB->yuv420Mode));
}

/*
 * Convert between Hz and kHz.
 *
 * Note that Hz ==> kHz ==> Hz is lossy.
 *
 * We do +500 before /1000 in order to round, rather than truncate.
 */
static inline NvU32 HzToKHz(NvU32 hz)
{
    return (hz + 500) / 1000;
}

static inline NvU32 KHzToHz(NvU32 kHz)
{
    return kHz * 1000;
}


#endif /* __NV_MODE_TIMINGS_H__ */
