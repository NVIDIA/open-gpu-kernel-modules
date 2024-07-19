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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Gsync module                                                      *
*                                                                           *
\***************************************************************************/

#include "core/core.h"

// The "ONLY_IF => 'EXTDEV_GSYNC'," condition in sources.def is ignored!

#include "nvrm_registry.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"
#include "gpu/external_device/gsync.h"
#include "os/os.h"
#include "rmapi/control.h"
#include "gpu/external_device/gsync_api.h"

#include "gpu/external_device/dac_p2060.h"
#include "gpu/external_device/dac_p2061.h"

#include "ctrl/ctrl0000/ctrl0000gsync.h"
#include "ctrl/ctrl30f1.h"

#include "class/cl30f1.h"

// local static funcs
static NV_STATUS gsyncSetupNullProvider   (OBJGSYNCMGR *pGsyncMgr, NvU32);

static OBJGPU   *gsyncGetMasterableGpu(OBJGSYNC *);
static NvBool    gsyncIsAnyHeadFramelocked(OBJGSYNC *);

static NV_STATUS gsyncGetGpuTopology(OBJGSYNC *,
            NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS*);
static NV_STATUS gsyncGetStatusSignals(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS*);
static NV_STATUS gsyncGetStatusParams(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS*);
static NV_STATUS gsyncSetControlParams(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS*);
static NV_STATUS gsyncGetStatusCaps(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS*);
static NV_STATUS gsyncGetControlSync(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS*);
static NV_STATUS gsyncSetControlSync(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS*);
static NV_STATUS gsyncSetControlUnsync(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS*);
static NV_STATUS gsyncGetStatusSync(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS* pParams);
static NV_STATUS gsyncGetStatus(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS*);
static NV_STATUS gsyncGetControlTesting(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS*);
static NV_STATUS gsyncSetControlTesting(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS*);
static NV_STATUS gsyncSetControlWatchdog(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS*);
static NV_STATUS gsyncGetControlInterlaceMode(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS*);
static NV_STATUS gsyncSetControlInterlaceMode(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS*);
static NV_STATUS gsyncGetControlSwapBarrier(OBJGSYNC *, OBJGPU *,
            NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS*);
static NV_STATUS gsyncSetControlSwapBarrier(OBJGSYNC *, OBJGPU *,
            NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS*);
static NV_STATUS gsyncGetControlSwapLockWindow(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS*);
static NV_STATUS gsyncGetOptimizedTiming(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS* pParams);
static NV_STATUS gsyncGetControlStereoLockMode(OBJGSYNC *,
            NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS*);
static NV_STATUS gsyncSetControlStereoLockMode(OBJGSYNC *,
            NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS*);
static NV_STATUS gsyncReadRegister(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *);
static NV_STATUS gsyncWriteRegister(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *);
static NV_STATUS gsyncSetLocalSync(OBJGSYNC *,
            NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS*);
static NV_STATUS gsyncConfigFlash(OBJGSYNC *,
            NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS*);
static NV_STATUS gsyncGetHouseSyncMode(OBJGSYNC *pGsync,
                      NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS*);
static NV_STATUS gsyncSetHouseSyncMode(OBJGSYNC *pGsync,
                      NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS*);

NV_STATUS
gsyncmgrConstruct_IMPL(OBJGSYNCMGR *pGsyncMgr)
{
    NvU32 i;

    NV_PRINTF(LEVEL_INFO, "\n");

    pGsyncMgr->gsyncCount = 0;

    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        portMemSet(&pGsyncMgr->gsyncTable[i], 0, sizeof (OBJGSYNC));
        gsyncSetupNullProvider(pGsyncMgr, i);
        pGsyncMgr->gsyncTable[i].gsyncId = NV0000_CTRL_GSYNC_INVALID_ID;
        // Automatic watchdog scheduling is enabled until 1st manual call.
        pGsyncMgr->gsyncTable[i].bAutomaticWatchdogScheduling = NV_TRUE;
        pGsyncMgr->gsyncTable[i].bDoEventFiltering = NV_FALSE;
    }

    return NV_OK;
}

// Destructor
void
gsyncmgrDestruct_IMPL(OBJGSYNCMGR *pGsyncMgr)
{
    OBJGSYNC *pGsync;
    NvU32 i;

    NV_PRINTF(LEVEL_INFO, "\n");

    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        pGsync = &pGsyncMgr->gsyncTable[i];

        // NULL everything
        portMemSet(pGsync, 0, sizeof (OBJGSYNC));
    }
}

//
// gsyncGetMasterableGpuByInstance
// Return a gpu that can be timing master for the specified gsync instance.
//
OBJGPU *
gsyncGetMasterableGpuByInstance(NvU32 gsyncInst)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);

    if (gsyncInst >= NV30F1_MAX_GSYNCS)
        return NULL;

    NV_ASSERT(pGsyncMgr->gsyncTable[gsyncInst].gpuCount > 0);

    return gsyncGetMasterableGpu(&pGsyncMgr->gsyncTable[gsyncInst]);

}

static OBJGPU *gsyncGetMasterableGpu(OBJGSYNC *pGsync)
{
    NvU32 i;

    for (i = 0; i < pGsync->gpuCount; i++)
    {
        if ((pGsync->masterableGpuConnectors & (1 << i)) &&
            (pGsync->gpus[i].connector != gsync_Connector_None))
        {
            return gpumgrGetGpuFromId(pGsync->gpus[i].gpuId);
        }
    }

    NV_PRINTF(LEVEL_ERROR, "Invalid gsync state\n");

    return NULL;
}

/*
 * Set up the function pointers for p2060 gsync object.
 */
