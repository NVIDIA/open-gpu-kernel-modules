/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"
#include "platform/sli/sli.h"
#include "core/core.h"
#include "core/locks.h"
#include "os/os.h"
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
#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
#include "diagnostics/instrumentation_manager.h"
#endif
#include "libraries/nvkv/nvkv.h"
#include "core/gmcapi_impl.h"
#include "gmcapi/gmcapi_vgpu.h"

ct_assert(NVA084_MAX_VMMU_SEGMENTS == NV2080_CTRL_MAX_VMMU_SEGMENTS);
ct_assert(NV2080_GPU_MAX_ENGINES == RM_ENGINE_TYPE_LAST);

static NV_STATUS
_kernelhostvgpudeviceapiKVEncodeBootloadParams
(
    const NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pBootloadParams,
    NvU64 *kvData,
    NvU64 *kvCount,
    NvU64 kvLimit
);

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
    NVA084_ALLOC_PARAMETERS *pAllocParams = pParams->pAllocParams;
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
                                   pAllocParams->accountingPid,
                                   &pKernelHostVgpuDevice);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to register HOST_VGPU_DEVICE object\n");
        return status;
    }

    status = serverAllocShare(&g_resServ, classInfo(KernelHostVgpuDeviceShr), &pShared);
    if (status != NV_OK)
        goto allocShareError;

    pKernelHostVgpuDeviceApi->pShared                = dynamicCast(pShared, KernelHostVgpuDeviceShr);
    pKernelHostVgpuDeviceApi->pShared->pDevice       = pKernelHostVgpuDevice;
    pKernelHostVgpuDevice->pGspPluginHeapMemDesc     = NULL;
    pKernelHostVgpuDevice->bGspPluginTaskInitialized = NV_FALSE;
    pKernelHostVgpuDevice->bGspPluginTaskShutdownComplete = NV_FALSE;
    pKernelHostVgpuDevice->vgpuDeviceInstanceId      = pAllocParams->vgpuDeviceInstanceId;

    pKernelHostVgpuDevice->bGpupLiveMigrationEnabled = pAllocParams->bGpupLiveMigrationEnabled;

    if (status == NV_OK)
    {
        if (pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE)
        {
            if (pDevice->pKernelHostVgpuDevice != NULL)
            {
                status = NV_ERR_INVALID_STATE;
                goto done;
            }
            pDevice->pKernelHostVgpuDevice = pKernelHostVgpuDevice;
        }

        CliNotifyVgpuConfigEvent(pGpu, NVA081_NOTIFIERS_EVENT_VGPU_GUEST_CREATED);
    }

done:
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

static NV_STATUS
_kernelhostvgpudeviceInvalidateGpuTLBL2Cache(OBJGPU *pGpu, Device *pDevice)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NV2080_CTRL_FB_FLUSH_GPU_CACHE_PARAMS *pl2_params = portMemAllocNonPaged(sizeof(*pl2_params));
    NV_STATUS rmStatus = NV_OK;

    if (pl2_params == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    /* Invalidate TLBs for non SR-IOV vGPUs */
    if (!gpuIsSriovEnabled(pGpu))
    {
        kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu, NULL, VASPACE_FLAGS_NONE,
                               PTE_DOWNGRADE, GPU_GFID_PF, NV_GMMU_INVAL_SCOPE_ALL_TLBS, NV_FALSE);

    }

    /* Invalidate L2 cache for sysmem */
    pl2_params->flags =
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _APERTURE, _SYSTEM_MEMORY) |
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE, _YES)         |
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FLUSH_MODE, _FULL_CACHE);

    rmStatus = kmemsysFlushGpuCache(pGpu, pKernelMemorySystem, pKernelBus, pl2_params);

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to invalidate gpu L2 cache for sysmem. Status:0x%x\n", rmStatus);
    }

    /* Invalidate L2 cache for vidmem */
    portMemSet(pl2_params, 0, sizeof(*pl2_params));
    pl2_params->flags =
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _APERTURE, _VIDEO_MEMORY)  |
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _INVALIDATE, _YES)         |
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FLUSH_MODE, _FULL_CACHE)  |
        DRF_DEF(2080, _CTRL_FB_FLUSH_GPU_CACHE_FLAGS, _FB_FLUSH, _NO);

    rmStatus = kmemsysFlushGpuCache(pGpu, pKernelMemorySystem, pKernelBus, pl2_params);

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to invalidate gpu L2 cache for vidmem. Status:0x%x\n", rmStatus);
    }

    portMemFree(pl2_params);
    return rmStatus;
}

