/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "kernel/gpu/fifo/kernel_ctxshare.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "vgpu/rpc.h"
#include "gpu/device/device.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "core/locks.h"
#include "gpu/mem_mgr/vaspace_api.h"
#include "rmapi/rs_utils.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

#define SUBCTXID_EHEAP_OWNER NvU32_BUILD('n','v','r','m')
#define VASPACEID_EHEAP_OWNER NvU32_BUILD('n','v','r','m')

NV_STATUS
kctxshareapiConstruct_IMPL
(
    KernelCtxShareApi *pKernelCtxShareApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                           rmStatus     = NV_OK;
    OBJVASPACE                         *pVAS;
    OBJGPU                             *pGpu         = GPU_RES_GET_GPU(pKernelCtxShareApi);
    KernelChannelGroupApi              *pKernelChannelGroupApi;
    KernelChannelGroup                 *pKernelChannelGroup;
    Device                             *pDevice      = NULL;
    RsResourceRef                      *pChanGrpRef;
    RsClient                           *pClient;
    NvHandle                            hDevice;
    NvHandle                            hClient      = pParams->hClient;
    NvHandle                            hVASpace     = 0;
    NV_CTXSHARE_ALLOCATION_PARAMETERS  *pUserParams  = pParams->pAllocParams;
    RsShared                           *pShared      = NULL;

    // To make context share a child of a TSG, a TSG must exist.
    if (CliGetChannelGroup(pParams->hClient, pParams->hParent,
            &pChanGrpRef, &hDevice) == NV_OK)
    {
        pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                             KernelChannelGroupApi);
        NV_ASSERT_OR_RETURN(pKernelChannelGroupApi != NULL,
                            NV_ERR_INVALID_STATE);
        pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
    }
    else
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    // Copy Constructor path
    if (RS_IS_COPY_CTOR(pParams))
    {
        rmStatus = kctxshareapiCopyConstruct_IMPL(pKernelCtxShareApi, pCallContext, pParams);
        return rmStatus;
    }

    rmStatus = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid client handle!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Depending on the va mode, allocating a context share might require allocation
    // parameters that has a va space handle. If multiple vaspace mode is
    // enabled (no va space under a device), a va handle is required!
    //
    // OPTIONAL_MULTIVA or SINGLE_VA MODES: Use the device va space.
    //

    rmStatus = deviceGetByHandle(pClient, hDevice, &pDevice);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid parent/device handle!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    hVASpace = pUserParams->hVASpace;
    NV_ASSERT((hVASpace == NV01_NULL_OBJECT) || (pDevice->vaMode != NV_DEVICE_ALLOCATION_VAMODE_SINGLE_VASPACE));

    if (pKernelChannelGroup->bLegacyMode)
    {
        //
        // RM is trying to pre-allocate the kctxshares to for legacy mode
        // In this case, we use the the parent TSG's pVAS rather than
        // the hVASpace param
        //
        NV_PRINTF(LEVEL_INFO, "Constructing Legacy Context Share\n");
        NV_ASSERT(hVASpace == NV01_NULL_OBJECT);
        pVAS = pKernelChannelGroup->pVAS;

        pKernelCtxShareApi->hVASpace = pKernelChannelGroupApi->hVASpace;
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Constructing Client Allocated Context Share\n");
        rmStatus = vaspaceGetByHandleOrDeviceDefault(pClient, hDevice, hVASpace, &pVAS);

        pKernelCtxShareApi->hVASpace = hVASpace;
    }

    NV_ASSERT_OR_RETURN((rmStatus == NV_OK), rmStatus);
    NV_ASSERT_OR_RETURN((pVAS != NULL), NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         serverAllocShareWithHalspecParent(&g_resServ, classInfo(KernelCtxShare), &pShared, staticCast(pGpu, Object)),
                         failed);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         kctxshareInitCommon(dynamicCast(pShared, KernelCtxShare),
                                             pKernelCtxShareApi,
                                             pGpu,
                                             pVAS,
                                             pUserParams->flags,
                                             &pUserParams->subctxId,
                                             pKernelChannelGroupApi),
                        failed);

    pKernelCtxShareApi->pShareData = dynamicCast(pShared, KernelCtxShare);

    if (hVASpace != NV01_NULL_OBJECT)
    {
        RsResourceRef *pVASpaceRef;
        rmStatus = clientGetResourceRef(pCallContext->pClient, hVASpace, &pVASpaceRef);
        if (rmStatus != NV_OK)
            goto failed;

        refAddDependant(pVASpaceRef, pCallContext->pResourceRef);
    }

    if (pKernelChannelGroupApi->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        RsResourceRef *pKernelGraphicsContextRef;
        rmStatus = clientGetResourceRef(pCallContext->pClient, pKernelChannelGroupApi->hKernelGraphicsContext, &pKernelGraphicsContextRef);
        if (rmStatus != NV_OK)
            goto failed;

        refAddDependant(pKernelGraphicsContextRef, pCallContext->pResourceRef);
    }

