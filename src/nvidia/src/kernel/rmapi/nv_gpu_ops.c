/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/prelude.h"


#include <class/cl0002.h>
#include <class/cl0005.h>
#include <class/cl003e.h> // NV01_MEMORY_SYSTEM
#include <class/cl0040.h> // NV01_MEMORY_LOCAL_USER
#include <class/cl0080.h>
#include <class/cl503b.h>
#include <class/cl50a0.h> // NV50_MEMORY_VIRTUAL
#include <class/cl90e6.h>
#include <class/cl90f1.h>
#include <class/cla06f.h>
#include <class/clb069.h>
#include <class/clb069sw.h>
#include <class/clb06f.h>
#include <class/clb0b5.h>
#include <class/clb0b5sw.h>
#include <class/clb0c0.h>
#include <class/clb1c0.h>
#include <class/clc06f.h>
#include <class/clc076.h>
#include <class/clc0b5.h>
#include <class/clc0c0.h>
#include <class/clc1b5.h>
#include <class/clc1c0.h>
#include <class/clc361.h>
#include <class/clc365.h>
#include <class/clc369.h>
#include <class/clc36f.h>
#include <class/clc3b5.h>
#include <class/clc3c0.h>
#include <class/clc46f.h>
#include <class/clc4c0.h>
#include <class/clc56f.h>
#include <class/clc572.h> // PHYSICAL_CHANNEL_GPFIFO
#include <class/clc574.h> // UVM_CHANNEL_RETAINER
#include <class/clc5b5.h>
#include <class/clc5c0.h>
#include <class/clc637.h>
#include <class/clc6b5.h>
#include <class/clc6c0.h>
#include <class/clc7b5.h>
#include <class/clc7c0.h>

#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl0000/ctrl0000system.h>
#include <ctrl/ctrl0080/ctrl0080fifo.h>
#include <ctrl/ctrl0080/ctrl0080gpu.h>
#include <ctrl/ctrl2080/ctrl2080fb.h>
#include <ctrl/ctrl2080/ctrl2080fifo.h>
#include <ctrl/ctrl2080/ctrl2080gpu.h>
#include <ctrl/ctrl2080/ctrl2080gr.h>
#include <ctrl/ctrl90e6.h>
#include <ctrl/ctrl90f1.h>
#include <ctrl/ctrla06f.h>
#include <ctrl/ctrlb069.h>
#include <ctrl/ctrlc365.h>
#include <ctrl/ctrlc369.h>
#include <ctrl/ctrlc36f.h>

#include <ampere/ga100/dev_runlist.h>
#include <containers/queue.h>
#include <core/locks.h>
#include <gpu/bus/kern_bus.h>
#include <gpu/device/device.h>
#include <gpu/gpu.h>
#include <gpu/mem_mgr/heap.h>
#include <gpu/mem_mgr/mem_mgr.h>
#include <gpu/mem_mgr/virt_mem_allocator.h>
#include <gpu/mem_sys/kern_mem_sys.h>
#include <gpu/mmu/kern_gmmu.h>
#include <gpu/subdevice/subdevice.h>
#include <gpu_mgr/gpu_mgr.h>
#include <kepler/gk104/dev_timer.h>
#include <kernel/gpu/fifo/kernel_channel.h>
#include <kernel/gpu/fifo/kernel_channel_group.h>
#include <kernel/gpu/fifo/kernel_channel_group_api.h>
#include <kernel/gpu/fifo/kernel_ctxshare.h>
#include <kernel/gpu/gr/kernel_graphics.h>
#include <kernel/gpu/mig_mgr/gpu_instance_subscription.h>
#include <kernel/gpu/mig_mgr/kernel_mig_manager.h>
#include <kernel/gpu/nvlink/kernel_nvlink.h>
#include <mem_mgr/fabric_vaspace.h>
#include <mem_mgr/fla_mem.h>
#include <mem_mgr/gpu_vaspace.h>
#include <mem_mgr/vaspace.h>
#include <mmu/gmmu_fmt.h>
#include <nv_uvm_types.h>
#include <objrpc.h>
#include <os/os.h>
#include <resserv/rs_client.h>
#include <rmapi/client.h>
#include <rmapi/nv_gpu_ops.h>
#include <rmapi/rs_utils.h>
#include <turing/tu102/dev_vm.h>
#include <gpu/mem_mgr/vaspace_api.h>
#include <vgpu/rpc.h>

#include <pascal/gp100/dev_mmu.h>


#define NV_GPU_OPS_NUM_GPFIFO_ENTRIES_DEFAULT 1024
#define NV_GPU_SMALL_PAGESIZE (4 * 1024)

#define PAGE_SIZE_DEFAULT UVM_PAGE_SIZE_DEFAULT

typedef struct
{
    NODE btreeNode;
    NvU64 address;
    NvHandle handle;
    NvU64 size;
    // childHandle tightly couples a physical allocation with a VA memdesc.
    // A VA memsdesc is considered as a parent memdesc i.e. childHandle will be non-zero (valid).
    //    - If childHandle is non-zero,there is a corresponding PA allocation present.
    //    - If childHandle is zero, this is an invalid state for a VA memdesc.
    NvHandle childHandle;
} gpuMemDesc;

typedef struct
{
    NvU64    vaStart;                    // Needs to be alinged to pagesize
    NvBool   bFixedAddressAllocate;      // rangeBegin & rangeEnd both included
    NvU32    pageSize;                   // default is 4k or 64k else use pagesize= 2M
} gpuVaAllocInfo;

typedef struct
{
    NODE btreeNode;
    NvU64 cpuPointer;
    NvHandle handle;
} cpuMappingDesc;

typedef struct
{
    NODE btreeNode;
    PORT_RWLOCK *btreeLock;
    NvHandle deviceHandle;
    PNODE subDevices;
    NvU32 subDeviceCount;
    NvU32 arch;
    NvU32 implementation;
} deviceDesc;

typedef struct
{
    NODE btreeNode;
    NvHandle subDeviceHandle;
    NvU64 refCount;
    struct
    {
        NvHandle                 handle;

        // Pointer to the SMC partition information. It is used as a flag to
        // indicate that the SMC information has been initialized.
        KERNEL_MIG_GPU_INSTANCE *info;
    } smcPartition;
    NvU32              eccOffset;
    NvU32              eccMask;
    void               *eccReadLocation;
    NvHandle           eccMasterHandle;
    NvHandle           eccCallbackHandle;
    NvBool             bEccInitialized;
    NvBool             bEccEnabled;
    NvBool             eccErrorNotifier;
    NVOS10_EVENT_KERNEL_CALLBACK_EX eccDbeCallback;

    // The below is used for controlling channel(s) in the GPU.
    // Example: Volta maps the doorbell work submission register in this
    // region.
    NvHandle           clientRegionHandle;
    volatile void      *clientRegionMapping;
} subDeviceDesc;

struct gpuSession
{
    NvHandle handle;
    PNODE devices;
    PORT_RWLOCK *btreeLock;
};


MAKE_MAP(MemdescMap, PMEMORY_DESCRIPTOR);

struct gpuDevice
{
    deviceDesc         *rmDevice;
    subDeviceDesc      *rmSubDevice;

    // same as rmDevice->deviceHandle
    NvHandle           handle;

    // same as rmSubDevice->subDeviceHandle
    NvHandle           subhandle;

    NvU32              deviceInstance;
    NvU32              subdeviceInstance;
    NvU32              gpuId;
    NvU32              hostClass;
    NvU32              ceClass;
    NvU32              sec2Class;
    NvU32              computeClass;
    NvU32              faultBufferClass;
    NvU32              accessCounterBufferClass;
    NvBool             isTccMode;
    NvBool             isWddmMode;
    struct gpuSession  *session;
    NvU8               gpuUUID[NV_GPU_UUID_LEN];
    gpuFbInfo          fbInfo;
    UVM_LINK_TYPE      sysmemLink;
    NvU32              sysmemLinkRateMBps;
    NvBool             connectedToSwitch;

    MemdescMap         kern2PhysDescrMap;
};

struct gpuAddressSpace
{
    NvHandle              handle;
    struct gpuDevice     *device;
    PNODE                 allocations;
    PORT_RWLOCK           *allocationsLock;
    PNODE                 cpuMappings;
    PORT_RWLOCK           *cpuMappingsLock;
    PNODE                 physAllocations;
    PORT_RWLOCK           *physAllocationsLock;
    NvU64                 vaBase;
    NvU64                 vaSize;
    // Dummy BAR1 allocation required on PCIe systems when GPPut resides in
    // sysmem.
    struct
    {
        NvU64             refCount;
        NvU64             gpuAddr;
        volatile void     *cpuAddr;
    } dummyGpuAlloc;
};

struct gpuChannel
{
    NvHandle                     channelHandle;
    NvU32                        hwRunlistId;
    NvU32                        hwChannelId;
    UVM_GPU_CHANNEL_ENGINE_TYPE  engineType;

    // If engineType is CE, engineIndex is a zero-based offset from
    // NV2080_ENGINE_TYPE_COPY0. If engineType is GR, engineIndex is a
    // zero-based offset from NV2080_ENGINE_TYPE_GR0.
    NvU32                        engineIndex;
    struct gpuAddressSpace       *vaSpace;
    NvU64                        gpFifo;
    NvNotification               *errorNotifier;
    NvU64                        errorNotifierOffset;
    NvU64                        *gpFifoEntries;
    NvU32                        fifoEntries;
    KeplerAControlGPFifo         *controlPage;
    struct gpuObject             *nextAttachedEngine;
    NvHandle                     hFaultCancelSwMethodClass;
    volatile unsigned            *workSubmissionOffset;
    NvU32                        workSubmissionToken;
    volatile NvU32               *pWorkSubmissionToken;
    NvHandle                     hUserdPhysHandle;
    NvU64                        userdGpuAddr;
    UVM_BUFFER_LOCATION          gpFifoLoc;
    UVM_BUFFER_LOCATION          gpPutLoc;
    NvBool                       retainedDummyAlloc;
};

// Add 3 to include local ctx buffer, patch context buffer and PM ctxsw buffer
ct_assert(UVM_GPU_CHANNEL_MAX_RESOURCES >= (GR_GLOBALCTX_BUFFER_COUNT + 3));

// A retained channel is a user client's channel which has been registered with
// the UVM driver.
struct gpuRetainedChannel_struct
{
    struct gpuDevice            *device;
    deviceDesc                  *rmDevice;
    subDeviceDesc               *rmSubDevice;
    struct gpuSession           *session;
    OBJGPU                     *pGpu;
    MEMORY_DESCRIPTOR           *instanceMemDesc;
    MEMORY_DESCRIPTOR           *resourceMemDesc[UVM_GPU_CHANNEL_MAX_RESOURCES];
    UVM_GPU_CHANNEL_ENGINE_TYPE channelEngineType;
    NvU32                       resourceCount;
    NvU32                       chId;
    NvU32                       runlistId;
    NvU32                       grIdx;

    // Dup of user's TSG (if one exists) under our RM client
    NvHandle                    hDupTsg;

    // Dup to context share object
    NvHandle                    hDupKernelCtxShare;

    // Handle for object that retains chId and instance mem
    NvHandle                    hChannelRetainer;
};

struct gpuObject
{
    NvHandle         handle;
    NvU32            type;
    struct gpuObject *next;
};

struct allocFlags
{
    NvBool bGetKernelVA;
    NvBool bfixedAddressAllocate;
};

struct ChannelAllocInfo
{
    NV_CHANNELGPFIFO_ALLOCATION_PARAMETERS gpFifoAllocParams;
    gpuAllocInfo gpuAllocInfo;
};

struct systemP2PCaps
{
    // peerId[i] contains gpu[i]'s peer id of gpu[1 - i]
    NvU32 peerIds[2];

    // true if the two GPUs are direct NvLink or PCIe peers
    NvU32 accessSupported : 1;

    // true if the two GPUs are indirect (NvLink) peers
    NvU32 indirectAccessSupported : 1;

    // true if the two GPUs are direct NvLink peers
    NvU32 nvlinkSupported : 1;

    NvU32 atomicSupported : 1;

    // optimalNvlinkWriteCEs[i] contains the index of the optimal CE to use when
    // writing from gpu[i] to gpu[1 - i]
    NvU32 optimalNvlinkWriteCEs[2];
};

static NV_STATUS findUvmAddressSpace(NvHandle hClient, NvU32 gpuInstance, NvHandle *pHandle, OBJVASPACE **ppVaspace);
static NV_STATUS nvGpuOpsGpuMalloc(struct gpuAddressSpace *vaSpace,
                                   NvBool isSystemMemory,
                                   NvLength length,
                                   NvU64 *gpuOffset,
                                   struct allocFlags flags,
                                   gpuAllocInfo *allocInfo);
static NV_STATUS trackDescriptor(PNODE *pRoot, NvU64 key, void *desc);
static NV_STATUS findDescriptor(PNODE pRoot, NvU64 key, void **desc);
static NV_STATUS deleteDescriptor(PNODE *pRoot, NvU64 key, void **desc);
static NV_STATUS destroyAllGpuMemDescriptors(NvHandle hClient, PNODE pNode);
static NV_STATUS getHandleForVirtualAddr(struct gpuAddressSpace *vaSpace,
                                         NvU64 allocationVa,
                                         NvBool bPhysical,
                                         NvHandle *pHandle);
static NV_STATUS findDeviceClasses(NvHandle hRoot,
                                   NvHandle hDevice,
                                   NvHandle hSubdevice,
                                   NvU32 *hostClass,
                                   NvU32 *ceClass,
                                   NvU32 *computeClass,
                                   NvU32 *faultBufferClass,
                                   NvU32 *accessCounterBufferClass,
                                   NvU32 *sec2Class);
static NV_STATUS queryCopyEngines(struct gpuDevice *gpu, gpuCesCaps *cesCaps);
static void nvGpuOpsFreeVirtual(struct gpuAddressSpace *vaSpace,
                                NvU64 vaOffset);
static NvBool isDeviceVoltaPlus(const struct gpuDevice *device);
static NvBool isDeviceTuringPlus(const struct gpuDevice *device);
static NV_STATUS gpuDeviceMapUsermodeRegion(struct gpuDevice *device);
static void gpuDeviceDestroyUsermodeRegion(struct gpuDevice *device);
static void gpuDeviceUnmapCpuFreeHandle(struct gpuDevice *device,
                                        NvHandle handle,
                                        void *ptr,
                                        NvU32 flags);
static NV_STATUS allocNvlinkStatusForSubdevice(struct gpuDevice *device,
                                               NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS **nvlinkStatusOut);
static NvU32 getNvlinkConnectionToNpu(const NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus,
                                      NvBool *atomicSupported,
                                      NvU32 *linkBandwidthMBps);
static NvU32 getNvlinkConnectionToSwitch(const NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus,
                                         NvU32 *linkBandwidthMBps);
static NV_STATUS nvGpuOpsGetMemoryByHandle(NvHandle hClient, NvHandle hMemory, Memory **ppMemory);
static void _nvGpuOpsReleaseChannel(gpuRetainedChannel *retainedChannel);
static NV_STATUS _nvGpuOpsRetainChannelResources(struct gpuDevice *device,
                                                 NvHandle hClient,
                                                 NvHandle hKernelChannel,
                                                 gpuRetainedChannel *retainedChannel,
                                                 gpuChannelInstanceInfo *channelInstanceInfo);
static void _nvGpuOpsReleaseChannelResources(gpuRetainedChannel *retainedChannel);

/*
 * This function will lock the RM API lock according to rmApiLockFlags, and then
 * examine numLocksNeeded. If this is 0, no GPU locks will be acquired. If it
 * is 1, the GPU lock for deviceInstance1 will be locked. If it is 2, both GPU
 * locks for deviceInstance1 and deviceInstance2 will be locked. If it is any
 * other number, all the GPU locks will be acquired.
 *
 * This function will attempt to grab the needed GPU locks, and will write the
 * resulting mask into *lockedGpusMask. In the event of a failure to acquire any
 * needed GPU locks, the written mask is 0 and the function returns
 * NV_ERR_INVALID_LOCK_STATE. In this case, all locks held are released and the
 * caller does not need to release any locks.
 */

typedef struct nvGpuOpsLockSet
{
    NvBool isRmLockAcquired;
    NvBool isRmSemaAcquired;
    GPU_MASK gpuMask;
    RsClient *pClientLocked;
} nvGpuOpsLockSet;

static void _nvGpuOpsLocksRelease(nvGpuOpsLockSet *acquiredLocks)
{
    OBJSYS *pSys;
    pSys = SYS_GET_INSTANCE();

    if (acquiredLocks->gpuMask != 0)
    {
        rmGpuGroupLockRelease(acquiredLocks->gpuMask, GPUS_LOCK_FLAGS_NONE);
        acquiredLocks->gpuMask = 0;
    }

    if (acquiredLocks->pClientLocked != NULL)
    {
        serverReleaseClient(&g_resServ, LOCK_ACCESS_WRITE, acquiredLocks->pClientLocked);
        acquiredLocks->pClientLocked = NULL;
    }

    if (acquiredLocks->isRmLockAcquired == NV_TRUE)
    {
        rmApiLockRelease();
        acquiredLocks->isRmLockAcquired = NV_FALSE;
    }

    if (acquiredLocks->isRmSemaAcquired == NV_TRUE)
    {
        osReleaseRmSema(pSys->pSema, NULL);
        acquiredLocks->isRmSemaAcquired = NV_FALSE;
    }
}

static NV_STATUS _nvGpuOpsLocksAcquire(NvU32 rmApiLockFlags,
                                       NvHandle hClient,
                                       RsClient **ppClient,
                                       NvU32 numLocksNeeded,
                                       NvU32 deviceInstance1,
                                       NvU32 deviceInstance2,
                                       nvGpuOpsLockSet *acquiredLocks)
{
    NV_STATUS status;
    OBJSYS  *pSys;
    GPU_MASK gpuMaskRequested;
    GPU_MASK gpuMaskAcquired;

    acquiredLocks->isRmSemaAcquired = NV_FALSE;
    acquiredLocks->isRmLockAcquired = NV_FALSE;
    acquiredLocks->gpuMask = 0;
    acquiredLocks->pClientLocked = NULL;

    pSys = SYS_GET_INSTANCE();
    if (pSys == NULL)
    {
        return NV_ERR_GENERIC;
    }

    status = osAcquireRmSema(pSys->pSema);
    if (status != NV_OK)
    {
        return status;
    }
    acquiredLocks->isRmSemaAcquired = NV_TRUE;

    status = rmApiLockAcquire(rmApiLockFlags, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(acquiredLocks);
        return status;
    }
    acquiredLocks->isRmLockAcquired = NV_TRUE;

    if (hClient != NV01_NULL_OBJECT)
    {
        status = serverAcquireClient(&g_resServ, hClient, LOCK_ACCESS_WRITE, &acquiredLocks->pClientLocked);

        if (status != NV_OK)
        {
            _nvGpuOpsLocksRelease(acquiredLocks);
            return status;
        }

        if (ppClient != NULL)
            *ppClient = acquiredLocks->pClientLocked;
    }

    //
    // Determine the GPU lock mask we need. If we are asked for 0, 1, or 2 locks
    // then we should use neither, just the first, or both deviceInstance
    // parameters, respectively. If any other number of locks is requested, we
    // acquire all of the lockable GPUS.
    //
    // We cannot simply determine the mask outside of this function and pass in
    // the mask, because gpumgrGetDeviceGpuMask requires that we hold the RM API
    // lock. Otherwise, SLI rewiring could preempt lock acquisition and render
    // the mask invalid.
    //
    gpuMaskRequested = 0;

    if (numLocksNeeded > 2)
    {
        gpuMaskRequested = GPUS_LOCK_ALL;
    }
    else
    {
        if (numLocksNeeded > 0)
        {
            gpuMaskRequested |= gpumgrGetDeviceGpuMask(deviceInstance1);
        }

        if (numLocksNeeded > 1)
        {
            gpuMaskRequested |= gpumgrGetDeviceGpuMask(deviceInstance2);
        }
    }

    //
    // The gpuMask parameter to rmGpuGroupLockAcquire is both input and output,
    // so we have to copy in what we want here to make comparisons later.
    //
    gpuMaskAcquired = gpuMaskRequested;
    if (gpuMaskRequested != 0)
    {
        status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_GPU_OPS, &gpuMaskAcquired);
    }
    acquiredLocks->gpuMask = gpuMaskAcquired;

    //
    // If we cannot acquire all the locks requested, we release all the locks
    // we *were* able to get and bail out here. There is never a safe way to
    // proceed with a GPU ops function with fewer locks than requested. If there
    // was a safe way to proceed, the client should have asked for fewer locks
    // in the first place.
    //
    // That said, callers sometimes want "all available GPUs", and then the call
    // to rmGpuGroupLockAcquire will mask off invalid GPUs for us. Hence the
    // exception for GPUS_LOCK_ALL.
    //
    if (gpuMaskAcquired != gpuMaskRequested && gpuMaskRequested != GPUS_LOCK_ALL)
    {
        status = NV_ERR_INVALID_LOCK_STATE;
    }

    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(acquiredLocks);
    }

    return status;
}

static NV_STATUS _nvGpuOpsLocksAcquireAll(NvU32 rmApiLockFlags,
                                          NvHandle hClient, RsClient **ppClient,
                                          nvGpuOpsLockSet *acquiredLocks)
{
    return _nvGpuOpsLocksAcquire(rmApiLockFlags, hClient, ppClient, 3, 0, 0, acquiredLocks);
}

static NV_STATUS nvGpuOpsCreateClient(RM_API *pRmApi, NvHandle *hClient)
{
    NV_STATUS status;
    RS_SHARE_POLICY sharePolicy;

    *hClient = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                           hClient, NV01_ROOT, hClient);
    if (status != NV_OK)
    {
        return status;
    }

    // Override default system share policy. Prohibit sharing of any and all
    // objects owned by this client.
    portMemSet(&sharePolicy, 0, sizeof(sharePolicy));
    sharePolicy.type = RS_SHARE_TYPE_ALL;
    sharePolicy.action = RS_SHARE_ACTION_FLAG_REVOKE;
    RS_ACCESS_MASK_ADD(&sharePolicy.accessMask, RS_ACCESS_DUP_OBJECT);

    status = pRmApi->Share(pRmApi, *hClient, *hClient, &sharePolicy);
    if (status != NV_OK)
    {
        pRmApi->Free(pRmApi, *hClient, *hClient);
    }

    return status;
}

NV_STATUS nvGpuOpsCreateSession(struct gpuSession **session)
{
    struct gpuSession *gpuSession = NULL;
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalNonPaged();

    gpuSession = portMemAllocNonPaged(sizeof(*gpuSession));
    if (gpuSession == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(gpuSession, 0, sizeof(*gpuSession));

    status = nvGpuOpsCreateClient(pRmApi, &gpuSession->handle);
    if (status != NV_OK)
    {
        portMemFree(gpuSession);
        return status;
    }

    gpuSession->devices = NULL;
    gpuSession->btreeLock = portSyncRwLockCreate(pAlloc);
    *session = (gpuSession);
    return status;
}

NV_STATUS nvGpuOpsDestroySession(struct gpuSession *session)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!session)
        return NV_OK;

    // Sanity Check: There should not be any attached devices with the session!
    NV_ASSERT(!session->devices);

    // freeing session will free everything under it
    pRmApi->Free(pRmApi, session->handle, session->handle);
    portSyncRwLockDestroy(session->btreeLock);
    portMemFree(session);
    return NV_OK;
}

static void *gpuBar0BaseAddress(OBJGPU *pGpu)
{
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);

    NV_ASSERT(pMapping);

    return pMapping->gpuNvAddr;
}

static void eccErrorCallback(void *pArg, void *pData, NvHandle hEvent,
                             NvU32 data, NvU32 status)
{
    subDeviceDesc *rmSubDevice = (subDeviceDesc *)pArg;

    NV_ASSERT(rmSubDevice);

    rmSubDevice->eccErrorNotifier = NV_TRUE;
}

static NvBool deviceNeedsDummyAlloc(struct gpuDevice *device)
{
    // The dummy mapping is needed so the client can issue a read to flush out
    // any CPU BAR1 PCIE writes prior to updating GPPUT. This is only needed
    // when the bus is non-coherent and when not in ZeroFB (where there can't be
    // any BAR1 mappings).
    return device->sysmemLink < UVM_LINK_TYPE_NVLINK_2 && !device->fbInfo.bZeroFb;
}

static NV_STATUS nvGpuOpsVaSpaceRetainDummyAlloc(struct gpuAddressSpace *vaSpace)
{
    struct gpuDevice *device;
    NV_STATUS status = NV_OK;
    gpuAllocInfo allocInfo = {0};
    struct allocFlags flags = {0};

    device = vaSpace->device;
    NV_ASSERT(device);
    NV_ASSERT(deviceNeedsDummyAlloc(device));

    if (vaSpace->dummyGpuAlloc.refCount > 0)
        goto done;

    flags.bGetKernelVA = NV_FALSE;
    status = nvGpuOpsGpuMalloc(vaSpace,
                               NV_FALSE,
                               NV_GPU_SMALL_PAGESIZE,
                               &vaSpace->dummyGpuAlloc.gpuAddr,
                               flags,
                               &allocInfo);
    if (status != NV_OK)
        return status;

    status = nvGpuOpsMemoryCpuMap(vaSpace,
                                  vaSpace->dummyGpuAlloc.gpuAddr,
                                  NV_GPU_SMALL_PAGESIZE,
                                  (void **)&vaSpace->dummyGpuAlloc.cpuAddr,
                                  PAGE_SIZE_DEFAULT);
    if (status != NV_OK)
        nvGpuOpsMemoryFree(vaSpace, vaSpace->dummyGpuAlloc.gpuAddr);

done:
    if (status == NV_OK)
    {
        ++vaSpace->dummyGpuAlloc.refCount;
        NV_ASSERT(vaSpace->dummyGpuAlloc.gpuAddr);
        NV_ASSERT(vaSpace->dummyGpuAlloc.cpuAddr);
    }

    return status;
}

static void nvGpuOpsVaSpaceReleaseDummyAlloc(struct gpuAddressSpace *vaSpace)
{
    NV_ASSERT(deviceNeedsDummyAlloc(vaSpace->device));
    NV_ASSERT(vaSpace->dummyGpuAlloc.refCount != 0);

    if (--vaSpace->dummyGpuAlloc.refCount > 0)
        return;

    if (vaSpace->dummyGpuAlloc.cpuAddr)
        nvGpuOpsMemoryCpuUnMap(vaSpace, (void *)vaSpace->dummyGpuAlloc.cpuAddr);

    if (vaSpace->dummyGpuAlloc.gpuAddr)
        nvGpuOpsMemoryFree(vaSpace, vaSpace->dummyGpuAlloc.gpuAddr);

    vaSpace->dummyGpuAlloc.cpuAddr = NULL;
    vaSpace->dummyGpuAlloc.gpuAddr = 0;
}

static NV_STATUS nvGpuOpsDisableVaSpaceChannels(struct gpuAddressSpace *vaSpace)
{
    NV_STATUS   status = NV_OK;
    OBJVASPACE *pVAS = NULL;
    Device     *pDevice;
    RsClient   *pClient;
    RS_ORDERED_ITERATOR it;
    RM_API     *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS disableParams = {0};

    if (vaSpace == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    status = serverGetClientUnderLock(&g_resServ, vaSpace->device->session->handle, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, vaSpace->device->handle, &pDevice);
    if (status != NV_OK)
        return status;

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if ((status != NV_OK) || (pVAS == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    // Stop all channels under the VAS, but leave them bound.
    it = kchannelGetIter(pClient, RES_GET_REF(pDevice));
    while (clientRefOrderedIterNext(pClient, &it))
    {
        KernelChannel *pKernelChannel = dynamicCast(it.pResourceRef->pResource, KernelChannel);

        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);
        if (pKernelChannel->pVAS != pVAS)
            continue;

        NV_ASSERT_OR_RETURN(disableParams.numChannels < NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES, NV_ERR_NOT_SUPPORTED);
        disableParams.hClientList[disableParams.numChannels] = RES_GET_CLIENT_HANDLE(pKernelChannel);
        disableParams.hChannelList[disableParams.numChannels] = RES_GET_HANDLE(pKernelChannel);
        disableParams.numChannels++;
    }

    if (disableParams.numChannels == 0)
        return status;

    disableParams.bDisable = NV2080_CTRL_FIFO_DISABLE_CHANNEL_TRUE;
    status = pRmApi->Control(pRmApi,
                             vaSpace->device->session->handle,
                             vaSpace->device->subhandle,
                             NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS,
                             &disableParams,
                             sizeof(disableParams));
    return status;
}

static NV_STATUS nvGpuOpsEnableVaSpaceChannels(struct gpuAddressSpace *vaSpace)
{
    NV_STATUS    status = NV_OK;
    OBJVASPACE  *pVAS = NULL;
    Device      *pDevice;
    RsClient    *pClient;
    RS_ORDERED_ITERATOR it;
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS disableParams = {0};
    RM_API      *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (vaSpace == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    status = serverGetClientUnderLock(&g_resServ, vaSpace->device->session->handle, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, vaSpace->device->handle, &pDevice);
    if (status != NV_OK)
        return status;

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if ((status != NV_OK) || (pVAS == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    it = kchannelGetIter(pClient, RES_GET_REF(pDevice));
    while (clientRefOrderedIterNext(pClient, &it))
    {
        KernelChannel *pKernelChannel = dynamicCast(it.pResourceRef->pResource, KernelChannel);

        NV_ASSERT_OR_ELSE(pKernelChannel != NULL, continue);
        if (pKernelChannel->pVAS != pVAS)
            continue;

        NV_ASSERT_OR_RETURN(disableParams.numChannels < NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES, NV_ERR_NOT_SUPPORTED);
        disableParams.hClientList[disableParams.numChannels] = RES_GET_CLIENT_HANDLE(pKernelChannel);
        disableParams.hChannelList[disableParams.numChannels] = RES_GET_HANDLE(pKernelChannel);
        disableParams.numChannels++;
    }

    if (disableParams.numChannels == 0)
        return status;

    disableParams.bDisable = NV2080_CTRL_FIFO_DISABLE_CHANNEL_FALSE;
    status = pRmApi->Control(pRmApi,
                             vaSpace->device->session->handle,
                             vaSpace->device->subhandle,
                             NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS,
                             &disableParams,
                             sizeof(disableParams));
    return status;
}

static NV_STATUS nvGpuOpsRmDeviceCreate(struct gpuDevice *device)
{
    NV_STATUS status;
    NV0080_ALLOC_PARAMETERS nv0080AllocParams = { 0 };
    deviceDesc *rmDevice = NULL;
    struct gpuSession *session = device->session;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalNonPaged();
    OBJGPU *pGpu;

    // Find the existing rmDevice.
    // Otherwise, allocate an rmDevice.
    portSyncRwLockAcquireRead(session->btreeLock);
    status = findDescriptor(session->devices, device->deviceInstance, (void**)&rmDevice);
    portSyncRwLockReleaseRead(session->btreeLock);
    if (status == NV_OK)
    {
        NV_ASSERT(rmDevice);
        device->rmDevice = rmDevice;
        device->handle = rmDevice->deviceHandle;
        return NV_OK;
    }

    rmDevice = portMemAllocNonPaged(sizeof(*rmDevice));
    if (rmDevice == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    portMemSet(rmDevice, 0, sizeof(*rmDevice));

    nv0080AllocParams.deviceId = device->deviceInstance;
    nv0080AllocParams.hClientShare = session->handle;
    device->handle = NV01_NULL_OBJECT;
    status =  pRmApi->Alloc(pRmApi,
                            session->handle,
                            session->handle,
                            &device->handle,
                            NV01_DEVICE_0,
                            &nv0080AllocParams);
    if (status != NV_OK)
        goto cleanup_device_desc;

    device->rmDevice = rmDevice;
    rmDevice->deviceHandle = device->handle;
    rmDevice->subDevices = NULL;
    rmDevice->subDeviceCount = 0;

    portSyncRwLockAcquireWrite(session->btreeLock);
    status = trackDescriptor(&session->devices, device->deviceInstance, rmDevice);
    portSyncRwLockReleaseWrite(session->btreeLock);
    if (status != NV_OK)
        goto cleanup_device;

    // TODO: Acquired because CliGetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        goto cleanup_device;
    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
        goto cleanup_device;

    rmDevice->arch = gpuGetChipArch(pGpu);
    rmDevice->implementation = gpuGetChipImpl(pGpu);
    rmDevice->btreeLock = portSyncRwLockCreate(pAlloc);

    return NV_OK;

cleanup_device:
    pRmApi->Free(pRmApi, session->handle, device->handle);
cleanup_device_desc:
    portMemFree(rmDevice);
    return status;
}

static void nvGpuOpsRmDeviceDestroy(struct gpuDevice *device)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    deviceDesc *rmDevice = device->rmDevice;

    NV_ASSERT(rmDevice != NULL);

    if (rmDevice->subDeviceCount == 0)
    {
        struct gpuSession *session = device->session;
        portSyncRwLockAcquireWrite(session->btreeLock);
        deleteDescriptor(&session->devices, device->deviceInstance, (void**)&rmDevice);
        pRmApi->Free(pRmApi, session->handle, rmDevice->deviceHandle);
        portSyncRwLockDestroy(rmDevice->btreeLock);
        portMemFree(rmDevice);
        portSyncRwLockReleaseWrite(session->btreeLock);
    }
}

static void gpuDeviceRmSubDeviceDeinitEcc(struct gpuDevice *device)
{
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS eventDbeParams = {0};
    subDeviceDesc *rmSubDevice = device->rmSubDevice;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!rmSubDevice->bEccInitialized || !rmSubDevice->bEccEnabled)
        return;

    // Disable all notifications specific to ECC on this device
    eventDbeParams.event = NV2080_NOTIFIERS_ECC_DBE;
    eventDbeParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;

    pRmApi->Control(pRmApi,
                    device->session->handle,
                    device->subhandle,
                    NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                    (void *)&eventDbeParams,
                    sizeof(eventDbeParams));

    if (!isDeviceTuringPlus(device))
    {
        gpuDeviceUnmapCpuFreeHandle(device,
                                    rmSubDevice->eccMasterHandle,
                                    rmSubDevice->eccReadLocation,
                                    DRF_DEF(OS33, _FLAGS, _ACCESS, _READ_ONLY));
    }

    rmSubDevice->eccReadLocation = NULL;

    if (rmSubDevice->eccCallbackHandle)
        pRmApi->Free(pRmApi, device->session->handle, rmSubDevice->eccCallbackHandle);

    rmSubDevice->bEccEnabled = NV_FALSE;
    rmSubDevice->bEccInitialized = NV_FALSE;
}

//
// Initialize the ECC state for an RM subdevice
//
// This can only be done once per RM subdevice as GF100_SUBDEVICE_MASTER can
// only be allocated once.
//
static NV_STATUS gpuDeviceRmSubDeviceInitEcc(struct gpuDevice *device)
{
    NV_STATUS status = NV_OK;
    int i = 0;
    int tempPtr = 0;

    NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS eccStatusParams;
    NV90E6_CTRL_MASTER_GET_ECC_INTR_OFFSET_MASK_PARAMS eccMaskParams = {0};
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS errContIntrMaskParams = {0};
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS eventDbeParams = {0};
    NV0005_ALLOC_PARAMETERS allocDbeParams = {0};
    OBJGPU *pGpu = NULL;
    NvBool supportedOnAnyUnits = NV_FALSE;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_ASSERT(device);

    // TODO: Acquired because CliGetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = CliSetGpuContext(device->session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    portMemSet(&eccStatusParams, 0, sizeof(eccStatusParams));

    rmSubDevice->eccOffset = 0;
    rmSubDevice->eccMask   = 0;
    rmSubDevice->eccReadLocation = NULL;
    rmSubDevice->eccMasterHandle = 0;
    rmSubDevice->bEccInitialized = NV_FALSE;
    rmSubDevice->bEccEnabled = NV_FALSE;

    // Do not initialize ECC for this device if SMC is enabled, but no partition
    // was subscribed to.  This will be the case for select devices created
    // on behalf of the UVM driver.
    if (IS_MIG_IN_USE(pGpu) && rmSubDevice->smcPartition.info == NULL)
        return NV_OK;

    // Check ECC before doing anything here
    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS,
                             &eccStatusParams,
                             sizeof(eccStatusParams));

    if (status == NV_ERR_NOT_SUPPORTED)
    {
        // Nothing to do if ECC not supported
        rmSubDevice->bEccEnabled = NV_FALSE;
        goto success;
    }
    else if (status != NV_OK)
    {
        return status;
    }

    //
    // ECC is considered as supported only if it's enabled for all supported units,
    // and there's at least 1 supported unit
    //
    rmSubDevice->bEccEnabled = NV_TRUE;

    for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT; i++)
    {
        // Check the ECC status only on the units supported by HW
        if (eccStatusParams.units[i].supported)
        {
            supportedOnAnyUnits = NV_TRUE;
            if (!eccStatusParams.units[i].enabled)
                rmSubDevice->bEccEnabled = NV_FALSE;
        }
    }

    if (!supportedOnAnyUnits)
        rmSubDevice->bEccEnabled = NV_FALSE;

    if (!rmSubDevice->bEccEnabled)
    {
        // ECC not enabled, early-out
        goto success;
    }

    //Allocate memory for interrupt tree
    rmSubDevice->eccMasterHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi, device->session->handle,
                           device->subhandle,
                           &rmSubDevice->eccMasterHandle,
                           GF100_SUBDEVICE_MASTER,
                           &tempPtr);
    if (status != NV_OK)
        goto error;

    if (isDeviceTuringPlus(device))
    {
        rmSubDevice->eccReadLocation = gpuBar0BaseAddress(pGpu);
        status = pRmApi->Control(pRmApi,
                                 device->session->handle,
                                 rmSubDevice->eccMasterHandle,
                                 NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK,
                                 &errContIntrMaskParams,
                                 sizeof(errContIntrMaskParams));
        if (status != NV_OK)
            goto error;

        rmSubDevice->eccOffset = GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_ERR_CONT);
        rmSubDevice->eccMask = errContIntrMaskParams.eccMask;
    }
    else
    {
        // Map memory
        status = pRmApi->MapToCpu(pRmApi,
                                  device->session->handle,
                                  device->subhandle,
                                  rmSubDevice->eccMasterHandle, 0,
                                  sizeof(GF100MASTERMap),
                                  (void **)(&rmSubDevice->eccReadLocation),
                                  DRF_DEF(OS33, _FLAGS, _ACCESS, _READ_ONLY));
        if (status != NV_OK)
            goto error;

        NV_ASSERT(rmSubDevice->eccReadLocation);

        status = pRmApi->Control(pRmApi,
                                 device->session->handle,
                                 rmSubDevice->eccMasterHandle,
                                 NV90E6_CTRL_CMD_MASTER_GET_ECC_INTR_OFFSET_MASK,
                                 (void *)&eccMaskParams,
                                 sizeof(eccMaskParams));
        if (status != NV_OK)
            goto error;

        // Fill the mask and offset which has been read from control call
        rmSubDevice->eccOffset = eccMaskParams.offset;
        rmSubDevice->eccMask   = eccMaskParams.mask;
    }

    // Setup callback for ECC DBE
    rmSubDevice->eccDbeCallback.func = eccErrorCallback;
    rmSubDevice->eccDbeCallback.arg = rmSubDevice;

    allocDbeParams.hParentClient = device->session->handle;
    allocDbeParams.hClass = NV01_EVENT_KERNEL_CALLBACK_EX;
    allocDbeParams.notifyIndex = NV2080_NOTIFIERS_ECC_DBE;
    allocDbeParams.data = NV_PTR_TO_NvP64(&rmSubDevice->eccDbeCallback);

    rmSubDevice->eccCallbackHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi, device->session->handle,
                           device->subhandle,
                           &rmSubDevice->eccCallbackHandle,
                           NV01_EVENT_KERNEL_CALLBACK_EX,
                           &allocDbeParams);

    if (status != NV_OK)
        goto error;

    eventDbeParams.event = NV2080_NOTIFIERS_ECC_DBE;
    eventDbeParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE;

    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                             (void *)&eventDbeParams,
                             sizeof(eventDbeParams));
    if (status != NV_OK)
        goto error;

