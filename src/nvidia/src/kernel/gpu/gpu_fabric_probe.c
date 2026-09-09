/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief GPU Fabric Probe handling
 */


#include "gpu/gpu.h"
#include "os/os.h"
#include "nvport/atomic.h"
#include "utils/nvprintf.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/gpu_fabric_probe.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "nvlink_inband_msg.h"
#include "kernel/mem_mgr/fabric_vaspace.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "nvrm_registry.h"

#include "compute/fabric.h"
#include "class/cl00f1.h"
#include "vgpu/rpc.h"

// Structure to hold gpu probe information
typedef struct GPU_FABRIC_PROBE_INFO_KERNEL
{
    PORT_ATOMIC NvU32 probeRespRcvd;
    NvU16 bwMode;

    OBJGPU *pGpu;

    NvU8 activeProbeVersion;  // NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ or _V2

    // Store response as union to support both V1 and V2
    union {
        nvlink_inband_gpu_probe_rsp_msg_t     v1Response;
        nvlink_inband_gpu_probe_rsp_v2_msg_t  v2Response;
    } probeResponseMsg;

    // Version-agnostic fields - not updated in probeResponseMsg
    NvU64  gpuHandle;
    NvU32  gfId;
    NvUuid clusterUuid;
    NvU16  fabricPartitionId;
    NvU64  gpaAddress;
    NvU64  gpaAddressRange;
    NvU64  flaAddress;
    NvU64  flaAddressRange;
    NvU64  egmGpaAddress;
    NvU32  ucPointerCliqueId;
    NvU32  ucHandleCliqueId;
    NvU32  mcPointerCliqueId;
    NvU32  mcHandleCliqueId;
    NvU32  pushRedCliqueId;
    NvU32  degradedCliqueId;
    NvU32  fabricHealthMask;
    NvU32  remapTableIdx;
    NvU64  fmCaps0;
    NvU64  sysmemFlaBaseAddr;
    NvU64  sysmemFlaSize;

    // Pre-converted bitvector fields
    NVLINK_BIT_VECTOR linkMaskToBeReduced;
    NVLINK_BIT_VECTOR rbmSupportedLinkCount;
} GPU_FABRIC_PROBE_INFO_KERNEL;

// Structure to hold RBM wake link 1Hz callback data
typedef struct RBM_WAKE_LINK_1HZ_CALLBACK_DATA
{
    NvU16 bwMode;
    RMTIMEOUT timeout;
} RBM_WAKE_LINK_1HZ_CALLBACK_DATA;

// Structure to hold ABM wake links work item data
typedef struct ABM_WAKE_LINKS_WORKITEM_DATA
{
    NVLINK_BIT_VECTOR linkMask;
} ABM_WAKE_LINKS_WORKITEM_DATA;

static NV_STATUS
_gpuFabricProbeFullSanityCheck
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfoKernel))
    {
        return NV_ERR_NOT_READY;
    }

    if (!gpuFabricProbeIsSuccess(pGpuFabricProbeInfoKernel))
    {
        // msgHdr is at the same offset in both V1 and V2 response
        NV_PRINTF(LEVEL_ERROR, "Fabric Probe failed: 0x%x\n",
                  pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr.status);

        return pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr.status;
    }

    // Verify active probe version matches the stored response message type
    if (pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ_V2)
    {
        if (pGpuFabricProbeInfoKernel->probeResponseMsg.v2Response.msgHdr.type != NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP_V2)
        {
            NV_PRINTF(LEVEL_ERROR, "Active probe version V2 but response type mismatch: 0x%x\n",
                      pGpuFabricProbeInfoKernel->probeResponseMsg.v2Response.msgHdr.type);
            return NV_ERR_INVALID_STATE;
        }
    }
    else if (pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ)
    {
        if (pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr.type != NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP)
        {
            NV_PRINTF(LEVEL_ERROR, "Active probe version V1 but response type mismatch: 0x%x\n",
                      pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr.type);
            return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

static void
_gpuFabricProbeInvalidate
(
    OBJGPU *pGpu
)
{
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel =
                                    pGpu->pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    portAtomicSetU32(&pGpuFabricProbeInfoKernel->probeRespRcvd, 0);

    //
    // Note: If we reach here, there are no p2p objects present and it's safe
    // to clean up the FLA sysmem window since UVM is not using it.
    //
    memmgrDestroySysmemFlaWindowForUvm_HAL(pGpu, pMemoryManager);

    if (pKernelNvlink != NULL)
    {
        knvlinkClearUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink);
        knvlinkClearUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink);
    }

    if (pFabricVAS != NULL)
        fabricvaspaceClearUCRanges(pFabricVAS);
}

static void
_gpuFabricProbeCheckResetRequired
(
    OBJGPU *pGpu,
    NvU32 fabricHealthMask
)
{
    if (REF_VAL(NVLINK_INBAND_FABRIC_HEALTH_MASK_INCORRECT_CONFIGURATION, fabricHealthMask) ==
        NVLINK_INBAND_FABRIC_HEALTH_MASK_INCORRECT_CONFIGURATION_GPU_STATE_INVALID)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_TRUE);
        gpuMarkDeviceForDrainAndReset(pGpu);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_SQUASH_XID154, NV_FALSE);
    }
}

/*
 * @brief This function checks if all links in the link mask are in ACTIVE state.
 * @param pGpu:          Pointer to GPU object
 * @param pKernelNvlink  Pointer to kernel NVLink object
 * @param pLinkMas       Pointer to the link mask to check
 * @param bPoll          Boolean if function should poll on transition
 * @return true if all links are in ACTIVE state, false otherwise
 */
static NvBool
_gpuFabricProbeRbmCheckLinkWake
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NVLINK_BIT_VECTOR *pLinkMask,
    NvBool bPoll
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    NvU32 linkId;
    RMTIMEOUT timeout;
    NVLINK_BIT_VECTOR linkMask;
    NvU64 linkStateChangeTimeMs = (NvU64)knvlinkGetLinkStateChangeTimeMs(pGpu, pKernelNvlink);
    NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pLinkClockParams;

    NV_ASSERT_OR_RETURN(pLinkMask != NULL, NV_FALSE);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, NV_OK == bitVectorCopy(&linkMask, pLinkMask), NV_FALSE);

    pLinkClockParams = portMemAllocNonPaged(sizeof(*pLinkClockParams));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pLinkClockParams != NULL, NV_FALSE);

    gpuSetTimeout(pGpu, (linkStateChangeTimeMs*1000U) + 1000000U, &timeout, 0U);

    do
    {
        status = gpuCheckTimeout(pGpu, &timeout);

        portMemSet(pLinkClockParams, 0, sizeof(*pLinkClockParams));

        NV_CHECK_OR_ELSE(LEVEL_ERROR,
           NV_OK == convertBitVectorToLinkMasks(&linkMask,
                &pLinkClockParams->linkMask,
                                        sizeof(pLinkClockParams->linkMask),
                                        &pLinkClockParams->links),
                {
                    portMemFree(pLinkClockParams);
                    return NV_FALSE;
                });

        NV_CHECK_OR_ELSE(LEVEL_INFO, NV_OK ==
                pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO,
                            pLinkClockParams, sizeof(*pLinkClockParams)),
                {
                    portMemFree(pLinkClockParams);
                    return NV_FALSE;
                });

        FOR_EACH_IN_BITVECTOR(&linkMask, linkId)
        {
            if (linkId >= NV2080_CTRL_INTERNAL_NVLINK_MAX_ARR_SIZE)
                break;

            if (pLinkClockParams->linkInfo[linkId].linkState ==
                NV2080_CTRL_NVLINK_STATUS_LINK_STATE_ACTIVE)
            {
                (void)bitVectorClr(&linkMask, linkId);
            }
        }
        FOR_EACH_IN_BITVECTOR_END();

        osSpinLoop();
    }
    while (bPoll && status != NV_ERR_TIMEOUT && !bitVectorTestAllCleared(&linkMask));
    portMemFree(pLinkClockParams);

    return bitVectorTestAllCleared(&linkMask);
}

 /*
 * @brief This function is used to check if all links are in ACTIVE
 *         state and set the BW mode and restart probes if they are.
 * @param gpuInstance: The GPU instance
 * @param pData:       not used
 */
void
_gpuFabricProbeRbmCheckLinkWake1HzCallback
(
    OBJGPU *pGpu,
    void* pData
)
{
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NVLINK_BIT_VECTOR *pEnabledLinkMask = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    RBM_WAKE_LINK_1HZ_CALLBACK_DATA *pCallbackData = (RBM_WAKE_LINK_1HZ_CALLBACK_DATA *)pData;

    NV_ASSERT(pCallbackData != NULL);

    //
    // If all links are active, set the BW mode and restart probes and remove the callback
    // if the timeout is reached, set the BW mode status to error and remove the callback
    //
    if (_gpuFabricProbeRbmCheckLinkWake(pGpu, pKernelNvlink, pEnabledLinkMask, NV_FALSE))
    {
        knvlinkSetBWMode(pGpu, pKernelNvlink, pCallbackData->bwMode);
        osRemove1HzCallback(pGpu, _gpuFabricProbeRbmCheckLinkWake1HzCallback, pData);
        portMemFree(pData);
        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, NV_OK == gpuFabricProbeResume(pGpuFabricProbeInfoKernel));

    }
    else if (gpuCheckTimeout(pGpu, &pCallbackData->timeout) == NV_ERR_TIMEOUT)
    {
        KNVLINK_SET_BW_MODE_STATUS(pKernelNvlink, NVLINK_BW_MODE_STATUS_ERROR_TIMEOUT_LINK_WAKE);
        osRemove1HzCallback(pGpu, _gpuFabricProbeRbmCheckLinkWake1HzCallback, pData);
        portMemFree(pData);
    }
}

