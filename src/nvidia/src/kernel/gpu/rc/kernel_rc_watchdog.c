/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/core/core.h"
#include "kernel/core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "kernel/gpu/bus/kern_bus.h"
#include "kernel/os/os.h"
#include "platform/sli/sli.h"

#include "class/cl0000.h" // NV01_NULL_OBJECT
#include "class/cl0002.h" // NV01_CONTEXT_DMA
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl0070.h" // NV01_MEMORY_VIRTUAL
#include "class/cl0080.h" // NV01_DEVICE_0
#include "class/cl2080.h" // NV20_SUBDEVICE_0
#include "class/cl902d.h" // FERMI_TWOD_A
#include "class/cl906f.h" // GF100_CHANNEL_GPFIFO
#include "class/cla06f.h" // KEPLER_CHANNEL_GPFIFO_A
#include "class/cla06fsubch.h"
#include "class/cla16f.h" // KEPLER_CHANNEL_GPFIFO_B
#include "class/clb06f.h" // MAXWELL_CHANNEL_GPFIFO_A
#include "class/clc06f.h" // PASCAL_CHANNEL_GPFIFO_A
#include "class/clc36f.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h" // TURING_CHANNEL_GPFIFO_A
#include "class/clc56f.h" // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc86f.h" // HOPPER_CHANNEL_GPFIFO_A

#include "class/clc96f.h" // BLACKWELL_CHANNEL_GPFIFO_A

#include "deprecated/rmapi_deprecated.h"
#include "nvrm_registry.h"

//
// Watchdog object ids
//
#define WATCHDOG_PUSHBUFFER_CHANNEL_ID 0x31415900
#define WATCHDOG_NOTIFIER_DMA_ID       (WATCHDOG_PUSHBUFFER_CHANNEL_ID + 2)
#define WATCHDOG_DEVICE_ID             (WATCHDOG_PUSHBUFFER_CHANNEL_ID + 3)
#define WATCHDOG_SUB_DEVICE_0_ID       (WATCHDOG_PUSHBUFFER_CHANNEL_ID + 4)
#define WATCHDOG_GROBJ_ID              (WATCHDOG_SUB_DEVICE_0_ID + NV_MAX_SUBDEVICES)
#define WATCHDOG_ERROR_DMA_ID          (WATCHDOG_GROBJ_ID + 1)
#define WATCHDOG_MEM_ID                (WATCHDOG_GROBJ_ID + 2)
#define WATCHDOG_VIRTUAL_CTX_ID        (WATCHDOG_GROBJ_ID + 3)
#define WATCHDOG_USERD_PHYS_MEM_ID     (WATCHDOG_GROBJ_ID + 4)

// Push buffer size in dwords
#define WATCHDOG_PUSHBUF_SIZE 128

// Default watchdog pushbuffer size (if no PERF engine)
#define WATCHDOG_PB_SIZE_DEFAULT 0xC000

#define WATCHDOG_PUSHBUFFERS        2
#define WATCHDOG_GPFIFO_ENTRIES     4
#define WATCHDOG_GRAPHICS_NOTIFIERS 3

#define GPFIFO_ALIGN   NV906F_GP_ENTRY__SIZE
#define NOTIFIER_ALIGN 16

#define WATCHDOG_GPFIFO_OFFSET(pbBytes)                        \
    ((((pbBytes)*WATCHDOG_PUSHBUFFERS) + (GPFIFO_ALIGN - 1)) & \
     ~(GPFIFO_ALIGN - 1))

#define WATCHDOG_BEGINNING_NOTIFIER_OFFSET(pbBytes)         \
    (((WATCHDOG_GPFIFO_OFFSET(pbBytes) +                    \
       (WATCHDOG_GPFIFO_ENTRIES * NV906F_GP_ENTRY__SIZE)) + \
      (NOTIFIER_ALIGN - 1)) &                               \
     ~(NOTIFIER_ALIGN - 1))

#define WATCHDOG_ERROR_NOTIFIER_OFFSET(pbBytes) \
    (WATCHDOG_BEGINNING_NOTIFIER_OFFSET(pbBytes))

#define WATCHDOG_NOTIFIER_OFFSET(pbBytes, gpuIndex, notifier)                \
    (WATCHDOG_BEGINNING_NOTIFIER_OFFSET(pbBytes) +                           \
     (sizeof(NvNotification) * NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1) + \
     ((gpuIndex) * sizeof(NvNotification) * WATCHDOG_GRAPHICS_NOTIFIERS) +   \
     (sizeof(NvNotification) * (notifier)))

#define WATCHDOG_WORK_SUBMIT_TOKEN_OFFSET(pbBytes)          \
    ((WATCHDOG_BEGINNING_NOTIFIER_OFFSET(pbBytes)) +        \
     NV_CHANNELGPFIFO_NOTIFICATION_TYPE_WORK_SUBMIT_TOKEN * \
         sizeof(NvNotification))

#define WATCHDOG_PUSHBUFFER_OFFSET(pbBytes, pbnum) ((pbBytes) * (pbnum))

#define SUBDEVICE_MASK_ALL DRF_MASK(NV906F_DMA_SET_SUBDEVICE_MASK_VALUE)

