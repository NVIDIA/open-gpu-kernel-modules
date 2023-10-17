/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *        subdevice (NV20_SUBDEVICE_0) class. Subdevice-level control calls
 *        are directed unicast to the associated GPU.
 *        File contains ctrls related to general GPU
 */

#include "core/core.h"
#include "core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_db.h"
#include "nvrm_registry.h"
#include "nvVer.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/mc/kernel_mc.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/gpu_fabric_probe.h"
#include "objtmr.h"
#include "platform/chipset/chipset.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "gpu/mem_sys/kern_mem_sys.h"

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "rmapi/resource_fwd_decls.h"
#include "rmapi/client.h"

#include "class/cl900e.h"



static NV_STATUS
getGpuInfos(Subdevice *pSubdevice, NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pParams, NvBool bCanAccessHw)
{
    OBJGPU *pGpu         = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status     = NV_OK;
    NvU32 i              = 0;
    NvU32 data           = 0;
    NvBool bGspForward = NV_FALSE;

    // bit to set when telling GSP to fill in an info entry
    const NvU32 indexForwardToGsp = 0x80000000;

    if ((pParams->gpuInfoListSize > NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE) ||
        (pParams->gpuInfoListSize == 0))
    {
       return NV_ERR_INVALID_ARGUMENT;
    }

    for (i = 0; i < pParams->gpuInfoListSize; i++)
    {
        if (pParams->gpuInfoList[i].index >= NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        switch (pParams->gpuInfoList[i].index)
        {
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY:
            {
                KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

                if (kbusIsFlaSupported(pKernelBus))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_FLA_CAPABILITY_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_MINOR_REVISION_EXT:
            {
                data = gpuGetChipMinExtRev(pGpu);
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_NETLIST_REV0:
            {
                data = 0;
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_NETLIST_REV1:
            {
                data = 0;
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_SYSMEM_ACCESS:
            {
                data = NV2080_CTRL_GPU_INFO_SYSMEM_ACCESS_YES;
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GEMINI_BOARD:
            {
                data = !!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_GEMINI);
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_SURPRISE_REMOVAL_POSSIBLE:
            {
                OBJSYS *pSys = SYS_GET_INSTANCE();
                OBJCL  *pCl  = SYS_GET_CL(pSys);
                data = !!pCl->getProperty(pCl, PDB_PROP_CL_IS_EXTERNAL_GPU);
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_IBMNPU_RELAXED_ORDERING:
            {
                NvBool mode = NV_FALSE;
                data = NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_UNSUPPORTED;

                if (osGetIbmnpuRelaxedOrderingMode(pGpu->pOsGpuInfo, &mode) == NV_OK)
                {
                    data = NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_DISABLED;

                    if (mode)
                    {
                        data = NV2080_CTRL_GPU_INFO_IBMNPU_RELAXED_ORDERING_ENABLED;
                    }
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED:
            {
                if (gpuIsGlobalPoisonFuseEnabled(pGpu))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GLOBAL_POISON_FUSE_ENABLED_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_NVSWITCH_PROXY_DETECTED:
            {
                NV_CHECK_OR_ELSE(LEVEL_WARNING, bCanAccessHw,
                    { data = 0; status = NV_ERR_INVALID_ARGUMENT; break; });

                KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

                data = NV2080_CTRL_GPU_INFO_NVSWITCH_PROXY_DETECTED_NO;

                if (pKernelNvlink != NULL &&
                    knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink))
                {
                    data = NV2080_CTRL_GPU_INFO_NVSWITCH_PROXY_DETECTED_YES;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_SMC_MODE:
            {
                NV_CHECK_OR_ELSE(LEVEL_WARNING, bCanAccessHw,
                    { data = 0; status = NV_ERR_INVALID_ARGUMENT; break; });

                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
                NV2080_CTRL_INTERNAL_GPU_GET_SMC_MODE_PARAMS params;

                if (IS_VIRTUAL(pGpu))
                {
                    data = IS_MIG_ENABLED(pGpu) ?
                        NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLED :
                        NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLED;
                    break;
                }

                portMemSet(&params, 0x0, sizeof(params));
                status = pRmApi->Control(pRmApi,
                                         pGpu->hInternalClient,
                                         pGpu->hInternalSubdevice,
                                         NV2080_CTRL_CMD_INTERNAL_GPU_GET_SMC_MODE,
                                         &params,
                                         sizeof(params));
                data = params.smcMode;
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_SPLIT_VAS_MGMT_SERVER_CLIENT_RM:
            {
                if (gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
                {
                    data = NV2080_CTRL_GPU_INFO_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_SM_VERSION:
            {
                KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

                if ((pKernelGraphicsManager == NULL) ||
                    !kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized ||
                    (kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo == NULL))
                {
                    NV_PRINTF(LEVEL_ERROR, "Unable to retrieve SM version!\n");
                    data = NV2080_CTRL_GR_INFO_SM_VERSION_NONE;
                    status = NV_ERR_INVALID_STATE;
                }
                else
                {
                    data = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_SM_VERSION].data;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM:
            {
                KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

                if (kfifoIsPerRunlistChramEnabled(pKernelFifo))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM_ENABLED;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_PER_RUNLIST_CHANNEL_RAM_DISABLED;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY:
            {
                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_ATS_CAPABILITY_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_NVENC_STATS_REPORTING_STATE:
            {
                data = NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_NOT_SUPPORTED;
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED:
            {
                if (pGpu->bNeed4kPageIsolation)
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_4K_PAGE_ISOLATION_REQUIRED_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_DISPLAY_ENABLED:
            {
                if (GPU_GET_KERNEL_DISPLAY(pGpu) != NULL)
                {
                    data = NV2080_CTRL_GPU_INFO_DISPLAY_ENABLED_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_DISPLAY_ENABLED_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED:
            {
                if (IsMobile(pGpu))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_MOBILE_CONFIG_ENABLED_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_PROFILING_CAPABILITY:
            {
                {
                    // Always return ENABLED for Baremetal/Host
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_PROFILING_CAPABILITY_ENABLED;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_DEBUGGING_CAPABILITY:
            {
                {
                    // Always return ENABLED for Baremetal/Host
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_DEBUGGING_CAPABILITY_ENABLED;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY:
            {
                MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
                if (memmgrIsLocalEgmEnabled(pMemoryManager))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY_YES;
                    data = FLD_SET_DRF_NUM(2080_CTRL_GPU_INFO, _INDEX_GPU_LOCAL_EGM, _PEERID, pMemoryManager->localEgmPeerId, data);
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_LOCAL_EGM_CAPABILITY_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY:
            {
                KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

                if (gpuIsSelfHosted(pGpu) &&
                    pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_GPU_SELF_HOSTED_CAPABILITY_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU:
            {
                if (gpuGetChipInfo(pGpu) && gpuGetChipInfo(pGpu)->isCmpSku)
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU_YES;
                }
                else
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_CMP_SKU_NO;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY:
            {
                data = NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY_NO;

                if (osDmabufIsSupported() &&
                    (!IS_VIRTUAL(pGpu)) &&
                    (!gpuIsApmFeatureEnabled(pGpu)) &&
                    (!NVCPU_IS_PPC64LE))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_DMABUF_CAPABILITY_YES;
                }
                break;
            }
            case NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED:
            {
                data = NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED_NO;

                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED))
                {
                    data = NV2080_CTRL_GPU_INFO_INDEX_IS_RESETLESS_MIG_SUPPORTED_YES;
                }
                break;
            }
            default:
            {
                // Only forward to GSP if we're in the HW-access-enabled control
                if (IS_GSP_CLIENT(pGpu) && bCanAccessHw)
                {
                    pParams->gpuInfoList[i].index |= indexForwardToGsp;
                    bGspForward = NV_TRUE;
                }
                else
                {
                    data = 0;
                    status = NV_ERR_INVALID_ARGUMENT;
                }
                break;
            }
        }

        // save off data value
        pParams->gpuInfoList[i].data = data;
    }

    if (IS_GSP_CLIENT(pGpu) && bGspForward && (status == NV_OK))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               RES_GET_CLIENT_HANDLE(pSubdevice),
                               RES_GET_HANDLE(pSubdevice),
                               NV2080_CTRL_CMD_GPU_GET_INFO_V2,
                               pParams, sizeof(*pParams));
    }

    return status;
}

NV_STATUS
subdeviceCtrlCmdGpuGetInfoV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams
)
{
    return getGpuInfos(pSubdevice, pGpuInfoParams, NV_TRUE);
}

//
// subdeviceCtrlCmdGpuGetCachedInfo: As subdeviceCtrlCmdGpuGetInfoV2, except
// does not perform any HW access (NO_GPUS_ACCESS and NO_GPUS_LOCK flags)
//
NV_STATUS
subdeviceCtrlCmdGpuGetCachedInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pGpuInfoParams
)
{
    return getGpuInfos(pSubdevice, pGpuInfoParams, NV_FALSE);
}

static POBJHWBC
getBridgeObject(OBJHWBC *pHWBC, NvU32 hwbcId)
{
    OBJHWBC *pBridgeObject = NULL;
    if (NULL != pHWBC)
    {
        if (hwbcId == pHWBC->hwbcId)
        {
            pBridgeObject = pHWBC;
        }
        else
        {
            pBridgeObject = getBridgeObject(pHWBC->pSibling, hwbcId);
            if (NULL == pBridgeObject)
            {
                pBridgeObject = getBridgeObject(pHWBC->pFirstChild, hwbcId);
            }
        }
    }
    return pBridgeObject;
}

static NV_STATUS
getPlxFirmwareAndBusInfo
(
    OBJHWBC *pHWBC,
    NvU32 *domainId,
    NvU8  *busId,
    NvU8  *deviceId,
    NvU8  *funcId,
    NvU32 *fwVersion,
    NvU8  *oemVersion,
    NvU8  *siliconRevision,
    NvU8  *bcRes
)
{
    if (NULL == pHWBC)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (domainId)
        *domainId           = pHWBC->ctrlDev.domain;
    if (busId)
        *busId              = pHWBC->ctrlDev.bus;
    if (deviceId)
        *deviceId           = pHWBC->ctrlDev.device;
    if (funcId)
        *funcId             = pHWBC->ctrlDev.func;
    if (fwVersion)
        *fwVersion          = pHWBC->fwVersion;
    if (oemVersion)
        *oemVersion         = pHWBC->fwOemVersion;
    if (siliconRevision)
        *siliconRevision    = pHWBC->plxRevision;
    if (bcRes)
        *bcRes              = (NvU8)pHWBC->bcRes;
    return NV_OK;
}

static NV_STATUS
getPlxFirmwareVersion
(
    NvU32 hwbcId,
    NvU32 *fwVersion,
    NvU8  *oemVersion,
    NvU8  *siliconRevision,
    NvU8  *bcRes
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl  = SYS_GET_CL(pSys);
    OBJHWBC *pHWBC = getBridgeObject(pCl->pHWBC, hwbcId);

    return getPlxFirmwareAndBusInfo(pHWBC, NULL, NULL, NULL, NULL, fwVersion,
                                    oemVersion, siliconRevision, bcRes);
}

static NvU8
getBridgeCountAndId(OBJHWBC *pHWBC, NvU32 pBridgeId[], NvU32 *bridgeIndex)
{
    NvU8 count = 0;
    if ((NULL == bridgeIndex) ||
        (*bridgeIndex >= NV2080_CTRL_MAX_PHYSICAL_BRIDGE))
    {
        return count;
    }
    if (NULL != pHWBC)
    {
        if ((HWBC_PLX_PEX8747 == pHWBC->bcRes) || (HWBC_NVIDIA_BR04 == pHWBC->bcRes))
        {
            pBridgeId[*bridgeIndex] = pHWBC->hwbcId;
            (*bridgeIndex)++;
            count++;
        }
        count += getBridgeCountAndId(pHWBC->pSibling, pBridgeId, bridgeIndex);
        count += getBridgeCountAndId(pHWBC->pFirstChild, pBridgeId, bridgeIndex);
    }
    return count;
}

static NV_STATUS
getBridgeData
(
    NvU8 *pPlxCount,
    NvU32 pBridgeId[]
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl  = SYS_GET_CL(pSys);
    NvU32 bridgeIndex = 0;

    if (NULL == pPlxCount)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pPlxCount = getBridgeCountAndId(pCl->pHWBC, pBridgeId, &bridgeIndex);
    NV_ASSERT_OR_RETURN(*pPlxCount < NV2080_CTRL_MAX_PHYSICAL_BRIDGE,
                        NV_ERR_OUT_OF_RANGE);
    return NV_OK;
}

static NV_STATUS
getUpstreamBridgeIds
(
    OBJGPU *pGpu,
    NvU8 *pPlxCount,
    NvU32 pBridgeId[]
)
{
    HWBC_LIST *pGpuHWBCList;
    NvU8 bridgeIndex = 0;

    if (NULL == pPlxCount)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpuHWBCList = pGpu->pHWBCList;
    while(pGpuHWBCList)
    {
        NV_ASSERT_OR_RETURN(pGpuHWBCList->pHWBC != NULL, NV_ERR_INVALID_POINTER);
        pBridgeId[bridgeIndex] = pGpuHWBCList->pHWBC->hwbcId;
        pGpuHWBCList = pGpuHWBCList->pNext;
        bridgeIndex++;
        NV_ASSERT_OR_RETURN(bridgeIndex < NV2080_CTRL_MAX_PHYSICAL_BRIDGE,
                            NV_ERR_OUT_OF_RANGE);
    }
    *pPlxCount = bridgeIndex;

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuGetPhysicalBridgeVersionInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PHYSICAL_BRIDGE_VERSION_INFO_PARAMS *pBridgeInfoParams
)
{
    NV_STATUS status = NV_OK;
    NvU8      bridgeIndex;
    status = getBridgeData(&pBridgeInfoParams->bridgeCount,
                            pBridgeInfoParams->hPhysicalBridges);
    if (status == NV_OK)
    {
        NV2080_CTRL_GPU_PHYSICAL_BRIDGE_VERSION_PARAMS *pBridgeVersionParams =
                                                    pBridgeInfoParams->bridgeList;
        for (bridgeIndex = 0;
             bridgeIndex < pBridgeInfoParams->bridgeCount;
             bridgeIndex++)
        {
            status = getPlxFirmwareVersion(pBridgeInfoParams->hPhysicalBridges[bridgeIndex],
                                            &pBridgeVersionParams->fwVersion,
                                            &pBridgeVersionParams->oemVersion,
                                            &pBridgeVersionParams->siliconRevision,
                                            &pBridgeVersionParams->hwbcResourceType);
            if (status != NV_OK)
            {
                break;
            }
            pBridgeVersionParams++;
        }
    }
    return status;
}

NV_STATUS
subdeviceCtrlCmdGpuGetAllBridgesUpstreamOfGpu_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ALL_BRIDGES_UPSTREAM_OF_GPU_PARAMS *pBridgeInfoParams
)
{
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS  status = NV_OK;
    NvU8       bridgeIndex;
    HWBC_LIST *pGpuHWBCList;
    status = getUpstreamBridgeIds(pGpu,
                                  &pBridgeInfoParams->bridgeCount,
                                  pBridgeInfoParams->physicalBridgeIds);
    if (status == NV_OK)
    {
        NV2080_CTRL_GPU_BRIDGE_VERSION_PARAMS *pBridgeVersionParams =
                                                pBridgeInfoParams->bridgeList;
        pGpuHWBCList = pGpu->pHWBCList;
        for (bridgeIndex = 0;
             bridgeIndex < pBridgeInfoParams->bridgeCount && pGpuHWBCList;
             bridgeIndex++)
        {
            status = getPlxFirmwareAndBusInfo(pGpuHWBCList->pHWBC,
                                              &pBridgeVersionParams->domain,
                                              &pBridgeVersionParams->bus,
                                              &pBridgeVersionParams->device,
                                              &pBridgeVersionParams->func,
                                              &pBridgeVersionParams->fwVersion,
                                              &pBridgeVersionParams->oemVersion,
                                              &pBridgeVersionParams->siliconRevision,
                                              &pBridgeVersionParams->hwbcResourceType);
            if (status != NV_OK)
            {
                break;
            }
            pGpuHWBCList = pGpuHWBCList->pNext;
            pBridgeVersionParams++;
        }
    }
    return status;
}

/*!
 * @brief This command can be used for Optimus enabled system.
 *
 * @return :
 *    NV_OK
 */
NV_STATUS
subdeviceCtrlCmdGpuSetOptimusInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_OPTIMUS_INFO_PARAMS *pGpuOptimusInfoParams
)
{
    NvU32   status  =   NV_OK;
    OBJGPU *pGpu    =   GPU_RES_GET_GPU(pSubdevice);

    if (pGpuOptimusInfoParams->isOptimusEnabled)
    {
        //
        // Setting pMemoryManager->bPersistentStandbyBuffer for Optimus system.
        // It is used for sys_mem allocation which is pinned across
        // S3 transitions.Sys_mem allocations are done at first S3 cycle
        // and release during driver unload, which reduces system
        // VM fragmentation, which was a problem in optimus system.
        // For more details refer bug 754122.
        //
        GPU_GET_MEMORY_MANAGER(pGpu)->bPersistentStandbyBuffer = NV_TRUE;
    }
    return status;
}

// RM reports dynamic encoder capacity as a percentage (0-100) of the encoders fixed
// capacity. Fixed capacity is readable via NvEncode API and is defined in
// drivers/video/encode/src/CNVVAEncoder.cpp#200
//
// Dynamic capacity of 0x0 indicates that encoder performance may be minimal for this
// GPU and software should fall back to CPU-based encode.
//

#define NV_ENC_CAPACITY_MAX_VALUE          100
//
// subdeviceCtrlCmdGpuGetEncoderCapacity
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEncoderCapacity_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_PARAMS *pEncoderCapacityParams
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu    =   GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (pEncoderCapacityParams->queryType == NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_AV1)
    {
        if (pGpu->bGpuNvEncAv1Supported == NV_FALSE)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    else if ((pEncoderCapacityParams->queryType != NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_H264) &&
             (pEncoderCapacityParams->queryType != NV2080_CTRL_GPU_GET_ENCODER_CAPACITY_HEVC))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pEncoderCapacityParams->encoderCapacity = NV_ENC_CAPACITY_MAX_VALUE;

    return rmStatus;
}

//
// subdeviceCtrlCmdGpuGetNvencSwSessionStats
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetNvencSwSessionStats_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_STATS_PARAMS *pParams
)
{
    pParams->encoderSessionCount = 0;
    pParams->averageEncodeFps = 0;
    pParams->averageEncodeLatency = 0;

    return NV_OK;
}

NV_STATUS
_subdeviceCtrlCmdGpuGetNvencSwSessionInfo
(
    OBJGPU                              *pGpu,
    NvU32                               sessionInfoTblEntry,
    NV2080_CTRL_NVENC_SW_SESSION_INFO   *pSessionInfo,
    NvU32                               *entryCount
)
{

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetNvencSwSessionInfo
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//

NV_STATUS
subdeviceCtrlCmdGpuGetNvencSwSessionInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_PARAMS *pParams
)
{
    NV_STATUS               status = NV_OK;
    pParams->sessionInfoTblEntry = 0;

    return status;
}

NV_STATUS
subdeviceCtrlCmdGpuGetNvencSwSessionInfoV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NVENC_SW_SESSION_INFO_V2_PARAMS *pParams
)
{
    NV_STATUS               status = NV_OK;
    pParams->sessionInfoTblEntry = 0;

    return status;
}

//
// subdeviceCtrlCmdGpuGetNvfbcSwSessionStats
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetNvfbcSwSessionStats_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_STATS_PARAMS *pParams
)
{
    pParams->sessionCount   = 0;
    pParams->averageFPS     = 0;
    pParams->averageLatency = 0;

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//

NV_STATUS
subdeviceCtrlCmdGpuGetNvfbcSwSessionInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NVFBC_SW_SESSION_INFO_PARAMS *pParams
)
{
    pParams->sessionInfoCount = 0;

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetSdm
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetSdm_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_SDM_PARAMS *pSdmParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pSdmParams->subdeviceMask = gpuGetSubdeviceMask(pGpu);

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuSetSdm
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuSetSdm_IMPL
(
    Subdevice* pSubdevice,
    NV2080_CTRL_GPU_SET_SDM_PARAMS* pSdmParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvU32   subdeviceInstance;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (!ONEBITSET(pSdmParams->subdeviceMask))
    {
        NV_PRINTF(LEVEL_ERROR, "Subdevice mask has none or more than one bit set");
        return NV_ERR_INVALID_DATA;
    }

    if (gpuIsStateLoaded(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "NV2080_CTRL_CMD_GPU_SET_SDM cannot be called after the GPU is loaded");
        return NV_ERR_INVALID_STATE;
    }
    subdeviceInstance = BIT_IDX_32(pSdmParams->subdeviceMask);

    if (subdeviceInstance >= NV_MAX_SUBDEVICES)
    {
        NV_PRINTF(LEVEL_ERROR, "Subdevice mask exceeds the max count of subdevices");
        return NV_ERR_INVALID_DATA;
    }
    pGpu->subdeviceInstance = subdeviceInstance;

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetSimulationInfo
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetSimulationInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS *pGpuSimulationInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (IS_SILICON(pGpu))
    {
        pGpuSimulationInfoParams->type = NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_NONE;
    }
    else
    {
        pGpuSimulationInfoParams->type = NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_UNKNOWN;
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetEngines
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEngines_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINES_PARAMS *pParams
)
{
    NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS getEngineParamsV2;
    NvU32    *pKernelEngineList = NvP64_VALUE(pParams->engineList);
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    portMemSet(&getEngineParamsV2, 0, sizeof(getEngineParamsV2));

    status = subdeviceCtrlCmdGpuGetEnginesV2(pSubdevice, &getEngineParamsV2);
    NV_CHECK_OR_RETURN(LEVEL_INFO, NV_OK == status, status);

    // NULL clients just want an engine count
    if (NULL != pKernelEngineList)
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, pParams->engineCount >= getEngineParamsV2.engineCount,
                         NV_ERR_BUFFER_TOO_SMALL);
        portMemCopy(pKernelEngineList,
                    getEngineParamsV2.engineCount * sizeof(*getEngineParamsV2.engineList), getEngineParamsV2.engineList,
                    getEngineParamsV2.engineCount * sizeof(*getEngineParamsV2.engineList));
    }

    pParams->engineCount = getEngineParamsV2.engineCount;

    return status;
}

//
// subdeviceCtrlCmdGpuGetEnginesV2
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEnginesV2_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *pEngineParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // Update the engine Database
    NV_ASSERT_OK_OR_RETURN(gpuUpdateEngineTable(pGpu));

    // Validate engine count
    if (pGpu->engineDB.size > NV2080_GPU_MAX_ENGINES_LIST_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "The engine database's size (0x%x) exceeds "
                  "NV2080_GPU_MAX_ENGINES_LIST_SIZE (0x%x)!\n",
                  pGpu->engineDB.size, NV2080_GPU_MAX_ENGINES_LIST_SIZE);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    {
        // Need this null check in case object doesn't exist when using Orin trimmed profile
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        if (pKernelMIGManager != NULL)
        {
            RM_ENGINE_TYPE rmEngineTypeList[NV2080_GPU_MAX_ENGINES_LIST_SIZE];

            // Filter engines based on current partitioning scheme
            status = kmigmgrFilterEngineList(pGpu,
                                             pKernelMIGManager,
                                             pSubdevice,
                                             rmEngineTypeList,
                                             &pEngineParams->engineCount);

            if (status == NV_OK)
            {
                // Convert the RM_ENGINE_TYPE list to NV2080_ENGINE_TYPE list
                gpuGetNv2080EngineTypeList(rmEngineTypeList,
                                           pEngineParams->engineCount,
                                           pEngineParams->engineList);
            }
        }
    }

    return status;
}

//
// subdeviceCtrlCmdGpuGetEngineClasslist
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEngineClasslist_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_CLASSLIST_PARAMS *pClassParams
)
{
    OBJGPU       *pGpu = GPU_RES_GET_GPU(pSubdevice);
    ENGDESCRIPTOR engDesc;
    NV_STATUS     status = NV_OK;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(pClassParams->engineType);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    {
        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, GPU_RES_GET_DEVICE(pSubdevice), &ref));

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                    rmEngineType,
                                                    &rmEngineType));
        }
    }

    status = gpuXlateClientEngineIdToEngDesc(pGpu, rmEngineType, &engDesc);

    NV_ASSERT(status == NV_OK);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST Invalid engine ID 0x%x\n",
                  pClassParams->engineType);
        DBG_BREAKPOINT();
        return status;
    }

    status = gpuGetClassList(pGpu, &pClassParams->numClasses, NvP64_VALUE(pClassParams->classList), engDesc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NV2080_CTRL_CMD_GPU_GET_ENGINE_CLASSLIST Class List query failed\n");
    }

    return status;
}

//
// subdeviceCtrlCmdGpuGetEnginePartnerList
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEnginePartnerList_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pPartnerListParams
)
{
    OBJGPU          *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    ENGDESCRIPTOR    engDesc;
    NvU32            localNv2080EngineType;
    RM_ENGINE_TYPE   rmEngineType;
    NvU32            i;
    PCLASSDESCRIPTOR pClass;
    NV_STATUS        status = NV_OK;

    pPartnerListParams->numPartners = 0;

    rmEngineType = gpuGetRmEngineType(pPartnerListParams->engineType);

    status = gpuXlateClientEngineIdToEngDesc(pGpu, rmEngineType, &engDesc);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid engine ID 0x%x (0x%x)\n",
                  pPartnerListParams->engineType, rmEngineType);
        return status;
    }

    // find class in class db
    status = gpuGetClassByClassId(pGpu, pPartnerListParams->partnershipClassId, &pClass);
    if (NV_OK != status)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid class ID 0x%x\n",
                  pPartnerListParams->partnershipClassId);
        return status;
    }

    // Make sure that the engine related to this class is FIFO...
    if (pClass->engDesc != ENG_KERNEL_FIFO)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Class 0x%x is not considered a partnership class.\n",
                  pPartnerListParams->partnershipClassId);
        return NV_ERR_NOT_SUPPORTED;
    }

    localNv2080EngineType = pPartnerListParams->engineType;

    // Translate the instance-local engine type to the global engine type in MIG mode
    if (IS_MIG_IN_USE(pGpu))
    {
        Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &rmEngineType));

        pPartnerListParams->engineType = gpuGetNv2080EngineType(rmEngineType);
    }

    // See if the hal wants to handle this
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    status = kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, pPartnerListParams);

    // Restore the client's passed engineType
    pPartnerListParams->engineType = localNv2080EngineType;

    if (NV_OK == status)
    {
        goto subdeviceCtrlCmdGpuGetEnginePartnerList_filter;
    }

    //
    // For channels that the hal didn't handle, we should just return
    // all of the supported engines except for the target engine.
    //

    // Update the engine Database
    NV_ASSERT_OK_OR_RETURN(gpuUpdateEngineTable(pGpu));

    // Make sure it all will fit
    if (pGpu->engineDB.size > NV2080_CTRL_GPU_MAX_ENGINE_PARTNERS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "partnerList space is too small, time to increase. This is fatal\n");
        DBG_BREAKPOINT();
        return status;
    }

    // Copy over all of the engines except the target
    for (i = 0; i < pGpu->engineDB.size; i++)
    {
        localNv2080EngineType = gpuGetNv2080EngineType(pGpu->engineDB.pType[i]);

        // Skip the engine handed in
        if (localNv2080EngineType != pPartnerListParams->engineType )
        {
            pPartnerListParams->partnerList[pPartnerListParams->numPartners++] = localNv2080EngineType;
        }
    }