static NV_STATUS
gsyncP2060StartupProvider(OBJGSYNC *pGsync)
{
    DACEXTERNALDEVICE      *pExtDev = pGsync->pExtDev;
    DACP2060EXTERNALDEVICE *p2060   = (DACP2060EXTERNALDEVICE *)pGsync->pExtDev;

    // All four GPU connectors are masterable on P2060
    pGsync->masterableGpuConnectors          = ((1 << NV30F1_GSYNC_CONNECTOR_ONE)   |
                                                (1 << NV30F1_GSYNC_CONNECTOR_TWO)   |
                                                (1 << NV30F1_GSYNC_CONNECTOR_THREE) |
                                                (1 << NV30F1_GSYNC_CONNECTOR_FOUR));

    pGsync->connectorCount                   = NV_P2060_MAX_IFACES_PER_GSYNC;

    pGsync->gsyncHal.gsyncGpuCanBeMaster     = gsyncGpuCanBeMaster_P2060;
    pGsync->gsyncHal.gsyncOptimizeTiming     = gsyncOptimizeTimingParameters_P2060;

    pGsync->gsyncHal.gsyncRefSignal          = gsyncRefSignal_P2060;
    pGsync->gsyncHal.gsyncRefMaster          = gsyncRefMaster_P2060;
    pGsync->gsyncHal.gsyncRefSlaves          = gsyncRefSlaves_P2060;
    pGsync->gsyncHal.gsyncGetCplStatus       = gsyncGetCplStatus_P2060;
    pGsync->gsyncHal.gsyncSetWatchdog        = gsyncSetWatchdog_P2060;
    pGsync->gsyncHal.gsyncGetRevision        = gsyncGetRevision_P2060;

    // Get and Set
    pGsync->gsyncHal.gsyncGetUseHouse        = gsyncGetUseHouse_P2060;
    pGsync->gsyncHal.gsyncSetUseHouse        = gsyncSetUseHouse_P2060;
    pGsync->gsyncHal.gsyncGetSyncPolarity    = gsyncGetSyncPolarity_P2060;
    pGsync->gsyncHal.gsyncSetSyncPolarity    = gsyncSetSyncPolarity_P2060;
    pGsync->gsyncHal.gsyncGetVideoMode       = gsyncGetVideoMode_P2060;
    pGsync->gsyncHal.gsyncSetVideoMode       = gsyncSetVideoMode_P2060;
    pGsync->gsyncHal.gsyncGetNSync           = gsyncGetNSync_P2060;
    pGsync->gsyncHal.gsyncSetNSync           = gsyncSetNSync_P2060;
    pGsync->gsyncHal.gsyncGetSyncSkew        = gsyncGetSyncSkew_P2060;
    pGsync->gsyncHal.gsyncSetSyncSkew        = gsyncSetSyncSkew_P2060;
    pGsync->gsyncHal.gsyncGetSyncStartDelay  = gsyncGetSyncStartDelay_P2060;
    pGsync->gsyncHal.gsyncSetSyncStartDelay  = gsyncSetSyncStartDelay_P2060;
    pGsync->gsyncHal.gsyncGetEmitTestSignal  = gsyncGetEmitTestSignal_P2060;
    pGsync->gsyncHal.gsyncSetEmitTestSignal  = gsyncSetEmitTestSignal_P2060;
    pGsync->gsyncHal.gsyncGetInterlaceMode   = gsyncGetInterlaceMode_P2060;
    pGsync->gsyncHal.gsyncSetInterlaceMode   = gsyncSetInterlaceMode_P2060;
    pGsync->gsyncHal.gsyncGetStereoLockMode  = gsyncGetStereoLockMode_P2060;
    pGsync->gsyncHal.gsyncSetStereoLockMode  = gsyncSetStereoLockMode_P2060;
    pGsync->gsyncHal.gsyncGetMulDiv          = gsyncGetMulDiv_P2060;
    pGsync->gsyncHal.gsyncSetMulDiv          = gsyncSetMulDiv_P2060;

    pGsync->gsyncHal.gsyncRefSwapBarrier     = gsyncRefSwapBarrier_P2060;
    pGsync->gsyncHal.gsyncSetMosaic          = gsyncSetMosaic_P2060;
    pGsync->gsyncHal.gsyncConfigFlashGsync   = gsyncConfigFlashGsync_P2060;

    // Constants to be returned in NV30F1_CTRL_GSYNC_GET_CAPS
    p2060->syncSkewResolutionInNs = NV_P2060_SYNC_SKEW_RESOLUTION;
    p2060->syncSkewMax            = gsyncSupportsLargeSyncSkew_P2060(pExtDev) ?
                                    NV_P2060_SYNC_SKEW_MAX_UNITS_FULL_SUPPORT :
                                    NV_P2060_SYNC_SKEW_MAX_UNITS_LIMITED_SUPPORT;

    return NV_OK;
}

/*
 * Set up the function pointers for p2061 gsync object.
 */
static NV_STATUS
gsyncP2061StartupProvider(OBJGSYNC *pGsync)
{
    NV_STATUS status;
    DACP2060EXTERNALDEVICE *p2060 = (DACP2060EXTERNALDEVICE *)pGsync->pExtDev;

    // Call P2060 startup provider for setting up those HALs that
    // are identical to P2060.
    status = gsyncP2060StartupProvider(pGsync);

    // Hals differ between p2060 and p2061, apply override
    pGsync->gsyncHal.gsyncGetCplStatus      = gsyncGetCplStatus_P2061;

    // HALs for P2061 specifically
    pGsync->gsyncHal.gsyncGetHouseSyncMode  = gsyncGetHouseSyncMode_P2061;
    pGsync->gsyncHal.gsyncSetHouseSyncMode  = gsyncSetHouseSyncMode_P2061;

    // SyncSkew is different for FW V2.04+
    if (P2061_FW_REV(pGsync->pExtDev) >= 0x204)
    {
        pGsync->gsyncHal.gsyncGetSyncSkew  = gsyncGetSyncSkew_P2061_V204;
        pGsync->gsyncHal.gsyncSetSyncSkew  = gsyncSetSyncSkew_P2061_V204;

        // Constants to be returned in NV30F1_CTRL_GSYNC_GET_CAPS
        p2060->syncSkewResolutionInNs      = NV_P2061_V204_SYNC_SKEW_RESOLUTION;
        p2060->syncSkewMax                 = NV_P2061_V204_SYNC_SKEW_MAX_UNITS;
        p2060->lastUserSkewSent            = NV_P2061_V204_SYNC_SKEW_INVALID;
    }

    return status;
}

/*
 * This is the general entrance for setting up interfaces for different objects.
 * We will direct it to a specific start-up provider based on board ID.
 */
static NV_STATUS
gsyncStartupProvider
(
    OBJGSYNC *pGsync,
    DAC_EXTERNAL_DEVICES externalDevice
)
{
    switch (externalDevice)
    {
        default:
        case DAC_EXTERNAL_DEVICE_NONE:
            return NV_OK;
        case DAC_EXTERNAL_DEVICE_P2060:
            return gsyncP2060StartupProvider(pGsync);
        case DAC_EXTERNAL_DEVICE_P2061:
            return gsyncP2061StartupProvider(pGsync);
    }
}

