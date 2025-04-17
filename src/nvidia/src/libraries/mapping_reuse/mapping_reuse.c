/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mapping_reuse/mapping_reuse.h"


static NV_STATUS _reusemappingdbAddMappingCallback(void *, NvU64, NvU64, NvU64);

/*!
 * @brief   Initialize the mapping reuse object
 *
 * @param[in]   pReuseMappingDb  Pointer to reuse mapping object
 * @param[in]   pAllocator  Pointer to PORT_MEM_ALLOCATOR to use internally
 * @param[in]   pGlobalCtx  Pointer that is passed into callbacks, is constant for all calls of callback
 * @param[in]   pMapCb      Callback called when we need a new mapping not present in caching structures
 * @param[in]   pUnmapCb    Callback called when refcount of a given cached mapping becomes 0
 * @param[in]   pSplitCb    Callback called when we need to split a given mapping due to partial map
 */
void
reusemappingdbInit
(
    ReuseMappingDb *pReuseMappingDb,
    PORT_MEM_ALLOCATOR *pAllocator,
    void *pGlobalCtx,
    ReuseMappingDbMapFunction pMapCb,
    ReuseMappingDbUnnmapFunction pUnmapCb,
    ReuseMappingDbSplitMappingFunction pSplitCb
)
{
    mapInitIntrusive(&(pReuseMappingDb->virtualMap));
    mapInit(&(pReuseMappingDb->allocCtxPhysicalMap), pAllocator);
    pReuseMappingDb->pGlobalCtx = pGlobalCtx;
    pReuseMappingDb->pAllocator = pAllocator;
    pReuseMappingDb->pMapCb = pMapCb;
    pReuseMappingDb->pUnmapCb = pUnmapCb;
    pReuseMappingDb->pSplitCb = pSplitCb;
}

/*!
 * @brief   Destroy the mapping reuse object
 *
 * @param[in]   pReuseMappingDb  Pointer to reuse mapping object
 */
void
reusemappingdbDestruct
(
    ReuseMappingDb *pReuseMappingDb
)
{
    mapDestroy(&(pReuseMappingDb->virtualMap));
    mapDestroy(&(pReuseMappingDb->allocCtxPhysicalMap));
    portMemSet(pReuseMappingDb, 0, sizeof(*pReuseMappingDb));
}

/*!
 * @brief   Unmap a range returned from a previous map calll
 *
 * @param[in]   pReuseMappingDb  Pointer to reuse mapping object
 * @param[in]   range            Range returned from previous map call. Only actually unmapped when refcnt is 0
 */
void
reusemappingdbUnmap
(
    ReuseMappingDb *pReuseMappingDb,
    void       *pAllocCtx,
    MemoryRange range
)
{
    ReuseMappingDbEntry *pEntry = mapFindGEQ(&(pReuseMappingDb->virtualMap), range.start);
    NvU64 curOffset = range.start;
    NvBool bFirstRange = NV_TRUE;

    while (pEntry != NULL)
    {
        ReuseMappingDbEntry *pNextEntry = mapNext(&(pReuseMappingDb->virtualMap), pEntry);
        NvU64 revOffset = mapKey(&(pReuseMappingDb->virtualMap), pEntry);
        MemoryRange revRange = mrangeMake(revOffset, pEntry->size);
        MemoryRange diffRange = mrangeMake(curOffset, revOffset - curOffset);

        // Only unmap ranges contained within the desired unmap range
        if (!mrangeContains(range, revRange))
        {
            if (bFirstRange)
            {
                break;
            }
            return;
        }

        bFirstRange = NV_FALSE;
        curOffset = mrangeLimit(revRange);

        // Unmap any partial range not tracked by data structure
        if (diffRange.size != 0)
        {
            pReuseMappingDb->pUnmapCb(pReuseMappingDb->pGlobalCtx, pAllocCtx, diffRange);
        }

        // Remove the range tracked by the data structure
        pEntry->refCount--;
        if (pEntry->refCount == 0)
        {
            // Only remove entry and unmap if refCount is 0.
            void *pEntryAllocCtx = pEntry->trackingInfo.pAllocCtx;
            ReuseMappingDbPhysicalMap *pPhysicalMap = mapFind(&(pReuseMappingDb->allocCtxPhysicalMap),
                (NvU64) pEntryAllocCtx);

            mapRemove(&(pReuseMappingDb->virtualMap), pEntry);
            mapRemove(pPhysicalMap, pEntry);

            pReuseMappingDb->pUnmapCb(pReuseMappingDb->pGlobalCtx, pEntryAllocCtx, revRange);
            PORT_FREE(pReuseMappingDb->pAllocator, pEntry);
        }

        pEntry = pNextEntry;
    }

    // Take care of any overhang.
    if (mrangeLimit(range) != curOffset)
    {
        MemoryRange diffRange = mrangeMake(curOffset, mrangeLimit(range) - curOffset);
        pReuseMappingDb->pUnmapCb(pReuseMappingDb->pGlobalCtx, pAllocCtx, diffRange);
    }
}