subdeviceCtrlCmdGpuGetEnginePartnerList_filter:
    if (IS_MIG_IN_USE(pGpu))
    {
        // Remove entries which don't exist in this client's GPU instance
        status = kmigmgrFilterEnginePartnerList(pGpu, pKernelMIGManager,
                                                pSubdevice,
                                                pPartnerListParams);
    }

    return status;
}

//
// subdeviceCtrlCmdGpuGetEngineFaultInfo
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetEngineFaultInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_FAULT_INFO_PARAMS *pParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS   status      = NV_OK;
    RM_ENGINE_TYPE rmEngineType = gpuGetRmEngineType(pParams->engineType);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    //
    // When MIG is enabled, clients pass in their instance-specific engineId
    // rather than physical engineId since each client only sees engines available in
    // its own instance. So we need to convert this local engineId to physical engineId
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        MIG_INSTANCE_REF ref;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &rmEngineType));
    }

    // Populate HW info for SW engine entry
    status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                      (NvU32)rmEngineType, ENGINE_INFO_TYPE_MMU_FAULT_ID,
                                      &pParams->mmuFaultId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NV2080_CTRL_CMD_GPU_GET_ENGINE_INFO failed\n");
        return status;
    }

    // Only GR engine supports subcontext faulting on Volta+ chips
    pParams->bSubcontextSupported = (RM_ENGINE_TYPE_IS_GR(rmEngineType) &&
        kfifoIsSubcontextSupported(pKernelFifo));

    return status;
}

