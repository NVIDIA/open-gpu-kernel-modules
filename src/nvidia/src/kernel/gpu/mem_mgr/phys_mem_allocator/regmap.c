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
 * @file regmap.c
 * @brief A bit map to keep track of FB frames
 *
 * @bug None
 */

#include "gpu/mem_mgr/phys_mem_allocator/regmap.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator_util.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"
#include "nvport/nvport.h"
#include "nvmisc.h"

#define _UINT_SIZE 64
#define _UINT_SHIFT 6

#define PAGE_BITIDX(n)              ((n) & (_UINT_SIZE - 1))
#define PAGE_MAPIDX(n)              ((n) >> _UINT_SHIFT)
#define MAKE_BITMASK(n)             ((NvU64)0x1 << (n))

#define SETBITS(bits, mask, newVal) ((bits & (~mask)) | (mask & newVal))

//////////////// DEBUG ///////////////

void
pmaRegmapPrint(PMA_REGMAP *pMap)
{
    NvU32 i, j;

    NV_ASSERT(pMap != NULL);
    NV_ASSERT(pMap->map != NULL);

    for (j = 0; j < PMA_BITS_PER_PAGE; j++)
    {
        NV_PRINTF(LEVEL_INFO, "*** %d-th MAP ***\n", j);
        for (i = 0; i < pMap->mapLength; i+=4)
        {
            NV_PRINTF(LEVEL_INFO, "map[%d]: %llx ", i, (pMap->map)[j][i]);
            NV_PRINTF(LEVEL_INFO, "map[%d]: %llx ", i+1, (i+1 > pMap->mapLength) ? 0 : (pMap->map)[j][i+1]);
            NV_PRINTF(LEVEL_INFO, "map[%d]: %llx ", i+2, (i+2 > pMap->mapLength) ? 0 : (pMap->map)[j][i+2]);
            NV_PRINTF(LEVEL_INFO, "map[%d]: %llx \n", i+3, (i+3 > pMap->mapLength) ? 0 : (pMap->map)[j][i+3]);
        }
    }
}

// Returns the longest string of zeros.
static NvU32
maxZerosGet(NvU64 bits)
{
    NvU32 y = 0;

    bits = ~bits;
    while (bits != 0)
    {
        bits = bits & (bits << 1);
        y++;
    }
    return y;
}

static NvS64
_checkOne(NvU64 *bits, NvU64 start, NvU64 end)
{
    NvS32 firstSetBit;
    NvU64 startMapIdx, startBitIdx, endMapIdx, endBitIdx, mapIdx;
    NvU64 startMask, endMask, handle;

    startMapIdx = PAGE_MAPIDX(start);
    startBitIdx = PAGE_BITIDX(start);
    endMapIdx   = PAGE_MAPIDX(end);
    endBitIdx   = PAGE_BITIDX(end);

    if (startMapIdx < endMapIdx)
    {
        //
        // Ensure the intermediate bitmaps are all good. We"ll handle end
        // bitmaps later.
        //
        for (mapIdx = startMapIdx + 1; mapIdx <= (endMapIdx - 1); mapIdx++)
        {
            if (bits[mapIdx] != 0)
            {
                firstSetBit = portUtilCountTrailingZeros64(bits[mapIdx]);
                return ((mapIdx << _UINT_SHIFT) + firstSetBit);
            }
        }

        // handle edge case
        endMask = (NV_U64_MAX >> (_UINT_SIZE - endBitIdx - 1));

        if ((bits[endMapIdx] & endMask) == 0)
        {
            startMask = (NV_U64_MAX << startBitIdx);

            if ((bits[startMapIdx] & startMask) == 0)
            {
                return -1;
            }
            else
            {
                handle = (bits[startMapIdx] & startMask);
                firstSetBit = portUtilCountTrailingZeros64(handle);
                NV_ASSERT((NvU64)firstSetBit >= startBitIdx);
                return (start - startBitIdx + firstSetBit);
            }
        }
        else
        {
            handle = bits[endMapIdx] & endMask;
            firstSetBit = portUtilCountTrailingZeros64(handle);
            return (end - endBitIdx + firstSetBit);
        }
    }
    else
    {
        NV_ASSERT(startMapIdx == endMapIdx);

        startMask = (NV_U64_MAX << startBitIdx);
        endMask = (NV_U64_MAX >> (_UINT_SIZE - endBitIdx - 1));

        handle = (startMask & endMask);
        if ((handle & bits[startMapIdx]) == 0)
        {
            return -1;
        }
        else
        {
            handle = handle & bits[startMapIdx];
            firstSetBit = portUtilCountTrailingZeros64(handle);
            NV_ASSERT(firstSetBit != 64);
            return (start - startBitIdx + firstSetBit);
        }
    }
}

