/*
 * Copyright (c) 2018-2021, NVIDIA CORPORATION. All rights reserved.
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

#ifndef _cl00c2_h_
#define _cl00c2_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define   NV01_MEMORY_LOCAL_PHYSICAL                               (0x000000c2)

typedef struct
{
    NvU64 memSize  NV_ALIGN_BYTES(8); // [OUT] 
    NvU32 format; // [IN] - PTE format to use
    NvU32 pageSize; // [IN] - Page size to use
} NV_PHYSICAL_MEMORY_ALLOCATION_PARAMS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _cl00c2_h_ */