failed:
    if (rmStatus != NV_OK)
    {
        if (pShared)
        {
            serverFreeShare(&g_resServ, pShared);
        }
    }

    return rmStatus;
}

void
kctxshareapiDestruct_IMPL
(
    KernelCtxShareApi *pKernelCtxShareApi
)
{
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    OBJGPU                      *pGpu = GPU_RES_GET_GPU(pKernelCtxShareApi);
    KernelChannelGroupApi       *pKernelChannelGroupApi = NULL;
    KernelChannelGroup          *pKernelChannelGroup    = NULL;
    RsResourceRef               *pChanGrpRef;
    RsShared                    *pShared    = NULL;
    NvS32                        refcnt     = 0;

    resGetFreeParams(staticCast(pKernelCtxShareApi, RsResource), &pCallContext, &pParams);
    pChanGrpRef = pCallContext->pResourceRef->pParentRef;
    if (pChanGrpRef != NULL)
    {
        pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                             KernelChannelGroupApi);
        pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
    }

    NV_ASSERT(pKernelChannelGroup);

    if (pKernelCtxShareApi->pShareData != NULL)
    {
        NV_ASSERT(pKernelCtxShareApi->pShareData->pKernelChannelGroup ==
                  pKernelChannelGroup);


        NV_PRINTF(LEVEL_INFO, "KernelCtxShareApi Ptr: %p   ChanGrp: %p  !\n",
                  pKernelCtxShareApi, pKernelCtxShareApi->pShareData->pKernelChannelGroup);

        pShared = staticCast(pKernelCtxShareApi->pShareData, RsShared);
        refcnt  = serverGetShareRefCount(&g_resServ, pShared);

        NV_PRINTF(LEVEL_INFO, "kctxshareapiDestruct_IMPL called on KernelCtxShare %p with refcnt %d\n",
          pShared, refcnt);

        NV_ASSERT_OR_RETURN_VOID(refcnt >= 1);

        if (refcnt > 1)
        {
            //
            // serverFreeShare will delete the object automatically if the count hits 0;
            // we'd still need it to free all underlying resourcees, however.
            // For this reason we only decrement here if no free is needed
            //
            serverFreeShare(&g_resServ, pShared);

            NV_PRINTF(LEVEL_INFO, "kctxshareapiDestruct_IMPL: KernelCtxShare %p has %d references left\n",
                      pShared, refcnt-1);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "kctxshareapiDestruct_IMPL: KernelCtxShare %p has no more references, destroying...\n",
                      pShared);

            pParams->status = kctxshareDestroyCommon(pKernelCtxShareApi->pShareData, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi);
            NV_ASSERT(pParams->status == NV_OK);

            serverFreeShare(&g_resServ, pShared);
        }
    }
}

