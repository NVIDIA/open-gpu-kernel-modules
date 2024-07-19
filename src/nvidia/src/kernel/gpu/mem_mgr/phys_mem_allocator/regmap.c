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

#define FRAME_TO_U64_SHIFT 6
#define FRAME_TO_U64_SIZE  (1llu << FRAME_TO_U64_SHIFT)
#define FRAME_TO_U64_MASK (FRAME_TO_U64_SIZE - 1llu)

#define PAGE_BITIDX(n)              ((n) & (FRAME_TO_U64_SIZE - 1llu))
#define PAGE_MAPIDX(n)              ((n) >> FRAME_TO_U64_SHIFT)
#define MAKE_BITMASK(n)             (1llu << (n))

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
                return ((mapIdx << FRAME_TO_U64_SHIFT) + firstSetBit);
            }
        }

        // handle edge case
        endMask = (NV_U64_MAX >> (FRAME_TO_U64_SIZE - endBitIdx - 1));

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
        endMask = (NV_U64_MAX >> (FRAME_TO_U64_SIZE - endBitIdx - 1));

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
    return ((frame - mod + alignment - 1ll) & ~(alignment - 1ll)) + mod;
}
static NvU64 alignDownToMod(NvU64 frame, NvU64 alignment, NvU64 mod)
{
    return ((frame - mod) & ~(alignment - 1ll)) + mod;
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
            mask = (mask >> (FRAME_TO_U64_SIZE - endBitIdx - 1));
        }

        if (mapIter == startMapIdx)
        {
            mask = (mask & ~(MAKE_BITMASK(startBitIdx) - 1));
        }

#ifdef DEBUG_VERBOSE

        NV_PRINTF(LEVEL_INFO, "mapIter %llx frame %llx mask %llx unpinbitmap %llx pinbitmap %llx evictbitmap %llx",
                 mapIter, (mapIter << FRAME_TO_U64_SHIFT), mask, unpinBitmap[mapIter], pinBitmap[mapIter], evictBitmap[mapIter]);
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
                unevictableFrameIndex = (mapIter << FRAME_TO_U64_SHIFT) +  (FRAME_TO_U64_SIZE - 1);
            break;
        }
#ifdef DEBUG_VERBOSE
        NV_PRINTF(LEVEL_INFO, "Check leading zero of %llx", ~(unpinBitmap[mapIter] & mask));
