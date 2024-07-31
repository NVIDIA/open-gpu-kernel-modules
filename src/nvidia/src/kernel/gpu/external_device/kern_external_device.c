/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/timer/objtmr.h"
#include "gpu/external_device/gsync.h"

#include "gpu/external_device/external_device.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/i2c/i2c_api.h"
#include "gpu/disp/kern_disp_max.h"

static
PDACEXTERNALDEVICE extdevGetExtDev
(
    OBJGPU *pGpu
)
{
    if (RMCFG_FEATURE_EXTDEV_GSYNC)
    {
        OBJGSYNC *pGsync = gsyncmgrGetGsync(pGpu);
        if (pGsync)
        {
            return pGsync->pExtDev;
        }
    }

    return NULL;
}

//
// RMCONFIG: when the EXTDEV feature is disabled there are #defines in
// extdev.h that stub this routine out.
//
void extdevDestroy_Base(OBJGPU *pGpu, PDACEXTERNALDEVICE pExternalDevice)
{
    portMemFree(pExternalDevice->pI);
}

static NvBool dacAttachExternalDevice_Base(OBJGPU *pGpu, PDACEXTERNALDEVICE *ppExtdevs)
{
    return NV_FALSE;
}

PDACEXTERNALDEVICE
extdevConstruct_Base
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pThis
)
{
    //
    // Alloc or find interface.
    // Not being able to do so is fatal.
    //
    {
        pThis->pI = portMemAllocNonPaged(sizeof(DACEXTERNALDEVICEIFACE));
        if (pThis->pI == NULL)
        {
            return 0;
        }
        portMemSet(pThis->pI, 0, sizeof(DACEXTERNALDEVICEIFACE));
    }

    // No parent constructor to call, we're the base class!

    // Setup interface(s)
    {
        pThis->pI->Destroy = extdevDestroy_Base;
        pThis->pI->Attach  = dacAttachExternalDevice_Base;

        pThis->pI->GetDevice = 0;
        pThis->pI->Init = 0;

        pThis->pI->Validate   = extdevValidate_Default;
    }

    // Init data members
    {
        pThis->MaxGpus                 = 1; // default, only connect to 1 gpu

        pThis->WatchdogControl.Scheduled = NV_FALSE;
        pThis->WatchdogControl.TimeOut = 1000000000; // 1 second in ns
    }

    return pThis;
}

void
extdevDestroy(OBJGPU *pGpu)
{
    OBJGSYNC *pGsync = NULL;

    if (RMCFG_FEATURE_EXTDEV_GSYNC)
    {
        // destroy gsync object if any
        pGsync = gsyncmgrGetGsync(pGpu);
        if (pGsync && pGsync->pExtDev)
        {
            pGsync->pExtDev->pI->Destroy(pGpu, pGsync->pExtDev);
            if (pGsync->pExtDev->ReferenceCount == 0)
            {
                portMemFree(pGsync->pExtDev);
                pGsync->pExtDev = NULL;
            }
        }
    }
}

// Service the Watchdog
NV_STATUS extdevServiceWatchdog
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    PDACEXTERNALDEVICE pExtDevice = NULL;
    PDACEXTERNALDEVICEIFACE pdsif = NULL;

    pExtDevice = extdevGetExtDev(pGpu);

    // No gsync and no gvo, return NV_ERR_GENERIC
    if (!pExtDevice)
    {
        return NV_ERR_GENERIC;
    }

    pdsif = pExtDevice->pI;

    // lower the flag, since it's no longer waiting to run
    pExtDevice->WatchdogControl.Scheduled = NV_FALSE;

    return pdsif->Watchdog(pGpu, pTmr, pExtDevice);
}

// Schedule Watchdog
NV_STATUS extdevScheduleWatchdog
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDevice
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS rmStatus = NV_ERR_GENERIC;

    // make sure it isn't already scheduled
    if (!pExtDevice->WatchdogControl.Scheduled)
    {
        pExtDevice->WatchdogControl.Scheduled = NV_TRUE;

        rmStatus = tmrEventScheduleRel(pTmr,
                                       pExtDevice->WatchdogControl.pTimerEvents[gpuGetInstance(pGpu)],
                                       pExtDevice->WatchdogControl.TimeOut);

        if (NV_OK != rmStatus)
        {
            pExtDevice->WatchdogControl.Scheduled = NV_FALSE;
        }
    }

    return rmStatus;
}

// deSchedule Watchdog
NV_STATUS extdevCancelWatchdog
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDevice
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    // cancel first...
    tmrEventCancel(pTmr, pExtDevice->WatchdogControl.pTimerEvents[gpuGetInstance(pGpu)]);

    // ... then lower the flag!
    pExtDevice->WatchdogControl.Scheduled = NV_FALSE;

    return NV_OK;
}

