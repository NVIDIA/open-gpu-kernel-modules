/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "os/os.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "mem_mgr/gpu_vaspace.h"
#include "core/locks.h"
#include "nvrm_registry.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr/ctx_buf_pool.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "platform/chipset/chipset.h"
#include "platform/sli/sli.h"

#include "class/clc0b5sw.h"
#include "class/cla06fsubch.h" // NVA06F_SUBCHANNEL_COPY_ENGINE
#include "class/cl003e.h"      // NV01_MEMORY_SYSTEM
#include "class/cl0040.h"      // NV01_MEMORY_LOCAL_USER
#include "class/cl50a0.h"      // NV50_MEMORY_VIRTUAL
#include "class/cl00c2.h"      // NV01_MEMORY_LOCAL_PHYSICAL
#include "class/clb0b5.h"      // MAXWELL_DMA_COPY_A
#include "class/clc8b5.h"      // HOPPER_DMA_COPY_A
#include "class/cl90f1.h"      // FERMI_VASPACE_A


static NV_STATUS _memUtilsChannelAllocatePB_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel);
static NV_STATUS _memUtilsAllocateChannel(OBJGPU *pGpu, MemoryManager *pMemoryManager, NvHandle hClientId,
                                    NvHandle hDeviceId, NvHandle hChannelId, NvHandle hObjectError,
                                    NvHandle hObjectBuffer, OBJCHANNEL *pChannel,
                                    NvBool bFixedChId, NvU32 chId);
static NV_STATUS _memUtilsAllocCe_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel,
                                             NvHandle hClientId, NvHandle hDeviceId, NvHandle hChannelId, NvHandle hCopyObjectId);
static NV_STATUS _memUtilsAllocateUserD(OBJGPU *pGpu, MemoryManager *pMemoryManager, NvHandle hClientId,
                                        NvHandle hDeviceId, OBJCHANNEL *pChannel);
static NV_STATUS _memUtilsMapUserd_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager,
                           OBJCHANNEL *pChannel, NvHandle hClientId, NvHandle hDeviceId,
                           NvHandle hChannelId, NvBool bUseRmApiForBar1);


static NV_STATUS
_memUtilsChannelAllocatePB_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel

    //  OBJMEMUTILS *to be added here
)
{
    NV_STATUS                   rmStatus = NV_OK;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    NvHandle                    hDevice;
    NvHandle                    hPhysMem;
    NvU64                       size;
    NvHandle                    hVirtMem;
    NvU32                       hClass;
    NvU32                       attr;
    NvU32                       flags        = 0;
    NvU32                       attrNotifier = NVOS32_ATTR_NONE;
    RM_API                     *pRmApi       = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Apply registry overrides to channel pushbuffer.
    switch (DRF_VAL(_REG_STR_RM, _INST_LOC_4, _CHANNEL_PUSHBUFFER, pGpu->instLocOverrides4))
    {
        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_VID:
            hClass = NV01_MEMORY_LOCAL_USER;
            attr   = DRF_DEF(OS32, _ATTR, _LOCATION,  _VIDMEM)     |
                     DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);

            flags = NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM;

            if (!IS_MIG_IN_USE(pGpu))
            {
                attr |= DRF_DEF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES);
            }
            attrNotifier = attr;
            break;

        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_COH:
            hClass = NV01_MEMORY_SYSTEM;
            attr   = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)        |
                     DRF_DEF(OS32, _ATTR, _COHERENCY, _CACHED);
            attrNotifier = attr;
            break;

        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_NCOH:
            hClass = NV01_MEMORY_SYSTEM;
            attr   = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)        |
                     DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);
            attrNotifier = attr;
            break;

        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_DEFAULT:
        default:
            hClass = NV01_MEMORY_SYSTEM;
            attr   = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)        |
                     DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);

            //
            // The work submit token is read from notifier memory to support
            // VM migration for the memory scrubber. The token is read from
            // the notifier memory every time when the scrubber submits the work.
            // It will help performance by changing the default setting of
            // the notifier memory to be cached.
            //
            attrNotifier = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)  |
                           DRF_DEF(OS32, _ATTR, _COHERENCY, _CACHED);
            break;
    }

    hDevice           =  pChannel->deviceId;
    hPhysMem          =  pChannel->physMemId;
    hVirtMem          =  pChannel->pushBufferId;
    size              =  pChannel->channelSize;

    NV_ASSERT_OR_RETURN(!rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // allocate the physical memory
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = size;
    memAllocParams.attr      = attr;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = flags;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
    {
        memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION,
                                        _UNPROTECTED);
    }

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                hDevice,
                                hPhysMem,
                                hClass,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // allocate the Virtual memory
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = size;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL;
    memAllocParams.hVASpace = pChannel->hVASpaceId;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                hDevice,
                                hVirtMem,
                                NV50_MEMORY_VIRTUAL,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // allocate the physmem for the notifier

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        //
        // Force error notifier to ncoh sysmem when CC is enabled
        // since key rotation notifier is part of error notifier and
        // it needs to be in sysmem so we can create persistent mapping for it.
        // we cannot create mappins on the fly since this notifier is
        // written as part of 1 sec callback where creating mappings is
        // not allowed.
        //
        hClass = NV01_MEMORY_SYSTEM;
        attrNotifier   = DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)        |
                         DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);
    }

    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = pChannel->channelNotifierSize;
    memAllocParams.attr      = attrNotifier;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = 0;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

    //
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
    {
        memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION,
                                        _UNPROTECTED);
    }

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                hDevice,
                                pChannel->errNotifierIdPhys,
                                hClass,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // allocate Virtual Memory for the notifier
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = pChannel->channelNotifierSize;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL;
    memAllocParams.hVASpace = pChannel->hVASpaceId;

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                hDevice,
                                pChannel->errNotifierIdVirt,
                                NV50_MEMORY_VIRTUAL,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    return rmStatus;
}

