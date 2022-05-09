/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef EFI_CONSOLE_H
#define EFI_CONSOLE_H

#include "gpu/disp/kern_disp_max.h"

struct OBJGPU;

typedef struct
{
    NvBool isDispStateSave;

    NvU32 activeDisplayId[OBJ_MAX_HEADS];

    struct
    {
        NvU32 displayId;

        struct {
            NvU32 index;
            NvU32 subLinkMask;
        } sorXBar;

        struct {
            NvU32 linkBw;
            NvU32 laneCount;
            NvU32 linkCtl;
        } displayPort;
    } activeDfpState[OBJ_MAX_DFPS];
    NvU32 numDfps;

    struct
    {
        NvU32           coreChannelClass;
        NvU32           cacheSize;
        NvU32          *pCache;
    } display;
} nv_efi_t;

void        RmSaveEFIDisplayState       (OBJGPU *pGpu);
void        RmRestoreEFIDisplayState    (OBJGPU *pGpu);

#endif // EFI_CONSOLE_H
