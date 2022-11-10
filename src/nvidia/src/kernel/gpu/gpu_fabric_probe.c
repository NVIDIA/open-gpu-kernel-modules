/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "objtmr.h"
#include "utils/nvprintf.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "ctrl/ctrl2080/ctrl2080pmgr.h"
#include "gpu/gpu_fabric_probe.h"
#include "compute/fabric.h"
#include "nvlink_inband_msg.h"
#include "kernel/mem_mgr/fabric_vaspace.h"

// Structure to hold gpu probe information
typedef struct GPU_FABRIC_PROBE_INFO
{
    NvU64 probeRetryDelay;
    NvBool bProbeRespRcvd;
    NvU64 numProbes;

    OBJGPU *pGpu;

    TMR_EVENT *pTmrEvent;

    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS sendDataParams;
    nvlink_inband_gpu_probe_rsp_msg_t probeResponseMsg;

} GPU_FABRIC_PROBE_INFO;


static NV_STATUS
_gpuFabricProbeFullSanityCheck
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(
                           gpuGetInstance(pGpuFabricProbeInfo->pGpu)));

    if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfo))
    {
        return NV_ERR_NOT_READY;
    }

    if (!gpuFabricProbeIsSuccess(pGpuFabricProbeInfo))
    {
        return pGpuFabricProbeInfo->probeResponseMsg.msgHdr.status;
    }

    return NV_OK;
}

NV_STATUS
gpuFabricProbeGetGpuFabricHandle
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pHandle
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pHandle = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.gpuHandle;

    return status;
}

NV_STATUS
gpuFabricProbeGetGfId
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU32 *pGfId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pGfId = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.gfId;

    return status;
}

NV_STATUS
gpuFabricProbeGetfmCaps
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pFmCaps
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pFmCaps = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.fmCaps;

    return status;
}

NV_STATUS
gpuFabricProbeGetClusterUuid
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvUuid *pClusterUuid
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    portMemCopy(&pClusterUuid->uuid[0],
                sizeof(pClusterUuid->uuid),
                &pGpuFabricProbeInfo->probeResponseMsg.probeRsp.clusterUuid.uuid[0],
                sizeof(pGpuFabricProbeInfo->probeResponseMsg.probeRsp.clusterUuid.uuid));

    return status;
}

NV_STATUS
gpuFabricProbeGetFabricPartitionId
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU16 *pFabricPartitionId
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pFabricPartitionId = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.fabricPartitionId;

    return status;
}

NV_STATUS
gpuFabricProbeGetGpaAddress
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pGpaAddress
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pGpaAddress = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.gpaAddress;

    return status;
}

NV_STATUS
gpuFabricProbeGetGpaAddressRange
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pGpaAddressRange
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pGpaAddressRange = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.gpaAddressRange;

    return status;
}

NV_STATUS
gpuFabricProbeGetFlaAddress
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pFlaAddress
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pFlaAddress = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.flaAddress;

    return status;
}

NV_STATUS
gpuFabricProbeGetFlaAddressRange
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *pFlaAddressRange
)
{
    NV_STATUS status;

    status = _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, status == NV_OK, status);

    *pFlaAddressRange = pGpuFabricProbeInfo->probeResponseMsg.probeRsp.flaAddressRange;

    return status;
}

NV_STATUS
gpuFabricProbeGetNumProbeReqs
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NvU64 *numProbes
)
{
    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(
                           gpuGetInstance(pGpuFabricProbeInfo->pGpu)));

    *numProbes = pGpuFabricProbeInfo->numProbes;

    return NV_OK;
}

NvBool
gpuFabricProbeIsReceived
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_FALSE;
    }

    LOCK_ASSERT_AND_RETURN_BOOL(rmDeviceGpuLockIsOwner(
                                gpuGetInstance(pGpuFabricProbeInfo->pGpu)),
                                NV_FALSE);

    return pGpuFabricProbeInfo->bProbeRespRcvd;
}

