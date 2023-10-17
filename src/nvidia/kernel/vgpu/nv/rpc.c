/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Description:
//       This module implements RPC send and receive ring buffers.
//
//******************************************************************************

// FIXME XXX
#define NVOC_KERNEL_GRAPHICS_CONTEXT_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "nvVer.h"
#include "nvBldVer.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "platform/chipset/chipset.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "rmapi/alloc_size.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "rmapi/client_resource.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_version.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"
#include "objtmr.h"
#include "lib/base_utils.h"

#include "gpu/conf_compute/conf_compute.h"

#define SDK_ALL_CLASSES_INCLUDE_FULL_HEADER
#include "g_allclasses.h"
#undef SDK_ALL_CLASSES_INCLUDE_FULL_HEADER
#include "nverror.h"


#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

#define RPC_MESSAGE_STRUCTURES
#define RPC_MESSAGE_GENERIC_UNION
#include "g_rpc-message-header.h"
#undef RPC_MESSAGE_STRUCTURES
#undef RPC_MESSAGE_GENERIC_UNION

#include "g_rpc_private.h"

#include "g_finn_rm_api.h"

#include "gpu/gsp/message_queue_priv.h"

static NvBool bProfileRPC = NV_FALSE;

typedef struct rpc_meter_list
{
    RPC_METER_ENTRY rpcData;
    struct rpc_meter_list *pNext;
} RPC_METER_LIST;

typedef struct rpc_meter_head
{
    RPC_METER_LIST *pHead;
    RPC_METER_LIST *pTail;
} RPC_METER_HEAD;

static RPC_METER_HEAD rpcMeterHead;
static NvU32 rpcProfilerEntryCount;

typedef struct rpc_vgx_version
{
    NvU32 majorNum;
    NvU32 minorNum;
} RPC_VGX_VERSION;

static RPC_VGX_VERSION rpcVgxVersion;
static NvBool bSkipRpcVersionHandshake = NV_FALSE;

void rpcSetIpVersion(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 ipVersion)
{
    OBJHAL *pHal = GPU_GET_HAL(pGpu);
    PMODULEDESCRIPTOR pMod = objhalGetModuleDescriptor(pHal);
    IGRP_IP_VERSIONS_TABLE_INFO info = {0};

    _objrpcAssignIpVersion(pRpc, ipVersion);
    pMod->pHalSetIfaces->rpcHalIfacesSetupFn(&pRpc->_hal);
    info.pGpu = pGpu;
    info.pDynamic = (void*) pRpc;
    rpc_iGrp_ipVersions_getInfo_HAL(pRpc, &info);
    info.ifacesWrapupFn(&info);

}

NV_STATUS rpcConstruct_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS rmStatus = NV_OK;
    return rmStatus;
}

void rpcDestroy_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
}

NV_STATUS rpcSendMessage_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_PRINTF(LEVEL_ERROR, "virtual function not implemented.\n");
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcRecvPoll_IMPL(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 expectedFunc)
{
    NV_PRINTF(LEVEL_ERROR, "virtual function not implemented.\n");
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _issueRpcAndWait(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    RPC_METER_LIST *pNewEntry = NULL;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (bProfileRPC)
    {
        // Create a new entry for our RPC profiler
        pNewEntry = portMemAllocNonPaged(sizeof(RPC_METER_LIST));
        if (pNewEntry == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to allocate RPC meter memory!\n");
            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        portMemSet(pNewEntry, 0, sizeof(RPC_METER_LIST));

        if (rpcMeterHead.pHead == NULL)
            rpcMeterHead.pHead = pNewEntry;
        else
            rpcMeterHead.pTail->pNext = pNewEntry;

        rpcMeterHead.pTail = pNewEntry;

        pNewEntry->rpcData.rpcDataTag = vgpu_rpc_message_header_v->function;

        rpcProfilerEntryCount++;

        osGetPerformanceCounter(&pNewEntry->rpcData.startTimeInNs);
    }

    // For HCC, cache expectedFunc value before encrypting.
    NvU32 expectedFunc = vgpu_rpc_message_header_v->function;

    status = rpcSendMessage(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "rpcSendMessage failed with status 0x%08x for fn %d!\n",
            status, vgpu_rpc_message_header_v->function);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (status == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : status;
    }

    // Use cached expectedFunc here because vgpu_rpc_message_header_v is encrypted for HCC.
    status = rpcRecvPoll(pGpu, pRpc, expectedFunc);
    if (status != NV_OK)
    {
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
                "rpcRecvPoll timedout for fn %d!\n",
                 vgpu_rpc_message_header_v->function);
        }
        else
        {
            NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
                "rpcRecvPoll failed with status 0x%08x for fn %d!\n",
                 status, vgpu_rpc_message_header_v->function);
        }
        return status;
    }

    if (bProfileRPC)
        osGetPerformanceCounter(&pNewEntry->rpcData.endTimeInNs);

    // Now check if RPC really succeeded
    if (vgpu_rpc_message_header_v->rpc_result != NV_VGPU_MSG_RESULT_SUCCESS)
    {
        NV_PRINTF(LEVEL_WARNING, "RPC failed with status 0x%08x for fn %d!\n",
                  vgpu_rpc_message_header_v->rpc_result,
                  vgpu_rpc_message_header_v->function);

        if (vgpu_rpc_message_header_v->rpc_result < DRF_BASE(NV_VGPU_MSG_RESULT__VMIOP))
            return vgpu_rpc_message_header_v->rpc_result;

        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcAsync(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    status = rpcSendMessage(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rpcSendMessage failed with status 0x%08x for fn %d!\n",
                  status, vgpu_rpc_message_header_v->function);
        NV_ASSERT(0);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (status == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : status;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer,
    NvBool bBidirectional,
    NvBool bWait
)
{
    NvU8      *pBuf8         = (NvU8 *)pBuffer;
    NV_STATUS  nvStatus      = NV_OK;
    NvU32      expectedFunc  = vgpu_rpc_message_header_v->function;
    NvU32      entryLength;
    NvU32      remainingSize = bufSize;
    NvU32      recordCount   = 0;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // Copy the initial buffer
    entryLength = NV_MIN(bufSize, pRpc->maxRpcSize);

    if ((NvU8 *)vgpu_rpc_message_header_v != pBuf8)
        portMemCopy(vgpu_rpc_message_header_v, entryLength, pBuf8, entryLength);

    // Set the correct length for this queue entry.
    vgpu_rpc_message_header_v->length = entryLength;

    nvStatus = rpcSendMessage(pGpu, pRpc);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rpcSendMessage failed with status 0x%08x for fn %d!\n",
                  nvStatus, expectedFunc);
        NV_ASSERT(0);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (nvStatus == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : nvStatus;
    }
    remainingSize -= entryLength;
    pBuf8   += entryLength;

    // Copy the remaining buffers
    entryLength = pRpc->maxRpcSize - sizeof(rpc_message_header_v);
    while (remainingSize != 0)
    {
        if (entryLength > remainingSize)
            entryLength = remainingSize;

        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
        if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENCRYPT_ENABLED))
        {
            // Zero out the entire RPC message header to clear the state of previous chunk.
            portMemSet(vgpu_rpc_message_header_v, 0, sizeof(rpc_message_header_v));
        }

        portMemCopy(rpc_message, entryLength, pBuf8, entryLength);

        // Set the correct length for this queue entry.
        vgpu_rpc_message_header_v->length   = entryLength + sizeof(rpc_message_header_v);
        vgpu_rpc_message_header_v->function = NV_VGPU_MSG_FUNCTION_CONTINUATION_RECORD;

        nvStatus = rpcSendMessage(pGpu, pRpc);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "rpcSendMessage failed with status 0x%08x for fn %d continuation record (remainingSize=0x%x)!\n",
                      nvStatus, expectedFunc, remainingSize);
            NV_ASSERT(0);
            //
            // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
            // buffers full and not being serviced) can make things worse, i.e. turn RPC
            // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
            // Avoid this for now while still returning the correct error in other cases.
            //
            return (nvStatus == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : nvStatus;
        }

        remainingSize -= entryLength;
        pBuf8         += entryLength;
        recordCount++;
    }

    if (!bWait)
    {
        // In case of Async RPC, we are done here.
        return nvStatus;
    }

    // Always receive at least one..
    nvStatus = rpcRecvPoll(pGpu, pRpc, expectedFunc);
    if (nvStatus != NV_OK)
    {
        if (nvStatus == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "rpcRecvPoll timedout for fn %d!\n",
                      vgpu_rpc_message_header_v->function);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "rpcRecvPoll failed with status 0x%08x for fn %d!\n",
                      nvStatus, vgpu_rpc_message_header_v->function);
        }
        NV_ASSERT(0);
        return nvStatus;
    }

    pBuf8 = (NvU8 *)pBuffer;
    remainingSize = bufSize;
    entryLength = NV_MIN(bufSize, vgpu_rpc_message_header_v->length);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength <= pRpc->maxRpcSize, NV_ERR_INVALID_STATE);

    if (((NvU8 *)vgpu_rpc_message_header_v != pBuf8) && bBidirectional)
      portMemCopy(pBuf8, entryLength, vgpu_rpc_message_header_v, entryLength);

    remainingSize -= entryLength;
    pBuf8   += entryLength;

    // For bidirectional transfer messages, need to receive all other frames as well
    if (bBidirectional && (recordCount > 0))
    {
        while (remainingSize > 0)
        {
            nvStatus = rpcRecvPoll(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CONTINUATION_RECORD);
            if (nvStatus != NV_OK)
            {
                if (nvStatus == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "rpcRecvPoll timedout for fn %d continuation record (remainingSize=0x%x)!\n",
                              vgpu_rpc_message_header_v->function, remainingSize);
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "rpcRecvPoll failed with status 0x%08x for fn %d continuation record! (remainingSize=0x%x)\n",
                              nvStatus, vgpu_rpc_message_header_v->function, remainingSize);
                }
                NV_ASSERT(0);
                return nvStatus;
            }

            entryLength = vgpu_rpc_message_header_v->length;
            NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength <= pRpc->maxRpcSize, NV_ERR_INVALID_STATE);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength >= sizeof(rpc_message_header_v), NV_ERR_INVALID_STATE);
            entryLength -= sizeof(rpc_message_header_v);

            if (entryLength > remainingSize)
                entryLength = remainingSize;

            portMemCopy(pBuf8, entryLength, rpc_message, entryLength);
            remainingSize -= entryLength;
            pBuf8         += entryLength;
            recordCount--;
        }
        vgpu_rpc_message_header_v->function = expectedFunc;
        NV_ASSERT(recordCount == 0);
    }

    // Now check if RPC really succeeded
    if (vgpu_rpc_message_header_v->rpc_result != NV_VGPU_MSG_RESULT_SUCCESS)
    {
        NV_PRINTF(LEVEL_WARNING, "RPC failed with status 0x%08x for fn %d!\n",
                  vgpu_rpc_message_header_v->rpc_result,
                  vgpu_rpc_message_header_v->function);

        if (vgpu_rpc_message_header_v->rpc_result < DRF_BASE(NV_VGPU_MSG_RESULT__VMIOP))
            return vgpu_rpc_message_header_v->rpc_result;

        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcAndWaitLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer,
    NvBool bBidirectional
)
{
    return _issueRpcLarge(pGpu, pRpc, bufSize, pBuffer,
                          bBidirectional,
                          NV_TRUE);  //bWait
}