static void
_gpuFabricProbeRbmWakeLinks
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvBool bSync,
    NvU16 bwMode
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS powerStatusParams = {0};
    NVLINK_BIT_VECTOR *pEnabledLinksVec = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    NVLINK_BIT_VECTOR linkMask;
    NV_STATUS status;
    NvU32 i;
    NvU64 linkStateChangeTimeMs;
    RBM_WAKE_LINK_1HZ_CALLBACK_DATA *pCallbackData;

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, NV_OK == bitVectorClrAll(&linkMask));

    FOR_EACH_IN_BITVECTOR(pEnabledLinksVec, i)
    {
        powerStatusParams.linkId = i;
        NV_CHECK_OK(status, LEVEL_INFO,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_NVLINK_GET_POWER_STATE,
                            &powerStatusParams, sizeof(powerStatusParams)));
        if (powerStatusParams.powerState == NV2080_CTRL_NVLINK_POWER_STATE_L2)
        {
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, NV_OK == bitVectorSet(&linkMask, i));
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    if (!bitVectorTestAllCleared(&linkMask))
    {
        // Wake all sleeping links
        status = knvlinkEnterExitSleep(pGpu, pKernelNvlink, &linkMask, NV_FALSE);
        if (status != NV_OK)
        {

            NV_PRINTF(LEVEL_ERROR, "Error waking links on linkmask "NV_BITVECTOR_INLINE_FMTX"\n",
                NV_BITVECTOR_INLINE_PRINTF_ARG(&linkMask));
        }
    }

    if (bSync)
    {
        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            _gpuFabricProbeRbmCheckLinkWake(pGpu, pKernelNvlink, &linkMask, NV_TRUE));
    }
    else
    {
        linkStateChangeTimeMs = (NvU64)knvlinkGetLinkStateChangeTimeMs(pGpu, pKernelNvlink);

        pCallbackData = (RBM_WAKE_LINK_1HZ_CALLBACK_DATA *)portMemAllocNonPaged(sizeof(RBM_WAKE_LINK_1HZ_CALLBACK_DATA));
        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pCallbackData != NULL);

        portMemSet(pCallbackData, 0, sizeof(RBM_WAKE_LINK_1HZ_CALLBACK_DATA));

        // Set the timeout to be the link state change time + 1s
        gpuSetTimeout(pGpu, linkStateChangeTimeMs*1000U + 1000000U, &pCallbackData->timeout, 0U);

        status = _gpuFabricProbeRbmCheckLinkWake(pGpu, pKernelNvlink, &linkMask, NV_FALSE);

        pCallbackData->bwMode = bwMode;

        // Launch repeated 1Hz workitem to wait for links to train
        status = osSchedule1HzCallback(pGpu, _gpuFabricProbeRbmCheckLinkWake1HzCallback,
                                        (void *)pCallbackData, NV_OS_1HZ_REPEAT);
        if (status != NV_OK)
        {
            portMemFree(pCallbackData);
        }
    }
}

static void
_gpuFabricProbeRbmSetCallback
(
    NvU32 gpuInstance,
    void* pData
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NvU16 mode = *(NvU16 *)pData;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

    // Should not happen
    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL && pGpuFabricProbeInfoKernel != NULL);

    // Need to wake links if LINK_COUNT or FULL BW mode is requested before probe update
    if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_LINK_COUNT) ||
        (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL))
    {
        if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
        {
            _gpuFabricProbeRbmWakeLinks(pGpu, pGpuFabricProbeInfoKernel, NV_FALSE, mode);

        }
    }
}

NV_STATUS
gpuFabricProbeGetGpuFabricHandle
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pHandle
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pHandle = pGpuFabricProbeInfoKernel->gpuHandle;

    return status;
}

NV_STATUS
gpuFabricProbeGetGfId
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32 *pGfId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pGfId = pGpuFabricProbeInfoKernel->gfId;

    return status;
}

NV_STATUS
gpuFabricProbeGetfmCaps
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pFmCaps
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFmCaps = pGpuFabricProbeInfoKernel->fmCaps0;

    return status;
}

NV_STATUS
gpuFabricProbeGetClusterUuid
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvUuid *pClusterUuid
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    portMemCopy(pClusterUuid, sizeof(*pClusterUuid),
                &pGpuFabricProbeInfoKernel->clusterUuid,
                sizeof(pGpuFabricProbeInfoKernel->clusterUuid));

    return status;
}

NV_STATUS
gpuFabricProbeGetFabricPartitionId
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU16 *pFabricPartitionId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFabricPartitionId = pGpuFabricProbeInfoKernel->fabricPartitionId;

    return status;
}

NV_STATUS
gpuFabricProbeGetGpaAddress
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pGpaAddress
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pGpaAddress = pGpuFabricProbeInfoKernel->gpaAddress;

    return status;
}

NV_STATUS
gpuFabricProbeGetGpaAddressRange
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pGpaAddressRange
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pGpaAddressRange = pGpuFabricProbeInfoKernel->gpaAddressRange;

    return status;
}

NV_STATUS
gpuFabricProbeGetFlaAddress
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pFlaAddress
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFlaAddress = pGpuFabricProbeInfoKernel->flaAddress;

    return status;
}

NV_STATUS
gpuFabricProbeGetFlaAddressRange
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pFlaAddressRange
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFlaAddressRange = pGpuFabricProbeInfoKernel->flaAddressRange;

    return status;
}

static NV_STATUS
gpuFabricProbeGetSysmemFlaAddressAndRange
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pFlaAddress,
    NvU64 *pFlaAddressRange
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFlaAddress      = pGpuFabricProbeInfoKernel->sysmemFlaBaseAddr;
    *pFlaAddressRange = pGpuFabricProbeInfoKernel->sysmemFlaSize;

    return status;
}

/*
 * This function is used to get the peer GPU EGM address from FM to RM.
 * FM passes only the upper 32 bits of the address.
 */
NV_STATUS
gpuFabricProbeGetEgmGpaAddress
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *pEgmGpaAddress
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pEgmGpaAddress = pGpuFabricProbeInfoKernel->egmGpaAddress;

    return status;
}

NV_STATUS
gpuFabricProbeGetNumProbeReqs
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU64 *numProbes
)
{
    NV2080_CTRL_CMD_INTERNAL_GET_GPU_FABRIC_PROBE_INFO_PARAMS params = { 0 };
    RM_API *pRmApi;
    OBJGPU *pGpu;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(
                           gpuGetInstance(pGpuFabricProbeInfoKernel->pGpu)),
        NV_ERR_INVALID_LOCK_STATE);

    //
    // This function gets called from CUDA stack to check the probe state (NV2080_CTRL_CMD_GET_GPU_FABRIC_PROBE_INFO).
    // The value returned by this function determines if probe is started or not.
    // We just return a non-zero value here for vgpu, because we don't track fabric probe stats in the guests yet.
    //
    if (IS_VIRTUAL(pGpu))
    {
        *numProbes = 1;
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
          pRmApi->Control(pRmApi,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_INTERNAL_GPU_GET_FABRIC_PROBE_INFO,
                          &params,
                          sizeof(params)));

    *numProbes = params.numProbes;

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetFabricCliqueIdByType
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU8                         cliqueType,
    NvU32                        *pCliqueId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    switch (cliqueType)
    {
        case NV_FABRIC_CLIQUE_TYPE_UNICAST_POINTER:
            *pCliqueId = pGpuFabricProbeInfoKernel->ucPointerCliqueId;
            break;
        case NV_FABRIC_CLIQUE_TYPE_MULTICAST_POINTER:
            *pCliqueId = pGpuFabricProbeInfoKernel->mcPointerCliqueId;
            break;
        case NV_FABRIC_CLIQUE_TYPE_UNICAST_HANDLE:
            *pCliqueId = pGpuFabricProbeInfoKernel->ucHandleCliqueId;
            break;
        case NV_FABRIC_CLIQUE_TYPE_MULTICAST_HANDLE:
            *pCliqueId = pGpuFabricProbeInfoKernel->mcHandleCliqueId;
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_ERR_NOT_SUPPORTED);
    }

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetFabricHealthStatus
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32 *pFabricHealthStatusMask
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFabricHealthStatusMask = pGpuFabricProbeInfoKernel->fabricHealthMask;

    return NV_OK;
}

NvBool
gpuFabricProbeIsReceived
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_FALSE;
    }

    return !!portAtomicOrU32(&pGpuFabricProbeInfoKernel->probeRespRcvd, 0);
}

NvBool
gpuFabricProbeIsSuccess
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    nvlink_inband_msg_header_t *pProbeRespMsgHdr;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_FALSE;
    }

    // msgHdr is at the same offset in both V1 and V2 response
    pProbeRespMsgHdr = &pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr;

    return pProbeRespMsgHdr->status == NV_OK;
}

NV_STATUS
gpuFabricProbeGetFmStatus
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // msgHdr is at the same offset in both V1 and V2 response
    return pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.msgHdr.status;
}

