/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvRmReg.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr/ctx_buf_pool.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "platform/chipset/chipset.h"

#include "class/clc0b5sw.h"
#include "class/cla06fsubch.h" // NVA06F_SUBCHANNEL_COPY_ENGINE
#include "class/cl003e.h"      // NV01_MEMORY_SYSTEM
#include "class/cl0040.h"      // NV01_MEMORY_LOCAL_USER
#include "class/cl0080.h"      // NV01_DEVICE_0
#include "class/cl50a0.h"      // NV50_MEMORY_VIRTUAL
#include "class/clc637.h"      // AMPERE_SMC_PARTITION_REF
#include "class/cl00c2.h"      // NV01_MEMORY_LOCAL_PHYSICAL
#include "class/clb0b5.h"      // MAXWELL_DMA_COPY_A
#include "class/clc8b5.h"      // HOPPER_DMA_COPY_A
#include "class/cl90f1.h"      // FERMI_VASPACE_A

#define NONSTALL_METHOD_SIZE            8
#define SEMAPHORE_ONLY_METHOD_SIZE      32
#define MAX_EXTRA_PAYLOAD               (NONSTALL_METHOD_SIZE + SEMAPHORE_ONLY_METHOD_SIZE)


static NV_STATUS _memUtilsChannelAllocatePB_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel);
static NV_STATUS _memUtilsAllocateChannel(OBJGPU *pGpu, MemoryManager *pMemoryManager, NvHandle hClientId,
                                    NvHandle hDeviceId, NvHandle hChannelId, NvHandle hObjectError,
                                    NvHandle hObjectBuffer, OBJCHANNEL *pChannel);
static NV_STATUS _memUtilsAllocCe_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel,
                                             NvHandle hClientId, NvHandle hDeviceId, NvHandle hChannelId, NvHandle hCopyObjectId);
static NV_STATUS _memUtilsAllocateUserD(OBJGPU *pGpu, MemoryManager *pMemoryManager, NvHandle hClientId,
                                        NvHandle hDeviceId, OBJCHANNEL *pChannel);
static NV_STATUS _memUtilsMapUserd_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager,
                           OBJCHANNEL *pChannel, NvHandle hClientId, NvHandle hDeviceId,
                           NvHandle hChannelId, NvBool bUseRmApiForBar1);
static NV_STATUS _memUtilsAllocateReductionSema(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel);
static NvU32 _ceChannelScheduleWork_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel,
                                          RmPhysAddr src, NV_ADDRESS_SPACE srcAddressSpace, NvU32 srcCpuCacheAttrib,
                                          RmPhysAddr dst, NV_ADDRESS_SPACE dstAddressSpace, NvU32 dstCpuCacheAttrib,
                                          NvU64 size, NvBool blocking, NvBool insertFinishPayload, NvBool bMemcopy);
static void  _ceChannelUpdateGpFifo_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel,
                                          NvU32 gpOffset,NvU32 gpSize);
static NvU32 _ceChannelPushMethodsBlock_GM107(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel,
                                              RmPhysAddr src, NV_ADDRESS_SPACE srcAddressSpace, NvU32 srcCpuCacheAttrib,
                                              RmPhysAddr dst, NV_ADDRESS_SPACE dstAddressSpace, NvU32 dstCpuCacheAttrib,
                                              NvU64 size, NvU32 **pPtr, NvBool addPayloadSema,
                                              NvBool addNonStallIntr, NvBool addFinishPayload, NvBool bMemcopy);
static NvU32 _getSpaceInPb(OBJCHANNEL *pChannel);
static NvBool _checkSynchronization(OBJGPU *pGpu, MemoryManager *pMemoryManager, OBJCHANNEL *pChannel, NvU32 block);

static NV_STATUS
_memUtilsAllocateReductionSema
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{

    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    NV_STATUS                   rmStatus;
    NvU32                       i;
    NV_STATUS                   lockStatus;
    RM_API                     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    // allocate physical memory for a bit map semaphore
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));

    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = (((pChannel->blockCount + 31)/32)*4);
    memAllocParams.attr      = 0;
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    //    memAllocParams.attr     |= NVOS32_ATTR_COHERENCY_WRITE_COMBINE;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = 0;

    //
    // When APM feature is enabled all RM internal sysmem allocations must
    // be in unprotected memory
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but those sysmem allocations that are not required to be accessed from
    // GPU should be in protected memory.
    //

    NV_ASSERT_OK_OR_RETURN(
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                pChannel->deviceId,
                                pChannel->bitMapSemPhysId,
                                NV01_MEMORY_SYSTEM,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    // allocate virtual memory for a bit map semaphore
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = (((pChannel->blockCount + 31)/32)*4);
    memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = 0;
    memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL;

    NV_ASSERT_OK_OR_RETURN(
        pRmApi->AllocWithHandle(pRmApi,
                                pChannel->hClient,
                                pChannel->deviceId,
                                pChannel->bitMapSemVirtId,
                                NV50_MEMORY_VIRTUAL,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM);
    if(lockStatus != NV_OK)
    {
        NV_ASSERT_FAILED("Could not get back lock after allocating reduction sema");
        return NV_ERR_GENERIC;
    }

    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        pRmApi->Map(pRmApi,
                    pChannel->hClient,
                    pChannel->deviceId,
                    pChannel->bitMapSemVirtId,
                    pChannel->bitMapSemPhysId, //hMemory,
                    0,
                    (((pChannel->blockCount + 31)/32)*4),
                    NV04_MAP_MEMORY_FLAGS_NONE,
                    &pChannel->pbGpuBitMapVA),
        exit_sema_creation);

    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        pRmApi->MapToCpu(pRmApi,
                         pChannel->hClient,
                         pChannel->deviceId,
                         pChannel->bitMapSemPhysId,
                         0,
                         (((pChannel->blockCount + 31)/32)*4),
                         (void **)&pChannel->pbBitMapVA,
                         0),
        exit_sema_creation);

    for(i = 0; i < (((pChannel->blockCount + 31) / 32) * 4);)
    {
        MEM_WR32((NvU8*)pChannel->pbBitMapVA + (i), 0);
        i = i + 4;
    }

    return NV_OK;
