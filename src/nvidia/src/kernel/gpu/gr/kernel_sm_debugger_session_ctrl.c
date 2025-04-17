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

/***************************************************************************\
*                                                                          *
*   Description:                                                           *
*       This module contains Nv04Control support for Debugger object       *
*       represented by GT200_DEBUGGER class instantiations.                *
*                                                                          *
*                                                                          *
\***************************************************************************/

#define NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/rmapi/control.h"
#include "kernel/rmapi/rmapi.h"
#include "kernel/rmapi/mapping_list.h"
#include "kernel/os/os.h"
#include "kernel/core/locks.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "kernel/rmapi/rs_utils.h"
#include "gpu/mmu/mmu_trace.h"
#include "kernel/gpu/gr/kernel_sm_debugger_session.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator_common.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"
#include "kernel/gpu/gr/kernel_graphics_object.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "class/cl83de.h"
#include "class/clc637.h"
#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR
#include "ctrl/ctrl83de.h"

//
// _nv83deCtrlCmdFetchVAS
//
// Helper to fetch the the OBJVASPACE *from the hChannel without needing
// for the caller to explicitly pass in the handle corresponding to the VaSpaceApi:
//
static NV_STATUS
_nv83deCtrlCmdFetchVAS(RsClient *pClient, NvU32 hChannel, OBJVASPACE **ppVASpace)
{
    KernelChannel *pKernelChannel = NULL;

    NV_ASSERT_OR_RETURN(ppVASpace != NULL, NV_ERR_INVALID_ARGUMENT);

    // Fetch the corresponding Channel object from our handle
    NV_ASSERT_OK_OR_RETURN(CliGetKernelChannel(pClient, hChannel, &pKernelChannel));
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    *ppVASpace = pKernelChannel->pVAS;

    return NV_OK;
}

//
// _nv83deCtrlCmdValidateRange
//
// Helper to traverse through the virtual memory page hierarchy and
// determine whether or not the virtual address (VA) range provided as
// input has valid and allocated pages mapped to it in its entirety.
//
// This command's input is NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS which
// contains a buffer of NV83DE_CTRL_DEBUG_ACCESS_OPs
//
// Possible return values:
//     NV_OK
//     NV_ERR_INVALID_ARGUMENT
//     NV_ERR_INVALID_XLATE
//
static NV_STATUS
_nv83deCtrlCmdValidateRange
(
    OBJGPU *pGpu,
    OBJVASPACE *pVASpace,
    NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *pParams
)
{
    NvU32 i;
    NvU64 totalLength;
    NV_STATUS status = NV_OK;

    // Loop through to validate range for all provided ops
    for (i = 0; i < pParams->count; i++)
    {
        MMU_TRACE_PARAM mmuParams;
        MMU_TRACE_ARG traceArg = {0};

        // Ensure that input gpuVA is 4-byte aligned. cpuVA is handled directly by portmemCopy.
        NV_ASSERT_OR_RETURN((pParams->opsBuffer[i].gpuVA & 3) == 0, NV_ERR_INVALID_ARGUMENT);

        // Sanity-check the requested size
        if (pParams->opsBuffer[i].size == 0 || !portSafeAddU64(pParams->opsBuffer[i].gpuVA, pParams->opsBuffer[i].size, &totalLength))
            return NV_ERR_INVALID_ARGUMENT;

        mmuParams.mode    = MMU_TRACE_MODE_VALIDATE;
        mmuParams.va      = pParams->opsBuffer[i].gpuVA;
        mmuParams.vaLimit = pParams->opsBuffer[i].gpuVA + pParams->opsBuffer[i].size - 1;
        mmuParams.pArg    = &traceArg;

        NV_ASSERT_OK_OR_RETURN(mmuTrace(pGpu, pVASpace, &mmuParams));

        //
        // mmuTrace may return NV_OK if the range is invalid but the translation did
        // not otherwise cause errors. Use traceArg.valid to satisfy the output
        // status needed for _nv83deCtrlCmdValidateRange.
        //
        if (traceArg.valid)
        {
            pParams->opsBuffer[i].valid = 1;
        }
        else
        {
            status = NV_ERR_INVALID_XLATE;
            pParams->opsBuffer[i].valid = 0;
        }
    }

    return status;
}