success:
    rmSubDevice->bEccInitialized = NV_TRUE;
    return NV_OK;

error:
    gpuDeviceRmSubDeviceDeinitEcc(device);
    return status;
}

static NV_STATUS getSwizzIdFromSmcPartHandle(RM_API *pRmApi,
                                             NvHandle hClient,
                                             NvHandle hGPUInstanceSubscription,
                                             NvU32 *swizzId)
{
    NV_STATUS status;
    RsResourceRef *pSmcResourceRef;
    GPUInstanceSubscription *pGPUInstanceSubscription;

    // get GPUInstanceSubscription handle
    // TODO: Acquired because serverutilGetResourceRef expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = serverutilGetResourceRef(hClient, hGPUInstanceSubscription, &pSmcResourceRef);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    pGPUInstanceSubscription = dynamicCast(pSmcResourceRef->pResource, GPUInstanceSubscription);
    if (!pGPUInstanceSubscription)
        return NV_ERR_INVALID_OBJECT;

    *swizzId = pGPUInstanceSubscription->pKernelMIGGpuInstance->swizzId;

    return NV_OK;
}

//
// Determine an SMC partition's swizzId given a user subscription
//
// This requires temporarily duplicating the handle to validate it, as well
// as to prevent removal of the partition for the duration of the look-up.
// However, neither the partition, nor the swizzId uniquely identifying
// it (within the scope of its parent GPU) are guaranteed to remain valid, and
// callers of this function must be prepared for removal of the partition
// between nvGpuOpsGetGpuInfo() and nvGpuOpsDeviceCreate().
//
static NV_STATUS getSwizzIdFromUserSmcPartHandle(RM_API *pRmApi,
                                                 NvHandle hClient,
                                                 NvHandle hParent,
                                                 NvHandle hUserClient,
                                                 NvHandle hUserGPUInstanceSubscription,
                                                 NvU32 *swizzId)
{
    NV_STATUS status;
    NvHandle dupedGPUInstanceSubscription;

    // TODO: Acquired because serverutilGenResourceHandle expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = serverutilGenResourceHandle(hClient, &dupedGPUInstanceSubscription);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    status = pRmApi->DupObject(pRmApi,
                               hClient,
                               hParent,
                               &dupedGPUInstanceSubscription,
                               hUserClient,
                               hUserGPUInstanceSubscription,
                               NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);
    if (status != NV_OK)
        return status;

    // get GPUInstanceSubscription handle
    status = getSwizzIdFromSmcPartHandle(pRmApi, hClient, dupedGPUInstanceSubscription,
                                         swizzId);

    pRmApi->Free(pRmApi, hClient, dupedGPUInstanceSubscription);

    return status;
}

static void nvGpuOpsRmSmcPartitionDestroy(struct gpuDevice *device)
{
    subDeviceDesc *rmSubDevice = device->rmSubDevice;

    if (rmSubDevice->smcPartition.info != NULL)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

        pRmApi->Free(pRmApi,
                     device->session->handle,
                     rmSubDevice->smcPartition.handle);

        rmSubDevice->smcPartition.info = NULL;
    }
}

static NV_STATUS nvGpuOpsRmSmcPartitionCreate(struct gpuDevice *device, const gpuInfo *pGpuInfo)
{
    NV_STATUS status;
    OBJGPU *pGpu = NULL;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;
    NvHandle dupUserHandle;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuSession *session = device->session;
    RsResourceRef *pSmcResourceRef;
    GPUInstanceSubscription *pGPUInstanceSubscription;
    NvU32 swizzId;

    NV_ASSERT(rmSubDevice->smcPartition.info == NULL);

    if (!pGpuInfo->smcEnabled)
        return NV_ERR_INVALID_ARGUMENT;

    // TODO: Acquired because CliSetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    // Allocate the SMC partition object

    // SMC GPU partitioning was disabled since we detected the partition in
    // nvGpuOpsGetGpuInfo
    if (!IS_MIG_IN_USE(pGpu))
        return NV_ERR_INVALID_STATE;

    status = pRmApi->DupObject(pRmApi,
                               session->handle,
                               rmSubDevice->subDeviceHandle,
                               &dupUserHandle,
                               pGpuInfo->smcUserClientInfo.hClient,
                               pGpuInfo->smcUserClientInfo.hSmcPartRef,
                               NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);
    if (status != NV_OK)
        return status;

    status = getSwizzIdFromSmcPartHandle(pRmApi,
                                         session->handle,
                                         dupUserHandle,
                                         &swizzId);
    if (status != NV_OK)
        goto cleanup_dup_user_handle;

    // The swizzId changed since the call to nvGpuOpsGetGpuInfo: either the
    // object identified by smcUser*Handle changed, or else its configuration
    // was altered.
    if (swizzId != pGpuInfo->smcSwizzId)
    {
        status = NV_ERR_INVALID_STATE;
        goto cleanup_dup_user_handle;
    }

    rmSubDevice->smcPartition.handle = dupUserHandle;

    // get GPUInstanceSubscription handle
    // TODO: Acquired because serverutilGetResourceRef expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        goto cleanup_dup_user_handle;
    status = serverutilGetResourceRef(session->handle, rmSubDevice->smcPartition.handle, &pSmcResourceRef);
    rmapiLockRelease();
    if (status != NV_OK)
        goto cleanup_dup_user_handle;

    pGPUInstanceSubscription = dynamicCast(pSmcResourceRef->pResource, GPUInstanceSubscription);
    NV_ASSERT(pGPUInstanceSubscription != NULL);

    NV_ASSERT(pGPUInstanceSubscription->pKernelMIGGpuInstance->swizzId == pGpuInfo->smcSwizzId);

    rmSubDevice->smcPartition.info = pGPUInstanceSubscription->pKernelMIGGpuInstance;

    return NV_OK;

cleanup_dup_user_handle:
    pRmApi->Free(pRmApi, session->handle, dupUserHandle);

    return status;
}

static NV_STATUS nvGpuOpsRmSubDeviceCreate(struct gpuDevice *device)
{
    NV_STATUS status;
    NV2080_ALLOC_PARAMETERS nv2080AllocParams = { 0 };
    deviceDesc *rmDevice = NULL;
    subDeviceDesc *rmSubDevice = NULL;
    struct gpuSession *session = device->session;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_ASSERT(session);

    // Query the rmDevice which needed to create an rmSubDevice.
    portSyncRwLockAcquireRead(session->btreeLock);
    status = findDescriptor(session->devices, device->deviceInstance, (void**)&rmDevice);
    if (status != NV_OK)
    {
        portSyncRwLockReleaseRead(session->btreeLock);
        return status;
    }

    NV_ASSERT(rmDevice);
    NV_ASSERT(rmDevice->deviceHandle == device->handle);

    // Find the existing rmSubDevice.
    // Otherwise, allocate an rmSubDevice.
    portSyncRwLockAcquireWrite(rmDevice->btreeLock);
    if (findDescriptor(rmDevice->subDevices, device->subdeviceInstance, (void**)&rmSubDevice) == NV_OK)
    {
        NV_ASSERT(rmSubDevice);
        device->rmSubDevice = rmSubDevice;
        device->subhandle = rmSubDevice->subDeviceHandle;
        rmSubDevice->refCount++;
        portSyncRwLockReleaseWrite(rmDevice->btreeLock);
        portSyncRwLockReleaseRead(session->btreeLock);
        return NV_OK;
    }

    rmSubDevice = portMemAllocNonPaged(sizeof(*rmSubDevice));
    if (rmSubDevice == NULL)
       return NV_ERR_INSUFFICIENT_RESOURCES;

    portMemSet(rmSubDevice, 0, sizeof(*rmSubDevice));

    device->rmSubDevice = rmSubDevice;
    rmSubDevice->refCount = 1;
    nv2080AllocParams.subDeviceId = device->subdeviceInstance;
    device->subhandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi,
                           session->handle,
                           device->handle,
                           &device->subhandle,
                           NV20_SUBDEVICE_0,
                           &nv2080AllocParams);
    if (status != NV_OK)
        goto cleanup_subdevice_desc;
    rmSubDevice->subDeviceHandle = device->subhandle;

    status = trackDescriptor(&rmDevice->subDevices, device->subdeviceInstance, rmSubDevice);
    if (status != NV_OK)
        goto cleanup_subdevice;

    rmDevice->subDeviceCount++;

    portSyncRwLockReleaseWrite(rmDevice->btreeLock);
    portSyncRwLockReleaseRead(session->btreeLock);
    return NV_OK;

cleanup_subdevice:
    pRmApi->Free(pRmApi, session->handle, device->subhandle);
cleanup_subdevice_desc:
    portMemFree(rmSubDevice);
    portSyncRwLockReleaseWrite(rmDevice->btreeLock);
    portSyncRwLockReleaseRead(session->btreeLock);
    return status;
}

static NvBool isDevicePascalPlus(const struct gpuDevice *device)
{
    NV_ASSERT(device->rmDevice);
    return device->rmDevice->arch >= GPU_ARCHITECTURE_PASCAL;
}

static NvBool isDeviceVoltaPlus(const struct gpuDevice *device)
{
    NV_ASSERT(device->rmDevice);
    return device->rmDevice->arch >= GPU_ARCHITECTURE_VOLTA;
}

static NvBool isDeviceTuringPlus(const struct gpuDevice *device)
{
    NV_ASSERT(device->rmDevice);
    return device->rmDevice->arch >= GPU_ARCHITECTURE_TURING;
}

static NvBool isDeviceAmperePlus(const struct gpuDevice *device)
{
    NV_ASSERT(device->rmDevice);
    return device->rmDevice->arch >= GPU_ARCHITECTURE_AMPERE;
}

static UVM_LINK_TYPE rmControlToUvmNvlinkVersion(NvU32 nvlinkVersion)
{
    if (nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
        return UVM_LINK_TYPE_NONE;
    else if (nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0)
        return UVM_LINK_TYPE_NVLINK_1;
    else if (nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_0 ||
             nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_2)
        return UVM_LINK_TYPE_NVLINK_2;
    else if (nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_0 ||
             nvlinkVersion == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_1)
        return UVM_LINK_TYPE_NVLINK_3;

    NV_ASSERT(0);
    return (NvU32)-1;
}

static NV_STATUS queryFbInfo(struct gpuDevice *device)
{
    NV_STATUS nvStatus = NV_OK;
    NV2080_CTRL_FB_GET_INFO_PARAMS fbInfoParams;
    NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS *fbRegionInfoParams;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NvU32 i;

    struct fbInputParams
    {
        NV2080_CTRL_FB_INFO heapSize;
        NV2080_CTRL_FB_INFO reservedHeapSize;
        NV2080_CTRL_FB_INFO zeroFb;
    } fbParams;

    fbRegionInfoParams = portMemAllocNonPaged(sizeof(*fbRegionInfoParams));
    if (fbRegionInfoParams == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(fbRegionInfoParams, 0, sizeof(*fbRegionInfoParams));
    portMemSet(&fbInfoParams, 0, sizeof(fbInfoParams));
    portMemSet(&fbParams, 0, sizeof(fbParams));

    // Set up the list of parameters we are looking to extract
    fbParams.heapSize.index         = NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE;
    fbParams.reservedHeapSize.index = NV2080_CTRL_FB_INFO_INDEX_VISTA_RESERVED_HEAP_SIZE;
    fbParams.zeroFb.index           = NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN;

    fbInfoParams.fbInfoListSize = sizeof(fbParams) / sizeof(fbParams.heapSize);
    fbInfoParams.fbInfoList = NV_PTR_TO_NvP64(&fbParams);

    nvStatus = pRmApi->Control(pRmApi,
                               device->session->handle,
                               device->subhandle,
                               NV2080_CTRL_CMD_FB_GET_INFO,
                               &fbInfoParams,
                               sizeof(fbInfoParams));
    if (nvStatus != NV_OK)
        goto out;

    nvStatus = pRmApi->Control(pRmApi,
                               device->session->handle,
                               device->subhandle,
                               NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO,
                               fbRegionInfoParams,
                               sizeof(*fbRegionInfoParams));
    if (nvStatus != NV_OK)
        goto out;

    device->fbInfo.heapSize         = fbParams.heapSize.data;
    device->fbInfo.reservedHeapSize = fbParams.reservedHeapSize.data;
    device->fbInfo.bZeroFb          = (NvBool)fbParams.zeroFb.data;

    device->fbInfo.maxAllocatableAddress = 0;

    for (i = 0; i < fbRegionInfoParams->numFBRegions; ++i)
    {
        device->fbInfo.maxAllocatableAddress = NV_MAX(device->fbInfo.maxAllocatableAddress,
                                                      fbRegionInfoParams->fbRegion[i].limit);
    }

out:
    portMemFree(fbRegionInfoParams);
    return nvStatus;
}

// Return the PCIE link cap max speed associated with the given GPU in
// megabytes per seconds..
static NV_STATUS getPCIELinkRateMBps(struct gpuDevice *device, NvU32 *pcieLinkRate)
{
    // PCI Express Base Specification: https://www.pcisig.com/specifications/pciexpress
    const NvU32 PCIE_1_ENCODING_RATIO_TOTAL = 10;
    const NvU32 PCIE_1_ENCODING_RATIO_EFFECTIVE = 8;
    const NvU32 PCIE_2_ENCODING_RATIO_TOTAL = 10;
    const NvU32 PCIE_2_ENCODING_RATIO_EFFECTIVE = 8;
    const NvU32 PCIE_3_ENCODING_RATIO_TOTAL = 130;
    const NvU32 PCIE_3_ENCODING_RATIO_EFFECTIVE = 128;
    const NvU32 PCIE_4_ENCODING_RATIO_TOTAL = 130;
    const NvU32 PCIE_4_ENCODING_RATIO_EFFECTIVE = 128;
    const NvU32 PCIE_5_ENCODING_RATIO_TOTAL = 130;
    const NvU32 PCIE_5_ENCODING_RATIO_EFFECTIVE = 128;

    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NV2080_CTRL_BUS_INFO busInfo = {0};
    NV2080_CTRL_BUS_GET_INFO_PARAMS busInfoParams = {0};
    NvU32 linkRate = 0;
    NvU32 lanes;

    busInfo.index = NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CAPS;
    busInfoParams.busInfoListSize = 1;
    busInfoParams.busInfoList = NV_PTR_TO_NvP64(&busInfo);

    NV_STATUS status = pRmApi->Control(pRmApi,
                                       device->session->handle,
                                       device->subhandle,
                                       NV2080_CTRL_CMD_BUS_GET_INFO,
                                       &busInfoParams,
                                       sizeof(busInfoParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                  __LINE__, nvstatusToString(status));
        return status;
    }

    lanes = DRF_VAL(2080, _CTRL_BUS_INFO, _PCIE_LINK_CAP_MAX_WIDTH, busInfo.data);

    // Bug 2606540: RM reports PCIe transfer rate in GT/s but labels it as Gbps
    switch (DRF_VAL(2080, _CTRL_BUS_INFO, _PCIE_LINK_CAP_MAX_SPEED, busInfo.data))
    {
        case NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_2500MBPS:
            linkRate = ((2500 * lanes * PCIE_1_ENCODING_RATIO_EFFECTIVE)
                / PCIE_1_ENCODING_RATIO_TOTAL) / 8;
            break;
        case NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_5000MBPS:
            linkRate = ((5000 * lanes * PCIE_2_ENCODING_RATIO_EFFECTIVE)
                / PCIE_2_ENCODING_RATIO_TOTAL) / 8;
            break;
        case NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_8000MBPS:
            linkRate = ((8000 * lanes * PCIE_3_ENCODING_RATIO_EFFECTIVE)
                / PCIE_3_ENCODING_RATIO_TOTAL) / 8;
            break;
        case NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_16000MBPS:
            linkRate = ((16000 * lanes * PCIE_4_ENCODING_RATIO_EFFECTIVE)
                / PCIE_4_ENCODING_RATIO_TOTAL) / 8;
            break;
        case NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_32000MBPS:
            linkRate = ((32000 * lanes * PCIE_5_ENCODING_RATIO_EFFECTIVE)
                / PCIE_5_ENCODING_RATIO_TOTAL) / 8;
            break;
        default:
            status = NV_ERR_INVALID_STATE;
            NV_PRINTF(LEVEL_ERROR, "Unknown PCIe speed\n");
    }

    *pcieLinkRate = linkRate;

    return status;
}

NV_STATUS nvGpuOpsDeviceCreate(struct gpuSession *session,
                               const gpuInfo *pGpuInfo,
                               const NvProcessorUuid *gpuUuid,
                               struct gpuDevice **outDevice,
                               NvBool bCreateSmcPartition)
{
    NV_STATUS status;
    struct gpuDevice *device = NULL;
    NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS gpuIdInfoParams = {{0}};
    NV2080_CTRL_BUS_GET_INFO_V2_PARAMS *busInfoParams;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus;
    NvU32 nvlinkVersion;
    NvU32 sysmemLink;
    NvU32 linkBandwidthMBps;
    NvU32 sysmemConnType;
    NvBool atomicSupported;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    device = portMemAllocNonPaged(sizeof(*device));
    if (device == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;
    portMemSet(device, 0, sizeof(*device));
    device->session = session;

    portMemCopy(&gpuIdInfoParams.gpuUuid, NV_UUID_LEN, gpuUuid->uuid, NV_UUID_LEN);
    gpuIdInfoParams.flags = NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_BINARY;
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             session->handle,
                             NV0000_CTRL_CMD_GPU_GET_UUID_INFO,
                             &gpuIdInfoParams,
                             sizeof(gpuIdInfoParams));
    if (status != NV_OK)
        goto cleanup_device_obj;

    device->deviceInstance = gpuIdInfoParams.deviceInstance;
    device->subdeviceInstance = gpuIdInfoParams.subdeviceInstance;
    device->gpuId = gpuIdInfoParams.gpuId;

    status = nvGpuOpsRmDeviceCreate(device);
    if (status != NV_OK)
        goto cleanup_device_obj;

    status = nvGpuOpsRmSubDeviceCreate(device);
    if (status != NV_OK)
        goto cleanup_rm_device;

    if (bCreateSmcPartition)
    {
        status = nvGpuOpsRmSmcPartitionCreate(device, pGpuInfo);
        if (status != NV_OK)
            goto cleanup_rm_subdevice;
    }

    // Create the work submission info mapping:
    //  * SMC is disabled, we create for the device.
    //  * SMC is enabled, we create only for SMC partitions.
    if (isDeviceVoltaPlus(device) && (!pGpuInfo->smcEnabled || bCreateSmcPartition))
    {
        status = gpuDeviceMapUsermodeRegion(device);
        if (status != NV_OK)
            goto cleanup_smc_partition;
    }

    status = gpuDeviceRmSubDeviceInitEcc(device);
    if (status != NV_OK)
        goto cleanup_subdevice_usermode;

    status = queryFbInfo(device);
    if (status != NV_OK)
        goto cleanup_ecc;

    device->isTccMode = NV_FALSE;

    // Non-TCC mode on Windows implies WDDM mode.
    device->isWddmMode = !device->isTccMode;

    status = findDeviceClasses(session->handle,
                               device->handle,
                               device->subhandle,
                               &device->hostClass,
                               &device->ceClass,
                               &device->computeClass,
                               &device->faultBufferClass,
                               &device->accessCounterBufferClass,
                               &device->sec2Class);
    if (status != NV_OK)
        goto cleanup_ecc;

    busInfoParams = portMemAllocNonPaged(sizeof(*busInfoParams));
    if (busInfoParams == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto cleanup_ecc;
    }
    portMemSet(busInfoParams, 0, sizeof(*busInfoParams));
    busInfoParams->busInfoListSize = 1;
    busInfoParams->busInfoList[0].index = NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE;
    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_BUS_GET_INFO_V2,
                             busInfoParams,
                             sizeof(*busInfoParams));
    if (status != NV_OK)
    {
        portMemFree(busInfoParams);
        goto cleanup_ecc;
    }

    sysmemConnType = busInfoParams->busInfoList[0].data;
    portMemFree(busInfoParams);

    sysmemLink = UVM_LINK_TYPE_NONE;
    switch (sysmemConnType)
    {
        case NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_NVLINK:
        {
            status = allocNvlinkStatusForSubdevice(device, &nvlinkStatus);
            if (status != NV_OK)
                goto cleanup_ecc;

            nvlinkVersion = getNvlinkConnectionToNpu(nvlinkStatus,
                                                     &atomicSupported,
                                                     &linkBandwidthMBps);

            sysmemLink = rmControlToUvmNvlinkVersion(nvlinkVersion);

            portMemFree(nvlinkStatus);
            nvlinkStatus = NULL;
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_PCIE:
        {
            sysmemLink = UVM_LINK_TYPE_PCIE;
            status = getPCIELinkRateMBps(device, &linkBandwidthMBps);
            if (status != NV_OK)
                goto cleanup_ecc;
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported sysmem connection type: %d\n",
                     sysmemConnType);
            NV_ASSERT(0);
            break;
        }
    }

    NV_PRINTF(LEVEL_INFO, "sysmem link type: %d bw: %u\n", sysmemLink, linkBandwidthMBps);

    NV_ASSERT(sysmemLink != UVM_LINK_TYPE_NONE);
    device->sysmemLink = sysmemLink;
    device->sysmemLinkRateMBps = linkBandwidthMBps;

    status = allocNvlinkStatusForSubdevice(device, &nvlinkStatus);
    if (status != NV_OK)
        goto cleanup_ecc;
    nvlinkVersion = getNvlinkConnectionToSwitch(nvlinkStatus,
                                                &linkBandwidthMBps);

    if (rmControlToUvmNvlinkVersion(nvlinkVersion) != UVM_LINK_TYPE_NONE)
    {
        NV_ASSERT(rmControlToUvmNvlinkVersion(nvlinkVersion) != UVM_LINK_TYPE_NVLINK_1);

        // If the GPU is ever connected to the CPU via a switch, sysmemLink
        // and sysmemLinkRateMBps need to be updated accordingly.
        NV_ASSERT(sysmemConnType != NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_NVLINK);

        device->connectedToSwitch = NV_TRUE;
    }

    portMemFree(nvlinkStatus);

    mapInit(&device->kern2PhysDescrMap, portMemAllocatorGetGlobalNonPaged());

    *outDevice = device;
    return NV_OK;

cleanup_ecc:
    gpuDeviceRmSubDeviceDeinitEcc(device);
cleanup_subdevice_usermode:
    gpuDeviceDestroyUsermodeRegion(device);
cleanup_smc_partition:
    nvGpuOpsRmSmcPartitionDestroy(device);
cleanup_rm_subdevice:
    nvGpuOpsDeviceDestroy(device);
    device = NULL;
cleanup_rm_device:
    if (device)
        nvGpuOpsRmDeviceDestroy(device);
cleanup_device_obj:
    portMemFree(device);
    return status;
}

NV_STATUS nvGpuOpsDeviceDestroy(struct gpuDevice *device)
{
    deviceDesc *rmDevice = device->rmDevice;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    rmSubDevice->refCount--;

    if (rmSubDevice->refCount == 0)
    {
        gpuDeviceDestroyUsermodeRegion(device);

        gpuDeviceRmSubDeviceDeinitEcc(device);

        nvGpuOpsRmSmcPartitionDestroy(device);

        portSyncRwLockAcquireWrite(rmDevice->btreeLock);
        rmDevice->subDeviceCount--;
        deleteDescriptor(&rmDevice->subDevices, device->subdeviceInstance, (void**)&rmSubDevice);
        pRmApi->Free(pRmApi, device->session->handle, rmSubDevice->subDeviceHandle);
        portMemFree(rmSubDevice);
        portSyncRwLockReleaseWrite(rmDevice->btreeLock);

        nvGpuOpsRmDeviceDestroy(device);
    }

    mapDestroy(&device->kern2PhysDescrMap);

    portMemFree(device);
    return NV_OK;
}

NV_STATUS nvGpuOpsOwnPageFaultIntr(struct gpuDevice *device,
                                   NvBool bOwnInterrupts)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS changeParams = {0};
    changeParams.bOwnedByRm = !bOwnInterrupts;
    return pRmApi->Control(pRmApi,
                           device->session->handle,
                           device->subhandle,
                           NV2080_CTRL_CMD_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP,
                           &changeParams,
                           sizeof(changeParams));
}