NV_STATUS
krcWatchdogChangeState_IMPL
(
    KernelRc  *pKernelRc,
    Subdevice *pSubdevice,
    RC_CHANGE_WATCHDOG_STATE_OPERATION_TYPE operation
)
{
    //
    // Provide automatic management of RC watchdog enabling and disabling.
    // Provide for cooperation between RM clients, and allow for independent
    // behavior or multiple client and multiple GPUs.
    //
    // RM clients can use the NV2080_CTRL_CMD_RC_ENABLE_WATCHDOG and related API
    // calls to request enabling or disabling of the RM watchdog, per GPU.
    // Whether or not the watchdog is actually enabled or disabled, however,
    // depends upon whether or not other, conflicting requests are already in
    // force.
    //
    // Some background as to how this is normally used:
    //
    // -- Normally, some clients (such as X) wants the watchdog running.
    // -- Normally, CUDA wants the watchdog disabled.
    // -- When the RM initializes, it sets the watchog to disabled.
    // -- X will normally tell the RM, for each GPU that it manages, to enable
    //    the watchdog.
    // -- Each CUDA client normally will tell the RM, for each GPU that it
    //    manages, to disable the watchdog.
    // -- X will have options that provide for either *not* enabling the
    //    watchdog, or at least, not blocking another client from disabling the
    //    watchdog.
    // -- Likewise, CUDA will have an option that provides for either enabling
    //    the watchdog, or at least, not blocking another client from enabling
    //    the watchdog.
    //
    // The watchdog is not allowed to transition directly between ENABLED and
    // DISABLED states. It must go through a "don't care" state, in between:
    //
    // ENABLED <--> DON'T-CARE <--> DISABLED
    //
    // Each of the three states may be reached with an associated RM API call:
    //
    // NV2080_CTRL_CMD_RC_ENABLE_WATCHDOG:             ENABLED state
    // NV2080_CTRL_CMD_RC_DISABLE_WATCHDOG:            DISABLED state
    // NV2080_CTRL_CMD_RC_RELEASE_WATCHDOG_REQUESTS:   DON'T-CARE state
    //
    // In addition, RM client destruction leads directly to the DON'T-CARE
    // state. This allows good behavior and cooperation between possibly
    // conflicting RM clients.
    //
    // Basic operation:
    //
    // ENABLE requests: Increment enableRequestsRefCount, disallow disable
    // operations from any client,  but *allow* additional enable operations
    // from any client.
    //
    // DISABLE requests: Increment disableRequestsRefCount, disallow enable
    // operations from any client,  but *allow* additional disable operations
    // from any client.
    //
    // CLIENT DESTRUCTION requests: Decrement the enableRequestsRefCount if the
    // client had an existing ENABLE request when it was destroyed. Reduce the
    // disableRequestsRefCount if the client had an existing DISABLE request
    // when it was destroyed.
    //
    // RELEASE requests: Possibly reduce the refCount, just as if the client had
    // been destroyed. This is convenenient for client such as MODS, that tend
    // to make multiple calls to enable and disable the watchdog, within the
    // lifetime of a single RM client.
    //
    //
    NvBool bCurrentEnableRequest      = NV_FALSE;
    NvBool bCurrentDisableRequest     = NV_FALSE;
    NvBool bCurrentSoftDisableRequest = NV_FALSE;
    NvS32  prevEnableRefCount      = pKernelRc->watchdogPersistent.enableRequestsRefCount;
    NvS32  prevDisableRefCount     = pKernelRc->watchdogPersistent.disableRequestsRefCount;
    NvS32  prevSoftDisableRefCount = pKernelRc->watchdogPersistent.softDisableRequestsRefCount;
    NvBool bPrevEnableRequest      = pSubdevice->bRcWatchdogEnableRequested;
    NvBool bPrevDisableRequest     = pSubdevice->bRcWatchdogDisableRequested;
    NvBool bPrevSoftDisableRequest = pSubdevice->bRcWatchdogSoftDisableRequested;
    OBJGPU *pGpu = ENG_GET_GPU(pKernelRc);
    const char *opstring;

    switch (operation)
    {
        case RMAPI_ENABLE_REQUEST:
            bCurrentEnableRequest      = NV_TRUE;
            bCurrentDisableRequest     = NV_FALSE;
            bCurrentSoftDisableRequest = NV_FALSE;
            opstring = "enable watchdog";
            break;

        case RMAPI_SOFT_DISABLE_REQUEST:
            bCurrentEnableRequest      = NV_FALSE;
            bCurrentDisableRequest     = NV_FALSE;
            bCurrentSoftDisableRequest = NV_TRUE;
            opstring = "soft disable watchdog";
            break;

        case RMAPI_DISABLE_REQUEST:
            bCurrentEnableRequest      = NV_FALSE;
            bCurrentDisableRequest     = NV_TRUE;
            bCurrentSoftDisableRequest = NV_FALSE;
            opstring = "disable watchdog";
            break;

        case RMAPI_RELEASE_ALL_REQUESTS:
            bCurrentEnableRequest      = NV_FALSE;
            bCurrentDisableRequest     = NV_FALSE;
            bCurrentSoftDisableRequest = NV_FALSE;
            opstring = "release all requests";
            break;

        case RM_CLIENT_DESTRUCTION:
            bCurrentEnableRequest      = NV_FALSE;
            bCurrentDisableRequest     = NV_FALSE;
            bCurrentSoftDisableRequest = NV_FALSE;
            opstring = "destroy RM client";
            break;

        default:
            NV_ASSERT(0);
            return NV_ERR_INVALID_ARGUMENT;
            break;
    }
    // -Wunused-but-set-variable nonsense if NV_PRINTF is compiled out
    (void)opstring;


    //
    // Step 1: check for conflicting requests, and bail out without changing
    // client state or watchdog state, if there are any such conflicts. We don't
    // consider the soft disable requests for conflicts, since they won't be
    // applied anyway, but we do still want them to be counted for when the
    // conflicting request is released - we'll fall back to the soft-disabled
    // state then.
    //
    if ((pKernelRc->watchdogPersistent.disableRequestsRefCount != 0 &&
         bCurrentEnableRequest) ||
        (pKernelRc->watchdogPersistent.enableRequestsRefCount != 0 &&
         bCurrentDisableRequest))
    {
        NV_PRINTF(LEVEL_ERROR,
            "Cannot %s on GPU 0x%x, due to another client's request\n"
            "(Enable requests: %d, Disable requests: %d)\n",
            opstring,
            pGpu->gpuId,
            pKernelRc->watchdogPersistent.enableRequestsRefCount,
            pKernelRc->watchdogPersistent.disableRequestsRefCount);

        return NV_ERR_STATE_IN_USE;
    }

    NV_PRINTF(LEVEL_INFO,
        "(before) op: %s, GPU 0x%x, enableRefCt: %d, disableRefCt: %d, softDisableRefCt: %d, WDflags: 0x%x\n",
        opstring,
        pGpu->gpuId,
        pKernelRc->watchdogPersistent.enableRequestsRefCount,
        pKernelRc->watchdogPersistent.disableRequestsRefCount,
        pKernelRc->watchdogPersistent.softDisableRequestsRefCount,
        pKernelRc->watchdog.flags);

    // Step 2: if client state has changed, adjust the per-GPU/RC refcount:
    if (!bPrevEnableRequest && bCurrentEnableRequest)
    {
        ++pKernelRc->watchdogPersistent.enableRequestsRefCount;
    }
    else if (bPrevEnableRequest && !bCurrentEnableRequest)
    {
        --pKernelRc->watchdogPersistent.enableRequestsRefCount;
    }

    if (!bPrevDisableRequest && bCurrentDisableRequest)
    {
        ++pKernelRc->watchdogPersistent.disableRequestsRefCount;
    }
    else if (bPrevDisableRequest && !bCurrentDisableRequest)
    {
        --pKernelRc->watchdogPersistent.disableRequestsRefCount;
    }

    if (!bPrevSoftDisableRequest && bCurrentSoftDisableRequest)
    {
        ++pKernelRc->watchdogPersistent.softDisableRequestsRefCount;
    }
    else if (bPrevSoftDisableRequest && !bCurrentSoftDisableRequest)
    {
        --pKernelRc->watchdogPersistent.softDisableRequestsRefCount;
    }

    // Step 3: record client state:
    pSubdevice->bRcWatchdogEnableRequested      = bCurrentEnableRequest;
    pSubdevice->bRcWatchdogDisableRequested     = bCurrentDisableRequest;
    pSubdevice->bRcWatchdogSoftDisableRequested = bCurrentSoftDisableRequest;

    //
    // Step 4: if per-GPU/RC refcount has changed from 0 to 1, then change the
    // watchdog state:
    //
    if (pKernelRc->watchdogPersistent.enableRequestsRefCount == 1 &&
        prevEnableRefCount == 0 &&
        pKernelRc->watchdogPersistent.disableRequestsRefCount == 0)
    {
        // Enable the watchdog:
        krcWatchdogEnable(pKernelRc, NV_FALSE /* bOverRide */);
    }
    else if (pKernelRc->watchdogPersistent.disableRequestsRefCount == 1 &&
             prevDisableRefCount == 0 &&
             pKernelRc->watchdogPersistent.enableRequestsRefCount == 0)
    {
        // Disable the watchdog:
        krcWatchdogDisable(pKernelRc);
    }
    else if ((pKernelRc->watchdogPersistent.enableRequestsRefCount == 0) &&
             (pKernelRc->watchdogPersistent.disableRequestsRefCount == 0) &&
             ((prevEnableRefCount > 0) || (prevSoftDisableRefCount == 0)) &&
             (pKernelRc->watchdogPersistent.softDisableRequestsRefCount > 0))
    {
        //
        // Go back to disabled if all of the below are true:
        //  (1) there are no outstanding enable or disable requests,
        //  (2) the change is the release of the last enable request OR
        //      there were previously no soft disable requests
        //  (3) there are now one or more outstanding soft disable requests
        //      (including the one currently being refcounted.
        //
        krcWatchdogDisable(pKernelRc);
    }

    NV_PRINTF(LEVEL_INFO,
        "(after) op: %s, GPU 0x%x, enableRefCt: %d, disableRefCt: %d, softDisableRefCt: %d, WDflags: 0x%x\n",
        opstring,
        pGpu->gpuId,
        pKernelRc->watchdogPersistent.enableRequestsRefCount,
        pKernelRc->watchdogPersistent.disableRequestsRefCount,
        pKernelRc->watchdogPersistent.softDisableRequestsRefCount,
        pKernelRc->watchdog.flags);

    return NV_OK;
}


