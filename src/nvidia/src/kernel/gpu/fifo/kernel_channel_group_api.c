/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel_group_api.h"

#include "kernel/core/locks.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/falcon/kernel_falcon.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "platform/sli/sli.h"

#include "kernel/gpu/conf_compute/conf_compute.h"

#include "class/cl0090.h" // KERNEL_GRAPHICS_CONTEXT
#include "class/cl9067.h" // FERMI_CONTEXT_SHARE_A

#include "libraries/utils/nvprintf.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/mem_mgr/vaspace_api.h"
#include "vgpu/rpc.h"
#include "rmapi/rs_utils.h"
#include "containers/eheap_old.h"

NV_STATUS
kchangrpapiConstruct_IMPL
(
    KernelChannelGroupApi        *pKernelChannelGroupApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NvBool                                  bTsgAllocated       = NV_FALSE;
    RsResourceRef                          *pResourceRef        = pCallContext->pResourceRef;
    NV_STATUS                               rmStatus;
    OBJVASPACE                             *pVAS                = NULL;
    OBJGPU                                 *pGpu                = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    KernelMIGManager                       *pKernelMIGManager   = NULL;
    KernelFifo                             *pKernelFifo         = GPU_GET_KERNEL_FIFO(pGpu);
    NvHandle                                hVASpace            = NV01_NULL_OBJECT;
    Device                                 *pDevice             = NULL;
    NvU32                                   gfid                = GPU_GFID_PF;
    RsShared                               *pShared             = NULL;
    RsClient                               *pClient;
    NvBool                                  bLockAcquired       = NV_FALSE;
    Heap                                   *pHeap               = GPU_GET_HEAP(pGpu);
    NvBool                                  bMIGInUse           = NV_FALSE;
    CTX_BUF_INFO                           *bufInfoList         = NULL;
    NvU32                                   bufCount            = 0;
    NvBool                                  bReserveMem         = NV_FALSE;
    MIG_INSTANCE_REF                        ref;
    RM_API                                 *pRmApi              = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelChannelGroup                     *pKernelChannelGroup = NULL;
    NV_CHANNEL_GROUP_ALLOCATION_PARAMETERS *pAllocParams        = NULL;
    RM_ENGINE_TYPE                          rmEngineType;


    NV_PRINTF(LEVEL_INFO,
              "hClient: 0x%x, hParent: 0x%x, hObject:0x%x, hClass: 0x%x\n",
              pParams->hClient, pParams->hParent, pParams->hResource,
              pParams->externalClassId);

    if (RS_IS_COPY_CTOR(pParams))
    {
        NV_ASSERT_OK_OR_GOTO(rmStatus,
                             rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FIFO),
                             done);
        bLockAcquired = NV_TRUE;
        rmStatus = kchangrpapiCopyConstruct_IMPL(pKernelChannelGroupApi,
                                                 pCallContext, pParams);
        goto done;
    }

    //
    // Make sure this GPU is not already locked by this thread
    // Ideally this thread shouldn't have locked any GPU in the system but
    // checking this is sufficient as memory allocation from PMA requires
    // current GPU's lock not to be held
    //
    if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_ERROR, "TSG alloc should be called without acquiring GPU lock\n");
        NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_LOCK_STATE);
    }

    bufInfoList = portMemAllocNonPaged(NV_ENUM_SIZE(GR_CTX_BUFFER) * sizeof(*bufInfoList));
    if (bufInfoList == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    // Acquire the lock *only after* PMA is done allocating.
    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FIFO),
                         done);
    bLockAcquired = NV_TRUE;

    pAllocParams = pParams->pAllocParams;
    hVASpace     = pAllocParams->hVASpace;

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        serverAllocShareWithHalspecParent(&g_resServ, classInfo(KernelChannelGroup),
                                          &pShared, staticCast(pGpu, Object)),
        failed);

    pKernelChannelGroup = dynamicCast(pShared, KernelChannelGroup);
    pKernelChannelGroupApi->pKernelChannelGroup = pKernelChannelGroup;

    if (!gpuIsClassSupported(pGpu, pResourceRef->externalClassId))
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pResourceRef->externalClassId);
        rmStatus = NV_ERR_NOT_SUPPORTED;
        goto failed;
    }

    pKernelChannelGroupApi->hVASpace = hVASpace;

    rmStatus = serverGetClientUnderLock(&g_resServ, pParams->hClient, &pClient);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid client handle!\n");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    rmStatus = deviceGetByHandle(pClient, pParams->hParent, &pDevice);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid parent/device handle!\n");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    bMIGInUse = IS_MIG_IN_USE(pGpu);

    rmEngineType = gpuGetRmEngineType(pAllocParams->engineType);

    if (kfifoIsPerRunlistChramSupportedInHw(pKernelFifo))
    {
        if (!RM_ENGINE_TYPE_IS_VALID(rmEngineType))
        {
            NV_PRINTF(LEVEL_NOTICE, "Valid engine Id must be specified while allocating TSGs or bare channels!\n");
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto failed;
        }

        //
        // If we have a separate channel RAM for each runlist then we need
        // to determine runlistId from engineId passed by client. This
        // runlistId is used to associate all future channels in this TSG to
        // that runlist. Setting the engineType will cause the runlist
        // corresponding to that engine to be chosen.
        //
        pKernelChannelGroup->engineType = rmEngineType;
    }

    //
    // If MIG is enabled, client passes a logical engineId w.r.t its own GPU instance
    // we need to convert this logical Id to a physical engine Id as we use it
    // to set runlistId
    //
    if (bMIGInUse)
    {
        // Engine type must be valid for MIG
        NV_CHECK_OR_ELSE(LEVEL_NOTICE, RM_ENGINE_TYPE_IS_VALID(pKernelChannelGroup->engineType),
                         rmStatus = NV_ERR_INVALID_STATE; goto failed);

        NV_CHECK_OK_OR_GOTO(
            rmStatus,
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                            pDevice, &ref),
            failed);

        NV_CHECK_OK_OR_GOTO(
            rmStatus,
            LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &rmEngineType),
            failed);

        // Rewrite the engineType with the global engine type
        pKernelChannelGroup->engineType = rmEngineType;
        pHeap = ref.pKernelMIGGpuInstance->pMemoryPartitionHeap;
    }
    else
    {
        // Only GR0 is allowed without MIG
        if ((RM_ENGINE_TYPE_IS_GR(rmEngineType)) && (rmEngineType != RM_ENGINE_TYPE_GR0))
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto failed;
        }
    }

    if((pDevice->vaMode != NV_DEVICE_ALLOCATION_VAMODE_MULTIPLE_VASPACES) || (hVASpace != 0))
    {
        NV_ASSERT_OK_OR_GOTO(rmStatus,
            vaspaceGetByHandleOrDeviceDefault(pClient, pParams->hParent, hVASpace, &pVAS),
            failed);

        if (pVAS == NULL)
        {
            rmStatus = NV_ERR_INVALID_STATE;
            goto failed;
        }
    }


    // vGpu plugin context flag should only be set on host if context is plugin
    if (gpuIsSriovEnabled(pGpu))
    {
        pKernelChannelGroup->bIsCallingContextVgpuPlugin = pAllocParams->bIsCallingContextVgpuPlugin;
    }

    if (pKernelChannelGroup->bIsCallingContextVgpuPlugin)
        gfid = GPU_GFID_PF;
    else
    {
        NV_ASSERT_OK_OR_GOTO(rmStatus, vgpuGetCallingContextGfid(pGpu, &gfid), failed);
    }

    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        RmClient *pRmClient = dynamicCast(pClient, RmClient);
        //
        // WAR for 4217716 - Force allocations made on behalf of internal clients to
        // RM reserved heap. This avoids a constant memory allocation from appearing
        // due to the ctxBufPool reservation out of PMA.
        //
        if ((pRmClient == NULL) || !(pRmClient->Flags & RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT))
        {
            NV_ASSERT_OK_OR_GOTO(rmStatus,
                ctxBufPoolInit(pGpu, pHeap, &pKernelChannelGroup->pCtxBufPool),
                failed);

            NV_ASSERT_OK_OR_GOTO(rmStatus,
                ctxBufPoolInit(pGpu, pHeap, &pKernelChannelGroup->pChannelBufPool),
                failed);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "Skipping ctxBufPoolInit for RC watchdog\n");
        }
    }

    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         kchangrpInit(pGpu, pKernelChannelGroup, pVAS, gfid),
                         failed);
    bTsgAllocated = NV_TRUE;

    pKernelChannelGroupApi->hLegacykCtxShareSync  = 0;
    pKernelChannelGroupApi->hLegacykCtxShareAsync = 0;

    if (hVASpace != 0)
    {
        RsResourceRef *pVASpaceRef;
        rmStatus = clientGetResourceRef(pCallContext->pClient, hVASpace, &pVASpaceRef);
        NV_ASSERT(rmStatus == NV_OK);
        if (rmStatus == NV_OK)
            refAddDependant(pVASpaceRef, pResourceRef);
    }

    pKernelChannelGroupApi->hErrorContext    = pAllocParams->hObjectError;
    pKernelChannelGroupApi->hEccErrorContext = pAllocParams->hObjectEccError;

    // Default interleave level
    NV_ASSERT_OK_OR_GOTO(
        rmStatus,
        kchangrpSetInterleaveLevel(pGpu, pKernelChannelGroup,
                                   NVA06C_CTRL_INTERLEAVE_LEVEL_MEDIUM),
        failed);

    ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    if ((pConfCompute != NULL) &&
        (pConfCompute->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED)))
    {
        // TODO: jira CONFCOMP-1621: replace this with actual flag for TSG alloc that skips scrub
        if ((pMemoryManager->bScrubChannelSetupInProgress) &&
            (pKernelChannelGroup->pChannelBufPool != NULL) &&
            (pKernelChannelGroup->pCtxBufPool != NULL))
        {
            if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_ERROR, "Only kernel priv clients can skip scrubber\n");
                goto failed;
            }
            ctxBufPoolSetScrubSkip(pKernelChannelGroup->pChannelBufPool, NV_TRUE);
            ctxBufPoolSetScrubSkip(pKernelChannelGroup->pCtxBufPool, NV_TRUE);
            NV_PRINTF(LEVEL_INFO, "Skipping scrubber for all allocations on this context\n");
        }
    }

    //
    // If ctx buf pools are enabled, filter out partitionable engines
    // that aren't part of our instance.
    //
    // Memory needs to be reserved in the pool only for buffers for
    // engines in instance.
    //

    //
    // Size of memory that will be calculated for ctxBufPool reservation if ctxBufPool is enabled and MIG is disabled
    // or current engine belongs to this MIG instance and MIG is enabled
    //
    if (pKernelChannelGroup->pCtxBufPool != NULL &&
        (!bMIGInUse || kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, pKernelChannelGroup->engineType, ref)))
    {
        // GR Buffers
        if (RM_ENGINE_TYPE_IS_GR(pKernelChannelGroup->engineType))
        {
            KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, RM_ENGINE_TYPE_GR_IDX(pKernelChannelGroup->engineType));
            NvU32 bufId = 0;
            portMemSet(&bufInfoList[0], 0, sizeof(CTX_BUF_INFO) * NV_ENUM_SIZE(GR_CTX_BUFFER));
            bufCount = 0;

            kgraphicsDiscoverMaxLocalCtxBufferSize(pGpu, pKernelGraphics);

            FOR_EACH_IN_ENUM(GR_CTX_BUFFER, bufId)
            {
                // TODO expose engine class capabilities to kernel RM
                if (kgrmgrIsCtxBufSupported(bufId, NV_FALSE))
                {
                    const CTX_BUF_INFO *pBufInfo = kgraphicsGetCtxBufferInfo(pGpu, pKernelGraphics, bufId);
                    bufInfoList[bufCount] = *pBufInfo;
                    NV_PRINTF(LEVEL_INFO, "Reserving 0x%llx bytes for GR ctx bufId = %d\n",
                                  bufInfoList[bufCount].size, bufId);
                    bufCount++;
                }
            }
            FOR_EACH_IN_ENUM_END;
            bReserveMem = NV_TRUE;
        }
        else
        {
            // Allocate falcon context buffers if engine has (Kernel) Falcon object
            NvU32 ctxBufferSize;
            if (IS_GSP_CLIENT(pGpu))
            {
                ENGDESCRIPTOR engDesc;
                KernelFalcon *pKernelFalcon = NULL;

                NV_ASSERT_OK_OR_GOTO(rmStatus,
                    gpuXlateClientEngineIdToEngDesc(pGpu,
                                                    pKernelChannelGroup->engineType,
                                                    &engDesc),
                    failed);

                pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, engDesc);
                if (pKernelFalcon != NULL)
                {
                    ctxBufferSize = pKernelFalcon->ctxBufferSize;
                    bReserveMem = NV_TRUE;
                }
            }

            if (bReserveMem)
            {
                bufInfoList[0].size  = ctxBufferSize;
                bufInfoList[0].align = RM_PAGE_SIZE;
                bufInfoList[0].attr  = RM_ATTR_PAGE_SIZE_4KB;
                bufInfoList[0].bContig = NV_TRUE;
                NV_PRINTF(LEVEL_INFO, "Reserving 0x%llx bytes for engineType %d (%d) flcn ctx buffer\n",
                              bufInfoList[0].size, gpuGetNv2080EngineType(pKernelChannelGroup->engineType),
                              pKernelChannelGroup->engineType);
                bufCount++;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "No buffer reserved for engineType %d (%d) in ctx_buf_pool\n",
                                  gpuGetNv2080EngineType(pKernelChannelGroup->engineType),
                                  pKernelChannelGroup->engineType);
            }
        }
    }

    if ((!bMIGInUse || RM_ENGINE_TYPE_IS_GR(pKernelChannelGroup->engineType))
        && !IsT234D(pGpu))
    {
        NV_ASSERT_OK_OR_GOTO(rmStatus,
            pRmApi->AllocWithSecInfo(pRmApi,
                pParams->hClient,
                RES_GET_HANDLE(pKernelChannelGroupApi),
                &pKernelChannelGroupApi->hKernelGraphicsContext,
                KERNEL_GRAPHICS_CONTEXT,
                NvP64_NULL,
                0,
                RMAPI_ALLOC_FLAGS_SKIP_RPC,
                NvP64_NULL,
                &pRmApi->defaultSecInfo),
            failed);
    }

    NV_PRINTF(LEVEL_INFO, "Adding group Id: %d hClient:0x%x\n",
              pKernelChannelGroup->grpID, pParams->hClient);

    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
        !(pParams->allocFlags & RMAPI_ALLOC_FLAGS_SKIP_RPC))
    {
        NV_RM_RPC_ALLOC_OBJECT(pGpu,
                               pParams->hClient,
                               pParams->hParent,
                               pParams->hResource,
                               pParams->externalClassId,
                               pAllocParams,
                               sizeof(*pAllocParams),
                               rmStatus);
        //
        // Make sure that corresponding RPC occurs when freeing
        // KernelChannelGroupApi. Resource server checks this variable during
        // free and ignores any RPC flags set in resource_list.h
        //
        staticCast(pKernelChannelGroupApi, RmResource)->bRpcFree = NV_TRUE;

        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "KernelChannelGroupApi alloc RPC to vGpu Host failed\n");
            goto failed;
        }

        if (IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) || IS_GSP_CLIENT(pGpu))
        {
            NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS params = {
                0};
            NvU32 runqueueIdx;
            NvU32 maxRunqueues = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);

            for (runqueueIdx = 0; runqueueIdx < maxRunqueues; ++runqueueIdx)
            {
                MEMORY_DESCRIPTOR          *pSrcMemDesc;
                HW_ENG_FAULT_METHOD_BUFFER *pMthdBuffer;
                pMthdBuffer = &pKernelChannelGroup->pMthdBuffers[runqueueIdx];
                pSrcMemDesc = pMthdBuffer->pMemDesc;

                params.methodBufferMemdesc[runqueueIdx].size = (
                    pSrcMemDesc->Size);
                params.methodBufferMemdesc[runqueueIdx].addressSpace = (
                    memdescGetAddressSpace(pSrcMemDesc));
                params.methodBufferMemdesc[runqueueIdx].cpuCacheAttrib = (
                    memdescGetCpuCacheAttrib(pSrcMemDesc));
                params.methodBufferMemdesc[runqueueIdx].alignment = 1;

                if (IS_VIRTUAL_WITH_FULL_SRIOV(pGpu))
                {
                    params.bar2Addr[runqueueIdx] = pMthdBuffer->bar2Addr;
                    params.methodBufferMemdesc[runqueueIdx].base = (
                        memdescGetPhysAddr(pSrcMemDesc, AT_CPU, 0));
                }
                else
                {
                    //
                    // The case of both vGpu full SRIOV + GSP_CLIENT host is not
                    // supported. This else branch considers the case of
                    // GSP_CLIENT only without vGpu.
                    //
                    params.methodBufferMemdesc[runqueueIdx].base = (
                        memdescGetPhysAddr(pSrcMemDesc, AT_GPU, 0));
                }
            }
            params.numValidEntries = runqueueIdx;

            rmStatus = pRmApi->Control(pRmApi,
                pParams->hClient,
                RES_GET_HANDLE(pKernelChannelGroupApi),
                NVA06C_CTRL_CMD_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS,
                &params,
                sizeof params);

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Control call to update method buffer memdesc failed\n");
                goto failed;
            }
        }
    }

    if (kfifoIsZombieSubctxWarEnabled(pKernelFifo))
    {
        kchangrpSetSubcontextZombieState_HAL(pGpu, pKernelChannelGroup, 0, NV_TRUE);
        kchangrpUpdateSubcontextMask_HAL(pGpu, pKernelChannelGroup, 0, NV_TRUE);
    }

    // initialize apiObjList with original client's KernelChannelGroupApi object
    listInit(&pKernelChannelGroup->apiObjList, portMemAllocatorGetGlobalNonPaged());

    if (listAppendValue(&pKernelChannelGroup->apiObjList, &pKernelChannelGroupApi) == NULL)
    {
        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        listClear(&pKernelChannelGroup->apiObjList);
        goto failed;
    }