// This gets called when the vGPU plugin task is shutdown and the event is delivered.
NV_STATUS
gmcapiShutdownGspVgpuPluginTaskComplete(GMCAPI_CONTEXT *pCtx)
{
    OBJGPU *pGpu = pCtx->pGpu;
    GmcApiShutdownGspVgpuPluginTaskCompleteEvent *pEvent = (GmcApiShutdownGspVgpuPluginTaskCompleteEvent *)pCtx->pInParams;

    // Find the KERNEL_HOST_VGPU_DEVICE object that matches the GFID on this GPU.
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = NULL;
    NV_ASSERT_OK_OR_RETURN(kvgpumgrGetHostVgpuDeviceFromGfid(pGpu->gpuId, pEvent->gfid, &pKernelHostVgpuDevice));

    // Mark the vGPU plugin task as shutdown complete.
    pKernelHostVgpuDevice->bGspPluginTaskShutdownComplete = NV_TRUE;
    return NV_OK;
}

//
// The GMCAPI_VGPU_PLUGIN_EVENT_* values are the stable wire encodings we get from GSP.
// The NVA084_NOTIFIERS_EVENT_* values are what RM uses internally, and what is used
// by RM clients via RMAPI. These are somewhat less stable in general, but there
// is no reason they shouldn't be the same - this way we don't have to do any translations
//
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_TASK_BOOTLOADED       == NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_BOOTLOADED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_TASK_UNLOADED         == NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_UNLOADED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_TASK_CRASHED          == NVA084_NOTIFIERS_EVENT_VGPU_PLUGIN_TASK_CRASHED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_GUEST_DRIVER_LOADED   == NVA084_NOTIFIERS_EVENT_GUEST_DRIVER_LOADED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_GUEST_DRIVER_UNLOADED == NVA084_NOTIFIERS_EVENT_GUEST_DRIVER_UNLOADED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_PRINT_ERROR_MESSAGE   == NVA084_NOTIFIERS_EVENT_PRINT_ERROR_MESSAGE);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_GUEST_LICENSE_STATE   == NVA084_NOTIFIERS_EVENT_GUEST_LICENSE_STATE_CHANGED);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_UPDATE_GUEST_OS_TYPE  == NVA084_NOTIFIERS_EVENT_UPDATE_GUEST_OS_TYPE);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_PRINT_GUEST_RPC_TRACE == NVA084_NOTIFIERS_EVENT_PRINT_GUEST_RPC_TRACE_LOG_MESSAGE);
ct_assert(GMCAPI_VGPU_PLUGIN_EVENT_INIT_GR_ENGINE        == NVA084_NOTIFIERS_EVENT_INIT_GR_ENGINE);

//
// GSP -> Kernel notification that the vGPU plugin task triggered an event.
// The kernel delivers this to any registered NVA084_KERNEL_HOST_VGPU_DEVICE listeners.
//
NV_STATUS
gmcapiVgpuPluginTriggeredEvent(GMCAPI_CONTEXT *pCtx)
{
    OBJGPU *pGpu = pCtx->pGpu;
    const GmcApiVgpuPluginTriggeredEvent *pEvt = (const GmcApiVgpuPluginTriggeredEvent *)pCtx->pInParams;

    NV_ASSERT_OR_RETURN(IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pEvt->eventId < NVA084_NOTIFIERS_MAXCOUNT, NV_ERR_INVALID_ARGUMENT);

    gpuGspPluginTriggeredEvent(pGpu, pEvt->gfid, pEvt->eventId);
    return NV_OK;
}