static NV_STATUS getAddressSpaceInfo(struct gpuAddressSpace *vaSpace,
                                     OBJGPU *pGpu,
                                     UvmGpuAddressSpaceInfo *vaSpaceInfo)
{
    NV_STATUS status;
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS params = {0};
    OBJVASPACE *pVAS = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuDevice *device = vaSpace->device;
    struct gpuSession *session = device->session;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;

    params.hVASpace = vaSpace->handle;
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             device->handle,
                             NV0080_CTRL_CMD_DMA_ADV_SCHED_GET_VA_CAPS,
                             &params,
                             sizeof(params));
    if (status != NV_OK)
        return status;

    vaSpaceInfo->bigPageSize = params.bigPageSize;

    // TODO: Acquired because resserv expects RMAPI lock. Necessary?
    {
        RsClient *pClient;
        status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
        if (status != NV_OK)
            return status;

        status = serverGetClientUnderLock(&g_resServ, session->handle, &pClient);
        if (status == NV_OK)
        {
            status = vaspaceGetByHandleOrDeviceDefault(pClient, device->handle, vaSpace->handle, &pVAS);
        }
        rmapiLockRelease();
        if (status != NV_OK)
            return status;
    }

    vaSpaceInfo->atsEnabled = vaspaceIsAtsEnabled(pVAS);

    if (isDeviceTuringPlus(vaSpace->device))
    {
        //
        // On Turing+ use the VIRTUAL_FUNCTION so this works fine in hosts and
        // guests
        //
        void *bar0Mapping = gpuBar0BaseAddress(pGpu);
        vaSpaceInfo->time0Offset = (NvU32 *)((NvU8*)bar0Mapping + GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_TIME_0));
        vaSpaceInfo->time1Offset = (NvU32 *)((NvU8*)bar0Mapping + GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_TIME_1));
    }
    else if (isDeviceVoltaPlus(vaSpace->device))
    {
        NV_ASSERT(rmSubDevice->clientRegionMapping);

        //
        // On Volta prefer USERMODE mappings for better passthrough
        // performance on some hypervisors (see CL23003453 for more details)
        //
        vaSpaceInfo->time0Offset = (NvU32 *)((NvU8*)rmSubDevice->clientRegionMapping + NVC361_TIME_0);
        vaSpaceInfo->time1Offset = (NvU32 *)((NvU8*)rmSubDevice->clientRegionMapping + NVC361_TIME_1);
    }
    else
    {
        void *bar0Mapping = gpuBar0BaseAddress(pGpu);
        vaSpaceInfo->time0Offset = (NvU32 *)((NvU8*)bar0Mapping + NV_PTIMER_TIME_0);
        vaSpaceInfo->time1Offset = (NvU32 *)((NvU8*)bar0Mapping + NV_PTIMER_TIME_1);
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance = rmSubDevice->smcPartition.info;
        MIG_RESOURCE_ALLOCATION *pResourceAllocation = &pKernelMIGGpuInstance->resourceAllocation;

        vaSpaceInfo->maxSubctxCount = pResourceAllocation->veidCount;
        vaSpaceInfo->smcGpcCount = pResourceAllocation->gpcCount;
    }
    else
    {
        NV2080_CTRL_FIFO_GET_INFO_PARAMS *fifoGetInfoParams;

        //
        // NV2080_CTRL_FIFO_GET_INFO_PARAMS takes over 2KB, so we use a heap
        // allocation
        //
        fifoGetInfoParams = portMemAllocNonPaged(sizeof(*fifoGetInfoParams));
        if (fifoGetInfoParams == NULL)
            return NV_ERR_NO_MEMORY;

        fifoGetInfoParams->fifoInfoTblSize = 1;
        fifoGetInfoParams->fifoInfoTbl[0].index = NV2080_CTRL_FIFO_INFO_INDEX_MAX_SUBCONTEXT_PER_GROUP;

        status = pRmApi->Control(pRmApi,
                                 session->handle,
                                 rmSubDevice->subDeviceHandle,
                                 NV2080_CTRL_CMD_FIFO_GET_INFO,
                                 fifoGetInfoParams,
                                 sizeof(*fifoGetInfoParams));

        vaSpaceInfo->maxSubctxCount = fifoGetInfoParams->fifoInfoTbl[0].data;

        portMemFree(fifoGetInfoParams);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

// This function will create a new address space object of type FERMI_VASPACE_A.
NV_STATUS nvGpuOpsAddressSpaceCreate(struct gpuDevice *device,
                                     NvU64 vaBase,
                                     NvU64 vaSize,
                                     struct gpuAddressSpace **vaSpace,
                                     UvmGpuAddressSpaceInfo *vaSpaceInfo)
{
    NV_STATUS status;
    struct gpuAddressSpace *gpuVaSpace = NULL;
    OBJGPU *pGpu = NULL;
    NV_VASPACE_ALLOCATION_PARAMETERS vaParams = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalNonPaged();

    gpuVaSpace = portMemAllocNonPaged(sizeof(*gpuVaSpace));
    if (gpuVaSpace == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(gpuVaSpace, 0, sizeof(*gpuVaSpace));
    gpuVaSpace->vaBase = vaBase;
    gpuVaSpace->vaSize = vaSize;
    gpuVaSpace->handle = NV01_NULL_OBJECT;
    gpuVaSpace->allocationsLock = portSyncRwLockCreate(pAlloc);
    gpuVaSpace->cpuMappingsLock = portSyncRwLockCreate(pAlloc);
    gpuVaSpace->physAllocationsLock = portSyncRwLockCreate(pAlloc);

    *vaSpace = NULL;
    portMemSet(vaSpaceInfo, 0, sizeof(*vaSpaceInfo));

    // Create a new vaSpace object
    vaParams.index= NV_VASPACE_ALLOCATION_INDEX_GPU_NEW;
    vaParams.vaBase = gpuVaSpace->vaBase;
    vaParams.vaSize = gpuVaSpace->vaSize;
    vaParams.flags  = gpuVaSpace->vaSize ?
                      NV_VASPACE_ALLOCATION_FLAGS_SHARED_MANAGEMENT :
                      NV_VASPACE_ALLOCATION_FLAGS_NONE;

    // TODO: Acquired because CliSetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        goto cleanup_vaspace;
    status = CliSetGpuContext(device->session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
    {
        goto cleanup_vaspace;
    }

    status =  pRmApi->Alloc(pRmApi,
                            device->session->handle,
                            device->handle,
                            &gpuVaSpace->handle, FERMI_VASPACE_A,
                            &vaParams);
    if (status != NV_OK)
    {
        goto cleanup_struct;
    }

    // If base & Size were not provided before, they would have been filled now
    gpuVaSpace->vaBase = vaParams.vaBase;
    gpuVaSpace->vaSize = vaParams.vaSize;
    gpuVaSpace->device = device;

    status = getAddressSpaceInfo(gpuVaSpace, pGpu, vaSpaceInfo);
    if (status != NV_OK)
    {
        goto cleanup_vaspace;
    }

    *vaSpace = gpuVaSpace;
    return status;

cleanup_vaspace:
    pRmApi->Free(pRmApi, device->session->handle, gpuVaSpace->handle);

cleanup_struct:
    portSyncRwLockDestroy(gpuVaSpace->allocationsLock);
    portSyncRwLockDestroy(gpuVaSpace->cpuMappingsLock);
    portSyncRwLockDestroy(gpuVaSpace->physAllocationsLock);
    portMemFree(gpuVaSpace);
    return status;
}

NV_STATUS nvGpuOpsDupAddressSpace(struct gpuDevice *device,
                                  NvHandle hUserClient,
                                  NvHandle hUserVASpace,
                                  struct gpuAddressSpace **vaSpace,
                                  UvmGpuAddressSpaceInfo *vaSpaceInfo)
{
    NV_STATUS status = NV_OK;
    struct gpuAddressSpace *gpuVaSpace = NULL;
    struct gpuSession *session = device->session;
    OBJVASPACE *pVAS = NULL;
    OBJGPU *pGpu = NULL;
    RsResourceRef *pVaSpaceRef;
    RsResourceRef *pDeviceRef;
    Device *pDevice = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    PORT_MEM_ALLOCATOR *pAlloc = portMemAllocatorGetGlobalNonPaged();

    *vaSpace = NULL;
    portMemSet(vaSpaceInfo, 0, sizeof(*vaSpaceInfo));

    // TODO - Move this check to RMDupObject later.
    // TODO: Acquired because serverutilGetResourceRef expects RMAPI lock. Necessary?
    // Find the device associated with the hUserVASpace and verify that the UUID belongs to it.
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = serverutilGetResourceRef(hUserClient, hUserVASpace, &pVaSpaceRef);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    if (!dynamicCast(pVaSpaceRef->pResource, VaSpaceApi))
        return NV_ERR_INVALID_OBJECT;

    // The parent must be valid and a device if this is a VA space handle
    // TODO: Acquired because serverutilGetResourceRef expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = serverutilGetResourceRef(hUserClient, pVaSpaceRef->pParentRef->hResource, &pDeviceRef);
    rmapiLockRelease();
    NV_ASSERT(status == NV_OK);

    pDevice = dynamicCast(pDeviceRef->pResource, Device);
    NV_ASSERT(pDevice != NULL);

    if (pDevice->deviceInst != device->deviceInstance)
        return NV_ERR_OTHER_DEVICE_FOUND;

    gpuVaSpace = portMemAllocNonPaged(sizeof(*gpuVaSpace));
    if (gpuVaSpace == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(gpuVaSpace, 0, sizeof(*gpuVaSpace));

    gpuVaSpace->device = device;
    gpuVaSpace->allocationsLock = portSyncRwLockCreate(pAlloc);
    gpuVaSpace->cpuMappingsLock = portSyncRwLockCreate(pAlloc);
    gpuVaSpace->physAllocationsLock = portSyncRwLockCreate(pAlloc);

    // dup the vaspace
    gpuVaSpace->handle = NV01_NULL_OBJECT;
    status = pRmApi->DupObject(pRmApi,
                               session->handle,
                               device->handle,
                               &gpuVaSpace->handle,
                               hUserClient,
                               hUserVASpace,
                               NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);
    if (status != NV_OK)
        goto cleanup_vaspace;

    // TODO: Acquired because these functions expect RMAPI lock. Necessary?
    {
        RsClient *pClient;
        status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
        if (status != NV_OK)
            goto cleanup_dup_vaspace;

        status = serverGetClientUnderLock(&g_resServ, session->handle, &pClient);
        if (status == NV_OK)
        {
            status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
            if (status == NV_OK)
            {
                status = vaspaceGetByHandleOrDeviceDefault(pClient, device->handle, gpuVaSpace->handle, &pVAS);
            }
        }
        rmapiLockRelease();
        if (status != NV_OK)
            goto cleanup_dup_vaspace;
    }

    if (!vaspaceIsExternallyOwned(pVAS))
    {
        status = NV_ERR_INVALID_FLAGS;
        goto cleanup_dup_vaspace;
    }

    status = getAddressSpaceInfo(gpuVaSpace, pGpu, vaSpaceInfo);
    if (status != NV_OK)
        goto cleanup_dup_vaspace;

    *vaSpace = gpuVaSpace;

    return NV_OK;

cleanup_dup_vaspace:
    pRmApi->Free(pRmApi, session->handle, gpuVaSpace->handle);
cleanup_vaspace:
    portSyncRwLockDestroy(gpuVaSpace->allocationsLock);
    portSyncRwLockDestroy(gpuVaSpace->cpuMappingsLock);
    portSyncRwLockDestroy(gpuVaSpace->physAllocationsLock);
    portMemFree(gpuVaSpace);
    return status;
}

// Get the NVLink connection status for the given device. On success, caller is
// responsible of freeing the memory.
static NV_STATUS allocNvlinkStatusForSubdevice(struct gpuDevice *device,
                                               NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS **nvlinkStatusOut)
{
    NV_STATUS status;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    *nvlinkStatusOut = NULL;

    nvlinkStatus = portMemAllocNonPaged(sizeof(*nvlinkStatus));
    if (nvlinkStatus == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(nvlinkStatus, 0, sizeof(*nvlinkStatus));
    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS,
                             nvlinkStatus,
                             sizeof(*nvlinkStatus));
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        portMemSet(nvlinkStatus, 0, sizeof(*nvlinkStatus));
    }
    else if (status != NV_OK)
    {
        portMemFree(nvlinkStatus);
        NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                  __LINE__, nvstatusToString(status));
        return status;
    }

    *nvlinkStatusOut = nvlinkStatus;

    return NV_OK;
}

// If the given NvLink connection has a GPU device as an endpoint, return the
// version of the NvLink connection with that GPU , and the maximum
// unidirectional bandwidth in megabytes per second. Otherwise, return
// NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID.
static NvU32 getNvlinkConnectionToGpu(const NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus,
                                      OBJGPU *pGpu,
                                      NvU32 *linkBandwidthMBps)
{
    NvU32 i;

    NvU32 version  = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID;
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU16 bus      = gpuGetBus(pGpu);
    NvU16 device   = gpuGetDevice(pGpu);
    NvU32 bwMBps   = 0;

    for (i = 0; i < NV2080_CTRL_NVLINK_MAX_LINKS; ++i)
    {
        if (((1 << i) & nvlinkStatus->enabledLinkMask) == 0)
            continue;

        if (!nvlinkStatus->linkInfo[i].connected)
            continue;

        // Skip loopback/loopout connections
        if (nvlinkStatus->linkInfo[i].loopProperty != NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE)
            continue;

        if (nvlinkStatus->linkInfo[i].remoteDeviceInfo.deviceType == NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU)
        {
            if ((nvlinkStatus->linkInfo[i].remoteDeviceInfo.deviceIdFlags &
                 NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI) == 0)
            {
                NV_PRINTF(LEVEL_ERROR, "No PCI information for GPU.\n");
                continue;
            }

            if ((domain                   == nvlinkStatus->linkInfo[i].remoteDeviceInfo.domain) &&
                (bus                      == nvlinkStatus->linkInfo[i].remoteDeviceInfo.bus)    &&
                (device                   == nvlinkStatus->linkInfo[i].remoteDeviceInfo.device) &&
                (pGpu->idInfo.PCIDeviceID == nvlinkStatus->linkInfo[i].remoteDeviceInfo.pciDeviceId))
            {
                NvU32 capsTbl = nvlinkStatus->linkInfo[i].capsTbl;

                NV_ASSERT(NV2080_CTRL_NVLINK_GET_CAP(((NvU8 *)&capsTbl), NV2080_CTRL_NVLINK_CAPS_P2P_ATOMICS));

                if (bwMBps == 0)
                    version = nvlinkStatus->linkInfo[i].nvlinkVersion;

                bwMBps += nvlinkStatus->linkInfo[i].nvlinkLineRateMbps;
                NV_ASSERT(version == nvlinkStatus->linkInfo[i].nvlinkVersion);
            }
        }
    }

    *linkBandwidthMBps = bwMBps;
    if (version == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
        NV_ASSERT(*linkBandwidthMBps == 0);

    return version;
}

// If the given NvLink connection has a NPU device as an endpoint, return the
// version of the NvLink connection with that NPU , and the maximum
// unidirectional bandwidth in megabytes per second. Otherwise, return
// NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID.
static NvU32 getNvlinkConnectionToNpu(const NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus,
                                      NvBool *atomicSupported,
                                      NvU32 *linkBandwidthMBps)
{
    NvU32 i;
    NvU32 bwMBps   = 0;
    NvU32 version  = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID;

    *atomicSupported = NV_FALSE;

    for (i = 0; i < NV2080_CTRL_NVLINK_MAX_LINKS; ++i)
    {
        if (((1 << i) & nvlinkStatus->enabledLinkMask) == 0)
            continue;

        if (!nvlinkStatus->linkInfo[i].connected)
            continue;

        // Skip loopback/loopout connections
        if (nvlinkStatus->linkInfo[i].loopProperty != NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE)
            continue;

        if (nvlinkStatus->linkInfo[i].remoteDeviceInfo.deviceType == NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NPU)
        {
            NvU32 capsTbl = nvlinkStatus->linkInfo[i].capsTbl;
            NvBool atomicCap = !!NV2080_CTRL_NVLINK_GET_CAP(((NvU8 *)&capsTbl), NV2080_CTRL_NVLINK_CAPS_SYSMEM_ATOMICS);

            if (bwMBps == 0)
            {
                *atomicSupported = atomicCap;
                version = nvlinkStatus->linkInfo[i].nvlinkVersion;
            }
            bwMBps += nvlinkStatus->linkInfo[i].nvlinkLineRateMbps;
            NV_ASSERT(version == nvlinkStatus->linkInfo[i].nvlinkVersion);
            NV_ASSERT(*atomicSupported == atomicCap);
        }
    }

    *linkBandwidthMBps = bwMBps;
    if (version == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
        NV_ASSERT(*linkBandwidthMBps == 0);

    return version;
}

// If the given NvLink connection has a switch as an endpoint, return the
// version of the NvLink connection with that switch, and the maximum
// unidirectional bandwidth in megabytes per second. Otherwise, return
// NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID.
static NvU32 getNvlinkConnectionToSwitch(const NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus,
                                         NvU32 *linkBandwidthMBps)
{
    NvU32 i;
    NvU32 bwMBps   = 0;
    NvU32 version  = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID;

    for (i = 0; i < NV2080_CTRL_NVLINK_MAX_LINKS; ++i)
    {
        if (((1 << i) & nvlinkStatus->enabledLinkMask) == 0)
            continue;

        if (!nvlinkStatus->linkInfo[i].connected)
            continue;

        // Skip loopback/loopout connections
        if (nvlinkStatus->linkInfo[i].loopProperty != NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE)
            continue;

        if (nvlinkStatus->linkInfo[i].remoteDeviceInfo.deviceType == NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH)
        {
            if (bwMBps == 0)
                version = nvlinkStatus->linkInfo[i].nvlinkVersion;

            bwMBps += nvlinkStatus->linkInfo[i].nvlinkLineRateMbps;
            NV_ASSERT(version == nvlinkStatus->linkInfo[i].nvlinkVersion);
       }
    }

    *linkBandwidthMBps = bwMBps;
    if (version == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
        NV_ASSERT(*linkBandwidthMBps == 0);

    return version;
}

// Compute whether the non-peer GPUs with the given NVLink connections can
// communicate through P9 NPUs
static NV_STATUS gpusHaveNpuNvlink(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus1,
                                   NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus2,
                                   NvU32 *nvlinkVersion,
                                   NvU32 *linkBandwidthMBps)
{
    NvU32 nvlinkVersion1, nvlinkVersion2;
    NvU32 tmpLinkBandwidthMBps;
    NvBool atomicSupported1, atomicSupported2;

    *nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID;

    nvlinkVersion1 = getNvlinkConnectionToNpu(nvlinkStatus1,
                                              &atomicSupported1,
                                              &tmpLinkBandwidthMBps);
    nvlinkVersion2 = getNvlinkConnectionToNpu(nvlinkStatus2,
                                              &atomicSupported2,
                                              &tmpLinkBandwidthMBps);

    if (nvlinkVersion1 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID ||
        nvlinkVersion2 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
        return NV_OK;

    // Non-peer GPU communication over NPU is only supported on NVLink 2.0 or
    // greater
    if (nvlinkVersion1 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0 ||
        nvlinkVersion2 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0)
    {
        // NVLink1 devices cannot be mixed with other versions. NVLink3
        // supports mixing NVLink2 and NVLink3 devices
        NV_ASSERT(nvlinkVersion1 == nvlinkVersion2);
        return NV_OK;
    }

    NV_ASSERT(atomicSupported1);
    NV_ASSERT(atomicSupported2);

    // We do not explore the whole connectivity graph. We assume that NPUs
    // connected to NVLink2 (and greater) can forward memory requests so that
    // if GPU A is connected to NPU M and GPU B is connected to NPU N, A can
    // access B.
    *nvlinkVersion = NV_MIN(nvlinkVersion1, nvlinkVersion2);

    // Link bandwidth not provided because the intermediate link rate could
    // vary a lot with system topologies & current load, making this bandwidth
    // obsolete.
    *linkBandwidthMBps = 0;

    return NV_OK;
}

static NV_STATUS rmSystemP2PCapsControl(struct gpuDevice *device1,
                                        struct gpuDevice *device2,
                                        NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *p2pCapsParams)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    portMemSet(p2pCapsParams, 0, sizeof(*p2pCapsParams));
    p2pCapsParams->gpuIds[0] = device1->gpuId;
    p2pCapsParams->gpuIds[1] = device2->gpuId;
    p2pCapsParams->gpuCount = 2;

    NvHandle handle = device1->session->handle;
    NV_STATUS status = pRmApi->Control(pRmApi,
                                       handle,
                                       handle,
                                       NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2,
                                       p2pCapsParams,
                                       sizeof(*p2pCapsParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                  __LINE__, nvstatusToString(status));
    }

    return status;
}

// Get R/W/A access capabilities and the link type between the two given GPUs
static NV_STATUS getSystemP2PCaps(struct gpuDevice *device1,
                                  struct gpuDevice *device2,
                                  struct systemP2PCaps *p2pCaps)
{
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *p2pCapsParams = NULL;
    NV_STATUS status = NV_OK;

    p2pCapsParams = portMemAllocNonPaged(sizeof(*p2pCapsParams));
    if (p2pCapsParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = rmSystemP2PCapsControl(device1, device2, p2pCapsParams);
    if (status != NV_OK)
        goto done;

    portMemSet(p2pCaps, 0, sizeof(*p2pCaps));
    p2pCaps->peerIds[0] = p2pCapsParams->busPeerIds[0 * 2 + 1];
    p2pCaps->peerIds[1] = p2pCapsParams->busPeerIds[1 * 2 + 0];
    p2pCaps->nvlinkSupported = !!REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED, p2pCapsParams->p2pCaps);
    p2pCaps->atomicSupported = !!REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_ATOMICS_SUPPORTED, p2pCapsParams->p2pCaps);
    p2pCaps->indirectAccessSupported = !!REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_INDIRECT_NVLINK_SUPPORTED,
                                                 p2pCapsParams->p2pCaps);

    // TODO: Bug 1768805: Check both reads and writes since RM seems to be
    //       currently incorrectly reporting just the P2P write cap on some
    //       systems that cannot support P2P at all. See the bug for more
    //       details.
    if (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, p2pCapsParams->p2pCaps) &&
        REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, p2pCapsParams->p2pCaps))
    {
        NV_ASSERT(!p2pCaps->indirectAccessSupported);

        p2pCaps->accessSupported = NV_TRUE;
    }

    if (p2pCaps->nvlinkSupported || p2pCaps->indirectAccessSupported)
    {
        // Exactly one CE is expected to be recommended for transfers between
        // NvLink peers
        NV_ASSERT(nvPopCount32(p2pCapsParams->p2pOptimalWriteCEs) == 1);

        // Query the write mask only; UVM has no use for the read mask
        p2pCaps->optimalNvlinkWriteCEs[0] = BIT_IDX_32(p2pCapsParams->p2pOptimalWriteCEs);

        // Query the P2P capabilities of device2->device1, which may be
        // different from those of device1->device2
        status = rmSystemP2PCapsControl(device2, device1, p2pCapsParams);
        if (status != NV_OK)
            goto done;

        NV_ASSERT(nvPopCount32(p2pCapsParams->p2pOptimalWriteCEs) == 1);

        p2pCaps->optimalNvlinkWriteCEs[1] = BIT_IDX_32(p2pCapsParams->p2pOptimalWriteCEs);
    }

done:
    portMemFree(p2pCapsParams);
    return status;
}

// Return the NVLink P2P capabilities of the peer GPUs with the given devices
static NV_STATUS getNvlinkP2PCaps(struct gpuDevice *device1,
                                  struct gpuDevice *device2,
                                  NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus1,
                                  NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus2,
                                  NvU32 *nvlinkVersion,
                                  NvU32 *linkBandwidthMBps)
{
    NvU32 nvlinkVersion1, nvlinkVersion2;
    NvU32 linkBandwidthMBps1, linkBandwidthMBps2;

    *nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID;

    if (device1->connectedToSwitch && device2->connectedToSwitch)
    {
        nvlinkVersion1 = getNvlinkConnectionToSwitch(nvlinkStatus1,
                                                     &linkBandwidthMBps1);
        nvlinkVersion2 = getNvlinkConnectionToSwitch(nvlinkStatus2,
                                                     &linkBandwidthMBps2);
    }
    else
    {
        OBJGPU *pGpu1, *pGpu2;

        pGpu1 = gpumgrGetGpuFromId(device1->gpuId);
        if (!pGpu1)
            return NV_ERR_OBJECT_NOT_FOUND;

        pGpu2 = gpumgrGetGpuFromId(device2->gpuId);
        if (!pGpu2)
            return NV_ERR_OBJECT_NOT_FOUND;

        nvlinkVersion1 = getNvlinkConnectionToGpu(nvlinkStatus1,
                                                  pGpu2,
                                                  &linkBandwidthMBps1);
        nvlinkVersion2 = getNvlinkConnectionToGpu(nvlinkStatus2,
                                                  pGpu1,
                                                  &linkBandwidthMBps2);
    }

    if (nvlinkVersion1 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID ||
        nvlinkVersion2 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_INVALID)
    {
        *linkBandwidthMBps = 0;
        return NV_OK;
    }

    // NVLink1 devices cannot be mixed with other versions. NVLink3 supports
    // mixing NVLink2 and NVLink3 devices. NVLink4 devices cannot be mixed with
    // prior NVLink versions.
    if (nvlinkVersion1 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0 ||
        nvlinkVersion2 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0 ||
        nvlinkVersion1 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_4_0 ||
        nvlinkVersion2 == NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_4_0)
    {
        NV_ASSERT(nvlinkVersion1 == nvlinkVersion2);
        NV_ASSERT(linkBandwidthMBps1 == linkBandwidthMBps2);
    }

    *nvlinkVersion = NV_MIN(nvlinkVersion1, nvlinkVersion2);
    *linkBandwidthMBps = NV_MIN(linkBandwidthMBps1, linkBandwidthMBps2);

    return NV_OK;
}

NV_STATUS nvGpuOpsGetP2PCaps(struct gpuDevice *device1,
                             struct gpuDevice *device2,
                             getP2PCapsParams *p2pCapsParams)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus1 = NULL;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *nvlinkStatus2 = NULL;
    struct systemP2PCaps p2pCaps;

    if (!device1 || !device2)
        return NV_ERR_INVALID_ARGUMENT;

    if (device1->session != device2->session)
        return NV_ERR_INVALID_ARGUMENT;

    if (!p2pCapsParams)
        return NV_ERR_INVALID_ARGUMENT;

    status = allocNvlinkStatusForSubdevice(device1, &nvlinkStatus1);
    if (status != NV_OK)
        goto cleanup;

    status = allocNvlinkStatusForSubdevice(device2, &nvlinkStatus2);
    if (status != NV_OK)
        goto cleanup;

    portMemSet(p2pCapsParams, 0, sizeof(*p2pCapsParams));
    p2pCapsParams->peerIds[0]      = (NvU32)-1;
    p2pCapsParams->peerIds[1]      = (NvU32)-1;
    p2pCapsParams->p2pLink         = UVM_LINK_TYPE_NONE;
    p2pCapsParams->indirectAccess  = NV_FALSE;

    status = getSystemP2PCaps(device1, device2, &p2pCaps);
    if (status != NV_OK)
        goto cleanup;

    if (p2pCaps.indirectAccessSupported)
    {
        NvU32 nvlinkVersion;
        NvU32 linkBandwidthMBps;
        NvU32 p2pLink;

        status = gpusHaveNpuNvlink(nvlinkStatus1,
                                   nvlinkStatus2,
                                   &nvlinkVersion,
                                   &linkBandwidthMBps);
        if (status != NV_OK)
            goto cleanup;

        p2pLink = rmControlToUvmNvlinkVersion(nvlinkVersion);

        NV_ASSERT(p2pLink >= UVM_LINK_TYPE_NVLINK_2);
        NV_ASSERT(linkBandwidthMBps == 0);

        p2pCapsParams->indirectAccess           = NV_TRUE;
        p2pCapsParams->p2pLink                  = p2pLink;
        p2pCapsParams->optimalNvlinkWriteCEs[0] = p2pCaps.optimalNvlinkWriteCEs[0];
        p2pCapsParams->optimalNvlinkWriteCEs[1] = p2pCaps.optimalNvlinkWriteCEs[1];
        p2pCapsParams->totalLinkLineRateMBps    = linkBandwidthMBps;
    }
    else if (p2pCaps.accessSupported)
    {
        p2pCapsParams->peerIds[0] = p2pCaps.peerIds[0];
        p2pCapsParams->peerIds[1] = p2pCaps.peerIds[1];

        if (p2pCaps.nvlinkSupported)
        {
            NvU32 nvlinkVersion;
            NvU32 linkBandwidthMBps;

            NV_ASSERT(p2pCaps.atomicSupported);

            status = getNvlinkP2PCaps(device1,
                                      device2,
                                      nvlinkStatus1,
                                      nvlinkStatus2,
                                      &nvlinkVersion,
                                      &linkBandwidthMBps);
            if (status != NV_OK)
                goto cleanup;

            p2pCapsParams->p2pLink                  = rmControlToUvmNvlinkVersion(nvlinkVersion);
            p2pCapsParams->optimalNvlinkWriteCEs[0] = p2pCaps.optimalNvlinkWriteCEs[0];
            p2pCapsParams->optimalNvlinkWriteCEs[1] = p2pCaps.optimalNvlinkWriteCEs[1];

            NV_ASSERT(p2pCapsParams->p2pLink != UVM_LINK_TYPE_NONE);
            NV_ASSERT(linkBandwidthMBps != 0);

            p2pCapsParams->totalLinkLineRateMBps    = linkBandwidthMBps;
        }
        else
        {
            NvU32 linkBandwidthMBps1, linkBandwidthMBps2;

            status = getPCIELinkRateMBps(device1, &linkBandwidthMBps1);
            if (status != NV_OK)
                goto cleanup;

            status = getPCIELinkRateMBps(device2, &linkBandwidthMBps2);
            if (status != NV_OK)
                goto cleanup;

            p2pCapsParams->p2pLink               = UVM_LINK_TYPE_PCIE;
            p2pCapsParams->totalLinkLineRateMBps = NV_MIN(linkBandwidthMBps1, linkBandwidthMBps2);
        }
    }

cleanup:
    portMemFree(nvlinkStatus1);
    portMemFree(nvlinkStatus2);

    return status;
}

static NV_STATUS nvGpuOpsGetExternalAllocP2pInfo(struct gpuSession *session,
                                                 NvU32 memOwnerGpuId,
                                                 NvU32 gpuId,
                                                 NvBool *isPeerSupported,
                                                 NvU32 *peerId)
{
    NV_STATUS status = NV_OK;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *p2pCapsParams = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT(gpuId != memOwnerGpuId);

    p2pCapsParams = portMemAllocNonPaged(sizeof(*p2pCapsParams));
    if (p2pCapsParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portMemSet(p2pCapsParams, 0, sizeof(*p2pCapsParams));
    p2pCapsParams->gpuIds[0] = gpuId;
    p2pCapsParams->gpuIds[1] = memOwnerGpuId;
    p2pCapsParams->gpuCount = 2;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             session->handle,
                             NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2,
                             p2pCapsParams,
                             sizeof(*p2pCapsParams));
    if (status != NV_OK)
        goto done;

    *isPeerSupported =
            (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, p2pCapsParams->p2pCaps) &&
             REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, p2pCapsParams->p2pCaps));

    *peerId = p2pCapsParams->busPeerIds[0 * 2 + 1];

done:
    portMemFree(p2pCapsParams);
    return status;
}

static GMMU_APERTURE nvGpuOpsGetExternalAllocAperture(PMEMORY_DESCRIPTOR pMemDesc,
                                                      NvBool isIndirectPeerSupported,
                                                      NvBool isPeerSupported)
{
    // Don't support both direct and indirect peers
    NV_ASSERT(!(isIndirectPeerSupported && isPeerSupported));

    // Get the aperture
    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
    {
        if (isIndirectPeerSupported)
            return GMMU_APERTURE_SYS_COH;

        if (isPeerSupported)
            return GMMU_APERTURE_PEER;

        return GMMU_APERTURE_VIDEO;
    }
    else if ((memdescGetAddressSpace(pMemDesc) == ADDR_FABRIC) ||
             (memdescGetAddressSpace(pMemDesc) == ADDR_FABRIC_V2))
    {
        return GMMU_APERTURE_PEER;
    }
    else
    {
        return GMMU_APERTURE_SYS_COH;
    }
}

static NvBool nvGpuOpsGetExternalAllocVolatility(PMEMORY_DESCRIPTOR pMemDesc,
                                                 GMMU_APERTURE aperture,
                                                 NvBool isIndirectPeerSupported,
                                                 UvmRmGpuCachingType cachingType)
{
    if (cachingType == UvmRmGpuCachingTypeDefault)
    {
        if (aperture == GMMU_APERTURE_PEER || isIndirectPeerSupported)
            return (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) ? NV_TRUE : NV_FALSE;
        else
            return (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) ? NV_TRUE : NV_FALSE;
    }
    else if (cachingType == UvmRmGpuCachingTypeForceUncached)
    {
        return NV_TRUE;
    }
    else
    {
        return NV_FALSE;
    }
}

static NV_STATUS nvGpuOpsGetExternalAllocMappingAttribute(UvmRmGpuMappingType mappingType,
                                                          PMEMORY_DESCRIPTOR pMemDesc,
                                                          NvBool *readOnly,
                                                          NvBool *atomic)
{
    *readOnly = NV_FALSE;
    *atomic = NV_FALSE;

    if (memdescGetFlag(pMemDesc, MEMDESC_FLAGS_DEVICE_READ_ONLY))
    {
        if (mappingType != UvmRmGpuMappingTypeDefault &&
            mappingType != UvmRmGpuMappingTypeReadOnly)
            return NV_ERR_INVALID_ACCESS_TYPE;

        *readOnly = NV_TRUE;
        *atomic = NV_FALSE;
    }
    else
    {
        *readOnly = (mappingType == UvmRmGpuMappingTypeReadOnly);
        *atomic = (mappingType == UvmRmGpuMappingTypeDefault ||
                   mappingType == UvmRmGpuMappingTypeReadWriteAtomic);
    }

    return NV_OK;
}

static NV_STATUS nvGpuOpsGetPteKind(OBJGPU *pMappingGpu,
                                    MemoryManager *pMemoryManager,
                                    PMEMORY_DESCRIPTOR pMemDesc,
                                    Memory *pMemory,
                                    gpuExternalMappingInfo *pGpuExternalMappingInfo,
                                    NvU32 *newKind)
{
    NV_STATUS               status              = NV_OK;
    FB_ALLOC_PAGE_FORMAT    fbAllocPageFormat   = {0};
    NvU32                   ctagId;

    if (pGpuExternalMappingInfo->compressionType == UvmRmGpuCompressionTypeEnabledNoPlc)
    {
        if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, *newKind))
        {
            status = memmgrChooseKind_HAL(pMappingGpu,
                                          pMemoryManager,
                                          &fbAllocPageFormat,
                                          NVOS32_ATTR_COMPR_DISABLE_PLC_ANY,
                                          newKind);
        }
        else
        {
            status = NV_ERR_INVALID_ARGUMENT;
        }

        if (status != NV_OK)
            return status;
    }

    if (pGpuExternalMappingInfo->formatType != UvmRmGpuFormatTypeDefault)
    {
        NV_ASSERT(pGpuExternalMappingInfo->elementBits != UvmRmGpuFormatElementBitsDefault);

        fbAllocPageFormat.attr = pMemory->Attr;
        fbAllocPageFormat.attr2 = pMemory->Attr2;
        fbAllocPageFormat.flags = pMemory->Flags;
        fbAllocPageFormat.type = pMemory->Type;

        switch (pGpuExternalMappingInfo->formatType)
        {
            case UvmRmGpuFormatTypeBlockLinear:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _FORMAT, _BLOCK_LINEAR, fbAllocPageFormat.attr);
                break;
            default:
                break;
        }

        switch (pGpuExternalMappingInfo->elementBits)
        {
            case UvmRmGpuFormatElementBits8:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _8, fbAllocPageFormat.attr);
                break;
            case UvmRmGpuFormatElementBits16:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _16, fbAllocPageFormat.attr);
                break;
            // CUDA does not support 24-bit width
            case UvmRmGpuFormatElementBits32:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _32, fbAllocPageFormat.attr);
                break;
            case UvmRmGpuFormatElementBits64:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _64, fbAllocPageFormat.attr);
                break;
            case UvmRmGpuFormatElementBits128:
                fbAllocPageFormat.attr = FLD_SET_DRF(OS32, _ATTR, _DEPTH, _128, fbAllocPageFormat.attr);
                break;
            default:
                break;
        }

        status = memmgrChooseKind_HAL(pMappingGpu, pMemoryManager, &fbAllocPageFormat,
                                      DRF_VAL(OS32, _ATTR, _COMPR, fbAllocPageFormat.attr),
                                      newKind);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid kind type (%x)\n", *newKind);
            return status;
        }

        //
        // Check that the memory descriptor already has allocated comptags
        // if the new mapping enables compression. Downgrade the kind if no
        // comptags are present.
        //
        ctagId = FB_HWRESID_CTAGID_VAL_FERMI(memdescGetHwResId(pMemDesc));
        if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, *newKind) && !ctagId)
            *newKind = memmgrGetUncompressedKind_HAL(pMappingGpu, pMemoryManager, *newKind, NV_FALSE);

        if (*newKind == NV_MMU_PTE_KIND_INVALID)
            return NV_ERR_INVALID_ARGUMENT;
    }
    else
    {
        NV_ASSERT((pGpuExternalMappingInfo->elementBits == UvmRmGpuFormatElementBitsDefault) ||
                  (pGpuExternalMappingInfo->elementBits == UvmRmGpuFormatElementBits8));
    }

    return NV_OK;
}

static
NV_STATUS
nvGpuOpsMemGetPageSize
(
    OBJGPU *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32  *pPageSize
)
{
    NvU32 pageSize;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS status;

    pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
    if (pageSize == 0)
    {
        status = memmgrSetMemDescPageSize_HAL(pGpu,
                                              pMemoryManager,
                                              pMemDesc,
                                              AT_GPU,
                                              RM_ATTR_PAGE_SIZE_DEFAULT);
        if (status != NV_OK)
            return status;

        pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
        NV_ASSERT(pageSize != 0);
    }

    *pPageSize = pageSize;

    return NV_OK;
}