failed:
    if (rmStatus != NV_OK)
    {
        if (pKernelChannelGroupApi->hKernelGraphicsContext != NV01_NULL_OBJECT)
        {
            pRmApi->Free(pRmApi, pParams->hClient,
                         pKernelChannelGroupApi->hKernelGraphicsContext);
        }

        if (pKernelChannelGroup != NULL)
        {
            if (bTsgAllocated)
                kchangrpDestroy(pGpu, pKernelChannelGroup);

            if (pKernelChannelGroup->pCtxBufPool != NULL)
                ctxBufPoolDestroy(&pKernelChannelGroup->pCtxBufPool);

            if (pKernelChannelGroup->pChannelBufPool != NULL)
                ctxBufPoolDestroy(&pKernelChannelGroup->pChannelBufPool);

        }

        if (pShared)
            serverFreeShare(&g_resServ, pShared);
    }

done:

    if (bLockAcquired)
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    if ((rmStatus == NV_OK) && bReserveMem)
    {
        // GPU lock should not be held when reserving memory for ctxBufPool
        NV_CHECK_OK(rmStatus, LEVEL_ERROR,
            ctxBufPoolReserve(pGpu, pKernelChannelGroup->pCtxBufPool, bufInfoList, bufCount));
        if (rmStatus != NV_OK)
        {
            // Acquire the lock again for the cleanup path
            NV_ASSERT_OK_OR_RETURN(rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_FIFO));
            bLockAcquired = NV_TRUE;
            goto failed;
        }
    }

    portMemFree(bufInfoList);

    return rmStatus;
}