static NvU64 alignUpToMod(NvU64 frame, NvU64 alignment, NvU64 mod)
{
    if ((frame & (alignment - 1)) <= mod)
        return NV_ALIGN_DOWN(frame, alignment) + mod;
    else
        return NV_ALIGN_UP(frame, alignment) + mod;
}

//
// Determine if all frames in the 2MB range is not allocated
// They could be in scrubbing or eviction state.
//
static NvBool _pmaRegmapAllFree2mb(PMA_REGMAP *pRegmap, NvU64 frameNum)
{
    NvU64 baseFrame = (NV_ALIGN_DOWN((frameNum << PMA_PAGE_SHIFT), _PMA_2MB)) >> PMA_PAGE_SHIFT;
    NvU32 numFrames = _PMA_2MB >> PMA_PAGE_SHIFT;

    // Always return false if the last 2MB range is incomplete
    if ((baseFrame + numFrames) >= pRegmap->totalFrames)
    {
        return NV_FALSE;
    }

    //
    // We only care about STATE_PIN and STATE_UNPIN because:
    // Even if the page is marked as SCRUBBING for example, we should not report OOM and prevent
    // the clients from scanning the bitmap.
    //
    if (_checkOne(pRegmap->map[MAP_IDX_ALLOC_PIN], baseFrame, (baseFrame + numFrames - 1)) != -1)
    {
        return NV_FALSE;
    }

    if (_checkOne(pRegmap->map[MAP_IDX_ALLOC_UNPIN], baseFrame, (baseFrame + numFrames - 1)) != -1)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

//
// Check whether the specified frame range is available completely for eviction
//
// Returns:
//  -  -1 if the whole range is evictable
//
//  -   Index of the last unevictable frame number
//
//
static NvS64
_pmaRegmapScanNumaUnevictable
(
    PMA_REGMAP *pRegmap,
    NvU64       frameBegin,
    NvU64       frameEnd
)
{
    NvS64    unevictableFrameIndex = -1;
    NvU32    unevictableIndex = 0;
    NvU64   *unpinBitmap = pRegmap->map[MAP_IDX_ALLOC_UNPIN];
    NvU64   *evictBitmap = pRegmap->map[MAP_IDX_EVICTING];
    NvU64    mapIter;
    NvU64    startMapIdx = PAGE_MAPIDX(frameBegin);
    NvU64    startBitIdx = PAGE_BITIDX(frameBegin);
    NvU64    endMapIdx   = PAGE_MAPIDX(frameEnd);
    NvU64    endBitIdx   = PAGE_BITIDX(frameEnd);

#ifdef DEBUG_VERBOSE
    NvU64   *pinBitmap = pRegmap->map[MAP_IDX_ALLOC_PIN];
#endif

    for (mapIter = endMapIdx; mapIter >= startMapIdx; mapIter--)
    {
        NvU64    mask = NV_U64_MAX;

        if (mapIter == endMapIdx)
        {
            mask = (mask >> (_UINT_SIZE - endBitIdx - 1));
        }

        if (mapIter == startMapIdx)
        {
            mask = (mask & ~(MAKE_BITMASK(startBitIdx) - 1));
        }

#ifdef DEBUG_VERBOSE

        NV_PRINTF(LEVEL_INFO, "mapIter %llx frame %llx mask %llx unpinbitmap %llx pinbitmap %llx evictbitmap %llx",
                 mapIter, (mapIter << _UINT_SHIFT), mask, unpinBitmap[mapIter], pinBitmap[mapIter], evictBitmap[mapIter]);
#endif
        // start from the end
        if ((unpinBitmap[mapIter] & mask) == mask)
            continue; // go to previous bits


        // MSB is 0 or all bits are 0 return the next frame
        if ((unpinBitmap[mapIter] & mask) == 0)
        {
            if (mapIter == endMapIdx)
                unevictableFrameIndex = frameEnd;
            else
                unevictableFrameIndex = (mapIter << _UINT_SHIFT) +  (_UINT_SIZE - 1);
            break;
        }
#ifdef DEBUG_VERBOSE
        NV_PRINTF(LEVEL_INFO, "Check leading zero of %llx", ~(unpinBitmap[mapIter] & mask));
#endif

        unevictableIndex = _UINT_SIZE - portUtilCountLeadingZeros64((~unpinBitmap[mapIter]) & mask) - 1;
        unevictableFrameIndex = (mapIter << _UINT_SHIFT) + unevictableIndex;
        break;
    }

    if (unevictableFrameIndex == -1)
    {
#ifdef DEBUG_VERBOSE
        NV_PRINTF(LEVEL_INFO, "Evictable range found between frameBegin %llx and frameEnd %llx", frameBegin, frameEnd);
#endif

        if (pRegmap->frameEvictionsInProcess && ((unevictableFrameIndex = _checkOne(evictBitmap, frameBegin, frameEnd)) != -1))
        {
#ifdef DEBUG_VERBOSE
            NV_PRINTF(LEVEL_INFO, "The allocation range is already being evicted frameBegin %llx and frameEnd %llx", frameBegin, frameEnd);
#endif
            return unevictableFrameIndex;
        }
    }

    return unevictableFrameIndex;
}


//
// Determine a contiguous evictable range of size actualSize
//
// Returns:
//  -  NV_ERR_NO_MEMORY if eviction is not possible for this size
//
//  -  NV_OK if there is a valid contiguous evictable range
//     starting and ending at address stored at evictStart and evictEnd
//
//

NV_STATUS
pmaRegMapScanContiguousNumaEviction
(
    void    *pMap,
    NvU64    addrBase,
    NvLength actualSize,
    NvU64    pageSize,
    NvU64   *evictStart,
    NvU64   *evictEnd
)
{
    NV_STATUS status = NV_ERR_NO_MEMORY;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    NvU64 alignedAddrBase;
    NvU64 frameNum;
    NvU64 endFrame, frameStart;
    NvU64 alignment = pageSize;
    NvU64 frameAlignment, frameAlignmentPadding;
    NvU64 numFrames = actualSize >> PMA_PAGE_SHIFT;

    endFrame = pRegmap->totalFrames - 1;

    if (pRegmap->totalFrames < numFrames)
        return status;

    //Copied from pmaRegmapScanContiguous
    frameAlignment = alignment >> PMA_PAGE_SHIFT;
    alignedAddrBase = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;
    frameStart = alignUpToMod(0, frameAlignment, frameAlignmentPadding);


    for (frameNum = frameStart; frameNum <= endFrame; )
    {
        PMA_PAGESTATUS startFrameAllocState;
        PMA_PAGESTATUS endFrameAllocState;
        NvS64 firstUnevictableFrame;

        startFrameAllocState  = pmaRegmapRead(pRegmap, frameNum, NV_TRUE);
        endFrameAllocState    = pmaRegmapRead(pRegmap, frameNum + numFrames - 1, NV_TRUE);

        if ((endFrameAllocState & STATE_MASK) != STATE_UNPIN)
        {
            // end is not available jump from start to after numFrames
            frameNum += numFrames;
            frameNum = alignUpToMod(frameNum, frameAlignment, frameAlignmentPadding);
            continue;
        }

        if ((startFrameAllocState & STATE_MASK) != STATE_UNPIN)
        {
            // startFrame is unavailable, jump to next aligned frame
            frameNum += frameAlignment;
            continue;
        }


        // First occurrence of 0 in STATE_UNPIN  from frameNum to frameNum + numFrames - 1
        firstUnevictableFrame = _pmaRegmapScanNumaUnevictable(pRegmap, frameNum, frameNum + numFrames - 1);

        if (firstUnevictableFrame == -1)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Evictable frame = %lld evictstart = %llx evictEnd = %llx\n",
                      frameNum, addrBase + (frameNum << PMA_PAGE_SHIFT),
                      (addrBase + (frameNum << PMA_PAGE_SHIFT) + actualSize - 1));

            *evictStart =   addrBase   + (frameNum << PMA_PAGE_SHIFT);
            *evictEnd   =  *evictStart + actualSize - 1;
            status = NV_OK;
            break;
        }
        else
        {
            // get the next aligned frame after the unevictable frame.
            frameNum = alignUpToMod(firstUnevictableFrame + 1, frameAlignment, frameAlignmentPadding);
        }
    }

    return status;
}
//
// Check whether the specified frame range is available for allocation or
// eviction.
//
// Returns:
//  - NV_OK if the whole range is available and leaves frameIndex unset.
//
//  - NV_ERR_IN_USE if some frames would need to be evicted, and sets frameIndex
//    to the first one.
//
//  - NV_ERR_NO_MEMORY if some frames are unavailable, and sets frameIndex to
//    the first one.
//
// TODO: Would it be better to return the last frame index instead, given how the
// search skips over right past it?
//
static NV_STATUS
_pmaRegmapStatus(PMA_REGMAP *pRegmap, NvU64 start, NvU64 end, NvU64 *frameIndex)
{
    NvS64 diff;

    if ((diff = _checkOne(pRegmap->map[MAP_IDX_ALLOC_PIN], start, end)) != -1)
    {
        *frameIndex = diff;
        return NV_ERR_NO_MEMORY;
    }

    if (pRegmap->frameEvictionsInProcess > 0)
    {
        //
        // Pages that are being evicted may be in the free state so we need to
        // check for eviction on all frames as long as any eviction is happening
        // in the region.
        //
        if ((diff = _checkOne(pRegmap->map[MAP_IDX_EVICTING], start, end)) != -1)
        {
            *frameIndex = diff;
            return NV_ERR_NO_MEMORY;
        }
    }

    //
    // Check SCRUBBING
    // TODO: Skip this check if scrubbing has been completed for all frames.
    //
    if ((diff = _checkOne(pRegmap->map[MAP_IDX_SCRUBBING], start, end)) != -1)
    {
        *frameIndex = diff;
        return NV_ERR_NO_MEMORY;
    }

    if ((diff = _checkOne(pRegmap->map[MAP_IDX_NUMA_REUSE], start, end)) != -1)
    {
        *frameIndex = diff;
        return NV_ERR_NO_MEMORY;
    }

    if ((diff = _checkOne(pRegmap->map[MAP_IDX_ALLOC_UNPIN], start, end)) != -1)
    {
        *frameIndex = diff;
        return NV_ERR_IN_USE;
    }

    if ((diff = _checkOne(pRegmap->map[MAP_IDX_BLACKLIST], start, end)) != -1)
    {
        *frameIndex = diff;
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

//
// Return ALL_FREE if all frames in the [start, end] range are available for
// allocation or the first frame index that isn't.
//
static NvS64
_pmaRegmapAvailable(PMA_REGMAP *pRegmap, NvU64 start, NvU64 end)
{
    NvU64 unavailableFrameIndex;
    NV_STATUS frameStatus = _pmaRegmapStatus(pRegmap, start, end, &unavailableFrameIndex);

    if (frameStatus == NV_OK)
        return ALL_FREE;

    NV_ASSERT(unavailableFrameIndex >= start);
    NV_ASSERT(unavailableFrameIndex <= end);

    return unavailableFrameIndex;
}

//
// Return ALL_FREE if all frames in the [start, end] range are available for
// allocation, EVICTABLE if some of them would need to be evicted, or the first
// frame index that isn't free nor evictable.
//
static NvS64
_pmaRegmapEvictable(PMA_REGMAP *pRegmap, NvU64 start, NvU64 end)
{
    NvU64 unavailableFrameIndex;
    NvS64 frameStatus = _pmaRegmapStatus(pRegmap, start, end, &unavailableFrameIndex);

    if (frameStatus == NV_OK)
        return ALL_FREE;

    NV_ASSERT(unavailableFrameIndex >= start);
    NV_ASSERT(unavailableFrameIndex <= end);

    if (frameStatus == NV_ERR_IN_USE)
        return EVICTABLE;

    return unavailableFrameIndex;
}

void *
pmaRegmapInit
(
    NvU64      numFrames,
    NvU64      addrBase,
    PMA_STATS *pPmaStats,
    NvBool     bProtected
)
{
    NvU32 i;
    PMA_REGMAP *newMap;
    NvU64 num2mbPages;

    newMap = (PMA_REGMAP*) portMemAllocNonPaged(sizeof(struct pma_regmap));
    if (newMap == NULL)
    {
        return NULL;
    }
    portMemSet(newMap, 0, sizeof(struct pma_regmap));

    newMap->totalFrames = numFrames;
    num2mbPages = numFrames / (_PMA_2MB >> PMA_PAGE_SHIFT);

    //
    // Initialize all tracking staructure
    // These data are added to the PMA structure; we do not keep per-region stats
    //
    pPmaStats->numFreeFrames += newMap->totalFrames;
    pPmaStats->num2mbPages += num2mbPages;
    pPmaStats->numFree2mbPages += num2mbPages;

    newMap->bProtected = bProtected;
    newMap->pPmaStats = pPmaStats;
    newMap->mapLength = PAGE_MAPIDX(numFrames-1) + 1;

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        // This cast is necessary because NVPORT requires NvLength -- NvU32
        // It is ok because max_reg = 2^32 * 2^6 * 64KB = 2^54B = 16 Petabytes
        newMap->map[i] = (NvU64*) portMemAllocNonPaged((NvLength)(newMap->mapLength * sizeof(NvU64)));
        if (newMap->map[i] == NULL)
        {
            pmaRegmapDestroy(newMap);
            return NULL;
        }
        portMemSet(newMap->map[i], 0, (NvLength) (newMap->mapLength * sizeof(NvU64)));
    }

    return (void *)newMap;
}

void
pmaRegmapDestroy(void *pMap)
{
    NvU32 i;
    NvU64 num2mbPages;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        portMemFree(pRegmap->map[i]);
    }

    pRegmap->pPmaStats->numFreeFrames -= pRegmap->totalFrames;

    num2mbPages = pRegmap->totalFrames / (_PMA_2MB >> PMA_PAGE_SHIFT);
    pRegmap->pPmaStats->numFree2mbPages -= num2mbPages;

    portMemFree(pRegmap);
}