static
NV_STATUS
nvGpuOpsBuildExternalAllocPtes
(
    OBJVASPACE *pVAS,
    OBJGPU     *pMappingGpu,
    MEMORY_DESCRIPTOR *pMemDesc,
    Memory     *pMemory,
    NvU64       offset,
    NvU64       size,
    NvBool      isIndirectPeerSupported,
    NvBool      isPeerSupported,
    NvU32       peerId,
    gpuExternalMappingInfo *pGpuExternalMappingInfo
)
{
    NV_STATUS               status              = NV_OK;
    OBJGVASPACE            *pGVAS               = NULL;
    const GMMU_FMT         *pFmt                = NULL;
    const GMMU_FMT_PTE     *pPteFmt             = NULL;
    const MMU_FMT_LEVEL    *pLevelFmt           = NULL;
    GMMU_APERTURE           aperture;
    COMPR_INFO              comprInfo;
    GMMU_ENTRY_VALUE        pte                 = {{0}};

    NvU64         fabricBaseAddress   = NVLINK_INVALID_FABRIC_ADDR;
    NvU32         kind, pageSize;
    NvU32         skipPteCount;
    NvBool        vol, atomic, readOnly;
    NvBool        encrypted, privileged;
    NvU64         iter, physAddr, mappingSize, pteCount;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pMappingGpu);
    KernelGmmu    *pKernelGmmu = GPU_GET_KERNEL_GMMU(pMappingGpu);
    NvU64          allocSize;
    NvBool         isCompressedKind;
    NvU64         *physicalAddresses = NULL;
    NvU32          newKind, oldKind;
    NvBool         kindChanged = NV_FALSE;
    NvU64          gpaOffset;
    NvBool         *isPLCable = NULL;
    NvU64          *guestPhysicalAddress = NULL;

    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    status = nvGpuOpsMemGetPageSize(pMappingGpu,
                                    pMemDesc,
                                    &pageSize);
    if (status != NV_OK)
        return status;

    // memdescGetSize returns the requested size of the allocation. But, the
    // actual allocation size could be larger than the requested size due
    // to alignment requirement. So, make sure the correct size is used.
    // Note, alignment can be greater than the pageSize.
    allocSize = RM_ALIGN_UP(pMemDesc->ActualSize, pageSize);

    if (offset >= allocSize)
        return NV_ERR_INVALID_BASE;

    if ((offset + size) > allocSize)
        return NV_ERR_INVALID_LIMIT;

    if ((size & (pageSize - 1)) != 0)
        return NV_ERR_INVALID_ARGUMENT;

    if ((offset & (pageSize - 1)) != 0)
        return NV_ERR_INVALID_ARGUMENT;

    pGVAS = dynamicCast(pVAS, OBJGVASPACE);

    // Get the GMMU format
    pFmt = gvaspaceGetGmmuFmt(pGVAS, pMappingGpu);
    pPteFmt = (GMMU_FMT_PTE*)pFmt->pPte;
    pLevelFmt = mmuFmtFindLevelWithPageShift(pFmt->pRoot, BIT_IDX_32(pageSize));

    oldKind = newKind = memdescGetPteKindForGpu(pMemDesc, pMappingGpu);
    if (pMemory)
    {
        //
        // The physical memory layout can be specified after allocation using
        // UvmMapExternalAllocation, so the kind attribute needs to be computed
        // again
        //
        status = nvGpuOpsGetPteKind(pMappingGpu, pMemoryManager, pMemDesc, pMemory,
                                    pGpuExternalMappingInfo, &newKind);

        if (status != NV_OK)
            return status;

        if (oldKind != newKind)
        {
            memdescSetPteKindForGpu(pMemDesc, pMappingGpu, newKind);
            kindChanged = NV_TRUE;
        }
    }

    // Get the CompTag range and Kind.
    status = memmgrGetKindComprForGpu_HAL(pMemoryManager, pMemDesc, pMappingGpu, 0, &kind, &comprInfo);
    if (status != NV_OK)
        return status;

    if (kindChanged)
        memdescSetPteKindForGpu(pMemDesc, pMappingGpu, oldKind);

    aperture = nvGpuOpsGetExternalAllocAperture(pMemDesc, isIndirectPeerSupported, isPeerSupported);

    vol = nvGpuOpsGetExternalAllocVolatility(pMemDesc, aperture, isIndirectPeerSupported,
                                             pGpuExternalMappingInfo->cachingType);

    status = nvGpuOpsGetExternalAllocMappingAttribute(pGpuExternalMappingInfo->mappingType,
                                                      pMemDesc,
                                                      &readOnly,
                                                      &atomic);
    if (status != NV_OK)
        return status;

    encrypted = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED);

    privileged = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_GPU_PRIVILEGED);

    mappingSize = size ? size : allocSize;

    skipPteCount = pLevelFmt->entrySize / sizeof(NvU64);

    isCompressedKind = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind);

    pteCount = NV_MIN((pGpuExternalMappingInfo->pteBufferSize / pLevelFmt->entrySize), (mappingSize / pageSize));
    if (!pteCount)
        return NV_ERR_BUFFER_TOO_SMALL;

    {
        if (nvFieldIsValid32(&pPteFmt->fldValid.desc))
            nvFieldSetBool(&pPteFmt->fldValid, NV_TRUE, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldVolatile.desc))
            nvFieldSetBool(&pPteFmt->fldVolatile, vol, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldPrivilege.desc))
            nvFieldSetBool(&pPteFmt->fldPrivilege, privileged, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldEncrypted.desc))
            nvFieldSetBool(&pPteFmt->fldEncrypted, encrypted, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldReadOnly.desc))
            nvFieldSetBool(&pPteFmt->fldReadOnly, readOnly, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldWriteDisable.desc))
            nvFieldSetBool(&pPteFmt->fldWriteDisable, readOnly, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldReadDisable.desc))
            nvFieldSetBool(&pPteFmt->fldReadDisable, NV_FALSE, pte.v8);

        if (nvFieldIsValid32(&pPteFmt->fldAtomicDisable.desc))
            nvFieldSetBool(&pPteFmt->fldAtomicDisable, !atomic, pte.v8);

        gmmuFieldSetAperture(&pPteFmt->fldAperture, aperture, pte.v8);

        if (!isCompressedKind)
        {
            nvFieldSet32(&pPteFmt->fldKind, kind, pte.v8);
            nvFieldSet32(&pPteFmt->fldCompTagLine, 0, pte.v8);
            if (nvFieldIsValid32(&pPteFmt->fldCompTagSubIndex))
                nvFieldSet32(&pPteFmt->fldCompTagSubIndex, 0, pte.v8);
        }
    }

    if (aperture == GMMU_APERTURE_PEER)
    {
        FlaMemory* pFlaMemory = dynamicCast(pMemory, FlaMemory);
        nvFieldSet32(&pPteFmt->fldPeerIndex, peerId, pte.v8);

        if ((memdescGetAddressSpace(pMemDesc) == ADDR_FABRIC) ||
            (memdescGetAddressSpace(pMemDesc) == ADDR_FABRIC_V2) || pFlaMemory)
        {
            //
            // ADDR_FABRIC/ADDR_FABRIC_V2 memory descriptors are pre-encoded with the fabric base address
            // use NVLINK_INVALID_FABRIC_ADDR to avoid encoding twice
            //
            fabricBaseAddress = NVLINK_INVALID_FABRIC_ADDR;
        }
        else
        {
            KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pMemDesc->pGpu);
            if (pKernelNvlink == NULL)
            {
                fabricBaseAddress = NVLINK_INVALID_FABRIC_ADDR;
            }
            else
            {
                fabricBaseAddress = knvlinkGetUniqueFabricBaseAddress(pMemDesc->pGpu, pKernelNvlink);
            }
        }
    }

    //
    // Both memdescGetPhysAddr() and kgmmuEncodePhysAddr() have pretty high overhead.
    // To avoid it, allocate an array for the physical addresses and use the
    // flavors of the APIs that work on multiple addresses at a time.
    //
    // Notably the pteBuffer array could be re-used for that, but it gets a bit
    // tricky if skipPteCount is greater than 1 so just keep it simple.
    //
    physicalAddresses = portMemAllocNonPaged((NvU32)pteCount * sizeof(*physicalAddresses));
    if (physicalAddresses == NULL)
        return NV_ERR_NO_MEMORY;

    //
    // Ask for physical addresses for the GPU being mapped as it may not be the
    // same as the GPU owning the memdesc. This matters for sysmem as accessing
    // it requires IOMMU mappings to be set up and these are different for each
    // GPU. The IOMMU mappings are currently added by nvGpuOpsDupMemory().
    //
    memdescGetPhysAddrsForGpu(pMemDesc, pMappingGpu, AT_GPU, offset, pageSize, pteCount, physicalAddresses);
    kgmmuEncodePhysAddrs(pKernelGmmu, aperture, physicalAddresses, fabricBaseAddress, pteCount);


    //
    // Get information whether given physical address needs PLCable kind
    //
    if (IS_VIRTUAL_WITH_SRIOV(pMappingGpu) &&
        gpuIsWarBug200577889SriovHeavyEnabled(pMappingGpu) &&
        isCompressedKind &&
        !memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, comprInfo.kind))
    {
        guestPhysicalAddress = portMemAllocNonPaged((NvU32)pteCount * sizeof(*guestPhysicalAddress));
        if (guestPhysicalAddress == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        portMemSet(guestPhysicalAddress, 0, ((NvU32)pteCount * sizeof(*guestPhysicalAddress)));

        gpaOffset = offset;
        for (iter = 0; iter < pteCount; iter++)
        {
            guestPhysicalAddress[iter] = gpaOffset;
            gpaOffset += pageSize;
        }

        isPLCable = portMemAllocNonPaged((NvU32)pteCount * sizeof(*isPLCable));
        if (isPLCable == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        portMemSet(isPLCable, 0, ((NvU32)pteCount * sizeof(*isPLCable)));

        NV_RM_RPC_GET_PLCABLE_ADDRESS_KIND(pMappingGpu, guestPhysicalAddress, pageSize, (NvU32)pteCount,
                                           isPLCable, status);
        if (status != NV_OK)
            goto done;
    }

    for (iter = 0; iter < pteCount; iter++)
    {
        physAddr = physicalAddresses[iter];

        gmmuFieldSetAddress(gmmuFmtPtePhysAddrFld(pPteFmt, aperture),
                            physAddr,
                            pte.v8);

        if (isCompressedKind)
        {
            // We have to reset pte.v8 fields in care of partially compressed allocations
            // Otherwise, non-compressed PTEs will get bits from compressed PTEs
            if (pFmt->version <= GMMU_FMT_VERSION_2)
            {
                NvBool bIsWarApplied = NV_FALSE;
                NvU32  savedKind = comprInfo.kind;
                MemoryManager  *pMemoryManager = GPU_GET_MEMORY_MANAGER(pMappingGpu);
                KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pMappingGpu);
                const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
                    kmemsysGetStaticConfig(pMappingGpu, pKernelMemorySystem);

                nvFieldSet32(&pPteFmt->fldKind, 0, pte.v8);
                nvFieldSet32(&pPteFmt->fldCompTagLine, 0, pte.v8);
                if (nvFieldIsValid32(&pPteFmt->fldCompTagSubIndex))
                    nvFieldSet32(&pPteFmt->fldCompTagSubIndex, 0, pte.v8);

                if (pMemorySystemConfig->bUseRawModeComptaglineAllocation &&
                    pMemorySystemConfig->bDisablePlcForCertainOffsetsBug3046774 &&
                        !memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, comprInfo.kind))
                {
                    NvBool bEnablePlc = NV_TRUE;

                    if (IS_VIRTUAL_WITH_SRIOV(pMappingGpu) &&
                        gpuIsWarBug200577889SriovHeavyEnabled(pMappingGpu))
                    {
                        bEnablePlc = isPLCable[iter];
                    }
                    else
                    {
                        bEnablePlc = kmemsysIsPagePLCable_HAL(pMappingGpu, pKernelMemorySystem, offset, pageSize);
                    }

                    if (!bEnablePlc)
                    {
                        bIsWarApplied = NV_TRUE;
                        memmgrGetDisablePlcKind_HAL(pMemoryManager, &comprInfo.kind);
                    }
                }

                kgmmuFieldSetKindCompTags(GPU_GET_KERNEL_GMMU(pMappingGpu), pFmt, pLevelFmt, &comprInfo, physAddr,
                                          offset, mmuFmtVirtAddrToEntryIndex(pLevelFmt, offset), pte.v8);
                    //
                    // restore the kind to PLC if changd, since kind is associated with entire surface, and the WAR applies to
                    // individual pages in the surface.
                    if (bIsWarApplied)
                        comprInfo.kind = savedKind;
            }
        }

        portMemCopy(&pGpuExternalMappingInfo->pteBuffer[iter * skipPteCount], pLevelFmt->entrySize, pte.v8, pLevelFmt->entrySize);

        offset += pageSize;
    }

    pGpuExternalMappingInfo->numWrittenPtes = pteCount;
    pGpuExternalMappingInfo->numRemainingPtes = (mappingSize / pageSize) - pteCount;
    pGpuExternalMappingInfo->pteSize = pLevelFmt->entrySize;

done:
    portMemFree(physicalAddresses);

    portMemFree(guestPhysicalAddress);

    portMemFree(isPLCable);

    return status;
}

NV_STATUS nvGpuOpsGetExternalAllocPtes(struct gpuAddressSpace *vaSpace,
                                       NvHandle hMemory,
                                       NvU64 offset,
                                       NvU64 size,
                                       gpuExternalMappingInfo *pGpuExternalMappingInfo)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    Memory *pMemory = NULL;
    PMEMORY_DESCRIPTOR pMemDesc = NULL;
    OBJGPU *pMappingGpu = NULL;
    NvU32 peerId = 0;
    NvBool isSliSupported = NV_FALSE;
    NvBool isPeerSupported = NV_FALSE;
    NvBool isIndirectPeerSupported = NV_FALSE;
    OBJVASPACE *pVAS = NULL;
    FlaMemory *pFlaMemory = NULL;
    OBJGPU    *pSrcGpu = NULL;
    OBJGPU    *pPeerGpu = NULL;
    RsClient  *pClient;
    MEMORY_DESCRIPTOR *pAdjustedMemDesc = NULL;
    FABRIC_VASPACE *pFabricVAS = NULL;

    if (!pGpuExternalMappingInfo || !hMemory || !vaSpace)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      vaSpace->device->session->handle,
                                      &pClient,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if (status != NV_OK)
        goto done;

    status = nvGpuOpsGetMemoryByHandle(vaSpace->device->session->handle,
                                       hMemory,
                                       &pMemory);
    if (status != NV_OK)
        goto done;

    // RM client allocations can't have multiple subDevice memdescs.
    pMemDesc = pMemory->pMemDesc;
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    // Do not support mapping on anything other than sysmem/vidmem/fabric!
    if ((memdescGetAddressSpace(pMemDesc) != ADDR_SYSMEM) &&
        (memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM)  &&
        (memdescGetAddressSpace(pMemDesc) != ADDR_FABRIC) &&
        (memdescGetAddressSpace(pMemDesc) != ADDR_FABRIC_V2))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    status = CliSetGpuContext(vaSpace->device->session->handle,
                              vaSpace->device->handle,
                              &pMappingGpu,
                              NULL);
    if (status != NV_OK)
        goto done;

    pAdjustedMemDesc = pMemDesc;
    pFabricVAS       = dynamicCast(pMappingGpu->pFabricVAS, FABRIC_VASPACE);
    if (pFabricVAS != NULL)
    {
        status = fabricvaspaceGetGpaMemdesc(pFabricVAS, pMemDesc, pMappingGpu, &pAdjustedMemDesc);
        if (status != NV_OK)
            goto done;
    }

    // Check if P2P supported
    if ((memdescGetAddressSpace(pAdjustedMemDesc) == ADDR_FABRIC) ||
        (memdescGetAddressSpace(pAdjustedMemDesc) == ADDR_FABRIC_V2))
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pMappingGpu);

        isPeerSupported = NV_TRUE;
        pPeerGpu        = pAdjustedMemDesc->pGpu;
        peerId          = BUS_INVALID_PEER;

        if (pPeerGpu != NULL)
        {
            if ((pKernelNvlink != NULL) &&
                knvlinkIsNvlinkP2pSupported(pMappingGpu, pKernelNvlink, pPeerGpu))
            {
                peerId = kbusGetPeerId_HAL(pMappingGpu, GPU_GET_KERNEL_BUS(pMappingGpu), pPeerGpu);
            }
        }

        if (peerId == BUS_INVALID_PEER)
        {
            status = NV_ERR_INVALID_STATE;
            goto freeGpaMemdesc;
        }
    }
    else if (memdescGetAddressSpace(pAdjustedMemDesc) == ADDR_FBMEM &&
             (pAdjustedMemDesc->pGpu->gpuId != pMappingGpu->gpuId ||
              dynamicCast(pMemory, FlaMemory)))
    {
        if (gpumgrCheckIndirectPeer(pAdjustedMemDesc->pGpu, pMappingGpu))
        {
            isIndirectPeerSupported = NV_TRUE;
        }
        else
        {
            pFlaMemory = dynamicCast(pMemory, FlaMemory);
            if (pFlaMemory != NULL)
            {
                pSrcGpu = gpumgrGetGpu(pFlaMemory->peerGpuInst);
                if (!pSrcGpu)
                {
                    status = NV_ERR_INVALID_ARGUMENT;
                    goto freeGpaMemdesc;
                }
            }

            status = nvGpuOpsGetExternalAllocP2pInfo(vaSpace->device->session,
                                                       (pFlaMemory) ? (pSrcGpu->gpuId) :(pAdjustedMemDesc->pGpu->gpuId),
                                                       pMappingGpu->gpuId,
                                                       &isPeerSupported,
                                                       &peerId);
            if (status != NV_OK)
                goto freeGpaMemdesc;
        }

        //
        // If GPUs are in the same SLI group, don't do peer mappings even if the GPUs are different. In SLI config,
        // if a caller can try to map a memory on a GPU other than the GPU which is associated with the memdesc,
        // always return local VIDMEM mapping because RM shares a memdesc among such GPUs for client allocations.
        // Note: This check could be avoided if we could know that pMemDesc->pGpu is always the SLI master i.e. same
        // as the pGPU returned by CliSetGpuContext.
        //
        if (!pFlaMemory && pAdjustedMemDesc->pGpu->deviceInstance == pMappingGpu->deviceInstance)
        {
            isPeerSupported = NV_FALSE;
            isSliSupported = NV_TRUE;
        }

        // Even if the RM returns P2P or indirect peer supported, make sure the GPUs are not from different SLI groups. See Bug# 759980.
        if ((isPeerSupported || isIndirectPeerSupported) &&
            (IsSLIEnabled(pMappingGpu) || IsSLIEnabled(pAdjustedMemDesc->pGpu)))
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto freeGpaMemdesc;
        }

        NV_ASSERT(!(isPeerSupported && isSliSupported));

        // If a caller is trying to map VIDMEM on GPUs with no P2P support and are not in the same SLI group, error out.
        if (!isPeerSupported && !isIndirectPeerSupported && !isSliSupported)
        {
            status = NV_ERR_NOT_SUPPORTED;
            goto freeGpaMemdesc;
        }
    }

    status = nvGpuOpsBuildExternalAllocPtes(pVAS, pMappingGpu, pAdjustedMemDesc, pMemory, offset, size,
                                            isIndirectPeerSupported, isPeerSupported, peerId,
                                            pGpuExternalMappingInfo);

freeGpaMemdesc:
    if (pAdjustedMemDesc != pMemDesc)
        fabricvaspacePutGpaMemdesc(pFabricVAS, pAdjustedMemDesc);

done:
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

void nvGpuOpsAddressSpaceDestroy(struct gpuAddressSpace *vaSpace)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_ASSERT(vaSpace->dummyGpuAlloc.refCount == 0);

    // free all the mallocs
    if (vaSpace->allocations)
    {
        portSyncRwLockAcquireWrite(vaSpace->allocationsLock);
        destroyAllGpuMemDescriptors(vaSpace->device->session->handle,
                                    vaSpace->allocations);
        portSyncRwLockReleaseWrite(vaSpace->allocationsLock);
    }

    // free all the physical allocations
    if (vaSpace->physAllocations)
    {
        portSyncRwLockAcquireWrite(vaSpace->physAllocationsLock);
        destroyAllGpuMemDescriptors(vaSpace->device->session->handle,
                                    vaSpace->physAllocations);
        portSyncRwLockReleaseWrite(vaSpace->physAllocationsLock);
    }

    // Destroy CPU mappings
    if (vaSpace->cpuMappings)
    {
        portSyncRwLockAcquireWrite(vaSpace->cpuMappingsLock);
        btreeDestroyData(vaSpace->cpuMappings);
        portSyncRwLockReleaseWrite(vaSpace->cpuMappingsLock);
    }

    if (vaSpace->handle)
        pRmApi->Free(pRmApi, vaSpace->device->session->handle, vaSpace->handle);

    portSyncRwLockDestroy(vaSpace->allocationsLock);
    portSyncRwLockDestroy(vaSpace->cpuMappingsLock);
    portSyncRwLockDestroy(vaSpace->physAllocationsLock);

    portMemFree(vaSpace);
}

static NV_STATUS nvGpuOpsAllocPhysical(struct gpuDevice *device,
                                       NvBool isSystemMemory,
                                       NvLength length,
                                       NvU64 *paOffset,
                                       gpuAllocInfo *allocInfo)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = {0};
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NvHandle physHandle  = 0;

    NV_ASSERT(allocInfo);
    NV_ASSERT(device);
    NV_ASSERT(paOffset);

    // then allocate the physical memory in either sysmem or fb.
    memAllocParams.owner = HEAP_OWNER_RM_KERNEL_CLIENT;

    // Physical allocations don't expect vaSpace handles
    memAllocParams.hVASpace = 0;

    // Reset previous offset
    memAllocParams.offset = 0;

    memAllocParams.size = length;
    memAllocParams.type = NVOS32_TYPE_IMAGE;
    memAllocParams.attr = isSystemMemory ?
                                      DRF_DEF(OS32, _ATTR, _LOCATION, _PCI) :
                                      DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);

    // Always enable caching for System Memory as all the currently supported
    // platforms are IO coherent.
    memAllocParams.attr |= isSystemMemory ?
                                       DRF_DEF(OS32, _ATTR, _COHERENCY, _CACHED):
                                       DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);

    // Allocate contigous allocation if requested by client
    memAllocParams.attr |= allocInfo->bContiguousPhysAlloc ?
                                       DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS):
                                       DRF_DEF(OS32, _ATTR, _PHYSICALITY, _DEFAULT);

    // Set pageSize for PA-allocation. RM default is Big page size
    switch (allocInfo->pageSize)
    {
        case RM_PAGE_SIZE:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB);
            break;
        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG);
            break;
        case RM_PAGE_SIZE_HUGE:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB);
            break;
        case RM_PAGE_SIZE_512M:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB);
            break;
        default:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT);
            break;
    }

    // Set the alignment
    if (allocInfo->alignment)
    {
        memAllocParams.flags |= NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
        memAllocParams.alignment = allocInfo->alignment;
    }

    // Do we have a range Hint ?
    if (allocInfo->rangeBegin != allocInfo->rangeEnd)
    {
        memAllocParams.flags |= NVOS32_ALLOC_FLAGS_USE_BEGIN_END;
        memAllocParams.rangeLo = allocInfo->rangeBegin;
        memAllocParams.rangeHi = allocInfo->rangeEnd;
    }

    // Do we need to allocate at top of FB
    if (allocInfo->bMemGrowsDown)
        memAllocParams.flags |= NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN;

    // Ask RM to allocate persistent video memory
    if (!isSystemMemory && allocInfo->bPersistentVidmem)
        memAllocParams.flags |= NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM;

    //
    // vid heap ctrl has a different policy as compared to other internal APIS
    // it expects the gpu lock to not be held. This means we have to drop the gpu lock
    // here. It is safe in this scenario because we still have the API lock and nothing
    // from a GPU interrupt can change anything in the OPS state.
    //

    physHandle = NV01_NULL_OBJECT;
    NV_ASSERT_OK_OR_GOTO(status, pRmApi->Alloc(pRmApi,
                                                device->session->handle,
                                                isSystemMemory ? device->handle : device->subhandle,
                                                &physHandle,
                                                isSystemMemory ? NV01_MEMORY_SYSTEM : NV01_MEMORY_LOCAL_USER,
                                                &memAllocParams), done);
    if (allocInfo->bContiguousPhysAlloc)
        allocInfo->gpuPhysOffset = memAllocParams.offset;

    allocInfo->hPhysHandle = physHandle;
    *paOffset = (NvU64)allocInfo->gpuPhysOffset;

done:

    if (status != NV_OK)
        pRmApi->Free(pRmApi, device->session->handle, physHandle);

    return status;
}

// The call allocates a virtual memory and associates a PA with it.
static NV_STATUS nvGpuOpsAllocVirtual(struct gpuAddressSpace *vaSpace,
                                      NvLength length,
                                      NvU64 *vaOffset,
                                      NvHandle physHandle,
                                      struct allocFlags flags,
                                      gpuVaAllocInfo *allocInfo)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { 0 };
    NV_STATUS status;
    gpuMemDesc *memDesc = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_ASSERT(allocInfo);
    NV_ASSERT(vaSpace);
    NV_ASSERT(vaOffset);
    NV_ASSERT(physHandle);

    memDesc = portMemAllocNonPaged(sizeof(*memDesc));
    if (memDesc == NULL)
        return NV_ERR_NO_MEMORY;

    // first allocate the virtual memory

    memAllocParams.owner = HEAP_OWNER_RM_KERNEL_CLIENT;
    memAllocParams.size = length;
    memAllocParams.type = NVOS32_TYPE_IMAGE;
    memAllocParams.alignment = NV_GPU_SMALL_PAGESIZE;
    memAllocParams.flags = NVOS32_ALLOC_FLAGS_VIRTUAL |
                                       NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE |
                                       NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED;

    if (allocInfo->bFixedAddressAllocate)
    {
        memAllocParams.flags |= NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
        memAllocParams.offset = allocInfo->vaStart;
    }

    // Set pageSize for VA-allocation. RM default is Big page size
    switch (allocInfo->pageSize)
    {
        case RM_PAGE_SIZE:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB);
            break;
        case RM_PAGE_SIZE_64K:
        case RM_PAGE_SIZE_128K:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG);
            break;
        case RM_PAGE_SIZE_HUGE:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB);
            break;
        case RM_PAGE_SIZE_512M:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB);
            break;
        default:
            memAllocParams.attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _DEFAULT);
            break;
    }

    memAllocParams.hVASpace = vaSpace->handle;

    memDesc->handle = NV01_NULL_OBJECT;
    NV_ASSERT_OK_OR_GOTO(status, pRmApi->Alloc(pRmApi,
                                                vaSpace->device->session->handle,
                                                vaSpace->device->handle,
                                                &memDesc->handle,
                                                NV50_MEMORY_VIRTUAL,
                                                &memAllocParams), done);
    memDesc->address = (NvU64)memAllocParams.offset;
    memDesc->size = length;
    memDesc->childHandle = physHandle;

    portSyncRwLockAcquireWrite(vaSpace->allocationsLock);
    status = trackDescriptor(&vaSpace->allocations, memDesc->address, memDesc);
    portSyncRwLockReleaseWrite(vaSpace->allocationsLock);

    if (status != NV_OK)
        goto done;

    // return the allocated GPU VA
    *vaOffset = memDesc->address;

done:

    if (status != NV_OK)
        pRmApi->Free(pRmApi, vaSpace->device->session->handle, memDesc->handle);

    if ((status != NV_OK) && (memDesc != NULL))
        portMemFree(memDesc);

    return status;
}

// will need to support offset within allocation
static NV_STATUS nvGpuOpsMapGpuMemory(struct gpuAddressSpace *vaSpace,
                                      NvU64 vaOffset,
                                      NvLength length,
                                      NvU32 pageSize,
                                      NvU64 *gpuOffset,
                                      struct allocFlags flags)
{
    gpuMemDesc *memDescVa = NULL;
    NV_STATUS status;
    NvU64 mappedVa = 0;
    NvU32 mapFlags = 0;
    NvU32 mapPageSize = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!vaSpace || !gpuOffset)
        return NV_ERR_INVALID_ARGUMENT;

    portSyncRwLockAcquireRead(vaSpace->allocationsLock);
    status = findDescriptor(vaSpace->allocations, vaOffset, (void**)&memDescVa);
    portSyncRwLockReleaseRead(vaSpace->allocationsLock);
    if (status != NV_OK)
        return status;

    NV_ASSERT(memDescVa);
    NV_ASSERT(memDescVa->handle);
    NV_ASSERT(memDescVa->childHandle);

    if (pageSize == RM_PAGE_SIZE)
    {
        mapPageSize |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _4KB);
    }
    else if (pageSize == RM_PAGE_SIZE_HUGE)
    {
        // TODO: this flag is ignored, remove it once it is deprecated
        mapPageSize |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _HUGE);
    }
    else
    {
       mapPageSize |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _DEFAULT);
    }

     // map the 2 surfaces
    mapFlags |= ((flags.bGetKernelVA) ? DRF_DEF(OS46, _FLAGS, _KERNEL_MAPPING, _ENABLE) :
            DRF_DEF(OS46, _FLAGS, _KERNEL_MAPPING, _NONE));
    mapFlags |= mapPageSize;

    // Always enable snooping as that's what's needed for sysmem allocations and
    // it's ignored for vidmem.
    mapFlags |= DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);

    // map the 2 surfaces
    status = pRmApi->Map(pRmApi,
                         vaSpace->device->session->handle,
                         vaSpace->device->handle,
                         memDescVa->handle,
                         memDescVa->childHandle,
                         0,
                         length,
                         mapFlags,
                         &mappedVa);
    if (status != NV_OK)
        return status;

    NV_ASSERT(memDescVa->address == mappedVa);

    *gpuOffset = memDescVa->address;

    return NV_OK;
}

//
// This function provides a gpu virtual address to a physical region
// that can either be in sysmem or vidmem.
//
static NV_STATUS nvGpuOpsGpuMalloc(struct gpuAddressSpace *vaSpace,
                                   NvBool isSystemMemory,
                                   NvLength length,
                                   NvU64 *gpuOffset,
                                   struct allocFlags flags,
                                   gpuAllocInfo *allocInfo)
{
    NV_STATUS status;
    NvU64 vaOffset = 0;
    NvU64 paOffset = 0;
    gpuVaAllocInfo vaAllocInfo = { 0 };
    NvHandle paMemDescHandle;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_ASSERT(allocInfo);
    NV_ASSERT(vaSpace);
    NV_ASSERT(gpuOffset);

    // Allocate physical memory first. So that we can associate PA with the memDesc of VA.
    // This simplifies tracking of VA and PA handles.
    status = nvGpuOpsAllocPhysical(vaSpace->device, isSystemMemory, length,
                                   &paOffset, allocInfo);
    if (status != NV_OK)
        return status;

    NV_ASSERT(allocInfo->hPhysHandle);

    paMemDescHandle = allocInfo->hPhysHandle;
    vaAllocInfo.pageSize = allocInfo->pageSize;

    status = nvGpuOpsAllocVirtual(vaSpace, length, &vaOffset, paMemDescHandle,
                                  flags, &vaAllocInfo);
    if (status != NV_OK)
        goto cleanup_physical;

    status = nvGpuOpsMapGpuMemory(vaSpace, vaOffset, length,
                                  allocInfo->pageSize, gpuOffset, flags);
    if (status != NV_OK)
        goto cleanup_virtual;

    return NV_OK;

cleanup_virtual:
    nvGpuOpsFreeVirtual(vaSpace, vaOffset);
cleanup_physical:
    pRmApi->Free(pRmApi, vaSpace->device->session->handle, paMemDescHandle);
    return status;
}

static void nvGpuOpsFreeVirtual(struct gpuAddressSpace *vaSpace, NvU64 vaOffset)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    gpuMemDesc *memDescVa = NULL;
    portSyncRwLockAcquireWrite(vaSpace->allocationsLock);
    deleteDescriptor(&vaSpace->allocations, vaOffset, (void**)&memDescVa);
    portSyncRwLockReleaseWrite(vaSpace->allocationsLock);
    NV_ASSERT(memDescVa);
    pRmApi->Free(pRmApi, vaSpace->device->session->handle, memDescVa->handle);
    portMemFree(memDescVa);
}

NV_STATUS nvGpuOpsMemoryAllocFb(struct gpuAddressSpace *vaSpace,
                                NvLength length,
                                NvU64 *gpuOffset,
                                gpuAllocInfo *allocInfo)
{
    gpuAllocInfo allocInfoTemp = {0};
    gpuAllocInfo *pAllocInfo;
    struct allocFlags flags = {0};

    if (!vaSpace || !gpuOffset)
        return NV_ERR_INVALID_ARGUMENT;

    // Use default settings if user hasn't provided one.
    if (allocInfo == NULL)
    {
        pAllocInfo = &allocInfoTemp;
    }
    else
    {
        pAllocInfo = allocInfo;
    }

    return nvGpuOpsGpuMalloc(vaSpace, NV_FALSE, length, gpuOffset, flags,
                               pAllocInfo);
}

NV_STATUS nvGpuOpsMemoryAllocSys(struct gpuAddressSpace *vaSpace,
                                 NvLength length,
                                 NvU64 *gpuOffset,
                                 gpuAllocInfo *allocInfo)
{
    gpuAllocInfo allocInfoTemp = {0};
    gpuAllocInfo *pAllocInfo;
    struct allocFlags flags = {0};

    if (!vaSpace || !gpuOffset)
        return NV_ERR_INVALID_ARGUMENT;

    // Use default settings if user hasn't provided one.
    if (allocInfo == NULL)
    {
        pAllocInfo = &allocInfoTemp;
    }
    else
    {
        pAllocInfo = allocInfo;
    }

    return nvGpuOpsGpuMalloc(vaSpace, NV_TRUE, length, gpuOffset, flags,
                             pAllocInfo);
}

