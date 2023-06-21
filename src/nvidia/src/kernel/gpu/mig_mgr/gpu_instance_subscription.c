/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing GPU instance subscriptions
 *
 *****************************************************************************/

// FIXME XXX
#define NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED

#define NVOC_GPU_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "kernel/gpu/mig_mgr/gpu_instance_subscription.h"
#include "kernel/gpu/mig_mgr/compute_instance_subscription.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "ctrl/ctrlc637.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "gpu/gpu_uuid.h"
#include "vgpu/rpc.h"
#include "rmapi/control.h"

static inline NvBool
_gisubscriptionClientSharesVASCrossPartition
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    CALL_CONTEXT *pCallContext,
    NvU32 targetedSwizzId
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    RsResourceRef *pDeviceRef;
    Device *pDevice;
    MIG_INSTANCE_REF shareRef;

    NV_ASSERT_OR_RETURN(pGPUInstanceSubscription != NULL, NV_TRUE);

    NV_ASSERT_OK(
        refFindAncestorOfType(pCallContext->pResourceRef,
                              classId(Device), &pDeviceRef));
    pDevice = dynamicCast(pDeviceRef->pResource, Device);

    if (pDevice->hClientShare == NV01_NULL_OBJECT)
    {
        // NULL Client Share : Legacy Global VASpace. This is cross-GPU-instance.
        return NV_TRUE;
    }
    else if (pDevice->hClientShare == pCallContext->pClient->hClient)
    {
        // Same Client Share : Self Scoped VASpace. This is not cross-GPU-instance.
        return NV_FALSE;
    }

    //
    // Different Client Share. Device default VASpace is shared between this
    // client and hClientShare. The VAS is cross-GPU-instance if the sharing client
    // is subscribed to a different GPU instance than the subscription request, or
    // if the sharing client isn't subscribed to any GPU instance.
    //
    status = kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager,
                                             pDevice->hClientShare,
                                             &shareRef);


    return (status != NV_OK) || (shareRef.pKernelMIGGpuInstance->swizzId != targetedSwizzId);
}

NV_STATUS
gisubscriptionConstruct_IMPL
(
    GPUInstanceSubscription      *pGPUInstanceSubscription,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    NVC637_ALLOCATION_PARAMETERS *pUserParams = pRmAllocParams->pAllocParams;
    RsClient *pRsClient = pCallContext->pClient;
    OBJGPU *pGpu;
    KernelMIGManager *pKernelMIGManager;
    NvU32 swizzId;
    NV_STATUS status;

    pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);

    osRmCapInitDescriptor(&pGPUInstanceSubscription->dupedCapDescriptor);

    if (RS_IS_COPY_CTOR(pRmAllocParams))
        return gisubscriptionCopyConstruct_IMPL(pGPUInstanceSubscription, pCallContext, pRmAllocParams);

    pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    swizzId = pUserParams->swizzId;

    if (!IS_MIG_ENABLED(pGpu))
    {
        NV_ASSERT_FAILED("Subscription failed: MIG not enabled\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Disable RMCTRL Cache before subscribe to GPU instance.
    // RMCTRL-CACHE-TODO: remove the workaround when CORERM-5016 is done.
    //
    rmapiControlCacheSetMode(NV0000_CTRL_SYSTEM_RMCTRL_CACHE_MODE_CTRL_MODE_DISABLE);

    //
    // Root-SwizzID is a special swizzID which doesn't have any GPU instance
    // associated with it. It can be subscribed to even without GPU instances
    //
    if (swizzId == NVC637_DEVICE_PROFILING_SWIZZID)
    {
        // Check if this is a root-client or un-privileged device profiling is allowed
        if (gpuIsRmProfilingPrivileged(pGpu) &&
            !rmclientIsAdminByHandle(pRmAllocParams->hClient, pCallContext->secInfo.privLevel))
        {
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }

        if (kmigmgrIsDeviceProfilingInUse(pGpu, pKernelMIGManager))
        {
            // Only one DeviceProfiling session is allowed to be used with-in a system
            NV_PRINTF(LEVEL_ERROR,
                      "Subscription failed: Device-Level-Monitoring already in use\n");
            return NV_ERR_INVALID_STATE;
        }

        // Mark the root swizzID in use and return
        NV_ASSERT_OK_OR_RETURN(kmigmgrSetDeviceProfilingInUse(pGpu, pKernelMIGManager));
        pGPUInstanceSubscription->bDeviceProfiling = NV_TRUE;
        goto done;
    }
    else
    {
        pGPUInstanceSubscription->bDeviceProfiling = NV_FALSE;
    }

    if (!IS_MIG_IN_USE(pGpu))
    {
        NV_ASSERT_FAILED("Subscription failed: MIG GPU instancing not done\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!kmigmgrIsSwizzIdInUse(pGpu, pKernelMIGManager, swizzId))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Subscription failed: swizzid 0x%0x doesn't exist!\n",
                  swizzId);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (_gisubscriptionClientSharesVASCrossPartition(pGPUInstanceSubscription, pCallContext, swizzId))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Subscription failed: Client shares VAS with client not subscribed to target GPU instance!\n");
        return NV_ERR_STATE_IN_USE;
    }

    NV_ASSERT_OK_OR_RETURN(
        kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager, swizzId,
                                  &pGPUInstanceSubscription->pKernelMIGGpuInstance));

    // For now skip kernel clients, such as UVM, until Bug 2729768 is fixed.
    if (pRsClient->type == CLIENT_TYPE_USER)
    {
        status = osRmCapAcquire(pGPUInstanceSubscription->pKernelMIGGpuInstance->pOsRmCaps,
                                NV_RM_CAP_SMC_PARTITION_ACCESS,
                                pUserParams->capDescriptor,
                                &pGPUInstanceSubscription->dupedCapDescriptor);
        if ((status != NV_ERR_NOT_SUPPORTED) && (status != NV_OK))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Capability validation failed: swizzid 0x%0x!\n",
                      swizzId);
            return status;
        }
    }

    status = kmigmgrIncRefCount(pGPUInstanceSubscription->pKernelMIGGpuInstance->pShare);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU instance ref-counting failed: swizzid 0x%0x!\n",
                  swizzId);
        goto cleanup_duped_desc;
    }