static NV_STATUS
_nv8deCtrlCmdReadWriteSurfaceLegacy
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *pParams,
    NvBool bWrite
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    RsClient *pClient = RES_GET_CLIENT(pKernelSMDebuggerSession);
    OBJVASPACE *pVASpace = NULL;
    NvU32 count = pParams->count;
    NvU32 i;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (count > MAX_ACCESS_OPS)
        return NV_ERR_INVALID_ARGUMENT;

    // Attempt to retrieve the VAS pointer
    NV_ASSERT_OK_OR_RETURN(
        _nv83deCtrlCmdFetchVAS(pClient, pKernelSMDebuggerSession->hChannel, &pVASpace));

    // Validate VA range and fail if invalid
    NV_ASSERT_OK_OR_RETURN(
        _nv83deCtrlCmdValidateRange(pGpu, pVASpace, pParams));

    for (i = 0; i < count; i++)
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        NvU64 virtAddr = pParams->opsBuffer[i].gpuVA;
        NvP64 bufPtr = pParams->opsBuffer[i].pCpuVA;
        NvU32 bufSize = pParams->opsBuffer[i].size;
        NvU32 pageStartOffset;
        NvU32 start4kPage;
        NvU32 end4kPage;
        NvU32 curSize;
        NvU32 cur4kPage;
        status = NV_OK;

        // Break it up by 4K pages for now
        pageStartOffset = NvOffset_LO32(virtAddr) & RM_PAGE_MASK;
        start4kPage = (NvOffset_LO32(virtAddr) >> 12) & 0x1FFFF;
        end4kPage = (NvOffset_LO32(virtAddr + bufSize - 1) >> 12) & 0x1FFFF;

        curSize = RM_PAGE_SIZE - pageStartOffset;
        virtAddr -= pageStartOffset;

        for (cur4kPage = start4kPage; cur4kPage <= end4kPage; ++cur4kPage)
        {
            MMU_TRACE_PARAM mmuParams     = {0};
            MMU_TRACE_ARG traceArg        = {0};
            TRANSFER_SURFACE surf         = {0};
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            NvU8 *pKernBuffer = NULL;

            mmuParams.mode    = MMU_TRACE_MODE_TRANSLATE;
            mmuParams.va      = virtAddr;
            mmuParams.vaLimit = virtAddr;
            mmuParams.pArg    = &traceArg;

            NV_ASSERT_OK_OR_RETURN(
                mmuTrace(pGpu, pVASpace, &mmuParams));

            if (curSize > bufSize)
            {
                curSize = bufSize;
            }

            pKernBuffer = portMemAllocNonPaged(curSize);
            if (pKernBuffer == NULL)
            {
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }

            if (traceArg.aperture == ADDR_SYSMEM)
            {
                NvP64 physAddr = NV_PTR_TO_NvP64(traceArg.pa);
                NvU64 limit = RM_PAGE_SIZE - 1;

                NvU32 os02Flags = DRF_DEF(OS02, _FLAGS, _LOCATION,      _PCI)           |
                                  DRF_DEF(OS02, _FLAGS, _MAPPING,       _NO_MAP)        |
                                  DRF_DEF(OS02, _FLAGS, _PHYSICALITY,   _CONTIGUOUS)    |
                                  DRF_DEF(OS02, _FLAGS, _COHERENCY,     _CACHED);

                NV_ASSERT_OK_OR_ELSE(status,
                    osCreateMemFromOsDescriptor(pGpu,
                                                physAddr,
                                                pKernelSMDebuggerSession->hInternalClient,
                                                os02Flags,
                                                &limit,
                                                &pMemDesc,
                                                NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR,
                                                RS_PRIV_LEVEL_KERNEL),
                    portMemFree(pKernBuffer); return status; );
            }
            else if (traceArg.aperture == ADDR_FBMEM)
            {
                NV_ASSERT_OK_OR_ELSE(status,
                    memdescCreate(&pMemDesc, pGpu, RM_PAGE_SIZE, 0, NV_TRUE,
                                  traceArg.aperture, NV_MEMORY_UNCACHED,
                                  MEMDESC_FLAGS_NONE),
                    portMemFree(pKernBuffer); return status; );
                memdescDescribe(pMemDesc, traceArg.aperture, traceArg.pa, RM_PAGE_SIZE);
            }

            surf.pMemDesc = pMemDesc;
            surf.offset = pageStartOffset;

            if (bWrite)
            {
                NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                    portMemExCopyFromUser(bufPtr, pKernBuffer, curSize));

                // Write out the buffer to memory
                if (status == NV_OK)
                {
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                        memmgrMemWrite(pMemoryManager, &surf, pKernBuffer, curSize,
                                       TRANSFER_FLAGS_DEFER_FLUSH));
                }
            }
            else
            {
                // Read from memory
                NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                    memmgrMemRead(pMemoryManager, &surf, pKernBuffer, curSize,
                                  TRANSFER_FLAGS_DEFER_FLUSH));

                if (status == NV_OK)
                {
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                        portMemExCopyToUser(pKernBuffer, bufPtr, curSize));
                }
            }

            portMemFree(pKernBuffer);
            memdescDestroy(pMemDesc);

            if (status != NV_OK)
                return status;

            pageStartOffset = 0;
            bufPtr = NvP64_PLUS_OFFSET(bufPtr,curSize);
            bufSize -= curSize;
            curSize = RM_PAGE_SIZE;
            virtAddr += RM_PAGE_SIZE;
        }
    }

    return status;
}

