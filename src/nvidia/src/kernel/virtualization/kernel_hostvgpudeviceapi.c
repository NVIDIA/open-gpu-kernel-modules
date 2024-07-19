/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl/ctrl2080.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "virtualization/vgpuconfigapi.h"

#include "core/core.h"
#include "core/locks.h"
#include "os/os.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "dev_ctrl_defines.h"
#include "mem_mgr/mem.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "gpu/device/device.h"
#include "gpu/intr/intr.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "nv_sriov_defines.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "kernel/gpu/gsp/kernel_gsp.h"
#include "nvpcie.h"
#include "kernel/rmapi/rs_utils.h"
#include "vgpu/vgpu_events.h"
#include "gpu/mem_sys/kern_mem_sys.h"

ct_assert(NVA084_MAX_VMMU_SEGMENTS == NV2080_CTRL_MAX_VMMU_SEGMENTS);
ct_assert(NV2080_GPU_MAX_ENGINES == RM_ENGINE_TYPE_LAST);

NV_STATUS
kernelhostvgpudeviceshrConstruct_IMPL
(
    KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr
)
{
    return NV_OK;
}

void
kernelhostvgpudeviceshrDestruct_IMPL
(
    KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr
)
{
}

NV_STATUS
kernelhostvgpudeviceapiConstruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NVA084_ALLOC_PARAMETERS *pAllocParams = pParams->pAllocParams;
    NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pBootloadParams = NULL;
    Memory *pMemory;
    NvU32 i;
    NvU64 vmmuSegmentSize;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;
    RsShared *pShared;
    Device *pDevice;
    RsClient *pClient = NULL;

    // Forbid allocation of this class on Guest-RM
    // to avoid fuzzing this class in such cases. See bug 3529160.
    if (IS_VIRTUAL(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OK_OR_RETURN(deviceGetByGpu(pCallContext->pClient, pGpu, NV_TRUE, &pDevice));

    if (RS_IS_COPY_CTOR(pParams))
    {
        return kernelhostvgpudeviceapiCopyConstruct_IMPL(pKernelHostVgpuDeviceApi, pCallContext, pParams);
    }

    pClient = pCallContext->pClient;

    status = kvgpumgrGuestRegister(pGpu,
                                   pAllocParams->gfid,
                                   pAllocParams->vgpuType,
                                   pAllocParams->vmPid,
                                   pAllocParams->vmIdType,
                                   pAllocParams->guestVmId,
                                   pClient->hClient,
                                   pAllocParams->numChannels,
                                   pAllocParams->numPluginChannels,
                                   pAllocParams->swizzId,
                                   pAllocParams->vgpuDeviceInstanceId,
                                   pAllocParams->bDisableDefaultSmcExecPartRestore,
                                   pAllocParams->placementId,
                                   pAllocParams->vgpuDevName,
                                   &pKernelHostVgpuDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to register HOST_VGPU_DEVICE object\n");
        return status;
    }

    status = serverAllocShare(&g_resServ, classInfo(KernelHostVgpuDeviceShr), &pShared);
    if (status != NV_OK)
        goto allocShareError;
    pKernelHostVgpuDeviceApi->pShared = dynamicCast(pShared, KernelHostVgpuDeviceShr);
    pKernelHostVgpuDeviceApi->pShared->pDevice = pKernelHostVgpuDevice;
    pKernelHostVgpuDevice->pGspPluginHeapMemDesc = NULL;

    if (IS_GSP_CLIENT(pGpu))
    {
        KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

        if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || !gpuIsSriovEnabled(pGpu))
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto done;
        }

        if (pAllocParams->numGuestFbHandles == 0 || pAllocParams->numGuestFbHandles > NVA084_MAX_VMMU_SEGMENTS)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        vmmuSegmentSize = gpuGetVmmuSegmentSize(pGpu);
        if (vmmuSegmentSize == 0)
        {
            status = NV_ERR_INVALID_STATE;
            goto done;
        }

        // This structure can't be allocated on stack because it will result function stack usage > 4KB
        pBootloadParams = portMemAllocNonPaged(sizeof(*pBootloadParams));
        if (pBootloadParams == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
        portMemSet(pBootloadParams, 0, sizeof(*pBootloadParams));

        if (gpuIsSelfHosted(pGpu))
        {
            pKernelHostVgpuDevice->hbmRegionList = portMemAllocNonPaged(sizeof(HBM_REGION_INFO)* pAllocParams->numGuestFbHandles);
            if (pKernelHostVgpuDevice->hbmRegionList == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto done;
            }

            pKernelHostVgpuDevice->numValidHbmRegions = 0;
        }

        for (i = 0; i < pAllocParams->numGuestFbHandles; i++)
        {
            status = memGetByHandle(pClient, pAllocParams->guestFbHandleList[i], &pMemory);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid Memory handle\n");
                goto done;
            }
            pBootloadParams->guestFbPhysAddrList[i] = memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU, 0);
            pBootloadParams->guestFbLengthList[i] = pMemory->pMemDesc->Size;

            if (!NV_IS_ALIGNED64(pBootloadParams->guestFbPhysAddrList[i], vmmuSegmentSize) ||
                !NV_IS_ALIGNED64(pBootloadParams->guestFbLengthList[i], vmmuSegmentSize))
            {
                NV_PRINTF(LEVEL_ERROR, "guest fb segment PA or length is not VMMU segment size aligned\n");
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }
            if (gpuIsSelfHosted(pGpu))
            {
                pKernelHostVgpuDevice->hbmRegionList[i].hbmBaseAddr = memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU, 0);
                pKernelHostVgpuDevice->hbmRegionList[i].hbmBaseAddr += pKernelMemorySystem->coherentCpuFbBase;
                pKernelHostVgpuDevice->hbmRegionList[i].size = pMemory->pMemDesc->Size;
                pKernelHostVgpuDevice->numValidHbmRegions++;
            }
        }

        status = memGetByHandle(pClient, pAllocParams->hPluginHeapMemory, &pMemory);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid plugin heap Memory handle\n");
            goto done;
        }

        pKernelHostVgpuDevice->pGspPluginHeapMemDesc = pMemory->pMemDesc;

        // As GSP-RM will map this memory in vGPU-GSP-Plugin's address space, it
        // should be referenced to make sure it doesn't get freed before
        // vGPU-Gsp-Plugin's usage has been removed
        memdescAddRef(pKernelHostVgpuDevice->pGspPluginHeapMemDesc);

        pBootloadParams->pluginHeapMemoryPhysAddr = memdescGetPhysAddr(pMemory->pMemDesc, AT_GPU, 0);
        pBootloadParams->pluginHeapMemoryLength = pMemory->pMemDesc->Size;

        // Initialize logging buffers for vgpu partition
        {
            if ((pAllocParams->initTaskLogBuffOffset + pAllocParams->initTaskLogBuffSize) >=
                pBootloadParams->pluginHeapMemoryLength)
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid init task log buffer\n");
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            if ((pAllocParams->vgpuTaskLogBuffOffset + pAllocParams->vgpuTaskLogBuffSize) >=
                pBootloadParams->pluginHeapMemoryLength)
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid vgpu task log buffer\n");
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            if ((pAllocParams->kernelLogBuffOffset + pAllocParams->kernelLogBuffSize) >=
                pBootloadParams->pluginHeapMemoryLength)
            {
                NV_PRINTF(LEVEL_ERROR, "Invalid vgpu kernel log buffer\n");
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }


            pBootloadParams->initTaskLogBuffOffset = pAllocParams->initTaskLogBuffOffset + pBootloadParams->pluginHeapMemoryPhysAddr;
            pBootloadParams->initTaskLogBuffSize = pAllocParams->initTaskLogBuffSize;
            pBootloadParams->vgpuTaskLogBuffOffset = pAllocParams->vgpuTaskLogBuffOffset + pBootloadParams->pluginHeapMemoryPhysAddr;
            pBootloadParams->vgpuTaskLogBuffSize = pAllocParams->vgpuTaskLogBuffSize;
            pBootloadParams->kernelLogBuffOffset = pAllocParams->kernelLogBuffOffset + pBootloadParams->pluginHeapMemoryPhysAddr;
            pBootloadParams->kernelLogBuffSize = pAllocParams->kernelLogBuffSize;

            NV_CHECK_OK_OR_GOTO(status,
                                LEVEL_ERROR,
                                kgspInitVgpuPartitionLogging_HAL(pGpu, pKernelGsp, pAllocParams->gfid,
                                                                 pBootloadParams->initTaskLogBuffOffset,
                                                                 pBootloadParams->initTaskLogBuffSize,
                                                                 pBootloadParams->vgpuTaskLogBuffOffset,
                                                                 pBootloadParams->vgpuTaskLogBuffSize,
                                                                 pBootloadParams->kernelLogBuffOffset,
                                                                 pBootloadParams->kernelLogBuffSize),
                                done);
        }


        pBootloadParams->gfid                              = pAllocParams->gfid;
        pBootloadParams->swizzId                           = pAllocParams->swizzId;
        pBootloadParams->numGuestFbSegments                = pAllocParams->numGuestFbHandles;
        pBootloadParams->ctrlBuffOffset                    = pAllocParams->ctrlBuffOffset;
        pBootloadParams->bDeviceProfilingEnabled           = pAllocParams->bDeviceProfilingEnabled;

        // Populate chidOffset for all engines to reserve same chid in GSP-RM
        for (i = 0; i < NV2080_ENGINE_TYPE_LAST; i++)
        {
            NvU32 rmEngineType = gpuGetRmEngineType(i);

            //
            // pHostVgpuDevice->chidOffset is in RM_ENGINE_TYPE order
            // pBootloadParams->chidOffset is in NV2080_ENGINE_TYPE order
            //
            pBootloadParams->chidOffset[i] = pKernelHostVgpuDevice->chidOffset[rmEngineType];
        }

        status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK,
                                 pBootloadParams, sizeof(*pBootloadParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to call NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK\n");
            goto freeLogging;
        }
    }

    if (status == NV_OK)
    {
        if (pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE)
        {
            if (pDevice->pKernelHostVgpuDevice != NULL)
            {
                status = NV_ERR_INVALID_STATE;
                goto freeLogging;
            }
            pDevice->pKernelHostVgpuDevice = pKernelHostVgpuDevice;
        }

        CliNotifyVgpuConfigEvent(pGpu, NVA081_NOTIFIERS_EVENT_VGPU_GUEST_CREATED);
    }