//
// gsyncAttachGpu
//
// Associate this gpu with this gsync. Some gsyncs have primary
// and secondary connectors, and it's important to keep track of
// that because the primary can talk to the device, whereas the
// secondary can only listen. Also, only a display on the primary
// connector can be a frame lock master.
//
NV_STATUS
gsyncAttachGpu(PDACEXTERNALDEVICE pExtDev, OBJGPU *pGpu,
    GSYNCCONNECTOR connector, OBJGPU *pProxyGpu,
    DAC_EXTERNAL_DEVICES externalDevice)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC    *pGsync = NULL;
    NvU32 i, j;

    NV_ASSERT(rmGpuLockIsOwner());

    // Check to see if we've already attached this gpu
    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        for (j = 0; j < pGsyncMgr->gsyncTable[i].gpuCount; j++)
        {
            if (pGsyncMgr->gsyncTable[i].gpus[j].gpuId == pGpu->gpuId)
            {
                NV_PRINTF(LEVEL_ERROR, "gpu is %d already attached!\n",
                          pGpu->gpuId);

                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    // try to match this external device
    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        if (pGsyncMgr->gsyncTable[i].pExtDev == pExtDev)
            break;
    }

    // allocate new entry in gsync table if extdev not found
    if (i == NV30F1_MAX_GSYNCS)
    {
        for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
        {
            if (pGsyncMgr->gsyncTable[i].gpuCount == 0)
            {
                pGsyncMgr->gsyncCount++;
                break;
            }
        }
    }

    if ((i == NV30F1_MAX_GSYNCS) ||
        (pGsyncMgr->gsyncTable[i].gpuCount == NV30F1_CTRL_MAX_GPUS_PER_GSYNC))
    {
        NV_PRINTF(LEVEL_ERROR, "gsync table full!\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pGsync = &pGsyncMgr->gsyncTable[i];

    pGsync->pExtDev = pExtDev;
    pGsync->gpus[pGsync->gpuCount].gpuId = pGpu->gpuId;
    pGsync->gpus[pGsync->gpuCount].connector = connector;
    pGsync->gpus[pGsync->gpuCount].proxyGpuId =
        (pProxyGpu != NULL) ? pProxyGpu->gpuId : NV30F1_CTRL_GPU_INVALID_ID;

    if (pGsync->gsyncId == NV30F1_CTRL_GPU_INVALID_ID)
    {
        // If invalid, GSYNC gsyncId will be any of attached GPU gpuId.
        pGsync->gsyncId = pGpu->gpuId;
    }

    pGsync->gpuCount++;

    pSys->setProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED, NV_TRUE);

    return gsyncStartupProvider(pGsync, externalDevice);
}

//
// gsyncRemoveGpu
//
// Remove a gpu from this gsync
//
NV_STATUS
gsyncRemoveGpu(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC    *pGsync;
    NvU32 i, j;

    NV_ASSERT(rmGpuLockIsOwner());

    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        pGsync = &pGsyncMgr->gsyncTable[i];

        for (j = 0; j < pGsync->gpuCount; j++)
        {
            if (pGpu->gpuId == pGsync->gpus[j].gpuId)
            {
                // Careful here: we are modifying one of the test conditions
                // for this for loop.
                pGsync->gpuCount--;

                if (j != pGsync->gpuCount)
                {
                    // copy the last gpu in the array over this one (if this
                    // isn't already the last element)
                    portMemCopy(&pGsync->gpus[j],
                                sizeof(pGsync->gpus[j]),
                                &pGsync->gpus[pGsync->gpuCount],
                                sizeof(pGsync->gpus[j]));
                }

                // zero out the element (which we may have just copied)
                portMemSet(&pGsync->gpus[pGsync->gpuCount], 0,
                         sizeof(pGsync->gpus[pGsync->gpuCount]));

                if (pGsync->gpuCount == 0)
                {
                    pGsyncMgr->gsyncCount--;
                    pGsync->gsyncId = NV0000_CTRL_GSYNC_INVALID_ID;
                    gsyncSetupNullProvider(pGsyncMgr, i);
                }

                if (pGsyncMgr->gsyncCount == 0)
                {
                    pSys->setProperty(pSys, PDB_PROP_SYS_IS_GSYNC_ENABLED, NV_FALSE);
                }
                // If we ever support multiple gsyncs per
                // gpu, need to make sure we handle this correctly since we
                // are modifying one of our test conditions inside the 'for'
                // loop. For now just return from here.
                return NV_OK;
            }
        }
    }

    NV_PRINTF(LEVEL_INFO, "GPU was not found in gsync object\n");

    return NV_OK;
}

//
// gsyncmgrGetGsync
//
// This routine returns a OBJGSYNC object associated with this GPU.
//
OBJGSYNC *
gsyncmgrGetGsync(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = NULL;
    OBJGSYNC    *pGsync    = NULL;
    NvU32 i, j;

    if (!RMCFG_FEATURE_EXTDEV_GSYNC)
        return NULL;

    if (!pGpu)
        return NULL;

    pGsyncMgr = SYS_GET_GSYNCMGR(pSys);

    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        pGsync = &pGsyncMgr->gsyncTable[i];

        for (j = 0; j < pGsync->gpuCount; j++)
        {
            if (pGpu->gpuId == pGsync->gpus[j].gpuId)
            {
                return pGsync;
            }
        }
    }

    return NULL;
}

//
// gsyncGetAttachedIds
//
// This routine services the NV0000_CTRL_GSYNC_GET_ATTACHED_IDS command.
// The passed in gsync table is filled in with the unique gsync value
// for each attached gsync.  Any remaining entries in the table
// are set to the invalid id value.
//
NV_STATUS
gsyncGetAttachedIds(NV0000_CTRL_GSYNC_GET_ATTACHED_IDS_PARAMS *pGsyncIdsParams)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    NvU32 *pGsyncIds = &pGsyncIdsParams->gsyncIds[0];
    NvU32 i, cnt;

    // fill the table w/valid entries
    for (cnt = 0, i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        if (pGsyncMgr->gsyncTable[i].gpuCount > 0)
        {
            pGsyncIds[cnt++] = pGsyncMgr->gsyncTable[i].gsyncId;
        }
    }

    // invalidate rest of the entries
    while (cnt < NV30F1_MAX_GSYNCS)
        pGsyncIds[cnt++] = NV0000_CTRL_GSYNC_INVALID_ID;

    return NV_OK;
}

//
// gsyncGetIdInfo
//
// Special purpose routine that handles NV0000_CTRL_CMD_GSYNC_GET_ID_INFO
// requests from clients.
//
NV_STATUS
gsyncGetIdInfo(NV0000_CTRL_GSYNC_GET_ID_INFO_PARAMS *pGsyncInfo)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    NvU32 i;

    // fill the table w/valid entries
    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        if (pGsyncMgr->gsyncTable[i].gpuCount > 0)
            if (pGsyncInfo->gsyncId == pGsyncMgr->gsyncTable[i].gsyncId)
                break;
    }

    if (i == NV30F1_MAX_GSYNCS)
        return NV_ERR_INVALID_ARGUMENT;

    pGsyncInfo->gsyncInstance = i;
    pGsyncInfo->gsyncFlags = 0;

    return NV_OK;
}

//
// gsyncGetGsyncId
//
// Return the associated gsyncId for the specified gpu.
//
NvU32
gsyncGetGsyncInstance(OBJGPU *pGpu)
{

    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync;
    NvU32 i, j;

    for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
    {
        pGsync = &pGsyncMgr->gsyncTable[i];

        for (j = 0; j < pGsync->gpuCount; j++)
        {
            if (pGpu->gpuId == pGsync->gpus[j].gpuId)
                return i;
        }
    }

    return NV30F1_MAX_GSYNCS;
}

NvBool
gsyncAreAllGpusInConfigAttachedToSameGsyncBoard(OBJGPU **pGpus, NvU32 gpuCount)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC    *pGsync    = NULL;
    NvBool       bIsGpuFoundInGsync = NV_FALSE;
    NvU32        gsyncCount = 0, i, j;

    while(gsyncCount < pGsyncMgr->gsyncCount)
    {
        pGsync = &pGsyncMgr->gsyncTable[gsyncCount++];
        if (pGsync == NULL)
        {
            continue;
        }

        for (i = 0; i < gpuCount; i++)
        {
            // Take any gpu form given pGpus
            OBJGPU *pGpu = pGpus[i];
            bIsGpuFoundInGsync = NV_FALSE;

            for (j = 0; j < pGsync->gpuCount; j++)
            {
               if (pGpu->gpuId == pGsync->gpus[j].gpuId)
               {
                   bIsGpuFoundInGsync = NV_TRUE;
               }
            }

            if (!bIsGpuFoundInGsync)
            {
                // pGpu is not present under this gsync.
                break;
            }
        }

        if (bIsGpuFoundInGsync)
        {
           // All `gpu form given pGpus are attached to same gysnc
           return NV_TRUE;
        }
    }
    return NV_FALSE;
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetVersion_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams
)
{
    pGsyncGetVersionParams->version = NV30F1_CTRL_GSYNC_API_VER;
    pGsyncGetVersionParams->revision = NV30F1_CTRL_GSYNC_API_REV;

    return NV_OK;
}