//
// Change a frame's state & attribute
//
// States:
//      STATE_FREE, STATE_UNPIN, STATE_PIN
// Attributes:
//      ATTRIB_PERSISTENT, ATTRIB_SCRUBBING, ATTRIB_EVICTING, ATTRIB_NUMA_REUSE
// Masks:
//      STATE_MASK, ATTRIB_MASK
//

void
pmaRegmapChangeStateAttribEx
(
    void          *pMap,
    NvU64          frameNum,
    PMA_PAGESTATUS newState,
    PMA_PAGESTATUS newStateMask
)
{
    NvU64 mapIndex, mapOffset, bits, newVal, mask;
    NvU32 i, bitWriteCount;
    PMA_PAGESTATUS oldState, updatedState;
    NvBool bUpdate2mbTracking = NV_FALSE;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    mapIndex = PAGE_MAPIDX(frameNum);
    mapOffset = PAGE_BITIDX(frameNum);

    NV_ASSERT(pRegmap != NULL); // possible error code return
    NV_ASSERT(mapIndex < pRegmap->mapLength);

    bitWriteCount = PMA_STATE_BITS_PER_PAGE + PMA_ATTRIB_BITS_PER_PAGE;

    mask = (NvU64)MAKE_BITMASK(mapOffset);

    oldState = pmaRegmapRead(pRegmap, frameNum, NV_TRUE);

    //
    // If we are going to allocate the 2MB page, we need bookkeeping
    // before the bitmap is changed
    //
    if (((newState & STATE_MASK) != STATE_FREE) && _pmaRegmapAllFree2mb(pRegmap, frameNum))
    {
        bUpdate2mbTracking = NV_TRUE;
    }

    for (i = 0; i < bitWriteCount; i++)
    {
        if (NVBIT(i) & newStateMask)
        {
            newVal = ((NvU64) (newState & (1 << i)) >> i) << mapOffset;
            bits = pRegmap->map[i][mapIndex];
            pRegmap->map[i][mapIndex] = (NvU64) SETBITS(bits, mask, newVal);
        }
    }

    // Update some stats for optimization
    updatedState = pmaRegmapRead(pRegmap, frameNum, NV_TRUE);

    pmaStatsUpdateState(&pRegmap->pPmaStats->numFreeFrames, 1,
                        oldState, updatedState);

    //
    // If we are freeing a frame, we should check if we need to update the 2MB
    // page tracking
    //
    if (bUpdate2mbTracking ||
        (((oldState & STATE_MASK) != STATE_FREE) && _pmaRegmapAllFree2mb(pRegmap, frameNum)))
    {
        pmaStatsUpdateState(&pRegmap->pPmaStats->numFree2mbPages, 1,
                            oldState, updatedState);

    }
}

