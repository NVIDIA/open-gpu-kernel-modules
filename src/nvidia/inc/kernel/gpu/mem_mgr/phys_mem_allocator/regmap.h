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

/*!
 *  @brief A bit map to keep track of FB frames
 *
 *  @bug Return status needs to conform to NV_STATUS
 *          -- take a reference as output and always return NV_STATUS
 *  @bug Is enum a good choice for this?
 *
 */

#ifndef REGMAP_H
#define REGMAP_H

#include "map_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Store the type here because we might use different algorithms for
// different types of memory scan
//
typedef struct pma_regmap
{
    NvU64 totalFrames;                /* Total number of frames */
    NvU64 mapLength;                  /* Length of the map */
    NvU64 *map[PMA_BITS_PER_PAGE];    /* The bit map */
    NvU64 frameEvictionsInProcess;    /* Count of frame evictions in-process */
    PMA_STATS *pPmaStats;             /* Point back to the public struct in PMA structure */
    NvBool bProtected;                /* The memory segment tracked by this regmap is protected (VPR/CPR) */
} PMA_REGMAP;

void pmaRegmapPrint(PMA_REGMAP *pMap);


/*!
 * @brief Initializes the regmap for PMA uses
 *
 * Allocates a bit map for all the pages being managed in this map. All bits
 * are initialized to be FREE.
 *
 * @param[in]   numPages    The number of pages being managed in this map
 * @param[in]   addrBase    The base address of this region. Unused in regmap
 * @param[in]   pPmaPubStats The PMA stat struct to update
 * @param[in]   bProtected   The map tracks pages in protected memory
 *
 * @return PMA_REGMAP Pointer to the regmap if succeeded, NULL otherwise
 */
void *pmaRegmapInit(NvU64 numPages, NvU64 addrBase, PMA_STATS *pPmaStats, NvBool bProtected);


/*!
 * @brief Destroys the region map and free the memory
 *
 * @param[in]   pMap         The regmap to destroy
 *
 * @return void
 */
void pmaRegmapDestroy(void *pMap);

/*!
 * @brief Get/set number of evicting frames
 * Used for sanity checking in PMA layer as well as performance optimization
 * for the map layer to scan faster.
 */
NvU64 pmaRegmapGetEvictingFrames(void *pMap);
void pmaRegmapSetEvictingFrames(void *pMap, NvU64 frameEvictionsInProcess);

/*!
 * @brief Changes the recorded state & attrib bits for an entire page
 *
 * Changes the state of the bits for an entire page given the physical
 * frame number and the page size
 *
 * @param[in]   pMap          The regmap to change
 * @param[in]   frameNumStart The frame number to change
 * @param[in]   pageSize      The page size of the page to change
 * @param[in]   newState      The new state to change to
 * @param[in]   writeAttrib   Write attribute bits as well
 *
 * @return void
 */
void pmaRegmapChangePageStateAttrib(void * pMap, NvU64 startFrame, NvU64 pageSize,
                                    PMA_PAGESTATUS newState, PMA_PAGESTATUS newStateMask);

/*!
 * @brief Changes the state & attrib bits specified by mask
 *
 * Changes the state of the bits given the physical frame number
 *
 * @param[in]   pMap         The regmap to change
 * @param[in]   frameNum     The frame number to change
 * @param[in]   newState     The new state to change to
 * @param[in]   newStateMask Specific bits to write
 *
 * @return void
 */
void pmaRegmapChangeStateAttrib(void *pMap, NvU64 frameNum,
                                PMA_PAGESTATUS newState,
                                PMA_PAGESTATUS newStateMask);

/*!
 * @brief Changes the state & attrib bits specified by mask
 *
 * Changes the state of the bits given the physical frame number
 * and the number of frames to change
 *
 * @param[in]   pMap         The regmap to change
 * @param[in]   frameNum     The frame number to change
 * @param[in]   numFrames    The number of frames to change
 * @param[in]   newState     The new state to change to
 * @param[in]   newStateMask Specific bits to write
 *
 * @return void
 */