static NV_STATUS _issueRpcAsyncLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer
)
{
    return _issueRpcLarge(pGpu, pRpc, bufSize, pBuffer,
                          NV_FALSE,  //bBidirectional
                          NV_FALSE); //bWait
}

static NV_STATUS _issuePteDescRpc
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvU32       offsetToPTE,
    NvU32       pageCount,
    RmPhysAddr *guestPages,
    NvBool      physicallyContiguous
)
{
    rpc_message_header_v    *pHdr             = vgpu_rpc_message_header_v;
    void                    *pAllocatedRecord = NULL;
    struct pte_desc         *pPteDesc;
    NvU64                    contigBase;
    NV_STATUS                nvStatus         = NV_OK;
    NvU32                    recordSize;
    NvU32                    i;
    DMA_PAGE_ARRAY           pageArray;

    NV_ASSERT_OR_RETURN(pGpu       != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRpc       != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(guestPages != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pHdr       != NULL, NV_ERR_INVALID_ARGUMENT);

    recordSize = offsetToPTE + NV_OFFSETOF(struct pte_desc, pte_pde[0].pte) +
        (pageCount * NV_VGPU_PTE_64_SIZE);

    if (recordSize > pRpc->maxRpcSize)
    {
        // Multiple queue entries.  Create a temporary buffer for the PTEs.
        pAllocatedRecord = portMemAllocNonPaged(recordSize);
        if (pAllocatedRecord == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "no memory for allocated record\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        // Copy in the record so far.
        portMemCopy(pAllocatedRecord, pHdr->length, pHdr, pHdr->length);

        // Point to the allocated record.
        pHdr = (rpc_message_header_v *)pAllocatedRecord;
    }

    dmaPageArrayInit(&pageArray, guestPages, pageCount);

    pPteDesc         = (struct pte_desc *)NvP64_PLUS_OFFSET(pHdr, offsetToPTE);
    pPteDesc->idr    = NV_VGPU_PTEDESC_IDR_NONE;
    pPteDesc->length = pageCount;
    contigBase       = (dmaPageArrayGetPhysAddr(&pageArray, 0) >> RM_PAGE_SHIFT);

    for (i = 0; i < pageCount; i++)
    {
        if (physicallyContiguous)
            pPteDesc->pte_pde[i].pte = contigBase + i;
        else
            pPteDesc->pte_pde[i].pte =
                (dmaPageArrayGetPhysAddr(&pageArray, i) >> RM_PAGE_SHIFT);
    }

    nvStatus = _issueRpcAndWaitLarge(pGpu, pRpc,  recordSize, pHdr, NV_FALSE);

    portMemFree(pAllocatedRecord);

    return nvStatus;
}

NV_STATUS rpcAllocMemory_v13_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hMemory, NvU32 hClass,
                               NvU32 flags, MEMORY_DESCRIPTOR *pMemDesc)
{
    NV_STATUS status = NV_OK;

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: AllocMemory: pMemDesc arg was NULL\n");
        return NV_ERR_GENERIC;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_MEMORY, sizeof(rpc_alloc_memory_v13_01));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_memory_v13_01.hClient   = hClient;
    rpc_message->alloc_memory_v13_01.hDevice   = hDevice;
    rpc_message->alloc_memory_v13_01.hMemory   = hMemory;
    rpc_message->alloc_memory_v13_01.hClass    = hClass;
    rpc_message->alloc_memory_v13_01.flags     = flags;
    rpc_message->alloc_memory_v13_01.pteAdjust = pMemDesc->PteAdjust;
    rpc_message->alloc_memory_v13_01.format    = memdescGetPteKind(pMemDesc);
    rpc_message->alloc_memory_v13_01.length    = pMemDesc->Size;
    rpc_message->alloc_memory_v13_01.pageCount = (NvU32)pMemDesc->PageCount;

    if (IS_GSP_CLIENT(pGpu))
    {
        status = _issuePteDescRpc(pGpu, pRpc,
                                  NV_OFFSETOF(rpc_message_header_v, rpc_message_data[0].alloc_memory_v13_01.pteDesc),
                                  pMemDesc->PageCount,
                                  memdescGetPteArray(pMemDesc, AT_GPU),
                                  memdescGetContiguity(pMemDesc, AT_GPU));
    }

    return status;
}

