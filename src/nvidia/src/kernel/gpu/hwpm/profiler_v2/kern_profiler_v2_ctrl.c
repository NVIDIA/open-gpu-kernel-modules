/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "nvoc/prelude.h"
#include "nvstatuscodes.h"
#include "rmapi/rs_utils.h"
#include "gpu/hwpm/profiler_v2.h"
#include "gpu/hwpm/kern_hwpm.h"
#include "gpu/hwpm/kern_hwpm_power.h"
#include "ctrl/ctrlb0cc/ctrlb0ccinternal.h"
#include "ctrl/ctrlb0cc/ctrlb0ccprofiler.h"
#include "mem_mgr/mem.h"
#include "vgpu/rpc.h"

static NV_STATUS _issueRpcToHost(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    NV_RM_RPC_CONTROL(pGpu,
                      pRmCtrlParams->hClient,
                      pRmCtrlParams->hObject,
                      pRmCtrlParams->cmd,
                      pRmCtrlParams->pParams,
                      pRmCtrlParams->paramsSize,
                      status);

    return status;
}

NV_STATUS
profilerBaseCtrlCmdFreePmaStream_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(GPU_RES_GET_GPU(pProfiler));
    NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS internalParams;

    portMemSet(&internalParams, 0, sizeof(NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS));
    internalParams.pmaChannelIdx = pParams->pmaChannelIdx;

    //
    // The following security checks are valid only if MemPmaBuffer and MemPmaBytesAvailable
    // buffers are accessible and are set during PMA alloc
    // Those buffers are not accessible on vGpu host (accessible on vGPU Guest and baremetal)
    //
    if (pProfiler->ppBytesAvailable != NULL && pProfiler->ppStreamBuffers != NULL)
    {
        RsResourceRef     *pCountRef = NULL;
        RsResourceRef     *pBufferRef = NULL;

        if (pProfiler->maxPmaChannels <= pParams->pmaChannelIdx)
        {
            goto err;
        }

        pCountRef = pProfiler->ppBytesAvailable[pParams->pmaChannelIdx];
        pProfiler->ppBytesAvailable[pParams->pmaChannelIdx] = NULL;
        pBufferRef = pProfiler->ppStreamBuffers[pParams->pmaChannelIdx];
        pProfiler->ppStreamBuffers[pParams->pmaChannelIdx] = NULL;

        if (pProfiler->pBoundCntBuf == pCountRef && pProfiler->pBoundPmaBuf == pBufferRef)
        {
            Memory *pCntMem = dynamicCast(pCountRef->pResource, Memory);
            Memory *pBufMem = dynamicCast(pBufferRef->pResource, Memory);
            pProfiler->pBoundCntBuf = NULL;
            pProfiler->pBoundPmaBuf = NULL;
            pCntMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
            pBufMem->pMemDesc->bRmExclusiveUse = NV_FALSE;

        }
        if (pCountRef != NULL)
        {
            refRemoveDependant(pCountRef, RES_GET_REF(pProfiler));
        }
        if (pBufferRef != NULL)
        {
            refRemoveDependant(pBufferRef, RES_GET_REF(pProfiler));
        }
    }
err:

    return pRmApi->Control(pRmApi,
                           RES_GET_CLIENT_HANDLE(pProfiler),
                           RES_GET_HANDLE(pProfiler),
                           NVB0CC_CTRL_CMD_INTERNAL_FREE_PMA_STREAM,
                           &internalParams, sizeof(internalParams));
}