#endif

        unevictableIndex = FRAME_TO_U64_SIZE - portUtilCountLeadingZeros64((~unpinBitmap[mapIter]) & mask) - 1;
        unevictableFrameIndex = (mapIter << FRAME_TO_U64_SHIFT) + unevictableIndex;
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
    NvU64 frameLimit, frameStart;
    NvU64 alignment = pageSize;
    NvU64 frameAlignment, frameAlignmentPadding;
    NvU64 numFrames = actualSize >> PMA_PAGE_SHIFT;
    NvU64 numFramesLimit = numFrames - 1;

    frameLimit = pRegmap->totalFrames - 1;

    if (pRegmap->totalFrames < numFrames)
        return status;

    //Copied from pmaRegmapScanContiguous
    frameAlignment = alignment >> PMA_PAGE_SHIFT;
    alignedAddrBase = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;
    frameStart = alignUpToMod(0, frameAlignment, frameAlignmentPadding);


    for (frameNum = frameStart; frameNum <= (frameLimit - numFramesLimit); )
    {
        PMA_PAGESTATUS startFrameAllocState;
        PMA_PAGESTATUS endFrameAllocState;
        NvS64 firstUnevictableFrame;

        startFrameAllocState  = pmaRegmapRead(pRegmap, frameNum, NV_TRUE);
        endFrameAllocState    = pmaRegmapRead(pRegmap, frameNum + numFramesLimit, NV_TRUE);

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


        // First occurrence of 0 in STATE_UNPIN  from frameNum to frameNum + numFramesLimit
        firstUnevictableFrame = _pmaRegmapScanNumaUnevictable(pRegmap, frameNum, frameNum + numFramesLimit);

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

    if (bProtected)
    {
        pPmaStats->numFreeFramesProtected += newMap->totalFrames;
        pPmaStats->num2mbPagesProtected += num2mbPages;
        pPmaStats->numFree2mbPagesProtected += num2mbPages;
    }

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
    {
        //
        // Simplify logic for 2M tracking. Set the last few nonaligned bits as pinned
        // so that the XOR logic for delta 2M tracking is never true for an incomplete final page
        //
        NvU64 endOffs = (numFrames - 1llu) >> FRAME_TO_U64_SHIFT;
        NvU64 endBit = (numFrames - 1llu) & FRAME_TO_U64_MASK;
        NvU64 endMask = endBit == FRAME_TO_U64_MASK ? 0llu : ~(NV_U64_MAX >> (FRAME_TO_U64_MASK - endBit));
        newMap->map[MAP_IDX_ALLOC_PIN][endOffs] |= endMask;
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

    if (pRegmap->bProtected)
    {
        pRegmap->pPmaStats->numFreeFramesProtected -= pRegmap->totalFrames;
    }

    num2mbPages = pRegmap->totalFrames / (_PMA_2MB >> PMA_PAGE_SHIFT);
    pRegmap->pPmaStats->numFree2mbPages -= num2mbPages;

    if (pRegmap->bProtected)
    {
        pRegmap->pPmaStats->numFree2mbPagesProtected -= num2mbPages;
    }

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
pmaRegmapChangeStateAttrib
(
    void          *pMap,
    NvU64          frameNum,
    PMA_PAGESTATUS newState,
    PMA_PAGESTATUS newStateMask
)
{
    pmaRegmapChangeBlockStateAttrib(pMap, frameNum, 1, newState, newStateMask);
}

void
pmaRegmapChangePageStateAttrib
(
    void * pMap,
    NvU64 startFrame,
    NvU64 pageSize,
    PMA_PAGESTATUS newState,
    PMA_PAGESTATUS newStateMask
)
{
    pmaRegmapChangeBlockStateAttrib(pMap, startFrame, pageSize / _PMA_64KB, newState, newStateMask);
}


static NV_FORCEINLINE
void
_pmaRegmapDoSingleStateChange
(
    PMA_REGMAP *pRegmap,
    NvU64 idx,
    NvU32 newState,
    NvU32 writeMask,
    NvU64 bitMask,
    NvU64 *delta2m,
    NvU64 *delta64k
)
{
    // get bits from map
    NvU64 pinIn = pRegmap->map[MAP_IDX_ALLOC_PIN][idx];
    NvU64 unpinIn = pRegmap->map[MAP_IDX_ALLOC_UNPIN][idx];
    // Or of state for delta-tracking purposes
    NvU64 initialState = pinIn | unpinIn;
    // Mask out bits that are being upda
    NvU64 maskedPin = pinIn & ~bitMask;
    NvU64 maskedUnpin = unpinIn & ~bitMask;
    // Update bits in new with bitMask
    NvU64 pinRes = ((newState & (1llu << MAP_IDX_ALLOC_PIN)) ? bitMask : 0llu);
    NvU64 unpinRes = ((newState & (1llu << MAP_IDX_ALLOC_UNPIN)) ? bitMask : 0llu);
    // Output state based on whether writeMask is set
    NvU64 pinOut = (writeMask & (1llu << MAP_IDX_ALLOC_PIN))  ? (maskedPin | pinRes) : pinIn;
    NvU64 unpinOut = (writeMask & (1llu << MAP_IDX_ALLOC_UNPIN)) ? (maskedUnpin | unpinRes) : unpinIn;
    // Or of final state for delta-tracking purposes
    NvU64 finalState = pinOut | unpinOut;
    NvU64 xored = initialState ^ finalState;

    // Write out new bits
    pRegmap->map[MAP_IDX_ALLOC_PIN][idx] = pinOut;
    pRegmap->map[MAP_IDX_ALLOC_UNPIN][idx] = unpinOut;

    // Update deltas
    (*delta64k) += nvPopCount64(xored);
    // Each 2M page is 32 64K pages, so we check each half of a 64-bit qword and xor them
    (*delta2m) += ((((NvU32)finalState) == 0) != (((NvU32)initialState) == 0)) +
                ((((NvU32)(finalState >> 32)) == 0) != (((NvU32)(initialState >> 32)) == 0));
}

void
pmaRegmapChangeBlockStateAttrib
(
    void *pMap,
    NvU64 frame,
    NvU64 len,
    PMA_PAGESTATUS newState,
    PMA_PAGESTATUS writeMask
)
{
    NvU64 initialIdx = PAGE_MAPIDX(frame);
    NvU64 finalIdx = PAGE_MAPIDX(frame + len - 1llu);
    NvU64 initialOffs = PAGE_BITIDX(frame);
    NvU64 finalOffs = PAGE_BITIDX(frame + len - 1llu);
    NvU64 initialMask = NV_U64_MAX << initialOffs;
    NvU64 finalMask = NV_U64_MAX >> (FRAME_TO_U64_MASK - finalOffs);
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;
    NvU64 i;
    NvU64 delta2m = 0, delta64k = 0;

    NV_ASSERT(pRegmap != NULL);
    NV_ASSERT(frame + len <= pRegmap->totalFrames);

    // Update non-state attributes first in a tight loop.
    for (i = PMA_STATE_BITS_PER_PAGE; i < PMA_BITS_PER_PAGE; i++)
    {
        NvU64 j;
        NvU64 toWrite = (newState & (1u << i)) ? NV_U64_MAX : 0llu;
        if (!((1u << i) & writeMask))
        {
            continue;
        }
        if (initialIdx == finalIdx)
        {
            pRegmap->map[i][initialIdx] &= ~(initialMask & finalMask);
            pRegmap->map[i][initialIdx] |= toWrite & (initialMask & finalMask);
            continue;
        }
       
        pRegmap->map[i][initialIdx] &= ~initialMask;
        pRegmap->map[i][initialIdx] |= toWrite & initialMask;
        
        for (j = initialIdx + 1; j < finalIdx; j++)
        {
            pRegmap->map[i][j] = toWrite;
            
        }
        pRegmap->map[i][finalIdx] &= ~finalMask;
        pRegmap->map[i][finalIdx] |= toWrite & finalMask;
        
    }

    if (!(writeMask & STATE_MASK))
    {
        return;
    }

    // Entire state is in one NvU64, so exit immediately after
    if (initialIdx == finalIdx)
    {
        _pmaRegmapDoSingleStateChange(pRegmap, initialIdx, newState, writeMask, initialMask & finalMask, &delta2m, &delta64k);
        goto set_regs;
    }

    // Checks for 64-aligned start/end so we don't have to deal with partial coverage in the main loop
    if (initialOffs != 0)
    {
        // Do first state update with partial NvU64 coverage
        _pmaRegmapDoSingleStateChange(pRegmap, initialIdx, newState, writeMask, initialMask, &delta2m, &delta64k);
        initialIdx++;
    }
    if (finalOffs != FRAME_TO_U64_MASK)
    {
        // Update last partial NvU64
        _pmaRegmapDoSingleStateChange(pRegmap, finalIdx, newState, writeMask, finalMask, &delta2m, &delta64k);
        finalIdx--;
    }

    // Update all full-size 
    for (i = initialIdx; i <= finalIdx; i++)
    {
        _pmaRegmapDoSingleStateChange(pRegmap, i, newState, writeMask, NV_U64_MAX, &delta2m, &delta64k);
    }

set_regs:
    if ((newState & writeMask & STATE_MASK) != 0)
    {
        pRegmap->pPmaStats->numFreeFrames -= delta64k;
        pRegmap->pPmaStats->numFree2mbPages -= delta2m;
    }
    else
    {
        pRegmap->pPmaStats->numFreeFrames += delta64k;
        pRegmap->pPmaStats->numFree2mbPages += delta2m;
    }
    if (!pRegmap->bProtected)
    {
        return;
    }
    if ((writeMask & newState & STATE_MASK) != 0)
    {
        pRegmap->pPmaStats->numFreeFramesProtected -= delta64k;
        pRegmap->pPmaStats->numFree2mbPagesProtected -= delta2m;
    }
    else
    {
        pRegmap->pPmaStats->numFreeFramesProtected += delta64k;
        pRegmap->pPmaStats->numFree2mbPagesProtected += delta2m;
    }
    return;
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

static NvS64 _scanContiguousSearchLoop
(
    PMA_REGMAP *pRegmap,
    NvU64 numFrames,
    NvU64 localStart,
    NvU64 localEnd,
    NvU64 frameAlignment,
    NvU64 frameAlignmentPadding,
    NvU64 localStride,
    NvU32 strideStart,
    NvU64 strideAlignmentPadding,
    NvU64 strideRegionAlignmentPadding,
    NvBool bSearchEvictable
)
{
    NvU64 frameBaseIdx = alignUpToMod(localStart, frameAlignment, frameAlignmentPadding); // this is already done by the caller
    NvU64 nextStrideStart;
    NvU64 latestFree[PMA_BITS_PER_PAGE];
    NvU64 i;

    // can't allocate contiguous memory > stride. This is guaranteed by the caller, but be sure here
    if ((localStride != 0) && (numFrames > localStride))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // latestFree stores the highest '0' seen in the given map array in the current run
    // ie we have the needed pages if frameBaseIdx + numPages == latestFree. Initialize to first aligned frame
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        latestFree[i] = frameBaseIdx;
    }

loop_begin:
    if (localStride != 0)
    {
        // Put the address into the correct strideStart
        if ((((frameBaseIdx - frameAlignmentPadding) / localStride) % 2) != strideStart)
        {
            // align up to next stride
            frameBaseIdx = alignUpToMod((frameBaseIdx + 1), localStride, strideAlignmentPadding);
        }
    }

    if (localStride != 0)
    {
        // we're always in the correct stride at this point
        nextStrideStart = alignUpToMod(frameBaseIdx + 1, localStride, strideAlignmentPadding);

        // Won't fit within current stride, need to align up again.
        if ((frameBaseIdx + numFrames) > nextStrideStart)
        {
            // even if it's in the correct stride, it won't fit here, so force it to the next one
            frameBaseIdx = nextStrideStart;

            // Put the address into the correct strideStart
            if ((((frameBaseIdx - frameAlignmentPadding) / localStride) % 2) != strideStart)
            {
                // align up to next stride
                frameBaseIdx = alignUpToMod((frameBaseIdx + 1), localStride, strideAlignmentPadding);
            }
        }
    }

    //
    // Always start a loop iteration with an updated frameBaseIdx by ensuring that latestFree is always >= frameBaseIdx
    // frameBaseIdx == latestFree[i] means that there are no observed 0s so far in the current run
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        if (latestFree[i] < frameBaseIdx)
        {
            latestFree[i] = frameBaseIdx;
        }
    }
    // At the end of memory, pages not available
    if ((frameBaseIdx + numFrames - 1llu) > localEnd)
    {
        return -1;
    }
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        // TODO, merge logic so we don't need multiple calls for unpin
        if (i == MAP_IDX_ALLOC_UNPIN && bSearchEvictable)
        {
            continue;
        }

        while (latestFree[i] < (frameBaseIdx + numFrames))
        {
            //
            // All this logic looks complicated, but essentially all it is doing is getting the NvU64 from
            // the correct index in the array and shifting and masking so that the first bit is latestFree[i].
            // endOffs is set then to the length of the run of zeros at the beginning
            //
            NvU64 curMapIdx = PAGE_MAPIDX(latestFree[i]);
            NvU64 beginOffs = PAGE_BITIDX(latestFree[i]);
            NvU64 mask = beginOffs == 0 ? 0 : NV_U64_MAX << (FRAME_TO_U64_SIZE - beginOffs);
            NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] >> beginOffs) | mask;
            NvU64 endOffs = portUtilCountTrailingZeros64(curWithOffs);
            //
            // If no more are free, we have not hit the needed number of pages. Following loop finds
            // the next free page
            //
            if (endOffs == 0)
            {
                mask = beginOffs == 0 ? 0 : NV_U64_MAX >> (FRAME_TO_U64_SIZE - beginOffs);
                NvU64 curMap = pRegmap->map[i][curMapIdx] | mask;
                frameBaseIdx = latestFree[i] - beginOffs;
                if (curMap != NV_U64_MAX)
                {
                    goto free_found;
                }
                curMapIdx++;
                frameBaseIdx += FRAME_TO_U64_SIZE;
                while (frameBaseIdx <= localEnd)
                {
                    curMap = pRegmap->map[i][curMapIdx];
                    if(curMap != NV_U64_MAX)
                    {
                        goto free_found;
                    }
                    frameBaseIdx += FRAME_TO_U64_SIZE;
                    curMapIdx++;
                }
                // No more free pages, exit
                return -1;
free_found:
                // Found a free page, set frameBaseIdx and go back to the beginning of the loop
                frameBaseIdx += portUtilCountTrailingZeros64(~curMap);
                frameBaseIdx = alignUpToMod(frameBaseIdx, frameAlignment, frameAlignmentPadding);
                goto loop_begin;
            }
            latestFree[i] += endOffs;
        }
    }

    return frameBaseIdx;
}