typedef struct ReuseMappingDbToken
{
    ReuseMappingDbEntry *pList;
    ReuseMappingDb *pDb;
    NvU64 numNewEntries;
} ReuseMappingDbToken;

//
// This callback appends a new mapping entry to the pending linked list. The caller calls this with
// an opaque token which is the head of the list as well as the physical and virtual offsets, and 
// this function attaches the entry to the head of the list.
//
static NV_STATUS
_reusemappingdbAddMappingCallback
(
    void *pToken,
    NvU64 physicalOffset,
    NvU64 virtualOffset,
    NvU64 size
)
{
    ReuseMappingDbToken *pRealToken = (ReuseMappingDbToken *) pToken;
    ReuseMappingDbEntry *pEntry = (ReuseMappingDbEntry *) PORT_ALLOC(pRealToken->pDb->pAllocator,
        sizeof(ReuseMappingDbEntry));
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_NO_MEMORY);

    pEntry->size = size;
    pEntry->refCount = 1;
    pEntry->newMappingNode.pNextEntry = pRealToken->pList;
    pEntry->newMappingNode.physicalOffset = physicalOffset;
    pEntry->newMappingNode.virtualOffset = virtualOffset;

    pRealToken->pList = pEntry;
    pRealToken->numNewEntries++;

    return NV_OK;
}

/*!
 * @brief   Initialize the mapping reuse object
 *
 * @param[in]   pReuseMappingDb  Pointer to reuse mapping object
 * @param[in]   pAllocCtx    Context for a given mapping, for this particular call, passed into the map/unmap callbacks.
                             This is cached with the mapping offset for reuse. 
 * @param[in]   range        Range within that allocation context to map.
 * @param[out]  pMemoryArea  MemoryArea representing (potentially cached) ranges returned from this database
 * @param[in]   cachingFlags Caching flags used for this mapping (ie _SINGLE_RANGE, _NO_REUSE)
 */
