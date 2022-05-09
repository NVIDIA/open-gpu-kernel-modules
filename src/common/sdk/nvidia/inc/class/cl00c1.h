/*
 * Copyright (c) 2018-2018, NVIDIA CORPORATION. All rights reserved.
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

#ifndef _cl00c1_h_
#define _cl00c1_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"
#include "nvlimits.h"

#define   NV_FB_SEGMENT                         (0x000000C1)

/*
 * NV_FB_SEGMENT_ALLOCATION_PARAMS - Allocation params to create FB segment through
 * NvRmAlloc.
 */
typedef struct
{
    NvHandle    hCtxDma;
    NvU32       subDeviceIDMask NV_ALIGN_BYTES(8);
    NvU64       dmaOffset       NV_ALIGN_BYTES(8);
    NvU64       VidOffset       NV_ALIGN_BYTES(8);
    NvU64       Offset          NV_ALIGN_BYTES(8); // To be deprecated
    NvU64       pOffset[NV_MAX_SUBDEVICES] NV_ALIGN_BYTES(8);
    NvU64       Length          NV_ALIGN_BYTES(8);
    NvU64       ValidLength     NV_ALIGN_BYTES(8);
    NvP64       pPageArray      NV_ALIGN_BYTES(8);
    NvU32       startPageIndex;
    NvHandle    AllocHintHandle;
    NvU32       Flags;
    NvHandle    hMemory; // Not used in NvRmAlloc path; only used in CTRL path
    NvHandle    hClient; // Not used in NvRmAlloc path; only used in CTRL path
    NvHandle    hDevice; // Not used in NvRmAlloc path; only used in CTRL path
    NvP64       pCpuAddress     NV_ALIGN_BYTES(8); // To be deprecated
    NvP64       ppCpuAddress[NV_MAX_SUBDEVICES] NV_ALIGN_BYTES(8);
    NvU64       GpuAddress      NV_ALIGN_BYTES(8); // To be deprecated
    NvU64       pGpuAddress[NV_MAX_SUBDEVICES] NV_ALIGN_BYTES(8);
    NvHandle    hAllocHintClient;
    NvU32       kind;
    NvU32       compTag;
} NV_FB_SEGMENT_ALLOCATION_PARAMS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl00c1_h_ */