NV_STATUS
profilerBaseCtrlCmdBindPmResources_IMPL
(
    ProfilerBase *pProfiler
)
{
    OBJGPU        *pGpu                       = GPU_RES_GET_GPU(pProfiler);
    RM_API        *pRmApi                     = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle       hClient                    = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle       hObject                    = RES_GET_HANDLE(pProfiler);
    NV_STATUS      status                     = NV_OK;
    Memory        *pCntMem                    = NULL;
    Memory        *pBufMem                    = NULL;

    //
    // The following security checks are valid only if MemPmaBuffer and MemPmaBytesAvailable
    // buffers are accessible and are set during PMA alloc
    // Those buffers are not accessible on vGpu host (accessible on vGPU Guest and baremetal)
    //
    if (pProfiler->ppBytesAvailable != NULL && pProfiler->ppStreamBuffers != NULL)
    {
        RsResourceRef *pCntRef                    = NULL;
        RsResourceRef *pBufRef                    = NULL;

        NV_CHECK_OR_GOTO(LEVEL_INFO,
            !pProfiler->bLegacyHwpm && pProfiler->maxPmaChannels != 0, physical_control);

        if (pProfiler->maxPmaChannels <= pProfiler->pmaVchIdx)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        pCntRef = pProfiler->ppBytesAvailable[pProfiler->pmaVchIdx];
        pBufRef = pProfiler->ppStreamBuffers[pProfiler->pmaVchIdx];

        NV_CHECK_OR_GOTO(LEVEL_INFO,
            pCntRef != NULL && pBufRef != NULL, physical_control);

        pCntMem = dynamicCast(pCntRef->pResource, Memory);
        pBufMem = dynamicCast(pBufRef->pResource, Memory);

        NV_ASSERT_OR_RETURN(pCntMem != NULL && pBufMem != NULL, NV_ERR_INVALID_STATE);

        if (!memdescAcquireRmExclusiveUse(pCntMem->pMemDesc) ||
            !memdescAcquireRmExclusiveUse(pBufMem->pMemDesc))
        {
            pCntMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
            pBufMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
            return NV_ERR_INVALID_ARGUMENT;
        }

        pProfiler->pBoundCntBuf = pCntRef;
        pProfiler->pBoundPmaBuf = pBufRef;
    }

physical_control:

    if (IS_VIRTUAL(pGpu))
    {
        return _issueRpcToHost(pGpu);
    }

    status = pRmApi->Control(pRmApi, hClient, hObject,
                             NVB0CC_CTRL_CMD_INTERNAL_BIND_PM_RESOURCES,
                             NULL, 0);
    if (status != NV_OK && pCntMem != NULL && pBufMem != NULL)
    {
        pCntMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
        pBufMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
        pProfiler->pBoundCntBuf = NULL;
        pProfiler->pBoundPmaBuf = NULL;
    }
    return status;
}

NV_STATUS
profilerBaseCtrlCmdUnbindPmResources_IMPL
(
    ProfilerBase *pProfiler
)
{
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle  hClient                         = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);
    RsResourceRef *pCntRef                    = NULL;
    RsResourceRef *pBufRef                    = NULL;

    pCntRef = pProfiler->pBoundCntBuf;
    pBufRef = pProfiler->pBoundPmaBuf;

    if (pCntRef != NULL)
    {
        Memory *pCntMem = dynamicCast(pCntRef->pResource, Memory);
        if (pCntMem != NULL)
        {
            pCntMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
        }
        pProfiler->pBoundCntBuf = NULL;
    }

    if (pBufRef != NULL)
    {
        Memory *pBufMem = dynamicCast(pBufRef->pResource, Memory);
        if (pBufMem != NULL)
        {
            pBufMem->pMemDesc->bRmExclusiveUse = NV_FALSE;
        }
        pProfiler->pBoundPmaBuf = NULL;
    }

    return pRmApi->Control(pRmApi, hClient, hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_UNBIND_PM_RESOURCES,
                           NULL, 0);
}

NV_STATUS
profilerBaseCtrlCmdReserveHwpmLegacy_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams
)
{
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle  hClient                         = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);

    pProfiler->bLegacyHwpm = NV_TRUE;
    return pRmApi->Control(pRmApi, hClient, hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_RESERVE_HWPM_LEGACY,
                           pParams, sizeof(*pParams));
}

