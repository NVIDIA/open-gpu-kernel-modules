/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "containers/map.h"
#include "containers/multimap.h"
#include "nvctassert.h"
#include "nvport/sync.h"
#include "nvrm_registry.h"
#include "os/os.h"
#include "rmapi/control.h"
#include "rmapi/rmapi.h"

typedef struct
{
    void* params;
} RmapiControlCacheEntry;

MAKE_MULTIMAP(CachedCallParams, RmapiControlCacheEntry);

ct_assert(sizeof(NvHandle) <= 4);

#define CLIENT_KEY_SHIFT (sizeof(NvHandle) * 8)

static NvHandle keyToClient(NvU64 key)
{
    return (key >> CLIENT_KEY_SHIFT);
}

static NvU64 handlesToKey(NvHandle hClient, NvHandle hObject)
{
    return ((NvU64)hClient << CLIENT_KEY_SHIFT) | hObject;
}

static struct {
    /* NOTE: Size unbounded for now */
    CachedCallParams cachedCallParams;
    NvU32 mode;
    PORT_MUTEX *mtx;
} RmapiControlCache;

NvBool rmapiControlIsCacheable(NvU32 flags, NvBool isGSPClient)
{
    if (RmapiControlCache.mode == NV_REG_STR_RM_CACHEABLE_CONTROLS_ENABLE)
    {
        return !!(flags & RMCTRL_FLAGS_CACHEABLE);
    }
    if (RmapiControlCache.mode == NV_REG_STR_RM_CACHEABLE_CONTROLS_GSP_ONLY)
    {
        return (flags & RMCTRL_FLAGS_CACHEABLE) &&
               (flags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL) &&
               isGSPClient;
    }
    return NV_FALSE;
}

void rmapiControlCacheInit()
{
    RmapiControlCache.mode = NV_REG_STR_RM_CACHEABLE_CONTROLS_GSP_ONLY;

    osReadRegistryDword(NULL, NV_REG_STR_RM_CACHEABLE_CONTROLS, &RmapiControlCache.mode);
    NV_PRINTF(LEVEL_INFO, "using cache mode %d\n", RmapiControlCache.mode);

    if (RmapiControlCache.mode)
    {
        multimapInit(&RmapiControlCache.cachedCallParams, portMemAllocatorGetGlobalNonPaged());
        RmapiControlCache.mtx = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
        if (!RmapiControlCache.mtx)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to create mutex");
            RmapiControlCache.mode = NV_REG_STR_RM_CACHEABLE_CONTROLS_DISABLE;
        }
    }
}

void* rmapiControlCacheGet(NvHandle hClient, NvHandle hObject, NvU32 cmd)
{
    NV_PRINTF(LEVEL_INFO, "cache lookup for 0x%x 0x%x 0x%x\n", hClient, hObject, cmd);
    portSyncMutexAcquire(RmapiControlCache.mtx);
    RmapiControlCacheEntry* entry = multimapFindItem(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject), cmd);
    portSyncMutexRelease(RmapiControlCache.mtx);
    NV_PRINTF(LEVEL_INFO, "cache entry for 0x%x 0x%x 0x%x: entry 0x%p\n", hClient, hObject, cmd, entry);
    if (entry)
        return entry->params;
    return NULL;
}

NV_STATUS rmapiControlCacheSet
(
    NvHandle hClient,
    NvHandle hObject,
    NvU32 cmd,
    void* params,
    NvU32 paramsSize
)
{
    portSyncMutexAcquire(RmapiControlCache.mtx);
    NV_STATUS status = NV_OK;
    RmapiControlCacheEntry* entry = multimapFindItem(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject), cmd);
    CachedCallParamsSubmap* insertedSubmap = NULL;

    if (!entry)
    {
        if (!multimapFindSubmap(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject)))
        {
            insertedSubmap = multimapInsertSubmap(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject));
            if (!insertedSubmap)
            {
                status = NV_ERR_NO_MEMORY;
                goto done;
            }
        }

        entry = multimapInsertItemNew(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject), cmd);
    }

    if (!entry)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    entry->params = portMemAllocNonPaged(paramsSize);
    if (!entry->params)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portMemCopy(entry->params, paramsSize, params, paramsSize);

done:
    if (status != NV_OK)
    {
        /* To avoid leaking memory, remove the newly inserted empty submap and entry */
        if (entry)
        {
            portMemFree(entry->params);
            multimapRemoveItem(&RmapiControlCache.cachedCallParams, entry);
        }

        if (insertedSubmap)
            multimapRemoveSubmap(&RmapiControlCache.cachedCallParams, insertedSubmap);
    }

    portSyncMutexRelease(RmapiControlCache.mtx);

    return status;
}

static void freeSubmap(CachedCallParamsSubmap* submap)
{
    /* (Sub)map modification invalidates the iterator, so we have to restart */
    while (NV_TRUE)
    {
        CachedCallParamsIter it = multimapSubmapIterItems(&RmapiControlCache.cachedCallParams, submap);

        if (multimapItemIterNext(&it))
        {
            RmapiControlCacheEntry* entry = it.pValue;
            portMemFree(entry->params);
            multimapRemoveItem(&RmapiControlCache.cachedCallParams, entry);
        }
        else
        {
            break;
        }
    }
    multimapRemoveSubmap(&RmapiControlCache.cachedCallParams, submap);
}

void rmapiControlCacheFreeClient(NvHandle hClient)
{
    if (!RmapiControlCache.mode)
        return;

    portSyncMutexAcquire(RmapiControlCache.mtx);
    while (NV_TRUE)
    {
        CachedCallParamsSubmap* start = multimapFindSubmapGEQ(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, 0));
        CachedCallParamsSubmap* end = multimapFindSubmapLEQ(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, NV_U32_MAX));

        if (!start || !end ||
            keyToClient(multimapSubmapKey(&RmapiControlCache.cachedCallParams, start)) != hClient ||
            keyToClient(multimapSubmapKey(&RmapiControlCache.cachedCallParams, end)) != hClient)
        {
            break;
        }

        CachedCallParamsSupermapIter it = multimapSubmapIterRange(&RmapiControlCache.cachedCallParams, start, end);

        if (multimapSubmapIterNext(&it))
        {
            CachedCallParamsSubmap* submap = it.pValue;
            freeSubmap(submap);
        }
        else
        {
            break;
        }
    }
    portSyncMutexRelease(RmapiControlCache.mtx);
}

void rmapiControlCacheFreeObject(NvHandle hClient, NvHandle hObject)
{
    CachedCallParamsSubmap* submap;

    if (!RmapiControlCache.mode)
        return;

    portSyncMutexAcquire(RmapiControlCache.mtx);

    submap = multimapFindSubmap(&RmapiControlCache.cachedCallParams, handlesToKey(hClient, hObject));
    if (submap)
        freeSubmap(submap);

    portSyncMutexRelease(RmapiControlCache.mtx);
}

void rmapiControlCacheFree(void) {
    CachedCallParamsIter it;

    if (!RmapiControlCache.mode)
        return;

    it = multimapItemIterAll(&RmapiControlCache.cachedCallParams);
    while (multimapItemIterNext(&it))
    {
        RmapiControlCacheEntry* entry = it.pValue;
        portMemFree(entry->params);
    }

    multimapDestroy(&RmapiControlCache.cachedCallParams);
    portSyncMutexDestroy(RmapiControlCache.mtx);
}