void
krcWatchdogDisable_IMPL
(
    KernelRc *pKernelRc
)
{
    pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;
}


void
krcWatchdogEnable_IMPL
(
    KernelRc *pKernelRc,
    NvBool    bOverRide
)
{
    //
    // Make sure no operations are pending from before
    // if bOverRide is NV_TRUE then we are enabling from a modeswitch
    //
    if (bOverRide)
        pKernelRc->watchdog.deviceResetRd = pKernelRc->watchdog.deviceResetWr;

    pKernelRc->watchdog.flags &= ~WATCHDOG_FLAGS_DISABLED;
}


NV_STATUS
krcWatchdogShutdown_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (!(pKernelRc->watchdog.flags & WATCHDOG_FLAGS_INITIALIZED))
        return NV_OK;

    krcWatchdogDisable(pKernelRc);
    osRemove1HzCallback(pGpu, krcWatchdogTimerProc, NULL /* pData */);

    // This should free the client and all associated resources
    pRmApi->Free(pRmApi,
                 pKernelRc->watchdog.hClient,
                 pKernelRc->watchdog.hClient);

    //
    // Make sure to clear any old watchdog data this also clears
    // WATCHDOG_FLAGS_INITIALIZED, bHandleValid, and hClient
    //
    portMemSet(&pKernelRc->watchdog, 0, sizeof pKernelRc->watchdog);
    portMemSet(&pKernelRc->watchdogChannelInfo, 0,
               sizeof pKernelRc->watchdogChannelInfo);

    return NV_OK;
}


void krcWatchdogGetReservationCounts_IMPL
(
    KernelRc *pKernelRc,
    NvS32    *pEnable,
    NvS32    *pDisable,
    NvS32    *pSoftDisable
)
{
    if (pEnable != NULL)
        *pEnable = pKernelRc->watchdogPersistent.enableRequestsRefCount;

    if (pDisable != NULL)
        *pDisable = pKernelRc->watchdogPersistent.disableRequestsRefCount;

    if (pSoftDisable != NULL)
        *pSoftDisable = pKernelRc->watchdogPersistent .softDisableRequestsRefCount;
}


