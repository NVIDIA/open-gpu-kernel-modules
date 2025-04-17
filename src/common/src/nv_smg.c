/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#if defined(NV_SMG_IN_NVKMS)
#  include "nvkms-utils.h"
#else
#    include <pthread.h>
#  include <stdlib.h>
#  include <string.h>
#endif


#include "nv_smg.h"
#include "nvmisc.h"

#include "class/cl0000.h"
#include "class/cl0080.h"
#include "class/cl2080.h"
#include "class/clc637.h"
#include "class/clc638.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrlc637.h"
#include "ctrl/ctrlc638.h"

/*
 * Since we will be compiled both into NVKMS and userspace drivers we do a
 * little renaming here to make the rest of the code target-agnostic.
 */
#if defined(NV_SMG_IN_NVKMS)
#define smg_memcmp nvkms_memcmp
#define smg_memcpy nvkms_memcpy
#define smg_memset nvkms_memset
#define smg_strcmp nvkms_strcmp
#else
#define smg_memcmp memcmp
#define smg_memcpy memcpy
#define smg_memset memset
#define smg_strcmp strcmp
#endif

/*
 * Stack is limited in kernelspace so in NVKMS builds we allocate our
 * workspace from heap. RM parameters are huge so this is needed in several
 * of the main RM-heavy functions. These macros hide the unavoidable
 * boilerplate and make the actual functions hopefully read cleaner.
 */
#if defined(NV_SMG_IN_NVKMS)
#define ENTER_WORKSPACE(ws) \
    (ws) = (struct workspace *)nvAlloc(sizeof(*(ws)));  \
    if (!(ws)) {                                        \
        return NV_FALSE;                                \
    }
#define EXIT_WORKSPACE_AND_RETURN(ws, retval)   \
    nvFree((ws));                               \
    return (retval)

#else /* !NV_SMG_IN_NVKMS */

#define ENTER_WORKSPACE(ws)                     \
    struct workspace _ws;                       \
    (ws) = &_ws
#define EXIT_WORKSPACE_AND_RETURN(ws, retval)   \
    return (retval)
#endif

/*
 * This is a conservative guess. Theoretically, looking at the current
 * templates, each GPU can be split into at least 8 partitions. We're also
 * limited by the number of GRCE's: each partition needs one so
 * realistically the maximum is like two partitions per GPU. Then,
 * NV_MAX_DEVICES itself is 32 which is also a rather high number. I'm not
 * quite sure if there exists hardware that supports 32 individual GPU
 * cards. We'll just arbitrarily set the MIG device array size to the same
 * 32: it likely overshoots conservatively but if we ever fill them all
 * ListPartitions() will fail and we can bump the size up.
 */
#define NV_MAX_MIG_DEVICES 32

static struct nvGlobalMigMappingRec {
    NvBool initialized;

    NvU32 deviceCount;
    nvMIGDeviceDescription deviceList[NV_MAX_MIG_DEVICES];
} globalMigMapping;

/* Having two macros establishes a meaningful vocabulary. */
#define MIG_DEVICE_ID_TO_INDEX(id) ((id) ^ 0xffffffff)
#define MIG_DEVICE_INDEX_TO_ID(ix) ((ix) ^ 0xffffffff)

static NvBool InitializeGlobalMapping(nvRMContextPtr rmctx);
static NvBool ListPartitions(nvRMContextPtr rmctx, struct nvGlobalMigMappingRec *mappings);
#if !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA)
static NvBool GetGraphicsPartitionUUIDForDevice(nvRMContextPtr rmctx, nvMIGDeviceDescription *migDev);
#endif
static NvU32 DeviceInMigMode (nvRMContextPtr rmctx, NvU32 hSubDevice, NvBool *inMigMode);
static NvBool GpuHasSMGPartitions (NvU32 gpuId);

/*
 * Set up SMG for the given subdevice. Handles partition allocation and
 * selection. Returns true if this subdevice is equipped to do graphics.
 * That is, either when not in MIG-mode at all, or when MIG-mode is enabled
 * and we were able to successfully set up an SMG partition for doing
 * graphics on this subdevice. Returns false otherwise when any kind of
 * unrecoverable error condition that means broken graphics is encountered.
 */