done:
    NV_PRINTF(LEVEL_INFO, "Client 0x%x subscribed to swizzid 0x%0x.\n",
              pRmAllocParams->hClient, swizzId);

    return NV_OK;

cleanup_duped_desc:
    osRmCapRelease(pGPUInstanceSubscription->dupedCapDescriptor);

    return status;
}

NV_STATUS
gisubscriptionCopyConstruct_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pSrcRef = pParams->pSrcRef;
    GPUInstanceSubscription *pGPUInstanceSubscriptionSrc = dynamicCast(pSrcRef->pResource, GPUInstanceSubscription);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);

    {
        // non kernel clients are not allowed to dup GPU instances
        NV_CHECK_OR_RETURN(LEVEL_SILENT, pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL,
                           NV_ERR_NOT_SUPPORTED);
    }

    if (pGPUInstanceSubscriptionSrc->bDeviceProfiling)
    {
        // Duping of root-swizzId is not allowed
        NV_PRINTF(LEVEL_ERROR,
                      "Subscription failed: Duping not allowed for Device-level-SwizzId\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        RsResourceRef *pDstRef = pCallContext->pResourceRef;
        NV_STATUS status = NV_OK;

        NV_RM_RPC_DUP_OBJECT(pGpu,
            pCallContext->pClient->hClient,
            pDstRef->pParentRef->hResource,
            pDstRef->hResource,
            pParams->pSrcClient->hClient,
            pSrcRef->hResource,
            0, NV_TRUE, // Send RPC for object free
            pDstRef, status);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    pGPUInstanceSubscription->pKernelMIGGpuInstance = pGPUInstanceSubscriptionSrc->pKernelMIGGpuInstance;
    // TODO XXX tracking this to support CI subscription bypass path for UVM
    pGPUInstanceSubscription->bIsDuped = NV_TRUE;

    NV_ASSERT_OK(
        kmigmgrIncRefCount(pGPUInstanceSubscription->pKernelMIGGpuInstance->pShare));

    return NV_OK;
}

void
gisubscriptionDestruct_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    CALL_CONTEXT *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    resGetFreeParams(staticCast(pGPUInstanceSubscription, RsResource), &pCallContext, &pParams);

    if (pGPUInstanceSubscription->bDeviceProfiling)
    {
        kmigmgrClearDeviceProfilingInUse(pGpu, pKernelMIGManager);
        pGPUInstanceSubscription->bDeviceProfiling = NV_FALSE;
        return;
    }

    NV_ASSERT_OK(
        kmigmgrDecRefCount(pGPUInstanceSubscription->pKernelMIGGpuInstance->pShare));

    osRmCapRelease(pGPUInstanceSubscription->dupedCapDescriptor);

    gisubscriptionCleanupOnUnsubscribe(pCallContext);

    NV_PRINTF(LEVEL_INFO, "Client 0x%x unsubscribed from swizzid 0x%0x.\n",
              RES_GET_CLIENT(pGPUInstanceSubscription)->hClient, pGPUInstanceSubscription->pKernelMIGGpuInstance->swizzId);
}