exit_sema_creation:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    NV_PRINTF(LEVEL_INFO, "end  NV_STATUS=0x%08x\n", rmStatus);
    return rmStatus;
}

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

    LOCK_ASSERT_AND_RETURN(!rmGpuLockIsOwner());
    // allocate the physical memory
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = size;
    memAllocParams.attr      = attr;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = flags;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

    //
    // When APM is enabled all RM internal allocations must to go to
    // unprotected memory irrespective of vidmem or sysmem
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (gpuIsApmFeatureEnabled(pGpu) ||
        FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
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
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = pChannel->channelNotifierSize;
    memAllocParams.attr      = attrNotifier;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = 0;
    memAllocParams.internalflags = NVOS32_ALLOC_INTERNAL_FLAGS_SKIP_SCRUB;

    //
    // When APM is enabled all RM internal allocations must to go to
    // unprotected memory irrespective of vidmem or sysmem
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (gpuIsApmFeatureEnabled(pGpu) ||
        FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
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

NV_STATUS
memmgrMemUtilsChannelInitialize_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    NV_STATUS         rmStatus;
    NV_STATUS         lockStatus;
    RsClient         *pRsClient;
    NvHandle          hClient;
    NvHandle          hDevice;
    NvHandle          hPhysMem;
    NvU64             size;
    NvHandle          hChannel;
    NvHandle          hErrNotifierVirt;
    NvHandle          hErrNotifierPhys;
    NvHandle          hPushBuffer;
    RM_API           *pRmApi              = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    Heap             *pHeap               = GPU_GET_HEAP(pGpu);
    NvBool            bMIGInUse           = IS_MIG_IN_USE(pGpu);
    NvU8             *pErrNotifierCpuVA   = NULL;
    NV_ADDRESS_SPACE  userdAddrSpace;
    NV_ADDRESS_SPACE  pushBuffAddrSpace;
    NV_ADDRESS_SPACE  gpFifoAddrSpace;
    OBJSYS           *pSys                = SYS_GET_INSTANCE();
    OBJCL            *pCl                 = SYS_GET_CL(pSys);
    NvU32             cacheSnoopFlag      = 0 ;
    NvBool            bUseRmApiForBar1    = NV_FALSE;

    //
    // Heap alloc one chunk of memory to hold all of our alloc parameters to
    // reduce stack usage
    //
    union
    {
        NV0080_ALLOC_PARAMETERS          nv0080;
        NV2080_ALLOC_PARAMETERS          nv2080;
        NVC637_ALLOCATION_PARAMETERS     nvC637;
        NV_VASPACE_ALLOCATION_PARAMETERS va;
        NV_MEMORY_ALLOCATION_PARAMS      mem;
    } *pParams = NULL;

    size             =  pChannel->channelSize;
    hPhysMem         =  pChannel->physMemId;
    hChannel         =  pChannel->channelId;
    hErrNotifierVirt =  pChannel->errNotifierIdVirt;
    hErrNotifierPhys =  pChannel->errNotifierIdPhys;
    hPushBuffer      =  pChannel->pushBufferId;

    if (pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT))
    {
        cacheSnoopFlag = DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);
    }

    if (!pChannel->bClientAllocated)
    {
        NV_CHECK_OK_OR_RETURN(
            LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                    NV01_NULL_OBJECT, NV01_ROOT,
                                    &pChannel->hClient, sizeof(pChannel->hClient)));

        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            serverGetClientUnderLock(&g_resServ, pChannel->hClient, &pRsClient),
            exit_free_client);

        pChannel->pRsClient = pRsClient;

        if (IS_VIRTUAL(pGpu))
        {
            NV_ASSERT_OK_OR_GOTO(
                rmStatus,
                clientSetHandleGenerator(pRsClient, RS_UNIQUE_HANDLE_BASE,
                                         RS_UNIQUE_HANDLE_RANGE/2 - VGPU_RESERVED_HANDLE_RANGE),
                exit_free_client);
        }
        else
        {
            NV_ASSERT_OK_OR_GOTO(
                rmStatus,
                clientSetHandleGenerator(pRsClient, 1U, ~0U - 1U),
                exit_free_client);
        }
    }
    else
        pRsClient = pChannel->pRsClient;

    hClient = pRsClient->hClient;

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        goto exit_free_client;
    }

    if (pChannel->deviceId == NV01_NULL_OBJECT)
    {
        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            clientGenResourceHandle(pRsClient, &pChannel->deviceId),
            exit_free_client);

        NV0080_ALLOC_PARAMETERS *pNv0080 = &pParams->nv0080;

        portMemSet(pNv0080, 0, sizeof(*pNv0080));
        // Which device are we?
        pNv0080->deviceId = gpuGetDeviceInstance(pGpu);
        pNv0080->hClientShare = hClient;

        NV_CHECK_OK_OR_GOTO(
            rmStatus,
            LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi, hClient, hClient, pChannel->deviceId,
                                    NV01_DEVICE_0, pNv0080, sizeof(*pNv0080)),
            exit_free_client);
    }
    hDevice = pChannel->deviceId;

    // allocate a subdevice
    if (pChannel->subdeviceId == NV01_NULL_OBJECT)
    {
        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            clientGenResourceHandle(pRsClient, &pChannel->subdeviceId),
            exit_free_client);

        NV2080_ALLOC_PARAMETERS *pNv2080 = &pParams->nv2080;
        portMemSet(pNv2080, 0, sizeof(*pNv2080));
        pNv2080->subDeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        NV_CHECK_OK_OR_GOTO(
            rmStatus,
            LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi, hClient, hDevice, pChannel->subdeviceId,
                                    NV20_SUBDEVICE_0,
                                    pNv2080,
                                    sizeof(*pNv2080)),
            exit_free_client);
    }

    // MIG support is only added for PMA scrubber
    if (bMIGInUse && (pChannel->pKernelMIGGpuInstance != NULL))
    {
        NVC637_ALLOCATION_PARAMETERS *pNvC637 = &pParams->nvC637;

        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            clientGenResourceHandle(pRsClient, &pChannel->hPartitionRef),
            exit_free_client);

        portMemSet(pNvC637, 0, sizeof(*pNvC637));
        pNvC637->swizzId = pChannel->pKernelMIGGpuInstance->swizzId;

        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            pRmApi->AllocWithHandle(pRmApi, hClient,
                                    pChannel->subdeviceId,
                                    pChannel->hPartitionRef,
                                    AMPERE_SMC_PARTITION_REF,
                                    pNvC637,
                                    sizeof(*pNvC637)),
            exit_free_client);

        pHeap = pChannel->pKernelMIGGpuInstance->pMemoryPartitionHeap;
    }

    //
    // client allocated userd only supported on volta+
    // TODO: Use property to check if client allocated userd is supported
    //
    pChannel->bClientUserd = NV_FALSE;
    if (IsVOLTAorBetter(pGpu))
    {
        NvU32 pmaConfig = 0;
        pmaConfig = PMA_QUERY_NUMA_ENABLED | PMA_QUERY_NUMA_ONLINED;
        NV_ASSERT_OK_OR_GOTO(
            rmStatus,
            pmaQueryConfigs(&pHeap->pmaObject, &pmaConfig),
            exit_free_client);
        if (pmaConfig & PMA_QUERY_NUMA_ENABLED)
        {
            if (pmaConfig & PMA_QUERY_NUMA_ONLINED)
                pChannel->bClientUserd = NV_TRUE;
            else
                pChannel->bClientUserd = NV_FALSE;
        }
        else
        {
            pChannel->bClientUserd = NV_TRUE;
        }
    }

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

            if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
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
            vaStartOffset += pGVAS->vaLimitServerRMOwned + 1;
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
            NV_CHECK_OK_OR_GOTO(
                rmStatus,
                LEVEL_ERROR,
                pRmApi->Map(pRmApi,
                            hClient,
                            pChannel->deviceId,
                            pChannel->hFbAliasVA,
                            pChannel->hFbAlias,
                            0,
                            pChannel->fbSize,
                            DRF_DEF(OS46, _FLAGS, _ACCESS,           _READ_WRITE) |
                            DRF_DEF(OS46, _FLAGS, _PAGE_SIZE,        _BIG)        |
                            DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP,      _ENABLE),
                            &pChannel->fbAliasVA),
                exit_free_client);

            NV_PRINTF(LEVEL_INFO, "Scrubber VAS :%x identity mapped with start addr: %llx, size: %llx\n",
                      pChannel->hFbAliasVA, pChannel->fbAliasVA, pChannel->fbSize);
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

    _memUtilsChannelAllocatePB_GM107(pGpu, pMemoryManager, pChannel);
    lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM);
    if(lockStatus != NV_OK)
    {
        NV_ASSERT_FAILED("Could not get back lock after allocating Push Buffer sema");
        goto exit_free_client;
    }

    // map the pushbuffer
    rmStatus = pRmApi->Map(pRmApi, hClient, hDevice,
                           hPushBuffer,
                           hPhysMem, //hMemory,
                           0,
                           size,
                           cacheSnoopFlag,
                           &pChannel->pbGpuVA);
    // map the error notifier
    rmStatus = pRmApi->Map(pRmApi, hClient, hDevice,
                           hErrNotifierVirt,
                           hErrNotifierPhys, //hMemory,
                           0,
                           pChannel->channelNotifierSize,
                           DRF_DEF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE) | cacheSnoopFlag,
                           &pChannel->pbGpuNotifierVA);

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
                                 pChannel),
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
                                    pChannel->pChannelBufferMemdesc, TRANSFER_FLAGS_USE_BAR1);
            NV_ASSERT_OR_GOTO(pChannel->pbCpuVA != NULL, exit_free_client);

            pErrNotifierCpuVA = memmgrMemDescBeginTransfer(pMemoryManager,
                                    pChannel->pErrNotifierMemdesc, TRANSFER_FLAGS_USE_BAR1);
            NV_ASSERT_OR_GOTO(pErrNotifierCpuVA != NULL, exit_free_client);
        }

        portMemSet(pChannel->pbCpuVA, 0, (NvLength)size);

        pChannel->pTokenFromNotifier =
            (NvNotification *)(pErrNotifierCpuVA +
                               (NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN *
                                sizeof(NvNotification)));
    }

    //
    // Allocate and map the doorbell region to use in scrub on free
    // Set the doorbellregister to False, since pre-volta chips doesn't support
    //
    NV_CHECK_OK_OR_GOTO(
        rmStatus,
        LEVEL_ERROR,
        memmgrScrubMapDoorbellRegion_HAL(pGpu, pMemoryManager, pChannel),
        exit_free_client);

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

