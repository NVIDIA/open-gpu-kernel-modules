/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions managing the ref-counted imported
 *       fabric memory
 *
 *****************************************************************************/

#define NVOC_MEM_FABRIC_IMPORT_REF_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/mem_fabric_import_ref.h"
#include "mem_mgr/mem_fabric_import_v2.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

#include "class/cl00fb.h"

NV_STATUS
memoryfabricimportedrefConstruct_IMPL
(
    MemoryFabricImportedRef      *pMemoryFabricImportedRef,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV00FB_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;
    NvHandle hClient = pParams->hClient;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    if (!rmclientIsCapableByHandle(hClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT) &&
        !rmclientIsCapableByHandle(hClient, NV_RM_CAP_EXT_FABRIC_MGMT))
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    if (pAllocParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    pMemoryFabricImportedRef->pFabricImportDesc =
        importDescriptorGetUnused((const NvUuid*) pAllocParams->exportUuid,
                                  pAllocParams->index, pAllocParams->id);
    if (pMemoryFabricImportedRef->pFabricImportDesc == NULL)
        return NV_WARN_NOTHING_TO_DO;

    return NV_OK;
}

void
memoryfabricimportedrefDestruct_IMPL
(
    MemoryFabricImportedRef *pMemoryFabricImportedRef
)
{
    memdescDestroy(pMemoryFabricImportedRef->pTempMemDesc);

    importDescriptorPutNonBlocking(pMemoryFabricImportedRef->pFabricImportDesc);
}

NvBool
memoryfabricimportedrefCanCopy_IMPL
(
    MemoryFabricImportedRef *pMemoryFabricImportedRef
)
{
    return NV_FALSE;
}

static void
_memoryfabricImportMemDescDestroyCallback
(
    OBJGPU            *pUnused,
    void              *pObject,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    FABRIC_IMPORT_MEMDESC_DATA *pMemdescData =
                    (FABRIC_IMPORT_MEMDESC_DATA *)memdescGetMemData(pMemDesc);

    if (pMemdescData != NULL)
    {
        portMemFree(pMemDesc->_pMemData);
        pMemDesc->_pMemData = NULL;
    }

    portMemFree(pObject);
}

MEMORY_DESCRIPTOR*
_createTempMemDesc
(
    NV_FABRIC_MEMORY_ATTRS   *pAttrs,
    NV_PHYSICAL_MEMORY_ATTRS *pPhysAttrs,
    NvU32                     memFlags,
    NvBool                    bConting
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    FABRIC_IMPORT_MEMDESC_DATA *pMemdescData = NULL;
    MEM_DESC_DESTROY_CALLBACK *pCallback = NULL;
    NV_STATUS status;

    status = memdescCreate(&pMemDesc, NULL, pAttrs->size, 0,
                           bConting,  ADDR_FABRIC_V2,
                           NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
        return NULL;

    memdescSetPageSize(pMemDesc, AT_GPU, pAttrs->pageSize);

    pMemDesc->_pteKind = pAttrs->kind;
    memdescSetFlag(pMemDesc, MEMDESC_FLAGS_SET_KIND, NV_TRUE);

    // Allocate memory for memory fabric memdesc private data.
    pMemdescData = portMemAllocNonPaged(sizeof(FABRIC_IMPORT_MEMDESC_DATA));
    if (pMemdescData == NULL)
        goto freeMemDesc;

    portMemSet(pMemdescData, 0, sizeof(FABRIC_IMPORT_MEMDESC_DATA));

    pMemdescData->physAttrs = *pPhysAttrs;
    pMemdescData->memFlags = memFlags;
    pMemdescData->cliqueId = pAttrs->cliqueId;

    // Associate the memdesc data release callback function.
    memdescSetMemData(pMemDesc, (void *)pMemdescData, NULL);

    // Allocate memory for the memory descriptor destroy callback.
    pCallback = portMemAllocNonPaged(sizeof(MEM_DESC_DESTROY_CALLBACK));
    if (pCallback == NULL)
        goto freeMemdescMemData;

    portMemSet(pCallback, 0, sizeof(MEM_DESC_DESTROY_CALLBACK));

    // Associate the memdescDestroy callback function.
    pCallback->pObject         = (void *)pCallback;
    pCallback->destroyCallback =
        (MemDescDestroyCallBack*) &_memoryfabricImportMemDescDestroyCallback;

    memdescAddDestroyCallback(pMemDesc, pCallback);

    return pMemDesc;

freeMemdescMemData:
    portMemFree(pMemdescData);

freeMemDesc:
    memdescDestroy(pMemDesc);

    return NULL;
}

NV_STATUS
memoryfabricimportedrefCtrlValidate_IMPL
(
    MemoryFabricImportedRef     *pMemoryFabricImportedRef,
    NV00FB_CTRL_VALIDATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    NvU64 i;
    NvU64 *pPfnArray = NULL;
    MEMORY_DESCRIPTOR *pTempMemDesc = pMemoryFabricImportedRef->pTempMemDesc;
    NvU64 result;

    pParams->bDone = NV_FALSE;

    if (pParams->flags & NV00FB_CTRL_FLAGS_IMPORT_FAILED)
    {
        pTempMemDesc = NULL;
        goto install;
    }

    if ((pParams->totalPfns == 0) ||
        (pMemoryFabricImportedRef->numUpdatedPfns >= pParams->totalPfns))
        return NV_ERR_INVALID_ARGUMENT;

    if (!portSafeAddU64(pParams->offset, pParams->numPfns, &result) ||
        (result > pParams->totalPfns))
        return NV_ERR_OUT_OF_RANGE;

    if ((pParams->attrs.pageSize != RM_PAGE_SIZE_HUGE) &&
        (pParams->attrs.pageSize != RM_PAGE_SIZE_512M))
    {
        if (pParams->attrs.pageSize != RM_PAGE_SIZE_256G)
            return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pParams->attrs.size == 0) ||
        !NV_IS_ALIGNED64(pParams->attrs.size, pParams->attrs.pageSize))
        return NV_ERR_INVALID_ARGUMENT;

    pPfnArray = portMemAllocNonPaged(sizeof(NvU64) * pParams->numPfns);
    if (pPfnArray == NULL)
        return NV_ERR_NO_MEMORY;

    for (i = 0; i < pParams->numPfns; i++)
        pPfnArray[i] = ((NvU64)pParams->pfnArray[i]) << RM_PAGE_SHIFT_HUGE;

    if (pTempMemDesc == NULL)
    {
        pTempMemDesc = _createTempMemDesc(&pParams->attrs,
                                          &pParams->physAttrs,
                                          pParams->memFlags,
                                          (pParams->totalPfns == 1));
        if (pTempMemDesc == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto out;
        }

        pMemoryFabricImportedRef->pTempMemDesc = pTempMemDesc;
    }

    if (pParams->totalPfns == 1)
    {
        memdescSetPte(pTempMemDesc, AT_GPU, 0, pPfnArray[0]);
    }
    else
    {
        memdescFillPages(pTempMemDesc, pParams->offset, pPfnArray,
                         pParams->numPfns, pParams->attrs.pageSize);
    }

    pMemoryFabricImportedRef->numUpdatedPfns += pParams->numPfns;

    pParams->bDone = (pMemoryFabricImportedRef->numUpdatedPfns == \
                      pParams->totalPfns);

    if (!pParams->bDone)
        goto out;

install:
    status = importDescriptorInstallMemDesc(
                   pMemoryFabricImportedRef->pFabricImportDesc, pTempMemDesc);

    //
    // Reset pTempMemDesc so that memoryfabricimportedrefDestruct_IMPL won't
    // destroy it. The ref-counting would be properly handled when
    // importDescriptorPutNonBlocking is called.
    //
    if ((status == NV_OK) && (pTempMemDesc != NULL))
        pMemoryFabricImportedRef->pTempMemDesc = NULL;

out:
    portMemFree(pPfnArray);

    return status;
}