NV_STATUS
gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetGpuTopology(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetStatusSignals(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlParams_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetStatusParams(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlParams_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlParams(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlSync_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetControlSync(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlSync_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlSync(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlUnsync(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetStatusSync_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetStatusSync(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetStatus_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetStatus(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlTesting_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetControlTesting(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlTesting_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlTesting(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlWatchdog(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetControlInterlaceMode(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlInterlaceMode(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_INVALID_STATE;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];
    OBJGPU *pGpu = gpumgrGetGpuFromId(pParams->gpuId);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)
    {
        status = gsyncGetControlSwapBarrier(pGsync, pGpu, pParams);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    return status;
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_INVALID_STATE;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];
    OBJGPU *pGpu = gpumgrGetGpuFromId(pParams->gpuId);

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_NONE)
    {
        status = gsyncSetControlSwapBarrier(pGsync, pGpu, pParams);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    return status;
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetControlSwapLockWindow(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetCaps_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetStatusCaps(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetOptimizedTiming(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetLocalSync_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetLocalSync(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncConfigFlash_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncConfigFlash(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetEventNotification_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];
    NV_STATUS status = NV_OK;

    if (NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_DISABLE == pSetEventParams->action)
    {
        pGsyncApi->notifyAction = pSetEventParams->action;
    }
    else
    {
        Notifier *pNotifier = staticCast(pGsyncApi, Notifier);
        PEVENTNOTIFICATION *ppEventNotifications = NULL;

        // NV01_EVENT must have been plugged in
        if (pNotifier->pNotifierShare == NULL)
            return NV_ERR_INVALID_STATE;

        ppEventNotifications = inotifyGetNotificationListPtr(staticCast(pNotifier, INotifier));
        if (*ppEventNotifications == NULL)
            return NV_ERR_INVALID_STATE;

        if (NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_ALL & pSetEventParams->action)
        {
            NvU32 eventNum = 0, tempMask = pSetEventParams->action;

            // If more than one bit is set, the highest on is choosen.
            // This is a fallback, we should only be called with one bit set.
            NV_ASSERT(ONEBITSET(tempMask));

            // convert mask to array index
            while (tempMask >>= 1)
            {
                eventNum++;
            }

            if (eventNum < NV30F1_CTRL_GSYNC_EVENT_TYPES)
            {
                NV_ASSERT_OR_RETURN(pGsyncApi->pEventByType[eventNum] == NULL, NV_ERR_INVALID_STATE);

                // pull event off of queue, put in the slot we calculated
                pGsyncApi->pEventByType[eventNum] = *ppEventNotifications;
                *ppEventNotifications = (*ppEventNotifications)->Next;
                pGsyncApi->pEventByType[eventNum]->Next = NULL;

                // add to the master mask
                pGsyncApi->notifyAction |= pSetEventParams->action;
            }
            pGsyncApi->oldEventNotification = NV_TRUE;
            pGsync->bDoEventFiltering = NV_TRUE;
        }
        else
        {
            status = NV_ERR_INVALID_ARGUMENT;
        }
    }
    return status;

}

NV_STATUS
gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetControlStereoLockMode(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetControlStereoLockMode(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncReadRegister_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncReadRegister(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncWriteRegister_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncWriteRegister(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncGetHouseSyncMode(pGsync, pParams);
}

NV_STATUS
gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL
(
    GSyncApi *pGsyncApi,
    NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = &pGsyncMgr->gsyncTable[pGsyncApi->instance];

    return gsyncSetHouseSyncMode(pGsync, pParams);
}

NV_STATUS
gsyncGetGpuTopology(OBJGSYNC *pGsync,
            NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS* pParams)
{
    NvU32 i;

    for (i = 0; i < NV30F1_CTRL_MAX_GPUS_PER_GSYNC; i++)
    {
        if (i < pGsync->gpuCount &&
            pGsync->gpus[i].connector <= NV30F1_GSYNC_CONNECTOR_COUNT)
        {
            pParams->gpus[i].gpuId      = pGsync->gpus[i].gpuId;
            pParams->gpus[i].connector  = pGsync->gpus[i].connector;
            pParams->gpus[i].proxyGpuId = pGsync->gpus[i].proxyGpuId;
        }
        else
        {
            pParams->gpus[i].gpuId = NV30F1_CTRL_GPU_INVALID_ID;
        }
    }
    pParams->connectorCount = pGsync->connectorCount;

    return NV_OK;
}

//
// gsyncIsAnyHeadFramelocked
//
// Returns NV_TRUE if any of the heads attached to any of the gpus
// attached to the gsync object is framelocked. NV_FALSE otherwise.
NvBool
gsyncIsAnyHeadFramelocked(OBJGSYNC *pGsync)
{
    if (pGsync && pGsync->pExtDev)
    {
        OBJGPU *pGpu = NULL;
        NvU32 i;
        NvU32 refresh, assigned;

        // Loop over all gpus of the gsync object.
        for (i = 0; i < pGsync->gpuCount; i++)
        {
            pGpu = gpumgrGetGpuFromId(pGsync->gpus[i].gpuId);

            if (pGpu)
            {
                // Check if assigned slaves displays are there.
                if ((NV_OK == pGsync->gsyncHal.gsyncRefSlaves(pGpu,
                     pGsync->pExtDev, refRead, &assigned, &refresh)) &&
                    (assigned != 0))
                {
                    return NV_TRUE;
                }
                // Check if assigned master displays are there.
                if ((NV_OK == pGsync->gsyncHal.gsyncRefMaster(pGpu,
                     pGsync->pExtDev, refRead, &assigned, &refresh, NV_FALSE, NV_FALSE)) &&
                    (assigned != 0))
                {
                    return NV_TRUE;
                }
            }
        }
    }

    return NV_FALSE;
}

NV_STATUS
gsyncGetStatusSignals(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS* pParams)
{
    NvBool bRate; // whether or not to test the rate.
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = NULL;
    REFTYPE rType = refFetchGet;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    bRate = !! (pParams->rate & NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_RJ45_0);
    status |= pGsync->gsyncHal.gsyncRefSignal(pGpu, pGsync->pExtDev, rType,
            gsync_Signal_RJ45_0, bRate, &pParams->RJ45[0]);

    bRate = !! (pParams->rate & NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_RJ45_1);
    status |= pGsync->gsyncHal.gsyncRefSignal(pGpu, pGsync->pExtDev, rType,
            gsync_Signal_RJ45_1, bRate, &pParams->RJ45[1]);

    bRate = !! (pParams->rate & NV30F1_CTRL_GSYNC_GET_SIGNALS_HOUSE);
    status |= pGsync->gsyncHal.gsyncRefSignal(pGpu, pGsync->pExtDev, rType,
            gsync_Signal_House, bRate, &pParams->house);

    return status;
}

NV_STATUS
gsyncGetStatusParams(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    GSYNCSYNCPOLARITY SyncPolarity = pParams->syncPolarity;
    GSYNCVIDEOMODE VideoMode   = pParams->syncVideoMode;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_POLARITY )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetSyncPolarity(pGpu, pGsync->pExtDev, &SyncPolarity));
        pParams->syncPolarity = (NvU32)SyncPolarity;
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_VIDEO_MODE )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetVideoMode(pGpu, pGsync->pExtDev, &VideoMode));
        pParams->syncVideoMode = (NvU32)VideoMode;
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_NSYNC )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetNSync(pGpu, pGsync->pExtDev, &pParams->nSync));
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_SKEW )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetSyncSkew(pGpu, pGsync->pExtDev, &pParams->syncSkew));
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_START_DELAY )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetSyncStartDelay(pGpu, pGsync->pExtDev, &pParams->syncStartDelay));
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_USE_HOUSE )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetUseHouse(pGpu, pGsync->pExtDev, &pParams->useHouseSync));
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_MULTIPLY_DIVIDE )
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO, pGsync->gsyncHal.gsyncGetMulDiv(pGpu, pGsync->pExtDev, &pParams->syncMulDiv));
    }

    return status;
}

NV_STATUS
gsyncSetControlParams(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    GSYNCSYNCPOLARITY SyncPolarity = pParams->syncPolarity;
    GSYNCVIDEOMODE VideoMode   = pParams->syncVideoMode;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_POLARITY )
    {
        status |= pGsync->gsyncHal.gsyncSetSyncPolarity(pGpu, pGsync->pExtDev, SyncPolarity);
        pParams->syncPolarity = (NvU32)SyncPolarity;
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_VIDEO_MODE )
    {
        status |= pGsync->gsyncHal.gsyncSetVideoMode(pGpu, pGsync->pExtDev, VideoMode);
        pParams->syncVideoMode = (NvU32)VideoMode;
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_NSYNC )
    {
        status |= pGsync->gsyncHal.gsyncSetNSync(pGpu, pGsync->pExtDev, pParams->nSync);
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_SKEW )
    {
        status |= pGsync->gsyncHal.gsyncSetSyncSkew(pGpu, pGsync->pExtDev, pParams->syncSkew);
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_START_DELAY )
    {
        status |= pGsync->gsyncHal.gsyncSetSyncStartDelay(pGpu, pGsync->pExtDev, pParams->syncStartDelay);
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_USE_HOUSE )
    {
        status |= pGsync->gsyncHal.gsyncSetUseHouse(pGpu, pGsync->pExtDev, pParams->useHouseSync);
    }

    if ( pParams->which & NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_MULTIPLY_DIVIDE )
    {
        status |= pGsync->gsyncHal.gsyncSetMulDiv(pGpu, pGsync->pExtDev, &pParams->syncMulDiv);
    }

    return status;
}

NV_STATUS gsyncGetStatusCaps(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    status |= pGsync->gsyncHal.gsyncGetRevision(pGpu, pGsync->pExtDev, pParams);

    return status;
}

static NvBool
gsyncIsGpuInGsync(OBJGPU *pGpu, OBJGSYNC *pGsync)
{
    NvU32 gpuCount;

    if (!pGpu || !pGsync)
    {
        return NV_FALSE;
    }

    for (gpuCount = 0; gpuCount < NV30F1_CTRL_MAX_GPUS_PER_GSYNC; gpuCount++)
    {
        if (pGpu->gpuId == pGsync->gpus[gpuCount].gpuId)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NV_STATUS
gsyncSetLocalSync(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS* pParams)
{
    OBJGPU *pSourceGpu = NULL;
    OBJGPU *pTempGpu = NULL;
    NvU32 i;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pSourceGpu = gpumgrGetGpuFromId(pParams->gpuTimingSource);
    if (pSourceGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pSourceGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->slaveGpuCount > NV30F1_CTRL_MAX_GPUS_PER_GSYNC)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    for (i = 0; i < pParams->slaveGpuCount; i++)
    {
        pTempGpu = gpumgrGetGpuFromId(pParams->gpuTimingSlaves[i]);
        if (pTempGpu == NULL)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        if(!gsyncIsGpuInGsync(pTempGpu, pGsync))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    status |= pGsync->gsyncHal.gsyncSetMosaic(pSourceGpu, pGsync->pExtDev, pParams);

    return status;
}

NV_STATUS
gsyncConfigFlash(OBJGSYNC *pGsync,
            NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status |= pGsync->gsyncHal.gsyncConfigFlashGsync(pGpu, pGsync->pExtDev,
                  pParams->preFlash);

    return status;
}

NV_STATUS
gsyncGetControlSync(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ( pParams->displays == 0 )
    {
        // This is the case where we want to query what is current.
        if ( pParams->master )
        {
            status |= pGsync->gsyncHal.gsyncRefMaster(pGpu, pGsync->pExtDev, refFetchGet,
                &pParams->displays, &pParams->refresh, NV_FALSE, NV_FALSE);
        }
        else
        {
            status |= pGsync->gsyncHal.gsyncRefSlaves(pGpu, pGsync->pExtDev, refFetchGet,
                &pParams->displays, &pParams->refresh);
        }
    }
    else
    {
        // This is where we want to see if the given Display is assignable
        // to this DisplaySync unit under the current conditions (mode, etc).
        // If there is no mode then this is definitely a failure.
        RM_API   *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NvU32     hClient     = pGpu->hInternalClient;
        NvU32     hSubdevice  = pGpu->hInternalSubdevice;
        NV2080_CTRL_INTERNAL_GSYNC_IS_DISPLAYID_VALID_PARAMS ctrlParams = {0};

        ctrlParams.displays = pParams->displays;
        status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_GSYNC_IS_DISPLAYID_VALID,
                                 &ctrlParams, sizeof(ctrlParams));

        if (status != NV_OK)
        {
            pParams->displays = 0; // Signals failure to start with.
                                   // If we find a head with this as its currenty bound
                                   // display then we'll signal success.
            NV_PRINTF(LEVEL_ERROR, "Extdev display IDs for the display doesn't exist!\n");
        }
        else
        {
            // Also check if this display is capable of being server
            pParams->master = pGsync->gsyncHal.gsyncGpuCanBeMaster(pGpu, pGsync->pExtDev);
            pParams->displays = ctrlParams.displayId;
        }
    }

    return status;
}

NV_STATUS
gsyncSetControlSync(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;
    NvU32 assigned, refresh;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->refresh == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->master)
    {
        NvBool skipSwapBarrierWar = !!(pParams->configFlags &
            NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_CONFIG_FLAGS_KEEP_MASTER_SWAPBARRIER_DISABLED);

        status |= pGsync->gsyncHal.gsyncRefMaster(pGpu, pGsync->pExtDev,
            refRead, &assigned, &refresh, NV_FALSE, NV_FALSE);
        pParams->displays |= assigned;
        status |= pGsync->gsyncHal.gsyncRefMaster(pGpu, pGsync->pExtDev,
            refSetCommit, &pParams->displays, &pParams->refresh, NV_FALSE, skipSwapBarrierWar);
    }
    else
    {
        status |= pGsync->gsyncHal.gsyncRefSlaves(pGpu, pGsync->pExtDev,
            refRead, &assigned, &refresh);
        pParams->displays |= assigned;
        status |= pGsync->gsyncHal.gsyncRefSlaves(pGpu, pGsync->pExtDev,
            refSetCommit, &pParams->displays, &pParams->refresh);
    }

    // Update watchdog here.
    // Only do that is client doesn't take care.
    if ((pGsync->bAutomaticWatchdogScheduling) && (NV_OK == status) &&
        (0 != pParams->displays))
    {
        NvU32 enable = 1;
        if (NV_OK != pGsync->gsyncHal.gsyncSetWatchdog(pGpu, pGsync->pExtDev, enable))
        {
            // Only assert as the master/slave assignment has succeded.
            NV_ASSERT(0);
        }
    }

    return status;
}

NV_STATUS
gsyncSetControlUnsync(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;
    NvU32 assigned, refresh;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->master)
    {
        status |= pGsync->gsyncHal.gsyncRefMaster(pGpu, pGsync->pExtDev,
            refRead, &assigned, &refresh, NV_FALSE, NV_FALSE);
        pParams->displays = assigned & ~pParams->displays;
        status |= pGsync->gsyncHal.gsyncRefMaster(pGpu, pGsync->pExtDev,
            refSetCommit, &pParams->displays, &refresh, !!pParams->retainMaster, NV_FALSE);
    }
    else
    {
        status |= pGsync->gsyncHal.gsyncRefSlaves(pGpu, pGsync->pExtDev,
            refRead, &assigned, &refresh);
        pParams->displays = assigned & ~pParams->displays;
        status |= pGsync->gsyncHal.gsyncRefSlaves(pGpu, pGsync->pExtDev,
            refSetCommit, &pParams->displays, &refresh);
    }

    // Update watchdog here: if no master or slave is
    // remaining on the gsync device, disable watchdog.
    // Only do that is client doesn't take care.
    if ((pGsync->bAutomaticWatchdogScheduling) &&
        (NV_OK == status) && (0 == pParams->displays) &&
        (!(gsyncIsAnyHeadFramelocked(pGsync))))
    {
        NvU32 enable = 0;
        // Only print erromessages as the
        if (NV_OK != pGsync->gsyncHal.gsyncSetWatchdog(pGpu, pGsync->pExtDev, enable))
        {
            // Only assert as the master/slave unassignment has succeded.
            NV_ASSERT(0);
        }
    }

    return status;
}

NV_STATUS
gsyncGetStatusSync(OBJGSYNC *pGsync, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if(!gsyncIsGpuInGsync(pGpu, pGsync))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status |= pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
              gsync_Status_bTiming, &pParams->bTiming);

    status |= pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
              gsync_Status_bStereoSync, &pParams->bStereoSync);

    status |= pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
              gsync_Status_bSyncReady, &pParams->bSyncReady);

    return status;
}


