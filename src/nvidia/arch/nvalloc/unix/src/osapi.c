/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



#include <nv_ref.h>
#include <nv.h>
#include <nv-priv.h>
#include <os/os.h>
#include <osapi.h>
#include <class/cl0000.h>
#include <rmosxfac.h> // Declares RmInitRm().
#include "gpu/gpu.h"
#include <osfuncs.h>
#include <platform/chipset/chipset.h>

#include <objtmr.h>
#include <gpu/subdevice/subdevice.h>
#include <mem_mgr/mem.h>
#include "kernel/gpu/mem_mgr/mem_mgr.h"

#include <gpu/mem_sys/kern_mem_sys.h>

#include <diagnostics/journal.h>
#include <nvrm_registry.h>

#include <nvUnixVersion.h>
#include <gpu_mgr/gpu_mgr.h>
#include <core/thread_state.h>
#include <platform/acpi_common.h>
#include <core/locks.h>

#include <mem_mgr/p2p.h>

#include "rmapi/exports.h"
#include "rmapi/rmapi_utils.h"
#include "rmapi/rs_utils.h"
#include "rmapi/resource_fwd_decls.h"
#include <nv-kernel-rmapi-ops.h>
#include <rmobjexportimport.h>
#include "nv-reg.h"
#include "core/hal_mgr.h"
#include "gpu/device/device.h"

#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_uuid.h"

#include "platform/chipset/pci_pbi.h"

#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "ctrl/ctrl0073/ctrl0073system.h"
#include "ctrl/ctrl0073/ctrl0073specific.h"
#include "ctrl/ctrl2080/ctrl2080bios.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl402c.h"

#include "g_nv_name_released.h"   // released chip entries from nvChipAutoFlags.h

#include <virtualization/hypervisor/hypervisor.h>

#include "gpu/bus/kern_bus.h"

//
// Helper function which can be called before doing any RM control
// This function:
//
// a. Performs threadStateInit().
// b. Acquires API lock.
// c. Checks if RMAPI client handle is valid (i.e. RM is initialized) and
//    returns early if RMAPI client handle is invalid.
// d. Increments the dynamic power refcount. If GPU is in RTD3 suspended
//    state, then it will wake-up the GPU.
// e. Returns the RMAPI interface handle.
//
// This function should be called only when caller doesn't have acquired API
// lock. Caller needs to use RmUnixRmApiEpilogue() after RM control, if
// RmUnixRmApiPrologue() is successful.
//
RM_API *RmUnixRmApiPrologue(nv_state_t *pNv, THREAD_STATE_NODE *pThreadNode, NvU32 module)
{
    threadStateInit(pThreadNode, THREAD_STATE_FLAGS_NONE);

    if ((rmApiLockAcquire(API_LOCK_FLAGS_NONE, module)) == NV_OK)
    {
        if ((pNv->rmapi.hClient != 0) &&
            (os_ref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE) == NV_OK))
        {
            return rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
        }

        rmApiLockRelease();
    }

    threadStateFree(pThreadNode, THREAD_STATE_FLAGS_NONE);

    return NULL;
}

//
// Helper function which can be called after doing RM control, if
// caller has used RmUnixRmApiPrologue() helper function. This function:
//
// a. Decrements the dynamic power refcount.
// b. Release API lock.
// c. Performs threadStateFree().
//
void RmUnixRmApiEpilogue(nv_state_t *pNv, THREAD_STATE_NODE *pThreadNode)
{
    os_unref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE);
    rmApiLockRelease();
    threadStateFree(pThreadNode, THREAD_STATE_FLAGS_NONE);
}

NvBool RmGpuHasIOSpaceEnabled(nv_state_t * nv)
{
    NvU16 val;
    NvBool has_io;
    os_pci_read_word(nv->handle, NV_CONFIG_PCI_NV_1, &val);
    has_io = FLD_TEST_DRF(_CONFIG, _PCI_NV_1, _IO_SPACE, _ENABLED, val);
    return has_io;
}

// This is a stub function for unix
void osHandleDeferredRecovery(
    OBJGPU *pGpu
)
{

}

// This is a stub function for unix
NvBool osIsSwPreInitOnly
(
    OS_GPU_INFO *pOsGpuInfo
)
{
    return NV_FALSE;
}

const NvU8 * RmGetGpuUuidRaw(
    nv_state_t *pNv
)
{
    NV_STATUS rmStatus;
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);
    NvU32 gidFlags;
    NvBool isApiLockTaken = NV_FALSE;

    if (pNv->nv_uuid_cache.valid)
        goto done;

    //
    // PBI is not present in simulation and the loop inside
    // pciPbiReadUuid takes up considerable amount of time in
    // simulation environment during RM load.
    //
    if (pGpu && IS_SIMULATION(pGpu))
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        rmStatus = pciPbiReadUuid(pNv->handle, pNv->nv_uuid_cache.uuid);
    }

    if (rmStatus == NV_OK)
    {
        rmStatus = gpumgrSetUuid(pNv->gpu_id, pNv->nv_uuid_cache.uuid);
        if (rmStatus != NV_OK)
        {
            return NULL;
        }

        pNv->nv_uuid_cache.valid = NV_TRUE;
        goto done;
    }
    else if (rmStatus == NV_ERR_NOT_SUPPORTED)
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: PBI is not supported for GPU " NV_PCI_DEV_FMT "\n",
                  NV_PCI_DEV_FMT_ARGS(pNv));
    }

    gidFlags = DRF_DEF(2080_GPU_CMD,_GPU_GET_GID_FLAGS,_TYPE,_SHA1)
             | DRF_DEF(2080_GPU_CMD,_GPU_GET_GID_FLAGS,_FORMAT,_BINARY);

    if (!rmApiLockIsOwner())
    {
        rmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU);
        if (rmStatus != NV_OK)
        {
            return NULL;
        }

        isApiLockTaken = NV_TRUE;
    }

    if (pGpu == NULL)
    {
        if (isApiLockTaken == NV_TRUE)
        {
            rmApiLockRelease();
        }

        return NULL;
    }

    rmStatus = gpuGetGidInfo(pGpu, NULL, NULL, gidFlags);
    if (isApiLockTaken == NV_TRUE)
    {
        rmApiLockRelease();
    }

    if (rmStatus != NV_OK)
        return NULL;

    if (!pGpu->gpuUuid.isInitialized)
        return NULL;

    // copy the uuid from the OBJGPU uuid cache
    os_mem_copy(pNv->nv_uuid_cache.uuid, pGpu->gpuUuid.uuid, GPU_UUID_LEN);
    pNv->nv_uuid_cache.valid = NV_TRUE;

done:
    return pNv->nv_uuid_cache.uuid;
}

static NV_STATUS RmGpuUuidRawToString(
    const NvU8     *pGidData,
    char           *pBuf,
    NvU32           bufLen
)
{
    NvU8     *pGidString;
    NvU32     GidStrlen;
    NV_STATUS rmStatus;
    NvU32     gidFlags;

    gidFlags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _ASCII) |
               DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1);

    rmStatus = transformGidToUserFriendlyString(pGidData, RM_SHA1_GID_SIZE,
                                                &pGidString, &GidStrlen,
                                                gidFlags);
    if (rmStatus != NV_OK)
        return rmStatus;

    if (bufLen >= GidStrlen)
        portMemCopy(pBuf, bufLen, pGidString, GidStrlen);
    else
        rmStatus = NV_ERR_BUFFER_TOO_SMALL;

    portMemFree((void *)pGidString);

    return rmStatus;
}

// This function should be called with the API and GPU locks already acquired.
NV_STATUS
RmLogGpuCrash(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;
    NvBool bGpuIsLost, bGpuIsConnected;

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Re-evaluate whether or not the GPU is accessible. This could be called
    // from a recovery context where the OS has re-enabled MMIO for the device.
    // This happens during EEH processing on IBM Power + Linux, and marking
    // the device as connected again will allow rcdbAddRmGpuDump() to collect
    // more GPU state.
    //
    bGpuIsLost = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST);
    bGpuIsConnected = pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED);
    if (!bGpuIsConnected || bGpuIsLost)
    {
        nv_state_t *nv = NV_GET_NV_STATE(pGpu);
        nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
        NvU32 pmcBoot0 = NV_PRIV_REG_RD32(nv->regs->map_u, NV_PMC_BOOT_0);
        if (pmcBoot0 == nvp->pmc_boot_0)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED, NV_TRUE);
            pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_LOST, NV_FALSE);
        }
    }

    //
    // Log the engine data to the Journal object, to be pulled out later. This
    // will return NV_WARN_MORE_PROCESSING_REQUIRED if the dump needed to be
    // deferred to a passive IRQL. We still log the crash dump as being created
    // in that case since it (should) be created shortly thereafter, and
    // there's currently not a good way to print the below notification
    // publicly from the core RM when the DPC completes.
    //
    status = rcdbAddRmGpuDump(pGpu);
    if (status != NV_OK && status != NV_WARN_MORE_PROCESSING_REQUIRED)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: failed to save GPU crash data\n", __FUNCTION__);
    }
    else
    {
        status = NV_OK;
        nv_printf(NV_DBG_ERRORS,
            "NVRM: A GPU crash dump has been created. If possible, please run\n"
            "NVRM: nvidia-bug-report.sh as root to collect this data before\n"
            "NVRM: the NVIDIA kernel module is unloaded.\n");
    }

    // Restore the disconnected properties, if they were reset
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED, bGpuIsConnected);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_LOST, bGpuIsLost);

    // Restore persistence mode to the way it was prior to the crash
    osModifyGpuSwStatePersistence(pGpu->pOsGpuInfo,
        pGpu->getProperty(pGpu, PDB_PROP_GPU_PERSISTENT_SW_STATE));

    return status;
}

static void free_os_event_under_lock(nv_event_t *event)
{
    event->active = NV_FALSE;

    // If refcount > 0, event will be freed by osDereferenceObjectCount
    // when the last associated RM event is freed.
    if (event->refcount == 0)
    {
        portMemFree(event);
    }
}

static void free_os_events(
    nv_file_private_t *nvfp,
    NvHandle client
)
{
    nv_state_t *nv = nv_get_ctl_state();
    nv_event_t **pprev;

    portSyncSpinlockAcquire(nv->event_spinlock);

    pprev = &nv->event_list;
    while (*pprev != NULL)
    {
        nv_event_t *cur = *pprev;
        //
        // XXX We must be called from either rm_client_free_os_events() or
        // RmFreeUnusedClients() for this to work.
        //
        if ((cur->hParent == client) || (cur->nvfp == nvfp))
        {
            *pprev = cur->next;
            free_os_event_under_lock(cur);
        }
        else
        {
            pprev = &cur->next;
        }
    }

    portSyncSpinlockRelease(nv->event_spinlock);
}

void rm_client_free_os_events(
    NvHandle client
)
{
    free_os_events(NULL, client);
}

void RmFreeUnusedClients(
    nv_state_t         *nv,
    nv_file_private_t  *nvfp
)
{
    NvU32 *pClientList;
    NvU32 numClients, i;
    NV_STATUS status;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    //
    // The 'nvfp' pointer uniquely identifies an open instance in kernel space
    // and the kernel interface layer guarantees that we are not called before
    // the associated nvfp descriptor is closed. We can thus safely free
    // abandoned clients with matching 'nvfp' pointers.
    //
    status = rmapiGetClientHandlesFromOSInfo(nvfp, &pClientList, &numClients);
    if (status != NV_OK)
    {
        numClients = 0;
    }

    for (i = 0; i < numClients; ++i)
    {
        NV_PRINTF(LEVEL_INFO, "freeing abandoned client 0x%x\n",
                  pClientList[i]);

    }

    if (numClients != 0)
    {
        pRmApi->FreeClientList(pRmApi, pClientList, numClients);

        portMemFree(pClientList);
    }

    // Clean up any remaining events using this nvfp.
    free_os_events(nvfp, 0);
}

static void RmUnbindLock(
    nv_state_t *nv
)
{
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

    if ((pGpu == NULL) || (gpuGetUserClientCount(pGpu) == 0))
    {
        nv->flags |= NV_FLAG_UNBIND_LOCK;
    }
}

static NV_STATUS allocate_os_event(
    NvHandle            hParent,
    nv_file_private_t  *nvfp,
    NvU32               fd
)
{
    nv_state_t *nv = nv_get_ctl_state();
    NvU32 status = NV_OK;
    nv_event_t *event;

    nv_event_t *new_event = portMemAllocNonPaged(sizeof(nv_event_t));
    if (new_event == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    portSyncSpinlockAcquire(nv->event_spinlock);
    for (event = nv->event_list; event; event = event->next)
    {
        // Only one event may be associated with a given fd.
        if (event->hParent == hParent && event->fd == fd)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            portSyncSpinlockRelease(nv->event_spinlock);
            goto done;
        }
    }

    new_event->next = nv->event_list;
    nv->event_list = new_event;
    portSyncSpinlockRelease(nv->event_spinlock);

done:
    if (status == NV_OK)
    {
        new_event->hParent  = hParent;
        new_event->nvfp     = nvfp;
        new_event->fd       = fd;
        new_event->active   = NV_TRUE;
        new_event->refcount = 0;

        NV_PRINTF(LEVEL_INFO, "allocated OS event:\n");
        NV_PRINTF(LEVEL_INFO, "   hParent: 0x%x\n", hParent);
        NV_PRINTF(LEVEL_INFO, "   fd: %d\n", fd);
    }
    else
    {
        portMemFree(new_event);
    }

    return status;
}

NV_STATUS RmAllocOsEvent(
    NvHandle            hParent,
    nv_file_private_t  *nvfp,
    NvU32               fd
)
{
    if (NV_OK != allocate_os_event(hParent, nvfp, fd))
    {
        NV_PRINTF(LEVEL_ERROR, "failed to allocate OS event\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    return NV_OK;
}

static NV_STATUS free_os_event(
    NvHandle    hParent,
    NvU32       fd
)
{
    nv_state_t *nv = nv_get_ctl_state();
    nv_event_t *event, *tmp;
    NV_STATUS result;

    portSyncSpinlockAcquire(nv->event_spinlock);
    tmp = event = nv->event_list;
    while (event)
    {
        if ((event->fd == fd) && (event->hParent == hParent))
        {
            if (event == nv->event_list)
                nv->event_list = event->next;
            else
                tmp->next = event->next;
            break;
        }
        tmp = event;
        event = event->next;
    }

    if (event != NULL)
    {
        free_os_event_under_lock(event);
        result = NV_OK;
    }
    else
        result = NV_ERR_INVALID_EVENT;
    portSyncSpinlockRelease(nv->event_spinlock);

    if (result == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "freed OS event:\n");
        NV_PRINTF(LEVEL_INFO, "   hParent: 0x%x\n", hParent);
        NV_PRINTF(LEVEL_INFO, "   fd: %d\n", fd);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "failed to find OS event:\n");
        NV_PRINTF(LEVEL_ERROR, "   hParent: 0x%x\n", hParent);
        NV_PRINTF(LEVEL_ERROR, "   fd: %d\n", fd);
    }

    return result;
}

NV_STATUS RmFreeOsEvent(
    NvHandle    hParent,
    NvU32       fd
)
{
    if (NV_OK != free_os_event(hParent, fd))
    {
        return NV_ERR_INVALID_EVENT;
    }
    return NV_OK;
}