freeLogging:
    if (status != NV_OK && IS_GSP_CLIENT(pGpu) && pBootloadParams != NULL)
    {
        // Free vgpu partition LIBOS task logging structures.
        NV_ASSERT_OK(kgspFreeVgpuPartitionLogging_HAL(pGpu, pKernelGsp, pKernelHostVgpuDevice->gfid));
    }

done:
    portMemFree(pBootloadParams);

    if (status != NV_OK)
    {
        if (pKernelHostVgpuDevice->hbmRegionList != NULL)
        {
            portMemFree(pKernelHostVgpuDevice->hbmRegionList);
            pKernelHostVgpuDevice->hbmRegionList = NULL;
            pKernelHostVgpuDevice->numValidHbmRegions = 0;
        }

        if (pKernelHostVgpuDevice->pGspPluginHeapMemDesc != NULL)
        {
            memdescRemoveRef(pKernelHostVgpuDevice->pGspPluginHeapMemDesc);
            pKernelHostVgpuDevice->pGspPluginHeapMemDesc = NULL;
        }

        serverFreeShare(&g_resServ, pShared);
    }

allocShareError:
    if (status != NV_OK)
        kvgpumgrGuestUnregister(pGpu, pKernelHostVgpuDevice);

    return status;
}

NvBool
kernelhostvgpudeviceapiCanCopy_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
    return NV_TRUE;
}

