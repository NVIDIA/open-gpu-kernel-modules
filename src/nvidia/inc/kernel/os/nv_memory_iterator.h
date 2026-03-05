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

#ifndef NV_MEMORY_ITERATOR_H
#define NV_MEMORY_ITERATOR_H

#include "os/nv_memory_area.h"

typedef enum MemoryIteratorType
{
    MEMORY_ITERATOR_TYPE_INVALID,
    MEMORY_ITERATOR_TYPE_ARRAY,
    MEMORY_ITERATOR_TYPE_MEMORY_AREA,
} MemoryIteratorType;

typedef struct MemoryAreaIterator
{
    MemoryArea memoryArea;
    NvU64 curIdx;
    NvU64 beginAlign;
    NvU64 endAlign;
} MemoryAreaIterator;

typedef struct MemoryArrayIterator
{
    NvU64 curIdx;
    NvU64 *pAddrArray;
    NvU64 numEntries;
    NvU64 entrySize;
    NvU64 beginAlign;
    NvU64 endAlign;
} MemoryArrayIterator;

typedef struct MemoryIterator
{
    MemoryIteratorType type;
    union
    {
        MemoryArrayIterator array;
        MemoryAreaIterator memoryArea;
    };
} MemoryIterator;

static const MemoryIterator memoryIteratorInvalid = {
    MEMORY_ITERATOR_TYPE_INVALID,
};

/*!
 * Initialize a memory area iterator
 *
 * @param[in] memoryArea: The memory area to iterate over
 * @param[in] beginAlign: The alignment offset at the beginning of the memory area
 * @param[in] endAlign: The alignment offset at the end of the memory area
 *
 * @return The initialized MemoryIterator
 */
static inline MemoryIterator
memoryAreaIteratorInit
(
    MemoryArea memoryArea,
    NvU64 beginAlign,
    NvU64 endAlign
)
{
    MemoryIterator iterator;

    while (memoryArea.numRanges != 0)
    {
        if (memoryArea.pRanges[0].size > beginAlign)
        {
            break;
        }
        beginAlign -= memoryArea.pRanges[0].size;
        memoryArea.pRanges = &memoryArea.pRanges[1];
        memoryArea.numRanges--;
    }

    while (memoryArea.numRanges != 0)
    {
        if (memoryArea.pRanges[memoryArea.numRanges - 1llu].size > endAlign)
        {
            break;
        }
        endAlign -= memoryArea.pRanges[memoryArea.numRanges - 1llu].size;
        memoryArea.numRanges--;
    }

    if (memoryArea.numRanges == 0)
    {
        return memoryIteratorInvalid;
    }

    if ((memoryArea.numRanges == 1) && ((memoryArea.pRanges[0].size - endAlign) <= beginAlign))
    {
        return memoryIteratorInvalid;
    }

    iterator.type = MEMORY_ITERATOR_TYPE_MEMORY_AREA;
    iterator.memoryArea.memoryArea = memoryArea;
    iterator.memoryArea.curIdx = 0;
    iterator.memoryArea.beginAlign = beginAlign;
    iterator.memoryArea.endAlign = endAlign;

    return iterator;
}

// Internal iterator implementation for the memory area iterator type. Do not call directly.
static inline NvBool
_memoryAreaIteratorNext
(
    MemoryAreaIterator *pIterator,
    MemoryRange *pMemRange
)
{
    NvU64 i = pIterator->curIdx;
    NvU64 numRanges = pIterator->memoryArea.numRanges;

    // If we've iterated over all ranges, return false
    if (i >= numRanges)
    {
        return NV_FALSE;
    }

    *pMemRange = pIterator->memoryArea.pRanges[i];
    i++;

    // Adjust the first range for any alignment offset at the beginning
    if (pIterator->curIdx == 0)
    {
        pMemRange->start += pIterator->beginAlign;
        pMemRange->size -= pIterator->beginAlign;
    }

    // Try to merge contiguous or adjacent pages into a single range
    while (i < numRanges)
    {
        // If the next range is not contiguous at the end, break
        if (pIterator->memoryArea.pRanges[i].start != mrangeLimit(*pMemRange))
        {
            break;
        }

        // Extend the range to include the next range
        pMemRange->size += pIterator->memoryArea.pRanges[i].size;
        i++;
    }

    // Adjust the last range for any alignment offset at the end
    if (i == numRanges)
    {
        pMemRange->size -= pIterator->endAlign;
    }

    // Update the iterator's current index
    pIterator->curIdx = i;
    return NV_TRUE;
}