static NV_STATUS _memUtilsGetCe_GM107
(
    OBJGPU *pGpu,
    NvHandle hClient,
    NvHandle hDevice,
    KernelCE **ppKCe
)
{
    KernelCE  *pKCe       = NULL;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NV_STATUS  status     = NV_OK;
    NvU32      ceInst     = 0;

    *ppKCe = NULL;

    if (IS_MIG_IN_USE(pGpu))
    {
        RsClient *pClient;
        Device *pDevice;

        NV_ASSERT_OK_OR_RETURN(
            serverGetClientUnderLock(&g_resServ, hClient, &pClient));

        NV_ASSERT_OK_OR_RETURN(
            deviceGetByHandle(pClient, hDevice, &pDevice));

        status = kmigmgrGetGPUInstanceScrubberCe(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu), pDevice, &ceInst);
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuUpdateEngineTable(pGpu));

        KCE_ITER_ALL_BEGIN(pGpu, pKCe, 0)
            if (kbusCheckEngine_HAL(pGpu, pKernelBus, ENG_CE(pKCe->publicID)) &&
               !ceIsCeGrce(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)) &&
               gpuCheckEngineTable(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)))
            {
                ceInst = kceInst;
                break;
            }
        KCE_ITER_END_OR_RETURN_ERROR
    }

    NV_ASSERT_OK_OR_RETURN(status);

    *ppKCe = GPU_GET_KCE(pGpu, ceInst);
    return status;
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
    KernelCE *pKCe = NULL;
    NVC0B5_ALLOCATION_PARAMETERS  createParams;
    RM_API                       *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    createParams.version = NVC0B5_ALLOCATION_PARAMETERS_VERSION_1;

    NV_ASSERT_OK_OR_RETURN(_memUtilsGetCe_GM107(pGpu, hClientId, hDeviceId, &pKCe));
    NV_ASSERT_OR_RETURN((pKCe != NULL), NV_ERR_INVALID_STATE);

    createParams.engineType = NV2080_ENGINE_TYPE_COPY(pKCe->publicID);
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

    pChannel->ceId = pKCe->publicID;
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
                                                   TRANSFER_FLAGS_USE_BAR1);
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
    LOCK_ASSERT_AND_RETURN(!rmGpuLockIsOwner());

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
            break;
    }

    //
    // When APM is enabled all RM internal allocations must to go to
    // unprotected memory irrespective of vidmem or sysmem
    // When Hopper CC is enabled all RM internal sysmem allocations that
    // are required to be accessed from GPU should be in unprotected memory
    // but all vidmem allocations must go to protected memory
    //
    if (gpuIsApmFeatureEnabled(pGpu) ||
        FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, memAllocParams.attr))
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
    OBJCHANNEL    *pChannel
)
{
    NV_CHANNEL_ALLOC_PARAMS channelGPFIFOAllocParams;
    NV_STATUS               rmStatus =  NV_OK;
    NvU32                   hClass;
    RM_API                 *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool                  bMIGInUse = IS_MIG_IN_USE(pGpu);
    RM_ENGINE_TYPE          engineType;
    NvU32                   flags = DRF_DEF(OS04, _FLAGS, _CHANNEL_SKIP_SCRUBBER, _TRUE);

    if (pChannel->type == SWL_SCRUBBER_CHANNEL)
    {
        engineType = RM_ENGINE_TYPE_SEC2;
        flags |= DRF_DEF(OS04, _FLAGS, _CC_SECURE, _TRUE);
    }
    else
    {
        KernelCE *pKCe = NULL;
        NV_ASSERT_OK_OR_RETURN(_memUtilsGetCe_GM107(pGpu, hClientId, hDeviceId, &pKCe));
        NV_ASSERT_OR_RETURN((pKCe != NULL), NV_ERR_INVALID_STATE);
        engineType = RM_ENGINE_TYPE_COPY(pKCe->publicID);
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

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        channelGPFIFOAllocParams.hUserdMemory[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = pChannel->hUserD;
        channelGPFIFOAllocParams.userdOffset[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = 0;
        SLI_LOOP_END
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
 * Do a Non Blocking Memeset
 *
 * @param[in]     pChannel    OBJCHANNEL pointer
 * @param[in]     base        Offset in FB
 * @param[in]     size        size to scrub
 * @param[in]     freeToken   will be returned as a semaphore
 * @param[in]     *pNumBlocks returns the number of blocks that were scrubbed
 * @returns NV_STATUS
 */
NV_STATUS
memmgrMemUtilsMemSet_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    RmPhysAddr     base,
    NvU64          size,
    NvU32          freeToken,
    NvU32         *pNumBlocks
)
{
    NvU32 blocksPushed = 0;

    if ((size % pChannel->minBlockSize) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Size should be a multiple of %d\n",
                  pChannel->minBlockSize);
        return NV_ERR_GENERIC;

    }

    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, NV_ERR_GENERIC);
    NV_ASSERT_OR_RETURN(pChannel->pControlGPFifo != NULL, NV_ERR_GENERIC);

    if (pChannel->isProgressChecked)
    {
        // if progress is checked insert the semaphore with freeToken as payload
         pChannel->finishPayload = freeToken;
         _ceChannelScheduleWork_GM107(pGpu, pMemoryManager, pChannel,
                                      0, 0, 0,             // src parameters
                                      base, ADDR_FBMEM, 0, // dst parameters
                                      size,
                                      NV_FALSE,            // blocking
                                      NV_TRUE,             // insertFinishPayload
                                      NV_FALSE);           // memcopy
    }
    else
    {
        // issue a standard async scrub
       blocksPushed = _ceChannelScheduleWork_GM107(pGpu, pMemoryManager, pChannel,
                          0, 0, 0,             // src parameters
                          base, ADDR_FBMEM, 0, // dst parameters
                          size,
                          NV_FALSE,            // blocking
                          NV_FALSE,            // insertFinishPayload
                          NV_FALSE);           // memcopy
    }
    *pNumBlocks = blocksPushed;
    return NV_OK;
}

/*!
 * Do a Blocking Memset
 *
 * @param[in]     pChannel   OBJCHANNEL pointer
 * @param[in]     base       Offset in FB
 * @param[in]     size       size to scrub
 * @returns NV_STATUS
 */

NV_STATUS
memmgrMemUtilsMemSetBlocking_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    RmPhysAddr     base,
    NvU64          size
)
{
    NvU32 blocksPushed = 0;

    if((size % pChannel->minBlockSize) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Size should be a multiple of %d\n",
                  pChannel->minBlockSize);
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;

    }

    NV_ASSERT_OR_RETURN(pChannel->pControlGPFifo != NULL, NV_ERR_GENERIC);
    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, NV_ERR_GENERIC);

    blocksPushed = _ceChannelScheduleWork_GM107(pGpu, pMemoryManager, pChannel,
                       0, 0, 0,              // src parameters
                       base, ADDR_FBMEM, 0,  // dst parameters
                       size,
                       NV_TRUE,              // blocking
                       NV_FALSE,             // insertFinishPayload
                       NV_FALSE);            // memcopy

    if (blocksPushed > 0)
    {
        NvU8     *semAddr       = pChannel->pbCpuVA + pChannel->semaOffset;
        NV_STATUS timeoutStatus = NV_OK;
        RMTIMEOUT timeout;

        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        while (MEM_RD32(semAddr) != pChannel->lastPayloadPushed)
        {
            NV_PRINTF(LEVEL_INFO, "Semaphore Payload is 0x%x last is 0x%x\n",
                      MEM_RD32(semAddr), pChannel->lastPayloadPushed);

            if (timeoutStatus == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Timed Out waiting for CE semaphore\n");

                NV_PRINTF(LEVEL_ERROR,
                          "GET=0x%x, PUT=0x%x, GPGET=0x%x, GPPUT=0x%x\n",
                          pChannel->pControlGPFifo->Get,
                          pChannel->pControlGPFifo->Put,
                          pChannel->pControlGPFifo->GPGet,
                          pChannel->pControlGPFifo->GPPut);

                DBG_BREAKPOINT_REASON(NV_ERR_TIMEOUT);
                return NV_ERR_GENERIC;
            }

            timeoutStatus = gpuCheckTimeout(pGpu, &timeout);
         }
    }

    return NV_OK;
}