static void
_gpuFabricProbeSetupGpaRange
(
    OBJGPU                *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    // setup GPA based system
    if (pKernelNvlink != NULL)
    {
        NvU64 gpaAddress;
        NvU64 gpaAddressSize;
        NvU64 egmGpaAddress;
        NvU64 fmCaps;

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    gpuFabricProbeGetGpaAddress(pGpuFabricProbeInfoKernel,
                                                &gpaAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    gpuFabricProbeGetGpaAddressRange(pGpuFabricProbeInfoKernel,
                                                    &gpaAddressSize) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    knvlinkSetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink,
                                                        gpaAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    gpuFabricProbeGetfmCaps(pGpuFabricProbeInfoKernel,
                                        &fmCaps) == NV_OK);

        if (fmCaps & NVLINK_INBAND_FM_CAPS_EGM_ENABLED)
        {
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                        gpuFabricProbeGetEgmGpaAddress(pGpuFabricProbeInfoKernel,
                                                    &egmGpaAddress) == NV_OK);

            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    knvlinkSetUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink,
                                                        egmGpaAddress) == NV_OK);
        }
    }
}

static void
_gpuFabricProbeSetupFlaRange
(
    OBJGPU                  *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL   *pGpuFabricProbeInfoKernel
)
{
    if (pGpu->pFabricVAS != NULL)
    {
        FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
        NvU64 flaBaseAddress;
        NvU64 flaSize;
        NvU64 ptrSize;
        NvU64 emulatedHandleSize;

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            gpuFabricProbeGetFlaAddress(pGpuFabricProbeInfoKernel, &flaBaseAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            gpuFabricProbeGetFlaAddressRange(pGpuFabricProbeInfoKernel, &flaSize) == NV_OK);

        if (IS_VIRTUAL(pGpu))
        {
            fabricvaspaceClearUCRanges(pFabricVAS);
        }

        //
        // WAR: GFM delivers a single UC FLA range per GPU. Until GFM supplies
        // the pointer/emulated-handle heap split inband, carve it here:
        //   - Reserve up to UC_FLA_EMU_HEAP_MAX_SIZE for the emulated-handle
        //     heap.
        //   - Always leave at least UC_FLA_PTR_HEAP_BASE_SIZE for the pointer
        //     heap, plus UC_FLA_PTR_HEAP_EGM_RESERVE when EGM is enabled. The
        //     EGM reserve mirrors the extra TiB GFM adds to flaAddressRange
        //     for EGM mappings, keeping it in the pointer heap rather than
        //     rolling it into the emu heap.
        //
        // SKU breakdown (flaSize → ptr / emu):
        //   Blackwell     EGM-off (6 TiB):  1 TiB / 5 TiB
        //   Blackwell     EGM-on  (7 TiB):  2 TiB / 5 TiB
        //   Blackwell 576 EGM-off (2 TiB):  1 TiB / 1 TiB
        //   Blackwell 576 EGM-on  (3 TiB):  2 TiB / 1 TiB
        //   Rubin                 (8 TiB):  3 TiB / 5 TiB
        //
        const NvU64 UC_FLA_PTR_HEAP_BASE_SIZE   = (1ULL << 40);            // 1 TiB
        const NvU64 UC_FLA_PTR_HEAP_EGM_RESERVE = (1ULL << 40);            // 1 TiB
        const NvU64 UC_FLA_EMU_HEAP_MAX_SIZE    = (5ULL * (1ULL << 40));   // 5 TiB

        NvU64  fmCaps      = 0;
        NvBool bEgmEnabled = NV_FALSE;
        NvU64  ptrMinSize;

        if (gpuFabricProbeGetfmCaps(pGpuFabricProbeInfoKernel, &fmCaps) == NV_OK)
            bEgmEnabled = !!(fmCaps & NVLINK_INBAND_FM_CAPS_EGM_ENABLED);

        ptrMinSize = UC_FLA_PTR_HEAP_BASE_SIZE +
                     (bEgmEnabled ? UC_FLA_PTR_HEAP_EGM_RESERVE : 0);

        ptrSize            = flaSize;
        emulatedHandleSize = 0;

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_HANDLE_TRANSLATION_DEF) &&
            (flaSize > ptrMinSize))
        {
            emulatedHandleSize = NV_MIN(UC_FLA_EMU_HEAP_MAX_SIZE, flaSize - ptrMinSize);
            ptrSize            = flaSize - emulatedHandleSize;
        }

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            fabricvaspaceInitUCRange(pFabricVAS, pGpu, flaBaseAddress, ptrSize) == NV_OK);

        if (emulatedHandleSize != 0)
        {
            NvU64 emulatedHandleBase = flaBaseAddress + ptrSize;
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                fabricvaspaceInitUCEmulatedHandleRange(pFabricVAS, pGpu, emulatedHandleBase,
                                                       emulatedHandleSize) == NV_OK);
        }
    }
}

static void
_gpuFabricProbeSetupSysmemFlaRange
(
    OBJGPU                       *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 sysmemFlaBase, sysmemFlaSize;
    NvU64 fmCaps = 0;
    NV_STATUS status;

    // Sysmem addressing over FLA is only supported on coherent platforms
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO,
        pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING));

    // Serial ATS support
    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
        memmgrIsFlaSysmemSupported_HAL(pGpu, pMemoryManager));

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
        gpuFabricProbeGetfmCaps(pGpuFabricProbeInfoKernel, &fmCaps) == NV_OK);

    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO,
        (fmCaps & NVLINK_INBAND_FM_CAPS_UVM_FLA_ENABLED) != 0);

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
        gpuFabricProbeGetSysmemFlaAddressAndRange(pGpuFabricProbeInfoKernel,
                                                  &sysmemFlaBase,
                                                  &sysmemFlaSize) == NV_OK);

    if (sysmemFlaSize == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%u Sysmem FLA: GFM returned invalid FLA range - FLA base=0x%llx size=0x%llx\n",
                  gpuGetInstance(pGpu), sysmemFlaBase, sysmemFlaSize);
        return;
    }

    status = memmgrInitSysmemFlaWindowForUvm_HAL(pGpu, pMemoryManager,
                                                 sysmemFlaBase, sysmemFlaSize);
    if (status == NV_ERR_NOT_SUPPORTED)
        return;

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%u Sysmem FLA setup failed: error=0x%x, FLA base=0x%llx size=0x%llx\n",
                  gpuGetInstance(pGpu), status, sysmemFlaBase, sysmemFlaSize);
        return;
    }

    NV_PRINTF(LEVEL_INFO,
              "GPU%u Sysmem FLA window established: FLA base=0x%llx size=0x%llx\n",
              gpuGetInstance(pGpu), sysmemFlaBase, sysmemFlaSize);
}

static void
_gpuFabricProbeSendImexCliqueIdChangeEvent
(
    OBJGPU *pGpu,
    NvU8    cliqueType,
    NvU32   oldCliqueId,
    NvU32   cliqueId
)
{
    NV_STATUS status;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV00F1_CTRL_FABRIC_EVENT event;
    event.id = fabricGenerateEventId_IMPL(pFabric);
    event.type = NV00F1_CTRL_FABRIC_EVENT_TYPE_CLIQUE_ID_CHANGE;
    event.imexChannel = 0;
    event.data.cliqueIdChange.gpuId = pGpu->gpuId;
    event.data.cliqueIdChange.clique = GPU_FABRIC_MAKE_CLIQUE(cliqueType, cliqueId);
    event.data.cliqueIdChange.oldClique = GPU_FABRIC_MAKE_CLIQUE(cliqueType, oldCliqueId);

    status = fabricPostEventsV2(pFabric, &event, 1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU%u Notifying cliqueId change failed\n",
              gpuGetInstance(pGpu));
    }
}

static void
_gpuFabricProbeImexCliqueIdChangeHandler
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32   oldUcPointerCliqueId,
    NvU32   oldUcHandleCliqueId
)
{
    // IMEXd only caches UC cliques, so notify UC clique change
    if (pGpuFabricProbeInfoKernel->ucPointerCliqueId != oldUcPointerCliqueId)
    {
        _gpuFabricProbeSendImexCliqueIdChangeEvent(pGpu, NV_FABRIC_CLIQUE_TYPE_UNICAST_POINTER,
                                                   oldUcPointerCliqueId,
                                                   pGpuFabricProbeInfoKernel->ucPointerCliqueId);
    }

    if (pGpuFabricProbeInfoKernel->ucHandleCliqueId != oldUcHandleCliqueId)
    {
        _gpuFabricProbeSendImexCliqueIdChangeEvent(pGpu, NV_FABRIC_CLIQUE_TYPE_UNICAST_HANDLE,
                                                   oldUcHandleCliqueId,
                                                   pGpuFabricProbeInfoKernel->ucHandleCliqueId);
    }
}

static void
_gpuFabricProbeUpdateSupportedBwModes
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU8 maxRbmLinks;

    if (pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ_V2)
    {
        return; // maxRbmLinks is not supported from GR10x onwards (Probe Response V2)
    }
    else
    {
        maxRbmLinks = pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response.probeRsp.maxRbmLinks;
    }

    if (pKernelNvlink != NULL)
    {
        // Check if maxRbmLinks is within max supported links
        NV_ASSERT_OR_RETURN_VOID(maxRbmLinks <= NVLINK_MAX_LINKS_SW);
        knvlinkSetMaxBWModeLinks(pGpu, pKernelNvlink, maxRbmLinks);
    }
}