NV_STATUS
krcWatchdogInit_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    NvHandle        hClient;
    NvU32           subDeviceInstance;
    NvU32           grObj;
    NvU32           gpfifoObj;
    NvU32           pushBufBytes;
    NvU32           allocationSize;
    NvU32           ctrlSize;
    NV_STATUS       status;
    RsClient       *pClient;
    KernelChannel  *pKernelChannel;
    NvBool          bCacheSnoop;
    RM_API         *pRmApi = rmGpuLockIsOwner() ?
                             rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL) :
                             rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    NvBool          bClientUserd = IsVOLTAorBetter(pGpu);
    NvBool          bAcquireLock = NV_FALSE;

    union
    {
        NV0080_ALLOC_PARAMETERS                nv0080;
        NV2080_ALLOC_PARAMETERS                nv2080;
        NV_CHANNEL_ALLOC_PARAMS channelGPFifo;
        NV_CONTEXT_DMA_ALLOCATION_PARAMS       ctxDma;
        NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS    virtual;
        NV_MEMORY_ALLOCATION_PARAMS            mem;
    } *pParams = NULL;

    // If booting in SMC mode, skip watchdog init since TWOD is not supported
    NV_CHECK_OR_RETURN(LEVEL_SILENT,
                       !IS_MIG_ENABLED(pGpu) &&
                           gpuIsClassSupported(pGpu, FERMI_TWOD_A),
                       NV_OK);

    if (pKernelRc->watchdog.flags &
        (WATCHDOG_FLAGS_DISABLED | WATCHDOG_FLAGS_INITIALIZED))
    {
        return NV_OK;
    }

    if (bClientUserd)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        if (memmgrIsPmaInitialized(pMemoryManager))
        {
            Heap *pHeap = GPU_GET_HEAP(pGpu);
            NvU32 pmaConfig = PMA_QUERY_NUMA_ENABLED | PMA_QUERY_NUMA_ONLINED;

            if (pmaQueryConfigs(&pHeap->pmaObject, &pmaConfig) == NV_OK)
            {
                // PMA can't be used until it's onlined
                if (pmaConfig & PMA_QUERY_NUMA_ENABLED)
                {
                    // PMA can't be used until it's onlined
                    if (!(pmaConfig & PMA_QUERY_NUMA_ONLINED))
                        bClientUserd = NV_FALSE;
                }
            }
        }
    }

    portMemSet(&pKernelRc->watchdogChannelInfo, 0,
               sizeof pKernelRc->watchdogChannelInfo);

    // Bug 4088184 WAR: release GPU lock before allocating NV01_ROOT
    if (rmGpuLockIsOwner())
    {
        bAcquireLock = NV_TRUE;
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }

    // Allocate a root.
    {
        hClient = NV01_NULL_OBJECT;
        if (pRmApi->AllocWithHandle(pRmApi,
                                    NV01_NULL_OBJECT /* hClient */,
                                    NV01_NULL_OBJECT /* hParent */,
                                    NV01_NULL_OBJECT /* hObject */,
                                    NV01_ROOT,
                                    &hClient,
                                    sizeof(hClient)) != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING, "Unable to allocate a watchdog client\n");
            return NV_ERR_GENERIC;
        }

        pParams = portMemAllocNonPaged(sizeof *pParams);
        if (pParams == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }
        pKernelRc->watchdog.hClient = hClient;
        pKernelRc->watchdog.bHandleValid = NV_TRUE;
    }

    if (bAcquireLock)
    {
        status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_RC);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to grab RM-Lock\n");
            DBG_BREAKPOINT();
            goto error;
        }
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        bAcquireLock = NV_FALSE;
    }

    // Due to the Bug 4088184 WAR above, watchdog has to be explicitly set to internal
    rmclientSetClientFlagsByHandle(hClient, RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT);

    // Alloc device
    {
        NV0080_ALLOC_PARAMETERS *pNv0080 = &pParams->nv0080;

        portMemSet(pNv0080, 0, sizeof *pNv0080);
        pNv0080->deviceId = gpuGetDeviceInstance(pGpu);
        pNv0080->hClientShare = hClient;

        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient            /* hClient */,
                                         hClient            /* hParent */,
                                         WATCHDOG_DEVICE_ID /* hObject */,
                                         NV01_DEVICE_0,
                                         pNv0080,
                                         sizeof(*pNv0080));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING, "Unable to allocate a watchdog device\n");
            goto error;
        }
    }

    // Alloc subdevices
    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)
    {
        NV2080_ALLOC_PARAMETERS *pNv2080 = &pParams->nv2080;

        subDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        portMemSet(pNv2080, 0, sizeof *pNv2080);
        pNv2080->subDeviceId = subDeviceInstance;

        status = pRmApi->AllocWithHandle(pRmApi,
            hClient                                        /* hClient */,
            WATCHDOG_DEVICE_ID                             /* hParent */,
            (WATCHDOG_SUB_DEVICE_0_ID + subDeviceInstance) /* hObject */,
            NV20_SUBDEVICE_0,
            pNv2080,
            sizeof(*pNv2080));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate a watchdog subdevice\n");
            SLI_LOOP_GOTO(error);
        }
    }
    SLI_LOOP_END

    //
    // Determine what class to allocate so we will know whether to use
    // context DMAs.  Context DMAs are not allowed on any gpu after Fermi
    //
    if (gpuIsClassSupported(pGpu, FERMI_TWOD_A))
    {
        grObj = FERMI_TWOD_A;
    }
    else
    {
        grObj = NV01_NULL_OBJECT; // Null object will kill RmAllocObject
    }

    {
        const struct
        {
            NvU32    gpfifoObject;
            NvLength ctrlSize;
        } gpfifoMapping[] = {
              {KEPLER_CHANNEL_GPFIFO_B,  sizeof(NvA16FControl)}
            , {KEPLER_CHANNEL_GPFIFO_A,  sizeof(NvA06FControl)}
            , {MAXWELL_CHANNEL_GPFIFO_A, sizeof(Nvb06FControl)}
            , {PASCAL_CHANNEL_GPFIFO_A,  sizeof(Nvc06fControl)}
            , {VOLTA_CHANNEL_GPFIFO_A,   sizeof(Nvc36fControl)}
            , {TURING_CHANNEL_GPFIFO_A,  sizeof(Nvc46fControl)}
            , {AMPERE_CHANNEL_GPFIFO_A,  sizeof(Nvc56fControl)}
            , {HOPPER_CHANNEL_GPFIFO_A,  sizeof(Nvc86fControl)}
            , {BLACKWELL_CHANNEL_GPFIFO_A,  sizeof(Nvc96fControl)}
        };

        NvU32 i;

        // Defaults if none match
        gpfifoObj = GF100_CHANNEL_GPFIFO;
        ctrlSize  = sizeof(Nv906fControl);
        pKernelRc->watchdogChannelInfo.class2dSubch = 0;

        for (i = 0; i < NV_ARRAY_ELEMENTS(gpfifoMapping); ++i)
        {
            if (gpuIsClassSupported(pGpu, gpfifoMapping[i].gpfifoObject))
            {
                gpfifoObj = gpfifoMapping[i].gpfifoObject;
                ctrlSize  = gpfifoMapping[i].ctrlSize;
                pKernelRc->watchdogChannelInfo
                    .class2dSubch = NVA06F_SUBCHANNEL_2D;
                break;
            }
        }
    }

    // RMCONFIG: only if PERF engine is enabled
    if (RMCFG_MODULE_KERNEL_PERF)
    {
        pushBufBytes = WATCHDOG_PUSHBUF_SIZE * 4;
    }
    else
    {
        pushBufBytes = WATCHDOG_PB_SIZE_DEFAULT;
    }

    // Allocate a virtual context handle
    {
        NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS *pVirtual = &pParams->virtual;

        portMemSet(pVirtual, 0, sizeof *pVirtual);
        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient                 /* hClient */,
                                         WATCHDOG_DEVICE_ID      /* hParent */,
                                         WATCHDOG_VIRTUAL_CTX_ID /* hObject */,
                                         NV01_MEMORY_VIRTUAL,
                                         pVirtual,
                                         sizeof(*pVirtual));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate unified heap for watchdog\n");
            goto error;
        }
    }

    //
    // Calculate the system memory allocation size based on size of push
    // buffers, notifers, GPFIFOs, etc., taking alignment requirements into
    // consideration.
    //
    pKernelRc->watchdogChannelInfo.pbBytes = pushBufBytes;
    pushBufBytes *= WATCHDOG_PUSHBUFFERS;
    allocationSize = (NvU32)(
        pushBufBytes +
        ((WATCHDOG_GPFIFO_ENTRIES * NV906F_GP_ENTRY__SIZE) + GPFIFO_ALIGN) +
        ((sizeof(NvNotification) + NOTIFIER_ALIGN) *
         NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1) +
        (sizeof(NvNotification) * WATCHDOG_GRAPHICS_NOTIFIERS *
         NV_MAX_SUBDEVICES));

    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    bCacheSnoop = FLD_TEST_REF(BIF_DMA_CAPS_SNOOP, _CTXDMA,
                               kbifGetDmaCaps(pGpu, pKernelBif));

    {
        NV_MEMORY_ALLOCATION_PARAMS *pMem = &pParams->mem;
        NvU32 hClass = NV01_MEMORY_SYSTEM;

        portMemSet(pMem, 0, sizeof *pMem);
        pMem->owner = HEAP_OWNER_RM_CLIENT_GENERIC;
        pMem->size  = allocationSize;
        pMem->type  = NVOS32_TYPE_IMAGE;

        pMem->attr2 = DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _NO);

        // Apply registry overrides to channel pushbuffer.
        switch (DRF_VAL(_REG_STR_RM, _INST_LOC_4, _CHANNEL_PUSHBUFFER, pGpu->instLocOverrides4))
        {
            case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_VID:
                hClass = NV01_MEMORY_LOCAL_USER;
                pMem->attr |= DRF_DEF(OS32, _ATTR, _LOCATION,  _VIDMEM) |
                              DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);
                break;

            case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_COH:
                hClass = NV01_MEMORY_SYSTEM;
                pMem->attr |= DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)    |
                              DRF_DEF(OS32, _ATTR, _COHERENCY, _CACHED) |
                              DRF_DEF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS);
                break;

            case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_NCOH:
                hClass = NV01_MEMORY_SYSTEM;
                pMem->attr |= DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)      |
                              DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED) |
                              DRF_DEF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS);
                break;

            case NV_REG_STR_RM_INST_LOC_4_CHANNEL_PUSHBUFFER_DEFAULT:
            default:
                hClass = NV01_MEMORY_SYSTEM;
                pMem->attr |= DRF_DEF(OS32, _ATTR, _LOCATION,  _PCI)      |
                              DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED) |
                              DRF_DEF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS);
        }

        if (bCacheSnoop && (hClass == NV01_MEMORY_SYSTEM))
        {
            pMem->attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _CACHED,
                                     pMem->attr);
        }

        if (((pKernelRc->watchdog.flags & WATCHDOG_FLAGS_ALLOC_UNCACHED_PCI) != 0) &&
            (hClass == NV01_MEMORY_SYSTEM))
        {
            pMem->attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _UNCACHED,
                                     pMem->attr);
        }

        //
        // When Hopper CC is enabled all RM internal sysmem allocations that are
        // required to be accessed from GPU should be in unprotected memory
        // All video allocations must be in CPR
        //

        //
        // Allocate memory using vidHeapControl
        //
        // vidHeapControl calls should happen outside GPU locks. This is a PMA
        // requirement as memory allocation calls may invoke eviction which UVM
        // could get stuck behind GPU lock
        //
        if (hClass == NV01_MEMORY_LOCAL_USER && rmGpuLockIsOwner())
        {
            bAcquireLock = NV_TRUE;
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        }

        // Allocate memory for the notifiers and pushbuffer
        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient            /* hClient */,
                                         WATCHDOG_DEVICE_ID /* hParent */,
                                         WATCHDOG_MEM_ID    /* hObject */,
                                         hClass,
                                         pMem,
                                         sizeof(*pMem));

        if (bAcquireLock)
        {
            // Re-acquire the GPU locks
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_RC) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to grab RM-Lock\n");
                DBG_BREAKPOINT();
                goto error;
            }
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
            bAcquireLock = NV_FALSE;
        }

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate %s memory for watchdog\n",
                      (hClass == NV01_MEMORY_LOCAL_USER) ? "video" : "system");
            goto error;
        }

        status = pRmApi->MapToCpu( pRmApi,
            hClient            /* hClient */,
            WATCHDOG_DEVICE_ID /* hDevice */,
            WATCHDOG_MEM_ID    /* hMemory */,
            0                  /* offset */,
            pMem->size         /* length */,
            (void **)&pKernelRc->watchdogChannelInfo.pCpuAddr,
            0                  /* flags */);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to map memory for watchdog\n");
            goto error;
        }

        portMemSet(pKernelRc->watchdogChannelInfo.pCpuAddr, 0, pMem->size);

        // Map the allocation into the unified heap.
        status = pRmApi->Map(pRmApi,
            hClient                 /* hClient */,
            WATCHDOG_DEVICE_ID      /* hDevice */,
            WATCHDOG_VIRTUAL_CTX_ID /* hMemctx */,
            WATCHDOG_MEM_ID         /* hMemory */,
            0                       /* offset */,
            allocationSize          /* length */,
            (bCacheSnoop ? DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE) :
                           DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _DISABLE)) |
                DRF_DEF(OS46, _FLAGS, _ACCESS, _READ_WRITE),
            &pKernelRc->watchdogChannelInfo.pGpuAddr);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to map memory into watchdog's heap\n");
            goto error;
        }
    }

    // Allocate the error notifier context DMA.
    {
        NV_CONTEXT_DMA_ALLOCATION_PARAMS *pCtxDma = &pParams->ctxDma;

        portMemSet(pCtxDma, 0, sizeof *pCtxDma);
        pCtxDma->hSubDevice = 0;
        pCtxDma->flags = (bCacheSnoop ?
                              DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _ENABLE) :
                              DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _DISABLE)) |
                         DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                         DRF_DEF(OS03, _FLAGS, _MAPPING, _KERNEL) |
                         DRF_DEF(OS03, _FLAGS, _HASH_TABLE, _DISABLE);
        pCtxDma->hMemory = WATCHDOG_MEM_ID;
        pCtxDma->offset  = WATCHDOG_ERROR_NOTIFIER_OFFSET(
            pKernelRc->watchdogChannelInfo.pbBytes);
        pCtxDma->limit = ((NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1 *
                           sizeof(NvNotification)) -
                          1);

        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient               /* hClient */ ,
                                         WATCHDOG_DEVICE_ID    /* hParent */ ,
                                         WATCHDOG_ERROR_DMA_ID /* hObject */,
                                         NV01_CONTEXT_DMA,
                                         pCtxDma,
                                         sizeof(*pCtxDma));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to set up watchdog's error context\n");
            goto error;
        }
    }

    // Allocate the graphics notifier context DMA.
    {
        NV_CONTEXT_DMA_ALLOCATION_PARAMS *pCtxDma = &pParams->ctxDma;

        portMemSet(pCtxDma, 0, sizeof *pCtxDma);
        pCtxDma->hSubDevice = 0;
        pCtxDma->flags = (bCacheSnoop ?
                              DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _ENABLE) :
                              DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _DISABLE)) |
                         DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
                         DRF_DEF(OS03, _FLAGS, _HASH_TABLE, _DISABLE);
        pCtxDma->hMemory = WATCHDOG_MEM_ID;
        pCtxDma->offset  = WATCHDOG_NOTIFIER_OFFSET(
            pKernelRc->watchdogChannelInfo.pbBytes,
            0 /* gpuIndex */,
            0 /* notifier */);
        pCtxDma->limit = ((sizeof(NvNotification) *
                           WATCHDOG_GRAPHICS_NOTIFIERS * NV_MAX_SUBDEVICES) -
                          1);

        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient                  /* hClient */,
                                         WATCHDOG_DEVICE_ID       /* hParent */,
                                         WATCHDOG_NOTIFIER_DMA_ID /* hObject */,
                                         NV01_CONTEXT_DMA,
                                         pCtxDma,
                                         sizeof(*pCtxDma));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING, "Unable to set up watchdog's notifier\n");
            goto error;
        }
    }

    if (bClientUserd)
    {
        NV_MEMORY_ALLOCATION_PARAMS *pMem          = &pParams->mem;
        NvU32                        userdMemClass = NV01_MEMORY_LOCAL_USER;

        portMemSet(pMem, 0, sizeof *pMem);
        pMem->owner = HEAP_OWNER_RM_CLIENT_GENERIC;
        pMem->size  = ctrlSize;
        pMem->type  = NVOS32_TYPE_IMAGE;

        // Apply registry overrides to USERD.
        switch (DRF_VAL(_REG_STR_RM, _INST_LOC, _USERD, pGpu->instLocOverrides))
        {
            case NV_REG_STR_RM_INST_LOC_USERD_COH:
            case NV_REG_STR_RM_INST_LOC_USERD_NCOH:
                userdMemClass = NV01_MEMORY_SYSTEM;
                pMem->attr    = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);
                break;

            case NV_REG_STR_RM_INST_LOC_USERD_VID:
            case NV_REG_STR_RM_INST_LOC_USERD_DEFAULT:
                pMem->attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
                break;
        }

        //
        // When APM is enabled all RM internal allocations must to go to
        // unprotected memory irrespective of vidmem or sysmem
        // When Hopper CC is enabled all RM internal sysmem allocations that
        // are required to be accessed from GPU should be in unprotected memory
        // and all vidmem allocations must go to protected memory
        //

        //
        // Allocate memory using vidHeapControl
        //
        // vidHeapControl calls should happen outside GPU locks. This is a PMA
        // requirement as memory allocation calls may invoke eviction which UVM
        // could get stuck behind GPU lock
        //
        if (userdMemClass == NV01_MEMORY_LOCAL_USER && rmGpuLockIsOwner())
        {
            bAcquireLock = NV_TRUE;
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        }

        //
        // Using device handle since VGPU doesnt support subdevice memory
        // allocations
        //
        pMem->attr |= DRF_DEF(OS32, _ATTR, _ALLOCATE_FROM_RESERVED_HEAP, _YES);
        status = pRmApi->AllocWithHandle(pRmApi,
            hClient                    /* hClient */,
            WATCHDOG_DEVICE_ID         /* hParent */,
            WATCHDOG_USERD_PHYS_MEM_ID /* hObject */,
            userdMemClass,
            pMem,
            sizeof(*pMem));

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to allocate video memory for USERD\n");
            goto error;
        }
    }

    {
        NV_CHANNEL_ALLOC_PARAMS *pChannelGPFifo =
            &pParams->channelGPFifo;

        //
        // RmAllocChannel recognizes our handle and attempts to give us
        // channel 30. This is not guaranteed; we could theoretically get any
        // channel.
        //
        portMemSet(pChannelGPFifo, 0, sizeof *pChannelGPFifo);
        pChannelGPFifo->hObjectError  = WATCHDOG_ERROR_DMA_ID;
        pChannelGPFifo->hObjectBuffer = WATCHDOG_VIRTUAL_CTX_ID;
        pChannelGPFifo->gpFifoOffset  = (
            pKernelRc->watchdogChannelInfo.pGpuAddr +
            WATCHDOG_GPFIFO_OFFSET(pKernelRc->watchdogChannelInfo.pbBytes));
        pChannelGPFifo->gpFifoEntries = WATCHDOG_GPFIFO_ENTRIES;

        // 2d object is only suppported on GR0
        pChannelGPFifo->engineType = RM_ENGINE_TYPE_GR0;

        if (bClientUserd)
            pChannelGPFifo->hUserdMemory[0] = WATCHDOG_USERD_PHYS_MEM_ID;

        // channel alloc API needs to be called without GPU lock
        if (!bAcquireLock && rmGpuLockIsOwner())
        {
            bAcquireLock = NV_TRUE;
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        }

        status = pRmApi->AllocWithHandle(pRmApi,
            hClient                        /* hClient */,
            WATCHDOG_DEVICE_ID             /* hParent */,
            WATCHDOG_PUSHBUFFER_CHANNEL_ID /* hObject */,
            gpfifoObj,
            pChannelGPFifo,
            sizeof(*pChannelGPFifo));

        if (bAcquireLock)
        {
            // Reaquire the GPU locks
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_RC) !=
                NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to grab RM-Lock\n");
                DBG_BREAKPOINT();
                status = NV_ERR_GENERIC;
                goto error;
            }
            pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        }

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING, "Unable to alloc watchdog channel\n");

            if (status == NV_ERR_INVALID_CLASS)
            {
                status = NV_ERR_NOT_SUPPORTED;
            }
            goto error;
        }
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)
    {
        Nv906fControl *pControlGPFifo = NULL;
        subDeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

        // USERD isn't mapped for us on Fermi by RmAllocChannel.
        status = pRmApi->MapToCpu(pRmApi,
            hClient                                        /* hClient */,
            (WATCHDOG_SUB_DEVICE_0_ID + subDeviceInstance) /* hDevice */,
            bClientUserd ? WATCHDOG_USERD_PHYS_MEM_ID :
                           WATCHDOG_PUSHBUFFER_CHANNEL_ID  /* hMemory */,
            0                                              /* offset */,
            ctrlSize                                       /* length */,
            (void **)&pControlGPFifo,
            0                                              /* flags */);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to create a watchdog GPFIFO mapping\n");
            SLI_LOOP_GOTO(error);
        }

        pKernelRc->watchdogChannelInfo.pControlGPFifo[subDeviceInstance] =
            pControlGPFifo;

        pKernelRc->watchdog.notifiers[subDeviceInstance] =(NvNotification *)(
            pKernelRc->watchdogChannelInfo.pCpuAddr +
            WATCHDOG_NOTIFIER_OFFSET(pKernelRc->watchdogChannelInfo.pbBytes,
                                     subDeviceInstance /* gpuIndex */,
                                     0                 /* notifier */));
    }
    SLI_LOOP_END

    pKernelRc->watchdog.errorContext = (NvNotification *)(
        pKernelRc->watchdogChannelInfo.pCpuAddr +
        WATCHDOG_ERROR_NOTIFIER_OFFSET(pKernelRc->watchdogChannelInfo.pbBytes));

    pKernelRc->watchdog.notifierToken = (NvNotification *)(
        pKernelRc->watchdogChannelInfo.pCpuAddr +
        WATCHDOG_WORK_SUBMIT_TOKEN_OFFSET(
            pKernelRc->watchdogChannelInfo.pbBytes));

    // Create an object that will require a trip through the graphics engine
    status = pRmApi->AllocWithHandle(pRmApi,
        hClient                        /* hClient */,
        WATCHDOG_PUSHBUFFER_CHANNEL_ID /* hParent */,
        WATCHDOG_GROBJ_ID              /* hObject */,
        grObj,
        NULL,
        0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "Unable to allocate class %x\n", grObj);
        goto error;
    }

    // Fetch the client object
    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to obtain client object\n");
        goto error;
    }

    //
    // Determine the (class + engine) handle the hardware will understand, if
    // necessary
    //
    if (CliGetKernelChannelWithDevice(pClient,
                                      WATCHDOG_DEVICE_ID,
                                      WATCHDOG_PUSHBUFFER_CHANNEL_ID,
                                      &pKernelChannel) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CliGetKernelChannelWithDevice failed\n");
        status = NV_ERR_INVALID_CHANNEL;
        goto error;
    }

    NV_ASSERT_OR_ELSE(pKernelChannel != NULL, status = NV_ERR_INVALID_CHANNEL;
                      goto error);

    {
        NvU32 classID;
        RM_ENGINE_TYPE engineID;

        status = kchannelGetClassEngineID_HAL(pGpu, pKernelChannel,
            WATCHDOG_GROBJ_ID,
            &pKernelRc->watchdogChannelInfo.classEngineID,
            &classID,
            &engineID);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to get class engine ID %x\n",
                      grObj);
            goto error;
        }
    }

    pKernelRc->watchdog.hClient   = hClient;
    pKernelRc->watchdog.runlistId = kchannelGetRunlistId(pKernelChannel);

    // Schedule the watchdog channel for execution.
    {
        NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS nvA06fScheduleParams;

        portMemSet(&nvA06fScheduleParams, 0, sizeof nvA06fScheduleParams);
        nvA06fScheduleParams.bEnable = NV_TRUE;

        status = pRmApi->Control(pRmApi,
                                 pKernelRc->watchdog.hClient,
                                 WATCHDOG_PUSHBUFFER_CHANNEL_ID,
                                 NVA06F_CTRL_CMD_GPFIFO_SCHEDULE,
                                 &nvA06fScheduleParams,
                                 sizeof nvA06fScheduleParams);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to schedule watchdog channel\n");
            goto error;
        }
    }

    // Get the work submit token that watchdog can use while submitting work
    {
        NvU32 workSubmitToken;
        status = kfifoRmctrlGetWorkSubmitToken_HAL(GPU_GET_KERNEL_FIFO(pGpu),
            hClient,
            WATCHDOG_PUSHBUFFER_CHANNEL_ID,
            &workSubmitToken);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "Unable to get work submit token for watchdog\n");
            goto error;
        }
    }

    krcWatchdogInitPushbuffer_HAL(pGpu, pKernelRc);

    pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_INITIALIZED;

    // Hook into the 1 Hz OS timer
    osSchedule1HzCallback(pGpu,
                          krcWatchdogTimerProc,
                          NULL /* pData */,
                          NV_OS_1HZ_REPEAT);

    // Schedule next interval to run immediately
    pKernelRc->watchdogPersistent.nextRunTime = 0;