/*!
 * This function allocates the ECC scrubber
 *
 * @param[in]     pChannel   OBJCHANNEL pointer
 * @returns Bool
 */
NV_STATUS
memmgrMemUtilsAllocateEccScrubber_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, pChannel);

    memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, pChannel);

    _memUtilsAllocateReductionSema(pGpu, pMemoryManager, pChannel);

    return NV_OK;
}

/*!
 * This function allocates the ecc scrubber and the
 * DUpes the bitmap semaphore which is used for sync
 *
 * @param[in]     pChannel   OBJCHANNEL pointer
 * @returns Bool
 */
NV_STATUS
memmgrMemUtilsAllocateEccAllocScrubber_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel
)
{
    OBJSCRUB                   *pEccTD           = &pMemoryManager->eccScrubberState;
    OBJCHANNEL                 *pEccSyncChannel  = &pEccTD->allocationScrubberState;
    OBJCHANNEL                 *pEccAsyncChannel = &pEccTD->tdHeapState;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    NV_STATUS                   lockStatus;
    RM_API                     *pRmApi           = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, pEccSyncChannel);
    memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, pEccSyncChannel);

    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    // dup the reduction sema bit map object
    NV_ASSERT_OK(
        pRmApi->DupObject(pRmApi,
                          pEccSyncChannel->hClient,
                          pEccSyncChannel->deviceId,
                          &pEccSyncChannel->bitMapSemPhysId,
                          pEccAsyncChannel->hClient,
                          pEccAsyncChannel->bitMapSemPhysId,
                          0));

    // allocate virtual memory for a bit map semaphore
    portMemSet(&memAllocParams, 0, sizeof(memAllocParams));
    memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
    memAllocParams.type      = NVOS32_TYPE_IMAGE;
    memAllocParams.size      = (((pEccSyncChannel->blockCount + 31)/32)*4);
    memAllocParams.attr      = NVOS32_ATTR_NONE;
    memAllocParams.attr2     = NVOS32_ATTR2_NONE;
    memAllocParams.flags     = 0;
    memAllocParams.flags    |= NVOS32_ALLOC_FLAGS_VIRTUAL;

    NV_ASSERT_OK(
        pRmApi->AllocWithHandle(pRmApi,
                                pEccSyncChannel->hClient,
                                pEccSyncChannel->deviceId,
                                pEccSyncChannel->bitMapSemVirtId,
                                NV50_MEMORY_VIRTUAL,
                                &memAllocParams,
                                sizeof(memAllocParams)));

    lockStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM);

    if (lockStatus != NV_OK)
    {
        NV_ASSERT_FAILED("Could not get back lock after allocating reduction sema");
        return NV_ERR_GENERIC;
    }

    NV_ASSERT_OK(
        pRmApi->Map(pRmApi,
                    pEccSyncChannel->hClient,
                    pEccSyncChannel->deviceId,
                    pEccSyncChannel->bitMapSemVirtId,
                    pEccSyncChannel->bitMapSemPhysId, //hMemory,
                    0,
                    (((pEccSyncChannel->blockCount + 31) / 32) * 4),
                    NV04_MAP_MEMORY_FLAGS_NONE,
                    &pEccSyncChannel->pbGpuBitMapVA));

    pEccSyncChannel->pbBitMapVA = pEccAsyncChannel->pbBitMapVA;

    return NV_OK;
}

