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

#include "intervaltree/intervaltree.h"

/*!
 * @brief Initialize an interval tree
 *
 * @param[in] pTree      Pointer to the interval tree to initialize
 * @param[in] pAllocator Memory allocator to use
 */
void
intervaltreeInit
(
    IntervalTree *pTree,
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    mapInitIntrusive(&pTree->offsetMap);
    mapInitIntrusive(&pTree->sizeMap);
    pTree->pAllocator = pAllocator;
}

/*!
 * @brief Get the size of a node
 *
 * @param[in] pTree Pointer to the interval tree
 * @param[in] pNode Pointer to the node
 *
 * @return Size of the node
 */
static inline NvU64
_intervaltreeNodeGetSize
(
    IntervalTree *pTree,
    IntervalTreeNode *pNode
)
{
    // If no previous node, use size from map node. Otherwise use the size union member.
    return pNode->pPrev == NULL ? mapKey(&pTree->sizeMap, pNode) : pNode->size;
}

/*!
 * @brief Get the range of a node
 *
 * @param[in] pTree Pointer to the interval tree
 * @param[in] pNode Pointer to the node
 *
 * @return Memory range of the node
 */
static inline MemoryRange
_intervaltreeNodeGetRange
(
    IntervalTree *pTree,
    IntervalTreeNode *pNode
)
{
    NvU64 size = _intervaltreeNodeGetSize(pTree, pNode);
    NvU64 offset = mapKey(&pTree->offsetMap, pNode);
    return mrangeMake(offset, size);
}

/*!
 * @brief Remove a node from the size map
 *
 * @param[in] pTree Pointer to the interval tree
 * @param[in] pNode Pointer to the node to remove
 */
static inline void
_intervaltreeNodeSizeRemove
(
    IntervalTree *pTree,
    IntervalTreeNode *pNode
)
{
    NvU64 size = _intervaltreeNodeGetSize(pTree, pNode);
    
    // Update the linked list pointers to remove this node
    if (pNode->pNext != NULL)
    {
        pNode->pNext->pPrev = pNode->pPrev;
    }
    if (pNode->pPrev != NULL)
    {
        pNode->pPrev->pNext = pNode->pNext;
        // Node was in the middle of the list, no need to update map
        return; 
    }
    
    // Node was the first in the size bucket, remove it from the map
    mapRemove(&pTree->sizeMap, pNode);
    
    // If there are other nodes with the same size, make the next one the map entry
    if (pNode->pNext != NULL)
    {
        mapInsertExisting(&pTree->sizeMap, size, pNode->pNext);
    }
}

/*!
 * @brief Insert a node into the size map
 *
 * @param[in] pTree Pointer to the interval tree
 * @param[in] pNode Pointer to the node to insert
 * @param[in] size  Size of the node
 */
static inline void
_intervaltreeNodeSizeInsert
(
    IntervalTree *pTree,
    IntervalTreeNode *pNode,
    NvU64 size
)
{
    IntervalTreeNode *pFirstSize = mapFind(&pTree->sizeMap, size);

    if (pFirstSize == NULL)
    {
        // If there is no node with this size, insert it as the map entry
        pNode->pNext = NULL;
        pNode->pPrev = NULL;
        mapInsertExisting(&pTree->sizeMap, size, pNode);
    }
    else
    {
        // If there are existing nodes with this size, insert after the first one
        // This maintains the linked list structure for nodes with the same size
        pNode->pNext = pFirstSize->pNext;
        pNode->pPrev = pFirstSize;
        pFirstSize->pNext = pNode;
        // Store size in the node for linked list traversal
        pNode->size = size; 
        if (pNode->pNext != NULL)
        {
            pNode->pNext->pPrev = pNode;
        }
    }
}

/*!
 * @brief Find a memory range containing the given offset
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] offset Offset to search for
 *
 * @return Memory range containing the offset, or empty range if not found
 */
MemoryRange
intervaltreeFindOffset
(
    IntervalTree *pTree,
    NvU64 offset
)
{
    // Find the node with the largest start offset that is <= the target offset
    IntervalTreeNode *pNode = mapFindLEQ(&pTree->offsetMap, offset);
    MemoryRange range;

    if (pNode == NULL)
    {
        return mrangeMake(0, 0);
    }

    range = _intervaltreeNodeGetRange(pTree, pNode);
    // Check if the offset falls within this range (start <= offset < start + size)
    if (mrangeLimit(range) <= offset)
    {
        return mrangeMake(0, 0);
    }
    return range;
}

/*!
 * @brief Find a memory range with size greater than or equal to the given size
 *
 * @param[in] pTree   Pointer to the interval tree
 * @param[in] size    Minimum size to search for
 *
 * @return Memory range with sufficient size, or empty range if not found
 */