NV_STATUS
kctxshareapiCopyConstruct_IMPL
(
    KernelCtxShareApi *pKernelCtxShareApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS      rmStatus     = NV_OK;
    OBJGPU        *pGpu         = GPU_RES_GET_GPU(pKernelCtxShareApi);
    RsClient      *pDstClient   = pCallContext->pClient;
    RsResourceRef *pDstRef      = pCallContext->pResourceRef;
    RsResourceRef *pSrcRef      = pParams->pSrcRef;
    KernelCtxShareApi *pKernelCtxShareSrc = dynamicCast(pSrcRef->pResource, KernelCtxShareApi);
    KernelChannelGroupApi *pKernelChannelGroupApi;
    RS_ITERATOR    iter;
    RsResourceRef *pVaspaceRef  = NULL;
    VaSpaceApi    *pVaspaceApi  = NULL;
    RsResourceRef *pChanGrpRef  = pDstRef->pParentRef;

    pKernelCtxShareApi->pShareData = pKernelCtxShareSrc->pShareData;

    RsShared *pShared = staticCast(pKernelCtxShareApi->pShareData, RsShared);
    serverRefShare(&g_resServ, pShared);

    iter =  serverutilRefIter(pDstClient->hClient, pDstRef->pParentRef->pParentRef->hResource, classId(VaSpaceApi), RS_ITERATE_DESCENDANTS, NV_TRUE);
    while (clientRefIterNext(iter.pClient, &iter))
    {
        pVaspaceRef = iter.pResourceRef;
        pVaspaceApi = dynamicCast(pVaspaceRef->pResource, VaSpaceApi);
        NV_ASSERT_OR_ELSE(pVaspaceApi != NULL, rmStatus = NV_ERR_INVALID_STATE; goto done);

        if (pVaspaceApi->pVASpace == pKernelCtxShareApi->pShareData->pVAS)
        {
            refAddDependant(pVaspaceRef, pDstRef);
            break;
        }
    }

    pKernelChannelGroupApi = dynamicCast(pChanGrpRef->pResource,
                                         KernelChannelGroupApi);
    NV_ASSERT_OR_ELSE(pKernelChannelGroupApi != NULL,
                      rmStatus = NV_ERR_INVALID_STATE; goto done);

    if (pKernelChannelGroupApi->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        RsResourceRef *pKernelGraphicsContextRef;
        NV_ASSERT_OK_OR_ELSE(rmStatus,
                             clientGetResourceRef(pCallContext->pClient, pKernelChannelGroupApi->hKernelGraphicsContext, &pKernelGraphicsContextRef),
                             goto done);

        refAddDependant(pKernelGraphicsContextRef, pDstRef);
    }

    //
    // For legacy internal kctxshares, RPC is handled by the channelgroup object's copy ctor,
    // so we skip the automatic RPC here
    //
    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) && !pKernelCtxShareApi->pShareData->pKernelChannelGroup->bLegacyMode)
    {
        NV_RM_RPC_DUP_OBJECT(pGpu, pDstClient->hClient, pDstRef->pParentRef->hResource, pDstRef->hResource,
                             pParams->pSrcClient->hClient, pSrcRef->hResource, 0,
                             NV_TRUE, // automatically issue RPC_FREE on object free
                             pDstRef, rmStatus);
    }

done:
    if (rmStatus != NV_OK)
    {
        serverFreeShare(&g_resServ, pShared);
    }

    return rmStatus;
}

NvBool
kctxshareapiCanCopy_IMPL
(
    KernelCtxShareApi *pKernelCtxShareApi
)
{
    return NV_TRUE;
}

NV_STATUS
kctxshareConstruct_IMPL
(
    KernelCtxShare *pKernelCtxShare
)
{
    return NV_OK;
}

/**
 * @brief Initializes a new context share tracking structure.
 *
 * To be called immediately after allocation, initializes a broadcast context share
 * object to what the client specified. Afterwards, include the context share object
 * inside of a ChannelGroup's heap object based on the flag provided.
 *
 * @param pKernelCtxShare
 * @param pKernelCtxShareApi
 * @param pGpu
 * @param[in] pVAS
 * @param[in] Flags
 * @param[in,out] subctxId
 * @param[in] pKernelChannelGroupApi
 */
