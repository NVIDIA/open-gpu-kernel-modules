/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @brief Implement PMA address tree
 *
 */

#ifndef ADDRTREE_H
#define ADDRTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "map_defines.h"

// Declare this before its definition because it refers to itself
typedef struct addrtree_node ADDRTREE_NODE;

struct addrtree_node
{
    NvU32 level;                    // The level this node belongs to
    NvU32 numChildren;              // The number of children in the children array
    NvU64 frame;                    // The first frame this node holds
    NvU64 state[PMA_BITS_PER_PAGE];          // Tracks the actual state for each map
    NvU64 seeChild[PMA_BITS_PER_PAGE];       // Whether this node is partially allocated
                                    //   If it is partially allocated, we must go to the children
                                    //   to find the correct information.

    ADDRTREE_NODE *parent;          // The node's parent
    ADDRTREE_NODE *children;        // Pointer to an array of children
};

typedef struct addrtree_level
{
    NvU64 nodeCount;                // Count of total number of nodes on this level
    ADDRTREE_NODE *pNodeList;       // Pointer to the start of the list of nodes on this level
    NvU32 pageSizeShift;            // Page size this level is tracking
    NvU32 maxFramesPerNode;         // The max number of this level frames per node
} ADDRTREE_LEVEL;

typedef struct pma_addrtree
{
    NvU64           totalFrames;    // Total number of 64KB frames being tracked
    NvU32           levelCount;     // Number of levels in this tree
    ADDRTREE_LEVEL *levels;         // List of levels in the tree
    ADDRTREE_NODE  *root;           // Start of the node list
    NvU64           numPaddingFrames; // Number of 64KB frames needed for padding for alignment

    NvU64 frameEvictionsInProcess;  // Count of frame evictions in-process
    PMA_STATS      *pPmaStats;      // Point back to the public struct in PMA structure
    NvBool          bProtected;     // The memory segment tracked by this tree is protected (VPR/CPR)
} PMA_ADDRTREE;

/*!
 * @brief Initializes the addrtree for PMA uses
 *
 * Allocates the address tree structure for all the pages being managed in this tree.
 * Address Tree implementation will use a default configuration for its own level
 * structures.
 *
 * @param[in] numPages    The number of pages being managed in this tree
 * @param[in] addrBase    The base address of this region. Required for addrtree alignment
 * @param[in] pPmaStats   Pointer to the PMA-wide stats structure
 * @param[in] bProtected  The tree tracks pages in protected memory
 *
 * @return PMA_ADDRTREE Pointer to the addrtree if succeeded, NULL otherwise
 */
void *pmaAddrtreeInit(NvU64 numFrames, NvU64 addrBase, PMA_STATS *pPmaStats, NvBool bProtected);

/*!
 * @brief Destroys the addrtree and free the memory
 *
 * @param[in] pMap  The addrtree to destroy
 *
 * @return void
 */
void pmaAddrtreeDestroy(void *pMap);

/*!
 * @brief Get/set number of evicting frames
 * Used for sanity checking in PMA layer as well as performance optimization
 * for the map layer to scan faster.
 */
NvU64 pmaAddrtreeGetEvictingFrames(void *pMap);
void pmaAddrtreeSetEvictingFrames(void *pMap, NvU64 frameEvictionsInProcess);


/*!
 * @brief Scans the addrtree for contiguous space that has the certain status.
 *
 * @param[in]   pMap            The addrtree to be scanned
 * @param[in]   addrBase        The base address of this region
 * @param[in]   rangeStart      The start of the restricted range
 * @param[in]   rangeEnd        The end of the restricted range
 * @param[in]   numPages        The number of pages we are scanning for
 * @param[out]  freeList        A list of free frame numbers -- contains only 1 element
 * @param[in]   pageSize        Size of one page
 * @param[in]   alignment       Alignment requested by client
 * @param[out]  pagesAllocated  Number of pages this call allocated
 * @param[in]   bSkipEvict      Whether it's ok to skip the scan for evictable pages
 *
 * @return NV_OK if succeeded
 * @return NV_ERR_IN_USE if found pages that can be evicted
 * @return NV_ERR_NO_MEMORY if no available pages could be found
 */
NV_STATUS pmaAddrtreeScanContiguous(
    void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
    NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
    NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);