static void RmExecuteWorkItem(
    void *pWorkItem
)
{
    nv_work_item_t *pWi = (nv_work_item_t *)pWorkItem;
    NvU32 gpuMask;
    NvU32 releaseLocks = 0;

    if (!(pWi->flags & NV_WORK_ITEM_FLAGS_REQUIRES_GPU) &&
        ((pWi->flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS_RW) ||
         (pWi->flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE_RW) ||
         (pWi->flags & OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE_RW) ||
         (pWi->flags & OS_QUEUE_WORKITEM_FLAGS_FULL_GPU_SANITY)))
    {
        // Requesting one of the GPU locks without providing a GPU instance
        NV_ASSERT(0);
        goto done;
    }

    // Get locks requested by workitem
    if (NV_OK != workItemLocksAcquire(pWi->gpuInstance, pWi->flags,
                                      &releaseLocks, &gpuMask))
    {
        goto done;
    }

    // Some work items may not require a valid GPU instance
    if (pWi->flags & NV_WORK_ITEM_FLAGS_REQUIRES_GPU)
    {
        pWi->func.pGpuFunction(pWi->gpuInstance, pWi->pData);
    }
    else
    {
        pWi->func.pSystemFunction(pWi->pData);
    }

    // Release any locks taken
    workItemLocksRelease(releaseLocks, gpuMask);

done:
    if ((pWi->pData != NULL) &&
        !(pWi->flags & NV_WORK_ITEM_FLAGS_DONT_FREE_DATA))
    {
        portMemFree(pWi->pData);
    }

    portMemFree((void *)pWi);
}

static NV_STATUS RmGetEventData(
    nv_file_private_t *nvfp,
    NvP64 pEvent,
    NvU32 *MoreEvents,
    NvBool bUserModeArgs
)
{
    NV_STATUS         RmStatus;
    NvUnixEvent      *pKernelEvent = NULL;
    nv_event_t        nv_event;
    RMAPI_PARAM_COPY  paramCopy;

    RmStatus = nv_get_event(nvfp, &nv_event, MoreEvents);
    if (RmStatus != NV_OK)
        return NV_ERR_OPERATING_SYSTEM;

    // setup for access to client's parameters
    RMAPI_PARAM_COPY_INIT(paramCopy, pKernelEvent, pEvent, 1, sizeof(NvUnixEvent));
    RmStatus = rmapiParamsAcquire(&paramCopy, bUserModeArgs);
    if (RmStatus != NV_OK)
        return NV_ERR_OPERATING_SYSTEM;

    pKernelEvent->hObject     = nv_event.hObject;
    pKernelEvent->NotifyIndex = nv_event.index;
    pKernelEvent->info32      = nv_event.info32;
    pKernelEvent->info16      = nv_event.info16;

    // release client buffer access, with copyout as needed
    if (rmapiParamsRelease(&paramCopy) != NV_OK)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static NV_STATUS RmAccessRegistry(
    NvHandle   hClient,
    NvHandle   hObject,
    NvU32      AccessType,
    NvP64      clientDevNodeAddress,
    NvU32      DevNodeLength,
    NvP64      clientParmStrAddress,
    NvU32      ParmStrLength,
    NvP64      clientBinaryDataAddress,
    NvU32      *pBinaryDataLength,
    NvU32      *Data,
    NvU32      *Entry
)
{
    NvU32      gpuMask = 0, gpuInstance = 0;
    OBJGPU    *pGpu;
    NvBool     isDevice = NV_FALSE;
    NV_STATUS  RmStatus = NV_ERR_OPERATING_SYSTEM;
    RsClient  *pClient;
    Device    *pDevice;
    Subdevice *pSubdevice;

    RMAPI_PARAM_COPY devNodeParamCopy;
    NvU8      *tmpDevNode = NULL;
    NvU32      copyOutDevNodeLength = 0;

    RMAPI_PARAM_COPY parmStrParamCopy;
    char      *tmpParmStr = NULL;
    NvU32      copyOutParmStrLength = 0;

    RMAPI_PARAM_COPY binaryDataParamCopy;
    NvU8      *tmpBinaryData = NULL;
    NvU32      BinaryDataLength = 0;
    NvU32      copyOutBinaryDataLength = 0;

    if (NV_OK != serverAcquireClient(&g_resServ, hClient, LOCK_ACCESS_WRITE, &pClient))
        return NV_ERR_INVALID_CLIENT;

    if (hClient == hObject)
    {
        pGpu = NULL;
    }
    else
    {
        RmStatus = deviceGetByHandle(pClient, hObject, &pDevice);
        if (RmStatus != NV_OK)
        {
            RmStatus = subdeviceGetByHandle(pClient, hObject, &pSubdevice);
            if (RmStatus != NV_OK)
                goto done;

            RmStatus = rmGpuGroupLockAcquire(pSubdevice->subDeviceInst,
                    GPU_LOCK_GRP_SUBDEVICE, 
                    GPUS_LOCK_FLAGS_NONE,
                    RM_LOCK_MODULES_GPU, 
                    &gpuMask); 
            if (RmStatus != NV_OK)
                return RmStatus;

            GPU_RES_SET_THREAD_BC_STATE(pSubdevice);
            pGpu = GPU_RES_GET_GPU(pSubdevice);
        }
        else
        {
            RmStatus = rmGpuGroupLockAcquire(pDevice->deviceInst,
                    GPU_LOCK_GRP_DEVICE, 
                    GPUS_LOCK_FLAGS_NONE,
                    RM_LOCK_MODULES_GPU, 
                    &gpuMask); 
            if (RmStatus != NV_OK)
                return RmStatus;

            GPU_RES_SET_THREAD_BC_STATE(pDevice);
            pGpu = GPU_RES_GET_GPU(pDevice);
            isDevice = NV_TRUE;
        }
    }

    if (pBinaryDataLength)
    {
        BinaryDataLength = *pBinaryDataLength;
    }

    // a passed-in devNode
    if (DevNodeLength)
    {
        // the passed-in DevNodeLength does not account for '\0'
        DevNodeLength++;

        if (DevNodeLength > NVOS38_MAX_REGISTRY_STRING_LENGTH)
        {
            RmStatus = NV_ERR_INVALID_STRING_LENGTH;
            goto done;
        }

        // get access to client's DevNode
        RMAPI_PARAM_COPY_INIT(devNodeParamCopy, tmpDevNode, clientDevNodeAddress, DevNodeLength, 1);
        devNodeParamCopy.flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;
        RmStatus = rmapiParamsAcquire(&devNodeParamCopy, NV_TRUE);
        if (RmStatus != NV_OK)
        {
            RmStatus = NV_ERR_OPERATING_SYSTEM;
            goto done;
        }
    }

    // a passed-in parmStr
    if (ParmStrLength)
    {
        // the passed-in ParmStrLength does not account for '\0'
        ParmStrLength++;

        if (ParmStrLength > NVOS38_MAX_REGISTRY_STRING_LENGTH)
        {
            RmStatus = NV_ERR_INVALID_STRING_LENGTH;
            goto done;
        }
        // get access to client's parmStr
        RMAPI_PARAM_COPY_INIT(parmStrParamCopy, tmpParmStr, clientParmStrAddress, ParmStrLength, 1);
        RmStatus = rmapiParamsAcquire(&parmStrParamCopy, NV_TRUE);
        if (RmStatus != NV_OK)
        {
            RmStatus = NV_ERR_OPERATING_SYSTEM;
            goto done;
        }
    }

    if ((AccessType == NVOS38_ACCESS_TYPE_READ_BINARY) ||
        (AccessType == NVOS38_ACCESS_TYPE_WRITE_BINARY))
    {
        if ((BinaryDataLength > NVOS38_MAX_REGISTRY_BINARY_LENGTH) ||
            (BinaryDataLength == 0))
        {
            RmStatus = NV_ERR_INVALID_STRING_LENGTH;
            goto done;
        }

        // get access to client's binaryData
        RMAPI_PARAM_COPY_INIT(binaryDataParamCopy, tmpBinaryData, clientBinaryDataAddress, BinaryDataLength, 1);
        if (AccessType == NVOS38_ACCESS_TYPE_READ_BINARY)
            binaryDataParamCopy.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
        RmStatus = rmapiParamsAcquire(&binaryDataParamCopy, NV_TRUE);
        if (RmStatus != NV_OK)
        {
            RmStatus = NV_ERR_OPERATING_SYSTEM;
            goto done;
        }
    }

    switch (AccessType)
    {
        case NVOS38_ACCESS_TYPE_READ_DWORD:
            RmStatus = osReadRegistryDword(pGpu,
                                tmpParmStr, Data);
            break;

        case NVOS38_ACCESS_TYPE_WRITE_DWORD:
            if (isDevice && osIsAdministrator())
            {
                while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
                {
                    RmStatus = osWriteRegistryDword(pGpu,
                                    tmpParmStr, *Data);

                    if (RmStatus != NV_OK)
                        goto done;
                }
                break;
            }

            RmStatus = osWriteRegistryDword(pGpu,
                                    tmpParmStr, *Data);
            break;

        case NVOS38_ACCESS_TYPE_READ_BINARY:
            RmStatus = osReadRegistryBinary(pGpu,
                                tmpParmStr, tmpBinaryData, &BinaryDataLength);

            if (RmStatus != NV_OK)
            {
                goto done;
            }

            if (BinaryDataLength)
                copyOutBinaryDataLength = BinaryDataLength;

            break;

        case NVOS38_ACCESS_TYPE_WRITE_BINARY:
            if (isDevice && osIsAdministrator())
            {
                while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
                {
                    RmStatus = osWriteRegistryBinary(pGpu,
                                tmpParmStr, tmpBinaryData,
                                BinaryDataLength);

                    if (RmStatus != NV_OK)
                        goto done;
                }
                break;
            }

            RmStatus = osWriteRegistryBinary(pGpu,
                                tmpParmStr, tmpBinaryData,
                                BinaryDataLength);
            break;

        default:
            RmStatus = NV_ERR_INVALID_ACCESS_TYPE;
    }

 done:
    if (gpuMask != 0)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    if (tmpDevNode != NULL)
    {
        // skip copyout on error
        if ((RmStatus != NV_OK) || (copyOutDevNodeLength == 0))
            devNodeParamCopy.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
        devNodeParamCopy.paramsSize = copyOutDevNodeLength;
        if (NV_OK != rmapiParamsRelease(&devNodeParamCopy))
            if (RmStatus == NV_OK)
                RmStatus = NV_ERR_OPERATING_SYSTEM;
    }
    if (tmpParmStr != NULL)
    {
        // skip copyout on error
        if ((RmStatus != NV_OK) || (copyOutParmStrLength == 0))
            parmStrParamCopy.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
        parmStrParamCopy.paramsSize = copyOutParmStrLength;
        if (NV_OK != rmapiParamsRelease(&parmStrParamCopy))
            if (RmStatus == NV_OK)
                RmStatus = NV_ERR_OPERATING_SYSTEM;
    }
    if (tmpBinaryData != NULL)
    {
        // skip copyout on error
        if ((RmStatus != NV_OK) || (copyOutBinaryDataLength == 0))
            binaryDataParamCopy.flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
        binaryDataParamCopy.paramsSize = copyOutBinaryDataLength;
        if (NV_OK != rmapiParamsRelease(&binaryDataParamCopy))
            if (RmStatus == NV_OK)
                RmStatus = NV_ERR_OPERATING_SYSTEM;
        *pBinaryDataLength = copyOutBinaryDataLength;
    }

    serverReleaseClient(&g_resServ, LOCK_ACCESS_WRITE, pClient);
    return RmStatus;
}

static NV_STATUS RmUpdateDeviceMappingInfo(
    NvHandle    hClient,
    NvHandle    hDevice,
    NvHandle    hMappable,
    void       *pOldCpuAddress,
    void       *pNewCpuAddress
)
{
    NV_STATUS status;
    RsClient *pClient;
    RsResourceRef *pMappableRef;
    RsCpuMapping *pCpuMapping;
    Device *pDevice;
    Subdevice *pSubdevice;
    NvU32 gpuMask = 0;

    status = serverAcquireClient(&g_resServ, hClient, LOCK_ACCESS_WRITE, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, hDevice, &pDevice);
    if (status != NV_OK)
    {
        status = subdeviceGetByHandle(pClient, hDevice, &pSubdevice);
        if (status != NV_OK)
            goto done;

        status = rmGpuGroupLockAcquire(pSubdevice->subDeviceInst,
                                       GPU_LOCK_GRP_SUBDEVICE, 
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_GPU, 
                                       &gpuMask); 
        if (status != NV_OK)
            goto done;

        GPU_RES_SET_THREAD_BC_STATE(pSubdevice);
    }
    else
    {
        status = rmGpuGroupLockAcquire(pDevice->deviceInst,
                                       GPU_LOCK_GRP_DEVICE, 
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_GPU, 
                                       &gpuMask); 
        if (status != NV_OK)
            goto done;

        GPU_RES_SET_THREAD_BC_STATE(pDevice);
    }

    status = clientGetResourceRef(pClient, hMappable, &pMappableRef);
    if (status != NV_OK)
        goto done;

    if ((objDynamicCastById(pMappableRef->pResource, classId(Memory)) == NULL) &&
        (objDynamicCastById(pMappableRef->pResource, classId(KernelChannel)) == NULL))
    {
        status = NV_ERR_INVALID_OBJECT_HANDLE;
        goto done;
    }

    status = refFindCpuMappingWithFilter(pMappableRef,
                                         NV_PTR_TO_NvP64(pOldCpuAddress),
                                         serverutilMappingFilterCurrentUserProc,
                                         &pCpuMapping);
    if (status != NV_OK)
        goto done;

    pCpuMapping->pLinearAddress = NV_PTR_TO_NvP64(pNewCpuAddress);

done:

    if (gpuMask != 0)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    serverReleaseClient(&g_resServ, LOCK_ACCESS_WRITE, pClient);
    return status;
}

static NV_STATUS RmPerformVersionCheck(
    void *pData,
    NvU32 dataSize
)
{
    nv_ioctl_rm_api_version_t *pParams;
    char clientCh, rmCh;
    const char *rmStr = NV_VERSION_STRING;
    NvBool relaxed = NV_FALSE;
    NvU32 i;

    if (dataSize != sizeof(nv_ioctl_rm_api_version_t))
        return NV_ERR_INVALID_ARGUMENT;

    pParams = pData;

    //
    // write the reply value, so that the client knows we recognized
    // the request
    //
    pParams->reply = NV_RM_API_VERSION_REPLY_RECOGNIZED;

    //
    // the client requested to override the version check; just return
    // success.
    //
    if (pParams->cmd == NV_RM_API_VERSION_CMD_OVERRIDE)
    {
        return NV_OK;
    }

    //
    // the client requested relaxed version checking; we will only
    // compare the strings until the first decimal point.
    //
    if (pParams->cmd == NV_RM_API_VERSION_CMD_RELAXED)
    {
        relaxed = NV_TRUE;
    }

    //
    // rmStr (i.e., NV_VERSION_STRING) must be null-terminated and fit within
    // NV_RM_API_VERSION_STRING_LENGTH, so that:
    //
    // (1) If the versions don't match, we can return rmStr in
    //     pParams->versionString.
    // (2) The below loop is guaranteed to not overrun rmStr.
    //
    if ((os_string_length(rmStr) + 1) > NV_RM_API_VERSION_STRING_LENGTH)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < NV_RM_API_VERSION_STRING_LENGTH; i++)
    {
        clientCh = pParams->versionString[i];
        rmCh = rmStr[i];

        //
        // fail if the current character is not the same
        //
        if (clientCh != rmCh)
        {
            break;
        }

        //
        // if relaxed matching was requested, succeed when we find the
        // first decimal point
        //
        if ((relaxed) && (clientCh == '.'))
        {
            return NV_OK;
        }

        //
        // we found the end of the strings: succeed
        //
        if (clientCh == '\0')
        {
            return NV_OK;
        }
    }

    //
    // the version strings did not match: print an error message and
    // copy the RM's version string into pParams->versionString, so
    // that the client can report the mismatch; explicitly NULL
    // terminate the client's string, since we cannot trust it
    //
    pParams->versionString[NV_RM_API_VERSION_STRING_LENGTH - 1] = '\0';

    nv_printf(NV_DBG_ERRORS,
              "NVRM: API mismatch: the client has the version %s, but\n"
              "NVRM: this kernel module has the version %s.  Please\n"
              "NVRM: make sure that this kernel module and all NVIDIA driver\n"
              "NVRM: components have the same version.\n",
              pParams->versionString, NV_VERSION_STRING);

    os_string_copy(pParams->versionString, rmStr);

    return NV_ERR_GENERIC;
}