NV_STATUS nvGpuOpsMemoryReopen(struct gpuAddressSpace *vaSpace,
                               NvHandle hSrcClient,
                               NvHandle hSrcAllocation,
                               NvLength length,
                               NvU64 *gpuOffset)
{
    NV_STATUS status;
    NvHandle hAllocation = 0;
    gpuVaAllocInfo allocInfoTemp = { 0 };
    struct allocFlags flags = { 0 };
    NvU64 vaOffset;
    NvHandle hVirtual = 0;
    RsResourceRef *pResourceRef;
    NvU64 addressOffset = 0;
    NvHandle hParent;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    // find device type
    // TODO: Acquired because serverutilGetResourceRef expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;

    status = serverutilGetResourceRef(hSrcClient, hSrcAllocation, &pResourceRef);
    if (status != NV_OK)
    {
        rmapiLockRelease();
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (!dynamicCast(pResourceRef->pResource, Memory))
    {
        rmapiLockRelease();
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    hParent = pResourceRef->pParentRef ? pResourceRef->pParentRef->hResource : 0;

    status = serverutilGetResourceRef(hSrcClient, hParent, &pResourceRef);
    rmapiLockRelease();
    if (status != NV_OK || !dynamicCast(pResourceRef->pResource, Device))
        return NV_ERR_GENERIC;

    if (!vaSpace || !gpuOffset || !hSrcAllocation || !hSrcClient)
        return NV_ERR_INVALID_ARGUMENT;

    // Dup the physical memory object
    hAllocation = NV01_NULL_OBJECT;
    status = pRmApi->DupObject(pRmApi,
                               vaSpace->device->session->handle,
                               vaSpace->device->handle,
                               &hAllocation,
                               hSrcClient,
                               hSrcAllocation,
                               NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);
    if (status != NV_OK)
        return status;

    // Associate the duped object with the newly created virtual memory object
    status = nvGpuOpsAllocVirtual(vaSpace, length, &vaOffset, hAllocation,
                                  flags, &allocInfoTemp);
    if (status != NV_OK)
        goto cleanup_dup;

    status = getHandleForVirtualAddr(vaSpace, vaOffset, NV_FALSE, &hVirtual);
    if (status != NV_OK)
        goto cleanup_virt_allocation;

    // map the memory
    status = pRmApi->Map(pRmApi,
                         vaSpace->device->session->handle,
                         vaSpace->device->handle,
                         hVirtual,
                         hAllocation,
                         0,
                         length,
                         0,
                         &addressOffset);
    if (status != NV_OK)
        goto cleanup_virt_allocation;

    NV_ASSERT((vaOffset == addressOffset) && "nvGpuOpsMemoryReopen: VA offset Mistmatch!");

    // return the mapped GPU pointer
    *gpuOffset = vaOffset;

    return NV_OK;

cleanup_virt_allocation:
    nvGpuOpsFreeVirtual(vaSpace, vaOffset);
cleanup_dup:
    pRmApi->Free(pRmApi, vaSpace->device->session->handle, hAllocation);
    return status;
}

NV_STATUS nvGpuOpsPmaAllocPages(void *pPma, NvLength pageCount, NvU32 pageSize,
                                gpuPmaAllocationOptions *pPmaAllocOptions,
                                NvU64 *pPages)
{
    NV_STATUS status;
    gpuPmaAllocationOptions pmaAllocOptionsTemp = {0};
    gpuPmaAllocationOptions *pAllocInfo;
    THREAD_STATE_NODE threadState;

    if (!pPma || !pPages)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // Use default settings if user hasn't provided one.
    if (NULL == pPmaAllocOptions)
    {
        pAllocInfo = &pmaAllocOptionsTemp;
    }
    else
    {
        pAllocInfo = pPmaAllocOptions;
    }

    // Invoke PMA module to alloc pages.
    status = pmaAllocatePages((PMA *)pPma,
                              pageCount,
                              pageSize,
                              (PMA_ALLOCATION_OPTIONS *)pAllocInfo,
                              pPages);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

//
// When this API is called from UVM as part of PMA eviction, the thread state
// should have been initialized already and recursive re-init needs to be
// skipped as it's not supported.
//
NV_STATUS nvGpuOpsPmaPinPages(void *pPma,
                              NvU64 *pPages,
                              NvLength pageCount,
                              NvU32 pageSize,
                              NvU32 flags)
{
    NV_STATUS status;
    THREAD_STATE_NODE threadState;
    NvBool pmaEvictionCall = (flags & UVM_PMA_CALLED_FROM_PMA_EVICTION) != 0;

    if (!pPma || !pPages)
        return NV_ERR_INVALID_ARGUMENT;

    if (!pmaEvictionCall)
        threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // Invoke PMA module to Pin pages.
    status = pmaPinPages((PMA *)pPma, pPages, pageCount, pageSize);

    if (!pmaEvictionCall)
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsPmaUnpinPages(void *pPma,
                                NvU64 *pPages,
                                NvLength pageCount,
                                NvU32 pageSize)
{
    NV_STATUS status;
    THREAD_STATE_NODE threadState;
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (!pPma || !pPages)
        return NV_ERR_INVALID_ARGUMENT;

    // Invoke PMA module to Unpin pages.
    status = pmaUnpinPages((PMA *)pPma, pPages, pageCount, pageSize);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

//
// When this API is called from UVM as part of PMA eviction, the thread state
// should have been initialized already and recursive re-init needs to be
// skipped as it's not supported.
//
void nvGpuOpsPmaFreePages(void *pPma,
                          NvU64 *pPages,
                          NvLength pageCount,
                          NvU32 pageSize,
                          NvU32 flags)
{
    THREAD_STATE_NODE threadState;
    NvU32 pmaFreeFlag = ((flags & UVM_PMA_FREE_IS_ZERO) ? PMA_FREE_SKIP_SCRUB : 0);
    NvBool pmaEvictionCall = (flags & UVM_PMA_CALLED_FROM_PMA_EVICTION) != 0;

    if (!pmaEvictionCall)
        threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (!pPma || !pPages)
        return;

    // Invoke PMA module to free pages.
    if (flags & UVM_PMA_ALLOCATE_CONTIGUOUS)
        pmaFreePages((PMA *)pPma, pPages, 1, pageCount * pageSize, pmaFreeFlag);
    else
        pmaFreePages((PMA *)pPma, pPages, pageCount, pageSize, pmaFreeFlag);

    if (!pmaEvictionCall)
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
}

static NV_STATUS nvGpuOpsChannelGetHwChannelId(struct gpuChannel *channel,
                                               NvU32 *hwChannelId)
{
    NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS params = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    params.numChannels        = 1;
    params.pChannelHandleList = NV_PTR_TO_NvP64(&channel->channelHandle);
    params.pChannelList       = NV_PTR_TO_NvP64(hwChannelId);

    return pRmApi->Control(pRmApi,
                           channel->vaSpace->device->session->handle,
                           channel->vaSpace->device->handle,
                           NV0080_CTRL_CMD_FIFO_GET_CHANNELLIST,
                           &params,
                           sizeof(params));
}

static void gpuDeviceUnmapCpuFreeHandle(struct gpuDevice *device,
                                        NvHandle handle,
                                        void *ptr,
                                        NvU32 flags)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuSession *session = device->session;

    // Unmap the pointer
    if (ptr)
    {
        NV_STATUS status;
        const NvU32 pid = osGetCurrentProcess();

        status = pRmApi->UnmapFromCpu(pRmApi, session->handle, device->subhandle, handle, ptr, flags, pid);
        NV_ASSERT(status == NV_OK);
    }

    // Free the handle
    if (handle)
        pRmApi->Free(pRmApi, session->handle, handle);
}

static void gpuDeviceDestroyUsermodeRegion(struct gpuDevice *device)
{
    subDeviceDesc *rmSubDevice = device->rmSubDevice;

    gpuDeviceUnmapCpuFreeHandle(device,
                                rmSubDevice->clientRegionHandle,
                                (void *)rmSubDevice->clientRegionMapping,
                                0);
}

static NV_STATUS gpuDeviceMapUsermodeRegion(struct gpuDevice *device)
{
    NV_STATUS status = NV_OK;
    NvHandle regionHandle = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuSession *session = device->session;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;

    NV_ASSERT(isDeviceVoltaPlus(device));
    NV_ASSERT(rmSubDevice->clientRegionHandle == 0 && rmSubDevice->clientRegionMapping == NULL);

    regionHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi,
                           session->handle,
                           device->subhandle,
                           &regionHandle,
                           VOLTA_USERMODE_A,
                           NULL);
    if (NV_OK != status)
        return status;

    status = pRmApi->MapToCpu(pRmApi,
                              session->handle,
                              device->subhandle,
                              regionHandle,
                              0,
                              NVC361_NV_USERMODE__SIZE,
                              (void **)(&rmSubDevice->clientRegionMapping),
                              DRF_DEF(OS33, _FLAGS, _ACCESS, _WRITE_ONLY));
    if (NV_OK != status)
        goto failure_case;

    rmSubDevice->clientRegionHandle = regionHandle;
    return status;

failure_case:
    pRmApi->Free(pRmApi, device->session->handle, regionHandle);
    return status;
}

//
// In Volta+, a channel can submit work by "ringing a doorbell" on the gpu after
// updating the GP_PUT. The doorbell is a register mapped in the client's address
// space and can be shared by all channels in that address space. Each channel writes
// a channel-specific token to the doorbell to trigger the work.
//
static NV_STATUS nvGpuOpsGetWorkSubmissionInfo(struct gpuAddressSpace *vaSpace,
                                               struct gpuChannel *channel)
{
    NV_STATUS status = NV_OK;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS params = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuDevice *device = vaSpace->device;
    struct gpuSession *session = device->session;
    subDeviceDesc *rmSubDevice = device->rmSubDevice;

    // Only valid for VOLTA+ (sub)Devices.
    NV_ASSERT(isDeviceVoltaPlus(vaSpace->device));

    // Now get the token for submission on given channel.
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             channel->channelHandle,
                             NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN,
                             &params,
                             sizeof(params));
    if (status != NV_OK)
        return status;

    channel->workSubmissionOffset = (NvU32 *)((NvU8*)rmSubDevice->clientRegionMapping + NVC361_NOTIFY_CHANNEL_PENDING);
    channel->workSubmissionToken = params.workSubmitToken;

    //
    // pWorkSubmissionToken cannot be NULL even if errorNotifier is NULL.
    // errorNotifier is checked for NULL previously, so just an assert is
    // sufficient.
    //
    NV_ASSERT_OR_RETURN((channel->errorNotifier != NULL), NV_ERR_INVALID_POINTER);

    channel->pWorkSubmissionToken =
        (NvU32 *)((NvU8 *)channel->errorNotifier +
            (NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN * sizeof(NvNotification)) +
            NV_OFFSETOF(NvNotification, info32));

    return status;
}

static NvBool channelNeedsDummyAlloc(struct gpuChannel *channel)
{
    return channel->gpPutLoc == UVM_BUFFER_LOCATION_SYS && deviceNeedsDummyAlloc(channel->vaSpace->device);
}

static NV_STATUS channelRetainDummyAlloc(struct gpuChannel *channel, gpuChannelInfo *channelInfo)
{
    struct gpuAddressSpace *vaSpace = channel->vaSpace;
    NV_STATUS status;

    if (!channelNeedsDummyAlloc(channel))
        return NV_OK;

    status = nvGpuOpsVaSpaceRetainDummyAlloc(vaSpace);
    if (status != NV_OK)
        return status;

    channel->retainedDummyAlloc = NV_TRUE;
    channelInfo->dummyBar1Mapping = vaSpace->dummyGpuAlloc.cpuAddr;

    return NV_OK;
}

static void channelReleaseDummyAlloc(struct gpuChannel *channel)
{
    if (channel != NULL && channel->retainedDummyAlloc)
    {
        NV_ASSERT(channelNeedsDummyAlloc(channel));
        nvGpuOpsVaSpaceReleaseDummyAlloc(channel->vaSpace);
    }
}

static NvU32 channelEngineType(const struct gpuChannel *channel)
{
    if (channel->engineType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE)
        return NV2080_ENGINE_TYPE_COPY(channel->engineIndex);
    else if (channel->engineType == UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2)
        return NV2080_ENGINE_TYPE_SEC2;
    else
        return NV2080_ENGINE_TYPE_GR(channel->engineIndex);
}

static NV_STATUS channelAllocate(struct gpuAddressSpace *vaSpace,
                                 UVM_GPU_CHANNEL_ENGINE_TYPE engineType,
                                 const gpuChannelAllocParams *params,
                                 struct gpuChannel **channelHandle,
                                 gpuChannelInfo *channelInfo)
{
    NV_STATUS status;
    struct gpuChannel *channel = NULL;
    struct gpuDevice *device = NULL;
    struct gpuSession *session = NULL;
    void *cpuMap = NULL;
    NvHandle hErrorNotifier;
    struct ChannelAllocInfo *pAllocInfo = NULL;
    void *gpfifoCtrl = NULL;
    PCLI_DMA_MAPPING_INFO pDmaMappingInfo = NULL;
    struct allocFlags flags = {0};
    OBJGPU *pGpu = NULL;
    KernelFifo *pKernelFifo = NULL;
    NvU32 pid = osGetCurrentProcess();
    NvU32 subdeviceInstance;
    UVM_BUFFER_LOCATION gpFifoLoc;
    UVM_BUFFER_LOCATION gpPutLoc;
    NvLength gpFifoSize, errorNotifierSize;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!vaSpace || !channelHandle || !params || !channelInfo)
        return NV_ERR_INVALID_ARGUMENT;

    if (params->numGpFifoEntries == 0)
        return NV_ERR_INVALID_ARGUMENT;

    if (engineType != UVM_GPU_CHANNEL_ENGINE_TYPE_CE &&
        engineType != UVM_GPU_CHANNEL_ENGINE_TYPE_GR &&
        engineType != UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2)
        return NV_ERR_INVALID_ARGUMENT;

    // TODO: Bug 2458492: Ampere-SMC Verify GR/CE indices within partition/SMC Engine

    device = vaSpace->device;
    NV_ASSERT(device);
    session = device->session;
    NV_ASSERT(session);

    // Set location defaults
    gpFifoLoc = UVM_BUFFER_LOCATION_SYS;
    if (device->fbInfo.bZeroFb)
        gpPutLoc = UVM_BUFFER_LOCATION_SYS;
    else
        gpPutLoc = UVM_BUFFER_LOCATION_VID;

    if (isDeviceVoltaPlus(device))
    {
        if (params->gpFifoLoc > UVM_BUFFER_LOCATION_VID)
            return NV_ERR_INVALID_ARGUMENT;
        if (params->gpPutLoc > UVM_BUFFER_LOCATION_VID)
            return NV_ERR_INVALID_ARGUMENT;

        if (params->gpFifoLoc != UVM_BUFFER_LOCATION_DEFAULT)
            gpFifoLoc = params->gpFifoLoc;
        if (params->gpPutLoc != UVM_BUFFER_LOCATION_DEFAULT)
            gpPutLoc = params->gpPutLoc;
    }
    else
    {
        // GPFIFO needs to be placed in sysmem on Pascal and
        // pre-Pascal devices (Bug 1750713)
        if (params->gpFifoLoc != UVM_BUFFER_LOCATION_DEFAULT || params->gpPutLoc != UVM_BUFFER_LOCATION_DEFAULT)
            return NV_ERR_INVALID_ARGUMENT;
    }

    // TODO: Acquired because CliSetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    pAllocInfo = portMemAllocNonPaged(sizeof(*pAllocInfo));
    if (pAllocInfo == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pAllocInfo, 0, sizeof(*pAllocInfo));

    subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    channel = portMemAllocNonPaged(sizeof(*channel));
    if (channel == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto cleanup_free_memory;
    }

    portMemSet(channel, 0, sizeof(*channel));

    channel->vaSpace = vaSpace;
    channel->fifoEntries = params->numGpFifoEntries;
    channel->gpFifoLoc   = gpFifoLoc;
    channel->gpPutLoc    = gpPutLoc;

    // Remember which engine we are using, so that RC recovery can reset it if
    // it hangs:
    channel->engineType = engineType;
    channel->engineIndex = params->engineIndex;

    gpFifoSize = (NvLength)params->numGpFifoEntries * NVA06F_GP_ENTRY__SIZE;

    // If the allocation is vidmem ask RM to allocate persistent vidmem
    pAllocInfo->gpuAllocInfo.bPersistentVidmem = NV_TRUE;

    // 1. Allocate the GPFIFO entries. Dont pass any special flags.
    flags.bGetKernelVA = NV_FALSE;
    status = nvGpuOpsGpuMalloc(vaSpace,
                               gpFifoLoc == UVM_BUFFER_LOCATION_SYS,
                               gpFifoSize,
                               &channel->gpFifo,
                               flags,
                               &pAllocInfo->gpuAllocInfo);
    if (status != NV_OK)
        goto cleanup_free_memory;

    // 2. Map the gpfifo entries
    status = nvGpuOpsMemoryCpuMap(vaSpace,
                                  channel->gpFifo,
                                  gpFifoSize,
                                  &cpuMap,
                                  PAGE_SIZE_DEFAULT);
    if (status != NV_OK)
        goto cleanup_free_gpfifo_entries;

    channel->gpFifoEntries = (NvU64 *) cpuMap;

    //
    // 3. Allocate memory for the error notifier. Make the allocation
    // sufficiently large to also accommodate any other channel
    // notifiers, and request a kernel VA and CPU caching.
    //
    flags.bGetKernelVA = NV_TRUE;
    errorNotifierSize = sizeof(NvNotification) *
                        NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1;
    status = nvGpuOpsGpuMalloc(vaSpace,
                               NV_TRUE,
                               errorNotifierSize,
                               &channel->errorNotifierOffset,
                               flags,
                               &pAllocInfo->gpuAllocInfo);
    if (status != NV_OK)
        goto cleanup_unmap_gpfifo_entries;

    NV_ASSERT(channel->errorNotifierOffset);

    status = getHandleForVirtualAddr(vaSpace,
                                     channel->errorNotifierOffset,
                                     NV_FALSE /*virtual*/,
                                     &hErrorNotifier);
    if (status != NV_OK)
        goto cleanup_free_virtual;

    // 4. Find and share the VA with UVM driver

    // TODO: Acquired because CliGetDmaMappingInfo expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        goto cleanup_free_virtual;

    if (!CliGetDmaMappingInfo(session->handle,
                              device->handle,
                              hErrorNotifier,
                              channel->errorNotifierOffset,
                              gpumgrGetDeviceGpuMask(device->deviceInstance),
                              &pDmaMappingInfo))
    {
        rmapiLockRelease();
        status = NV_ERR_GENERIC;
        goto cleanup_free_virtual;
    }

    rmapiLockRelease();

    //
    // RM uses the parent subdevice index to fill the notifier on SYSMEM. So use the same.
    // NOTE: the same assumption does not hold for VIDMEM allocations.
    //
    channel->errorNotifier = (NvNotification*)pDmaMappingInfo->KernelVAddr[subdeviceInstance];
    if (!channel->errorNotifier)
    {
        status = NV_ERR_GENERIC;
        goto cleanup_free_virtual;
    }

    // Let's allocate the channel
    pAllocInfo->gpFifoAllocParams.hObjectError  = hErrorNotifier;
    status = getHandleForVirtualAddr(vaSpace,
                                     channel->gpFifo,
                                     NV_FALSE /*virtual*/,
                                     &pAllocInfo->gpFifoAllocParams.hObjectBuffer);
    if (status != NV_OK)
        goto cleanup_free_virtual;

    pAllocInfo->gpFifoAllocParams.gpFifoOffset  = channel->gpFifo;
    pAllocInfo->gpFifoAllocParams.gpFifoEntries = channel->fifoEntries;
    // If zero then it will attach to the device address space
    pAllocInfo->gpFifoAllocParams.hVASpace = vaSpace->handle;
    pAllocInfo->gpFifoAllocParams.engineType = channelEngineType(channel);

    if (isDeviceVoltaPlus(device))
    {
        flags.bGetKernelVA = NV_FALSE;
        status = nvGpuOpsGpuMalloc(vaSpace,
                                   gpPutLoc == UVM_BUFFER_LOCATION_SYS,
                                   sizeof(KeplerAControlGPFifo),
                                   &channel->userdGpuAddr,
                                   flags,
                                   &pAllocInfo->gpuAllocInfo);
        if (status != NV_OK)
            goto cleanup_free_virtual;

        channel->hUserdPhysHandle = pAllocInfo->gpuAllocInfo.hPhysHandle;

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        pAllocInfo->gpFifoAllocParams.hUserdMemory[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = channel->hUserdPhysHandle;
        pAllocInfo->gpFifoAllocParams.userdOffset[gpumgrGetSubDeviceInstanceFromGpu(pGpu)] = 0;
        SLI_LOOP_END

        status = nvGpuOpsMemoryCpuMap(vaSpace,
                                      channel->userdGpuAddr,
                                      sizeof(KeplerAControlGPFifo),
                                      &gpfifoCtrl,
                                      PAGE_SIZE_DEFAULT);
        if (status != NV_OK)
            goto cleanup_free_virtual;
    }

    channel->channelHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi, session->handle,
                           device->handle,
                           &channel->channelHandle,
                           device->hostClass,
                           &pAllocInfo->gpFifoAllocParams);
    if (status != NV_OK)
    {
        goto cleanup_free_virtual;
    }

    // Query runlist ID
    pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    status = kfifoEngineInfoXlate_HAL(pGpu,
                                      pKernelFifo,
                                      ENGINE_INFO_TYPE_NV2080,
                                      channelEngineType(channel),
                                      ENGINE_INFO_TYPE_RUNLIST,
                                      &channel->hwRunlistId);
    if (status != NV_OK)
        goto cleanup_free_virtual;

    // Query channel ID
    status = nvGpuOpsChannelGetHwChannelId(channel, &channel->hwChannelId);
    if (status != NV_OK)
        goto cleanup_free_channel;

    // Map USERD (controlPage)
    if (!isDeviceVoltaPlus(device))
    {
        status = pRmApi->MapToCpu(pRmApi,
                                  session->handle,
                                  device->subhandle,
                                  channel->channelHandle,
                                  0,
                                  sizeof(KeplerAControlGPFifo),
                                  &gpfifoCtrl,
                                  0);
        if (status != NV_OK)
            goto cleanup_free_channel;
    }

    channel->controlPage = gpfifoCtrl;

    status = channelRetainDummyAlloc(channel, channelInfo);
    if (status != NV_OK)
        goto cleanup_free_controlpage;

    // Allocate the SW method class for fault cancel
    if (isDevicePascalPlus(device) && (engineType != UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2))
    {
        channel->hFaultCancelSwMethodClass = NV01_NULL_OBJECT;
        status = pRmApi->Alloc(pRmApi,
                               session->handle,
                               channel->channelHandle,
                               &channel->hFaultCancelSwMethodClass,
                               GP100_UVM_SW,
                               NULL);
        if (status != NV_OK)
            goto cleanup_free_controlpage;
    }

    portMemFree(pAllocInfo);

    *channelHandle = channel;
    channelInfo->gpGet = &channel->controlPage->GPGet;
    channelInfo->gpPut = &channel->controlPage->GPPut;
    channelInfo->gpFifoEntries = channel->gpFifoEntries;
    channelInfo->channelClassNum = device->hostClass;
    channelInfo->numGpFifoEntries = channel->fifoEntries;
    channelInfo->errorNotifier = channel->errorNotifier;
    channelInfo->hwRunlistId = channel->hwRunlistId;
    channelInfo->hwChannelId = channel->hwChannelId;

    return NV_OK;

cleanup_free_controlpage:
    if (!isDeviceVoltaPlus(device) && (gpfifoCtrl != NULL))
        pRmApi->UnmapFromCpu(pRmApi, session->handle, device->subhandle, channel->channelHandle, gpfifoCtrl, 0, pid);
cleanup_free_channel:
    pRmApi->Free(pRmApi, session->handle, channel->channelHandle);
cleanup_free_virtual:
    if (isDeviceVoltaPlus(device))
    {
        if (gpfifoCtrl != NULL)
            nvGpuOpsMemoryCpuUnMap(vaSpace, gpfifoCtrl);

        if (channel->userdGpuAddr != 0)
            nvGpuOpsMemoryFree(vaSpace, channel->userdGpuAddr);
    }

    nvGpuOpsMemoryFree(vaSpace, channel->errorNotifierOffset);
cleanup_unmap_gpfifo_entries:
    nvGpuOpsMemoryCpuUnMap(vaSpace, channel->gpFifoEntries);
cleanup_free_gpfifo_entries:
    nvGpuOpsMemoryFree(vaSpace, channel->gpFifo);
cleanup_free_memory:
    channelReleaseDummyAlloc(channel);
    portMemFree(channel);
    portMemFree(pAllocInfo);

    return status;
}

static NV_STATUS engineAllocate(struct gpuChannel *channel, gpuChannelInfo *channelInfo, UVM_GPU_CHANNEL_ENGINE_TYPE engineType)
{
    NV_STATUS status = NV_OK;
    struct gpuObject *object = NULL;
    NVB0B5_ALLOCATION_PARAMETERS ceAllocParams = {0};
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS channelGrpParams = {0};
    struct gpuAddressSpace *vaSpace = NULL;
    struct gpuDevice *device = NULL;
    struct gpuSession *session = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NvU32 class;
    void *params;

    NV_ASSERT(channel);
    NV_ASSERT(channelInfo);
    NV_ASSERT(channel->engineType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE ||
        channel->engineType == UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2);

    // TODO: Bug 2458492: Ampere-SMC Verify GR/CE indices within partition

    vaSpace = channel->vaSpace;
    NV_ASSERT(vaSpace);
    device = vaSpace->device;
    NV_ASSERT(device);
    session = device->session;
    NV_ASSERT(session);

    object = portMemAllocNonPaged(sizeof(*object));
    if (object == NULL)
        return NV_ERR_NO_MEMORY;

    object->handle = NV01_NULL_OBJECT;

    if (engineType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE)
    {
        ceAllocParams.version = NVB0B5_ALLOCATION_PARAMETERS_VERSION_1;
        ceAllocParams.engineType = NV2080_ENGINE_TYPE_COPY(channel->engineIndex);
        params = &ceAllocParams;
        class = device->ceClass;
    }
    else
    {
        params = NULL;
        class = device->sec2Class;
    }

    status = pRmApi->Alloc(pRmApi, session->handle,
                       channel->channelHandle,
                       &object->handle,
                       class,
                       params);

    if (status != NV_OK)
        goto cleanup_free_memory;

    // In volta+ gpus, the channel has a submission offset used as doorbell.
    if (isDeviceVoltaPlus(device))
    {
        status = nvGpuOpsGetWorkSubmissionInfo(vaSpace, channel);
        if (status != NV_OK)
            goto cleanup_free_engine;

        channelInfo->workSubmissionOffset = channel->workSubmissionOffset;
        channelInfo->workSubmissionToken = channel->workSubmissionToken;
        channelInfo->pWorkSubmissionToken = channel->pWorkSubmissionToken;
    }

    // Schedule the channel
    channelGrpParams.bEnable = NV_TRUE;
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             channel->channelHandle,
                             NVA06F_CTRL_CMD_GPFIFO_SCHEDULE,
                             &channelGrpParams,
                             sizeof(channelGrpParams));

    if (status != NV_OK)
        goto cleanup_free_engine;

    object->next = channel->nextAttachedEngine;
    channel->nextAttachedEngine = object;
    object->type = class;

    return NV_OK;

cleanup_free_engine:
    pRmApi->Free(pRmApi, session->handle, object->handle);
cleanup_free_memory:
    portMemFree(object);
    return status;
}

NV_STATUS nvGpuOpsChannelAllocate(struct gpuAddressSpace *vaSpace,
                                  const gpuChannelAllocParams *params,
                                  struct gpuChannel **channelHandle,
                                  gpuChannelInfo *channelInfo)
{
    NV_STATUS status;
    UVM_GPU_CHANNEL_ENGINE_TYPE channelType = params->engineType;

    NV_ASSERT_OR_RETURN((channelType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE || channelType == UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2), NV_ERR_NOT_SUPPORTED);

    status = channelAllocate(vaSpace, channelType, params,
                             channelHandle, channelInfo);
    if (status != NV_OK)
        return status;

    status = engineAllocate(*channelHandle, channelInfo, channelType);
    if (status != NV_OK)
        nvGpuOpsChannelDestroy(*channelHandle);

    return status;
}

void nvGpuOpsChannelDestroy(struct gpuChannel *channel)
{
    struct gpuObject *nextEngine;
    struct gpuObject *currEngine;
    NvU32 pid = osGetCurrentProcess();
    struct gpuAddressSpace *vaSpace = NULL;
    struct gpuDevice *device = NULL;
    struct gpuSession *session = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!channel)
        return;

    vaSpace = channel->vaSpace;
    NV_ASSERT(vaSpace);
    device = vaSpace->device;
    NV_ASSERT(device);
    session = device->session;
    NV_ASSERT(session);

    // destroy the engines under this channel
    if (channel->nextAttachedEngine)
    {
        currEngine = channel->nextAttachedEngine;
        nextEngine = currEngine;
        do
        {
            currEngine = nextEngine;
            nextEngine = currEngine->next;
            pRmApi->Free(pRmApi, session->handle, currEngine->handle);
            portMemFree(currEngine);
        } while (nextEngine != NULL);
    }

    // Tear down the channel
    if (isDevicePascalPlus(device))
        pRmApi->Free(pRmApi, session->handle, channel->hFaultCancelSwMethodClass);

    if (isDeviceVoltaPlus(device))
    {
        nvGpuOpsMemoryCpuUnMap(vaSpace, (void *)channel->controlPage);
        nvGpuOpsMemoryFree(vaSpace, channel->userdGpuAddr);
    }
    else
    {
        pRmApi->UnmapFromCpu(pRmApi,
                             session->handle,
                             device->subhandle,
                             channel->channelHandle,
                             (void *)channel->controlPage,
                             0,
                             pid);
    }

    // Free the channel
    pRmApi->Free(pRmApi, session->handle, channel->channelHandle);

    nvGpuOpsMemoryFree(vaSpace, channel->errorNotifierOffset);

    nvGpuOpsMemoryCpuUnMap(vaSpace, channel->gpFifoEntries);

    nvGpuOpsMemoryFree(vaSpace, channel->gpFifo);

    channelReleaseDummyAlloc(channel);

    portMemFree(channel);
}

static NV_STATUS trackDescriptor(PNODE *pRoot, NvU64 key, void *desc)
{
    PNODE btreeNode;
    NV_ASSERT(desc);
    NV_ASSERT(pRoot);

    btreeNode = (PNODE)desc;

    btreeNode->keyStart = key;
    btreeNode->keyEnd = key;
    btreeNode->Data = desc;
    return btreeInsert(btreeNode, pRoot);
}

static NV_STATUS findDescriptor(PNODE pRoot, NvU64 key, void **desc)
{
    PNODE btreeNode = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT(desc);

    status = btreeSearch(key, &btreeNode, pRoot);
    if (status != NV_OK)
        return status;

    *desc = btreeNode->Data;
    return NV_OK;
}

static NV_STATUS deleteDescriptor(PNODE *pRoot, NvU64 key, void **desc)
{
    PNODE btreeNode = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT(desc);
    NV_ASSERT(pRoot);

    status = btreeSearch(key, &btreeNode, *pRoot);
    if (status != NV_OK)
        return status ;

    *desc = btreeNode->Data;
    status = btreeUnlink(btreeNode, pRoot);
    return NV_OK;
}

static NV_STATUS destroyAllGpuMemDescriptors(NvHandle hClient, PNODE pNode)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    gpuMemDesc *memDesc = NULL;

    if (pNode == NULL)
        return NV_OK;

    destroyAllGpuMemDescriptors(hClient, pNode->left);
    destroyAllGpuMemDescriptors(hClient, pNode->right);

    memDesc = (gpuMemDesc*)pNode->Data;
    if (memDesc->childHandle)
        pRmApi->Free(pRmApi, hClient, memDesc->childHandle);

    if (memDesc->handle)
        pRmApi->Free(pRmApi, hClient, memDesc->handle);

    portMemFree(pNode->Data);

    return NV_OK;
}

// Returns childHandle/handle to a VA memdesc associated with a VA.
static NV_STATUS getHandleForVirtualAddr(struct gpuAddressSpace *vaSpace,
                                         NvU64 allocationAddress,
                                         NvBool bPhysical,
                                         NvHandle *pHandle)
{
    NV_STATUS status = NV_OK;
    gpuMemDesc *memDesc = NULL;

    NV_ASSERT(vaSpace);
    NV_ASSERT(pHandle);

    portSyncRwLockAcquireRead(vaSpace->allocationsLock);
    status = findDescriptor(vaSpace->allocations, allocationAddress, (void**)&memDesc);
    portSyncRwLockReleaseRead(vaSpace->allocationsLock);
    if (status != NV_OK)
        return status;

    NV_ASSERT(memDesc);

    *pHandle =  bPhysical ? memDesc->childHandle : memDesc->handle;

    if (!*pHandle)
        return NV_ERR_GENERIC;

    return NV_OK;
}

//
// Returns a cpu mapping to the provided GPU Offset
//
NV_STATUS nvGpuOpsMemoryCpuMap(struct gpuAddressSpace *vaSpace,
                               NvU64 memory,
                               NvLength length,
                               void **cpuPtr,
                               NvU32 pageSize)
{
    gpuMemDesc *memDesc = NULL;
    cpuMappingDesc *cpuMapDesc = NULL;
    NV_STATUS status;
    void *pMappedAddr = NULL;
    NvP64 mappedAddr = 0;
    NvU32 flags = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!vaSpace || !cpuPtr)
        return NV_ERR_INVALID_ARGUMENT;

    cpuMapDesc = portMemAllocNonPaged(sizeof(*cpuMapDesc));
    if (cpuMapDesc == NULL)
        return NV_ERR_GENERIC;

    portSyncRwLockAcquireRead(vaSpace->allocationsLock);
    status = findDescriptor(vaSpace->allocations, memory, (void**)&memDesc);
    portSyncRwLockReleaseRead(vaSpace->allocationsLock);
    if (status != NV_OK)
        goto cleanup_desc;

    NV_ASSERT(memDesc);
    NV_ASSERT(memDesc->childHandle);

    //
    // Set correct page size for Bar mappings.
    //
    if (pageSize == RM_PAGE_SIZE)
    {
        flags |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _4KB);
    }
    else if (pageSize == RM_PAGE_SIZE_HUGE)
    {
        // TODO: this flag is ignored, remove it once it is deprecated
        flags |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _HUGE);
    }
    else
    {
        flags |= DRF_DEF(OS46, _FLAGS, _PAGE_SIZE, _DEFAULT);
    }

    //
    // If the length passed in is zero we will force the mapping
    // to the size that was used for allocation of the passed in
    // NvU64
    //
    status = pRmApi->MapToCpu(pRmApi,
                              vaSpace->device->session->handle,
                              vaSpace->device->subhandle,
                              memDesc->childHandle,
                              0,
                              length != 0 ? length : memDesc->size,
                              &pMappedAddr,
                              flags);
    if (status != NV_OK)
        goto cleanup_desc;

    mappedAddr = NV_PTR_TO_NvP64(pMappedAddr);

    cpuMapDesc->cpuPointer = (NvUPtr) mappedAddr;
    cpuMapDesc->handle = memDesc->childHandle;
    cpuMapDesc->btreeNode.keyStart = (NvU64)cpuMapDesc->cpuPointer;
    cpuMapDesc->btreeNode.keyEnd = (NvU64)cpuMapDesc->cpuPointer;
    cpuMapDesc->btreeNode.Data = (void *) cpuMapDesc;

    // Track CPU memdesc
    portSyncRwLockAcquireWrite(vaSpace->cpuMappingsLock);
    status = btreeInsert(&cpuMapDesc->btreeNode, &vaSpace->cpuMappings);
    portSyncRwLockReleaseWrite(vaSpace->cpuMappingsLock);
    if (status != NV_OK)
        goto cleanup_desc;

    // can use this address as key as Bar1 address space is unique
    *cpuPtr = NvP64_VALUE(mappedAddr);

    return NV_OK;

cleanup_desc:
    portMemFree(cpuMapDesc);
    return status;
}

void nvGpuOpsMemoryCpuUnMap(struct gpuAddressSpace *vaSpace, void *cpuPtr)
{
    unsigned pid =0;
    cpuMappingDesc *mappingDesc = NULL;
    PNODE btreeNode;
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!vaSpace || !cpuPtr)
        return;

    portSyncRwLockAcquireRead(vaSpace->cpuMappingsLock);
    status = btreeSearch((NvUPtr)cpuPtr, &btreeNode, vaSpace->cpuMappings);
    portSyncRwLockReleaseRead(vaSpace->cpuMappingsLock);
    if (status != NV_OK)
        return;

    mappingDesc = (cpuMappingDesc *)btreeNode->Data;
    if (mappingDesc)
    {
        pid = osGetCurrentProcess();
        status = pRmApi->UnmapFromCpu(pRmApi,
                                      vaSpace->device->session->handle,
                                      vaSpace->device->subhandle,
                                      mappingDesc->handle,
                                      NvP64_VALUE(((NvP64)mappingDesc->cpuPointer)),
                                      0,
                                      pid);
        NV_ASSERT(status == NV_OK);
    }

    portSyncRwLockAcquireWrite(vaSpace->cpuMappingsLock);
    btreeUnlink(btreeNode, &vaSpace->cpuMappings);
    portSyncRwLockReleaseWrite(vaSpace->cpuMappingsLock);

    portMemFree(mappingDesc);
    return;
}

// This function frees both physical and and virtual memory allocations
// This is a counter-function of nvGpuOpsGpuMalloc!
void nvGpuOpsMemoryFree(struct gpuAddressSpace *vaSpace, NvU64 pointer)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    gpuMemDesc *memDesc = NULL;

    NV_ASSERT(vaSpace);

    portSyncRwLockAcquireWrite(vaSpace->allocationsLock);
    deleteDescriptor(&vaSpace->allocations, pointer, (void**)&memDesc);
    portSyncRwLockReleaseWrite(vaSpace->allocationsLock);

    NV_ASSERT(memDesc);
    NV_ASSERT(memDesc->childHandle);
    NV_ASSERT(memDesc->handle);

    // Free physical allocation
    pRmApi->Free(pRmApi, vaSpace->device->session->handle, memDesc->childHandle);

    // Free virtual allocation
    pRmApi->Free(pRmApi, vaSpace->device->session->handle, memDesc->handle);

    portMemFree(memDesc);
}



NV_STATUS nvGpuOpsQueryCesCaps(struct gpuDevice *device,
                               gpuCesCaps *cesCaps)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;

    if (!device || !cesCaps)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      device->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    // Refresh CE information, which may have changed if a GPU has been
    // initialized by RM for the first time
    status = queryCopyEngines(device, cesCaps);
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsQueryCaps(struct gpuDevice *device, gpuCaps *caps)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    OBJGPU *pGpu = NULL;
    KernelMemorySystem *pKernelMemorySystem;
    NV0000_CTRL_GPU_GET_ID_INFO_PARAMS infoParams = {0};
    struct gpuSession *session = device->session;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ, device->session->handle, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    caps->sysmemLink = device->sysmemLink;
    caps->sysmemLinkRateMBps = device->sysmemLinkRateMBps;
    caps->connectedToSwitch = device->connectedToSwitch;

    infoParams.gpuId = device->gpuId;
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             session->handle,
                             NV0000_CTRL_CMD_GPU_GET_ID_INFO,
                             &infoParams,
                             sizeof(infoParams));
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    if (infoParams.numaId != NV0000_CTRL_NO_NUMA_NODE)
    {
        caps->numaEnabled = NV_TRUE;
        caps->numaNodeId = infoParams.numaId;
    }

    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    if (!pKernelMemorySystem)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ATS_SUPPORTED))
    {
        caps->systemMemoryWindowStart = pKernelMemorySystem->coherentCpuFbBase;
        caps->systemMemoryWindowSize = pKernelMemorySystem->coherentCpuFbEnd -
            pKernelMemorySystem->coherentCpuFbBase;
    }
    else
    {
        caps->systemMemoryWindowStart = 0;
        caps->systemMemoryWindowSize = 0;
    }

    if (device->connectedToSwitch)
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        if (pKernelNvlink == NULL)
        {
            caps->nvswitchMemoryWindowStart = NVLINK_INVALID_FABRIC_ADDR;
        }
        else
        {
            caps->nvswitchMemoryWindowStart = knvlinkGetUniqueFabricBaseAddress(
                                                            pGpu, pKernelNvlink);
        }
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return NV_OK;
}

static NV_STATUS findVaspaceFromPid(unsigned pid, unsigned gpuId,
                 NvHandle *hClient, NvHandle *hDevice,
                 NvHandle *hSubdevice, NvHandle *hVaSpace)
{
    //
    // This function iterates through all the vaspace objects under the client,
    // that matches the pid argument, and returns any address space that is
    // tagged as UVM.
    //
    Device *pDevice = NULL;
    Subdevice *pSubDevice = NULL;
    OBJVASPACE *pVAS = NULL;
    OBJGPU *pGpu;
    unsigned hDeviceLocal = 0;
    unsigned hSubDeviceLocal = 0;
    NV_STATUS status;
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);
        if (pClient->ProcID == pid)
        {
            pGpu = gpumgrGetGpuFromId(gpuId);
            if (!pGpu)
                return NV_ERR_INVALID_ARGUMENT;

            pSubDevice = CliGetSubDeviceInfoFromGpu(pRsClient->hClient,
                                                    pGpu);

            status = deviceGetByGpu(pRsClient, pGpu, NV_TRUE, &pDevice);
            if (status == NV_OK)
            {
                hDeviceLocal = RES_GET_HANDLE(pDevice);

                if (pSubDevice != NULL)
                    hSubDeviceLocal = RES_GET_HANDLE(pSubDevice);

                *hClient = pRsClient->hClient;
                *hDevice = hDeviceLocal;
                *hSubdevice = hSubDeviceLocal;

                if (pDevice->vaMode !=
                    NV_DEVICE_ALLOCATION_VAMODE_MULTIPLE_VASPACES)
                {
                    status = vaspaceGetByHandleOrDeviceDefault(pRsClient, hDeviceLocal, 0, &pVAS);
                    if ((status != NV_OK) || (pVAS == NULL))
                        return NV_ERR_GENERIC;

                    //
                    // TODO: Bug 1632484:
                    // Check to see if pVAS is UVM_MANAGED, once
                    // that vaspace property is introduced.
                    // No need to check FaultCapable.
                    //
                    if ((vaspaceIsMirrored(pVAS)) ||
                        (vaspaceIsFaultCapable(pVAS)))
                    {
                        //
                        // This means that this client is
                        // using the vaspace associated to its device
                        //
                        *hVaSpace = 0;
                        return NV_OK;
                    }
                }

                //
                // if the default VASPACE is not tagged as UVM
                // will search for all vaspace objects under
                // this client for this device to find the first
                // vaspace that is tagged as UVM.
                //
                if (findUvmAddressSpace(*hClient, pGpu->gpuInstance, hVaSpace, &pVAS) == NV_OK)
                {
                    return NV_OK;
                }
            }
        }
    }
    return NV_ERR_GENERIC;
}

