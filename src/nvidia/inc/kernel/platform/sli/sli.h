/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef RMSLI_H
#define RMSLI_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Private SLI related defines and structures.                         *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "nvlimits.h"
#include "nvmisc.h"

#define IsDeviceDestroyed(p) (gpuGetDeviceInstance(p) == NV_MAX_DEVICES)

// Unlinked SLI is implemented in RM clients
#define IsUnlinkedSLIEnabled(p) ((p)->getProperty((p), PDB_PROP_GPU_RM_UNLINKED_SLI))

void RmInitScalability(OBJGPU *pGpu);

#define IsSLIEnabled(p)         0
#define NumSubDevices(p)        0

#define SLI_LOOP_START(sliLoopFlags)   { NvU32 loopIndex = 0; do {
#define SLI_LOOP_END                   } while (loopIndex); }
#define SLI_LOOP_BREAK                 break
#define SLI_LOOP_CONTINUE              continue
#define SLI_LOOP_GOTO(loc)             { goto loc; }
#define SLI_LOOP_RETURN(SLi_ret)       { return(SLi_ret); }
#define SLI_LOOP_RETURN_VOID           { return; }

// macro to use when declaring array vars that'll be used w/i SLI_LOOPs
#define SLI_LOOP_ARRAY_SIZE             (NV_MAX_SUBDEVICES+1)

// macro to veirfy that arrays are properly sized
#define VERIFY_SLI_LOOP_ARRAY_SIZE(arr)                                     \
do {                                                                        \
    if (sizeof(arr) > sizeof(void *))                                       \
    {                                                                       \
        NV_ASSERT(SLI_LOOP_ARRAY_SIZE == NV_ARRAY_ELEMENTS(arr));   	    \
    }                                                                       \
} while (0)

#endif // RMSLI_H