NvBool
gisubscriptionIsDuped_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription
)
{
    return pGPUInstanceSubscription->bIsDuped;
}

NV_STATUS
gisubscriptionGetGPUInstanceSubscription_IMPL
(
    RsClient *pClient,
    NvHandle  hParent,
    GPUInstanceSubscription **ppGPUInstanceSubscription
)
{
    RsResourceRef *pResourceRef;

    NV_ASSERT_OR_RETURN(NULL != ppGPUInstanceSubscription, NV_ERR_INVALID_ARGUMENT);

    pResourceRef = serverutilFindChildRefByType(pClient->hClient,
                                                hParent, classId(GPUInstanceSubscription),
                                                NV_TRUE);
    if (pResourceRef == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    *ppGPUInstanceSubscription = dynamicCast(pResourceRef->pResource, GPUInstanceSubscription);

    return NV_OK;
}

NvBool
gisubscriptionCanCopy_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription
)
{
    return NV_TRUE;
}

//
// gisubscriptionCtrlCmdExecPartitionsCreate
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
gisubscriptionCtrlCmdExecPartitionsCreate_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_ERROR, "Non-privileged context issued privileged cmd\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
}

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, (pParams->execPartCount <= NVC637_CTRL_MAX_EXEC_PARTITIONS),
                     NV_ERR_INVALID_ARGUMENT);

    // Check for trivial arguments
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pParams->execPartCount > 0, NV_WARN_NOTHING_TO_DO);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize, status);

        // Only continue if execution partition creation succeeded in the host
        NV_ASSERT_OK_OR_RETURN(status);
    }

    if (!IS_GSP_CLIENT(pGpu))
    {
        KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS request =
        {
            .type = KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_REQUEST,
            .inst.request.count = pParams->execPartCount,
            .inst.request.pReqComputeInstanceInfo = pParams->execPartInfo,
            .inst.request.requestFlags = pParams->flags
        };

        if (!hypervisorIsVgxHyper())
        {
            request.inst.request.requestFlags = FLD_SET_DRF(C637_CTRL, _DMA_EXEC_PARTITIONS_CREATE_REQUEST, _WITH_PART_ID, _FALSE, request.inst.request.requestFlags);
        }

        if (IS_VIRTUAL(pGpu))
        {
            status = kmigmgrCreateComputeInstances_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                                       pParams->bQuery,
                                                       request,
                                                       pParams->execPartId,
                                                       NV_TRUE /* create MIG compute instance capabilities */);
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        NvU32 i;

        for (i = 0; i < pParams->execPartCount; i++)
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
            NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS export;
            GPUMGR_SAVE_COMPUTE_INSTANCE save;
            KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS restore =
            {
                .type = KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE,
                .inst.restore.pComputeInstanceSave = &save,
            };
            portMemSet(&export, 0, sizeof(export));
            export.id = pParams->execPartId[i];

            // Retrieve the CI state created by GSP-RM, then restore it to CPU-RM
            NV_ASSERT_OK_OR_RETURN(
                pRmApi->Control(pRmApi,
                                pKernelMIGGpuInstance->instanceHandles.hClient,
                                pKernelMIGGpuInstance->instanceHandles.hSubscription,
                                NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT,
                                &export,
                                sizeof(export)));

            portMemSet(&save, 0, sizeof(save));
            save.bValid = NV_TRUE;
            save.id = pParams->execPartId[i];
            save.ciInfo = export.info;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrCreateComputeInstances_HAL(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                                  NV_FALSE, restore, &pParams->execPartId[i], NV_TRUE));
        }
    }

    //
    // Generate a subdevice event stating something has changed in GPU instance
    // config. Clients currently do not care about changes and their scope
    //
    if (!pParams->bQuery)
    {
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SMC_CONFIG_UPDATE, NULL,
                                0, 0, 0);
    }

    return status;
}

