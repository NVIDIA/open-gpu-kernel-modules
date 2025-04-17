/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_GRAPHICS_H_PRIVATE_ACCESS_ALLOWED

#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/gr/kernel_graphics_manager.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/rmapi/rmapi_utils.h"
#include "kernel/core/locks.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mmu/kern_gmmu.h"
#include "platform/sli/sli.h"
#include "rmapi/rs_utils.h"
#include "rmapi/client.h"
#include "nvrm_registry.h"
#include "gpu/mem_mgr/heap.h"

#include "ctrl/ctrl0080/ctrl0080fifo.h"

#include "class/cla06f.h"
#include "class/cl90f1.h" // FERMI_VASPACE_A
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/clc36f.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h" // TURING_CHANNEL_GPFIFO_A

/*!
 * @brief Allocate common local/global buffers that are required by the graphics context
 *
 * @param[in] pGpu
 * @param[in] pKernelGraphics
 * @param[in] gfid                   host or guest gfid
 * @param[in] pKernelGraphicsContext context pointer - if valid allocate local
 */
NV_STATUS
kgraphicsAllocGrGlobalCtxBuffers_TU102
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 gfid,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    extern NV_STATUS kgraphicsAllocGrGlobalCtxBuffers_GP100(OBJGPU *pGpu, KernelGraphics *pKernelGraphics, NvU32 gfid, KernelGraphicsContext *pKernelGraphicsContext);
    MEMORY_DESCRIPTOR           **ppMemDesc;
    GR_GLOBALCTX_BUFFERS         *pCtxBuffers;
    GR_BUFFER_ATTR               *pCtxAttr;
    NvU64                         allocFlags = MEMDESC_FLAGS_NONE;
    NvBool                        bIsFbBroken = NV_FALSE;
    NvU32                         rtvcbBufferSize;
    NvU32                         rtvcbBufferAlign;
    NV_STATUS                     status;
    const KGRAPHICS_STATIC_INFO  *pKernelGraphicsStaticInfo;
    CTX_BUF_POOL_INFO            *pCtxBufPool = NULL;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
       (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM)))
    {
        bIsFbBroken = NV_TRUE;
    }

    // Setup the Circular Buffer DB
    allocFlags = MEMDESC_FLAGS_LOST_ON_SUSPEND;

    if (kgraphicsShouldSetContextBuffersGPUPrivileged(pGpu, pKernelGraphics))
    {
        allocFlags |= MEMDESC_FLAGS_GPU_PRIVILEGED;
    }

    if (pKernelGraphicsContext != NULL)
    {
        KernelGraphicsContextUnicast *pKernelGraphicsContextUnicast;
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgrctxGetUnicast(pGpu, pKernelGraphicsContext, &pKernelGraphicsContextUnicast));

        pCtxBuffers = &pKernelGraphicsContextUnicast->localCtxBuffer;
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.localCtxAttr;

        //
        // if we already have local buffers allocated, return as we may get
        // called multiple times per-channel
        //
        if (pCtxBuffers->bAllocated)
            return NV_OK;

        // check for allocating local buffers in VPR memory (don't want for global memory)
        if (
            pKernelGraphicsContextUnicast->bVprChannel)
            allocFlags |= MEMDESC_ALLOC_FLAGS_PROTECTED;

        // If allocated per channel, ensure allocations goes into Suballocator if available
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }
    else
    {
        pCtxBuffers = &pKernelGraphics->globalCtxBuffersInfo.pGlobalCtxBuffers[gfid];
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.globalCtxAttr;
        NV_ASSERT_OK_OR_RETURN(ctxBufPoolGetGlobalPool(pGpu, CTX_BUF_ID_GR_GLOBAL,
            RM_ENGINE_TYPE_GR(pKernelGraphics->instance), &pCtxBufPool));
    }

    // Don't use context buffer pool for VF allocations managed by host RM.
    if (ctxBufPoolIsSupported(pGpu) && (pCtxBufPool != NULL))
    {
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL;
    }

    if (IS_GFID_VF(gfid))
    {
        pCtxAttr = pKernelGraphics->globalCtxBuffersInfo.vfGlobalCtxAttr;
        allocFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pContextBuffersInfo != NULL, NV_ERR_INVALID_STATE);

    rtvcbBufferSize =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL].size;
    rtvcbBufferAlign =
        pKernelGraphicsStaticInfo->pContextBuffersInfo->engine[NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_RTV_CB_GLOBAL].alignment;

    if (rtvcbBufferSize > 0)
    {
        ppMemDesc = &pCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_RTV_CB];
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(ppMemDesc, pGpu,
                          rtvcbBufferSize,
                          rtvcbBufferAlign,
                          !bIsFbBroken,
                          ADDR_UNKNOWN,
                          pCtxAttr[GR_GLOBALCTX_BUFFER_RTV_CB].cpuAttr,
                          allocFlags));

        memdescSetGpuCacheAttrib(*ppMemDesc, NV_MEMORY_CACHED);
        if ((allocFlags & MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL) != 0)
        {
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

            memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
            NV_ASSERT_OK_OR_RETURN(memdescSetCtxBufPool(*ppMemDesc, pCtxBufPool));
        }
        memdescTagAllocList(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_113,
                    (*ppMemDesc), pCtxAttr[GR_GLOBALCTX_BUFFER_RTV_CB].pAllocList);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }
    status = kgraphicsAllocGrGlobalCtxBuffers_GP100(pGpu, pKernelGraphics, gfid, pKernelGraphicsContext);

    return status;
}