static NV_STATUS
_nv8deCtrlCmdReadWriteSurface
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *pParams,
    NvBool bWrite
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    RsClient *pClient = RES_GET_CLIENT(pKernelSMDebuggerSession);
    NvU32 count = pParams->count;
    NvU32 i;
    NV_STATUS status = NV_OK;
    KernelChannel *pKernelChannel;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || IS_VIRTUAL_WITH_HEAVY_SRIOV(pGpu)
        || (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)) && gpuIsCCDevToolsModeEnabled(pGpu))
        )
    {
        return _nv8deCtrlCmdReadWriteSurfaceLegacy(pKernelSMDebuggerSession, pParams, bWrite);
    }

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (count > MAX_ACCESS_OPS)
        return NV_ERR_INVALID_ARGUMENT;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        CliGetKernelChannel(pClient, pKernelSMDebuggerSession->hChannel, &pKernelChannel));

    for (i = 0; i < count; i++)
    {
        CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
        NvU64 virtAddr = pParams->opsBuffer[i].gpuVA;
        NvP64 bufPtr = pParams->opsBuffer[i].pCpuVA;
        NvU64 bufSize = pParams->opsBuffer[i].size;

        NV_CHECK_OR_RETURN(LEVEL_ERROR, bufSize != 0, NV_ERR_INVALID_ARGUMENT);

        while (bufSize != 0)
        {
            NvU64 curSize = bufSize;

            // The memory has to be mapped in a single in a single call by the same client
            NV_CHECK_OR_RETURN(LEVEL_ERROR,
                CliGetDmaMappingInfo(pClient, RES_GET_PARENT_HANDLE(pKernelSMDebuggerSession),  pKernelChannel->hVASpace,
                                     virtAddr, gpumgrGetDeviceGpuMask(pGpu->deviceInstance), &pDmaMappingInfo),
                NV_ERR_INVALID_ARGUMENT);

            NvU64 offsetInMapping = virtAddr - pDmaMappingInfo->DmaOffset;
            curSize = NV_MIN(curSize, pDmaMappingInfo->pMemDesc->Size - offsetInMapping);

            TRANSFER_SURFACE surf = { .pMemDesc = pDmaMappingInfo->pMemDesc, .offset = offsetInMapping };
            NvU8 *pKernBuffer = portMemAllocNonPaged(curSize);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernBuffer != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

            NvU32 transferFlags = TRANSFER_FLAGS_NONE;
            if (!memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_SWIZZLED, memdescGetPteKind(pDmaMappingInfo->pMemDesc)) &&
                !IS_SIMULATION(pGpu))
            {
                //
                // CeUtils uses a compressed GMK mapping of the entire FB
                // It won't respect PTE kind swizzling
                // Don't set the flag on simulation, as it forces BAR0 path
                //
                transferFlags |= TRANSFER_FLAGS_PREFER_CE;
            }

            if (bWrite)
            {
                NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                    portMemExCopyFromUser(bufPtr, pKernBuffer, curSize));

                // Write out the buffer to memory
                if (status == NV_OK)
                {
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                        memmgrMemWrite(pMemoryManager, &surf, pKernBuffer, curSize, transferFlags));
                }
            }
            else
            {
                // Read from memory
                NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                    memmgrMemRead(pMemoryManager, &surf, pKernBuffer, curSize, transferFlags));

                if (status == NV_OK)
                {
                    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                        portMemExCopyToUser(pKernBuffer, bufPtr, curSize));
                }
            }

            portMemFree(pKernBuffer);

            if (status != NV_OK)
                return status;

            bufPtr = NvP64_PLUS_OFFSET(bufPtr, curSize);
            bufSize -= curSize;
            virtAddr += curSize;
        }
    }

    return NV_OK;
}