NV_STATUS
kchangrpapiControl_IMPL
(
    KernelChannelGroupApi          *pKernelChannelGroupApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pResourceRef = RES_GET_REF(pKernelChannelGroupApi);

    (void)pResourceRef;
    NV_PRINTF(LEVEL_INFO, "grpID 0x%x handle 0x%x cmd 0x%x\n",
              pKernelChannelGroupApi->pKernelChannelGroup->grpID,
              pResourceRef->hResource, pParams->pLegacyParams->cmd);

    return gpuresControl_IMPL(staticCast(pKernelChannelGroupApi, GpuResource),
                              pCallContext, pParams);
}

void
kchangrpapiDestruct_IMPL
(
    KernelChannelGroupApi *pKernelChannelGroupApi
)
{
    CALL_CONTEXT           *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    RsResourceRef          *pResourceRef;
    RsClient               *pClient;
    KernelChannelGroup *pKernelChannelGroup =
        pKernelChannelGroupApi->pKernelChannelGroup;
    OBJGPU                 *pGpu = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    NV_STATUS               rmStatus = NV_OK;
    RS_ORDERED_ITERATOR     it;
    RsShared               *pShared = staticCast(pKernelChannelGroup, RsShared);
    RM_API                 *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    resGetFreeParams(staticCast(pKernelChannelGroupApi, RsResource),
                     &pCallContext, &pParams);
    pResourceRef = pCallContext->pResourceRef;
    pClient = pCallContext->pClient;

    NV_PRINTF(LEVEL_INFO, "\n");

    // RS-TODO should still free channels?
    if (serverGetShareRefCount(&g_resServ, pShared) > 1)
    {
        // Remove this kchangrpapi object from the list of owners in the shared object
        listRemoveFirstByValue(&pKernelChannelGroupApi->pKernelChannelGroup->apiObjList, &pKernelChannelGroupApi);
        goto done;
    }

    if (pKernelChannelGroup != NULL)
        kchangrpSetRealtime_HAL(pGpu, pKernelChannelGroup, NV_FALSE);

    // If channels still exist in this group, free them
    // RS-TODO this can be removed after re-parenting support is added
    it = kchannelGetIter(pClient, pResourceRef);
    while (clientRefOrderedIterNext(pClient, &it))
    {
        NV_STATUS tmpStatus;

        tmpStatus = pRmApi->Free(pRmApi, pClient->hClient, it.pResourceRef->hResource);
        if ((tmpStatus != NV_OK) && (rmStatus == NV_OK))
            rmStatus = tmpStatus;
    }

    NV_ASSERT(rmStatus == NV_OK);

    if (pKernelChannelGroup != NULL)
    {
        kchangrpDestroy(pGpu, pKernelChannelGroup);

        if (pKernelChannelGroup->pCtxBufPool != NULL)
        {
            ctxBufPoolRelease(pKernelChannelGroup->pCtxBufPool);
            ctxBufPoolDestroy(&pKernelChannelGroup->pCtxBufPool);
        }

        if (pKernelChannelGroup->pChannelBufPool != NULL)
        {
            ctxBufPoolRelease(pKernelChannelGroup->pChannelBufPool);
            ctxBufPoolDestroy(&pKernelChannelGroup->pChannelBufPool);
        }

        listClear(&pKernelChannelGroup->apiObjList);
    }

done:
    serverFreeShare(&g_resServ, pShared);

    pParams->status = rmStatus;
}