error:
    NV_ASSERT(status == NV_OK);

    if (status != NV_OK)
    {
        pRmApi->Free(pRmApi, hClient, hClient);
        pKernelRc->watchdog.bHandleValid = NV_FALSE;
    }

    portMemFree(pParams);
    return status;
}


void
krcWatchdogInitPushbuffer_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    NvU32 *ptr, *ptrbase, *ptrbase1;
    NvU32  pbOffset;

    //
    // Set up the pushbuffer.
    // Create two seperate pushbuffer segments
    // First -  Set object on graphics class
    // Second - Notifier, setref
    // Create GPFIFO
    // Point to setobject pushbuffer, gp_put++
    // Then keep on pointing gp_entry to second pushbuffer segment everytime we
    // need a notifier
    //
    pbOffset = WATCHDOG_PUSHBUFFER_OFFSET(
        pKernelRc->watchdogChannelInfo.pbBytes,
        0);
    ptrbase = ptr = (NvU32 *)(pKernelRc->watchdogChannelInfo.pCpuAddr +
                              pbOffset);

    if (IsSLIEnabled(pGpu))
    {
        PUSH_DATA(
            DRF_DEF(906F, _DMA, _SEC_OP, _GRP0_USE_TERT) |
            DRF_DEF(906F, _DMA, _TERT_OP, _GRP0_SET_SUB_DEV_MASK) |
            DRF_NUM(906F, _DMA, _SET_SUBDEVICE_MASK_VALUE, SUBDEVICE_MASK_ALL));
    }

    // Set up object in first pushbuffer
    PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
              NV902D_SET_OBJECT,
              pKernelRc->watchdogChannelInfo.classEngineID);

    //
    // Construct GPFIFO entries
    // Pushbuffer 0
    //
    {
        NvU64 get    = pKernelRc->watchdogChannelInfo.pGpuAddr + pbOffset;
        NvU32 length = (NvU8 *)ptr - (NvU8 *)ptrbase;

        pKernelRc->watchdogChannelInfo.gpEntry0[0] =
            DRF_DEF(906F, _GP_ENTRY0, _NO_CONTEXT_SWITCH, _FALSE) |
            DRF_NUM(906F, _GP_ENTRY0, _GET,               NvU64_LO32(get) >> 2);

        pKernelRc->watchdogChannelInfo.gpEntry0[1] =
            DRF_NUM(906F, _GP_ENTRY1, _GET_HI, NvU64_HI32(get)) |
            DRF_NUM(906F, _GP_ENTRY1, _LENGTH, length >> 2)     |
            DRF_DEF(906F, _GP_ENTRY1, _PRIV,   _USER)           |
            DRF_DEF(906F, _GP_ENTRY1, _LEVEL,  _MAIN);
    }

    // Set up notifiers in second pushbuffer
    pbOffset = WATCHDOG_PUSHBUFFER_OFFSET(
        pKernelRc->watchdogChannelInfo.pbBytes,
        1);
    ptrbase1 = ptr = (NvU32 *)(pKernelRc->watchdogChannelInfo.pCpuAddr +
                               pbOffset);

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
    {
        NvU64  offset;
        if (IsSLIEnabled(pGpu))
        {
            PUSH_DATA(DRF_DEF(906F, _DMA, _SEC_OP, _GRP0_USE_TERT) |
                      DRF_DEF(906F, _DMA, _TERT_OP, _GRP0_SET_SUB_DEV_MASK) |
                      DRF_NUM(906F, _DMA, _SET_SUBDEVICE_MASK_VALUE,
                              NVBIT(gpumgrGetSubDeviceInstanceFromGpu(pGpu))));
        }

        offset = (pKernelRc->watchdogChannelInfo.pGpuAddr +
                  WATCHDOG_NOTIFIER_OFFSET(
                      pKernelRc->watchdogChannelInfo.pbBytes,
                      gpumgrGetSubDeviceInstanceFromGpu(pGpu),
                      0));

        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
            NV902D_SET_NOTIFY_A,
            DRF_NUM(902D, _SET_NOTIFY_A, _ADDRESS_UPPER, NvU64_HI32(offset)));
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
            NV902D_SET_NOTIFY_B,
            DRF_NUM(902D, _SET_NOTIFY_B, _ADDRESS_LOWER, NvU64_LO32(offset)));
    }
    SLI_LOOP_END;

    if (IsSLIEnabled(pGpu))
    {
        PUSH_DATA(
            DRF_DEF(906F, _DMA, _SEC_OP, _GRP0_USE_TERT) |
            DRF_DEF(906F, _DMA, _TERT_OP, _GRP0_SET_SUB_DEV_MASK) |
            DRF_NUM(906F, _DMA, _SET_SUBDEVICE_MASK_VALUE, SUBDEVICE_MASK_ALL));
    }

    // Notifiers
    PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
              NV902D_NOTIFY, NV902D_NOTIFY_TYPE_WRITE_ONLY);
    PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
              NV902D_NO_OPERATION, 0x0);

    if (!gpuIsClassSupported(pGpu, HOPPER_CHANNEL_GPFIFO_A))
    {
        //
        // TODO Bug 4588210
        // Ticking bomb when Hopper eventually gets deprecated and
        // HOPPER_CHANNEL_GPFIFO_A is no longer supported.
        //
        // When this assert starts failing, delete this branch of the if
        // condition above.
        //
        NV_ASSERT(!IsHOPPERorBetter(pGpu));

        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
                  NV906F_SET_REFERENCE, 0x0);
    }
    else
    {
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch, NVC86F_WFI, 0);
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
                  NVC86F_MEM_OP_A,
                  0);
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
                  NVC86F_MEM_OP_B,
                  0);
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
                  NVC86F_MEM_OP_C,
                  0);
        PUSH_PAIR(pKernelRc->watchdogChannelInfo.class2dSubch,
                  NVC86F_MEM_OP_D,
                  DRF_DEF(C86F, _MEM_OP_D, _OPERATION, _MEMBAR));
    }

    // Pushbuffer 1
    {
        NvU64 get    = pKernelRc->watchdogChannelInfo.pGpuAddr + pbOffset;
        NvU32 length = (NvU8 *)ptr - (NvU8 *)ptrbase1;

        pKernelRc->watchdogChannelInfo.gpEntry1[0] =
            DRF_DEF(906F, _GP_ENTRY0, _NO_CONTEXT_SWITCH, _FALSE) |
            DRF_NUM(906F, _GP_ENTRY0, _GET,               NvU64_LO32(get) >> 2);

        pKernelRc->watchdogChannelInfo.gpEntry1[1] =
            DRF_NUM(906F, _GP_ENTRY1, _GET_HI, NvU64_HI32(get)) |
            DRF_NUM(906F, _GP_ENTRY1, _LENGTH, length >> 2)     |
            DRF_DEF(906F, _GP_ENTRY1, _PRIV,   _USER)           |
            DRF_DEF(906F, _GP_ENTRY1, _LEVEL,  _MAIN)           |
            DRF_DEF(906F, _GP_ENTRY1, _SYNC,   _WAIT);
    }

    // Write a new entry to the GPFIFO  (pushbuffer 0)
    {
        NvU32 *pGpEntry = (NvU32 *)(
            pKernelRc->watchdogChannelInfo.pCpuAddr +
            WATCHDOG_GPFIFO_OFFSET(pKernelRc->watchdogChannelInfo.pbBytes));
        MEM_WR32(&pGpEntry[0], pKernelRc->watchdogChannelInfo.gpEntry0[0]);
        MEM_WR32(&pGpEntry[1], pKernelRc->watchdogChannelInfo.gpEntry0[1]);
    }

    // Flush the WRC buffer using fence operation before updating gp_put
    osFlushCpuWriteCombineBuffer();

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
    {
        NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        MEM_WR32(
            &pKernelRc->watchdogChannelInfo.pControlGPFifo[subdeviceId]->GPPut,
            1);
        pKernelRc->watchdog.notifiers[subdeviceId]->status = 0;
    }
    SLI_LOOP_END;

    //
    // Flush the WRC buffer using fence operation before updating the usermode
    // channel ID register
    //
    osFlushCpuWriteCombineBuffer();

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
    {
        //
        // On some architectures, if doorbell is mapped via bar0, we need to send
        // an extra flush
        //
        if (kbusFlushPcieForBar0Doorbell_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu)) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Busflush failed.\n");
            return;
        }
        kfifoUpdateUsermodeDoorbell_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            pKernelRc->watchdog.notifierToken->info32);
    }
    SLI_LOOP_END;

    krcWatchdogWriteNotifierToGpfifo(pGpu, pKernelRc);
}