ct_assert(NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT ==
          NV2080_CTRL_CMD_GPU_GET_PES_INFO_MAX_TPC_PER_GPC_COUNT);

//
// subdeviceCtrlCmdGpuGetFermiGpcInfo
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetFermiGpcInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS *pParams
)
{
    NV2080_CTRL_GR_GET_GPC_MASK_PARAMS gpcMaskParams;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle  hSubdevice = RES_GET_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance));

    portMemSet(&gpcMaskParams, 0, sizeof(gpcMaskParams));

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_GR_GET_GPC_MASK,
                        &gpcMaskParams,
                        sizeof(gpcMaskParams)));

    pParams->gpcMask = gpcMaskParams.gpcMask;
    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetFermiTpcInfo
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetFermiTpcInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS *pParams
)
{
    NV2080_CTRL_GR_GET_TPC_MASK_PARAMS tpcMaskParams;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle  hSubdevice = RES_GET_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance));

    portMemSet(&tpcMaskParams, 0, sizeof(tpcMaskParams));
    tpcMaskParams.gpcId = pParams->gpcId;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_GR_GET_TPC_MASK,
                        &tpcMaskParams,
                        sizeof(tpcMaskParams)));

    pParams->tpcMask = tpcMaskParams.tpcMask;
    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetFermiZcullInfo
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
// WARNING: This control call is deprecated.
//
NV_STATUS
subdeviceCtrlCmdGpuGetFermiZcullInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS *pParams
)
{
    NV2080_CTRL_GR_GET_ZCULL_MASK_PARAMS zcullMaskParams;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NvHandle  hSubdevice = RES_GET_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance));

    portMemSet(&zcullMaskParams, 0, sizeof(zcullMaskParams));
    zcullMaskParams.gpcId = pParams->gpcId;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_GR_GET_ZCULL_MASK,
                        &zcullMaskParams,
                        sizeof(zcullMaskParams)));

    pParams->zcullMask = zcullMaskParams.zcullMask;

    return NV_OK;
}