static NV_STATUS
memmgrMemUtilsMapFbAlias
(
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV2080_CTRL_GPU_GET_MAX_SUPPORTED_PAGE_SIZE_PARAMS maxPageSizeParams = {0};
    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi, pChannel->hClient, pChannel->subdeviceId,
                        NV2080_CTRL_CMD_GPU_GET_MAX_SUPPORTED_PAGE_SIZE,
                        &maxPageSizeParams, sizeof(maxPageSizeParams)));
    NV_ASSERT_OR_RETURN(maxPageSizeParams.maxSupportedPageSize != 0, NV_ERR_INVALID_STATE);

    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS vasCapsParams = {0};
    vasCapsParams.hVASpace = pChannel->hVASpaceId;
    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi, pChannel->hClient, pChannel->deviceId,
                        NV0080_CTRL_CMD_DMA_ADV_SCHED_GET_VA_CAPS,
                        &vasCapsParams, sizeof(vasCapsParams)));
    NV_ASSERT_OR_RETURN(vasCapsParams.supportedPageSizeMask != 0, NV_ERR_INVALID_STATE);

    // Bug 4737595: Clear 512MB and higher page supported mask
    vasCapsParams.supportedPageSizeMask &= ~RM_PAGE_SIZE_512M;
    vasCapsParams.supportedPageSizeMask &= ~RM_PAGE_SIZE_256G;

    NvU64 currentFbOffset = 0;
    NvU64 remainingMapSize = pChannel->fbSize;
    NvU64 currentVaddr = pChannel->vaStartOffset;

    NvU64 pageSizeMask = vasCapsParams.supportedPageSizeMask;

    // Vidmem allocation page size is limited by VMMU segment size
    pageSizeMask &= (maxPageSizeParams.maxSupportedPageSize << 1) - 1;

    // Pick only page sizes to which currentVaddr is aligned
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(pChannel->startFbOffset, RM_PAGE_SIZE_512M), NV_ERR_INVALID_STATE);
    pageSizeMask &= (LOWESTBIT(pChannel->startFbOffset) << 1) - 1;

    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(pChannel->vaStartOffset, RM_PAGE_SIZE_512M), NV_ERR_INVALID_STATE);
    pageSizeMask &= (LOWESTBIT(pChannel->vaStartOffset) << 1) - 1;

    NV_PRINTF(LEVEL_INFO, "fb start 0x%llx size 0x%llx supported page sizes 0x%llx (0x%llx) va start 0x%llx\n",
        pChannel->startFbOffset, pChannel->fbSize, pageSizeMask,
        vasCapsParams.supportedPageSizeMask, pChannel->vaStartOffset);

    while ((currentFbOffset < pChannel->fbSize) && (pageSizeMask != 0))
    {
        NvU32 pageSizeMap;

        // Biggest page size from the mask
        NvU64 pageSize = nvPrevPow2_U64(pageSizeMask);
        pageSizeMask &= ~pageSize;

        NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(currentFbOffset, pageSize), NV_ERR_INVALID_STATE);
        NV_ASSERT_OR_RETURN(NV_IS_ALIGNED64(currentVaddr, pageSize), NV_ERR_INVALID_STATE);

        if (pageSize > remainingMapSize)
        {
            continue;
        }

        switch (pageSize)
        {
            case RM_PAGE_SIZE:
                pageSizeMap = NVOS46_FLAGS_PAGE_SIZE_4KB;
                break;
            case RM_PAGE_SIZE_64K:
            case RM_PAGE_SIZE_128K:
                NV_ASSERT_OR_RETURN(pageSize == vasCapsParams.bigPageSize, NV_ERR_INVALID_STATE);
                pageSizeMap = NVOS46_FLAGS_PAGE_SIZE_BIG;
                break;
            case RM_PAGE_SIZE_HUGE:
                pageSizeMap = NVOS46_FLAGS_PAGE_SIZE_HUGE;
                break;
            case RM_PAGE_SIZE_512M:
                pageSizeMap = NVOS46_FLAGS_PAGE_SIZE_512M;
                break;
            default:
                NV_PRINTF(LEVEL_INFO, "page size not supported: 0x%llx\n", pageSize);
                continue;
        }

        NvU64 mapSize = NV_ALIGN_DOWN(remainingMapSize, pageSize);
        NVOS46_PARAMETERS mapDmaParams = {0};

        mapDmaParams.hClient   = pChannel->hClient;
        mapDmaParams.hDevice   = pChannel->deviceId;
        mapDmaParams.hDma      = pChannel->hFbAliasVA;
        mapDmaParams.hMemory   = pChannel->hFbAlias;
        mapDmaParams.offset    = currentFbOffset;
        mapDmaParams.length    = mapSize;
        mapDmaParams.flags     = DRF_NUM(OS46, _FLAGS, _PAGE_SIZE, pageSizeMap) |
                                 DRF_DEF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE),
        mapDmaParams.dmaOffset = currentVaddr;

        NV_ASSERT_OK_OR_RETURN(pRmApi->Map(pRmApi, &mapDmaParams));

        currentVaddr = mapDmaParams.dmaOffset;

        NV_PRINTF(LEVEL_INFO,
            "CeUtils VAS : FB (addr: %llx, size: %llx) identity mapped to VAS at addr: %llx, page size: 0x%llx\n",
             currentFbOffset, mapSize, currentVaddr, pageSize);

        remainingMapSize -= mapSize;
        currentFbOffset += mapSize;
        currentVaddr += mapSize;
    }
    NV_ASSERT_OR_RETURN(currentFbOffset == pChannel->fbSize, NV_ERR_INVALID_STATE);
    pChannel->fbAliasVA = pChannel->vaStartOffset;
    return NV_OK;
}

