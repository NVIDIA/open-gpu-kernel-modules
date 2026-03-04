/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(NVKMS_MODETIMINGS_H)
#define NVKMS_MODETIMINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nv_mode_timings.h"

/*
 * For Kepler HW HDMI 1.4 frame packed stereo, HW combines two flips
 * into a single top-down double-height frame, and it needs a
 * doubled refresh rate to accommodate this.
 */
static inline void nvKmsUpdateNvModeTimingsForHdmi3D(NvModeTimings *pModeTimings,
                                                     NvBool hdmi3D)
{
    if (pModeTimings->hdmi3D == hdmi3D) {
        return;
    }

    if (hdmi3D) {
        pModeTimings->pixelClockHz *= 2;
        pModeTimings->RRx1k *= 2;
    } else {
        pModeTimings->pixelClockHz /= 2;
        pModeTimings->RRx1k /= 2;
    }

    pModeTimings->hdmi3D = hdmi3D;
}

#ifdef __cplusplus
};
#endif

#endif /* NVKMS_MODETIMINGS_H */