NV_STATUS
ksmdbgssnCtrlCmdReadSurface_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *pParams
)
{
    return _nv8deCtrlCmdReadWriteSurface(pKernelSMDebuggerSession, pParams, NV_FALSE);
}

NV_STATUS
ksmdbgssnCtrlCmdWriteSurface_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *pParams
)
{
    return _nv8deCtrlCmdReadWriteSurface(pKernelSMDebuggerSession, pParams, NV_TRUE);
}

NV_STATUS
ksmdbgssnCtrlCmdGetMappings_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    RsClient *pClient = RES_GET_CLIENT(pKernelSMDebuggerSession);
    OBJVASPACE *pVASpace = NULL;
    MMU_TRACE_ARG traceArg = {0};
    MMU_TRACE_PARAM mmuParams = {0};

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Attempt to retrieve the VAS pointer
    NV_ASSERT_OK_OR_RETURN(
        _nv83deCtrlCmdFetchVAS(pClient, pKernelSMDebuggerSession->hChannel, &pVASpace));

    traceArg.pMapParams = pParams;

    mmuParams.mode    = MMU_TRACE_MODE_DUMP_RANGE;
    mmuParams.va      = pParams->vaLo;
    mmuParams.vaLimit = pParams->vaHi;
    mmuParams.pArg    = &traceArg;

    return mmuTrace(pGpu, pVASpace, &mmuParams);
}

typedef enum {
    GRDBG_MEM_ACCESS_TYPE_INVALID,
    GRDBG_MEM_ACCESS_TYPE_READ,
    GRDBG_MEM_ACCESS_TYPE_WRITE,
} GrdbgMemoryAccessType;

static NV_STATUS
_nv83deFlushAllGpusL2Cache(MEMORY_DESCRIPTOR *pMemDesc)
{
    NvU32 gpuCount;
    NvU32 gpuMask;
    NvU32 gpuInstance = 0;
    OBJGPU *pTempGpu;
    NV_STATUS rmStatus = NV_OK;

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);

    while ((pTempGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        //
        // On GPUs with write-back caches, FB_CACHE_INVALIDATE is reduced
        // to FB_CACHE_EVICT, which first writes back dirty lines and then
        // invalidates clean ones, exactly what we want. On write-through
        // caches, it will invalidate clean lines as expected.
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kmemsysCacheOp_HAL(pTempGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pTempGpu), pMemDesc,
                               FB_CACHE_SYSTEM_MEMORY, FB_CACHE_INVALIDATE));
    }

    return rmStatus;
}

static NV_STATUS
_nv83deUnmapMemoryFromGrdbgClient
(
    OBJGPU *pTargetGpu,
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NvP64 pCpuVirtAddr,
    NvU32 flags
)
{
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Unmap memory.
    status = pRmApi->UnmapFromCpu(pRmApi,
                                  pKernelSMDebuggerSession->hInternalClient,
                                  pKernelSMDebuggerSession->hInternalDevice,
                                  pKernelSMDebuggerSession->hInternalMemMapping,
                                  NvP64_VALUE(pCpuVirtAddr),
                                  flags,
                                  osGetCurrentProcess());

    // Free the memory handle
    pRmApi->Free(pRmApi,
                 pKernelSMDebuggerSession->hInternalClient,
                 pKernelSMDebuggerSession->hInternalMemMapping);
    pKernelSMDebuggerSession->hInternalMemMapping = NV01_NULL_OBJECT;

    return status;
}