NvBool nvSMGSubscribeSubDevToPartition(nvRMContextPtr rmctx,
                                       NvU32 subDevHandle,
                                       MIGDeviceId migDevice,
                                       NvU32 hGpuInstSubscription,
                                       NvU32 hComputeInstSubscription)
{
    /*
     * These RM parameters can be huge: allocate them from the heap to keep
     * kernel stack usage low. For userspace, allocate on stack.
     */
    struct workspace {
        NV2080_CTRL_GPU_GET_ID_PARAMS getIdParams;
        NVC637_ALLOCATION_PARAMETERS allocParams;
        NVC638_ALLOCATION_PARAMETERS execAllocParams;
        NVC638_CTRL_GET_UUID_PARAMS getUuidParams;
    } *ws;
    const nvMIGDeviceDescription *desc = NULL;
    NvU32 gpuId = NV0000_CTRL_GPU_INVALID_ID;
    NvBool inMigMode;
    NvU32 res;

    /* No N-way SMG with partition selection until GB20X. */

    /* First, make sure we've created the identities for all MIG devices. */
    if (!InitializeGlobalMapping(rmctx)) {
        return NV_FALSE;
    }

    ENTER_WORKSPACE(ws);
    smg_memset(ws, 0, sizeof(*ws));

    /* Read gpuID for reference and cross-checking. */
    res = rmctx->control(rmctx,
                         rmctx->clientHandle,
                         subDevHandle,
                         NV2080_CTRL_CMD_GPU_GET_ID,
                         &ws->getIdParams,
                         sizeof(ws->getIdParams));
    if (res != NV_OK) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }
    gpuId = ws->getIdParams.gpuId;

    /*
     * Before anything, check explicitly whether the GPU is in MIG mode.
     */
    if (DeviceInMigMode(rmctx, subDevHandle, &inMigMode) != NV_OK) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /*
     * If not in MIG mode then just return true to indicate we're able to do
     * graphics. No MIG mode, no partitions, no subscriptions.
     */
    if (!inMigMode) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_TRUE);
    }

    /*
     * However, if we're in MIG mode but there are no (graphics) partitions
     * this means we won't be able to do graphics. Bail out with false.
     */
    if (!GpuHasSMGPartitions(gpuId)) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /* Get the device description for exact partition parameters. */
    if (nvSMGGetDeviceById(rmctx, migDevice, &desc) != NV_OK) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /* If the GPU id doesn't match, something is wrong. */
    if (desc->gpuId != gpuId) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /*
     * Try to subscribe to the graphics partition.
     */
    ws->allocParams.swizzId = desc->gpuInstanceId;

    res = rmctx->alloc(rmctx,
                       rmctx->clientHandle,
                       subDevHandle,
                       hGpuInstSubscription,
                       AMPERE_SMC_PARTITION_REF,
                       &ws->allocParams);

    if (res != NV_OK) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /*
     * Next try to subscribe to the compute instance (exec partition)
     * that should be available under our graphics partition.
     */
    ws->execAllocParams.execPartitionId = desc->computeInstanceId;

    res = rmctx->alloc(rmctx,
                       rmctx->clientHandle,
                       hGpuInstSubscription,
                       hComputeInstSubscription,
                       AMPERE_SMC_EXEC_PARTITION_REF,
                       &ws->execAllocParams);

    if (res == NV_OK) {
        /*
         * Ok, found and allocated the desired compute instance (exec
         * partition). The subDevHandle is now fully subscribed to do SMG on
         * the requested MIG partition.
         */
        EXIT_WORKSPACE_AND_RETURN(ws, NV_TRUE);
    }

    rmctx->free(rmctx, rmctx->clientHandle, subDevHandle, hGpuInstSubscription);

    EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
}


/*
 * Get unified device description matching the given MIG UUID.
 */