/*!
 * @brief Get graphics engine PES configuration
 *
 * This can be called before floor sweeping is determined, so we cannot use cached
 * values.
 */
NV_STATUS
subdeviceCtrlCmdGpuGetPesInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PES_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    RsClient *pRsClient;
    NvU32 gpcId = pParams->gpcId;
    NvU32 maxGpcCount;

    //
    // XXX Bug 2681931 - GET_PES_INFO overloads interpretation of gpcId parameter
    // This ctrl call is due for deprecation and should not be used.
    //

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OK_OR_RETURN(
        serverGetClientUnderLock(&g_resServ, hClient, &pRsClient));

    NV_CHECK_OR_RETURN(LEVEL_INFO, !IS_MIG_IN_USE(pGpu), NV_ERR_NOT_SUPPORTED);
    pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);
    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    maxGpcCount = gpuGetLitterValues_HAL(pGpu, NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS);
    if (gpcId >= maxGpcCount)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pParams->numPesInGpc = pKernelGraphicsStaticInfo->floorsweepingMasks.numPesPerGpc[gpcId];

    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelGraphicsStaticInfo->pPpcMasks != NULL, NV_ERR_NOT_SUPPORTED);
    pParams->activePesMask = pKernelGraphicsStaticInfo->pPpcMasks->mask[gpcId];

    pParams->maxTpcPerGpcCount = pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPC_PER_GPC].data;
    portMemCopy(pParams->tpcToPesMap, sizeof(pParams->tpcToPesMap),
                pKernelGraphicsStaticInfo->floorsweepingMasks.tpcToPesMap, sizeof(pKernelGraphicsStaticInfo->floorsweepingMasks.tpcToPesMap));

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuQueryMode_IMPL
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuQueryMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_QUERY_MODE_PARAMS *pQueryMode
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    switch (gpuGetMode(pGpu))
    {
        case NV_GPU_MODE_GRAPHICS_MODE:
        {
            pQueryMode->mode = NV2080_CTRL_GPU_QUERY_MODE_GRAPHICS_MODE;
            break;
        }
        case NV_GPU_MODE_COMPUTE_MODE:
        {
            pQueryMode->mode = NV2080_CTRL_GPU_QUERY_MODE_COMPUTE_MODE;
            break;
        }
        default:
        {
            pQueryMode->mode = NV2080_CTRL_GPU_QUERY_MODE_UNKNOWN_MODE;
            break;
        }
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuHandleGpuSR
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuHandleGpuSR_IMPL
(
    Subdevice *pSubdevice
)
{
    return NV_OK;
}

//
// subdeviceCtrlCmdGpuSetComputeModeRules
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuSetComputeModeRules_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_COMPUTE_MODE_RULES_PARAMS *pSetRulesParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_STATUS status = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);

        //
        // Client RM still needs to set its value and update the registry,
        // so don't return unless there was an error.
        //
        NV_ASSERT_OK_OR_RETURN(status);
    }

    //TODO Bug 2718406  will extend compute mode support for MIG
    if (IS_MIG_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    switch(pSetRulesParams->rules)
    {
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE:
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE:
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_COMPUTE_PROHIBITED:
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE_PROCESS:
            pGpu->computeModeRules = pSetRulesParams->rules;

            //
            // Store this setting in the registry so that it persists even
            // after the last client disconnects.
            // Client RM handles this so skip on GSP.
            //
            if (NV_OK !=
                osWriteRegistryDword(pGpu,
                                     NV_REG_STR_RM_COMPUTE_MODE_RULES,
                                     pGpu->computeModeRules))
            {
                // Non-fatal but worth reporting
                NV_PRINTF(LEVEL_ERROR,
                          "Could not store compute mode rule in the registry, current setting may not persist if all clients disconnect!\n");
            }
            break;

        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuQueryComputeModeRules
//
// Lock Requirements:
//      Assert that API lock held on entry
//
// TODO Bug 2718406  will extend compute mode support for MIG
//
NV_STATUS
subdeviceCtrlCmdGpuQueryComputeModeRules_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_QUERY_COMPUTE_MODE_RULES_PARAMS *pQueryRulesParams
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    {
        pQueryRulesParams->rules = pGpu->computeModeRules;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuAcquireComputeModeReservation_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    //TODO Bug 2718406  will extend compute mode support for MIG
    if (IS_MIG_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    switch (pGpu->computeModeRules)
    {
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE:
            {
                // If a GPU is in "normal" mode, then the caller can always get the reservation:
                pGpu->hComputeModeReservation = hClient;
            }
            return NV_OK;
            break; // For the Coverity code checker.

        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE:
        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_EXCLUSIVE_COMPUTE_PROCESS:
            {
                //
                // If a GPU is in "cuda exclusive" mode, then the caller can only get the
                // reservation if no other client holds the reservation:
                //
                if (NV01_NULL_OBJECT == pGpu->hComputeModeReservation)
                {
                    pGpu->hComputeModeReservation = hClient;
                    return NV_OK;
                }
                else
                {
                    // Someone else holds the reservation:
                    return NV_ERR_STATE_IN_USE;
                }
            }
            break;

        case NV2080_CTRL_GPU_COMPUTE_MODE_RULES_COMPUTE_PROHIBITED:
            //
            // If a GPU is in "cuda prohibited" mode, then the caller can never get the
            // reservation:
            //
            return NV_ERR_STATE_IN_USE;
            break;

        default:
            NV_ASSERT(0); // This *should* be unreachable code.
            break;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdGpuReleaseComputeModeReservation_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    //TODO Bug 2718406  will extend compute mode support for MIG
    if (IS_MIG_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Release the reservation ONLY IF we had the reservation to begin with. Otherwise,
    // leave it alone, because someone else has acquired it:
    if (pGpu->hComputeModeReservation == hClient)
    {
        pGpu->hComputeModeReservation = NV01_NULL_OBJECT;
    }
    else
    {
        return NV_ERR_STATE_IN_USE;
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuGetId
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetId_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ID_PARAMS *pIdParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    pIdParams->gpuId = pGpu->gpuId;

    return NV_OK;
}

//
// nv2080CtrlCmdGpuGetPids
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetPids_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PIDS_PARAMS *pGetPidsParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvU32 internalClassId;
    NV_STATUS status;
    MIG_INSTANCE_REF *pRef = NULL;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    switch (pGetPidsParams->idType)
    {
        case (NV2080_CTRL_GPU_GET_PIDS_ID_TYPE_CLASS):
        {
            if (pGetPidsParams->id == NV20_SUBDEVICE_0)
            {
                internalClassId = classId(Subdevice);
            }
            else if (pGetPidsParams->id == MPS_COMPUTE)
            {
                internalClassId = classId(MpsApi);
            }
            else
            {
                internalClassId = classId(ChannelDescendant);
            }
            break;
        }
        case (NV2080_CTRL_GPU_GET_PIDS_ID_TYPE_VGPU_GUEST):
        {
            internalClassId = classId(KernelHostVgpuDeviceApi);
            break;
        }

        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // With MIG GPU instancing enabled, get associated instance ref
    // Clients with MIG_MONITOR capability are allowed to get full device
    // info
    //
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    MIG_INSTANCE_REF partitionRef = kmigmgrMakeNoMIGReference();
    pRef = &partitionRef;
    if (IS_MIG_IN_USE(pGpu) &&
        !rmclientIsCapableByHandle(hClient, NV_RM_CAP_SYS_SMC_MONITOR))
    {
        Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        //
        // Check instanceSubscription to limit the scope of the call
        // Clients with mig-monitor capability are allowed to get full device
        // info
        //
        NV_CHECK_OR_RETURN(LEVEL_INFO, (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                                        pDevice, pRef) == NV_OK),
                         NV_ERR_INSUFFICIENT_PERMISSIONS);
    }

    //
    // Search over all clients to see if any contain objects of type = id.
    // If they do, then add their PID to the PIDArray param and also
    // return the amount of valid entries in the Array through pidTblCount.
    //
    status = gpuGetProcWithObject(pGpu, pGetPidsParams->id, internalClassId,
                                  pGetPidsParams->pidTbl, &pGetPidsParams->pidTblCount,
                                  pRef);
    return status;
}

//
// subdeviceCtrlCmdGpuGetPidInfo
//
// Lock Requirements:
//      Assert that API and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuGetPidInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_PID_INFO_PARAMS *pGetPidInfoParams
)
{
    NV2080_CTRL_GPU_PID_INFO_DATA *pPidInfoData;
    NV2080_CTRL_SMC_SUBSCRIPTION_INFO *pSmcInfo;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV2080_CTRL_GPU_PID_INFO *pPidInfo;
    NvU32 internalClassId;
    NvU32 i;
    MIG_INSTANCE_REF *pRef = NULL;
    NvBool bGlobalInfo = NV_TRUE;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if ((pGetPidInfoParams->pidInfoListCount <= 0) ||
        (pGetPidInfoParams->pidInfoListCount >
         NV2080_CTRL_GPU_GET_PID_INFO_MAX_COUNT))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // With MIG GPU instancing enabled, get associated instance ref
    // Clients with MIG_MONITOR capability are allowed to get full device
    // info
    //
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    MIG_INSTANCE_REF ref = kmigmgrMakeNoMIGReference();
    pRef = &ref;
    if (IS_MIG_IN_USE(pGpu) &&
        !rmclientIsCapableByHandle(hClient, NV_RM_CAP_SYS_SMC_MONITOR))
    {
        Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
        //
        // Check instanceSubscription to limit the scope of the call
        // Clients with mig-monitor capability are allowed to get full device
        // info
        //
        NV_CHECK_OR_RETURN(LEVEL_INFO, (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                                        pDevice, pRef) == NV_OK),
                         NV_ERR_INSUFFICIENT_PERMISSIONS);
        bGlobalInfo = NV_FALSE;
    }

    for (i = 0; i < pGetPidInfoParams->pidInfoListCount; ++i)
    {
        pPidInfo = &pGetPidInfoParams->pidInfoList[i];

        pSmcInfo = &pPidInfo->smcSubscription;
        pSmcInfo->computeInstanceId = PARTITIONID_INVALID;
        pSmcInfo->gpuInstanceId = PARTITIONID_INVALID;

        switch (pPidInfo->index)
        {
            case (NV2080_CTRL_GPU_PID_INFO_INDEX_VIDEO_MEMORY_USAGE):
            {
                internalClassId = classId(Memory);

                pPidInfoData = &pPidInfo->data;
                portMemSet(pPidInfoData, 0, sizeof(NV2080_CTRL_GPU_PID_INFO_DATA));
                pPidInfo->result = gpuFindClientInfoWithPidIterator(pGpu, pPidInfo->pid, 0,
                                                                    internalClassId,
                                                                    pPidInfoData,
                                                                    pSmcInfo,
                                                                    pRef,
                                                                    bGlobalInfo);
                break;
            }
            default:
            {
                pPidInfo->result = NV_ERR_INVALID_ARGUMENT;
                break;
            }
        }
    }

    return NV_OK;
}

// Control call to fetch the Runlist pri base for the engine(s) specified
NV_STATUS
subdeviceCtrlCmdGpuGetEngineRunlistPriBase_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_PARAMS *pParams
)
{
    NV_STATUS   status      = NV_OK;
    NV_STATUS   tmpStatus   = NV_OK;
    NvU32       i;
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!kfifoIsHostEngineExpansionSupported(pKernelFifo))
    {
       status = NV_ERR_NOT_SUPPORTED;
       return status;
    }

    for (i = 0; i < NV2080_GPU_MAX_ENGINES_LIST_SIZE; i++)
    {
        RM_ENGINE_TYPE rmEngineType;

        // Check if input is NULL or a SW engine; return a NULL value since SW engine does not have a runlist pri base
        // and this should not be returned as an error
        if ((pParams->engineList[i] == NV2080_ENGINE_TYPE_NULL) || (pParams->engineList[i] == NV2080_ENGINE_TYPE_SW))
        {
            pParams->runlistPriBase[i] = NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_NULL;
            continue;
        }

        rmEngineType = gpuGetRmEngineType(pParams->engineList[i]);

        //
        // See if MIG is enabled. If yes, then we have to convert instanceLocal
        // engine to global engine before moving ahead
        //
        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_RETURN(
                LEVEL_ERROR,
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                pDevice, &ref));

            NV_CHECK_OK_OR_RETURN(
                LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &rmEngineType));
        }

        tmpStatus = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                             rmEngineType, ENGINE_INFO_TYPE_RUNLIST_PRI_BASE,
                                             &pParams->runlistPriBase[i]);

        if (tmpStatus != NV_OK)
        {
            pParams->runlistPriBase[i] = NV2080_CTRL_GPU_GET_ENGINE_RUNLIST_PRI_BASE_ERROR;
            status = tmpStatus;
        }
    }
    return status;
}

