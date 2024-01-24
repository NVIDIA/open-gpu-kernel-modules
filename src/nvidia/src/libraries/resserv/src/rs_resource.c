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

#define NVOC_RS_RESOURCE_H_PRIVATE_ACCESS_ALLOWED

#include "nvlog_inc.h"
#include "resserv/resserv.h"
#include "resserv/rs_resource.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"

#if !(RS_STANDALONE)
#include "os/os.h"
#endif

NV_STATUS
resConstruct_IMPL
(
    RsResource *pResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pResourceRef;

    if (pCallContext == NULL)
    {
        return NV_OK;
    }

    pResourceRef = pCallContext->pResourceRef;

    pResource->bConstructed = NV_TRUE;

    // Init pResourceRef->pResource so iteration APIs work during ctor
    pResourceRef->pResource = pResource;

    // Init back-ref so we can use during ctor
    pResource->pResourceRef = pResourceRef;

    // Set context for free in case a chained constructor fails.
    resSetFreeParams(pResource, pCallContext, NULL);

    // NV_PRINTF(LEVEL_INFO, "Constructing resource with external class: 0x%x\n", pParams->externalClassId);

    return NV_OK;
}

void
resPreDestruct_IMPL
(
    RsResource *pResource
)
{
}

void
resDestruct_IMPL
(
    RsResource *pResource
)
{
    if (!pResource->bConstructed)
    {
        return;
    }

    // NV_PRINTF(LEVEL_INFO, "Freeing resource: " NvP64_fmt "\n", NV_PTR_TO_NvP64(pResource));
}

NV_STATUS
resSetFreeParams_IMPL(RsResource *pResource, CALL_CONTEXT *pCallContext, RS_RES_FREE_PARAMS_INTERNAL *pParams)
{
    if (!pResource->bConstructed)
    {
        return NV_OK;
    }

    pResource->dtorParams.pFreeContext = pCallContext;
    pResource->dtorParams.pFreeParams = pParams;

    return NV_OK;
}

NV_STATUS
resGetFreeParams_IMPL(RsResource *pResource, CALL_CONTEXT **ppCallContext, RS_RES_FREE_PARAMS_INTERNAL **ppParams)
{
    if (ppCallContext != NULL)
        *ppCallContext = pResource->dtorParams.pFreeContext;

    if (ppParams != NULL)
        *ppParams = pResource->dtorParams.pFreeParams;

    return NV_OK;
}

NV_STATUS resControlLookup_IMPL
(
    RsResource                     *pResource,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams,
    const struct NVOC_EXPORTED_METHOD_DEF **ppEntry
)
{
    const struct NVOC_EXPORTED_METHOD_DEF *pEntry;
    NvU32 cmd = pRsParams->cmd;

    *ppEntry = NULL;
    pEntry = objGetExportedMethodDef(staticCast(objFullyDerive(pResource), Dynamic), cmd);

    if (pEntry == NULL)
        return NV_ERR_NOT_SUPPORTED;

    if ((pEntry->paramSize != 0) && (pRsParams->paramsSize != pEntry->paramSize))
    {
        NV_PRINTF(LEVEL_NOTICE,
                "hObject 0x%08x, cmd 0x%08x: bad paramsize %d, expected %d\n",
                RES_GET_HANDLE(pResource), pRsParams->cmd,
                (int)pRsParams->paramsSize,
                (int)pEntry->paramSize);

        return NV_ERR_INVALID_PARAM_STRUCT;
    }

    *ppEntry = pEntry;
    return NV_OK;
}

typedef NV_STATUS (*CONTROL_EXPORT_FNPTR)(void*, void*);
typedef NV_STATUS (*CONTROL_EXPORT_FNPTR_NO_PARAMS)(void*);