NV_STATUS
kchangrpapiCopyConstruct_IMPL
(
    KernelChannelGroupApi        *pKernelChannelGroupApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsClient *pDstClient = pCallContext->pClient;
    RsResourceRef *pDstRef = pCallContext->pResourceRef;
    RsResourceRef *pSrcRef = pParams->pSrcRef;
    KernelChannelGroupApi *pChanGrpSrc = dynamicCast(pSrcRef->pResource,
                                                     KernelChannelGroupApi);
    RS_ITERATOR iter;
    OBJGPU       *pGpu   = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    NV_STATUS     status = NV_OK;
    RsResourceRef *pVaspaceRef = NULL;
    VaSpaceApi *pVaspaceApi = NULL;

    pKernelChannelGroupApi->hKernelGraphicsContext  = NV01_NULL_OBJECT;
    pKernelChannelGroupApi->hLegacykCtxShareSync    = NV01_NULL_OBJECT;
    pKernelChannelGroupApi->hLegacykCtxShareAsync   = NV01_NULL_OBJECT;

    pKernelChannelGroupApi->pKernelChannelGroup =
        pChanGrpSrc->pKernelChannelGroup;
    serverRefShare(&g_resServ,
        staticCast(pKernelChannelGroupApi->pKernelChannelGroup, RsShared));

    iter =  serverutilRefIter(pDstClient->hClient, pDstRef->pParentRef->hResource, classId(VaSpaceApi), RS_ITERATE_DESCENDANTS, NV_TRUE);
    while (clientRefIterNext(iter.pClient, &iter))
    {
        pVaspaceRef = iter.pResourceRef;
        pVaspaceApi = dynamicCast(pVaspaceRef->pResource, VaSpaceApi);
        NV_ASSERT_OR_RETURN(pVaspaceApi != NULL, NV_ERR_INVALID_STATE);

        if (pVaspaceApi->pVASpace ==
            pKernelChannelGroupApi->pKernelChannelGroup->pVAS)
        {
            refAddDependant(pVaspaceRef, pDstRef);
            break;
        }
    }

    if (pChanGrpSrc->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->DupObject(pRmApi,
                              pDstClient->hClient,
                              pDstRef->hResource,
                              &pKernelChannelGroupApi->hKernelGraphicsContext,
                              pParams->pSrcClient->hClient,
                              pChanGrpSrc->hKernelGraphicsContext,
                              0),
            fail);
    }

    //
    // If this channel group is in legacy mode, new client needs its own handles to the
    // sync and async internally allocated kctxshares
    //
    if (pChanGrpSrc->pKernelChannelGroup->bLegacyMode)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->DupObject(pRmApi,
                              pDstClient->hClient,
                              pDstRef->hResource,
                              &pKernelChannelGroupApi->hLegacykCtxShareSync,
                              pParams->pSrcClient->hClient,
                              pChanGrpSrc->hLegacykCtxShareSync,
                              0),
            fail);

        // All chips have SYNC, Some chips won't have an ASYNC kctxshare
        if (pChanGrpSrc->hLegacykCtxShareAsync != 0)
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                pRmApi->DupObject(pRmApi,
                                  pDstClient->hClient,
                                  pDstRef->hResource,
                                  &pKernelChannelGroupApi->hLegacykCtxShareAsync,
                                  pParams->pSrcClient->hClient,
                                  pChanGrpSrc->hLegacykCtxShareAsync,
                                  0),
            fail);
        }
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_DUP_OBJECT(pGpu, pDstClient->hClient, pDstRef->pParentRef->hResource, pDstRef->hResource,
                             pParams->pSrcClient->hClient, pSrcRef->hResource, 0,
                             NV_TRUE, // automatically issue RPC_FREE on object free
                             pDstRef, status);

        if (status != NV_OK)
            goto fail;
    }

    if (listAppendValue(&pKernelChannelGroupApi->pKernelChannelGroup->apiObjList, &pKernelChannelGroupApi) == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto fail;
    }

    return status;

