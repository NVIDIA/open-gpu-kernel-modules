
#ifndef _G_KERN_HWPM_COMMON_DEFS_NVOC_H_
#define _G_KERN_HWPM_COMMON_DEFS_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERN_HWPM_COMMON_DEFS_H
#define KERN_HWPM_COMMON_DEFS_H

#include "lib/ref_count.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#define NV_HWPM_STREAM_FLAGS_CPUVA_EXTERNAL 0x0001

typedef struct
{
    struct OBJREFCNT *pPmaVasRefcnt;
    struct OBJREFCNT *pHwpmIbBindRefcnt;
    struct OBJVASPACE *pPmaVAS;
    NvBool bPmaVasRequested;
    MEMORY_DESCRIPTOR *pInstBlkMemDesc;
    NvBool hwpmIbBindState;
} HWPM_STREAMOUT_STATE;

typedef struct
{
    MEMORY_DESCRIPTOR *pRecordBufDesc;
    MEMORY_DESCRIPTOR *pNumBytesBufDesc;
    RsResourceRef *pMemoryPmaBufferRef;
    RsResourceRef *pMemoryPmaAvailBytesRef;
    NvU64 vaddr;
    NvU64 size;
    NvU64 vaddrRecordBuf;
    NvU64 vaddrNumBytesBuf;
    NvP64 pNumBytesCpuAddr;
    NvP64 pNumBytesCpuAddrPriv;
    NvU32 pmaChannelIdx;
    NvU32 bValid;
    NvU32 flags;
    NvBool bMemBytesBufferAccessAllowed;
} HWPM_PMA_STREAM;

#define MAX_PMA_CREDIT_POOL     2

#endif // KERN_HWPM_COMMON_DEFS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_HWPM_COMMON_DEFS_NVOC_H_
