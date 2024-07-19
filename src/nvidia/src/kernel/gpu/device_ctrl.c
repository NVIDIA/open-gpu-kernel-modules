/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief This module contains the gpu control interfaces for the
 *        device (NV01_DEVICE_0) class. Device-level control calls
 *        are broadcasted to all GPUs within the device.
 */

#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "kernel/virtualization/kernel_vgpu_mgr.h"
#include "vgpu/rpc.h"

#if defined(NV_UNIX)
#include "os-interface.h"
#endif


//
// This rmctrl MUST NOT touch hw since it's tagged as NO_GPUS_ACCESS in ctrl0080.def
// RM allow this type of rmctrl to go through when GPU is not available.
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdGpuGetClasslist_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *pClassListParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    return gpuGetClassList(pGpu, &pClassListParams->numClasses,
                           NvP64_VALUE(pClassListParams->classList), ENG_INVALID);
}

//
// This rmctrl MUST NOT touch hw since it's tagged with flag NO_GPUS_ACCESS in device.h
// RM allow this type of rmctrl to go through when GPU is not available.
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdGpuGetClasslistV2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *pClassListParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pClassListParams->numClasses = NV0080_CTRL_GPU_CLASSLIST_MAX_SIZE;

    return gpuGetClassList(pGpu, &pClassListParams->numClasses,
                           pClassListParams->classList, ENG_INVALID);
}

//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdGpuGetNumSubdevices_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *pSubDeviceCountParams
)
{
    pSubDeviceCountParams->numSubDevices = 1;

    return NV_OK;
}

NV_STATUS
deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NvU32 gpuMask, index;
    NvBool bEnable;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
    OBJGPU *pTmpGpu;

    if (NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_ENABLED ==
            pParams->newState)
    {
        bEnable = NV_TRUE;
    }
    else if (NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_DISABLED ==
            pParams->newState)
    {
        bEnable = NV_FALSE;
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the gpuMask for the device pGpu belongs to
    gpuMask = gpumgrGetGpuMask(pGpu);

    index = 0;
    while ((pTmpGpu = gpumgrGetNextGpu(gpuMask, &index)) != NULL)
    {
        if (bEnable)
        {
            pGpuMgr->persistentSwStateGpuMask |= NVBIT(pTmpGpu->gpuInstance);
            pTmpGpu->setProperty(pTmpGpu, PDB_PROP_GPU_PERSISTENT_SW_STATE,
                    NV_TRUE);
        }
        else
        {
            pGpuMgr->persistentSwStateGpuMask &= ~NVBIT(pTmpGpu->gpuInstance);
            pTmpGpu->setProperty(pTmpGpu, PDB_PROP_GPU_PERSISTENT_SW_STATE,
                    NV_FALSE);
        }

        // Set/Clear OS-specific persistence flags
        osModifyGpuSwStatePersistence(pTmpGpu->pOsGpuInfo, bEnable);
    }

    return NV_OK;
}

NV_STATUS
deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_PERSISTENT_SW_STATE))
    {
        pParams->swStatePersistence =
            NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_ENABLED;
    }
    else
    {
        pParams->swStatePersistence =
            NV0080_CTRL_GPU_SW_STATE_PERSISTENCE_DISABLED;
    }

    return NV_OK;
}

/*!
 * @brief   This Command is used to get the virtualization mode of GPU. GPU
 *          can be in NMOS/VGX/host-vGPU/host-vSGA mode.
 *
 * @return  Returns NV_STATUS
 *          NV_OK                     If GPU is present.
 *          NV_ERR_INVALID_ARGUMENT   If GPU is not present.
 */
NV_STATUS
deviceCtrlCmdGpuGetVirtualizationMode_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

#if defined(NV_UNIX) && !RMCFG_FEATURE_MODS_FEATURES
    pParams->isGridBuild = os_is_grid_supported();