NV_STATUS RmSystemEvent(
    nv_state_t *pNv,
    NvU32       event_type,
    NvU32       event_val
)
{
    NV_STATUS rmStatus = NV_OK;
    NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS params;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    switch (event_type)
    {
        case NV_SYSTEM_ACPI_BATTERY_POWER_EVENT:
        {
            Nv2080PowerEventNotification powerParams;
            portMemSet(&powerParams, 0, sizeof(powerParams));
            powerParams.bSwitchToAC = NV_TRUE;
            powerParams.bGPUCapabilityChanged = NV_FALSE;
            powerParams.displayMaskAffected = 0;

            params.eventType = NV0000_CTRL_SYSTEM_EVENT_TYPE_POWER_SOURCE;
            if (event_val == NV_SYSTEM_ACPI_EVENT_VALUE_POWER_EVENT_BATTERY)
            {
                params.eventData = NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_BATTERY;
                powerParams.bSwitchToAC = NV_FALSE;
            }
            else if (event_val == NV_SYSTEM_ACPI_EVENT_VALUE_POWER_EVENT_AC)
            {
                params.eventData = NV0000_CTRL_SYSTEM_EVENT_DATA_POWER_AC;
                powerParams.bSwitchToAC = NV_TRUE;
            }
            else
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
            }
            if (rmStatus == NV_OK)
            {
                OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);

                rmStatus = pRmApi->Control(pRmApi,
                                           pNv->rmapi.hClient,
                                           pNv->rmapi.hClient,
                                           NV0000_CTRL_CMD_SYSTEM_NOTIFY_EVENT,
                                           (void *)&params,
                                           sizeof(NV0000_CTRL_SYSTEM_NOTIFY_EVENT_PARAMS));

                //
                // TODO: bug 2812848 Investigate if we can use system event
                // or if we can broadcast NV2080_NOTIFIERS_POWER_EVENT for all GPUs
                //
                gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_EVENT,
                            &powerParams, sizeof(powerParams), 0, 0);
            }
            break;
        }
        default:
            rmStatus = NV_ERR_INVALID_ARGUMENT;
    }

    return rmStatus;
}

/*!
 * @brief Deal with D-notifier events to apply a performance
 * level based on the requested auxiliary power-state.
 * Read confluence page "D-Notifiers on Linux" for more details.
 *
 * @param[in]   pGpu         OBJGPU pointer.
 * @param[in]   event_type   NvU32 Event type.
 */
static void RmHandleDNotifierEvent(
    nv_state_t *pNv,
    NvU32       event_type
)
{
}