fail:
    if (pKernelChannelGroupApi->hLegacykCtxShareAsync != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, pDstClient->hClient,
                     pKernelChannelGroupApi->hLegacykCtxShareAsync);
    }
    if (pKernelChannelGroupApi->hLegacykCtxShareSync != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, pDstClient->hClient,
                     pKernelChannelGroupApi->hLegacykCtxShareSync);
    }
    if (pKernelChannelGroupApi->hKernelGraphicsContext != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, pDstClient->hClient,
                     pKernelChannelGroupApi->hKernelGraphicsContext);
    }

    serverFreeShare(&g_resServ,
        staticCast(pKernelChannelGroupApi->pKernelChannelGroup, RsShared));

    return status;
}

NvBool
kchangrpapiCanCopy_IMPL
(
    KernelChannelGroupApi *pKernelChannelGroupApi
)
{
    return NV_TRUE;
}

NV_STATUS
CliGetChannelGroup
(
    NvHandle                 hClient,
    NvHandle                 hChanGrp,
    RsResourceRef          **ppChanGrpRef,
    NvHandle                *phDevice
)
{
    NV_STATUS status;
    RsClient *pRsClient;
    RsResourceRef *pResourceRef;
    RsResourceRef *pParentRef;

    if (!ppChanGrpRef)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = serverGetClientUnderLock(&g_resServ, hClient, &pRsClient);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
        return status;

    status = clientGetResourceRefByType(pRsClient, hChanGrp,
                                        classId(KernelChannelGroupApi),
                                        &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppChanGrpRef = pResourceRef;

    if (phDevice)
    {
        pParentRef = pResourceRef->pParentRef;
        *phDevice = pParentRef->hResource;
    }

    return NV_OK;
}

/*!
 * @brief Use TSG in legacy mode
 *
 * In legacy mode, RM pre-allocates the subcontexts in a TSG.
 * This is needed for the following reasons:
 *
 *  1. We are also using subcontext to represent TSG contexts in pre-VOLTA chips (see below).
 *     But RM clients haven't yet moved to the subcontext model in production code.
 *     So RM implicitly creates it for them, until they make the switch.
 *
 *  2. Pre-VOLTA, we only support one address space in a TSG.
 *     Preallocating the subcontext prevents accidental use of multiple address spaces within a TSG.
 *     So we use the vaspace specified/implied at TSG creation to create the subcontexts.
 *
 *  3. Tests and clients on VOLTA that don't explicitly specify subcontexts need to behave similar
 *     to previous chips until they allocate the kctxshares themselves.
 *
 *  Legacy subcontexts are interpreted in the following ways:
 *
 *     VOLTA+            : subcontext 0 is VEID 0, subcontext 1 is VEID 1
 *     GM20X thru PASCAL : subcontext 0 is SCG type 0, subcontext 1 is SCG type 1
 *     pre-GM20X         : just a single subcontext 0; no SCG or VEIDs attached to it.
 *
 * @param[in] pKernelChannelGroupApi Channel group pointer
 * @param[in] pGpu                   GPU object pointer
 * @param[in] pKernelFifo            FIFO object pointer
 * @param[in] hClient                Client handle
 *
 */
NV_STATUS
kchangrpapiSetLegacyMode_IMPL
(
    KernelChannelGroupApi *pKernelChannelGroupApi,
    OBJGPU                *pGpu,
    KernelFifo            *pKernelFifo,
    NvHandle               hClient
)
{
    KernelChannelGroup *pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;
    NvHandle hTsg = RES_GET_HANDLE(pKernelChannelGroupApi);
    NvHandle hkCtxShare = 0;
    NV_STATUS status = NV_OK;
    NvU32 maxSubctx = 0;
    NvU64 numMax = 0;
    NvU64 numFree = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelChannelGroupApiListIter it;

    NV_CTXSHARE_ALLOCATION_PARAMETERS kctxshareParams = { 0 };

    ct_assert(NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC == 0);
    ct_assert(NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC == 1);

    NV_ASSERT_OK(pKernelChannelGroup->pSubctxIdHeap->eheapGetSize(
        pKernelChannelGroup->pSubctxIdHeap,
        &numMax));

    NV_ASSERT_OK(pKernelChannelGroup->pSubctxIdHeap->eheapGetFree(
        pKernelChannelGroup->pSubctxIdHeap,
        &numFree));

    NV_ASSERT(numMax ==
              kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu, pKernelFifo,
                                                         pKernelChannelGroup,
                                                         NV_FALSE));

    NV_ASSERT_OR_RETURN(numMax == numFree && numMax != 0, NV_ERR_INVALID_STATE);

    pKernelChannelGroup->pSubctxIdHeap->eheapDestruct(
        pKernelChannelGroup->pSubctxIdHeap);
    pKernelChannelGroup->pVaSpaceIdHeap->eheapDestruct(
        pKernelChannelGroup->pVaSpaceIdHeap);
    //
    // There should only be 1 (SYNC) or 2 legacy kctxshares (SYNC + ASYNC),
    // depending on chip
    //
    maxSubctx = kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu, pKernelFifo,
                                                           pKernelChannelGroup,
                                                           NV_TRUE);
    NV_ASSERT_OR_RETURN(numMax == numFree, NV_ERR_INVALID_STATE);
    NV_ASSERT(maxSubctx == 1 || maxSubctx == 2);

    constructObjEHeap(pKernelChannelGroup->pSubctxIdHeap,
                      0, maxSubctx, sizeof(KernelCtxShare *), 0);
    constructObjEHeap(pKernelChannelGroup->pVaSpaceIdHeap,
                      0, maxSubctx, sizeof(KernelCtxShare *), 0);

    pKernelChannelGroup->bLegacyMode = NV_TRUE;

    // Allocate SYNC
    hkCtxShare = 0;
    kctxshareParams.hVASpace = 0;
    kctxshareParams.flags    = NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC;
    kctxshareParams.subctxId = 0xFFFFFFFF;

    NV_ASSERT_OK_OR_GOTO(status,
                         pRmApi->AllocWithSecInfo(pRmApi,
                                                  hClient,
                                                  hTsg,
                                                  &hkCtxShare,
                                                  FERMI_CONTEXT_SHARE_A,
                                                  NV_PTR_TO_NvP64(&kctxshareParams),
                                                  sizeof(kctxshareParams),
                                                  RMAPI_ALLOC_FLAGS_SKIP_RPC,
                                                  NvP64_NULL,
                                                  &pRmApi->defaultSecInfo),
                         fail);

    NV_ASSERT(kctxshareParams.subctxId == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_SYNC);

    pKernelChannelGroupApi->hLegacykCtxShareSync = hkCtxShare;

    if(maxSubctx == 2)
    {
        // Allocate ASYNC
        hkCtxShare = 0;
        kctxshareParams.hVASpace = 0;
        kctxshareParams.flags    = NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC;
        kctxshareParams.subctxId = 0xFFFFFFFF;

        NV_ASSERT_OK_OR_GOTO(status,
                             pRmApi->AllocWithSecInfo(pRmApi,
                                                      hClient,
                                                      hTsg,
                                                      &hkCtxShare,
                                                      FERMI_CONTEXT_SHARE_A,
                                                      NV_PTR_TO_NvP64(&kctxshareParams),
                                                      sizeof(kctxshareParams),
                                                      RMAPI_ALLOC_FLAGS_SKIP_RPC,
                                                      NvP64_NULL,
                                                      &pRmApi->defaultSecInfo),
                             fail);

        NV_ASSERT(kctxshareParams.subctxId == NV_CTXSHARE_ALLOCATION_FLAGS_SUBCONTEXT_ASYNC);

        pKernelChannelGroupApi->hLegacykCtxShareAsync = hkCtxShare;
    }

    NV_ASSERT_OK_OR_GOTO(status,
                         pKernelChannelGroup->pSubctxIdHeap->eheapGetFree(
                             pKernelChannelGroup->pSubctxIdHeap,
                             &numFree),
                         fail);

    NV_ASSERT_OR_GOTO(numFree == 0, fail);

    //
    // If this channel group has been duped, we need to provide kctxshareApi handles to the
    // other channelGroupApi objects that share this channel group since the handles will
    // only work for a single client.
    //
    it = listIterAll(&pKernelChannelGroup->apiObjList);
    while (listIterNext(&it))
    {
        KernelChannelGroupApi *pChanGrpDest = *it.pValue;

        if(pChanGrpDest == pKernelChannelGroupApi)
            continue;

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->DupObject(pRmApi,
                              RES_GET_CLIENT_HANDLE(pChanGrpDest),
                              RES_GET_HANDLE(pChanGrpDest),
                              &pChanGrpDest->hLegacykCtxShareSync,
                              RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi),
                              pKernelChannelGroupApi->hLegacykCtxShareSync,
                              0),
            fail);

        if (maxSubctx == 2)
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                pRmApi->DupObject(pRmApi,
                                  RES_GET_CLIENT_HANDLE(pChanGrpDest),
                                  RES_GET_HANDLE(pChanGrpDest),
                                  &pChanGrpDest->hLegacykCtxShareAsync,
                                  RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi),
                                  pKernelChannelGroupApi->hLegacykCtxShareAsync,
                                  0),
            fail);
        }
    }

    return status;