static NV_STATUS
_nv83deMapMemoryIntoGrdbgClient
(
    OBJGPU *pTargetGpu,
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NvHandle hClient,
    NvHandle hMemory,
    NvU64 offset,
    NvU32 length,
    NvP64 *ppCpuVirtAddr,
    NvU32 flags
)
{
    NV_STATUS rmStatus = NV_OK;
    void *pCpuVirtAddr = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    //
    // We use our own internal client, dup the memory object into our client
    // and then call RmMapMemory on it, so that the memory is never mapped
    // into the client's address space.
    // hInternalMemMappping must be freed after use, since we don't have space to
    // store multiple memory handles.
    //
    NV_ASSERT_OR_RETURN(pKernelSMDebuggerSession->hInternalMemMapping == NV01_NULL_OBJECT,
                        NV_ERR_INVALID_STATE);

    rmStatus = pRmApi->DupObject(pRmApi,
                                 pKernelSMDebuggerSession->hInternalClient,
                                 pKernelSMDebuggerSession->hInternalDevice,
                                 &pKernelSMDebuggerSession->hInternalMemMapping,
                                 hClient,
                                 hMemory,
                                 0);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Unable to dup source memory (0x%x,0x%x) under device (status = 0x%x). Attempting subdevice dup.\n",
                  hClient, hMemory, rmStatus);

        rmStatus = pRmApi->DupObject(pRmApi,
                                     pKernelSMDebuggerSession->hInternalClient,
                                     pKernelSMDebuggerSession->hInternalSubdevice,
                                     &pKernelSMDebuggerSession->hInternalMemMapping,
                                     hClient,
                                     hMemory,
                                     0);

        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to dup source memory (0x%x,0x%x) under subdevice (status = 0x%x). Aborting.\n",
                      hClient, hMemory, rmStatus);
            return rmStatus;
        }
    }

    // Map memory
    rmStatus = pRmApi->MapToCpu(pRmApi,
                                pKernelSMDebuggerSession->hInternalClient,
                                pKernelSMDebuggerSession->hInternalSubdevice,
                                pKernelSMDebuggerSession->hInternalMemMapping,
                                offset,
                                length,
                                &pCpuVirtAddr,
                                flags);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_WARNING, "RmMapMemory failed 0x%x\n", rmStatus);

        // Free the memory handle
        pRmApi->Free(pRmApi,
                     pKernelSMDebuggerSession->hInternalClient,
                     pKernelSMDebuggerSession->hInternalMemMapping);
        pKernelSMDebuggerSession->hInternalMemMapping = NV01_NULL_OBJECT;

        return rmStatus;
    }

    *ppCpuVirtAddr = NV_PTR_TO_NvP64(pCpuVirtAddr);

    return rmStatus;
}