//
// This function will look through all the vaspaces under a client for a device and return
// the one that is tagged as UVM, or NULL if there is no UVM vaspace.
//
static NV_STATUS findUvmAddressSpace(NvHandle hClient, NvU32 gpuInstance, NvHandle *phVaSpace, OBJVASPACE **ppVASpace)
{
    RsResourceRef *pResourceRef;
    RS_ITERATOR    iter;
    NvU32          gpuMask = NVBIT(gpuInstance);

    iter = serverutilRefIter(hClient, NV01_NULL_OBJECT, classId(VaSpaceApi), RS_ITERATE_DESCENDANTS, NV_TRUE);

    while (clientRefIterNext(iter.pClient, &iter))
    {
        pResourceRef = iter.pResourceRef;

        *ppVASpace = dynamicCast(pResourceRef->pResource, VaSpaceApi)->pVASpace;
        *phVaSpace = pResourceRef->hResource;

        if ((vaspaceIsMirrored(*ppVASpace) || vaspaceIsExternallyOwned(*ppVASpace)) &&
            (((*ppVASpace)->gpuMask & gpuMask) == gpuMask))
        {
            return NV_OK;
        }
    }
    *phVaSpace = 0;
    *ppVASpace = NULL;
    return NV_ERR_INVALID_ARGUMENT;
}

// Make sure UVM_GPU_NAME_LENGTH has the same length as
// NV2080_GPU_MAX_NAME_STRING_LENGTH.
ct_assert(NV2080_GPU_MAX_NAME_STRING_LENGTH == UVM_GPU_NAME_LENGTH);

static void getGpcTpcInfo(OBJGPU *pGpu, gpuInfo *pGpuInfo)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);

    pGpuInfo->maxTpcPerGpcCount = 0;
    pGpuInfo->maxGpcCount = 0;
    pGpuInfo->gpcCount = 0;
    pGpuInfo->tpcCount = 0;

    NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsManager->legacyKgraphicsStaticInfo.bInitialized);
    NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo != NULL);

    pGpuInfo->maxTpcPerGpcCount =
        pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_TPC_PER_GPC].data;
    pGpuInfo->maxGpcCount =
        pKernelGraphicsManager->legacyKgraphicsStaticInfo.pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_LITTER_NUM_GPCS].data;
    pGpuInfo->gpcCount =
        nvPopCount32(pKernelGraphicsManager->legacyKgraphicsStaticInfo.floorsweepingMasks.gpcMask);

    //
    // When MIG GPU partitioning is enabled, compute the upper bound on the number
    // of TPCs that may be available in this partition, to enable UVM to
    // conservatively size relevant data structures.
    //
    if (IS_MIG_IN_USE(pGpu))
    {
        pGpuInfo->tpcCount = pGpuInfo->gpcCount * pGpuInfo->maxTpcPerGpcCount;
    }
    else
    {
        KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, 0);
        const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);

        NV_ASSERT_OR_RETURN_VOID(pKernelGraphicsStaticInfo != NULL);
        pGpuInfo->tpcCount = pKernelGraphicsStaticInfo->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_SHADER_PIPE_SUB_COUNT].data;
    }
}

static NV_STATUS queryVirtMode(NvHandle hClient, NvHandle hDevice, NvU32 *virtMode)
{
    NV_STATUS status = NV_OK;
    *virtMode = UVM_VIRT_MODE_NONE;
    return status;
}

NV_STATUS nvGpuOpsGetGpuInfo(const NvProcessorUuid *pUuid,
                             const gpuClientInfo *pGpuClientInfo,
                             gpuInfo *pGpuInfo)
{
    NV_STATUS                            status;
    NV0080_ALLOC_PARAMETERS              nv0080AllocParams = {0};
    NV2080_ALLOC_PARAMETERS              nv2080AllocParams = {0};
    NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS gpuIdInfoParams = {{0}};
    NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS  archInfoParams = {0};
    NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS gpuNameParams = {0};
    NvHandle                             clientHandle           = 0;
    NvHandle                             deviceHandle           = 1;
    NvHandle                             subDeviceHandle        = 2;
    NvBool                               isClientAllocated      = NV_FALSE;
    NvBool                               isDeviceAllocated      = NV_FALSE;
    NvBool                               isSubdeviceAllocated   = NV_FALSE;
    NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS subDevParams = { 0 };
    NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS simulationInfoParams = {0};
    OBJGPU                              *pGpu = NULL;
    RM_API                              *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NvU32                                dummy;

    pGpu = gpumgrGetGpuFromUuid(pUuid->uuid,
                                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));
    if (!pGpu)
    {
        return NV_ERR_GPU_UUID_NOT_FOUND;
    }

    if (!osIsGpuAccessible(pGpu))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    status = nvGpuOpsCreateClient(pRmApi, &clientHandle);
    if (status != NV_OK)
    {
        return status;
    }

    isClientAllocated = NV_TRUE;

    portMemCopy(&gpuIdInfoParams.gpuUuid, sizeof(*pUuid), pUuid, sizeof(*pUuid));

    gpuIdInfoParams.flags = NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_BINARY;
    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             clientHandle,
                             NV0000_CTRL_CMD_GPU_GET_UUID_INFO,
                             &gpuIdInfoParams,
                             sizeof(gpuIdInfoParams));
    if (NV_OK != status)
        goto cleanup;

    nv0080AllocParams.deviceId = gpuIdInfoParams.deviceInstance;

    status = pRmApi->Alloc(pRmApi,
                           clientHandle,
                           clientHandle,
                           &deviceHandle,
                           NV01_DEVICE_0,
                           &nv0080AllocParams);
    if (NV_OK != status)
        goto cleanup;

    isDeviceAllocated = NV_TRUE;

    nv2080AllocParams.subDeviceId = gpuIdInfoParams.subdeviceInstance;
    status = pRmApi->Alloc(pRmApi,
                           clientHandle,
                           deviceHandle,
                           &subDeviceHandle,
                           NV20_SUBDEVICE_0,
                           &nv2080AllocParams);
    if (NV_OK != status)
        goto cleanup;

    isSubdeviceAllocated = NV_TRUE;

    portMemCopy(&pGpuInfo->uuid, sizeof(*pUuid), pUuid, sizeof(*pUuid));

    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             subDeviceHandle,
                             NV2080_CTRL_CMD_MC_GET_ARCH_INFO,
                             &archInfoParams,
                             sizeof(archInfoParams));
    if (NV_OK != status)
        goto cleanup;

    pGpuInfo->gpuArch = archInfoParams.architecture;
    pGpuInfo->gpuImplementation = archInfoParams.implementation;

    gpuNameParams.gpuNameStringFlags = NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII;
    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             subDeviceHandle,
                             NV2080_CTRL_CMD_GPU_GET_NAME_STRING,
                             &gpuNameParams,
                             sizeof(gpuNameParams));
    if (NV_OK != status)
        goto cleanup;

    portStringCopy(pGpuInfo->name, sizeof(pGpuInfo->name),
                   (const char *)gpuNameParams.gpuNameString.ascii,
                   sizeof(gpuNameParams.gpuNameString.ascii));

    status = queryVirtMode(clientHandle, deviceHandle, &pGpuInfo->virtMode);
    if (status != NV_OK)
        goto cleanup;

    pGpuInfo->gpuInTcc = NV_FALSE;

    status = findDeviceClasses(clientHandle,
                               deviceHandle,
                               subDeviceHandle,
                               &pGpuInfo->hostClass,
                               &pGpuInfo->ceClass,
                               &pGpuInfo->computeClass,
                               &dummy,
                               &dummy,
                               &dummy);
    if (status != NV_OK)
        goto cleanup;

    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             deviceHandle,
                             NV0080_CTRL_CMD_GPU_GET_NUM_SUBDEVICES,
                             &subDevParams,
                             sizeof(subDevParams));
    if (status != NV_OK)
        goto cleanup;

    pGpuInfo->subdeviceCount = subDevParams.numSubDevices;

    getGpcTpcInfo(pGpu, pGpuInfo);

    if (IS_MIG_IN_USE(pGpu))
    {
        NvU32 swizzId;

        NV_ASSERT(pGpuInfo->subdeviceCount == 1);

        status = getSwizzIdFromUserSmcPartHandle(pRmApi,
                                                 clientHandle,
                                                 deviceHandle,
                                                 pGpuClientInfo->hClient,
                                                 pGpuClientInfo->hSmcPartRef,
                                                 &swizzId);
        if (status != NV_OK)
            goto cleanup;

        pGpuInfo->smcEnabled              = NV_TRUE;
        pGpuInfo->smcSwizzId              = swizzId;
        pGpuInfo->smcUserClientInfo.hClient     = pGpuClientInfo->hClient;
        pGpuInfo->smcUserClientInfo.hSmcPartRef = pGpuClientInfo->hSmcPartRef;
    }

    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             subDeviceHandle,
                             NV2080_CTRL_CMD_GPU_GET_SIMULATION_INFO,
                             &simulationInfoParams,
                             sizeof(simulationInfoParams));
    if (status != NV_OK)
        goto cleanup;

    pGpuInfo->isSimulated = (simulationInfoParams.type != NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_NONE);

cleanup:
    if (isSubdeviceAllocated)
        pRmApi->Free(pRmApi, clientHandle, subDeviceHandle);

    if (isDeviceAllocated)
        pRmApi->Free(pRmApi, clientHandle, deviceHandle);

    if (isClientAllocated)
        pRmApi->Free(pRmApi, clientHandle, clientHandle);

    return status;
}

NV_STATUS nvGpuOpsGetGpuIds(const NvU8 *pUuid,
                            unsigned uuidLength,
                            NvU32 *pDeviceId,
                            NvU32 *pSubdeviceId)
{
    NV_STATUS nvStatus;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS gpuIdInfoParams = {{0}};
    NvHandle clientHandle = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    nvStatus = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_NONE, NV01_NULL_OBJECT, NULL, &acquiredLocks);
    if (nvStatus != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return nvStatus;
    }

    nvStatus = nvGpuOpsCreateClient(pRmApi, &clientHandle);
    if (nvStatus != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return nvStatus;
    }

    portMemCopy(&gpuIdInfoParams.gpuUuid, uuidLength, pUuid, uuidLength);

    gpuIdInfoParams.flags = NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_BINARY;
    nvStatus = pRmApi->Control(pRmApi,
                               clientHandle,
                               clientHandle,
                               NV0000_CTRL_CMD_GPU_GET_UUID_INFO,
                               &gpuIdInfoParams,
                               sizeof(gpuIdInfoParams));
    if (NV_OK == nvStatus)
    {
        *pDeviceId = gpuIdInfoParams.deviceInstance;
        *pSubdeviceId = gpuIdInfoParams.subdeviceInstance;
    }

    pRmApi->Free(pRmApi, clientHandle, clientHandle);

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return nvStatus;
}

NV_STATUS nvGpuOpsServiceDeviceInterruptsRM(struct gpuDevice *device)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS params = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_NONE, device->session->handle, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    params.engines = NV2080_CTRL_MC_ENGINE_ID_ALL;
    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_MC_SERVICE_INTERRUPTS,
                             &params,
                             sizeof(params));

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsCheckEccErrorSlowpath(struct gpuChannel *channel,
                                        NvBool *bEccDbeSet)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS eccStatus;
    NvU32 i = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!channel || !bEccDbeSet)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      channel->vaSpace->device->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    *bEccDbeSet = NV_FALSE;

    // Do anything only if ECC is enabled on this device
    if (channel->vaSpace->device->rmSubDevice->bEccEnabled)
    {
        portMemSet(&eccStatus, 0, sizeof(eccStatus));

        status = pRmApi->Control(pRmApi,
                                 channel->vaSpace->device->session->handle,
                                 channel->vaSpace->device->subhandle,
                                 NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS,
                                 &eccStatus,
                                 sizeof(eccStatus));
        if (status != NV_OK)
        {
            _nvGpuOpsLocksRelease(&acquiredLocks);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            return NV_ERR_GENERIC;
        }

        for (i = 0; i < NV2080_CTRL_GPU_ECC_UNIT_COUNT; i++)
        {
            if (eccStatus.units[i].dbe.count != 0)
            {
                *bEccDbeSet = NV_TRUE;
            }
        }
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

static NV_STATUS nvGpuOpsFillGpuMemoryInfo(PMEMORY_DESCRIPTOR pMemDesc,
                                           OBJGPU *pMappingGpu,
                                           gpuMemoryInfo *pGpuMemoryInfo)
{
    NV_STATUS status;
    PMEMORY_DESCRIPTOR pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);
    OBJGPU *pGpu = (pMemDesc->pGpu == NULL) ? pMappingGpu : pMemDesc->pGpu;

    status = nvGpuOpsMemGetPageSize(pMappingGpu,
                                    pMemDesc,
                                    &pGpuMemoryInfo->pageSize);
    if (status != NV_OK)
        return status;

    pGpuMemoryInfo->size = memdescGetSize(pMemDesc);

    pGpuMemoryInfo->contig = memdescGetContiguity(pMemDesc, AT_GPU);

    if (pGpuMemoryInfo->contig)
    {
        GMMU_APERTURE aperture = nvGpuOpsGetExternalAllocAperture(pMemDesc, NV_FALSE, NV_FALSE);
        NvU64 physAddr;

        memdescGetPhysAddrsForGpu(pMemDesc, pMappingGpu, AT_GPU, 0, 0, 1, &physAddr);

        pGpuMemoryInfo->physAddr =
            kgmmuEncodePhysAddr(GPU_GET_KERNEL_GMMU(pGpu), aperture, physAddr, NVLINK_INVALID_FABRIC_ADDR);
    }

    pGpuMemoryInfo->kind = memdescGetPteKindForGpu(pMemDesc, pMappingGpu);

    pGpuMemoryInfo->sysmem = (memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM);

    pGpuMemoryInfo->deviceDescendant = pRootMemDesc->pGpu != NULL;

    if (pGpuMemoryInfo->deviceDescendant)
    {
        NvU8 *uuid;
        NvU32 uuidLength, flags;
        NV_STATUS status;
        flags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);

        // on success, allocates memory for uuid
        status = gpuGetGidInfo(pGpu, &uuid, &uuidLength, flags);
        if (status != NV_OK)
            return status;

        portMemCopy(&pGpuMemoryInfo->uuid, uuidLength, uuid, uuidLength);
        portMemFree(uuid);
    }

    return NV_OK;
}

static NvBool memdescIsSysmem(PMEMORY_DESCRIPTOR pMemDesc)
{
    return memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM;
}

static NV_STATUS dupMemory(struct gpuDevice *device,
                           NvHandle hClient,
                           NvHandle hPhysMemory,
                           NvU32 flags,
                           NvHandle *hDupMemory,
                           gpuMemoryInfo *pGpuMemoryInfo)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NvHandle  dupedMemHandle;
    Memory *pMemory =  NULL;
    PMEMORY_DESCRIPTOR pMemDesc = NULL;
    MEMORY_DESCRIPTOR *pAdjustedMemDesc = NULL;
    FABRIC_VASPACE *pFabricVAS = NULL;
    OBJGPU *pMappingGpu;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RsResourceRef *pResourceRef;
    RsResourceRef *pParentRef;
    struct gpuSession *session;
    NvHandle hParent;
    NvHandle hSubDevice;
    NvBool bIsIndirectPeer = NV_FALSE;

    if (!device || !hDupMemory)
        return NV_ERR_INVALID_ARGUMENT;

    NV_ASSERT((flags == NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE) || (flags == NV04_DUP_HANDLE_FLAGS_NONE));

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // RS-TODO use dual client locking
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_NONE, NV01_NULL_OBJECT, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = CliSetSubDeviceContext(device->session->handle,
                                    device->subhandle,
                                    &hSubDevice,
                                    &pMappingGpu);

    if (status != NV_OK)
        goto done;

    // Get all the necessary information about the memory
    status = nvGpuOpsGetMemoryByHandle(hClient,
                                       hPhysMemory,
                                       &pMemory);
    if (status != NV_OK)
        goto done;

    // RM client allocations can't have multiple memdesc.
    pMemDesc = pMemory->pMemDesc;
    NV_ASSERT(!memdescHasSubDeviceMemDescs(pMemDesc));

    pAdjustedMemDesc = pMemDesc;
    pFabricVAS       = dynamicCast(pMappingGpu->pFabricVAS, FABRIC_VASPACE);
    if (pFabricVAS != NULL)
    {
        status = fabricvaspaceGetGpaMemdesc(pFabricVAS, pMemDesc, pMappingGpu, &pAdjustedMemDesc);
        if (status != NV_OK)
            goto done;
    }

    if (memdescGetAddressSpace(pAdjustedMemDesc) != ADDR_FBMEM &&
        memdescGetAddressSpace(pAdjustedMemDesc) != ADDR_SYSMEM &&
        memdescGetAddressSpace(pAdjustedMemDesc) != ADDR_FABRIC &&
        memdescGetAddressSpace(pAdjustedMemDesc) != ADDR_FABRIC_V2)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto freeGpaMemdesc;
    }

    // For SYSMEM or indirect peer mappings
    bIsIndirectPeer = gpumgrCheckIndirectPeer(pMappingGpu, pAdjustedMemDesc->pGpu);
    if (bIsIndirectPeer ||
        memdescIsSysmem(pAdjustedMemDesc))
    {
        // For sysmem allocations, the dup done below is very shallow and in
        // particular doesn't create IOMMU mappings required for the mapped GPU
        // to access the memory. That's a problem if the mapped GPU is different
        // from the GPU that the allocation was created under. Add them
        // explicitly here and remove them when the memory is freed in n
        // nvGpuOpsFreeDupedHandle(). Notably memdescMapIommu() refcounts the
        // mappings so it's ok to call it if the mappings are already there.
        //
        // TODO: Bug 1811060: Add native support for this use-case in RM API.
        status = memdescMapIommu(pAdjustedMemDesc, pMappingGpu->busInfo.iovaspaceId);
        if (status != NV_OK)
            goto freeGpaMemdesc;
    }

    session = device->session;

    if (pGpuMemoryInfo)
    {
        RsClient *pClient;
        status = serverGetClientUnderLock(&g_resServ, session->handle, &pClient);
        if (status != NV_OK)
            goto freeGpaMemdesc;

        status = nvGpuOpsFillGpuMemoryInfo(pAdjustedMemDesc, pMappingGpu, pGpuMemoryInfo);
        if (status != NV_OK)
            goto freeGpaMemdesc;
    }

    pResourceRef = RES_GET_REF(pMemory);
    pParentRef = pResourceRef->pParentRef;

    // TODO: Bug 2479851: temporarily detect the type of the parent of the
    // memory object (device or subdevice). Once CUDA switches to subdevices,
    // we will use subdevice handles unconditionally, here.
    if (dynamicCast(pParentRef->pResource, Subdevice))
    {
        hParent = device->subhandle;
    }
    else if (dynamicCast(pParentRef->pResource, RsClientResource))
    {
        NV_ASSERT((memdescGetAddressSpace(pAdjustedMemDesc) == ADDR_FABRIC) ||
                  (memdescGetAddressSpace(pAdjustedMemDesc) == ADDR_FABRIC_V2));
        hParent = session->handle;
    }
    else
    {
        NV_ASSERT(dynamicCast(pParentRef->pResource, Device));
        hParent = device->handle;
    }

    dupedMemHandle = NV01_NULL_OBJECT;
    status = pRmApi->DupObject(pRmApi,
                               session->handle,
                               hParent,
                               &dupedMemHandle,
                               hClient,
                               hPhysMemory,
                               flags);
    if (status != NV_OK)
        goto freeGpaMemdesc;

    *hDupMemory = dupedMemHandle;

freeGpaMemdesc:
    if (pAdjustedMemDesc != pMemDesc)
        fabricvaspacePutGpaMemdesc(pFabricVAS, pAdjustedMemDesc);

done:
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsDupMemory(struct gpuDevice *device,
                            NvHandle hClient,
                            NvHandle hPhysMemory,
                            NvHandle *hDupMemory,
                            gpuMemoryInfo *pGpuMemoryInfo)
{
    return dupMemory(device,
                     hClient,
                     hPhysMemory,
                     NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE,
                     hDupMemory,
                     pGpuMemoryInfo);
}

NV_STATUS nvGpuOpsDupAllocation(struct gpuAddressSpace *srcVaSpace,
                                NvU64 srcAddress,
                                struct gpuAddressSpace *dstVaSpace,
                                NvU64 *dstAddress)
{
    NV_STATUS status;
    NvHandle dstPhysHandle = 0;
    NvHandle srcPhysHandle = 0;
    NvU64 tmpDstAddress = 0;
    gpuMemoryInfo gpuMemoryInfo = {0};
    gpuVaAllocInfo allocInfo = {0};
    struct allocFlags flags = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, srcVaSpace != 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, dstVaSpace != 0, NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, srcVaSpace != dstVaSpace, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, srcAddress != 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, dstAddress != NULL, NV_ERR_INVALID_ARGUMENT);

    // If the given combination of source VA space and address does not
    // correspond to a previous allocation, the physical handle retrieval fails
    status = getHandleForVirtualAddr(srcVaSpace, srcAddress, NV_TRUE, &srcPhysHandle);
    if (status != NV_OK)
        return status;

    // Dupe the physical allocation, and return information about the associated
    // memory descriptor
    //
    // Passing NV04_DUP_HANDLE_FLAGS_NONE allows duping across MIG partitions
    status = dupMemory(dstVaSpace->device,
                       srcVaSpace->device->session->handle,
                       srcPhysHandle,
                       NV04_DUP_HANDLE_FLAGS_NONE,
                       &dstPhysHandle,
                       &gpuMemoryInfo);

    if (status != NV_OK)
        return status;

    // Vidmem dups across GPUs are not currently supported
    if (!gpuMemoryInfo.sysmem && (srcVaSpace->device != dstVaSpace->device))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup_dup;
    }

    // The virtual allocation and mapping use the size, and page size, of the
    // destination memory descriptor
    allocInfo.pageSize = gpuMemoryInfo.pageSize;

    status = nvGpuOpsAllocVirtual(dstVaSpace,
                                  gpuMemoryInfo.size,
                                  dstAddress,
                                  dstPhysHandle,
                                  flags,
                                  &allocInfo);
    if (status != NV_OK)
        goto cleanup_dup;

    // Map the entire memory
    status = nvGpuOpsMapGpuMemory(dstVaSpace,
                                  *dstAddress,
                                  gpuMemoryInfo.size,
                                  gpuMemoryInfo.pageSize,
                                  &tmpDstAddress,
                                  flags);

    if (status != NV_OK)
        goto cleanup_virt_allocation;

    NV_ASSERT(tmpDstAddress == *dstAddress);

    return NV_OK;

cleanup_virt_allocation:
    nvGpuOpsFreeVirtual(dstVaSpace, *dstAddress);

cleanup_dup:
    pRmApi->Free(pRmApi, dstVaSpace->device->session->handle, dstPhysHandle);
    return status;
}

NV_STATUS nvGpuOpsGetGuid(NvHandle hClient, NvHandle hDevice,
                          NvHandle hSubDevice, NvU8 *gpuGuid,
                          unsigned guidLength)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV2080_CTRL_GPU_GET_GID_INFO_PARAMS getGidParams = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!gpuGuid)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ, hClient, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    getGidParams.index = 0;
    getGidParams.flags = NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT_BINARY;
    status = pRmApi->Control(pRmApi,
                             hClient, hSubDevice,
                             NV2080_CTRL_CMD_GPU_GET_GID_INFO,
                             &getGidParams,
                             sizeof(getGidParams));

    if ((guidLength !=  getGidParams.length) || (status != NV_OK))
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemCopy(gpuGuid, guidLength, &getGidParams.data, guidLength);

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

// Make sure UVM_COPY_ENGINE_COUNT_MAX is at least the number of copy engines
// supported by RM.
ct_assert(UVM_COPY_ENGINE_COUNT_MAX >= NV2080_ENGINE_TYPE_COPY_SIZE);

static void setCeCaps(const NvU8 *rmCeCaps, gpuCeCaps *ceCaps)
{
    ceCaps->grce        = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_GRCE);
    ceCaps->shared      = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_SHARED);
    ceCaps->sysmemRead  = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_SYSMEM_READ);
    ceCaps->sysmemWrite = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_SYSMEM_WRITE);
    ceCaps->nvlinkP2p   = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_NVLINK_P2P);
    ceCaps->sysmem      = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_SYSMEM);
    ceCaps->p2p         = !!NV2080_CTRL_CE_GET_CAP(rmCeCaps, NV2080_CTRL_CE_CAPS_CE_P2P);
}

static NV_STATUS queryCopyEngines(struct gpuDevice *gpu, gpuCesCaps *cesCaps)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GPU_GET_ENGINES_PARAMS getEnginesParams = {0};
    NvU32 *engineList;
    NvU32 i;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    status = pRmApi->Control(pRmApi,
                             gpu->session->handle,
                             gpu->subhandle,
                             NV2080_CTRL_CMD_GPU_GET_ENGINES,
                             &getEnginesParams,
                             sizeof(getEnginesParams));
    if (status != NV_OK)
        return status;

    engineList = portMemAllocNonPaged(
                        sizeof(*engineList) * getEnginesParams.engineCount);
    if (engineList == NULL)
        return NV_ERR_NO_MEMORY;

    getEnginesParams.engineList = NV_PTR_TO_NvP64(engineList);

    status = pRmApi->Control(pRmApi,
                             gpu->session->handle,
                             gpu->subhandle,
                             NV2080_CTRL_CMD_GPU_GET_ENGINES,
                             &getEnginesParams,
                             sizeof(getEnginesParams));
    if (status != NV_OK)
        goto done;

    portMemSet(cesCaps, 0, sizeof(*cesCaps));

    for (i = 0; i < getEnginesParams.engineCount; i++)
    {
        NV2080_CTRL_CE_GET_CAPS_PARAMS ceParams = {0};
        NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS pceMaskParams = {0};
        NvU8 rmCeCaps[NV2080_CTRL_CE_CAPS_TBL_SIZE] = {0};
        UvmGpuCopyEngineCaps *ceCaps;

        NvU32 ceIndex = engineList[i] - NV2080_ENGINE_TYPE_COPY0;
        if (ceIndex >= NV2080_ENGINE_TYPE_COPY_SIZE)
            continue;

        ceParams.ceEngineType = NV2080_ENGINE_TYPE_COPY(ceIndex);
        ceParams.capsTblSize = NV2080_CTRL_CE_CAPS_TBL_SIZE;
        ceParams.capsTbl = NV_PTR_TO_NvP64(rmCeCaps);

        status = pRmApi->Control(pRmApi,
                                 gpu->session->handle,
                                 gpu->subhandle,
                                 NV2080_CTRL_CMD_CE_GET_CAPS,
                                 &ceParams,
                                 sizeof(ceParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                      __LINE__, nvstatusToString(status));
            goto done;
        }

        ceCaps = cesCaps->copyEngineCaps + ceIndex;
        setCeCaps(rmCeCaps, ceCaps);

        pceMaskParams.ceEngineType = NV2080_ENGINE_TYPE_COPY(ceIndex);
        pceMaskParams.pceMask = 0;
        status = pRmApi->Control(pRmApi,
                                 gpu->session->handle,
                                 gpu->subhandle,
                                 NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK,
                                 &pceMaskParams,
                                 sizeof(pceMaskParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                      __LINE__, nvstatusToString(status));
            goto done;
        }
        ceCaps->cePceMask = pceMaskParams.pceMask;

        ceCaps->supported = NV_TRUE;
    }

done:
    portMemFree(engineList);
    return status;
}

static NvBool isClassHost(NvU32 class)
{
    NvBool bHostClass = NV_FALSE;
    CLI_CHANNEL_CLASS_INFO classInfo;
    CliGetChannelClassInfo(class, &classInfo);
    bHostClass = (classInfo.classType == CHANNEL_CLASS_TYPE_GPFIFO);
    return bHostClass;
}

static NvBool isClassCE(NvU32 class)
{
    switch (class)
    {
        case MAXWELL_DMA_COPY_A:
        case PASCAL_DMA_COPY_A:
        case PASCAL_DMA_COPY_B:
        case VOLTA_DMA_COPY_A:
        case TURING_DMA_COPY_A:
        case AMPERE_DMA_COPY_A:
        case AMPERE_DMA_COPY_B:
            return NV_TRUE;

        default:
            return NV_FALSE;
    }
}

static NvBool isClassSec2(NvU32 class)
{
    switch (class)
    {

        default:
            return NV_FALSE;
    }
}

static NvBool isClassCompute(NvU32 class)
{
    switch (class)
    {
        case MAXWELL_COMPUTE_A:
        case MAXWELL_COMPUTE_B:
        case PASCAL_COMPUTE_A:
        case PASCAL_COMPUTE_B:
        case VOLTA_COMPUTE_A:
        case VOLTA_COMPUTE_B:
        case TURING_COMPUTE_A:
        case AMPERE_COMPUTE_A:
        case AMPERE_COMPUTE_B:
            return NV_TRUE;

        default:
            return NV_FALSE;
    }
}

static NvBool isClassFaultBuffer(NvU32 class)
{
    switch (class)
    {
        case MAXWELL_FAULT_BUFFER_A:
        case MMU_FAULT_BUFFER:
            return NV_TRUE;

        default:
            return NV_FALSE;
    }
}

static NvBool isClassAccessCounterBuffer(NvU32 class)
{
    switch (class)
    {
        case ACCESS_COUNTER_NOTIFY_BUFFER:
            return NV_TRUE;

        default:
            return NV_FALSE;
    }
}

static NV_STATUS findDeviceClasses(NvHandle hRoot,
                                   NvHandle hDevice,
                                   NvHandle hSubdevice,
                                   NvU32 *hostClass,
                                   NvU32 *ceClass,
                                   NvU32 *computeClass,
                                   NvU32 *faultBufferClass,
                                   NvU32 *accessCounterBufferClass,
                                   NvU32 *sec2Class)
{
    NvU32 *classList;
    NV_STATUS status = NV_OK;
    NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS classParams = {0};
    NvU32 i = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    *hostClass                = 0;
    *ceClass                  = 0;
    *computeClass             = 0;
    *faultBufferClass         = 0;
    *accessCounterBufferClass = 0;

    status = pRmApi->Control(pRmApi,
                             hRoot,
                             hDevice,
                             NV0080_CTRL_CMD_GPU_GET_CLASSLIST,
                             &classParams,
                             sizeof(classParams));
    if (status != NV_OK)
        return status;

    classList = portMemAllocNonPaged(
                        (sizeof(NvU32) * classParams.numClasses));

    if (classList == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    classParams.classList = NV_PTR_TO_NvP64(classList);
    status = pRmApi->Control(pRmApi,
                             hRoot,
                             hDevice,
                             NV0080_CTRL_CMD_GPU_GET_CLASSLIST,
                             &classParams,
                             sizeof(classParams));

    if (status != NV_OK)
        goto Cleanup_classlist;

    for (i = 0; i < classParams.numClasses; i++)
    {
        if (classList[i] == PHYSICAL_CHANNEL_GPFIFO)
            continue;
        if (isClassHost(classList[i]))
            *hostClass = NV_MAX(*hostClass, classList[i]);
        else if (isClassCE(classList[i]))
            *ceClass = NV_MAX(*ceClass, classList[i]);
        else if (isClassCompute(classList[i]))
            *computeClass = NV_MAX(*computeClass, classList[i]);
        else if (isClassFaultBuffer(classList[i]))
            *faultBufferClass = NV_MAX(*faultBufferClass, classList[i]);
        else if (isClassAccessCounterBuffer(classList[i]))
        {
            NV_ASSERT(accessCounterBufferClass);
            *accessCounterBufferClass = NV_MAX(*accessCounterBufferClass, classList[i]);
        }
        else if (isClassSec2(classList[i]))
            *sec2Class = NV_MAX(*sec2Class, classList[i]);
    }

Cleanup_classlist:
    portMemFree(classList);
    return status;
}

NV_STATUS nvGpuOpsGetClientInfoFromPid(unsigned pid,
                                       const NvU8 *gpuUuid,
                                       NvHandle *hClient,
                                       NvHandle *hDevice,
                                       NvHandle *hSubDevice)
{
    NV0000_CTRL_GPU_GET_UUID_INFO_PARAMS gpuIdInfoParams = {{0}};
    unsigned                             gpuId          = 0;
    NvHandle                             hPidClient     = 0;
    NvHandle                             hPidDevice     = 0;
    NvHandle                             hPidVaSpace    = 0;
    NvHandle                             hPidSubDevice  = 0;
    NvHandle                             clientHandle   = 0;
    NV_STATUS                            status;
    nvGpuOpsLockSet                      acquiredLocks;
    THREAD_STATE_NODE                    threadState;
    RM_API                              *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_NONE, NV01_NULL_OBJECT, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = nvGpuOpsCreateClient(pRmApi, &clientHandle);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    // find the gpuId from the given uuid
    portMemCopy(&gpuIdInfoParams.gpuUuid, NV_GPU_UUID_LEN, gpuUuid, NV_GPU_UUID_LEN);
    gpuIdInfoParams.flags = NV0000_CTRL_CMD_GPU_GET_UUID_INFO_FLAGS_FORMAT_BINARY;
    status = pRmApi->Control(pRmApi,
                             clientHandle,
                             clientHandle,
                             NV0000_CTRL_CMD_GPU_GET_UUID_INFO,
                             &gpuIdInfoParams,
                             sizeof(gpuIdInfoParams));
    if (status != NV_OK)
        goto cleanup;

    gpuId = gpuIdInfoParams.gpuId;

    status = findVaspaceFromPid(pid, gpuId, &hPidClient,
                                &hPidDevice, &hPidSubDevice, &hPidVaSpace);

    // free the session we just created
    pRmApi->Free(pRmApi, clientHandle, clientHandle);
    if (status != NV_OK)
        goto cleanup;

    *hClient = hPidClient;
    *hDevice = hPidDevice;
    *hSubDevice = hPidSubDevice;
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return NV_OK;

cleanup:
    *hClient = 0;
    *hDevice = 0;
    *hSubDevice = 0;
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsSetPageDirectory(struct gpuAddressSpace *vaSpace,
                                   NvU64 physAddress,
                                   unsigned numEntries,
                                   NvBool bVidMemAperture, NvU32 pasid)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS params = {0};
    OBJGPU *pGpu = NULL;
    OBJVASPACE *pVAS = NULL;
    RsClient *pClient;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!vaSpace || !numEntries)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      vaSpace->device->session->handle,
                                      &pClient,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = CliSetGpuContext(vaSpace->device->session->handle,
                              vaSpace->device->handle,
                              &pGpu,
                              NULL);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if ((status != NV_OK) || (pVAS == NULL))
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (vaspaceIsExternallyOwned(pVAS))
    {
        // make sure there is no PDB set if already externally owned
        if ((NULL != vaspaceGetPageDirBase(pVAS, pGpu)))
        {
            _nvGpuOpsLocksRelease(&acquiredLocks);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            return NV_ERR_NOT_SUPPORTED;
        }

        // Stop all channels under the VAS
        status = nvGpuOpsDisableVaSpaceChannels(vaSpace);
        if (status != NV_OK)
        {
            //
            // If stopping any channels failed, reenable the channels which were
            // able to be stopped before bailing
            //
            nvGpuOpsEnableVaSpaceChannels(vaSpace);
            _nvGpuOpsLocksRelease(&acquiredLocks);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            return status;
        }
    }

    params.physAddress = physAddress;
    params.numEntries = numEntries;
    params.hVASpace = vaSpace->handle;
    params.flags = bVidMemAperture ?
                   DRF_DEF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS, _APERTURE, _VIDMEM) :
                   DRF_DEF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS, _APERTURE, _SYSMEM_COH);
    params.flags |= DRF_DEF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY_FLAGS,
                            _ALL_CHANNELS, _TRUE);
    params.pasid = pasid;

    // Always do Unicast by passing non-zero subDeviceId!
    params.subDeviceId = vaSpace->device->subdeviceInstance + 1;

    status = pRmApi->Control(pRmApi,
                             vaSpace->device->session->handle,
                             vaSpace->device->handle,
                             NV0080_CTRL_CMD_DMA_SET_PAGE_DIRECTORY,
                             &params,
                             sizeof(params));

    if (vaspaceIsExternallyOwned(pVAS))
    {
        // Reschedule all channels in this VAS
        nvGpuOpsEnableVaSpaceChannels(vaSpace);
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsUnsetPageDirectory(struct gpuAddressSpace *vaSpace)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS params = {0};
    OBJGPU *pGpu = NULL;
    OBJVASPACE *pVAS = NULL;
    RsClient *pClient;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!vaSpace)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      vaSpace->device->session->handle,
                                      &pClient,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = CliSetGpuContext(vaSpace->device->session->handle,
                              vaSpace->device->handle,
                              &pGpu,
                              NULL);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if ((status != NV_OK) || (pVAS == NULL))
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (vaspaceIsExternallyOwned(pVAS))
    {
        // Stop all channels under the VAS
        status = nvGpuOpsDisableVaSpaceChannels(vaSpace);
        if (status != NV_OK)
        {
            //
            // If stopping any channels failed, reenable the channels which were
            // able to be stopped before bailing
            //
            nvGpuOpsEnableVaSpaceChannels(vaSpace);
            _nvGpuOpsLocksRelease(&acquiredLocks);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            return status;
        }
    }

    params.hVASpace = vaSpace->handle;

    // Always do Unicast by passing non-zero subDeviceId!
    params.subDeviceId = vaSpace->device->subdeviceInstance + 1;

    status = pRmApi->Control(pRmApi,
                             vaSpace->device->session->handle,
                             vaSpace->device->handle,
                             NV0080_CTRL_CMD_DMA_UNSET_PAGE_DIRECTORY,
                             &params,
                             sizeof(params));

    if (vaspaceIsExternallyOwned(pVAS))
    {
        // Reschedule all channels in this VAS
        nvGpuOpsEnableVaSpaceChannels(vaSpace);
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsGetGmmuFmt(struct gpuAddressSpace *vaSpace, void **pFmt)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS params = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!vaSpace || !pFmt)
        return NV_ERR_INVALID_ARGUMENT;

    if (!vaSpace->handle)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      vaSpace->device->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    *pFmt = NULL;
    params.hSubDevice = vaSpace->device->subhandle;

    status = pRmApi->Control(pRmApi,
                             vaSpace->device->session->handle,
                             vaSpace->handle,
                             NV90F1_CTRL_CMD_VASPACE_GET_GMMU_FORMAT,
                             &params,
                             sizeof(params));
    if (status == NV_OK)
        *pFmt = (void *)params.pFmt;

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsInvalidateTlb(struct gpuAddressSpace *vaSpace)
{
    NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS params = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (!vaSpace)
        return NV_ERR_INVALID_ARGUMENT;

    params.hVASpace = vaSpace->handle;
    return pRmApi->Control(pRmApi,
                           vaSpace->device->session->handle,
                           vaSpace->device->subhandle,
                           NV2080_CTRL_CMD_DMA_INVALIDATE_TLB,
                           &params,
                           sizeof(params));
}