//
// gisubscriptionCtrlCmdExecPartitionsDelete
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
gisubscriptionCtrlCmdExecPartitionsDelete_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    NvU32 execPartIdx;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_ERROR, "Non-privileged context issued privileged cmd\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
}

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED),
                        NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pParams->execPartCount <= NVC637_CTRL_MAX_EXEC_PARTITIONS,
                     NV_ERR_INVALID_ARGUMENT);

    // Check for trivial arguments
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pParams->execPartCount > 0, NV_WARN_NOTHING_TO_DO);

    // Check that the passed indices are valid compute instances
    for (execPartIdx = 0; execPartIdx < pParams->execPartCount; ++execPartIdx)
    {
        NvU32 execPartId = pParams->execPartId[execPartIdx];
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            execPartId < KMIGMGR_MAX_COMPUTE_INSTANCES,
            NV_ERR_INVALID_ARGUMENT);
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            pKernelMIGGpuInstance->MIGComputeInstance[execPartId].bValid,
            NV_ERR_INVALID_ARGUMENT);
    }

    for (execPartIdx = 0; execPartIdx < pParams->execPartCount; ++execPartIdx)
    {
        if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrDeleteComputeInstance(pGpu, pKernelMIGManager, pKernelMIGGpuInstance,
                                             pParams->execPartId[execPartIdx],
                                             NV_FALSE));
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    //
    // Generate a subdevice event stating something has changed in GPU instance
    // config. Clients currently do not care about changes and their scope
    //
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_SMC_CONFIG_UPDATE, NULL, 0, 0, 0);

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize, status);

        NV_ASSERT_OK_OR_RETURN(status);
    }

    return status;
}

//
// gisubscriptionCtrlCmdExecPartitionsGet
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
gisubscriptionCtrlCmdExecPartitionsGet_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    ComputeInstanceSubscription *pComputeInstanceSubscription = NULL;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    NvU32 ciIdx;
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvBool bEnumerateAll = NV_FALSE;

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    // Capability checks shouldn't be done on
    if (!RMCFG_FEATURE_PLATFORM_GSP)
    {
        bEnumerateAll = rmclientIsCapableOrAdminByHandle(hClient,
                                                         NV_RM_CAP_SYS_SMC_CONFIG,
                                                         pCallContext->secInfo.privLevel);
    }

    MIG_COMPUTE_INSTANCE *pTargetComputeInstanceInfo = NULL;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED),
                        NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    (void)cisubscriptionGetComputeInstanceSubscription(RES_GET_CLIENT(pGPUInstanceSubscription), RES_GET_HANDLE(pGPUInstanceSubscription), &pComputeInstanceSubscription);
    if (pComputeInstanceSubscription != NULL)
    {
        pTargetComputeInstanceInfo = pComputeInstanceSubscription->pMIGComputeInstance;
    }
    else if (!bEnumerateAll)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    pParams->execPartCount = 0;
    for (ciIdx = 0;
         ciIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
         ++ciIdx)
    {
        NVC637_CTRL_EXEC_PARTITIONS_INFO *pOutInfo;
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance =
            &pKernelMIGGpuInstance->MIGComputeInstance[ciIdx];

        if (!pMIGComputeInstance->bValid)
            continue;

        if (!bEnumerateAll && (pMIGComputeInstance != pTargetComputeInstanceInfo))
            continue;

        pParams->execPartId[pParams->execPartCount] = ciIdx;
        pOutInfo = &pParams->execPartInfo[pParams->execPartCount];
        ++pParams->execPartCount;

        pOutInfo->gpcCount = pMIGComputeInstance->resourceAllocation.gpcCount;
        pOutInfo->gfxGpcCount = pMIGComputeInstance->resourceAllocation.gfxGpcCount;
        pOutInfo->veidCount = pMIGComputeInstance->resourceAllocation.veidCount;
        pOutInfo->ceCount = kmigmgrCountEnginesOfType(&pMIGComputeInstance->resourceAllocation.engines,
                                                      RM_ENGINE_TYPE_COPY(0));
        pOutInfo->nvEncCount = kmigmgrCountEnginesOfType(&pMIGComputeInstance->resourceAllocation.engines,
                                                      RM_ENGINE_TYPE_NVENC(0));
        pOutInfo->nvDecCount = kmigmgrCountEnginesOfType(&pMIGComputeInstance->resourceAllocation.engines,
                                                      RM_ENGINE_TYPE_NVDEC(0));
        pOutInfo->nvJpgCount = kmigmgrCountEnginesOfType(&pMIGComputeInstance->resourceAllocation.engines,
                                                      RM_ENGINE_TYPE_NVJPG);
        pOutInfo->ofaCount = kmigmgrCountEnginesOfType(&pMIGComputeInstance->resourceAllocation.engines,
                                                      RM_ENGINE_TYPE_OFA);
        pOutInfo->sharedEngFlag = pMIGComputeInstance->sharedEngFlag;
        pOutInfo->veidStartOffset = pMIGComputeInstance->resourceAllocation.veidOffset;
        pOutInfo->smCount = pMIGComputeInstance->resourceAllocation.smCount;
        pOutInfo->computeSize = pMIGComputeInstance->computeSize;
        pOutInfo->spanStart = pMIGComputeInstance->spanStart;
    }

    return status;
}