NvU32 nvSMGGetDeviceByUUID(nvRMContextPtr rmctx,
                           const char *migUuid,
                           const nvMIGDeviceDescription **uniDev)
{
    NvU32 i;

    if (!InitializeGlobalMapping(rmctx)) {
        return NV_ERR_INVALID_STATE;
    }

    for (i = 0; i < globalMigMapping.deviceCount; i++) {
        if (smg_strcmp(globalMigMapping.deviceList[i].migUuid, migUuid) == 0) {
            *uniDev = &globalMigMapping.deviceList[i];
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

/*
 * Get unified device description matching a MIG device ID.
 */
NvU32 nvSMGGetDeviceById(nvRMContextPtr rmctx,
                         MIGDeviceId migDevice,
                         const nvMIGDeviceDescription **uniDev)
{
    NvU32 index = MIG_DEVICE_ID_TO_INDEX(migDevice);

    if (!InitializeGlobalMapping(rmctx)) {
        return NV_ERR_INVALID_STATE;
    }

    if (index < globalMigMapping.deviceCount) {
        *uniDev = &globalMigMapping.deviceList[index];
        return NV_OK;
    }

    return NV_ERR_INVALID_ARGUMENT;
}

NvU32 nvSMGGetDeviceList(nvRMContextPtr rmctx,
                         nvMIGDeviceDescription **devices,
                         NvU32 *deviceCount)
{
    if (!InitializeGlobalMapping(rmctx)) {
        return NV_ERR_INVALID_STATE;
    }

    *devices = globalMigMapping.deviceList;
    *deviceCount = globalMigMapping.deviceCount;

    return NV_OK;
}

NvU32 nvSMGGetDefaultDeviceForDeviceInstance(nvRMContextPtr rmctx,
                                             NvU32 deviceInstance,
                                             const nvMIGDeviceDescription **uniDev)
{
    NvU32 i;

    if (!InitializeGlobalMapping(rmctx)) {
        return NV_ERR_INVALID_STATE;
    }

    for (i = 0; i < globalMigMapping.deviceCount; i++) {
        if (globalMigMapping.deviceList[i].deviceInstance == deviceInstance &&
            globalMigMapping.deviceList[i].migAccessOk) {
            *uniDev = &globalMigMapping.deviceList[i];
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*
 * Do this once.
 */
static NvBool InitializeGlobalMapping(nvRMContextPtr rmctx)
{
    static NvBool firstRun = NV_TRUE;

    if (firstRun) {
#if !defined(NV_SMG_IN_NVKMS)
        static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;

        pthread_mutex_lock(&initMutex);
#endif

        if (firstRun) {
            firstRun = NV_FALSE;

            /*
             * Copy the RMContext and replace the clientHandle with a newly
             * allocated, dedicated RM client for our one-time probe.
             */
            nvRMContext clientctx = *rmctx;
            NvU32 res;

            if (rmctx->allocRoot) {
                res = rmctx->allocRoot(rmctx,
                                       &clientctx.clientHandle);
            } else {
                res = rmctx->alloc(rmctx,
                                   NV01_NULL_OBJECT,
                                   NV01_NULL_OBJECT,
                                   NV01_NULL_OBJECT,
                                   NV01_ROOT,
                                   &clientctx.clientHandle);
            }

            if (res == NV_OK) {
                /* Initialize the global struct. */
                smg_memset(&globalMigMapping, 0, sizeof(globalMigMapping));

                if (ListPartitions(&clientctx, &globalMigMapping)) {
                    globalMigMapping.initialized = NV_TRUE;
                }

                rmctx->free(&clientctx, clientctx.clientHandle, clientctx.clientHandle, clientctx.clientHandle);
            }
        }
#if !defined(NV_SMG_IN_NVKMS)
        pthread_mutex_unlock(&initMutex);
#endif
    }

    return globalMigMapping.initialized;
}

/*
 * Query active GPUs from RM and pick all GPUs with MIG partitions into our
 * own device list for further reference.
 */
static NvBool ListPartitions(nvRMContextPtr rmctx, struct nvGlobalMigMappingRec *mapping)
{
#if !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA)
    struct workspace {
        NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS activeParams;
        NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS idInfoParams;
    } *ws;

    NvU32 i;
    NvU32 res;

    ENTER_WORKSPACE(ws);
    smg_memset(ws, 0, sizeof(*ws));

    res = rmctx->control(rmctx,
                         rmctx->clientHandle,
                         rmctx->clientHandle,
                         NV0000_CTRL_CMD_GPU_GET_ACTIVE_DEVICE_IDS,
                         &ws->activeParams,
                         sizeof(ws->activeParams));
    if (res != NV_OK) {
        /* Explicitly not being supported can be considered a non-error. */
        if (res == NV_ERR_NOT_SUPPORTED) {
            EXIT_WORKSPACE_AND_RETURN(ws, NV_TRUE);
        }

        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /* Add MIGDevice active devices. */
    for (i = 0; i < ws->activeParams.numDevices; i++) {
        NV0000_CTRL_GPU_ACTIVE_DEVICE *dev = &ws->activeParams.devices[i];
        nvMIGDeviceDescription *migDev;

        /* First, skip over any GPU not in MIG mode. */
        if (dev->gpuInstanceId == NV0000_CTRL_GPU_INVALID_ID) {
            continue;
        }

        /* If we ever fill up the device array, bail out for a good reason. */
        if (mapping->deviceCount == NV_MAX_MIG_DEVICES) {
            EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
        }

        /* Add MIG GPUs to our list, start with getting GPU info. */
        smg_memset(&ws->idInfoParams, 0, sizeof(ws->idInfoParams));
        ws->idInfoParams.gpuId = dev->gpuId;

        res = rmctx->control(rmctx,
                             rmctx->clientHandle,
                             rmctx->clientHandle,
                             NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2,
                             &ws->idInfoParams,
                             sizeof(ws->idInfoParams));
        if (res != NV_OK) {
            EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
        }

        /* Fill in device data. */
        migDev = &mapping->deviceList[mapping->deviceCount];
        migDev->migDeviceId = MIG_DEVICE_INDEX_TO_ID(mapping->deviceCount);
        migDev->deviceInstance = ws->idInfoParams.deviceInstance;
        migDev->subDeviceInstance = ws->idInfoParams.subDeviceInstance;
        migDev->gpuId = dev->gpuId;
        migDev->gpuInstanceId = dev->gpuInstanceId;
        migDev->computeInstanceId = dev->computeInstanceId;
        migDev->migAccessOk = NV_FALSE;

        /* If it's a graphics partition and we can access it, mark valid */
        if (GetGraphicsPartitionUUIDForDevice(rmctx, migDev)) {
            migDev->migAccessOk = NV_TRUE;
        }

        mapping->deviceCount++;
    }

    EXIT_WORKSPACE_AND_RETURN(ws, NV_TRUE);

#else /* !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA) */
    /*
     * MODS and TEgra builds don't have all the MIG related rmcontrols so
     * don't do any queries, just leave it with zero SMG partitions.
     */
    return NV_TRUE;
#endif /* !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA) */
}

/*
 * Obtain the UUID of the MIG device. This means allocating the devices and
 * partition refs so that we can access the GET_UUID control.
 */
#if !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA)
static NvBool GetGraphicsPartitionUUIDForDevice(nvRMContextPtr rmctx, nvMIGDeviceDescription *migDev)
{
    struct workspace {
        NV0080_ALLOC_PARAMETERS nv0080Params;
        NV2080_ALLOC_PARAMETERS nv2080Params;
        NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS partInfoParams;
        NVC637_ALLOCATION_PARAMETERS allocParams;
        NVC638_ALLOCATION_PARAMETERS execAllocParams;
        NVC638_CTRL_GET_UUID_PARAMS getUuidParams;
        NV2080_CTRL_SMC_SUBSCRIPTION_INFO subInfoParams;
    } *ws;

    const NvU32 hDevice    = 0x1;
    const NvU32 hSubDevice = 0x2;
    const NvU32 hGpuInstSub  = 0x3;
    const NvU32 hCompInstSub = 0x4;
    NV2080_CTRL_GPU_GET_PARTITION_INFO *partInfo;
    NvU32 gfxSize;
    NvBool success = NV_FALSE;
    NvBool inMigMode;
    NvU32 i;

    ENTER_WORKSPACE(ws);
    smg_memset(ws, 0, sizeof(*ws));

    /* Allocate the corresponding device. */
    ws->nv0080Params.deviceId = migDev->deviceInstance;
    ws->nv0080Params.hClientShare = rmctx->clientHandle;

    if (rmctx->alloc(rmctx,
                     rmctx->clientHandle,
                     rmctx->clientHandle,
                     hDevice,
                     NV01_DEVICE_0,
                     &ws->nv0080Params) != NV_OK) {
        EXIT_WORKSPACE_AND_RETURN(ws, NV_FALSE);
    }

    /* And the corresponding subdevice. */
    ws->nv2080Params.subDeviceId = migDev->subDeviceInstance;

    if (rmctx->alloc(rmctx,
                     rmctx->clientHandle,
                     hDevice,
                     hSubDevice,
                     NV20_SUBDEVICE_0,
                     &ws->nv2080Params) != NV_OK) {
        goto out;
    }

    /* If not in MIG mode we can't obtain MIG UUID either. */
    if (DeviceInMigMode(rmctx, hSubDevice, &inMigMode) != NV_OK) {
        goto out;
    }

    if (!inMigMode) {
        goto out;
    }

    /* Allocate partition ref per supplied id: GPU_GET_PARTITIONS won't work
     * without a subscription for unprivileged tasks. */
    ws->allocParams.swizzId = migDev->gpuInstanceId;

    if (rmctx->alloc(rmctx,
                     rmctx->clientHandle,
                     hSubDevice,
                     hGpuInstSub,
                     AMPERE_SMC_PARTITION_REF,
                     &ws->allocParams) != NV_OK) {
        goto out;
    }

    /* Find info for the partition inferred from the partition ref object. */
    if (rmctx->control(rmctx,
                       rmctx->clientHandle,
                       hSubDevice,
                       NV2080_CTRL_CMD_GPU_GET_PARTITIONS,
                       &ws->partInfoParams,
                       sizeof(ws->partInfoParams)) != NV_OK) {
        goto out;
    }

    for (i = 0, partInfo = NULL; i <ws->partInfoParams.validPartitionCount; i++) {
        if (ws->partInfoParams.queryPartitionInfo[i].bValid &&
            ws->partInfoParams.queryPartitionInfo[i].swizzId == migDev->gpuInstanceId) {
            partInfo = &ws->partInfoParams.queryPartitionInfo[i];
            break;
        }
    }

    /* This shouldn't happen but it doesn't hurt to fail instead of asserting. */
    if (!partInfo) {
        goto out;
    }

    /* Check that the referenced partition can actually do graphics */
    gfxSize = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _GFX_SIZE, partInfo->partitionFlag);
    if (gfxSize == NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE) {
        goto out;
    }

    /* And exec partition. */
    ws->execAllocParams.execPartitionId = migDev->computeInstanceId;

    if (rmctx->alloc(rmctx,
                     rmctx->clientHandle,
                     hGpuInstSub,
                     hCompInstSub,
                     AMPERE_SMC_EXEC_PARTITION_REF,
                     &ws->execAllocParams) != NV_OK) {
        goto out;
    }

    /* Query UUID. */
    if (rmctx->control(rmctx,
                       rmctx->clientHandle,
                       hCompInstSub,
                       NVC638_CTRL_CMD_GET_UUID,
                       &ws->getUuidParams,
                       sizeof(ws->getUuidParams)) != NV_OK) {
        goto out;
    }

    /* Got one: update migDev and call it a success. */
    smg_memcpy(migDev->migUuid,
               ws->getUuidParams.uuidStr,
               NV_MIG_DEVICE_UUID_STR_LENGTH);
    success = NV_TRUE;

out:
    rmctx->free(rmctx, rmctx->clientHandle, rmctx->clientHandle, hDevice);
    EXIT_WORKSPACE_AND_RETURN(ws, success);
}
#endif /* !defined(NV_MODS) && !defined(NV_RMAPI_TEGRA) */

/*
 * Quick getter for SMC mode.
 */
static NvU32 DeviceInMigMode (nvRMContextPtr rmctx,
                              NvU32 hSubDevice,
                              NvBool *inMigMode)
{
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS gpuInfoParams;
    NvU32 res;

    gpuInfoParams.gpuInfoListSize = 1;
    gpuInfoParams.gpuInfoList[0].index = NV2080_CTRL_GPU_INFO_INDEX_GPU_SMC_MODE;

    res = rmctx->control(rmctx,
                         rmctx->clientHandle,
                         hSubDevice,
                         NV2080_CTRL_CMD_GPU_GET_INFO_V2,
                         &gpuInfoParams,
                         sizeof(gpuInfoParams));

    if (res == NV_OK) {
        *inMigMode = (
            gpuInfoParams.gpuInfoList[0].data == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLED ||
            gpuInfoParams.gpuInfoList[0].data == NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLE_PENDING);
    }

    return res;
}

/*
 * This reduces down to a simple search for the gpuId.
 */
static NvBool GpuHasSMGPartitions (NvU32 gpuId)
{
    NvU32 i;

    for (i = 0; i < globalMigMapping.deviceCount; i++) {
        if (globalMigMapping.deviceList[i].gpuId == gpuId &&
            globalMigMapping.deviceList[i].migAccessOk) {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}