NV_STATUS nvGpuOpsGetFbInfo(struct gpuDevice *device, gpuFbInfo *fbInfo)
{
    NV_STATUS status;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;

    if (!device || !fbInfo)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      device->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    portMemCopy(fbInfo, sizeof(*fbInfo), &device->fbInfo, sizeof(*fbInfo));

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return NV_OK;
}

NV_STATUS nvGpuOpsGetEccInfo(struct gpuDevice *device, gpuEccInfo *eccInfo)
{
    subDeviceDesc *rmSubDevice;

    if (!device || !eccInfo)
        return NV_ERR_INVALID_ARGUMENT;

    rmSubDevice = device->rmSubDevice;

    if (!rmSubDevice->bEccInitialized)
        return NV_ERR_NOT_SUPPORTED;

    eccInfo->eccMask = rmSubDevice->eccMask;
    eccInfo->eccOffset = rmSubDevice->eccOffset;
    eccInfo->eccReadLocation = rmSubDevice->eccReadLocation;
    eccInfo->bEccEnabled = rmSubDevice->bEccEnabled;
    eccInfo->eccErrorNotifier = &rmSubDevice->eccErrorNotifier;

    return NV_OK;
}

//
// Do not acquire the GPU locks as all nvGpuOpsFreeDupedHandle() does is
// call pRmApi->Free(pRmApi, ) that drops the GPU locks if acquired (and
// re-acquires it later).
//
NV_STATUS nvGpuOpsFreeDupedHandle(struct gpuDevice *device,
                                  NvHandle hPhysHandle)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    Memory *pMemory = NULL;
    OBJGPU *pMappingGpu = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle hClient;
    NvHandle hSubDevice;

    if (!device)
        return NV_ERR_INVALID_ARGUMENT;

    hClient = device->session->handle;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquire(RMAPI_LOCK_FLAGS_READ, hClient, NULL, 0, 0, 0, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = CliSetSubDeviceContext(device->session->handle,
                                    device->subhandle,
                                    &hSubDevice,
                                    &pMappingGpu);
    if (status != NV_OK)
        goto out;

    status = nvGpuOpsGetMemoryByHandle(device->session->handle,
                                       hPhysHandle,
                                       &pMemory);
    if (status != NV_OK)
        goto out;

    if (memdescIsSysmem(pMemory->pMemDesc))
    {
        // Release the mappings acquired in nvGpuOpsDupMemory().
        //
        // TODO: Bug 1811060: Add native support for this use-case in RM API.
        memdescUnmapIommu(pMemory->pMemDesc, pMappingGpu->busInfo.iovaspaceId);
    }

out:
    pRmApi->Free(pRmApi, device->session->handle, hPhysHandle);
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsInitFaultInfo(struct gpuDevice *device,
                                gpuFaultInfo *pFaultInfo)
{
    struct gpuSession *session = device->session;
    NV_STATUS status = NV_OK;
    NVB069_ALLOCATION_PARAMETERS faultBufferAllocParams = {0};
    NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS sizeParams = {0};
    NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS registermappingsParams = {0};
    void *bufferAddress;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    pFaultInfo->faultBufferHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi,
                           session->handle,
                           device->subhandle,
                           &pFaultInfo->faultBufferHandle,
                           device->faultBufferClass,
                           &faultBufferAllocParams);
    if (status != NV_OK)
        goto cleanup;

    // Get the Size of the fault buffer
    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pFaultInfo->faultBufferHandle,
                             NVB069_CTRL_CMD_FAULTBUFFER_GET_SIZE,
                             &sizeParams,
                             sizeof(sizeParams));
    if (status != NV_OK)
        goto cleanup_fault_buffer;

    // Map the fault buffer pointer to CPU
    status = pRmApi->MapToCpu(pRmApi,
                              session->handle,
                              device->subhandle,
                              pFaultInfo->faultBufferHandle,
                              0,
                              pFaultInfo->replayable.bufferSize,
                              &bufferAddress,
                              0);
    if (status != NV_OK)
        goto cleanup_fault_buffer;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pFaultInfo->faultBufferHandle,
                             NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS,
                             &registermappingsParams,
                             sizeof(registermappingsParams));
    if (status != NV_OK)
        goto cleanup_fault_buffer;

    if (isDeviceVoltaPlus(device))
    {
        NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS nonReplayableFaultsParams = {0};

        status = pRmApi->Control(pRmApi,
                                 session->handle,
                                 pFaultInfo->faultBufferHandle,
                                 NVC369_CTRL_CMD_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF,
                                 &nonReplayableFaultsParams,
                                 sizeof(nonReplayableFaultsParams));
        if (status != NV_OK)
            goto cleanup_fault_buffer;

        pFaultInfo->nonReplayable.shadowBufferAddress = (void *)NvP64_VALUE(nonReplayableFaultsParams.pShadowBuffer);
        pFaultInfo->nonReplayable.shadowBufferContext = (void *)NvP64_VALUE(nonReplayableFaultsParams.pShadowBufferContext);
        pFaultInfo->nonReplayable.bufferSize          = nonReplayableFaultsParams.bufferSize;
    }

    pFaultInfo->replayable.pFaultBufferGet        = (NvU32*)(NvUPtr)registermappingsParams.pFaultBufferGet;
    pFaultInfo->replayable.pFaultBufferPut        = (NvU32*)(NvUPtr)registermappingsParams.pFaultBufferPut;
    pFaultInfo->replayable.pFaultBufferInfo       = (NvU32*)(NvUPtr)registermappingsParams.pFaultBufferInfo;
    pFaultInfo->replayable.pPmcIntr               = (NvU32*)(NvUPtr)registermappingsParams.pPmcIntr;
    pFaultInfo->replayable.pPmcIntrEnSet          = (NvU32*)(NvUPtr)registermappingsParams.pPmcIntrEnSet;
    pFaultInfo->replayable.pPmcIntrEnClear        = (NvU32*)(NvUPtr)registermappingsParams.pPmcIntrEnClear;
    pFaultInfo->replayable.replayableFaultMask    = registermappingsParams.replayableFaultMask;
    pFaultInfo->replayable.pPrefetchCtrl          = (NvU32*)(NvUPtr)registermappingsParams.pPrefetchCtrl;
    pFaultInfo->replayable.bufferSize             = sizeParams.faultBufferSize;
    pFaultInfo->replayable.bufferAddress          = bufferAddress;

    return NV_OK;

cleanup_fault_buffer:
    gpuDeviceUnmapCpuFreeHandle(device,
                                pFaultInfo->faultBufferHandle,
                                pFaultInfo->replayable.bufferAddress,
                                0);
cleanup:
    portMemSet(pFaultInfo, 0, sizeof(*pFaultInfo));
    return status;
}

NV_STATUS nvGpuOpsInitAccessCntrInfo(struct gpuDevice *device,
                                     gpuAccessCntrInfo *pAccessCntrInfo)
{
    struct gpuSession *session = device->session;
    NV_STATUS status = NV_OK;
    NvU32 accessCntrBufferAllocParams = {0};
    NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS sizeParams = {0};
    NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS registermappings;
    void *bufferAddress;
    NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS getDmaBaseSysmemAddrParams = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    OBJGPU *pGpu = NULL;

    // TODO: Acquired because CliSetGpuContext expects RMAPI lock. Necessary?
    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;
    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    rmapiLockRelease();
    if (status != NV_OK)
        return status;

    pAccessCntrInfo->accessCntrBufferHandle = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi,
                           session->handle,
                           device->subhandle,
                           &pAccessCntrInfo->accessCntrBufferHandle,
                           device->accessCounterBufferClass,
                           &accessCntrBufferAllocParams);

    if (status != NV_OK)
        goto cleanup;

    status = pRmApi->MapToCpu(pRmApi, session->handle, device->subhandle, pAccessCntrInfo->accessCntrBufferHandle,
                              0, pAccessCntrInfo->bufferSize, &bufferAddress, 0);

    if (status != NV_OK)
        goto cleanup_access_ctr_buffer;

    pAccessCntrInfo->bufferAddress = bufferAddress;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pAccessCntrInfo->accessCntrBufferHandle,
                             NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_SIZE,
                             &sizeParams,
                             sizeof(sizeParams));

    if (status != NV_OK)
        goto cleanup_access_ctr_buffer;

    pAccessCntrInfo->bufferSize = sizeParams.accessCntrBufferSize;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pAccessCntrInfo->accessCntrBufferHandle,
                             NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS,
                             &registermappings,
                             sizeof(registermappings));
    if (status != NV_OK)
        goto cleanup_access_ctr_buffer;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             device->handle,
                             NV0080_CTRL_CMD_BIF_GET_DMA_BASE_SYSMEM_ADDR,
                             &getDmaBaseSysmemAddrParams,
                             sizeof(getDmaBaseSysmemAddrParams));
    if (status != NV_OK)
        goto cleanup_access_ctr_buffer;

    pAccessCntrInfo->pAccessCntrBufferGet  = (NvU32*)(NvUPtr)registermappings.pAccessCntrBufferGet;
    pAccessCntrInfo->pAccessCntrBufferPut  = (NvU32*)(NvUPtr)registermappings.pAccessCntrBufferPut;
    pAccessCntrInfo->pAccessCntrBufferFull = (NvU32*)(NvUPtr)registermappings.pAccessCntrBufferFull;
    pAccessCntrInfo->pHubIntr              = (NvU32*)(NvUPtr)registermappings.pHubIntr;
    pAccessCntrInfo->pHubIntrEnSet         = (NvU32*)(NvUPtr)registermappings.pHubIntrEnSet;
    pAccessCntrInfo->pHubIntrEnClear       = (NvU32*)(NvUPtr)registermappings.pHubIntrEnClear;
    pAccessCntrInfo->accessCounterMask     = registermappings.accessCntrMask;
    pAccessCntrInfo->baseDmaSysmemAddr     = getDmaBaseSysmemAddrParams.baseDmaSysmemAddr;

    return NV_OK;

cleanup_access_ctr_buffer:
    gpuDeviceUnmapCpuFreeHandle(device,
                                pAccessCntrInfo->accessCntrBufferHandle,
                                pAccessCntrInfo->bufferAddress,
                                0);
cleanup:
    pAccessCntrInfo->accessCntrBufferHandle = 0;
    pAccessCntrInfo->bufferAddress = 0;
    return status;
}

static NV_STATUS
getAccessCounterGranularityValue(UVM_ACCESS_COUNTER_GRANULARITY granularity, NvU32 *value)
{
    *value = 0;

    switch (granularity)
    {
        case UVM_ACCESS_COUNTER_GRANULARITY_64K:
            *value = NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_64K;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_2M:
            *value = NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_16M:
            *value = NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_16G:
            *value = NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    };

    return NV_OK;
}

static NV_STATUS
getAccessCounterLimitValue(UVM_ACCESS_COUNTER_USE_LIMIT limit, NvU32 *value)
{
    *value = 0;

    switch (limit)
    {
        case UVM_ACCESS_COUNTER_USE_LIMIT_NONE:
            *value = NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_NONE;
            break;
        case UVM_ACCESS_COUNTER_USE_LIMIT_QTR:
            *value = NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_QTR;
            break;
        case UVM_ACCESS_COUNTER_USE_LIMIT_HALF:
            *value = NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_HALF;
            break;
        case UVM_ACCESS_COUNTER_USE_LIMIT_FULL:
            *value = NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_FULL;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    };

    return NV_OK;
}

NV_STATUS nvGpuOpsEnableAccessCntr(struct gpuDevice *device,
                                   gpuAccessCntrInfo *pAccessCntrInfo,
                                   gpuAccessCntrConfig *pAccessCntrConfig)
{
    NV_STATUS status = NV_OK;
    NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS setConfigParams = { 0 };
    NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS enableParams = { 0 };
    struct gpuSession *session = device->session;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    status = getAccessCounterGranularityValue(pAccessCntrConfig->mimcGranularity, &setConfigParams.mimcGranularity);
    if (status != NV_OK)
        return status;

    status = getAccessCounterGranularityValue(pAccessCntrConfig->momcGranularity, &setConfigParams.momcGranularity);
    if (status != NV_OK)
        return status;

    status = getAccessCounterLimitValue(pAccessCntrConfig->mimcUseLimit, &setConfigParams.mimcLimit);
    if (status != NV_OK)
        return status;

    status = getAccessCounterLimitValue(pAccessCntrConfig->momcUseLimit, &setConfigParams.momcLimit);
    if (status != NV_OK)
        return status;

    setConfigParams.threshold = pAccessCntrConfig->threshold;
    setConfigParams.cmd = NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_GRANULARITY |
                          NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_GRANULARITY |
                          NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_LIMIT |
                          NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_LIMIT |
                          NVC365_CTRL_ACCESS_COUNTER_SET_THRESHOLD;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pAccessCntrInfo->accessCntrBufferHandle,
                             NVC365_CTRL_CMD_ACCESS_CNTR_SET_CONFIG,
                             &setConfigParams,
                             sizeof(setConfigParams));
    if (status != NV_OK)
        return status;

    enableParams.intrOwnership = NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_NOT_RM;
    enableParams.enable        = NV_TRUE;

    status = pRmApi->Control(pRmApi,
                             session->handle,
                             pAccessCntrInfo->accessCntrBufferHandle,
                             NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE,
                             &enableParams,
                             sizeof(enableParams));
    return status;
}

NV_STATUS nvGpuOpsDisableAccessCntr(struct gpuDevice *device,
                                    gpuAccessCntrInfo *pAccessCntrInfo)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS enableParams = { 0 };

    enableParams.intrOwnership = NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_RM;
    enableParams.enable        = NV_FALSE;
    return pRmApi->Control(pRmApi,
                           device->session->handle,
                           pAccessCntrInfo->accessCntrBufferHandle,
                           NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE,
                           &enableParams,
                           sizeof(enableParams));
}

NV_STATUS nvGpuOpsDestroyAccessCntrInfo(struct gpuDevice *device,
                                        gpuAccessCntrInfo *pAccessCntrInfo)
{
    gpuDeviceUnmapCpuFreeHandle(device,
                                pAccessCntrInfo->accessCntrBufferHandle,
                                pAccessCntrInfo->bufferAddress,
                                0);
    portMemSet(pAccessCntrInfo, 0, sizeof(gpuAccessCntrInfo));
    return NV_OK;
}

NV_STATUS nvGpuOpsDestroyFaultInfo(struct gpuDevice *device,
                                   gpuFaultInfo *pFaultInfo)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (pFaultInfo->faultBufferHandle && isDeviceVoltaPlus(device))
    {
        NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS params = {0};

        params.pShadowBuffer = NV_PTR_TO_NvP64(pFaultInfo->nonReplayable.shadowBufferAddress);

        status = pRmApi->Control(pRmApi,
                                 device->session->handle,
                                 pFaultInfo->faultBufferHandle,
                                 NVC369_CTRL_CMD_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF,
                                 &params,
                                 sizeof(params));
        NV_ASSERT(status == NV_OK);
    }

    gpuDeviceUnmapCpuFreeHandle(device,
                                pFaultInfo->faultBufferHandle,
                                pFaultInfo->replayable.bufferAddress,
                                0);

    portMemSet(pFaultInfo, 0, sizeof(gpuFaultInfo));
    return status;
}

NV_STATUS nvGpuOpsHasPendingNonReplayableFaults(gpuFaultInfo *pFaultInfo,
                                                NvBool *hasPendingFaults)
{
    GMMU_SHADOW_FAULT_BUF *pQueue =
        (GMMU_SHADOW_FAULT_BUF *) pFaultInfo->nonReplayable.shadowBufferAddress;

    if (!pQueue || !hasPendingFaults)
        return NV_ERR_INVALID_ARGUMENT;

    *hasPendingFaults = !queueIsEmpty(pQueue);

    return NV_OK;
}

NV_STATUS nvGpuOpsGetNonReplayableFaults(gpuFaultInfo *pFaultInfo,
                                         void *faultBuffer,
                                         NvU32 *numFaults)
{
    GMMU_SHADOW_FAULT_BUF *pQueue =
        (GMMU_SHADOW_FAULT_BUF *) pFaultInfo->nonReplayable.shadowBufferAddress;
    QueueContext *pQueueCtx =
        (QueueContext *) pFaultInfo->nonReplayable.shadowBufferContext;

    if (!pQueue || !faultBuffer || !numFaults)
        return NV_ERR_INVALID_ARGUMENT;

    *numFaults = 0;

    // Copy all faults in the client shadow fault buffer to the given buffer
    while (queuePopAndCopyNonManaged(pQueue, pQueueCtx, faultBuffer))
    {
        ++(*numFaults);
        faultBuffer = (char *)faultBuffer + NVC369_BUF_SIZE;
    }

    return NV_OK;
}

static NV_STATUS nvGpuOpsVerifyChannel(struct gpuAddressSpace *vaSpace,
                                       NvHandle hClient,
                                       NvHandle hKernelChannel,
                                       OBJGPU **pGpu,
                                       KernelChannel **ppKernelChannel)
{
    NV_STATUS status = NV_OK;
    NvHandle hDevice, hSubDevice;
    OBJVASPACE *pVAS = NULL;
    OBJGPU *pVaSpaceGpu;
    RsClient *pClient;

    NV_ASSERT_OR_RETURN(ppKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    status = serverGetClientUnderLock(&g_resServ, vaSpace->device->session->handle, &pClient);
    if (status != NV_OK)
        return status;

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if (status != NV_OK)
        return status;

    status = CliGetKernelChannel(hClient, hKernelChannel, ppKernelChannel);
    if (status != NV_OK)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    hDevice = RES_GET_HANDLE(GPU_RES_GET_DEVICE(*ppKernelChannel));
    status = CliSetGpuContext(hClient, hDevice, pGpu, NULL);
    if (status != NV_OK)
        return status;

    if ((*ppKernelChannel)->pVAS != pVAS)
    {
        if (CliSetGpuContext(vaSpace->device->session->handle,
                             vaSpace->device->handle,
                             &pVaSpaceGpu,
                             NULL) == NV_OK && pVaSpaceGpu != *pGpu)
            return NV_ERR_OTHER_DEVICE_FOUND;

        return NV_ERR_INVALID_CHANNEL;
    }

    // In SLI config, RM's internal allocations such as channel instance
    // are tracked with a memdesc per subdevice. Hence, Get the correct pGpu.
    status = CliSetSubDeviceContext(vaSpace->device->session->handle,
                                    vaSpace->device->subhandle,
                                    &hSubDevice,
                                    pGpu);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

static NV_STATUS nvGpuOpsGetChannelEngineType(OBJGPU *pGpu,
                                              KernelChannel *pKernelChannel,
                                              UVM_GPU_CHANNEL_ENGINE_TYPE *engineType)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 engDesc, engineType2080;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    status = kchannelGetEngine_HAL(pGpu, pKernelChannel, &engDesc);
    if (status != NV_OK)
        return status;

    status = kfifoEngineInfoXlate_HAL(pGpu,
                                      pKernelFifo,
                                      ENGINE_INFO_TYPE_ENG_DESC,
                                      engDesc,
                                      ENGINE_INFO_TYPE_NV2080,
                                      &engineType2080);
    if (status != NV_OK)
        return status;

    if (NV2080_ENGINE_TYPE_IS_GR(engineType2080))
        *engineType = UVM_GPU_CHANNEL_ENGINE_TYPE_GR;
    else if (engineType2080 == NV2080_ENGINE_TYPE_SEC2)
        *engineType = UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2;
    else
        *engineType = UVM_GPU_CHANNEL_ENGINE_TYPE_CE;

    return NV_OK;
}

static void _memdescRetain(MEMORY_DESCRIPTOR *pMemDesc)
{
    if (pMemDesc->Allocated > 0)
    {
        pMemDesc->Allocated++;
    }

    memdescAddRef(pMemDesc);
}

static NV_STATUS nvGpuOpsGetChannelInstanceMemInfo(gpuRetainedChannel *retainedChannel,
                                                   gpuChannelInstanceInfo *channelInstanceInfo)
{
    PMEMORY_DESCRIPTOR pMemDesc = NULL;
    NV2080_CTRL_FIFO_MEM_INFO instanceMemInfo;
    NV_STATUS status;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(retainedChannel->pGpu);
    KernelChannel *pKernelChannel = NULL;
    CHID_MGR *pChidMgr = kfifoGetChidMgr(retainedChannel->pGpu,
                                         pKernelFifo,
                                         retainedChannel->runlistId);

    pKernelChannel = kfifoChidMgrGetKernelChannel(retainedChannel->pGpu,
                                                  pKernelFifo,
                                                  pChidMgr,
                                                  channelInstanceInfo->chId);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    status = kfifoChannelGetFifoContextMemDesc_HAL(retainedChannel->pGpu,
                                                   pKernelFifo,
                                                   pKernelChannel,
                                                   FIFO_CTX_INST_BLOCK,
                                                   &pMemDesc);
    if (status != NV_OK)
        return status;

    pMemDesc = memdescGetMemDescFromGpu(pMemDesc, retainedChannel->pGpu);

    kfifoFillMemInfo(pKernelFifo, pMemDesc, &instanceMemInfo);

    if (instanceMemInfo.aperture == NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_INVALID)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    retainedChannel->instanceMemDesc = pMemDesc;
    channelInstanceInfo->base = instanceMemInfo.base;
    channelInstanceInfo->sysmem = (instanceMemInfo.aperture != NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_VIDMEM);

    return NV_OK;
}

static NV_STATUS nvGpuOpsGetChannelTsgInfo(gpuRetainedChannel *retainedChannel,
                                           gpuChannelInstanceInfo *channelInstanceInfo,
                                           KernelChannel *pKernelChannel)
{
    OBJGPU             *pGpu = retainedChannel->pGpu;
    KernelFifo         *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    RM_API             *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelChannelGroup *pKernelChannelGroup;
    NvHandle            hDupTsg;
    NvU32               tsgMaxSubctxCount;
    NV_STATUS           status;
    NvBool              bLockAcquire = NV_FALSE;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    pKernelChannelGroup = pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup;
    NV_ASSERT_OR_RETURN(pKernelChannelGroup != NULL, NV_ERR_INVALID_STATE);

    if (rmGpuLockIsOwner())
    {
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        bLockAcquire = NV_TRUE;
    }
    // Take a reference on the TSG ID by duping the TSG. Note that this is
    // the easy way out because we dup more than strictly necessary: every
    // channel registered under the same TSG will re-dup that TSG. In
    // practice there's very little overhead to re-duping the TSG for each
    // channel.
    hDupTsg = NV01_NULL_OBJECT;
    status  = pRmApi->DupObject(pRmApi,
        retainedChannel->session->handle,
        retainedChannel->rmDevice->deviceHandle,
        &hDupTsg,
        RES_GET_CLIENT_HANDLE(pKernelChannel),
        RES_GET_HANDLE(pKernelChannel->pKernelChannelGroupApi),
        NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);
    if (status != NV_OK)
    {
        if (bLockAcquire)
        {
            NV_ASSERT_OK_OR_RETURN(rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GPU_OPS));
        }
        return status;
    }

    if (bLockAcquire)
    {
        if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GPU_OPS)) != NV_OK)
        {
            pRmApi->Free(pRmApi, retainedChannel->session->handle, hDupTsg);
            return status;
        }
    }

    tsgMaxSubctxCount = kfifoChannelGroupGetLocalMaxSubcontext_HAL(
        pGpu, pKernelFifo,
        pKernelChannelGroup,
        pKernelChannelGroup->bLegacyMode);

    channelInstanceInfo->bTsgChannel = NV_TRUE;
    channelInstanceInfo->tsgId = pKernelChannelGroup->grpID;
    channelInstanceInfo->tsgMaxSubctxCount = tsgMaxSubctxCount;

    retainedChannel->hDupTsg = hDupTsg;

    return NV_OK;
}


static NV_STATUS nvGpuOpsGetChannelSmcInfo(gpuRetainedChannel *retainedChannel,
                                           gpuChannelInstanceInfo *channelInstanceInfo,
                                           KernelChannel *pKernelChannel,
                                           struct gpuDevice *device)
{
    channelInstanceInfo->smcEngineId         = 0;
    channelInstanceInfo->smcEngineVeIdOffset = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);

    if (isDeviceAmperePlus(device) && retainedChannel->channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_GR)
    {
        OBJGPU *pGpu = retainedChannel->pGpu;

        if (IS_MIG_IN_USE(pGpu))
        {
            NvU32 grFaultId;
            NvU32 grMmuFaultEngId;

            const NvU32 grIdx = NV2080_ENGINE_TYPE_GR_IDX(kchannelGetEngineType(pKernelChannel));

            NV_ASSERT_OK_OR_RETURN(kfifoEngineInfoXlate_HAL(pGpu,
                                                            GPU_GET_KERNEL_FIFO(pGpu),
                                                            ENGINE_INFO_TYPE_ENG_DESC,
                                                            ENG_GR(grIdx),
                                                            ENGINE_INFO_TYPE_MMU_FAULT_ID,
                                                            &grFaultId));

            grMmuFaultEngId = kgmmuGetGraphicsEngineId_HAL(GPU_GET_KERNEL_GMMU(pGpu));
            NV_ASSERT(grFaultId >= grMmuFaultEngId);

            channelInstanceInfo->smcEngineId         = grIdx;
            channelInstanceInfo->smcEngineVeIdOffset = grFaultId - grMmuFaultEngId;
        }
    }

    return NV_OK;
}


static void nvGpuOpsGetChannelSubctxInfo(gpuRetainedChannel *retainedChannel,
                                         gpuChannelInstanceInfo *channelInstanceInfo,
                                         KernelChannel *pKernelChannel)
{
    OBJGPU *pGpu = retainedChannel->pGpu;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvHandle hDupKernelCtxShare = NV01_NULL_OBJECT;
    RM_API *pRmApi;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN_VOID(pKernelChannel != NULL);

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // Subcontexts are parented by the TSG, so we must have a reference on the
    // TSG in order to retain the subcontext. The exception is if this channel
    // was allocated without a TSG, in which case RM creates an internal TSG and
    // subcontext which we shouldn't attempt to retain. In that case, we will
    // have skipped duping the TSG earlier and hDupTsg == 0.
    //
    // pKernelChannelGroup->bLegacyMode means that the subcontext was
    // created internally by RM, not by the user.
    if (kfifoIsSubcontextSupported(pKernelFifo) &&
        pKernelChannel->pKernelCtxShareApi &&
        retainedChannel->channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_GR &&
        retainedChannel->hDupTsg &&
        !pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup->bLegacyMode)
    {

        status = pRmApi->DupObject(pRmApi,
                                   retainedChannel->session->handle,
                                   retainedChannel->hDupTsg,
                                   &hDupKernelCtxShare,
                                   RES_GET_CLIENT_HANDLE(pKernelChannel),
                                   RES_GET_HANDLE(pKernelChannel->pKernelCtxShareApi),
                                   NV04_DUP_HANDLE_FLAGS_REJECT_KERNEL_DUP_PRIVILEGE);

        NV_ASSERT(status == NV_OK);
        retainedChannel->hDupKernelCtxShare = hDupKernelCtxShare;

        // Faults report the VEID (aka subcontext ID), so we need to retain the
        // subcontext ID. We do that by taking a reference on the entire
        // subcontext object.
        //
        // pKernelCtxShare->pShareData is a pointer to the broadcast kctxshare data object
        // We get VEID for this retained channel's GPU through that.

        // Possibly better to go through the handle qw just duped for this? Nor sure how to do so.
        channelInstanceInfo->subctxId = pKernelChannel->pKernelCtxShareApi->pShareData->subctxId;
        channelInstanceInfo->bInSubctx = NV_TRUE;

        // Make sure that we saw our GPU
        NV_ASSERT(channelInstanceInfo->bInSubctx);
        NV_ASSERT(channelInstanceInfo->subctxId < channelInstanceInfo->tsgMaxSubctxCount);
    }
    else
    {
        channelInstanceInfo->subctxId = 0;
        channelInstanceInfo->bInSubctx = NV_FALSE;
    }
}

// This function verifies that the instance pointer of the retainedChannel still
// refers to a valid channel.
static NV_STATUS nvGpuOpsGetChannelData(gpuRetainedChannel *retainedChannel,
                                        KernelChannel **ppKernelChannel)
{
    NV2080_CTRL_FIFO_MEM_INFO instanceMemInfo;
    INST_BLOCK_DESC inst;
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(retainedChannel->pGpu);

    kfifoFillMemInfo(pKernelFifo, retainedChannel->instanceMemDesc, &instanceMemInfo);

    switch (instanceMemInfo.aperture)
    {
        case NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_COH:
            inst.aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
            break;
        case NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_SYSMEM_NCOH:
            inst.aperture = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
            break;
        case NV2080_CTRL_CMD_FIFO_GET_CHANNEL_MEM_APERTURE_VIDMEM:
            inst.aperture = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
            break;
        default:
            return NV_ERR_INVALID_CHANNEL;
    }

    inst.address = instanceMemInfo.base;
    inst.gfid = GPU_GFID_PF;      // Run in VF context w/o GFID

    return kfifoConvertInstToKernelChannel_HAL(retainedChannel->pGpu,
                                              pKernelFifo,
                                              &inst,
                                              ppKernelChannel);
}

NV_STATUS nvGpuOpsRetainChannel(struct gpuAddressSpace *vaSpace,
                                NvHandle hClient,
                                NvHandle hKernelChannel,
                                gpuRetainedChannel **retainedChannel,
                                gpuChannelInstanceInfo *channelInstanceInfo)
{
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    KernelChannel *pKernelChannel = NULL;
    OBJGPU *pGpu = NULL;
    gpuRetainedChannel *channel = NULL;
    NV_STATUS status = NV_OK;
    struct gpuDevice *device;
    subDeviceDesc *rmSubDevice;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS params = {0};
    NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS channelRetainerParams = {0};
    RM_API *pRmApi = NULL;
    NvHandle hChannelParent = 0;

    if (!vaSpace || !channelInstanceInfo)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      hClient,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    device = vaSpace->device;
    rmSubDevice = device->rmSubDevice;

    status = nvGpuOpsVerifyChannel(vaSpace, hClient, hKernelChannel, &pGpu,
                                   &pKernelChannel);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    portMemSet(channelInstanceInfo, 0, sizeof(*channelInstanceInfo));

    channel = portMemAllocNonPaged(sizeof(*channel));
    if (channel == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    portMemSet(channel, 0, sizeof(*channel));
    channel->device = device;
    channel->rmDevice = device->rmDevice;
    channel->rmSubDevice = rmSubDevice;
    channel->session = device->session;
    channel->pGpu = pGpu;

    channelInstanceInfo->runlistId = kchannelGetRunlistId(pKernelChannel);
    channelInstanceInfo->chId = pKernelChannel->ChID;
    channel->chId = pKernelChannel->ChID;
    channel->runlistId = kchannelGetRunlistId(pKernelChannel);

    status = nvGpuOpsGetChannelEngineType(pGpu, pKernelChannel, &channel->channelEngineType);
    if (status != NV_OK)
        goto error;

    status = nvGpuOpsGetChannelInstanceMemInfo(channel, channelInstanceInfo);
    if (status != NV_OK)
        goto error;

    status = nvGpuOpsGetChannelTsgInfo(channel, channelInstanceInfo,
                                       pKernelChannel);
    if (status != NV_OK)
        goto error;

    status = nvGpuOpsGetChannelSmcInfo(channel, channelInstanceInfo,
                                       pKernelChannel, device);
    if (status != NV_OK)
        goto error;

    nvGpuOpsGetChannelSubctxInfo(channel, channelInstanceInfo, pKernelChannel);

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (channelInstanceInfo->bTsgChannel)
        hChannelParent = channel->hDupTsg;
    else
        hChannelParent = channel->rmDevice->deviceHandle;

    channelRetainerParams.hClient = hClient;
    channelRetainerParams.hChannel = hKernelChannel;

    NV_PRINTF(LEVEL_INFO, "%s:Channel duping is not supported. Fall back to UVM_CHANNEL_RETAINER\n",
              __FUNCTION__);

    status = pRmApi->Alloc(pRmApi,
                           device->session->handle,
                           hChannelParent,
                          &channel->hChannelRetainer,
                           UVM_CHANNEL_RETAINER,
                          &channelRetainerParams);
    if (status != NV_OK)
        goto error;

    // Now get the token for submission on given channel.
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hKernelChannel,
                             NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN,
                             &params,
                             sizeof(params));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                  __LINE__, nvstatusToString(status));
        goto error;
    }

    channelInstanceInfo->workSubmissionToken = params.workSubmitToken;
    channelInstanceInfo->workSubmissionOffset =
            (NvU32 *)((NvU8*)rmSubDevice->clientRegionMapping + NVC361_NOTIFY_CHANNEL_PENDING);

    // In Turing+ gpus, the CLEAR_FAULTED method requires a RM-provided handle
    // to identify the channel.
    //
    // TODO: Bug 1905719: We are currently using the channel handle that is
    // used for the doorbell mechanism. However, the values may differ in the
    // future, so we may need a dedicated API to get the channel handle for
    // CLEAR_FAULTED in RM.
    channelInstanceInfo->clearFaultedToken = channelInstanceInfo->workSubmissionToken;

    if (isDeviceAmperePlus(device))
    {
        void *bar0Mapping = gpuBar0BaseAddress(pGpu);
        NvU32 chramPri;

        NV_ASSERT_OK_OR_GOTO(status, kfifoEngineInfoXlate_HAL(pGpu,
                                                              GPU_GET_KERNEL_FIFO(pGpu),
                                                              ENGINE_INFO_TYPE_RUNLIST,
                                                              kchannelGetRunlistId(pKernelChannel),
                                                              ENGINE_INFO_TYPE_CHRAM_PRI_BASE,
                                                              &chramPri), error);

        chramPri += NV_CHRAM_CHANNEL(pKernelChannel->ChID);

        channelInstanceInfo->pChramChannelRegister = (NvU32 *)((NvU8*)bar0Mapping + chramPri);
    }

    status = _nvGpuOpsRetainChannelResources(device,
                                             hClient,
                                             hKernelChannel,
                                             channel,
                                             channelInstanceInfo);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "%s:%d: %s\n", __FUNCTION__,
                  __LINE__, nvstatusToString(status));
        goto error;
    }

    channelInstanceInfo->channelEngineType = channel->channelEngineType;
    *retainedChannel = channel;

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return NV_OK;