#endif

    if (IS_VIRTUAL(pGpu))
    {
        pParams->virtualizationMode =
            NV0080_CTRL_GPU_VIRTUALIZATION_MODE_VGX;
    }
    else if (IS_PASSTHRU(pGpu))
    {
        pParams->virtualizationMode =
            NV0080_CTRL_GPU_VIRTUALIZATION_MODE_NMOS;
    }
    else if (hypervisorIsVgxHyper() && (gpuIsSriovEnabled(pGpu)
    ))
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU))
        {
            pParams->virtualizationMode =
                NV0080_CTRL_GPU_VIRTUALIZATION_MODE_HOST_VGPU;
        }
        else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA))
        {
            pParams->virtualizationMode =
                NV0080_CTRL_GPU_VIRTUALIZATION_MODE_HOST_VSGA;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "invalid virtualization Mode: %x. Returning NONE!\n",
                      pParams->virtualizationMode);

            pParams->virtualizationMode =
                NV0080_CTRL_GPU_VIRTUALIZATION_MODE_NONE;
        }
    }
    else
    {
        pParams->virtualizationMode =
            NV0080_CTRL_GPU_VIRTUALIZATION_MODE_NONE;
    }

    NV_PRINTF(LEVEL_INFO, "Virtualization Mode: %x\n",
              pParams->virtualizationMode);

    return NV_OK;
}

/*!
 * @brief   This Command issues an RPC call to the host to switch the "backdoor
 *          VNC" view to the console.
 *
 * @return  Returns NV_STATUS
 *          NV_ERR_NOT_SUPPORTED      If GPU is not present under host hypervisor.
 *          NV_OK                     If GPU is present under host hypervisor.
 *          NV_ERR_INVALID_ARGUMENT   If GPU is not present.
 *
 */
NV_STATUS
deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL
(
    Device *pDevice
)
{
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pDevice);
    NV_STATUS status = NV_OK;

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_RM_RPC_SWITCH_TO_VGA(pGpu, status);
    return status;
}

/*!
 * @brief   This Command is used to get GPU SRIOV capabilities
 *
 * @return NV_OK
 */
NV_STATUS
deviceCtrlCmdGpuGetSriovCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    return gpuGetSriovCaps_HAL(pGpu, pParams);
}

/*!
 * @brief   This command is used to find a subdevice handle by subdeviceinst
 */
NV_STATUS
deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *pParams
)
{
    NV_STATUS       status;
    Subdevice      *pSubdevice;

    status = subdeviceGetByInstance(RES_GET_CLIENT(pDevice),
                                    RES_GET_HANDLE(pDevice),
                                    pParams->subDeviceInst,
                                    &pSubdevice);

    if (status == NV_OK)
    {
        pParams->hSubDevice = RES_GET_HANDLE(pSubdevice);
    }

    return status;
}

NV_STATUS
deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams
)
{
    OBJGPU                  *pGpu           = GPU_RES_GET_GPU(pDevice);
    OBJSYS                  *pSys           = SYS_GET_INSTANCE();
    KernelVgpuMgr           *pKernelVgpuMgr = SYS_GET_KERNEL_VGPUMGR(pSys);
    NvU32                    pgpuIndex;
    KERNEL_PHYS_GPU_INFO    *pPgpuInfo;
    NvU32                    i;
    VGPU_TYPE               *pVgpuTypeInfo;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Call not supported with SMC enabled\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrGetPgpuIndex(pKernelVgpuMgr, pGpu->gpuId, &pgpuIndex));

    pPgpuInfo = &pKernelVgpuMgr->pgpuInfo[pgpuIndex];

    if (pPgpuInfo->heterogeneousTimesliceSizesSupported == NV_FALSE)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU does not support heterogenenous vGPU mode\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pPgpuInfo->numActiveVgpu != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to set heterogeneous vGPU mode as vGPU instance is active\n");
        return NV_ERR_IN_USE;
    }

    kvgpumgrSetVgpuType(pGpu, pPgpuInfo, NVA081_CTRL_VGPU_CONFIG_INVALID_TYPE);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE, pParams->bHeterogeneousMode);

    if (pParams->bHeterogeneousMode)
    {
        for (i = 0; i < pPgpuInfo->numVgpuTypes; i++)
        {
            pVgpuTypeInfo = pPgpuInfo->vgpuTypes[i];

            if (pVgpuTypeInfo == NULL)
                continue;

            /*
             * When heterogeneous vGPU mode is enabled, initially all
             * supported placement IDs are creatable placement IDs
             */
            portMemCopy(pPgpuInfo->creatablePlacementIds[i],
                        sizeof(pPgpuInfo->creatablePlacementIds[i]),
                        pVgpuTypeInfo->supportedPlacementIds,
                        sizeof(pVgpuTypeInfo->supportedPlacementIds));
        }

        /* No vGPU instances running, so placement region is empty. */
        bitVectorClrAll(&pPgpuInfo->usedPlacementRegionMap);
    }

    return NV_OK;
}