NV_STATUS
resControl_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams
)
{
    RsServer *pServer = pCallContext->pServer;
    const struct NVOC_EXPORTED_METHOD_DEF   *pEntry;
    NV_STATUS status;
    Dynamic *pDynamicObj;
    NvU32 releaseFlags = 0;
    LOCK_ACCESS_TYPE access = LOCK_ACCESS_WRITE;

    status = resControlLookup(pResource, pRsParams, &pEntry);
    if (status != NV_OK)
    {
        if (status == NV_WARN_NOTHING_TO_DO)
            return NV_OK;
        return status;
    }

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_NOT_SUPPORTED);

    // Initialize the execution cookie
    serverControl_InitCookie(pEntry, pRsParams->pCookie);

    status = resControlFilter(pResource, pCallContext, pRsParams);
    if (status != NV_OK)
        return status;

    status = serverControl_Prologue(pServer, pRsParams, &access, &releaseFlags);
    if (status != NV_OK)
        return status;

    status = resControlSerialization_Prologue(pResource, pCallContext, pRsParams);
    if (status != NV_OK)
        goto done;

    status = resControl_Prologue(pResource, pCallContext, pRsParams);
    if ((status != NV_OK) && (status != NV_WARN_NOTHING_TO_DO))
        goto done;

    pDynamicObj = objDynamicCastById(pResource, pEntry->pClassInfo->classId);

    if (status == NV_WARN_NOTHING_TO_DO)
    {
        // Call handled by the prologue.
        status = NV_OK;
    }
    else
    {
        // Check the size of paramSize while it is non-zero.
        // Zero size means the exported method only have one param (pResource)
        if (pEntry->paramSize == 0)
        {
            CONTROL_EXPORT_FNPTR_NO_PARAMS pFunc = ((CONTROL_EXPORT_FNPTR_NO_PARAMS) pEntry->pFunc);
            status = pFunc(pDynamicObj);
        }
        else
        {
            CONTROL_EXPORT_FNPTR pFunc = ((CONTROL_EXPORT_FNPTR) pEntry->pFunc);

            status = pFunc(pDynamicObj, pRsParams->pParams);
        }
    }

    resControl_Epilogue(pResource, pCallContext, pRsParams);

done:
    resControlSerialization_Epilogue(pResource, pCallContext, pRsParams);
    status = serverControl_Epilogue(pServer, pRsParams, access, &releaseFlags, status);

    return status;
}

NV_STATUS
resControlFilter_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
resControlSerialization_Prologue_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

void
resControlSerialization_Epilogue_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
}

NV_STATUS
resControl_Prologue_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

void
resControl_Epilogue_IMPL
(
    RsResource                     *pResource,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    return;
}

NvU32 resGetRefCount_IMPL
(
    RsResource *pResource
)
{
    return 1;
}

