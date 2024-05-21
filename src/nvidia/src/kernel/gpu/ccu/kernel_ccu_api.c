/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************************************************************\
 *                                                                          *
 *      Kernel Ccu Api Object Module                                        *
 *                                                                          *
 \**************************************************************************/

#include "gpu/gpu.h"
#include "gpu/ccu/kernel_ccu_api.h"
#include "gpu/ccu/kernel_ccu.h"
#include "rmapi/client.h"
#include "gpu/mig_mgr/kernel_mig_manager.h"

NV_STATUS
kccuapiConstruct_IMPL
(
    KernelCcuApi                 *pKernelCcuApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: Construct\n");

    return NV_OK;
}

void
kccuapiDestruct_IMPL
(
    KernelCcuApi *pKernelCcuApi
)
{
    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: Destruct\n");
    return;
}

static NV_STATUS
_kccuapiMemdescGet
(
    KernelCcuApi      *pKernelCcuApi,
    RmClient          *pClient,
    Device            *pDevice,
    MEMORY_DESCRIPTOR **pMemDesc
)
{
    MIG_INSTANCE_REF ref;
    NV_STATUS status      = NV_OK;
    OBJGPU    *pGpu       = GPU_RES_GET_GPU(pKernelCcuApi);
    KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: Get memdesc info\n");

    if (pKernelCcu == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    if (!IS_MIG_ENABLED(pGpu))
    {
        return kccuMemDescGetForShrBufId(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, pMemDesc);
    }

    // In case of MIG
    status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref);
    if (status != NV_OK)
    {
        // Check if client is allowed to query for device counters
        if (rmclientIsCapable(pClient, NV_RM_CAP_SYS_SMC_MONITOR))
        {
            return kccuMemDescGetForShrBufId(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, pMemDesc);
        }

        // Permission denied
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if ((ref.pKernelMIGGpuInstance == NULL) || (ref.pMIGComputeInstance == NULL))
    {
        return NV_ERR_INVALID_POINTER;
    }

    // Fetch the shared buffer memdesc for the computeId
    return kccuMemDescGetForComputeInst(pGpu, pKernelCcu, ref.pKernelMIGGpuInstance->swizzId,
                                        ref.pMIGComputeInstance->id, pMemDesc);
}

NV_STATUS
kccuapiMap_IMPL
(
    KernelCcuApi      *pKernelCcuApi,
    CALL_CONTEXT      *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping      *pCpuMapping
)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV_STATUS status  = NV_OK;
    NvBool bKernel    = NV_FALSE;
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);
    Device *pDevice   = GPU_RES_GET_DEVICE(pKernelCcuApi);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: memdesc map\n");

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Kernel mapping validation failed with status: 0x%x\n", status);
        return status;
    }

    pCpuMapping->processId = osGetCurrentProcess();

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kccuapiMemdescGet(pKernelCcuApi, pClient, pDevice, &pMemDesc));

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Shared buffer memdesc is NULL\n");
        return NV_ERR_INVALID_OBJECT;
    }

    // Map entire buffer (no offsets supported)
    status = memdescMap(pMemDesc,
                        0,
                        pMemDesc->Size,
                        bKernel,
                        pCpuMapping->pPrivate->protect,
                        &pCpuMapping->pLinearAddress,
                        &pCpuMapping->pPrivate->pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "kernelCcuApi shared buffer memdescMap failed with status: 0x%x\n", status);
        return status;
    }

    return NV_OK;
}

NV_STATUS
kccuapiUnmap_IMPL
(
    KernelCcuApi *pKernelCcuApi,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS status = NV_OK;
    NvBool bKernel   = NV_FALSE;
    Device *pDevice  = GPU_RES_GET_DEVICE(pKernelCcuApi);
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: memdesc unmap\n");

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Kernel mapping validation failed with status: 0x%x\n", status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kccuapiMemdescGet(pKernelCcuApi, pClient, pDevice, &pMemDesc));

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Shared buffer memdesc is NULL\n");
        return NV_ERR_INVALID_OBJECT;
    }

    memdescUnmap(pMemDesc,
                 bKernel,
                 pCpuMapping->processId,
                 pCpuMapping->pLinearAddress,
                 pCpuMapping->pPrivate->pPriv);

    return status;
}