NV_STATUS
profilerBaseCtrlCmdAllocPmaStream_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams
)
{
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    RsClient *pClient                         = RES_GET_CLIENT(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);
    NvHandle  hDevice;
    NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS internalParams;
    RsResourceRef     *pMemoryRef = NULL;
    RsResourceRef     *pDevice = NULL;
    NvBool    bHasCountBuffer = clientGetResourceRef(pClient, pParams->hMemPmaBytesAvailable, &pMemoryRef) == NV_OK;
    NvBool    bHasMemBuffer   = clientGetResourceRef(pClient, pParams->hMemPmaBuffer, &pMemoryRef) == NV_OK;
    NvBool    bHasBuffers = bHasCountBuffer && bHasMemBuffer;


    // find handle of Device
    NV_ASSERT_OK_OR_RETURN(
        refFindAncestorOfType(RES_GET_REF(pProfiler), classId(Device), &pDevice));
    hDevice = pDevice->hResource;

    // Allocate requisite arrays if not already done
    if (bHasBuffers && (pProfiler->ppBytesAvailable == NULL))
    {
        NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS maxPmaParams;
        portMemSet(&maxPmaParams, 0, sizeof(NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS));
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->Control(pRmApi, pClient->hClient, hObject,
                NVB0CC_CTRL_CMD_INTERNAL_GET_MAX_PMAS,
                &maxPmaParams, sizeof(maxPmaParams)));

        pProfiler->maxPmaChannels = maxPmaParams.maxPmaChannels;
        pProfiler->ppBytesAvailable = (RsResourceRef**)portMemAllocNonPaged(maxPmaParams.maxPmaChannels * sizeof(RsResourceRef*));
        NV_ASSERT_OR_RETURN(pProfiler->ppBytesAvailable !=  NULL, NV_ERR_NO_MEMORY);
        pProfiler->ppStreamBuffers = (RsResourceRef**)portMemAllocNonPaged(maxPmaParams.maxPmaChannels * sizeof(RsResourceRef*));
        NV_ASSERT_OR_ELSE(pProfiler->ppStreamBuffers !=  NULL, {
            portMemFree(pProfiler->ppBytesAvailable);
            pProfiler->ppBytesAvailable = NULL;
            return NV_ERR_NO_MEMORY;
        });
    }

    //
    // REGISTER  MEMDESCs TO GSP
    // These are no-op with BareMetal/No GSP
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          memRegisterWithGsp(pGpu, pClient, hDevice, pParams->hMemPmaBuffer));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          memRegisterWithGsp(pGpu, pClient, hDevice, pParams->hMemPmaBytesAvailable));

    portMemSet(&internalParams, 0, sizeof(NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS));
    internalParams.hMemPmaBuffer = pParams->hMemPmaBuffer;
    internalParams.pmaBufferOffset = pParams->pmaBufferOffset;
    internalParams.pmaBufferSize = pParams->pmaBufferSize;
    internalParams.hMemPmaBytesAvailable = pParams->hMemPmaBytesAvailable;
    internalParams.pmaBytesAvailableOffset = pParams->pmaBytesAvailableOffset;
    internalParams.ctxsw = pParams->ctxsw;
    internalParams.pmaChannelIdx = pParams->pmaChannelIdx;
    internalParams.pmaBufferVA = pParams->pmaBufferVA;
    internalParams.bInputPmaChIdx = NV_FALSE;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        pClient->hClient,
                        hObject,
                        NVB0CC_CTRL_CMD_INTERNAL_ALLOC_PMA_STREAM,
                        &internalParams, sizeof(internalParams)));

    pParams->pmaChannelIdx = internalParams.pmaChannelIdx;

    if (bHasBuffers)
    {

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            clientGetResourceRef(pClient, pParams->hMemPmaBytesAvailable, &pMemoryRef));
        pProfiler->ppBytesAvailable[pParams->pmaChannelIdx] = pMemoryRef;
        refAddDependant(pMemoryRef, RES_GET_REF(pProfiler));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            clientGetResourceRef(pClient, pParams->hMemPmaBuffer, &pMemoryRef));
        pProfiler->ppStreamBuffers[pParams->pmaChannelIdx] = pMemoryRef;
        refAddDependant(pMemoryRef, RES_GET_REF(pProfiler));

        // Copy output params to external struct.
        pProfiler->pmaVchIdx = pParams->pmaChannelIdx;
        pProfiler->bLegacyHwpm = NV_FALSE;
    }

    // Copy output params to external struct.
    pParams->pmaBufferVA = internalParams.pmaBufferVA;

    return NV_OK;
}