static NV_STATUS
RmDmabufVerifyMemHandle(
    OBJGPU  *pGpu,
    NvHandle hSrcClient,
    NvHandle hMemory,
    NvU64    offset,
    NvU64    size,
    void    *pGpuInstanceInfo
)
{
    NV_STATUS status;
    RsClient *pClient = NULL;
    RsResourceRef *pSrcMemoryRef = NULL;
    Memory *pSrcMemory = NULL;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hSrcClient, &pClient));

    status = clientGetResourceRef(pClient, hMemory, &pSrcMemoryRef);
    if (status != NV_OK)
    {
        return status;
    }

    pSrcMemory = dynamicCast(pSrcMemoryRef->pResource, Memory);
    if (pSrcMemory == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    pMemDesc = pSrcMemory->pMemDesc;

    if (pGpuInstanceInfo != NULL)
    {
        KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
        pKernelMIGGpuInstance = (KERNEL_MIG_GPU_INSTANCE *) pGpuInstanceInfo;

        if ((pKernelMIGGpuInstance->pMemoryPartitionHeap != pSrcMemory->pHeap))
        {
            return NV_ERR_INVALID_OBJECT_PARENT;
        }
    }

    // Check if hMemory belongs to the same pGpu
    if ((pMemDesc->pGpu != pGpu) &&
        (pSrcMemory->pGpu != pGpu))
    {
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    // Offset and size must be aligned to OS page-size
    if (!NV_IS_ALIGNED64(offset, os_page_size) ||
        !NV_IS_ALIGNED64(size, os_page_size))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Only supported for vidmem handles
    if (memdescGetAddressSpace(pMemDesc) != ADDR_FBMEM)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((size == 0) ||
        (size > memdescGetSize(pMemDesc)) ||
        (offset > (memdescGetSize(pMemDesc) - size)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

static NV_STATUS
RmDmabufGetClientAndDevice(
    OBJGPU   *pGpu,
    NvHandle  hClient,
    NvHandle *phClient,
    NvHandle *phDevice,
    NvHandle *phSubdevice,
    void    **ppGpuInstanceInfo
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (IS_MIG_ENABLED(pGpu))
    {
        NV_STATUS status;
        MIG_INSTANCE_REF ref;
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

        status = kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager,
                                                 hClient, &ref);
        if (status != NV_OK)
        {
            return status;
        }

        status = kmigmgrIncRefCount(ref.pKernelMIGGpuInstance->pShare);
        if (status != NV_OK)
        {
            return status;
        }

        *phClient    = ref.pKernelMIGGpuInstance->instanceHandles.hClient;
        *phDevice    = ref.pKernelMIGGpuInstance->instanceHandles.hDevice;
        *phSubdevice = ref.pKernelMIGGpuInstance->instanceHandles.hSubdevice;
        *ppGpuInstanceInfo = (void *) ref.pKernelMIGGpuInstance;

        return NV_OK;
    }

    *phClient    = pMemoryManager->hClient;
    *phDevice    = pMemoryManager->hDevice;
    *phSubdevice = pMemoryManager->hSubdevice;
    *ppGpuInstanceInfo = NULL;

    return NV_OK;
}

static void
RmDmabufPutClientAndDevice(
    OBJGPU   *pGpu,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hSubdevice,
    void     *pGpuInstanceInfo
)
{
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;

    if (pGpuInstanceInfo == NULL)
    {
        return;
    }

    pKernelMIGGpuInstance = (KERNEL_MIG_GPU_INSTANCE *) pGpuInstanceInfo;

    NV_ASSERT_OK(kmigmgrDecRefCount(pKernelMIGGpuInstance->pShare));
}

/*
 * ---------------------------------------------------------------------------
 *
 * The routines below are part of the interface between the kernel interface
 * layer and the kernel-agnostic portions of the resource manager.
 *
 * ---------------------------------------------------------------------------
 */

NvBool NV_API_CALL rm_init_private_state(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    NvBool retval;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    retval = RmInitPrivateState(pNv);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return retval;
}

void NV_API_CALL rm_free_private_state(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    RmFreePrivateState(pNv);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

NvBool NV_API_CALL rm_init_adapter(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    NvBool     retval = NV_FALSE;
    void      *fp;
    NvBool     bEnabled;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_DEVICE_INIT);

    // LOCK: acquire API lock
    if (rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT) == NV_OK)
    {
        if (!((gpumgrQueryGpuDrainState(pNv->gpu_id, &bEnabled, NULL) == NV_OK)
              && bEnabled))
        {
            if (pNv->flags & NV_FLAG_PERSISTENT_SW_STATE)
            {
                retval = RmPartiallyInitAdapter(pNv);
            }
            else
            {
                retval = RmInitAdapter(pNv);
            }
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return retval;
}

void NV_API_CALL rm_disable_adapter(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    void      *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue));

    // LOCK: acquire API lock
    if (rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DESTROY) == NV_OK)
    {
        if (pNv->flags & NV_FLAG_PERSISTENT_SW_STATE)
        {
            RmPartiallyDisableAdapter(pNv);
        }
        else
        {
            RmDisableAdapter(pNv);
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue));
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

void NV_API_CALL rm_shutdown_adapter(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    void      *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if (rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DESTROY) == NV_OK)
    {
        RmShutdownAdapter(pNv);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_exclude_adapter(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE threadState;
    void      *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    rmStatus = RmExcludeAdapter(pNv);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_acquire_api_lock(
    nvidia_stack_t *sp
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_release_api_lock(
    nvidia_stack_t *sp
)
{
    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // UNLOCK: release API lock
    rmApiLockRelease();

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return NV_OK;
}

NV_STATUS NV_API_CALL rm_acquire_gpu_lock(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire GPU lock
    rmStatus = rmDeviceGpuLocksAcquire(NV_GET_NV_PRIV_PGPU(nv),
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_OSAPI);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_release_gpu_lock(
    nvidia_stack_t *sp,
    nv_state_t     *nv
)
{
    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // UNLOCK: release GPU lock
    rmDeviceGpuLocksRelease(NV_GET_NV_PRIV_PGPU(nv), GPUS_LOCK_FLAGS_NONE, NULL);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return NV_OK;
}

NV_STATUS NV_API_CALL rm_acquire_all_gpus_lock(
    nvidia_stack_t *sp
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire all GPUs lock
    rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_release_all_gpus_lock(
    nvidia_stack_t *sp
)
{
    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // UNLOCK: release all GPUs lock
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return NV_OK;
}

/*!
 * @brief Handle ACPI_NOTIFY_GPS_STATUS_CHANGE event.
 *
 * This function is called for GPS when SBIOS trigger
 * gps STATUS_CHANGE event, which calls rm control call
 * NV0000_CTRL_CMD_SYSTEM_GPS_CONTROL to init the GPS
 * data from SBIOS.
 */
static void RmHandleGPSStatusChange
(
    nv_state_t *pNv
)
{
}

/*!
 * @brief Function to handle device specific ACPI events.
 *
 * @param[in]   sp           nvidia_stack_t pointer.
 * @param[in]   nv           nv_state_t pointer.
 * @param[in]   event_type   NvU32 Event type.
 */
void NV_API_CALL rm_acpi_notify(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvU32 event_type
)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);

    switch (event_type)
    {
        case ACPI_VIDEO_NOTIFY_PROBE:
        {
            THREAD_STATE_NODE threadState;

            threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
            RmHandleDisplayChange(sp, nv);
            threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
            break;
        }

        case ACPI_NOTIFY_GPS_STATUS_CHANGE:
            RmHandleGPSStatusChange(nv);
            break;

        case ACPI_NOTIFY_POWER_LEVEL_D1:    /* fallthrough */
        case ACPI_NOTIFY_POWER_LEVEL_D2:    /* fallthrough */
        case ACPI_NOTIFY_POWER_LEVEL_D3:    /* fallthrough */
        case ACPI_NOTIFY_POWER_LEVEL_D4:    /* fallthrough */
        case ACPI_NOTIFY_POWER_LEVEL_D5:
            RmHandleDNotifierEvent(nv, event_type);
            break;

        default:
            NV_PRINTF(LEVEL_INFO, "No support for 0x%x event\n", event_type);
            NV_ASSERT(0);
            break;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

static void nv_align_mmap_offset_length(
    nv_usermap_access_params_t *nvuap)
{
    NvU64 page_size = os_page_size;
    NvU64 end = nvuap->size + (nvuap->addr & (page_size - 1));

    nvuap->mmap_start = NV_ALIGN_DOWN(nvuap->addr, page_size);
    nvuap->mmap_size = NV_ALIGN_UP(end, page_size);
    nvuap->offset = NV_ALIGN_DOWN(nvuap->offset, page_size);
}

static inline NV_STATUS RmGetArrayMinMax(
    NvU64 *array,
    NvU64 count,
    NvU64 *min,
    NvU64 *max
)
{
    NvU64 i;

    if (array == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *min = array[0];
    *max = array[0];

    if (count == 1)
        return NV_OK;

    for (i = 1; i < count; i++)
    {
        if (array[i] > *max)
            *max = array[i];

        if (array[i] < *min)
            *min = array[i];
    }

    return NV_OK;
}

static NV_STATUS RmSetUserMapAccessRange(
    nv_usermap_access_params_t *nvuap
)
{
    NV_STATUS status = NV_OK;

    if (nvuap->contig)
    {
         nvuap->access_start = nvuap->mmap_start;
         nvuap->access_size = nvuap->mmap_size;
    }
    else
    {
        NvU64 highest_address_mapped;
        NvU64 lowest_address_mapped;

        status = RmGetArrayMinMax(nvuap->page_array, nvuap->num_pages,
                                  &lowest_address_mapped,
                                  &highest_address_mapped);
        if (status != NV_OK)
        {
            return status;
        }

        nvuap->access_start = lowest_address_mapped;
        nvuap->access_size = (highest_address_mapped + os_page_size) - lowest_address_mapped;
    }

    return status;
}

static NV_STATUS RmGetAllocPrivate(NvU32, NvU32, NvU64, NvU64, NvU32 *, void **,
                                   NvU64 *);
static NV_STATUS RmValidateMmapRequest(nv_state_t *, NvU64, NvU64, NvU32 *);

static NV_STATUS RmGetMmapPteArray(
    KernelMemorySystem         *pKernelMemorySystem,
    NvHandle                    hClient,
    NvHandle                    hMemory,
    nv_usermap_access_params_t *nvuap
)
{
    NV_STATUS status = NV_OK;
    NvU64 pages = 0;
    Memory *pMemory = NULL;
    PMEMORY_DESCRIPTOR pMemDesc  = NULL;
    RsResourceRef *pResourceRef;
    NvU64 i;
    NvU64 *pteArray;
    NvU64 index;

    if (nvuap == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // If we're mapping a memory handle, we can get the data from the
    // descriptor (required if the allocation being mapped is discontiguous).
    //
    if (serverutilGetResourceRefWithType(hClient, hMemory, classId(Memory),
                                        &pResourceRef) == NV_OK)
    {
        pMemory = dynamicCast(pResourceRef->pResource, Memory);
        pMemDesc = pMemory->pMemDesc;
        nvuap->contig = memdescGetContiguity(pMemDesc, AT_CPU);
    }

    //
    // In the discontiguous case, the page array needs to be large enough to hold
    // the 4K-page-sized addresses that we will then deflate to OS page addresses.
    // For the contiguous case, we can create the OS-page-sized addresses
    // directly in the array.
    //
    if (nvuap->contig)
    {
        pages = nvuap->mmap_size / os_page_size;
    }
    else
    {
        pages = nvuap->mmap_size / NV_RM_PAGE_SIZE;
    }

    NV_ASSERT_OR_RETURN(pages != 0, NV_ERR_INVALID_ARGUMENT);

    status = os_alloc_mem((void **)&nvuap->page_array, (pages * sizeof(NvU64)));
    if (status != NV_OK)
    {
        return status;
    }

    if (!nvuap->contig)
    {
        pteArray = memdescGetPteArray(pMemDesc, AT_CPU);
        index = nvuap->offset / NV_RM_PAGE_SIZE;

        //
        // We're guaranteed to have a MEMORY_DESCRIPTOR in the discontiguous
        // case. Copy over the addresses now.
        //
        portMemCopy((void *)nvuap->page_array,
                    pages * sizeof(NvU64), (void *)&pteArray[index],
                    pages * sizeof(NvU64));

        if (NV_RM_PAGE_SIZE < os_page_size)
        {
            RmDeflateRmToOsPageArray(nvuap->page_array, pages);
            pages = NV_RM_PAGES_TO_OS_PAGES(pages);
        }

        //
        // Convert the GPU physical addresses to system physical addresses,
        // if applicable.
        //
        for (i = 0; i < pages; i++)
        {
            nvuap->page_array[i] += pKernelMemorySystem->coherentCpuFbBase;
        }
    }
    else
    {
        // Offset is accounted in mmap_start.
        for (nvuap->page_array[0] = nvuap->mmap_start, i = 1;
             i < pages; i++)
        {
            nvuap->page_array[i] = nvuap->page_array[i-1] + os_page_size;
        }
    }

    nvuap->num_pages = pages;

    return status;
}

/* Must be called with the API lock and the GPU locks */
static NV_STATUS RmCreateMmapContextLocked(
    NvHandle    hClient,
    NvHandle    hDevice,
    NvHandle    hMemory,
    NvP64       address,
    NvU64       size,
    NvU64       offset,
    NvU32       fd
)
{
    NV_STATUS status = NV_OK;
    void *pAllocPriv = NULL;
    OBJGPU *pGpu = NULL;
    KernelMemorySystem *pKernelMemorySystem = NULL;
    NvBool bCoherentAtsCpuOffset = NV_FALSE;
    nv_state_t *pNv = NULL;
    NvU64 addr = (NvU64)address;
    NvU32 prot = 0;
    NvU64 pageIndex = 0;
    nv_usermap_access_params_t *nvuap = NULL;
    NvBool bClientMap = (hClient == hDevice);

    if (!bClientMap)
    {
        if (CliSetGpuContext(hClient, hDevice, &pGpu, NULL) != NV_OK)
        {
            NvU32 tmp;
            if (CliSetSubDeviceContext(hClient, hDevice, &tmp, &pGpu) != NV_OK)
            {
                //
                // If this mapping isn't for a GPU then we don't need to
                // create a context for it.
                //
                return status;
            }
        }
    }

    status = os_alloc_mem((void**)&nvuap, sizeof(nv_usermap_access_params_t));
    if (status != NV_OK)
    {
        return status;
    }

    portMemSet(nvuap, 0, sizeof(nv_usermap_access_params_t));
    nvuap->addr = addr;
    nvuap->size = size;
    nvuap->offset = offset;

    //
    // Assume the allocation is contiguous until RmGetMmapPteArray
    // determines otherwise.
    //
    nvuap->contig = NV_TRUE;
    nv_align_mmap_offset_length(nvuap);

    if (pGpu != NULL)
    {
        pNv = NV_GET_NV_STATE(pGpu);
        pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
        bCoherentAtsCpuOffset = IS_COHERENT_CPU_ATS_OFFSET(pKernelMemorySystem, addr, size);
    }

    //
    // If no device is given, or the address isn't in the given device's BARs,
    // validate this as a system memory mapping and associate it with the
    // control device.
    //
    if ((pNv == NULL) ||
        (!IS_REG_OFFSET(pNv, addr, size) &&
         !IS_FB_OFFSET(pNv, addr, size) &&
         !bCoherentAtsCpuOffset &&
         !IS_IMEM_OFFSET(pNv, addr, size)))
    {
        pNv = nv_get_ctl_state();

        //
        // Validate the mapping request by looking up the underlying sysmem
        // allocation.
        //
        status = RmGetAllocPrivate(hClient, hMemory, addr, size, &prot, &pAllocPriv,
                                   &pageIndex);

        if (status != NV_OK)
        {
            goto done;
        }
    }
    else
    {
        //
        // Validate the mapping request for ATS and get pteArray
        //
        if (bCoherentAtsCpuOffset)
        {
            status = RmGetMmapPteArray(pKernelMemorySystem, hClient, hMemory, nvuap);
            if (status != NV_OK)
            {
                goto done;
            }
        }

        if (RmSetUserMapAccessRange(nvuap) != NV_OK)
        {
            goto done;
        }

        status = nv_get_usermap_access_params(pNv, nvuap);
        if (status != NV_OK)
        {
            goto done;
        }

        // Validate the mapping request for BAR's.
        status = RmValidateMmapRequest(pNv, nvuap->access_start,
                                       nvuap->access_size, &prot);
        if (status != NV_OK)
        {
            goto done;
        }
    }

    status = nv_add_mapping_context_to_file(pNv, nvuap, prot, pAllocPriv,
                                            pageIndex, fd);

done:
    os_free_mem(nvuap);
    return status;
}

// TODO: Bug 1802250: [uvm8] Use an alt stack in all functions in unix/src/osapi.c
NV_STATUS rm_create_mmap_context(
    nv_state_t *pNv,
    NvHandle    hClient,
    NvHandle    hDevice,
    NvHandle    hMemory,
    NvP64       address,
    NvU64       size,
    NvU64       offset,
    NvU32       fd
)
{
    NV_STATUS rmStatus = NV_OK;
    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_OSAPI)) == NV_OK)
    {
        RmClient *pClient;

        if (NV_OK != serverutilAcquireClient(hClient, LOCK_ACCESS_READ, &pClient))
            return NV_ERR_INVALID_CLIENT;

        if (pClient->ProcID != osGetCurrentProcess())
        {
            rmStatus = NV_ERR_INVALID_CLIENT;
        }
        // LOCK: acquire GPUs lock
        else if ((rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI)) == NV_OK)
        {
            rmStatus = RmCreateMmapContextLocked(hClient, hDevice, hMemory,
                                                 address, size, offset, fd);
            // UNLOCK: release GPUs lock
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        }

        serverutilReleaseClient(LOCK_ACCESS_READ, pClient);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return rmStatus;
}

static NV_STATUS RmGetAllocPrivate(
    NvU32       hClient,
    NvU32       hMemory,
    NvU64       offset,
    NvU64       length,
    NvU32      *pProtection,
    void      **ppPrivate,
    NvU64      *pPageIndex
)
{
    RmClient *pClient;
    NV_STATUS rmStatus;
    PMEMORY_DESCRIPTOR pMemDesc;
    NvU32 pageOffset;
    NvU64 pageCount;
    NvU64 endingOffset;
    RsResourceRef *pResourceRef;
    RmResource *pRmResource;
    void *pMemData;
    NvBool bPeerIoMem;
    NvBool bReadOnlyMem;
    *pProtection = NV_PROTECT_READ_WRITE;
    *ppPrivate = NULL;

    pageOffset = (offset & ~os_page_mask);
    offset &= os_page_mask;

    NV_ASSERT_OR_RETURN(rmApiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (NV_OK != serverutilAcquireClient(hClient, LOCK_ACCESS_READ, &pClient))
        return NV_ERR_INVALID_CLIENT;

    rmStatus = clientGetResourceRef(staticCast(pClient, RsClient), hMemory, &pResourceRef);
    if (rmStatus != NV_OK)
        goto done;

    pRmResource = dynamicCast(pResourceRef->pResource, RmResource);
    if (!pRmResource)
    {
        rmStatus = NV_ERR_INVALID_OBJECT;
        goto done;
    }

    rmStatus = rmresGetMemoryMappingDescriptor(pRmResource, &pMemDesc);
    if (rmStatus != NV_OK)
        goto done;

    bReadOnlyMem = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY);
    bPeerIoMem = memdescGetFlag(pMemDesc, MEMDESC_FLAGS_PEER_IO_MEM);

    if (!(pMemDesc->Allocated || bPeerIoMem))
    {
        rmStatus = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_SYSMEM:
            break;
        default:
            rmStatus = NV_ERR_OBJECT_NOT_FOUND;
            goto done;
    }

    pMemData = memdescGetMemData(pMemDesc);
    if (pMemData == NULL)
    {
        rmStatus = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    rmStatus = os_match_mmap_offset(pMemData, offset, pPageIndex);
    if (rmStatus != NV_OK)
        goto done;

    endingOffset = pageOffset + length;
    pageCount = (endingOffset / os_page_size);
    pageCount += (*pPageIndex + ((endingOffset % os_page_size) ? 1 : 0));

    if (pageCount > NV_RM_PAGES_TO_OS_PAGES(pMemDesc->PageCount))
    {
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    if (bReadOnlyMem)
       *pProtection = NV_PROTECT_READABLE;

    *ppPrivate = pMemData;

done:
    serverutilReleaseClient(LOCK_ACCESS_READ, pClient);

    return rmStatus;
}

static NV_STATUS RmValidateMmapRequest(
    nv_state_t *pNv,
    NvU64       offset,
    NvU64       length,
    NvU32      *pProtection
)
{
    NV2080_CTRL_GPU_VALIDATE_MEM_MAP_REQUEST_PARAMS params = { 0 };
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS status;

    if (osIsAdministrator())
    {
        *pProtection = NV_PROTECT_READ_WRITE;
        return NV_OK;
    }

    params.addressStart = offset;
    params.addressLength = length;

    status = pRmApi->Control(pRmApi, pNv->rmapi.hClient,
                             pNv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_GPU_VALIDATE_MEM_MAP_REQUEST,
                             &params, sizeof(params));

    if (status == NV_OK)
    {
        *pProtection = params.protection;
    }

    return status;
}

NV_STATUS rm_get_adapter_status(
    nv_state_t *pNv,
    NvU32      *pStatus
)
{
    NV_STATUS rmStatus = NV_ERR_OPERATING_SYSTEM;

    // LOCK: acquire API lock
    if (rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_OSAPI) == NV_OK)
    {
        rmStatus = RmGetAdapterStatus(pNv, pStatus);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return rmStatus;
}

NvBool NV_API_CALL rm_init_rm(
    nvidia_stack_t *sp
)
{
    NvBool retval;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    retval = RmInitRm();

    NV_EXIT_RM_RUNTIME(sp,fp);

    return retval;
}

void NV_API_CALL rm_shutdown_rm(
    nvidia_stack_t *sp
)
{
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    RmShutdownRm();

    NV_EXIT_RM_RUNTIME(sp,fp);
}

NvBool NV_API_CALL rm_init_event_locks(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    void *fp;
    NvBool ret;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pNv->event_spinlock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    ret = (pNv->event_spinlock != NULL);

    NV_EXIT_RM_RUNTIME(sp,fp);
    return ret;
}

void NV_API_CALL rm_destroy_event_locks(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    if (pNv && pNv->event_spinlock)
        portSyncSpinlockDestroy(pNv->event_spinlock);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

void NV_API_CALL rm_get_vbios_version(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    char *vbiosString
)
{
    void   *fp;
    NV2080_CTRL_BIOS_GET_INFO_V2_PARAMS *params;
    RM_API *pRmApi;
    THREAD_STATE_NODE threadState;
    const size_t vbiosStringLen = 15; // "xx.xx.xx.xx.xx"

    os_snprintf(vbiosString, vbiosStringLen, "??.??.??.??.??");

    NV_ENTER_RM_RUNTIME(sp,fp);

    params = portMemAllocNonPaged(sizeof(*params));
    if (params == NULL)
    {
        NV_EXIT_RM_RUNTIME(sp,fp);
        return;
    }

    portMemSet(params, 0, sizeof(*params));

    params->biosInfoList[0].index = NV2080_CTRL_BIOS_INFO_INDEX_REVISION;
    params->biosInfoList[1].index = NV2080_CTRL_BIOS_INFO_INDEX_OEM_REVISION;
    params->biosInfoListSize = 2;

    pRmApi = RmUnixRmApiPrologue(pNv, &threadState, RM_LOCK_MODULES_VBIOS);
    if (pRmApi != NULL)
    {
        NV_STATUS rmStatus;

        rmStatus = pRmApi->Control(pRmApi,
                                   pNv->rmapi.hClient,
                                   pNv->rmapi.hSubDevice,
                                   NV2080_CTRL_CMD_BIOS_GET_INFO_V2,
                                   params,
                                   sizeof(*params));

        if (rmStatus == NV_OK)
        {
            const NvU32 biosRevision = params->biosInfoList[0].data;
            const NvU32 biosOEMRevision = params->biosInfoList[1].data;

            os_snprintf(vbiosString, vbiosStringLen,
                        "%02x.%02x.%02x.%02x.%02x",
                        (biosRevision & 0xff000000) >> 24,
                        (biosRevision & 0x00ff0000) >> 16,
                        (biosRevision & 0x0000ff00) >>  8,
                        (biosRevision & 0x000000ff) >>  0,
                        biosOEMRevision);
        }

        RmUnixRmApiEpilogue(pNv, &threadState);
    }

    portMemFree(params);

    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_stop_user_channels(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS params = { 0 };
    RM_API            *pRmApi;
    THREAD_STATE_NODE  threadState;
    NV_STATUS          rmStatus = NV_ERR_INVALID_STATE;
    void              *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pRmApi = RmUnixRmApiPrologue(pNv, &threadState, RM_LOCK_MODULES_FIFO);
    if (pRmApi != NULL)
    {
        params.bDisable = NV_TRUE;
        rmStatus = pRmApi->Control(pRmApi, pNv->rmapi.hClient,
                                   pNv->rmapi.hSubDevice,
                                   NV2080_CTRL_CMD_FIFO_DISABLE_USERMODE_CHANNELS,
                                   &params, sizeof(params));

        RmUnixRmApiEpilogue(pNv, &threadState);
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_restart_user_channels(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    NV2080_CTRL_FIFO_DISABLE_USERMODE_CHANNELS_PARAMS params = { 0 };
    RM_API            *pRmApi;
    THREAD_STATE_NODE  threadState;
    NV_STATUS          rmStatus = NV_ERR_INVALID_STATE;
    void              *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pRmApi = RmUnixRmApiPrologue(pNv, &threadState, RM_LOCK_MODULES_FIFO);
    if (pRmApi != NULL)
    {
        params.bDisable = NV_FALSE;
        rmStatus = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hSubDevice,
                                   NV2080_CTRL_CMD_FIFO_DISABLE_USERMODE_CHANNELS,
                                   &params, sizeof(params));

        RmUnixRmApiEpilogue(pNv, &threadState);
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

//
// Use this call to check if the chipset is io coherent
//
NvBool NV_API_CALL rm_is_chipset_io_coherent(
    nvidia_stack_t  *sp)
{
    void *fp;
    OBJSYS *pSys;
    OBJCL *pCl;
    NvBool bIoCoherent = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pSys = SYS_GET_INSTANCE();
    pCl = SYS_GET_CL(pSys);
    if (pCl == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: no CL object found, setting io coherent by default\n",
                  __FUNCTION__);
        goto done;
    }

    bIoCoherent = pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT);

done:
    NV_EXIT_RM_RUNTIME(sp,fp);

    return bIoCoherent;
}

NV_STATUS NV_API_CALL rm_ioctl(
    nvidia_stack_t     *sp,
    nv_state_t         *pNv,
    nv_file_private_t  *nvfp,
    NvU32               Command,
    void               *pData,
    NvU32               dataSize
)
{
    NV_STATUS rmStatus;
    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    rmStatus = RmIoctl(pNv, nvfp, Command, pData, dataSize);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

void NV_API_CALL rm_cleanup_file_private(
    nvidia_stack_t     *sp,
    nv_state_t         *pNv,
    nv_file_private_t  *nvfp
)
{
    THREAD_STATE_NODE threadState;
    void      *fp;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_EXTERNAL);
    RM_API_CONTEXT rmApiContext = {0};
    NvU32 i;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    threadStateSetTimeoutOverride(&threadState, 10 * 1000);

    if (rmapiPrologue(pRmApi, &rmApiContext) != NV_OK)
        return;

    // LOCK: acquire API lock
    if (rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI) == NV_OK)
    {
        // Unref any object which was exported on this file.
        if (nvfp->handles != NULL)
        {
            for (i = 0; i < nvfp->maxHandles; i++)
            {
                if (nvfp->handles[i] == 0)
                {
                    continue;
                }

                RmFreeObjExportHandle(nvfp->handles[i]);
                nvfp->handles[i] = 0;
            }

            os_free_mem(nvfp->handles);
            nvfp->handles = NULL;
            nvfp->maxHandles = 0;
        }

        // Free any RM clients associated with this file.
        RmFreeUnusedClients(pNv, nvfp);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    rmapiEpilogue(pRmApi, &rmApiContext);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    if (nvfp->ctl_nvfp != NULL)
    {
        nv_put_file_private(nvfp->ctl_nvfp_priv);
        nvfp->ctl_nvfp = NULL;
        nvfp->ctl_nvfp_priv = NULL;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

void NV_API_CALL rm_unbind_lock(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    void      *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if (rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI) == NV_OK)
    {
        RmUnbindLock(pNv);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS rm_alloc_os_event(
    NvHandle            hClient,
    nv_file_private_t  *nvfp,
    NvU32               fd
)
{
    NV_STATUS RmStatus;

    // LOCK: acquire API lock
    if ((RmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_EVENT)) == NV_OK)
    {
        RmStatus = RmAllocOsEvent(hClient, nvfp, fd);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return RmStatus;
}

NV_STATUS rm_free_os_event(
    NvHandle    hClient,
    NvU32       fd
)
{
    NV_STATUS RmStatus;

    // LOCK: acquire API lock
    if ((RmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_EVENT)) == NV_OK)
    {
        RmStatus = RmFreeOsEvent(hClient, fd);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return RmStatus;
}

NV_STATUS rm_get_event_data(
    nv_file_private_t  *nvfp,
    NvP64               pEvent,
    NvU32              *MoreEvents
)
{
    NV_STATUS RmStatus;

    // LOCK: acquire API lock
    if ((RmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_EVENT)) == NV_OK)
    {
        RmStatus = RmGetEventData(nvfp, pEvent, MoreEvents, NV_TRUE);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return RmStatus;
}

NV_STATUS NV_API_CALL rm_read_registry_dword(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    const char *regParmStr,
    NvU32      *Data
)
{
    OBJGPU    *pGpu = NULL;
    NV_STATUS  RmStatus;
    void      *fp;
    NvBool     isApiLockTaken = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);

    //
    // We can be called from different contexts:
    //
    // 1) early initialization without device state.
    // 2) from outside the RM API (without the lock held)
    //
    // In context 1)the API lock is not needed and
    // in context 2), it needs to be acquired.
    //
    if (nv != NULL)
    {
        // LOCK: acquire API lock
        if ((RmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_OSAPI)) != NV_OK)
        {
            NV_EXIT_RM_RUNTIME(sp,fp);
            return RmStatus;
        }

        isApiLockTaken = NV_TRUE;
    }

    pGpu = NV_GET_NV_PRIV_PGPU(nv);

    // Skipping the NULL check as osReadRegistryDword takes care of it.
    RmStatus = osReadRegistryDword(pGpu, regParmStr, Data);

    if (isApiLockTaken == NV_TRUE)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return RmStatus;
}

NV_STATUS NV_API_CALL rm_write_registry_dword(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    const char *regParmStr,
    NvU32       Data
)
{
    NV_STATUS  RmStatus;
    void      *fp;
    NvBool     isApiLockTaken = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);

    if (nv != NULL)
    {
        // LOCK: acquire API lock
        if ((RmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI)) != NV_OK)
        {
            NV_EXIT_RM_RUNTIME(sp,fp);
            return RmStatus;
        }

        isApiLockTaken = NV_TRUE;
    }

    RmStatus = RmWriteRegistryDword(nv, regParmStr, Data);

    if (isApiLockTaken == NV_TRUE)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return RmStatus;
}

NV_STATUS NV_API_CALL rm_write_registry_binary(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    const char *regParmStr,
    NvU8       *Data,
    NvU32       cbLen
)
{
    NV_STATUS  RmStatus;
    void      *fp;
    NvBool     isApiLockTaken = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);

    if (nv != NULL)
    {
        // LOCK: acquire API lock
        if ((RmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI)) != NV_OK)
        {
            NV_EXIT_RM_RUNTIME(sp,fp);
            return RmStatus;
        }

        isApiLockTaken = NV_TRUE;
    }

    RmStatus = RmWriteRegistryBinary(nv, regParmStr, Data, cbLen);

    if (isApiLockTaken == NV_TRUE)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return RmStatus;
}

NV_STATUS NV_API_CALL rm_write_registry_string(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    const char *regParmStr,
    const char *string,
    NvU32       stringLength
)
{
    NV_STATUS  rmStatus;
    void      *fp;
    NvBool     isApiLockTaken = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);

    if (nv != NULL)
    {
        // LOCK: acquire API lock
        if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI)) != NV_OK)
        {
            NV_EXIT_RM_RUNTIME(sp,fp);
            return rmStatus;
        }

        isApiLockTaken = NV_TRUE;
    }

    rmStatus = RmWriteRegistryString(nv, regParmStr, string, (stringLength + 1));

    if (isApiLockTaken == NV_TRUE)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

static NvBool NV_API_CALL rm_is_space(const char ch)
{
    //
    // return true if it is a:
    // ' '  : (space - decimal 32.)
    // '\t' : (TAB - decimal 9)
    // 'LF' : (Line feed, new line - decimal 10)
    // 'VT' : (Vertical TAB - decimal 11)
    // 'FF' : (Form feed, new page - decimal 12)
    // '\r' : (carriage return - decimal 13)
    //
    return ((ch == ' ') || ((ch >= '\t') && (ch <= '\r')));
}

char* NV_API_CALL rm_remove_spaces(const char *in)
{
    unsigned int len = os_string_length(in) + 1;
    const char *in_ptr;
    char *out, *out_ptr;

    if (os_alloc_mem((void **)&out, len) != NV_OK)
        return NULL;

    in_ptr = in;
    out_ptr = out;

    while (*in_ptr != '\0')
    {
        if (!rm_is_space(*in_ptr))
            *out_ptr++ = *in_ptr;
        in_ptr++;
    }
    *out_ptr = '\0';

    return out;
}

char* NV_API_CALL rm_string_token(char **strp, const char delim)
{
    char *s, *token;

    if ((strp == NULL) || (*strp == NULL))
        return NULL;

    s = token = *strp;
    *strp = NULL;

    for (; *s != '\0'; s++) {
        if (*s == delim) {
            *s = '\0';
            *strp = ++s;
            break;
        }
    }

    return token;
}

// Parse string passed in NVRM as module parameter.
void NV_API_CALL rm_parse_option_string(nvidia_stack_t *sp, const char *nvRegistryDwords)
{
    unsigned int i;
    nv_parm_t *entry;
    char *option_string = NULL;
    char *ptr, *token;
    char *name, *value;
    NvU32 data;

    if (nvRegistryDwords != NULL)
    {
        if ((option_string = rm_remove_spaces(nvRegistryDwords)) == NULL)
        {
            return;
        }

        ptr = option_string;

        while ((token = rm_string_token(&ptr, ';')) != NULL)
        {
            if (!(name = rm_string_token(&token, '=')) || !os_string_length(name))
            {
                continue;
            }

            if (!(value = rm_string_token(&token, '=')) || !os_string_length(value))
            {
                continue;
            }

            if (rm_string_token(&token, '=') != NULL)
            {
                continue;
            }

            data = os_strtoul(value, NULL, 0);

            for (i = 0; (entry = &nv_parms[i])->name != NULL; i++)
            {
                if (os_string_compare(entry->name, name) == 0)
                    break;
            }

            if (!entry->name)
                rm_write_registry_dword(sp, NULL, name, data);
            else
                *entry->data = data;
        }

        // Free the memory allocated by rm_remove_spaces()
        os_free_mem(option_string);
    }
}

NV_STATUS NV_API_CALL rm_run_rc_callback(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    OBJGPU *pGpu;
    void   *fp;

    /* make sure our timer's not still running when it shouldn't be */
    if (nv == NULL)
        return NV_ERR_GENERIC;

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    if (pGpu == NULL)
        return NV_ERR_GENERIC;

    if (nv->rc_timer_enabled == 0)
        return NV_ERR_GENERIC;

    if (!FULL_GPU_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp,fp);

    osRun1HzCallbacksNow(pGpu);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return NV_OK;
}

static NV_STATUS RmRunNanoTimerCallback(
    OBJGPU *pGpu,
    void *pTmrEvent
)
{
    OBJSYS             *pSys = SYS_GET_INSTANCE();
    POBJTMR             pTmr = GPU_GET_TIMER(pGpu);
    THREAD_STATE_NODE   threadState;
    NV_STATUS status = NV_OK;

    // LOCK: try to acquire GPUs lock
    if ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_TMR)) != NV_OK)
    {
        return status;
    }

    if ((status = osCondAcquireRmSema(pSys->pSema)) != NV_OK)
    {
        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        return status;
    }

    threadStateInitISRAndDeferredIntHandler(&threadState, pGpu,
        THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

    // Call timer event service
    status = tmrEventServiceOSTimerCallback_HAL(pGpu, pTmr, (PTMR_EVENT)pTmrEvent);

    // Out of conflicting thread
    threadStateFreeISRAndDeferredIntHandler(&threadState,
        pGpu, THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

    osReleaseRmSema(pSys->pSema, NULL);
    // UNLOCK: release GPUs lock
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, pGpu);

    return status;
}

NV_STATUS NV_API_CALL rm_run_nano_timer_callback
(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    void *pTmrEvent
)
{
    NV_STATUS status;
    OBJGPU *pGpu = NULL;
    void   *fp;

    if (nv == NULL)
        return NV_ERR_GENERIC;

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    if (pGpu == NULL)
        return NV_ERR_GENERIC;

    if (!FULL_GPU_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp,fp);

    status = RmRunNanoTimerCallback(pGpu, pTmrEvent);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

void NV_API_CALL rm_execute_work_item(
    nvidia_stack_t *sp,
    void *pNvWorkItem
)
{
    void *fp;
    THREAD_STATE_NODE threadState;

    NV_ENTER_RM_RUNTIME(sp, fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    RmExecuteWorkItem(pNvWorkItem);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp, fp);
}

const char* NV_API_CALL rm_get_device_name(
     NvU16       device,
     NvU16       subsystem_vendor,
     NvU16       subsystem_device
)
{
    unsigned int i;
    const char *tmpName = NULL;

    for (i = 0; i < NV_ARRAY_ELEMENTS(sChipsReleased); i++)
    {
        // if the device ID doesn't match, go to the next entry
        if (device != sChipsReleased[i].devID)
        {
            continue;
        }

        // if the entry has 0 for the subsystem IDs, then the device
        // ID match is sufficient, but continue scanning through
        // sChipsReleased[] in case there is a subsystem ID match later
        // in the table
        if (sChipsReleased[i].subSystemVendorID == 0 &&
            sChipsReleased[i].subSystemID == 0)
        {
            tmpName = sChipsReleased[i].name;
            continue;
        }

        if (subsystem_vendor == sChipsReleased[i].subSystemVendorID &&
            subsystem_device == sChipsReleased[i].subSystemID)
        {
            tmpName = sChipsReleased[i].name;
            break;
        }
    }

    return (tmpName != NULL) ? tmpName : "Unknown";
}

NV_STATUS rm_access_registry(
    NvHandle   hClient,
    NvHandle   hObject,
    NvU32      AccessType,
    NvP64      clientDevNodeAddress,
    NvU32      DevNodeLength,
    NvP64      clientParmStrAddress,
    NvU32      ParmStrLength,
    NvP64      clientBinaryDataAddress,
    NvU32      *pBinaryDataLength,
    NvU32      *Data,
    NvU32      *Entry
)
{
    NV_STATUS RmStatus;
    NvBool bReadOnly = (AccessType == NVOS38_ACCESS_TYPE_READ_DWORD) || 
                       (AccessType == NVOS38_ACCESS_TYPE_READ_BINARY);

    // LOCK: acquire API lock
    if ((RmStatus = rmApiLockAcquire(bReadOnly ? RMAPI_LOCK_FLAGS_READ : RMAPI_LOCK_FLAGS_NONE,
                                     RM_LOCK_MODULES_OSAPI)) == NV_OK)
    {
        RmStatus = RmAccessRegistry(hClient,
                                    hObject,
                                    AccessType,
                                    clientDevNodeAddress,
                                    DevNodeLength,
                                    clientParmStrAddress,
                                    ParmStrLength,
                                    clientBinaryDataAddress,
                                    pBinaryDataLength,
                                    Data,
                                    Entry);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return RmStatus;
}

NV_STATUS rm_update_device_mapping_info(
    NvHandle    hClient,
    NvHandle    hDevice,
    NvHandle    hMemory,
    void       *pOldCpuAddress,
    void       *pNewCpuAddress
)
{
    NV_STATUS RmStatus;

    // LOCK: acquire API lock
    if ((RmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU)) == NV_OK)
    {
        RmStatus = RmUpdateDeviceMappingInfo(hClient,
                                             hDevice,
                                             hMemory,
                                             pOldCpuAddress,
                                             pNewCpuAddress);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    return RmStatus;
}

static void rm_is_device_rm_firmware_capable(
    nv_state_t *pNv,
    NvU32       pmcBoot42,
    NvBool     *pbIsFirmwareCapable,
    NvBool     *pbEnableByDefault
)
{
    NvBool bIsFirmwareCapable = NV_FALSE;
    NvBool bEnableByDefault = NV_FALSE;
    NvU16 pciDeviceId = pNv->pci_info.device_id;

    if (NV_IS_SOC_DISPLAY_DEVICE(pNv))
    {
        bIsFirmwareCapable = NV_TRUE;
    }
    else
    {
        bIsFirmwareCapable = gpumgrIsDeviceRmFirmwareCapable(pciDeviceId,
                                                             pmcBoot42,
                                                             &bEnableByDefault);
    }

    if (pbIsFirmwareCapable != NULL)
    {
        *pbIsFirmwareCapable = bIsFirmwareCapable;
    }
    if (pbEnableByDefault != NULL)
    {
        *pbEnableByDefault = bEnableByDefault;
    }
}

static NvBool NV_API_CALL rm_is_legacy_device(
    NvU16       device_id,
    NvU16       subsystem_vendor,
    NvU16       subsystem_device,
    NvBool      print_warning
)
{
    return NV_FALSE;
}

static NvBool NV_API_CALL rm_is_legacy_arch(
    NvU32 pmc_boot_0,
    NvU32 pmc_boot_42
)
{
    NvBool      legacy = NV_FALSE;

    return legacy;
}

NV_STATUS NV_API_CALL rm_is_supported_device(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS   rmStatus;
    OBJSYS     *pSys;
    POBJHALMGR  pHalMgr;
    GPUHWREG   *reg_mapping;
    NvU32       myHalPublicID;
    void       *fp;
    NvU32       pmc_boot_0;
    NvU32       pmc_boot_42;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pSys = SYS_GET_INSTANCE();
    pHalMgr = SYS_GET_HALMGR(pSys);

    reg_mapping = osMapKernelSpace(pNv->regs->cpu_address,
                                   os_page_size,
                                   NV_MEMORY_UNCACHED,
                                   NV_PROTECT_READABLE);

    if (reg_mapping == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to map registers!\n");
        rmStatus = NV_ERR_OPERATING_SYSTEM;
        goto threadfree;
    }
    pmc_boot_0 = NV_PRIV_REG_RD32(reg_mapping, NV_PMC_BOOT_0);
    pmc_boot_42 = NV_PRIV_REG_RD32(reg_mapping, NV_PMC_BOOT_42);

    osUnmapKernelSpace(reg_mapping, os_page_size);

    if ((pmc_boot_0 == 0xFFFFFFFF) && (pmc_boot_42 == 0xFFFFFFFF))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: The NVIDIA GPU %04x:%02x:%02x.%x\n"
            "NVRM: (PCI ID: %04x:%04x) installed in this system has\n"
            "NVRM: fallen off the bus and is not responding to commands.\n",
            pNv->pci_info.domain, pNv->pci_info.bus, pNv->pci_info.slot,
            pNv->pci_info.function, pNv->pci_info.vendor_id,
            pNv->pci_info.device_id);
        rmStatus = NV_ERR_GPU_IS_LOST;
        goto threadfree;
    }

    /*
     * For legacy architectures, rm_is_legacy_arch() prints "legacy" message.
     * We do not want to print "unsupported" message for legacy architectures
     * to avoid confusion. Also, the probe should not continue for legacy
     * architectures. Hence, we set rmStatus to NV_ERR_NOT_SUPPORTED and
     * goto threadfree.
     */
    if (rm_is_legacy_arch(pmc_boot_0, pmc_boot_42))
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
        goto threadfree;
    }

    rmStatus = halmgrGetHalForGpu(pHalMgr, pmc_boot_0, pmc_boot_42, &myHalPublicID);

    if (rmStatus != NV_OK)
    {
        NvBool bIsFirmwareCapable;

        rm_is_device_rm_firmware_capable(pNv,
                                         pmc_boot_42,
                                         &bIsFirmwareCapable,
                                         NULL);
        if (!bIsFirmwareCapable)
        {
            nv_printf(NV_DBG_ERRORS,
               "NVRM: The NVIDIA GPU %04x:%02x:%02x.%x (PCI ID: %04x:%04x)\n"
               "NVRM: installed in this system is not supported by open\n"
               "NVRM: nvidia.ko because it does not include the required GPU\n"
               "NVRM: System Processor (GSP).\n"
               "NVRM: Please see the 'Open Linux Kernel Modules' and 'GSP\n"
               "NVRM: Firmware' sections in the driver README, available on\n"
               "NVRM: the Linux graphics driver download page at\n"
               "NVRM: www.nvidia.com.\n",
               pNv->pci_info.domain, pNv->pci_info.bus, pNv->pci_info.slot,
               pNv->pci_info.function, pNv->pci_info.vendor_id,
               pNv->pci_info.device_id, NV_VERSION_STRING);
            goto threadfree;
        }
        goto print_unsupported;
    }

    goto threadfree;

print_unsupported:
    nv_printf(NV_DBG_ERRORS,
       "NVRM: The NVIDIA GPU %04x:%02x:%02x.%x (PCI ID: %04x:%04x)\n"
       "NVRM: installed in this system is not supported by the\n"
       "NVRM: NVIDIA %s driver release.\n"
       "NVRM: Please see 'Appendix A - Supported NVIDIA GPU Products'\n"
       "NVRM: in this release's README, available on the operating system\n"
       "NVRM: specific graphics driver download page at www.nvidia.com.\n",
       pNv->pci_info.domain, pNv->pci_info.bus, pNv->pci_info.slot,
       pNv->pci_info.function, pNv->pci_info.vendor_id,
       pNv->pci_info.device_id, NV_VERSION_STRING);

threadfree:
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NvBool NV_API_CALL rm_is_supported_pci_device(
    NvU8   pci_class,
    NvU8   pci_subclass,
    NvU16  vendor,
    NvU16  device,
    NvU16  subsystem_vendor,
    NvU16  subsystem_device,
    NvBool print_legacy_warning
)
{
    const NvU16 nv_pci_vendor_id            = 0x10DE;
    const NvU16 nv_pci_id_riva_tnt          = 0x0020;
    const NvU8  nv_pci_class_display        = 0x03;
    const NvU8  nv_pci_subclass_display_vga = 0x00;
    const NvU8  nv_pci_subclass_display_3d  = 0x02;

    if (pci_class != nv_pci_class_display)
    {
        return NV_FALSE;
    }

    if ((pci_subclass != nv_pci_subclass_display_vga) &&
        (pci_subclass != nv_pci_subclass_display_3d))
    {
        return NV_FALSE;
    }

    if (vendor != nv_pci_vendor_id)
    {
        return NV_FALSE;
    }

    if (device < nv_pci_id_riva_tnt)
    {
        return NV_FALSE;
    }

    if (rm_is_legacy_device(
            device,
            subsystem_vendor,
            subsystem_device,
            print_legacy_warning))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

/*
 * Performs the I2C transfers which are related with DP AUX channel
 */
static NV_STATUS RmDpAuxI2CTransfer
(
    nv_state_t  *pNv,
    NvU32       displayId,
    NvU8        addr,
    NvU32       len,
    NvU8       *pData,
    NvBool      bWrite
)
{
    NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *pParams;
    RM_API    *pRmApi   = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS  status;

    if (len > NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_MAX_DATA_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "%s: requested I2C transfer length %u is greater than maximum supported length %u\n",
                  __FUNCTION__, len, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_MAX_DATA_SIZE);
        return NV_ERR_NOT_SUPPORTED;
    }

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pParams, 0, sizeof(*pParams));

    pParams->subDeviceInstance = 0;
    pParams->displayId         = displayId;
    pParams->addr              = addr;
    pParams->size              = len;
    pParams->bWrite            = bWrite;

    if (bWrite)
    {
        portMemCopy(pParams->data, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_MAX_DATA_SIZE,
                    pData, len);
    }

    status = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hDisp,
                             NV0073_CTRL_CMD_DP_AUXCH_I2C_TRANSFER_CTRL,
                             pParams, sizeof(*pParams));

    if ((status == NV_OK) && !bWrite)
    {
        portMemCopy(pData, len, pParams->data, pParams->size);
    }

    portMemFree(pParams);

    return status;
}

/*
 * Performs the I2C transfers which are not related with DP AUX channel
 */
static NV_STATUS RmNonDPAuxI2CTransfer
(
    nv_state_t *pNv,
    NvU8        portId,
    NvU8        type,
    NvU8        addr,
    NvU8        command,
    NvU32       len,
    NvU8       *pData
)
{
    NV402C_CTRL_I2C_TRANSACTION_PARAMS *params;
    RM_API    *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS  rmStatus = NV_OK;

    params = portMemAllocNonPaged(sizeof(*params));
    if (params == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(params, 0, sizeof(*params));

    params->portId = portId;
    // precondition our address (our stack requires this)
    params->deviceAddress = addr << 1;

    switch (type)
    {
        case NV_I2C_CMD_WRITE:
            params->transData.i2cBlockData.bWrite = NV_TRUE;
            /* fall through*/

        case NV_I2C_CMD_READ:
            params->transType = NV402C_CTRL_I2C_TRANSACTION_TYPE_I2C_BLOCK_RW;
            params->transData.i2cBlockData.messageLength = len;
            params->transData.i2cBlockData.pMessage = pData;
            break;

        case NV_I2C_CMD_SMBUS_WRITE:
            params->transData.smbusByteData.bWrite = NV_TRUE;
            /* fall through*/

        case NV_I2C_CMD_SMBUS_READ:
            params->transType = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW;
            params->transData.smbusByteData.message = pData[0];
            params->transData.smbusByteData.registerAddress = command;
            break;

        case NV_I2C_CMD_SMBUS_BLOCK_WRITE:
            params->transData.smbusBlockData.bWrite = NV_TRUE;
            /* fall through*/

        case NV_I2C_CMD_SMBUS_BLOCK_READ:
            params->transType = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BLOCK_RW;
            params->transData.smbusBlockData.registerAddress = command;
            params->transData.smbusBlockData.messageLength = len;
            params->transData.smbusBlockData.pMessage = pData;
            break;

        case NV_I2C_CMD_SMBUS_QUICK_WRITE:
            params->transData.smbusQuickData.bWrite = NV_TRUE;
            /* fall through*/

        case NV_I2C_CMD_SMBUS_QUICK_READ:
            params->transType = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_QUICK_RW;
            break;

        default:
            portMemFree(params);
            return NV_ERR_INVALID_ARGUMENT;
    }

    rmStatus = pRmApi->Control(pRmApi, pNv->rmapi.hClient,
                               pNv->rmapi.hI2C,
                               NV402C_CTRL_CMD_I2C_TRANSACTION,
                               params, sizeof(*params));

    //
    // For NV_I2C_CMD_SMBUS_READ, copy the read data to original
    // data buffer.
    //
    if (rmStatus == NV_OK && type == NV_I2C_CMD_SMBUS_READ)
    {
        pData[0] = params->transData.smbusByteData.message;
    }

    portMemFree(params);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_i2c_transfer(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    void       *pI2cAdapter,
    NvU8        type,
    NvU8        addr,
    NvU8        command,
    NvU32       len,
    NvU8       *pData
)
{
    THREAD_STATE_NODE threadState;
    nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);
    NV_STATUS  rmStatus = NV_OK;
    OBJGPU    *pGpu  = NULL;
    NvBool     unlockApi = NV_FALSE;
    NvBool     unlockGpu = NV_FALSE;
    NvU32      x;
    void      *fp;
    NvU32      numDispId = 0;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (pNvp->flags & NV_INIT_FLAG_PUBLIC_I2C)
    {
        // LOCK: acquire API lock
        if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_I2C)) != NV_OK)
            goto finish;

        unlockApi = NV_TRUE;

        // LOCK: acquire GPUs lock
        if ((rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_I2C)) != NV_OK)
            goto finish;

        unlockGpu = NV_TRUE;
    }

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);

    if (!pGpu)
    {
        rmStatus = NV_ERR_GENERIC;
        goto finish;
    }

    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        if (pNvp->i2c_adapters[x].pOsAdapter == pI2cAdapter)
        {
            break;
        }
    }

    if (x == MAX_I2C_ADAPTERS)
    {
        rmStatus = NV_ERR_GENERIC;
        goto finish;
    }

    for (numDispId = 0; numDispId < MAX_DISP_ID_PER_ADAPTER; numDispId++)
    {
        NvU32 displayId = pNvp->i2c_adapters[x].displayId[numDispId];

        if (displayId == INVALID_DISP_ID)
        {
            continue;
        }

        // Handle i2c-over-DpAux adapters separately from regular i2c adapters
        if (displayId == 0)
        {
            rmStatus = RmNonDPAuxI2CTransfer(pNv, pNvp->i2c_adapters[x].port,
                                             type, addr, command, len, pData);
        }
        else
        {
            if ((type != NV_I2C_CMD_READ) && (type != NV_I2C_CMD_WRITE))
            {
                rmStatus = NV_ERR_NOT_SUPPORTED;
                goto semafinish;
            }

            rmStatus = RmDpAuxI2CTransfer(pNv, displayId, addr, len, pData,
                                          type == NV_I2C_CMD_WRITE);
        }
semafinish:
        if (rmStatus == NV_OK)
        {
            break;
        }
    }