fail:
    NV_PRINTF(LEVEL_ERROR, "Failed to set channel group in legacy mode.\n");

    pKernelChannelGroup->bLegacyMode = NV_FALSE;

    it = listIterAll(&pKernelChannelGroup->apiObjList);

    while (listIterNext(&it))
    {
        KernelChannelGroupApi *pChanGrpIt = *it.pValue;

        if (pChanGrpIt->hLegacykCtxShareSync != 0)
        {
           pRmApi->Free(pRmApi, RES_GET_CLIENT_HANDLE(pChanGrpIt), pChanGrpIt->hLegacykCtxShareSync);
           pChanGrpIt->hLegacykCtxShareSync = 0;
        }

        if (pChanGrpIt->hLegacykCtxShareAsync != 0)
        {
           pRmApi->Free(pRmApi, RES_GET_CLIENT_HANDLE(pChanGrpIt), pChanGrpIt->hLegacykCtxShareAsync);
           pChanGrpIt->hLegacykCtxShareAsync = 0;
        }
    }

    if(status == NV_OK)
    {
        status = NV_ERR_INVALID_STATE;
    }

    return status;
}

NV_STATUS
kchangrpapiCtrlCmdGpFifoSchedule_IMPL
(
    KernelChannelGroupApi              *pKernelChannelGroupApi,
    NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams
)
{
    OBJGPU              *pGpu         = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    RsResourceRef       *pResourceRef = RES_GET_REF(pKernelChannelGroupApi);
    KernelChannelGroup  *pKernelChannelGroup = NULL;
    NV_STATUS            status       = NV_OK;
    KernelFifo          *pKernelFifo;
    CLASSDESCRIPTOR     *pClass       = NULL;
    CHANNEL_NODE        *pChanNode    = NULL;
    CHANNEL_LIST        *pChanList    = NULL;
    NvU32                runlistId    = INVALID_RUNLIST_ID;
    RM_API              *pRmApi       = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        return NV_ERR_INVALID_OBJECT;
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

    if (gpuGetClassByClassId(pGpu, pResourceRef->externalClassId, &pClass) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pResourceRef->externalClassId);
    }
    NV_ASSERT_OR_RETURN((pClass != NULL), NV_ERR_NOT_SUPPORTED);

    //
    // Bug 1737765: Prevent Externally Owned Channels from running unless bound
    //  It is possible for clients to allocate and schedule channels while
    //  skipping the UVM registration step which binds the appropriate
    //  allocations in RM. We need to fail channel scheduling if the channels
    //  have not been registered with UVM.
    //  We include this check for every channel in the group because it is
    //  expected that Volta+ may use a separate VAS for each channel.
    //

    pChanList = pKernelChannelGroup->pChanList;

    for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
    {
        NV_CHECK_OR_RETURN(LEVEL_NOTICE, kchannelIsSchedulable_HAL(pGpu, pChanNode->pKernelChannel),
            NV_ERR_INVALID_STATE);
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
    pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    pChanList = pKernelChannelGroup->pChanList;

    //
    // Some channels may not have objects allocated on them, so they won't have
    // a runlist committed yet.  Force them all onto the same runlist so the
    // low level code knows what do to with them.
    //
    // First we walk through the channels to see if there is a runlist assigned
    // already and if so are the channels consistent.
    //
    runlistId = pKernelChannelGroup->runlistId; // Start with TSG runlistId
    for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
    {
        KernelChannel *pKernelChannel = pChanNode->pKernelChannel;

        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);

        if (kchannelIsRunlistSet(pGpu, pKernelChannel))
        {
            if (runlistId == INVALID_RUNLIST_ID)
            {
                runlistId = kchannelGetRunlistId(pKernelChannel);
            }
            else // Catch if 2 channels in the same TSG have different runlistId
            {
                if (runlistId != kchannelGetRunlistId(pKernelChannel))
                {
                    NV_PRINTF(LEVEL_ERROR,
                        "Channels in TSG %d have different runlist IDs this should never happen!\n",
                        pKernelChannelGroup->grpID);
                    DBG_BREAKPOINT();
                }
            }
        }
    }

    // If no channels have a runlist set, get the default and use it.
    if (runlistId == INVALID_RUNLIST_ID)
    {
        runlistId = kfifoGetDefaultRunlist_HAL(pGpu, pKernelFifo,
            pKernelChannelGroup->engineType);
    }

    // We can rewrite TSG runlist id just as we will do that for all TSG channels below
    pKernelChannelGroup->runlistId = runlistId;

    //
    // Now go through and force any channels w/o the runlist set to use either
    // the default or whatever we found other channels to be allocated on.
    //
    for (pChanNode = pChanList->pHead; pChanNode; pChanNode = pChanNode->pNext)
    {
        KernelChannel *pKernelChannel = pChanNode->pKernelChannel;

        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);

        if (!kchannelIsRunlistSet(pGpu, pKernelChannel))
        {
            kfifoRunlistSetId_HAL(pGpu, pKernelFifo, pKernelChannel, runlistId);
        }
    }
    SLI_LOOP_END

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NvHandle hClient = RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi);
        NvHandle hObject = RES_GET_HANDLE(pKernelChannelGroupApi);

        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }


    //
    // Do an internal control call to do channel reset
    // on Host (Physical) RM
    //
    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi),
                             RES_GET_HANDLE(pKernelChannelGroupApi),
                             NVA06C_CTRL_CMD_INTERNAL_GPFIFO_SCHEDULE,
                             pSchedParams,
                             sizeof(NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS));

    return status;
}