NV_STATUS
kctxshareInitCommon_IMPL
(
    KernelCtxShare        *pKernelCtxShare,
    KernelCtxShareApi     *pKernelCtxShareApi,
    OBJGPU                *pGpu,
    OBJVASPACE            *pVAS,
    NvU32                  Flags,
    NvU32                 *pSubctxId,
    KernelChannelGroupApi *pKernelChannelGroupApi
)
{
    KernelFifo         *pKernelFifo           = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS           status                = NV_OK;
    NvU32               sbctxHeapFlag         = 0;
    NvU32               vaSpaceHeapFlag       = 0;
    NvU64               subctxOffset          = 0;
    NvU64               vaSpaceOffset         = 0;
    NvU64               size                  = 1;
    NvU64               origSbctxRangeLo      = 0;
    NvU64               origSbctxRangeHi      = 0;
    EMEMBLOCK          *pSbctxBlock;
    EMEMBLOCK          *pVaSpaceBlock         = NULL;
    KernelChannelGroup *pKernelChannelGroup;
    VaSpaceMapEntry    *pVaSpaceEntry;

    NV_ASSERT_OR_RETURN(pKernelChannelGroupApi != NULL, NV_ERR_INVALID_STATE);
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
    NV_ASSERT(pKernelChannelGroup != NULL);
    NV_ASSERT(pVAS != NULL);

    // GPU lock must be held before calling this function
    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), NV_ERR_INVALID_LOCK_STATE);

    //
    // For external VAS, create subcontext only after SetPageDirectory() call is made.
    // This will ensure that new PDB will be updated in all channels subcontext array.
    // See Bug 1805222 comment #11 for more details
    //
    if (!IsAMODEL(pGpu) && vaspaceIsExternallyOwned(pVAS))
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        if (vaspaceGetPageDirBase(pVAS, pGpu) == NULL)
        {
            NV_ASSERT(0);
            SLI_LOOP_RETURN(NV_ERR_INVALID_STATE);
        }
        SLI_LOOP_END
    }

   // If flag is equal to SYNC, allocate context share from veId 0.
    if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC)
    {
        sbctxHeapFlag = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        subctxOffset   = 0;
    }
    //
    // If the flag is Async, we want to allocate a free block in reverse order.
    // This allocates a block between veId 1 and veId 63.
    // If no blocks are available between veId 1 and veId 63, use veId 0.
    //
    else if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC)
    {
        sbctxHeapFlag = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
    }
    // ASYNC_PREFER_LOWER is similar to above, but lower veIds are preferred and allocated in order.
    else if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC_PREFER_LOWER)
    {
        sbctxHeapFlag = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP;

        origSbctxRangeLo = pKernelChannelGroup->pSubctxIdHeap->rangeLo;
        origSbctxRangeHi = pKernelChannelGroup->pSubctxIdHeap->rangeHi;
        NV_ASSERT_OR_RETURN(origSbctxRangeLo == 0, NV_ERR_INVALID_STATE);

        // Avoid allocating veId 0 unless requested.
        status = pKernelChannelGroup->pSubctxIdHeap->eheapSetAllocRange(pKernelChannelGroup->pSubctxIdHeap, 1, origSbctxRangeHi);
        NV_ASSERT_OK_OR_RETURN(status);
    }
    else if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SPECIFIED)
    {
        sbctxHeapFlag = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        subctxOffset   = *pSubctxId;
    }
    else
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = pKernelChannelGroup->pSubctxIdHeap->eheapAlloc(
        pKernelChannelGroup->pSubctxIdHeap,
        SUBCTXID_EHEAP_OWNER,
        &sbctxHeapFlag,
        &subctxOffset,
        &size,
        1,
        1,
        &pSbctxBlock,
        NULL,
        NULL);

    if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC_PREFER_LOWER)
    {
        // Restore original pSubctxIdHeap ranges.
        NV_ASSERT_OK_OR_RETURN(pKernelChannelGroup->pSubctxIdHeap->eheapSetAllocRange(pKernelChannelGroup->pSubctxIdHeap, origSbctxRangeLo, origSbctxRangeHi));

        // Retry with full range. (Alloc veId 0 if no others are available.)
        if (status != NV_OK)
        {
            status = pKernelChannelGroup->pSubctxIdHeap->eheapAlloc(
                pKernelChannelGroup->pSubctxIdHeap,
                SUBCTXID_EHEAP_OWNER,
                &sbctxHeapFlag,
                &subctxOffset,
                &size,
                1,
                1,
                &pSbctxBlock,
                NULL,
                NULL);
        }
    }

    if (status != NV_OK)
    {
        return status;
    }

    pVaSpaceEntry = mapFind(&pKernelChannelGroup->vaSpaceMap, (NvU64)pVAS);

    if (pVaSpaceEntry == NULL)
    {
        /*
         * Do not follow sbctxHeapFlag, since we do not know that a subcontext using FIXED_ADDRESS_ALLOCATE
         * will not be deallocated and reallocated with a different VASpace while another subcontext using the
         * original VASpace remains allocated, preventing that offset in pVaSpaceIdHeap from being freed.
         */
        vaSpaceHeapFlag = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_UP;
        status = pKernelChannelGroup->pVaSpaceIdHeap->eheapAlloc(
            pKernelChannelGroup->pVaSpaceIdHeap,
            VASPACEID_EHEAP_OWNER,
            &vaSpaceHeapFlag,
            &vaSpaceOffset,
            &size,
            1,
            1,
            &pVaSpaceBlock,
            NULL,
            NULL);
        if (status != NV_OK)
        {
            goto done;
        }

        pVaSpaceBlock->pData = (void *)pVAS;

        pVaSpaceEntry = mapInsertNew(&pKernelChannelGroup->vaSpaceMap, (NvU64)pVAS);
        if (pVaSpaceEntry == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        pVaSpaceEntry->heapOffset = vaSpaceOffset;
        pVaSpaceEntry->refCount = 1;
    }
    else
    {
        vaSpaceOffset = pVaSpaceEntry->heapOffset;
        pVaSpaceEntry->refCount += 1;
    }

    pKernelCtxShare->pVAS                = pVAS;
    pKernelCtxShare->subctxId            = NvU64_LO32(subctxOffset);
    pKernelCtxShare->vaSpaceId           = NvU64_LO32(vaSpaceOffset);
    pKernelCtxShare->pKernelChannelGroup = pKernelChannelGroup;
    pKernelCtxShare->flags               = Flags;

    NV_PRINTF(LEVEL_INFO, "Allocated subctxId: 0x%02llx, vaSpaceId: 0x%02llx\n", subctxOffset, vaSpaceOffset);

    pSbctxBlock->pData = (void *)pKernelCtxShare;

    status = kctxshareInit_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS,
                               pKernelChannelGroupApi, subctxOffset, pSbctxBlock);

    if(status != NV_OK)
    {
        goto done;
    }