MemoryRange
intervaltreeFindGESize
(
    IntervalTree *pTree,
    NvU64 size,
    NvU64 baseAlignment
)
{
    // Find the first node with size >= requested size
    IntervalTreeNode *pNode = mapFindGEQ(&pTree->sizeMap, size);
    while (pNode != NULL)
    {
        MemoryRange range = _intervaltreeNodeGetRange(pTree, pNode);
        
        // Align the start address upward to meet alignment requirements
        NvU64 newStart = NV_ALIGN_UP64(range.start, baseAlignment);
        NvU64 newSize = mrangeLimit(range) - newStart;
        
        // Check if the aligned range still has sufficient size
        if (newSize >= size && mrangeLimit(range) >= newStart)
        {
            return mrangeMake(newStart, size);
        }
        
        // Try the next node with larger size
        pNode = mapNext(&pTree->sizeMap, pNode);
    }
    return mrangeMake(0, 0);
}

/*!
 * @brief Insert a memory range into the interval tree
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] range  Memory range to insert
 *
 * @return NV_OK on success, error code on failure
 */
NV_STATUS
intervaltreeInsert
(
    IntervalTree *pTree,
    MemoryRange range
)
{
    // Find the node that starts before or at our range start
    IntervalTreeNode *pPrevNode = mapFindLEQ(&pTree->offsetMap, mrangeLimit(range) - 1llu);
    // Find the node that starts at or after our range end
    IntervalTreeNode *pNextNode = mapFindGEQ(&pTree->offsetMap, range.start);
    IntervalTreeNode *pNewNode = NULL;

    if (pPrevNode != NULL)
    {
        MemoryRange prevRange = _intervaltreeNodeGetRange(pTree, pPrevNode);

        // Check for overlap with the previous range
        if (mrangeLimit(prevRange) > range.start)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        // If ranges are contiguous (previous ends exactly where this starts), merge them
        if (mrangeLimit(prevRange) == range.start)
        {
            // Remove the previous node from size map and offset map
            _intervaltreeNodeSizeRemove(pTree, pPrevNode);
            mapRemove(&pTree->offsetMap, pPrevNode);
            PORT_FREE(pTree->pAllocator, pPrevNode);
            
            // Extend our range to include the previous range
            range.start = prevRange.start;
            range.size += prevRange.size;
        }
    }

    if (pNextNode != NULL)
    {
        MemoryRange nextRange = _intervaltreeNodeGetRange(pTree, pNextNode);

        // Check for overlap with the next range
        if (nextRange.start < mrangeLimit(range))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        // If ranges are contiguous (this ends exactly where next starts), merge them
        if (mrangeLimit(range) == nextRange.start)
        {
            // Remove the next node from size map and offset map
            _intervaltreeNodeSizeRemove(pTree, pNextNode);
            mapRemove(&pTree->offsetMap, pNextNode);
            PORT_FREE(pTree->pAllocator, pNextNode);
            
            // Extend our range to include the next range
            range.size += nextRange.size;
        }
    }

    // Allocate and initialize the new node
    pNewNode = PORT_ALLOC(pTree->pAllocator, sizeof(IntervalTreeNode));
    if (pNewNode == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    // Update the total range tracking
    if (pTree->totalRange.size == 0)
    {
        // First range in the tree
        pTree->totalRange = range;
    }
    else if (range.start < pTree->totalRange.start)
    {
        // New range extends the total range to the left
        pTree->totalRange.size += pTree->totalRange.start - range.start;
        pTree->totalRange.start = range.start;
    }
    else if (mrangeLimit(range) > mrangeLimit(pTree->totalRange))
    {
        // New range extends the total range to the right
        pTree->totalRange.size = mrangeLimit(range) - pTree->totalRange.start;
    }
    
    // Insert the node into both maps
    mapInsertExisting(&pTree->offsetMap, range.start, pNewNode);
    _intervaltreeNodeSizeInsert(pTree, pNewNode, range.size);
    return NV_OK;
}

/*!
 * @brief Remove a memory range from the interval tree
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] range  Memory range to remove
 *
 * @return NV_OK on success, error code on failure
 */
NV_STATUS
intervaltreeRemove
(
    IntervalTree *pTree,
    MemoryRange range
)
{
    // Find the node that contains the start of the range to remove
    IntervalTreeNode *pNode = mapFindLEQ(&pTree->offsetMap, range.start);
    MemoryRange nodeRange;

    if (pNode == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    nodeRange = _intervaltreeNodeGetRange(pTree, pNode);
    // Verify that the range to remove is completely contained within the node
    if (!mrangeContains(nodeRange, range))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (range.start == nodeRange.start && range.size == nodeRange.size)
    {
        // Exact match: remove the entire node
        _intervaltreeNodeSizeRemove(pTree, pNode);
        mapRemove(&pTree->offsetMap, pNode);
        PORT_FREE(pTree->pAllocator, pNode);
    }
    else if (range.start == nodeRange.start && mrangeLimit(range) < mrangeLimit(nodeRange))
    {
        // Remove from the beginning: adjust the node to start after the removed range
        nodeRange.size = mrangeLimit(nodeRange) - mrangeLimit(range);
        nodeRange.start = mrangeLimit(range);
        
        // Update the node's position in the offset map and size map
        _intervaltreeNodeSizeRemove(pTree, pNode);
        mapRemove(&pTree->offsetMap, pNode);
        mapInsertExisting(&pTree->offsetMap, nodeRange.start, pNode);
        _intervaltreeNodeSizeInsert(pTree, pNode, nodeRange.size);
    }
    else if (range.start > nodeRange.start && mrangeLimit(range) == mrangeLimit(nodeRange))
    {
        // Remove from the end: reduce the node's size
        nodeRange.size -= nodeRange.start - range.start;
        
        // Update the node's size in the size map
        _intervaltreeNodeSizeRemove(pTree, pNode);
        _intervaltreeNodeSizeInsert(pTree, pNode, nodeRange.size);
    }
    else
    {
        // Remove from the middle: split the node into two parts
        // Create a new node for the right portion
        IntervalTreeNode *pSplitNode = PORT_ALLOC(pTree->pAllocator, sizeof(IntervalTreeNode));
        if (pSplitNode == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }
        
        // Insert the right portion (after the removed range)
        mapInsertExisting(&pTree->offsetMap, mrangeLimit(range), pSplitNode);
        _intervaltreeNodeSizeInsert(pTree, pSplitNode, mrangeLimit(nodeRange) - mrangeLimit(range));
        
        // Adjust the original node to cover the left portion (before the removed range)
        _intervaltreeNodeSizeRemove(pTree, pNode);
        _intervaltreeNodeSizeInsert(pTree, pNode, range.start - nodeRange.start);
    }
    
    // Update the total range tracking when removing ranges
    if(mrangeLimit(pTree->totalRange) == mrangeLimit(range))
    {
        // If we're removing the rightmost range, recalculate total range
        IntervalTreeNode *pNode = mapFindLEQ(&pTree->offsetMap, NV_U64_MAX);
        pTree->totalRange.size = 0;
        if(pNode != NULL)
        {
            MemoryRange newRange = _intervaltreeNodeGetRange(pTree, pNode);
            pTree->totalRange.size = mrangeLimit(newRange) - pTree->totalRange.start;
        }
    }
    else if(range.start == pTree->totalRange.start)
    {
        // If we're removing the leftmost range, recalculate total range
        IntervalTreeNode *pNode = mapFindGEQ(&pTree->offsetMap, 0);
        NvU64 oldLimit = mrangeLimit(pTree->totalRange);
        pTree->totalRange.size = 0;
        if(pNode != NULL)
        {
            MemoryRange newRange = _intervaltreeNodeGetRange(pTree, pNode);
            pTree->totalRange.size = oldLimit - newRange.start;
            pTree->totalRange.start = newRange.start;
        }
    }
    return NV_OK;
}

/*!
 * @brief Find the maximum size node in the interval tree
 *
 * @param[in] pTree Pointer to the interval tree
 *
 * @return Memory range of the maximum size node
 */
MemoryRange
intervaltreeFindMaxSize
(
    IntervalTree *pTree
)
{
    // Find the last node in the size map (largest size)
    IntervalTreeNode *pNode = mapFindLEQ(&pTree->sizeMap, NV_U64_MAX);
    if (pNode == NULL)
    {
        return mrangeMake(0, 0);
    }
    return _intervaltreeNodeGetRange(pTree, pNode);
}

/*!
 * @brief Find the minimum size node in the interval tree
 *
 * @param[in] pTree Pointer to the interval tree
 *
 * @return Memory range of the minimum size node
 */
MemoryRange
intervaltreeFindMinSize
(
    IntervalTree *pTree
)
{
    // Find the first node in the size map (smallest size)
    IntervalTreeNode *pNode = mapFindGEQ(&pTree->sizeMap, 0);
    if (pNode == NULL)
    {
        return mrangeMake(0, 0);
    }
    return _intervaltreeNodeGetRange(pTree, pNode);
}

/*!
 * @brief Allocate a memory range from the interval tree
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] size   Size of the memory range to allocate
 * @param[in] baseAlignment Base alignment of the memory range
 * @param[out] pRange Pointer to the memory range to store the allocated memory
 *
 * @return NV_OK on success, error code on failure
 */
NV_STATUS
intervaltreeAllocate
(
    IntervalTree *pTree,
    NvU64 size,
    NvU64 baseAlignment,
    MemoryRange *pRange
)
{
    // Find a range that's large enough and meets alignment requirements
    MemoryRange range = intervaltreeFindGESize(pTree, size, baseAlignment);
    if(range.size == 0)
    {
        return NV_ERR_NO_MEMORY;
    }
    
    // Store the allocated range and remove it from the tree
    *pRange = range;
    return intervaltreeRemove(pTree, range);
}

/*!
 * @brief Allocate a memory range from a boundary range
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] boundaryRange  Boundary range to allocate from
 * @param[in] size   Size of the memory range to allocate
 * @param[in] baseAlignment Base alignment of the memory range
 * @param[out] pRange Pointer to the memory range to store the allocated memory
 *
 * @return NV_OK on success, error code on failure
 */ 
NV_STATUS
intervaltreeAllocateFromRange
(
    IntervalTree *pTree,
    MemoryRange boundaryRange,
    NvU64 size,
    NvU64 baseAlignment,
    MemoryRange *pRange
)
{
    IntervalTreeNode *pNode = NULL;
    
    // If the boundary range completely contains the tree's total range, use normal allocation
    if (mrangeContains(boundaryRange, pTree->totalRange))
    {
        return intervaltreeAllocate(pTree, size, baseAlignment, pRange);
    }
    
    // Find the first node that overlaps with or starts after the boundary range
    pNode = mapFindLEQ(&pTree->offsetMap, boundaryRange.start);
    if (pNode == NULL)
    {
        pNode = mapFindGEQ(&pTree->offsetMap, boundaryRange.start);
    }
    
    // Iterate through nodes that overlap with the boundary range
    while(pNode != NULL)
    {
        MemoryRange nodeRange = _intervaltreeNodeGetRange(pTree, pNode);
        NvU64 newStart = 0;
        
        // Stop if we've moved past the boundary range
        if (nodeRange.start >= mrangeLimit(boundaryRange))
        {
            break;
        }
        
        // Adjust the node range to fit within the boundary range
        if (nodeRange.start < boundaryRange.start)
        {
            // Clip the left side to the boundary start
            NvU64 diff = boundaryRange.start - nodeRange.start;
            nodeRange.start += diff;
            nodeRange.size -= diff;
        }
        if (mrangeLimit(nodeRange) > mrangeLimit(boundaryRange))
        {
            // Clip the right side to the boundary end
            nodeRange.size = mrangeLimit(boundaryRange) - nodeRange.start;
        }
        
        // Align the start address upward to meet alignment requirements
        newStart = NV_ALIGN_UP64(nodeRange.start, baseAlignment);

        if (newStart < mrangeLimit(nodeRange))
        {
            // If alignment adjustment is valid, adjust the node range
            nodeRange.size -= newStart - nodeRange.start;
            nodeRange.start = newStart;

            // Check if the adjusted range is large enough for allocation
            if(nodeRange.size >= size)
            {
                // Allocate exactly the requested size and remove it from the tree
                nodeRange.size = size;
                *pRange = nodeRange;
                return intervaltreeRemove(pTree, nodeRange);
            }
        }
        
        // Try the next node
        pNode = mapNext(&pTree->offsetMap, pNode);
    }
    return NV_ERR_NO_MEMORY;
}

/*!
 * @brief Free a memory range to the interval tree
 *
 * @param[in] pTree  Pointer to the interval tree
 * @param[in] range  Memory range to free
 *
 * @return NV_OK on success, error code on failure
 */
NV_STATUS
intervaltreeFree
(
    IntervalTree *pTree,
    MemoryRange range
)
{
    // Freeing is just inserting the range back into the tree
    return intervaltreeInsert(pTree, range);
}

/*!
 * @brief Destroy an interval tree
 *
 * @param[in] pTree Pointer to the interval tree
 */
void
intervaltreeDestroy
(
    IntervalTree *pTree
)
{
    // Free all nodes in the offset map
    IntervalTreeNode *pNode = mapFindGEQ(&pTree->offsetMap, 0);
    while (pNode != NULL)
    {
        IntervalTreeNode *pNext = mapNext(&pTree->offsetMap, pNode);
        PORT_FREE(pTree->pAllocator, pNode);
        pNode = pNext;
    }
    portMemSet(pTree, 0, sizeof(IntervalTree));
}