NV_STATUS
gsyncGetStatus(OBJGSYNC *pGsync, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_POLARITY)
    {
        GSYNCSYNCPOLARITY SyncPolarity = gsync_SyncPolarity_RisingEdge;

        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetSyncPolarity(pGpu, pGsync->pExtDev,
            &SyncPolarity));

        if (status == NV_OK)
        {
            pParams->bLeadingEdge = (SyncPolarity == gsync_SyncPolarity_BothEdges ||
                                    SyncPolarity == gsync_SyncPolarity_RisingEdge);

            pParams->bFallingEdge = (SyncPolarity == gsync_SyncPolarity_BothEdges ||
                                    SyncPolarity == gsync_SyncPolarity_FallingEdge);
        }
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_DELAY)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetSyncStartDelay(pGpu, pGsync->pExtDev,
            &pParams->syncDelay));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_REFRESH)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_Refresh, &pParams->refresh));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC_INCOMING)
    {
         NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_HouseSyncIncoming, &pParams->houseSyncIncoming));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_INTERVAL)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetNSync(pGpu, pGsync->pExtDev,
            &pParams->syncInterval));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_READY)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bSyncReady, &pParams->bSyncReady));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_SWAP_READY)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bSwapReady, &pParams->bSwapReady));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_HOUSE_SYNC)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bHouseSync, &pParams->bHouseSync));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_PORT_INPUT)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bPort0Input, &pParams->bPort0Input));
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bPort1Input, &pParams->bPort1Input));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_PORT_ETHERNET)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bPort0Ethernet, &pParams->bPort0Ethernet));
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bPort1Ethernet, &pParams->bPort1Ethernet));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_UNIVERSAL_FRAME_COUNT)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_UniversalFrameCount, &pParams->universalFrameCount));
    }

    if (pParams->which & NV30F1_CTRL_GSYNC_GET_STATUS_INTERNAL_SLAVE)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_INFO,
            pGsync->gsyncHal.gsyncGetCplStatus(pGpu, pGsync->pExtDev,
            gsync_Status_bInternalSlave, &pParams->bInternalSlave));
    }

    return status;
}