void
pmaRegmapChangeStateAttrib
(
    void *pMap,
    NvU64 frameNum,
    PMA_PAGESTATUS newState,
    NvBool writeAttrib
)
{
    NvU64 mapIndex, mapOffset, bits, newVal, mask;
    NvU32 i;
    NvU32 bitWriteCount;
    PMA_PAGESTATUS oldState;
    NvBool bUpdate2mbTracking = NV_FALSE;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    mapIndex = PAGE_MAPIDX(frameNum);
    mapOffset = PAGE_BITIDX(frameNum);

    NV_ASSERT(pRegmap != NULL); // possible error code return
    NV_ASSERT(mapIndex < pRegmap->mapLength);

    bitWriteCount = (writeAttrib ?
        (PMA_STATE_BITS_PER_PAGE + PMA_ATTRIB_BITS_PER_PAGE) :
        PMA_STATE_BITS_PER_PAGE);

    mask = (NvU64)MAKE_BITMASK(mapOffset);

    oldState = pmaRegmapRead(pRegmap, frameNum, NV_TRUE);

    //
    // If we are going to allocate the 2MB page, we need bookkeeping
    // before the bitmap is changed
    //
    if (((newState & STATE_MASK) != STATE_FREE) && _pmaRegmapAllFree2mb(pRegmap, frameNum))
    {
        bUpdate2mbTracking = NV_TRUE;
    }

    for (i = 0; i < bitWriteCount; i++)
    {
        newVal = ((NvU64) (newState & (1 << i)) >> i) << mapOffset;
        bits = pRegmap->map[i][mapIndex];
        pRegmap->map[i][mapIndex] = (NvU64) SETBITS(bits, mask, newVal);
    }

    NV_ASSERT(pmaRegmapRead(pRegmap, frameNum, writeAttrib) == newState);

    // Update some stats for optimization
    pmaStatsUpdateState(&pRegmap->pPmaStats->numFreeFrames, 1,
                        oldState, newState);

    //
    // If we are freeing a frame, we should check if we need to update the 2MB
    // page tracking
    //
    if (bUpdate2mbTracking ||
        (((oldState & STATE_MASK) != STATE_FREE) && _pmaRegmapAllFree2mb(pRegmap, frameNum)))
    {
        pmaStatsUpdateState(&pRegmap->pPmaStats->numFree2mbPages, 1,
                                   oldState, newState);

    }
}