finish:
    if (unlockGpu)
    {
        // UNLOCK: release GPU lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }

    if (unlockApi)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

static void rm_i2c_add_adapter(
    nv_state_t *pNv,
    NvU32       port,
    NvU32       displayId
)
{
    NvU32 y, free;
    nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);
    NvU32 numDispId = 0;

    for (y = 0, free = MAX_I2C_ADAPTERS; y < MAX_I2C_ADAPTERS; y++)
    {
        if (pNvp->i2c_adapters[y].pOsAdapter == NULL)
        {
            // Only find the first free entry, and ignore the rest
            if (free == MAX_I2C_ADAPTERS)
            {
                free = y;
            }
        }
        else if (pNvp->i2c_adapters[y].port == port)
        {
            break;
        }
    }

    if (y < MAX_I2C_ADAPTERS)
    {
        for (numDispId = 0; numDispId < MAX_DISP_ID_PER_ADAPTER; numDispId++)
        {
            if (pNvp->i2c_adapters[y].displayId[numDispId] == INVALID_DISP_ID)
            {
                pNvp->i2c_adapters[y].displayId[numDispId] = displayId;
                break;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO,
                          "%s: adapter  already exists (port=0x%x, displayId=0x%x)\n",
                          __FUNCTION__, port,
                          pNvp->i2c_adapters[y].displayId[numDispId]);
            }
        }

        if (numDispId == MAX_DISP_ID_PER_ADAPTER)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "%s: no more free display Id entries in adapter\n",
                      __FUNCTION__);
        }

        return;
    }

    if (free == MAX_I2C_ADAPTERS)
    {
        NV_PRINTF(LEVEL_ERROR, "%s: no more free adapter entries exist\n",
                  __FUNCTION__);
        return;
    }

    pNvp->i2c_adapters[free].pOsAdapter = nv_i2c_add_adapter(pNv, port);
    pNvp->i2c_adapters[free].port       = port;
    // When port is added, numDispId will be 0.
    pNvp->i2c_adapters[free].displayId[numDispId] = displayId;
}