NV_STATUS
gsyncGetControlTesting(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    status = pGsync->gsyncHal.gsyncGetEmitTestSignal(pGpu, pGsync->pExtDev,
             &pParams->bEmitTestSignal);

    return status;
}

NV_STATUS
gsyncSetControlTesting(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    status = pGsync->gsyncHal.gsyncSetEmitTestSignal(pGpu, pGsync->pExtDev,
             pParams->bEmitTestSignal);

    return status;
}

NV_STATUS
gsyncSetControlWatchdog(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    // Client indicates it's taking care
    // and doesn't want any automatic solution.
    pGsync->bAutomaticWatchdogScheduling = NV_FALSE;

    status = pGsync->gsyncHal.gsyncSetWatchdog(pGpu, pGsync->pExtDev,
             pParams->enable);

    return status;
}

NV_STATUS
gsyncGetControlInterlaceMode(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    status = pGsync->gsyncHal.gsyncGetInterlaceMode(pGpu, pGsync->pExtDev,
             &pParams->enable);

    return status;
}

NV_STATUS
gsyncSetControlInterlaceMode(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    status = pGsync->gsyncHal.gsyncSetInterlaceMode(pGpu, pGsync->pExtDev,
             pParams->enable);

    return status;
}

NV_STATUS
gsyncGetControlSwapBarrier(OBJGSYNC *pGsync, OBJGPU *pGpu,
            NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS* pParams)
{
    REFTYPE rType = refFetchGet;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    status = pGsync->gsyncHal.gsyncRefSwapBarrier(pGpu, pGsync->pExtDev,
        rType, &pParams->enable);

    return status;
}

NV_STATUS
gsyncSetControlSwapBarrier(OBJGSYNC *pGsync, OBJGPU *pGpu,
            NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS* pParams)
{
    REFTYPE rType = refSetCommit;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    status = pGsync->gsyncHal.gsyncRefSwapBarrier(pGpu, pGsync->pExtDev,
        rType, &pParams->enable);

    return status;
}

NV_STATUS
gsyncGetControlSwapLockWindow(OBJGSYNC *pGsync,
               NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;
    NvU32 data = 0;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);
    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }

    if (osReadRegistryDword(pGpu,
        NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_SWAP_LOCKOUT_START, &data) != NV_OK)
    {
        // Bug 967618 - default value for tSwapRdyHi is 250 micro seconds.
        pParams->tSwapRdyHi =
          NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_SWAP_LOCKOUT_START_DEFAULT;
    }
    else
    {
        // regkey added value for tSwapRdyHi i.e. swap lock window.
        pParams->tSwapRdyHi = data;
    }

    return status;
}

