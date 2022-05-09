/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _EHEAP_H_
#define _EHEAP_H_

/*!
 * @brief
 *    EHEAP is an extent allocator.  It is just an abstract E(xtent)Heap.
 */

#include "nvtypes.h"
#include "nvos.h"
#include "containers/btree.h"
#include "utils/nvrange.h"

typedef struct OBJEHEAP *POBJEHEAP;
typedef struct OBJEHEAP OBJEHEAP;

typedef struct EMEMBLOCK *PEMEMBLOCK;
typedef struct EMEMBLOCK
{
    NvU64      begin;
    NvU64      end;
    NvU64      align;
    NvU32      growFlags;
    NvU32      refCount;
    NvU32      owner;
    NODE       node;
    PEMEMBLOCK prevFree;
    PEMEMBLOCK nextFree;
    PEMEMBLOCK prev;
    PEMEMBLOCK next;
    void      *pData;
} EMEMBLOCK;

typedef NvBool EHeapOwnershipComparator(void*, void*);

typedef NV_STATUS  (*EHeapDestruct)(POBJEHEAP);
typedef NV_STATUS  (*EHeapAlloc)(POBJEHEAP, NvU32, NvU32 *, NvU64 *, NvU64 *, NvU64 , NvU64, PEMEMBLOCK*, void*, EHeapOwnershipComparator*);
typedef NV_STATUS  (*EHeapFree)(POBJEHEAP, NvU64);
typedef void         (*EHeapInfo)(POBJEHEAP, NvU64 *, NvU64 *,NvU64 *, NvU64 *, NvU32 *, NvU64 *);
typedef void         (*EHeapInfoForRange)(POBJEHEAP, NV_RANGE, NvU64 *, NvU64 *, NvU32 *, NvU64 *);
typedef NV_STATUS  (*EHeapGetSize)(POBJEHEAP, NvU64 *);
typedef NV_STATUS  (*EHeapGetFree)(POBJEHEAP, NvU64 *);
typedef NV_STATUS  (*EHeapGetBase)(POBJEHEAP, NvU64 *);
typedef PEMEMBLOCK   (*EHeapGetBlock)(POBJEHEAP, NvU64, NvBool bReturnFreeBlock);
typedef NV_STATUS  (*EHeapSetAllocRange)(POBJEHEAP, NvU64 rangeLo, NvU64 rangeHi);
typedef NV_STATUS  (*EHeapTraversalFn)(POBJEHEAP, void *pEnv, PEMEMBLOCK, NvU32 *pContinue, NvU32 *pInvalCursor);
typedef NV_STATUS  (*EHeapTraverse)(POBJEHEAP, void *pEnv, EHeapTraversalFn, NvS32 direction);
typedef NvU32        (*EHeapGetNumBlocks)(POBJEHEAP);
typedef NV_STATUS  (*EHeapGetBlockInfo)(POBJEHEAP, NvU32, NVOS32_HEAP_DUMP_BLOCK *);
typedef NV_STATUS  (*EHeapSetOwnerIsolation)(POBJEHEAP, NvBool bEnable, NvU32 granularity);

struct OBJEHEAP
{
    // Public heap interface methods
    EHeapDestruct          eheapDestruct;
    EHeapAlloc             eheapAlloc;
    EHeapFree              eheapFree;
    EHeapInfo              eheapInfo;
    EHeapInfoForRange      eheapInfoForRange;
    EHeapGetSize           eheapGetSize;
    EHeapGetFree           eheapGetFree;
    EHeapGetBase           eheapGetBase;
    EHeapGetBlock          eheapGetBlock;
    EHeapSetAllocRange     eheapSetAllocRange;
    EHeapTraverse          eheapTraverse;
    EHeapGetNumBlocks      eheapGetNumBlocks;
    EHeapGetBlockInfo      eheapGetBlockInfo;
    EHeapSetOwnerIsolation eheapSetOwnerIsolation;

    // private data
    NvU64      base;
    NvU64      total;
    NvU64      free;
    NvU64      rangeLo;
    NvU64      rangeHi;
    NvBool     bOwnerIsolation;
    NvU32      ownerGranularity;
    PEMEMBLOCK pBlockList;
    PEMEMBLOCK pFreeBlockList;
    NvU32      memHandle;
    NvU32      numBlocks;
    NvU32      sizeofMemBlock;
    PNODE      pBlockTree;
    // user can specify num of EMEMBLOCK structs to
    // be allocated at heap construction time so that
    // we will not call portMemAllocNonPaged during eheapAlloc.
    NvU32      numPreAllocMemStruct;
    PEMEMBLOCK pFreeMemStructList;
    PEMEMBLOCK pPreAllocAddr;
};

extern void constructObjEHeap(POBJEHEAP, NvU64, NvU64, NvU32, NvU32);

#endif // _EHEAP_H_