// Control call to fetch the HW engine ID for the engine(s) specified
NV_STATUS
subdeviceCtrlCmdGpuGetHwEngineId_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_HW_ENGINE_ID_PARAMS *pParams
)
{
    NV_STATUS   status      = NV_OK;
    NV_STATUS   tmpStatus   = NV_OK;
    NvU32       i;
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!kfifoIsHostEngineExpansionSupported(pKernelFifo))
    {
       status = NV_ERR_NOT_SUPPORTED;
       return status;
    }

    for (i = 0; i < NV2080_GPU_MAX_ENGINES_LIST_SIZE; i++)
    {
        RM_ENGINE_TYPE rmEngineType;

        // Check if input is NULL or a SW engine; return a NULL value since SW engine does not have a runlist pri base
        // and this should not be returned as an error
        if (pParams->engineList[i] == NV2080_ENGINE_TYPE_NULL || (pParams->engineList[i] == NV2080_ENGINE_TYPE_SW))
        {
            pParams->hwEngineID[i] = NV2080_CTRL_GPU_GET_HW_ENGINE_ID_NULL;
            continue;
        }

        rmEngineType = gpuGetRmEngineType(pParams->engineList[i]);

        //
        // See if MIG is enabled. If yes, then we have to convert instanceLocal
        // engine to global engine before moving ahead
        //
        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_RETURN(
                LEVEL_ERROR,
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                pDevice, &ref));

            NV_CHECK_OK_OR_RETURN(
                LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &rmEngineType));
        }

        tmpStatus = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_RM_ENGINE_TYPE,
                                             (NvU32)rmEngineType,
                                             ENGINE_INFO_TYPE_FIFO_TAG,
                                             &pParams->hwEngineID[i]);

        if (tmpStatus != NV_OK)
        {
            pParams->hwEngineID[i] = NV2080_CTRL_GPU_GET_HW_ENGINE_ID_ERROR;
            status = tmpStatus;
        }
    }
    return status;
}