NV_STATUS rpcMapMemoryDma_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hDma, NvHandle hMemory,
                                NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset)
{
    NV_STATUS status;
    NVOS46_PARAMETERS_v03_00 *rpc_params = &rpc_message->map_memory_dma_v03_00.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_MAP_MEMORY_DMA, sizeof(rpc_map_memory_dma_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hDevice = hDevice;
    rpc_params->hDma    = hDma;
    rpc_params->hMemory = hMemory;
    rpc_params->flags   = flags;

    rpc_params->offset      = offset;
    rpc_params->length      = length;
    rpc_params->dmaOffset   = *pDmaOffset;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        *pDmaOffset = rpc_params->dmaOffset;
    }
    return status;
}

NV_STATUS rpcUnmapMemoryDma_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hDma,
                                  NvHandle hMemory, NvU32 flags, NvU64 pDmaOffset)
{
    NV_STATUS status;
    NVOS47_PARAMETERS_v03_00 *rpc_params = &rpc_message->unmap_memory_dma_v03_00.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNMAP_MEMORY_DMA, sizeof(rpc_unmap_memory_dma_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient     = hClient;
    rpc_params->hDevice     = hDevice;
    rpc_params->hDma        = hDma;
    rpc_params->hMemory     = hMemory;
    rpc_params->flags       = flags;
    rpc_params->dmaOffset   = pDmaOffset;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

/* max entries is how many 3 DWORD entries fit in what remains of the message_buffer */
#define IDLE_CHANNELS_MAX_ENTRIES_v03_00   \
    ((pRpc->maxRpcSize - (sizeof(rpc_message_header_v) + sizeof(rpc_idle_channels_v03_00))) / sizeof(idle_channel_list_v03_00))

NV_STATUS rpcIdleChannels_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle *phClients, NvHandle *phDevices, NvHandle *phChannels,
                                NvU32 numEntries, NvU32 flags, NvU32 timeout)
{
    NV_STATUS status;
    NvU32 i;

    if (numEntries > IDLE_CHANNELS_MAX_ENTRIES_v03_00)
    {
        // unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: IdleChannels: requested %u entries (but only room for %u)\n",
                  numEntries, (NvU32)IDLE_CHANNELS_MAX_ENTRIES_v03_00);
        return NV_ERR_GENERIC;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_IDLE_CHANNELS,
                                sizeof(rpc_idle_channels_v03_00) + numEntries * sizeof(idle_channel_list_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->idle_channels_v03_00.flags       = flags;
    rpc_message->idle_channels_v03_00.timeout     = timeout;
    rpc_message->idle_channels_v03_00.nchannels   = numEntries;

    for (i = 0; i < numEntries; i++)
    {
        rpc_message->idle_channels_v03_00.channel_list[i].phClient  = ((NvU32) phClients[i]);
        rpc_message->idle_channels_v03_00.channel_list[i].phDevice  = ((NvU32) phDevices[i]);
        rpc_message->idle_channels_v03_00.channel_list[i].phChannel = ((NvU32) phChannels[i]);
    }

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS RmRpcSetGuestSystemInfo(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS status = NV_OK;
    NvS32 message_buffer_remaining;
    NvU32 data_len;

    if (pGpuMgr->numGpuHandles == 0)
    {
        rpcVgxVersion.majorNum = 0;
        rpcVgxVersion.minorNum = 0;
    }

    //
    // Skip RPC version handshake if we've already done it on one GPU.
    //
    // For GSP: Multi GPU setup can have pre-Turing GPUs
    // and GSP offload is disabled for all pre-Turing GPUs.
    // Don't skip RPC version handshake for GSP_CLIENT or if VGPU-GSP plugin offload is enabled.
    // There are different GSPs/plugins for different GPUs and we need to have a handshake with all of them.
    //

    if (pGpuMgr->numGpuHandles > 1 && !IS_GSP_CLIENT(pGpu) && !(IS_VIRTUAL(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))
    {
        if (rpcVgxVersion.majorNum != 0)
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH) && !bSkipRpcVersionHandshake)
            {
                bSkipRpcVersionHandshake = NV_TRUE;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO,
                          "NVRM_RPC: Skipping RPC version handshake for instance 0x%x\n",
                          gpuGetInstance(pGpu));
                goto skip_ver_handshake;
            }
        }
        else
        {
            status = NV_ERR_GENERIC;
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: RPC version handshake already failed. Bailing out for device"
                      " instance 0x%x\n", gpuGetInstance(pGpu));
            goto skip_ver_handshake;
        }
    }

    message_buffer_remaining = pRpc->maxRpcSize - (sizeof(rpc_message_header_v) +
                                               sizeof(rpc_set_guest_system_info_v));

    if (message_buffer_remaining < 0)
    {
        // unable to fit the data in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: SetGuestSystemInfo: Insufficient space on message buffer\n");

        return NV_ERR_BUFFER_TOO_SMALL;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_GUEST_SYSTEM_INFO,
                       sizeof(rpc_set_guest_system_info_v));
    if (status != NV_OK)
        return status;

    if(sizeof(NV_VERSION_STRING) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestDriverVersionBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestDriverVersion,
                       sizeof(rpc_message->set_guest_system_info_v.guestDriverVersion),
                       (const char*)NV_VERSION_STRING, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if(sizeof(NV_BUILD_BRANCH_VERSION) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestVersionBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestVersion,
                       sizeof(rpc_message->set_guest_system_info_v.guestVersion),
                       (const char*)NV_BUILD_BRANCH_VERSION, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if (sizeof(NV_DISPLAY_DRIVER_TITLE) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestTitleBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestTitle,
                       sizeof(rpc_message->set_guest_system_info_v.guestTitle),
                       (const char*)NV_DISPLAY_DRIVER_TITLE, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    rpc_message->set_guest_system_info_v.guestClNum = NV_BUILD_CHANGELIST_NUM;
    rpc_message->set_guest_system_info_v.vgxVersionMajorNum = VGX_MAJOR_VERSION_NUMBER;
    rpc_message->set_guest_system_info_v.vgxVersionMinorNum = VGX_MINOR_VERSION_NUMBER;

    status = _issueRpcAndWait(pGpu, pRpc);

    if ((status == NV_OK) && (vgpu_rpc_message_header_v->rpc_result_private != NV_OK))
    {
        status = vgpu_rpc_message_header_v->rpc_result_private;
        if ((rpc_message->set_guest_system_info_v.vgxVersionMajorNum != VGX_MAJOR_VERSION_NUMBER) ||
            (rpc_message->set_guest_system_info_v.vgxVersionMinorNum != VGX_MINOR_VERSION_NUMBER))
        {
            if (RPC_VERSION_FROM_VGX_VERSION(rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                                             rpc_message->set_guest_system_info_v.vgxVersionMinorNum) >=
                RPC_VERSION_FROM_VGX_VERSION(NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MAJOR,
                                             NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MINOR))
            {
                NV_PRINTF(LEVEL_WARNING,
                          "NVRM_RPC: SetGuestSystemInfo: Guest VGX version (%d.%d) is newer than "
                          "the host VGX version (%d.%d)\n"
                          "NVRM_RPC: SetGuestSystemInfo: Retrying with the VGX version requested "
                          "by the host.\n", VGX_MAJOR_VERSION_NUMBER,
                          VGX_MINOR_VERSION_NUMBER,
                          rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                          rpc_message->set_guest_system_info_v.vgxVersionMinorNum);
                status = _issueRpcAndWait(pGpu, pRpc);
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "NVRM_RPC: SetGuestSystemInfo: The host version (%d.%d) is too old.\n"
                          "NVRM_RPC: SetGuestSystemInfo: Minimum required host version is %d.%d.\n",
                          rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                          rpc_message->set_guest_system_info_v.vgxVersionMinorNum,
                          NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MAJOR,
                          NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MINOR);

                NV_RM_RPC_LOG(pGpu, "######## Guest NVIDIA Driver Information: ########", NV_VGPU_LOG_LEVEL_NOTICE);
                NV_RM_RPC_LOG(pGpu, "Driver Version: "NV_VERSION_STRING, NV_VGPU_LOG_LEVEL_NOTICE);
                NV_RM_RPC_LOG(pGpu, "Incompatible Guest/Host drivers: Host VGX version is older than the minimum version "
                             "supported by the Guest. Disabling vGPU.", NV_VGPU_LOG_LEVEL_ERROR);
            }
        }
    }

    if (status == NV_OK)
    {
        if (rpcVgxVersion.majorNum != 0)
        {
            if (rpcVgxVersion.majorNum != rpc_message->set_guest_system_info_v.vgxVersionMajorNum ||
                rpcVgxVersion.minorNum != rpc_message->set_guest_system_info_v.vgxVersionMinorNum)
            {
                return NV_ERR_INVALID_STATE;
            }
        }

        rpcVgxVersion.majorNum = rpc_message->set_guest_system_info_v.vgxVersionMajorNum;
        rpcVgxVersion.minorNum = rpc_message->set_guest_system_info_v.vgxVersionMinorNum;
    }

skip_ver_handshake:
    if (status == NV_OK)
    {
        rpcSetIpVersion(pGpu, pRpc,
                        RPC_VERSION_FROM_VGX_VERSION(rpcVgxVersion.majorNum,
                                                     rpcVgxVersion.minorNum));

        NV_RM_RPC_SET_GUEST_SYSTEM_INFO_EXT(pGpu, status);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "SET_GUEST_SYSTEM_INFO_EXT : failed.\n");
        }
    }

    return status;
}