NV_STATUS
memmgrMemUtilsChannelInitialize_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvBool         bFixedChId,
    NvU32          chId
)
{
    NV_STATUS         rmStatus;
    NV_STATUS         lockStatus;
    RsClient         *pRsClient           = pChannel->pRsClient;
    NvHandle          hClient             = pChannel->hClient;
    NvHandle          hDevice             = pChannel->deviceId;
    NvHandle          hPhysMem            = pChannel->physMemId;
    NvU64             size                = pChannel->channelSize;
    NvHandle          hChannel            = pChannel->channelId;
    NvHandle          hErrNotifierVirt    = pChannel->errNotifierIdVirt;
    NvHandle          hErrNotifierPhys    = pChannel->errNotifierIdPhys;
    NvHandle          hPushBuffer         = pChannel->pushBufferId;
    RM_API           *pRmApi              = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool            bMIGInUse           = IS_MIG_IN_USE(pGpu);
    NvU8             *pErrNotifierCpuVA   = NULL;
    NV_ADDRESS_SPACE  userdAddrSpace;
    NV_ADDRESS_SPACE  pushBuffAddrSpace;
    NV_ADDRESS_SPACE  gpFifoAddrSpace;
    OBJSYS           *pSys                = SYS_GET_INSTANCE();
    OBJCL            *pCl                 = SYS_GET_CL(pSys);
    NvU32             cacheSnoopFlag      = 0 ;
    NvBool            bUseRmApiForBar1    = NV_FALSE;
    NvU32             transferFlags       = pChannel->bUseBar1 ? TRANSFER_FLAGS_USE_BAR1 : TRANSFER_FLAGS_NONE;
    NVOS46_PARAMETERS mapDmaParams;

    // Legacy SLI support is not implemented
    NV_ASSERT_OR_RETURN(!IsSLIEnabled(pGpu) , NV_ERR_NOT_SUPPORTED);

    //
    // Heap alloc one chunk of memory to hold all of our alloc parameters to
    // reduce stack usage
    //
    union
    {
        NV_VASPACE_ALLOCATION_PARAMETERS va;
        NV_MEMORY_ALLOCATION_PARAMS      mem;
    } *pParams = NULL;

    if (pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT))
    {
        cacheSnoopFlag = DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);
    }

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        goto exit_free_client;
    }

    //
    // client allocated userd only supported on volta+
    // TODO: Use property to check if client allocated userd is supported
    //
    pChannel->bClientUserd = NV_TRUE; // TODO: MPV

    //
    // We need to allocate a VAS to use for CE copies, but also for
    // GSP-RM + MIG, so that it doesn't get the device
    // default VAS during channel bind (which is not properly handled
    // by split VAS in MIG currently). We only need the identity mapping
    // when actually using the VAS for copies.
    //
    if (pChannel->bUseVasForCeCopy ||
        (IS_GSP_CLIENT(pGpu) && bMIGInUse))
    {
        NvBool bAcquireLock = NV_FALSE;
        NvU64 startFbOffset = GPU_GET_HEAP(pGpu)->base;
        NvU64 fbSize        = GPU_GET_HEAP(pGpu)->total;
        NvU64 vaStartOffset = startFbOffset;

        NV_PRINTF(LEVEL_INFO, "Channel VAS heap base: %llx total: %llx \n", GPU_GET_HEAP(pGpu)->base,
                  GPU_GET_HEAP(pGpu)->total);

        pChannel->startFbOffset = startFbOffset;
        pChannel->fbSize = fbSize;

        if (pChannel->bUseVasForCeCopy)
        {
            NV_ASSERT_OK_OR_GOTO(rmStatus,
                clientGenResourceHandle(pRsClient, &pChannel->hFbAlias),
                exit_free_client);

            rmStatus = memmgrMemUtilsCreateMemoryAlias_HAL(pGpu, pMemoryManager, pChannel);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Setting Identity mapping failed.. status: %x\n", rmStatus);
                goto exit_free_client;
            }
        }

        {
            NV_VASPACE_ALLOCATION_PARAMETERS *pVa = &pParams->va;

            portMemSet(pVa, 0, sizeof(*pVa));
            pVa->index  = NV_VASPACE_ALLOCATION_INDEX_GPU_NEW;
            pVa->vaBase = pChannel->startFbOffset;
            //
            // how large should we go here ? we definitely need more than heapSize to allocate
            // other metadata related to chnanel. Also need to account the discontiguous VA Range
            // for split VAS, where we allocate 4GB to (4GB + 512MB) for Server VAS (mirrored).
            // Rough VASpace Layout will be documented here:
            //
            //
            if (gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
            {
                pVa->vaSize += (SPLIT_VAS_SERVER_RM_MANAGED_VA_START +
                                SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE) ;
            }
            pVa->vaSize += fbSize + pChannel->channelSize + SCRUBBER_VASPACE_BUFFER_SIZE;

            //
            // We definitely need ALLOW_ZERO_ADDRESS, but SKIP_SCRUB_MEMPOOL is a patch
            // until we figure out the right place for Scrubber page tables
            //
            pVa->flags |= NV_VASPACE_ALLOCATION_FLAGS_ALLOW_ZERO_ADDRESS |
                          NV_VASPACE_ALLOCATION_FLAGS_SKIP_SCRUB_MEMPOOL |
                          NV_VASPACE_ALLOCATION_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE;

            if (!IS_MIG_IN_USE(pGpu))
            {
                pVa->flags |= NV_VASPACE_ALLOCATION_FLAGS_PTETABLE_HEAP_MANAGED;
            }

            if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance) && !rmapiInRtd3PmPath())
            {
                rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
                bAcquireLock = NV_TRUE;
                pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
            }

            rmStatus = pRmApi->AllocWithHandle(pRmApi, hClient, pChannel->deviceId,
                                               pChannel->hVASpaceId, FERMI_VASPACE_A,
                                               pVa, sizeof(*pVa));
        }
        if (bAcquireLock)
        {
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus, rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM));
            bAcquireLock = NV_FALSE;
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }

        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed allocating scrubber vaspace, status=0x%x\n",
                    rmStatus);
            goto exit_free_client;
        }

        rmStatus = vaspaceGetByHandleOrDeviceDefault(pRsClient,
                                                     pChannel->deviceId,
                                                     pChannel->hVASpaceId,
                                                     &pChannel->pVAS);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "failed getting the scrubber vaspace from handle, status=0x%x\n",
                    rmStatus);
            goto exit_free_client;
        }

        if (pChannel->bUseVasForCeCopy)
        {
            if (!gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
            {
                rmStatus = vaspacePinRootPageDir(pChannel->pVAS, pGpu);
                if (rmStatus != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "failed pinning down Scrubber VAS, status=0x%x\n",
                            rmStatus);
                    goto exit_free_client;
                }
            }

            NV_ASSERT_OK_OR_GOTO(rmStatus,
                 clientGenResourceHandle(pRsClient, &pChannel->hFbAliasVA), exit_free_client);
        }

        if (gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
        {
            OBJGVASPACE *pGVAS = dynamicCast(pChannel->pVAS, OBJGVASPACE);
            // Align to 512M in order to use it as page size for identity mapping
            vaStartOffset += NV_ALIGN_UP64(pGVAS->vaLimitServerRMOwned + 1, RM_PAGE_SIZE_512M);
            pChannel->vaStartOffset = vaStartOffset;
        }

        if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        {
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            bAcquireLock = NV_TRUE;
            pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        }

        // Allocate virtual memory for Identity Mapping
        if (pChannel->bUseVasForCeCopy)
        {
            NV_MEMORY_ALLOCATION_PARAMS *pMem = &pParams->mem;
            portMemSet(pMem, 0, sizeof(*pMem));
            pMem->owner     = NVOS32_TYPE_OWNER_RM;
            pMem->type      = NVOS32_TYPE_IMAGE;
            pMem->size      = pChannel->fbSize;
            pMem->attr      = (DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI) |
                               DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG));
            pMem->attr2     = NVOS32_ATTR2_NONE;
            pMem->offset    = vaStartOffset;
            pMem->flags     = 0;
            pMem->flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL |
                              NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE |
                              NVOS32_ALLOC_FLAGS_LAZY;
            pMem->hVASpace = pChannel->hVASpaceId;

            rmStatus = pRmApi->AllocWithHandle(pRmApi,
                                               hClient,
                                               pChannel->deviceId,
                                               pChannel->hFbAliasVA,
                                               NV50_MEMORY_VIRTUAL,
                                               pMem,
                                               sizeof(*pMem));
        }

        if (bAcquireLock)
        {
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus, rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM));
            bAcquireLock = NV_FALSE;
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }

        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Allocating VASpace for (base, size): (%llx, %llx) failed,"
                                   " with status: %x\n", vaStartOffset, pChannel->fbSize, rmStatus);
            goto exit_free_client;
        }

        // set up mapping of VA -> PA
        if (pChannel->bUseVasForCeCopy)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                memmgrMemUtilsMapFbAlias(pMemoryManager, pChannel), exit_free_client);
        }
    }

    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    //
    // Fetch the physical location of the push buffer
    //
    // Bug 3434881 filed to track the following
    // a.Implementation of the utility function to parse the
    //   push buffer and userd regkeys
    // b.Replace all instances of regkey pushbuffer/userd regkey
    //   parsing with the utility function
    //
    switch (DRF_VAL(_REG_STR_RM, _INST_LOC_4, _CHANNEL_PUSHBUFFER, pGpu->instLocOverrides4))
    {
        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_VID:
            pushBuffAddrSpace = ADDR_FBMEM;
            break;

        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_COH:
        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_NCOH:
        case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_DEFAULT:
        default:
            pushBuffAddrSpace = ADDR_SYSMEM;
            break;
    }

    gpFifoAddrSpace = pushBuffAddrSpace;

    //Fetch the physical location of userD
    switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _USERD, pGpu->instLocOverrides))
    {
        case NV_REG_STR_RM_INST_LOC_USERD_NCOH:
        case NV_REG_STR_RM_INST_LOC_USERD_COH:
            userdAddrSpace = ADDR_SYSMEM;
            break;

        case NV_REG_STR_RM_INST_LOC_USERD_VID:
        case NV_REG_STR_RM_INST_LOC_USERD_DEFAULT:
        default:
            userdAddrSpace = ADDR_FBMEM;
            break;
    }

    // RM WAR for Bug 3313719
    // Disallow USERD in sysmem and (GPFIFO or pushbuffer) in vidmem
    rmStatus = kfifoCheckChannelAllocAddrSpaces_HAL(GPU_GET_KERNEL_FIFO(pGpu),
                                                    userdAddrSpace,
                                                    pushBuffAddrSpace,
                                                    gpFifoAddrSpace);
    if (rmStatus != NV_OK)
    {
        NV_ASSERT_FAILED("USERD in sysmem and PushBuffer/GPFIFO in vidmem not allowed");
        goto exit_free_client;
    }

    rmStatus = _memUtilsChannelAllocatePB_GM107(pGpu, pMemoryManager, pChannel);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Push buffer allocation failed: 0x%x\n", rmStatus);
        goto exit_free_client;
    }
    lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM);
    if(lockStatus != NV_OK)
    {
        NV_ASSERT_FAILED("Could not get back lock after allocating Push Buffer sema");
        goto exit_free_client;
    }

    // map the pushbuffer
    portMemSet(&mapDmaParams, 0, sizeof(mapDmaParams));
    mapDmaParams.hClient   = hClient;
    mapDmaParams.hDevice   = hDevice;
    mapDmaParams.hDma      = hPushBuffer;
    mapDmaParams.hMemory   = hPhysMem;
    mapDmaParams.length    = size;
    mapDmaParams.flags     = cacheSnoopFlag;

    rmStatus = pRmApi->Map(pRmApi, &mapDmaParams);

    pChannel->pbGpuVA = mapDmaParams.dmaOffset;

    // map the error notifier
    portMemSet(&mapDmaParams, 0, sizeof(mapDmaParams));
    mapDmaParams.hClient   = hClient;
    mapDmaParams.hDevice   = hDevice;
    mapDmaParams.hDma      = hErrNotifierVirt;
    mapDmaParams.hMemory   = hErrNotifierPhys;
    mapDmaParams.length    = pChannel->channelNotifierSize;
    mapDmaParams.flags     = DRF_DEF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE) | cacheSnoopFlag,

    rmStatus = pRmApi->Map(pRmApi, &mapDmaParams);

    pChannel->pbGpuNotifierVA = mapDmaParams.dmaOffset;

    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        _memUtilsAllocateChannel(pGpu,
                                 pMemoryManager,
                                 hClient,
                                 hDevice,
                                 hChannel,
                                 hErrNotifierVirt,
                                 hPushBuffer,
                                 pChannel,
                                 bFixedChId,
                                 chId),
        exit_free_client);

    // _memUtilsMapUserd
    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        _memUtilsMapUserd_GM107(pGpu, pMemoryManager, pChannel,
                                hClient, hDevice, hChannel, bUseRmApiForBar1),
        exit_free_client);

    // Set up pushbuffer and semaphore memdesc and memset the buffer
    pChannel->pChannelBufferMemdesc =
        memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, pChannel->hClient, hPhysMem);
    NV_ASSERT_OR_GOTO(pChannel->pChannelBufferMemdesc != NULL, exit_free_client);

    // Set up notifier memory
    pChannel->pErrNotifierMemdesc =
        memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, pChannel->hClient, hErrNotifierPhys);
    NV_ASSERT_OR_GOTO(pChannel->pErrNotifierMemdesc != NULL, exit_free_client);

    if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)))
    {
        rmStatus = memmgrMemDescMemSet(pMemoryManager, pChannel->pChannelBufferMemdesc, 0,
                                       (TRANSFER_FLAGS_SHADOW_ALLOC | TRANSFER_FLAGS_SHADOW_INIT_MEM));
        NV_ASSERT_OR_GOTO(rmStatus == NV_OK, exit_free_client);

        pChannel->pbCpuVA = NULL;
        pChannel->pTokenFromNotifier = NULL;
    }
    else
    {
        if (bUseRmApiForBar1)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                pRmApi->MapToCpu(pRmApi, hClient, hDevice, hPhysMem, 0, size,
                                 (void **)&pChannel->pbCpuVA, 0),
                exit_free_client);

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                pRmApi->MapToCpu(pRmApi, hClient, hDevice, hErrNotifierPhys, 0,
                    pChannel->channelNotifierSize, (void **)&pErrNotifierCpuVA, 0),
                exit_free_client);
        }
        else
        {
            //
            // Most use cases can migrate to the internal memdescMap path for BAR1
            // And it is preferred because external path will not work with CC
            //
            pChannel->pbCpuVA = memmgrMemDescBeginTransfer(pMemoryManager,
                                    pChannel->pChannelBufferMemdesc, transferFlags);
            NV_ASSERT_OR_GOTO(pChannel->pbCpuVA != NULL, exit_free_client);

            pErrNotifierCpuVA = memmgrMemDescBeginTransfer(pMemoryManager,
                                    pChannel->pErrNotifierMemdesc, transferFlags);
            NV_ASSERT_OR_GOTO(pErrNotifierCpuVA != NULL, exit_free_client);
        }

        portMemSet(pChannel->pbCpuVA, 0, (NvLength)size);

        pChannel->pTokenFromNotifier =
            (NvNotification *)(pErrNotifierCpuVA +
                               (NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN *
                                sizeof(NvNotification)));
    }

    portMemFree(pParams);
    return NV_OK;