NV_STATUS
deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu != NULL, NV_ERR_INVALID_ARGUMENT);

    pParams->bHeterogeneousMode = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_VGPU_HETEROGENEOUS_MODE);

    return NV_OK;
}

/*!
 * @brief Get the GPU's sparse texture compute mode setting information.
 *
 * This setting indicates how the RM should set the large page size for the
 * GPU, based on which use case it should optimize for.
 *
 * @param[in, out] pModeParams      Pointer to struct of user params.
 *                 defaultSetting:  The default use case to optimize for on this
 *                                  GPU.
 *                 currentSetting:  The use case that the large page size was
 *                                  optimized for on this GPU, on the last driver
 *                                  load.
 *                 pendingSetting:  The use case that the large page size will
 *                                  be optimized for on this GPU, on the next
 *                                  driver reload.
 *
 * @returns NV_STATUS
 *          NV_OK                   Success
 */
NV_STATUS
deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams
)
{
    NV_STATUS status;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pDevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    status = gpuGetSparseTextureComputeMode(pGpu,
                                           &pModeParams->defaultSetting,
                                           &pModeParams->currentSetting,
                                           &pModeParams->pendingSetting);

    return status;
}

/*!
 * @brief Set the GPU's sparse texture compute mode setting to apply on the
 *        next driver load.
 *
 * This setting indicates how the RM should set the large page size for the
 * GPU, based on which use case it should optimize for.
 *
 * @param[in, out] pModeParams      Pointer to struct of user params.
 *                 setting:         The use case that the large page size should
 *                                  be optimized for on this GPU, on the next
 *                                  driver reload.
 *
 * @returns NV_STATUS
 *          NV_ERR_INVALID_ARGUMENT The specified setting is invalid
 *          NV_OK                   Success
 */
NV_STATUS
deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pDevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    //
    // In SLI, both GPUs will have the same setting for sparse texture/compute
    // mode. Individual toggling is not allowed.
    //
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        status = gpuSetSparseTextureComputeMode(pGpu, pModeParams->setting);
    }
    SLI_LOOP_END

    return status;
}

/*!
 * @brief Get the GPU's VGX capability depending upon state of VGX hardware fuse.
 *
 * @returns NV_STATUS
 *          NV_OK                   Success
 */
NV_STATUS
deviceCtrlCmdGpuGetVgxCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    pParams->isVgx = gpuIsVgxBranded(pGpu);

    return NV_OK;
}

/*!
 * @brief Get the GPU's branding information.
 *
 * @returns NV_STATUS
 *          NV_OK                   Success
 */
NV_STATUS
deviceCtrlCmdGpuGetBrandCaps_VF
(
    Device *pDevice,
    NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        NV_STATUS     status        = NV_OK;
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NvU32         gpuMask       = 0;

        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                  GPU_LOCK_GRP_DEVICE,
                                  GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                                  RM_LOCK_MODULES_GPU,
                                  &gpuMask));

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

        return status;
    }

    return NV_ERR_NOT_SUPPORTED;
}