// @todo Code under label "fail" should handle failure case
done:
    if (status == NV_OK)
    {
       *pSubctxId   = NvU64_LO32(subctxOffset);

        if (Flags == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC_PREFER_LOWER)
        {
            *pSubctxId = ( subctxOffset < kfifoGetMaxLowerSubcontext(pGpu, pKernelFifo) ?
                FLD_SET_DRF(_CTXSHARE_ALLOCATION, _SUBCTXID, _ASYNC_PREFER_LOWER_ALLOCATION, _SUCCESS, *pSubctxId) :
                FLD_SET_DRF(_CTXSHARE_ALLOCATION, _SUBCTXID, _ASYNC_PREFER_LOWER_ALLOCATION, _FAIL,    *pSubctxId) );
        }

        NV_PRINTF(LEVEL_INFO,
                  "New Context Share 0x%p allocated with id 0x%x\n",
                  pKernelCtxShare, NvU64_LO32(subctxOffset));
    }
    else
    {
        NV_STATUS tmpStatus;

        tmpStatus = pKernelChannelGroup->pSubctxIdHeap->eheapFree(
            pKernelChannelGroup->pSubctxIdHeap,
            subctxOffset);
        NV_ASSERT(tmpStatus == NV_OK);

        if (pVaSpaceEntry != NULL)
        {
            pVaSpaceEntry->refCount -= 1;
            if (pVaSpaceEntry->refCount == 0)
            {
                tmpStatus = pKernelChannelGroup->pVaSpaceIdHeap->eheapFree(
                    pKernelChannelGroup->pVaSpaceIdHeap,
                    vaSpaceOffset);
                NV_ASSERT(tmpStatus == NV_OK);

                mapRemove(&pKernelChannelGroup->vaSpaceMap, pVaSpaceEntry);
            }
        }

        NV_PRINTF(LEVEL_INFO,
                  "Context Share 0x%p allocation with id 0x%x failed, status is %x\n",
                  pKernelCtxShare, NvU64_LO32(subctxOffset), status);
    }

    return status;
}

/**
 * @brief Frees a context share tracking structure if no references remain.
 *
 * This funtion should be used to free kctxshare rather than freeing object
 * directly using serverFreeShare or objDelete.
 * Frees child ENGINE_CTX_DESCRIPTORs but does not free any memory pointed at
 * by pPrivCtxData.  It is the responsiblity of the user of that memory to
 * ensure it is freed before this function is called (or that another pointer
 * exists).
 *
 * @param[in] pKernelCtxShare
 * @param[in] pKernelCtxShareApi
 * @param pGpu
 * @param pKernelChannelGroupApi
 */