static NV_STATUS
_nv83deCtrlCmdDebugAccessMemory
(
    OBJGPU *pTargetGpu,
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NvHandle hClient,
    NvHandle hMemory,
    NvU64 offset,
    NvU32 length,
    NvP64 buffer,
    GrdbgMemoryAccessType accessType
)
{
    RsResourceRef *pResourceRef;
    Memory *pMemory;
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 totalLength;
    NV_STATUS rmStatus = NV_OK;
    NV_STATUS rmUnmapStatus = NV_OK;
    NvU32 flags = 0;
    NvBool bGpuCached;
    NvBool bCpuMemory;

    //
    // SECURITY: Find the hMemory object in the RmCtrl caller's database.
    // This ensures the RmCtrl caller has rights to access hMemory.
    //
    if (serverutilGetResourceRef(hClient, hMemory, &pResourceRef) != NV_OK)
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    pMemory = dynamicCast(pResourceRef->pResource, Memory);
    if (pMemory == NULL)
    {
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        NV_PRINTF(LEVEL_WARNING,
                  "Invalid handle: hMemory %x is not of type classId(Memory): (GPU 0x%llx, hClient 0x%x, hMemory %x, offset 0x%llx, length 0x%x, flags 0x%x)\n",
                  hMemory,
                  pTargetGpu->busInfo.gpuPhysAddr,
                  hClient,
                  hMemory,
                  offset,
                  length,
                  flags);
        return rmStatus;
    }

    // Get a memdesc for this object to determine its attributes
    pMemDesc = pMemory->pMemDesc;
    if (pMemDesc == NULL)
        return NV_ERR_INVALID_STATE;

    // Sanity-check the requested size
    if ((length == 0) || (!portSafeAddU64(offset, length, &totalLength)))
        return NV_ERR_INVALID_ARGUMENT;
    if (totalLength > pMemDesc->Size)
        return NV_ERR_INVALID_ARGUMENT;

    bCpuMemory = (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM);
    bGpuCached = (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED);

    // Ensure previous in-flight accesses are complete
    osFlushCpuWriteCombineBuffer();

    if (bCpuMemory && bGpuCached)
    {
        //
        // Flush and invalidate SYSMEM lines from L2s of all GPUs.
        // Some GPUs have write-back caches, so this must be done both for
        // accessType == READ and accessType == WRITE.
        //
        NV_ASSERT_OK_OR_RETURN(_nv83deFlushAllGpusL2Cache(pMemDesc));
    }

    if (pMemory->categoryClassId == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR)
    {
        // WAR: cudaHostRegister debugger test fails, temporary keep legacy behavior
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_VIRTUAL_WITHOUT_SRIOV(pTargetGpu) || IS_VIRTUAL_WITH_HEAVY_SRIOV(pTargetGpu)
        || (!bCpuMemory && kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pTargetGpu)) &&
        gpuIsCCDevToolsModeEnabled(pTargetGpu))
    )
    {
        NvP64 pCpuVirtAddr = NvP64_NULL;

        // Setup mapping flags based on the kind of memory, access type etc.
        if (accessType == GRDBG_MEM_ACCESS_TYPE_READ)
        {
            flags = FLD_SET_DRF(OS33, _FLAGS, _ACCESS, _READ_ONLY, flags);
        }
        else
        {
            flags = FLD_SET_DRF(OS33, _FLAGS, _ACCESS, _WRITE_ONLY, flags);
        }

        if (!bCpuMemory &&
            kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pTargetGpu)) &&
            gpuIsCCDevToolsModeEnabled(pTargetGpu))
        {
            // Allow the mapping to happen successfully on HCC devtools mode
            flags = FLD_SET_DRF(OS33, _FLAGS, _ALLOW_MAPPING_ON_HCC, _YES, flags);
        }

        // Map memory into the internal smdbg client
        rmStatus = _nv83deMapMemoryIntoGrdbgClient(pTargetGpu,
                                                   pKernelSMDebuggerSession,
                                                   hClient,
                                                   hMemory,
                                                   offset,
                                                   length,
                                                   &pCpuVirtAddr,
                                                   flags);
        if (NV_OK != rmStatus)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Failed to map memory into internal smdbg client (GPU 0x%llx, hClient 0x%x, hMemory %x, offset 0x%llx, length 0x%x, flags 0x%x): (rmStatus = %x)\n",
                      pTargetGpu->busInfo.gpuPhysAddr,
                      hClient,
                      hMemory,
                      offset,
                      length,
                      flags,
                      rmStatus);
            return rmStatus;
        }

        // Perform the requested accessType operation
        if (accessType == GRDBG_MEM_ACCESS_TYPE_READ)
        {
            if (!portMemCopy(NvP64_VALUE(buffer), length, NvP64_VALUE(pCpuVirtAddr), length))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_WARNING,
                          "portMemCopy failed (from VA 0x" NvP64_fmt " to 0x" NvP64_fmt ", length 0x%x)\n",
                          pCpuVirtAddr, buffer, length);
                goto cleanup_mapping;
            }
            NV_PRINTF(LEVEL_INFO, "Reading %d bytes of memory from 0x%x\n",
                      length, hMemory);
        }
        else
        {
            if (!portMemCopy(NvP64_VALUE(pCpuVirtAddr), length, NvP64_VALUE(buffer), length))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_WARNING,
                          "portMemCopy failed (from VA 0x" NvP64_fmt " to 0x" NvP64_fmt ", length 0x%x)\n",
                          buffer, pCpuVirtAddr, length);
                goto cleanup_mapping;
            }

            NV_PRINTF(LEVEL_INFO, "Writing %d bytes of memory to 0x%x\n", length,
                      hMemory);
        }

