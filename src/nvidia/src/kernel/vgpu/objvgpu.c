/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "gpu/gpu.h"
#include "vgpu/vgpu_version.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "rmapi/rs_utils.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "vgpu/vgpu_events.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_util.h"

#include "nvrm_registry.h"

OBJVGPU *NvVGPU_Table[NV_VGPU_MAX_INSTANCES];

static void vgpuGetUsmType(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    // Cache NV_VGPU_CONFIG_USM_TYPE field
    {
        // From GSP response buffer
        pVGpu->vgpuConfigUsmType = pVGpu->gspResponseBuf->v1.usmType;
    }
}

// Create vGpu object and initialize RPC infrastructure
NV_STATUS
vgpuCreateObject
(
    OBJGPU *pGpu
)
{
    NV_STATUS  rmStatus = NV_OK;
    OBJVGPU   *pVGpu;
    NvU32      gpuMaskRelease = 0;
    NvBool     bLockAcquired = NV_FALSE;

    if (gpuGetInstance(pGpu) >= NV_VGPU_MAX_INSTANCES)
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
        NV_PRINTF(LEVEL_ERROR,
                  "vGPU instances more than %d are not supported\n",
                  NV_VGPU_MAX_INSTANCES);
        goto error_exit;
    }

    if (NvVGPU_Table[gpuGetInstance(pGpu)] != NULL)
    {
        rmStatus = NV_ERR_INVALID_DEVICE;
        NV_PRINTF(LEVEL_ERROR, ": %d vGPU instance already allocated\n",
                  gpuGetInstance(pGpu));
        goto error_exit;
    }

    if (hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED, NV_FALSE);
    }

    pVGpu = portMemAllocNonPaged(sizeof(OBJVGPU));
    if (pVGpu == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "cannot allocate memory for OBJVGPU (instance %d)\n",
                  gpuGetInstance(pGpu));
        goto error_exit;
    }

    NvVGPU_Table[gpuGetInstance(pGpu)] = pVGpu;

    rmStatus = initRpcInfrastructure_VGPU(pGpu);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: _initRpcInfrastructure: failed.\n");
        NV_ASSERT(0);
        goto error_exit;
    }

    if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskRelease))
    {
        // Acquire lock
        NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                     GPU_LOCK_GRP_SUBDEVICE,
                                                     GPUS_LOCK_FLAGS_NONE,
                                                     RM_LOCK_MODULES_RPC,
                                                     &gpuMaskRelease));
        bLockAcquired = NV_TRUE;
    }

    NV_RM_RPC_GET_STATIC_DATA(pGpu, rmStatus);

    if (bLockAcquired && gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: GET_STATIC_DATA : failed.\n");
        goto error_exit;
    }

    vgpuGetUsmType(pGpu, pVGpu);

error_exit:
    return rmStatus;
}

// Free RPC infrastructure and vGpu object
void
vgpuDestructObject
(
    OBJGPU *pGpu
)
{
    OBJVGPU       *pVGpu = GPU_GET_VGPU(pGpu);
    NV_STATUS      rmStatus = NV_OK;

    NV_RM_RPC_UNLOADING_GUEST_DRIVER(pGpu, rmStatus, NV_FALSE, NV_FALSE, 0);

    {
        rmStatus = freeRpcInfrastructure_VGPU(pGpu);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: _freeRpcInfrastructure: failed.\n");
            NV_ASSERT(0);
        }
    }

    vgpuGspTeardownBuffers(pGpu);

    if (pVGpu != NULL)
        teardownSysmemPfnBitMap(pGpu, pVGpu);

    portMemFree(pVGpu);
    NvVGPU_Table[gpuGetInstance(pGpu)] = NULL;
}