/*!
 * Initialize a memory array iterator
 *
 * @param[in] pAddrArray: The array of addresses to iterate over
 * @param[in] numEntries: The number of entries in the array
 * @param[in] entrySize: The size of each entry in the array
 * @param[in] beginAlign: The alignment offset at the beginning of the array
 * @param[in] endAlign: The alignment offset at the end of the array
 *
 * @return The initialized MemoryIterator
 */
static inline MemoryIterator
memoryArrayIteratorInit
(
    NvU64 *pAddrArray,
    NvU64 numEntries,
    NvU64 entrySize,
    NvU64 beginAlign,
    NvU64 endAlign
)
{
    MemoryIterator iterator;
    NvU64          beginIdx = beginAlign / entrySize;
    NvU64          endIdxOffset = endAlign / entrySize;

    beginAlign %= entrySize;
    endAlign %= entrySize;

    if (beginIdx >= numEntries)
    {
        return memoryIteratorInvalid;
    }

    numEntries -= beginIdx;
    pAddrArray += beginIdx;

    if (endIdxOffset >= numEntries)
    {
        return memoryIteratorInvalid;
    }

    numEntries -= endIdxOffset;

    if ((numEntries == 1) && ((entrySize - endAlign) <= beginAlign))
    {
        return memoryIteratorInvalid;
    }

    iterator.type = MEMORY_ITERATOR_TYPE_ARRAY;
    iterator.array.pAddrArray = pAddrArray;
    iterator.array.numEntries = numEntries;
    iterator.array.entrySize = entrySize;
    iterator.array.beginAlign = beginAlign;
    iterator.array.endAlign = endAlign;
    iterator.array.curIdx = 0;

    return iterator;
}

// Internal iterator implementation for the array iterator type. Do not call directly.
static inline NvBool
_memoryArrayIteratorNext
(
    MemoryArrayIterator *pIterator,
    MemoryRange *pMemRange
)
{
    NvU64 i = pIterator->curIdx;
    NvU64 numEntries = pIterator->numEntries;

    // If we've iterated over all pages, return false
    if (i >= numEntries)
    {
        return NV_FALSE;
    }

    // Set the start and size for the current memory range
    pMemRange->start = pIterator->pAddrArray[i];
    pMemRange->size = pIterator->entrySize;
    i++;

    // Adjust the first range for any alignment offset at the beginning
    if (pIterator->curIdx == 0)
    {
        pMemRange->start += pIterator->beginAlign;
        pMemRange->size -= pIterator->beginAlign;
    }

    // Try to merge contiguous or adjacent pages into a single range
    while (i < numEntries)
    {
        // If the next entry is not contiguous at the end, break
        if (pIterator->pAddrArray[i] != mrangeLimit(*pMemRange))
        {
            break;
        }

        // Extend the range to include the next entry
        pMemRange->size += pIterator->entrySize;
        i++;
    }

    // Adjust the last range for any alignment offset at the end
    if (i == numEntries)
    {
        pMemRange->size -= pIterator->endAlign;
    }

    // Update the iterator's current index
    pIterator->curIdx = i;
    return NV_TRUE;
}

/*!
 * Get the next memory range from the iterator. Will dispatch to the appropriate iterator type.
 *
 * @param[in,out] pIterator: The iterator to get the next memory range from
 * @param[out] pMemRange: The memory range to store the next memory range in
 *
 * @return NV_TRUE if there is a next memory range, NV_FALSE otherwise
 */
static inline NvBool
memoryIteratorNext
(
    MemoryIterator *pIterator,
    MemoryRange *pMemRange
)
{
    switch (pIterator->type)
    {
        case MEMORY_ITERATOR_TYPE_ARRAY:
            return _memoryArrayIteratorNext(&pIterator->array, pMemRange);
        case MEMORY_ITERATOR_TYPE_MEMORY_AREA:
            return _memoryAreaIteratorNext(&pIterator->memoryArea, pMemRange);
        case MEMORY_ITERATOR_TYPE_INVALID:
        default:
            return NV_FALSE;
    }
}

#endif // NV_MEMORY_ITERATOR_H