void RmI2cAddGpuPorts(nv_state_t * pNv)
{
    NvU32      x = 0;
    nv_priv_t *pNvp = NV_GET_NV_PRIV(pNv);
    RM_API    *pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    NvU32      displayMask;
    NV_STATUS  status;
    NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS systemGetSupportedParams = { 0 };

    // Make displayId as Invalid.
    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        NvU32 numDispId;

        for (numDispId = 0; numDispId < MAX_DISP_ID_PER_ADAPTER; numDispId++)
        {
            pNvp->i2c_adapters[x].displayId[numDispId] = INVALID_DISP_ID;
        }
    }

    // First, set up the regular i2c adapters - one per i2c port
    if (pNv->rmapi.hI2C != 0)
    {
        NV402C_CTRL_I2C_GET_PORT_INFO_PARAMS i2cPortInfoParams = { 0 };

        status = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hI2C,
                                 NV402C_CTRL_CMD_I2C_GET_PORT_INFO,
                                 &i2cPortInfoParams, sizeof(i2cPortInfoParams));

        if (status == NV_OK)
        {
            for (x = 0; x < NV_ARRAY_ELEMENTS(i2cPortInfoParams.info); x++)
            {
                //
                // Check if this port is implemented and RM I2C framework has
                // validated this port. Only limited amount of ports can
                // be added to the OS framework.
                //
                if (FLD_TEST_DRF(402C_CTRL, _I2C_GET_PORT_INFO, _IMPLEMENTED,
                                 _YES, i2cPortInfoParams.info[x]) &&
                    FLD_TEST_DRF(402C_CTRL, _I2C_GET_PORT_INFO, _VALID,
                                 _YES, i2cPortInfoParams.info[x]))
                {
                    rm_i2c_add_adapter(pNv, x, 0);
                }
            }
        }
    }

    //
    // Now set up the i2c-over-DpAux adapters - one per DP OD
    //
    // 1. Perform NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS RM control which
    //    will return the mask for all the display ID's.
    // 2. Loop for all the display ID's and do
    //    NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO RM control call. For each
    //    output resource, check for the following requirements:
    //    a. It must be DisplayPort.
    //    b. It must be internal to the GPU (ie, not on the board)
    //    c. It must be directly connected to the physical connector (ie, no DP
    //       1.2 multistream ODs).
    // 3. Perform NV0073_CTRL_CMD_SPECIFIC_GET_I2C_PORTID RM control for
    //    getting the I2C port data.
    //
    // With these restrictions, we should only end up with at most one OD
    // per DP connector.
    //

    if (pNv->rmapi.hDisp == 0)
    {
        return;
    }

    systemGetSupportedParams.subDeviceInstance = 0;
    status = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hDisp,
                             NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED,
                             &systemGetSupportedParams, sizeof(systemGetSupportedParams));

    if (status != NV_OK)
    {
        return;
    }

    for (displayMask = systemGetSupportedParams.displayMask;
         displayMask != 0;
         displayMask &= ~LOWESTBIT(displayMask))
    {
        NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS orInfoParams = { 0 };
        NvU32 displayId = LOWESTBIT(displayMask);

        orInfoParams.subDeviceInstance = 0;
        orInfoParams.displayId         = displayId;

        status = pRmApi->Control(pRmApi, pNv->rmapi.hClient, pNv->rmapi.hDisp,
                                NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO,
                                &orInfoParams, sizeof(orInfoParams));

        if ((status == NV_OK) &&
            (orInfoParams.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) &&
            ((orInfoParams.protocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A) ||
             (orInfoParams.protocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B)) &&
            (orInfoParams.location == NV0073_CTRL_SPECIFIC_OR_LOCATION_CHIP) &&
            (!orInfoParams.bIsDispDynamic))
        {
            NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS i2cPortIdParams = { 0 };

            i2cPortIdParams.subDeviceInstance = 0;
            i2cPortIdParams.displayId = displayId;

            status = pRmApi->Control(pRmApi,
                                     pNv->rmapi.hClient,
                                     pNv->rmapi.hDisp,
                                     NV0073_CTRL_CMD_SPECIFIC_GET_I2C_PORTID,
                                     &i2cPortIdParams,
                                     sizeof(i2cPortIdParams));

            if ((status == NV_OK) &&
                (i2cPortIdParams.ddcPortId != NV0073_CTRL_SPECIFIC_I2C_PORT_NONE))
            {
                rm_i2c_add_adapter(pNv, i2cPortIdParams.ddcPortId - 1, displayId);
            }
        }
    }
}