//
// gisubscriptionCtrlCmdExecPartitionsGetActiveIds
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    NvU32 ciIdx;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OR_RETURN(pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED),
                        NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(IS_MIG_IN_USE(pGpu), NV_ERR_INVALID_STATE);

    pParams->execPartCount = 0;
    for (ciIdx = 0;
         ciIdx < NV_ARRAY_ELEMENTS(pKernelMIGGpuInstance->MIGComputeInstance);
         ++ciIdx)
    {
        MIG_COMPUTE_INSTANCE *pMIGComputeInstance =
            &pKernelMIGGpuInstance->MIGComputeInstance[ciIdx];

        if (!pMIGComputeInstance->bValid)
            continue;

        pParams->execPartId[pParams->execPartCount] = ciIdx;

        ct_assert(NV_UUID_LEN == NVC637_UUID_LEN);
        ct_assert(NV_UUID_STR_LEN == NVC637_UUID_STR_LEN);

        nvGetSmcUuidString(&pMIGComputeInstance->uuid,
                           pParams->execPartUuid[pParams->execPartCount].str);

        ++pParams->execPartCount;
    }

    return status;
}

NV_STATUS
gisubscriptionCtrlCmdExecPartitionsExport_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KERNEL_MIG_GPU_INSTANCE *pGPUInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    NvU32 gpcIdx;

    // No partitions to export
    if (!IS_MIG_IN_USE(pGpu))
        return NV_ERR_NOT_SUPPORTED;

{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    // An unprivileged client has no use case for import/export
    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
}

    if (IS_VIRTUAL(pGpu))
    {
        // Guest RM does not support import/export
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_STATUS status = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        return status;
    }

    if (pParams->id >= NV_ARRAY_ELEMENTS(pGPUInstance->MIGComputeInstance))
        return NV_ERR_INVALID_ARGUMENT;

    if (!pGPUInstance->MIGComputeInstance[pParams->id].bValid)
        return NV_ERR_OBJECT_NOT_FOUND;

    pMIGComputeInstance = &pGPUInstance->MIGComputeInstance[pParams->id];

    portMemCopy(pParams->info.uuid, sizeof(pParams->info.uuid),
                pMIGComputeInstance->uuid.uuid, sizeof(pMIGComputeInstance->uuid.uuid));
    pParams->info.sharedEngFlags = pMIGComputeInstance->sharedEngFlag;
    pParams->info.veidOffset     = pMIGComputeInstance->resourceAllocation.veidOffset;
    pParams->info.veidCount      = pMIGComputeInstance->resourceAllocation.veidCount;
    pParams->info.smCount        = pMIGComputeInstance->resourceAllocation.smCount;
    pParams->info.spanStart      = pMIGComputeInstance->spanStart;
    pParams->info.computeSize    = pMIGComputeInstance->computeSize;

    for (gpcIdx = 0; gpcIdx < pMIGComputeInstance->resourceAllocation.gpcCount; ++gpcIdx)
    {
         pParams->info.gpcMask |= NVBIT32(pMIGComputeInstance->resourceAllocation.gpcIds[gpcIdx]);
    }
    bitVectorToRaw(&pMIGComputeInstance->resourceAllocation.engines,
                   pParams->info.enginesMask, sizeof(pParams->info.enginesMask));

    return NV_OK;
}