exit_free_client:
    if(!pChannel->bClientAllocated)
    {
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    }
    portMemFree(pParams);
    NV_PRINTF(LEVEL_INFO, "end  NV_STATUS=0x%08x\n", rmStatus);
    return rmStatus;
}


/** memmgrMemUtilsCreateMemoryAlias_GM107
 *
 *  @brief Creates an alias for the FB region
 *         This function doesn't allocate any memory but just creates memory handle
 *         which refers to FB range. This call can support for both baremetal and vGPU.
 *  @param[in] pChannel             CHANNEL Pointer
 *
 *  @returns NV_OK on success
 */
NV_STATUS
memmgrMemUtilsCreateMemoryAlias_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    RM_API  *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS status = NV_OK;

    NV_PHYSICAL_MEMORY_ALLOCATION_PARAMS physMemParams = {0};

    memmgrGetPteKindForScrubber_HAL(pMemoryManager, &physMemParams.format);


    status = pRmApi->AllocWithHandle(pRmApi,
                                    pChannel->hClient,
                                    pChannel->deviceId,
                                    pChannel->hFbAlias,
                                    NV01_MEMORY_LOCAL_PHYSICAL,
                                    &physMemParams,
                                    sizeof(physMemParams));
    if (status != NV_OK)
    {
        NV_CHECK_OK_FAILED(LEVEL_WARNING, "Aliasing FbListMem", status);
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "Allocating FbAlias: %x for size: %llx, kind: %x\n", pChannel->hFbAlias,
              pChannel->fbSize, physMemParams.format);


    return NV_OK;
}