static NvS64 _scanContiguousSearchLoopReverse
(
    PMA_REGMAP *pRegmap,
    NvU64 numFrames,
    NvU64 localStart,
    NvU64 localEnd,
    NvU64 frameAlignment,
    NvU64 frameAlignmentPadding,
    NvBool bSearchEvictable
)
{
    NvU64 realAlign = (frameAlignmentPadding + numFrames) & (frameAlignment - 1ll);
    NvU64 frameBaseIdx = alignDownToMod(localEnd + 1llu, frameAlignment, realAlign);
    //
    // latestFree stores the lowest '0' seen in the given map array in the current run
    // ie we have the needed pages if frameBaseIdx - numPages == latestFree. Initialize to last aligned frame
    //
    NvU64 latestFree[PMA_BITS_PER_PAGE];
    NvU64 i;
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
            latestFree[i] = frameBaseIdx;
    }
loop_begin:
    //
    // Always start a loop iteration with an updated frameBaseIdx by ensuring that latestFree is always <= frameBaseIdx
    // frameBaseIdx == latestFree[i] means that there are no observed 0s so far in the current run
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        if (latestFree[i] > frameBaseIdx)
        {
            latestFree[i] = frameBaseIdx;
        }
    }
    // At the beginning of memory, pages not available
    if ((localStart + numFrames) > frameBaseIdx)
    {
        return -1;
    }
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        // TODO, merge logic so we don't need multiple calls for unpin
        if (i == MAP_IDX_ALLOC_UNPIN && bSearchEvictable)
        {
            continue;
        }
        while (latestFree[i] > (frameBaseIdx - numFrames))
        {
            //
            // All this logic looks complicated, but essentially all it is doing is getting the NvU64 from
            // the correct index in the array and shifting and masking so that the last bit is latestFree[i].
            // endOffs is set then to the length of the run of zeros at the end
            //
            NvU64 curId = latestFree[i] - 1llu;
            NvU64 curMapIdx = PAGE_MAPIDX(curId);
            NvU64 beginOffs = PAGE_BITIDX(curId);
            NvU64 mask = beginOffs == FRAME_TO_U64_MASK ? 0 : NV_U64_MAX >> (1llu + beginOffs);
            NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] << (FRAME_TO_U64_MASK - beginOffs)) | mask;
            NvU64 endOffs = portUtilCountLeadingZeros64(curWithOffs);
            //
            // If no more are free, we have not hit the needed number of pages. Following loop finds
            // the next free page
            //
            if (endOffs == 0)
            {
                mask = beginOffs == FRAME_TO_U64_MASK ? 0 : NV_U64_MAX << (1llu + beginOffs);
                NvU64 curMap = (pRegmap->map[i][curMapIdx]) | mask;
                frameBaseIdx = latestFree[i] + FRAME_TO_U64_MASK - beginOffs;
                if (curMap != NV_U64_MAX)
                {
                    goto free_found;
                }
                curMapIdx--;
                frameBaseIdx -= FRAME_TO_U64_SIZE;
                while (frameBaseIdx > localStart)
                {
                    curMap = pRegmap->map[i][curMapIdx];
                    if(curMap != NV_U64_MAX)
                    {
                        goto free_found;
                    }
                    frameBaseIdx -= FRAME_TO_U64_SIZE;
                    curMapIdx--;
                }
                // No more free pages, exit
                return -1;
free_found:
                // Found a free page, set frameBaseIdx and go back to the beginning of the loop
                frameBaseIdx -= portUtilCountLeadingZeros64(~curMap);
                frameBaseIdx = alignDownToMod(frameBaseIdx, frameAlignment, realAlign);
                goto loop_begin;
            }
            latestFree[i] -= endOffs;
        }
    }

    return frameBaseIdx - numFrames;
}