NV_STATUS
kernelhostvgpudeviceapiCopyConstruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApiSrc = dynamicCast(pParams->pSrcRef->pResource, KernelHostVgpuDeviceApi);
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    Device *pDevice;

    if (pKernelHostVgpuDeviceApiSrc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    NV_ASSERT_OK_OR_RETURN(deviceGetByGpu(pCallContext->pClient, pGpu, NV_TRUE, &pDevice));

    serverRefShare(&g_resServ, staticCast(pKernelHostVgpuDeviceApiSrc->pShared, RsShared));
    pKernelHostVgpuDeviceApi->pShared = pKernelHostVgpuDeviceApiSrc->pShared;

    if (pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE)
    {
        if (pDevice->pKernelHostVgpuDevice != NULL)
            return NV_ERR_INVALID_STATE;
        pDevice->pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    }

    return NV_OK;
}

void
destroyKernelHostVgpuDeviceShare(OBJGPU *pGpu, KernelHostVgpuDeviceShr* pShare)
{
    NV_CHECK_OR_RETURN_VOID(LEVEL_NOTICE, pShare != NULL);

    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = pShare->pDevice;
    RsShared *pShared = staticCast(pShare, RsShared);
    NvS32 refCount;
    NV_STATUS status;

    refCount = serverGetShareRefCount(&g_resServ, pShared);
    serverFreeShare(&g_resServ, pShared);
    if (refCount > 1)
        return;

    Device *pDevice = vgpuGetCallingContextDevice(pGpu);

    NV_ASSERT(pDevice != NULL);
    if (pDevice != NULL)
    {
        pDevice->pKernelHostVgpuDevice = NULL;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_PARAMS shutdownParams = { 0 };
        NV2080_CTRL_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP_PARAMS cleanupResourcesParams = {0};

        shutdownParams.gfid = pKernelHostVgpuDevice->gfid;

        status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK,
                                 &shutdownParams, sizeof(shutdownParams));
        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Failed to call NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SHUTDOWN_GSP_VGPU_PLUGIN_TASK\n");

        cleanupResourcesParams.gfid = pKernelHostVgpuDevice->gfid;
        status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_VGPU_PLUGIN_CLEANUP,
                                 &cleanupResourcesParams, sizeof(cleanupResourcesParams));
        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Failed to call cleanup plugin resources\n");

        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && pKernelHostVgpuDevice->pGspPluginHeapMemDesc != NULL)
        {
            memdescDestroy(pKernelHostVgpuDevice->pGspPluginHeapMemDesc);
        }

        // Free vgpu partition LIBOS task logging structures.
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        kgspFreeVgpuPartitionLogging(pGpu, pKernelGsp, pKernelHostVgpuDevice->gfid);
    }

    status = kvgpumgrGuestUnregister(pGpu, pKernelHostVgpuDevice);
    if (status != NV_OK)
        NV_PRINTF(LEVEL_ERROR, "Failed to unregister HOST_VGPU_DEVICE object\n");

    CliNotifyVgpuConfigEvent(pGpu, NVA081_NOTIFIERS_EVENT_VGPU_GUEST_DESTROYED);
}