NV_STATUS
reusemappingdbMap
(
    ReuseMappingDb *pReuseMappingDb,
    void *pAllocCtx,
    MemoryRange range,
    MemoryArea *pMemoryArea,
    NvU64 cachingFlags
)
{
    ReuseMappingDbPhysicalMap *pPhysicalMap;
    ReuseMappingDbToken token;
    NvBool bNoReuse = !!(cachingFlags & REUSE_MAPPING_DB_MAP_FLAGS_NO_REUSE);
    NvBool bSingleRange = !!(cachingFlags & REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE);
    NvBool bAddToMap = !bNoReuse;
    NV_STATUS status = NV_OK;

    // TODO: Remove when we support multi-range reuse
    NV_ASSERT_OR_RETURN( bSingleRange || bNoReuse, NV_ERR_NOT_SUPPORTED);

    pPhysicalMap = mapFind(&(pReuseMappingDb->allocCtxPhysicalMap), (NvU64) pAllocCtx);
    
    // We don't currently have any mappings for this alloc context, create new map
    if (pPhysicalMap == NULL)
    {
        pPhysicalMap = mapInsertNew(&(pReuseMappingDb->allocCtxPhysicalMap), (NvU64) pAllocCtx);
        mapInitIntrusive(pPhysicalMap);
        NV_ASSERT_OR_RETURN(pPhysicalMap != NULL, NV_ERR_NO_MEMORY);
    }

    if (!bNoReuse && bSingleRange)
    {
        ReuseMappingDbEntry *pEntry = mapFindLEQ(pPhysicalMap, range.start);
        // If no range LEQ, then try GEQ
        if (pEntry == NULL)
        {
            pEntry = mapFindGEQ(pPhysicalMap, range.start);
        }
        if (pEntry != NULL)
        {
            NvU64 physicalOffset = mapKey(pPhysicalMap, pEntry);
            MemoryRange physRange = mrangeMake(physicalOffset, pEntry->size);

            // LEQ returned a resultant range before current range
            if(!mrangeIntersects(physRange, range))
            {
                pEntry = mapNext(pPhysicalMap, pEntry);
            }
            if (pEntry != NULL)
            {
                // We at least now have an entry thats not before the desired range.
                physicalOffset = mapKey(pPhysicalMap, pEntry);
                physRange = mrangeMake(physicalOffset, pEntry->size);
                NvU64 virtualOffset = mapKey(&(pReuseMappingDb->virtualMap), pEntry);

                // Do another intersection check becasue the range might be after the desired range
                if (mrangeIntersects(physRange, range))
                {
                    bAddToMap = NV_FALSE;
                    // Only return exact match
                    if (physRange.start == range.start && physRange.size == range.size)
                    {
                        pMemoryArea->numRanges = 1;
                        pEntry->refCount++;
                        pMemoryArea->pRanges = PORT_ALLOC(pReuseMappingDb->pAllocator, sizeof(MemoryRange));
                        NV_ASSERT_OR_RETURN(pMemoryArea->pRanges != NULL, NV_ERR_NO_MEMORY);
                        pMemoryArea->pRanges[0] = mrangeMake(virtualOffset, range.size);
                        return NV_OK;
                    }
                }
            }
        }
    }
    // Initialize linked list of new entries
    token.numNewEntries = 0;
    token.pDb = pReuseMappingDb;
    token.pList = NULL;

    // Get new mappings, added to linked list
    NV_ASSERT_OK_OR_GOTO(status, pReuseMappingDb->pMapCb(pReuseMappingDb->pGlobalCtx, pAllocCtx,
                                     range, cachingFlags, &token, _reusemappingdbAddMappingCallback), err_unmap);
    
    pMemoryArea->pRanges = PORT_ALLOC(pReuseMappingDb->pAllocator, sizeof(MemoryRange) * token.numNewEntries);
    pMemoryArea->numRanges = 0;
    
    NV_ASSERT_TRUE_OR_GOTO(status, pMemoryArea->pRanges != NULL, NV_ERR_NO_MEMORY, err_unmap);

    // Now append the mappings  to the result memory area
    while (token.pList != NULL)
    {
        ReuseMappingDbEntry *pEntry = token.pList;
        NvU64 physicalOffset = pEntry->newMappingNode.physicalOffset;
        NvU64 virtualOffset = pEntry->newMappingNode.virtualOffset;
        token.pList = pEntry->newMappingNode.pNextEntry;

        pMemoryArea->pRanges[pMemoryArea->numRanges++] = mrangeMake(virtualOffset, range.size);

        // Add data to tracking structures only if the mapped range does not overlap
        if (bAddToMap)
        {
            pEntry->trackingInfo.pAllocCtx = pAllocCtx;
            mapInsertExisting(pPhysicalMap, physicalOffset, pEntry);
            mapInsertExisting(&(pReuseMappingDb->virtualMap), virtualOffset, pEntry);
        }
        else
        {
            PORT_FREE(pReuseMappingDb->pAllocator, pEntry);
        }
    }

    return NV_OK;

err_unmap:
    // Unmap and free if we can't allocate the required space for the result array.
    while (token.pList != NULL)
    {
        void *pCur = token.pList;
        pReuseMappingDb->pUnmapCb(pReuseMappingDb->pGlobalCtx, pAllocCtx,
            mrangeMake(token.pList->newMappingNode.virtualOffset, range.size));
        token.pList = token.pList->newMappingNode.pNextEntry;
        PORT_FREE(pReuseMappingDb->pAllocator, pCur);
    }
    return status;
}