error:
    _nvGpuOpsReleaseChannel(channel);
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

static void _nvGpuOpsReleaseChannel(gpuRetainedChannel *retainedChannel)
{
    NV_STATUS status = NV_OK;
    struct gpuSession *session;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!retainedChannel)
        return;

    _nvGpuOpsReleaseChannelResources(retainedChannel);

    session = retainedChannel->session;
    NV_ASSERT(session);

    if (retainedChannel->hChannelRetainer)
    {
        status = pRmApi->Free(pRmApi, session->handle, retainedChannel->hChannelRetainer);
        NV_ASSERT(status == NV_OK);
    }

    // Release the subcontext if we retained it. Subcontexts are parented by the
    // TSG, so we must release the subcontext before releasing the TSG.
    if (retainedChannel->hDupKernelCtxShare)
    {
        NV_ASSERT(retainedChannel->hDupTsg);
        status = pRmApi->Free(pRmApi, session->handle, retainedChannel->hDupKernelCtxShare);
        NV_ASSERT(status == NV_OK);
    }

    if (retainedChannel->hDupTsg)
    {
        status = pRmApi->Free(pRmApi, session->handle, retainedChannel->hDupTsg);
        NV_ASSERT(status == NV_OK);
    }


    // Releasing the channel ID can only fail if the ID is no longer valid,
    // which indicates a bug elsewhere.
    NV_ASSERT(status == NV_OK);

    portMemFree(retainedChannel);
}

void nvGpuOpsReleaseChannel(gpuRetainedChannel *retainedChannel)
{
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    // TODO can we lock fewer GPUS with Channel information?
    if (_nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                 retainedChannel->session->handle,
                                 NULL,
                                 &acquiredLocks) != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return;
    }
    _nvGpuOpsReleaseChannel(retainedChannel);
    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
}

static void
_shadowMemdescDestroy(gpuRetainedChannel *retainedChannel,
                      MEMORY_DESCRIPTOR *pMemDesc)
{
    if (pMemDesc->RefCount == 1)
    {
        mapRemoveByKey(&retainedChannel->device->kern2PhysDescrMap, (NvU64) pMemDesc);
    }

    memdescDestroy(pMemDesc);
}

NvBool _memDescFindAndRetain(gpuRetainedChannel *retainedChannel,
                             MEMORY_DESCRIPTOR *pBufferHandle,
                             MEMORY_DESCRIPTOR **ppMemDesc)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    MemdescMapIter iter = mapIterAll(&retainedChannel->device->kern2PhysDescrMap);
    while (mapIterNext(&iter))
    {
        MEMORY_DESCRIPTOR **ppValue = iter.pValue;
        if (pBufferHandle == *ppValue)
        {
            NvU64 key = mapKey(&retainedChannel->device->kern2PhysDescrMap, ppValue);
            pMemDesc = (MEMORY_DESCRIPTOR *) key;
            break;
        }
    }

    if (pMemDesc != NULL)
    {
        _memdescRetain(pMemDesc);
        *ppMemDesc = pMemDesc;
        return NV_TRUE;
    }
    return NV_FALSE;
}

static NV_STATUS
_shadowMemdescCreateFlcn(gpuRetainedChannel *retainedChannel,
                     NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pCtxBufferInfo,
                     MEMORY_DESCRIPTOR **ppMemDesc)
{
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    MEMORY_DESCRIPTOR *pBufferHandle = (MEMORY_DESCRIPTOR *) pCtxBufferInfo->bufferHandle;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pCtxBufferInfo->bIsContigous, NV_ERR_INVALID_STATE);

    if (_memDescFindAndRetain(retainedChannel, pBufferHandle, ppMemDesc))
        return status;

    memdescCreate(&pMemDesc,
        retainedChannel->pGpu,
        pCtxBufferInfo->size,
        pCtxBufferInfo->alignment,
        pCtxBufferInfo->bIsContigous,
        pCtxBufferInfo->aperture,
        NV_MEMORY_CACHED,
        MEMDESC_FLAGS_NONE
    );

    memdescSetPageSize(pMemDesc, 0, pCtxBufferInfo->pageSize);

    memdescDescribe(pMemDesc, pCtxBufferInfo->aperture, pCtxBufferInfo->physAddr, pCtxBufferInfo->size);

    (void) mapInsertValue(&retainedChannel->device->kern2PhysDescrMap,
                          (NvU64) pMemDesc,
                          &pBufferHandle);
    *ppMemDesc = pMemDesc;

    return status;
}


static NV_STATUS
_shadowMemdescCreate(gpuRetainedChannel *retainedChannel,
                     NV2080_CTRL_GR_CTX_BUFFER_INFO *pCtxBufferInfo,
                     MEMORY_DESCRIPTOR **ppMemDesc)
{
    NvU32 j;
    NvU32 pageSize = pCtxBufferInfo->pageSize;
    NvU32 numBufferPages = NV_ROUNDUP(pCtxBufferInfo->size, pageSize) / pageSize;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    MEMORY_DESCRIPTOR *pBufferHandle = (MEMORY_DESCRIPTOR *) pCtxBufferInfo->bufferHandle;
    NV2080_CTRL_KGR_GET_CTX_BUFFER_PTES_PARAMS params = { 0 };
    NvU64 *pPages = NULL;
    NV_STATUS status = NV_OK;
    KernelChannel *pKernelChannel;
    RM_API *pRmApi;

    if (_memDescFindAndRetain(retainedChannel, pBufferHandle, ppMemDesc))
        goto done;

    pPages = portMemAllocNonPaged(sizeof(*pPages) * numBufferPages);
    if (pPages == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    status = nvGpuOpsGetChannelData(retainedChannel, &pKernelChannel);
    if (status != NV_OK)
    {
        goto done;
    }

    params.hUserClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
    params.hChannel = RES_GET_HANDLE(pKernelChannel);
    params.bufferType = pCtxBufferInfo->bufferType;

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    for (j = 0; j < numBufferPages;)
    {
        params.firstPage = j;
        status = pRmApi->Control(pRmApi,
                                 retainedChannel->session->handle,
                                 retainedChannel->rmSubDevice->subDeviceHandle,
                                 NV2080_CTRL_CMD_KGR_GET_CTX_BUFFER_PTES,
                                 &params,
                                 sizeof(params));
        if (status != NV_OK)
        {
            goto done;
        }

        NV_ASSERT(j + params.numPages <= numBufferPages);

        if (pCtxBufferInfo->bIsContigous)
        {
            pPages[0] = (NvU64)params.physAddrs[0];
            break;
        }

        portMemCopy(&pPages[j], params.numPages * sizeof(*pPages),
                    params.physAddrs, params.numPages * sizeof(*pPages));
        j += params.numPages;
    }

    NV_ASSERT(params.bNoMorePages);

    memdescCreate(&pMemDesc,
        retainedChannel->pGpu,
        pCtxBufferInfo->size,
        pCtxBufferInfo->alignment,
        pCtxBufferInfo->bIsContigous,
        pCtxBufferInfo->aperture,
        NV_MEMORY_CACHED,
        MEMDESC_FLAGS_NONE
    );

    memdescSetPageSize(pMemDesc, 0, pCtxBufferInfo->pageSize);

    if (pCtxBufferInfo->bIsContigous)
    {
        memdescDescribe(pMemDesc, pCtxBufferInfo->aperture, pPages[0], pCtxBufferInfo->size);
    }
    else
    {
        memdescFillPages(pMemDesc, 0, pPages, numBufferPages, pCtxBufferInfo->pageSize);
    }

    (void) mapInsertValue(&retainedChannel->device->kern2PhysDescrMap,
                          (NvU64) pMemDesc,
                          &pBufferHandle);
    *ppMemDesc = pMemDesc;

done:
    portMemFree(pPages);
    return status;
}

static NV_STATUS _nvGpuOpsRetainChannelResources(struct gpuDevice *device,
                                                 NvHandle hClient,
                                                 NvHandle hKernelChannel,
                                                 gpuRetainedChannel *retainedChannel,
                                                 gpuChannelInstanceInfo *channelInstanceInfo)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GR_GET_CTX_BUFFER_INFO_PARAMS *pParams = NULL;
    NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pFlcnParams = NULL;
    gpuChannelResourceInfo *channelResourceInfo = channelInstanceInfo->resourceInfo;
    KernelChannel *pKernelChannel;
    RM_API *pRmApi;
    NvU32 channelEngineType = retainedChannel->channelEngineType;
    NvU32 i;
    NvU32 j;

    NV_ASSERT(channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE ||
              channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_GR ||
              channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2);

    // CE channels have 0 resources, so they skip this step
    if (channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_CE)
    {
        goto done;
    }

    status = nvGpuOpsGetChannelData(retainedChannel, &pKernelChannel);
    if (status != NV_OK)
    {
        goto done;
    }

    if (channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2)
    {
        // get engine context memdesc, then get its PTEs.
        MEMORY_DESCRIPTOR *pMemDesc = NULL;

        // single buffer
        NV_ASSERT_OR_GOTO(NV_ARRAY_ELEMENTS(channelInstanceInfo->resourceInfo) >= 1, done);

        pFlcnParams = portMemAllocNonPaged(sizeof(*pFlcnParams));
        if (pFlcnParams == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
        pFlcnParams->hUserClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
        pFlcnParams->hChannel = RES_GET_HANDLE(pKernelChannel);

        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        // This RM CTRL refcounts all the resource memdescs.
        status = pRmApi->Control(pRmApi,
                                 retainedChannel->session->handle,
                                 retainedChannel->rmSubDevice->subDeviceHandle,
                                 NV2080_CTRL_CMD_FLCN_GET_CTX_BUFFER_INFO,
                                 pFlcnParams,
                                 sizeof(*pFlcnParams));
        if (status != NV_OK)
            goto done;

        gpuMemoryInfo *pGpuMemoryInfo = &channelResourceInfo[0].resourceInfo;

        channelResourceInfo[0].resourceDescriptor = pFlcnParams->bufferHandle;
        channelResourceInfo[0].alignment = pFlcnParams->alignment;
        pGpuMemoryInfo->pageSize = pFlcnParams->pageSize;
        pGpuMemoryInfo->size = pFlcnParams->size;
        pGpuMemoryInfo->contig = pFlcnParams->bIsContigous;
        pGpuMemoryInfo->physAddr = pFlcnParams->physAddr;
        pGpuMemoryInfo->kind = pFlcnParams->kind;
        pGpuMemoryInfo->sysmem = pFlcnParams->aperture == ADDR_SYSMEM;
        pGpuMemoryInfo->deviceDescendant = pFlcnParams->bDeviceDescendant;

        portMemCopy(pGpuMemoryInfo->uuid.uuid, sizeof(pGpuMemoryInfo->uuid.uuid),
            pFlcnParams->uuid, sizeof(pFlcnParams->uuid));

        status = _shadowMemdescCreateFlcn(retainedChannel, pFlcnParams, &pMemDesc);
        if (status != NV_OK)
            goto done;

        channelResourceInfo[0].resourceDescriptor = (NvP64) pMemDesc;
        retainedChannel->resourceMemDesc[0] =  pMemDesc;

        channelInstanceInfo->resourceCount = 1;
        retainedChannel->resourceCount = 1;
        goto done;
    }

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pParams->hUserClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
    pParams->hChannel = RES_GET_HANDLE(pKernelChannel);

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    // This RM CTRL refcounts all the resource memdescs.
    status = pRmApi->Control(pRmApi,
                             retainedChannel->session->handle,
                             retainedChannel->rmSubDevice->subDeviceHandle,
                             NV2080_CTRL_CMD_GR_GET_CTX_BUFFER_INFO,
                             pParams,
                             sizeof(*pParams));
    if (status != NV_OK)
        goto done;

    NV_ASSERT(pParams->bufferCount <= NV_ARRAY_ELEMENTS(channelInstanceInfo->resourceInfo));

    for (i = 0; i < pParams->bufferCount; i++)
    {
        MEMORY_DESCRIPTOR *pMemDesc = NULL;
        NV2080_CTRL_GR_CTX_BUFFER_INFO *pCtxBufferInfo = &pParams->ctxBufferInfo[i];
        gpuMemoryInfo *pGpuMemoryInfo = &channelResourceInfo[i].resourceInfo;

        channelResourceInfo[i].resourceDescriptor = pCtxBufferInfo->bufferHandle;
        channelResourceInfo[i].resourceId = pCtxBufferInfo->bufferType;
        channelResourceInfo[i].alignment = pCtxBufferInfo->alignment;
        pGpuMemoryInfo->pageSize = pCtxBufferInfo->pageSize;
        pGpuMemoryInfo->size = pCtxBufferInfo->size;
        pGpuMemoryInfo->contig = pCtxBufferInfo->bIsContigous;
        pGpuMemoryInfo->physAddr = pCtxBufferInfo->physAddr;
        pGpuMemoryInfo->kind = pCtxBufferInfo->kind;
        pGpuMemoryInfo->sysmem = pCtxBufferInfo->aperture == ADDR_SYSMEM;
        pGpuMemoryInfo->deviceDescendant = pCtxBufferInfo->bDeviceDescendant;

        portMemCopy(pGpuMemoryInfo->uuid.uuid, sizeof(pGpuMemoryInfo->uuid.uuid),
                    pCtxBufferInfo->uuid, sizeof(pCtxBufferInfo->uuid));

        status = _shadowMemdescCreate(retainedChannel, pCtxBufferInfo, &pMemDesc);
        if (status != NV_OK)
            goto cleanup;

        channelResourceInfo[i].resourceDescriptor = (NvP64) pMemDesc;
        retainedChannel->resourceMemDesc[i] =  pMemDesc;
    }

    channelInstanceInfo->resourceCount = pParams->bufferCount;
    retainedChannel->resourceCount = pParams->bufferCount;

cleanup:
    if (status != NV_OK)
    {
        for (j = 0; j < i; j++)
        {
            _shadowMemdescDestroy(retainedChannel, retainedChannel->resourceMemDesc[j]);
        }
    }

done:
    portMemFree(pParams);
    portMemFree(pFlcnParams);
    return status;
}

static void _nvGpuOpsReleaseChannelResources(gpuRetainedChannel *retainedChannel)
{
    NvU32 i;
    NvU32 descriptorCount = retainedChannel->resourceCount;

    for (i = 0; i < descriptorCount; i++)
    {
        MEMORY_DESCRIPTOR *pMemDesc = retainedChannel->resourceMemDesc[i];

        _shadowMemdescDestroy(retainedChannel, pMemDesc);
    }
}

NV_STATUS nvGpuOpsGetChannelResourcePtes(struct gpuAddressSpace *vaSpace,
                                         NvP64 resourceDescriptor,
                                         NvU64 offset,
                                         NvU64 size,
                                         gpuExternalMappingInfo *pGpuExternalMappingInfo)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NvHandle hSubDevice;
    PMEMORY_DESCRIPTOR pMemDesc = NULL;
    OBJGPU *pMappingGpu = NULL;
    OBJVASPACE *pVAS = NULL;
    RsClient *pClient;

    if (!vaSpace || !resourceDescriptor || !pGpuExternalMappingInfo)
        return NV_ERR_INVALID_ARGUMENT;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      vaSpace->device->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    pMemDesc = (MEMORY_DESCRIPTOR *) NvP64_VALUE(resourceDescriptor);

    status = CliSetSubDeviceContext(vaSpace->device->session->handle,
                                    vaSpace->device->subhandle,
                                    &hSubDevice,
                                    &pMappingGpu);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    if (pMemDesc->pGpu != pMappingGpu)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_NOT_SUPPORTED;
    }

    // Do not support mapping on anything other than sysmem/vidmem!
    if ((memdescGetAddressSpace(pMemDesc) != ADDR_SYSMEM) &&
        (memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM))
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_NOT_SUPPORTED;
    }

    status = serverGetClientUnderLock(&g_resServ, vaSpace->device->session->handle, &pClient);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = vaspaceGetByHandleOrDeviceDefault(pClient,
                                               vaSpace->device->handle,
                                               vaSpace->handle,
                                               &pVAS);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = nvGpuOpsBuildExternalAllocPtes(pVAS, pMappingGpu, pMemDesc, NULL,
                                            offset, size, NV_FALSE, NV_FALSE,
                                            0, pGpuExternalMappingInfo);

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

NV_STATUS nvGpuOpsBindChannelResources(gpuRetainedChannel *retainedChannel,
                                       gpuChannelResourceBindParams *channelResourceBindParams)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pParams;
    NvU32 i;
    KernelChannel *pKernelChannel = NULL;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                      retainedChannel->session->handle,
                                      NULL,
                                      &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = nvGpuOpsGetChannelData(retainedChannel, &pKernelChannel);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    // Unregister channel resources. CE channels have 0 resources, so they skip this step
    if (retainedChannel->resourceCount != 0)
    {
        pParams = portMemAllocNonPaged(sizeof(*pParams));
        if (pParams == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            _nvGpuOpsLocksRelease(&acquiredLocks);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        portMemSet(pParams, 0, sizeof(*pParams));

        pParams->hChanClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
        pParams->hObject = RES_GET_HANDLE(pKernelChannel);
        pParams->entryCount = retainedChannel->resourceCount;

        status = kfifoEngineInfoXlate_HAL(retainedChannel->pGpu,
                                          GPU_GET_KERNEL_FIFO(retainedChannel->pGpu),
                                          ENGINE_INFO_TYPE_RUNLIST,
                                          retainedChannel->runlistId,
                                          ENGINE_INFO_TYPE_NV2080,
                                          &(pParams->engineType));

        for (i = 0; i < retainedChannel->resourceCount; i++)
        {
            if (NV2080_ENGINE_TYPE_IS_GR(pParams->engineType))
                pParams->promoteEntry[i].bufferId = channelResourceBindParams[i].resourceId;
            pParams->promoteEntry[i].gpuVirtAddr = channelResourceBindParams[i].resourceVa;
        }

        status = pRmApi->Control(pRmApi,
                                 retainedChannel->session->handle,
                                 retainedChannel->rmSubDevice->subDeviceHandle,
                                 NV2080_CTRL_CMD_GPU_PROMOTE_CTX,
                                 pParams,
                                 sizeof(*pParams));

        portMemFree(pParams);
    }

    if (NV_OK == status)
    {
        pKernelChannel->bIsContextBound = NV_TRUE;
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return status;
}

// nvGpuOpsRetainChannelResources only increments the ref-counts of the memdescs under the channel.
// It does not prevent the user from freeing the associated hClient and hChannel handles, which means
// the instance pointer may no longer be associated with a user object at this point.
// If the instance pointer still has an associated channel, the channel is preempted and disabled.
// Otherwise that must have already happened, so we just need to drop the ref counts on the resources
void nvGpuOpsStopChannel(gpuRetainedChannel *retainedChannel,
                         NvBool bImmediate)
{
    NV_STATUS status = NV_OK;
    nvGpuOpsLockSet acquiredLocks;
    THREAD_STATE_NODE threadState;
    KernelChannel *pKernelChannel = NULL;
    RsResourceRef *pResourceRef;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NVA06F_CTRL_STOP_CHANNEL_PARAMS stopChannelParams = {0};

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (_nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ,
                                 retainedChannel->session->handle,
                                 NULL,
                                 &acquiredLocks) != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return;
    }

    status = nvGpuOpsGetChannelData(retainedChannel, &pKernelChannel);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return;
    }

    // Verify this channel handle is still valid
    status = serverutilGetResourceRef(RES_GET_CLIENT_HANDLE(pKernelChannel), RES_GET_HANDLE(pKernelChannel), &pResourceRef);
    if (status != NV_OK)
    {
        NV_ASSERT(0);
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return;
    }

    stopChannelParams.bImmediate = bImmediate;
    NV_ASSERT_OK(
        pRmApi->Control(pRmApi,
                        RES_GET_CLIENT_HANDLE(pKernelChannel),
                        RES_GET_HANDLE(pKernelChannel),
                        NVA06F_CTRL_CMD_STOP_CHANNEL,
                        &stopChannelParams,
                        sizeof(stopChannelParams)));

    pKernelChannel->bIsContextBound = NV_FALSE;

    if (retainedChannel->channelEngineType == UVM_GPU_CHANNEL_ENGINE_TYPE_GR)
    {
        NV2080_CTRL_GPU_EVICT_CTX_PARAMS params;

        portMemSet(&params, 0, sizeof(params));
        params.engineType = NV2080_ENGINE_TYPE_GR(0);
        params.hClient = retainedChannel->session->handle;
        params.hChanClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
        params.hObject = RES_GET_HANDLE(pKernelChannel);

        NV_ASSERT_OK(
            pRmApi->Control(pRmApi,
                            retainedChannel->session->handle,
                            retainedChannel->rmSubDevice->subDeviceHandle,
                            NV2080_CTRL_CMD_GPU_EVICT_CTX,
                            &params,
                            sizeof(params)));
    }

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
}

// Make sure the UVM and PMA structs are in sync
// The following location(s) need to be synced as well:
// - uvm8_pmm_gpu.c:uvm8_test_pmm_query_pma_stats
ct_assert(sizeof(UvmPmaStatistics) == sizeof(PMA_STATS));
ct_assert(NV_OFFSETOF(UvmPmaStatistics, numPages2m) == NV_OFFSETOF(PMA_STATS, num2mbPages));
ct_assert(NV_OFFSETOF(UvmPmaStatistics, numFreePages64k) == NV_OFFSETOF(PMA_STATS, numFreeFrames));
ct_assert(NV_OFFSETOF(UvmPmaStatistics, numFreePages2m) == NV_OFFSETOF(PMA_STATS, numFree2mbPages));

/*!
 *  Retrieve the PMA (Physical Memory Allocator) object initialized by RM
 *  for the given device.
 *
 *  @param[in]  device          device handle obtained in a prior call
 *                              to nvGpuOpsRmDeviceCreate.
 *
 *  @param[out] pPmaObject      Void pointer to RM PMA object of associated GPU
 *                              NULL if PMA not enabled & initialized.
 *  @param[out] pPmaPubStats    Pointer to UVM PMA statistics object of
 *                              associated GPU. Cannot be NULL.
 *
 * @returns     NV_OK on success,
 *              NV_ERR_INVALID_ARGUMENT if NULL pPmaObject,
 *              NV_ERR_OBJECT_NOT_FOUND if PMA object not found
 *              NV_ERR_NOT_SUPPORTED if PMA not supported
 */
NV_STATUS nvGpuOpsGetPmaObject(struct gpuDevice *device,
                               void **pPmaObject,
                               const UvmPmaStatistics **pPmaStats)
{
    nvGpuOpsLockSet     acquiredLocks;
    THREAD_STATE_NODE   threadState;
    OBJGPU             *pGpu    = NULL;
    Heap               *pHeap   = NULL;
    MemoryManager *pMemoryManager;
    struct gpuSession *session = device->session;
    NV_STATUS status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = _nvGpuOpsLocksAcquireAll(RMAPI_LOCK_FLAGS_READ, session->handle, NULL, &acquiredLocks);
    if (status != NV_OK)
    {
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return status;
    }

    status = CliSetGpuContext(session->handle, device->handle, &pGpu, NULL);
    if (status != NV_OK)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    if (pMemoryManager == NULL)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

        status = kmigmgrGetMemoryPartitionHeapFromClient(pGpu, pKernelMIGManager, session->handle, &pHeap);
        if (status != NV_OK)
            return status;
    }
    else
        pHeap = GPU_GET_HEAP(pGpu);

    if (pHeap == NULL)
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    if (!memmgrIsPmaInitialized(pMemoryManager))
    {
        _nvGpuOpsLocksRelease(&acquiredLocks);
        threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *pPmaObject = (void *)&pHeap->pmaObject;
    *pPmaStats = (const UvmPmaStatistics *)&pHeap->pmaObject.pmaStats;

    _nvGpuOpsLocksRelease(&acquiredLocks);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    return NV_OK;
}

NV_STATUS nvGpuOpsP2pObjectCreate(struct gpuDevice *device1,
                                  struct gpuDevice *device2,
                                  NvHandle *hP2pObject)
{
    NV_STATUS status;
    NV503B_ALLOC_PARAMETERS p2pAllocParams = {0};
    NvHandle hTemp = 0;
    struct systemP2PCaps p2pCaps;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    struct gpuSession *session;

    if (!device1 || !device2 || !hP2pObject)
        return NV_ERR_INVALID_ARGUMENT;

    if (device1->session != device2->session)
        return NV_ERR_INVALID_ARGUMENT;

    status = getSystemP2PCaps(device1, device2, &p2pCaps);
    if (status != NV_OK)
        return status;

    if (!p2pCaps.accessSupported)
        return NV_ERR_NOT_SUPPORTED;

    p2pAllocParams.hSubDevice = device1->subhandle;
    p2pAllocParams.hPeerSubDevice = device2->subhandle;

    session = device1->session;
    hTemp = NV01_NULL_OBJECT;
    status = pRmApi->Alloc(pRmApi, session->handle, session->handle, &hTemp, NV50_P2P, &p2pAllocParams);
    if (status == NV_OK)
        *hP2pObject = hTemp;

    return status;
}

NV_STATUS nvGpuOpsP2pObjectDestroy(struct gpuSession *session,
                                   NvHandle hP2pObject)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NV_ASSERT(session);

    status = pRmApi->Free(pRmApi, session->handle, hP2pObject);
    NV_ASSERT(status == NV_OK);
    return status;
}

NV_STATUS nvGpuOpsReportNonReplayableFault(struct gpuDevice *device,
                                           const void *pFaultPacket)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GPU_REPORT_NON_REPLAYABLE_FAULT_PARAMS params;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);

    if (device == NULL || pFaultPacket == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    portMemSet(&params, 0, sizeof(params));

    portMemCopy(&params.faultPacket.data,
                NV2080_CTRL_GPU_FAULT_PACKET_SIZE,
                pFaultPacket,
                NV2080_CTRL_GPU_FAULT_PACKET_SIZE);

    status = pRmApi->Control(pRmApi,
                             device->session->handle,
                             device->subhandle,
                             NV2080_CTRL_CMD_GPU_REPORT_NON_REPLAYABLE_FAULT,
                             &params,
                             sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: NV2080_CTRL_CMD_GPU_REPORT_NON_REPLAYABLE_FAULTreturned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
    }

    return status;
}

NV_STATUS nvGpuOpsPagingChannelAllocate(struct gpuDevice *device,
                                        const gpuPagingChannelAllocParams *params,
                                        gpuPagingChannelHandle *channelHandle,
                                        gpuPagingChannelInfo *channelInfo)
{
    NV_STATUS status, status2;
    UvmGpuPagingChannel *channel = NULL;
    Device *pDevice;
    RsClient *pClient;
    NvHandle hClient;
    NvLength errorNotifierSize;
    NvU64 paOffset;
    gpuAllocInfo allocInfo = {0};
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NvU32 pid = osGetCurrentProcess();

    if (!device || !params || !channelHandle || !channelInfo)
        return NV_ERR_INVALID_ARGUMENT;

    if (!NV2080_ENGINE_TYPE_IS_COPY(NV2080_ENGINE_TYPE_COPY(params->engineIndex)))
        return NV_ERR_INVALID_ARGUMENT;

    hClient = device->session->handle;
    NV_ASSERT(hClient);

    channel = portMemAllocNonPaged(sizeof(*channel));
    if (!channel)
        return NV_ERR_NO_MEMORY;

    portMemSet(channel, 0, sizeof(*channel));
    channel->device = device;

    errorNotifierSize = sizeof(NvNotification) *
                        NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1;
    status = nvGpuOpsAllocPhysical(device,
                                   NV_TRUE,
                                   errorNotifierSize,
                                   &paOffset,
                                   &allocInfo);
    if (status != NV_OK)
        goto cleanup_free_channel;

    channel->errorNotifierHandle = allocInfo.hPhysHandle;
    NV_ASSERT(channel->errorNotifierHandle);

    status = pRmApi->MapToCpu(pRmApi,
                              hClient,
                              device->subhandle,
                              channel->errorNotifierHandle,
                              0,
                              errorNotifierSize,
                              (void **)&channel->errorNotifier,
                              0);
    if (status != NV_OK)
        goto cleanup_free_error_notifier;

    NV_ASSERT(channel->errorNotifier);

    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        goto cleanup_unmap_error_notifier;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        goto cleanup_under_rmapi_lock;

    status = deviceGetByHandle(pClient, device->handle, &pDevice);
    if (status != NV_OK)
        goto cleanup_under_rmapi_lock;

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

    rmapiLockRelease();

    *channelHandle = channel;

    channelInfo->shadowErrorNotifier = channel->errorNotifier;

    return NV_OK;

cleanup_under_rmapi_lock:
    rmapiLockRelease();

cleanup_unmap_error_notifier:
    status2 = pRmApi->UnmapFromCpu(pRmApi,
                                   hClient,
                                   device->subhandle,
                                   channel->errorNotifierHandle,
                                   (void *)channel->errorNotifier,
                                   0,
                                   pid);
    NV_ASSERT(status2 == NV_OK);

cleanup_free_error_notifier:
    pRmApi->Free(pRmApi, hClient, channel->errorNotifierHandle);

cleanup_free_channel:
    portMemFree(channel);

    return status;
}

void nvGpuOpsPagingChannelDestroy(UvmGpuPagingChannel *channel)
{
    NV_STATUS status;
    struct gpuDevice *device;
    Device *pDevice;
    RsClient *pClient;
    NvHandle hClient;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL_KERNEL);
    NvU32 pid = osGetCurrentProcess();

    NV_ASSERT(channel);

    device = channel->device;
    NV_ASSERT(device);

    hClient = device->session->handle;
    NV_ASSERT(hClient);

    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: rmapiLockAcquire returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        goto cleanup;
    }

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: serverGetClientUnderLock returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        goto cleanup_under_rmapi_lock;
    }

    status = deviceGetByHandle(pClient, device->handle, &pDevice);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: deviceGetByHandle returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        goto cleanup_under_rmapi_lock;
    }

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

cleanup_under_rmapi_lock:
    rmapiLockRelease();

cleanup:
    status = pRmApi->UnmapFromCpu(pRmApi,
                                  hClient,
                                  device->subhandle,
                                  channel->errorNotifierHandle,
                                  (void *)channel->errorNotifier,
                                  0,
                                  pid);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: UnmapFromCpu returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
    }

    pRmApi->Free(pRmApi, hClient, channel->errorNotifierHandle);
    portMemFree(channel);
}

NV_STATUS nvGpuOpsPagingChannelsMap(struct gpuAddressSpace *srcVaSpace,
                                    NvU64 srcAddress,
                                    struct gpuDevice *device,
                                    NvU64 *dstAddress)
{
    NV_STATUS status;
    Device *pDevice;
    RsClient *pClient;
    NvHandle hAllocation;
    NvHandle hClient;

    if (!srcVaSpace || !device || !dstAddress)
        return NV_ERR_INVALID_ARGUMENT;

    hClient = device->session->handle;
    NV_ASSERT(hClient);


    status = getHandleForVirtualAddr(srcVaSpace, srcAddress, NV_TRUE, &hAllocation);
    if (status != NV_OK)
        return status;

    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    if (status != NV_OK)
        return status;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        goto exit_under_rmapi_lock;

    status = deviceGetByHandle(pClient, device->handle, &pDevice);
    if (status != NV_OK)
        goto exit_under_rmapi_lock;

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

exit_under_rmapi_lock:
    rmapiLockRelease();

    return status;
}

void nvGpuOpsPagingChannelsUnmap(struct gpuAddressSpace *srcVaSpace,
                                 NvU64 srcAddress,
                                 struct gpuDevice *device)
{
    NV_STATUS status;
    Device *pDevice;
    RsClient *pClient;
    NvHandle hAllocation;
    NvHandle hClient;

    NV_ASSERT(srcVaSpace && device);
    if (!srcVaSpace || !device)
        return;

    hClient = device->session->handle;
    NV_ASSERT(hClient);

    status = getHandleForVirtualAddr(srcVaSpace, srcAddress, NV_TRUE, &hAllocation);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: getHandleForVirtualAddr returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        return;
    }


    status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU_OPS);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: rmapiLockAcquire returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        return;
    }

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: serverGetClientUnderLock returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        goto exit_under_rmapi_lock;
    }

    status = deviceGetByHandle(pClient, device->handle, &pDevice);
    NV_ASSERT(status == NV_OK);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: deviceGetByHandle returned error %s!\n",
                  __FUNCTION__, nvstatusToString(status));
        goto exit_under_rmapi_lock;
    }

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

exit_under_rmapi_lock:
    rmapiLockRelease();
}

NV_STATUS nvGpuOpsPagingChannelPushStream(UvmGpuPagingChannel *channel,
                                          char *methodStream,
                                          NvU32 methodStreamSize)
{
    NV_STATUS status;
    struct gpuDevice *device = NULL;
    Device *pDevice;
    RsClient *pClient;

    if (!channel || !methodStream)
        return NV_ERR_INVALID_ARGUMENT;
    if (methodStreamSize == 0)
        return NV_OK;

    device = channel->device;
    NV_ASSERT(device);

    status = serverGetClientUnderLock(&g_resServ, device->session->handle, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, device->handle, &pDevice);
    if (status != NV_OK)
        return status;

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

    return status;
}

static NV_STATUS nvGpuOpsGetMemoryByHandle(NvHandle hClient, NvHandle hMemory, Memory **ppMemory)
{
    RsClient *pRsClient = NULL;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ,
        hClient, &pRsClient));

    return memGetByHandle(pRsClient,
                          hMemory,
                          ppMemory);
}