/*!
 * FUnction calculates the available space in PB
 * This is based on the reading the semaphore that
 * has the previous PUT pointer where methods were
 * inserted
 *
 * @param[in]     pChannel  OBJCHANNEL pointer
 * @returns size
 */
static NvU32
_getSpaceInPb(OBJCHANNEL *pChannel)
{
    NvU32 filledSpace;
    NvU32 avlblSpace;

    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, 0);

    if (pChannel->channelPutOffset >= MEM_RD32((NvU8*)pChannel->pbCpuVA + pChannel->semaOffset))
    {
        filledSpace = (pChannel->channelPutOffset - MEM_RD32((NvU8*)pChannel->pbCpuVA + pChannel->semaOffset));
        avlblSpace = pChannel->channelPbSize - filledSpace;

    }
    else
    {
        avlblSpace = (MEM_RD32((NvU8*)pChannel->pbCpuVA + pChannel->semaOffset) - pChannel->channelPutOffset);
    }

    NV_PRINTF(LEVEL_INFO, "Space in PB is %d\n", avlblSpace);

    return avlblSpace;

}

/*!
 * This function manages the PushBUffer
 * It will insert methods into the PB, manage wrap around
 * and decide when we need to add NON-STALL interrupts
 * and etra token semaphores
 *
 * @param[in]     pChannel          OBJCHANNEL pointer
 * @param[in]     src               Offset of src to copy from
 * @param[in]     srcAddressSpace   source surface address space type
 * @param[in]     srcCpuCacheAttrib source surface address space attributes
 * @param[in]     dst               Offset of dst to scrub/copy to
 * @param[in]     dstAddressSpace   destination surface address space type
 * @param[in]     dstCpuCacheAttrib destination surface address space attributes
 * @param[in]     size              size to scrub/copy
 * @param[in]     blocking          blocking will not insert non-stall
 * @param[in]     payload           will insert a token for the last block submitted
 * @param[in]     bMemcopy          NV_TRUE for memory copy / NV_FALSE for scrubbing
 * @returns Bool
 */