void
kernelhostvgpudeviceapiDestruct_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
    destroyKernelHostVgpuDeviceShare(GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi),
                                     pKernelHostVgpuDeviceApi->pShared);
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;

    portMemCopy(pKernelHostVgpuDevice->vgpuUuid, RM_SHA1_GID_SIZE, pParams->vgpuUuid, RM_SHA1_GID_SIZE);

    CliNotifyVgpuConfigEvent(pGpu, NVA081_NOTIFIERS_EVENT_VGPU_GUEST_CREATED);

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    if (pParams->vmLifeCycleState == NVA081_NOTIFIERS_EVENT_VGPU_GUEST_DESTROYED)
        status = kvgpumgrClearGuestVmInfo(pGpu, pKernelHostVgpuDeviceApi->pShared->pDevice);

    CliNotifyVgpuConfigEvent(pGpu, pParams->vmLifeCycleState);

    return status;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams
)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS
_kernelhostvgpudeviceValidateOfflinedPageInfoAndGetSpa
(
    OBJGPU                  *pGpu,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64                   gpa,
    NvHandle                hMemory,
    NvU64                   size,
    NvU64                   *Spa
)
{
    NV_STATUS     status          = NV_OK;
    Memory        *pMemoryInfo    = NULL;
    RsClient      *pClient        = NULL;

    // Validate GPA from offlined page list
    if (pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.bValid)
    {
        NvU64 guestFbLength = pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.length;

        NV_ASSERT_OR_RETURN((gpa < guestFbLength)           &&
                            (size < guestFbLength)          &&
                            ((gpa + size) < guestFbLength),
                            NV_ERR_OUT_OF_RANGE);
    }

    // Validate SPA from good backup page list
    status = serverGetClientUnderLock(&g_resServ, pKernelHostVgpuDevice->hPluginFBAllocationClient, &pClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get pClient\n");
        return NV_ERR_INVALID_CLIENT;
    }

    status = memGetByHandle(pClient, hMemory, &pMemoryInfo);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid Memory handle\n");
        return status;
    }

    *Spa = memdescGetPhysAddr(pMemoryInfo->pMemDesc, AT_GPU, 0);

    return status;
}