NV_STATUS
kchangrpapiCtrlCmdBind_IMPL
(
    KernelChannelGroupApi   *pKernelChannelGroupApi,
    NVA06C_CTRL_BIND_PARAMS *pParams
)
{
    NV_STATUS     rmStatus = NV_OK;
    OBJGPU       *pGpu     = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    Device       *pDevice  = GPU_RES_GET_DEVICE(pKernelChannelGroupApi);
    CHANNEL_NODE *pChanNode;
    RM_ENGINE_TYPE localEngineType;
    RM_ENGINE_TYPE globalEngineType;
    ENGDESCRIPTOR engineDesc;
    NvBool        bMIGInUse = IS_MIG_IN_USE(pGpu);

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);

    localEngineType = globalEngineType = gpuGetRmEngineType(pParams->engineType);

    if (bMIGInUse)
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              localEngineType,
                                              &globalEngineType));
    }

    NV_PRINTF(LEVEL_INFO,
              "Binding TSG %d to Engine %d (%d)\n",
              pKernelChannelGroupApi->pKernelChannelGroup->grpID,
              gpuGetNv2080EngineType(globalEngineType), globalEngineType);

    // Translate globalEnginetype -> enginedesc
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        gpuXlateClientEngineIdToEngDesc(pGpu, globalEngineType, &engineDesc));

    // Translate engineDesc -> runlistId for TSG
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
        kfifoEngineInfoXlate_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            ENGINE_INFO_TYPE_ENG_DESC,
            engineDesc,
            ENGINE_INFO_TYPE_RUNLIST,
            &pKernelChannelGroupApi->pKernelChannelGroup->runlistId));

    for (pChanNode =
             pKernelChannelGroupApi->pKernelChannelGroup->pChanList->pHead;
         pChanNode != NULL;
         pChanNode = pChanNode->pNext)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
            kchannelBindToRunlist(pChanNode->pKernelChannel,
                                  localEngineType,
                                  engineDesc));
        if (rmStatus != NV_OK)
        {
            break;
        }
    }

    return rmStatus;
}