static NV_FORCEINLINE
NvU64
_scanDiscontiguousSearchLoop
(
    PMA_REGMAP *pRegmap,
    NvU64 numPages,
    NvU64 framesPerPage,
    NvU64 localStart,
    NvU64 localEnd,
    NvU64 frameAlignment,
    NvU64 frameAlignmentPadding,
    NvU64 localStride,
    NvU32 strideStart,
    NvU64 strideAlignmentPadding,
    NvU64 strideRegionAlignmentPadding,
    NvU64 *pPages,
    NvU64 *pNumEvictablePages
)
{
    // discontig doesn't align it before starting the search, should be cleaned up
    NvU64 frameBaseIdx = alignUpToMod(localStart, frameAlignment, frameAlignmentPadding);
    NvU64 nextStrideStart;
    NvU64 latestFree[PMA_BITS_PER_PAGE];
    NvU64 totalFound = 0;

    // Evictable pages count down from end of array
    NvU64 curEvictPage = numPages;
    NvBool bEvictablePage = NV_FALSE;
    NvU64 i;

    //
    // latestFree stores the lowest '0' seen in the given map array in the current run
    // ie we have the needed pages if frameBaseIdx - numPages == latestFree. Initialize to last aligned frame
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        latestFree[i] = frameBaseIdx;
    }