// Overwrite registry keys
void
vgpuInitRegistryOverWrite
(
    OBJGPU *pGpu
)
{
    NvU32 data;

    // if "RMFermiBigPageSize" regkey is set explicitly, then don't
    // overwrite it.
    if (NV_OK != osReadRegistryDword(pGpu,
                                     NV_REG_STR_RM_DISABLE_BIG_PAGE_PER_ADDRESS_SPACE,
                                     &data))
    {
        NV_PRINTF(LEVEL_INFO, "Overwriting big page size to 64K\n");

        osWriteRegistryDword(pGpu,
                             NV_REG_STR_FERMI_BIG_PAGE_SIZE,
                             NV_REG_STR_FERMI_BIG_PAGE_SIZE_64KB);
        if (IS_VIRTUAL(pGpu))
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K, NV_TRUE);
        }
    }

    if (IS_VIRTUAL(pGpu))
    {
        if (NV_OK != osReadRegistryDword(pGpu,
                                         NV_REG_STR_RM_WATCHDOG_TIMEOUT,
                                         &data))
        {
            osWriteRegistryDword(pGpu, NV_REG_STR_RM_WATCHDOG_TIMEOUT, 2);
        }

    }

    NvU32 min = 0, max = 0;

    /* Default user provided vGPU supported range is set to be the
     * same as per the host */
    NvU32 user_min_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MIN_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);
    NvU32 user_max_supported_version
        = GRIDSW_VERSION_EXTERNAL(NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MAJOR,
                                  NV_VGPU_MAX_SUPPORTED_GRIDSW_VERSION_EXTERNAL_MINOR);

    if (NV_OK == osReadRegistryDword(pGpu,
                                     NV_REG_STR_RM_SET_VGPU_VERSION_MAX,
                                     &max))
    {
        /* Override max vGPU supported version */
        user_max_supported_version = max;
    }

    if (NV_OK == osReadRegistryDword(pGpu,
                                     NV_REG_STR_RM_SET_VGPU_VERSION_MIN,
                                     &min))
    {
        /* Override min vGPU supported version */
        user_min_supported_version = min;
    }

    /* Convey the vGPU range information to the vGPU manager */
    kvgpumgrSetHostVgpuVersion(user_min_supported_version, user_max_supported_version);
}

/*
 * @brief Gets the calling context's device pointer
 *
 * @param pGpu         OBJGPU pointer
 *
 * @return Pointer to the calling context's device pointer
 */
Device *
vgpuGetCallingContextDevice
(
    OBJGPU *pGpu
)
{
    RsResourceRef *pDeviceRef = NULL;
    Device        *pDevice    = NULL;

    pDeviceRef = resservGetContextRefByType(classId(Device), NV_TRUE);
    if (pDeviceRef != NULL)
    {
        pDevice = dynamicCast(pDeviceRef->pResource, Device);
    }

    return pDevice;
}

/*
 * @brief Gets the calling context's Host VGPU device pointer
 *
 * @param pGpu               OBJGPU pointer
 * @param ppHostVgpuDevice   Pointer to pointer to the calling
 *                           context's Host VGPU device pointer
 *
 * @return Error code
 */