static NvBool
_vgpuPluginTaskIsShutdownComplete(OBJGPU *pGpu, void *contextData)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = (KERNEL_HOST_VGPU_DEVICE *)contextData;
    // If this is NULL then something bad happened, but in any case don't let
    // the event polling call continue to wait.
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice != NULL, NV_TRUE);

    return pKernelHostVgpuDevice->bGspPluginTaskShutdownComplete;
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
        status = _kernelhostvgpudeviceInvalidateGpuTLBL2Cache(pGpu, pDevice);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to invalidate TLBs and L2 cache. Status:0x%x\n", status);
        }
        pDevice->pKernelHostVgpuDevice = NULL;
    }

    if (IS_GSP_CLIENT(pGpu) && pKernelHostVgpuDevice->bGspPluginTaskInitialized)
    {
        NvU32 defaultus = pGpu->timeoutData.defaultus;

        pGpu->timeoutData.defaultus = 30*1000*1000;

        GmcApiShutdownGspVgpuPluginTaskRequest shutdown_request = {.gfid = pKernelHostVgpuDevice->gfid};
        // Note: We don't forcibly clear the bGspPluginTaskShutdownComplete flag here because
        // it may have already crashed, in which case GSP may not send us a completion event when
        // we request the shutdown here.
        status = gmcapiCommand(pGpu, GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK, &shutdown_request, (NvU32)sizeof(shutdown_request), NULL, NULL);
        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Failed to call GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK\n");

        // Shutting down the vGPU plugin task can take some time, and GSP needs to be able to respond to
        // RPCs and send events while this is going on.  When the work is completed, it will send us
        // a GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_COMPLETE event.  So we process RPCs until the
        // kernel handler for that is called, which will set the shutdown complete flag our condition
        // function is looking for.
        status = gpuRpcConditionWait(pGpu, _vgpuPluginTaskIsShutdownComplete, pKernelHostVgpuDevice);
        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Failed to wait for GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_COMPLETE\n");

        GmcApiCleanupGspVgpuPluginResourcesRequest cleanup_request = {.gfid = pKernelHostVgpuDevice->gfid};
        status = gmcapiCommand(pGpu, GMCAPI_CMD_CLEANUP_GSP_VGPU_PLUGIN_RESOURCES, &cleanup_request, (NvU32)sizeof(cleanup_request), NULL, NULL);
        if (status != NV_OK)
            NV_PRINTF(LEVEL_ERROR, "Failed to call GMCAPI_CMD_CLEANUP_GSP_VGPU_PLUGIN_RESOURCES\n");

        pGpu->timeoutData.defaultus = defaultus;

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       pParams->vmLifeCycleState < NVA081_NOTIFIERS_MAXCOUNT,
                       NV_ERR_INVALID_ARGUMENT);

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
    NvU32      *pNewGuestFbSegment     = NULL;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

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

        pNewGuestFbSegment = portMemAllocNonPaged(sizeof(NvU32) * guestPageCount);
        if (pNewGuestFbSegment == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "GuestFbSegment allocation failed\n");
            return NV_ERR_NO_MEMORY;
        }

        // Initialize the default hpfn of each segment.
        for (i = 0; i < guestPageCount; i++)
        {
            pNewGuestFbSegment[i] = hpfn + i;
        }

        // Patch offlined page with good page.
        for (i = 0; i < pParams->offlinedPageCount ;i++)
        {
            rmStatus = _kernelhostvgpudeviceValidateOfflinedPageInfoAndGetSpa(pGpu, pKernelHostVgpuDevice, pParams->gpa[i], pParams->hMemory[i],
                                                                              pParams->guestFbSegmentPageSize, &Spa);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Offlined Page info Validation Failed\n");
                portMemFree(pNewGuestFbSegment);
                return rmStatus;
            }

            index = pParams->gpa[i] >> guestFbSegmentPageShift;
            pNewGuestFbSegment[index] = Spa >> guestFbSegmentPageShift;
            pKernelHostVgpuDevice->offlinedPageGpa[i]           = pParams->gpa[i];
        }

        if (pKernelHostVgpuDevice->pGuestFbSegment != NULL)
        {
            portMemFree(pKernelHostVgpuDevice->pGuestFbSegment);
        }
        pKernelHostVgpuDevice->pGuestFbSegment = pNewGuestFbSegment;

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pParams->handle == NV_DOORBELL_NOTIFY_LEAF_VF_CPU_PLUGIN_HANDLE,
        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

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
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

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

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu) || pKernelHostVgpuDeviceApi->pShared->pDevice->bGspPluginTaskInitialized,
                        NV_ERR_INVALID_STATE);

    NV2080_CTRL_VGPU_MGR_INTERNAL_FREE_STATES_PARAMS params;
    params.gfid = pKernelHostVgpuDevice->gfid;
    params.flags = pParams->flags;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_FREE_STATES, &params, sizeof(params)));

    return NV_OK;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BOOTLOAD_VGPU_TASK_PARAMS* pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pBootloadParams = NULL;
    Memory *pMemory;
    NvU32 i;
    NvU64 vmmuSegmentSize;
    RsClient *pClient = NULL;

    if (!IS_GSP_CLIENT(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    NV_ASSERT_OR_RETURN(pKernelHostVgpuDevice != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(!pKernelHostVgpuDevice->bGspPluginTaskInitialized, NV_ERR_INVALID_STATE);

    status = serverGetClientUnderLock(&g_resServ, pKernelHostVgpuDevice->hPluginFBAllocationClient, &pClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get pClient\n");
        return NV_ERR_INVALID_CLIENT;
    }

    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || !gpuIsSriovEnabled(pGpu))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    if (pParams->numGuestFbHandles == 0 || pParams->numGuestFbHandles > NVA084_MAX_VMMU_SEGMENTS)
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
        pKernelHostVgpuDevice->hbmRegionList = portMemAllocNonPaged(
                                                   sizeof(HBM_REGION_INFO)* pParams->numGuestFbHandles);
        if (pKernelHostVgpuDevice->hbmRegionList == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        pKernelHostVgpuDevice->numValidHbmRegions = 0;
    }

    for (i = 0; i < pParams->numGuestFbHandles; i++)
    {
        status = memGetByHandle(pClient, pParams->guestFbHandleList[i], &pMemory);
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

    status = memGetByHandle(pClient, pParams->hPluginHeapMemory, &pMemory);
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
        NvU64 logBuffEnd;

        if (!portSafeAddU64(pParams->initTaskLogBuffOffset, pParams->initTaskLogBuffSize, &logBuffEnd) ||
            (logBuffEnd >= pBootloadParams->pluginHeapMemoryLength))
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid init task log buffer\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (!portSafeAddU64(pParams->vgpuTaskLogBuffOffset, pParams->vgpuTaskLogBuffSize, &logBuffEnd) ||
            (logBuffEnd >= pBootloadParams->pluginHeapMemoryLength))
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid vgpu task log buffer\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (!portSafeAddU64(pParams->kernelLogBuffOffset, pParams->kernelLogBuffSize, &logBuffEnd) ||
            (logBuffEnd >= pBootloadParams->pluginHeapMemoryLength))
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid vgpu kernel log buffer\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (!portSafeAddU64(pParams->initTaskLogBuffOffset,
                            pBootloadParams->pluginHeapMemoryPhysAddr,
                            &pBootloadParams->initTaskLogBuffOffset) ||
            !portSafeAddU64(pParams->vgpuTaskLogBuffOffset,
                            pBootloadParams->pluginHeapMemoryPhysAddr,
                            &pBootloadParams->vgpuTaskLogBuffOffset) ||
            !portSafeAddU64(pParams->kernelLogBuffOffset,
                            pBootloadParams->pluginHeapMemoryPhysAddr,
                            &pBootloadParams->kernelLogBuffOffset))
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid vgpu log buffer offset\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
        pBootloadParams->initTaskLogBuffSize    = pParams->initTaskLogBuffSize;
        pBootloadParams->vgpuTaskLogBuffSize    = pParams->vgpuTaskLogBuffSize;
        pBootloadParams->kernelLogBuffSize      = pParams->kernelLogBuffSize;

        NV_CHECK_OK_OR_GOTO(status,
                            LEVEL_ERROR,
                            kgspInitVgpuPartitionLogging_HAL(pGpu, pKernelGsp, pKernelHostVgpuDevice->gfid,
                                                             pBootloadParams->initTaskLogBuffOffset,
                                                             pBootloadParams->initTaskLogBuffSize,
                                                             pBootloadParams->vgpuTaskLogBuffOffset,
                                                             pBootloadParams->vgpuTaskLogBuffSize,
                                                             pBootloadParams->kernelLogBuffOffset,
                                                             pBootloadParams->kernelLogBuffSize),
                            done);
    }

    pBootloadParams->gfid                              = pKernelHostVgpuDevice->gfid;
    pBootloadParams->swizzId                           = pKernelHostVgpuDevice->swizzId;
    pBootloadParams->numGuestFbSegments                = pParams->numGuestFbHandles;
    if (pParams->ctrlBuffOffset >= pBootloadParams->pluginHeapMemoryLength)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }
    pBootloadParams->ctrlBuffOffset                    = pParams->ctrlBuffOffset;
    pBootloadParams->bDeviceProfilingEnabled           = pParams->bDeviceProfilingEnabled;

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

    // The worst case size for the NVKV data should be less than 16KB.  In practice
    // it seems to be ~500 bytes.  Since the original params struct is about 6K we
    // just allocate (roughly) 2x that to be certain it will always fit.

    NvU64 *kvData = portMemAllocNonPaged(sizeof(NvU64) * 2048);
    if(kvData == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for NVKV data\n");
        status = NV_ERR_NO_MEMORY;
        goto done;
    }
    NvU64 kvCount = 0;
    NvU64 kvLimit = 2048;
    // Long term we should just encode directly to NVKV format and skip the intermediate
    // step of encoding to the 2080 control structure.
    status = _kernelhostvgpudeviceapiKVEncodeBootloadParams(pBootloadParams, kvData, &kvCount, kvLimit);
    if (status == NV_OK)
    {
        status = gmcapiCommand(pGpu, GMCAPI_CMD_BOOTLOAD_GSP_VGPU_PLUGIN_TASK, kvData, (NvU32)(sizeof(NvU64) * kvCount), NULL, NULL);
    }
    portMemFree(kvData);

    // Preserve any captured vGPU Partition logs
    NV_ASSERT_OK(kgspPreserveVgpuPartitionLogging(pGpu, pKernelGsp, pKernelHostVgpuDevice->gfid));

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    OBJSYS *pSys = SYS_GET_INSTANCE();
    instrumentationmanagerReset(pSys->pInstrumentationManager, pKernelHostVgpuDevice->gfid, pGpu->gpuInstance);
#endif
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to call GMCAPI_CMD_BOOTLOAD_GSP_VGPU_PLUGIN_TASK\n");
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

        kvgpumgrGuestUnregister(pGpu, pKernelHostVgpuDevice);
    }
    else
    {
        pKernelHostVgpuDevice->bGspPluginTaskInitialized = NV_TRUE;
    }

    return status;
}