/**
 * @brief Initializes Bug 4208224 by performing the following actions
 *        1.) Sets up static handles inside an info struct to be referenced later
 *        2.) Creates a channel tied to VEID0 on GR0
 *        3.) Sends an RPC to physical RM for the physical side initialization
 */
NV_STATUS
kgraphicsInitializeBug4208224WAR_TU102
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NV_STATUS   status = NV_OK;
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV2080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS params = {0};

    if (pKernelGraphics->bug4208224Info.bConstructed)
    {
        return NV_OK;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        kgraphicsCreateBug4208224Channel_HAL(pGpu, pKernelGraphics));

    params.bTeardown = NV_FALSE;
    status =  pRmApi->Control(pRmApi,
                        pKernelGraphics->bug4208224Info.hClient,
                        pKernelGraphics->bug4208224Info.hSubdeviceId,
                        NV2080_CTRL_CMD_INTERNAL_KGR_INIT_BUG4208224_WAR,
                        &params,
                        sizeof(params));

    if (status != NV_OK)
    {
        NV_ASSERT_OK(pRmApi->Free(pRmApi,
            pKernelGraphics->bug4208224Info.hClient,
            pKernelGraphics->bug4208224Info.hClient));
    }

    return status;
}

/*!
 * @brief Creates a VEID0 channel for Bug 4208224 WAR
 *
 * @return NV_OK if channel created successfully
 */