void NV_API_CALL rm_i2c_remove_adapters(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    //
    // Cycle through all adapter entries, and first remove the adapter
    // from the list from the kernel, then remove the i2c adapter
    // list once that is completed. This should only be used from exit
    // module time. Otherwise it could fail to remove some of the
    // kernel adapters and subsequent transfer requests would result
    // in crashes.
    //
    NvU32 x = 0;
    nv_priv_t  *pNvp = NV_GET_NV_PRIV(pNv);
    NvU32 numDispId;

    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        if (pNvp->i2c_adapters[x].pOsAdapter != NULL)
        {
            nv_i2c_del_adapter(pNv, pNvp->i2c_adapters[x].pOsAdapter);

            pNvp->i2c_adapters[x].pOsAdapter = NULL;
            pNvp->i2c_adapters[x].port       = 0;
            for (numDispId = 0; numDispId < MAX_DISP_ID_PER_ADAPTER; numDispId++)
            {
                pNvp->i2c_adapters[x].displayId[numDispId] = INVALID_DISP_ID;
            }
        }
    }
}

NvBool NV_API_CALL rm_i2c_is_smbus_capable(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    void       *pI2cAdapter
)
{
    THREAD_STATE_NODE threadState;
    nv_priv_t  *pNvp = NV_GET_NV_PRIV(pNv);
    NV_STATUS   rmStatus = NV_OK;
    OBJGPU     *pGpu = NULL;
    NvBool      unlock = NV_FALSE;
    NvU32       x;
    NvBool      ret = NV_FALSE;
    void       *fp;
    NvU32       numDispId = 0;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (pNvp->flags & NV_INIT_FLAG_PUBLIC_I2C)
    {
        // LOCK: acquire API lock
        if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_I2C)) != NV_OK)
            goto semafinish;

        unlock = NV_TRUE;
    }

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);

    if (!pGpu)
    {
        goto semafinish;
    }

    for (x = 0; x < MAX_I2C_ADAPTERS; x++)
    {
        if (pNvp->i2c_adapters[x].pOsAdapter == pI2cAdapter)
        {
            break;
        }
    }

    if (x == MAX_I2C_ADAPTERS)
    {
        goto semafinish;
    }

    // we do not support smbus functions on i2c-over-DPAUX
    for (numDispId = 0; numDispId < MAX_DISP_ID_PER_ADAPTER; numDispId++)
    {
        if (pNvp->i2c_adapters[x].displayId[numDispId] == 0x0)
        {
            ret = NV_TRUE;
        }
    }

semafinish:
    if (unlock)
    {
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return ret;
}