void
krcWatchdogWriteNotifierToGpfifo_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    NvU32  GPPut;

    // Write a second entry to the GPFIFO  (notifier)
    {
        NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        NvU32 *pGpEntry;

        GPPut = MEM_RD32(
            &pKernelRc->watchdogChannelInfo.pControlGPFifo[subdeviceId]->GPPut);

        if (GPPut >= WATCHDOG_GPFIFO_ENTRIES)
        {
            NV_ASSERT(GPPut < WATCHDOG_GPFIFO_ENTRIES);
            return;
        }

        pGpEntry = (NvU32 *)(
            pKernelRc->watchdogChannelInfo.pCpuAddr +
            WATCHDOG_GPFIFO_OFFSET(pKernelRc->watchdogChannelInfo.pbBytes) +
            (GPPut * NV906F_GP_ENTRY__SIZE));
        MEM_WR32(&pGpEntry[0], pKernelRc->watchdogChannelInfo.gpEntry1[0]);
        MEM_WR32(&pGpEntry[1], pKernelRc->watchdogChannelInfo.gpEntry1[1]);
    }

    //
    // Flush the WRC buffer using fence operation before updating the usermode
    // channel ID register
    //
    osFlushCpuWriteCombineBuffer();

    //
    // Write out incremented GPPut (we need > 2 GP fifo entries as one entry
    // must always be empty, as there is no extra state bit to distinguish
    // between a full GPFIFO buffer and an empty GPFIFO buffer).
    //
    GPPut = (GPPut + 1) % WATCHDOG_GPFIFO_ENTRIES;

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
    {
        NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        MEM_WR32(
            &pKernelRc->watchdogChannelInfo.pControlGPFifo[subdeviceId]->GPPut,
            GPPut);
    }
    SLI_LOOP_END;

    //
    // Flush the WRC buffer using fence operation before updating the usermode
    // channel ID register
    //
    osFlushCpuWriteCombineBuffer();

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
    {
        kfifoUpdateUsermodeDoorbell_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
            pKernelRc->watchdog.notifierToken->info32);
    }
    SLI_LOOP_END;
}

NV_STATUS krcWatchdogGetClientHandle(KernelRc *pKernelRc, NvHandle *phClient)
{
    if (!pKernelRc->watchdog.bHandleValid)
        return NV_ERR_INVALID_STATE;

    *phClient = pKernelRc->watchdog.hClient;
    return NV_OK;
}