NV_STATUS
vgpuGetCallingContextHostVgpuDevice
(
    OBJGPU *pGpu,
    HOST_VGPU_DEVICE **ppHostVgpuDevice
)
{
    *ppHostVgpuDevice = NULL;

    Device *pDevice = NULL;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    if (IS_GSP_CLIENT(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    // This check is needed to handle cases where this function was called
    // without client (for example, during adapter initialization).
    if (resservGetTlsCallContext() == NULL)
    {
        return NV_OK;
    }

    pDevice = vgpuGetCallingContextDevice(pGpu);
    if (pDevice == NULL)
    {
        // There are several places where this function can be called without TLS call context
        // in SRIOV-heavy mode. Return error only for SRIOV-full.
        NV_ASSERT_OR_RETURN(!gpuIsSriovEnabled(pGpu) || IS_SRIOV_HEAVY(pGpu), NV_ERR_OBJECT_NOT_FOUND);
        return NV_OK;
    }

    pKernelHostVgpuDevice = pDevice->pKernelHostVgpuDevice;

    NV_ASSERT_OR_RETURN((pKernelHostVgpuDevice != NULL) ==
            !!(pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE),
            NV_ERR_INVALID_STATE);

    if (pKernelHostVgpuDevice != NULL)
    {
        if (pKernelHostVgpuDevice->pHostVgpuDevice == NULL)
            return NV_ERR_INVALID_STATE;

        *ppHostVgpuDevice = pKernelHostVgpuDevice->pHostVgpuDevice;
    }

    return NV_OK;
}

NV_STATUS
vgpuGetCallingContextKernelHostVgpuDevice
(
    OBJGPU *pGpu,
    KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice
)
{
    Device *pDevice;

    if (RMCFG_FEATURE_PLATFORM_GSP)
        return NV_ERR_NOT_SUPPORTED;
    
    *ppKernelHostVgpuDevice = NULL;

    // This check is needed to handle cases where this function was called
    // without client (for example, during adapter initialization).
    if (resservGetTlsCallContext() == NULL)
    {
        return NV_OK;
    }

    pDevice = vgpuGetCallingContextDevice(pGpu);
    if (pDevice == NULL)
    {
        // There are several places where this function can be called without TLS call context
        // in SRIOV-heavy mode. Return error only for SRIOV-full.
        NV_ASSERT_OR_RETURN(!gpuIsSriovEnabled(pGpu) || IS_SRIOV_HEAVY(pGpu), NV_ERR_OBJECT_NOT_FOUND);
        return NV_OK;
    }
    
    *ppKernelHostVgpuDevice = pDevice->pKernelHostVgpuDevice;

    NV_ASSERT_OR_RETURN((pDevice->pKernelHostVgpuDevice != NULL) ==
            !!(pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE),
            NV_ERR_INVALID_STATE);

    return NV_OK;
}

/*
 * @brief Gets the calling context's GFID
 *     gsp-rm: when the vgpu plugin rpcs to gsp-rm, gfid is stored in an unused
 *             field, pProcessToken.
 *     cpu-rm: Retrieve from TLS a host vgpu device, and then gfid from it.
 *
 * @param pGpu         OBJGPU pointer
 * @param pGfid        calling context's GFID pointer
 *
 * @return Error code
 */
NV_STATUS
vgpuGetCallingContextGfid
(
    OBJGPU *pGpu,
    NvU32 *pGfid
)
{
    GFID_ALLOC_STATUS gfidState;

    *pGfid = GPU_GFID_PF;

    if (!gpuIsSriovEnabled(pGpu))
    {
        return NV_OK;
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        CALL_CONTEXT *pTls = resservGetTlsCallContext();

        if (pTls)
        {
            *pGfid = (NvU32)(NvU64)pTls->secInfo.pProcessToken;
        }
    }
    else
    {
        KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

        NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextKernelHostVgpuDevice(pGpu, &pKernelHostVgpuDevice));
        if (pKernelHostVgpuDevice != NULL)
        {
            *pGfid = pKernelHostVgpuDevice->gfid;
        }
    }

    // work around for bug 3432243 where this is called before setting gfid.
    if (IS_GFID_PF(*pGfid))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(gpuGetGfidState(pGpu, *pGfid, &gfidState));

    // Allow invalidated state to be retreived as GFID is still active in system
    NV_ASSERT_OR_RETURN((gfidState != GFID_FREE), NV_ERR_INVALID_STATE);

    return NV_OK;
}

NV_STATUS
vgpuGetGfidFromDeviceInfo
(
    OBJGPU  *pGpu,
    Device  *pDevice,
    NvU32   *pGfid
)
{
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

    if (RMCFG_FEATURE_PLATFORM_GSP)
        return NV_ERR_NOT_SUPPORTED;


    *pGfid = GPU_GFID_PF;

    NV_ASSERT_OR_RETURN(pDevice != NULL, NV_ERR_INVALID_ARGUMENT);

    if (!gpuIsSriovEnabled(pGpu))
    {
        return NV_OK;
    }

    // TODO: fix the calling sites in VAB and P2P (GPUSWSEC-783)
    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    // Get the HOST_VGPU_DEVICE from hClient
    pKernelHostVgpuDevice = pDevice->pKernelHostVgpuDevice;

    NV_ASSERT_OR_RETURN((pKernelHostVgpuDevice != NULL) ==
                        !!(pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE),
                        NV_ERR_INVALID_STATE);

    if (pKernelHostVgpuDevice != NULL)
    {
        GFID_ALLOC_STATUS gfidState;
        *pGfid = pKernelHostVgpuDevice->gfid;
        NV_ASSERT_OK_OR_RETURN(gpuGetGfidState(pGpu, *pGfid, &gfidState));
        NV_ASSERT_OR_RETURN((gfidState != GFID_FREE), NV_ERR_INSUFFICIENT_RESOURCES);
    }

    return NV_OK;
}

NV_STATUS
vgpuIsCallingContextPlugin
(
    OBJGPU *pGpu,
    NvBool *pIsCallingContextPlugin
)
{
    Device *pDevice         = NULL;

    *pIsCallingContextPlugin = NV_FALSE;

    if (!gpuIsSriovEnabled(pGpu))
    {
        return NV_OK;
    }

    // This check is needed to handle cases where this function was called
    // without client (for example, during adapter initialization).
    if (resservGetTlsCallContext() == NULL)
    {
        return NV_OK;
    }

    pDevice = vgpuGetCallingContextDevice(pGpu);
    if (pDevice == NULL)
    {
        // There are several places where this function can be called without TLS call context
        // in SRIOV-heavy mode. Return error only for SRIOV-full.
            NV_ASSERT_OR_RETURN(!gpuIsSriovEnabled(pGpu) || IS_SRIOV_HEAVY(pGpu), NV_ERR_OBJECT_NOT_FOUND);
        return NV_OK;
    }

    if (pDevice->deviceAllocFlags & NV_DEVICE_ALLOCATION_FLAGS_PLUGIN_CONTEXT)
    {
        if (!RMCFG_FEATURE_PLATFORM_GSP)
            NV_ASSERT_OR_RETURN(pDevice->pKernelHostVgpuDevice, NV_ERR_INVALID_STATE);

        *pIsCallingContextPlugin = NV_TRUE;
    }

    return NV_OK;
}