/*
 * To be called on vGPU guest only.
 * Will issue RPC to vGPU host and map memBytes buffer to guest CPU VA
 */
static NV_STATUS
_profilerBaseCtrlCmdAllocPmaStreamVgpuGuest
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams,
    Memory *pMemPmaAvailBytes,
    Memory *pMemPmaBuffer
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);
    NvP64 pPriv = NvP64_NULL;
    NvP64 pCpuAddr = NvP64_NULL;
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle hObject = RES_GET_HANDLE(pProfiler);
    MEMORY_DESCRIPTOR *pTgtMemDescPmaAvailBytes = NULL;
    MEMORY_DESCRIPTOR *pTgtMemDescPmaBuffer = NULL;
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS promoteParams;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    // vGPU only supports PMA buffers to be allocated in sysmem
    NV_ASSERT_OR_RETURN((memdescGetAddressSpace(pMemPmaAvailBytes->pMemDesc) == ADDR_SYSMEM),
                         NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN((memdescGetAddressSpace(pMemPmaBuffer->pMemDesc) == ADDR_SYSMEM),
                         NV_ERR_NOT_SUPPORTED);

    NV_RM_RPC_CONTROL(pGpu,
                      pRmCtrlParams->hClient,
                      pRmCtrlParams->hObject,
                      NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM,
                      pRmCtrlParams->pParams,
                      pRmCtrlParams->paramsSize,
                      status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate PMA stream. Error 0x%x\n", status);
        return status;
    }

    // Map user-sent MEM_BYTES_ADDR buffer in CPU VA for guest RM to access the available bytes
    status = memdescCreateSubMem(&pTgtMemDescPmaAvailBytes,
                                 pMemPmaAvailBytes->pMemDesc, NULL,
                                 pParams->pmaBytesAvailableOffset,
                                 NVB0CC_PMA_BYTES_AVAILABLE_SIZE);
    if (status != NV_OK)
    {
        goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
    }

    status = memdescMap(pTgtMemDescPmaAvailBytes, 0, pTgtMemDescPmaAvailBytes->Size, NV_TRUE,
                        NV_PROTECT_READ_WRITE, &pCpuAddr, &pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Mapping of MEM_BYTES_ADDR buffer into CPU VA failed: 0x%x\n", status);
        goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
    }

    if (pProfiler->pPmaStreamList)
    {
        pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesBufDesc = pTgtMemDescPmaAvailBytes;
        pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesCpuAddr = pCpuAddr;
        pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesCpuAddrPriv = pPriv;
    }
    else
    {
        status = NV_ERR_INVALID_STATE;
        goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
    }

    // In full SRIOV, buffer VAs will be generated on guest, but they can be bound to PMA engine
    // on host only. Thus, generate VAs and HWPM IB and convey them to host.
    if (IS_SRIOV_FULL_GUEST(pGpu))
    {
        status = memdescCreateSubMem(&pTgtMemDescPmaBuffer,
                                     pMemPmaBuffer->pMemDesc, NULL,
                                     pParams->pmaBufferOffset,
                                     pParams->pmaBufferSize);
        if (status != NV_OK)
        {
            goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
        }

        pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pRecordBufDesc = pTgtMemDescPmaBuffer;

        status = khwpmStreamoutAllocPmaStream(pGpu, pKernelHwpm, pProfiler->profilerId,
                                              pTgtMemDescPmaBuffer,
                                              pTgtMemDescPmaAvailBytes,
                                              pParams->pmaChannelIdx,
                                              &pProfiler->pPmaStreamList[pParams->pmaChannelIdx]);
        if (status != NV_OK)
        {
            goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
        }

        promoteParams.pmaChannelIdx = pParams->pmaChannelIdx;

        status = pRmApi->Control(pRmApi,
                                 hClient,
                                 hObject,
                                 NVB0CC_CTRL_CMD_INTERNAL_SRIOV_PROMOTE_PMA_STREAM,
                                 &promoteParams, sizeof(promoteParams));
        if (status != NV_OK)
        {
            goto _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail;
        }

        pParams->pmaBufferVA = pProfiler->pPmaStreamList[pParams->pmaChannelIdx].vaddrRecordBuf;
    }

    pProfiler->pPmaStreamList[pParams->pmaChannelIdx].bValid = NV_TRUE;
    pProfiler->pPmaStreamList[pParams->pmaChannelIdx].bMemBytesBufferAccessAllowed = NV_TRUE;

    return NV_OK;

_profilerBaseCtrlCmdAllocPmaStreamVgpuGuest_fail:
    if (pCpuAddr != NvP64_NULL)
    {
        memdescUnmap(pTgtMemDescPmaAvailBytes, NV_TRUE, osGetCurrentProcess(), pCpuAddr, pPriv);
        if (pProfiler->pPmaStreamList)
        {
            pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesCpuAddr = NvP64_NULL;
            pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesCpuAddrPriv = NvP64_NULL;
        }
    }
    if (pTgtMemDescPmaAvailBytes != NULL)
    {
        memdescFree(pTgtMemDescPmaAvailBytes);
        memdescDestroy(pTgtMemDescPmaAvailBytes);
        if (pProfiler->pPmaStreamList)
        {
            pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesBufDesc = NULL;
        }
    }
    if (pTgtMemDescPmaBuffer != NULL)
    {
        memdescFree(pTgtMemDescPmaBuffer);
        memdescDestroy(pTgtMemDescPmaBuffer);
        if (pProfiler->pPmaStreamList)
        {
            pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pRecordBufDesc = NULL;
        }
    }

    return status;
}