NV_STATUS
gisubscriptionCtrlCmdExecPartitionsImport_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KERNEL_MIG_GPU_INSTANCE *pGPUInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    NV_STATUS status = NV_OK;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
        return NV_ERR_NOT_SUPPORTED;

{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    // An unprivileged client has no use case for import/export
    if (!rmclientIsCapableOrAdminByHandle(RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription),
                                          NV_RM_CAP_SYS_SMC_CONFIG,
                                          pCallContext->secInfo.privLevel))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }
}

    if (IS_VIRTUAL(pGpu))
    {
        // Guest RM does not support import/export
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        if (status != NV_OK)
            return status;
    }

    {
        GPUMGR_SAVE_COMPUTE_INSTANCE save;
        KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS restore =
        {
            .type = KMIGMGR_CREATE_COMPUTE_INSTANCE_PARAMS_TYPE_RESTORE,
            .inst.restore.pComputeInstanceSave = &save,
        };

        portMemSet(&save, 0, sizeof(save));
        save.bValid = NV_TRUE;
        save.id = pParams->id;
        save.ciInfo = pParams->info;

        if (IS_GSP_CLIENT(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kmigmgrCreateComputeInstances_HAL(pGpu, pKernelMIGManager,
                 pGPUInstance, NV_FALSE, restore, &pParams->id, pParams->bCreateCap),
                cleanup_rpc);
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;

cleanup_rpc:
    if (IS_GSP_CLIENT(pGpu))
    {
        NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS params;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        portMemSet(&params, 0, sizeof(params));
        params.execPartCount = 1;
        params.execPartId[0] = pParams->id;

        NV_ASSERT_OK(
            pRmApi->Control(pRmApi,
                            RES_GET_CLIENT_HANDLE(pGPUInstanceSubscription),
                            RES_GET_HANDLE(pGPUInstanceSubscription),
                            NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE,
                            &params,
                            sizeof(params)));
    }

    return status;
}

/*!
 * @brief Determines whether an object of the given class id is affected by
 * gpu/compute instance subscription and should be automatically freed if a
 * client unsubscribes from a gpu/compute instance.
 */
NvBool
gisubscriptionShouldClassBeFreedOnUnsubscribe_IMPL
(
    NvU32 internalClassId
)
{
    NvBool bShouldFree = NV_TRUE;

    switch (internalClassId)
    {
        case (classId(Device)):
            // fall-through
        case (classId(Subdevice)):
            // fall-through
        case (classId(GPUInstanceSubscription)):
            // fall-through
        case (classId(ComputeInstanceSubscription)):
            bShouldFree = NV_FALSE;
            break;
        default:
            break;
    }

    return bShouldFree;
}

/*!
 * @brief Automatically frees client resources which may be affected by
 * subscription objects. This is intended to be called on unsubscription.
 *
 * @see gisubscriptionShouldClassBeFreedOnUnsubscribe
 *
 * @param[in] pCallContext         Call context of client to clean up
 */
void
gisubscriptionCleanupOnUnsubscribe_IMPL
(
    CALL_CONTEXT *pCallContext
)
{
    RsResourceRef *pDeviceRef;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RS_ITERATOR iter;
    NvHandle *pHandles;
    NvU32 handleCount;
    NvU32 i;

    NV_ASSERT_OK(
        refFindAncestorOfType(pCallContext->pResourceRef, classId(Device), &pDeviceRef));

    // Determine the number of handles we need to free
    handleCount = 0;
    iter = serverutilRefIter(pCallContext->pClient->hClient,
                             pDeviceRef->hResource,
                             0,
                             RS_ITERATE_DESCENDANTS,
                             NV_FALSE);
    while (clientRefIterNext(iter.pClient, &iter))
    {
        RsResourceRef *pResourceRef = iter.pResourceRef;

        if (!gisubscriptionShouldClassBeFreedOnUnsubscribe(pResourceRef->internalClassId))
            continue;

        ++handleCount;
        NV_PRINTF(LEVEL_INFO,
                  "Will be freeing resource class id 0x%x on unsubscription!\n",
                  pResourceRef->internalClassId);
    }

    // If we have nothing to free then bail early
    if (handleCount == 0)
        goto done;

    // Allocate an array large enough to store the handles we need to free
    pHandles = portMemAllocNonPaged(handleCount * sizeof(*pHandles));
    if (NULL == pHandles)
    {
        NV_ASSERT(0);
        goto done;
    }

    // Store the handles that we need to free
    i = 0;
    iter = serverutilRefIter(pCallContext->pClient->hClient,
                             pDeviceRef->hResource,
                             0,
                             RS_ITERATE_DESCENDANTS,
                             NV_FALSE);
    while (clientRefIterNext(iter.pClient, &iter))
    {
        RsResourceRef *pResourceRef = iter.pResourceRef;

        if (!gisubscriptionShouldClassBeFreedOnUnsubscribe(pResourceRef->internalClassId))
            continue;

        NV_ASSERT_OR_GOTO(i < handleCount, cleanup);
        pHandles[i++] = pResourceRef->hResource;
    }

    //
    // Free all of the handles we flagged for deletion.
    // Note - some of these resources will free other dependant resources, so
    // some of these free calls will do nothing. That's fine for our purposes.
    //
    NV_ASSERT_OR_GOTO(i == handleCount, cleanup);
    for (i = 0; i < handleCount; ++i)
        pRmApi->Free(pRmApi, pCallContext->pClient->hClient, pHandles[i]);

cleanup:
    portMemFree(pHandles);

done:
    return;
}

NV_STATUS
gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL
(
    GPUInstanceSubscription *pGPUInstanceSubscription,
    NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGPUInstanceSubscription);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = pGPUInstanceSubscription->pKernelMIGGpuInstance;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bCtsRequired = kmigmgrIsCTSAlignmentRequired(pGpu, pKernelMIGManager);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        pParams->computeSize < NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE,
        NV_ERR_INVALID_ARGUMENT);

    if (bCtsRequired)
    {
        NV_RANGE totalRange = kmigmgrComputeProfileSizeToCTSIdRange(pParams->computeSize);
        NvU32 slotBasisComputeSize = kmigmgrSmallestComputeProfileSize(pGpu, pKernelMIGManager);
        NvU64 slotBasisMask;
        NvU64 validQueryMask;
        NvU64 inUseIdMask;
        NvU64 ctsId;
        NvU32 totalSpanCount;
        NvU32 availableSpanCount;
        NV_RANGE slotBasisIdRange;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, slotBasisComputeSize != KMIGMGR_COMPUTE_SIZE_INVALID, NV_ERR_INVALID_STATE);

        slotBasisIdRange = kmigmgrComputeProfileSizeToCTSIdRange(slotBasisComputeSize);

        NV_CHECK_OR_RETURN(LEVEL_ERROR, !rangeIsEmpty(totalRange), NV_ERR_INVALID_ARGUMENT);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, !rangeIsEmpty(slotBasisIdRange), NV_ERR_INVALID_ARGUMENT);

        slotBasisMask = DRF_SHIFTMASK64(slotBasisIdRange.hi:slotBasisIdRange.lo);
        validQueryMask = DRF_SHIFTMASK64(totalRange.hi:totalRange.lo) & pKernelMIGGpuInstance->pProfile->validCTSIdMask;

        // Find mask of un-usable IDs due to current in-use CTS Ids
        inUseIdMask = 0x0;
        FOR_EACH_INDEX_IN_MASK(64, ctsId, pKernelMIGGpuInstance->ctsIdsInUseMask)
        {
            NvU64 invalidMask;

            NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, ctsId, &invalidMask));

            inUseIdMask |= invalidMask;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        //
        // The slot basis defines the smallest divison of the GPU instance.
        // CTS IDs from this range are used as a means to specify span placements
        // for compute profiles.
        //
        totalSpanCount = 0;
        availableSpanCount = 0;

        FOR_EACH_INDEX_IN_MASK(64, ctsId, validQueryMask)
        {
            NvU64 invalidMask;

            NV_ASSERT_OK(kmigmgrGetInvalidCTSIdMask(pGpu, pKernelMIGManager, ctsId, &invalidMask));

            invalidMask &= slotBasisMask;
            pParams->totalSpans[totalSpanCount].lo = portUtilCountTrailingZeros64(invalidMask) - slotBasisIdRange.lo;
            pParams->totalSpans[totalSpanCount].hi = nvPopCount64(invalidMask) + pParams->totalSpans[totalSpanCount].lo - 1;

            if (!(NVBIT64(ctsId) & inUseIdMask))
            {
                pParams->availableSpans[availableSpanCount].lo = pParams->totalSpans[totalSpanCount].lo;
                pParams->availableSpans[availableSpanCount].hi = pParams->totalSpans[totalSpanCount].hi;
                availableSpanCount++;
            }
            totalSpanCount++;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        pParams->totalSpansCount     = totalSpanCount;
        pParams->totalProfileCount   = totalSpanCount;
        pParams->availableSpansCount = availableSpanCount;
        pParams->profileCount        = availableSpanCount;
    }
    else
    {
        KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE profile;
        NvU64 veidMask;
        NvU32 GPUInstanceVeidEnd;
        NvU64 GPUInstanceVeidMask;
        NvU64 GPUInstanceFreeVeidMask;
        NvU64 GPUInstancePseudoMask;
        NvU32 availableSpanCount;
        NvU32 totalSpanCount;
        NvU32 veidStepSize;
        NvU32 veidSlotCount;
        NvU32 count;
        NvU32 i;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetComputeProfileFromSize(pGpu, pKernelMIGManager, pParams->computeSize, &profile));
        NV_ASSERT_OK_OR_RETURN(
            kgrmgrGetMaxVeidsPerGpc(pGpu, pKernelGraphicsManager, &veidStepSize));

        // Create a mask for VEIDs associated with this GPU instance
        veidMask = DRF_SHIFTMASK64(profile.veidCount - 1:0);
        GPUInstanceVeidEnd = pKernelMIGGpuInstance->resourceAllocation.veidOffset + pKernelMIGGpuInstance->resourceAllocation.veidCount - 1;
        GPUInstanceVeidMask = DRF_SHIFTMASK64(GPUInstanceVeidEnd:pKernelMIGGpuInstance->resourceAllocation.veidOffset);
        GPUInstanceFreeVeidMask = GPUInstanceVeidMask & ~pKernelGraphicsManager->veidInUseMask;
        GPUInstancePseudoMask = GPUInstanceFreeVeidMask;
        veidSlotCount = 0;
        availableSpanCount = 0;
        totalSpanCount = 0;
        count = 0;
        for (i = pKernelMIGGpuInstance->resourceAllocation.veidOffset; i < GPUInstanceVeidEnd; i += veidStepSize)
        {
            // Determine max correctly sized VEID segments
            if (((GPUInstanceFreeVeidMask >> i) & veidMask) == veidMask)
            {
                pParams->availableSpans[availableSpanCount].lo = count;
                pParams->availableSpans[availableSpanCount].hi = count + (profile.veidCount / veidStepSize) - 1;
                availableSpanCount++;
            }

            // Determine max correctly sized VEID segments
            if (((GPUInstanceVeidMask >> i) & veidMask) == veidMask)
            {
                pParams->totalSpans[totalSpanCount].lo = count;
                pParams->totalSpans[totalSpanCount].hi = count + (profile.veidCount / veidStepSize) - 1;
                totalSpanCount++;
            }

            // Determine max correctly sized VEID segments
            if (((GPUInstancePseudoMask >> i) & veidMask) == veidMask)
            {
                veidSlotCount++;
                GPUInstancePseudoMask &= ~(veidMask << i);
            }
            count++;
        }
        pParams->totalProfileCount = NV_MIN(pKernelMIGGpuInstance->pProfile->virtualGpcCount / profile.gpcCount,
                                                pKernelMIGGpuInstance->pProfile->veidCount / profile.veidCount);
        pParams->totalSpansCount     = totalSpanCount;
        pParams->profileCount        = veidSlotCount;
        pParams->availableSpansCount = availableSpanCount;
    }

    return NV_OK;
}