NV_STATUS
kchangrpapiCtrlCmdGetTimeslice_IMPL
(
    KernelChannelGroupApi        *pKernelChannelGroupApi,
    NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams
)
{
    KernelChannelGroup *pKernelChannelGroup = NULL;

    if (pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        return NV_ERR_INVALID_OBJECT;
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

    pTsParams->timesliceUs = pKernelChannelGroup->timesliceUs;

    return NV_OK;
}

NV_STATUS
kchangrpapiCtrlCmdSetTimeslice_IMPL
(
    KernelChannelGroupApi        *pKernelChannelGroupApi,
    NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams
)
{
    OBJGPU             *pGpu                = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    RsResourceRef      *pResourceRef        = RES_GET_REF(pKernelChannelGroupApi);
    KernelChannelGroup *pKernelChannelGroup = NULL;
    NV_STATUS           status              = NV_OK;
    CLASSDESCRIPTOR    *pClass              = NULL;
    RM_API             *pRmApi              = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        return NV_ERR_INVALID_OBJECT;
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

    if (gpuGetClassByClassId(pGpu, pResourceRef->externalClassId, &pClass) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pResourceRef->externalClassId);
    }
    NV_ASSERT_OR_RETURN((pClass != NULL), NV_ERR_NOT_SUPPORTED);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NvHandle hClient = RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi);
        NvHandle hObject = RES_GET_HANDLE(pKernelChannelGroupApi);
        NVA06C_CTRL_TIMESLICE_PARAMS *pParams = (NVA06C_CTRL_TIMESLICE_PARAMS *)(pRmCtrlParams->pParams);

        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        // Update guest RM's internal bookkeeping with the timeslice.
        if (status == NV_OK)
        {
            pKernelChannelGroup->timesliceUs = pParams->timesliceUs;
        }

        return status;
    }

    //
    // Do an internal control call to do channel reset
    // on Host (Physical) RM
    //
    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi),
                             RES_GET_HANDLE(pKernelChannelGroupApi),
                             NVA06C_CTRL_CMD_INTERNAL_SET_TIMESLICE,
                             pTsParams,
                             sizeof(NVA06C_CTRL_TIMESLICE_PARAMS));

    return status;
}

NV_STATUS
kchangrpapiCtrlCmdGetInfo_IMPL
(
    KernelChannelGroupApi       *pKernelChannelGroupApi,
    NVA06C_CTRL_GET_INFO_PARAMS *pParams
)
{
    KernelChannelGroup *pKernelChannelGroup = NULL;

    if (pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        return NV_ERR_INVALID_OBJECT;
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

    pParams->tsgID = pKernelChannelGroup->grpID;

    return NV_OK;
}

NV_STATUS
kchangrpapiCtrlCmdSetInterleaveLevel_IMPL
(
    KernelChannelGroupApi               *pKernelChannelGroupApi,
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams
)
{
    OBJGPU          *pGpu         = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    RsResourceRef   *pResourceRef = RES_GET_REF(pKernelChannelGroupApi);
    KernelChannelGroup *pKernelChannelGroup =
        pKernelChannelGroupApi->pKernelChannelGroup;
    CLASSDESCRIPTOR *pClass       = NULL;
    NV_STATUS        status       = NV_OK;

    if (gpuGetClassByClassId(pGpu, pResourceRef->externalClassId, &pClass) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pResourceRef->externalClassId);
    }
    NV_ASSERT_OR_RETURN((pClass != NULL), NV_ERR_NOT_SUPPORTED);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NvHandle hClient = RES_GET_CLIENT_HANDLE(pKernelChannelGroupApi);
        NvHandle hObject = RES_GET_HANDLE(pKernelChannelGroupApi);

        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, NV_ERR_NOT_SUPPORTED);
    }

    status = kchangrpSetInterleaveLevel(pGpu, pKernelChannelGroup, pParams->tsgInterleaveLevel);

    return status;
}

NV_STATUS
kchangrpapiCtrlCmdGetInterleaveLevel_IMPL
(
    KernelChannelGroupApi               *pKernelChannelGroupApi,
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams
)
{
    KernelChannelGroup *pKernelChannelGroup = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelChannelGroupApi);
    NvU32 subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    if (pKernelChannelGroupApi->pKernelChannelGroup == NULL)
        return NV_ERR_INVALID_OBJECT;
    pKernelChannelGroup = pKernelChannelGroupApi->pKernelChannelGroup;

    pParams->tsgInterleaveLevel = pKernelChannelGroup->pInterleaveLevel[subdevInst];

    return NV_OK;
}