static void
_gpuFabricProbeRbmSleepLinks
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS status = NV_OK;
    NVLINK_BIT_VECTOR sleepLinkVec;
    NVLINK_BIT_VECTOR linkMaskToBeReduced;

    // Only put links to sleep if RBM was requested
    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, pGpuFabricProbeInfoKernel->bwMode) != GPU_NVLINK_BW_MODE_LINK_COUNT)
    {
        return;
    }

    // Set links requested in linkMaskToBeReduced to sleep
    NVLINK_BIT_VECTOR *pEnabledLinksVec = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);

    gpuFabricProbeGetlinkMaskToBeReduced(pGpuFabricProbeInfoKernel, &linkMaskToBeReduced);
    bitVectorAnd(&sleepLinkVec, &linkMaskToBeReduced, pEnabledLinksVec);

    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, !bitVectorTestAllCleared(&sleepLinkVec));

    NV_PRINTF(LEVEL_NOTICE, "GPU%u Updating RBM linkmask via probe request: linkMask "NV_BITVECTOR_INLINE_FMTX"\n",
              gpuGetInstance(pGpu), NV_BITVECTOR_INLINE_PRINTF_ARG(&sleepLinkVec));

    status = knvlinkEnterExitSleep(pGpu, pKernelNvlink, &sleepLinkVec, NV_TRUE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting links to sleep on linkmask "NV_BITVECTOR_INLINE_FMTX"\n",
                  NV_BITVECTOR_INLINE_PRINTF_ARG(&sleepLinkVec));
    }
}

/*!
 * @brief Helper function to populate GPU_FABRIC_PROBE_INFO_KERNEL from V1 probe response.
 *
 * @param[in,out] pGpuFabricProbeInfoKernel  Probe info structure to populate
 * @param[in]     pInbandRcvParams           Received inband data parameters
 */
static void
_gpuFabricProbeProcessV1Response
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    nvlink_inband_gpu_probe_rsp_msg_t *pRespV1 =
        (nvlink_inband_gpu_probe_rsp_msg_t *)&pInbandRcvParams->data[0];

    portMemCopy(&pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response,
                sizeof(pGpuFabricProbeInfoKernel->probeResponseMsg.v1Response),
                pRespV1,
                sizeof(*pRespV1));

    pGpuFabricProbeInfoKernel->activeProbeVersion = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ;

    // Populate version-agnostic fields
    pGpuFabricProbeInfoKernel->gpuHandle = pRespV1->probeRsp.gpuHandle;
    pGpuFabricProbeInfoKernel->gfId = pRespV1->probeRsp.gfId;
    portMemCopy(&pGpuFabricProbeInfoKernel->clusterUuid,
                sizeof(pGpuFabricProbeInfoKernel->clusterUuid),
                &pRespV1->probeRsp.clusterUuid,
                sizeof(pRespV1->probeRsp.clusterUuid));
    pGpuFabricProbeInfoKernel->fabricPartitionId = pRespV1->probeRsp.fabricPartitionId;
    pGpuFabricProbeInfoKernel->gpaAddress = pRespV1->probeRsp.gpaAddress;
    pGpuFabricProbeInfoKernel->gpaAddressRange = pRespV1->probeRsp.gpaAddressRange;
    pGpuFabricProbeInfoKernel->flaAddress = pRespV1->probeRsp.flaAddress;
    pGpuFabricProbeInfoKernel->flaAddressRange = pRespV1->probeRsp.flaAddressRange;
    pGpuFabricProbeInfoKernel->egmGpaAddress = (NvU64)pRespV1->probeRsp.gpaAddressEGMHi << 32;
    pGpuFabricProbeInfoKernel->fabricHealthMask = pRespV1->probeRsp.fabricHealthMask;
    pGpuFabricProbeInfoKernel->remapTableIdx = pRespV1->probeRsp.remapTableIdx;
    pGpuFabricProbeInfoKernel->fmCaps0 = pRespV1->probeRsp.fmCaps;
    pGpuFabricProbeInfoKernel->degradedCliqueId = pRespV1->probeRsp.degradedCliqueId;

    if (pRespV1->probeRsp.fmCaps & NVLINK_INBAND_FM_CAPS_MULTI_CLIQUE_SUPPORT)
    {
        if (pRespV1->probeRsp.fmCaps & NVLINK_INBAND_FM_CAPS_UC_HANDLE_EQ_UC_POINTER_CLIQUE)
        {
            pGpuFabricProbeInfoKernel->ucPointerCliqueId = pRespV1->probeRsp.ucHandleCliqueId;
        }
        else
        {
            pGpuFabricProbeInfoKernel->ucPointerCliqueId = pRespV1->probeRsp.cliqueId;
        }

        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pRespV1->probeRsp.ucHandleCliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pRespV1->probeRsp.cliqueId;
    }
    else
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pRespV1->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pRespV1->probeRsp.cliqueId;
    }

    // Convert linkMaskToBeReduced (NvU32 in V1) to bitvector
    nvlinkConvertSplitMasksToBitVector((NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)), pRespV1->probeRsp.linkMaskToBeReduced, 0, 0, 0,
                                 &pGpuFabricProbeInfoKernel->linkMaskToBeReduced);
}

/*!
 * @brief Helper function to populate GPU_FABRIC_PROBE_INFO_KERNEL from V2 probe response.
 *
 * @param[in,out] pGpuFabricProbeInfoKernel  Probe info structure to populate
 * @param[in]     pInbandRcvParams           Received inband data parameters
 */
static void
_gpuFabricProbeProcessV2Response
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    nvlink_inband_gpu_probe_rsp_v2_msg_t *pRespV2 =
        (nvlink_inband_gpu_probe_rsp_v2_msg_t *)&pInbandRcvParams->data[0];

    portMemCopy(&pGpuFabricProbeInfoKernel->probeResponseMsg.v2Response,
                sizeof(pGpuFabricProbeInfoKernel->probeResponseMsg.v2Response),
                pRespV2,
                sizeof(*pRespV2));

    pGpuFabricProbeInfoKernel->activeProbeVersion = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ_V2;

    // Populate version-agnostic fields
    pGpuFabricProbeInfoKernel->gpuHandle = pRespV2->probeRsp.gpuHandle;
    pGpuFabricProbeInfoKernel->gfId = pRespV2->probeRsp.gfId;
    portMemCopy(&pGpuFabricProbeInfoKernel->clusterUuid,
                sizeof(pGpuFabricProbeInfoKernel->clusterUuid),
                &pRespV2->probeRsp.clusterUuid,
                sizeof(pRespV2->probeRsp.clusterUuid));
    pGpuFabricProbeInfoKernel->fabricPartitionId = pRespV2->probeRsp.fabricPartitionId;
    pGpuFabricProbeInfoKernel->gpaAddress = (NvU64)pRespV2->probeRsp.gpaAddressHi << 32;
    pGpuFabricProbeInfoKernel->gpaAddressRange = (NvU64)pRespV2->probeRsp.gpaAddressRangeHi << 32;
    pGpuFabricProbeInfoKernel->flaAddress = (NvU64)pRespV2->probeRsp.flaAddressHi << 32;
    pGpuFabricProbeInfoKernel->flaAddressRange = (NvU64)pRespV2->probeRsp.flaAddressRangeHi << 32;
    pGpuFabricProbeInfoKernel->egmGpaAddress = 0;
    pGpuFabricProbeInfoKernel->fabricHealthMask = pRespV2->probeRsp.fabricHealthMask;
    pGpuFabricProbeInfoKernel->remapTableIdx = pRespV2->probeRsp.remapTableIdx;
    pGpuFabricProbeInfoKernel->fmCaps0 = pRespV2->probeRsp.fmCaps0;
    pGpuFabricProbeInfoKernel->sysmemFlaBaseAddr = (NvU64)pRespV2->probeRsp.uvmAddressStart << 40;
    pGpuFabricProbeInfoKernel->sysmemFlaSize = (NvU64)pRespV2->probeRsp.uvmAddressRange << 40;
    pGpuFabricProbeInfoKernel->degradedCliqueId = pRespV2->probeRsp.degradedCliqueId;

    if (pRespV2->probeRsp.fmCaps0 & NVLINK_INBAND_FM_CAPS_MULTI_CLIQUE_SUPPORT)
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pRespV2->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pRespV2->probeRsp.ucHandleCliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pRespV2->probeRsp.mcPointerCliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pRespV2->probeRsp.mcHandleCliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pRespV2->probeRsp.mcPushCliqueId;
    }
    else
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pRespV2->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pRespV2->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pRespV2->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pRespV2->probeRsp.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pRespV2->probeRsp.cliqueId;
    }

    // Convert linkMaskToBeReduced0/64 to bitvector
    nvlinkConvertSplitMasksToBitVector((NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
                                 pRespV2->probeRsp.linkMaskToBeReduced0,
                                 pRespV2->probeRsp.linkMaskToBeReduced64, 0, 0,
                                 &pGpuFabricProbeInfoKernel->linkMaskToBeReduced);

    // Convert RBM supported link count0/64 to bitvector
    nvlinkConvertSplitMasksToBitVector((NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
                                    pRespV2->probeRsp.rbmSupportedLinkCount0,
                                    pRespV2->probeRsp.rbmSupportedLinkCount64, 0, 0,
                                    &pGpuFabricProbeInfoKernel->rbmSupportedLinkCount);
}

NV_STATUS
gpuFabricProbeOverrideFabricHealthStatus
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32 fabricHealthStatusMask
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    pGpuFabricProbeInfoKernel->fabricHealthMask = fabricHealthStatusMask;

    return NV_OK;
}

NV_STATUS
gpuFabricProbeDegradeCliques
(
    OBJGPU *pGpu
)
{
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;
    NvU32 degradedCliqueId;
    NvU32 oldUcPointerCliqueId;
    NvU32 oldUcHandleCliqueId;
    NV_STATUS status;
    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    degradedCliqueId = pGpuFabricProbeInfoKernel->degradedCliqueId;
    oldUcPointerCliqueId = pGpuFabricProbeInfoKernel->ucPointerCliqueId;
    oldUcHandleCliqueId = pGpuFabricProbeInfoKernel->ucHandleCliqueId;

    pGpuFabricProbeInfoKernel->ucPointerCliqueId = degradedCliqueId;
    pGpuFabricProbeInfoKernel->ucHandleCliqueId = degradedCliqueId;
    pGpuFabricProbeInfoKernel->mcPointerCliqueId = degradedCliqueId;
    pGpuFabricProbeInfoKernel->mcHandleCliqueId = degradedCliqueId;
    pGpuFabricProbeInfoKernel->pushRedCliqueId = degradedCliqueId;

    _gpuFabricProbeImexCliqueIdChangeHandler(pGpu, pGpuFabricProbeInfoKernel,
                                             oldUcPointerCliqueId, oldUcHandleCliqueId);

    return NV_OK;
}