NV_STATUS rpcUnloadingGuestDriver_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvBool bInPMTransition, NvBool bGc6Entering, NvU32 newPMLevel)
{
    NV_STATUS status = NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNLOADING_GUEST_DRIVER, 0);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}


NV_STATUS rpcUnloadingGuestDriver_v1F_07(OBJGPU *pGpu, OBJRPC *pRpc, NvBool bInPMTransition, NvBool bGc6Entering, NvU32 newPMLevel)
{
    NV_STATUS status = NV_OK;
    NvU32 headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_unloading_guest_driver_v1F_07);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unloading guest driver parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);

        NV_ASSERT(0);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNLOADING_GUEST_DRIVER, sizeof(rpc_unloading_guest_driver_v1F_07));
    if (status != NV_OK)
        return status;
    rpc_message->unloading_guest_driver_v1F_07.bInPMTransition = bInPMTransition;
    rpc_message->unloading_guest_driver_v1F_07.bGc6Entering = bGc6Entering;
    rpc_message->unloading_guest_driver_v1F_07.newLevel = newPMLevel;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcGpuExecRegOps_v12_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject,
                             NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pParams,
                             NV2080_CTRL_GPU_REG_OP *pRegOps)
{
    NV_STATUS status;
    NvU32 i, j, regOpsExecuted = 0;

    if (pParams == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->regOpCount == 0)
    {
        NV_PRINTF(LEVEL_ERROR,"RegOps RPC failed: Invalid regOp count - requested 0x%x regOps\n", pParams->regOpCount);
        return NV_ERR_INVALID_ARGUMENT;
    }

    /* RPC message buffer can accomodate a maximum of VGPU_MAX_REGOPS_PER_RPC regops only.
     * This value must be adjusted(if required) in case of any change to the internal
     * RegOps RPC structures.
     */
    if (pRpc->maxRpcSize <
        (sizeof(rpc_message_header_v) +
         sizeof(rpc_gpu_exec_reg_ops_v12_01) +
         VGPU_MAX_REGOPS_PER_RPC * sizeof(NV2080_CTRL_GPU_REG_OP_v03_00))) {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: rpcGpuExecRegOps_v12_01: Insufficient space on message buffer\n");
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GPU_EXEC_REG_OPS,
                       sizeof(rpc_gpu_exec_reg_ops_v12_01));
    if (status != NV_OK)
        return status;

    rpc_message->gpu_exec_reg_ops_v12_01.hClient = hClient;
    rpc_message->gpu_exec_reg_ops_v12_01.hObject = hObject;

    // copy params into the message buffer
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.hClientTarget     = pParams->hClientTarget;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.hChannelTarget    = pParams->hChannelTarget;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.regOpCount        = pParams->regOpCount;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.grRouteInfo.flags = pParams->grRouteInfo.flags;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.grRouteInfo.route = pParams->grRouteInfo.route;

    while (regOpsExecuted < pParams->regOpCount){
        for (i = 0, j = regOpsExecuted; i < VGPU_MAX_REGOPS_PER_RPC && j < pParams->regOpCount; i++, j++)
        {
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regOp           = pRegOps[j].regOp;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regType         = pRegOps[j].regType;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regStatus       = pRegOps[j].regStatus;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regQuad         = pRegOps[j].regQuad;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regGroupMask    = pRegOps[j].regGroupMask;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regSubGroupMask = pRegOps[j].regSubGroupMask;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regOffset       = pRegOps[j].regOffset;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueHi      = pRegOps[j].regValueHi;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueLo      = pRegOps[j].regValueLo;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regAndNMaskHi   = pRegOps[j].regAndNMaskHi;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regAndNMaskLo   = pRegOps[j].regAndNMaskLo;
        }
        rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.regOpCount = i;

        status = _issueRpcAndWait(pGpu, pRpc);

        if (status == NV_OK)
        {
            status = vgpu_rpc_message_header_v->rpc_result_private;
            if (status == NV_OK)
            {
                for (i = 0, j = regOpsExecuted; i < VGPU_MAX_REGOPS_PER_RPC && j < pParams->regOpCount; i++, j++)
                {
                    pRegOps[j].regStatus  = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regStatus;
                    pRegOps[j].regValueHi = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueHi;
                    pRegOps[j].regValueLo = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueLo;
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,"RegOps RPC failed: skipping 0x%x regOps\n", pParams->regOpCount - regOpsExecuted);
            }
        }
        regOpsExecuted = j;
    }

    return status;
}

