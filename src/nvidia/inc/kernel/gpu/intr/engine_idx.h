/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef ENGINE_IDX_H
#define ENGINE_IDX_H

#include "utils/nvbitvector.h"

/***************************************************************************\
*                                                                           *
* Module: engine_idx.h
*       List of engines for use by INTR (and MC) modules.
*                                                                           *
\***************************************************************************/

//
// Engine bits for use by various MC HAL routines
//
#define MC_ENGINE_IDX_NULL                          0 // This must be 0
#define MC_ENGINE_IDX_TMR                           1
#define MC_ENGINE_IDX_DISP                          2
#define MC_ENGINE_IDX_FB                            3
#define MC_ENGINE_IDX_FIFO                          4
#define MC_ENGINE_IDX_VIDEO                         5
#define MC_ENGINE_IDX_MD                            6
#define MC_ENGINE_IDX_BUS                           7
#define MC_ENGINE_IDX_PMGR                          8
#define MC_ENGINE_IDX_VP2                           9
#define MC_ENGINE_IDX_CIPHER                        10
#define MC_ENGINE_IDX_BIF                           11
#define MC_ENGINE_IDX_PPP                           12
#define MC_ENGINE_IDX_PRIVRING                      13
#define MC_ENGINE_IDX_PMU                           14
#define MC_ENGINE_IDX_CE0                           15
#define MC_ENGINE_IDX_CE1                           16
#define MC_ENGINE_IDX_CE2                           17
#define MC_ENGINE_IDX_CE3                           18
#define MC_ENGINE_IDX_CE4                           19
#define MC_ENGINE_IDX_CE5                           20
#define MC_ENGINE_IDX_CE6                           21
#define MC_ENGINE_IDX_CE7                           22
#define MC_ENGINE_IDX_CE8                           23
#define MC_ENGINE_IDX_CE9                           24
#define MC_ENGINE_IDX_CE_MAX                        MC_ENGINE_IDX_CE9
#define MC_ENGINE_IDX_VIC                           35
#define MC_ENGINE_IDX_ISOHUB                        36
#define MC_ENGINE_IDX_VGPU                          37
#define MC_ENGINE_IDX_MSENC                         38
#define MC_ENGINE_IDX_MSENC1                        39
#define MC_ENGINE_IDX_MSENC2                        40
#define MC_ENGINE_IDX_C2C                           41
#define MC_ENGINE_IDX_LTC                           42
#define MC_ENGINE_IDX_FBHUB                         43
#define MC_ENGINE_IDX_HDACODEC                      44
#define MC_ENGINE_IDX_GMMU                          45
#define MC_ENGINE_IDX_SEC2                          46
#define MC_ENGINE_IDX_FSP                           47
#define MC_ENGINE_IDX_NVLINK                        48
#define MC_ENGINE_IDX_GSP                           49
#define MC_ENGINE_IDX_NVJPG                         50
#define MC_ENGINE_IDX_NVJPEG                        MC_ENGINE_IDX_NVJPG
#define MC_ENGINE_IDX_NVJPEG0                       MC_ENGINE_IDX_NVJPEG
#define MC_ENGINE_IDX_NVJPEG1                       51
#define MC_ENGINE_IDX_NVJPEG2                       52
#define MC_ENGINE_IDX_NVJPEG3                       53
#define MC_ENGINE_IDX_NVJPEG4                       54
#define MC_ENGINE_IDX_NVJPEG5                       55
#define MC_ENGINE_IDX_NVJPEG6                       56
#define MC_ENGINE_IDX_NVJPEG7                       57
#define MC_ENGINE_IDX_REPLAYABLE_FAULT              58
#define MC_ENGINE_IDX_ACCESS_CNTR                   59
#define MC_ENGINE_IDX_NON_REPLAYABLE_FAULT          60
#define MC_ENGINE_IDX_REPLAYABLE_FAULT_ERROR        61
#define MC_ENGINE_IDX_NON_REPLAYABLE_FAULT_ERROR    62
#define MC_ENGINE_IDX_INFO_FAULT                    63
#define MC_ENGINE_IDX_BSP                           64
#define MC_ENGINE_IDX_NVDEC                         MC_ENGINE_IDX_BSP
#define MC_ENGINE_IDX_NVDEC0                        MC_ENGINE_IDX_NVDEC
#define MC_ENGINE_IDX_NVDEC1                        65
#define MC_ENGINE_IDX_NVDEC2                        66
#define MC_ENGINE_IDX_NVDEC3                        67
#define MC_ENGINE_IDX_NVDEC4                        68
#define MC_ENGINE_IDX_NVDEC5                        69
#define MC_ENGINE_IDX_NVDEC6                        70
#define MC_ENGINE_IDX_NVDEC7                        71
#define MC_ENGINE_IDX_CPU_DOORBELL                  72
#define MC_ENGINE_IDX_PRIV_DOORBELL                 73
#define MC_ENGINE_IDX_MMU_ECC_ERROR                 74
#define MC_ENGINE_IDX_BLG                           75
#define MC_ENGINE_IDX_PERFMON                       76
#define MC_ENGINE_IDX_BUF_RESET                     77
#define MC_ENGINE_IDX_XBAR                          78
#define MC_ENGINE_IDX_ZPW                           79
#define MC_ENGINE_IDX_OFA0                          80
#define MC_ENGINE_IDX_TEGRA                         81
#define MC_ENGINE_IDX_GR                            82
#define MC_ENGINE_IDX_GR0                           MC_ENGINE_IDX_GR
#define MC_ENGINE_IDX_GR1                           83
#define MC_ENGINE_IDX_GR2                           84
#define MC_ENGINE_IDX_GR3                           85
#define MC_ENGINE_IDX_GR4                           86
#define MC_ENGINE_IDX_GR5                           87
#define MC_ENGINE_IDX_GR6                           88
#define MC_ENGINE_IDX_GR7                           89
#define MC_ENGINE_IDX_ESCHED                        90
#define MC_ENGINE_IDX_ESCHED__SIZE                  64
#define MC_ENGINE_IDX_GR_FECS_LOG                   154
#define MC_ENGINE_IDX_GR0_FECS_LOG                  MC_ENGINE_IDX_GR_FECS_LOG
#define MC_ENGINE_IDX_GR1_FECS_LOG                  155
#define MC_ENGINE_IDX_GR2_FECS_LOG                  156
#define MC_ENGINE_IDX_GR3_FECS_LOG                  157
#define MC_ENGINE_IDX_GR4_FECS_LOG                  158
#define MC_ENGINE_IDX_GR5_FECS_LOG                  159
#define MC_ENGINE_IDX_GR6_FECS_LOG                  160
#define MC_ENGINE_IDX_GR7_FECS_LOG                  161
#define MC_ENGINE_IDX_TMR_SWRL                      162
#define MC_ENGINE_IDX_DISP_GSP                      163
#define MC_ENGINE_IDX_REPLAYABLE_FAULT_CPU          164
#define MC_ENGINE_IDX_NON_REPLAYABLE_FAULT_CPU      165
#define MC_ENGINE_IDX_PXUC                          166
#define MC_ENGINE_IDX_MAX                           167 // This must be kept as the max bit if
                                                        // we need to add more engines