NV_STATUS pmaAddrtreeScanDiscontiguous(
    void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
    NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment,
    NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);

void pmaAddrtreePrintTree(void *pMap, const char* str);


/*!
 * @brief Changes the state & attrib bits specified by mask
 *
 * Changes the state of the bits given the physical frame number
 * TODO: all four interfaces need to be merged from PMA level so we can remove them!
 *
 * @param[in]   pMap         The addrtree to change
 * @param[in]   frameNum     The frame number to change
 * @param[in]   newState     The new state to change to
 * @param[in]   newStateMask Specific bits to write
 *
 * @return void
 */
void pmaAddrtreeChangeStateAttribEx(void *pMap, NvU64 frameNum, PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);
void pmaAddrtreeChangePageStateAttribEx(void * pMap, NvU64 startFrame, NvU64 pageSize,
                                        PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);

/*!
 * @brief Changes the state & attrib bits specified by mask
 *
 * Changes the state of the bits given the physical frame number
 * and the number of frames to change
 *
 * @param[in]   pMap         The addrtree to change
 * @param[in]   frameNum     The frame number to change
 * @param[in]   numFrames    The number of frames to change
 * @param[in]   newState     The new state to change to
 * @param[in]   newStateMask Specific bits to write
 *
 * @return void
 */
void pmaAddrtreeChangeBlockStateAttrib(void *pMap, NvU64 frameNum,
                                     NvU64 numFrames,
                                     PMA_PAGESTATUS newState,
                                     PMA_PAGESTATUS newStateMask);

/*!
 * @brief Read the page state & attrib bits
 *
 * Read the state of the page given the physical frame number
 *
 * @param[in]   pMap        The addrtree to read
 * @param[in]   frameNum    The frame number to read
 * @param[in]   readAttrib  Read attribute bits as well
 *
 * @return PAGESTATUS of the frame
 */
PMA_PAGESTATUS pmaAddrtreeRead(void *pMap, NvU64 frameNum, NvBool readAttrib);


/*!
 * @brief Gets the total size of specified PMA managed region.
 *
 * Gets the total size of current PMA managed region in the FB.
 *
 * @param[in]  pMap         Pointer to the addrtree for the region
 * @param[in]  pBytesTotal  Pointer that will return total bytes for current region.
 *
 */
void pmaAddrtreeGetSize(void *pMap, NvU64 *pBytesTotal);


/*!
 * @brief Gets the size of the maximum free chunk of memory in specified region.
 *
 * Gets the size of the maximum free chunk of memory in the specified PMA managed
 * region of the FB.
 *
 * @param[in]  pMap         Pointer to the addrtree for the region
 * @param[in]  pLargestFree Pointer that will return largest free in current region.
 *
 */
void pmaAddrtreeGetLargestFree(void *pMap, NvU64 *pLargestFree);

/*!
 * @brief Returns the address range that is completely available for eviction.
 *        - Should be ALLOC_UNPIN.
 * In NUMA, OS manages memory and PMA will only track allocated memory in ALLOC_PIN
 * and ALLOC_UNPIN state. FREE memory is managed by OS and cannot be tracked by PMA
 * and hence PMA cannot consider FREE memory for eviction and can only consider frames
 * in known state to PMA or eviction. ALLOC_PIN cannot be evicted and hence only ALLOC_UNPIN
 * can be evictable.
 *
 *
 * @param[in]  pMap         Pointer to the regmap for the region
 * @param[in]  addrBase     Base address of the region
 * @param[in]  actualSize   Size of the eviction range
 * @param[in]  pageSize     Pagesize
 * @param[out] evictStart   Starting address of the eviction range
 * @param[out] evictEnd     End address of the eviction range.
 *
 * Returns:
 *  -  NV_OK If there is evictable range of given size : actualSize
 *
 *  -  NV_ERR_NO_MEMORY if no contiguous range is evictable.
 */
NV_STATUS pmaAddrtreeScanContiguousNumaEviction(void *pMap, NvU64 addrBase,
    NvLength actualSize, NvU64 pageSize, NvU64 *evictStart, NvU64 *evictEnd);

#ifdef __cplusplus
}
#endif

#endif // ADDRTREE_H