static NV_STATUS
_profilerBaseCtrlCmdFreePmaStreamVgpuGuest
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams
)
{
    NV_STATUS      status                  = NV_OK;
    OBJGPU        *pGpu                    = GPU_RES_GET_GPU(pProfiler);
    KernelHwpm    *pKernelHwpm             = GPU_GET_KERNEL_HWPM(pGpu);
    CALL_CONTEXT  *pCallContext            = resservGetTlsCallContext();
    RmCtrlParams  *pRmCtrlParams           = pCallContext->pControlParams;

    // Handle quiesce streamout on guest, then issue RPC to host, which will handle
    // freeing of PMA stream.
    status = profilerBaseQuiesceStreamout(pProfiler, pGpu, pKernelHwpm, pParams->pmaChannelIdx);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to quiesce HWPM streamout. Error 0x%x\n", status);
        return status;
    }

    NV_RM_RPC_CONTROL(pGpu,
                      pRmCtrlParams->hClient,
                      pRmCtrlParams->hObject,
                      NVB0CC_CTRL_CMD_FREE_PMA_STREAM,
                      pRmCtrlParams->pParams,
                      pRmCtrlParams->paramsSize,
                      status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to free PMA stream. Error 0x%x\n", status);
        return status;
    }

    if (IS_SRIOV_FULL_GUEST(pGpu))
    {
        khwpmStreamoutFreePmaStream(pGpu, pKernelHwpm, pProfiler->profilerId,
                                    &pProfiler->pPmaStreamList[pParams->pmaChannelIdx], pParams->pmaChannelIdx);
    }

    pProfiler->pPmaStreamList[pParams->pmaChannelIdx].bValid = NV_FALSE;
    pProfiler->pPmaStreamList[pParams->pmaChannelIdx].bMemBytesBufferAccessAllowed = NV_FALSE;

    return status;
}