NV_STATUS
gsyncGetOptimizedTiming(OBJGSYNC *pGsync,
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS* pParams)
{
    OBJGPU *pGpu = NULL;

    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status |= pGsync->gsyncHal.gsyncOptimizeTiming(pGpu, pParams);

    return status;
}

NV_STATUS
gsyncGetControlStereoLockMode(OBJGSYNC *pGsync,
            NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;
    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pGsync->gsyncHal.gsyncGetStereoLockMode(pGpu, pGsync->pExtDev,
           &pParams->enable);
}

NV_STATUS
gsyncSetControlStereoLockMode(OBJGSYNC *pGsync,
            NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;
    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gpumgrGetGpuFromId(pParams->gpuId);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pGsync->gsyncHal.gsyncSetStereoLockMode(pGpu, pGsync->pExtDev,
           pParams->enable);
}

static NV_STATUS
gsyncReadRegister(OBJGSYNC *pGsync,
                  NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    for (i = 0; i < NV30F1_CTRL_MAX_GPUS_PER_GSYNC && i < pGsync->gpuCount; i++)
    {
        if (pParams->gpuId == pGsync->gpus[i].gpuId)
        {
            pGpu = gpumgrGetGpuFromId(pGsync->gpus[i].gpuId);
            break;
        }
    }

    if (!pGpu)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return readregu008_extdeviceTargeted(pGpu,
                                         pGsync->pExtDev,
                                         pParams->reg,
                                         &pParams->data);
}

static NV_STATUS
gsyncWriteRegister(OBJGSYNC *pGsync,
                   NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;
    NvU32 i;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    for (i = 0; i < NV30F1_CTRL_MAX_GPUS_PER_GSYNC && i < pGsync->gpuCount; i++)
    {
        if (pParams->gpuId == pGsync->gpus[i].gpuId)
        {
            pGpu = gpumgrGetGpuFromId(pGsync->gpus[i].gpuId);
            break;
        }
    }

    if (!pGpu)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return writeregu008_extdeviceTargeted(pGpu,
                                          pGsync->pExtDev,
                                          pParams->reg,
                                          pParams->data);
}

NvBool
gsyncIsInstanceValid(NvU32 gsyncInstance)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync;
    OBJGPU *pGpu;
    DACEXTERNALDEVICE *pExtDev;

    if (gsyncInstance >= NV30F1_MAX_GSYNCS)
        return NV_FALSE;

    pGsync = &pGsyncMgr->gsyncTable[gsyncInstance];

    if (pGsync->gpuCount == 0)
        return NV_FALSE;

    pGpu = gsyncGetMasterableGpu(pGsync);
    // make sure we don't dereference a NULL ptr if the primary gpu
    // was already freed.
    if (pGpu == NULL)
        return NV_FALSE;

    pExtDev = pGsync->pExtDev;

    return pExtDev->pI->Validate(pGpu,pExtDev);
}


//
// gsyncSignalServiceRequested
//
// Something has happened on this Gsync.  Signal interested clients.
//
NV_STATUS
gsyncSignalServiceRequested(NvU32 gsyncInst, NvU32 eventFlags, NvU32 iface)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync;

    if (gsyncInst >= NV30F1_MAX_GSYNCS)
        return NV_ERR_GENERIC;

    pGsync = &pGsyncMgr->gsyncTable[gsyncInst];

    if (pGsync->gpuCount == 0)
        return NV_ERR_GENERIC;

    if (pGsync->bDoEventFiltering)
    {
        eventFlags = gsyncFilterEvents(eventFlags, iface);
    }

    CliNotifyGsyncEvent(gsyncInst, eventFlags);

    return NV_OK;
}

static NV_STATUS
gsyncGetHouseSyncMode(OBJGSYNC *pGsync,
                      NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    return pGsync->gsyncHal.gsyncGetHouseSyncMode(pGpu, pGsync->pExtDev,
                                                  &pParams->houseSyncMode);
}

static NV_STATUS
gsyncSetHouseSyncMode(OBJGSYNC *pGsync,
                      NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams)
{
    OBJGPU *pGpu = NULL;

    NV_ASSERT_OR_RETURN(pGsync && pGsync->pExtDev, NV_ERR_INVALID_DEVICE);

    pGpu = gsyncGetMasterableGpu(pGsync);

    return pGsync->gsyncHal.gsyncSetHouseSyncMode(pGpu, pGsync->pExtDev,
                                                  pParams->houseSyncMode);
}

static NvBool
gsyncNullGpuCanBeMaster
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    return NV_FALSE;
}