NV_STATUS
memmgrMemUtilsCopyEngineInitialize_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    NV_STATUS rmStatus = NV_OK;
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    //allocce
    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        _memUtilsAllocCe_GM107(pGpu,
                               pMemoryManager,
                               pChannel,
                               pChannel->hClient,
                               pChannel->deviceId,
                               pChannel->channelId,
                               pChannel->engineObjectId),
        exit_free);

    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        memmgrMemUtilsChannelSchedulingSetup(pGpu, pMemoryManager, pChannel), exit_free);

    return NV_OK;

 exit_free:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    NV_PRINTF(LEVEL_INFO, "end  NV_STATUS=0x%08x\n", rmStatus);
    return rmStatus;
}

static NV_STATUS _memUtilsAllocCe_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvHandle       hClientId,
    NvHandle       hDeviceId,
    NvHandle       hChannelId,
    NvHandle       hCopyObjectId

)
{
    NVC0B5_ALLOCATION_PARAMETERS  createParams = {0};
    RM_API                       *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    createParams.version = NVC0B5_ALLOCATION_PARAMETERS_VERSION_1;
    createParams.engineType = NV2080_ENGINE_TYPE_COPY(pChannel->ceId);
    memmgrMemUtilsGetCopyEngineClass_HAL(pGpu, pMemoryManager, &pChannel->hTdCopyClass);
    pChannel->engineType = gpuGetRmEngineType(createParams.engineType);

    if (!pChannel->hTdCopyClass)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to determine CE's engine class.\n");
        return NV_ERR_GENERIC;
    }

    NV_CHECK_OK_OR_RETURN(
        LEVEL_ERROR,
        pRmApi->AllocWithHandle(pRmApi,
                                hClientId,
                                hChannelId,
                                hCopyObjectId,
                                pChannel->hTdCopyClass,
                                &createParams,
                                sizeof(createParams)));

    return NV_OK;
}