NvBool
gpuFabricProbeIsSuccess
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeResponseMsg;
    nvlink_inband_msg_header_t *pProbeRespMsgHdr;

    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_FALSE;
    }

    LOCK_ASSERT_AND_RETURN_BOOL(rmDeviceGpuLockIsOwner(gpuGetInstance(
                                pGpuFabricProbeInfo->pGpu)),
                                NV_FALSE);

    pProbeResponseMsg = &pGpuFabricProbeInfo->probeResponseMsg;
    pProbeRespMsgHdr = &pProbeResponseMsg->msgHdr;

    return pProbeRespMsgHdr->status == NV_OK;
}

NV_STATUS
gpuFabricProbeGetFmStatus
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(
                           gpuGetInstance(pGpuFabricProbeInfo->pGpu)));

    return pGpuFabricProbeInfo->probeResponseMsg.msgHdr.status;
}

static void
_gpuFabricProbeForceCompletionError
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo,
    NV_STATUS status
)
{
    NV_ASSERT(!pGpuFabricProbeInfo->bProbeRespRcvd);
    pGpuFabricProbeInfo->bProbeRespRcvd = NV_TRUE;
    pGpuFabricProbeInfo->probeResponseMsg.msgHdr.status = status;
}

static NV_STATUS
_gpuFabricProbeSchedule
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    NV_STATUS status;
    OBJGPU *pGpu = pGpuFabricProbeInfo->pGpu;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    NV_ASSERT(pGpuFabricProbeInfo->pTmrEvent != NULL);

    status = tmrEventScheduleRel(pTmr, pGpuFabricProbeInfo->pTmrEvent,
                                 pGpuFabricProbeInfo->probeRetryDelay);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) tmrEventScheduleRel failed\n",
                  gpuGetInstance(pGpu));
        return status;
    }

    pGpuFabricProbeInfo->numProbes++;

    if ((pGpuFabricProbeInfo->numProbes %
        pGpu->fabricProbeSlowdownThreshold) == 0)
    {
        pGpuFabricProbeInfo->probeRetryDelay += pGpuFabricProbeInfo->probeRetryDelay;
    }

    NV_PRINTF(LEVEL_INFO, "GPU (ID: %d) Num retried probes %lld \n",
              gpuGetInstance(pGpu), pGpuFabricProbeInfo->numProbes);

    return NV_OK;
}

static NV_STATUS
_gpuFabricProbeConstructReq
(
    nvlink_inband_gpu_probe_req_t *pProbeReq,
    OBJGPU *pGpu
)
{
    NV_STATUS status;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU8 *pUuid;
    NvU32 uuidLength, flags = 0;

    portMemSet(pProbeReq, 0, sizeof(*pProbeReq));

    flags = FLD_SET_DRF_NUM(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE,
                            NV2080_GPU_CMD_GPU_GET_GID_FLAGS_TYPE_SHA1, flags);
    flags = FLD_SET_DRF_NUM(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT,
                            NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT_BINARY, flags);
    status = gpuGetGidInfo(pGpu, &pUuid, &uuidLength, flags);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Failed to update GPU UUID\n",
                  gpuGetInstance(pGpu));
        return status;
    }
    portMemCopy(&pProbeReq->gpuUuid.uuid[0], uuidLength, pUuid, uuidLength);
    portMemFree(pUuid);

    pProbeReq->pciInfo = gpuGetDBDF(pGpu);
    pProbeReq->moduleId = pGpu->moduleId;
    pProbeReq->discoveredLinkMask = pKernelNvlink->discoveredLinks;
    pProbeReq->enabledLinkMask = pKernelNvlink->enabledLinks;
    pProbeReq->gpuCapMask = (IS_VIRTUAL_WITH_SRIOV(pGpu)) ?
                            NVLINK_INBAND_GPU_PROBE_CAPS_SRIOV_ENABLED : 0;
    return status;
}