cleanup_mapping:
        // Unmap memory.
        rmUnmapStatus = _nv83deUnmapMemoryFromGrdbgClient(pTargetGpu,
                                                          pKernelSMDebuggerSession,
                                                          pCpuVirtAddr,
                                                          flags);
    }
    else
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pTargetGpu);
        TRANSFER_SURFACE surf = { .pMemDesc = pMemDesc, .offset = offset };
        // Prefer CE, but use BAR1 if not available; disable for maxwell due to undebugged issues in specific tests
        NvU32 transferFlags = (IsMAXWELL(pTargetGpu) ? 0 : TRANSFER_FLAGS_PREFER_CE) |
                              TRANSFER_FLAGS_USE_BAR1;

        if (accessType == GRDBG_MEM_ACCESS_TYPE_READ)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                memmgrMemRead(pMemoryManager, &surf, NvP64_VALUE(buffer), length, transferFlags));
        }
        else
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                memmgrMemWrite(pMemoryManager, &surf, NvP64_VALUE(buffer), length, transferFlags));
        }
    }

    // Return the first failure
    return (rmStatus != NV_OK ? rmStatus: rmUnmapStatus);
}

NV_STATUS
ksmdbgssnCtrlCmdDebugReadMemory_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *pParams
)
{
    return _nv83deCtrlCmdDebugAccessMemory(GPU_RES_GET_GPU(pKernelSMDebuggerSession),
                                           pKernelSMDebuggerSession,
                                           RES_GET_CLIENT_HANDLE(pKernelSMDebuggerSession),
                                           pParams->hMemory,
                                           pParams->offset,
                                           pParams->length,
                                           pParams->buffer,
                                           GRDBG_MEM_ACCESS_TYPE_READ);
}

NV_STATUS
ksmdbgssnCtrlCmdDebugWriteMemory_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *pParams
)
{
    return _nv83deCtrlCmdDebugAccessMemory(GPU_RES_GET_GPU(pKernelSMDebuggerSession),
                                           pKernelSMDebuggerSession,
                                           RES_GET_CLIENT_HANDLE(pKernelSMDebuggerSession),
                                           pParams->hMemory,
                                           pParams->offset,
                                           pParams->length,
                                           pParams->buffer,
                                           GRDBG_MEM_ACCESS_TYPE_WRITE);
}

NV_STATUS
ksmdbgssnCtrlCmdDebugGetHandles_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_GET_HANDLES_PARAMS *pParams
)
{
    pParams->hChannel = pKernelSMDebuggerSession->hChannel;
    pParams->hSubdevice = pKernelSMDebuggerSession->hSubdevice;

    return NV_OK;
}

NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    NvBool isClientGspPlugin = NV_FALSE;

    NV_CHECK_OR_RETURN(LEVEL_WARNING,
        pParams->regOpCount <= NV83DE_CTRL_GPU_EXEC_REG_OPS_MAX_OPS,
        NV_ERR_INVALID_ARGUMENT);

    // Check if User have permission to access register offset
    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        gpuValidateRegOps(pGpu, pParams->regOps, pParams->regOpCount,
                          pParams->bNonTransactional, isClientGspPlugin, NV_FALSE));

    if (IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        RM_API       *pRmApi        = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pRmCtrlParams->hClient,
                               pRmCtrlParams->hObject,
                               pRmCtrlParams->cmd,
                               pRmCtrlParams->pParams,
                               pRmCtrlParams->paramsSize);
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    NV_STATUS status = NV_OK;
    NvU32 i;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, pParams->count <= MAX_ACCESS_MEMORY_OPS,
                       NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < pParams->count; ++i)
    {
        NV_STATUS localStatus = NV_OK;
        NvP64 pData = (NvP64)(((NvU8 *)pParams->pData) + pParams->entries[i].dataOffset);
        NvU32 endingOffset;

        NV_CHECK_OR_ELSE(LEVEL_WARNING,
            portSafeAddU32(pParams->entries[i].dataOffset, pParams->entries[i].length, &endingOffset) &&
            (endingOffset <= pParams->dataLength),
            localStatus = NV_ERR_INVALID_OFFSET;
            goto updateStatus; );

        NV_CHECK_OK_OR_GOTO(localStatus, LEVEL_ERROR,
            _nv83deCtrlCmdDebugAccessMemory(pGpu,
                                            pKernelSMDebuggerSession,
                                            RES_GET_CLIENT_HANDLE(pKernelSMDebuggerSession),
                                            pParams->entries[i].hMemory,
                                            pParams->entries[i].memOffset,
                                            pParams->entries[i].length,
                                            pData,
                                            GRDBG_MEM_ACCESS_TYPE_READ),
            updateStatus);

updateStatus:
        pParams->entries[i].status = localStatus;
        if ((status == NV_OK) && (localStatus != NV_OK))
            status = localStatus;
    }

    return status;
}