NV_STATUS NV_API_CALL rm_perform_version_check(
    nvidia_stack_t *sp,
    void       *pData,
    NvU32       dataSize
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    rmStatus = RmPerformVersionCheck(pData, dataSize);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_system_event(
    nvidia_stack_t *sp,
    NvU32 event_type,
    NvU32 event_val
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS  rmStatus;
    void      *fp;
    nv_state_t *nv;
    OBJGPU *pGpu = gpumgrGetGpu(0);// Grab the first GPU

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_EVENT)) == NV_OK)
    {
        if (pGpu != NULL)
        {
            nv = NV_GET_NV_STATE(pGpu);
            if ((rmStatus = os_ref_dynamic_power(nv, NV_DYNAMIC_PM_FINE)) ==
                                                                         NV_OK)
            {
                // LOCK: acquire GPU lock
                if ((rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_EVENT)) ==
                                                                         NV_OK)
                {
                    rmStatus = RmSystemEvent(nv, event_type, event_val);

                    // UNLOCK: release GPU lock
                    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
                }
                os_unref_dynamic_power(nv, NV_DYNAMIC_PM_FINE);
            }
            // UNLOCK: release API lock
            rmApiLockRelease();
        }
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_dma_map_pages(
    nvidia_stack_t  *sp,
    nv_dma_device_t *peer,
    NvU8            *pGpuUuid,
    NvU32            pageSize,
    NvU32            pageCount,
    NvU64           *pDmaAddresses,
    void           **ppPriv
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    if (ppPriv == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppPriv = NULL;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        OBJGPU *pGpu = gpumgrGetGpuFromUuid(pGpuUuid,
            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));
        if (pGpu == NULL)
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
        }
        else
        {
            NvU32 i;

            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
            {
                NV_ASSERT(pageSize == os_page_size);

                rmStatus = nv_dma_map_alloc(peer, pageCount, pDmaAddresses,
                                            NV_FALSE, ppPriv);
            }
            else
            {
                nv_state_t *nv = NV_GET_NV_STATE(pGpu);
                for (i = 0; i < pageCount; i++)
                {
                    // Peer mappings through this API are always via BAR1
                    rmStatus = nv_dma_map_peer(peer, nv->dma_dev, 0x1,
                                               pageSize / os_page_size,
                                               &pDmaAddresses[i]);
                    if ((rmStatus != NV_OK) && (i > 0))
                    {
                        NvU32 j;
                        for (j = i - 1; j < pageCount; j--)
                        {
                            nv_dma_unmap_peer(peer, pageSize / os_page_size,
                                              pDmaAddresses[j]);
                        }
                    }
                }
            }
        }

        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_get_gpu_info(
    nvidia_stack_t *sp,
    NvU64           gpuVirtualAddress,
    NvU64           length,
    NvU8          **ppGpuUuid,
    void          **ppGpuInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P);
    if (rmStatus == NV_OK)
    {
        OBJGPU *pGpu;
        rmStatus = RmP2PGetGpuByAddress(gpuVirtualAddress,
                                        length,
                                        &pGpu);
        if (rmStatus == NV_OK)
        {
            nv_state_t *nv = NV_GET_NV_STATE(pGpu);
            const NvU8 *pGid;

            pGid = RmGetGpuUuidRaw(nv);
            if (pGid == NULL)
            {
                rmStatus = NV_ERR_GPU_UUID_NOT_FOUND;
            }
            else
            {
                rmStatus = os_alloc_mem((void **)ppGpuUuid, GPU_UUID_LEN);
                if (rmStatus == NV_OK)
                {
                    os_mem_copy(*ppGpuUuid, pGid, GPU_UUID_LEN);
                }
            }

            *ppGpuInfo = (void *) pGpu;
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_get_pages_persistent(
    nvidia_stack_t *sp,
    NvU64           gpuVirtualAddress,
    NvU64           length,
    void          **p2pObject,
    NvU64          *pPhysicalAddresses,
    NvU32          *pEntries,
    void           *pPlatformData,
    void           *pGpuInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        rmStatus = RmP2PGetPagesPersistent(gpuVirtualAddress,
                                           length,
                                           p2pObject,
                                           pPhysicalAddresses,
                                           pEntries,
                                           pPlatformData,
                                           pGpuInfo);
        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_get_pages(
    nvidia_stack_t *sp,
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       gpuVirtualAddress,
    NvU64       length,
    NvU64      *pPhysicalAddresses,
    NvU32      *pWreqMbH,
    NvU32      *pRreqMbH,
    NvU32      *pEntries,
    NvU8      **ppGpuUuid,
    void       *pPlatformData
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        OBJGPU *pGpu;
        rmStatus = RmP2PGetPagesWithoutCallbackRegistration(p2pToken,
                                                            vaSpaceToken,
                                                            gpuVirtualAddress,
                                                            length,
                                                            pPhysicalAddresses,
                                                            pWreqMbH,
                                                            pRreqMbH,
                                                            pEntries,
                                                            &pGpu,
                                                            pPlatformData);
        if (rmStatus == NV_OK)
        {
            nv_state_t *nv = NV_GET_NV_STATE(pGpu);
            const NvU8 *pGid;

            pGid = RmGetGpuUuidRaw(nv);
            if (pGid == NULL)
            {
                NV_ASSERT_OK(RmP2PPutPages(p2pToken, vaSpaceToken,
                                               gpuVirtualAddress,
                                               pPlatformData));
                rmStatus = NV_ERR_GENERIC;
            }
            else
            {
                rmStatus = os_alloc_mem((void **)ppGpuUuid, GPU_UUID_LEN);
                if (rmStatus == NV_OK)
                    os_mem_copy(*ppGpuUuid, pGid, GPU_UUID_LEN);
            }
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_register_callback(
    nvidia_stack_t *sp,
    NvU64       p2pToken,
    NvU64       gpuVirtualAddress,
    NvU64       length,
    void       *pPlatformData,
    void      (*pFreeCallback)(void *pData),
    void       *pData
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        rmStatus = RmP2PRegisterCallback(p2pToken, gpuVirtualAddress, length,
                                         pPlatformData, pFreeCallback, pData);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_put_pages_persistent(
    nvidia_stack_t *sp,
    void           *p2pObject,
    void           *pKey
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        rmStatus = RmP2PPutPagesPersistent(p2pObject, pKey);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_p2p_put_pages(
    nvidia_stack_t *sp,
    NvU64       p2pToken,
    NvU32       vaSpaceToken,
    NvU64       gpuVirtualAddress,
    void       *pKey
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    if ((rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_P2P)) == NV_OK)
    {
        rmStatus = RmP2PPutPages(p2pToken,
                                 vaSpaceToken,
                                 gpuVirtualAddress,
                                 pKey);

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

char* NV_API_CALL rm_get_gpu_uuid(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    NV_STATUS rmStatus;
    const NvU8 *pGid;
    OBJGPU *pGpu;
    char *pGidString;

    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // Allocate space for the ASCII string
    rmStatus = os_alloc_mem((void **)&pGidString, GPU_UUID_ASCII_LEN);
    if (rmStatus != NV_OK)
    {
        pGidString = NULL;
        goto done;
    }

    // Get the raw UUID; note the pGid is cached, so we do not need to free it
    pGid = RmGetGpuUuidRaw(nv);

    if (pGid != NULL)
    {
        // Convert the raw UUID to ASCII
        rmStatus = RmGpuUuidRawToString(pGid, pGidString, GPU_UUID_ASCII_LEN);
        if (rmStatus != NV_OK)
        {
            os_free_mem(pGidString);
            pGidString = NULL;
        }
    }
    else
    {
        const char *pTmpString;

        // No raw GID, but we still return a string
        pGpu = NV_GET_NV_PRIV_PGPU(nv);

        if (rmStatus == NV_ERR_NOT_SUPPORTED && pGpu != NULL &&
            pGpu->getProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED))
            pTmpString = "N/A";
        else
            pTmpString = "GPU-???????\?-???\?-???\?-???\?-????????????";

        portStringCopy(pGidString, GPU_UUID_ASCII_LEN, pTmpString,
                       portStringLength(pTmpString) + 1);
    }

done:
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return pGidString;
}

//
// This function will return the UUID in the binary format
//
const NvU8 * NV_API_CALL rm_get_gpu_uuid_raw(
    nvidia_stack_t *sp,
    nv_state_t *nv)
{
    THREAD_STATE_NODE threadState;
    void *fp;
    const NvU8 *pGid;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGid = RmGetGpuUuidRaw(nv);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return pGid;
}

static void rm_set_firmware_logs(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    NV_STATUS status;
    NvU32 data;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    status = RmReadRegistryDword(nv, NV_REG_ENABLE_GPU_FIRMWARE_LOGS, &data);
    if (status == NV_OK)
    {
        if ((data == NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE)
#if defined(DEBUG) || defined(DEVELOP)         
            || (data == NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE_ON_DEBUG)
#endif
           )
        {
            nv->enable_firmware_logs = NV_TRUE;
        }
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

void NV_API_CALL rm_set_rm_firmware_requested(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    nv->request_firmware = NV_TRUE;
    nv->allow_fallback_to_monolithic_rm = NV_FALSE;

    // Check if we want firmware logs
    if (nv->request_firmware)
        rm_set_firmware_logs(sp, nv);
}

//
// This function will be called by nv_procfs_read_gpu_info().
// nv_procfs_read_gpu_info() will not print the 'GPU Firmware:' field at
// all if the 'version' string is empty.
//
// If GSP is enabled (firmware was requested), this function needs to return
// the firmware version or "NA" in case of any errors.
//
// If GSP is not enabled (firmware was not requested), this function needs to
// return the empty string, regardless of error cases.
//
void NV_API_CALL rm_get_firmware_version(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    char *version,
    NvLength version_length
)
{
    NV2080_CTRL_GSP_GET_FEATURES_PARAMS params = { 0 };
    RM_API            *pRmApi;
    THREAD_STATE_NODE  threadState;
    NV_STATUS          rmStatus = NV_OK;
    void              *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pRmApi = RmUnixRmApiPrologue(nv, &threadState, RM_LOCK_MODULES_GPU);
    if (pRmApi != NULL)
    {
        rmStatus = pRmApi->Control(pRmApi,
                                   nv->rmapi.hClient,
                                   nv->rmapi.hSubDevice,
                                   NV2080_CTRL_CMD_GSP_GET_FEATURES,
                                   &params,
                                   sizeof(params));

        RmUnixRmApiEpilogue(nv, &threadState);
    }
    else
    {
        rmStatus = NV_ERR_INVALID_STATE;
    }

    if (rmStatus != NV_OK)
    {
        if (RMCFG_FEATURE_GSP_CLIENT_RM && nv->request_firmware)
        {
            const char *pTmpString = "N/A";
            portStringCopy(version, version_length, pTmpString, portStringLength(pTmpString) + 1);
        }
        NV_PRINTF(LEVEL_INFO,
                  "%s: Failed to query gpu build versions, status=0x%x\n",
                  __FUNCTION__,
                  rmStatus);
        goto finish;
    }
    portMemCopy(version, version_length, params.firmwareVersion, sizeof(params.firmwareVersion));

finish:
    NV_EXIT_RM_RUNTIME(sp,fp);
}

//
// disable GPU SW state persistence
//

void NV_API_CALL rm_disable_gpu_state_persistence(nvidia_stack_t *sp, nv_state_t *nv)
{
    THREAD_STATE_NODE threadState;
    OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_PERSISTENT_SW_STATE, NV_FALSE);
    osModifyGpuSwStatePersistence(pGpu->pOsGpuInfo, NV_FALSE);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_log_gpu_crash(
    nv_stack_t *sp,
    nv_state_t *nv
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS status;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if ((status = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DIAG)) == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

        if ((pGpu != NULL) &&
           ((status = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DIAG)) == NV_OK))
        {
            status = RmLogGpuCrash(pGpu);

            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        }
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

void  NV_API_CALL  rm_kernel_rmapi_op(nvidia_stack_t *sp, void *ops_cmd)
{
    nvidia_kernel_rmapi_ops_t *ops = ops_cmd;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    switch (ops->op)
    {
        case NV01_FREE:
            Nv01FreeKernel(&ops->params.free);
            break;

        case NV01_ALLOC_MEMORY:
            Nv01AllocMemoryKernel(&ops->params.allocMemory64);
            break;

        case NV04_ALLOC:
            Nv04AllocKernel(&ops->params.alloc);
            break;

        case NV04_VID_HEAP_CONTROL:
            Nv04VidHeapControlKernel(ops->params.pVidHeapControl);
            break;

        case NV04_MAP_MEMORY:
            Nv04MapMemoryKernel(&ops->params.mapMemory);
            break;

        case NV04_UNMAP_MEMORY:
            Nv04UnmapMemoryKernel(&ops->params.unmapMemory);
            break;

        case NV04_ALLOC_CONTEXT_DMA:
            Nv04AllocContextDmaKernel(&ops->params.allocContextDma2);
            break;

        case NV04_MAP_MEMORY_DMA:
            Nv04MapMemoryDmaKernel(&ops->params.mapMemoryDma);
            break;

        case NV04_UNMAP_MEMORY_DMA:
            Nv04UnmapMemoryDmaKernel(&ops->params.unmapMemoryDma);
            break;

        case NV04_BIND_CONTEXT_DMA:
            Nv04BindContextDmaKernel(&ops->params.bindContextDma);
            break;

        case NV04_CONTROL:
            Nv04ControlKernel(&ops->params.control);
            break;

        case NV04_DUP_OBJECT:
            Nv04DupObjectKernel(&ops->params.dupObject);
            break;

        case NV04_SHARE:
            Nv04ShareKernel(&ops->params.share);
            break;

        case NV04_ADD_VBLANK_CALLBACK:
            Nv04AddVblankCallbackKernel(&ops->params.addVblankCallback);
            break;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);
}

//
// ACPI method (NVIF/_DSM/WMMX/MXM*/etc.) initialization
//
void RmInitAcpiMethods(OBJOS *pOS, OBJSYS *pSys, OBJGPU *pGpu)
{
    NvU32 handlesPresent;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_NVIF_INIT_DONE))
        return;

    nv_acpi_methods_init(&handlesPresent);

}

//
// ACPI method (NVIF/_DSM/WMMX/MXM*/etc.) teardown
//
void RmUnInitAcpiMethods(OBJSYS *pSys)
{
    pSys->setProperty(pSys, PDB_PROP_SYS_NVIF_INIT_DONE, NV_FALSE);

    nv_acpi_methods_uninit();
}

//
// Converts an array of OS page address to an array of RM page addresses.This
// assumes that:
//   (1) The pteArray is at least pageCount entries large,
//   (2) The pageCount is given in RM pages, and
//   (3) The OS page entries start at index 0.
//
void RmInflateOsToRmPageArray(RmPhysAddr *pteArray, NvU64 pageCount)
{
    NvUPtr osPageIdx, osPageOffset;
    NvU64 i;

    //
    // We can do the translation in place by moving backwards, since there
    // will always be more RM pages than OS pages
    //
    for (i = pageCount - 1; i != NV_U64_MAX; i--)
    {
        osPageIdx = i >> NV_RM_TO_OS_PAGE_SHIFT;
        osPageOffset = (i & ((1 << NV_RM_TO_OS_PAGE_SHIFT) - 1)) *
                NV_RM_PAGE_SIZE;
        pteArray[i] = pteArray[osPageIdx] + osPageOffset;
    }
}

void RmDeflateRmToOsPageArray(RmPhysAddr *pteArray, NvU64 pageCount)
{
    NvU64 i;

    for (i = 0; i < NV_RM_PAGES_TO_OS_PAGES(pageCount); i++)
    {
        pteArray[i] = pteArray[(i << NV_RM_TO_OS_PAGE_SHIFT)];
    }

    // Zero out the rest of the addresses, which are now invalid
    portMemSet(pteArray + i, 0, sizeof(*pteArray) * (pageCount - i));
}

NvBool NV_API_CALL
rm_get_device_remove_flag
(
    nvidia_stack_t * sp,
    NvU32 gpu_id
)
{
    THREAD_STATE_NODE threadState;
    void *fp;
    NvBool bRemove;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (gpumgrQueryGpuDrainState(gpu_id, NULL, &bRemove) != NV_OK)
    {
        bRemove = NV_FALSE;
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return bRemove;
}

NvBool NV_API_CALL
rm_gpu_need_4k_page_isolation
(
    nv_state_t *nv
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);

    return nvp->b_4k_page_isolation_required;
}

NV_STATUS NV_API_CALL rm_get_gpu_numa_info(
    nvidia_stack_t *sp,
    nv_state_t *nv,
    NvS32 *pNid,
    NvU64 *pNumaMemAddr,
    NvU64 *pNumaMemSize,
    NvU64 *pOfflineAddresses,
    NvU32 *pOfflineAddressesCount
)
{
    NV2080_CTRL_FB_GET_NUMA_INFO_PARAMS *pParams;
    RM_API             *pRmApi;
    THREAD_STATE_NODE   threadState;
    void               *fp;
    NV_STATUS           status = NV_OK;

    if ((pNid == NULL) || (pNumaMemAddr == NULL) || (pNumaMemSize == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pOfflineAddressesCount != NULL) &&
        ((pOfflineAddresses == NULL) ||
         (*pOfflineAddressesCount > NV_ARRAY_ELEMENTS(pParams->numaOfflineAddresses))))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ENTER_RM_RUNTIME(sp,fp);

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        NV_EXIT_RM_RUNTIME(sp,fp);
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pParams, 0, sizeof(*pParams));

    if (pOfflineAddressesCount != NULL)
    {
        pParams->numaOfflineAddressesCount = *pOfflineAddressesCount;
    }

    pRmApi = RmUnixRmApiPrologue(nv, &threadState, RM_LOCK_MODULES_MEM);
    if (pRmApi == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto finish;
    }

    status = pRmApi->Control(pRmApi, nv->rmapi.hClient, nv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_FB_GET_NUMA_INFO,
                             pParams, sizeof(*pParams));

    RmUnixRmApiEpilogue(nv, &threadState);

    if (status == NV_OK)
    {
        NvU32 i;

        *pNid = pParams->numaNodeId;
        *pNumaMemAddr = pParams->numaMemAddr;
        *pNumaMemSize = pParams->numaMemSize;
        *pOfflineAddressesCount = pParams->numaOfflineAddressesCount;

        for (i = 0; i < pParams->numaOfflineAddressesCount; i++)
        {
            pOfflineAddresses[i] = pParams->numaOfflineAddresses[i];
        }
    }

finish:
    portMemFree(pParams);

    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

NV_STATUS NV_API_CALL rm_gpu_numa_online(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS params = { 0 };
    RM_API            *pRmApi;
    THREAD_STATE_NODE  threadState;
    void              *fp;
    NV_STATUS          status = NV_OK;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pRmApi = RmUnixRmApiPrologue(nv, &threadState, RM_LOCK_MODULES_MEM);
    if (pRmApi == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto finish;
    }

    params.bOnline = NV_TRUE;

    status = pRmApi->Control(pRmApi, nv->rmapi.hClient, nv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_FB_UPDATE_NUMA_STATUS,
                             &params, sizeof(params));

    RmUnixRmApiEpilogue(nv, &threadState);

finish:
    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}


NV_STATUS NV_API_CALL rm_gpu_numa_offline(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    NV2080_CTRL_FB_UPDATE_NUMA_STATUS_PARAMS params = { 0 };
    RM_API            *pRmApi;
    THREAD_STATE_NODE  threadState;
    void              *fp;
    NV_STATUS          status = NV_OK;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pRmApi = RmUnixRmApiPrologue(nv, &threadState, RM_LOCK_MODULES_MEM);
    if (pRmApi == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto finish;
    }

    params.bOnline = NV_FALSE;

    status = pRmApi->Control(pRmApi, nv->rmapi.hClient,
                             nv->rmapi.hSubDevice,
                             NV2080_CTRL_CMD_FB_UPDATE_NUMA_STATUS,
                             &params, sizeof(params));

    RmUnixRmApiEpilogue(nv, &threadState);

finish:
    NV_EXIT_RM_RUNTIME(sp,fp);

    return status;
}

//
// A device is considered "sequestered" if it has drain state enabled for it.
// The kernel interface layer can use this to check the drain state of a device
// in paths outside of initialization, e.g., when clients attempt to reference
// count the device.
//
NvBool NV_API_CALL rm_is_device_sequestered(
    nvidia_stack_t *sp,
    nv_state_t *pNv
)
{
    THREAD_STATE_NODE threadState;
    void *fp;
    NvBool bDrain = NV_FALSE;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    if (rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU) == NV_OK)
    {
        //
        // If gpumgrQueryGpuDrainState succeeds, bDrain will be set as needed.
        // If gpumgrQueryGpuDrainState fails, bDrain will stay false; we assume
        // that if core RM can't tell us the drain state, it must not be
        // attached and the "sequestered" question is not relevant.
        //
        (void) gpumgrQueryGpuDrainState(pNv->gpu_id, &bDrain, NULL);

        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return bDrain;
}

void NV_API_CALL rm_check_for_gpu_surprise_removal(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    THREAD_STATE_NODE threadState;
    void *fp;
    NV_STATUS rmStatus;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock.
    if ((rmStatus = rmApiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_GPU)) == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

        if ((rmStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GPU)) == NV_OK)
        {
            osHandleGpuLost(pGpu);
            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }

        // UNLOCK: release api lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_set_external_kernel_client_count(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    NvBool bIncr
)
{
    THREAD_STATE_NODE threadState;
    void *fp;
    OBJGPU *pGpu;
    NV_STATUS rmStatus = NV_OK;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu = NV_GET_NV_PRIV_PGPU(pNv);

    if (pGpu != NULL)
    {
        rmStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                           RM_LOCK_MODULES_GPU);
        if (rmStatus == NV_OK)
        {
            rmStatus = gpuSetExternalKernelClientCount(pGpu, bIncr);
            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NvBool rm_get_uefi_console_status(
    nv_state_t *nv
)
{
    NvU16 fbWidth, fbHeight, fbDepth, fbPitch;
    NvU64 fbSize;
    NvU64 fbBaseAddress = 0;
    NvBool bConsoleDevice = NV_FALSE;

    // os_get_screen_info() will return dimensions and an address for
    // any fbdev driver (e.g., efifb, vesafb, etc).  To find if this is a
    // UEFI console check the fbBaseAddress: if it was set up by the EFI GOP
    // driver, it will point into BAR1 (FB); if it was set up by the VBIOS,
    // it will point to BAR2 + 16MB.
    os_get_screen_info(&fbBaseAddress, &fbWidth, &fbHeight, &fbDepth, &fbPitch,
                       nv->bars[NV_GPU_BAR_INDEX_FB].cpu_address,
                       nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address + 0x1000000);

    fbSize = fbHeight * fbPitch;

    bConsoleDevice = (fbSize != 0);

    return bConsoleDevice;
}

NvU64 rm_get_uefi_console_size(
    nv_state_t *nv,
    NvU64      *pFbBaseAddress
)
{
    NvU16 fbWidth, fbHeight, fbDepth, fbPitch;
    NvU64 fbSize;

    fbSize = fbWidth = fbHeight = fbDepth = fbPitch = 0;

    // os_get_screen_info() will return dimensions and an address for
    // any fbdev driver (e.g., efifb, vesafb, etc).  To find if this is a
    // UEFI console check the fbBaseAddress: if it was set up by the EFI GOP
    // driver, it will point into BAR1 (FB); if it was set up by the VBIOS,
    // it will point to BAR2 + 16MB.
    os_get_screen_info(pFbBaseAddress, &fbWidth, &fbHeight, &fbDepth, &fbPitch,
                       nv->bars[NV_GPU_BAR_INDEX_FB].cpu_address,
                       nv->bars[NV_GPU_BAR_INDEX_IMEM].cpu_address + 0x1000000);

    fbSize = fbHeight * fbPitch;

    return fbSize;
}

/*
 * IOMMU needs to be present on the server to support SR-IOV vGPU, unless
 * we have SR-IOV enabled for remote GPU.
 */

NvBool NV_API_CALL rm_is_iommu_needed_for_sriov(
    nvidia_stack_t *sp,
    nv_state_t * nv
)
{
    OBJGPU *pGpu;
    NvU32 data;
    NvBool ret = NV_TRUE;
    void       *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_REMOTE_GPU, &data) == NV_OK)
    {
        if (data == NV_REG_STR_RM_REMOTE_GPU_ENABLE)
            ret = NV_FALSE;
    }

    NV_EXIT_RM_RUNTIME(sp,fp);

    return ret;
}

NvBool NV_API_CALL rm_disable_iomap_wc(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    return pSys->pCl->getProperty(pSys, PDB_PROP_CL_DISABLE_IOMAP_WC) == NV_TRUE;
}

//
// Verifies the handle, offset and size and dups hMemory.
// Must be called with API lock and GPU lock held.
//
NV_STATUS NV_API_CALL rm_dma_buf_dup_mem_handle(
    nvidia_stack_t  *sp,
    nv_state_t      *nv,
    NvHandle         hSrcClient,
    NvHandle         hDstClient,
    NvHandle         hDevice,
    NvHandle         hSubdevice,
    void            *pGpuInstanceInfo,
    NvHandle         hMemory,
    NvU64            offset,
    NvU64            size,
    NvHandle        *phMemoryDuped
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    OBJGPU *pGpu;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);

    NV_ASSERT(rmApiLockIsOwner());

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    rmStatus = RmDmabufVerifyMemHandle(pGpu, hSrcClient, hMemory,
                                       offset, size, pGpuInstanceInfo);
    if (rmStatus == NV_OK)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NvHandle hMemoryDuped = 0;

        rmStatus = pRmApi->DupObject(pRmApi,
                                     hDstClient,
                                     hDevice,
                                     &hMemoryDuped,
                                     hSrcClient,
                                     hMemory,
                                     0);
        if (rmStatus == NV_OK)
        {
            *phMemoryDuped = hMemoryDuped;
        }
        else if (rmStatus == NV_ERR_INVALID_OBJECT_PARENT)
        {
            hMemoryDuped = 0;

            // If duping under Device fails, try duping under Subdevice
            rmStatus = pRmApi->DupObject(pRmApi,
                                         hDstClient,
                                         hSubdevice,
                                         &hMemoryDuped,
                                         hSrcClient,
                                         hMemory,
                                         0);
            if (rmStatus == NV_OK)
            {
                *phMemoryDuped = hMemoryDuped;
            }
        }
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

//
// Frees dup'd hMemory.
// Must be called with API lock and GPU lock held.
//
void NV_API_CALL rm_dma_buf_undup_mem_handle(
    nvidia_stack_t  *sp,
    nv_state_t      *nv,
    NvHandle         hClient,
    NvHandle         hMemory
)
{
    THREAD_STATE_NODE threadState;
    RM_API *pRmApi;
    OBJGPU *pGpu;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);

    NV_ASSERT(rmApiLockIsOwner());

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    pRmApi->Free(pRmApi, hClient, hMemory);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

//
// Maps a handle to BAR1.
// Must be called with API lock and GPU lock held.
//
NV_STATUS NV_API_CALL rm_dma_buf_map_mem_handle(
    nvidia_stack_t  *sp,
    nv_state_t      *nv,
    NvHandle         hClient,
    NvHandle         hMemory,
    NvU64            offset,
    NvU64            size,
    NvU64           *pBar1Va
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    OBJGPU *pGpu;
    KernelBus *pKernelBus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT(rmApiLockIsOwner());

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    rmStatus = kbusMapFbApertureByHandle(pGpu, pKernelBus, hClient,
                                         hMemory, offset, size, pBar1Va);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

//
// Unmaps a handle from BAR1.
// Must be called with API lock and GPU lock held.
//
NV_STATUS NV_API_CALL rm_dma_buf_unmap_mem_handle(
    nvidia_stack_t  *sp,
    nv_state_t      *nv,
    NvHandle         hClient,
    NvHandle         hMemory,
    NvU64            size,
    NvU64            bar1Va
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    OBJGPU *pGpu;
    KernelBus *pKernelBus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pGpu = NV_GET_NV_PRIV_PGPU(nv);
    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    NV_ASSERT(rmApiLockIsOwner());

    NV_ASSERT(rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)));

    rmStatus = kbusUnmapFbApertureByHandle(pGpu, pKernelBus, hClient,
                                           hMemory, bar1Va);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_dma_buf_get_client_and_device(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvHandle        hClient,
    NvHandle       *phClient,
    NvHandle       *phDevice,
    NvHandle       *phSubdevice,
    void          **ppGpuInstanceInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);
    if (rmStatus == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

        rmStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);
        if (rmStatus == NV_OK)
        {
            rmStatus = RmDmabufGetClientAndDevice(pGpu, hClient, phClient, phDevice,
                                                  phSubdevice, ppGpuInstanceInfo);

            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}

void NV_API_CALL rm_dma_buf_put_client_and_device(
    nvidia_stack_t *sp,
    nv_state_t     *nv,
    NvHandle        hClient,
    NvHandle        hDevice,
    NvHandle        hSubdevice,
    void           *pGpuInstanceInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    // LOCK: acquire API lock
    rmStatus = rmApiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);
    if (rmStatus == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(nv);

        rmStatus = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);
        if (rmStatus == NV_OK)
        {
            RmDmabufPutClientAndDevice(pGpu, hClient, hDevice, hSubdevice,
                                       pGpuInstanceInfo);

            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }

        // UNLOCK: release API lock
        rmApiLockRelease();
    }
    NV_ASSERT_OK(rmStatus);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}