void
pmaRegmapChangeState(void *pMap, NvU64 frameNum, PMA_PAGESTATUS newState)
{
    NV_ASSERT(newState <= STATE_PIN);
    // Write state bits, but not attrib bits
    pmaRegmapChangeStateAttrib((PMA_REGMAP *)pMap, frameNum, newState, NV_FALSE);
}

void
pmaRegmapChangePageStateAttrib
(
    void * pMap,
    NvU64 startFrame,
    NvU32 pageSize,
    PMA_PAGESTATUS newState,
    NvBool writeAttrib
)
{
    NvU32 framesPerPage = pageSize >> PMA_PAGE_SHIFT;
    NvU64 frame;
    for (frame = startFrame; frame < startFrame + framesPerPage; frame++)
    {
        pmaRegmapChangeStateAttrib((PMA_REGMAP *)pMap, frame, newState, writeAttrib);
    }
}

PMA_PAGESTATUS
pmaRegmapRead(void *pMap, NvU64 frameNum, NvBool readAttrib)
{
    NvU64 mapIndex, mapOffset, bits, mask, val;
    NvU32 i;
    NvU32 bitReadCount;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    NV_ASSERT(pRegmap != NULL); // possible error code return instead of assertion failure

    val = 0;
    mapIndex = PAGE_MAPIDX(frameNum);
    mapOffset = PAGE_BITIDX(frameNum);

    bitReadCount = (readAttrib ? PMA_STATE_BITS_PER_PAGE + PMA_ATTRIB_BITS_PER_PAGE : PMA_STATE_BITS_PER_PAGE);

    mask = (NvU64)MAKE_BITMASK(mapOffset);
    for (i = 0; i < bitReadCount; i++)
    {
        bits = pRegmap->map[i][mapIndex];
        val |= (((bits & mask) >> mapOffset) << i);
    }
    return (PMA_PAGESTATUS)val;
}