#define MC_ENGINE_IDX_INVALID                0xFFFFFFFF

// Index GR reference
#define MC_ENGINE_IDX_GRn(x)            (MC_ENGINE_IDX_GR0 + (x))
#define MC_ENGINE_IDX_GRn_FECS_LOG(x)   (MC_ENGINE_IDX_GR0_FECS_LOG + (x))

// Index CE reference
#define MC_ENGINE_IDX_CE(x)             (MC_ENGINE_IDX_CE0 + (x))

// Index MSENC reference
#define MC_ENGINE_IDX_MSENCn(x)         (MC_ENGINE_IDX_MSENC + (x))

// Index NVDEC reference
#define MC_ENGINE_IDX_NVDECn(x)         (MC_ENGINE_IDX_NVDEC + (x))

// Index NVJPEG reference
#define MC_ENGINE_IDX_NVJPEGn(x)        (MC_ENGINE_IDX_NVJPEG + (x))

// Index ESCHED reference
#define MC_ENGINE_IDX_ESCHEDn(x)        (MC_ENGINE_IDX_ESCHED + (x))

#define MC_ENGINE_IDX_IS_CE(x) \
    ((MC_ENGINE_IDX_CE(0) <= (x)) && ((x) <= MC_ENGINE_IDX_CE_MAX))

MAKE_BITVECTOR(MC_ENGINE_BITVECTOR, MC_ENGINE_IDX_MAX);
typedef MC_ENGINE_BITVECTOR *PMC_ENGINE_BITVECTOR;

#endif // ENGINE_IDX_H