NV_STATUS
resMap_IMPL
(
    RsResource *pResource,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
resUnmap_IMPL
(
    RsResource *pResource,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
resMapTo_IMPL
(
    RsResource          *pResource,
    RS_RES_MAP_TO_PARAMS *pParams
)
{
    return NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
resUnmapFrom_IMPL
(
    RsResource              *pResource,
    RS_RES_UNMAP_FROM_PARAMS *pParams
)
{
    return NV_ERR_INVALID_OBJECT_HANDLE;
}

NvBool
resCanCopy_IMPL
(
    RsResource *pResource
)
{
    return NV_FALSE;
}

NV_STATUS
resIsDuplicate_IMPL
(
    RsResource *pResource,
    NvHandle    hMemory,
    NvBool     *pDuplicate
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NvBool
resAccessCallback_IMPL
(
    RsResource *pResource,
    RsClient *pInvokingClient,
    void *pAllocParams,
    RsAccessRight accessRight
)
{
    return NV_FALSE;
}

NvBool
resShareCallback_IMPL
(
    RsResource *pResource,
    RsClient *pInvokingClient,
    RsResourceRef *pParentRef,
    RS_SHARE_POLICY *pSharePolicy
)
{
    switch (pSharePolicy->type)
    {
        case RS_SHARE_TYPE_ALL:
            return NV_TRUE;
        case RS_SHARE_TYPE_CLIENT:
            if (pSharePolicy->target == pInvokingClient->hClient)
                return NV_TRUE;
            break;
    }

    return NV_FALSE;
}

NV_STATUS
refFindCpuMapping
(
    RsResourceRef *pResourceRef,
    NvP64 pAddress,
    RsCpuMapping **ppMapping
)
{
    return refFindCpuMappingWithFilter(pResourceRef, pAddress, NULL, ppMapping);
}

NV_STATUS
refFindCpuMappingWithFilter
(
    RsResourceRef *pResourceRef,
    NvP64 pAddress,
    NvBool (*fnFilter)(RsCpuMapping*),
    RsCpuMapping **ppMapping
)
{
    RsCpuMappingListIter it;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;
    RsCpuMapping *pMapping = NULL;

    if (pResourceRef == NULL)
    {
        NV_ASSERT(0);
        return status;
    }

    it = listIterAll(&pResourceRef->cpuMappings);
    while (listIterNext(&it))
    {
        pMapping = it.pValue;
        if ((pMapping->pLinearAddress == pAddress) &&
            ((fnFilter == NULL) || fnFilter(pMapping)))
        {
            status = NV_OK;
            break;
        }
    }

    if (status != NV_OK)
        pMapping = NULL;

    if (pMapping != NULL)
        *ppMapping = pMapping;

    return status;
}

NV_STATUS
refFindChildOfType
(
    RsResourceRef *pParentRef,
    NvU32 internalClassId,
    NvBool bExactMatch,
    RsResourceRef **ppResourceRef
)
{
    if (bExactMatch)
    {
        RsIndexIter it = indexRefIter(&pParentRef->childRefMap, internalClassId);
        if (indexRefIterNext(&it))
        {
            RsResourceRef *pResourceRef = *it.pValue;

            if (ppResourceRef != NULL)
                *ppResourceRef = pResourceRef;

            return NV_OK;
        }
    }
    else
    {
        RsIndexSupermapIter it = multimapSubmapIterAll(&pParentRef->childRefMap);
        while (multimapSubmapIterNext(&it))
        {
            RsIndexSubmap *pSubmap = it.pValue;
            RsIndexIter subIt = multimapSubmapIterItems(&pParentRef->childRefMap, pSubmap);
            if (multimapItemIterNext(&subIt))
            {
                RsResourceRef *pResourceRef = *subIt.pValue;

                if (objDynamicCastById(pResourceRef->pResource, internalClassId) == NULL)
                    continue;

                if (ppResourceRef != NULL)
                    *ppResourceRef = pResourceRef;

                return NV_OK;
            }
        }

    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
refFindAncestorOfType
(
    RsResourceRef *pDescendantRef,
    NvU32 internalClassId,
    RsResourceRef **ppAncestorRef
)
{
    RsResourceRef *pAncestorRef = pDescendantRef->pParentRef;

    while (pAncestorRef != NULL)
    {
        if (pAncestorRef->internalClassId == internalClassId)
        {
            if(pAncestorRef->bInvalidated)
                return NV_ERR_OBJECT_NOT_FOUND;

            if (ppAncestorRef != NULL)
                *ppAncestorRef = pAncestorRef;

            return NV_OK;
        }

        pAncestorRef = pAncestorRef->pParentRef;
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NvBool
refHasAncestor
(
    RsResourceRef *pDescendantRef,
    RsResourceRef *pAncestorRef
)
{
    RsResourceRef *pSearchRef = pDescendantRef->pParentRef;

    while (pSearchRef != NULL)
    {
        if (pSearchRef == pAncestorRef)
            return NV_TRUE;

        pSearchRef = pSearchRef->pParentRef;
    }

    return NV_FALSE;
}

NV_STATUS
refAddMapping
(
    RsResourceRef *pResourceRef,
    RS_CPU_MAP_PARAMS *pParams,
    RsResourceRef *pContextRef,
    RsCpuMapping **ppMapping
)
{
    NV_STATUS status;
    RsCpuMapping *pCpuMapping = listAppendNew(&pResourceRef->cpuMappings);
    if (pCpuMapping == NULL)
        return NV_ERR_NO_MEMORY;

    status = refAllocCpuMappingPrivate(pParams, pCpuMapping);
    if (status != NV_OK)
    {
        listRemove(&pResourceRef->cpuMappings, pCpuMapping);
        return status;
    }

    if ((pContextRef != NULL) &&
        (pContextRef != pResourceRef) &&
        !refHasAncestor(pResourceRef, pContextRef))
    {
        RS_CPU_MAPPING_BACK_REF *pBackRefItem = listAppendNew(&pContextRef->backRefs);
        if (pBackRefItem == NULL)
        {
            refFreeCpuMappingPrivate(pCpuMapping);
            listRemove(&pResourceRef->cpuMappings, pCpuMapping);
            return NV_ERR_NO_MEMORY;
        }

        pBackRefItem->pBackRef = pResourceRef;
        pBackRefItem->pCpuMapping = pCpuMapping;
    }

    pCpuMapping->offset = pParams->offset;
    pCpuMapping->length = pParams->length;
    pCpuMapping->flags = pParams->flags;
    pCpuMapping->pContextRef = pContextRef;

    if (ppMapping != NULL)
        *ppMapping = pCpuMapping;

    return NV_OK;
}

void
refRemoveMapping
(
    RsResourceRef *pResourceRef,
    RsCpuMapping *pCpuMapping
)
{
    if ((pCpuMapping->pContextRef != NULL) &&
        !refHasAncestor(pResourceRef, pCpuMapping->pContextRef))
    {
        RS_CPU_MAPPING_BACK_REF *pBackRefItem;
        RsCpuMappingBackRefListIter it = listIterAll(&pCpuMapping->pContextRef->backRefs);

        while (listIterNext(&it))
        {
            pBackRefItem = it.pValue;
            if ((pBackRefItem->pBackRef == pResourceRef) &&
                (pBackRefItem->pCpuMapping == pCpuMapping))
            {
                listRemove(&pCpuMapping->pContextRef->backRefs, pBackRefItem);
                break;
            }
        }
    }

    refFreeCpuMappingPrivate(pCpuMapping);
    listRemove(&pResourceRef->cpuMappings, pCpuMapping);
}

#if RS_STANDALONE
NV_STATUS
refAllocCpuMappingPrivate
(
    RS_CPU_MAP_PARAMS *pMapParams,
    RsCpuMapping *pCpuMapping
)
{
    return NV_OK;
}

void
refFreeCpuMappingPrivate
(
    RsCpuMapping *pCpuMapping
)
{
}
#endif /* RS_STANDALONE */

NV_STATUS
refAddInterMapping
(
    RsResourceRef *pMapperRef,
    RsResourceRef *pMappableRef,
    RsResourceRef *pContextRef,
    RsInterMapping **ppMapping
)
{
    RsInterMapping *pInterMapping;
    RS_INTER_MAPPING_BACK_REF *pBackRefItem;
    RS_INTER_MAPPING_BACK_REF *pContextBackRefItem;

    NV_ASSERT(pMapperRef != NULL);
    NV_ASSERT(pMappableRef != NULL);
    NV_ASSERT(pMappableRef != pMapperRef);

    pInterMapping = listAppendNew(&pMapperRef->interMappings);
    if (pInterMapping == NULL)
        return NV_ERR_NO_MEMORY;

    // Add backref linked to this inter-mapping
    pBackRefItem = listAppendNew(&pMappableRef->interBackRefs);
    if (pBackRefItem == NULL)
    {
        listRemove(&pMapperRef->interMappings, pInterMapping);
        return NV_ERR_NO_MEMORY;
    }

    pBackRefItem->pMapperRef = pMapperRef;
    pBackRefItem->pMapping = pInterMapping;

    //
    // Either pMapperRef or pMappableRef should be a descendant of pContextRef
    // Otherwise, it becomes possible to have a stale reference if hContext is freed first
    // If this is not the case, add a backref to pContextRef as well
    //
    if (!refHasAncestor(pMapperRef, pContextRef) &&
        !refHasAncestor(pMappableRef, pContextRef))
    {
        pContextBackRefItem = listAppendNew(&pContextRef->interBackRefs);
        if (pContextBackRefItem == NULL)
        {
            listRemove(&pMapperRef->interMappings, pInterMapping);
            listRemove(&pMappableRef->interBackRefs, pBackRefItem);
            return NV_ERR_NO_MEMORY;
        }

        pContextBackRefItem->pMapperRef = pMapperRef;
        pContextBackRefItem->pMapping = pInterMapping;
    }

    pInterMapping->pMappableRef = pMappableRef;
    pInterMapping->pContextRef = pContextRef;

    if (ppMapping != NULL)
        *ppMapping = pInterMapping;

    return NV_OK;
}

void
refRemoveInterMapping
(
    RsResourceRef *pMapperRef,
    RsInterMapping *pMapping
)
{
    RsInterMappingBackRefListIter it;
    RS_INTER_MAPPING_BACK_REF *pBackRefItem = NULL;
    RsResourceRef *pMappableRef = pMapping->pMappableRef;
    RsResourceRef *pContextRef = pMapping->pContextRef;

    // Find and remove the mappable's backref linked to this inter-mapping
    it = listIterAll(&pMappableRef->interBackRefs);
    while (listIterNext(&it))
    {
        pBackRefItem = it.pValue;
        if (pBackRefItem->pMapping == pMapping)
        {
            listRemove(&pMappableRef->interBackRefs, pBackRefItem);
            break;
        }
    }

    // Find and remove the context's backref linked to this inter-mapping, if present
    it = listIterAll(&pContextRef->interBackRefs);
    while (listIterNext(&it))
    {
        pBackRefItem = it.pValue;
        if (pBackRefItem->pMapping == pMapping)
        {
            listRemove(&pContextRef->interBackRefs, pBackRefItem);
            break;
        }
    }

    listRemove(&pMapperRef->interMappings, pMapping);
}

NV_STATUS
refCacheRef
(
    RsResourceRef *pParentRef,
    RsResourceRef *pResourceRef
)
{
    return indexAdd(&pParentRef->cachedRefMap, pResourceRef->internalClassId, pResourceRef);
}

NV_STATUS
refUncacheRef
(
    RsResourceRef *pParentRef,
    RsResourceRef *pResourceRef
)
{
    return indexRemove(&pParentRef->cachedRefMap, pResourceRef->internalClassId, pResourceRef);
}

NV_STATUS
refAddDependant
(
    RsResourceRef *pResourceRef,
    RsResourceRef *pDependantRef
)
{
    NV_STATUS status;

    // dependencies are implicit between a parent resource reference and child resource reference
    if (refHasAncestor(pDependantRef, pResourceRef))
        return NV_OK;

    status = indexAdd(&pDependantRef->depBackRefMap, pResourceRef->internalClassId, pResourceRef);
    if (status != NV_OK)
        return status;

    return indexAdd(&pResourceRef->depRefMap, pDependantRef->internalClassId, pDependantRef);
}

void
refRemoveDependant
(
    RsResourceRef *pResourceRef,
    RsResourceRef *pDependantRef
)
{
    indexRemove(&pDependantRef->depBackRefMap, pResourceRef->internalClassId, pResourceRef);
    indexRemove(&pResourceRef->depRefMap, pDependantRef->internalClassId, pDependantRef);
}

NvBool
refPendingFree
(
    RsResourceRef *pResourceRef,
    RsClient *pClient
)
{
    return ((pResourceRef->freeNode.pNext != NULL) ||
            (pResourceRef->freeNode.pPrev != NULL) ||
            (pResourceRef == listHead(&pClient->pendingFreeList)));
}

void
resAddAdditionalDependants_IMPL
(
    RsClient *pClient,
    RsResource *pResource,
    RsResourceRef *pReference
)
{
    return;
}