NV_STATUS
kgraphicsCreateBug4208224Channel_TU102
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    NV_STATUS                              status = NV_OK;
    NvHandle                               hClientId = NV01_NULL_OBJECT;
    NvHandle                               hDeviceId;
    NvHandle                               hSubdeviceId;
    NvHandle                               hVASpace     = KGRAPHICS_SCRUBBER_HANDLE_VAS;
    NvHandle                               hPBVirtMemId = KGRAPHICS_SCRUBBER_HANDLE_PBVIRT;
    NvHandle                               hPBPhysMemId = KGRAPHICS_SCRUBBER_HANDLE_PBPHYS;
    NvHandle                               hChannelId   = KGRAPHICS_SCRUBBER_HANDLE_CHANNEL;
    NvHandle                               hObj3D       = KGRAPHICS_SCRUBBER_HANDLE_3DOBJ;
    NvHandle                               hUserdId     = KGRAPHICS_SCRUBBER_HANDLE_USERD;
    NvU32                                  gpFifoEntries = 32;       // power-of-2 random choice
    NvU64                                  gpFifoSize = NVA06F_GP_ENTRY__SIZE * gpFifoEntries;
    NvU64                                  chSize = gpFifoSize;
    RM_API                                *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsClient                              *pClientId;
    NvBool                                 bBcStatus;
    NvBool                                 bClientUserd = IsVOLTAorBetter(pGpu);
    NvBool                                 bAcquireLock = NV_FALSE;
    NvU32                                  sliLoopReentrancy;
    NV_VASPACE_ALLOCATION_PARAMETERS       vaParams;
    NV_MEMORY_ALLOCATION_PARAMS            memAllocParams;
    NV_CHANNEL_ALLOC_PARAMS channelGPFIFOAllocParams;
    NvU32                                  classNum;
    NvU32                                  primarySliSubDeviceInstance;
    // XXX This should be removed when broadcast SLI support is deprecated
    if (!gpumgrIsParentGPU(pGpu))
    {
        return NV_OK;
    }

    bBcStatus = gpumgrGetBcEnabledStatus(pGpu);

    // FIXME these allocations corrupt BC state
    NV_ASSERT_OK_OR_RETURN(
        rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClientId, &hDeviceId, &hSubdeviceId));

    pKernelGraphics->bug4208224Info.hClient = hClientId;
    pKernelGraphics->bug4208224Info.hDeviceId = hDeviceId;
    pKernelGraphics->bug4208224Info.hSubdeviceId = hSubdeviceId;

    // rmapiutilAllocClientAndDeviceHandles allocates a subdevice object for this subDeviceInstance
    primarySliSubDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClientId, &pClientId));

    gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);

    // As we have forced here SLI broadcast mode, temporarily reset the reentrancy count
    sliLoopReentrancy = gpumgrSLILoopReentrancyPop(pGpu);

    // Allocate subdevices for secondary GPUs
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        NvHandle hSecondary;
        NV2080_ALLOC_PARAMETERS nv2080AllocParams;
        NvU32 thisSubDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        // Skip if already allocated by rmapiutilAllocClientAndDeviceHandles()
        if (thisSubDeviceInstance == primarySliSubDeviceInstance)
            SLI_LOOP_CONTINUE;

        // Allocate a subDevice
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            clientGenResourceHandle(pClientId, &hSecondary),
            cleanup);

        portMemSet(&nv2080AllocParams, 0, sizeof(nv2080AllocParams));
        nv2080AllocParams.subDeviceId = thisSubDeviceInstance;

        NV_CHECK_OK(status, LEVEL_SILENT,
            pRmApi->AllocWithHandle(pRmApi,
                                    hClientId,
                                    hDeviceId,
                                    hSecondary,
                                    NV20_SUBDEVICE_0,
                                    &nv2080AllocParams,
                                    sizeof(nv2080AllocParams)));
    }
    SLI_LOOP_END;

    //
    // VidHeapControl and vaspace creation calls should happen outside GPU locks
    // UVM/CUDA may be holding the GPU locks here and the allocation may subsequently fail
    // So explicitly release GPU locks before RmVidHeapControl
    //
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    bAcquireLock = NV_TRUE;
    pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);

    // Create a new VAspace for channel
    portMemSet(&vaParams, 0, sizeof(NV_VASPACE_ALLOCATION_PARAMETERS));

    vaParams.flags = NV_VASPACE_ALLOCATION_FLAGS_PTETABLE_HEAP_MANAGED;

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hVASpace, FERMI_VASPACE_A, &vaParams, sizeof(vaParams)),
        cleanup);

    // Allocate gpfifo entries
    portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = chSize;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.hVASpace  = 0; // Physical allocations don't expect vaSpace handles

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hPBPhysMemId, NV01_MEMORY_SYSTEM, &memAllocParams, sizeof(memAllocParams)),
        cleanup);

    portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = chSize;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.flags     = NVOS32_ALLOC_FLAGS_VIRTUAL;
    memAllocParams.hVASpace  = hVASpace; // Virtual allocation expect vaSpace handles
                                         // 0 handle = allocations on gpu default vaSpace

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hPBVirtMemId, NV50_MEMORY_VIRTUAL, &memAllocParams, sizeof(memAllocParams)),
        cleanup);

    // Allocate Userd
    if (bClientUserd)
    {
        NvU32 userdMemClass = NV01_MEMORY_LOCAL_USER;
        NvU32 ctrlSize;

        if (gpuIsClassSupported(pGpu, VOLTA_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc36fControl);
        }
        else if (gpuIsClassSupported(pGpu, TURING_CHANNEL_GPFIFO_A))
        {
            ctrlSize = sizeof(Nvc46fControl);
        }
        else
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto cleanup;
        }

        portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));
        memAllocParams.owner = HEAP_OWNER_RM_CLIENT_GENERIC;
        memAllocParams.size  = ctrlSize;
        memAllocParams.type  = NVOS32_TYPE_IMAGE;

        // Apply registry overrides to USERD.
        switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _USERD, pGpu->instLocOverrides))
        {
            case NV_REG_STR_RM_INST_LOC_USERD_NCOH:
            case NV_REG_STR_RM_INST_LOC_USERD_COH:
                userdMemClass = NV01_MEMORY_SYSTEM;
                memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
                break;

            case NV_REG_STR_RM_INST_LOC_USERD_VID:
            case NV_REG_STR_RM_INST_LOC_USERD_DEFAULT:
                memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) |
                                      DRF_DEF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES);
                memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _INTERNAL, _YES);
                memAllocParams.flags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;
                break;
        }

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hUserdId,
                                    userdMemClass, &memAllocParams, sizeof(memAllocParams)),
            cleanup);
    }

    // Get fifo channel class Id
    classNum = kfifoGetChannelClassId(pGpu, GPU_GET_KERNEL_FIFO(pGpu));
    NV_ASSERT_OR_GOTO(classNum != 0, cleanup);

    // Allocate a bare channel
    portMemSet(&channelGPFIFOAllocParams, 0, sizeof(NV_CHANNEL_ALLOC_PARAMS));
    channelGPFIFOAllocParams.hVASpace      = hVASpace;
    channelGPFIFOAllocParams.hObjectBuffer = hPBVirtMemId;
    channelGPFIFOAllocParams.gpFifoEntries = gpFifoEntries;

    //
    // Set the gpFifoOffset to zero intentionally since we only need this channel
    // to be created, but will not submit any work to it. So it's fine not to
    // provide a valid offset here.
    //
    channelGPFIFOAllocParams.gpFifoOffset  = 0;
    if (bClientUserd)
    {
        channelGPFIFOAllocParams.hUserdMemory[0] = hUserdId;
    }

    channelGPFIFOAllocParams.engineType = gpuGetNv2080EngineType(RM_ENGINE_TYPE_GR0);

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId, hChannelId,
                                classNum, &channelGPFIFOAllocParams, sizeof(channelGPFIFOAllocParams)),
        cleanup);

    // Reaquire the GPU locks
    NV_ASSERT_OK_OR_GOTO(status,
        rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GR),
        cleanup);
    bAcquireLock = NV_FALSE;
    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Get KernelGraphicsObject class Id
    NV_ASSERT_OK_OR_GOTO(status,
        kgraphicsGetClassByType(pGpu, pKernelGraphics, GR_OBJECT_TYPE_3D, &classNum),
        cleanup);
    NV_ASSERT_OR_GOTO(classNum != 0, cleanup);

    // Allocate a GR object on the channel
    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->AllocWithHandle(pRmApi, hClientId, hChannelId, hObj3D, classNum, NULL, 0),
        cleanup);

cleanup:

    if (bAcquireLock)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GR));
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    }

    if (status != NV_OK)
    {
        // Drop GPU lock while freeing memory and channel handles
        // Free all handles
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            pRmApi->Free(pRmApi, hClientId, hClientId));
    }

    pKernelGraphics->bug4208224Info.bConstructed = (status == NV_OK);

    // Restore the reentrancy count
    gpumgrSLILoopReentrancyPush(pGpu, sliLoopReentrancy);

    gpumgrSetBcEnabledStatus(pGpu, bBcStatus);

    return status;
}

/*!
 * @brief Determines if a channel for Bug 4208224 is needed
 */
NvBool
kgraphicsIsBug4208224WARNeeded_TU102
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM))
    {
        return NV_FALSE;
    }

    return kgraphicsGetBug4208224WAREnabled(pGpu, pKernelGraphics);
}