NV_STATUS
subdeviceCtrlCmdGpuGetMaxSupportedPageSize_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status  = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    // Default to minimal page size (4k)
    pParams->maxSupportedPageSize = RM_PAGE_SIZE;

    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    if (kgmmuIsPageSize512mbSupported(pKernelGmmu))
    {
        pParams->maxSupportedPageSize = RM_PAGE_SIZE_512M;
    }
    else if (kgmmuIsHugePageSupported(pKernelGmmu))
    {
        pParams->maxSupportedPageSize = RM_PAGE_SIZE_HUGE;
    }
    else
    {
        pParams->maxSupportedPageSize = (NvU32)kgmmuGetMaxBigPageSize_HAL(pKernelGmmu);
    }

    if (gpuIsSriovEnabled(pGpu)
        || gpuIsCCFeatureEnabled(pGpu)
       )
    {
        NvU64 vmmuSegmentSize = gpuGetVmmuSegmentSize(pGpu);
        if (vmmuSegmentSize > 0 &&
            vmmuSegmentSize < NV2080_CTRL_GPU_VMMU_SEGMENT_SIZE_512MB)
        {
            pParams->maxSupportedPageSize = RM_PAGE_SIZE_HUGE;
        }
    }

    return status;
}

#if (defined(DEBUG) || defined(DEVELOP) || RMCFG_FEATURE_MODS_FEATURES) && RMCFG_MODULE_KERNEL_GRAPHICS
NV_STATUS
subdeviceCtrlCmdGpuGetNumMmusPerGpc_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_NUM_MMUS_PER_GPC_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KernelGraphics *pKernelGraphics;
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo;
    NvU32 count;
    NvU32 maxGpcCount;

    // Ensure that the gpcId is within range
    maxGpcCount = gpuGetLitterValues_HAL(pGpu, NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS);
    if (pParams->gpcId >= maxGpcCount)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgrmgrCtrlRouteKGRWithDevice(pGpu, pKernelGraphicsManager,
                                     GPU_RES_GET_DEVICE(pSubdevice),
                                     &pParams->grRouteInfo, &pKernelGraphics));

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);

    count = pKernelGraphicsStaticInfo->floorsweepingMasks.mmuPerGpc[pParams->gpcId];

    // If the litter value doesn't exist (pre Ampere) than default to 1 gpcmmu
    pParams->count = ((count != 0) ? count : 1);

    return NV_OK;
}
#endif

// Stubbed for Orin

/*
 * @brief Update/Set the compute policy config for a GPU
 *
 * @param[in] pSubdevice
 * @param[in] pParams    pointer to control parameters
 *
 * @return
 *  NV_OK                           Success
 *  NV_ERR_NOT_SUPPORTED            Setting policy is not supported on requested GPU
 *  NV_ERR_INVALID_ARGUMENT         Invalid config type/value specified
 *  else appropriate error code.
 */
NV_STATUS
subdeviceCtrlCmdGpuSetComputePolicyConfig_IMPL
(
    Subdevice                                        *pSubdevice,
    NV2080_CTRL_GPU_SET_COMPUTE_POLICY_CONFIG_PARAMS *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    GPU_COMPUTE_POLICY_INFO policyInfo;
    NvU32 gidFlags;
    NV_STATUS status = NV_OK;

    switch(pParams->config.type)
    {
        case NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE:
            if (!gpuIsComputePolicyTimesliceSupported(pGpu))
            {
                NV_PRINTF(LEVEL_ERROR, "Setting the timeslice policy is not supported for gpu with pci id 0x%llx\n",
                          gpuGetDBDF(pGpu));
                return NV_ERR_NOT_SUPPORTED;

            }

            if (pParams->config.data.timeslice >= NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_MAX)
            {
                NV_PRINTF(LEVEL_ERROR, "Unsupported timeslice value %u specified for gpu with pci id 0x%llx\n",
                          pParams->config.data.timeslice,  gpuGetDBDF(pGpu));
                return NV_ERR_INVALID_ARGUMENT;
            }

            policyInfo.timeslice = pParams->config.data.timeslice;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported compute policy %u specified for gpu id 0x%llx\n",
                      pParams->config.type, gpuGetDBDF(pGpu));
            return NV_ERR_INVALID_ARGUMENT;
    }

    gidFlags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
               DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);
    status = gpuGetGidInfo(pGpu, NULL, NULL, gidFlags);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT(pGpu->gpuUuid.isInitialized);
    status = gpudbSetGpuComputePolicyConfig(pGpu->gpuUuid.uuid, pParams->config.type,
                                            &policyInfo);

    return status;
}