static NV_STATUS
_memUtilsMapUserd_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvHandle       hClientId,
    NvHandle       hDeviceId,
    NvHandle       hChannelId,
    NvBool         bUseRmApiForBar1
)
{
    //
    // The memTransfer API only works for client-allocated USERD
    // because otherwise we are calling MapToCpu using the channel
    // handle instead.
    //
    if (pChannel->bClientUserd && !bUseRmApiForBar1)
    {
        pChannel->pUserdMemdesc =
            memmgrMemUtilsGetMemDescFromHandle(pMemoryManager, hClientId, pChannel->hUserD);
        NV_ASSERT_OR_RETURN(pChannel->pUserdMemdesc != NULL, NV_ERR_GENERIC);

        if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)))
        {
            //
            // GPFIFO aceess will not be set up in order to facilitate memTransfer APIs
            // which will use GSP-DMA/CE with shadow buffers
            //
            pChannel->pControlGPFifo = NULL;
        }
        else
        {
            pChannel->pControlGPFifo =
                (void *)memmgrMemDescBeginTransfer(pMemoryManager, pChannel->pUserdMemdesc,
                                                   pChannel->bUseBar1 ? TRANSFER_FLAGS_USE_BAR1 : TRANSFER_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(pChannel->pControlGPFifo != NULL, NV_ERR_GENERIC);
        }
    }
    else
    {
        NvU32   userdSize = 0;
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        kfifoGetUserdSizeAlign_HAL(GPU_GET_KERNEL_FIFO(pGpu), &userdSize, NULL);

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            pRmApi->MapToCpu(pRmApi, hClientId, hDeviceId,
                             pChannel->bClientUserd ? pChannel->hUserD : hChannelId, 0,
                             userdSize, (void **)&pChannel->pControlGPFifo, 0));
    }
    return NV_OK;
}