loop_begin:
    if (localStride != 0)
    {
        // Put the address into the correct strideStart
        if ((((frameBaseIdx - frameAlignmentPadding) / localStride) % 2) != strideStart)
        {
            // align up to next stride
            frameBaseIdx = alignUpToMod((frameBaseIdx + 1), localStride, strideAlignmentPadding);
        }
    }

    if (localStride != 0)
    {
        // we're always in the correct stride at this point
        nextStrideStart = alignUpToMod(frameBaseIdx + 1, localStride, strideAlignmentPadding);

        // Won't fit within current stride, need to align up again.
        if ((frameBaseIdx + framesPerPage) > nextStrideStart)
        {
            // even if it's in the correct stride, it won't fit here, so force it to the next one
            frameBaseIdx = nextStrideStart;

            // Put the address into the correct strideStart
            if ((((frameBaseIdx - frameAlignmentPadding) / localStride) % 2) != strideStart)
            {
                // align up to next stride
                frameBaseIdx = alignUpToMod((frameBaseIdx + 1), localStride, strideAlignmentPadding);
            }
        }
    }

    //
    // Always start a loop iteration with an updated frameBaseIdx by ensuring that latestFree is always >= frameBaseIdx
    // frameBaseIdx == latestFree[i] means that there are no observed 0s so far in the current run
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        if (latestFree[i] < frameBaseIdx)
        {
            latestFree[i] = frameBaseIdx;
        }
    }

    // Initialize to standard free page state
    bEvictablePage = NV_FALSE;

    // At the end of memory, pages not available
    if ((frameBaseIdx + framesPerPage - 1llu) > localEnd)
    {
        *pNumEvictablePages = numPages - curEvictPage;
        return totalFound;
    }

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {

        // If array is not already full of evictable and free pages, go to evictable loop
        if ((i != MAP_IDX_ALLOC_UNPIN) || (curEvictPage <= totalFound))
        {
            while (latestFree[i] < (frameBaseIdx + framesPerPage))
            {
                //
                // All this logic looks complicated, but essentially all it is doing is getting the NvU64 from
                // the correct index in the array and shifting and masking so that the first bit is latestFree[i].
                // endOffs is set then to the length of the run of zeros at the beginning
                //
                NvU64 curMapIdx = PAGE_MAPIDX(latestFree[i]);
                NvU64 beginOffs = PAGE_BITIDX(latestFree[i]);
                NvU64 mask = beginOffs == 0 ? 0 : NV_U64_MAX << (FRAME_TO_U64_SIZE - beginOffs);
                NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] >> beginOffs) | mask;
                NvU64 endOffs = portUtilCountTrailingZeros64(curWithOffs);
                //
                // If no more are free, we have not hit the needed number of pages. Following loop finds
                // the next free page
                //
                if (endOffs == 0)
                {
                    mask = beginOffs == 0 ? 0 : NV_U64_MAX >> (FRAME_TO_U64_SIZE - beginOffs);
                    NvU64 curMap = pRegmap->map[i][curMapIdx] | mask;
                    frameBaseIdx = latestFree[i] - beginOffs;
                    if (curMap != NV_U64_MAX)
                    {
                        goto free_found;
                    }
                    curMapIdx++;
                    frameBaseIdx += FRAME_TO_U64_SIZE;
                    while (frameBaseIdx <= localEnd)
                    {
                        curMap =  pRegmap->map[i][curMapIdx];
                        if(curMap != NV_U64_MAX)
                        {
                            goto free_found;
                        }
                        frameBaseIdx += FRAME_TO_U64_SIZE;
                        curMapIdx++;
                    }
                    // No more free pages, exit
                    *pNumEvictablePages = numPages - curEvictPage;
                    return totalFound;
free_found:
                    // Found a free page, set frameBaseIdx and go back to the beginning of the loop
                    frameBaseIdx += portUtilCountTrailingZeros64(~curMap);
                    frameBaseIdx = alignUpToMod(frameBaseIdx, frameAlignment, frameAlignmentPadding);
                    goto loop_begin;
                }
                latestFree[i] += endOffs;
            }
        }
        else
        {
            // Loop to check if current range has an unpinned page, then it gets stored in the evictable area
            while (latestFree[i] < (frameBaseIdx + framesPerPage))
            {
                // Basically same as above loop, just not exiting if 0 not found, instead setting bEvictablePage
                NvU64 curMapIdx = PAGE_MAPIDX(latestFree[i]);
                NvU64 beginOffs = PAGE_BITIDX(latestFree[i]);
                NvU64 mask = beginOffs == 0 ? 0 : NV_U64_MAX << (FRAME_TO_U64_SIZE - beginOffs);
                NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] >> beginOffs) | mask;
                NvU64 endOffs = portUtilCountTrailingZeros64(curWithOffs);
                latestFree[i] += endOffs;
                if (endOffs == 0)
                {
                    bEvictablePage = NV_TRUE;
                    break;
                }
            }
        }
    }

    // Store evictable pages at end of array to not interfere with free pages
    if (bEvictablePage)
    {
        curEvictPage--;
        pPages[curEvictPage] = frameBaseIdx;
        frameBaseIdx += framesPerPage;
        goto loop_begin;
    }

    pPages[totalFound] = frameBaseIdx;
    totalFound++;
    frameBaseIdx += framesPerPage;

    // Found all needed pages (all free and not STATE_UNPIN)
    if (totalFound == numPages)
    {
        *pNumEvictablePages = 0;
        return numPages;
    }
    goto loop_begin;
}