NV_STATUS rpcGetStaticInfo_v17_05(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_03(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_04(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_0E(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_10(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_11(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_13(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v18_16(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v19_00(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v1A_00(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v1A_05(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetStaticInfo_v20_01(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    return status;
}

NV_STATUS rpcGetGspStaticInfo_v14_00(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        NvU32 headerLength;
        GspStaticConfigInfo *pSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        GspStaticConfigInfo *rpcInfo = (GspStaticConfigInfo *)&rpc_message->get_gsp_static_info_v14_00.data;

        NV_ASSERT_OR_RETURN(pSCI, NV_ERR_INVALID_POINTER);

        headerLength = sizeof(rpc_message_header_v) +
                       sizeof(GspStaticConfigInfo);
        if (headerLength > pRpc->maxRpcSize)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Gsp static info parameters size (0x%x) exceed message_buffer size (0x%x)\n",
                      headerLength, pRpc->maxRpcSize);

            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc,
                                    NV_VGPU_MSG_FUNCTION_GET_GSP_STATIC_INFO,
                                    sizeof(GspStaticConfigInfo));
        if (status != NV_OK)
            return status;

        status = _issueRpcAndWait(pGpu, pRpc);
        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

        // Copy info
        portMemCopy(pSCI, sizeof(*pSCI), rpcInfo, sizeof(*rpcInfo));
    }

    return status;
}

NV_STATUS rpcUpdateBarPde_v15_00(OBJGPU *pGpu, OBJRPC *pRpc, NV_RPC_UPDATE_PDE_BAR_TYPE barType, NvU64 entryValue, NvU64 entryLevelShift)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        UpdateBarPde_v15_00 *rpc_params = &rpc_message->update_bar_pde_v15_00.info;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UPDATE_BAR_PDE,
                                    sizeof(rpc_update_bar_pde_v15_00));
        if (status != NV_OK)
        {
            return status;
        }

        rpc_params->barType         = barType;
        rpc_params->entryValue      = entryValue;
        rpc_params->entryLevelShift = entryLevelShift;

        status = _issueRpcAndWait(pGpu, pRpc);
    }

    return status;
}

NV_STATUS rpcSetPageDirectory_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS_v03_00 *rpc_params = &rpc_message->set_page_directory_v03_00.params;

    if (hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {
        if (!FLD_TEST_DRF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY, _FLAGS_APERTURE, _VIDMEM, pParams->flags))
        {
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_PAGE_DIRECTORY, sizeof(rpc_set_page_directory_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->set_page_directory_v03_00.hClient = hClient;
    rpc_message->set_page_directory_v03_00.hDevice = hDevice;

    rpc_params->physAddress     = pParams->physAddress;
    rpc_params->numEntries      = pParams->numEntries;
    rpc_params->flags           = pParams->flags;
    rpc_params->hVASpace        = pParams->hVASpace;
    rpc_params->chId            = pParams->chId;
    rpc_params->subDeviceId     = pParams->subDeviceId;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;

}

NV_STATUS rpcSetPageDirectory_v1E_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS_v1E_05 *rpc_params = &rpc_message->set_page_directory_v1E_05.params;

    if (hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {
        if (!FLD_TEST_DRF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY, _FLAGS_APERTURE, _VIDMEM, pParams->flags))
        {
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_PAGE_DIRECTORY, sizeof(rpc_set_page_directory_v1E_05));
    if (status != NV_OK)
        return status;

    rpc_message->set_page_directory_v1E_05.hClient = hClient;
    rpc_message->set_page_directory_v1E_05.hDevice = hDevice;

    rpc_params->physAddress     = pParams->physAddress;
    rpc_params->numEntries      = pParams->numEntries;
    rpc_params->flags           = pParams->flags;
    rpc_params->hVASpace        = pParams->hVASpace;
    rpc_params->chId            = pParams->chId;
    rpc_params->subDeviceId     = pParams->subDeviceId;
    rpc_params->pasid           = pParams->pasid;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;

}

NV_STATUS rpcUnsetPageDirectory_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                       NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS_v03_00 *rpc_params = &rpc_message->unset_page_directory_v03_00.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNSET_PAGE_DIRECTORY, sizeof(rpc_unset_page_directory_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->unset_page_directory_v03_00.hClient = hClient;
    rpc_message->unset_page_directory_v03_00.hDevice = hDevice;

    rpc_params->hVASpace    = pParams->hVASpace;
    rpc_params->subDeviceId = pParams->subDeviceId;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcUnsetPageDirectory_v1E_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                       NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS_v1E_05 *rpc_params = &rpc_message->unset_page_directory_v1E_05.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNSET_PAGE_DIRECTORY, sizeof(rpc_unset_page_directory_v1E_05));
    if (status != NV_OK)
        return status;

    rpc_message->unset_page_directory_v1E_05.hClient = hClient;
    rpc_message->unset_page_directory_v1E_05.hDevice = hDevice;

    rpc_params->hVASpace    = pParams->hVASpace;
    rpc_params->subDeviceId = pParams->subDeviceId;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcVgpuPfRegRead32_v15_00(OBJGPU *pGpu,
                                    OBJRPC *pRpc,
                                    NvU64 address,
                                    NvU32 *value,
                                    NvU32 grEngId)
{

    NV_STATUS status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_VGPU_PF_REG_READ32,
                                sizeof(rpc_vgpu_pf_reg_read32_v15_00));
    if (status != NV_OK)
        return status;

    rpc_message->vgpu_pf_reg_read32_v15_00.address = address;
    rpc_message->vgpu_pf_reg_read32_v15_00.grEngId = grEngId;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        *value = rpc_message->vgpu_pf_reg_read32_v15_00.value;
    }

    return status;
}

/*
 * Tells GSP-RM about the overall system environment, such as what physical
 * memory addresses to use.
 *
 * Note that this is an asynchronous RPC.  It is stuffed into the message queue
 * before the GSP is booted.
 */
NV_STATUS rpcGspSetSystemInfo_v17_00
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
        GspSystemInfo *rpcInfo = (GspSystemInfo *)&rpc_message->gsp_set_system_info_v17_00.data;
        const NvU32 messageLength = sizeof(rpc_message_header_v) + sizeof(*rpcInfo);

        if (messageLength > pRpc->maxRpcSize)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GSP_SET_SYSTEM_INFO parameters size (0x%x) exceed message_buffer size (0x%x)\n",
                      messageLength, pRpc->maxRpcSize);

            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc,
                                    NV_VGPU_MSG_FUNCTION_GSP_SET_SYSTEM_INFO,
                                    sizeof(GspSystemInfo));
        if (status != NV_OK)
            return status;

        rpcInfo->gpuPhysAddr           = pGpu->busInfo.gpuPhysAddr;
        rpcInfo->gpuPhysFbAddr         = pGpu->busInfo.gpuPhysFbAddr;
        rpcInfo->gpuPhysInstAddr       = pGpu->busInfo.gpuPhysInstAddr;
        rpcInfo->nvDomainBusDeviceFunc = pGpu->busInfo.nvDomainBusDeviceFunc;
        rpcInfo->oorArch               = (NvU8)pGpu->busInfo.oorArch;

        KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        if (pKernelBif != NULL)
        {
            NV_ASSERT_OK(kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif,
                                                            &rpcInfo->pciConfigMirrorBase,
                                                            &rpcInfo->pciConfigMirrorSize));

            // Cache MNOC interface support
            rpcInfo->bMnocAvailable = pKernelBif->bMnocAvailable;

            // Cache FLR and 64b Bar0 support
            rpcInfo->bFlrSupported     = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED);
            rpcInfo->b64bBar0Supported = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED);
        }

        if (IS_SIMULATION(pGpu))
        {
            KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
            rpcInfo->simAccessBufPhysAddr = memdescGetPhysAddr(pKernelGsp->pMemDesc_simAccessBuf, AT_CPU, 0);
        }
        else
        {
            rpcInfo->simAccessBufPhysAddr = 0;
        }
        rpcInfo->pcieAtomicsOpMask = GPU_GET_KERNEL_BIF(pGpu) ?
            GPU_GET_KERNEL_BIF(pGpu)->osPcieAtomicsOpMask : 0U;
        rpcInfo->consoleMemSize = GPU_GET_MEMORY_MANAGER(pGpu)->Ram.ReservedConsoleDispMemSize;
        rpcInfo->maxUserVa      = osGetMaxUserVa();

        OBJCL *pCl = SYS_GET_CL(SYS_GET_INSTANCE());
        if (pCl != NULL)
        {
            clSyncWithGsp(pCl, rpcInfo);
        }

        // Fill in the cached ACPI method data
        rpcInfo->acpiMethodData = pGpu->acpiMethodData;

        // Fill in ASPM related GPU flags
        rpcInfo->bGpuBehindBridge         = pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE);
        rpcInfo->bUpstreamL0sUnsupported  = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED);
        rpcInfo->bUpstreamL1Unsupported   = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED);
        rpcInfo->bUpstreamL1PorSupported  = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED);
        rpcInfo->bUpstreamL1PorMobileOnly = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY);
        rpcInfo->upstreamAddressValid     = pGpu->gpuClData.upstreamPort.addr.valid;

        rpcInfo->hypervisorType           = hypervisorGetHypervisorType(pHypervisor);
        rpcInfo->bIsPassthru              = pGpu->bIsPassthru;

        // Fill in VF related GPU flags
        rpcInfo->gspVFInfo.totalVFs           = pGpu->sriovState.totalVFs;
        rpcInfo->gspVFInfo.firstVFOffset      = pGpu->sriovState.firstVFOffset;
        rpcInfo->gspVFInfo.FirstVFBar0Address = pGpu->sriovState.firstVFBarAddress[0];
        rpcInfo->gspVFInfo.FirstVFBar1Address = pGpu->sriovState.firstVFBarAddress[1];
        rpcInfo->gspVFInfo.FirstVFBar2Address = pGpu->sriovState.firstVFBarAddress[2];
        rpcInfo->gspVFInfo.b64bitBar0         = pGpu->sriovState.b64bitVFBar0;
        rpcInfo->gspVFInfo.b64bitBar1         = pGpu->sriovState.b64bitVFBar1;
        rpcInfo->gspVFInfo.b64bitBar2         = pGpu->sriovState.b64bitVFBar2;

        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        rpcInfo->sysTimerOffsetNs = pTmr->sysTimerOffsetNs;

        status = _issueRpcAsync(pGpu, pRpc);
    }

    return status;
}

