/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing MIG compute instance subscriptions
 *
 *****************************************************************************/

#define NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "gpu/gpu.h"
#include "core/system.h"
#include "os/os.h"

#include "kernel/gpu/mig_mgr/gpu_instance_subscription.h"
#include "kernel/gpu/mig_mgr/compute_instance_subscription.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "class/clc638.h"
#include "rmapi/rs_utils.h"
#include "gpu/gpu_uuid.h"
#include "vgpu/rpc.h"

NV_STATUS
cisubscriptionConstruct_IMPL
(
    ComputeInstanceSubscription  *pComputeInstanceSubscription,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    NVC638_ALLOCATION_PARAMETERS *pUserParams = pRmAllocParams->pAllocParams;
    RsClient *pRsClient = pCallContext->pClient;
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    RsResourceRef *pParentRef = pResourceRef->pParentRef;
    GPUInstanceSubscription *pGPUInstanceSubscription = dynamicCast(pParentRef->pResource, GPUInstanceSubscription);
    OBJGPU *pGpu;
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    NV_STATUS status;

    pGpu = GPU_RES_GET_GPU(pComputeInstanceSubscription);

    osRmCapInitDescriptor(&pComputeInstanceSubscription->dupedCapDescriptor);

    if (RS_IS_COPY_CTOR(pRmAllocParams))
        return cisubscriptionCopyConstruct_IMPL(pComputeInstanceSubscription, pCallContext, pRmAllocParams);

    if (!IS_MIG_IN_USE(pGpu))
    {
        NV_ASSERT_FAILED("Compute instance Subscription failed: MIG GPU partitioning not done");
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pUserParams->execPartitionId < KMIGMGR_MAX_COMPUTE_INSTANCES, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription) != NULL, NV_ERR_INVALID_STATE);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription)->bValid, NV_ERR_INVALID_STATE);

    pMIGComputeInstance = &gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription)->MIGComputeInstance[pUserParams->execPartitionId];
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pMIGComputeInstance->bValid, NV_ERR_INVALID_ARGUMENT);

    //
    // For now skip kernel clients, such as UVM, until Bug 2729768 is fixed.
    //
    if (pRsClient->type == CLIENT_TYPE_USER)
    {
        status = osRmCapAcquire(pMIGComputeInstance->pOsRmCaps,
                                NV_RM_CAP_SMC_EXEC_PARTITION_ACCESS,
                                pUserParams->capDescriptor,
                                &pComputeInstanceSubscription->dupedCapDescriptor);
        if ((status != NV_ERR_NOT_SUPPORTED) && (status != NV_OK))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Capability validation failed: ID 0x%0x!\n",
                      pUserParams->execPartitionId);
            return status;
        }
    }

    pComputeInstanceSubscription->pMIGComputeInstance = pMIGComputeInstance;

    NV_ASSERT_OK_OR_GOTO(status,
        kmigmgrIncRefCount(pMIGComputeInstance->pShare),
        cleanup_duped_desc);

    return NV_OK;

cleanup_duped_desc:
    osRmCapRelease(pComputeInstanceSubscription->dupedCapDescriptor);

    return status;
}

NV_STATUS
cisubscriptionCopyConstruct_IMPL
(
    ComputeInstanceSubscription *pComputeInstanceSubscription,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    RsResourceRef *pSrcRef = pParams->pSrcRef;
    ComputeInstanceSubscription *pComputeInstanceSubscriptionSrc = dynamicCast(pSrcRef->pResource, ComputeInstanceSubscription);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pComputeInstanceSubscription);

    // non kernel clients are not allowed to dup MIG instances
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL,
                       NV_ERR_NOT_SUPPORTED);

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

    pComputeInstanceSubscription->pMIGComputeInstance = pComputeInstanceSubscriptionSrc->pMIGComputeInstance;

    NV_ASSERT_OK(
        kmigmgrIncRefCount(pComputeInstanceSubscription->pMIGComputeInstance->pShare));

    return NV_OK;
}

void
cisubscriptionDestruct_IMPL
(
    ComputeInstanceSubscription *pComputeInstanceSubscription
)
{
    CALL_CONTEXT *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;

    resGetFreeParams(staticCast(pComputeInstanceSubscription, RsResource), &pCallContext, &pParams);

    osRmCapRelease(pComputeInstanceSubscription->dupedCapDescriptor);

    gisubscriptionCleanupOnUnsubscribe(pCallContext);

    NV_ASSERT_OK(
        kmigmgrDecRefCount(pComputeInstanceSubscription->pMIGComputeInstance->pShare));
}

NV_STATUS
cisubscriptionGetComputeInstanceSubscription_IMPL
(
    RsClient *pClient,
    NvHandle hParent,
    ComputeInstanceSubscription **ppComputeInstanceSubscription
)
{
    RsResourceRef *pResourceRef;

    NV_ASSERT_OR_RETURN(ppComputeInstanceSubscription != NULL, NV_ERR_INVALID_ARGUMENT);

    pResourceRef = serverutilFindChildRefByType(pClient->hClient,
                                                hParent, classId(ComputeInstanceSubscription),
                                                NV_TRUE);
    if (pResourceRef == NULL)
        return NV_ERR_OBJECT_NOT_FOUND;

    *ppComputeInstanceSubscription = dynamicCast(pResourceRef->pResource, ComputeInstanceSubscription);

    return NV_OK;
}

NvBool
cisubscriptionCanCopy_IMPL
(
    ComputeInstanceSubscription *pComputeInstanceSubscription
)
{
    return NV_TRUE;
}

NV_STATUS
cisubscriptionCtrlCmdGetUuid_IMPL
(
    ComputeInstanceSubscription *pComputeInstanceSubscription,
    NVC638_CTRL_GET_UUID_PARAMS *pParams
)
{
    ct_assert(NV_UUID_LEN == NVC638_UUID_LEN);
    ct_assert(NV_UUID_STR_LEN == NVC638_UUID_STR_LEN);

    portMemCopy(pParams->uuid, NVC638_UUID_LEN,
                pComputeInstanceSubscription->pMIGComputeInstance->uuid.uuid, NV_UUID_LEN);

    nvGetSmcUuidString(&pComputeInstanceSubscription->pMIGComputeInstance->uuid,
                       pParams->uuidStr);

    return NV_OK;
}