static NV_FORCEINLINE
NvU64 
_scanDiscontiguousSearchLoopReverse
(
    PMA_REGMAP *pRegmap,
    NvU64 numPages,
    NvU64 framesPerPage,
    NvU64 localStart,
    NvU64 localEnd,
    NvU64 frameAlignment,
    NvU64 frameAlignmentPadding,
    NvU64 *pPages,
    NvU64 *pNumEvictablePages
)
{
    NvU64 realAlign = (frameAlignmentPadding + framesPerPage) & (frameAlignment - 1ll);
    NvU64 frameBaseIdx = alignDownToMod(localEnd+1llu, frameAlignment, realAlign);

    //
    // latestFree stores the lowest '0' seen in the given map array in the current run
    // ie we have the needed pages if frameBaseIdx - numPages == latestFree. Initialize to last aligned frame
    //
    NvU64 latestFree[PMA_BITS_PER_PAGE];
    NvU64 totalFound = 0;

    // Evictable pages count down from end of array
    NvU64 curEvictPage = numPages;
    NvBool bEvictablePage = NV_FALSE;
    NvU64 i;

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        latestFree[i] = frameBaseIdx;
    }
loop_begin:
    //
    // Always start a loop iteration with an updated frameBaseIdx by ensuring that latestFree is always <= frameBaseIdx
    // frameBaseIdx == latestFree[i] means that there are no observed 0s so far in the current run
    //
    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        if (latestFree[i] > frameBaseIdx)
        {
            latestFree[i] = frameBaseIdx;
        }
    }

    // Initialize to standard free page state
    bEvictablePage = NV_FALSE;

    // At the beginning of memory, pages not available
    if ((localStart + framesPerPage) > frameBaseIdx)
    {
        *pNumEvictablePages = numPages - curEvictPage;
        return totalFound;
    }

    for (i = 0; i < PMA_BITS_PER_PAGE; i++)
    {
        // If array is not already full of evictable and free pages, go to evictable loop
        if ((i != MAP_IDX_ALLOC_UNPIN) || (curEvictPage <= totalFound))
        {
            while (latestFree[i] > (frameBaseIdx - framesPerPage))
            {
                //
                // All this logic looks complicated, but essentially all it is doing is getting the NvU64 from
                // the correct index in the array and shifting and masking so that the last bit is latestFree[i].
                // endOffs is set then to the length of the run of zeros at the end
                //
                NvU64 curId = latestFree[i] - 1llu;
                NvU64 curMapIdx = PAGE_MAPIDX(curId);
                NvU64 beginOffs = PAGE_BITIDX(curId);
                NvU64 mask = beginOffs == FRAME_TO_U64_MASK ? 0 : NV_U64_MAX >> (1llu + beginOffs);
                NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] << (FRAME_TO_U64_MASK - beginOffs)) | mask;
                NvU64 endOffs = portUtilCountLeadingZeros64(curWithOffs);

                //
                // If no more are free, we have not hit the needed number of pages. Following loop finds
                // the next free page
                //
                if (endOffs == 0)
                {
                    mask = beginOffs == FRAME_TO_U64_MASK ? 0 : NV_U64_MAX << (1llu + beginOffs);
                    NvU64 curMap = pRegmap->map[i][curMapIdx] | mask;
                    frameBaseIdx = latestFree[i] + FRAME_TO_U64_MASK - beginOffs;
                    if (curMap != NV_U64_MAX)
                    {
                        goto free_found;
                    }
                    curMapIdx--;
                    frameBaseIdx -= 64;
                    while (frameBaseIdx > localStart)
                    {
                        curMap = pRegmap->map[i][curMapIdx];
                        if(curMap != NV_U64_MAX)
                        {
                            goto free_found;
                        }
                        frameBaseIdx -= 64;
                        curMapIdx--;
                    }

                    // No more free pages, exit
                    *pNumEvictablePages = numPages - curEvictPage;
                    return totalFound;
free_found:
                    // Found a free page, set frameBaseIdx and go back to the beginning of the loop
                    frameBaseIdx -= portUtilCountLeadingZeros64(~curMap);
                    frameBaseIdx = alignDownToMod(frameBaseIdx, frameAlignment, realAlign);
                    goto loop_begin;
                }
                latestFree[i] -= endOffs;
            }
        }
        else
        {
            // Loop to check if current range has an unpinned page, then it gets stored in the evictable area
            while (latestFree[i] > (frameBaseIdx - framesPerPage))
            {
                // Basically same as above loop, just not exiting if 0 not found, instead setting bEvictablePage
                NvU64 curId = latestFree[i] - 1llu;
                NvU64 curMapIdx = PAGE_MAPIDX(curId);
                NvU64 beginOffs = PAGE_BITIDX(curId);
                NvU64 mask = beginOffs == FRAME_TO_U64_MASK ? 0 : NV_U64_MAX >> (1llu + beginOffs);
                NvU64 curWithOffs = (pRegmap->map[i][curMapIdx] << (FRAME_TO_U64_MASK - beginOffs)) | mask;
                NvU64 endOffs = portUtilCountLeadingZeros64(curWithOffs);
                latestFree[i] -= endOffs;
                if (endOffs == 0)
                {
                    bEvictablePage = NV_TRUE;
                    break;
                }
            }
        }
    }

    frameBaseIdx -= framesPerPage;

    // Store evictable pages at end of array to not interfere with free pages
    if (bEvictablePage)
    {
        curEvictPage--;
        pPages[curEvictPage] = frameBaseIdx;
        goto loop_begin;
    }

    pPages[totalFound] = frameBaseIdx;
    totalFound++;

    // Found all needed pages (all free and not STATE_UNPIN)
    if (totalFound == numPages)
    {
        *pNumEvictablePages = 0;
        return numPages;
    }

    goto loop_begin;
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
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 stride,
    NvU32 strideStart,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    NvU64 numFrames, localStart, localEnd, framesPerPage;
    NvU64 frameAlignment, alignedAddrBase, frameAlignmentPadding, localStride;
    NvU64 strideAlignmentPadding = 0, strideRegionAlignmentPadding = 0;
    NvS64 frameFound;
    PMA_REGMAP *pRegmap = (PMA_REGMAP *)pMap;

    framesPerPage = pageSize >> PMA_PAGE_SHIFT;
    numFrames = framesPerPage * numPages;
    frameAlignment = alignment >> PMA_PAGE_SHIFT;
    localStride = stride >> PMA_PAGE_SHIFT;

    if (stride != 0)
    {
        if (bReverseAlloc)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        // assume stride is larger than alignment. Both must be pow2
        if (alignment > stride)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    //
    // Find how much is the base address short of the alignment and stride requirements
    // and adjust that value in the scanning range before starting the scan.
    //
    alignedAddrBase       = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

    if (stride != 0)
    {
        alignedAddrBase       = NV_ALIGN_UP(addrBase, stride);
        strideAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

        alignedAddrBase       = NV_ALIGN_UP(addrBase, stride * 2);
        strideRegionAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;
    }

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

    if (localStride != 0)
    {
        // Put the address into the correct strideStart
        if ((((localStart - frameAlignmentPadding) / localStride) % 2) != strideStart)
        {
            // align up to next stride
            localStart = alignUpToMod((localStart + 1), localStride, strideAlignmentPadding);
        }
    }

    if (!bReverseAlloc)
    {
        frameFound = _scanContiguousSearchLoop(pRegmap, numFrames, localStart, localEnd,
                                               frameAlignment, frameAlignmentPadding, localStride, strideStart,
                                               strideAlignmentPadding, strideRegionAlignmentPadding, NV_FALSE);
    }
    else
    {
        frameFound = _scanContiguousSearchLoopReverse(pRegmap, numFrames, localStart, localEnd,
                                                      frameAlignment, frameAlignmentPadding, NV_FALSE);
    }

    if (frameFound >= 0)
    {
        *freeList = addrBase + ((NvU64)frameFound << PMA_PAGE_SHIFT);
        *numPagesAlloc = numPages;
        return NV_OK;
    }

    *numPagesAlloc = 0;

    if (bSkipEvict) return NV_ERR_NO_MEMORY;

    // Loop back to the beginning and continue searching
    if (!bReverseAlloc)
    {
        frameFound = _scanContiguousSearchLoop(pRegmap, numFrames, localStart, localEnd,
                                               frameAlignment, frameAlignmentPadding, localStride, strideStart,
                                               strideAlignmentPadding, strideRegionAlignmentPadding, NV_TRUE);
    }
    else
    {
        frameFound = _scanContiguousSearchLoopReverse(pRegmap, numFrames, localStart, localEnd,
                                                      frameAlignment, frameAlignmentPadding, NV_TRUE);
    }

    if (frameFound >= 0)
    {
        *freeList = addrBase + ((NvU64)frameFound << PMA_PAGE_SHIFT);
        return NV_ERR_IN_USE;
    }
    else
    {
        return NV_ERR_NO_MEMORY;
    }
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
    NvU64 pageSize,
    NvU64 alignment,
    NvU64 stride,
    NvU32 strideStart,
    NvU64 *numPagesAlloc,
    NvBool bSkipEvict,
    NvBool bReverseAlloc
)
{
    PMA_REGMAP *pRegmap = (PMA_REGMAP*) pMap;
    NvU64 localStart, localEnd, framesPerPage, alignedAddrBase, frameAlignmentPadding, localStride;
    NvU64 strideAlignmentPadding = 0, strideRegionAlignmentPadding = 0;
    NvU64 freeFound = 0, evictFound = 0;
    NvU64 totalFound = 0;
    NV_STATUS status = NV_OK;
    NvU64 i;

    NV_ASSERT(alignment == pageSize);

    framesPerPage = pageSize >> PMA_PAGE_SHIFT;
    localStride = stride >> PMA_PAGE_SHIFT;

    if (stride != 0)
    {
        if (bReverseAlloc)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        // assume stride is larger than alignment. Both must be pow2
        if (alignment > stride)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    //
    // Find how much is the base address short of the alignment and stride requirements
    // and adjust that value in the scanning range before starting the scan.
    //
    alignedAddrBase       = NV_ALIGN_UP(addrBase, alignment);
    frameAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

    if (stride != 0)
    {
        alignedAddrBase       = NV_ALIGN_UP(addrBase, stride);
        strideAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;

        alignedAddrBase       = NV_ALIGN_UP(addrBase, stride * 2);
        strideRegionAlignmentPadding = (alignedAddrBase - addrBase) >> PMA_PAGE_SHIFT;
    }

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
        localEnd   = pRegmap->totalFrames - 1;
    }

    //
    // Do the actual scanning here. The scanning functions return free pages at the beginning of
    // the array, and evictable pages in reverse order at the end of the array
    //
    if (!bReverseAlloc)
    {
        freeFound = _scanDiscontiguousSearchLoop(pRegmap, numPages, framesPerPage,
            localStart, localEnd, alignment >> PMA_PAGE_SHIFT,
            frameAlignmentPadding, localStride, strideStart, strideAlignmentPadding,
            strideRegionAlignmentPadding,
            freeList, &evictFound);
    }
    else
    {
        freeFound = _scanDiscontiguousSearchLoopReverse(pRegmap, numPages, framesPerPage,
            localStart, localEnd, alignment >> PMA_PAGE_SHIFT,
            frameAlignmentPadding, freeList, &evictFound);
    }

    *numPagesAlloc = freeFound;

    // Scanning implementations don't actually decrement evictFound, so adjust appropriately here
    evictFound = freeFound + evictFound > numPages ? numPages - freeFound : evictFound;

    // Not enough pages
    if (((freeFound + evictFound) != numPages) ||
        (bSkipEvict && (freeFound != numPages)))
    {
        status = NV_ERR_NO_MEMORY;
    }
    else if (evictFound != 0)
    {
        status = NV_ERR_IN_USE;
    }

    // Set totalFound appropriately to shift pages at the end of the function
    totalFound = freeFound + evictFound;
    if (bSkipEvict)
    {
        totalFound = freeFound;
        goto alignAndReturn;
    }

    // End of list contains the evictable pages, swap elements from beginning of range to end
    for (i = (numPages - freeFound) >> 1; i != 0; i--)
    {
        NvU64 temp = freeList[freeFound + i - 1llu];
        freeList[freeFound + i - 1llu] = freeList[numPages - i];
        freeList[numPages - i] = temp;
    }

alignAndReturn:
    while (totalFound != 0)
    {
        totalFound--;
        freeList[totalFound] <<= PMA_PAGE_SHIFT;
        freeList[totalFound] += addrBase;
    }
    return status;
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

        if (maxZerosGet(bitmap) == FRAME_TO_U64_SIZE)
        {
            mapTrailZeros += FRAME_TO_U64_SIZE;
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