static NV_STATUS
gsyncNullOptimizeTimingParameters
(
    OBJGPU         *pGpu,
    GSYNCTIMINGPARAMS *pParams
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetStereoLockMode
(
    OBJGPU              *pGpu,
    PDACEXTERNALDEVICE   pExtDev,
    NvU32                *val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetStereoLockMode
(
    OBJGPU              *pGpu,
    PDACEXTERNALDEVICE   pExtDev,
    NvU32                val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetSyncPolarity
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCSYNCPOLARITY *polarity
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetSyncPolarity
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCSYNCPOLARITY polarity
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetVideoMode
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCVIDEOMODE *videoMode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetVideoMode
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCVIDEOMODE videoMode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetNSync
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *count
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetNSync
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 count
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetSyncSkew
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *skew
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetSyncSkew
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 skew
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetUseHouse
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetUseHouse
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetSyncStartDelay
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *delay
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetSyncStartDelay
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 delay
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetEmitTestSignal
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetEmitTestSignal
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 val
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetInterlaceMode
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *mode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetInterlaceMode
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 mode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullRefSwapBarrier
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 REFTYPE rType,
 NvBool *enable
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullRefSignal
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 REFTYPE rType,
 GSYNCSYNCSIGNAL Signal,
 NvBool bRate,
 NvU32 *pPresence
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullRefMaster
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 REFTYPE rType,
 NvU32 *pDisplayMask,
 NvU32 *pRefresh,
 NvBool retainMaster,
 NvBool skipSwapBarrierWar
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullRefSlaves
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 REFTYPE rType,
 NvU32 *pDisplayMasks,
 NvU32 *pRefresh
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetCplStatus
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCSTATUS status,
 NvU32 *pVal
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetWatchdog
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 pVal
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetRevision
(
 OBJGPU       *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 GSYNCCAPSPARAMS *pParams
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetMosaic
(
 OBJGPU *pSourceGpu,
 PDACEXTERNALDEVICE pExtDev,
 NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullConfigFlashGsync
(
 OBJGPU *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 preFlash
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetHouseSyncMode
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU8* mode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullSetHouseSyncMode
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU8 mode
)
{
    return NV_ERR_GENERIC;
}

static NV_STATUS
gsyncNullGetMulDiv
(
    OBJGPU       *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *pMulDivSettings
)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS
gsyncNullSetMulDiv
(
    OBJGPU       *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *pMulDivSettings
)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS
gsyncSetupNullProvider(OBJGSYNCMGR *pGsyncMgr, NvU32 gsyncInst)
{
    OBJGSYNC *pGsync;
    NV_STATUS status = NV_OK;

    NV_ASSERT(gsyncInst < NV30F1_MAX_GSYNCS);

    pGsync = &pGsyncMgr->gsyncTable[gsyncInst];

    pGsync->connectorCount                     = 0;

    pGsync->gsyncHal.gsyncGpuCanBeMaster       = gsyncNullGpuCanBeMaster;
    pGsync->gsyncHal.gsyncOptimizeTiming       = gsyncNullOptimizeTimingParameters;
    pGsync->gsyncHal.gsyncGetStereoLockMode    = gsyncNullGetStereoLockMode;
    pGsync->gsyncHal.gsyncSetStereoLockMode    = gsyncNullSetStereoLockMode;

    pGsync->gsyncHal.gsyncGetSyncPolarity      = gsyncNullGetSyncPolarity;
    pGsync->gsyncHal.gsyncSetSyncPolarity      = gsyncNullSetSyncPolarity;
    pGsync->gsyncHal.gsyncGetVideoMode         = gsyncNullGetVideoMode;
    pGsync->gsyncHal.gsyncSetVideoMode         = gsyncNullSetVideoMode;
    pGsync->gsyncHal.gsyncGetNSync             = gsyncNullGetNSync;
    pGsync->gsyncHal.gsyncSetNSync             = gsyncNullSetNSync;
    pGsync->gsyncHal.gsyncGetSyncSkew          = gsyncNullGetSyncSkew;
    pGsync->gsyncHal.gsyncSetSyncSkew          = gsyncNullSetSyncSkew;
    pGsync->gsyncHal.gsyncGetUseHouse          = gsyncNullGetUseHouse;
    pGsync->gsyncHal.gsyncSetUseHouse          = gsyncNullSetUseHouse;
    pGsync->gsyncHal.gsyncGetSyncStartDelay    = gsyncNullGetSyncStartDelay;
    pGsync->gsyncHal.gsyncSetSyncStartDelay    = gsyncNullSetSyncStartDelay;
    pGsync->gsyncHal.gsyncGetEmitTestSignal    = gsyncNullGetEmitTestSignal;
    pGsync->gsyncHal.gsyncSetEmitTestSignal    = gsyncNullSetEmitTestSignal;
    pGsync->gsyncHal.gsyncGetInterlaceMode     = gsyncNullGetInterlaceMode;
    pGsync->gsyncHal.gsyncSetInterlaceMode     = gsyncNullSetInterlaceMode;
    pGsync->gsyncHal.gsyncRefSwapBarrier       = gsyncNullRefSwapBarrier;
    pGsync->gsyncHal.gsyncRefSignal            = gsyncNullRefSignal;
    pGsync->gsyncHal.gsyncRefMaster            = gsyncNullRefMaster;
    pGsync->gsyncHal.gsyncRefSlaves            = gsyncNullRefSlaves;
    pGsync->gsyncHal.gsyncGetCplStatus         = gsyncNullGetCplStatus;
    pGsync->gsyncHal.gsyncSetWatchdog          = gsyncNullSetWatchdog;
    pGsync->gsyncHal.gsyncGetRevision          = gsyncNullGetRevision;
    pGsync->gsyncHal.gsyncSetMosaic            = gsyncNullSetMosaic;
    pGsync->gsyncHal.gsyncConfigFlashGsync     = gsyncNullConfigFlashGsync;
    pGsync->gsyncHal.gsyncGetHouseSyncMode     = gsyncNullGetHouseSyncMode;
    pGsync->gsyncHal.gsyncSetHouseSyncMode     = gsyncNullSetHouseSyncMode;
    pGsync->gsyncHal.gsyncGetMulDiv            = gsyncNullGetMulDiv;
    pGsync->gsyncHal.gsyncSetMulDiv            = gsyncNullSetMulDiv;

    return status;
}

//
// gsyncFilterEvents
//
// Perform event filtering.
// 1. Report stereo sync loss if both stereo sync loss and sync gain
//    happen at the same time.
// 2. Report sync loss if both stereo sync loss and sync loss
//    happen at the same time.
// 3. Report sync gain if both stero sync gain and sync gain
//    happen at the same time. (Bug 580086)
//
NvU32
gsyncFilterEvents
(
    NvU32 eventFlags,
    NvU32 iface
)
{
    if ((eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface))) &&
        (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface))))
    {
        // report only stereo sync loss
        eventFlags &= ~NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface));
    }

    if ((eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface))) &&
        (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface))))
    {
        // report only sync loss
        eventFlags &= ~NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface));
    }

    if ((eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface))) &&
        (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(iface))))
    {
        // report sync gain (bug 580086)
        eventFlags &= ~NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(iface));
    }

    return eventFlags;
}

//
// gsyncConvertNewEventToOldEventNum
//
// contevert new event numbers to old event numbers.
//
NvU32
gsyncConvertNewEventToOldEventNum
(
    NvU32 eventFlags
)
{
    NvU32 eventNum = 0;
    NvU32 isEventOccured = 0;
    NvU32 connectorCount;

    // SYNC_LOSS events
    for (connectorCount = 0; connectorCount < NV30F1_GSYNC_CONNECTOR_COUNT; connectorCount++)
    {
      isEventOccured |= (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(connectorCount)));
    }
    if (isEventOccured)
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_LOSS;
    }

    // SYNC_GAIN events
    isEventOccured = 0;
    for (connectorCount = 0; connectorCount < NV30F1_GSYNC_CONNECTOR_COUNT; connectorCount++)
    {
      isEventOccured |= (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(connectorCount)));
    }
    if (isEventOccured)
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_SYNC_GAIN;
    }

    // STEREO_GAIN events
    isEventOccured = 0;
    for (connectorCount = 0; connectorCount < NV30F1_GSYNC_CONNECTOR_COUNT; connectorCount++)
    {
      isEventOccured |= (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(connectorCount)));
    }
    if (isEventOccured)
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_GAIN;
    }

    // STEREO_LOSS events
    isEventOccured = 0;
    for (connectorCount = 0; connectorCount < NV30F1_GSYNC_CONNECTOR_COUNT; connectorCount++)
    {
      isEventOccured |= (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(connectorCount)));
    }
    if (isEventOccured)
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_STEREO_LOSS;
    }

    // HOUSE_GAIN events
    if (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_GAIN))
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_GAIN;
    }

    // HOUSE_LOSS events
    if (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_LOSS))
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_HOUSE_LOSS;
    }

    // RJ45_GAIN events
    if (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_GAIN))
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_GAIN;
    }

    // RJ45_LOSS events
    if (eventFlags & NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_LOSS))
    {
        eventNum = eventNum | NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_ACTION_SMART_RJ45_LOSS;
    }

    return eventNum;
}

#ifdef DEBUG
void
gsyncDbgPrintGsyncEvents(NvU32 events, NvU32 iface)
{
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface)))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SYNC_LOSS ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface)))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SYNC_GAIN ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface)))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STEREO_LOSS ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(iface)))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STEREO_GAIN ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_GAIN))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "HOUSE_GAIN ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_LOSS))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "HOUSE LOSS ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_GAIN))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "RJ45 GAIN ");
    if (events & NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_LOSS))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "RJ45 LOSS ");
}
#endif // DEBUG

