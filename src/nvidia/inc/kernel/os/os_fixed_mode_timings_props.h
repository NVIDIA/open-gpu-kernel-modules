/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _OS_FIXED_MODE_TIMINGS_PROPS_H_
#define _OS_FIXED_MODE_TIMINGS_PROPS_H_

#include "gpu/disp/kern_disp_max.h"
#include "nvtypes.h"

typedef struct
{
    NvU16 hActive;
    NvU16 hFrontPorch;
    NvU16 hSyncWidth;
    NvU16 hBackPorch;

    NvU16 vActive;
    NvU16 vFrontPorch;
    NvU16 vSyncWidth;
    NvU16 vBackPorch;

    NvU32 pclkKHz;
    NvU32 rrx1k;
} OS_MODE_TIMING;

typedef struct
{
    OS_MODE_TIMING timingsPerStream[OBJ_MAX_HEADS];
    NvU8 numTimings;
} OS_FIXED_MODE_TIMINGS;

#endif
