/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef NV_ARCH_PUBLISHED_H
#define NV_ARCH_PUBLISHED_H

#include "nvcfg_sdk.h"

// high byte indicates GPU-SERIES, as defined in Gpus.pm.
#define NVGPU_ARCHITECTURE_SERIES               31:24
#define NVGPU_ARCHITECTURE_SERIES_CLASSIC        0x00
#define NVGPU_ARCHITECTURE_SERIES_SIMULATION     0x00      // XXX - really should be distinct from CLASSIC_GPUS
#define NVGPU_ARCHITECTURE_SERIES_TEGRA          0xE0
#define NVGPU_ARCHITECTURE_ARCH                  23:0

#define GPU_ARCHITECTURE(series, arch)   (DRF_DEF(GPU, _ARCHITECTURE, _SERIES, series) | \
                                          DRF_NUM(GPU, _ARCHITECTURE, _ARCH, arch))

//
// Architecture constants.
//
#define GPU_ARCHITECTURE_MAXWELL         GPU_ARCHITECTURE(_CLASSIC, 0x0110)
#define GPU_ARCHITECTURE_MAXWELL2        GPU_ARCHITECTURE(_CLASSIC, 0x0120)
#define GPU_ARCHITECTURE_PASCAL          GPU_ARCHITECTURE(_CLASSIC, 0x0130)
#define GPU_ARCHITECTURE_VOLTA           GPU_ARCHITECTURE(_CLASSIC, 0x0140)
#define GPU_ARCHITECTURE_VOLTA2          GPU_ARCHITECTURE(_CLASSIC, 0x0150)
#define GPU_ARCHITECTURE_TURING          GPU_ARCHITECTURE(_CLASSIC, 0x0160)
#define GPU_ARCHITECTURE_AMPERE          GPU_ARCHITECTURE(_CLASSIC, 0x0170)
#define GPU_ARCHITECTURE_HOPPER          GPU_ARCHITECTURE(_CLASSIC, 0x0180)
#define GPU_ARCHITECTURE_ADA             GPU_ARCHITECTURE(_CLASSIC, 0x0190)
#define GPU_ARCHITECTURE_BLACKWELL       GPU_ARCHITECTURE(_CLASSIC, 0x01A0)

#define GPU_ARCHITECTURE_T12X            GPU_ARCHITECTURE(_TEGRA,   0x0040)
#define GPU_ARCHITECTURE_T13X            GPU_ARCHITECTURE(_TEGRA,   0x0013)
#define GPU_ARCHITECTURE_T21X            GPU_ARCHITECTURE(_TEGRA,   0x0021)
#define GPU_ARCHITECTURE_T18X            GPU_ARCHITECTURE(_TEGRA,   0x0018)
#define GPU_ARCHITECTURE_T19X            GPU_ARCHITECTURE(_TEGRA,   0x0019)
#define GPU_ARCHITECTURE_T23X            GPU_ARCHITECTURE(_TEGRA,   0x0023)

#define GPU_ARCHITECTURE_SIMS            GPU_ARCHITECTURE(_SIMULATION, 0x01f0)  // eg: AMODEL

//
// Implementation constants.
// These must be unique within a single architecture.
//

#define GPU_IMPLEMENTATION_GM108         0x08
#define GPU_IMPLEMENTATION_GM107         0x07
#define GPU_IMPLEMENTATION_GM200         0x00
#define GPU_IMPLEMENTATION_GM204         0x04
#define GPU_IMPLEMENTATION_GM206         0x06

#define GPU_IMPLEMENTATION_GP100         0x00
#define GPU_IMPLEMENTATION_GP102         0x02
#define GPU_IMPLEMENTATION_GP104         0x04
#define GPU_IMPLEMENTATION_GP106         0x06
#define GPU_IMPLEMENTATION_GP107         0x07
#define GPU_IMPLEMENTATION_GP108         0x08

#define GPU_IMPLEMENTATION_GV100         0x00
#define GPU_IMPLEMENTATION_GV11B         0x0B

#define GPU_IMPLEMENTATION_TU102         0x02
#define GPU_IMPLEMENTATION_TU104         0x04
#define GPU_IMPLEMENTATION_TU106         0x06
#define GPU_IMPLEMENTATION_TU116         0x08    // TU116 has implementation ID 8 in HW
#define GPU_IMPLEMENTATION_TU117         0x07

#define GPU_IMPLEMENTATION_GA100         0x00
#define GPU_IMPLEMENTATION_GA102         0x02
#define GPU_IMPLEMENTATION_GA103         0x03
#define GPU_IMPLEMENTATION_GA104         0x04
#define GPU_IMPLEMENTATION_GA106         0x06
#define GPU_IMPLEMENTATION_GA107         0x07
#define GPU_IMPLEMENTATION_GA102F        0x0F
#define GPU_IMPLEMENTATION_GH100         0x00
#define GPU_IMPLEMENTATION_AD102         0x02
#define GPU_IMPLEMENTATION_AD103         0x03
#define GPU_IMPLEMENTATION_AD104         0x04
#define GPU_IMPLEMENTATION_AD106         0x06
#define GPU_IMPLEMENTATION_AD107         0x07

#define GPU_IMPLEMENTATION_GB100         0x00
#define GPU_IMPLEMENTATION_GB102         0x02

#define GPU_IMPLEMENTATION_T124          0x00
#define GPU_IMPLEMENTATION_T132          0x00
#define GPU_IMPLEMENTATION_T210          0x00
#define GPU_IMPLEMENTATION_T186          0x00
#define GPU_IMPLEMENTATION_T194          0x00
#define GPU_IMPLEMENTATION_T234          0x04
#define GPU_IMPLEMENTATION_T234D         0x05

/* SIMS gpus */
#define GPU_IMPLEMENTATION_AMODEL        0x00

#endif // NV_ARCH_PUBLISHED_H