/*
 * Transfers registry entries from CPU-RM to GSP-RM during init.
 *
 * Note that this is an asynchronous RPC.  It is stuffed into the message queue
 * before the GSP is booted.
 */
NV_STATUS rpcSetRegistry_v17_00
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        NvU32 regTableSize = 0;
        NvU32 totalSize;
        NvU32 remainingMessageSize;
        PACKED_REGISTRY_TABLE *pRegTable;
        rpc_message_header_v *largeRpcBuffer = NULL;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_REGISTRY, 0);
        if (status != NV_OK)
            return status;

        remainingMessageSize = pRpc->maxRpcSize - sizeof(rpc_message_header_v);

        // Compute size of registry table
        status = osPackageRegistry(pGpu, NULL, &regTableSize);
        if (status != NV_OK)
            return status;

        //
        // SET_REGISTRY is async RPC. If registry table exceeds size of
        // message queue, we won't be able to send complete data and it's
        // time to evaluate alternate implementations. Some ways to resolve
        // this issue are use bigger queue, use sync RPC or allocate dedicated
        // memory for sharing regkey table with GSP-RM.
        //
        totalSize = sizeof(rpc_message_header_v) + regTableSize;
        NV_ASSERT(totalSize < pRpc->pMessageQueueInfo->commandQueueSize);

        // Find out if we need to issue large RPC
        if (regTableSize > remainingMessageSize)
        {
            largeRpcBuffer = portMemAllocNonPaged(totalSize);
            if (largeRpcBuffer == NULL)
                return NV_ERR_NO_MEMORY;

            portMemCopy(largeRpcBuffer, totalSize,
                        vgpu_rpc_message_header_v, sizeof(rpc_message_header_v));

            pRegTable = (PACKED_REGISTRY_TABLE *)(&largeRpcBuffer->rpc_message_data);
        }
        else
        {
            pRegTable = (PACKED_REGISTRY_TABLE *)&rpc_message;
        }

        status = osPackageRegistry(pGpu, pRegTable, &regTableSize);
        if (status != NV_OK)
            goto fail;

        if (largeRpcBuffer != NULL)
        {
            status = _issueRpcAsyncLarge(pGpu, pRpc, totalSize, largeRpcBuffer);
        }
        else
        {
            vgpu_rpc_message_header_v->length = totalSize;
            status = _issueRpcAsync(pGpu, pRpc);
        }

    fail:
        portMemFree(largeRpcBuffer);
    }

    return status;
}

NV_STATUS rpcDumpProtobufComponent_v18_12
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc,
    PRB_ENCODER           *pPrbEnc,
    NVD_STATE             *pNvDumpState,
    NVDUMP_COMPONENT       component
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        rpc_dump_protobuf_component_v18_12 *rpc_params = &rpc_message->dump_protobuf_component_v18_12;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DUMP_PROTOBUF_COMPONENT,
                                    sizeof(*rpc_params));
        if (status != NV_OK)
            return status;

        rpc_params->component    = component;
        rpc_params->nvDumpType   = pNvDumpState->nvDumpType;
        rpc_params->countOnly    = ((pPrbEnc->flags & PRB_COUNT_ONLY) != 0);
        rpc_params->bugCheckCode = pNvDumpState->bugCheckCode;
        rpc_params->internalCode = pNvDumpState->internalCode;
        rpc_params->bufferSize   = NV_MIN(pRpc->maxRpcSize, prbEncBufLeft(pPrbEnc));

        status = _issueRpcAndWait(pGpu, pRpc);

        // Add blob to protobuf.
        if ((status == NV_OK) && rpc_params->bufferSize > 0)
            status = prbEncStubbedAddBytes(pPrbEnc, rpc_params->blob, rpc_params->bufferSize);
    }

    return status;
}