static NvU32
_ceChannelScheduleWork_GM107
(
    OBJGPU          *pGpu,
    MemoryManager   *pMemoryManager,
    OBJCHANNEL      *pChannel,
    RmPhysAddr       src,
    NV_ADDRESS_SPACE srcAddressSpace,
    NvU32            srcCpuCacheAttrib,
    RmPhysAddr       dst,
    NV_ADDRESS_SPACE dstAddressSpace,
    NvU32            dstCpuCacheAttrib,
    NvU64            size,
    NvBool           blocking,
    NvBool           insertFinishPayload,
    NvBool           bMemcopy
)
{
    RMTIMEOUT        timeout;
    NvU32            spaceInPb;
    NvU32            numBytes;
    NvU32            bytesPushed;
    NvU32            *ptr;
    NvU32            gpBase;
    NvU32            semaCount = 0;
    NvBool           addNonStallIntr = NV_FALSE;
    NvU32            blocksPushed = 0;
    NvBool           addFinishPayload;
    NvU32            blockSize = 0;

    NV_ASSERT_OR_RETURN(pChannel->pbCpuVA != NULL, 0);
    NV_ASSERT_OR_RETURN(pChannel->pControlGPFifo != NULL, 0);

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    spaceInPb = _getSpaceInPb(pChannel);

    NV_PRINTF(LEVEL_INFO, "Space in PB is %d and starting fill at 0x%x\n",
              spaceInPb, pChannel->channelPutOffset);

    ptr = (NvU32 *)(pChannel->pbCpuVA + pChannel->channelPutOffset);
    gpBase = pChannel->channelPutOffset;
    numBytes = 0;
    do
    {
        // while we have space greater than one block
        while((spaceInPb > (pChannel->methodSizePerBlock+MAX_EXTRA_PAYLOAD)))
        {
            // if inserting one more block is greater than PB size then wrap around to the beginning
            if((pChannel->channelPutOffset + (pChannel->methodSizePerBlock+MAX_EXTRA_PAYLOAD)) > pChannel->channelPbSize)
            {
                NV_PRINTF(LEVEL_INFO, "Wrap numBytes %d\n", numBytes);
                //submit to gpfifo with numBytes and wrap around the PutOffset
                if(numBytes > 0)
                {
                    _ceChannelUpdateGpFifo_GM107(pGpu, pMemoryManager, pChannel, (gpBase), numBytes);
                }
                pChannel->channelPutOffset = 0;
                ptr = (NvU32 *)(pChannel->pbCpuVA + pChannel->channelPutOffset);
                gpBase = 0;
                numBytes = 0;
                bytesPushed = 0;
                // update the available space
                spaceInPb = _getSpaceInPb(pChannel);
                NV_PRINTF(LEVEL_INFO, "Wrapping PB around\n");
                continue;
            }

            blockSize  = (size > pChannel->maxBlockSize) ?
                         pChannel->maxBlockSize : (NvU32) size;

            // add a non-stall interupt every (8th of the size) or when we insert the last block
            if((semaCount > (pChannel->channelPbSize >> 3)) || (size <= pChannel->maxBlockSize))
            {
                addNonStallIntr = NV_TRUE;
                semaCount = 0;
            }
            else
            {
                addNonStallIntr = NV_FALSE;
            }
            // the finsh payload corresponds to inserting a token for every call to scrub that finishes
            if((insertFinishPayload) && (size <= pChannel->maxBlockSize))
            {
                addFinishPayload = NV_TRUE;
                NV_PRINTF(LEVEL_INFO, "Inserting Finish Payload!!!!!!!!!!\n");
            }
            else
            {
                addFinishPayload = NV_FALSE;
            }
            if(_checkSynchronization(pGpu, pMemoryManager, pChannel, BLOCK_INDEX_FROM_ADDR(dst, pChannel->blockShift)))
            {
                bytesPushed = _ceChannelPushMethodsBlock_GM107(pGpu, pMemoryManager, pChannel,
                    src, srcAddressSpace, srcCpuCacheAttrib, // src parameters
                    dst, dstAddressSpace, dstCpuCacheAttrib, // dst parameters
                    blockSize, &ptr, NV_TRUE, (addNonStallIntr && !blocking),
                    addFinishPayload, bMemcopy);
                spaceInPb = spaceInPb - bytesPushed;
                numBytes  = numBytes + bytesPushed;
                semaCount = semaCount + bytesPushed;
                blocksPushed++;
                // we are done pushing all methods
            }

            dst += (NvU64) blockSize;
            if (bMemcopy)
                src += (NvU64) blockSize;
            size -= (NvU64) blockSize;

            if(size == 0)
            {
                _ceChannelUpdateGpFifo_GM107(pGpu, pMemoryManager, pChannel, gpBase, numBytes);
                return blocksPushed;
            }
        }
        spaceInPb = _getSpaceInPb(pChannel);
        if(spaceInPb <= (pChannel->methodSizePerBlock + MAX_EXTRA_PAYLOAD))
        {
            //no space in pb to push all blocks so put what we have and wait for space
            if(numBytes > 0)
            {
                _ceChannelUpdateGpFifo_GM107(pGpu, pMemoryManager, pChannel, gpBase, numBytes);
            }
            gpBase = pChannel->channelPutOffset;
            numBytes = 0;
        }
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_ASSERT_FAILED("Timed out waiting for Space in PB!");
            return NV_ERR_GENERIC;
        }
    } while(1);
}


/*!
 * This function checks if the block has already been submitted
 * or scrubbed based on 2 bitmaps. One is a pending bitmap
 * updated by the CPU and one is a "Finished" bitmap updated by
 * the GPU
 *
 * @param[in]     pChannel   OBJCHANNEL pointer
 * @param[in]     block      block number
 *
 * @returns Bool
 */
static NvBool
_checkSynchronization
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvU32          block
)
{
    NvU32 blockSema;

    if (!pChannel->isChannelSynchronized)
    {
        //synchronization is not required for this channel
        return NV_TRUE;
    }

    blockSema = MEM_RD32((NvU8*)pChannel->pbBitMapVA + ((block/32)*4));

    if( ((blockSema) & (1 << (block%32))) == 0 )
    {
        if (((pChannel->pBlockPendingState[block / 32] & (1 << (block % 32))) == 0) &&
               ((pChannel->pBlockDoneState[block / 32] & (1 << (block % 32))) == 0) )
        {
            pChannel->pBlockPendingState[block / 32] |= (1 << (block % 32));
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * Updates the GPfifo with the methods in the PB for
 * the given channel
 * @param[in]     pChannel   OBJCHANNEL pointer
 * @param[in]     gpOffset   Offset in the PB
 * @param[in]     gpSize     Size of segment
 * @returns None
 */
static void
_ceChannelUpdateGpFifo_GM107
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager,
    OBJCHANNEL    *pChannel,
    NvU32          gpOffset,
    NvU32          gpSize

)
{
    RMTIMEOUT        timeout;
    NvU32            GPPut;
    NvU32            GPGet;
    NvU64            get;
    NvU32            length;
    NvU32            *pGpEntry;
    NvU32            GpEntry0;
    NvU32            GpEntry1;
    NvU32            GPPutNext;
    NvU32            workSubmitToken = 0;
    KernelChannel   *pFifoKernelChannel;
    KernelFifo      *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    NV_ASSERT_OR_RETURN_VOID(pChannel->pbCpuVA != NULL);
    NV_ASSERT_OR_RETURN_VOID(pChannel->pControlGPFifo != NULL);

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    GPPut = MEM_RD32(&pChannel->pControlGPFifo->GPPut);
    GPGet = MEM_RD32(&pChannel->pControlGPFifo->GPGet);

    GPPutNext = (GPPut + 1) % pChannel->channelNumGpFifioEntries;

    NV_PRINTF(LEVEL_INFO, "Put %d Get %d PutNext%d\n", GPPut, GPGet,
              GPPutNext);

    NV_PRINTF(LEVEL_INFO, "gp Base 0x%x, Size %d\n", (NvU32)(gpOffset),
              gpSize);

    // if the size passed is zero do not update gpput
    if (gpSize == 0)
        return;

    if (GPPut >= pChannel->channelNumGpFifioEntries)
    {
        // if the Put pointer is invalid, the GPU is likely inaccessible
        NV_PRINTF(LEVEL_INFO, "invalid Put %u >= %u\n", GPPut,
                  pChannel->channelNumGpFifioEntries);
        return;
    }

    while (GPPutNext == GPGet)
    {
        // need to wait for space
        GPGet = MEM_RD32(&pChannel->pControlGPFifo->GPGet);

        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_ASSERT_FAILED("Timed Out waiting for space in GPFIFIO!");
            return;
        }
        else if (GPGet >= pChannel->channelNumGpFifioEntries)
        {
            // if the Get pointer is invalid, the GPU is likely inaccessible
            NV_PRINTF(LEVEL_INFO, "invalid Get %u >= %u\n", GPGet,
                      pChannel->channelNumGpFifioEntries);
            return;
        }
    }

    get = pChannel->pbGpuVA + gpOffset;
    length = gpSize;

    GpEntry0 =
       DRF_DEF(906F, _GP_ENTRY0, _NO_CONTEXT_SWITCH, _FALSE) |
       DRF_NUM(906F, _GP_ENTRY0, _GET, NvU64_LO32(get) >> 2);
    GpEntry1 =
       DRF_NUM(906F, _GP_ENTRY1, _GET_HI, NvU64_HI32(get)) |
       DRF_NUM(906F, _GP_ENTRY1, _LENGTH, length >> 2) |
       DRF_DEF(906F, _GP_ENTRY1, _PRIV, _USER) |
       DRF_DEF(906F, _GP_ENTRY1, _LEVEL, _MAIN);


    pGpEntry = (NvU32 *)(((NvU8*)pChannel->pbCpuVA) + pChannel->channelPbSize +
        GPPut*NV906F_GP_ENTRY__SIZE);

    MEM_WR32(&pGpEntry[0], GpEntry0);
    MEM_WR32(&pGpEntry[1], GpEntry1);

    // need to flush WRC buffer
    osFlushCpuWriteCombineBuffer();

    // write gpput
    MEM_WR32(&pChannel->pControlGPFifo->GPPut, GPPutNext);
    osFlushCpuWriteCombineBuffer();

    if (kfifoIsLiteModeEnabled_HAL(pGpu, pKernelFifo))
    {
        NV_ASSERT_OR_RETURN_VOID(0);
    }
    else
    {
        workSubmitToken = pChannel->workSubmitToken;
        NV_ASSERT_OR_RETURN_VOID(CliGetKernelChannelWithDevice(pChannel->pRsClient,
                                 pChannel->deviceId, pChannel->channelId,
                                 &pFifoKernelChannel) == NV_OK);
    }
    if (!kchannelIsRunlistSet(pGpu, pFifoKernelChannel))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FAILED Channel 0x%x is not assigned to runlist yet\n",
                  kchannelGetDebugTag(pFifoKernelChannel));
        return;
    }
    // update doorbell register
    kfifoUpdateUsermodeDoorbell_HAL(pGpu, pKernelFifo, workSubmitToken, kchannelGetRunlistId(pFifoKernelChannel));
}