//
// Make sure number of compute policies per GPU is always less than or equal
// to the number of policy configs that can be handled by the
// NV2080_CTRL_CMD_GPU_GET_COMPUTE_POLICY_CONFIG command.
//
ct_assert(NV2080_CTRL_GPU_COMPUTE_POLICY_MAX <= NV2080_CTRL_GPU_COMPUTE_POLICY_CONFIG_LIST_MAX);

/*
 * @brief Get all compute policy configs for a GPU
 *
 * @param[in] pSubdevice
 * @param[in] pParams    pointer to control parameters
 *
 * @return
 *  NV_OK on success
 *  else appropriate error code.
 */
NV_STATUS
subdeviceCtrlCmdGpuGetComputePolicyConfig_IMPL
(
    Subdevice                                         *pSubdevice,
    NV2080_CTRL_GPU_GET_COMPUTE_POLICY_CONFIG_PARAMS  *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    GPU_COMPUTE_POLICY_INFO policyInfo;
    NvU32 policyId;
    NvU32 gidFlags;
    NV_STATUS status = NV_OK;

    gidFlags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
               DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);
    status = gpuGetGidInfo(pGpu, NULL, NULL, gidFlags);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT(pGpu->gpuUuid.isInitialized);
    status = gpudbGetGpuComputePolicyConfigs(pGpu->gpuUuid.uuid, &policyInfo);
    if (status != NV_OK)
    {
        return status;
    }

    pParams->numConfigs = 0;
    // Loop through all compute policies and retrieve the configured settings
    for (policyId = NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE;
         policyId < NV2080_CTRL_GPU_COMPUTE_POLICY_MAX;
         policyId++)
    {
        switch (policyId)
        {
            case NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE:
                pParams->configList[policyId].type = NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE;
                pParams->configList[policyId].data.timeslice = policyInfo.timeslice;
                pParams->numConfigs++;
                break;
            default:
                NV_ASSERT(0);
                break;
        }
    }

    return status;
}

/*!
 * @brief Check if address range is within the provided limits
 *
 * @param[in]  addrStart       Staring address of address range
 * @param[in]  addrLength      Size of address range
 * @param[in]  limitStart      Staring address of limit
 * @param[in]  limitLength     Size of limit
 *
 * @return
 *     NV_TRUE, if address range is within the provided limits
 *     NV_FALSE, if address range is outside the provided limits
 *
 */
static NvBool isAddressWithinLimits
(
    NvU64 addrStart,
    NvU64 addrLength,
    NvU64 limitStart,
    NvU64 limitLength
)
{
    NvU64 addrEnd  = 0;
    NvU64 limitEnd = 0;

    //
    // Calculate End address of address range and limit,
    // Return NV_FALSE in case of 64-bit addition overflow
    //
    if (!portSafeAddU64(addrStart, addrLength - 1, &addrEnd) ||
        !portSafeAddU64(limitStart, limitLength - 1, &limitEnd))
    {
        return NV_FALSE;
    }

    return ((addrStart >= limitStart) && (addrEnd <= limitEnd));
}

/*!
 * @brief Validate the address range for Memory Map request by comparing the
 *        user supplied address range with GPU BAR0/BAR1 range.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice
 * @param[in] pParams    pointer to control parameters
 *
 * Possible status values returned are:
 *     NV_OK
 *     NV_ERR_PROTECTION_FAULT
 *
 */
NV_STATUS subdeviceCtrlCmdValidateMemMapRequest_IMPL
(
    Subdevice                                       *pSubdevice,
    NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS *pParams
)
{
    OBJGPU       *pGpu     = GPU_RES_GET_GPU(pSubdevice);
    NvU64         start    = pParams->addressStart;
    NvU64         length   = pParams->addressLength;
    NV_STATUS     rmStatus;
    NvU32         bar0MapSize;
    NvU64         bar0MapOffset;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance));

    pParams->protection = NV_PROTECT_READ_WRITE;

    if (isAddressWithinLimits(start, length, pGpu->busInfo.gpuPhysAddr,
                        pGpu->deviceMappings[0].gpuNvLength))
    {
        start -= pGpu->busInfo.gpuPhysAddr;

        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        rmStatus = tmrGetTimerBar0MapInfo_HAL(pGpu, pTmr,
                                              &bar0MapOffset, &bar0MapSize);
        if ((rmStatus == NV_OK) &&
            isAddressWithinLimits(start, length, bar0MapOffset, bar0MapSize))
        {
            pParams->protection = NV_PROTECT_READABLE;
            return NV_OK;
        }

        KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
        rmStatus = kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo,
                                               &bar0MapOffset, &bar0MapSize);
        if ((rmStatus == NV_OK) &&
            isAddressWithinLimits(start, length, bar0MapOffset, bar0MapSize))
        {
            return NV_OK;
        }

        KernelMc *pKernelMc = GPU_GET_KERNEL_MC(pGpu);
        rmStatus = kmcGetMcBar0MapInfo_HAL(pGpu, pKernelMc,
                &bar0MapOffset, &bar0MapSize);
        if ((rmStatus == NV_OK) &&
            isAddressWithinLimits(start, length, bar0MapOffset, bar0MapSize))
        {
            pParams->protection = NV_PROTECT_READABLE;
            return NV_OK;
        }

        //
        // If the kernel side does not know about the object being mapped,
        // fall-through to GSP and see if it knows anything.
        //
        if (IS_GSP_CLIENT(pGpu))
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

            return pRmApi->Control(pRmApi,
                                   pGpu->hInternalClient,
                                   pGpu->hInternalSubdevice,
                                   NV2080_CTRL_CMD_GPU_VALIDATE_MEM_MAP_REQUEST,
                                   pParams, sizeof(*pParams));
        }

        return NV_ERR_PROTECTION_FAULT;
    }
    // See bug 1784955
    else if (isAddressWithinLimits(start, length, pGpu->busInfo.gpuPhysFbAddr, pGpu->fbLength)
            || GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->coherentCpuFbBase)
    {
        return NV_OK;
    }

    return NV_ERR_PROTECTION_FAULT;
}

/*!
 * @brief Computes the GFID (GPU Function ID) for a given SR-IOV
 *        Virtual Function (VF) of the physical GPU based on the
 *        BDF parameters provided by the caller.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice
 * @param[in] pParams    pointer to control parameters
 *
 * Possible status values returned are:
 *     NV_OK                on successful computation of a valid GFID
 *     NV_ERR_NOT_SUPPORTED if ctrl call is made when
 *                          SRIOV is not enabled OR
 *                          caller is not FM from Host RM
 *     NV_ERR_INVALID_STATE if computed GFID is greater than
 *                          max GFID that is expected/allowed
 */