NV_STATUS
ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);
    NV_STATUS status = NV_OK;
    NvU32 i;

    NV_CHECK_OR_RETURN(LEVEL_WARNING, pParams->count <= MAX_ACCESS_MEMORY_OPS,
                       NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < pParams->count; ++i)
    {
        NV_STATUS localStatus = NV_OK;
        NvP64 pData = (NvP64)(((NvU8 *)pParams->pData) + pParams->entries[i].dataOffset);
        NvU32 endingOffset;

        NV_CHECK_OR_ELSE(LEVEL_WARNING,
            portSafeAddU32(pParams->entries[i].dataOffset, pParams->entries[i].length, &endingOffset) &&
            (endingOffset <= pParams->dataLength),
            localStatus = NV_ERR_INVALID_OFFSET;
            goto updateStatus; );

        NV_CHECK_OK_OR_GOTO(localStatus, LEVEL_ERROR,
            _nv83deCtrlCmdDebugAccessMemory(pGpu,
                                            pKernelSMDebuggerSession,
                                            RES_GET_CLIENT_HANDLE(pKernelSMDebuggerSession),
                                            pParams->entries[i].hMemory,
                                            pParams->entries[i].memOffset,
                                            pParams->entries[i].length,
                                            pData,
                                            GRDBG_MEM_ACCESS_TYPE_WRITE),
            updateStatus);

updateStatus:
        pParams->entries[i].status = localStatus;
        if ((status == NV_OK) && (localStatus != NV_OK))
            status = localStatus;
    }

    return status;
}

NV_STATUS
ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_CONTROL(pRmCtrlParams->pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);

        //
        // SR-IOV vGPU
        //
        // MMU fault info is to be managed from within the guest, since host is
        // not aware of MMU fault info about the VF.
        // SM exception info is still fetched from host via the RPC above.
        //
        if (IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            NV_ASSERT_OK(
                kgrctxLookupMmuFault(pGpu,
                                     kgrobjGetKernelGraphicsContext(pGpu, pKernelSMDebuggerSession->pObject),
                                     &pParams->mmuFault));
        }

        return rmStatus;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams
)
{
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;
        NV_RM_RPC_CONTROL(pRmCtrlParams->pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          rmStatus);

        //
        // SR-IOV vGPU
        //
        // MMU fault info is to be managed from within the guest, since host is
        // not aware of MMU fault info about the VF.
        // SM exception info is still fetched from host via the RPC above.
        //
        if (IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            NV_ASSERT_OK(
                kgrctxClearMmuFault(pGpu, kgrobjGetKernelGraphicsContext(pGpu, pKernelSMDebuggerSession->pObject)));
        }

        return rmStatus;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_IMPL
(
    KernelSMDebuggerSession *pKernelSMDebuggerSession,
    NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelSMDebuggerSession);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        return pRmApi->Control(pRmApi,
                                pRmCtrlParams->hClient,
                                pRmCtrlParams->hObject,
                                pRmCtrlParams->cmd,
                                pRmCtrlParams->pParams,
                                pRmCtrlParams->paramsSize);
    }

    NV_ASSERT_OK_OR_RETURN(
        kgrctxLookupMmuFaultInfo(pGpu,
                                 kgrobjGetKernelGraphicsContext(pGpu, pKernelSMDebuggerSession->pObject),
                                 pParams));

    return NV_OK;
}