NV_STATUS
kernelhostvgpudeviceapiCtrlCmdSetPlacementId_IMPL
(
    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi,
    NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_PLACEMENT_ID_PARAMS* pParams
)
{
    NV_STATUS                rmStatus               = NV_OK;
    OBJGPU                  *pGpu                   = GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi);
    NvBool                   bMIGInUse              = IS_MIG_IN_USE(pGpu);
    VGPU_TYPE               *vgpuTypeInfo           = NULL;
    Device                  *pMigDevice             = NULL;
    RsClient                *pClient;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    NV_PRINTF(LEVEL_INFO, "%s\n", __FUNCTION__);

    pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;
    pKernelHostVgpuDevice->placementId = pParams->placementId;

    if (!gpuIsSriovEnabled(pGpu))
        return rmStatus;

    NV_ASSERT_OK_OR_RETURN(kvgpumgrGetVgpuTypeInfo(pKernelHostVgpuDevice->vgpuType, &vgpuTypeInfo));

    // This block will execute in case of openRM
    if (IS_GSP_CLIENT(pGpu))
    {
        KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        ENGINE_INFO tmpEngineInfo;

        if (bMIGInUse)
        {
            NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, pKernelHostVgpuDevice->hMigClient, &pClient));
            NV_ASSERT_OK_OR_RETURN(deviceGetByHandle(pClient, pKernelHostVgpuDevice->hMigDevice, &pMigDevice));
        }

        portMemSet(&tmpEngineInfo, 0, sizeof(ENGINE_INFO));

        rmStatus = kfifoGetHostDeviceInfoTable_HAL(pGpu, pKernelFifo, &tmpEngineInfo, pMigDevice);
        if (rmStatus == NV_OK)
        {
            rmStatus = vgpuMgrReserveSystemChannelIDs(pGpu,
                                                      vgpuTypeInfo,
                                                      pKernelHostVgpuDevice->gfid,
                                                      pKernelHostVgpuDevice->chidOffset,
                                                      pKernelHostVgpuDevice->channelCount,
                                                      pMigDevice,
                                                      pParams->numChannels,
                                                      pKernelHostVgpuDevice->placementId,
                                                      tmpEngineInfo.engineInfoListSize,
                                                      tmpEngineInfo.engineInfoList);
        }

        portMemFree(tmpEngineInfo.engineInfoList);
        tmpEngineInfo.engineInfoList = NULL;
    }

    // This block will execute in case of monolithic RM

    return rmStatus;
}