NV_STATUS
kccuapiGetMapAddrSpace_IMPL
(
    KernelCcuApi     *pKernelCcuApi,
    CALL_CONTEXT     *pCallContext,
    NvU32            mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    NV_ADDRESS_SPACE addrSpace  = 0;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    Device *pDevice   = GPU_RES_GET_DEVICE(pKernelCcuApi);
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU    *pGpu   = GPU_RES_GET_GPU(pKernelCcuApi);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: get memdesc address-space\n");

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kccuapiMemdescGet(pKernelCcuApi, pClient, pDevice, &pMemDesc));

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Shared buffer memdesc is NULL\n");
        return NV_ERR_INVALID_OBJECT;
    }

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pMemDesc, mapFlags, &addrSpace));

    *pAddrSpace = addrSpace;

    return NV_OK;
}

NV_STATUS
kccuapiGetMemoryMappingDescriptor_IMPL
(
    KernelCcuApi      *pKernelCcuApi,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    Device *pDevice   = GPU_RES_GET_DEVICE(pKernelCcuApi);
    RmClient *pClient = dynamicCast(RES_GET_CLIENT(pKernelCcuApi), RmClient);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: get memdesc\n");

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _kccuapiMemdescGet(pKernelCcuApi, pClient, pDevice, &pMemDesc));

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Shared buffer memdesc is NULL\n");
        return NV_ERR_INVALID_OBJECT;
    }

    *ppMemDesc = pMemDesc;

    return NV_OK;
}

NV_STATUS
kccuapiCtrlCmdSubscribe_IMPL
(
    KernelCcuApi *pKernelCcuApi,
    NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams
)
{
    MIG_INSTANCE_REF ref;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV_STATUS status      = NV_OK;
    Device *pDevice       = GPU_RES_GET_DEVICE(pKernelCcuApi);
    OBJGPU    *pGpu       = GPU_RES_GET_GPU(pKernelCcuApi);
    KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);
    RmClient *pClient     = dynamicCast(RES_GET_CLIENT(pKernelCcuApi), RmClient);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: CCU subscribe request\n");

    if (pKernelCcu == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    if (!IS_MIG_ENABLED(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kccuMemDescGetForShrBufId(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, &pMemDesc));

        if (pMemDesc == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }

        pParams->counterBlockSize = kccuCounterBlockSizeGet(pGpu, pKernelCcu, NV_TRUE);
        pParams->bufferSize       = memdescGetSize(pMemDesc);

        return status;
    }

    // In case of MIG
    status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref);
    if (status != NV_OK)
    {
        // Check if client is allowed to query for device counters or not
        if (!rmclientIsCapable(pClient, NV_RM_CAP_SYS_SMC_MONITOR))
        {
            // Return if the client is not allowed
            return status;
        }

        // Get device counter block size
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kccuMemDescGetForShrBufId(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, &pMemDesc));
        pParams->counterBlockSize = kccuCounterBlockSizeGet(pGpu, pKernelCcu, NV_TRUE);
    }
    else
    {
        if ((ref.pKernelMIGGpuInstance == NULL) || (ref.pMIGComputeInstance == NULL))
        {
            return NV_ERR_INVALID_POINTER;
        }

        // Get counter block size for mig gpu inst
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kccuMemDescGetForComputeInst(pGpu, pKernelCcu, ref.pKernelMIGGpuInstance->swizzId,
                                            ref.pMIGComputeInstance->id, &pMemDesc));
        pParams->counterBlockSize = kccuCounterBlockSizeGet(pGpu, pKernelCcu, NV_FALSE);
    }

    if (pMemDesc == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    pParams->bufferSize = memdescGetSize(pMemDesc);

    return NV_OK;
}

NV_STATUS
kccuapiCtrlCmdUnsubscribe_IMPL
(
    KernelCcuApi *pKernelCcuApi
)
{
    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: CCU unsubscribe request\n");

    return NV_OK;
}

NV_STATUS
kccuapiCtrlCmdSetStreamState_IMPL
(
    KernelCcuApi *pKernelCcuApi,
    NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams
)
{
    OBJGPU    *pGpu       = GPU_RES_GET_GPU(pKernelCcuApi);
    KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: CCU set stream state request\n");

    if (pKernelCcu == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    // Set counter collection unit stream state
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kccuStreamStateSet(pGpu, pKernelCcu, pParams));

    return NV_OK;
}

NV_STATUS
kccuapiCtrlCmdGetStreamState_IMPL
(
    KernelCcuApi *pKernelCcuApi,
    NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams
)
{
    OBJGPU    *pGpu       = GPU_RES_GET_GPU(pKernelCcuApi);
    KernelCcu *pKernelCcu = GPU_GET_KERNEL_CCU(pGpu);

    NV_PRINTF(LEVEL_INFO, "Kernel Ccu Api: CCU get stream state request\n");

    if (pKernelCcu == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    // Get counter collection unit stream state
    pParams->bStreamState = kccuStreamStateGet(pGpu, pKernelCcu);

    return NV_OK;
}