NV_STATUS
gpuFabricProbeReceiveKernelCallback
(
    NvU32 gpuInstance,
    NvU64 *pNotifyGfidMask,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    OBJGPU *pGpu;
    NvU32 gpuMaskUnused;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink;
    NV_STATUS status;
    nvlink_inband_msg_header_t *pRespMsgHdr;

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // There is a scenario where _gpuFabricProbeStart fails in the GSP
    // and returns failure to kernel ctrl call to start probe.
    // This will set the pGpuFabricProbeInfoKernel to NULL.
    // GSP also sends a probe response with failure error code.
    // Handling this response causes kernel driver to crash since
    // pGpuFabricProbeInfoKernel is already cleared in the kernel.
    // This check is added to handle this scenario.
    //
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu->pGpuFabricProbeInfoKernel != NULL, NV_OK);

    NV_ASSERT(rmGpuGroupLockIsOwner(gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                    &gpuMaskUnused));

    NV_ASSERT(pInbandRcvParams != NULL);

    pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

    pRespMsgHdr = (nvlink_inband_msg_header_t *)&pInbandRcvParams->data[0];

    // Version-aware response handling based on message type
    if (pRespMsgHdr->type == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP_V2)
    {
        _gpuFabricProbeProcessV2Response(pGpuFabricProbeInfoKernel, pInbandRcvParams);

        if (pGpuFabricProbeInfoKernel->fmCaps0 & NVLINK_INBAND_GPU_PROBE_CAPS_NON_DISRUPTIVE_LINK_MASK_CHANGE)
        {
            knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
                    NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
                    NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
                    NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_FALSE);
        }
    }
    else
    {
        _gpuFabricProbeProcessV1Response(pGpuFabricProbeInfoKernel, pInbandRcvParams);
    }

    gpuFabricProbeSyncFabricProbeInfo(pGpuFabricProbeInfoKernel);

    portAtomicMemoryFenceFull();

    // If the probe failed and the BW mode is in progress, set the BW mode status to error
    if (pKernelNvlink != NULL &&
        knvlinkGetBWModeStatus(pGpu, pKernelNvlink) == NVLINK_BW_MODE_STATUS_IN_PROGRESS)
    {
        NvBool bSuccess = gpuFabricProbeIsSuccess(pGpuFabricProbeInfoKernel);
        KNVLINK_SET_BW_MODE_STATUS(pKernelNvlink, bSuccess ? NVLINK_BW_MODE_STATUS_COMPLETED : NVLINK_BW_MODE_STATUS_ERROR_PROBE_FAILED);
    }

    portAtomicSetU32(&pGpuFabricProbeInfoKernel->probeRespRcvd, 1);

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);



    NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

    _gpuFabricProbeSetupGpaRange(pGpu, pGpuFabricProbeInfoKernel);
    _gpuFabricProbeSetupFlaRange(pGpu, pGpuFabricProbeInfoKernel);
    _gpuFabricProbeSetupSysmemFlaRange(pGpu, pGpuFabricProbeInfoKernel);

    // Update supported bandwidth modes from probe response
    _gpuFabricProbeUpdateSupportedBwModes(pGpu, pGpuFabricProbeInfoKernel);

    _gpuFabricProbeCheckResetRequired(pGpu, pGpuFabricProbeInfoKernel->fabricHealthMask);

    if (pKernelNvlink != NULL)
    {
        if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
        {
            // Set links to sleep based on probe response
            _gpuFabricProbeRbmSleepLinks(pGpu, pGpuFabricProbeInfoKernel);
        }
    }

    return NV_OK;
}

/*
 * @brief Wake all links in the given link mask for ABM
 *
 * @param[in] pGpu                      GPU instance
 * @param[in] pGpuFabricProbeInfoKernel Probe info structure
 * @param[in] pKernelNvlink            Kernel NVLink instance
 * @param[in] pLinkMask                 Link mask to wake for ABM
 */
static void
_gpuFabricProbeAbmWakeLinks
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    KernelNvlink *pKernelNvlink,
    NVLINK_BIT_VECTOR *pLinkMask
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS powerStatusParams = {0};
    NV_STATUS status;
    NVLINK_BIT_VECTOR wakeLinkVec = {0};
    NvU32 i;

    // Check if the links are asleep
    FOR_EACH_IN_BITVECTOR(pLinkMask, i)
    {
        powerStatusParams.linkId = i;
        NV_CHECK_OK(status, LEVEL_INFO,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_NVLINK_GET_POWER_STATE,
                            &powerStatusParams, sizeof(powerStatusParams)));

        // Links that are down will return a status of NV_ERR_INVALID_STATE, ignore them
        if (status == NV_OK &&
            powerStatusParams.powerState == NV2080_CTRL_NVLINK_POWER_STATE_L2)
        {
            NV_PRINTF(LEVEL_NOTICE, "GPU%u Waking link %u\n",
                pGpu->gpuInstance, i);
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, NV_OK == bitVectorSet(&wakeLinkVec, i));
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    if (!bitVectorTestAllCleared(&wakeLinkVec))
    {
        // Wake all sleeping links
        status = knvlinkEnterExitSleep(pGpu, pKernelNvlink, &wakeLinkVec, NV_FALSE);
        if (status != NV_OK)
        {

            NV_PRINTF(LEVEL_ERROR, "Error waking links on linkmask "NV_BITVECTOR_INLINE_FMTX"\n",
                NV_BITVECTOR_INLINE_PRINTF_ARG(&wakeLinkVec));
        }
    }
}

static void
_gpuFabricProbeAbmWakeLinks_WORKITEM
(
    NvU32 gpuInstance,
    void *pData
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    ABM_WAKE_LINKS_WORKITEM_DATA *pCallbackData = (ABM_WAKE_LINKS_WORKITEM_DATA *)pData;
    KernelNvlink *pKernelNvlink;

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, ((pGpu != NULL) && (pCallbackData != NULL)));

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, ((pGpu->pGpuFabricProbeInfoKernel != NULL) && (pKernelNvlink != NULL)));

    _gpuFabricProbeAbmWakeLinks(pGpu, pGpu->pGpuFabricProbeInfoKernel,
                                pKernelNvlink, &pCallbackData->linkMask);
}

static void
_gpuFabricProbeUpdateABMLinkMask
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU8    action,
    NVLINK_BIT_VECTOR *pLinkMask
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvBool bNeedsRCRecovery = NV_FALSE;
    NVLINK_BIT_VECTOR pendingAbmLinkMaskToBeReduced;
    NVLINK_BIT_VECTOR invertedLinkMask;
    NV_STATUS status;

    if (pKernelNvlink == NULL)
        return;

    NVLINK_BIT_VECTOR *pEnabledLinksVec = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    bitVectorClrAll(&invertedLinkMask);
    bitVectorCopy(&invertedLinkMask, pLinkMask);
    bitVectorInvAll(&invertedLinkMask);
    bitVectorAnd(&pendingAbmLinkMaskToBeReduced, pEnabledLinksVec, &invertedLinkMask);


    //
    //  If RBM is enabled and the pending ABM linkMaskToBeReduced is not equal to the current linkMaskToBeReduced,
    //  then wake all the sleeping links that are not part of the pending ABM linkMaskToBeReduced
    //
    if (knvlinkGetBWMode(pGpu, pKernelNvlink) != 0 &&
        !bitVectorTestEqual(&pendingAbmLinkMaskToBeReduced, &pGpuFabricProbeInfoKernel->linkMaskToBeReduced))
    {
        ABM_WAKE_LINKS_WORKITEM_DATA *pCallbackData =
            (ABM_WAKE_LINKS_WORKITEM_DATA *)portMemAllocNonPaged(sizeof(ABM_WAKE_LINKS_WORKITEM_DATA));

        if (pCallbackData == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                "Failed to alloc ABM wake links workitem data\n");
        }
        else
        {
            bitVectorClrAll(&pCallbackData->linkMask);
            bitVectorCopy(&pCallbackData->linkMask, &pendingAbmLinkMaskToBeReduced);
            bitVectorInvAll(&pCallbackData->linkMask);
            bitVectorAnd(&pCallbackData->linkMask,  &pGpuFabricProbeInfoKernel->linkMaskToBeReduced, &pCallbackData->linkMask);

            status = osQueueWorkItem(pGpu,
                _gpuFabricProbeAbmWakeLinks_WORKITEM,
                (void *)pCallbackData,
                (OsQueueWorkItemFlags){
                    .bLockSema = NV_TRUE,
                    .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                    .bLockGpuGroupDevice = NV_TRUE});

            if (status != NV_OK)
            {
                portMemFree(pCallbackData);
                NV_PRINTF(LEVEL_ERROR,
                    "Failed to queue ABM wake links workitem: 0x%x\n",
                    status);
            }

            // Clear out the BW mode
            knvlinkSetBWMode(pGpu, pKernelNvlink, 0U);
            pGpu->pGpuFabricProbeInfoKernel->bwMode = 0U;
        }
    }

    if ((action != NVLINK_INBAND_GPU_PROBE_UPDATE_ACTION_CHANGE_AMAP) &&
        (action != NVLINK_INBAND_GPU_PROBE_UPDATE_ACTION_CHANGE_AMAP_AND_QUIESCE))
    {
        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
        return;
    }

    // Store linkmask to be updated after P2P is idle
    knvlinkSetPendingAbmLinkMaskToBeReduced(pGpu, pKernelNvlink, &pendingAbmLinkMaskToBeReduced);

    NV_PRINTF(LEVEL_NOTICE, "GPU%u Queing ABM linkmask via probe update: linkMask="NV_BITVECTOR_INLINE_FMTX"\n",
                pGpu->gpuInstance, NV_BITVECTOR_INLINE_PRINTF_ARG(pLinkMask));

    bNeedsRCRecovery = (action == NVLINK_INBAND_GPU_PROBE_UPDATE_ACTION_CHANGE_AMAP_AND_QUIESCE);
    knvlinkABMLinkMaskUpdate(pGpu, pKernelNvlink, bNeedsRCRecovery);
}

