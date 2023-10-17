/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _MEM_UTILS_H_
#define _MEM_UTILS_H_

#include "core/prelude.h"

#define CLEAR_HAL_ATTR(a)                         \
    a = (a &~(DRF_NUM(OS32, _ATTR, _COMPR, 0x3) | \
              DRF_NUM(OS32, _ATTR, _ZCULL, 0x3)));

#define CLEAR_HAL_ATTR2(a)                       \
    a = (a & ~(DRF_SHIFTMASK(NVOS32_ATTR2_ZBC) | \
               DRF_SHIFTMASK(NVOS32_ATTR2_GPU_CACHEABLE)));

NvU64 memUtilsLeastCommonAlignment(NvU64 align1, NvU64 align2);

void memUtilsInitFBAllocInfo(NV_MEMORY_ALLOCATION_PARAMS *pAllocParams, FB_ALLOC_INFO *pFbAllocInfo,
                             NvHandle hClient, NvHandle hDevice);

NV_STATUS memUtilsAllocMemDesc(OBJGPU *pGpu, MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo,
                               MEMORY_DESCRIPTOR **ppMemDesc, Heap *pHeap, NV_ADDRESS_SPACE addrSpace,
                               NvBool bContig, NvBool *bAllocedMemDesc);

NV_STATUS  memUtilsMemSetNoBAR2(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, NvU8 value);

#endif //_MEM_UTILS_H_