static NV_STATUS
_memUtilsAllocateUserD
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvHandle       hClientId,
    NvHandle       hDeviceId,
    OBJCHANNEL    *pChannel
)
{
    NV_STATUS                    rmStatus = NV_OK;
    KernelFifo                  *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    RM_API                      *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_MEMORY_ALLOCATION_PARAMS  memAllocParams;
    NvU32                        userdMemClass = NV01_MEMORY_LOCAL_USER;

    // Ensure that call is not made with lock held
    NV_ASSERT_OR_RETURN(!rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));

    memAllocParams.owner = HEAP_OWNER_RM_CLIENT_GENERIC;
    kfifoGetUserdSizeAlign_HAL(pKernelFifo, (NvU32 *)&memAllocParams.size, NULL);
    memAllocParams.type  = NVOS32_TYPE_IMAGE;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

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
            userdMemClass = NV01_MEMORY_LOCAL_USER;
            memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
            memAllocParams.flags |= NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM;
            if (!IS_MIG_IN_USE(pGpu))
            {
                memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES);
            }
            break;
    }

    //
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
    {
        memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _MEMORY_PROTECTION,
                                        _UNPROTECTED);
    }

    NV_ASSERT_OK_OR_RETURN(pRmApi->AllocWithHandle(pRmApi, hClientId, hDeviceId,
                                                   pChannel->hUserD,
                                                   userdMemClass,
                                                   &memAllocParams,
                                                   sizeof(memAllocParams)));

    return rmStatus;
}