/*
 * To be called on vGPU guest only.
 * Will issue RPC to vGPU host and optionally, wait in guest to get updated available membytes
 */
static NV_STATUS
_profilerBaseCtrlCmdPmaStreamUpdateGetPutVgpuGuest
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);
    volatile NvU32 *pMemBytesAddr = NvP64_VALUE(pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pNumBytesCpuAddr);

    if (pParams->bUpdateAvailableBytes)
    {
        *pMemBytesAddr = NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE;
    }

    status = _issueRpcToHost(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    // Wait on guest for PMA unit to streamout updated available membytes
    if (pProfiler->pPmaStreamList[pParams->pmaChannelIdx].bMemBytesBufferAccessAllowed &&
        pParams->bUpdateAvailableBytes && pParams->bWait)
    {
        RMTIMEOUT timeout = {0};
        threadStateResetTimeout(pGpu);
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        while (*pMemBytesAddr == NVB0CC_AVAILABLE_BYTES_DEFAULT_VALUE)
        {
            if (status == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR, "timeout occurred while waiting for PM streamout to idle.\n");
                break;
            }
            osSpinLoop();
            status = gpuCheckTimeout(pGpu, &timeout);
        }

        pParams->bytesAvailable = *pMemBytesAddr;

        NV_PRINTF(LEVEL_INFO, "status=0x%08x, *MEM_BYTES_ADDR=0x%08x.\n", status, *pMemBytesAddr);
    }

    return status;
}