static NV_STATUS
_kernelhostvgpudeviceGetFbSegmentPageShift
(
    NvU64 guestFbSegmentPageSize,
    NvU32 *guestFbSegmentPageShift
)
{
    switch (guestFbSegmentPageSize)
    {
        case RM_PAGE_SIZE_HUGE:
             *guestFbSegmentPageShift = RM_PAGE_SHIFT_HUGE;
             break;
        case RM_PAGE_SIZE_64K:
             *guestFbSegmentPageShift = RM_PAGE_SHIFT_64K;
             break;
        default:
             return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams
)
{
    NvU32       guestPageCount;
    NvU32       hpfn;
    NvU32       i;
    NvU32       index;
    NvU32       guestFbSegmentPageShift;
    NV_STATUS   rmStatus               = NV_OK;
    OBJGPU     *pGpu                   = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    NvBool      bPageRetirementEnabled = NV_FALSE;
    NvU64       Spa                    = 0;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    bPageRetirementEnabled = (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT) &&
                              gpuCheckPageRetirementSupport_HAL(pGpu));

    if (gpuIsSriovEnabled(pGpu) || !bPageRetirementEnabled)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    if (pKernelHostVgpuDevice && pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.bValid)
    {
        rmStatus = _kernelhostvgpudeviceGetFbSegmentPageShift(pParams->guestFbSegmentPageSize, &guestFbSegmentPageShift);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid Guest FB Segment Page Size\n");
            return rmStatus;
        }

        guestPageCount = pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.length >> guestFbSegmentPageShift;
        hpfn           = pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.offset >> guestFbSegmentPageShift;

        NV_ASSERT_OR_RETURN((pParams->offlinedPageCount <= NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES), NV_ERR_OUT_OF_RANGE);

        pKernelHostVgpuDevice->pGuestFbSegment = portMemAllocNonPaged(sizeof(NvU32) * guestPageCount);
        if (pKernelHostVgpuDevice->pGuestFbSegment == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "GuestFbSegment allocation failed\n");
            return NV_ERR_NO_MEMORY;
        }

        // Initialize the default hpfn of each segment.
        for (i = 0; i < guestPageCount; i++)
        {
            pKernelHostVgpuDevice->pGuestFbSegment[i] = hpfn + i;
        }

        // Patch offlined page with good page.
        for (i = 0; i < pParams->offlinedPageCount ;i++)
        {
            rmStatus = _kernelhostvgpudeviceValidateOfflinedPageInfoAndGetSpa(pGpu, pKernelHostVgpuDevice, pParams->gpa[i], pParams->hMemory[i],
                                                                              pParams->guestFbSegmentPageSize, &Spa);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Offlined Page info Validation Failed\n");
                portMemFree(pKernelHostVgpuDevice->pGuestFbSegment);
                pKernelHostVgpuDevice->pGuestFbSegment = NULL;
                return rmStatus;
            }

            index = pParams->gpa[i] >> guestFbSegmentPageShift;
            pKernelHostVgpuDevice->pGuestFbSegment[index] = Spa >> guestFbSegmentPageShift;
            pKernelHostVgpuDevice->offlinedPageGpa[i]           = pParams->gpa[i];
        }

        pKernelHostVgpuDevice->guestFbSegmentPageSize = pParams->guestFbSegmentPageSize;
        pKernelHostVgpuDevice->offlinedPageCount      = pParams->offlinedPageCount;
        pKernelHostVgpuDevice->bOfflinedPageInfoValid = NV_TRUE;
    }

    return rmStatus;
}

