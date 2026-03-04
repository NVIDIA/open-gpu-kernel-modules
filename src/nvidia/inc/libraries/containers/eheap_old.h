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
#ifndef EHEAP_H
#define EHEAP_H

/*!
 * @brief
 *    EHEAP is an extent allocator.  It is just an abstract E(xtent)Heap.
 */

#include "nvtypes.h"
#include "nvos.h"
#include "containers/btree.h"
#include "utils/nvrange.h"

typedef struct OBJEHEAP OBJEHEAP;

typedef struct EMEMBLOCK EMEMBLOCK;
struct EMEMBLOCK
{
    NvU64      begin;
    NvU64      end;
    NvU64      align;
    NvU32      growFlags;
    NvU32      refCount;
    NvU32      owner;
    NODE       node;
    EMEMBLOCK *prevFree;
    EMEMBLOCK *nextFree;
    EMEMBLOCK *prev;
    EMEMBLOCK *next;
    void      *pData;
};

typedef NvBool EHeapOwnershipComparator(void*, void*);

typedef NV_STATUS  (*EHeapDestruct)(OBJEHEAP *);
typedef NV_STATUS  (*EHeapAlloc)(OBJEHEAP *, NvU32, NvU32 *, NvU64 *, NvU64 *, NvU64 , NvU64, EMEMBLOCK **, void*, EHeapOwnershipComparator*);
typedef NV_STATUS  (*EHeapFree)(OBJEHEAP *, NvU64);
typedef void       (*EHeapInfo)(OBJEHEAP *, NvU64 *, NvU64 *,NvU64 *, NvU64 *, NvU32 *, NvU64 *);
typedef void       (*EHeapInfoForRange)(OBJEHEAP *, NV_RANGE, NvU64 *, NvU64 *, NvU32 *, NvU64 *);
typedef NV_STATUS  (*EHeapGetSize)(OBJEHEAP *, NvU64 *);
typedef NV_STATUS  (*EHeapGetFree)(OBJEHEAP *, NvU64 *);
typedef NV_STATUS  (*EHeapGetBase)(OBJEHEAP *, NvU64 *);
typedef EMEMBLOCK *(*EHeapGetBlock)(OBJEHEAP *, NvU64, NvBool bReturnFreeBlock);
typedef NV_STATUS  (*EHeapSetAllocRange)(OBJEHEAP *, NvU64 rangeLo, NvU64 rangeHi);
typedef NV_STATUS  (*EHeapTraversalFn)(OBJEHEAP *, void *pEnv, EMEMBLOCK *, NvU32 *pContinue, NvU32 *pInvalCursor);
typedef NV_STATUS  (*EHeapTraverse)(OBJEHEAP *, void *pEnv, EHeapTraversalFn, NvS32 direction);
typedef NvU32      (*EHeapGetNumBlocks)(OBJEHEAP *);
typedef NV_STATUS  (*EHeapSetOwnerIsolation)(OBJEHEAP *, NvBool bEnable, NvU32 granularity);

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
    EHeapSetOwnerIsolation eheapSetOwnerIsolation;

    // private data
    NvU64      base;
    NvU64      total;
    NvU64      free;
    NvU64      rangeLo;
    NvU64      rangeHi;
    NvBool     bOwnerIsolation;
    NvU32      ownerGranularity;
    EMEMBLOCK *pBlockList;
    EMEMBLOCK *pFreeBlockList;
    NvU32      memHandle;
    NvU32      numBlocks;
    NvU32      sizeofMemBlock;
    PNODE      pBlockTree;
    // user can specify num of EMEMBLOCK structs to
    // be allocated at heap construction time so that
    // we will not call portMemAllocNonPaged during eheapAlloc.
    NvU32      numPreAllocMemStruct;
    EMEMBLOCK *pFreeMemStructList;
    EMEMBLOCK *pPreAllocAddr;
};

extern void constructObjEHeap(OBJEHEAP *, NvU64, NvU64, NvU32, NvU32);

#endif // EHEAP_H