// Trivial validation
NvBool extdevValidate_Default
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDevice
)
{
    if (!pExtDevice)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NV_STATUS
writeregu008_extdevice
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pThis,
    NvU8               SubAdr,
    NvU8               Data
)
{

    NV_STATUS status = NV_ERR_GENERIC;

    NvU32 i2cPort = (pGpu->i2cPortForExtdev < NV402C_CTRL_NUM_I2C_PORTS) ? pGpu->i2cPortForExtdev : pThis->I2CPort;
    status = i2c_extdeviceHelper(pGpu, pThis, i2cPort, SubAdr, &Data, NV_TRUE);

    return status;
}

//
// This is a wrapper function for writeregu008_extdevice to be used when a
// specific GPU is to be targeted (rather than an SLI abstraction). For
// example, a P294 framelock board has two connectors to gpus, one for a
// primary gpu and one for a secondary gpu. It is only valid to read and
// write registers over i2c connected to the primary gpu. If the MC_PARENT
// is not the same gpu as the primary, the ThisGpuFromHal macro at the
// beginning of writeregu008_extdevice will return the secondary gpu unless
// the primary gpu is specifically loaded beforehand.
// This wrapper does that loading.
//
NV_STATUS
writeregu008_extdeviceTargeted
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pThis,
    NvU8                SubAdr,
    NvU8                Data
)
{
    NV_STATUS status = NV_ERR_GENERIC;
    NvBool bcState;

    if (!pGpu)
    {
       return status;
    }

    bcState = gpumgrGetBcEnabledStatus(pGpu);

    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
    status = writeregu008_extdevice(pGpu, pThis, SubAdr, Data);
    gpumgrSetBcEnabledStatus(pGpu, bcState);

    return status;
}

NV_STATUS
readregu008_extdevice
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pThis,
    NvU8                SubAdr,
    NvU8               *pData
)
{

    NV_STATUS status = NV_ERR_GENERIC;

    NvU32 i2cPort = (pGpu->i2cPortForExtdev < NV402C_CTRL_NUM_I2C_PORTS) ? pGpu->i2cPortForExtdev : pThis->I2CPort;
    status = i2c_extdeviceHelper(pGpu, pThis, i2cPort, SubAdr, pData, NV_FALSE);

    return status;
}

//
// This is a wrapper function for readregu008_extdevice to be used when a
// specific GPU is to be targeted (rather than an SLI abstraction). For
// example, a P294 framelock board has two connectors to gpus, one for a
// primary gpu and one for a secondary gpu. It is only valid to read and
// write registers over i2c connected to the primary gpu. If the MC_PARENT
// is not the same gpu as the primary, the ThisGpuFromHal macro at the
// beginning of readregu008_extdevice will return the secondary gpu unless
// the primary gpu is specifically loaded beforehand.
// This wrapper does that loading.
//
NV_STATUS
readregu008_extdeviceTargeted
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pThis,
    NvU8                SubAdr,
    NvU8               *pData
)
{
    NV_STATUS status = NV_ERR_GENERIC;
    NvBool bcState;

    if (!pGpu)
    {
       return status;
    }

    bcState = gpumgrGetBcEnabledStatus(pGpu);

    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
    status = readregu008_extdevice(pGpu, pThis, SubAdr, pData);
    gpumgrSetBcEnabledStatus(pGpu, bcState);

    return status;
}

void
extdevGsyncService
(
    OBJGPU             *pGpu,
    NvU8                lossRegStatus,
    NvU8                gainRegStatus,
    NvU8                miscRegStatus,
    NvBool              rmStatus
)
{
    PDACEXTERNALDEVICE pExtDevice = NULL;
    PDACEXTERNALDEVICEIFACE pdsif = NULL;

    pExtDevice = extdevGetExtDev(pGpu);
    if (!pExtDevice)
        return;

    pdsif = pExtDevice->pI;

    pdsif->Service(pGpu, pExtDevice, lossRegStatus, gainRegStatus, miscRegStatus, rmStatus);
}

void
extdevGetBoundHeadsAndDisplayIds
(
    OBJGPU      *pGpu,
    NvU32       *pDisplayId
)
{
    RM_API   *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32     hClient     = pGpu->hInternalClient;
    NvU32     hSubdevice  = pGpu->hInternalSubdevice;
    NV_STATUS status      = NV_OK;
    NV2080_CTRL_INTERNAL_GSYNC_GET_DISPLAY_IDS_PARAMS ctrlParams = {0};

    portMemSet(pDisplayId, 0, OBJ_MAX_HEADS * sizeof(NvU32));

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_DISPLAY_IDS,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Extdev getting display IDs have failed!\n");
    }
    else
    {
        portMemCopy(pDisplayId, OBJ_MAX_HEADS * sizeof(NvU32), ctrlParams.displayIds,
                    OBJ_MAX_HEADS * sizeof(NvU32));
    }
}