static NV_STATUS
_kernelhostvgpudeviceapiKVEncodeBootloadParams
(
    const NV2080_CTRL_VGPU_MGR_INTERNAL_BOOTLOAD_GSP_VGPU_PLUGIN_TASK_PARAMS *pBootloadParams,
    NvU64 *kvData,
    NvU64 *kvCount,
    NvU64 kvLimit
)
{
    NV_ASSERT_OR_RETURN(kvCount != NULL, NV_ERR_INVALID_ARGUMENT);

    #define NVKV_PREFIX NVGMC_VGPU_BOOTLOAD

    NVKVContext ctx = NVKV_BEGIN(kvData, 0, kvLimit);
    NVKV_SET_SEQ32_4U(&ctx,  0, DBDF, pBootloadParams->dbdf,
                                GFID, pBootloadParams->gfid,
                                VGPU_TYPE, pBootloadParams->vgpuType,
                                VM_PID, pBootloadParams->vmPid);
    NVKV_SET_SEQ32_3U(&ctx,  0, SWIZZ_ID, pBootloadParams->swizzId,
                                NUM_CHANNELS, pBootloadParams->numChannels,
                                NUM_PLUGIN_CHANNELS, pBootloadParams->numPluginChannels);

    NvU32 i;
    NvU64 channelMappings[NV2080_ENGINE_TYPE_LAST] = { 0 };
    NvU64 channelMappingCount = 0;
    for (i = 0; i < NV2080_ENGINE_TYPE_LAST; i++)
    {
        // pBootloadParams->chidOffset is in NV2080_ENGINE_TYPE order.  So we reach each
        // entry from the table to get the offset for the NV2080_ENGINE_TYPE corresponding
        // to the numeric index.  We then convert that to the GMC engine ID.  We skip any
        // entries with a channel offset of 0.
        if(pBootloadParams->chidOffset[i] != 0)
        {
            channelMappings[channelMappingCount++] = NVKV_NUM64(CHANNEL_MAPPING_ENGINE_ID, gpuGetGMCEngineIdFromNv2080EngineType(i)) |
                                                     NVKV_NUM64(CHANNEL_MAPPING_OFFSET, pBootloadParams->chidOffset[i]);
        }
    }
    // Send as single bulk array.
    NVKV_SET_ARRAY64(&ctx, 0, CHANNEL_MAPPING, channelMappings, channelMappingCount);

    NVKV_SET_SEQ32_1U(&ctx, 0, GUEST_FB_SEGMENT_COUNT, pBootloadParams->numGuestFbSegments);
    NVKV_SET_ARRAY64(&ctx,  0, GUEST_FB_SEGMENT_PHYS_ADDR_LIST, pBootloadParams->guestFbPhysAddrList, pBootloadParams->numGuestFbSegments);
    NVKV_SET_ARRAY64(&ctx,  0, GUEST_FB_SEGMENT_LENGTH_LIST, pBootloadParams->guestFbLengthList, pBootloadParams->numGuestFbSegments);
    NVKV_SET_SEQ64_3U(&ctx, 0, PLUGIN_HEAP_MEMORY_PHYS_ADDR, pBootloadParams->pluginHeapMemoryPhysAddr,
                               PLUGIN_HEAP_MEMORY_LENGTH, pBootloadParams->pluginHeapMemoryLength,
                               CTRL_BUFF_OFFSET, pBootloadParams->ctrlBuffOffset);
    NVKV_SET_SEQ64_4U(&ctx, 0, INIT_TASK_LOG_BUFF_OFFSET, pBootloadParams->initTaskLogBuffOffset,
                               INIT_TASK_LOG_BUFF_SIZE, pBootloadParams->initTaskLogBuffSize,
                               VGPU_TASK_LOG_BUFF_OFFSET, pBootloadParams->vgpuTaskLogBuffOffset,
                               VGPU_TASK_LOG_BUFF_SIZE, pBootloadParams->vgpuTaskLogBuffSize);
    NVKV_SET_SEQ64_4U(&ctx, 0, KERNEL_LOG_BUFF_OFFSET, pBootloadParams->kernelLogBuffOffset,
                               KERNEL_LOG_BUFF_SIZE, pBootloadParams->kernelLogBuffSize,
                               MIG_RM_HEAP_MEMORY_PHYS_ADDR, pBootloadParams->migRmHeapMemoryPhysAddr,
                               MIG_RM_HEAP_MEMORY_LENGTH, pBootloadParams->migRmHeapMemoryLength);
    {
        NvU64 options = NVKV_NUM64(OPTIONS_DEVICE_PROFILING_ENABLED, pBootloadParams->bDeviceProfilingEnabled) |
                        NVKV_NUM64(OPTIONS_DISABLE_DEFAULT_SMC_EXEC_PART_RESTORE, pBootloadParams->bDisableDefaultSmcExecPartRestore);
        NVKV_SET_SEQ64_1U(&ctx, 0, OPTIONS, options);
    }
    *kvCount = NVKV_END(&ctx);

    if(*kvCount > kvLimit)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}