static NV_STATUS
_gpuFabricProbeSend
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    NV_STATUS status;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo =
        (GPU_FABRIC_PROBE_INFO *)pEvent->pUserData;

    if (pKernelNvlink->bIsGpuDegraded ||
        (pKernelNvlink->discoveredLinks == 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU (ID: %d) Degraded. Not sending probe\n",
                  gpuGetInstance(pGpu));
        _gpuFabricProbeForceCompletionError(pGpuFabricProbeInfo, NV_ERR_NOT_SUPPORTED);
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pGpuFabricProbeInfo->bProbeRespRcvd)
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU (ID: %d) Probe Resp rcvd. Not sending probe\n",
                  gpuGetInstance(pGpu));
        return NV_OK;
    }

    status = knvlinkSendInbandData(pGpu,
                                  pKernelNvlink,
                                  &pGpuFabricProbeInfo->sendDataParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Send Inband data failed\n",
                  gpuGetInstance(pGpu));
        //
        // Deliberately ignoring return value as we want probes to be
        // retried until success
        //
    }

    status = _gpuFabricProbeSchedule(pGpuFabricProbeInfo);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Schedule Probe failed\n",
                  gpuGetInstance(pGpu));
        _gpuFabricProbeForceCompletionError(pGpuFabricProbeInfo, NV_ERR_OPERATING_SYSTEM);
        return status;
    }

    return status;
}


static void
_gpuFabricProbeSetupGpaRange
(
    OBJGPU                *pGpu,
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    // setup GPA based system
    if (pKernelNvlink != NULL)
    {
        NvU64 gpaAddress;
        NvU64 gpaAddressSize;

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, 
                                gpuFabricProbeGetGpaAddress(pGpuFabricProbeInfo,
                                                            &gpaAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                                gpuFabricProbeGetGpaAddressRange(pGpuFabricProbeInfo,
                                                                 &gpaAddressSize) == NV_OK);

        //pKernelNvlink->fabricBaseAddr = gpaAddress;
        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, 
                                knvlinkSetUniqueFabricBaseAddress_HAL(pGpu,
                                                                      pKernelNvlink,
                                                                      gpaAddress) == NV_OK);
    }
}

static void
_gpuFabricProbeSetupFlaRange
(
    OBJGPU                  *pGpu,
    GPU_FABRIC_PROBE_INFO   *pGpuFabricProbeInfo
)
{
    if (pGpu->pFabricVAS != NULL)
    {
        NvU64 flaBaseAddress;
        NvU64 flaSize;

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                                gpuFabricProbeGetFlaAddress(pGpuFabricProbeInfo, &flaBaseAddress) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                                gpuFabricProbeGetFlaAddressRange(pGpuFabricProbeInfo, &flaSize) == NV_OK);

        NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                                fabricvaspaceInitUCRange(dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE),
                                                         pGpu, flaBaseAddress, flaSize) == NV_OK);
    }
}

