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

#ifndef __NV_MODE_TIMINGS_UTILS_H__
#define __NV_MODE_TIMINGS_UTILS_H__

/*
 * Utility code to operate on NvModeTimings and NVT_TIMINGs.
 */

#include "nvtypes.h"

#include "nv_mode_timings.h"
#include "timing/nvtiming.h"

#include <stddef.h> /* size_t */

/*
 * Macros used for printing values divided by 1000 without floating
 * point division and printing.
 *
 * Example:
 * printf("pclk is %.2f MHz\n", pclk_khz / 1000.0f);
 * becomes:
 * printf("pclk is " NV_FMT_DIV_1000_POINT_2 " MHz\n",
 *        NV_VA_DIV_1000_POINT_2(pclk_khz));
 *
 * Different precision controls the number of digits printed after the
 * decimal point. Bias is added for correct rounding.
 */
#define NV_FMT_DIV_1000_POINT_1 "%d.%d"
#define NV_FMT_DIV_1000_POINT_2 "%d.%02d"
#define NV_VA_DIV_1000_POINT_1(x) \
    ((x) + 49) / 1000, (((x) + 49) % 1000) / 100
#define NV_VA_DIV_1000_POINT_2(x) \
    ((x) +  4) / 1000, (((x) + 4)  % 1000) / 10

/*
 * macro to use integer math to convert an NvU32 kHz value to Hz; we
 * add 500 Hz before dividing by 1000 to round rather than truncate.
 */

#define NV_U32_KHZ_TO_HZ(_x) (((_x) + 500) / 1000)

/*
 * NVT_TIMING stores HVisible multiplied by the horizontal replication
 * factor (e.g., a 720 mode with hrep=2 has HVisible of 1440).  For
 * reporting purposes, divide HVisible by hrep.
 */
static inline NvU16 NV_NVT_TIMING_HVISIBLE(const NVT_TIMING *pTiming)
{
    if (pTiming->etc.rep > 1) {
        return pTiming->HVisible / pTiming->etc.rep;
    } else {
        return pTiming->HVisible;
    }
}

/*
 * NVT_TIMING stores VVisible as half height when interlaced (e.g.,
 * 1920x1080i has VVisible 540).
 */
static inline NvU16 NV_NVT_TIMING_VVISIBLE(const NVT_TIMING *pTiming)
{
    return pTiming->VVisible * (pTiming->interlaced ? 2 : 1);
}

/*
 * When non-zero, NVT_TIMING::etc::aspect contains bytes 12, 13, and
 * 14 from the Detailed Timing Definition of the EDID.  This contains
 * a packed width and height.  The width and height is either an
 * aspect ratio (16:9 or 4:3), or a physical image size in
 * millimeters.  See Table 3.21, and the subsequent notes, in the
 * E-EDID 1.4 specification.
 */
static inline NvU16 NV_NVT_TIMING_IMAGE_SIZE_WIDTH(const NVT_TIMING *pTiming)
{
    return (pTiming->etc.aspect >> 16) & 0xFFFF;
}

static inline NvU16 NV_NVT_TIMING_IMAGE_SIZE_HEIGHT(const NVT_TIMING *pTiming)
{
    return pTiming->etc.aspect & 0xFFFF;
}

static inline NvBool NV_NVT_TIMING_HAS_ASPECT_RATIO(const NVT_TIMING *pTiming)
{
    NvU16 w = NV_NVT_TIMING_IMAGE_SIZE_WIDTH(pTiming);
    NvU16 h = NV_NVT_TIMING_IMAGE_SIZE_HEIGHT(pTiming);

    return (((w == 16) && (h == 9)) ||
            ((w ==  4) && (h == 3)));
}

static inline NvBool NV_NVT_TIMING_HAS_IMAGE_SIZE(const NVT_TIMING *pTiming)
{
    return ((pTiming->etc.aspect != 0) &&
            !NV_NVT_TIMING_HAS_ASPECT_RATIO(pTiming));
}

NvBool IsEdid640x480_60_NVT_TIMING(const NVT_TIMING *pTiming);

void NVT_TIMINGtoNvModeTimings(const NVT_TIMING *pTiming,
                               NvModeTimingsPtr pModeTimings);

void nvBuildModeName(NvU16 width, NvU16 height, char *name, size_t nameLen);

/*
 * Users of nvBuildModeName() should provide an implementation of
 * nvBuildModeNameSnprintf().
 */
int nvBuildModeNameSnprintf(char *str, size_t size, const char *format, ...)
    __attribute__((format (printf, 3, 4)));

#endif /* __NV_MODE_TIMINGS_UTILS_H__ */