NV_STATUS rpcRmfsInit_v15_00
(
    OBJGPU              *pGpu,
    OBJRPC              *pRpc,
    PMEMORY_DESCRIPTOR   pStatusQueueMemDesc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    return status;
}

NV_STATUS rpcRmfsCloseQueue_v15_00
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    return status;
}

NV_STATUS rpcRmfsCleanup_v15_00
(
    OBJGPU *pGpu,
    OBJRPC *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    return status;
}

NV_STATUS rpcRmfsTest_v15_00
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32   numReps,
    NvU32   flags,
    NvU32   testData1,
    NvU32   testData2
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    return status;
}

#if NV_PRINTF_STRINGS_ALLOWED
void osAssertFailed(void);
#define RPC_LOCK_DEBUG_DUMP_STACK()                                     \
    do {                                                                \
        static NvU64 previousRetAddr;                                   \
        NvU64 retAddr = (NvU64)NV_RETURN_ADDRESS();                     \
        if (retAddr != previousRetAddr)                                 \
        {                                                               \
            previousRetAddr = retAddr;                                  \
            osAssertFailed();                                           \
        }                                                               \
        /* Add an assert so it shows as test score regression */        \
        NV_ASSERT_FAILED("RPC locking violation - see kernel_log.txt"); \
    } while(0)
#else
#define RPC_LOCK_DEBUG_DUMP_STACK()
#endif

NV_STATUS rpcRmApiControl_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject,
    NvU32 cmd,
    void *pParamStructPtr,
    NvU32 paramsSize
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    rpc_message_header_v *large_message_copy = NULL;
    rpc_gsp_rm_control_v03_00 *rpc_params = &rpc_message->gsp_rm_control_v03_00;

    const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(*rpc_params);
    NvU32 message_buffer_remaining = pRpc->maxRpcSize - fixed_param_size;
    NvU32 rpc_params_size;
    NvU32 total_size;

    NvU32 gpuMaskRelease = 0;
    NvU32 ctrlFlags = 0;
    NvU32 ctrlAccessRight = 0;
    NvBool bCacheable;

    CALL_CONTEXT *pCallContext;
    CALL_CONTEXT newContext;
    NvU32 resCtrlFlags = NVOS54_FLAGS_NONE;
    NvBool bPreSerialized = NV_FALSE;
    void *pOriginalParams = pParamStructPtr;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmControl 0x%08x without adequate locks!\n", cmd);
        RPC_LOCK_DEBUG_DUMP_STACK();

        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    (void) rmapiutilGetControlInfo(cmd, &ctrlFlags, &ctrlAccessRight);
    bCacheable = rmapiControlIsCacheable(ctrlFlags, ctrlAccessRight, NV_TRUE);

    pCallContext = resservGetTlsCallContext();
    if (pCallContext == NULL || pCallContext->bReserialize)
    {
        // This should only happen when using the internal physical RMAPI
        NV_ASSERT_OR_RETURN(pRmApi == GPU_GET_PHYSICAL_RMAPI(pGpu), NV_ERR_INVALID_STATE);

        portMemSet(&newContext, 0, sizeof(newContext));
        pCallContext = &newContext;
    }

    if (pCallContext->pControlParams != NULL)
    {
        resCtrlFlags = pCallContext->pControlParams->flags;
    }

    if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        bPreSerialized = NV_TRUE;
    }
    else
    {
        status = serverSerializeCtrlDown(pCallContext, cmd, &pParamStructPtr, &paramsSize, &resCtrlFlags);
        if (status != NV_OK)
            goto done;
    }

    // If this is a serializable API, rpc_params->params is a serialized buffer.
    // otherwise this is a flat API and paramsSize is the param struct size
    if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        NV_ASSERT_OR_RETURN(!bCacheable, NV_ERR_INVALID_STATE);
    }

    if (bCacheable)
    {
        status = rmapiControlCacheGet(hClient, hObject, cmd, pParamStructPtr, paramsSize);
        if (status == NV_OK)
            goto done;
    }

    // Initialize these values now that paramsSize is known
    rpc_params_size = sizeof(*rpc_params) + paramsSize;
    total_size = fixed_param_size + paramsSize;

    // Write the header assuming one record.  If continuation records are used,
    // then the length in the header will be overwritten by _issueRpcAndWaitLarge
    NV_ASSERT_OK_OR_GOTO(status,
        rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GSP_RM_CONTROL, rpc_params_size),
        done);

    rpc_params->hClient        = hClient;
    rpc_params->hObject        = hObject;
    rpc_params->cmd            = cmd;
    rpc_params->paramsSize     = paramsSize;
    rpc_params->flags          = RMAPI_RPC_FLAGS_NONE;

    if (ctrlFlags & RMCTRL_FLAGS_COPYOUT_ON_ERROR)
        rpc_params->flags |= RMAPI_RPC_FLAGS_COPYOUT_ON_ERROR;

    if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
        rpc_params->flags |= RMAPI_RPC_FLAGS_SERIALIZED;

    // If we have a big payload control, we need to make a local copy...
    if (message_buffer_remaining < paramsSize)
    {
        large_message_copy = portMemAllocNonPaged(total_size);
        NV_ASSERT_OR_ELSE(large_message_copy != NULL, {status = NV_ERR_NO_MEMORY; goto done; });
        portMemCopy(large_message_copy, total_size, vgpu_rpc_message_header_v, fixed_param_size);
        rpc_params = &large_message_copy->rpc_message_data->gsp_rm_control_v03_00;
        message_buffer_remaining = total_size - fixed_param_size;
    }

    if (paramsSize != 0)
    {
        if (pParamStructPtr == NULL)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
        else
        {
            if (portMemCopy(rpc_params->params, message_buffer_remaining, pParamStructPtr, paramsSize) == NULL)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                goto done;
            }
        }
    }
    else if (pParamStructPtr != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad params: ptr " NvP64_fmt " size: 0x%x\n",
                                pParamStructPtr, paramsSize);
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }
    else
    {
        //
        // paramsSize = 0 and pParamStructPtr == NULL
        // rpc_params->params is static, cannot be set to NULL.
        // We will allow rpc_params->paramsSize = 0 and
        // rpc_params->params != NULL from here, but in
        // _rpcGspRmControl() have the logic that
        // pc_params->paramsSize = 0 means no params.
        //
    }

    // Issue RPC
    if (large_message_copy)
    {
        status = _issueRpcAndWaitLarge(pGpu, pRpc, total_size, large_message_copy, NV_TRUE);
    }
    else
    {
        status = _issueRpcAndWait(pGpu, pRpc);
    }

    //
    // At this point we have:
    //    status: The status of the RPC transfer. If NV_OK, we got something back
    //    rpc_params->status: Status returned by the actual ctrl handler on GSP
    //
    if (status == NV_OK)
    {
        // Skip copyout if we got an error from the GSP control handler
        if (rpc_params->status != NV_OK && !(rpc_params->flags & RMAPI_RPC_FLAGS_COPYOUT_ON_ERROR))
        {
            status = rpc_params->status;
            goto done;
        }

        if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
        {
            //
            // If it was preserialized, copy it to call context for deserialization by caller
            // Otherwise deserialize it because it was serialized here
            //
            if (bPreSerialized)
            {
                portMemCopy(pCallContext->pSerializedParams, pCallContext->serializedSize, rpc_params->params, rpc_params->paramsSize);
            }
            else
            {
                status = serverDeserializeCtrlUp(pCallContext, cmd, &pParamStructPtr, &paramsSize, &resCtrlFlags);
                if (status != NV_OK)
                    goto done;
            }
        }
        else
        {
            if (paramsSize != 0)
            {
                portMemCopy(pParamStructPtr, paramsSize, rpc_params->params, paramsSize);
            }
        }

        if (rpc_params->status != NV_OK)
            status = rpc_params->status;
        else if (bCacheable)
            NV_ASSERT_OK(rmapiControlCacheSet(hClient, hObject, cmd, rpc_params->params, paramsSize));
    }

    if (status != NV_OK)
    {
        NvBool bQuiet = NV_FALSE;
        switch (status)
        {
            case NV_ERR_NOT_SUPPORTED:
            case NV_ERR_OBJECT_NOT_FOUND:
                bQuiet = NV_TRUE;
                break;
        }
        NV_PRINTF_COND((pRpc->bQuietPrints || bQuiet), LEVEL_INFO, LEVEL_WARNING,
            "GspRmControl failed: hClient=0x%08x; hObject=0x%08x; cmd=0x%08x; paramsSize=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
            hClient, hObject, cmd, paramsSize, rpc_params->status, status);
    }