/*!
 * @brief Process V1 probe update message and extract common fields.
 *
 * @param[in]     pGpuFabricProbeInfoKernel  Probe info structure
 * @param[in]     pInbandRcvParams           Received inband data parameters
 * @param[out]    pOldUcPointerCliqueId      Old pointer clique ID that will be updated
 * @param[out]    pOldUcHandleCliqueId       Old handle clique ID that will be updated
 * @param[out]    pUpdateAction              Extracted action from update
 * @param[in]     bEnableABM                 Whether ABM is enabled
 * @param[out]    pEnabledLinkMask           Extracted enabled link mask (if ABM enabled)
 *
 * @return NV_OK on success, error code otherwise
 */
static NV_STATUS
_gpuFabricProbeProcessUpdateV1
(
    GPU_FABRIC_PROBE_INFO_KERNEL                   *pGpuFabricProbeInfoKernel,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams,
    NvU32                                          *pOldUcPointerCliqueId,
    NvU32                                          *pOldUcHandleCliqueId,
    NvU8                                           *pUpdateAction,
    NvBool                                          bEnableABM,
    NVLINK_BIT_VECTOR                              *pEnabledLinkMask
)
{
    NvU64 fmCaps;
    nvlink_inband_gpu_probe_update_req_msg_t *pUpdateV1 =
        (nvlink_inband_gpu_probe_update_req_msg_t *)&pInbandRcvParams->data[0];

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        gpuFabricProbeGetfmCaps(pGpuFabricProbeInfoKernel, &fmCaps));

    *pUpdateAction = pUpdateV1->probeUpdate.action;
    pGpuFabricProbeInfoKernel->fabricHealthMask = pUpdateV1->probeUpdate.fabricHealthMask;

    *pOldUcPointerCliqueId = pGpuFabricProbeInfoKernel->ucPointerCliqueId;
    *pOldUcHandleCliqueId = pGpuFabricProbeInfoKernel->ucHandleCliqueId;

    if (fmCaps & NVLINK_INBAND_FM_CAPS_MULTI_CLIQUE_SUPPORT)
    {
        if (fmCaps & NVLINK_INBAND_FM_CAPS_UC_HANDLE_EQ_UC_POINTER_CLIQUE)
        {
            pGpuFabricProbeInfoKernel->ucPointerCliqueId = pUpdateV1->probeUpdate.ucHandleCliqueId;
        }
        else
        {
            pGpuFabricProbeInfoKernel->ucPointerCliqueId = pUpdateV1->probeUpdate.cliqueId;
        }

        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pUpdateV1->probeUpdate.ucHandleCliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pUpdateV1->probeUpdate.cliqueId;
    }
    else
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pUpdateV1->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pUpdateV1->probeUpdate.cliqueId;
    }

    if (bEnableABM)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            nvlinkConvertSplitMasksToBitVector(
                (NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
                pUpdateV1->probeUpdate.linkMask, 0, 0, 0,
                pEnabledLinkMask));
    }

    return NV_OK;
}

/*!
 * @brief Process V2 probe update message and extract common fields.
 *
 * @param[in]     pGpuFabricProbeInfoKernel  Probe info structure
 * @param[in]     pInbandRcvParams           Received inband data parameters
 * @param[out]    pOldUcPointerCliqueId      Old pointer clique ID that will be updated
 * @param[out]    pOldUcHandleCliqueId       Old handle clique ID that will be updated
 * @param[out]    pUpdateAction              Extracted action from update
 * @param[in]     bEnableABM                 Whether ABM is enabled
 * @param[out]    pEnabledLinkMask           Extracted enabled link mask (if ABM enabled)
 *
 * @return NV_OK on success, error code otherwise
 */
static NV_STATUS
_gpuFabricProbeProcessUpdateV2
(
    GPU_FABRIC_PROBE_INFO_KERNEL                   *pGpuFabricProbeInfoKernel,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams,
    NvU32                                          *pOldUcPointerCliqueId,
    NvU32                                          *pOldUcHandleCliqueId,
    NvU8                                           *pUpdateAction,
    NvBool                                          bEnableABM,
    NVLINK_BIT_VECTOR                              *pEnabledLinkMask
)
{
    NvU64 fmCaps;
    nvlink_inband_gpu_probe_update_req_v2_msg_t *pUpdateV2 =
        (nvlink_inband_gpu_probe_update_req_v2_msg_t *)&pInbandRcvParams->data[0];

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        gpuFabricProbeGetfmCaps(pGpuFabricProbeInfoKernel, &fmCaps));

    *pUpdateAction = pUpdateV2->probeUpdate.action;
    pGpuFabricProbeInfoKernel->fabricHealthMask = pUpdateV2->probeUpdate.fabricHealthMask;

    *pOldUcPointerCliqueId = pGpuFabricProbeInfoKernel->ucPointerCliqueId;
    *pOldUcHandleCliqueId = pGpuFabricProbeInfoKernel->ucHandleCliqueId;

    if (fmCaps & NVLINK_INBAND_FM_CAPS_MULTI_CLIQUE_SUPPORT)
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pUpdateV2->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pUpdateV2->probeUpdate.ucHandleCliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pUpdateV2->probeUpdate.mcPointerCliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pUpdateV2->probeUpdate.mcHandleCliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pUpdateV2->probeUpdate.mcPushCliqueId;
    }
    else
    {
        pGpuFabricProbeInfoKernel->ucPointerCliqueId = pUpdateV2->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->ucHandleCliqueId = pUpdateV2->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->mcPointerCliqueId = pUpdateV2->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->mcHandleCliqueId = pUpdateV2->probeUpdate.cliqueId;
        pGpuFabricProbeInfoKernel->pushRedCliqueId = pUpdateV2->probeUpdate.cliqueId;
    }

    if (bEnableABM)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            nvlinkConvertSplitMasksToBitVector(
                (NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
                pUpdateV2->probeUpdate.enabledLinkMask0,
                pUpdateV2->probeUpdate.enabledLinkMask64, 0, 0,
                pEnabledLinkMask));
    }

    // Update RBM supported link count
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        nvlinkConvertSplitMasksToBitVector((NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
                                    pUpdateV2->probeUpdate.rbmSupportedLinkCount0,
                                    pUpdateV2->probeUpdate.rbmSupportedLinkCount64, 0, 0,
                                    &pGpuFabricProbeInfoKernel->rbmSupportedLinkCount));

    return NV_OK;
}

NV_STATUS
gpuFabricProbeReceiveUpdateKernelCallback
(
    NvU32 gpuInstance,
    NvU64 *pNotifyGfidMask,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    OBJGPU *pGpu;
    NvU32 gpuMaskUnused;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink;
    NvBool bEnableABM;
    NV_STATUS status = NV_OK;
    nvlink_inband_msg_header_t *pUpdateMsgHdr;
    NvU32 oldUcPointerCliqueId = 0;
    NvU32 oldUcHandleCliqueId = 0;
    NvU8 updateAction = 0;
    NVLINK_BIT_VECTOR enabledLinkMask;

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelNvlink != NULL, NV_ERR_INVALID_STATE);

    bEnableABM = knvlinkGetAbmEnabled(pGpu, pKernelNvlink);

    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pGpu->pGpuFabricProbeInfoKernel != NULL,
                        NV_ERR_INVALID_STATE, done);

    NV_ASSERT(rmGpuGroupLockIsOwner(gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                    &gpuMaskUnused));

    NV_ASSERT(pInbandRcvParams != NULL);

    pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel), done);

    // Get message header to determine version
    pUpdateMsgHdr = (nvlink_inband_msg_header_t *)&pInbandRcvParams->data[0];

    // Version-aware update handling - dispatch to version-specific processor
    if ((pUpdateMsgHdr->type == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_UPDATE_REQ_V2) &&
        (pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ_V2))
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _gpuFabricProbeProcessUpdateV2(pGpuFabricProbeInfoKernel,
                     pInbandRcvParams, &oldUcPointerCliqueId, &oldUcHandleCliqueId, &updateAction,
                     bEnableABM, &enabledLinkMask), done);
    }
    else if ((pUpdateMsgHdr->type == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_UPDATE_REQ) &&
             (pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ))
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _gpuFabricProbeProcessUpdateV1(pGpuFabricProbeInfoKernel,
                     pInbandRcvParams, &oldUcPointerCliqueId, &oldUcHandleCliqueId, &updateAction,
                     bEnableABM, &enabledLinkMask), done);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
            "GPU%u Probe update version mismatch: activeProbeVersion=0x%x, updateMsgType=0x%x\n",
            gpuGetInstance(pGpu), pGpuFabricProbeInfoKernel->activeProbeVersion, pUpdateMsgHdr->type);

        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
        return NV_OK;
    }

    gpuFabricProbeSyncFabricProbeInfo(pGpuFabricProbeInfoKernel);

    _gpuFabricProbeImexCliqueIdChangeHandler(pGpu, pGpuFabricProbeInfoKernel,
                                             oldUcPointerCliqueId, oldUcHandleCliqueId);

    // Support for fabric attributes enhancements (Bug: 5345385)
    if (updateAction == NVLINK_INBAND_GPU_PROBE_UPDATE_ACTION_PROBE_REQUEST_NEEDED)
    {
        if (pKernelNvlink != NULL)
        {
            knvlinkTriggerProbeRequest(pGpu, pKernelNvlink);
        }
    }

    // Check if fabric health requires GPU reset
    _gpuFabricProbeCheckResetRequired(pGpu, pGpuFabricProbeInfoKernel->fabricHealthMask);

    if (bEnableABM)
    {
        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_TRUE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_FALSE);
        _gpuFabricProbeUpdateABMLinkMask(pGpu,
                                         pGpuFabricProbeInfoKernel,
                                         updateAction,
                                         &enabledLinkMask);
    }
