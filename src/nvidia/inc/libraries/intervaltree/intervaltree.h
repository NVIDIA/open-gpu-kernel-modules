/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INTERVALTREE_H_
#define _INTERVALTREE_H_

#include "containers/map.h"
#include "os/nv_memory_area.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IntervalTreeNode IntervalTreeNode;

typedef struct IntervalTreeNode {
    IntervalTreeNode *pNext;
    IntervalTreeNode *pPrev;
    MapNode offsetNode;
    union {
        MapNode sizeNode;
        NvU64 size;
    };
} IntervalTreeNode;

MAKE_INTRUSIVE_MAP(IntervalTreeOffsetMap, IntervalTreeNode, offsetNode);
MAKE_INTRUSIVE_MAP(IntervalTreeSizeMap, IntervalTreeNode, sizeNode);

typedef struct IntervalTree {
    IntervalTreeOffsetMap offsetMap;
    IntervalTreeSizeMap sizeMap;
    PORT_MEM_ALLOCATOR *pAllocator;
    MemoryRange totalRange;
} IntervalTree;

void intervaltreeInit(IntervalTree *pTree, PORT_MEM_ALLOCATOR *pAllocator);
void intervaltreeDestroy(IntervalTree *pTree);

// Utility functions for directly inserting and removing ranges.
NV_STATUS intervaltreeInsert(IntervalTree *pTree, MemoryRange range);
NV_STATUS intervaltreeRemove(IntervalTree *pTree, MemoryRange range);
MemoryRange intervaltreeFindOffset(IntervalTree *pTree, NvU64 offset);
MemoryRange intervaltreeFindGESize(IntervalTree *pTree, NvU64 size, NvU64 baseAlignment);
MemoryRange intervaltreeFindMaxSize(IntervalTree *pTree);
MemoryRange intervaltreeFindMinSize(IntervalTree *pTree);

//
// API for allocating and freeing memory ranges. Partial frees are supported. Note that the "FromRange" variant is slower
// than the regular allocation variant because the slow path essentially has to linearly iterate over all ranges within the boundary
// range.
//
NV_STATUS intervaltreeAllocate(IntervalTree *pTree, NvU64 size, NvU64 baseAlignment, MemoryRange *pRange);
NV_STATUS intervaltreeAllocateFromRange(IntervalTree *pTree, MemoryRange boundaryRange, NvU64 size, NvU64 baseAlignment, MemoryRange *pRange);
NV_STATUS intervaltreeFree(IntervalTree *pTree, MemoryRange range);

#ifdef __cplusplus
}
#endif

#endif // _INTERVALTREE_H_