void
_gpuFabricProbeReceive
(
    OBJGPU *pGpu,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    nvlink_inband_gpu_probe_rsp_msg_t *pProbeRespMsg;
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo;
    nvlink_inband_gpu_probe_req_msg_t *pProbeReqMsg = NULL;
    NvU8 *pRsvd = NULL;
    OBJSYS *pSys = SYS_GET_INSTANCE();

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    NV_ASSERT(pInbandRcvParams != NULL);

    pGpuFabricProbeInfo = pGpu->pGpuFabricProbeInfo;

    pProbeReqMsg = (nvlink_inband_gpu_probe_req_msg_t *)&pGpuFabricProbeInfo->
                                                         sendDataParams.buffer[0];
    pProbeRespMsg = (nvlink_inband_gpu_probe_rsp_msg_t *)&pInbandRcvParams->data[0];

    if (pProbeRespMsg->msgHdr.requestId != pProbeReqMsg->msgHdr.requestId)
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU (ID: %d) Probe resp invalid reqId %lld respId %lld\n",
                  gpuGetInstance(pGpu),
                  pProbeReqMsg->msgHdr.requestId,
                  pProbeRespMsg->msgHdr.requestId);
        return;
    }

    // Assert reserved in probeResponse are zero
    pRsvd = &pProbeRespMsg->probeRsp.reserved[0];
    NV_ASSERT((pRsvd[0] == 0) && portMemCmp(pRsvd, pRsvd + 1,
              sizeof(pProbeRespMsg->probeRsp.reserved) - 1) == 0);

    portMemCopy(&pGpuFabricProbeInfo->probeResponseMsg,
                sizeof(pGpuFabricProbeInfo->probeResponseMsg),
                pProbeRespMsg,
                sizeof(*pProbeRespMsg));

    //
    // TODO - Add additional check with versioning to continue with the
    // timer and send lower version requests
    //
    pGpuFabricProbeInfo->bProbeRespRcvd = NV_TRUE;

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR,
                            _gpuFabricProbeFullSanityCheck(pGpuFabricProbeInfo) == NV_OK);
    _gpuFabricProbeSetupGpaRange(pGpu, pGpuFabricProbeInfo);
    _gpuFabricProbeSetupFlaRange(pGpu, pGpuFabricProbeInfo);

    // if MC FLA is disabled, reset the fmCaps
    if (!pSys->bMulticastFlaEnabled)
    {
        pGpuFabricProbeInfo->probeResponseMsg.probeRsp.fmCaps &= 
                             ~NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1;
        pGpuFabricProbeInfo->probeResponseMsg.probeRsp.fmCaps &= 
                             ~NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1;
    }
}

void
gpuFabricProbeSuspend
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    OBJGPU *pGpu;
    OBJTMR *pTmr;

    if (pGpuFabricProbeInfo == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfo->pGpu;

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    pTmr = GPU_GET_TIMER(pGpu);

    NV_ASSERT(pGpuFabricProbeInfo->pTmrEvent != NULL);

    if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfo))
    {
        tmrEventCancel(pTmr, pGpuFabricProbeInfo->pTmrEvent);
    }
}

NV_STATUS
gpuFabricProbeResume
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    OBJGPU *pGpu;
    OBJTMR *pTmr;
    NV_STATUS status = NV_OK;

    if (pGpuFabricProbeInfo == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpu = pGpuFabricProbeInfo->pGpu;

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    pTmr = GPU_GET_TIMER(pGpu);

    NV_ASSERT(pGpuFabricProbeInfo->pTmrEvent != NULL);

    if (!gpuFabricProbeIsReceived(pGpuFabricProbeInfo))
    {
        status = _gpuFabricProbeSend(pGpu, pTmr, pGpuFabricProbeInfo->pTmrEvent);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Resume and Sending probe request failed\n",
                      gpuGetInstance(pGpu));
        }
    }

    return status;
}

