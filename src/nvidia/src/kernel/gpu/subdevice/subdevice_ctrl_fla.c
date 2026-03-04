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

#include "core/core.h"
#include "core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/bus/kern_bus.h"

#include "vgpu/rpc.h"
#include "rmapi/client.h"

static NV_STATUS
_subdeviceFlaRangeModeHostManagedVasDestroy
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_RANGE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvU32   gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // See _subdeviceFlaRangeModeHostManagedVasInit for details about this check.
    if (!RMCFG_FEATURE_PLATFORM_GSP && IS_GFID_PF(gfid) &&
        (pKernelNvlink != NULL) && !knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
        return NV_ERR_NOT_SUPPORTED;

    kbusDestroyHostManagedFlaVaspace_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), gfid);

    return NV_OK;
}

static NV_STATUS
_subdeviceFlaRangeModeHostManagedVasInit
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_RANGE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 hSubdevice = RES_GET_HANDLE(pSubdevice);
    NvHandle hDevice = RES_GET_PARENT_HANDLE(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvU32 gfid;
    NV_STATUS status;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    if (!RMCFG_FEATURE_PLATFORM_GSP && IS_GFID_PF(gfid) &&
        (pKernelNvlink != NULL) && !knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
        return NV_ERR_NOT_SUPPORTED;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pParams->size != 0, NV_ERR_INVALID_ARGUMENT);

    if (pKernelBus->flaInfo.bFlaAllocated)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FLA VAS is not allowed for base: %llx, size:%llx in gpu: %x\n",
                  pParams->base, pParams->size, pGpu->gpuInstance);

        return NV_ERR_INVALID_STATE;
    }

    status = kbusAllocateHostManagedFlaVaspace_HAL(pGpu, pKernelBus, hClient, hDevice,
                                                  hSubdevice, pParams->hVASpace,
                                                  pParams->base, pParams->size,
                                                  gfid);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Host Managed FLA Vaspace failed, status: %x, gpu: %x \n",
                  status, pGpu->gpuInstance);
        return status;
    }

    return NV_OK;
}

static NV_STATUS
_subdeviceFlaRangeModeInit
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_RANGE_PARAMS *pParams
)
{
    OBJGPU    *pGpu       = GPU_RES_GET_GPU(pSubdevice);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NV_STATUS  status;

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // Must not be called from vGPU guests, except for arch MODS.
    if (IS_VIRTUAL(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pParams->size != 0, NV_ERR_INVALID_ARGUMENT);

    //
    // check if FM has previously allocated for the same FLA range.
    //
    // Note that in case of vGPU, FLA VAS is allocated in guests, thus
    // pKernelBus->flaInfo.bFlaAllocated shouldn't be set in the host. We allow
    // FM running in the host to override the FLA range as we expect device
    // to be idle when the vGPU partitions are being configured.
    //
    if (pKernelBus->flaInfo.bFlaAllocated)
    {
        // check if FM has previously allocated for the same FLA range
        if (!kbusVerifyFlaRange_HAL(pGpu, pKernelBus, pParams->base, pParams->size))
            return NV_ERR_IN_USE;

        return NV_OK;
    }

    //
    // We don't have to care about Arch Mods/pre-silicon verification scenarios
    // for programming FLA base address to scratch register
    //
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelNvlink != NULL, NV_ERR_NOT_SUPPORTED);

    status  = knvlinkSetUniqueFlaBaseAddress(pGpu, pKernelNvlink, pParams->base);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to set FLA range because of invalid config for gpu: %x\n",
                  pGpu->gpuInstance);
        return status;
    }

    status = kbusAllocateFlaVaspace_HAL(pGpu, pKernelBus, pParams->base, pParams->size);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Allocating new FLA Vaspace failed, status: %x, gpu: %x \n",
                  status, pGpu->gpuInstance);
        return status;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdFlaRange_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_RANGE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    KernelMIGManager *pKernelMIGManager;
    RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pSubdevice), RmClient);
    CALL_CONTEXT *pCallContext;

    NV_ASSERT_OR_RETURN(NULL != pRmClient, NV_ERR_INVALID_CLIENT);

    pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (!rmclientIsCapableOrAdmin(pRmClient,
                                  NV_RM_CAP_EXT_FABRIC_MGMT,
                                  pCallContext->secInfo.privLevel))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!kbusIsFlaSupported(pKernelBus))
    {
        NV_PRINTF(LEVEL_INFO,
                  "FLA is not supported in this platform\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    if (!ONEBITSET(pParams->mode))
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Even if FLA is enabled, it is not compatible with MIG memory partitioning
    // If MIG memory partitioning is enabled, we should not allow FLA address
    // range creation
    //
    pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    if ((pKernelMIGManager != NULL) && !kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager))
    {
        NV_PRINTF(LEVEL_INFO,
                  "ERROR: FLA cannot be enabled with peer support disabled with MIG\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    switch (pParams->mode)
    {
        case NV2080_CTRL_FLA_RANGE_PARAMS_MODE_INITIALIZE:
            status = _subdeviceFlaRangeModeInit(pSubdevice, pParams);
            break;

        // Deprecated as no client should be able to invoke this control option.
        case NV2080_CTRL_FLA_RANGE_PARAMS_MODE_DESTROY:
            status = NV_OK;
            break;

        case NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_INITIALIZE:
            status = _subdeviceFlaRangeModeHostManagedVasInit(pSubdevice, pParams);
            break;

        case NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_DESTROY:
            status = _subdeviceFlaRangeModeHostManagedVasDestroy(pSubdevice, pParams);
            break;

        default:
            status = NV_ERR_INVALID_OPERATION;
            break;
    }

    return status;
}

// Control call to manage FLA range in RM
NV_STATUS
subdeviceCtrlCmdFlaGetRange_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_GET_RANGE_PARAMS *pParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    KernelBus    *pKernelBus    = GPU_GET_KERNEL_BUS(pGpu);
    NvBool        bIsConntectedToNvswitch;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelNvlink != NULL, NV_ERR_NOT_SUPPORTED);
        bIsConntectedToNvswitch = knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink);
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, kbusIsFlaSupported(pKernelBus), NV_ERR_NOT_SUPPORTED);

    return kbusGetFlaRange_HAL(pGpu, pKernelBus, &pParams->base, &pParams->size, bIsConntectedToNvswitch);
}

NV_STATUS
subdeviceCtrlCmdFlaGetFabricMemStats_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams
)
{
    OBJGPU         *pGpu       = GPU_RES_GET_GPU(pSubdevice);
    FABRIC_VASPACE *pFabricVAS = NULL;
    NV_STATUS       status = NV_OK;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (pGpu->pFabricVAS == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    if (pFabricVAS->bRpcAlloc)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    pParams->totalSize = fabricvaspaceGetUCFlaLimit(pFabricVAS) -
                         fabricvaspaceGetUCFlaStart(pFabricVAS) + 1;

    return fabricvaspaceGetFreeHeap(pFabricVAS, &pParams->freeSize);
}