NV_STATUS
kernelhostvgpudeviceGetGuestFbInfo
(
    OBJGPU                      *pGpu,
    KERNEL_HOST_VGPU_DEVICE     *pKernelHostVgpuDevice,
    VGPU_DEVICE_GUEST_FB_INFO   *pFbInfo
)
{
    if (gpuIsSriovEnabled(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    *pFbInfo = pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo;

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceSetGuestFbInfo
(
    OBJGPU *pGpu,
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
    NvU64 offset,
    NvU64 length
)
{
    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    if (gpuIsSriovEnabled(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.bValid = NV_TRUE;
    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.offset = offset;
    pKernelHostVgpuDevice->vgpuDeviceGuestFbInfo.length = length;

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    Intr     *pIntr    = GPU_GET_INTR(pGpu);
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    NvU32 handle;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pParams->handle == NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_HANDLE,
        NV_ERR_INVALID_ARGUMENT);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) ||
        !gpuIsSriovEnabled(pGpu) || IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    handle = NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_START(pKernelHostVgpuDevice->gfid) + pParams->handle;

    status = intrTriggerPrivDoorbell_HAL(pGpu, pIntr, handle);

    return status;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    NV_STATUS status = NV_OK;

    // NV01_EVENT must have been plugged into this subdevice
    if (inotifyGetNotificationList(staticCast(pKernelHostVgpuDeviceApi, INotifier)) == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "cmd 0x%x: no event list\n",
            NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION);
        return NV_ERR_INVALID_STATE;
    }

    if (pSetEventParams->event >= NVA084_NOTIFIERS_MAXCOUNT)
    {
        NV_PRINTF(LEVEL_ERROR, "bad event 0x%x\n", pSetEventParams->event);
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // @todo: We will define the actual event values later based on the use case.
    // These event values are only for Test purpose.
    //
    switch (pSetEventParams->action)
    {
        case NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE:
        case NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT:
        {
            // must be in disabled state to transition to an active state
            if (pKernelHostVgpuDeviceApi->notifyActions[pSetEventParams->event] != NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                status = NV_ERR_INVALID_STATE;
                break;
            }

            pKernelHostVgpuDeviceApi->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }

        case NVA084_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
        {
            pKernelHostVgpuDeviceApi->notifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return status;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    NvU64 prevOffset = 0, prevSize = 0;
    NvU64 vfRegionOffsets[NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES];
    NvU64 vfRegionSizes[NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES];
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32 numAreas = 0, i = 0, j = 0;

    NV_ASSERT_OR_RETURN(pKernelBif != NULL, NV_ERR_INVALID_STATE);

    pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice != NULL, NV_ERR_INVALID_STATE);

    // Get the total number of ranges
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kbifGetVFSparseMmapRegions_HAL(
            pGpu,
            pKernelBif,
            pKernelHostVgpuDevice,
            pParams->osPageSize,
            &numAreas,
            NULL,
            NULL));

    NV_ASSERT_OR_RETURN(numAreas <= NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES, NV_ERR_INVALID_STATE);

    // Fill the regions
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kbifGetVFSparseMmapRegions_HAL(
            pGpu,
            pKernelBif,
            pKernelHostVgpuDevice,
            pParams->osPageSize,
            &numAreas,
            vfRegionOffsets,
            vfRegionSizes));

    if (numAreas == 0 || numAreas > NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_MAX_BAR_MAPPING_RANGES)
        return NV_ERR_INVALID_STATE;

    for (i = 0, j = 0; i < numAreas; i++)
    {
        if (i == 0 && vfRegionOffsets[i] == prevOffset)
        {
            pParams->offsets[j] = vfRegionOffsets[i];
            pParams->sizes[j] = vfRegionSizes[i];
            pParams->mitigated[j] = NV_FALSE;
            j++;
        }
        else
        {
            pParams->offsets[j] = prevOffset + prevSize;
            pParams->sizes[j] = vfRegionOffsets[i] - pParams->offsets[j];
            pParams->mitigated[j] = NV_TRUE;
            j++;

            pParams->offsets[j] = vfRegionOffsets[i];
            pParams->sizes[j] = vfRegionSizes[i];
            pParams->mitigated[j] = NV_FALSE;
            j++;
        }

        prevOffset = vfRegionOffsets[i];
        prevSize = vfRegionSizes[i];
    }

    i = numAreas - 1;
    if ((vfRegionOffsets[i] + vfRegionSizes[i]) != pGpu->sriovState.vfBarSize[0])
    {
        pParams->offsets[j] = vfRegionOffsets[i] + vfRegionSizes[i];
        pParams->sizes[j] = pGpu->sriovState.vfBarSize[0] - pParams->offsets[j];
        pParams->mitigated[j] = NV_TRUE;
        j++;
    }

    pParams->numRanges = j;
    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);

    pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice != NULL, NV_ERR_INVALID_STATE);

    if (pKernelHostVgpuDevice->bDisableDefaultSmcExecPartRestore)
    {
        NV_PRINTF(LEVEL_ERROR, "Default MIG compute instance save/restore disabled\n");
        return NV_ERR_INVALID_STATE;
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

        //
        // This function should only be called when MIG is enabled and should be
        // called for partitionable engines only
        //
        NV_ASSERT_OK_OR_RETURN(kmigmgrGetGPUInstanceInfo(pGpu, pKernelMIGManager,
                                                         pKernelHostVgpuDevice->swizzId,
                                                         &pKernelMIGGpuInstance));

        NV_ASSERT_OK_OR_RETURN(kvgpuMgrRestoreSmcExecPart(pGpu,
                                                          pKernelHostVgpuDevice,
                                                          pKernelMIGGpuInstance));
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdFreeStates_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS* pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;

    NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS params;
    params.gfid = pKernelHostVgpuDevice->gfid;
    params.flags = pParams->flags;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_FREE_STATES, &params, sizeof(params)));

    return NV_OK;
}