NV_STATUS
gpuFabricProbeStart
(
    OBJGPU *pGpu,
    GPU_FABRIC_PROBE_INFO **ppGpuFabricProbeInfo
)
{
    NV_STATUS status;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    nvlink_inband_gpu_probe_req_msg_t *pProbeReqMsg = NULL;
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo;
    NVLINK_INBAND_MSG_CALLBACK inbandMsgCbParams;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    LOCK_ASSERT_AND_RETURN(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    //
    // TODO - probe versioning
    //        After retrying the latest version probe for a few times, if GPU
    //        doesn't receive any response, GPU fallsback to the previous
    //        version of probe request. This process continues until it
    //        reaches the lowest possible probe request version
    //

    // Check if NVSwitch based system. If not return without doing anything
    if (!gpuFabricProbeIsSupported(pGpu))
    {
        return NV_OK;
    }

    *ppGpuFabricProbeInfo = portMemAllocNonPaged(sizeof(*pGpuFabricProbeInfo));
    NV_ASSERT_OR_RETURN(*ppGpuFabricProbeInfo != NULL, NV_ERR_NO_MEMORY);

    pGpuFabricProbeInfo = *ppGpuFabricProbeInfo;

    portMemSet(pGpuFabricProbeInfo, 0, sizeof(*pGpuFabricProbeInfo));

    pGpuFabricProbeInfo->pGpu = pGpu;

    status = tmrEventCreate(pTmr, &pGpuFabricProbeInfo->pTmrEvent,
                            _gpuFabricProbeSend, pGpuFabricProbeInfo,
                            TMR_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Timer create failure\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    pGpuFabricProbeInfo->probeRetryDelay =
        pGpu->fabricProbeRetryDelay * GPU_FABRIC_PROBE_SEC_TO_NS;

    ct_assert(sizeof(nvlink_inband_gpu_probe_req_msg_t) <=
              sizeof(pGpuFabricProbeInfo->sendDataParams.buffer));

    pProbeReqMsg = (nvlink_inband_gpu_probe_req_msg_t *)&pGpuFabricProbeInfo->
                                                         sendDataParams.buffer[0];

    status = _gpuFabricProbeConstructReq(&pProbeReqMsg->probeReq, pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Init Probe request failed\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    status = fabricInitInbandMsgHdr(&pProbeReqMsg->msgHdr,
                                    NVLINK_INBAND_MSG_TYPE_GPU_PROBE_REQ,
                                    sizeof(pProbeReqMsg->probeReq));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Init of Inband msg hdr failed\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    pGpuFabricProbeInfo->sendDataParams.dataSize = sizeof(*pProbeReqMsg);

    // Register the receive callback
    inbandMsgCbParams.messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP;
    inbandMsgCbParams.pCallback = _gpuFabricProbeReceive;
    inbandMsgCbParams.wqItemFlags = (OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA |
                                     OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE_RW);
    status = knvlinkRegisterInbandCallback(pGpu,
                                           pKernelNvlink,
                                           &inbandMsgCbParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Registering Inband Cb failed\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    status = _gpuFabricProbeSend(pGpu, pTmr, pGpuFabricProbeInfo->pTmrEvent);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) Sending probe request failed\n",
                  gpuGetInstance(pGpu));
        goto fail;
    }

    return NV_OK;

fail:
    if (pGpuFabricProbeInfo->pTmrEvent != NULL)
    {
        tmrEventDestroy(pTmr, pGpuFabricProbeInfo->pTmrEvent);
    }

    portMemFree(pGpuFabricProbeInfo);
    pGpu->pGpuFabricProbeInfo = NULL;

    return status;
}

void
gpuFabricProbeStop
(
    GPU_FABRIC_PROBE_INFO *pGpuFabricProbeInfo
)
{
    OBJGPU *pGpu;
    OBJTMR *pTmr;
    KernelNvlink *pKernelNvlink;

    if (pGpuFabricProbeInfo == NULL)
    {
        return;
    }

    pGpu = pGpuFabricProbeInfo->pGpu;

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    pTmr = GPU_GET_TIMER(pGpu);
    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV_ASSERT(pKernelNvlink != NULL);

    // Unregister the receive callback
    NV_ASSERT_OK(knvlinkUnregisterInbandCallback(pGpu, pKernelNvlink,
                 NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP));

    if (pGpuFabricProbeInfo->pTmrEvent != NULL)
    {
        tmrEventDestroy(pTmr, pGpuFabricProbeInfo->pTmrEvent);
    }

    portMemFree(pGpuFabricProbeInfo);

    pGpu->pGpuFabricProbeInfo = NULL;
}

NvBool
gpuFabricProbeIsSupported
(
    OBJGPU *pGpu
)
{
    if (pGpu->fabricProbeRetryDelay == 0)
    {
        NV_PRINTF(LEVEL_INFO, "GPU (ID: %d) Probe handling is disabled\n",
                  gpuGetInstance(pGpu));
        return NV_FALSE;
    }

    if (GPU_GET_KERNEL_NVLINK(pGpu) == NULL)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}