done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    // Free the local copy we might have allocated above
    portMemFree(large_message_copy);

    //
    // Free data structures if we serialized/deserialized here
    // Also check for serialized flag here as we may be called directly from within another control call
    //
    if ((resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED) && !bPreSerialized)
    {
        serverFreeSerializeStructures(pCallContext, pOriginalParams);
    }

    return status;
}

NV_STATUS rpcRmApiAlloc_GSP
(
    RM_API  *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle hObject,
    NvU32    hClass,
    void    *pAllocParams,
    NvU32    allocParamsSize
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    rpc_gsp_rm_alloc_v03_00 *rpc_params = &rpc_message->gsp_rm_alloc_v03_00;
    CALL_CONTEXT callContext = {0};
    NvU32 flags = RMAPI_ALLOC_FLAGS_NONE;
    NvU32 paramsSize;
    void *pOriginalParams = pAllocParams;
    NvBool bNullAllowed;

    const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(*rpc_params);
    const NvU32 message_buffer_remaining = pRpc->maxRpcSize - fixed_param_size;

    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmAlloc 0x%04x without adequate locks!\n", hClass);
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    NV_ASSERT_OK_OR_GOTO(status,
        rmapiGetClassAllocParamSize(&paramsSize, NV_PTR_TO_NvP64(pAllocParams), &bNullAllowed, hClass),
        done);

    // TODO CORERM-2934: Remove this when all client allocations take NV0000_ALLOC_PARAMETERS.
    // Manually set paramsSize for client as a temporary WAR for bug 3183091, so that NV0000_ALLOC_PARAMETERS
    // can be passed as pAllocParams while NvHandle is still listed in resource_list.h.
    if ((hClass == NV01_ROOT) || (hClass == NV01_ROOT_CLIENT))
    {
        paramsSize = sizeof(NV0000_ALLOC_PARAMETERS);
    }

    if (pAllocParams == NULL && !bNullAllowed)
    {
        NV_PRINTF(LEVEL_ERROR, "NULL allocation params not allowed for class 0x%x\n", hClass);
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC,
                             sizeof(rpc_gsp_rm_alloc_v03_00)),
        done);

    rpc_params->hClient    = hClient;
    rpc_params->hParent    = hParent;
    rpc_params->hObject    = hObject;
    rpc_params->hClass     = hClass;
    rpc_params->flags      = RMAPI_RPC_FLAGS_NONE;

    // Serialize RM alloc
    if (paramsSize > 0)
    {
        void *memCopyResult;

        NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO, serverSerializeAllocDown(&callContext, hClass, &pAllocParams, &paramsSize, &flags), done);
        if (flags & RMAPI_ALLOC_FLAGS_SERIALIZED)
        {
            rpc_params->flags |= RMAPI_RPC_FLAGS_SERIALIZED;
        }

        memCopyResult = portMemCopy(rpc_params->params, message_buffer_remaining, pAllocParams, paramsSize);
        rpc_params->paramsSize = paramsSize;

        if (memCopyResult == NULL)
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            goto done;
        }
    }
    else
    {
        rpc_params->paramsSize = 0;
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        // Deserialize. pAllocParams will be populated correctly if deserialized
        NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO, serverDeserializeAllocUp(&callContext, hClass, &pAllocParams, &paramsSize, &flags), done);
        if (!(flags & RMAPI_ALLOC_FLAGS_SERIALIZED) && (paramsSize > 0))
        {
            portMemCopy(pAllocParams, paramsSize, rpc_params->params, paramsSize);
        }
    }
    else
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmAlloc failed: hClient=0x%08x; hParent=0x%08x; hObject=0x%08x; hClass=0x%08x; paramsSize=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
            hClient, hParent, hObject, hClass, paramsSize, rpc_params->status, status);
        status = rpc_params->status;
    }

done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    serverFreeSerializeStructures(&callContext, pOriginalParams);

    return status;
}

NV_STATUS rpcRmApiDupObject_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle *phObject,
    NvHandle hClientSrc,
    NvHandle hObjectSrc,
    NvU32 flags
)
{
    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    NVOS55_PARAMETERS_v03_00 *rpc_params = &rpc_message->dup_object_v03_00.params;
    NV_STATUS status;
    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmDupObject without adequate locks!\n");
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DUP_OBJECT, sizeof(rpc_dup_object_v03_00));
    if (status != NV_OK)
        goto done;

    rpc_params->hClient     = hClient;
    rpc_params->hParent     = hParent;
    rpc_params->hObject     = *phObject;
    rpc_params->hClientSrc  = hClientSrc;
    rpc_params->hObjectSrc  = hObjectSrc;
    rpc_params->flags       = flags;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmDupObject failed: hClient=0x%08x; hParent=0x%08x; hObject=0x%08x; hClientSrc=0x%08x; hObjectSrc=0x%08x; flags=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
             hClient, hParent, *phObject, hClientSrc, hObjectSrc, flags, rpc_params->status, status);
    }
done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    return status;
}

NV_STATUS rpcRmApiFree_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject
)
{
    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    NVOS00_PARAMETERS_v03_00 *rpc_params = &rpc_message->free_v03_00.params;
    NV_STATUS status = NV_OK;
    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmFree without adequate locks!\n");
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_FREE, sizeof(rpc_free_v03_00));
    if (status != NV_OK)
        goto done;

    rpc_params->hRoot = hClient;
    rpc_params->hObjectParent = NV01_NULL_OBJECT;
    rpc_params->hObjectOld = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmFree failed: hClient=0x%08x; hObject=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
             hClient, hObject, rpc_params->status, status);
    }
done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    return status;
}

/*
 * Sends ack from CPU-RM to GSP-RM that ECC error
 * notifier write has completed.
 */
NV_STATUS rpcEccNotifierWriteAck_v23_05
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ECC_NOTIFIER_WRITE_ACK, 0);
        if (status != NV_OK)
            return status;

        status = _issueRpcAsync(pGpu, pRpc);
    }

    return status;
}
