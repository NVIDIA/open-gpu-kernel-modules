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
    mapInitIntrusive(&(pReuseMappingDb->reverseMap));
    mapInit(&(pReuseMappingDb->forwardMap), pAllocator);
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
    mapDestroy(&(pReuseMappingDb->reverseMap));
    mapDestroy(&(pReuseMappingDb->forwardMap));
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
    MemoryRange range
)
{
    return;
}

/*!
 * @brief   Initialize the mapping reuse object
 *
 * @param[in]   pReuseMappingDb  Pointer to reuse mapping object
 * @param[in]   pAllocCtx    Context for a given mapping, for this particular call, passed into the map/unmap callbacks.
                             This is cached with the mapping offset for reuse. 
 * @param[in]   range        Range within that allocation context to map.
 * @param[out]  pMemoryArea  MemoryArea representing (potentially cached) ranges returned from this database
 * @param[in]   cachingFlags Caching flags used for this mapping (ie REUSE_ANY_RANGES, REUSE_NONE, REUSE_SINGLE_RANGE)
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
    return NV_ERR_NOT_SUPPORTED;
}