done:
    if (bEnableABM && (status != NV_OK))
    {
        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
    }
    return status;
}

NV_STATUS
gpuFabricReceiveGpuGetCurrentStateRequestKernelCallback
(
    NvU32 gpuInstance,
    NvU64 *pNotifyGfidMask,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    OBJGPU *pGpu;
    NvU32 gpuMaskUnused;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink;
    NV_STATUS status;
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *sendDataParams;
    nvlink_inband_gpu_get_current_state_req_msg_t *pGpuGetCurrentStateReqMsg = NULL;
    nvlink_inband_gpu_get_current_state_rsp_msg_t *pGpuGetCurrentStateRspMsg = NULL;
    nvlink_inband_gpu_get_current_state_rsp_t *pGpuGetCurrentStateRsp = NULL;
    NvU32 payloadSize;
    NvU32 sendDataSize;
    NVLINK_BIT_VECTOR *pEnabledLinkMask = NULL;

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelNvlink != NULL, NV_OK);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu->pGpuFabricProbeInfoKernel != NULL, NV_OK);

    NV_ASSERT(rmGpuGroupLockIsOwner(gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                    &gpuMaskUnused));

    NV_ASSERT(pInbandRcvParams != NULL);

    pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

    pGpuGetCurrentStateReqMsg = (nvlink_inband_gpu_get_current_state_req_msg_t *)&pInbandRcvParams->data[0];

    // Setup send data params
    sendDataParams = (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));
    if (sendDataParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Out of memory, Dropping GPU_GET_CURRENT_STATE_RESPONSE message\n");
        return NV_ERR_NO_MEMORY;
    }
    payloadSize = (NvU32)(sizeof(nvlink_inband_gpu_get_current_state_rsp_t));
    sendDataSize = (NvU32)(sizeof(nvlink_inband_msg_header_t) + payloadSize);
    if (sendDataSize > sizeof(sendDataParams->buffer))
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }
    portMemSet(sendDataParams, 0, sendDataSize);
    sendDataParams->dataSize = sendDataSize;

    // Populate GPU_GET_CURRENT_STATE_RESPONSE message
    pGpuGetCurrentStateRspMsg = (nvlink_inband_gpu_get_current_state_rsp_msg_t *)&sendDataParams->buffer[0];
    pGpuGetCurrentStateRsp = (nvlink_inband_gpu_get_current_state_rsp_t *)&pGpuGetCurrentStateRspMsg->currStateRsp;

    pGpuGetCurrentStateRsp->gpuHandle = pGpuGetCurrentStateReqMsg->currStateReq.gpuHandle;
    pGpuGetCurrentStateRsp->supportedFieldMask0 = pGpuFabricProbeInfoKernel->fmCaps0;


    // Enabled link mask for this messages is the effective peer link mask RM will use for P2P traffic.
    pEnabledLinkMask = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    if (pEnabledLinkMask != NULL)
    {
        NVLINK_BIT_VECTOR effectivePeerLinkMask = { 0 };
        bitVectorCopy(&effectivePeerLinkMask, pEnabledLinkMask);
        knvlinkGetEffectivePeerLinkMask_HAL(pGpu, pKernelNvlink, NULL, &effectivePeerLinkMask);
        NV_PRINTF(LEVEL_INFO, "Enabled link mask: "NV_BITVECTOR_INLINE_FMTX"\n", NV_BITVECTOR_INLINE_PRINTF_ARG(&effectivePeerLinkMask));
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            nvlinkConvertBitVectorToSplitMasks(&effectivePeerLinkMask,
            (NV2080_CTRL_NVLINK_MAX_LINKS / NV_NBITS_IN_TYPE(NvU64)),
            &pGpuGetCurrentStateRsp->enabledLinkMask0,
            &pGpuGetCurrentStateRsp->enabledLinkMask64,
            NULL, NULL),
            done);
    }

    // sleepLinkMask0/64 is not populated. Reserved for future use.

    pGpuGetCurrentStateRsp->fabricHealthMask = pGpuFabricProbeInfoKernel->fabricHealthMask;
    pGpuGetCurrentStateRsp->gpuStateFlags = knvlinkGetAmapUpdateStatus(pGpu, pKernelNvlink);
    pGpuGetCurrentStateRsp->cliqueId = pGpuFabricProbeInfoKernel->ucPointerCliqueId;
    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, pGpuFabricProbeInfoKernel->bwMode) == GPU_NVLINK_BW_MODE_LINK_COUNT)
    {
        pGpuGetCurrentStateRsp->rbmLinkCount = (NvU8)DRF_VAL(_GPU, _NVLINK, _BW_MODE_LINK_COUNT, pGpuFabricProbeInfoKernel->bwMode);
    }

    nvlinkInitInbandMsgHdr(&pGpuGetCurrentStateRspMsg->msgHdr,
                            NVLINK_INBAND_MSG_TYPE_GPU_GET_CURRENT_STATE_RSP,
                            payloadSize, pGpuGetCurrentStateReqMsg->msgHdr.requestId);

    status = knvlinkSendInbandData(pGpu, pKernelNvlink, sendDataParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to send NVLINK_INBAND_MSG_TYPE_GPU_GET_CURRENT_STATE_RSP message\n");
        goto done;
    }

done:
    portMemFree(sendDataParams);
    return status;
}

void
gpuFabricProbeSyncFabricProbeInfo
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    OBJGPU *pGpu;
    RM_API *pRmApi;
    NV_STATUS status;
    NV2080_CTRL_CMD_INTERNAL_GPU_SYNC_FABRIC_PROBE_INFO_PARAMS params = { 0 };

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    NV_CHECK_OK(status, LEVEL_ERROR,
        convertBitVectorToLinkMasks(&pGpuFabricProbeInfoKernel->linkMaskToBeReduced,
                                    NULL, 0, &params.linkMaskToBeReduced));

    NV_CHECK_OK(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_SYNC_FABRIC_PROBE_INFO,
                            &params,
                            sizeof(params)));
}

void
gpuFabricProbeSuspend
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    OBJGPU *pGpu;
    RM_API *pRmApi;
    NV_STATUS status;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    NV_CHECK_OK(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_SUSPEND_FABRIC_PROBE,
                            NULL, 0));
}

void
gpuFabricProbeInvalidate
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    OBJGPU *pGpu;
    RM_API *pRmApi;
    NV_STATUS status;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    NV_CHECK_OK(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE,
                            NULL, 0));

    _gpuFabricProbeInvalidate(pGpu);
}

NV_STATUS
gpuFabricProbeResume
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    OBJGPU *pGpu;
    RM_API *pRmApi;
    NV2080_CTRL_CMD_INTERNAL_RESUME_GPU_FABRIC_PROBE_INFO_PARAMS params = { 0 };

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    params.bwMode = pGpuFabricProbeInfoKernel->bwMode;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
              pRmApi->Control(pRmApi,
                              pGpu->hInternalClient,
                              pGpu->hInternalSubdevice,
                              NV2080_CTRL_CMD_INTERNAL_GPU_RESUME_FABRIC_PROBE,
                              &params, sizeof(params)));

    return NV_OK;
}

NV_STATUS
gpuFabricProbeStart
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL **ppGpuFabricProbeInfoKernel
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS status = NV_OK;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS params = { 0 };
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32 strLength = 32;
    NvU8 pStr[32];
    NvU32 linkCount;

    NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)),
        NV_ERR_INVALID_LOCK_STATE);

    // Check if NVSwitch based system. If not return without doing anything
    if (!gpuFabricProbeIsSupported(pGpu))
    {
        return NV_OK;
    }

    *ppGpuFabricProbeInfoKernel =
                portMemAllocNonPaged(sizeof(*pGpuFabricProbeInfoKernel));
    NV_ASSERT_OR_RETURN(*ppGpuFabricProbeInfoKernel != NULL, NV_ERR_NO_MEMORY);

    pGpuFabricProbeInfoKernel = *ppGpuFabricProbeInfoKernel;

    portMemSet(pGpuFabricProbeInfoKernel, 0, sizeof(*pGpuFabricProbeInfoKernel));

    pGpuFabricProbeInfoKernel->pGpu = pGpu;
    params.bLocalEgmEnabled = pMemoryManager->bLocalEgmEnabled;

    if (pGpuMgr->bwModeScope == GPU_NVLINK_BW_MODE_SCOPE_PER_GPU)
    {
        if (pKernelNvlink != NULL)
        {
            pGpuFabricProbeInfoKernel->bwMode = knvlinkGetBWMode(pGpu, pKernelNvlink);
        }
    }
    else
    {
        pGpuFabricProbeInfoKernel->bwMode = gpumgrGetGpuNvlinkBwMode();
    }
    params.bwMode = pGpuFabricProbeInfoKernel->bwMode;

    // pKernelNvlink is allowed to be NULL for vGPU case to send probe via RPC. Skip RBM for this case.
    if (pKernelNvlink != NULL)
    {
        // RBM requested via regkey. Wake the links in this path if LINKCOUNT or FULL is set.
        if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_NVLINK_BW_LINK_COUNT, &linkCount) == NV_OK) ||
            ((osReadRegistryString(pGpu, NV_REG_STR_RM_NVLINK_BW, pStr, &strLength) == NV_OK) &&
            (DRF_VAL(_GPU, _NVLINK, _BW_MODE, params.bwMode) == GPU_NVLINK_BW_MODE_FULL)))
        {
            if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
            {
                _gpuFabricProbeRbmWakeLinks(pGpu, pGpuFabricProbeInfoKernel, NV_TRUE, params.bwMode);
            }
        }
    }

    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                           NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE,
                           &params, sizeof(params),
                           status);

        if (status != NV_OK)
            goto fail;
    }
    else
    {
        // Send IOCTL to start probe
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE,
                                &params, sizeof(params)),
                fail);
    }

    return NV_OK;