NV_STATUS
subdeviceCtrlCmdGpuGetGfid_IMPL
(
    Subdevice                           *pSubdevice,
    NV2080_CTRL_GPU_GET_GFID_PARAMS     *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NvU32  pciFunction, gfid;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!gpuIsSriovEnabled(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    // Host RM && FM
    if ((!IS_VIRTUAL(pGpu)) &&
        (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED)))
    {
        //
        // In unix based systems, OS uses lspci format which is "ssss:bb:dd.f",
        // so device is 5 bits and function 3 bits.
        // for SR-IOV when ARI is enabled, device and function gets combined and
        // we need to consider 8 bits function.
        //
        pciFunction = (pParams->device << 3) | pParams->func;
        gfid = (pciFunction - pGpu->sriovState.firstVFOffset) + 1;

        if (gfid > pGpu->sriovState.maxGfid)
        {
            NV_PRINTF(LEVEL_ERROR, "Computed GFID %d greater than max supported GFID\n", gfid);
            return NV_ERR_INVALID_STATE;
        }

        pParams->gfid = gfid;
        // Also set the mask for max gfid supported currently in the driver
        pParams->gfidMask = (pGpu->sriovState.maxGfid - 1);
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

/*!
 * @brief Sets or unsets the SW state to inform the GPU driver that the GPU instance
 *        associated with input GFID has been activated or de-activated respectively.
 *
 * Lock Requirements:
 *      Assert that API and GPUs lock held on entry
 *
 * @param[in] pSubdevice
 * @param[in] pParams    pointer to control parameters
 *
 * Possible status values returned are:
 *     NV_OK                    on success
 *     NV_ERR_INVALID_STATE     if SRIOV state for P2P in driver is not setup
 *     NV_ERR_INVALID_ARGUMENT  if input GFID is greater than the max GFID allowed
 *     NV_ERR_NOT_SUPPORTED     if ctrl call is made when
 *                              SRIOV is not enabled OR
 *                              caller is not FM from Host RM
 *     NV_ERR_IN_USE            If MAX_NUM_P2P_GFIDS have already been enabled for P2P
 */
NV_STATUS
subdeviceCtrlCmdUpdateGfidP2pCapability_IMPL
(
    Subdevice                                               *pSubdevice,
    NV2080_CTRL_CMD_GPU_UPDATE_GFID_P2P_CAPABILITY_PARAMS   *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    PSRIOV_P2P_INFO pP2PInfo = pGpu->sriovState.pP2PInfo;
    NvBool  bSetP2PAccess = NV_FALSE;
    NvU32   idx;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!gpuIsSriovEnabled(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    NV_ASSERT_OR_RETURN(pP2PInfo != NULL, NV_ERR_INVALID_STATE);

    // Ctrl call should only be called by the FM from Host RM
    if ((!IS_VIRTUAL(pGpu)) &&
        (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED)))
    {
        if (pParams->gfid > pGpu->sriovState.maxGfid)
        {
            NV_PRINTF(LEVEL_ERROR, "Input GFID %d greater than max allowed GFID\n", pParams->gfid);
            return NV_ERR_INVALID_ARGUMENT;
        }

        for (idx = 0; idx < pGpu->sriovState.maxP2pGfid; idx++)
        {
            //
            // Check if Host RM is already using a GFID for P2P,
            // Since only "MAX_NUM_P2P_GFIDS" GFID(s) is(are) allowed to do P2P at any time,
            // we should fail here if a GFID greater than supported number is being enabled
            //
            if (pParams->bEnable)
            {
                if (pP2PInfo[idx].gfid == INVALID_P2P_GFID)
                {
                    pP2PInfo[idx].gfid = pParams->gfid;
                    pGpu->sriovState.p2pFabricPartitionId = pParams->fabricPartitionId;
                    bSetP2PAccess = NV_TRUE;
                    break;
                }
            }
            else
            {
                if (pP2PInfo[idx].gfid == pParams->gfid)
                {
                    pP2PInfo[idx].gfid = INVALID_P2P_GFID;
                    pGpu->sriovState.p2pFabricPartitionId = INVALID_FABRIC_PARTITION_ID;
                    bSetP2PAccess = NV_TRUE;
                    break;
                }
            }
        }

        if (bSetP2PAccess == NV_TRUE)
        {
            pP2PInfo[idx].bAllowP2pAccess = pParams->bEnable;
        }
        else
        {
            // Some other GFID(s) has already been enabled to do P2P
            // Fail the call
            return NV_ERR_IN_USE;
        }
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

/*
 * Set the EGM fabric base address
 */
NV_STATUS
subdeviceCtrlCmdGpuSetEgmGpaFabricAddr_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_EGM_GPA_FABRIC_BASE_ADDR_PARAMS *pParams
)
{
    return NV_OK;
}

/*!
 * @brief: This command returns the load time (latency) of each engine,
 *         implementing NV2080_CTRL_CMD_GPU_GET_ENGINE_LOAD_TIMES control call.
 *
 * @param[in]   pSubdevice
 * @param[in]   pParams
 *
 * @return
 *  NV_OK       Success
 */
NV_STATUS
subdeviceCtrlCmdGpuGetEngineLoadTimes_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_GET_ENGINE_LOAD_TIMES_PARAMS *pParams
)
{
    OBJGPU        *pGpu              = GPU_RES_GET_GPU(pSubdevice);
    PENGDESCRIPTOR engDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    NvU32          numEngDescriptors = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    NV_ASSERT_OR_RETURN(numEngDescriptors < NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS, NV_ERR_BUFFER_TOO_SMALL);

    pParams->engineCount = numEngDescriptors;
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGDESCRIPTOR   curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE    *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            pParams->engineIsInit[curEngDescIdx] = NV_FALSE;
            continue;
        }

        pParams->engineList[curEngDescIdx]          = pEngstate->engDesc;
        pParams->engineStateLoadTime[curEngDescIdx] = pEngstate->stats[ENGSTATE_STATE_LOAD].transitionTimeUs * 1000;
        pParams->engineIsInit[curEngDescIdx]        = NV_TRUE;
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdGpuSetFabricAddr
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdGpuSetFabricAddr_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GPU_SET_FABRIC_BASE_ADDR_PARAMS *pParams
)
{
    OBJGPU       *pGpu         = GPU_RES_GET_GPU(pSubdevice);
    NvHandle      hClient      = RES_GET_CLIENT_HANDLE(pSubdevice);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (!rmclientIsCapableOrAdminByHandle(hClient,
                                          NV_RM_CAP_EXT_FABRIC_MGMT,
                                          pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_ERROR, "Non-privileged context issued privileged cmd\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink == NULL)
        return NV_ERR_NOT_SUPPORTED;

    return knvlinkSetUniqueFabricBaseAddress(pGpu, pKernelNvlink, pParams->fabricBaseAddr);
}

static NvU64
_convertGpuFabricProbeInfoCaps
(
    NvU64 fmCaps
)
{
    NvU64 fabricCaps = 0;
    NvU32 i = 0;

    FOR_EACH_INDEX_IN_MASK(64, i, fmCaps)
    {
        switch (NVBIT64(i))
        {
            case NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1:
            case NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1:
            {
                fabricCaps |= NV2080_CTRL_GPU_FABRIC_PROBE_CAP_MC_SUPPORTED;
                break;
            }
            case NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V2:
            {
                fabricCaps |= NV2080_CTRL_GPU_FABRIC_PROBE_CAP_MC_SUPPORTED;
                break;
            }
            default:
            {
                break;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return fabricCaps;
}

NV_STATUS
subdeviceCtrlCmdGetGpuFabricProbeInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS status;
    NvU64 numProbeReqs = 0;
    NvU64 fmCaps = 0;
    NvUuid *pClusterUuid = (NvUuid*) pParams->clusterUuid;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() &&
                           rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    if (pGpu->pGpuFabricProbeInfoKernel == NULL)
    {
        pParams->state = NV2080_CTRL_GPU_FABRIC_PROBE_STATE_UNSUPPORTED;
        return NV_OK;
    }

    status = gpuFabricProbeGetNumProbeReqs(pGpu->pGpuFabricProbeInfoKernel,
                                           &numProbeReqs);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error while retrieving numProbeReqs\n");
        return status;
    }

    pParams->state = (numProbeReqs == 0) ?
                     NV2080_CTRL_GPU_FABRIC_PROBE_STATE_NOT_STARTED :
                     NV2080_CTRL_GPU_FABRIC_PROBE_STATE_IN_PROGRESS;

    if (gpuFabricProbeIsReceived(pGpu->pGpuFabricProbeInfoKernel))
    {
        pParams->state  = NV2080_CTRL_GPU_FABRIC_PROBE_STATE_COMPLETE;
        pParams->status = gpuFabricProbeGetFmStatus(pGpu->pGpuFabricProbeInfoKernel);
        if (pParams->status != NV_OK)
        {
            // Nothing needs to be done as probe response status is not success
            return NV_OK;
        }

        ct_assert(NV2080_GPU_FABRIC_CLUSTER_UUID_LEN == NV_UUID_LEN);

        NV_ASSERT_OK(gpuFabricProbeGetClusterUuid(pGpu->pGpuFabricProbeInfoKernel,
                    pClusterUuid));

        NV_ASSERT_OK(gpuFabricProbeGetFabricPartitionId(pGpu->pGpuFabricProbeInfoKernel,
                     &pParams->fabricPartitionId));

        NV_ASSERT_OK(gpuFabricProbeGetFabricCliqueId(pGpu->pGpuFabricProbeInfoKernel,
                     &pParams->fabricCliqueId));

        NV_ASSERT_OK(gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel, &fmCaps));

        pParams->fabricCaps = _convertGpuFabricProbeInfoCaps(fmCaps);
    }

    return NV_OK;
}
