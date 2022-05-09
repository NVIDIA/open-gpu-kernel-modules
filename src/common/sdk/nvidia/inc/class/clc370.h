/* 
 * Copyright (c) 2015-2020, NVIDIA CORPORATION. All rights reserved.
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

#ifndef _clc370_h_
#define _clc370_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#include "class/cl5070.h"

#define  NVC370_DISPLAY                                             (0x0000C370)

/* event values */
#define NVC370_NOTIFIERS_SW                     NV5070_NOTIFIERS_SW
#define NVC370_NOTIFIERS_BEGIN                  NV5070_NOTIFIERS_MAXCOUNT
#define NVC370_NOTIFIERS_VPR                    NVC370_NOTIFIERS_BEGIN + (0)
#define NVC370_NOTIFIERS_RG_SEM_NOTIFICATION    NVC370_NOTIFIERS_VPR + (1)
#define NVC370_NOTIFIERS_WIN_SEM_NOTIFICATION   NVC370_NOTIFIERS_RG_SEM_NOTIFICATION + (1)
#define NVC370_NOTIFIERS_MAXCOUNT               NVC370_NOTIFIERS_WIN_SEM_NOTIFICATION + (1)

typedef struct
{
    NvU32   numHeads; // Number of HEADs in this chip/display
    NvU32   numSors;  // Number of SORs in this chip/display
    NvU32   numPiors; // Number of PIORs in this chip/display
} NVC370_ALLOCATION_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc370_h_ */
