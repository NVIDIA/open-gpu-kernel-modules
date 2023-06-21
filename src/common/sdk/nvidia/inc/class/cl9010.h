/*
 * Copyright (c) 2016-2022, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl9010.finn
//

#include "class/cl9010_callback.h"

#define NV9010_VBLANK_CALLBACK (0x9010U) /* finn: Evaluated from "NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS_MESSAGE_ID" */

#define NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS_MESSAGE_ID (0x9010U)

typedef struct NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS {
    NV_DECLARE_ALIGNED(NvP64 pProc, 8);      // Routine to call at vblank time
                                             // A function pointer of OSVBLANKCALLBACKPROC
    NvV32 LogicalHead;                       // Logical Head
    NV_DECLARE_ALIGNED(NvP64 pParm1, 8);     // pParm1
    NV_DECLARE_ALIGNED(NvP64 pParm2, 8);     // pParm2
} NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS;

