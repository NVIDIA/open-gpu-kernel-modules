/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    volatile NvU32 probeRespRcvd;
    NvU8 bwMode;

    OBJGPU *pGpu;

    nvlink_inband_gpu_probe_rsp_msg_t probeResponseMsg;

} GPU_FABRIC_PROBE_INFO_KERNEL;

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
        NV_PRINTF(LEVEL_ERROR, "Fabric Probe failed: 0x%x\n",
                  pGpuFabricProbeInfoKernel->probeResponseMsg.msgHdr.status);

        return pGpuFabricProbeInfoKernel->probeResponseMsg.msgHdr.status;
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

    portAtomicSetU32(&pGpuFabricProbeInfoKernel->probeRespRcvd, 0);

    if (pKernelNvlink != NULL)
    {
        knvlinkClearUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink);
        knvlinkClearUniqueFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink);
    }

    if (pFabricVAS != NULL)
        fabricvaspaceClearUCRange(pFabricVAS);
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

    *pHandle = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.gpuHandle;

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

    *pGfId = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.gfId;

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

    *pFmCaps = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.fmCaps;

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

    portMemCopy(&pClusterUuid->uuid[0],
                sizeof(pClusterUuid->uuid),
                &pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.clusterUuid.uuid[0],
                sizeof(pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.clusterUuid.uuid));

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

    *pFabricPartitionId = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.fabricPartitionId;

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

    *pGpaAddress = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.gpaAddress;

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

    *pGpaAddressRange = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.gpaAddressRange;

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

    *pFlaAddress = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.flaAddress;

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

    *pFlaAddressRange = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.flaAddressRange;

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

    *pEgmGpaAddress = (NvU64)pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.gpaAddressEGMHi << 32ULL;

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
gpuFabricProbeGetFabricCliqueId
(
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel,
    NvU32 *pFabricCliqueId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *pFabricCliqueId =
        pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.cliqueId;

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

    *pFabricHealthStatusMask =
        pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.fabricHealthMask;

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
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeResponseMsg;
    nvlink_inband_msg_header_t *pProbeRespMsgHdr;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return NV_FALSE;
    }

    pProbeResponseMsg = &pGpuFabricProbeInfoKernel->probeResponseMsg;
    pProbeRespMsgHdr = &pProbeResponseMsg->msgHdr;

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

    return pGpuFabricProbeInfoKernel->probeResponseMsg.msgHdr.status;
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
        NvU64 flaBaseAddress;
        NvU64 flaSize;

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            gpuFabricProbeGetFlaAddress(pGpuFabricProbeInfoKernel,
                                        &flaBaseAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            gpuFabricProbeGetFlaAddressRange(pGpuFabricProbeInfoKernel,
                                             &flaSize) == NV_OK);

        if (IS_VIRTUAL(pGpu))
        {
            fabricvaspaceClearUCRange(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE));
        }

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            fabricvaspaceInitUCRange(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE),
                                     pGpu, flaBaseAddress, flaSize) == NV_OK);
    }
}

static void
_gpuFabricProbeSendCliqueIdChangeEvent
(
    OBJGPU *pGpu,
    NvU32 cliqueId
)
{
    NV_STATUS status;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV00F1_CTRL_FABRIC_EVENT event;
    event.id = fabricGenerateEventId_IMPL(pFabric);
    event.type = NV00F1_CTRL_FABRIC_EVENT_TYPE_CLIQUE_ID_CHANGE;
    event.imexChannel = 0;
    event.data.cliqueIdChange.gpuId = pGpu->gpuId;
    event.data.cliqueIdChange.cliqueId = cliqueId;
    status = fabricPostEventsV2(pFabric, &event, 1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU%u Notifying cliqueId change failed\n",
              gpuGetInstance(pGpu));
    }
}

static void
_gpuFrabricProbeUpdateSupportedBwModes
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU8 maxRbmLinks = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.maxRbmLinks;

    if (pKernelNvlink != NULL)
    {
        // Check if maxRbmLinks is within max supported links
        NV_ASSERT_OR_RETURN_VOID(maxRbmLinks <= NVLINK_MAX_LINKS_SW);
        pKernelNvlink->maxRbmLinks = maxRbmLinks;
    }
}

