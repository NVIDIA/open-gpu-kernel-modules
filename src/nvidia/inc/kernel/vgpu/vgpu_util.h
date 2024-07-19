/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Declarations for VGPU util functions.
//
//******************************************************************************

#ifndef __vgpu_util_h__
#define __vgpu_util_h__

#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "vgpu/vgpu_events.h"

#include "vgpu/dev_vgpu.h"

NV_STATUS vgpuAllocSysmemPfnBitMapNode(OBJGPU *pGpu, VGPU_SYSMEM_PFN_BITMAP_NODE_P *node, NvU32 index);
NvU64 vgpuGspSysmemPfnMakeBufferAddress(MEMORY_DESCRIPTOR *pMemDesc,  NvU64 pfn);
void vgpuFreeSysmemPfnBitMapNode(VGPU_SYSMEM_PFN_BITMAP_NODE_P node);
NV_STATUS vgpuUpdateSysmemPfnBitMap(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvBool bAlloc);
NV_STATUS vgpuUpdateGuestSysmemPfnBitMap(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvBool bAlloc);

static inline NvBool vgpuIsGuestManagedHwAlloc(OBJGPU *pGpu)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    return pVSI && pVSI->guestManagedHwAlloc;
}

#endif // __vgpu_util_h__