NV_STATUS
kctxshareDestroyCommon_IMPL
(
    KernelCtxShare *pKernelCtxShare,
    KernelCtxShareApi *pKernelCtxShareApi,
    OBJGPU *pGpu,
    KernelChannelGroupApi *pKernelChannelGroupApi
)
{
    NV_STATUS               status = NV_OK;
    NvU32                   subctxId;
    KernelChannelGroup     *pKernelChannelGroup;
    NvBool                  bRelease;
    RsShared               *pShared = NULL;
    NvS32                   refcnt = 0;
    VaSpaceMapEntry        *pVaSpaceEntry;

    NV_ASSERT_OR_RETURN(pKernelCtxShare != NULL, NV_ERR_INVALID_STATE);

    // This function should only be called on the last free of the object
    pShared = staticCast(pKernelCtxShare, RsShared);
    refcnt  = serverGetShareRefCount(&g_resServ, pShared);
    NV_ASSERT_OR_RETURN(refcnt == 1, NV_ERR_INVALID_STATE);

    // GPU lock must be held before calling this function
    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), NV_ERR_INVALID_LOCK_STATE);

    pKernelChannelGroup = pKernelCtxShare->pKernelChannelGroup;
    NV_ASSERT(pKernelChannelGroup == pKernelChannelGroupApi->pKernelChannelGroup);
    subctxId = pKernelCtxShare->subctxId;

    pVaSpaceEntry = mapFind(&pKernelChannelGroup->vaSpaceMap, (NvU64)(pKernelCtxShare->pVAS));
    NV_ASSERT_OR_ELSE(pVaSpaceEntry != NULL && pVaSpaceEntry->refCount != 0,
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "VASpace map entry not found.\n");
        goto fail);

    //
    // Handle the case when VAS is shared by subcontexts.
    // Release the shared resources only when the last subcontext using this VAS is freed.
    //
    bRelease = (pVaSpaceEntry->refCount == 1);

    status = kctxshareDestroy_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease);
    if (status != NV_OK)
    {
        goto fail;
    }

    status = pKernelChannelGroup->pSubctxIdHeap->eheapFree(
        pKernelChannelGroup->pSubctxIdHeap,
        subctxId);
    NV_ASSERT_OR_ELSE(status == NV_OK,
        NV_PRINTF(LEVEL_ERROR,
                  "Subcontext ID heap free failed with status = %s (0x%x)\n",
                  nvstatusToString(status), status);
        goto fail);

    pVaSpaceEntry->refCount -= 1;
    if (pVaSpaceEntry->refCount == 0)
    {
        status = pKernelChannelGroup->pVaSpaceIdHeap->eheapFree(
            pKernelChannelGroup->pVaSpaceIdHeap,
            pVaSpaceEntry->heapOffset);
    NV_ASSERT_OR_ELSE(status == NV_OK,
        NV_PRINTF(LEVEL_ERROR,
                  "VASpace ID heap free failed with status = %s (0x%x)\n",
                  nvstatusToString(status), status);
        goto fail);
        
        mapRemove(&pKernelChannelGroup->vaSpaceMap, pVaSpaceEntry);
    }

fail:
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Freed Context Share 0x%p with id 0x%x\n",
                  pKernelCtxShare, subctxId);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Failed to free Context Share 0x%p with id 0x%x\n",
                  pKernelCtxShare, subctxId);
    }

    return status;
}

void
kctxshareDestruct_IMPL
(
    KernelCtxShare *pKernelCtxShare
)
{
    //
    // Assert that kctxshareDestroyCommon was called to free kctxshare resources before
    // getting here by checking if subctxId has been freed from heap.
    // pKernelChannelGroup may not be set if kctxshare failed initialization.
    //
    if(pKernelCtxShare->pKernelChannelGroup != NULL)
    {
        EMEMBLOCK *pBlock =
            pKernelCtxShare->pKernelChannelGroup->pSubctxIdHeap->eheapGetBlock(
                pKernelCtxShare->pKernelChannelGroup->pSubctxIdHeap,
                pKernelCtxShare->subctxId,
                NV_FALSE);

        NV_ASSERT(pBlock == NULL);
    }
}