static NV_STATUS
_memUtilsAllocateChannel
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvHandle       hClientId,
    NvHandle       hDeviceId,
    NvHandle       hChannelId,
    NvHandle       hObjectError,
    NvHandle       hObjectBuffer,
    OBJCHANNEL    *pChannel,
    NvBool         bFixedChId,
    NvU32          chId
)
{
    NV_CHANNEL_ALLOC_PARAMS channelGPFIFOAllocParams;
    NV_STATUS               rmStatus =  NV_OK;
    NvU32                   hClass;
    RM_API                 *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool                  bMIGInUse = IS_MIG_IN_USE(pGpu);
    NvU32                   flags = DRF_DEF(OS04, _FLAGS, _CHANNEL_SKIP_SCRUBBER, _TRUE);
    RM_ENGINE_TYPE          engineType = (pChannel->type == SWL_SCRUBBER_CHANNEL) ?
                                RM_ENGINE_TYPE_SEC2 : RM_ENGINE_TYPE_COPY(pChannel->ceId);

    if (pChannel->bSecure)
    {
        flags |= DRF_DEF(OS04, _FLAGS, _CC_SECURE, _TRUE);
    }

    if (bFixedChId)
    {
        NvU32 userdSize = 0;
        NvU32 userdAlignment;
        NvU32 numChannels;
        KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

        kfifoGetUserdSizeAlign_HAL(pKernelFifo, &userdSize, NULL);
        userdAlignment = RM_PAGE_SIZE / userdSize;
        numChannels = kfifoChidMgrGetNumChannels(pGpu, pKernelFifo,
                          kfifoGetChidMgr(pGpu, pKernelFifo, 0));

        NV_ASSERT_OR_RETURN(chId < numChannels, NV_ERR_INVALID_ARGUMENT);

        flags |=
            DRF_DEF(OS04, _FLAGS, _CHANNEL_USERD_INDEX_PAGE_FIXED, _TRUE) |
            DRF_NUM(OS04, _FLAGS, _CHANNEL_USERD_INDEX_PAGE_VALUE,
                    chId / userdAlignment) |
            DRF_DEF(OS04, _FLAGS, _CHANNEL_USERD_INDEX_FIXED, _FALSE) |
            DRF_NUM(OS04, _FLAGS, _CHANNEL_USERD_INDEX_VALUE,
                    chId % userdAlignment);
    }

    portMemSet(&channelGPFIFOAllocParams, 0, sizeof(NV_CHANNEL_ALLOC_PARAMS));
    channelGPFIFOAllocParams.hObjectError  = hObjectError;
    channelGPFIFOAllocParams.hObjectBuffer = hObjectBuffer;
    channelGPFIFOAllocParams.gpFifoOffset  = pChannel->pbGpuVA + pChannel->channelPbSize;
    channelGPFIFOAllocParams.gpFifoEntries = pChannel->channelNumGpFifioEntries;
    channelGPFIFOAllocParams.hContextShare = NV01_NULL_OBJECT;
    channelGPFIFOAllocParams.flags         = flags;
    channelGPFIFOAllocParams.hVASpace      = pChannel->hVASpaceId;

    //
    // Use GPU instance local Id if MIG is enabled
    // TODO: Maybe we need a VAS for each GPU instance ?
    //
    if (bMIGInUse && (pChannel->pKernelMIGGpuInstance != NULL))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE localCe;
        RsClient *pClient;
        Device *pDevice;

        NV_ASSERT_OK_OR_RETURN(
            serverGetClientUnderLock(&g_resServ, hClientId, &pClient));

        NV_ASSERT_OK_OR_RETURN(
            deviceGetByHandle(pClient, hDeviceId, &pDevice));

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));
        // Clear the Compute instance portion, if present
        ref = kmigmgrMakeGIReference(ref.pKernelMIGGpuInstance);
        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              engineType,
                                              &localCe));
        channelGPFIFOAllocParams.engineType = gpuGetNv2080EngineType(localCe);
    }
    else
    {
        channelGPFIFOAllocParams.engineType = gpuGetNv2080EngineType(engineType);
    }

    hClass = kfifoGetChannelClassId(pGpu, GPU_GET_KERNEL_FIFO(pGpu));
    if (!hClass)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to determine CE's channel class.\n");
        return NV_ERR_GENERIC;
    }

    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    if (pChannel->bClientUserd)
    {
        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            _memUtilsAllocateUserD(pGpu,
                                   pMemoryManager,
                                   hClientId,
                                   hDeviceId,
                                   pChannel),
            cleanup);

        channelGPFIFOAllocParams.hUserdMemory[0] = pChannel->hUserD;
        channelGPFIFOAllocParams.userdOffset[0] = 0;
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        channelGPFIFOAllocParams.internalFlags = DRF_DEF(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _PRIVILEGE, _KERNEL) |
                                                 DRF_DEF(_KERNELCHANNEL, _ALLOC_INTERNALFLAGS, _GSP_OWNED, _YES);
    }

    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(
        rmStatus,
        pRmApi->AllocWithHandle(pRmApi,
                                hClientId,
                                hDeviceId,
                                hChannelId,
                                hClass,
                                &channelGPFIFOAllocParams,
                                sizeof(channelGPFIFOAllocParams)));

cleanup:
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(rmStatus,
                                        rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM));

    return rmStatus;
}

/*!
 * Getting the Copy Engine Class
 *
 * @param[in]     pGpu         OBJGPU pointer
 * @param[out]    pClass       pointer to class
 */
NV_STATUS
memmgrMemUtilsGetCopyEngineClass_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    NvU32         *pClass
)
{
    NV_STATUS status;
    NvU32 numClasses;
    NvU32 *pClassList = NULL;
    NvU32 i;
    NvU32 class = 0;
    NvU32 eng;

    //
    // Pascal+ chips will have any combination of the 6 CEs
    // available. Loop over all the CEs to get the CE class
    // for the first available CE instead of using ENG_CE(0)
    //
    for (eng = 0; eng < ENG_CE__SIZE_1; eng++)
    {
        NV_ASSERT_OK_OR_ELSE(
            status,
            gpuGetClassList(pGpu, &numClasses, NULL, ENG_CE(eng)),
            return 0);

        if (numClasses > 0)
        {
            break;
        }
    }

    pClassList = portMemAllocNonPaged(sizeof(*pClassList) * numClasses);
    NV_ASSERT_OR_RETURN((pClassList != NULL), 0);

    if (NV_OK == gpuGetClassList(pGpu, &numClasses, pClassList, ENG_CE(eng)))
    {
        for (i = 0; i < numClasses; i++)
        {
            class = NV_MAX(class, pClassList[i]);
        }
    }

    NV_ASSERT(class != 0);
    portMemFree(pClassList);
    *pClass = class;

    return NV_OK;
}