void pmaRegmapChangeBlockStateAttrib(void *pMap, NvU64 frameNum,
                                     NvU64 numFrames,
                                     PMA_PAGESTATUS newState,
                                     PMA_PAGESTATUS newStateMask);

/*!
 * @brief Read the page state & attrib bits
 *
 * Read the state of the page given the physical frame number
 *
 * @param[in]   pMap        The regmap to read
 * @param[in]   frameNum    The frame number to read
 * @param[in]   readAttrib  Read attribute bits as well
 *
 * @return PAGESTATUS of the frame
 */
PMA_PAGESTATUS pmaRegmapRead(void *pMap, NvU64 frameNum, NvBool readAttrib);


/*!
 * @brief Scan the list for contiguous space
 *
 * Scans the regmap for contiguous space that has the certain status. This
 * function is optimized for performance if PMA_BITS_PER_PAGE is 2. It uses
 * trailing-zero algorithm to determine which frame(s) has different status.
 *
 * @param[in]   pMap            The regmap to be scanned
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
NV_STATUS pmaRegmapScanContiguous(
    void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
    NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment, NvU64 stride, NvU32 strideStart,
    NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);

/*!
 * @brief Scan the list for any space
 *
 * Scans the regmap for numFrames number of frames, possibly scattered across
 * the regmap. This function will allocate contiguous space whenever possible.
 * Not optimized for performance yet.
 *
 * @param[in]   pMap            The regmap to be scanned
 * @param[in]   addrBase        The base address of this region
 * @param[in]   rangeStart      The start of the restricted range
 * @param[in]   rangeEnd        The end of the restricted range
 * @param[in]   numFrames       The number of frames we are scanning for
 * @param[out]  freeList        A list of free frame numbers (allocated by client)
 * @param[in]   pageSize        Size of one page
 * @param[in]   alignment       Alignment requested by client. Has to be equal to the pageSize.
 * @param[out]  pagesAllocated  Number of pages this call allocated
 * @param[in]   bSkipEvict      Whether it's ok to skip the scan for evictable pages
 *
 * @return NV_OK if succeeded
 * @return NV_ERR_IN_USE if found pages that can be evicted
 * @return NV_ERR_NO_MEMORY if no available pages could be found
 */
NV_STATUS pmaRegmapScanDiscontiguous(
    void *pMap, NvU64 addrBase, NvU64 rangeStart, NvU64 rangeEnd,
    NvU64 numPages, NvU64 *freelist, NvU64 pageSize, NvU64 alignment, NvU64 stride, NvU32 strideStart, 
    NvU64 *pagesAllocated, NvBool bSkipEvict, NvBool bReverseAlloc);

/*!
 * @brief Gets the total size of specified PMA managed region.
 *
 * Gets the total size of current PMA managed region in the FB.
 *
 * @param[in]  pMap         Pointer to the regmap for the region
 * @param[in]  pBytesTotal  Pointer that will return total bytes for current region.
 *
 */
void pmaRegmapGetSize(void *pMap, NvU64 *pBytesTotal);


/*!
 * @brief Gets the size of the maximum free chunk of memory in specified region.
 *
 * Gets the size of the maximum free chunk of memory in the specified PMA managed
 * region of the FB.
 *
 * @param[in]  pMap         Pointer to the regmap for the region
 * @param[in]  pLargestFree Pointer that will return largest free in current region.
 *
 */
void pmaRegmapGetLargestFree(void *pMap, NvU64 *pLargestFree);


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
NV_STATUS pmaRegMapScanContiguousNumaEviction(void *pMap, NvU64 addrBase,
    NvLength actualSize, NvU64 pageSize, NvU64 *evictStart, NvU64 *evictEnd);

#ifdef __cplusplus
}
#endif

#endif // REGMAP_H