//
// Note that these functions only return the free regions but doesn't reserve them
// therefore locks should not be released after they return until you mark them allocated
//
NV_STATUS
pmaRegmapScanContiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU32 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict
)
{
    NvU64 freeStart, numFrames, localStart, localEnd, framesPerPage;
    NvU64 frameAlignment, alignedAddrBase, frameAlignmentPadding;
    NvBool found;
    PMA_PAGESTATUS startStatus, endStatus;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    framesPerPage = pageSize >> PMA_PAGE_SHIFT;
    numFrames = framesPerPage * numPages;
    frameAlignment = alignment >> PMA_PAGE_SHIFT;

    //
    // Find how much is the base address short of the alignment requirements
    // and adjust that value in the scanning range before starting the scan.
    //
    alignedAddrBase       = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

    // Handle restricted allocations
    if (rangeStart != 0 || rangeEnd != 0)
    {
        localStart = rangeStart >> PMA_PAGE_SHIFT;
        localEnd   = NV_MIN(rangeEnd >> PMA_PAGE_SHIFT, pRegmap->totalFrames - 1);
    }
    else
    {
        localStart = 0;
        localEnd   = pRegmap->totalFrames - 1;
    }

    localStart = alignUpToMod(localStart, frameAlignment, frameAlignmentPadding);
    freeStart = localStart;
    found = 0;

    NV_PRINTF(LEVEL_INFO,
              "Scanning with addrBase 0x%llx in frame range 0x%llx..0x%llx, pages to allocate 0x%llx\n",
              addrBase, localStart, localEnd, numPages);

    while (!found)
    {
        if ((freeStart + numFrames - 1) > localEnd)
        {
            // freeStart + numFrames too close to local search end.  Re-starting search
            break;
        }

        startStatus = pmaRegmapRead(pRegmap, freeStart, NV_TRUE);
        endStatus = pmaRegmapRead(pRegmap, (freeStart + numFrames - 1), NV_TRUE);

        if (endStatus == STATE_FREE)
        {
            if (startStatus == STATE_FREE)
            {
                NvS64 diff = _pmaRegmapAvailable(pRegmap, freeStart, (freeStart + numFrames - 1));
                if (diff == ALL_FREE)
                {
                    found = NV_TRUE;
                    *freeList = addrBase + (freeStart << PMA_PAGE_SHIFT);
                    *numPagesAlloc = numPages;
                }
                else
                {
                    //
                    // Find the next aligned free frame and set it as the start
                    // frame for next iteration's scan.
                    //
                    NV_ASSERT(diff >= 0);

                    freeStart = alignUpToMod(diff + 1, frameAlignment, frameAlignmentPadding);

                    NV_ASSERT(freeStart != 0);
                }
            }
            else
            {
                // Start point isn't free, so bump to check the next aligned frame
                freeStart += frameAlignment;
            }
        }
        else
        {
            //
            // End point isn't usable, so jump to after the end to check again
            // However, align the new start point properly before next iteration.
            //
            freeStart += NV_ALIGN_UP(numFrames, frameAlignment);
        }
    }

    if (found) return NV_OK;

    *numPagesAlloc = 0;

    if (bSkipEvict) return NV_ERR_NO_MEMORY;

    // Loop back to the beginning and continue searching

    freeStart = localStart;
    while (!found)
    {
        if ((freeStart + numFrames - 1) > localEnd)
        {
            // Failed searching for STATE_FREE or STATE_UNPIN
            return NV_ERR_NO_MEMORY;
        }

        startStatus = pmaRegmapRead(pRegmap, freeStart, NV_TRUE);
        endStatus = pmaRegmapRead(pRegmap, (freeStart + numFrames - 1), NV_TRUE);

        if (endStatus == STATE_FREE || endStatus == STATE_UNPIN)
        {
            if (startStatus == STATE_FREE || startStatus == STATE_UNPIN)
            {
                NvS64 diff = _pmaRegmapEvictable(pRegmap, freeStart, (freeStart + numFrames - 1));
                if (diff == EVICTABLE)
                {
                    found = NV_TRUE;
                    *freeList = addrBase + (freeStart << PMA_PAGE_SHIFT);
                }
                else
                {
                    //
                    // The previous search should have found an all free region
                    // and we wouldn't be looking for an evictable one.
                    //
                    NV_ASSERT(diff != ALL_FREE);
                    NV_ASSERT(diff >= 0);

                    //
                    // Find the next aligned free frame and set it as the start
                    // frame for next iteration's scan.
                    //
                    freeStart = alignUpToMod(diff + 1, frameAlignment, frameAlignmentPadding);
                    NV_ASSERT(freeStart != 0);
                }
            }
            else
            {
                // Start point isn't usable, so bump to the next aligned frame to check again
                freeStart += frameAlignment;
            }
        }
        else
        {
            //
            // End point isn't usable, so jump to after the end to check again
            // However, align the new start point properly before next iteration.
            //
            freeStart += NV_ALIGN_UP(numFrames, frameAlignment);
        }
    }

    return NV_ERR_IN_USE;
}

