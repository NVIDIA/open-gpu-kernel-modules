/*
 * Copyright (c) 2018-2022, NVIDIA CORPORATION. All rights reserved.
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
// Source file:      class/cl00c1.finn
//

#include "nvlimits.h"
#define NV_FB_SEGMENT (0xc1U) /* finn: Evaluated from "NV_FB_SEGMENT_ALLOCATION_PARAMS_MESSAGE_ID" */

/*
 * NV_FB_SEGMENT_ALLOCATION_PARAMS - Allocation params to create FB segment through
 * NvRmAlloc.
 */
#define NV_FB_SEGMENT_ALLOCATION_PARAMS_MESSAGE_ID (0x00C1U)

typedef struct NV_FB_SEGMENT_ALLOCATION_PARAMS {
    NvHandle hCtxDma;                       // unused
    NvU32    subDeviceIDMask;
    NV_DECLARE_ALIGNED(NvU64 dmaOffset, 8); // unused
    NV_DECLARE_ALIGNED(NvU64 VidOffset, 8);
    NV_DECLARE_ALIGNED(NvU64 Offset, 8);    // To be deprecated
    NV_DECLARE_ALIGNED(NvU64 pOffset[NV_MAX_SUBDEVICES], 8);
    NV_DECLARE_ALIGNED(NvU64 Length, 8);
    NV_DECLARE_ALIGNED(NvU64 ValidLength, 8);
    NV_DECLARE_ALIGNED(NvP64 pPageArray, 8);
    NvU32    startPageIndex;
    NvHandle AllocHintHandle;
    NvU32    Flags;
    NvHandle hMemory; // Not used in NvRmAlloc path; only used in CTRL path
    NvHandle hClient; // Not used in NvRmAlloc path; only used in CTRL path
    NvHandle hDevice; // Not used in NvRmAlloc path; only used in CTRL path
    NV_DECLARE_ALIGNED(NvP64 pCpuAddress, 8); // To be deprecated
    NV_DECLARE_ALIGNED(NvP64 ppCpuAddress[NV_MAX_SUBDEVICES], 8);
    NV_DECLARE_ALIGNED(NvU64 GpuAddress, 8); // To be deprecated
    NV_DECLARE_ALIGNED(NvU64 pGpuAddress[NV_MAX_SUBDEVICES], 8);
    NvHandle hAllocHintClient;
    NvU32    kind;
    NvU32    compTag;
} NV_FB_SEGMENT_ALLOCATION_PARAMS;

