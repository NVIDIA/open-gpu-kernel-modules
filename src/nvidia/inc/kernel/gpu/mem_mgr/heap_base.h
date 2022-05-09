/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _HEAP_BASE_H_
#define _HEAP_BASE_H_

#include "nvtypes.h"
#include "core/prelude.h"
#include "gpu/mem_mgr/mem_desc.h"

// Contains the minimal set of resources used to compute a PTE kind
typedef struct _def_fb_alloc_page_format
{
    NvU32    attr;
    NvU32    attr2;
    NvU32    flags;
    NvU32    kind;
    NvU32    type;
} FB_ALLOC_PAGE_FORMAT;

//
// FB allocation resources structure
// Need to be allocated from heap
//
typedef struct _def_fb_alloc_info
{
    NvU32    owner;
    NvU32    hwResId;
    NvU32    height;
    NvU32    width;
    NvU32    pitch;
    NvU64    size;
    NvU64    align;
    NvU64    alignPad;
    NvU64    pad;
    NvU64    offset;
    NvU32    internalflags;
    NvU32    retAttr;
    NvU32    retAttr2;
    NvU32    format;
    NvU32    comprCovg;
    NvU32    zcullCovg;
    NvU32    uncompressedKind;
    NvU32    compPageShift;
    NvU32    compressedKind;
    NvU32    compTagLineMin;
    NvU32    compPageIndexLo;
    NvU32    compPageIndexHi;
    NvU32    compTagLineMultiplier;
    NvU32    startCovg;
    NvU64    origSize;
    NvU64    adjustedSize;
    NvU64    desiredOffset;

    FB_ALLOC_PAGE_FORMAT * pageFormat;

    // Tracking client for VGPU
    NvHandle hClient;
    NvHandle hDevice;

    // These are only used in Vista
    // no need yet for possAttr2
    NvU32    possAttr;          // AllocHint, BindCompr
    NvU32    ctagOffset;

    // Special flag for kernel allocations
    NvBool bIsKernelAlloc;

    //
    // Number of 4KB pages in the PTE array
    // For contiguous allocation, this will be set to '1'
    //
    NvU64 pageCount4k;

    // denote that underlying physical allocation is contiguous or not
    NvBool bContig;

    //
    // Store the PTE Array to be used for allocating comptaglines
    // If the NVOS32_ATTR_PHYSICALITY_CONTIGUOUS is set, it will only have
    // one entry, otherwise it will have dynamically allocated memory
    // This will track the pages in 4KB granularity
    //
    RmPhysAddr pteArray[1];
} FB_ALLOC_INFO;

//
// Contains information on the various hw resources (compr, etc...) that
// can be associated with a memory allocation.
//
typedef struct HWRESOURCE_INFO
{
    NvU32 attr;       // NVOS32_ATTR_*
    NvU32 attr2;      // NVOS32_ATTR2_*
    NvU32 comprCovg;  // compression coverage
    NvU32 ctagOffset; // comptag offset
    NvU32 hwResId;
    NvU32 refCount;
    NvBool isVgpuHostAllocated; // used in vGPU guest RM to indicate if this HW resource is allocated by host RM or not. Used in Windows guest.
    NvBool isGuestAllocated;    // used in vGPU host RM to indicate if this HW resource is allocated from LIST_OBJECT path on behalf of Linux guest.
} HWRESOURCE_INFO;


typedef struct PMA_ALLOC_INFO
{
    NvBool bContig;
    NvU32  pageCount;
    NvU32  pageSize;
    NvU32  refCount;
    NvU64  allocSize;
    NvU32  flags;
    //
    // If bContig is TRUE, this array consists of one element.
    // If bContig is FALSE, this array is actually larger and
    // has one entry for each physical page in the allocation.
    // As a result, this structure must be allocated from heap.
    //
    NvU64  pageArray[1];
    //!!! Place nothing behind pageArray!!!
} PMA_ALLOC_INFO;

typedef struct MEMORY_ALLOCATION_REQUEST
{
    NV_MEMORY_ALLOCATION_PARAMS *pUserParams;
    OBJGPU                      *pGpu;
    NvHandle                     hMemory;            // in: can be NULL (translates to 0)
    NvU32                        internalflags;      // Extended flags ?! flags seem exhausted.
    HWRESOURCE_INFO             *pHwResource;        // out: data copied in if non-NULL
    MEMORY_DESCRIPTOR           *pMemDesc;           // in/out: allocate memdesc if NULL
    PMA_ALLOC_INFO              *pPmaAllocInfo[NV_MAX_SUBDEVICES];      // out: tracks the pre-allocated memory per GPU.
    NvU32                        classNum;
    NvHandle                     hClient;
    NvHandle                     hParent;
} MEMORY_ALLOCATION_REQUEST;

typedef struct
{
    NvU64 address;
    NvU32 type;
} BLACKLIST_ADDRESS;

#endif //_HEAP_BASE_H_