NV_STATUS
pmaRegmapScanDiscontiguous
(
    void *pMap,
    NvU64 addrBase,
    NvU64 rangeStart,
    NvU64 rangeEnd,
    NvU64 numPages,
    NvU64 *freeList,
    NvU32 pageSize,
    NvU64 alignment,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict
)
{
    NvU64 freeStart, found, framesPerPage, localStart, localEnd;
    NvU64 alignedAddrBase, frameAlignmentPadding;
    PMA_PAGESTATUS startStatus, endStatus;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    NV_ASSERT(alignment == pageSize);

    framesPerPage = pageSize >> PMA_PAGE_SHIFT;

    //
    // Find how much is the base address short of the alignment requirements
    // and adjust that value in the scanning range before starting the scan.
    //
    alignedAddrBase       = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

    // Handle restricted allocations
    if (rangeStart != 0 || rangeEnd != 0)
    {
        // Embedded % requires special handling.
        NV_ASSERT_OR_ELSE_STR(rangeStart % pageSize == 0,
            "rangeStart %% pageSize == 0", /*do nothing*/);
        NV_ASSERT_OR_ELSE_STR((rangeEnd + 1) % pageSize == 0,
            "(rangeEnd + 1) %% pageSize == 0", /*do nothing*/);

        localStart = rangeStart >> PMA_PAGE_SHIFT;
        localEnd   = NV_MIN(rangeEnd >> PMA_PAGE_SHIFT, pRegmap->totalFrames - 1);
    }
    else
    {
        localStart = 0;
        localEnd   = pRegmap->totalFrames-1;
    }

    localStart = alignUpToMod(localStart, framesPerPage, frameAlignmentPadding);
    freeStart = localStart;
    found = 0;

    NV_PRINTF(LEVEL_INFO,
              "Scanning with addrBase 0x%llx in frame range 0x%llx..0x%llx, pages to allocate 0x%llx\n",
              addrBase, localStart, localEnd, numPages);

    // scan for allocatable pages
    // two-pass algorithm
    while (found != numPages)
    {
        if ((freeStart + framesPerPage - 1) > localEnd) break;

        startStatus = pmaRegmapRead(pRegmap, freeStart, NV_TRUE);
        endStatus = pmaRegmapRead(pRegmap, (freeStart + framesPerPage - 1), NV_TRUE);

        if (startStatus == STATE_FREE)
        {
            if(endStatus == STATE_FREE)
            {
                NvS64 diff = _pmaRegmapAvailable(pRegmap, freeStart, (freeStart + framesPerPage - 1));
                if (diff == ALL_FREE)
                {
                    freeList[found++] = addrBase + (freeStart << PMA_PAGE_SHIFT);
                }
            }
        }
        freeStart += framesPerPage;
    }

    *numPagesAlloc = found;
    if(found == numPages) return NV_OK;
    if(bSkipEvict) return NV_ERR_NO_MEMORY;

    freeStart = localStart;
    while (found != numPages)
    {
        if ((freeStart + framesPerPage - 1) > localEnd) return NV_ERR_NO_MEMORY;

        startStatus = pmaRegmapRead(pRegmap, freeStart, NV_TRUE);
        endStatus = pmaRegmapRead(pRegmap, (freeStart + framesPerPage - 1), NV_TRUE);

        if (startStatus == STATE_FREE || startStatus == STATE_UNPIN)
        {
            if(endStatus == STATE_FREE || endStatus == STATE_UNPIN)
            {
                NvS64 diff = _pmaRegmapEvictable(pRegmap, freeStart, (freeStart + framesPerPage - 1));
                if (diff == EVICTABLE)
                {
                    freeList[found++] = addrBase + (freeStart << PMA_PAGE_SHIFT);
                }
            }
        }
        freeStart += framesPerPage;
    }

    return NV_ERR_IN_USE;
}

