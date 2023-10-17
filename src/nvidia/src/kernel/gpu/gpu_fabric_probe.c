/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(
                           gpuGetInstance(pGpuFabricProbeInfoKernel->pGpu)));

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

    *pFabricCliqueId = pGpuFabricProbeInfoKernel->probeResponseMsg.probeRsp.cliqueId;

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

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    gpuFabricProbeGetGpaAddress(pGpuFabricProbeInfoKernel,
                                                &gpaAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    gpuFabricProbeGetGpaAddressRange(pGpuFabricProbeInfoKernel,
                                                    &gpaAddressSize) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                    knvlinkSetUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink,
                                                        gpaAddress) == NV_OK);
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

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
            fabricvaspaceInitUCRange(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE),
                                     pGpu, flaBaseAddress, flaSize) == NV_OK);
    }
}

static NV_STATUS
_gpuFabricProbeReceiveKernel
(
    NvU32 gpuInstance,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    OBJGPU *pGpu;
    NvU32 gpuMaskUnused;
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeRespMsg;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
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
    NV_STATUS status;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    NVLINK_INBAND_MSG_CALLBACK inbandMsgCbParams;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS params = { 0 };

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    // Check if NVSwitch based system. If not return without doing anything
    if (!gpuFabricProbeIsSupported(pGpu))
    {
        return NV_OK;
    }

    *ppGpuFabricProbeInfoKernel = portMemAllocNonPaged(sizeof(*pGpuFabricProbeInfoKernel));
    NV_ASSERT_OR_RETURN(*ppGpuFabricProbeInfoKernel != NULL, NV_ERR_NO_MEMORY);

    pGpuFabricProbeInfoKernel = *ppGpuFabricProbeInfoKernel;

    portMemSet(pGpuFabricProbeInfoKernel, 0, sizeof(*pGpuFabricProbeInfoKernel));

    pGpuFabricProbeInfoKernel->pGpu = pGpu;

    // Register the receive callback
    inbandMsgCbParams.messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP;
    inbandMsgCbParams.pCallback = _gpuFabricProbeReceiveKernel;
    inbandMsgCbParams.wqItemFlags = (OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA |
                            OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE_RW);
    status = knvlinkRegisterInbandCallback(pGpu,
                                           pKernelNvlink,
                                           &inbandMsgCbParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU%u Registering Inband Cb failed\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    pGpuFabricProbeInfoKernel->bwMode = gpumgrGetGpuNvlinkBwMode();
    params.bwMode = pGpuFabricProbeInfoKernel->bwMode;

    // Send IOCTL to start probe
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE,
                            &params, sizeof(params)),
            fail);

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
    KernelNvlink *pKernelNvlink;
    RM_API *pRmApi;

    if (pGpuFabricProbeInfoKernel == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfoKernel->pGpu;

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_ASSERT(pKernelNvlink != NULL);

    // Unregister the receive callback
    NV_ASSERT_OK(knvlinkUnregisterInbandCallback(pGpu, pKernelNvlink,
                 NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP));

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GPU_STOP_FABRIC_PROBE,
                                 NULL, 0));

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

    if (GPU_GET_KERNEL_NVLINK(pGpu) == NULL)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
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
        knvlinkClearUniqueFabricBaseAddress_HAL(pGpu, pKernelNvlink);

    if (pFabricVAS != NULL)
        fabricvaspaceClearUCRange(pFabricVAS);
}

#define GPU_FABRIC_CHECK_BW_MODE(fmCaps, mode)                    \
    do                                                            \
    {                                                             \
        if ((fmCaps & NVLINK_INBAND_FM_CAPS_BW_MODE_##mode) == 0) \
            return NV_ERR_NOT_SUPPORTED;                          \
    } while (0)

static NV_STATUS
_gpuFabricProbeUpdateBwMode
(
    OBJGPU *pGpu,
    NvU8 mode
)
{
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel =
                                                pGpu->pGpuFabricProbeInfoKernel;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    pGpuFabricProbeInfoKernel->bwMode = mode;

    gpuFabricProbeSuspend(pGpuFabricProbeInfoKernel);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
          pRmApi->Control(pRmApi,
                          pGpu->hInternalClient,
                          pGpu->hInternalSubdevice,
                          NV2080_CTRL_CMD_INTERNAL_GPU_INVALIDATE_FABRIC_PROBE,
                          NULL, 0));

    _gpuFabricProbeInvalidate(pGpu);

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
        status = _gpuFabricProbeUpdateBwMode(pGpu, mode);
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