/*!
 * Inserts methods into the push buffer for one block
 *
 * @param[in]     pChannel          OBJCHANNEL pointer
 * @param[in]     src               Offset of src to copy from
 * @param[in]     srcAddressSpace   source surface address space type
 * @param[in]     srcCpuCacheAttrib source surface address space attributes
 * @param[in]     dst               Offset of dst to scrub/copy to
 * @param[in]     dstAddressSpace   destination surface address space type
 * @param[in]     dstCpuCacheAttrib destination surface address space attributes
 * @param[in]     pPtr              Double pointer to PB offset
 * @returns None
 */
static void
_ceChannelPushMethodAperture_GM107
(
    OBJCHANNEL      *pChannel,
    NV_ADDRESS_SPACE srcAddressSpace,
    NvU32            srcCpuCacheAttrib,
    NV_ADDRESS_SPACE dstAddressSpace,
    NvU32            dstCpuCacheAttrib,
    NvU32          **pPtr
)
{
    NvU32 *ptr  = *pPtr;
    NvU32  data = 0;

    // Set source parameters
    data = ((srcAddressSpace == ADDR_FBMEM) ? DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _LOCAL_FB) :
        (srcCpuCacheAttrib == NV_MEMORY_CACHED) ? DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _COHERENT_SYSMEM) :
            DRF_DEF(B0B5, _SET_SRC_PHYS_MODE, _TARGET, _NONCOHERENT_SYSMEM));

    PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_SET_SRC_PHYS_MODE, data);

    // Set destination parameters
    data = ((dstAddressSpace == ADDR_FBMEM) ? DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _LOCAL_FB) :
        (dstCpuCacheAttrib == NV_MEMORY_CACHED) ? DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _COHERENT_SYSMEM) :
            DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _NONCOHERENT_SYSMEM));

    PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_SET_DST_PHYS_MODE, data);

    *pPtr = ptr;
}

/*!
 * Inserts methods into the push buffer for one block
 *
 * @param[in]     pChannel          OBJCHANNEL pointer
 * @param[in]     src               Offset of src to copy from
 * @param[in]     srcAddressSpace   source surface address space type
 * @param[in]     srcCpuCacheAttrib source surface address space attributes
 * @param[in]     dst               Offset of dst to scrub/copy to
 * @param[in]     dstAddressSpace   destination surface address space type
 * @param[in]     dstCpuCacheAttrib destination surface address space attributes
 * @param[in]     size              size of the region to scrub/copy
 * @param[in]     pPtr              Double pointer to PB offset
 * @param[in]     addPayloadSema    Bool to add default payload
 * @param[in]     addNonStallInt    Bool to add a non stall at the end
 * @param[in]     addFinishPayload  Bool to add an extra sema release for token
 * @param[in]     bMemcopy          NV_TRUE for memcopy / NV_FALSE for scrubbing
 * @returns None
 */