fail:
    portMemFree(pGpuFabricProbeInfoKernel);
    pGpu->pGpuFabricProbeInfoKernel = NULL;

    return status;
}

void
gpuFabricProbeStop
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    OBJGPU *pGpu;
    RM_API *pRmApi;
    MemoryManager *pMemoryManager;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;
    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    memmgrDestroySysmemFlaWindowForUvm_HAL(pGpu, pMemoryManager);

    if (!IS_VIRTUAL(pGpu))
    {
        //
        // On VGPU, we have to stop the probe for guest driver unload as well as for guest clean/forced shutdown/reboot.
        // An RPC from this point will not be triggered for forced shutdown/reboot.
        // vmioplugin already has callbacks for guest driver unload, guest shutdown and guest reboot.
        // Hence it is simpler to handle the probe stop directly from these callbacks in plugin instead of a separate
        // RPC for this RM control.
        //

        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                    pGpu->hInternalClient,
                                    pGpu->hInternalSubdevice,
                                    NV2080_CTRL_CMD_INTERNAL_GPU_STOP_FABRIC_PROBE,
                                    NULL, 0));
    }

    portMemFree(pGpuFabricProbeInfoKernel);
    pGpu->pGpuFabricProbeInfoKernel = NULL;
}

NvBool
gpuFabricProbeIsSupported
(
    OBJGPU *pGpu
)
{
    if (pGpu->fabricProbeRetryDelay == 0)
    {
        NV_PRINTF(LEVEL_INFO, "GPU%u Probe handling is disabled\n",
                  gpuGetInstance(pGpu));
        return NV_FALSE;
    }

    if (!IS_VIRTUAL(pGpu) && (GPU_GET_KERNEL_NVLINK(pGpu) == NULL))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

#define GPU_FABRIC_CHECK_BW_MODE(fmCaps, mode)                         \
    do                                                                 \
    {                                                                  \
        if (((fmCaps) & (NVLINK_INBAND_FM_CAPS_BW_MODE_##mode)) == 0)  \
            return NV_ERR_NOT_SUPPORTED;                               \
    } while (0)

NV_STATUS
gpuFabricProbeSetBwModePerGpu
(
    OBJGPU *pGpu,
    NvU16 mode,
    NvBool bSync
)
{
    NV_STATUS status = NV_OK;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel =
                                    pGpu->pGpuFabricProbeInfoKernel;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU16 *pMode;

    // Function assumes caller has checked requested mode is supported
    pGpuFabricProbeInfoKernel->bwMode = mode;


    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
          pRmApi->Control(pRmApi,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE,
                          NULL, 0));

    _gpuFabricProbeInvalidate(pGpu);

    gpuFabricProbeSuspend(pGpuFabricProbeInfoKernel);

    if (bSync)
    {
        // Need to wake links if LINK_COUNT or FULL BW mode is requested before probe update
        if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_LINK_COUNT) ||
            (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL))
        {
            if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
            {
                _gpuFabricProbeRbmWakeLinks(pGpu, pGpuFabricProbeInfoKernel, NV_TRUE, mode);
                knvlinkSetBWMode(pGpu, pKernelNvlink, mode);
            }
        }

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuFabricProbeResume(pGpuFabricProbeInfoKernel));

        return NV_OK;
    }
    else
    {
        pMode = (NvU16 *)portMemAllocNonPaged(sizeof(NvU16));
        if (pMode == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }
        *pMode = mode;

        KNVLINK_SET_BW_MODE_STATUS(pKernelNvlink, NVLINK_BW_MODE_STATUS_IN_PROGRESS);

        osQueueWorkItem(pGpu,
            _gpuFabricProbeRbmSetCallback,
            (void *)pMode,
            (OsQueueWorkItemFlags){
                .bLockSema = NV_FALSE,
                .apiLock = WORKITEM_FLAGS_API_LOCK_READ_ONLY,
                .bLockGpuGroupDevice = NV_TRUE});
    }

    return status;
}

NV_STATUS
gpuFabricProbeSetBwMode
(
    NvU16 mode,
    NvBool bSync
)
{
    NvU32 attachedGpuCount;
    NvU32 attachedGpuMask;
    NV_STATUS status;
    NvU32 gpuIndex;
    OBJGPU *pGpu;
    KernelNvlink *pKernelNvlink;

    status = gpumgrGetGpuAttachInfo(&attachedGpuCount, &attachedGpuMask);
    if (status != NV_OK)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Check if all GPUs belong to NvSwitch
    gpuIndex = 0;
    for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
        pGpu != NULL;
        pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        if (!gpuFabricProbeIsSupported(pGpu))
        {
            // For directed connected system
            return NV_OK;
        }
    }

    //
    // Check if all GPUs received fabric probe and
    //       if the mode is supported on all GPUs.
    //
    gpuIndex = 0;
    for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
        pGpu != NULL;
        pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        if (!gpuFabricProbeIsReceived(pGpu->pGpuFabricProbeInfoKernel) ||
            !gpuFabricProbeIsSuccess(pGpu->pGpuFabricProbeInfoKernel))
        {
            return NV_ERR_NOT_READY;
        }

        pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        // Check if BW mode is in progress
        if (knvlinkGetBWModeStatus(pGpu, pKernelNvlink) == NVLINK_BW_MODE_STATUS_IN_PROGRESS)
        {
            NV_PRINTF(LEVEL_ERROR, "BW mode is in progress. Mode: %d\n", mode);
            return NV_ERR_BUSY_RETRY;
        }

        // Confirm requested BW mode is supported by GPU
        if (!knvlinkIsBwModeSupported_HAL(pGpu, pKernelNvlink, mode))
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        // These checks only apply to legacy bw mode settings
        if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) !=
            GPU_NVLINK_BW_MODE_LINK_COUNT)
        {
            NvU64 fmCaps;
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel, &fmCaps));
            switch(mode)
            {
                case GPU_NVLINK_BW_MODE_MIN:
                    GPU_FABRIC_CHECK_BW_MODE(fmCaps, MIN);
                    break;
                case GPU_NVLINK_BW_MODE_HALF:
                    GPU_FABRIC_CHECK_BW_MODE(fmCaps, HALF);
                    break;
                case GPU_NVLINK_BW_MODE_3QUARTER:
                    GPU_FABRIC_CHECK_BW_MODE(fmCaps, 3QUARTER);
                    break;
                case GPU_NVLINK_BW_MODE_OFF:
                    return NV_OK; // Don't need to ask FM
                default:
                    break;
            }
        }
    }

    gpuIndex = 0;
    for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
        pGpu != NULL;
        pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS,
                                                 FABRIC_VASPACE);
        if (pFabricVAS == NULL)
        {
            continue;
        }

        if (fabricvaspaceIsInUse(pFabricVAS))
        {
            return NV_ERR_STATE_IN_USE;
        }
    }

    gpuIndex = 0;
    for(pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex);
        pGpu != NULL;
        pGpu = gpumgrGetNextGpu(attachedGpuMask, &gpuIndex))
    {
        status = gpuFabricProbeSetBwModePerGpu(pGpu, mode, bSync);
        if (status != NV_OK)
        {
            return status;
        }
    }

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetlinkMaskToBeReduced
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NVLINK_BIT_VECTOR *pLinkMaskToBeReduced
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    // Return the pre-converted bitvector
    bitVectorCopy(pLinkMaskToBeReduced, &pGpuFabricProbeInfoKernel->linkMaskToBeReduced);

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetSupportedBwModes
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NVLINK_BIT_VECTOR *pSupportedBwModes
)
{
    NV_STATUS status = NV_OK;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpuFabricProbeInfoKernel->activeProbeVersion == NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ_V2, NV_ERR_NOT_SUPPORTED);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pSupportedBwModes != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorClrAll(pSupportedBwModes);
    bitVectorCopy(pSupportedBwModes, &pGpuFabricProbeInfoKernel->rbmSupportedLinkCount);

    return status;
}

NV_STATUS
gpuFabricProbeSetlinkMaskToBeReduced
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NVLINK_BIT_VECTOR *pLinkMaskToBeReduced
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    bitVectorCopy(&pGpuFabricProbeInfoKernel->linkMaskToBeReduced, pLinkMaskToBeReduced);

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetRemapTableIndex
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32 *pRemapTableIdx
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pRemapTableIdx = pGpuFabricProbeInfoKernel->remapTableIdx;

    return NV_OK;
}