void
pmaRegmapGetSize
(
    void  *pMap,
    NvU64 *pBytesTotal
)
{
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;
    *pBytesTotal = (pRegmap->totalFrames << PMA_PAGE_SHIFT);
}

void
pmaRegmapGetLargestFree
(
    void  *pMap,
    NvU64 *pLargestFree
)
{
    NvU64 mapIndex       = 0;
    NvU32 mapMaxZeros    = 0;
    NvU32 mapTrailZeros  = 0;
    NvU32 regionMaxZeros = 0;
    NvU64 mapMaxIndex;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    mapMaxIndex = PAGE_MAPIDX(pRegmap->totalFrames - 1);

    while (mapIndex <= mapMaxIndex)
    {
        NvU64 bitmap = pRegmap->map[MAP_IDX_ALLOC_UNPIN][mapIndex] | pRegmap->map[MAP_IDX_ALLOC_PIN][mapIndex];

        // If the last map[] is only partially used, mask the valid bits
        if (mapIndex == mapMaxIndex && (PAGE_BITIDX(pRegmap->totalFrames) != 0))
        {
            bitmap |= (~0ULL) << PAGE_BITIDX(pRegmap->totalFrames);
        }

        if (maxZerosGet(bitmap) == _UINT_SIZE)
        {
            mapTrailZeros += _UINT_SIZE;
        }
        else
        {
            mapTrailZeros += portUtilCountTrailingZeros64(bitmap);
            mapMaxZeros = maxZerosGet(bitmap);
            regionMaxZeros = NV_MAX(regionMaxZeros,
                                    NV_MAX(mapMaxZeros, mapTrailZeros));
            mapTrailZeros = portUtilCountLeadingZeros64(bitmap);
        }

        mapIndex++;
    }
    regionMaxZeros = NV_MAX(regionMaxZeros, mapTrailZeros);
    *pLargestFree = ((NvU64) regionMaxZeros) << PMA_PAGE_SHIFT;
}

NvU64 pmaRegmapGetEvictingFrames(void *pMap)
{
    return ((PMA_REGMAP *)pMap)->frameEvictionsInProcess;
}

void pmaRegmapSetEvictingFrames(void *pMap, NvU64 frameEvictionsInProcess)
{
    ((PMA_REGMAP *)pMap)->frameEvictionsInProcess = frameEvictionsInProcess;
}