static NvU32
_ceChannelPushMethodsBlock_GM107
(
    OBJGPU          *pGpu,
    MemoryManager   *pMemoryManager,
    OBJCHANNEL      *channel,
    RmPhysAddr       src,
    NV_ADDRESS_SPACE srcAddressSpace,
    NvU32            srcCpuCacheAttrib,
    RmPhysAddr       dst,
    NV_ADDRESS_SPACE dstAddressSpace,
    NvU32            dstCpuCacheAttrib,
    NvU64            size,
    NvU32          **pPtr,
    NvBool           addPayloadSema,
    NvBool           addNonStallIntr,
    NvBool           addFinishPayload,
    NvBool           bMemcopy
)
{
    NvU32  launchParams       = 0;
    NvU32 *ptr                = *pPtr;
    NvU32 *pStartPtr          = ptr;
    NvBool addReductionOp     = channel->isChannelSynchronized;
    NvBool bMemoryScrubEnable = NV_FALSE;
    NvU32  remapConstB        = 0;
    NvU32  remapComponentSize = 0;

    NV_PRINTF(LEVEL_INFO, "Base = 0x%llx, Size = 0x%llx, PB location = %p\n",
              dst, size, ptr);

    PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NV906F_SET_OBJECT, channel->classEngineID);

    if (size > 0)
    {
        NvU32 payLoad = channel->channelPutOffset + channel->methodSizePerBlock;

        if (addNonStallIntr)  payLoad = payLoad + NONSTALL_METHOD_SIZE;
        if (addReductionOp)   payLoad = payLoad + SEMAPHORE_ONLY_METHOD_SIZE;
        if (addFinishPayload) payLoad = payLoad + SEMAPHORE_ONLY_METHOD_SIZE;

        if (addPayloadSema)
        {
            memmgrChannelPushSemaphoreMethodsBlock_HAL(pMemoryManager,
                NVA06F_SUBCHANNEL_COPY_ENGINE,
                channel->pbGpuVA+channel->semaOffset, payLoad, &ptr);

            NV_PRINTF(LEVEL_INFO, "Pushing Semaphore Payload 0x%x\n", payLoad);
            channel->lastPayloadPushed = payLoad;
        }

        if (IS_SIMULATION(pGpu))
        {
            //
            // fmodel CE is slow (compared to emulation) so we don't bother
            // scrubbing the whole block. Fmodel already scrubs memory via ramif
            // so we'll never get exceptions
            //
            size = NV_MIN(size, 0x20);
        }

        memmgrChannelPushAddressMethodsBlock_HAL(pMemoryManager, NV_FALSE,
            NVA06F_SUBCHANNEL_COPY_ENGINE, dst, &ptr);

        if (bMemcopy)
        {
            memmgrChannelPushAddressMethodsBlock_HAL(pMemoryManager, NV_TRUE,
                NVA06F_SUBCHANNEL_COPY_ENGINE, src, &ptr);

            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LINE_LENGTH_IN, NvU64_LO32(size));
        }
        else
        {
            bMemoryScrubEnable = memmgrMemUtilsCheckMemoryFastScrubEnable_HAL(pGpu,
                                                   pMemoryManager,
                                                   channel->hTdCopyClass,
                                                   channel->bUseVasForCeCopy,
                                                   dst,
                                                   NvU64_LO32(size),
                                                   dstAddressSpace);
            if (bMemoryScrubEnable)
            {
                NV_PRINTF(LEVEL_INFO, "Using Fast memory scrubber\n");
                remapConstB        = DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_B);
                PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_SET_REMAP_CONST_B, 0x00000000);

                remapComponentSize = DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, _ONE);
                PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LINE_LENGTH_IN, NvU64_LO32(size));
            }
            else
            {
                remapComponentSize = DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, _FOUR);
                PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LINE_LENGTH_IN, NvU64_LO32(size >> 2));
            }

            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_SET_REMAP_COMPONENTS,
                                            DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_A)              |
                                            DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _NUM_SRC_COMPONENTS, _ONE)     |
                                            DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _NUM_DST_COMPONENTS, _ONE)     |
                                            remapComponentSize                                                  |
                                            remapConstB);

            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_SET_REMAP_CONST_A, 0x00000000);

            NV_ASSERT(srcAddressSpace == 0);
            NV_ASSERT(dstAddressSpace == ADDR_FBMEM);

            srcAddressSpace = ADDR_FBMEM;
        }

        PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LINE_COUNT, 1);

        _ceChannelPushMethodAperture_GM107(channel, srcAddressSpace, srcCpuCacheAttrib, dstAddressSpace, dstCpuCacheAttrib, &ptr);

        launchParams = DRF_DEF(B0B5, _LAUNCH_DMA, _INTERRUPT_TYPE, _NONE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_TYPE, _PHYSICAL) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _DST_TYPE, _PHYSICAL) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PIPELINED);

        if (addPayloadSema)
        {
            launchParams |= DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE) |
                            DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE);
        }
        else
        {
            launchParams |= DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _NONE);
        }

        if (bMemoryScrubEnable)
        {
            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVC8B5_SET_MEMORY_SCRUB_PARAMETERS,
                          DRF_DEF(C8B5, _SET_MEMORY_SCRUB_PARAMETERS, _DISCARDABLE, _FALSE));

            launchParams |= DRF_DEF(C8B5, _LAUNCH_DMA, _MEMORY_SCRUB_ENABLE, _TRUE);
            launchParams |= DRF_DEF(C8B5, _LAUNCH_DMA, _REMAP_ENABLE, _FALSE);

            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVC8B5_LAUNCH_DMA, launchParams);
        }
        else
        {
            if (!bMemcopy)
            {
                launchParams |= DRF_DEF(B0B5, _LAUNCH_DMA, _REMAP_ENABLE, _TRUE);
            }

            PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LAUNCH_DMA, launchParams);
        }
    }

    if (addReductionOp)
    {
        NvU32 currentBlock = BLOCK_INDEX_FROM_ADDR((dst), channel->blockShift);
        NvU32 blockOffset;
        NvU32 bitFlip;

        blockOffset = (currentBlock / 32) * 4;
        bitFlip     = ((NvU32)1 << (currentBlock % 32));
        memmgrChannelPushSemaphoreMethodsBlock_HAL(pMemoryManager,
            NVA06F_SUBCHANNEL_COPY_ENGINE,
            channel->pbGpuBitMapVA+(blockOffset), bitFlip, &ptr);

        launchParams = DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _INTERRUPT_TYPE, _NONE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _REMAP_ENABLE, _TRUE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_REDUCTION_ENABLE, _TRUE) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_REDUCTION_SIGN, _UNSIGNED) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_REDUCTION, _IOR) |
                       DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _NONE);
        // push only the second semaphore release
        PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LAUNCH_DMA, launchParams);
    }

    if (addFinishPayload)
    {
        memmgrChannelPushSemaphoreMethodsBlock_HAL(pMemoryManager,
                NVA06F_SUBCHANNEL_COPY_ENGINE,
                channel->pbGpuVA+channel->finishPayloadOffset,
                channel->finishPayload, &ptr);

        launchParams =  DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _INTERRUPT_TYPE, _NONE) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _REMAP_ENABLE, _TRUE) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH) |
                        DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _NONE);
        PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NVB0B5_LAUNCH_DMA, launchParams);
        NV_PRINTF(LEVEL_INFO, "Pushing Finishing Semaphore Payload 0x%x\n",
                  channel->finishPayload);
    }

    if (addNonStallIntr)
    {
        PUSH_PAIR(NVA06F_SUBCHANNEL_COPY_ENGINE, NV906F_NON_STALL_INTERRUPT, 0);
    }

    channel->channelPutOffset = (NvU32)((NvU8 *)ptr - (NvU8 *)channel->pbCpuVA);
    *pPtr = ptr;

    // return length of methods inserted
    return (NvU32)((NvU8*)ptr - (NvU8*)pStartPtr);
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