NV_STATUS
profilerBaseCtrlCmdInternalAllocPmaStream_VF
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams
)
{
    RsResourceRef     *pMemoryPmaBufferRef      = NULL;
    RsResourceRef     *pMemoryPmaAvailBytesRef  = NULL;
    Memory            *pMemPmaBuffer            = NULL;
    Memory            *pMemPmaAvailBytes        = NULL;
    NV_STATUS          status                   = NV_OK;
    NvHandle           hClient                  = RES_GET_CLIENT_HANDLE(pProfiler);
    OBJGPU            *pGpu                     = GPU_RES_GET_GPU(pProfiler);

    if (IS_SILICON(pGpu))
    {
        // bInputPmaChIdx is only allowed to set on emulation/simulation.
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pParams->bInputPmaChIdx == NV_FALSE, NV_ERR_INVALID_ARGUMENT);
    }

    status = serverutilGetResourceRef(hClient, pParams->hMemPmaBuffer, &pMemoryPmaBufferRef);
    if (status != NV_OK)
    {
        return status;
    }

    pMemPmaBuffer = dynamicCast(pMemoryPmaBufferRef->pResource, Memory);
    if (pMemPmaBuffer == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    status = serverutilGetResourceRef(hClient, pParams->hMemPmaBytesAvailable,
                                      &pMemoryPmaAvailBytesRef);
    if (status != NV_OK)
    {
        return status;
    }

    pMemPmaAvailBytes = dynamicCast(pMemoryPmaAvailBytesRef->pResource, Memory);
    if (pMemPmaAvailBytes == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    if ((pParams->pmaBufferSize > NVB0CC_PMA_BUFFER_SIZE_MAX) ||
        (pMemPmaBuffer->pMemDesc == NULL) ||
        (pMemPmaBuffer->pMemDesc->Size < (pParams->pmaBufferSize + pParams->pmaBufferOffset)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pMemPmaAvailBytes->pMemDesc == NULL) ||
        (pMemPmaAvailBytes->pMemDesc->Size < (pParams->pmaBytesAvailableOffset + NVB0CC_PMA_BYTES_AVAILABLE_SIZE)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Client must pass a RO buffer handle for available bytes buffer
    if (!RMCFG_FEATURE_MODS_FEATURES &&
        !FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY, pMemPmaAvailBytes->Attr2))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed: Memory 0x%x provided is not read only (Attr2: 0x%08x).\n",
                   pParams->hMemPmaBytesAvailable, pMemPmaAvailBytes->Attr2);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return _profilerBaseCtrlCmdAllocPmaStreamVgpuGuest(pProfiler, pParams, pMemPmaAvailBytes, pMemPmaBuffer);
}

NV_STATUS
profilerBaseCtrlCmdPmaStreamUpdateGetPut_VF
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);

    if (pParams->pmaChannelIdx > (pKernelHwpm->maxPmaChannels - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return _profilerBaseCtrlCmdPmaStreamUpdateGetPutVgpuGuest(pProfiler, pParams);
}

NV_STATUS
profilerBaseCtrlCmdInternalFreePmaStream_VF
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pNewParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS extParams, *pParams = &extParams;

    portMemSet(pParams, 0, sizeof(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS));
    extParams.pmaChannelIdx = pNewParams->pmaChannelIdx;

    if (pParams->pmaChannelIdx > (pKernelHwpm->maxPmaChannels - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return _profilerBaseCtrlCmdFreePmaStreamVgpuGuest(pProfiler, pParams);
}

/*
 * To be used on full SRIOV only.
 * Promotes the PMA buffers and HWPM IB allocated on VF to PF context
 */
NV_STATUS
profilerBaseCtrlCmdInternalSriovPromotePmaStream_VF
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);
    NvU32 vPmaChIdx = pParams->pmaChannelIdx;
    NvU32 vBpcIdx = vPmaChIdx / pKernelHwpm->maxChannelPerCblock;
    MEMORY_DESCRIPTOR *pInstBlkMemDesc = NULL;

    if (!IS_SRIOV_FULL_GUEST(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pParams->pmaBufferVA = pProfiler->pPmaStreamList[vPmaChIdx].vaddrRecordBuf;
    pParams->membytesVA = pProfiler->pPmaStreamList[vPmaChIdx].vaddrNumBytesBuf;
    pParams->pmaBufferSize = pProfiler->pPmaStreamList[pParams->pmaChannelIdx].pRecordBufDesc->Size;

    pInstBlkMemDesc = pKernelHwpm->streamoutState[vBpcIdx].pInstBlkMemDesc;

    pParams->hwpmIBPA = memdescGetPhysAddr(pInstBlkMemDesc, AT_GPU, 0);
    pParams->hwpmIBAperture = memdescGetAddressSpace(pInstBlkMemDesc);

    // vGPU only supports IBs allocation in FB memory
    NV_ASSERT_OR_RETURN((pParams->hwpmIBAperture == ADDR_FBMEM),
                         NV_ERR_NOT_SUPPORTED);

    return _issueRpcToHost(pGpu);
}

NV_STATUS
profilerBaseCtrlCmdInternalGetMaxPmas_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *pParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pProfiler);
    KernelHwpm *pKernelHwpm = GPU_GET_KERNEL_HWPM(pGpu);

    pParams->maxPmaChannels = pKernelHwpm->maxPmaChannels;
    return NV_OK;
}

/*!
 * @brief Request a set of clock-gating controls on behalf of the calling
 *        profiler object.
 *
 * @param[in]     pProfiler
 * @param[in,out] pParams
 */
NV_STATUS
profilerBaseCtrlCmdRequestCgControls_VF
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    HWPM_POWER_REQUEST_FEATURES_PARAMS requestParams;

    portMemSet(&requestParams, 0, sizeof(HWPM_POWER_REQUEST_FEATURES_PARAMS));

    // Status for all power features will be set to 0 in statusMask only if
    // HWPM_POWER_FEATURE_REQUEST_FULFILLED = 0.
    ct_assert(HWPM_POWER_FEATURE_REQUEST_FULFILLED == 0);
    requestParams.statusMask = HWPM_POWER_FEATURE_REQUEST_FULFILLED;
    requestParams.globalStatus = NV_OK;

    // Copy output status.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, khwpmGetRequestCgStatusMask(&pParams->statusMask, &requestParams));
    pParams->globalStatus = requestParams.globalStatus;

    return status;
}