static void
_gpuFrabricProbeRbmSleepLinks
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS status = NV_OK;
    NvU32 enabledLinkMask;
    NvU32 linkMask;

    // Check if probe request has rbmRequested set
    if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, pGpuFabricProbeInfoKernel->bwMode) !=
        GPU_NVLINK_BW_MODE_LINK_COUNT)
    {
        return;
    }

    // Set links requested in linkMaskToBeReduced to sleep
    enabledLinkMask = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    linkMask = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.linkMaskToBeReduced &
                            enabledLinkMask;
    status = knvlinkEnterExitSleep(pGpu, pKernelNvlink, linkMask, NV_TRUE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error setting links to sleep on linkmask 0x%x\n",
                  linkMask);
    }
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
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeRespMsg;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    KernelNvlink *pKernelNvlink;
    NV_STATUS status;

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

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

    pProbeRespMsg = \
        (nvlink_inband_gpu_probe_rsp_msg_t *)&pInbandRcvParams->data[0];

    portMemCopy(&pGpuFabricProbeInfoKernel->probeResponseMsg,
                sizeof(pGpuFabricProbeInfoKernel->probeResponseMsg),
                pProbeRespMsg,
                sizeof(*pProbeRespMsg));

    portAtomicMemoryFenceFull();
    //
    // TODO - Add additional check with versioning to continue with the
    // timer and send lower version requests
    //
    portAtomicSetU32(&pGpuFabricProbeInfoKernel->probeRespRcvd, 1);

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    _gpuFabricProbeSetupGpaRange(pGpu, pGpuFabricProbeInfoKernel);
    _gpuFabricProbeSetupFlaRange(pGpu, pGpuFabricProbeInfoKernel);

    // Update supported bandwidth modes from probe response
    _gpuFrabricProbeUpdateSupportedBwModes(pGpu, pGpuFabricProbeInfoKernel);

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    if (pKernelNvlink != NULL)
    {
        if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
        {
            // Set links to sleep based on probe response
            _gpuFrabricProbeRbmSleepLinks(pGpu, pGpuFabricProbeInfoKernel);
        }
    }


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
    nvlink_inband_gpu_probe_update_req_msg_t *pProbeUpdateReqMsg;
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeRespMsg;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    NV_STATUS status;

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pGpu->pGpuFabricProbeInfoKernel != NULL, NV_OK);

    NV_ASSERT(rmGpuGroupLockIsOwner(gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                    &gpuMaskUnused));

    NV_ASSERT(pInbandRcvParams != NULL);

    pGpuFabricProbeInfoKernel = pGpu->pGpuFabricProbeInfoKernel;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    pProbeRespMsg = &pGpuFabricProbeInfoKernel->probeResponseMsg;

    pProbeUpdateReqMsg = \
        (nvlink_inband_gpu_probe_update_req_msg_t *)&pInbandRcvParams->data[0];

    pProbeRespMsg->probeRsp.fabricHealthMask =
        pProbeUpdateReqMsg->probeUpdate.fabricHealthMask;

    if (pProbeRespMsg->probeRsp.cliqueId !=
            pProbeUpdateReqMsg->probeUpdate.cliqueId)
    {
        pProbeRespMsg->probeRsp.cliqueId =
            pProbeUpdateReqMsg->probeUpdate.cliqueId;
        _gpuFabricProbeSendCliqueIdChangeEvent(pGpu,
                                               pProbeRespMsg->probeRsp.cliqueId);
    }

    return NV_OK;
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

static void
_gpuFabricProbeRbmWakeLinks
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_GET_POWER_STATE_PARAMS powerStatusParams = {0};
    NvU32 enabledLinkMask = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    NvU32 linkMask = 0;
    NV_STATUS status;
    NvU32 i;

    FOR_EACH_INDEX_IN_MASK(32, i, enabledLinkMask)
    {
        powerStatusParams.linkId = i;
        NV_CHECK_OK(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_NVLINK_GET_POWER_STATE,
                            &powerStatusParams, sizeof(powerStatusParams)));
        if (powerStatusParams.powerState == NV2080_CTRL_NVLINK_POWER_STATE_L2)
        {
            linkMask |= NVBIT32(i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (linkMask)
    {
        // Wake all sleeping links
        status = knvlinkEnterExitSleep(pGpu, pKernelNvlink, linkMask, NV_FALSE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error waking links on linkmask 0x%x\n",
                      linkMask);
        }
    }
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
            pGpuFabricProbeInfoKernel->bwMode = pKernelNvlink->nvlinkBwMode;
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
                _gpuFabricProbeRbmWakeLinks(pGpu, pGpuFabricProbeInfoKernel);
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

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

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

#define GPU_FABRIC_CHECK_BW_MODE(fmCaps, mode)                    \
    do                                                            \
    {                                                             \
        if ((fmCaps & NVLINK_INBAND_FM_CAPS_BW_MODE_##mode) == 0) \
            return NV_ERR_NOT_SUPPORTED;                          \
    } while (0)

NV_STATUS
gpuFabricProbeSetBwModePerGpu
(
    OBJGPU *pGpu,
    NvU8 mode
)
{
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel =
                                    pGpu->pGpuFabricProbeInfoKernel;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // Function assumes caller has checked requested mode is supported
    pGpuFabricProbeInfoKernel->bwMode = mode;

    gpuFabricProbeSuspend(pGpuFabricProbeInfoKernel);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
          pRmApi->Control(pRmApi,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE,
                          NULL, 0));

    _gpuFabricProbeInvalidate(pGpu);

    // Need to wake links if LINK_COUNT or FULL BW mode is requested before probe update
    if ((DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_LINK_COUNT) ||
        (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) == GPU_NVLINK_BW_MODE_FULL))
    {
        if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED))
        {
            _gpuFabricProbeRbmWakeLinks(pGpu, pGpuFabricProbeInfoKernel);
            pKernelNvlink->nvlinkBwMode = mode;
        }
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuFabricProbeResume(pGpuFabricProbeInfoKernel));

    return NV_OK;
}

NV_STATUS
gpuFabricProbeSetBwMode
(
    NvU8 mode
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

        // Confirm requested BW mode is supported by GPU
        pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        if (!knvlinkIsBwModeSupported_HAL(pGpu, pKernelNvlink, mode))
        {
            return NV_ERR_NOT_SUPPORTED;
        }

        // These checks only apply to legacy bw mode settings
        if (DRF_VAL(_GPU, _NVLINK, _BW_MODE, mode) !=
            GPU_NVLINK_BW_MODE_LINK_COUNT)
        {
            NvU64 fmCaps = pGpu->pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.fmCaps;
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
        status = gpuFabricProbeSetBwModePerGpu(pGpu, mode);
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
    NvU32 *linkMaskToBeReduced
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfoKernel);

    NV_CHECK_OR_RETURN(LEVEL_SILENT, status == NV_OK, status);

    *linkMaskToBeReduced =
        pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.linkMaskToBeReduced;

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

    *pRemapTableIdx =
        pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.remapTableIdx;

    return NV_OK;
}
