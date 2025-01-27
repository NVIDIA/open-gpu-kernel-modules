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

#include "nvrm_registry.h"
#include "gpu/timer/objtmr.h"
#include "gpu/external_device/gsync.h"
#include "gpu/external_device/dev_p2060.h"
#include "gpu/external_device/dac_p2060.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/disp/kern_disp.h"
#include "rmapi/rmapi_utils.h"
#include "class/cl402c.h" // NV40_I2C
#include "kernel/gpu/i2c/i2c_api.h"
#include "platform/sli/sli.h"
#include "nvmisc.h"
/*
 * statics
 */

static NvBool     GpuIsP2060Master   (OBJGPU *, PDACP2060EXTERNALDEVICE);
static NvBool     GpuIsP2060Connected(OBJGPU *, PDACP2060EXTERNALDEVICE);
static NvBool     GpuIsMosaicTimingSlave(OBJGPU *, PDACP2060EXTERNALDEVICE);
static NvBool     GpuIsConnectedToMasterViaBridge(OBJGPU *, PDACP2060EXTERNALDEVICE);

static OBJGPU*    GetP2060MasterableGpu (OBJGPU *, PDACP2060EXTERNALDEVICE);
static OBJGPU*    GetP2060WatchdogGpu   (OBJGPU *, PDACP2060EXTERNALDEVICE);

static NV_STATUS  GetP2060GpuLocation   (OBJGPU *, PDACP2060EXTERNALDEVICE, NvU32*);
static NV_STATUS  GetP2060ConnectorIndexFromGpu (OBJGPU *, PDACP2060EXTERNALDEVICE, NvU32*);
static NvU32      GetP2060GpuSnapshot   (OBJGPU *, PDACP2060EXTERNALDEVICE);

static void       gsyncProgramFramelockEnable_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE, NvU32, NvBool);
static NvBool     gsyncIsStereoEnabled_p2060 (OBJGPU *, PDACEXTERNALDEVICE);
static NV_STATUS  gsyncProgramExtStereoPolarity_P2060 (OBJGPU *, PDACEXTERNALDEVICE);

static NV_STATUS  gsyncProgramSlaves_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE, NvU32);
static NvU32      gsyncReadSlaves_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE);
static NV_STATUS  gsyncProgramMaster_P2060(OBJGPU *, OBJGSYNC *, NvU32, NvBool, NvBool);
static NvU32      gsyncReadMaster_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE);

static NV_STATUS  gsyncUpdateGsyncStatusSnapshot_P2060(OBJGPU *, PDACEXTERNALDEVICE);

static void       gsyncCancelWatchdog_P2060(PDACP2060EXTERNALDEVICE);
static NV_STATUS  gsyncDisableFrameLockInterrupt_P2060(PDACEXTERNALDEVICE);
static NV_STATUS  gsyncEnableFramelockInterrupt_P2060(PDACEXTERNALDEVICE);
static NV_STATUS  gsyncDisableNonFramelockInterrupt_P2060(OBJGPU *, PDACEXTERNALDEVICE);
static NV_STATUS  gsyncEnableNonFramelockInterrupt_P2060(OBJGPU *, PDACEXTERNALDEVICE);
static void       gsyncResetMosaicData_P2060(NvU32, PDACP2060EXTERNALDEVICE);
static NV_STATUS  gsyncUpdateSwapRdyConnectionForGpu_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE, NvBool);

static NvBool     gsyncIsFrameLocked_P2060(PDACP2060EXTERNALDEVICE);
static NvBool     gsyncIsOnlyFrameLockMaster_P2060(PDACP2060EXTERNALDEVICE);
static NvBool     gsyncIsFrameLockMaster_P2060(PDACP2060EXTERNALDEVICE);
static NvBool     gsyncIsP2060MasterBoard(OBJGPU *, PDACP2060EXTERNALDEVICE);

static NV_STATUS  gsyncSetLsrMinTime(OBJGPU *, PDACEXTERNALDEVICE, NvU32);

static NV_STATUS  gsyncUpdateFrameCount_P2060(PDACP2060EXTERNALDEVICE, OBJGPU *);
static NvU32      gsyncGetNumberOfGpuFrameCountRollbacks_P2060(NvU32, NvU32, NvU32);
static NV_STATUS  gsyncFrameCountTimerService_P2060(OBJGPU *, OBJTMR *, TMR_EVENT *);
static NV_STATUS  gsyncResetFrameCountData_P2060(OBJGPU *, PDACP2060EXTERNALDEVICE);

static NV_STATUS  gsyncGpuStereoHeadSync(OBJGPU *, NvU32, PDACEXTERNALDEVICE, NvU32);
static NvBool     supportsMulDiv(DACEXTERNALDEVICE *);
static NvBool     needsMasterBarrierWar(PDACEXTERNALDEVICE);
static NvBool     isFirmwareRevMismatch(OBJGPU *, OBJGSYNC *pGsync, DAC_EXTERNAL_DEVICE_REVS);

static NvBool     isBoardWithNvlinkQsyncContention(OBJGPU *);
static void       _extdevService(NvU32 , void *);

NvBool
extdevGetDevice_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExternalDevice
)
{
    NvU8 revId;
    NvU8 data;
    DAC_EXTERNAL_DEVICES externalDeviceId;
    DAC_EXTERNAL_DEVICE_REVS externalDeviceRev;
    NV_STATUS status;

    if (!RMCFG_FEATURE_EXTDEV_GSYNC_P2060 ||
        IS_EMULATION(pGpu) || IS_SIMULATION(pGpu))
    {
        return NV_FALSE;
    }

    // Read the FPGA revision register
    status = readregu008_extdevice(pGpu, pExternalDevice, (NvU8)NV_P2060_FPGA, &data);
    if (status != NV_OK)
    {
        return NV_FALSE;
    }
    revId = data;

    // Decode the register value into device ID
    if (DRF_VAL(_P2060, _FPGA, _ID, data) == NV_P2060_FPGA_ID_5)
    {
        externalDeviceId = DAC_EXTERNAL_DEVICE_P2060;
    }
    else if (DRF_VAL(_P2061, _FPGA, _ID, data) == NV_P2061_FPGA_ID_4)
    {
        externalDeviceId = DAC_EXTERNAL_DEVICE_P2061;
    }
    else
    {
        return NV_FALSE;
    }

    // Decode the register value into device revision (major revision)
    externalDeviceRev = DRF_VAL(_P2060, _FPGA, _REV, data);

    // Read device extended revision (minor revision)
    status = readregu008_extdevice(pGpu, pExternalDevice, (NvU8)NV_P2060_FPGA_EXREV, &data);
    if (status != NV_OK)
    {
        return NV_FALSE;
    }

    // Caching revId, device ID, device revision, and device extended revision
    pExternalDevice->revId = revId;
    pExternalDevice->deviceId = externalDeviceId;
    pExternalDevice->deviceRev = externalDeviceRev;
    pExternalDevice->deviceExRev = data;

    return NV_TRUE;
}

/*
 * Return Extdev with setting of the data structures and
 * function pointers for P2060.
 */
PDACEXTERNALDEVICE
extdevConstruct_P2060
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pExternalDevice
)
{
    DACP2060EXTERNALDEVICE *pThis = (PDACP2060EXTERNALDEVICE)pExternalDevice;
    KernelDisplay          *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32 iface, head, i;
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    if ( !extdevConstruct_Base(pGpu, pExternalDevice) )
    {
        return 0;
    }

    // Setup interfaces
    pThis->ExternalDevice.pI->Destroy = extdevDestroy_P2060;
    pThis->ExternalDevice.pI->Attach  = gsyncAttachExternalDevice_P2060;

    pThis->ExternalDevice.pI->GetDevice             = extdevGetDevice_P2060;
    pThis->ExternalDevice.pI->Init                  = extdevInit_P2060;

    pThis->ExternalDevice.pI->Service               = extdevService_P2060;
    pThis->ExternalDevice.pI->Watchdog              = extdevWatchdog_P2060;
    pThis->ExternalDevice.pI->setI2cHandles         = extdevSaveI2cHandles_P2060;


    // Init data members
    pThis->ExternalDevice.I2CAddr     = 0x20;
    pThis->ExternalDevice.I2CPort     = pGpu->i2cPortForExtdev;
    pThis->ExternalDevice.MaxGpus     = NV_P2060_MAX_IFACES_PER_GSYNC * NV_P2060_MAX_GPUS_PER_IFACE;

    pThis->gpuAttachMask = 0;
    pThis->id = 0;
    pThis->watchdogCountDownValue = 0;
    pThis->isNonFramelockInterruptEnabled = NV_FALSE;
    pThis->interruptEnabledInterface = 0;
    pThis->tSwapRdyHi = 0;
    pThis->tSwapRdyHiLsrMinTime = 0;

    //init FrameCountData
    pThis->FrameCountData.totalFrameCount               = 0;
    pThis->FrameCountData.currentFrameCount             = 0;
    pThis->FrameCountData.initialDifference             = 0;
    pThis->FrameCountData.numberOfRollbacks             = 0;
    pThis->FrameCountData.previousFrameCount            = 0;
    pThis->FrameCountData.lastFrameCounterQueryTime     = 0;
    pThis->FrameCountData.bReCheck                      = 0;
    pThis->FrameCountData.vActive                       = 0;
    pThis->FrameCountData.isFrmCmpMatchIntMasterEnabled = NV_FALSE;
    pThis->FrameCountData.enableFrmCmpMatchIntSlave     = NV_FALSE;
    pThis->FrameCountData.head                          = NV_P2060_MAX_HEADS_PER_GPU;
    pThis->FrameCountData.iface                         = NV_P2060_MAX_IFACES_PER_GSYNC;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        pThis->Iface[iface].GpuInfo.gpuId     = NV0000_CTRL_GPU_INVALID_ID;
        pThis->Iface[iface].GpuInfo.connected = NV_FALSE;

        pThis->Iface[iface].RasterSyncGpio.saved   = NV_FALSE;
        pThis->Iface[iface].DsiFliplock.saved      = NV_FALSE;

        for (head = 0; head < numHeads; head++)
        {
            pThis->Iface[iface].Sync.Master[head]     = 0;
            pThis->Iface[iface].Sync.Slaved[head]     = 0;
            pThis->Iface[iface].Sync.LocalSlave[head] = 0;
        }

        pThis->Iface[iface].lastEventNotified = 0;
        pThis->Iface[iface].gainedSync        = 0;

        pThis->i2cHandles[iface].hClient       = 0;
        pThis->i2cHandles[iface].hDevice       = 0;
        pThis->i2cHandles[iface].hSubdevice    = 0;
        pThis->i2cHandles[iface].hSubscription = 0;
        pThis->i2cHandles[iface].gpuId         = 0;
    }

    //init MosaicData
    for (i = 0; i < NV_P2060_MAX_MOSAIC_GROUPS; i++)
    {
        gsyncResetMosaicData_P2060(i, pThis);
    }

    return pExternalDevice;
}

/*
 * setup device registers
 */
static void
_externalDeviceInit_P2060
(
    OBJGPU            *pGpu,
    NvBool             bExtDevFound
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    RM_API   *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32     hClient     = pGpu->hInternalClient;
    NvU32     hSubdevice  = pGpu->hInternalSubdevice;
    NV_STATUS status      = NV_OK;
    NV2080_CTRL_INTERNAL_GSYNC_ATTACH_AND_INIT_PARAMS ctrlParams = {0};

    ctrlParams.bExtDevFound = bExtDevFound;

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_GSYNC_ATTACH_AND_INIT,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Extdev GPIO interrupt enable failed\n");
    }
    else
    {
        pKernelDisplay->bExtdevIntrSupported = NV_TRUE;
    }

    return;
}

NV_STATUS
gsyncFindGpuHandleLocation
(
    DACEXTERNALDEVICE      *pExternalDevice,
    NvU32                   gpuId,
    NvU32                  *iface
)
{
    DACP2060EXTERNALDEVICE *pThis = (PDACP2060EXTERNALDEVICE)pExternalDevice;
    NvU32 tempIface;
    NV_STATUS rmStatus = NV_ERR_GENERIC;

    for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
    {
        if (pThis->i2cHandles[tempIface].gpuId == gpuId)
        {
            *iface = tempIface;
            rmStatus = NV_OK;
        }
    }

    return rmStatus;
}

static NV_STATUS
gsyncFindFreeHandleLocation
(
    DACP2060EXTERNALDEVICE *pThis,
    NvU32                  *iface
)
{
    NvU32 tempIface;
    NV_STATUS rmStatus = NV_ERR_GENERIC;

    for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
    {
        if (pThis->i2cHandles[tempIface].gpuId == 0)
        {
            *iface = tempIface;
            rmStatus = NV_OK;
        }
    }

    return rmStatus;
}

NvBool
extdevSaveI2cHandles_P2060
(
    OBJGPU             *pGpu,
    DACEXTERNALDEVICE  *pExternalDevice
)
{
    DACP2060EXTERNALDEVICE *pThis = (PDACP2060EXTERNALDEVICE)pExternalDevice;
    RM_API                 *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle                hClient;
    NvHandle                hDevice;
    NvHandle                hSubdevice;
    NvHandle                hSubscription = NV01_NULL_OBJECT;
    NvU32                   iface;
    NV_STATUS               rmStatus;

    rmStatus = gsyncFindFreeHandleLocation(pThis, &iface);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Maximum number of GPUs have been attached\n");
        return NV_FALSE;
    }

    rmStatus = rmapiutilAllocClientAndDeviceHandles(pRmApi,
                                                    pGpu, &hClient, &hDevice, &hSubdevice);
    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, NV_FALSE);

    rmStatus = pRmApi->Alloc(pRmApi, hClient, hSubdevice,
                            &hSubscription, NV40_I2C, NULL, 0);

    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, NV_FALSE);

    pThis->i2cHandles[iface].hClient       = hClient;
    pThis->i2cHandles[iface].hDevice       = hDevice;
    pThis->i2cHandles[iface].hSubdevice    = hSubdevice;
    pThis->i2cHandles[iface].hSubscription = hSubscription;
    pThis->i2cHandles[iface].gpuId         = pGpu->gpuId;

    return NV_TRUE;
}

NV_STATUS
i2c_extdeviceHelper
(
    OBJGPU            *pGpu,
    DACEXTERNALDEVICE *pExternalDevice,
    NvU32              i2cPort,
    NvU8               SubAdr,
    NvU8              *pData,
    NvBool             write
)
{
    RM_API                 *pRmApi  = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    DACP2060EXTERNALDEVICE *pThis   = (PDACP2060EXTERNALDEVICE)pExternalDevice;
    NV_STATUS               status  = NV_ERR_GENERIC;
    NvU32                   iface;
    NV402C_CTRL_I2C_TRANSACTION_PARAMS *pParams;

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = gsyncFindGpuHandleLocation(pExternalDevice, pGpu->gpuId, &iface);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Couldn't find saved GPU entry, check saved i2chandles. \n");
        return status;
    }

    portMemSet(pParams, 0, sizeof(*pParams));

    pParams->portId        = (NvU8)i2cPort;
    pParams->transType     = NV402C_CTRL_I2C_TRANSACTION_TYPE_SMBUS_BYTE_RW;
    pParams->deviceAddress = (NvU16)pExternalDevice->I2CAddr;

    pParams->transData.smbusByteData.bWrite = write;
    pParams->transData.smbusByteData.registerAddress = SubAdr;
    if (write)
    {
        pParams->transData.smbusByteData.message = *pData;
    }

    status = pRmApi->Control(pRmApi, pThis->i2cHandles[iface].hClient,
                             pThis->i2cHandles[iface].hSubscription,
                             NV402C_CTRL_CMD_I2C_TRANSACTION,
                             pParams, sizeof(*pParams));

    if (!write)
    {
        *pData = pParams->transData.smbusByteData.message;
    }

    portMemFree(pParams);

    return status;
}

/*
 * Initialize P2060 for all GPUs in loop.
 */
NvBool
extdevInit_P2060
(
    OBJGPU  *pGpu,
    PDACEXTERNALDEVICE pExternalDevice
)
{
    OBJGPU  *pGpuTemp;
    OBJSYS  *pSys = SYS_GET_INSTANCE();
    OBJGSYNC *pGsyncTemp = NULL;
    NvU32 gpuAttachCnt, gpuAttachMask, gpuInstance;
    NvU32 data;

    if (!GpuIsP2060Connected(pGpu, (PDACP2060EXTERNALDEVICE)pExternalDevice))
    {
        return NV_FALSE;
    }

    if (NV_OK != gsyncProgramExtStereoPolarity_P2060(pGpu, pExternalDevice))
    {
        return NV_FALSE;
    }

    // Check regkeys
    if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_QSYNC_FW_REV_CHECK, &data))
    {
        if (NV_REG_STR_RM_QSYNC_FW_REV_CHECK_DISABLE == data)
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED, NV_TRUE);
        }
    }

    // Initialize SyncPolarity to FALLING_EDGE. Refer Bug 1035880
    if (NV_OK != gsyncSetSyncPolarity_P2060(pGpu, pExternalDevice, gsync_SyncPolarity_FallingEdge))
    {
        return NV_FALSE;
    }

    // get count of all other gpus in the system
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

    // loop over
    gpuInstance = 0;
    while ((pGpuTemp = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
    {
        pGsyncTemp = gsyncmgrGetGsync(pGpuTemp);

        if (!pGsyncTemp || !pGsyncTemp->pExtDev)
            continue;

        _externalDeviceInit_P2060(pGpuTemp, (pGpu == pGpuTemp));
    }

    return NV_TRUE;
}

static NV_STATUS
gsyncReadBoardId_P2060
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pExternalDevice,
    NvU32              *uniqueId
)
{
    NvU8  i, id = 0;
    NV_STATUS rmStatus = NV_OK;

    *uniqueId = 0;
    for (i = 0; i < 4; i++)
    {
        rmStatus = readregu008_extdeviceTargeted(pGpu, pExternalDevice,
                                  NV_P2060_FPGA_ASGN_ID(i), &id);
        if (rmStatus != NV_OK)
        {
            return rmStatus;
        }
        *uniqueId |= id << (i * 8);
    }
    return NV_OK;
}

/*
 * Attach P2060 to GPU on correct connector index.
 */
NvBool
gsyncAttachExternalDevice_P2060
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE *ppExtdevs
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJGSYNCMGR *pGsyncMgr = SYS_GET_GSYNCMGR(pSys);
    OBJGSYNC *pGsync = NULL;
    OBJGPU   *pOtherGpu = NULL;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    DACP2060EXTERNALDEVICE *pThis, *pExt2060Temp;
    PDACEXTERNALDEVICE pExtdev;
    NvU8  i, id = 0, regCtrl2 = 0;
    NvU32 iface, connector, uniqueId = 0, pOtherGpuId = 0, bSkipResetForVM = 0, index = 0;
    NvU32 gpuInstance = gpuGetInstance(pGpu);
    NvU32 tempIface;
    NvBool bExtDevFound = NV_FALSE;
    NV_STATUS rmStatus = NV_OK;
    NvU8 ctrl = 0;

    rmStatus = gsyncReadBoardId_P2060(pGpu, *ppExtdevs, &uniqueId);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to read P2060 device Id.\n");
        return NV_FALSE;
    }

    if (uniqueId != 0x0)
    {
        // HW says another GPU has been here first. Confirm this from SW.
        for (i = 0; i < NV30F1_MAX_GSYNCS; i++)
        {
            if (pGsyncMgr->gsyncTable[i].gpuCount)
            {
                pGsync = &pGsyncMgr->gsyncTable[i];
                if (pGsync->pExtDev)
                {
                    pThis  = (PDACP2060EXTERNALDEVICE) pGsync->pExtDev;
                    if (pThis->id == uniqueId)
                    {
                        pOtherGpuId = pGsync->gpus[0].gpuId;
                        bExtDevFound = NV_TRUE;
                    }
                }
            }

            if (bExtDevFound)
            {
                break;
            }
        }

        if (!bExtDevFound)
        {
            if ((IS_PASSTHRU(pGpu)))
            {
                // look for master board
                rmStatus = readregu008_extdeviceTargeted(pGpu, *ppExtdevs,
                                                            (NvU8)NV_P2060_CONTROL, &ctrl);
                if (rmStatus != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "Failed to read Ctrl data.\n");
                    return NV_FALSE;
                }

                pThis = (PDACP2060EXTERNALDEVICE)*ppExtdevs;
                bSkipResetForVM = FLD_TEST_DRF(_P2060, _CONTROL, _I_AM, _MASTER, (NvU32)ctrl);
                rmStatus = GetP2060ConnectorIndexFromGpu(pGpu, pThis, &index);

                if (rmStatus != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "Failed to get connector index for Gpu.\n");
                    return NV_FALSE;
                }

                // Look for SYNC source gpu.
                bSkipResetForVM = bSkipResetForVM && !(DRF_VAL(_P2060, _CONTROL, _SYNC_SRC, (NvU32)ctrl) == index);
            }

            if (!bSkipResetForVM)
            {
                // ExtDev is not preset in pGsyncMgr. Issue RESET to P2060 HW.
                regCtrl2 = FLD_SET_DRF_NUM(_P2060, _CONTROL2, _RESET, NV_TRUE, regCtrl2);
                writeregu008_extdeviceTargeted(pGpu, *ppExtdevs,
                    NV_P2060_CONTROL2, regCtrl2);

                osDelay(5); // Add delay of 5ms before I2C read as board is in reset phase.

                rmStatus = gsyncReadBoardId_P2060(pGpu, *ppExtdevs, &uniqueId);
                if ((rmStatus != NV_OK) || (uniqueId != 0))
                {
                    NV_PRINTF(LEVEL_ERROR,
                        "failed to read P2060 device Id after reset.\n");
                    return NV_FALSE;
                }
            }
        }
        else
        {
            pOtherGpu = gpumgrGetGpuFromId(pOtherGpuId);
            pGsync    = gsyncmgrGetGsync(pOtherGpu);

            if (!pGsync || !pGsync->pExtDev)
            {
                NV_ASSERT(0);
                return NV_FALSE;
            }

            NV_ASSERT(pGsync->pExtDev != *ppExtdevs);

            pThis = (PDACP2060EXTERNALDEVICE)*ppExtdevs;
            pExt2060Temp = (PDACP2060EXTERNALDEVICE)(pGsync->pExtDev);

            rmStatus = gsyncFindFreeHandleLocation(pExt2060Temp, &iface);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to free index for new GPU entry. \n");
                return NV_FALSE;
            }

            rmStatus = gsyncFindGpuHandleLocation(*ppExtdevs, pGpu->gpuId, &tempIface);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Couldn't find saved GPU entry, check extdevSaveI2cHandles. \n");
                return NV_FALSE;
            }

            pExt2060Temp->i2cHandles[iface].gpuId         = pThis->i2cHandles[tempIface].gpuId;
            pExt2060Temp->i2cHandles[iface].hClient       = pThis->i2cHandles[tempIface].hClient;
            pExt2060Temp->i2cHandles[iface].hDevice       = pThis->i2cHandles[tempIface].hDevice;
            pExt2060Temp->i2cHandles[iface].hSubdevice    = pThis->i2cHandles[tempIface].hSubdevice;
            pExt2060Temp->i2cHandles[iface].hSubscription = pThis->i2cHandles[tempIface].hSubscription;

            pThis->ExternalDevice.pI->Destroy(pGpu, *ppExtdevs);

            // Free our current pointer and replace it
            portMemFree(*ppExtdevs);
            *ppExtdevs = pGsync->pExtDev;
        }
    }

    pExtdev = *ppExtdevs;
    pThis = (PDACP2060EXTERNALDEVICE)pExtdev;

    if (uniqueId == 0x0)
    {
        // Use pGpu->gpuId as unique value.
        uniqueId = pGpu->gpuId;

        for (i = 0; i < 4; i++)
        {
            id = (NvU8)(uniqueId >> (i * 8));
            rmStatus = writeregu008_extdeviceTargeted(pGpu, *ppExtdevs,
                                                      NV_P2060_FPGA_ASGN_ID(i),
                                                      id);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to update P2060 device Id.\n");
                return NV_FALSE;
            }
        }
        pThis->id = uniqueId;
    }

    rmStatus = GetP2060ConnectorIndexFromGpu(pGpu, pThis, &iface);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to find P2060 connector.\n");
        return NV_FALSE;
    }

    //
    // Add 1 to index we got from status2 register read beacuse connector
    // NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_ONE start with value 1.
    //
    connector = iface + 1;

    //
    // Create timer events per GPU attached to a DACEXTERNALDEVICE instance.
    // These events should be deleted when a GPU instance decrements its
    // reference to a DACEXTERNALDEVICE instance. This is not done in the base
    // instance that utilizes the timer events since the base uses a nop
    // attachment logic that will not be executed by the child instance types.
    //

    rmStatus = tmrEventCreate(pTmr,
                              &pExtdev->WatchdogControl.pTimerEvents[gpuInstance],
                              extdevServiceWatchdog,
                              pExtdev,
                              TMR_FLAG_RECUR);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to create P2060 watchdog timer event.\n");
        return NV_FALSE;
    }

    rmStatus = tmrEventCreate(pTmr,
                              &pThis->FrameCountData.pTimerEvents[gpuInstance],
                              gsyncFrameCountTimerService_P2060,
                              pThis,
                              TMR_FLAG_RECUR);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to create P2060 frame count timer event.\n");
        goto fail_tmr_event_create;
    }

    //
    // If adding a check before the gsyncAttachGpu call and returning before
    // that please add the following code:
    // pThis->gpuAttachMask &= ~NVBIT(pGpu->gpuInstance);
    //    (*ppExtdevs)->ReferenceCount--;
    // before returning NV_FALSE so that the caller can destroy the
    // ext device structure. The destroy funciton only decrements the ref count
    // if the gpu has already been attached.
    //
    (*ppExtdevs)->ReferenceCount++;
    pThis->gpuAttachMask |= NVBIT(pGpu->gpuInstance);

    pThis->Iface[iface].GpuInfo.gpuId = pGpu->gpuId;
    pThis->Iface[iface].GpuInfo.connected = NV_TRUE;

    rmStatus = gsyncAttachGpu(*ppExtdevs, pGpu, connector, NULL, (*ppExtdevs)->deviceId);

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to attach P2060 gsync to gpu.\n");
        goto fail_tmr_event_create;
    }

    if (pThis->ExternalDevice.deviceId == DAC_EXTERNAL_DEVICE_P2061)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_QSYNC_II_ATTACHED, NV_TRUE);
    }
    else
    {
        NV_ASSERT(pThis->ExternalDevice.deviceId == DAC_EXTERNAL_DEVICE_P2060);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_GSYNC_III_ATTACHED, NV_TRUE);
    }

    if (!pThis->isNonFramelockInterruptEnabled)
    {
        rmStatus = gsyncEnableNonFramelockInterrupt_P2060(pGpu, *ppExtdevs);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to enable non-framelock interrupts on gsync GPU.\n");
            goto fail_tmr_event_create;
        }
        pThis->isNonFramelockInterruptEnabled = NV_TRUE;
        pThis->interruptEnabledInterface = iface;
    }

    return NV_TRUE;

fail_tmr_event_create:
    tmrEventDestroy(pTmr, pThis->FrameCountData.pTimerEvents[gpuInstance]);
    pThis->FrameCountData.pTimerEvents[gpuInstance] = NULL;
    tmrEventDestroy(pTmr, pExtdev->WatchdogControl.pTimerEvents[gpuInstance]);
    pExtdev->WatchdogControl.pTimerEvents[gpuInstance] = NULL;
    return NV_FALSE;
}

/*
 * Destroy the device P2060.
 */
void
extdevDestroy_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExternalDevice
)
{
    DACP2060EXTERNALDEVICE *pThis = (PDACP2060EXTERNALDEVICE)pExternalDevice;
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32 iface, head;
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);
    NvU8 ctrl2 = 0;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS rmStatus;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId == pGpu->gpuId)
        {
            NvU32 gpuInstance = gpuGetInstance(pGpu);

            pThis->gpuAttachMask &= ~NVBIT(pGpu->gpuInstance);
            pExternalDevice->ReferenceCount--;

            tmrEventDestroy(pTmr, pThis->FrameCountData.pTimerEvents[gpuInstance]);
            pThis->FrameCountData.pTimerEvents[gpuInstance] = NULL;
            tmrEventDestroy(pTmr, pExternalDevice->WatchdogControl.pTimerEvents[gpuInstance]);
            pExternalDevice->WatchdogControl.pTimerEvents[gpuInstance] = NULL;

            if (pThis->Iface[iface].GpuInfo.connected)
            {
                if (pExternalDevice->ReferenceCount == 0)
                {
                    // clear id for this gsync device.
                    pThis->id = 0;

                    // reset the gsync hw
                    ctrl2 = FLD_SET_DRF_NUM(_P2060, _CONTROL2, _RESET, NV_TRUE, ctrl2);
                    writeregu008_extdeviceTargeted(pGpu, pExternalDevice,
                                                   NV_P2060_CONTROL2, ctrl2);
                }
            }

            // Restore saved swap lockout window values that may not have
            // been restored by disabling swap barriers.
            if (pThis->Iface[iface].DsiFliplock.saved == NV_TRUE)
            {
                for (head = 0; head < numHeads; head++)
                {
                    kdispRestoreOriginalLsrMinTime_HAL(pGpu, pKernelDisplay, head,
                    pThis->Iface[iface].DsiFliplock.OrigLsrMinTime[head]);
                }
                pThis->Iface[iface].DsiFliplock.saved = NV_FALSE;
            }

            gsyncRemoveGpu(pGpu);

            pThis->Iface[iface].GpuInfo.gpuId = NV0000_CTRL_GPU_INVALID_ID;
            pThis->Iface[iface].GpuInfo.connected = NV_FALSE;

            pGpu->setProperty(pGpu, PDB_PROP_GPU_GSYNC_III_ATTACHED, NV_FALSE);
            pGpu->setProperty(pGpu, PDB_PROP_GPU_QSYNC_II_ATTACHED, NV_FALSE);

            rmStatus = gsyncFindGpuHandleLocation(pExternalDevice, pGpu->gpuId, &iface);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Couldn't find saved GPU entry, check saved i2chandles. \n");
                goto cleanup;
            }

            rmapiutilFreeClientAndDeviceHandles(pRmApi,
                                                &pThis->i2cHandles[iface].hClient,
                                                &pThis->i2cHandles[iface].hDevice,
                                                &pThis->i2cHandles[iface].hSubdevice);

            pThis->i2cHandles[iface].hClient   = 0;
            pThis->i2cHandles[iface].hDevice   = 0;
            pThis->i2cHandles[iface].hSubdevice    = 0;
            pThis->i2cHandles[iface].hSubscription = 0;
            pThis->i2cHandles[iface].gpuId   = 0;

            break;
        }
    }

cleanup:
    if (pExternalDevice->ReferenceCount == 0)
    {
        // And continue the chain running.
        extdevDestroy_Base(pGpu, pExternalDevice);
    }
}

/*
 * Handles the loss/gain of sync and other interrupts.
 */
void
extdevService_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU8               lossRegStatus,
    NvU8               gainRegStatus,
    NvU8               miscRegStatus,
    NvBool             rmStatus
)
{
    EXTDEV_INTR_DATA intrData;

    if (!rmStatus)
    {
        return;
    }

    intrData.lossRegStatus = lossRegStatus;
    intrData.gainRegStatus = gainRegStatus;
    intrData.miscRegStatus = miscRegStatus;
    intrData.pExtDevice    = pExtDev;

    if (IS_GSP_CLIENT(pGpu))
    {
        EXTDEV_INTR_DATA *workerThreadData = NULL;

        workerThreadData = portMemAllocNonPaged(sizeof(EXTDEV_INTR_DATA));
        if (NULL != workerThreadData)
        {
            *workerThreadData = intrData;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Memalloc failed\n");
        }

        // Attempt to queue a work item.
        if (NV_OK != osQueueWorkItemWithFlags(pGpu,
                                              _extdevService,
                                              (void *)workerThreadData,
                                              OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE))
        {
            portMemFree((void *)workerThreadData);
        }
    }
    else
    {
        _extdevService(gpuGetInstance(pGpu), (void *)&intrData);
    }
}

static void
_extdevService
(
    NvU32 gpuInstance,
    void *workerThreadData
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NV_STATUS rmStatus;

    EXTDEV_INTR_DATA intrData = *(EXTDEV_INTR_DATA *)workerThreadData;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)intrData.pExtDevice;
    NvU32 iface, ifaceEvents[NV_P2060_MAX_IFACES_PER_GSYNC];

    rmStatus = gsyncUpdateGsyncStatusSnapshot_P2060(pGpu, intrData.pExtDevice);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Couldn't raad the register status physical RMs.\n");
        return;
    }
    rmStatus = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot get P2060 Gpu location for serving interrupt.\n");
        return;
    }

    ifaceEvents[iface] = 0x00;

    if (intrData.lossRegStatus) //lost signal interrupts
    {
        if (DRF_VAL(_P2060, _STATUS4, _SYNC, (NvU32)intrData.lossRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface));
        }
        if (DRF_VAL(_P2060, _STATUS4, _STEREO, (NvU32)intrData.lossRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface));
        }
        if (DRF_VAL(_P2060, _STATUS4, _HS, (NvU32)intrData.lossRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_LOSS);
        }
        if (DRF_VAL(_P2060, _STATUS4, _RJ45, (NvU32)intrData.lossRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_LOSS);
        }

        //
        // Enable the watchdog again if we got any loss events.
        // Otherise sync gain tracking and stereo sync won't work as desired.
        //
        if (ifaceEvents[iface])
        {
            extdevScheduleWatchdog(pGpu, (PDACEXTERNALDEVICE)pThis);
            if (!gsyncIsOnlyFrameLockMaster_P2060(pThis))
            {
                pThis->watchdogCountDownValue = NV_P2060_WATCHDOG_COUNT_DOWN_VALUE;
            }
        }

        if (ifaceEvents[iface] && (pThis->Iface[iface].lastEventNotified != ifaceEvents[iface]))
        {
             gsyncSignalServiceRequested(gsyncGetGsyncInstance(pGpu), ifaceEvents[iface], iface);
             pThis->Iface[iface].lastEventNotified = ifaceEvents[iface];
        }
    }

    if (intrData.gainRegStatus) //Gain signal interrupts
    {
        if (DRF_VAL(_P2060, _STATUS4, _SYNC, (NvU32)intrData.gainRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface));
        }
        if (DRF_VAL(_P2060, _STATUS4, _STEREO, (NvU32)intrData.gainRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(iface));
        }
        if (DRF_VAL(_P2060, _STATUS4, _HS, (NvU32)intrData.gainRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_HOUSE_GAIN);
        }
        if (DRF_VAL(_P2060, _STATUS4, _RJ45, (NvU32)intrData.gainRegStatus))
        {
            ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_RJ45_GAIN);
        }

        if (ifaceEvents[iface] && (pThis->Iface[iface].lastEventNotified != ifaceEvents[iface]))
        {
            gsyncSignalServiceRequested(gsyncGetGsyncInstance(pGpu), ifaceEvents[iface], iface);
            pThis->Iface[iface].lastEventNotified = ifaceEvents[iface];
        }
    }

    if (intrData.miscRegStatus) //Other interrupts
    {
        if (FLD_TEST_DRF(_P2060, _STATUS4, _FRM_CNT_MATCH_INT, _PENDING, (NvU32)intrData.miscRegStatus))
        {
            //
            // To enable frameCountTimerService callback to verify the cached difference 1 second
            // after the test signal is received.
            //
            pThis->FrameCountData.bReCheck = 1;

            //
            // Reset framecountData.Therefore whenever user queries after frame compare
            // interrupt, gsync and gpu frame count register are read and difference is cached again.
            //
            // This will also disable frame compare match interrupt, which will be then enabled
            // frame compare match interrupt in the next user query or in FrameCountTimerService.
            // This is required to clear frmCmpInt bit of status1 register. A read to status1
            // register should clear it, but it is possible that the read to status1 register by
            // calling  gsyncUpdateGsyncStatusSnapshot_P2060() function above may happen in the same
            // frame and this would result interrupt to come back again setting frmCmpint bit.
            //
            rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);

            if (rmStatus == NV_OK)
            {
                //
                // Set enableFrmCmpMatchInt flag NV_TRUE and return. This indicates
                // that the frame compare match interrupt for slave needs to be
                // enabled, which is done in the next user query or in
                // FrameCountTimerService callback.
                //
                pThis->FrameCountData.enableFrmCmpMatchIntSlave = NV_TRUE;
            }
        }

        if (FLD_TEST_DRF(_P2060, _STATUS4, _ERROR_INT, _PENDING, (NvU32)intrData.miscRegStatus))
        {
            // Some error condition observed. Update snapshot
            rmStatus  = gsyncUpdateGsyncStatusSnapshot_P2060(pGpu, intrData.pExtDevice);
            if (rmStatus != NV_OK)
            {
                return;
            }
        }
    }
}

/*
 * waits for hardware to (re-)establish sync.
 * once sync obtains, the watchdog enables interrupt, de-sechedules
 * itself, and waits for an interrupt to go off before running again.
 */
NV_STATUS
extdevWatchdog_P2060
(
    OBJGPU            *pGpu,
    OBJTMR            *pTmr,
    PDACEXTERNALDEVICE pExtDev
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NvU32 iface, head;
    NvBool bStereoLocked;
    NV_STATUS rmStatus = NV_OK;
    NvBool bStereoEnabled[NV_P2060_MAX_IFACES_PER_GSYNC];
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    if (pThis->watchdogCountDownValue)
        pThis->watchdogCountDownValue--;

    // we now can trust our selection of GPU
    pGpu = GetP2060WatchdogGpu(pGpu, pThis);
    NV_ASSERT(pGpu);

    // schedule the next callback.  we can cancel, if it's not needed.
    extdevScheduleWatchdog(pGpu, (PDACEXTERNALDEVICE)pThis);

    rmStatus = gsyncUpdateGsyncStatusSnapshot_P2060(pGpu, pExtDev);

    if (!gsyncIsFrameLocked_P2060(pThis))
    {
        gsyncCancelWatchdog_P2060(pThis);
        rmStatus = gsyncDisableFrameLockInterrupt_P2060((PDACEXTERNALDEVICE)pThis);
        return rmStatus;
    }

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        OBJGPU *pTmpGpu = NULL;
        if (pThis->Iface[iface].GpuInfo.gpuId == NV0000_CTRL_GPU_INVALID_ID)
        {
            continue;
        }
        pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

        NV_ASSERT(pTmpGpu);

        // figure out if stereo is enabled
        bStereoEnabled[iface] = gsyncIsStereoEnabled_p2060(pTmpGpu, pExtDev);

        // loop over the heads of the current gpu on this interface
        for ( head = 0; head < numHeads; head++ )
        {
            if (pThis->Iface[iface].Sync.Slaved[head] ||
                pThis->Iface[iface].Sync.LocalSlave[head])
            {
                bStereoLocked  = FLD_TEST_DRF(_P2060, _STATUS, _STEREO, _LOCK,
                                             (NvU32)pThis->Snapshot[iface].Status1);

                 // check for sync and locks, noting that all heads share the status
                 if ((pThis->Iface[iface].gainedSync) &&
                     (!bStereoEnabled[iface] || bStereoLocked))
                 {
                      break;
                 }
                 else
                 {
                     return NV_ERR_GENERIC; // hope things are better, on next watchdog run
                 }
            }
        }
    }

    if ( NV_OK == rmStatus && pKernelDisplay->bExtdevIntrSupported
         && !pThis->watchdogCountDownValue)
    {
        NV_PRINTF(LEVEL_INFO, "P2060[%d] extdevCancelWatchdog.\n", iface);

        // disable the watchdog,
        extdevCancelWatchdog(pGpu, (PDACEXTERNALDEVICE)pThis);

        // enable the framelock interrupt, if either Master or Slaves are desired
        gsyncEnableFramelockInterrupt_P2060((PDACEXTERNALDEVICE)pThis);
    }

    return NV_OK;
}

static NV_STATUS
gsyncApplyStereoPinAlwaysHiWar
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{

    return NV_OK;

}

static NV_STATUS
gsyncUnApplyStereoPinAlwaysHiWar
(
    OBJGPU *pGpu
)
{

    return NV_OK;

}

//
// gsyncReadUniversalFrameCount_P2060()
//
// When user queries for the first time or after 10 seconds, Gsync and Gpu
// hardware framecount are read and then the difference between them is cached.
// For rest of the instances whenever user queries for frame count, gpu
// frame count is read, and software framecount is updated accordingly based
// on previous cached values.
//
static NV_STATUS
gsyncReadUniversalFrameCount_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pFrameCount
)
{
    OBJGPU   *pTmpGpu = NULL;
    KernelDisplay *pKernelDisplay = NULL;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NV_STATUS rmStatus = NV_OK;
    NvU32 lineCount;
    NvU32 frameCount;
    NvS32 calculatedDiff;
    NvU64 currentTime = 0;
    NvU64 queryTimeDiff;
    OBJTMR *pTmpTmr = NULL;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    if (!(pThis->FrameCountData.iface == NV_P2060_MAX_IFACES_PER_GSYNC))
    {
        //
        // pThis->FrameCountData.iface exists.
        // Thus deriving pTmpGpu from it, and to maintain consistency reading the time from it.
        //
        pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[pThis->FrameCountData.iface].GpuInfo.gpuId);

        if (pTmpGpu)
        {
            pTmpTmr = GPU_GET_TIMER(pTmpGpu);
            currentTime = tmrGetTime_HAL(pTmpGpu, pTmpTmr);
        }
    }

    if (currentTime == 0)
    {
        // pTmpGpu doesn't exists, so getting the time from pGpu.
        currentTime = tmrGetTime_HAL(pGpu, pTmr);
    }

    if (NV_P2060_STATUS_SYNC_LOSS_TRUE == DRF_VAL(_P2060, _STATUS, _SYNC_LOSS, GetP2060GpuSnapshot(pGpu,pThis)))
    {
        // don't increment the frame counter in case of sync loss
        *pFrameCount = pThis->FrameCountData.totalFrameCount;
        pThis->FrameCountData.lastFrameCounterQueryTime = currentTime;

        return NV_OK;
    }

    queryTimeDiff = currentTime - pThis->FrameCountData.lastFrameCounterQueryTime;

    //
    // If user queries for the first time or after 10 secs then read gsync and
    // gpu frame count registers and update software frame count and cached
    //difference. Also enable the frmCmpMatchInt if not enabled.
    //
    if ((!pThis->FrameCountData.lastFrameCounterQueryTime) ||
       (queryTimeDiff > 2 * NV_P2060_FRAME_COUNT_TIMER_INTERVAL))
    {
        //
        // P2060 refreshrate is in 0.00001 Hz, so divide by 10000 to get Hz.
        // divide 1000000 by refreshRate to get the frame time in us.
        //
        pThis->FrameCountData.frameTime = 1000000 / (pThis->RefreshRate/10000); //in us

        //
        // Enable FrameCountTimerService to verify FrameCountData.initialDifference.
        //
        pThis->FrameCountData.bReCheck = 1;

        rmStatus = gsyncUpdateFrameCount_P2060(pThis, pGpu);
        *pFrameCount = pThis->FrameCountData.totalFrameCount;

        // enable frame count match interrupt if not master
        if (!gsyncIsFrameLockMaster_P2060(pThis))
        {
            NvU8 regCtrl3;

            // set frame count match value 1
            rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                      (NvU8)NV_P2060_FRAME_CMPR_LOW,  0x1);
            rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                      (NvU8)NV_P2060_FRAME_CMPR_MID,  0x0);
            rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                      (NvU8)NV_P2060_FRAME_CMPR_HIGH,  0x0);

            //
            // Enable frame count match interrupt for the first time. For rest of the
            // instances when, TEST_SIGNAL is received, interrupt is enable in
            // gsyncUpdateFrameCount_P2060() based on enableFrmCmpMatchIntSlave bit.
            //
            rmStatus |= readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                      NV_P2060_CONTROL3,  &regCtrl3);

            if (rmStatus == NV_OK)
            {
                regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_FRAME_MATCH);
                rmStatus = writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                          NV_P2060_CONTROL3, regCtrl3);
            }
        }

        if (rmStatus != NV_OK)
        {
            rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);
            return rmStatus;
        }

        pThis->FrameCountData.lastFrameCounterQueryTime = currentTime;
    }

    // Update software framecount throught gpu frame count register.
    else
    {
        //
        // To avoid any inconsistency, linecount and framecount should always
        // be read from one specific Gpu head. Its value is stored in pThis->FrameCountData.
        //
        NV_ASSERT_OR_RETURN(pTmpGpu, NV_ERR_INVALID_DEVICE);

        pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pTmpGpu);

        // Read the GPU frame count and line count
        rmStatus = kdispReadRgLineCountAndFrameCount_HAL(pTmpGpu, pKernelDisplay,
                       pThis->FrameCountData.head, &lineCount, &frameCount);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to read RG_DPCA.\n");
            return rmStatus;
        }

        //
        // Check to ensure previousFrameCount != currentGpuFrmCnt. If this is not ensured,
        // it is possible to miss rollover condition.
        //
        if (pThis->FrameCountData.currentFrameCount != frameCount)
        {
            pThis->FrameCountData.previousFrameCount = pThis->FrameCountData.currentFrameCount;
            pThis->FrameCountData.currentFrameCount  = frameCount;

            // If rollback for gpu framecount has occured.
            if (pThis->FrameCountData.previousFrameCount > pThis->FrameCountData.currentFrameCount)
            {
                pThis->FrameCountData.numberOfRollbacks++;
            }
        }

        calculatedDiff = pThis->FrameCountData.initialDifference +
                         pThis->FrameCountData.numberOfRollbacks * (NV_P2060_MAX_GPU_FRAME_COUNT + 1);

        pThis->FrameCountData.totalFrameCount = pThis->FrameCountData.currentFrameCount +
                                                calculatedDiff;

        //
        // To keep sync between the sw framecount and gsync framecount.
        //
        // Gpu framecount increments after VTotal scanout lines  whereas Gsync
        // increments after VActive scanout lines. Thus it is necessary to
        // mitigate this difference of 1 when linecount > VActive.
        //
        if (lineCount > pThis->FrameCountData.vActive)
        {
            pThis->FrameCountData.totalFrameCount++;
        }

        *pFrameCount = pThis->FrameCountData.totalFrameCount;

        pThis->FrameCountData.lastFrameCounterQueryTime = currentTime;
    }

    //
    // Enable the frame compare match interrupt in master gsync, to detect if
    // rollover has occured. So that gsync and gpu frame count can be cached
    // again and difference between them is verified.
    //
    if ((!pThis->FrameCountData.isFrmCmpMatchIntMasterEnabled) &&
        (pThis->FrameCountData.totalFrameCount > (NV_P2060_MAX_GSYNC_FRAME_COUNT - 1000)))
    {
        if (gsyncIsOnlyFrameLockMaster_P2060(pThis))
        {
            NvU8 regCtrl3;

            // enable frame count match interrupt
            rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3,  &regCtrl3);

            regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_FRAME_MATCH);
            rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, regCtrl3);

            pThis->FrameCountData.isFrmCmpMatchIntMasterEnabled = NV_TRUE;

            if (rmStatus != NV_OK)
            {
                rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);
            }
        }
    }
    return rmStatus;
}

/*
 * Read Frame Rate register and calculate Framerate.
 */
static NV_STATUS
gsyncReadFrameRate_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pFrameRate
)
{
    NvU8  FrameCountLow, FrameCountMid, FrameCountHigh;
    NvU32 FrameCount;
    NV_STATUS rmStatus = NV_OK;

    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_FRAMERATE_LOW,  &FrameCountLow);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_FRAMERATE_MID,  &FrameCountMid);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_FRAMERATE_HIGH, &FrameCountHigh);

    if ( NV_OK == rmStatus )
    {
        NvU32 divisor;

        FrameCount = ( ( (((NvU32)FrameCountHigh) & DRF_MASK(NV_P2060_FRAMERATE_HIGH_VAL)) << 16 )  |
                       ( (((NvU32)FrameCountMid)  & DRF_MASK(NV_P2060_FRAMERATE_MID_VAL )) <<  8 )  |
                       ( (((NvU32)FrameCountLow)  & DRF_MASK(NV_P2060_FRAMERATE_LOW_VAL ))       )  );

        divisor     = FrameCount + 2; // FPGA divider is 1
        *pFrameRate = FrameCount ? OVERFLOW_CAREFUL_MUL_DIV(160000000, 2048, divisor) : 0;
        *pFrameRate += 5; // take back one kadam, to honor the Extdev god,
        *pFrameRate -= *pFrameRate % 10;  // whose GSync board this is...
    }
    return rmStatus;
}

/*
 * Read House Sync Frame Rate register and calculate Framerate.
 */
static NV_STATUS
gsyncReadHouseSyncFrameRate_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pFrameRate
)
{
    NvU8  FrameCountLow, FrameCountMid, FrameCountHigh;
    NvU32 FrameCount;
    NV_STATUS rmStatus = NV_OK;

    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_HS_FRAMERATE_LOW,  &FrameCountLow);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_HS_FRAMERATE_MID,  &FrameCountMid);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_HS_FRAMERATE_HIGH, &FrameCountHigh);

    if ( NV_OK == rmStatus )
    {
        NvU32 divisor;

        FrameCount = ( ( (((NvU32)FrameCountHigh) & DRF_MASK(NV_P2060_HS_FRAMERATE_HIGH_VAL)) << 16 )  |
                       ( (((NvU32)FrameCountMid)  & DRF_MASK(NV_P2060_HS_FRAMERATE_MID_VAL )) <<  8 )  |
                       ( (((NvU32)FrameCountLow)  & DRF_MASK(NV_P2060_HS_FRAMERATE_LOW_VAL ))       )  );

        divisor     = FrameCount + 2; // FPGA divider is 1
        *pFrameRate = FrameCount ? OVERFLOW_CAREFUL_MUL_DIV(160000000, 2048, divisor) : 0;
        *pFrameRate += 5; // take back one kadam, to honor the Extdev god,
        *pFrameRate -= *pFrameRate % 10;  // whose GSync board this is...
    }
    return rmStatus;
}

NV_STATUS
gsyncOptimizeTimingParameters_P2060
(
    OBJGPU            *pGpu,
    GSYNCTIMINGPARAMS *pParams
)
{
    RM_API   *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32     hClient     = pGpu->hInternalClient;
    NvU32     hSubdevice  = pGpu->hInternalSubdevice;
    NV_STATUS status      = NV_OK;
    NV2080_CTRL_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS_PARAMS ctrlParams = {0};

    ctrlParams.timingParameters = *pParams;

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_GSYNC_OPTIMIZE_TIMING_PARAMETERS,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "OptimizeTimingParameters control call has failed! \n");
    }
    else
    {
        *pParams = ctrlParams.timingParameters;
    }

    return status;
}

/*
 * Program External Stereo Polarity to High side of master stereo
 */
static NV_STATUS
gsyncProgramExtStereoPolarity_P2060
(
    OBJGPU  *pGpu,
    PDACEXTERNALDEVICE pExternalDevice
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU8  ctrl4 = 0x00;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExternalDevice,
                                             NV_P2060_CONTROL4, &ctrl4);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    // This bit controls stereo polarity relative to an external sync.
    ctrl4 = (NvU8) FLD_SET_DRF(_P2060, _CONTROL4, _EXT_STEREO_SYNC_POL, _HI, (NvU32)ctrl4);

    rmStatus = writeregu008_extdeviceTargeted(pGpu, pExternalDevice,
                                             NV_P2060_CONTROL4, ctrl4);
    return rmStatus;
}

NV_STATUS
gsyncSetStereoLockMode_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32              enable
)
{
    NvU8 ctrl4;
    NV_STATUS rmStatus;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL4, &ctrl4);

    if (rmStatus != NV_OK)
    {
         return rmStatus;
    }

    if (enable)
    {
        ctrl4 = FLD_SET_DRF(_P2060, _CONTROL4, _STEREO_LOCK_MODE, _ON, ctrl4);
    }
    else
    {
        ctrl4 = FLD_SET_DRF(_P2060, _CONTROL4, _STEREO_LOCK_MODE, _OFF, ctrl4);
    }

    rmStatus = writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL4, ctrl4);

    return rmStatus;
}

NV_STATUS
gsyncGetStereoLockMode_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32              *enable
)
{
    NvU8 ctrl4;
    NV_STATUS rmStatus;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL4, &ctrl4);

    if (NV_OK == rmStatus)
    {
        *enable = FLD_TEST_DRF(_P2060, _CONTROL4, _STEREO_LOCK_MODE, _ON, ctrl4);
    }

    return rmStatus;
}

NV_STATUS
gsyncSetVideoMode_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCVIDEOMODE VideoMode
)
{
    return NV_OK;
}

NV_STATUS
gsyncGetVideoMode_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCVIDEOMODE *pVideoMode
)
{
    NvU8 videoMode;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, &videoMode);

    if ( NV_OK == rmStatus )
    {
        *pVideoMode = DRF_VAL(_P2060, _STATUS2, _HS_DETECT, videoMode);
        if (*pVideoMode == 0x02)
        {
            // reported videoMode is composite. Convert it to RMAPI exported value.
            *pVideoMode = gsync_VideoMode_COMPOSITE;
        }
    }

    // update p2060 object
    pThis->VideoMode = *pVideoMode;

    return rmStatus;
}

NV_STATUS
gsyncSetEmitTestSignal_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 bEmitTestSignal
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->EmitTestSignal = bEmitTestSignal;

    if (!gsyncIsFrameLockMaster_P2060(pThis))
    {
        return NV_ERR_INVALID_DEVICE;
    }

    pGpu = GetP2060MasterableGpu(pGpu, (PDACP2060EXTERNALDEVICE)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( bEmitTestSignal )
    {
        ctrl = FLD_SET_DRF(_P2060, _CONTROL, _TEST_MODE,  _ON, ctrl);
    }
    else
    {
        ctrl = FLD_SET_DRF(_P2060, _CONTROL, _TEST_MODE, _OFF, ctrl);

        //
        // To enable frameCountTimerService callback which verifies
        // the cache difference 1 second after sending the test signal
        //
        pThis->FrameCountData.bReCheck = 1;

        //
        // Reset framecountData.Therefore whenever user queries after TEST_MODE_OFF,
        // gsync and gpu frame count register are read and difference is cached again.
        //
        rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);
    }

    rmStatus |= writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, ctrl);

    //
    // Add some OS delay between test mode transitions. This is also
    // necessary for proper propogation of test signal to other slaves.
    //
    {
        NvU32 refreshTime = 0;
        //
        // The test mode detection logic will return the previous value until the
        // next frame.  Although only the master will only manipulate test mode,
        // we may transition to slave while test mode is still on due to this.
        // So delay for a couple of frames to make sure the value transitions
        // correctly. Fix for bug #82809, back in the day.
        //
        // P2060 refreshrate is in 0.00001 Hz, so divide by 10000 to get Hz.
        //
        if ((pThis->RefreshRate/10000) > 0)
        {
            refreshTime = 1000 / (pThis->RefreshRate/10000);
        }
        NV_ASSERT(refreshTime != 0);

        // Only wait a maximum amount of time.
        refreshTime = NV_MIN(refreshTime, 35);
        osDelay(2*refreshTime /* ms */);
    }

    return rmStatus;
}

NV_STATUS
gsyncGetEmitTestSignal_P2060
(
 OBJGPU *pGpu,
 PDACEXTERNALDEVICE pExtDev,
 NvU32 *pVal
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( NV_OK == rmStatus )
    {
        *pVal = FLD_TEST_DRF(_P2060, _CONTROL, _TEST_MODE, _ON, ctrl);
    }

    // update p2060 object
    pThis->EmitTestSignal = *pVal;

    return rmStatus;
}

NV_STATUS
gsyncSetInterlaceMode_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 InterlaceMode
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->InterlaceMode = InterlaceMode;

    pGpu = GetP2060MasterableGpu(pGpu, (PDACP2060EXTERNALDEVICE)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( rmStatus == NV_OK )
    {
        ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _INTERLACE_MODE, (NvU8)InterlaceMode, ctrl);
        rmStatus = writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, ctrl);
    }

    return rmStatus;
}

NV_STATUS
gsyncGetInterlaceMode_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pVal
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    pGpu = GetP2060MasterableGpu(pGpu, (PDACP2060EXTERNALDEVICE)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( NV_OK == rmStatus )
    {
        *pVal = FLD_TEST_DRF(_P2060, _CONTROL, _INTERLACE_MODE, _TRUE, ctrl);
    }

    // update p2060 object
    pThis->InterlaceMode = *pVal;

    return rmStatus;
}

NV_STATUS
gsyncSetUseHouse_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 UseHouseSync
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->UseHouseSync = UseHouseSync;

    pGpu = GetP2060MasterableGpu(pGpu, (PDACP2060EXTERNALDEVICE)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, &ctrl);

    if (NV_OK == rmStatus)
    {
        ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _SYNC_SELECT, (NvU8)UseHouseSync, ctrl);
        rmStatus = writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, ctrl);
    }

    return rmStatus;
}

NV_STATUS
gsyncGetUseHouse_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *val
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, &ctrl);

    if (NV_OK == rmStatus)
    {
        *val = FLD_TEST_DRF(_P2060, _CONTROL, _SYNC_SELECT, _HOUSE, ctrl);

        // update p2060 object
        pThis->UseHouseSync = *val;
    }

    return rmStatus;
}

NV_STATUS
gsyncSetSyncPolarity_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCSYNCPOLARITY SyncPolarity
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    GSYNCSYNCPOLARITY currentSyncPolarity;

    // update p2060 object
    pThis->SyncPolarity = SyncPolarity;

    pGpu = GetP2060MasterableGpu(pGpu, (PDACP2060EXTERNALDEVICE)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( NV_OK == rmStatus )
    {
        currentSyncPolarity = DRF_VAL(_P2060, _CONTROL, _SYNC_POLARITY, ctrl);

        if (currentSyncPolarity != SyncPolarity)
        {
            NvU32 frameTime = 0;

            ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _SYNC_POLARITY, (NvU8)SyncPolarity, ctrl);
            rmStatus = writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_CONTROL, ctrl);

            if ((pThis->RefreshRate/10000) > 0)
            {
                frameTime = 1000 / (pThis->RefreshRate/10000);
            }

            //
            // Wait for max of 10 frames or 100 ms so that hardware can collect
            // the new house sync frame rate.
            //
            frameTime = NV_MAX(frameTime, 10);
            osDelay(10 * frameTime /* ms */);
        }
    }

    return rmStatus;
}

NV_STATUS
gsyncGetSyncPolarity_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCSYNCPOLARITY *pSyncPolarity
)
{
    NvU8 ctrl;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_CONTROL, &ctrl);

    if ( NV_OK == rmStatus )
    {
        *pSyncPolarity = DRF_VAL(_P2060, _CONTROL, _SYNC_POLARITY, ctrl);
    }

    // update p2060 object
    pThis->SyncPolarity = *pSyncPolarity;

    return rmStatus;
}

NV_STATUS
gsyncSetNSync_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 NSync
)
{
    NvU8 regNSync;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->NSync = NSync;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_NSYNC, &regNSync);

    if ( NV_OK == rmStatus )
    {
        regNSync = FLD_SET_DRF_NUM(_P2060, _NSYNC, _FL, (NvU8)NSync, regNSync);
        rmStatus = writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_NSYNC, regNSync);
    }

    return rmStatus;
}

NV_STATUS
gsyncGetNSync_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pNSync
)
{
    NvU8 regNSync;
    NV_STATUS rmStatus;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_NSYNC, &regNSync);

    if ( NV_OK == rmStatus )
    {
        *pNSync = DRF_VAL(_P2060, _NSYNC, _FL, regNSync);
    }

    // update p2060 object
    pThis->NSync = *pNSync;

    return rmStatus;
}

NV_STATUS
gsyncSetSyncSkew_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 SyncSkew
)
{
    NvU8 SyncSkewLow, SyncSkewHigh;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->SyncSkew = SyncSkew;

    if (gsyncSupportsLargeSyncSkew_P2060(pExtDev))
    {
        SyncSkewLow  = (NvU8)((SyncSkew     ) & DRF_MASK(NV_P2060_SYNC_SKEW_LOW_VAL ));
        SyncSkewHigh = (NvU8)((SyncSkew >> 8) & DRF_MASK(NV_P2060_SYNC_SKEW_HIGH_VAL));
    }
    else
    {
        if ((SyncSkew != 0) && (SyncSkew != 1))
        {
            return NV_ERR_NOT_SUPPORTED;
        }
        else
        {
            SyncSkewLow  = (NvU8)SyncSkew;
            SyncSkewHigh = 0x00;
       }
    }

    rmStatus |= writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_LOW, SyncSkewLow);
    rmStatus |= writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_HIGH, SyncSkewHigh);

    return rmStatus;

}

NV_STATUS
gsyncGetSyncSkew_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pSyncSkew
)
{
    NvU8 SyncSkewLow, SyncSkewHigh;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_LOW,  &SyncSkewLow);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_SYNC_SKEW_HIGH, &SyncSkewHigh);

    if ( NV_OK == rmStatus )
    {
        *pSyncSkew =
            ( ((((NvU32)SyncSkewHigh) & DRF_MASK(NV_P2060_SYNC_SKEW_HIGH_VAL)) << 8 ) |
              ((((NvU32)SyncSkewLow ) & DRF_MASK(NV_P2060_SYNC_SKEW_LOW_VAL ))      ) ) ;
    }

    // update p2060 object
    pThis->SyncSkew = *pSyncSkew;

    return rmStatus;
}

NV_STATUS
gsyncSetSyncStartDelay_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 StartDelay
)
{
    NvU8 StartDelayLow, StartDelayHigh;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    // update p2060 object
    pThis->SyncStartDelay = StartDelay;

    StartDelayLow = (NvU8)((StartDelay     ) & DRF_MASK(NV_P2060_START_DELAY_LOW_VAL ));
    StartDelayHigh= (NvU8)((StartDelay >> 8) & DRF_MASK(NV_P2060_START_DELAY_HIGH_VAL));

    rmStatus |= writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_START_DELAY_LOW, StartDelayLow);
    rmStatus |= writeregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_START_DELAY_HIGH, StartDelayHigh);

    return rmStatus;
}

NV_STATUS
gsyncGetSyncStartDelay_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pStartDelay
)
{
    NvU8 StartDelayLow, StartDelayHigh;
    NV_STATUS rmStatus = NV_OK;
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_START_DELAY_LOW, &StartDelayLow);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_START_DELAY_HIGH, &StartDelayHigh);

    if ( NV_OK == rmStatus )
    {
        *pStartDelay =
            ( ((((NvU32)StartDelayHigh) & DRF_MASK(NV_P2060_START_DELAY_HIGH_VAL)) << 8 ) |
              ((((NvU32)StartDelayLow ) & DRF_MASK(NV_P2060_START_DELAY_LOW_VAL ))      ) );
    }

    // update p2060 object
    pThis->SyncStartDelay = *pStartDelay;

    return rmStatus;
}

/*
 * check if housesync is present or not.
 */
static NV_STATUS
gsyncReadHouseSignalPresent_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvBool bTestSyncLoss,
    NvU32 *pVal
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NvU8 regStatus2;
    NvU32 regStatus = GetP2060GpuSnapshot(pGpu,pThis);
    NV_STATUS rmStatus = NV_OK;

    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    if (bTestSyncLoss && FLD_TEST_DRF(_P2060, _STATUS, _SYNC_LOSS, _TRUE, (NvU32)regStatus))
    {
        rmStatus |= gsyncUpdateGsyncStatusSnapshot_P2060(pGpu, pExtDev);

        if ( NV_OK != rmStatus )
            return rmStatus;

        if (FLD_TEST_DRF(_P2060, _STATUS, _SYNC_LOSS, _TRUE, GetP2060GpuSnapshot(pGpu,pThis)))
        {
            *pVal = 0; // bTestSyncLoss and (SYNC_LOSS == NV_TRUE)
        }
    }
    else
    {
        rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                 (NvU8)NV_P2060_STATUS2, &regStatus2);
        if ( NV_OK != rmStatus )
            return rmStatus;

        *pVal = !!(DRF_VAL(_P2060, _STATUS2, _HS_DETECT, (NvU32)regStatus2));
    }

    return rmStatus;
}

/*
 * This function returns whether there is a sync source present.
 *
 * If framelock is not enabled, the only sync source possible is an external signal.
 *
 * If framelock is enabled, a local master may be providing the sync signal, or
 * housesync may be providing a signal via a local master, or we may need to
 * poll for an external signal.
 */
static NV_STATUS
gsyncReadIsSyncDetected_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pVal
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    // Assume we are not synced, unless we match one of the cases below
    *pVal = NV_FALSE;

    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_GENERIC);

    if (!gsyncIsFrameLocked_P2060(pThis))
    {
        NvU8 regStatus;

        //
        // Framelock is not enabled; read the NV_P2060_STATUS register to get
        // the external sync status
        //
        NV_ASSERT_OK_OR_RETURN(
            readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS, &regStatus));
        *pVal = FLD_TEST_DRF(_P2060, _STATUS, _SYNC_LOSS, _FALSE, regStatus);
    }
    else
    {
        NvU32 iface, head, tempIface, tempHead;

        NV_ASSERT_OK_OR_RETURN(GetP2060GpuLocation(pGpu, pThis, &iface));


        for (head = 0; head < numHeads; head++)
        {
            // Check if we're slaved to another master head in the same system
            if (pThis->Iface[iface].Sync.LocalSlave[head])
            {
                for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
                {
                    for (tempHead = 0; tempHead < numHeads; tempHead++)
                    {
                        if (pThis->Iface[tempIface].Sync.Master[tempHead])
                        {
                            //
                            // If we're slaved to another local head, we are
                            // receiving a sync signal from it. (But if it uses
                            // housesync, then it must also be receiving housesync.)
                            //
                            if (!pThis->Iface[tempIface].Sync.Slaved[tempHead])
                            {
                                *pVal = NV_TRUE;
                            }
                            else
                            {
                                NV_ASSERT_OK_OR_RETURN(
                                    gsyncReadHouseSignalPresent_P2060(pGpu, pExtDev,
                                                                      NV_TRUE, pVal));
                            }
                        }
                    }
                }
                break;
            }

            if (pThis->Iface[iface].Sync.Master[head])
            {
                //
                // A master head with no house signal has its own sync signal.
                // A master head with house signal has a sync signal if the
                // house signal is present.
                //
                if (pThis->Iface[iface].Sync.Slaved[head])
                {
                    NV_ASSERT_OK_OR_RETURN(
                        gsyncReadHouseSignalPresent_P2060(pGpu, pExtDev, NV_TRUE, pVal));
                    break;
                }
                else
                {
                    *pVal = NV_TRUE;
                    break;
                }
            }

            if (pThis->Iface[iface].Sync.Slaved[head])
            {
                NvU8 regStatus;

                //
                // A slaved head with external master signal must poll
                // NV_P2060_STATUS_SYNC_LOSS for sync status.
                //
                NV_ASSERT_OK_OR_RETURN(
                    readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS, &regStatus));
                *pVal = FLD_TEST_DRF(_P2060, _STATUS, _SYNC_LOSS, _FALSE, regStatus);
                break;
            }
        }
    }

    return NV_OK;
}

/*
 * Check if stereo is locked or not.
 *
 * stereo is locked means
 * - a framelock master signal is available as reference
 * - sync to this master signal has been gained
 * - master and local gpu both have either stereo enabled or disabled
 * - master and local stereo signal is in phase (in case it's enabled)
 */
static NV_STATUS
gsyncReadStereoLocked_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pVal
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NV_STATUS rmStatus = NV_ERR_GENERIC;

    if (pVal)
    {
        NvU32 iface;

        // Default return is stereo not locked.
        *pVal = 0;

        // which gpu's interface are we talking to?
        rmStatus = GetP2060GpuLocation(pGpu, pThis, &iface);

        if (NV_OK == rmStatus)
        {
            // stereo status reporting only makes sense if we've gained sync.
            if (pThis->Iface[iface].gainedSync)
            {
                NvU32 regStatus = GetP2060GpuSnapshot(pGpu,pThis);

                if ((NV_P2060_STATUS_MSTR_STEREO_NOT_ACTIVE ==
                    DRF_VAL(_P2060, _STATUS, _MSTR_STEREO, regStatus)) &&
                    (NV_P2060_STATUS_GPU_STEREO_NOT_ACTIVE ==
                    DRF_VAL(_P2060, _STATUS, _GPU_STEREO, regStatus)))
                {
                    //
                    // If neither local nor master stereo is enabled
                    // stereo is locked.
                    //
                    *pVal = 1;
                }
                else
                {
                    //
                    // If local or master stereo signals are present,
                    // return back P358s stereo_lock reporting.
                    //
                    *pVal = (NV_P2060_STATUS_STEREO_LOCK ==
                        DRF_VAL(_P2060, _STATUS, _STEREO, regStatus));
                }
            }
        }
    }

    return rmStatus;
}

//
// Check if we are in sync, i.e. we supply the master sync signal or are servoed
// to the master sync signal. The servo should be stable for about 5 seconds if
// the signal is external (i.e. use the gainedSync value which already maintains
// this.)
//
static NV_STATUS
gsyncReadIsTiming_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 *pVal
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NvU32 iface;

    *pVal = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(GetP2060GpuLocation(pGpu, pThis, &iface));

    *pVal = pThis->Iface[iface].gainedSync;

    return NV_OK;
}


/*
 * Program P2060 Master for Framelock.
 */
static NV_STATUS
gsyncProgramMaster_P2060
(
    OBJGPU   *pGpu,
    OBJGSYNC *pGsync,
    NvU32     Master,
    NvBool    bRetainMaster,
    NvBool    skipSwapBarrierWar
)
{
    KernelDisplay          *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32                   DisplayIds[OBJ_MAX_HEADS];
    NvU32                   iface, head, index;
    NvU8                    ctrl = 0;
    NvBool                  bTestModePresent;
    NvBool                  bHouseSelect, bEnableMaster = (0 != Master);
    NvBool                  bGPUAlreadyMaster;
    NvBool                  bQSyncAlreadyMaster;
    NV_STATUS               rmStatus = NV_OK;
    NvU32                   numHeads = kdispGetNumHeads(pKernelDisplay);
    DACP2060EXTERNALDEVICE *pThis;

    pThis = (DACP2060EXTERNALDEVICE *)pGsync->pExtDev;

    if ( Master && bRetainMaster)
    {
        for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
        {
            for ( head = 0; head < numHeads; head++ )
            {
                if ( pThis->Iface[iface].Sync.Master[head] )
                {
                    pThis->Iface[iface].Sync.Master[head] = 0;
                    pThis->Iface[iface].Sync.Slaved[head] = 0;
                    pThis->Iface[iface].Sync.LocalSlave[head] = 0;
                }
            }
        }
        return rmStatus;
    }

    // This utility fn returns display id's associated with each head.
    extdevGetBoundHeadsAndDisplayIds(pGpu, DisplayIds);

    // which gpu's are we talking to?
    rmStatus = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get Gpu location. Can not program Master.\n");
        return rmStatus;
    }

    // no failure allowed!
    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                            (NvU8)NV_P2060_CONTROL, &ctrl);
    if ((NV_OK != rmStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to read Ctrl data. Can not program Master.\n");
        return rmStatus;
    }

    // Check if TEST MODE present
    bTestModePresent = FLD_TEST_DRF(_P2060, _CONTROL, _TEST_MODE, _ON, (NvU32)ctrl);

    // Check for House sync select as sync source
    bHouseSelect = FLD_TEST_DRF(_P2060, _CONTROL, _SYNC_SELECT, _HOUSE, (NvU32)ctrl);

    rmStatus = GetP2060ConnectorIndexFromGpu(pGpu, pThis, &index);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get connector index for Gpu. Can not program Master.\n");
        return rmStatus;
    }
    //
    // For P2060, Already Mastership based on FPGA -> I_AM_MASTER + GPU -> TIMING SOURCE.
    // First check for display is already Master or not i.e. GPU is TS or not.
    // Then check for FPGA board is already Master or not.
    //
    bGPUAlreadyMaster = (DRF_VAL(_P2060, _CONTROL, _SYNC_SRC, (NvU32)ctrl) == index);
    bQSyncAlreadyMaster = FLD_TEST_DRF(_P2060, _CONTROL, _I_AM, _MASTER, (NvU32)ctrl);

    // In case of Passthru mode, Qsync board can be shared across multiple VMs.
    // If Qsync board is already master, then only TS Gpu should be allowed to change it's mastership.
    // Bail out if non TS GPU tries to change it.
    if (IS_PASSTHRU(pGpu) && (bQSyncAlreadyMaster & !bGPUAlreadyMaster))
    {
        return rmStatus;
    }

    if (bQSyncAlreadyMaster != bEnableMaster)
    {
        if (pThis->ExternalDevice.deviceRev == DAC_EXTERNAL_DEVICE_REV_NONE)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to read NV_P2060_FPGA. Can not program Master.\n");
            return rmStatus;
        }

        if (bEnableMaster)
        {
            rmStatus = gsyncApplyStereoPinAlwaysHiWar(pGpu, (PDACEXTERNALDEVICE)pThis);

            if (NV_OK != rmStatus)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to drive stereo output pin for bug3362661.\n");
            }

            //
            // Set the RasterSync Decode Mode
            // This may return an error if the FW and GPU combination is invalid
            //
            NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                pGsync->gsyncHal.gsyncSetRasterSyncDecodeMode(pGpu, pGsync->pExtDev));

            //
            // GPU will now be TS - Mark sync source for GPU on derived index.
            // This needs to be done first as only TS can write I_AM_MASTER bit.
            //
            ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _SYNC_SRC, (NvU8)index, ctrl);
            rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis,
                                                      (NvU8)NV_P2060_CONTROL, ctrl);
            if (NV_OK != rmStatus)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to write SYNC_SRC. Can not program Master.\n");
                return rmStatus;
            }
        }

        if (bTestModePresent)
        {
            // Clear the TEST mode bit before handling enable/disable master.
            ctrl = FLD_SET_DRF(_P2060, _CONTROL, _TEST_MODE, _OFF, ctrl);

            if (!bEnableMaster)
            {
                //
                // Clear the TEST mode bit before disabling master as TEST mode
                // can only be modified by the GPU TS under FPGA master mode.
                //
                rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis,
                                                        (NvU8)NV_P2060_CONTROL, ctrl);
                osDelay(30); // Add delay of 30 ms as we are turning OFF TEST mode.
            }
        }

        // Gsync/FPGA card will be master or not based on bEnableMaster.
        ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _I_AM, (NvU8)bEnableMaster, ctrl);
        rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis,
                                                  (NvU8)NV_P2060_CONTROL, ctrl);
        if (NV_OK != rmStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to write I_AM_MSTR. Can not program Master.\n");
            return rmStatus;
        }

        //
        // Now we have updated Master status of Gsync board, wait for some time. This will allow FPGA to
        // reflect state-change in other registers e.g portStat, Frame Rate etc. Refer Bug 1053022.
        //
        osDelay(200);

        if (bEnableMaster)
        {
            // Remember the desired skipSwapBarrierWar setting for enable master calls.
            pThis->Iface[iface].skipSwapBarrierWar = skipSwapBarrierWar;
        }
        else
        {
            // Fetch the real skipSwapBarrierWar value from cache in case of an unsync call.
            skipSwapBarrierWar = pThis->Iface[iface].skipSwapBarrierWar;
            // And reset the cached value.
            pThis->Iface[iface].skipSwapBarrierWar = NV_FALSE;
        }

        //
        // Fpga revisions <= 5 need to sw the swapbarrier on the framelock master
        // to drive the swap_rdy signal. This can be overridden by skipSwapBarrierWar.
        //
        if ((!skipSwapBarrierWar) &&
            needsMasterBarrierWar(&pThis->ExternalDevice))
        {
            // enable/disable SwapRdy for GPU during enable/disable of Framelock Master.
            rmStatus = gsyncUpdateSwapRdyConnectionForGpu_P2060(pGpu, pThis, bEnableMaster);
            if (NV_OK != rmStatus)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to update SwapRdyEnable. Can not program Master.\n");
                return rmStatus;
            }
        }
    }

    // now we're ready to let the software know about it.
    for ( head = 0; head < numHeads; head++ )
    {
        // is this head the desired master, or are we are disabling mastership?
        // If mastership is currently disabled, don't touch cache as this could destroy slave values.0
        if ((Master & DisplayIds[head]) || (!bEnableMaster && bQSyncAlreadyMaster))
        {
            pThis->Iface[iface].Sync.Master[head] = (bEnableMaster);
            pThis->Iface[iface].Sync.Slaved[head] = (bEnableMaster && bHouseSelect);

            gsyncProgramFramelockEnable_P2060(pGpu, pThis, iface, bEnableMaster);
        }
         else
        {
            // we are setting a master, but it's not on this head, so just to be safe:
            pThis->Iface[iface].Sync.Master[head] = 0;
        }
    }

    if (!bEnableMaster && !gsyncIsFrameLocked_P2060(pThis))
    {
        // Disable Framelock interrupts as board is not framelocked now.
        gsyncDisableFrameLockInterrupt_P2060((PDACEXTERNALDEVICE)pThis);

        rmStatus = gsyncUnApplyStereoPinAlwaysHiWar(pGpu);

        if (NV_OK != rmStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to drive stereo output pin for bug3362661.\n");
        }
    }

    if (!IsSLIEnabled(pGpu))
    {
        NvU32 otherGpuId;
        OBJGPU   *pOtherGpu;
        RM_API   *pRmApi;
        NvU32     hClient;
        NvU32     hSubdevice;
        NvU32 Slaves, drOut, drIn;
        NvU32 tempIface;
        NV2080_CTRL_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC_PARAMS ctrlParams = {0};

        for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
        {
            if (tempIface == iface)
            {
                continue;
            }

            otherGpuId = pThis->Iface[tempIface].GpuInfo.gpuId;
            if (otherGpuId == NV0000_CTRL_GPU_INVALID_ID)
            {
                continue;
            }

            pOtherGpu = gpumgrGetGpuFromId(otherGpuId);
            NV_ASSERT(pOtherGpu);

            if (gpumgrGetGpuLockAndDrPorts(pGpu, pOtherGpu, &drOut, &drIn) != NV_OK)
            {
                continue;
            }
            //
            // If this is the master gpu, we need to disable the raster sync
            // gpio on the other P2060 GPU that's connected to master over
            // Video bridge. Otherwise, if the video bridge is connected,
            // the raster sync signals from the two cards will interfere,
            // giving us an unreliable sync signal.
            //
            pRmApi      = GPU_GET_PHYSICAL_RMAPI(pOtherGpu);
            hClient     = pOtherGpu->hInternalClient;
            hSubdevice  = pOtherGpu->hInternalSubdevice;

            ctrlParams.bEnableMaster = bEnableMaster;
            ctrlParams.bRasterSyncGpioSaved = pThis->Iface[tempIface].RasterSyncGpio.saved;
            ctrlParams.bRasterSyncGpioDirection = pThis->Iface[tempIface].RasterSyncGpio.direction;

            rmStatus = pRmApi->Control(pRmApi, hClient, hSubdevice,
                                       NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_OR_RESTORE_RASTER_SYNC,
                                       &ctrlParams, sizeof(ctrlParams));

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Extdev control call to save/restore GPIO direction is failed!\n");
            }
            else
            {
                if (bEnableMaster && !pThis->Iface[tempIface].RasterSyncGpio.saved)
                {
                    pThis->Iface[tempIface].RasterSyncGpio.direction = ctrlParams.bRasterSyncGpioDirection;
                    pThis->Iface[tempIface].RasterSyncGpio.saved = NV_TRUE;
                }
                else if (!bEnableMaster && pThis->Iface[tempIface].RasterSyncGpio.saved)
                {
                     pThis->Iface[tempIface].RasterSyncGpio.saved = NV_FALSE;
                 }
            }

            Slaves = gsyncReadSlaves_P2060(pOtherGpu, pThis);
            if (Slaves)
            {
                rmStatus = gsyncProgramSlaves_P2060(pOtherGpu, pThis, Slaves);
                if (NV_OK != rmStatus)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to program SLI slaves. Can not program Master.\n");
                    return rmStatus;
                }
            }
        }
    }

    // Reset the frame count data and also disable frame compare match interrupt.
    if (!bEnableMaster)
    {
        iface = pThis->FrameCountData.iface;
        head  = pThis->FrameCountData.head;

        if ((iface < NV_P2060_MAX_IFACES_PER_GSYNC) && (head  < numHeads))
        {
            rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);
        }
    }
    return rmStatus;
}

/*
 * Read Framelock Master P2060.
 */
static NvU32
gsyncReadMaster_P2060
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32       DisplayIds[OBJ_MAX_HEADS];
    NvU32       iface, head;
    NvU32       Master = 0;
    NV_STATUS   status;
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    extdevGetBoundHeadsAndDisplayIds(pGpu, DisplayIds);

    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
        return 0;

    for ( head = 0; head < numHeads; head++ )
    {
        if (pThis->Iface[iface].Sync.Master[head])
        {
            Master |= DisplayIds[head];
        }
    }

    // check hardware's opinion on the Master Gsync and Timing source GPU.
    if (gsyncIsP2060MasterBoard(pGpu, pThis) &&
        GpuIsP2060Master(pGpu, pThis))
    {
        if (Master)
        {
            return Master;
        }
        else
        {
            // HW is set to be master, but SW isn't treating it as master?
            // we must be on external sync, so no one display is associated...
            return ~0;
        }
    }
    else
    {
        // if HW says it's not the master, SW's opinion doesn't count.
        return 0;
    }
}

/*
 * Program P2060 Slave for framelock.
 */
static NV_STATUS
gsyncProgramSlaves_P2060
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis,
    NvU32 Slaves
)
{
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32       DisplayIds[OBJ_MAX_HEADS];
    NvU32       iface, head, index;
    NvU8        ctrl = 0, ctrl3 = 0;
    NvBool      bCoupled, bHouseSelect, bLocalMaster, bEnableSlaves = (0 != Slaves);
    NV_STATUS   rmStatus = NV_OK;
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    // This utility fn returns display id's associated with each head.
    extdevGetBoundHeadsAndDisplayIds(pGpu, DisplayIds);

    // which gpu's are we talking to?
    rmStatus = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get Gpu location. Can not program Slave.\n");
        return rmStatus;
    }

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                            (NvU8)NV_P2060_CONTROL, &ctrl);
    if (NV_OK != rmStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to read ctrl register. Can not program slave.\n");
        return rmStatus; // ouch
    }

    // Check for House sync select as sync source and FPGA board is master or not
    bHouseSelect = FLD_TEST_DRF(_P2060, _CONTROL, _SYNC_SELECT, _HOUSE,  (NvU32)ctrl);
    bLocalMaster = FLD_TEST_DRF(_P2060, _CONTROL, _I_AM,        _MASTER, (NvU32)ctrl);

    if (bEnableSlaves || bLocalMaster)
    {
        rmStatus = gsyncApplyStereoPinAlwaysHiWar(pGpu, (PDACEXTERNALDEVICE)pThis);

        if (NV_OK != rmStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to drive stereo output pin for bug3362661.\n");
        }
    }

    if (bHouseSelect && bEnableSlaves && bLocalMaster)
    {
        rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                (NvU8)NV_P2060_CONTROL3, &ctrl3);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to read ctrl3 register. Can not program slave.\n");
            return rmStatus;
        }
        ctrl3 |= (NvU8) FLD_SET_DRF(_P2060, _CONTROL3, _RESYNC, _ON, (NvU32)ctrl3);
        writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                      (NvU8)NV_P2060_CONTROL3, ctrl3);
    }

    //
    // No need to check for every gpu connected to this gsync board. If gsync board
    // is not master/server, none of the gpu connected to it will have master head.
    //

    if (bEnableSlaves && !bLocalMaster)
    {
       //
       // Sync source should be taken from GPU connected to Gsync board.
       // Get index of current GPU and make it sync source.
       // No idea is this safe or not. Adding TODO for future check.
       //
       rmStatus = GetP2060ConnectorIndexFromGpu(pGpu, pThis, &index);
       if (NV_OK != rmStatus)
       {
           NV_PRINTF(LEVEL_ERROR,
                     "Failed to get connector index for Gpu. Can not program slave.\n");
           return rmStatus;
       }

       ctrl = FLD_SET_DRF_NUM(_P2060, _CONTROL, _SYNC_SRC, (NvU8)index, ctrl);

       rmStatus = writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis,
                                                (NvU8)NV_P2060_CONTROL, ctrl);
       if (NV_OK != rmStatus)
       {
           NV_PRINTF(LEVEL_ERROR,
                     "Failed to write SYNC_SRC. Can not program slave.\n");
           return rmStatus;
       }
    }

    //
    // With House sync enabled the crashlocking still need some investigations.
    // So filter out Housesyced systems before doing local crashlocks.
    //
    if ((!bHouseSelect) && bEnableSlaves && bLocalMaster)
    {
        rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                (NvU8)NV_P2060_CONTROL3, &ctrl3);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to read ctrl3 register. Can not program slave.\n");
            return rmStatus;
        }
        ctrl3 |= (NvU8) FLD_SET_DRF(_P2060, _CONTROL3, _RESYNC, _ON, (NvU32)ctrl3);
        writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                       (NvU8)NV_P2060_CONTROL3, ctrl3);
    }

    //
    // If we on the same gpu as the framelock master, the P2060 will
    // not servo, so we must rely on the gpu to get our timing...
    // unless house sync is expected, in which case we get it back.
    //
    bCoupled = (!bLocalMaster) || (bHouseSelect);

    // disable all existing slaves before enabling new slaves.
    if (bEnableSlaves)
    {
        for ( head = 0; head < numHeads; head++ )
        {
            pThis->Iface[iface].Sync.Slaved[head] =     0;
            pThis->Iface[iface].Sync.LocalSlave[head] = 0;
        }
    }

    for ( head = 0; head < numHeads; head++ )
    {
        // is this head to be slaved, or are we freeing all slaves?
        if ((Slaves & DisplayIds[head]) || !bEnableSlaves)
        {
            pThis->Iface[iface].Sync.Slaved[head] =     (bEnableSlaves &&  bCoupled);
            pThis->Iface[iface].Sync.LocalSlave[head] = (bEnableSlaves && !bCoupled);

            gsyncProgramFramelockEnable_P2060(pGpu, pThis, iface, bEnableSlaves);
        }
        else
        {
            // we are setting a slave, but it's not on this head, so nothing needs doing.
        }
    }

    if (!bEnableSlaves && !gsyncIsFrameLocked_P2060(pThis))
    {
        // Disable Framelock interrupts as board is not framelocked now.
        gsyncDisableFrameLockInterrupt_P2060((PDACEXTERNALDEVICE)pThis);

        rmStatus = gsyncUnApplyStereoPinAlwaysHiWar(pGpu);

        if (NV_OK != rmStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to drive stereo output pin for bug3362661.\n");
        }
    }

    // Reset FrameCountData and disable frame compare match interrupt.
    if (iface == pThis->FrameCountData.iface)
    {
        if (!(Slaves & DisplayIds[pThis->FrameCountData.head]))
        {
            if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
            {
                rmStatus |= gsyncResetFrameCountData_P2060(pGpu, pThis);
            }
        }
    }
    return rmStatus;
}

/*
 * Read P2060 slave for framelock.
 */
static NvU32
gsyncReadSlaves_P2060
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32       DisplayIds[OBJ_MAX_HEADS];
    NvU32       iface, head;
    NvU32       Slaves = 0;
    NV_STATUS   status;
    NvU32 numHeads = kdispGetNumHeads(pKernelDisplay);

    extdevGetBoundHeadsAndDisplayIds(pGpu, DisplayIds);

    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
        return 0;

    for ( head = 0; head < numHeads; head++ )
    {
        if (!pThis->Iface[iface].Sync.Master[head] &&
            (pThis->Iface[iface].Sync.Slaved[head] || pThis->Iface[iface].Sync.LocalSlave[head]))
        {
            Slaves |= DisplayIds[head];
        }
    }
    return Slaves;
}

static NV_STATUS
gsyncProgramSwapBarrier_P2060
(
    OBJGPU              *pGpu,
    PDACEXTERNALDEVICE   pExtDev,
    NvBool               bEnable
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU32          iface;
    NvU8           ctrl2 = 0;
    NV_STATUS      status = NV_OK;

    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
    {
       if (!IsSLIEnabled(pGpu))
       {
           //
           // If not in SLI, this function must only be called on GPUs connected
           // to the framelock board.
           //
           return status;
       }
       else
       {
           //
           // In SLI, we will try to add all GPUs in a topology to a swap
           // barrier, but not all of the GPUs actually have to be connected to
           // the framelock board, so we can return NV_OK to let the caller continue
           // on to the next GPU.
           //
           NV_PRINTF(LEVEL_INFO,
                     "Ignoring GPU %u not connected to the framelock board.\n",
                     gpumgrGetSubDeviceInstanceFromGpu(pGpu));
           return NV_OK;
       }
    }

    // Each connected GPU accesses it's own version of CONTROL2, on P2060
    if (GpuIsP2060Connected(pGpu, pThis))
    {
        status = readregu008_extdeviceTargeted(pGpu, pExtDev,
                                     NV_P2060_CONTROL2, &ctrl2);
        if (status != NV_OK)
        {
            return status;
        }

        if (pExtDev->deviceRev >= DAC_EXTERNAL_DEVICE_REV_MAX)
        {
            return NV_ERR_INVALID_STATE;
        }
    }

    if (pThis->Iface[iface].SwapReadyRequested == bEnable)
    {
       //
       // The SwapReadyRequested boolean keeps tracks of the current
       // requested state for this GPU. Skip the WAR algorithm if it's
       // already taken this GPU's state into account.
       //
       return NV_OK;
    }

    if (bEnable)  // Enable Swap_rdy
    {
        ctrl2 = FLD_SET_DRF(_P2060, _CONTROL2, _SWAP_READY, _ENABLE, ctrl2);

        if (pThis->tSwapRdyHiLsrMinTime == 0)
        {
            NvU32 data = 0;

            // store Swap Lockout Window in pThis.
            pThis->tSwapRdyHiLsrMinTime = NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME_DEFAULT;
            if (osReadRegistryDword(pGpu,
             NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME, &data) == NV_OK)
            {
                pThis->tSwapRdyHiLsrMinTime = data;
            }
        }

        NV_ASSERT(pThis->tSwapRdyHiLsrMinTime != 0);

        if (pThis->Iface[iface].DsiFliplock.saved == NV_FALSE)
        {
            KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
            NvU32          numHeads = kdispGetNumHeads(pKernelDisplay);
            NvU32          head ;

            for (head = 0; head < numHeads; head++)
            {
                NvU32 newLsrMinTime;
                if ((status = kdispComputeLsrMinTimeValue_HAL(pGpu, pKernelDisplay, head,
                             pThis->tSwapRdyHiLsrMinTime, &newLsrMinTime)) == NV_OK)
                {
                    NvU32 origLsrMinTime;
                    kdispSetSwapBarrierLsrMinTime_HAL(pGpu, pKernelDisplay, head, &origLsrMinTime,
                                      newLsrMinTime);

                    pThis->Iface[iface].DsiFliplock.OrigLsrMinTime[head] = origLsrMinTime;
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Error occured while computing LSR_MIN_TIME for Swap Barrier\n");
                    NV_ASSERT(0);
                }
            }
            pThis->Iface[iface].DsiFliplock.saved = NV_TRUE;
        }

        // The swapbarrier on master war is assumed to be fixed with fpga rev > 5.
        if ((!pThis->Iface[iface].skipSwapBarrierWar) &&
             needsMasterBarrierWar(&pThis->ExternalDevice))
        {
            if (gsyncIsP2060MasterBoard(pGpu, pThis) && GpuIsP2060Master(pGpu, pThis))
            {
                //
                // Swap Rdy signal on Master + TS GPU will enabled or disabled during
                // Enable/Disable of Master i.e. gsyncProgramMaster_P2060
                //
                pThis->Iface[iface].SwapReadyRequested = bEnable;
                return NV_OK;
            }
        }

        if (GpuIsConnectedToMasterViaBridge(pGpu, pThis) &&
           (gpumgrGetGpuBridgeType() == SLI_BT_VIDLINK || isBoardWithNvlinkQsyncContention(pGpu)))
        {
            //
            // Do not enable swapRdy Connection of pGpu. pGpu will take swap Rdy signal
            // from Master + TS GPU via SLI (MIO) bridge
            //
            pThis->Iface[iface].SwapReadyRequested = bEnable;
            return status; //NV_OK
        }
    }
    else
    {
        ctrl2 = FLD_SET_DRF(_P2060, _CONTROL2, _SWAP_READY, _DISABLE, ctrl2); // Disable Swap_rdy

        if (pThis->Iface[iface].DsiFliplock.saved == NV_TRUE)
        {
            KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
            NvU32          numHeads = kdispGetNumHeads(pKernelDisplay);
            NvU32          head ;

            for (head = 0; head < numHeads; head++)
            {
                kdispRestoreOriginalLsrMinTime_HAL(pGpu, pKernelDisplay, head,
                pThis->Iface[iface].DsiFliplock.OrigLsrMinTime[head]);
            }
            pThis->Iface[iface].DsiFliplock.saved = NV_FALSE;
        }

        // The swapbarrier on master war is assumed to be fixed with fpga rev > 5.
        if ((!pThis->Iface[iface].skipSwapBarrierWar) &&
             needsMasterBarrierWar(&pThis->ExternalDevice))
        {
            if (gsyncIsP2060MasterBoard(pGpu, pThis) && GpuIsP2060Master(pGpu, pThis))
            {
                //
                // Swap Rdy signal on Master + TS GPU will enabled or disabled during
                // Enable/Disable of Master i.e. gsyncProgramMaster_P2060
                //
                pThis->Iface[iface].SwapReadyRequested = bEnable;
                return NV_OK;
            }
        }
    }

    // Save the requested state for this GPU
    pThis->Iface[iface].SwapReadyRequested = bEnable;

    // Each connected GPU accesses it's own version of CONTROL2, on P2060
    if (GpuIsP2060Connected(pGpu, pThis))
    {
        status = writeregu008_extdeviceTargeted(pGpu, pExtDev,
                                                NV_P2060_CONTROL2, ctrl2);
    }

    return status;
}


static NV_STATUS
gsyncReadSwapBarrier_P2060
(
    OBJGPU             *pGpu,
    PDACEXTERNALDEVICE  pExtDev,
    NvBool             *bEnable
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU32 iface;
    NV_STATUS status = NV_OK;

    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
    {
       if (!IsSLIEnabled(pGpu))
       {
           //
           // If not in SLI, this function must only be called on GPUs connected
           // to the framelock board.
           //
           return status;
       }
       else
       {
           //
           // In SLI, we will try to read the swap barrier info from all GPUs in a
           // topology, but not all of the GPUs actually have to be connected to
           // the framelock board, so we can return NV_OK to let the caller continue
           // on to the next GPU.
           //
           NV_PRINTF(LEVEL_INFO,
                     "Ignoring GPU %u not connected to the framelock board.\n",
                     gpumgrGetSubDeviceInstanceFromGpu(pGpu));
           return NV_OK;
       }
    }

    if (gsyncIsP2060MasterBoard(pGpu, pThis) && GpuIsP2060Master(pGpu, pThis))
    {
        // Read swapRdy of Master + TS GPU.
        *bEnable = pThis->Iface[iface].SwapReadyRequested;
        return status; //NV_OK
    }

    if (GpuIsConnectedToMasterViaBridge(pGpu, pThis) &&
       (gpumgrGetGpuBridgeType() == SLI_BT_VIDLINK || isBoardWithNvlinkQsyncContention(pGpu)))
    {
        // Read swapRdy of pGpu connected to Master + TS GPU via SLI (MIO) bridge.
        *bEnable = pThis->Iface[iface].SwapReadyRequested;
        return status; //NV_OK
    }

    // Each connected GPU accesses it's own version of CONTROL2, on P2060
    if (GpuIsP2060Connected(pGpu, pThis))
    {
        NvU8 ctrl2 = 0;
        status = readregu008_extdeviceTargeted(pGpu,
                       pExtDev, NV_P2060_CONTROL2, &ctrl2);
        if (status != NV_OK)
        {
            return status;
        }
        *bEnable = (NV_P2060_CONTROL2_SWAP_READY_ENABLE ==
                       DRF_VAL(_P2060, _CONTROL2, _SWAP_READY, (NvU32)ctrl2));
    }

    return status;
}

static NV_STATUS
gsyncSetLsrMinTime
(
    OBJGPU                                   *pSourceGpu,
    PDACEXTERNALDEVICE                        pExtDev,
    NvU32                                     enable
)
{
    PDACP2060EXTERNALDEVICE pThis    = (PDACP2060EXTERNALDEVICE)pExtDev;
    NV_STATUS               rmStatus = NV_OK;
    NvU32                   index;

    // Get Mosaic Timing Source GPU Connector Index.
    if (NV_OK != GetP2060ConnectorIndexFromGpu(pSourceGpu, pThis, &index))
    {
        return NV_ERR_GENERIC;
    }

    if (enable)
    {
        // Set LSR_MIN_TIME
        if (pThis->tSwapRdyHiLsrMinTime == 0)
        {
            NvU32 data = 0;

            // store Swap Lockout Window in pThis.
            pThis->tSwapRdyHiLsrMinTime = NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME_DEFAULT;

            if (osReadRegistryDword(pSourceGpu,
                    NV_REG_STR_TIME_SWAP_RDY_HI_MODIFY_LSR_MIN_TIME, &data) == NV_OK)
            {
                pThis->tSwapRdyHiLsrMinTime = data;
            }
        }

        if (pThis->Iface[index].DsiFliplock.saved == NV_FALSE)
        {
            KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pSourceGpu);
            NvU32           numHeads = kdispGetNumHeads(pKernelDisplay);
            NvU32           head;

            // Do we need to loop over numHeads?
            for (head = 0; head < numHeads; head++)
            {
                NvU32 newLsrMinTime;

                if ((rmStatus = kdispComputeLsrMinTimeValue_HAL(pSourceGpu, pKernelDisplay, head,
                                    pThis->tSwapRdyHiLsrMinTime, &newLsrMinTime)) == NV_OK)
                {
                    NvU32 origLsrMinTime;

                    kdispSetSwapBarrierLsrMinTime_HAL(pSourceGpu, pKernelDisplay, head, &origLsrMinTime,
                        newLsrMinTime);

                    pThis->Iface[index].DsiFliplock.OrigLsrMinTime[head] = origLsrMinTime;
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Error occured while computing LSR_MIN_TIME for Swap Barrier\n");
                    NV_ASSERT(0);
                }
            }

            pThis->Iface[index].DsiFliplock.saved = NV_TRUE;
        }
    }
    else
    {
        if (pThis->Iface[index].DsiFliplock.saved == NV_TRUE)
        {
            KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pSourceGpu);
            NvU32           numHeads = kdispGetNumHeads(pKernelDisplay);
            NvU32           head;

            for (head = 0; head < numHeads; head++)
            {
                kdispRestoreOriginalLsrMinTime_HAL(pSourceGpu, pKernelDisplay, head,
                    pThis->Iface[index].DsiFliplock.OrigLsrMinTime[head]);
            }

            pThis->Iface[index].DsiFliplock.saved = NV_FALSE;
        }
    }

    return rmStatus;
}

NV_STATUS
gsyncSetMosaic_P2060
(
    OBJGPU                                   *pSourceGpu,
    PDACEXTERNALDEVICE                        pExtDev,
    NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NV_STATUS rmStatus = NV_OK;
    OBJGPU *pTempGpu = NULL;
    NvU8 mosaicReg;
    NvU32 i;
    NvU32 mosaicGroup = pParams->mosaicGroupNumber;

    if (mosaicGroup >= NV_P2060_MAX_MOSAIC_GROUPS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "mosaicGroup equaling/extending NV_P2060_MAX_MOSAIC_GROUPS.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->slaveGpuCount > NV_P2060_MAX_MOSAIC_SLAVES)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "mosaic slaveGpuCount extending NV_P2060_MAX_MOSAIC_SLAVES.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->enableMosaic) {

        NvU32 index;
        if (pThis->MosaicGroup[mosaicGroup].enabledMosaic)
        {
            // mosaic is already enabled for this group,
            // client should disable it first and re-query for this group.
            NV_PRINTF(LEVEL_ERROR,
                      "trying to enable mosaicGroup which is already enabled.\n");
            return NV_ERR_INVALID_ARGUMENT;
        }

        // Get Mosaic Timing Source GPU Connector Index.
        if ( NV_OK != GetP2060ConnectorIndexFromGpu(pSourceGpu, pThis, &index))
        {
             return NV_ERR_GENERIC;
        }

        pThis->MosaicGroup[mosaicGroup].slaveGpuCount   = pParams->slaveGpuCount;
        pThis->MosaicGroup[mosaicGroup].gpuTimingSource = pThis->Iface[index].GpuInfo.gpuId;

        for (i = 0; i < pThis->MosaicGroup[mosaicGroup].slaveGpuCount; i++)
        {
           pThis->MosaicGroup[mosaicGroup].gpuTimingSlaves[i] = pParams->gpuTimingSlaves[i];

           pTempGpu = gpumgrGetGpuFromId(pParams->gpuTimingSlaves[i]);
           NV_ASSERT_OR_RETURN(pTempGpu, NV_ERR_GENERIC);

           // Update register of mosaic timing slaves first
           mosaicReg = 0;
           mosaicReg = FLD_SET_DRF_NUM(_P2060, _MOSAIC_MODE, _TS,    (NvU8)index, mosaicReg);
           mosaicReg = FLD_SET_DRF_NUM(_P2060, _MOSAIC_MODE, _GROUP, (NvU8)mosaicGroup, mosaicReg);
           mosaicReg = FLD_SET_DRF(_P2060, _MOSAIC_MODE, _ENABLE, _TRUE, mosaicReg);

           rmStatus |= writeregu008_extdeviceTargeted(pTempGpu,
                          (PDACEXTERNALDEVICE)pThis, NV_P2060_MOSAIC_MODE, mosaicReg);
           if (rmStatus != NV_OK)
           {
               NV_PRINTF(LEVEL_ERROR,
                         "Failed to write P2060 mosaic slave register.\n");
               return NV_ERR_GENERIC;
           }

           // Set LSR_MIN_TIME
           gsyncSetLsrMinTime(pTempGpu, pExtDev, pParams->enableMosaic);
        }

        gsyncSetLsrMinTime(pSourceGpu, pExtDev, pParams->enableMosaic);

        // Update registers of mosaic timing source.
        mosaicReg = 0;
        mosaicReg = FLD_SET_DRF_NUM(_P2060, _MOSAIC_MODE, _TS,    (NvU8)index, mosaicReg);
        mosaicReg = FLD_SET_DRF_NUM(_P2060, _MOSAIC_MODE, _GROUP, (NvU8)mosaicGroup, mosaicReg);
        mosaicReg = FLD_SET_DRF(_P2060, _MOSAIC_MODE, _ENABLE, _TRUE, mosaicReg);

        rmStatus |= writeregu008_extdeviceTargeted(pSourceGpu,
                          (PDACEXTERNALDEVICE)pThis, NV_P2060_MOSAIC_MODE, mosaicReg);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to write P2060 mosaic Source register.\n");
            return NV_ERR_GENERIC;
        }

        // Mark as Mosaic enabled for specified group
        pThis->MosaicGroup[mosaicGroup].enabledMosaic = NV_TRUE;
    }
    else
    {
        if (!pThis->MosaicGroup[mosaicGroup].enabledMosaic)
        {
            // mosaicgroup is not enabled, so can not disable
            NV_PRINTF(LEVEL_ERROR,
                      "trying to disable mosaicGroup which is not enabled.\n");
            return NV_ERR_INVALID_ARGUMENT;
        }

        for (i = 0; i < pThis->MosaicGroup[mosaicGroup].slaveGpuCount; i++)
        {
            pTempGpu = gpumgrGetGpuFromId(pThis->MosaicGroup[mosaicGroup].gpuTimingSlaves[i]);
            NV_ASSERT_OR_RETURN(pTempGpu, NV_ERR_GENERIC);

            rmStatus |= writeregu008_extdeviceTargeted(pTempGpu,
                          (PDACEXTERNALDEVICE)pThis, NV_P2060_MOSAIC_MODE, 0x00);
            if (rmStatus != NV_OK)
            {
               NV_PRINTF(LEVEL_ERROR,
                         "Failed to write P2060 mosaic slave register.\n");
               return NV_ERR_GENERIC;
            }

            // Reset LSR_MIN_TIME
            gsyncSetLsrMinTime(pTempGpu, pExtDev, pParams->enableMosaic);
        }

        gsyncSetLsrMinTime(pSourceGpu, pExtDev, pParams->enableMosaic);

        rmStatus |= writeregu008_extdeviceTargeted(pSourceGpu,
                          (PDACEXTERNALDEVICE)pThis, NV_P2060_MOSAIC_MODE, 0x00);
        if (rmStatus != NV_OK)
        {
             NV_PRINTF(LEVEL_ERROR,
                       "Failed to write P2060 mosaic Source register.\n");
             return NV_ERR_GENERIC;
        }

        // reset structure for specified group
        gsyncResetMosaicData_P2060(mosaicGroup, pThis);
    }

    return rmStatus;
}

#ifdef DEBUG
/*
 * Helper function to printout the most relevant P2060_status
 * register informations in a human readable form.
 */
static void
DbgPrintP2060StatusRegister(NvU32 regStatus)
{
    if (DRF_VAL(_P2060, _STATUS, _SYNC_LOSS, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "SYNC_LOSS ");
    if (DRF_VAL(_P2060, _STATUS, _STEREO, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "STEREO_LOCK ");
    if (NV_P2060_STATUS_VCXO_LOCK == DRF_VAL(_P2060, _STATUS, _VCXO, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "VCXO_LOCK ");
    if (NV_P2060_STATUS_VCXO_NOLOCK_TOO_FAST == DRF_VAL(_P2060, _STATUS, _VCXO, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "VCXO_NOLOCK_TOO_FAST ");
    if (NV_P2060_STATUS_VCXO_NOLOCK_TOO_SLOW == DRF_VAL(_P2060, _STATUS, _VCXO, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "VCXO_NOLOCK_TOO_SLOW ");
    if (NV_P2060_STATUS_VCXO_NOT_SERVO == DRF_VAL(_P2060, _STATUS, _VCXO, regStatus))
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "VCXO_NOT_SERVO ");
}
#else
#define DbgPrintP2060StatusRegister(regStatus)
#endif

static NV_STATUS
gsyncGpuStereoHeadSync(OBJGPU *pGpu, NvU32 iface, PDACEXTERNALDEVICE pExtDev, NvU32 status1)
{
    DACP2060EXTERNALDEVICE *pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    RM_API   *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32     hClient     = pGpu->hInternalClient;
    NvU32     hSubdevice  = pGpu->hInternalSubdevice;
    NV_STATUS status      = NV_OK;
    NvU32     numHeads    = kdispGetNumHeads(pKernelDisplay);
    NvU32     headIdx;
    NV2080_CTRL_INTERNAL_GSYNC_SET_STREO_SYNC_PARAMS ctrlParams = {0};

    for (headIdx = 0; headIdx < numHeads; headIdx++)
    {
        ctrlParams.slave[headIdx]      = pThis->Iface[iface].Sync.Slaved[headIdx];
        ctrlParams.localSlave[headIdx] = pThis->Iface[iface].Sync.LocalSlave[headIdx];
        ctrlParams.master[headIdx]     = pThis->Iface[iface].Sync.Master[headIdx];
    }

    ctrlParams.regStatus = status1;
    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_GSYNC_SET_STREO_SYNC,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Stereo headsync failed\n");
    }

    return status;
}

/*
 * Update the status register snapshot and
 * send loss/gain events to client if any.
 */
static NV_STATUS
gsyncUpdateGsyncStatusSnapshot_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NvU32 iface;
    NvU8 regStatus = 0;
    NV_STATUS rmStatus = NV_OK;
    NvU32 ifaceEvents[NV_P2060_MAX_IFACES_PER_GSYNC];

    // Only read status variables if the board is framelocked at all.
    if (!gsyncIsFrameLocked_P2060(pThis))
    {
        return rmStatus;
    }

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        NvU32 diffStatus = 0x00;
        NvU32 oldStatus  = 0x00;
        NvU32 newStatus  = 0x00;
        ifaceEvents[iface] = 0x00;

        // get status update for each interface
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            int updateSnapshot = 1;
            int localMaster = 0;

            // Take care of tracking state of connected gpu, not per incoming.
            OBJGPU *pIfaceGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

            NV_ASSERT(pIfaceGpu);

            rmStatus  = readregu008_extdeviceTargeted(pIfaceGpu, pExtDev, (NvU8)NV_P2060_STATUS, &regStatus);
            if ( NV_OK != rmStatus )
            {
                return NV_ERR_GENERIC;
            }

            oldStatus = pThis->Snapshot[iface].Status1;
            newStatus = (NvU32)regStatus;

            // Check for a local master which generates the sync so it's always synched.
            if ( (NV_P2060_STATUS_VCXO_NOT_SERVO  == DRF_VAL(_P2060, _STATUS, _VCXO,      newStatus)) &&
                 (NV_P2060_STATUS_SYNC_LOSS_FALSE == DRF_VAL(_P2060, _STATUS, _SYNC_LOSS, newStatus)))
            {
                localMaster = 1;
                if (!pThis->Iface[iface].gainedSync)
                {
                    NV_PRINTF(LEVEL_INFO,
                              "P2060[%d] is local master => GAINED SYNC\n",
                              iface);
                    pThis->Iface[iface].gainedSync = 1;
                }
            }
            else
            {
                // For slaves or masters driven by housesync we need to wait for a sync.

                // 1st wait for syncgain before doing anything else.
                if ((!pThis->Iface[iface].gainedSync) &&
                    (NV_P2060_STATUS_VCXO_LOCK == DRF_VAL(_P2060, _STATUS, _VCXO, newStatus)))
                {
                    OBJTMR *pTmr = GPU_GET_TIMER(pIfaceGpu);
                    NvU64 timeDiff;
                    NvU64 currentTime;

                    // get the current time
                    currentTime = tmrGetTime_HAL(pIfaceGpu, pTmr);

                    //
                    // Initialize waittime.
                    // We're using a threshold of 10 seconds before we're accepting sync gain.
                    //
                    if (0 == pThis->Snapshot[iface].lastSyncCheckTime)
                    {
                        pThis->Snapshot[iface].lastSyncCheckTime = currentTime;
                    }

                    // calculate the time difference from last change.
                    timeDiff = ((currentTime - pThis->Snapshot[iface].lastSyncCheckTime) / 1000000); // time in ms

                    NV_PRINTF(LEVEL_INFO,
                              "P2060[%d] snapshot timeDiff is %d ms\n", iface,
                              (NvU32)timeDiff);

                    //
                    // Update settings if we got no lock or if lock has settled long enough.
                    // This is currently selected to 5 seconds by experiments with syncing
                    // with stereo enabled.
                    //
                    if (timeDiff >= 5000)
                    {
                        NV_PRINTF(LEVEL_INFO, "P2060[%d] GAINED SYNC\n",
                                  iface);

                        pThis->Iface[iface].gainedSync = 1;

                        // We've gained sync, right time to also sync the stereo phase.
                        if (FLD_TEST_DRF(_P2060, _STATUS, _GPU_STEREO, _ACTIVE, newStatus))
                        {
                            gsyncGpuStereoHeadSync(pIfaceGpu, iface, pExtDev, newStatus);
                            // Reset toggle time to wait some time before checking stereo sync again.
                            pThis->Snapshot[iface].lastStereoToggleTime = 0;
                        }
                    }
                    else
                    {
                        //
                        // We haven't reached the settle down time for a sync,
                        // so don't update the snapshot this time.
                        //
                        updateSnapshot = 0;
                    }
                }
            }

            // Take over new status and send events only if desired.
            if (updateSnapshot)
            {
                //
                // If we lost sync again reestablish syncwait mechanism.
                // Local master can't loose sync per definition.
                //
                if ((NV_P2060_STATUS_VCXO_LOCK != DRF_VAL(_P2060, _STATUS, _VCXO, newStatus)) &&
                    (!localMaster))
                {
                    pThis->Iface[iface].gainedSync = 0;
                    pThis->Snapshot[iface].lastSyncCheckTime = 0;
                }

                NV_PRINTF(LEVEL_INFO, "Update P2060[%d] settled from 0x%x ( ",
                          iface, oldStatus);
                DbgPrintP2060StatusRegister(oldStatus);
                NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, ") to 0x%x ( ", newStatus);
                DbgPrintP2060StatusRegister(newStatus);
                NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, ")\n");

                diffStatus = (oldStatus ^ newStatus);

                pThis->Snapshot[iface].Status1 = newStatus;
            }
        }
        else
        {
            continue;
        }

        if ((DRF_VAL(_P2060, _STATUS, _VCXO,      diffStatus)) ||
            (DRF_VAL(_P2060, _STATUS, _SYNC_LOSS, diffStatus)) ) // diff state: sync or lock
        {
            if (FLD_TEST_DRF(_P2060, _STATUS, _SYNC_LOSS, _FALSE, newStatus) &&
                (FLD_TEST_DRF(_P2060, _STATUS, _VCXO,      _LOCK, newStatus) ||
                 FLD_TEST_DRF(_P2060, _STATUS, _VCXO, _NOT_SERVO, newStatus)))
            {
                ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_GAIN(iface));
            }
            else
            {
                ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface));
            }
        }

        // Only track stereo diff states when we are VCXO locked or NOT SERVO.
        if ((FLD_TEST_DRF(_P2060, _STATUS, _VCXO, _LOCK, newStatus) ||
             FLD_TEST_DRF(_P2060, _STATUS, _VCXO, _NOT_SERVO, newStatus)) &&
             DRF_VAL(_P2060, _STATUS, _STEREO, diffStatus)) // diff state: stereo
        {
            if (FLD_TEST_DRF(_P2060, _STATUS, _STEREO, _LOCK, newStatus))
            {
                ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_GAIN(iface));
            }
            else
            {
                ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface));
            }
        }
    }

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        // Only check stereo phase if we've gained sync.
        if (pThis->Iface[iface].gainedSync)
        {
            // Only check and adjust stereo phase if stereo is enabled on this system.
            if (FLD_TEST_DRF(_P2060, _STATUS, _GPU_STEREO, _ACTIVE, pThis->Snapshot[iface].Status1))
            {
                //
                // gsyncGpuStereoHeadSync() works without any gsync board interaction
                // with gpu register access only. In addition it will also sync heads
                // which are not driving the stereo pin and can't be monitored by the
                // gsync board.
                // So don't look at the gsync stereolock status as this doesn't cover
                // all heads.
                // Take care of tracking state of connected gpu, not per incoming.
                //
                OBJGPU *pIfaceGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);
                OBJTMR *pIfaceTmr;
                NvU32 timeDiff;
                NvU64 currentTime;

                NV_ASSERT(pIfaceGpu);

                pIfaceTmr = GPU_GET_TIMER(pIfaceGpu);

                currentTime = tmrGetTime_HAL(pIfaceGpu, pIfaceTmr);  // get the current time

                if (pThis->Snapshot[iface].lastStereoToggleTime == 0)
                {
                    pThis->Snapshot[iface].lastStereoToggleTime = currentTime;
                }
                timeDiff = (NvU32)((currentTime - pThis->Snapshot[iface].lastStereoToggleTime) / 1000000); // time in ms

                // toggle stereo if it is not locked more than 5 sec.
                if (timeDiff >= 5000)
                {
                    gsyncGpuStereoHeadSync(pIfaceGpu, iface, pExtDev, pThis->Snapshot[iface].Status1);
                    pThis->Snapshot[iface].lastStereoToggleTime = currentTime;
                }

                // Only report stereo loss if stereo is not in phase.
                if (!(FLD_TEST_DRF(_P2060, _STATUS, _STEREO, _LOCK, pThis->Snapshot[iface].Status1)))
                {
                    ifaceEvents[iface] |= NVBIT(NV30F1_GSYNC_NOTIFIERS_STEREO_LOSS(iface)); // report loss this time.
                }
            }
        }
    }

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (ifaceEvents[iface] && (pThis->Iface[iface].lastEventNotified != ifaceEvents[iface]))
        {
            NV_PRINTF(LEVEL_INFO, "Event P2060[%d]: 0x%x (", iface,
                      ifaceEvents[iface]);
            gsyncDbgPrintGsyncEvents(ifaceEvents[iface], iface);
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, ")\n");

            // notify clients that something has happened!
            gsyncSignalServiceRequested(gsyncGetGsyncInstance(pGpu), ifaceEvents[iface], iface);
            pThis->Iface[iface].lastEventNotified = ifaceEvents[iface];
        }
    }

    return NV_OK;
}


/*
 * Handle Get and Set queries related to signals.
 */
NV_STATUS
gsyncRefSignal_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    REFTYPE            rType,
    GSYNCSYNCSIGNAL    Signal,
    NvBool             bRate,
    NvU32              *pPresence
)
{
    NV_STATUS status = NV_OK;
    NvU32 rate;
    NvU32 value = 0;
    NvU32 bMaster; // Is this Gsync in Master mode
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;

    if (!bRate)
    {
        return NV_OK;
    }

    //
    // Is any display on this P2060 a master, or are we using an ext
    // sync to be master.
    //
    bMaster = !!gsyncReadMaster_P2060(pGpu, pThis);

    switch ( rType )
    {
    case refFetchGet:
         if (gsync_Signal_RJ45_0 == Signal || gsync_Signal_RJ45_1 == Signal)
         {
             // Relevant only if this port is an input
             NvU32 port0Direction, expectedPort0Direction;
             NvU32 port1Direction, expectedPort1Direction;

             status = gsyncReadFrameRate_P2060(pGpu, pExtDev, &rate);
             if (NV_OK != status)
                 return status;

             status  = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, (NvU8*)&value);
             if (NV_OK != status)
                 return status;

             port0Direction = DRF_VAL(_P2060, _STATUS2, _PORT0, value);
             port1Direction = DRF_VAL(_P2060, _STATUS2, _PORT1, value);

             if (gsync_Signal_RJ45_0 == Signal)
             {
                 expectedPort0Direction = NV_P2060_STATUS2_PORT0_INPUT;
                 expectedPort1Direction = NV_P2060_STATUS2_PORT1_OUTPUT;
             }
             else
             {
                 expectedPort0Direction = NV_P2060_STATUS2_PORT0_OUTPUT;
                 expectedPort1Direction = NV_P2060_STATUS2_PORT1_INPUT;
             }
             if (port0Direction == expectedPort0Direction && port1Direction == expectedPort1Direction)
             {
                 if (bMaster)
                 {
                     *pPresence = ~0;
                 }
                 else
                 {
                     *pPresence = rate;
                 }
             }
             else
             {
                *pPresence = 0;
             }
         }
         else if (gsync_Signal_House == Signal)
         {
             status = gsyncReadHouseSignalPresent_P2060(pGpu, pExtDev, NV_FALSE, &value);

             if (value && NV_OK == status)
             {
                 status = gsyncReadHouseSyncFrameRate_P2060(pGpu, pExtDev, &rate);
                 if (NV_OK != status)
                     return status;

                 if (bMaster)
                 {
                    *pPresence = rate;
                 }
                 else
                 {
                    *pPresence = ~0;
                 }
             }
             else
             {
                 *pPresence = 0;
             }
         }
         else
         {
             return NV_ERR_GENERIC;
         }
         break;

    case refRead:
         break;
    default:
         return NV_ERR_GENERIC;
    }

    return status;
}

/*
 * Handle Get and Set queries related to Master.
 */
NV_STATUS
gsyncRefMaster_P2060
(
    OBJGPU *pGpu,
    OBJGSYNC *pGsync,
    REFTYPE rType,
    NvU32 *pDisplayMask,
    NvU32 *pRefresh,
    NvBool retainMaster,
    NvBool skipSwapBarrierWar
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pGsync->pExtDev;
    NvU32 Master = pThis->Master;
    NvU32 RefreshRate = pThis->RefreshRate;
    NV_STATUS status = NV_OK;

    switch ( rType )
    {
    case refSetCommit:
        if (!retainMaster)
        {
            Master = pThis->Master = *pDisplayMask;
        }
        RefreshRate = pThis->RefreshRate = *pRefresh;
        break;
    default:
        break;
    }

    switch ( rType )
    {
    case refSetCommit:
        // Only masterable gpus can be set to master, but either can be set to non-master.
        if (Master && (!gsyncGpuCanBeMaster_P2060(pGpu, (PDACEXTERNALDEVICE)pThis)))
        {
            NV_PRINTF(LEVEL_INFO, "P2060 GPU can not be Framelock Master.\n");
            return NV_ERR_GENERIC;
        }

        if (GpuIsMosaicTimingSlave(pGpu, pThis))
        {
            NV_PRINTF(LEVEL_INFO,
                      "P2060 GPU is mosaic timing slave. Can not set Framelock Master.\n");
            return NV_ERR_GENERIC;
        }

        status = gsyncProgramMaster_P2060(pGpu, pGsync, Master, retainMaster, skipSwapBarrierWar);
        break;

    case refFetchGet:
    case refRead:
        Master = gsyncReadMaster_P2060(pGpu, pThis);
        break;
    default:
        break;
    }

    switch ( rType )
    {
    case refFetchGet:
        pThis->Master = Master;
        /*NOBREAK*/
    case refRead:
        *pDisplayMask = Master;
        *pRefresh = RefreshRate;
        break;

    default:
        break;
    }

    return status;
}

/*
 * Handle Get and Set queries related to Slaves.
 */
NV_STATUS
gsyncRefSlaves_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    REFTYPE rType,
    NvU32 *pDisplayMasks,
    NvU32 *pRefresh
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    NV_STATUS status = NV_OK;
    NvU32 Slaves = pThis->Slaves;
    NvU32 RefreshRate = pThis->RefreshRate;

    switch ( rType )
    {
    case refSetCommit:
        pThis->Slaves = *pDisplayMasks;
        pThis->RefreshRate = *pRefresh;
        Slaves = pThis->Slaves;
        RefreshRate = pThis->RefreshRate;
        break;
    default:
        break;
    }

    switch ( rType )
    {
    case refSetCommit:
        status = gsyncProgramSlaves_P2060(pGpu, pThis, Slaves);
        break;

    case refFetchGet:
    case refRead:
        Slaves = gsyncReadSlaves_P2060(pGpu, pThis);
        break;
    default:
        break;
    }

    switch ( rType )
    {
    case refFetchGet:
        pThis->Slaves = Slaves;
        /*NOBREAK*/
    case refRead:
        *pDisplayMasks = Slaves;
        *pRefresh = RefreshRate;
        break;

    default:
        break;
    }
    return status;
}

/*
 * Handle Get queries related to CPL status.
 */
NV_STATUS
gsyncGetCplStatus_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    GSYNCSTATUS CplStatus,
    NvU32 *pVal
)
{
    NV_STATUS status = NV_OK;
    NvU8  regStatus2;

    *pVal = 0; // A little safety for those that do not check return codes

    switch (CplStatus)
    {
        case gsync_Status_Refresh:
            // Read GSYNC Framerate value.
            status = gsyncReadFrameRate_P2060(pGpu, pExtDev, pVal);
            break;

        case gsync_Status_HouseSyncIncoming:
            *pVal = 0;
            status = gsyncReadHouseSignalPresent_P2060(pGpu, pExtDev, NV_FALSE, pVal);
            if (NV_OK == status &&  *pVal)
            {
                // Read HS Framerate value.
                status = gsyncReadHouseSyncFrameRate_P2060(pGpu, pExtDev, pVal);
            }
            break;

        case gsync_Status_bSyncReady:
            status = gsyncReadIsSyncDetected_P2060(pGpu, pExtDev, pVal);
            break;

        case gsync_Status_bSwapReady:
            *pVal = 0; // counters should exist in P2060?
            break;

        case gsync_Status_bTiming:
            status = gsyncReadIsTiming_P2060(pGpu, pExtDev, pVal);
            break;

        case gsync_Status_bStereoSync:
            status = gsyncReadStereoLocked_P2060(pGpu, pExtDev, pVal);
            break;

        case gsync_Status_bHouseSync:
            status = gsyncReadHouseSignalPresent_P2060(pGpu, pExtDev, NV_FALSE, pVal);
            break;

        case gsync_Status_bPort0Input:
            status = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, &regStatus2);
            if ( NV_OK == status )
            {
                *pVal = FLD_TEST_DRF(_P2060, _STATUS2, _PORT0, _INPUT, (NvU32)regStatus2);
            }
            break;

        case gsync_Status_bPort1Input:
            status = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, &regStatus2);
            if ( NV_OK == status )
            {
                *pVal = FLD_TEST_DRF(_P2060, _STATUS2, _PORT1, _INPUT, (NvU32)regStatus2);
            }
            break;

        case gsync_Status_bPort0Ethernet:
            status = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, &regStatus2);
            if ( NV_OK == status )
            {
                *pVal = FLD_TEST_DRF(_P2060, _STATUS2, _ETHER0_DETECTED, _TRUE, (NvU32)regStatus2);
            }
            break;

        case gsync_Status_bPort1Ethernet:
            status = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS2, &regStatus2);
            if ( NV_OK == status )
            {
                *pVal = FLD_TEST_DRF(_P2060, _STATUS2, _ETHER1_DETECTED, _TRUE, (NvU32)regStatus2);
            }
            break;

        case gsync_Status_UniversalFrameCount:
            status = gsyncReadUniversalFrameCount_P2060(pGpu, pExtDev, pVal);
            break;

        default:
            status = NV_ERR_GENERIC;
    }

    return status;
}

/*
 * Handle Get and Set queries related to Watchdog.
 */
NV_STATUS
gsyncSetWatchdog_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev,
    NvU32 pVal
)
{
    OBJGPU *pTempGpu = NULL;
    PDACP2060EXTERNALDEVICE pP2060 = (PDACP2060EXTERNALDEVICE)pExtDev;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pGpu && pP2060, NV_ERR_INVALID_DEVICE);

    if (pVal)
    {
        gsyncCancelWatchdog_P2060(pP2060);
        pTempGpu = GetP2060WatchdogGpu(pGpu, pP2060);

        extdevScheduleWatchdog(pTempGpu, (PDACEXTERNALDEVICE)pP2060);
        if (!gsyncIsOnlyFrameLockMaster_P2060(pP2060))
        {
            pP2060->watchdogCountDownValue = NV_P2060_WATCHDOG_COUNT_DOWN_VALUE;
        }
    }

    return status;
}

/*
 * Handle Get and Set queries related to board revision.
 */
NV_STATUS
gsyncGetRevision_P2060
(
    OBJGPU *pGpu,
    OBJGSYNC *pGsync,
    GSYNCCAPSPARAMS *pParams
)
{
    OBJSYS  *pSys = SYS_GET_INSTANCE();
    DACEXTERNALDEVICE *pExtDev;
    NV_STATUS status = NV_OK;
    DAC_EXTERNAL_DEVICES deviceId;
    DAC_EXTERNAL_DEVICE_REVS deviceRev;

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_PARAMETER);
    NV_ASSERT_OR_RETURN(pGsync != NULL, NV_ERR_INVALID_PARAMETER);
    pExtDev = pGsync->pExtDev;
    NV_ASSERT_OR_RETURN(pExtDev != NULL, NV_ERR_INVALID_PARAMETER);
    deviceId = pExtDev->deviceId;
    deviceRev = pExtDev->deviceRev;

    portMemSet(pParams, 0, sizeof(*pParams));

    pParams->revId = (NvU32)pExtDev->revId;
    pParams->boardId = (NvU32)deviceId;
    pParams->revision = (NvU32)deviceRev;
    pParams->extendedRevision = (NvU32)pExtDev->deviceExRev;

    if ((deviceRev != DAC_EXTERNAL_DEVICE_REV_NONE) &&
        (deviceId == DAC_EXTERNAL_DEVICE_P2060 ||
         deviceId == DAC_EXTERNAL_DEVICE_P2061))
    {
        DACP2060EXTERNALDEVICE *p2060 = (DACP2060EXTERNALDEVICE *)pExtDev;

        pParams->capFlags = NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_FREQ_ACCURACY_3DPS;

        if (!pSys->getProperty(pSys, PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED))
        {
            pParams->isFirmwareRevMismatch = isFirmwareRevMismatch(pGpu, pGsync, deviceRev);
        }
        else
        {
            pParams->isFirmwareRevMismatch = NV_FALSE;
        }

        pParams->maxSyncSkew          = p2060->syncSkewMax;
        pParams->syncSkewResolution   = p2060->syncSkewResolutionInNs;
        pParams->maxStartDelay        = NV_P2060_START_DELAY_MAX_UNITS;
        pParams->startDelayResolution = NV_P2060_START_DELAY_RESOLUTION;
        pParams->maxSyncInterval      = NV_P2060_SYNC_INTERVAL_MAX_UNITS;

        // let client know which events we support
        pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_SYNC_LOCK_EVENT;
        pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_HOUSE_SYNC_EVENT;

        // all connectors are capable of generating events
        pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ALL_CONNECTOR_EVENT;

        // clients can only request (i.e. not SET) for video mode at BNC connector.
        pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_ONLY_GET_VIDEO_MODE;

        // Fpga revisions <= 5 need to have the swapbarrier set on framelock masters
        // to drive (pull up) the swap_rdy line of the whole framelock setup.
        // This is a behaviour with unwanted side effects which needs drivers wars
        // for certain configs.
        if (needsMasterBarrierWar(pExtDev))
        {
            pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_NEED_MASTER_BARRIER_WAR;
        }

        if (supportsMulDiv(pExtDev))
        {
            pParams->capFlags |= NV30F1_CTRL_GSYNC_GET_CAPS_CAP_FLAGS_MULTIPLY_DIVIDE_SYNC;
            pParams->maxMulDivValue = (NV_P2060_MULTIPLIER_DIVIDER_VALUE_MINUS_ONE_MAX + 1);
        }
    }
    else
    {
        pParams->boardId = DAC_EXTERNAL_DEVICE_NONE;
    }

    return status;
}

/*
 * Handle Get and Set queries related to Swap barrier.
 */
NV_STATUS
gsyncRefSwapBarrier_P2060
(
    OBJGPU              *pGpu,
    PDACEXTERNALDEVICE   pExtDev,
    REFTYPE              rType,
    NvBool               *bEnable
)
{
    NV_STATUS status = NV_OK;

    switch (rType)
    {
        case refSetCommit:
            status = gsyncProgramSwapBarrier_P2060(pGpu, pExtDev, *bEnable);
            break;
        case refFetchGet:
        case refRead:
            status = gsyncReadSwapBarrier_P2060(pGpu, pExtDev, bEnable);
            break;
        default:
            break;
    }

    return status;
}

/*
 * Configure GSYNC registers for pre-flash and post-flash operations.
 */
NV_STATUS
gsyncConfigFlashGsync_P2060
(
    OBJGPU                *pGpu,
    PDACEXTERNALDEVICE     pExtDev,
    NvU32                  preFlash
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU32 iface;
    NV_STATUS rmStatus = NV_OK;

    if (preFlash)
    {
        if (pThis->isNonFramelockInterruptEnabled == NV_FALSE)
        {
            // Non-Framelock interrupts are already disabled.
            return NV_OK;
        }

        // Disable non-Framelock interrupts for given gpu
        for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
        {
            if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID &&
                pThis->interruptEnabledInterface == iface)
            {
                OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

                NV_ASSERT(pTmpGpu);

                rmStatus = gsyncDisableNonFramelockInterrupt_P2060(pTmpGpu, (PDACEXTERNALDEVICE)pThis);
                if (rmStatus != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to disable non-framelock interrupts on gsync GPU.\n");
                    return rmStatus;
                }
                break;
            }
        }
        pThis->isNonFramelockInterruptEnabled = NV_FALSE;
    }
    else
    {
        if (pThis->isNonFramelockInterruptEnabled == NV_FALSE)
        {
            // Enable non-Framelock interrupts for given gpu
            rmStatus = GetP2060ConnectorIndexFromGpu(pGpu, pThis, &iface);
            if (NV_OK != rmStatus)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "failed to find P2060 connector of the GPU.\n");
                return rmStatus;
            }

            rmStatus = gsyncEnableNonFramelockInterrupt_P2060(pGpu, (PDACEXTERNALDEVICE)pThis);
            if (NV_OK != rmStatus)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to enable non-framelock interrupts on gsync GPU.\n");
                return rmStatus;
            }
            pThis->isNonFramelockInterruptEnabled = NV_TRUE;
            pThis->interruptEnabledInterface = iface;
        }

        // Program External Stereo Sync Polarity for all attached gpus.
        for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
        {
            if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
            {
                OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

                NV_ASSERT(pTmpGpu);

                rmStatus = gsyncProgramExtStereoPolarity_P2060(pTmpGpu, (PDACEXTERNALDEVICE)pThis);
                if (NV_OK != rmStatus)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to Program External Stereo Polarity for GPU.\n");
                    return rmStatus;
                }
            }
        }
    }
    return rmStatus;
}

/*
 * Return snapshot of status1 register.
 */
static NvU32
GetP2060GpuSnapshot
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;
    NV_STATUS status;

    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
        return 0;

    return pThis->Snapshot[iface].Status1;
}

/*
 * Return NV_TRUE if pGpu is Timing Source else return NV_FALSE.
 */
static NvBool
GpuIsP2060Master
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU8 regControl;
    NvU32 index;
    NvBool bIsMasterGpu;
    NV_STATUS rmStatus;

    if (!pGpu || !GpuIsP2060Connected(pGpu, pThis))
    {
        return NV_FALSE;
    }

    // Get the connector index and check if it is master
    if ( NV_OK != GetP2060ConnectorIndexFromGpu(pGpu, pThis, &index))
    {
        return NV_FALSE;
    }

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, (NvU8)NV_P2060_CONTROL, &regControl);
    if (NV_OK != rmStatus)
    {
        return NV_FALSE;
    }

    bIsMasterGpu = (DRF_VAL(_P2060, _CONTROL, _SYNC_SRC, (NvU32)regControl) == index);
    return bIsMasterGpu;
}

/*
 * Return whether pGpu is connected to pThis or not.
 */
static NvBool
GpuIsP2060Connected
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;

    if (NV_OK == GetP2060GpuLocation(pGpu, pThis, &iface))
    {
        return pThis->Iface[iface].GpuInfo.connected;
    }

    return NV_FALSE;
}

/*
 * Return Masterable(TS) Gpu for pthis.
 */
static OBJGPU*
GetP2060MasterableGpu
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    OBJGPU *tempGpu;
    NvU32 iface;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            tempGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

            NV_ASSERT(tempGpu);

            if (gsyncGpuCanBeMaster_P2060(tempGpu, (PDACEXTERNALDEVICE)pThis))
            {
                return tempGpu;
            }
        }
    }

    return NULL;
}

/*
 * Return connector index for given pGpu.
 */
static NV_STATUS
GetP2060ConnectorIndexFromGpu
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis,
    NvU32 *index
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU8 regStatus2;

    rmStatus  = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, (NvU8)NV_P2060_STATUS2, &regStatus2);
    if (NV_OK == rmStatus)
    {
        *index = DRF_VAL(_P2060, _STATUS2, _GPU_PORT, (NvU32)regStatus2);
    }

    return rmStatus;
}

/*
 * Return location of pGpu attached to P2060.
 */
static NV_STATUS
GetP2060GpuLocation
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis,
    NvU32 *iface
)
{
    NvU32 tempIface;

    for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
    {
        if (pThis->Iface[tempIface].GpuInfo.gpuId == pGpu->gpuId)
        {
            *iface = tempIface;

            return NV_OK;
        }
    }

    return NV_ERR_GENERIC;
}

static void
gsyncProgramFramelockEnable_P2060
(
    OBJGPU                 *pGpu,
    PDACP2060EXTERNALDEVICE pThis,
    NvU32                   iface,
    NvBool                  bEnable
)
{
    //
    // Key here is to force the status register snapshot
    // to unsynched and setting back the timeout for
    // syncgain. To ensure that the eventhandling won't
    // filter away any gain, send a syncloss event.
    // also track overall framelock state (on off) now.
    //
    NV_PRINTF(LEVEL_INFO,
              "P2060[%d]:%s snapshot reset to _SYNC_LOSS_TRUE _VCXO_NOT_SERVO _STEREO_NOLOCK\n",
              iface, bEnable ? "ON" : "OFF");

    pThis->Snapshot[iface].Status1 =
        DRF_DEF(_P2060, _STATUS, _SYNC_LOSS, _TRUE) |
        DRF_DEF(_P2060, _STATUS, _VCXO, _NOT_SERVO) |
        DRF_DEF(_P2060, _STATUS, _STEREO, _NOLOCK);

    pThis->Snapshot[iface].lastStereoToggleTime = 0;

    // Also reload delayed updates.
    pThis->Snapshot[iface].lastSyncCheckTime = 0;

    // We will waiting for a syncgain.
    pThis->Iface[iface].gainedSync = 0;

    if (bEnable)
    {
        gsyncSignalServiceRequested(gsyncGetGsyncInstance(pGpu),
            NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface)), iface);
        pThis->Iface[iface].lastEventNotified =
            NVBIT(NV30F1_GSYNC_NOTIFIERS_SYNC_LOSS(iface));
    }
}

static NvBool
GpuIsMosaicTimingSlave
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    OBJGPU *pTempGpu = NULL;
    NvU8 i, j;

    for (i = 0; i < NV_P2060_MAX_MOSAIC_GROUPS; i++)
    {
        if (pThis->MosaicGroup[i].enabledMosaic)
        {
             for (j = 0; j < NV_P2060_MAX_MOSAIC_SLAVES; j++)
             {
                  pTempGpu = gpumgrGetGpuFromId(pThis->MosaicGroup[i].gpuTimingSlaves[j]);
                  NV_ASSERT(pTempGpu);

                  if (pTempGpu == pGpu)
                  {
                      return NV_TRUE;
                  }
             }
        }
    }
    return NV_FALSE;
}

/*
 * Return NV_TRUE if pGpu can be master(TS) i.e. no other pGpu
 * attached to pThis is master.
 */
NvBool
gsyncGpuCanBeMaster_P2060
(
    OBJGPU *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE)pExtDev;
    OBJGPU *pTempGpu;
    NvU32 iface, tempIface;
    NV_STATUS status;

    // If FPGA board in not master, Any GPU attached to board can be master.
    if (!gsyncIsP2060MasterBoard(pGpu, pThis))
    {
        return NV_TRUE;
    }

    // Board is master, only TS GPU will be master.
    status = GetP2060GpuLocation(pGpu, pThis, &iface);
    if (NV_OK != status)
    {
        return NV_FALSE;
    }

    for (tempIface = 0; tempIface < NV_P2060_MAX_IFACES_PER_GSYNC; tempIface++)
    {
        if (tempIface == iface)
        {
            continue;
        }

        if (pThis->Iface[tempIface].GpuInfo.gpuId == NV0000_CTRL_GPU_INVALID_ID)
        {
            continue;
        }

        pTempGpu = gpumgrGetGpuFromId(pThis->Iface[tempIface].GpuInfo.gpuId);

        NV_ASSERT(pTempGpu);

        if (GpuIsP2060Master(pTempGpu, pThis))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}


static POBJGPU
GetP2060Gpu
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;
    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

            NV_ASSERT(pTmpGpu);

            return pTmpGpu;
        }
    }
    return NULL;
}

static OBJGPU*
GetP2060WatchdogGpu
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
   return GetP2060Gpu(pGpu, pThis);
}

/*
 * Return NV_TRUE if either GPU stereo or MASTER stereo or
 * both are enabled else return NV_FALSE.
 */
static NvBool
gsyncIsStereoEnabled_p2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    NvU8 regStatus;
    NV_STATUS rmStatus;

    rmStatus  = readregu008_extdeviceTargeted(pGpu, pExtDev, (NvU8)NV_P2060_STATUS,  &regStatus);

    if (rmStatus == NV_OK)
    {
        if (FLD_TEST_DRF(_P2060, _STATUS, _GPU_STEREO, _ACTIVE, regStatus) ||
            FLD_TEST_DRF(_P2060, _STATUS, _MSTR_STEREO, _ACTIVE, regStatus))
        {
            // stereo is enabled on the client or the server or both
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

/*
 * Cancel the Watchdog for P2060.
 */
static void
gsyncCancelWatchdog_P2060
(
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;
    OBJGPU *pTempGpu = NULL;

    pThis->watchdogCountDownValue = 0;

    // Cancel callbacks on all gpus
    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId == NV0000_CTRL_GPU_INVALID_ID)
            continue;

        pTempGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);
        NV_ASSERT(pTempGpu);

        NV_PRINTF(LEVEL_INFO, "P2060[%d] extdevCancelWatchdog.\n", iface);

        extdevCancelWatchdog(pTempGpu, (PDACEXTERNALDEVICE)pThis);
    }

    return;
}

/*
 * Enable FrameLock Interrupts for P2060.
 */
static NV_STATUS
gsyncEnableFramelockInterrupt_P2060
(
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU8  regCtrl3;
    NvU32 iface;
    NV_STATUS status = NV_OK;

    // Turn ON the interrupts
    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);
            NV_ASSERT(pTmpGpu);

            if (gsyncReadMaster_P2060(pTmpGpu, pThis) || gsyncReadSlaves_P2060(pTmpGpu, pThis))
            {
                status = readregu008_extdeviceTargeted(pTmpGpu,
                         (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3,  &regCtrl3);

                if (gsyncIsStereoEnabled_p2060(pTmpGpu, pExtDev))
                {
                      regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_STEREO_CHG);
                }
                regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_SYNC_CHG);

                status |= writeregu008_extdeviceTargeted(pTmpGpu,
                          (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, regCtrl3);

                NV_PRINTF(LEVEL_INFO, "P2060[%d] enabled interrupt\n", iface);
            }
        }
    }
    return status;
}

/*
 * Disable FrameLock Interrupts for P2060.
 */
static NV_STATUS
gsyncDisableFrameLockInterrupt_P2060
(
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU8  regCtrl3;
    NvU32 iface;
    NV_STATUS status = NV_OK;

    // Turn Off the interrupts
    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

            NV_ASSERT(pTmpGpu);

            status = readregu008_extdeviceTargeted(pTmpGpu,
                       (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, &regCtrl3);

            regCtrl3 &= ~DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_STEREO_CHG);
            regCtrl3 &= ~DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_SYNC_CHG);

            status |= writeregu008_extdeviceTargeted(pTmpGpu,
                      (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, regCtrl3);

            NV_PRINTF(LEVEL_INFO, "P2060[%d] disabled interrupt\n", iface);
        }
    }

    return status;
}

/*
 * Enable Non-FrameLock Interrupts for P2060.
 */
static NV_STATUS
gsyncEnableNonFramelockInterrupt_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU8  regCtrl3 = 0x00;
    NV_STATUS rmStatus = NV_OK;

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, &regCtrl3);

    // Enable Non-Framelock interrupts on given gsync attached gpu.
    regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_HS_CHG);
    regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_RJ45_CHG);

    rmStatus = writeregu008_extdeviceTargeted(pGpu,(PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, regCtrl3);

    return rmStatus;
}

/*
 * Disable Non-FrameLock Interrupts for P2060.
 */
static NV_STATUS
gsyncDisableNonFramelockInterrupt_P2060
(
    OBJGPU            *pGpu,
    PDACEXTERNALDEVICE pExtDev
)
{
    PDACP2060EXTERNALDEVICE pThis = (PDACP2060EXTERNALDEVICE) pExtDev;
    NvU8  regCtrl3 = 0x00;
    NV_STATUS rmStatus = NV_OK;

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, &regCtrl3);

    // Disable Non-Framelock interrupts on given gsync attached gpu.
    regCtrl3 &= ~DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_HS_CHG);
    regCtrl3 &= ~DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_RJ45_CHG);

    rmStatus = writeregu008_extdeviceTargeted(pGpu,(PDACEXTERNALDEVICE)pThis, NV_P2060_CONTROL3, regCtrl3);

    return rmStatus;
}

static void
gsyncResetMosaicData_P2060
(
    NvU32 mosaicGroup,
    PDACP2060EXTERNALDEVICE pThis
)
{
   NvU8 i;

   if (!pThis)
   {
        return;
   }

   pThis->MosaicGroup[mosaicGroup].gpuTimingSource = NV0000_CTRL_GPU_INVALID_ID;

   for (i = 0; i < NV_P2060_MAX_MOSAIC_SLAVES; i++)
   {
      pThis->MosaicGroup[mosaicGroup].gpuTimingSlaves[i] = NV0000_CTRL_GPU_INVALID_ID;
   }

   pThis->MosaicGroup[mosaicGroup].slaveGpuCount = 0;
   pThis->MosaicGroup[mosaicGroup].enabledMosaic = NV_FALSE;

}

/*
 * Enable/Disable SwapRdy Connection For GPU
 */
static NV_STATUS
gsyncUpdateSwapRdyConnectionForGpu_P2060
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis,
    NvBool bEnable
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU8 ctrl2 = 0x00;

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                             NV_P2060_CONTROL2, &ctrl2);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    ctrl2 = FLD_SET_DRF_NUM(_P2060, _CONTROL2, _SWAP_READY, (NvU8)bEnable, ctrl2);

    rmStatus = writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                             NV_P2060_CONTROL2, ctrl2);

    return rmStatus;
}

/*
 * returns true if there is a framelock master on this P2060.
 * otherwise returns false.
 */
static NvBool
gsyncIsFrameLockMaster_P2060
(
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);

            NV_ASSERT(pTmpGpu);

            if (gsyncReadMaster_P2060(pTmpGpu, pThis))
            {
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

/*
 * returns NV_TRUE if this gsync device is framelocked.
 */
static NvBool
gsyncIsFrameLocked_P2060
(
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            OBJGPU *pTmpGpu = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);
            NV_ASSERT(pTmpGpu);

            if (gsyncReadMaster_P2060(pTmpGpu, pThis) ||
                gsyncReadSlaves_P2060(pTmpGpu, pThis))
            {
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

/*
 * returns NV_TRUE if this gsync device is only framelock master.
 */
static NvBool
gsyncIsOnlyFrameLockMaster_P2060
(
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU32 iface, numHeads, head;
    KernelDisplay  *pKernelDisplay;
    OBJGPU  *pGpu;
    NvBool bIsMaster = NV_FALSE;

    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId != NV0000_CTRL_GPU_INVALID_ID)
        {
            pGpu  = gpumgrGetGpuFromId(pThis->Iface[iface].GpuInfo.gpuId);
            NV_ASSERT_OR_RETURN(pGpu, NV_FALSE);

            pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
            numHeads = kdispGetNumHeads(pKernelDisplay);

            for (head = 0; head < numHeads; head++)
            {
                if (pThis->Iface[iface].Sync.Master[head])
                    bIsMaster = NV_TRUE;

                if (pThis->Iface[iface].Sync.Slaved[head] ||
                    pThis->Iface[iface].Sync.LocalSlave[head])
                {
                    return NV_FALSE;
                }
            }
        }
    }

    return bIsMaster;
}

/*
 * return NV_TRUE if HW is OK with board as Framelock Master.
 */
static NvBool
gsyncIsP2060MasterBoard
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    NvU8 ctrl;
    NvBool bIsMasterBoard;
    NV_STATUS rmStatus;

    if (!pGpu || !GpuIsP2060Connected(pGpu, pThis))
    {
        return NV_FALSE;
    }

    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis, (NvU8)NV_P2060_CONTROL, &ctrl);
    if (NV_OK != rmStatus)
    {
        return NV_FALSE;
    }

    // Check HW opinion on Mastership of board.
    bIsMasterBoard = FLD_TEST_DRF(_P2060, _CONTROL, _I_AM, _MASTER, (NvU32)ctrl);

    return bIsMasterBoard;
}
/*
 * return NV_TRUE if pGpu is connected to Master + TS GPU via SLI bridge
 */
static NvBool
GpuIsConnectedToMasterViaBridge
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{
    OBJGPU *pOtherGpu = NULL;
    NvU32 gpuMask, gpuIndex, tempIface;
    NvU32 drOut, drIn;

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    gpuIndex = 0;
    while ((pOtherGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        if ((pGpu == pOtherGpu) || gpumgrGetGpuLockAndDrPorts(pGpu, pOtherGpu, &drOut, &drIn) != NV_OK)
        {
            continue;
        }

        if (GetP2060GpuLocation(pOtherGpu, pThis, &tempIface) == NV_OK)
        {
            if (gsyncIsP2060MasterBoard(pOtherGpu, pThis) && GpuIsP2060Master(pOtherGpu, pThis))
            {
                // pGpu is connected to pOtherGpu via SLI bridge.
                // Both GPUs are connected to same P2060.
                return NV_TRUE;
            }
        }
    }
    return NV_FALSE;
}

//
// gsyncFrameCountTimerService_P2060()
//
// frame count timer callback service.
// this function will read the actual gsync and gpu frame count value
// and adjust the cached difference between them if required.
//
// this function is added to prevent any deviation of cached difference
// between gpu and gsync hw frame count values from the actual.
// As all the heads are framelocked, it is expected that cached
// framecount value to be same on the master as well as slave
// system. But during experiment, it is found that reading the hw gsync and
// gpu frame count values immediately after the test signal is sent/received
// may lead to inconsistent cached difference. Therefore the difference is
// reverified after FRAME_COUNT_TIMER_INTERVAL period.
//
static
NV_STATUS gsyncFrameCountTimerService_P2060
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    PDACP2060EXTERNALDEVICE pThis = NULL;
    NV_STATUS status;
    OBJGSYNC *pGsync = NULL;

    pGsync = gsyncmgrGetGsync(pGpu);

    NV_ASSERT_OR_RETURN((pGsync && pGsync->pExtDev), NV_ERR_INVALID_DEVICE);

    pThis = (PDACP2060EXTERNALDEVICE)pGsync->pExtDev;

    // disable the timer callback
    tmrEventCancel(pTmr, pThis->FrameCountData.pTimerEvents[gpuGetInstance(pGpu)]);

    //
    // read the gsync and gpu frame count values.Cache the difference between them.
    //
    status = gsyncUpdateFrameCount_P2060(pThis, pGpu);

    return status;
}
/*
 * Reset the FrameCount Data structure.
 */
//
// gsyncResetFrameCountData_P2060()
//
// this function resets the FrameCountDate structure.
//
static NV_STATUS
gsyncResetFrameCountData_P2060
(
    OBJGPU *pGpu,
    PDACP2060EXTERNALDEVICE pThis
)
{

    NvU8 regCtrl3;
    NV_STATUS rmStatus;

    if (!pThis)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pThis->FrameCountData.totalFrameCount               = 0;
    pThis->FrameCountData.currentFrameCount             = 0;
    pThis->FrameCountData.initialDifference             = 0;
    pThis->FrameCountData.numberOfRollbacks             = 0;
    pThis->FrameCountData.previousFrameCount            = 0;
    pThis->FrameCountData.lastFrameCounterQueryTime     = 0;
    pThis->FrameCountData.bReCheck                      = 0;
    pThis->FrameCountData.vActive                       = 0;
    pThis->FrameCountData.isFrmCmpMatchIntMasterEnabled = NV_FALSE;
    pThis->FrameCountData.enableFrmCmpMatchIntSlave     = NV_FALSE;
    pThis->FrameCountData.head                          = NV_P2060_MAX_HEADS_PER_GPU;
    pThis->FrameCountData.iface                         = NV_P2060_MAX_IFACES_PER_GSYNC;

    // disable frame count match interrupt
    rmStatus = readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                             NV_P2060_CONTROL3, &regCtrl3);
    regCtrl3 &= ~DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_FRAME_MATCH);
    rmStatus |= writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                               NV_P2060_CONTROL3, regCtrl3);
    return rmStatus;
}

//
// gsyncUpdateFrameCount_P2060()
//
// For all heads in a framelocked state gpu framecount is equal.This also
// implies for gsync frame count.i.e. gsync frame count = (gpu frame count + difference)
// Therefore to reduce the i2c reads to access gsync frame count,
// (gpu frame count + difference) can be returned. This is done by caching the
// difference between the gpu and gsync framecount.
//
// FrameCountTimerService (1 second callback) is also enabled here to verify
// the cache difference.
//
static NV_STATUS
gsyncUpdateFrameCount_P2060
(
    PDACP2060EXTERNALDEVICE pThis,
    OBJGPU *pGpu
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    RM_API *pRmApi;
    NvU32 hClient;
    NvU32 hSubdevice;
    NvU8  FrameCountLow;
    NvU8  FrameCountMid;
    NvU8  FrameCountHigh;
    NvU8  regCtrl3;
    NvU32 rawGsyncFrameCount;
    NvU32 iface;
    NvU32 head = 0;
    NvU32 numHeads;
    NvU32 modGsyncFrameCount;
    NvU32 lineCount;
    NvU32 frameCount;
    RMTIMEOUT timeout;
    NvU32 safeRegionUpperLimit;
    NvU32 safeRegionLowerLimit;
    NV_STATUS rmStatus = NV_OK;
    NV2080_CTRL_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES_PARAMS ctrlParams = {0};

    numHeads = kdispGetNumHeads(pKernelDisplay);

    // get any framelocked head
    for (iface = 0; iface < NV_P2060_MAX_IFACES_PER_GSYNC; iface++)
    {
        if (pThis->Iface[iface].GpuInfo.gpuId == NV0000_CTRL_GPU_INVALID_ID)
        {
            continue;
        }

        for (head = 0; head < numHeads; head++)
        {
            if (pThis->Iface[iface].Sync.Master[head] ||
                pThis->Iface[iface].Sync.Slaved[head] ||
                pThis->Iface[iface].Sync.LocalSlave[head])
            {
                // Update pThis->FrameCountData with iface and head
                pThis->FrameCountData.iface = iface;
                pThis->FrameCountData.head  = head;

                // Get out of for loop
                iface = NV_P2060_MAX_IFACES_PER_GSYNC;
                break;
            }
        }
    }

    if (head == numHeads)
    {
        return NV_ERR_GENERIC;
    }

    pGpu = gpumgrGetGpuFromId(pThis->Iface[pThis->FrameCountData.iface].GpuInfo.gpuId);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_INVALID_DEVICE);

    pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);
    hClient    = pGpu->hInternalClient;
    hSubdevice = pGpu->hInternalSubdevice;

    // Re-fetch pDisp as pGpu might have changed.
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_ASSERT_OR_RETURN(pKernelDisplay != NULL, NV_ERR_INVALID_DEVICE);
    NV_ASSERT_OR_RETURN(head < kdispGetNumHeads(pKernelDisplay), NV_ERR_INVALID_DEVICE);

    ctrlParams.headIdx = head;

    rmStatus = pRmApi->Control(pRmApi, hClient, hSubdevice,
                               NV2080_CTRL_CMD_INTERNAL_GSYNC_GET_VERTICAL_ACTIVE_LINES,
                               &ctrlParams, sizeof(ctrlParams));

    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    pThis->FrameCountData.vActive = ctrlParams.vActiveLines;

    //
    // To read Gpu framecount, line count should be in between 5-70% of VVisible.
    //
    safeRegionUpperLimit = (pThis->FrameCountData.vActive * 7) / 10;
    safeRegionLowerLimit = pThis->FrameCountData.vActive / 20;

    // Read the GPU frame count and line count
    rmStatus = kdispReadRgLineCountAndFrameCount_HAL(pGpu, pKernelDisplay,
                   pThis->FrameCountData.head, &lineCount, &frameCount);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to read RG_DPCA.\n");
        return rmStatus;
    }

    //
    // Wait for a safe region i.e. 5-70 percent of the VActive. Then read the
    // gsync framecount. This is done to ensure that both gsync and gpu
    // registers are read in the safe zone otherwise there will be -/+ 1
    // frame inconsistency ( if read during the transition from frame N to
    // frame N + 1 i.e linecount > vActive)
    //
    if ((lineCount >= safeRegionUpperLimit) || (lineCount <= safeRegionLowerLimit))
    {
        //
        // timeout of one frameTime(in nano seconds), to avoid going into an infinite
        // loop in case linecount is stuck to some value.
        //
        gpuSetTimeout(pGpu, (pThis->FrameCountData.frameTime * 1000), &timeout, 0);

        // Read the linecount until we are in the safe region i.e taken as 5%-70% of VActive.
        while (((lineCount >= safeRegionUpperLimit) || (lineCount <= safeRegionLowerLimit)) &&
                (gpuCheckTimeout(pGpu, &timeout) != NV_ERR_TIMEOUT))
        {
            rmStatus = kdispReadRgLineCountAndFrameCount_HAL(pGpu, pKernelDisplay,
                           pThis->FrameCountData.head, &lineCount, &frameCount);
            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to read RG_DPCA.\n");
                return rmStatus;
            }
        }

        if ((lineCount >= safeRegionUpperLimit) || (lineCount <= safeRegionLowerLimit))
        {
            return NV_ERR_TIMEOUT;
        }
    }

    rmStatus |= readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis, (NvU8)NV_P2060_FRAMECNTR_LOW,  &FrameCountLow);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis, (NvU8)NV_P2060_FRAMECNTR_MID,  &FrameCountMid);
    rmStatus |= readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE) pThis, (NvU8)NV_P2060_FRAMECNTR_HIGH, &FrameCountHigh);

    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

     rawGsyncFrameCount = (
                          ((((NvU32)FrameCountHigh)    & DRF_MASK(NV_P2060_FRAMECNTR_HIGH_VAL))<< 16 ) |
                          ((((NvU32)FrameCountMid)     & DRF_MASK(NV_P2060_FRAMECNTR_MID_VAL)) << 8 )  |
                          ((((NvU32)FrameCountLow)     & DRF_MASK(NV_P2060_FRAMECNTR_LOW_VAL))));

    pThis->FrameCountData.currentFrameCount = frameCount;

    //
    // Gsync frame count is 24 bit register whereas Gpu frame count register is 16 bit.
    // Therefore number of rollovers of Gpu frame count register is required.
    // Else gsync frame count and (gpu frame count + difference) can be off by (2^16*N).
    // where maximum value of N can be 256. << gsync frame count 2^24 = 256* 2^16.
    //
    pThis->FrameCountData.numberOfRollbacks  = gsyncGetNumberOfGpuFrameCountRollbacks_P2060(rawGsyncFrameCount, 0, 256);
    modGsyncFrameCount                       = rawGsyncFrameCount - (pThis->FrameCountData.numberOfRollbacks * (NV_P2060_MAX_GPU_FRAME_COUNT + 1));
    pThis->FrameCountData.initialDifference  = modGsyncFrameCount - pThis->FrameCountData.currentFrameCount;
    pThis->FrameCountData.previousFrameCount = 0;

    pThis->FrameCountData.totalFrameCount  =  pThis->FrameCountData.currentFrameCount +
                                              pThis->FrameCountData.initialDifference +
                                              pThis->FrameCountData.numberOfRollbacks * (NV_P2060_MAX_GPU_FRAME_COUNT + 1);

    if (pThis->FrameCountData.enableFrmCmpMatchIntSlave)
    {
        pThis->FrameCountData.enableFrmCmpMatchIntSlave = NV_FALSE;

        // enable frame count match interrupt
        rmStatus |= readregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                  NV_P2060_CONTROL3,  &regCtrl3);

        if (rmStatus == NV_OK)
        {
            regCtrl3 |= DRF_DEF(_P2060, _CONTROL3, _INTERRUPT, _ON_FRAME_MATCH);
            rmStatus = writeregu008_extdeviceTargeted(pGpu, (PDACEXTERNALDEVICE)pThis,
                                                      NV_P2060_CONTROL3, regCtrl3);
        }
    }

    //
    // Schedule 1 second timer callback, to verify initialDifference.
    //
    if (pThis->FrameCountData.bReCheck)
    {

        NV_STATUS status = NV_OK;
        OBJTMR *pTmr  = GPU_GET_TIMER(pGpu);

        status = tmrEventScheduleRel(pTmr,
                                     pThis->FrameCountData.pTimerEvents[gpuGetInstance(pGpu)],
                                     (NV_P2060_FRAME_COUNT_TIMER_INTERVAL / 5));

        if (status == NV_OK)
        {
            pThis->FrameCountData.bReCheck = 0;
        }

    }
    return rmStatus;
}

//
// gsyncGetNumberOfGpuFrameCountRollbacks_P2060
//
// Get N where N is the maximum value for gsync framecount > N*(Gpu frame count)
//
static NvU32
gsyncGetNumberOfGpuFrameCountRollbacks_P2060
(
    NvU32 FrameCount,
    NvU32 low,
    NvU32 high
)
{
    NvU32 mid = (low + high) / 2;

    if (FrameCount >= (high * NV_P2060_MAX_GPU_FRAME_COUNT))
    {
        return high;
    }
    else if ((FrameCount >= (mid * NV_P2060_MAX_GPU_FRAME_COUNT)) &&
             (FrameCount < ((mid+1) * NV_P2060_MAX_GPU_FRAME_COUNT)))
    {
        return mid;
    }
    else if ((FrameCount > (NV_P2060_MAX_GPU_FRAME_COUNT * low)) && (FrameCount < (mid * NV_P2060_MAX_GPU_FRAME_COUNT)))
    {
        return gsyncGetNumberOfGpuFrameCountRollbacks_P2060(FrameCount, low, mid);
    }
    else if ((FrameCount > (NV_P2060_MAX_GPU_FRAME_COUNT * mid)) && (FrameCount < (high * NV_P2060_MAX_GPU_FRAME_COUNT)))
    {
        return gsyncGetNumberOfGpuFrameCountRollbacks_P2060(FrameCount, mid+1, high);
    }
    else
    {
        return 0;
    }
}

// Return NV_TRUE if the current Qsync revision supports large sync skew
NvBool
gsyncSupportsLargeSyncSkew_P2060
(
    DACEXTERNALDEVICE *pExtdev
)
{
    if (pExtdev->deviceId == DAC_EXTERNAL_DEVICE_P2061)
    {
        // All p2061 revisions support sync skew > 1.
        return NV_TRUE;
    }
    else
    {
        //
        // P2060 FPGA (revision < 3) does not support SyncSkew more than 1 us(HW limitation).
        // If set to value more than 1 us, we observe screen flashing. Refer bug 1058215
        //
        NV_ASSERT(pExtdev->deviceId == DAC_EXTERNAL_DEVICE_P2060);
        return (pExtdev->deviceRev >= DAC_EXTERNAL_DEVICE_REV_3);
    }
}

// Return NV_TRUE if the current Qsync revision needs the Swapbarrier WAR on master
static NvBool
needsMasterBarrierWar
(
    PDACEXTERNALDEVICE pExtdev
)
{
    if (pExtdev->deviceId == DAC_EXTERNAL_DEVICE_P2061)
    {
        // All p2061 revisions do not need the WAR.
        return NV_FALSE;
    }
    else
    {
        //
        // P2060 Fpga (revision <= 5) needs to have the swapbarrier set on framelock masters
        // to drive (pull up) the swap_rdy line of the whole framelock setup.
        // This is a behaviour with unwanted side effects which needs drivers wars
        // for certain configs.
        //
        NV_ASSERT(pExtdev->deviceId == DAC_EXTERNAL_DEVICE_P2060);
        return (pExtdev->deviceRev <= DAC_EXTERNAL_DEVICE_REV_5);
    }
}

// Return NV_TRUE if the Qsync revision is not compatible with GPU
static NvBool
isFirmwareRevMismatch
(
    OBJGPU *pGpu,
    OBJGSYNC *pGsync,
    DAC_EXTERNAL_DEVICE_REVS currentRev
)
{
    NvU32 i;

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_TRUE);
    NV_ASSERT_OR_RETURN(pGsync != NULL, NV_TRUE);

    // Check each GPU on this Gsync board for a firmware mismatch
    for (i = 0; i < pGsync->gpuCount; i++)
    {
        OBJGPU *pOtherGpu = gpumgrGetGpuFromId(pGsync->gpus[i].gpuId);
        NV_ASSERT_OR_RETURN(pOtherGpu != NULL, NV_TRUE);

        if (gsyncmgrIsFirmwareGPUMismatch(pOtherGpu, pGsync))
        {
            return NV_TRUE;
        }
    }

    if (IsMAXWELL(pGpu))
    {
        return (currentRev < NV_P2060_MIN_REV);
    }
    else
    {
        return NV_FALSE;
    }
}

/*
 * Nvlink and QSync can both transmit inter-GPU Display sync signals.
 * Contention in these signals is observed on some boards, if both Nvlink and
 * QSync are present between the boards.
 *
 * Returns TRUE if contention in transmission of sync signals possible on the
 * given GPU board if both mediums (QSync and Nvlink) are present between GPUs
 */

static NvBool
isBoardWithNvlinkQsyncContention
(
    OBJGPU *pGpu
)
{
    NvU16 devIds[] = {
        0x2230,     // Nvidia RTX A6000 (PG133 SKU 500)
        0x2231,     // Nvidia RTX A5000 (PG132 SKU 500)
        0x2233      // Nvidia RTX A5500 (PG132 SKU 520)
    };

    NvU16 thisDevId = (NvU16)(((pGpu->idInfo.PCIDeviceID) >> 16) & 0x0000FFFF);
    NvU32 i;

    for (i=0; i < (NV_ARRAY_ELEMENTS(devIds)); i++)
    {
        if (thisDevId == devIds[i])
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

// Return NV_TRUE if the current Qsync revision supports sync multiply/divide
static NvBool
supportsMulDiv
(
    DACEXTERNALDEVICE *pExtDev
)
{
    // Supported only for 2061 boards with >= 2.4
    if (pExtDev->deviceId == DAC_EXTERNAL_DEVICE_P2061)
    {
        if ((pExtDev->deviceRev >= DAC_EXTERNAL_DEVICE_REV_3) ||
            ((pExtDev->deviceRev == DAC_EXTERNAL_DEVICE_REV_2) &&
             (pExtDev->deviceExRev >= 4)))
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

NV_STATUS
gsyncGetMulDiv_P2060
(
    OBJGPU *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *pMulDivSettings
)
{
    DACP2060EXTERNALDEVICE *pThis = (DACP2060EXTERNALDEVICE *)pExtDev;
    NvU8 reg;

    NV_ASSERT_OR_RETURN(pMulDivSettings != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, supportsMulDiv(pExtDev), NV_ERR_NOT_SUPPORTED);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        readregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_MULTIPLIER_DIVIDER, &reg));

    pMulDivSettings->multiplyDivideValue =
        DRF_VAL(_P2060, _MULTIPLIER_DIVIDER, _VALUE_MINUS_ONE, reg) + 1;
    pMulDivSettings->multiplyDivideMode =
        FLD_TEST_DRF(_P2060, _MULTIPLIER_DIVIDER, _MODE, _DIVIDE, reg) ?
            NV30F1_CTRL_GSYNC_SET_CONTROL_MULTIPLY_DIVIDE_MODE_DIVIDE :
            NV30F1_CTRL_GSYNC_SET_CONTROL_MULTIPLY_DIVIDE_MODE_MULTIPLY;

    // Cache this for debugging
    portMemCopy(&pThis->mulDivSettings, sizeof(pThis->mulDivSettings),
                pMulDivSettings, sizeof(*pMulDivSettings));

    return NV_OK;
}

NV_STATUS
gsyncSetMulDiv_P2060
(
    OBJGPU *pGpu,
    DACEXTERNALDEVICE *pExtDev,
    NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *pMulDivSettings
)
{
    DACP2060EXTERNALDEVICE *pThis = (DACP2060EXTERNALDEVICE *)pExtDev;
    NvU8 reg;

    NV_ASSERT_OR_RETURN(pMulDivSettings != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, supportsMulDiv(pExtDev), NV_ERR_NOT_SUPPORTED);
    pGpu = GetP2060MasterableGpu(pGpu, (DACP2060EXTERNALDEVICE *)pExtDev);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_GENERIC);

    //
    // Assume that there are no other fields inside NV_P2060_MULTIPLIER_DIVIDER
    // to necessitate a read-modify-write
    //
    reg = 0;

    switch (pMulDivSettings->multiplyDivideMode)
    {
        case NV30F1_CTRL_GSYNC_SET_CONTROL_MULTIPLY_DIVIDE_MODE_MULTIPLY:
            reg = FLD_SET_DRF(_P2060, _MULTIPLIER_DIVIDER, _MODE, _MULTIPLY, reg);
            break;
        case NV30F1_CTRL_GSYNC_SET_CONTROL_MULTIPLY_DIVIDE_MODE_DIVIDE:
            reg = FLD_SET_DRF(_P2060, _MULTIPLIER_DIVIDER, _MODE, _DIVIDE, reg);
            break;
        default:
            return NV_ERR_INVALID_PARAMETER;
    }

    // The register is a 3-bit value ranging from 0-7 representing the integers from 1-8, so check the input param
    if ((pMulDivSettings->multiplyDivideValue < 1) ||
        (pMulDivSettings->multiplyDivideValue > (NV_P2060_MULTIPLIER_DIVIDER_VALUE_MINUS_ONE_MAX + 1)))
        return NV_ERR_INVALID_PARAMETER;
    // Subtract 1 while packing the register
    reg = FLD_SET_DRF_NUM(_P2060, _MULTIPLIER_DIVIDER, _VALUE_MINUS_ONE,
                          pMulDivSettings->multiplyDivideValue - 1, reg);

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, writeregu008_extdeviceTargeted(pGpu, pExtDev, NV_P2060_MULTIPLIER_DIVIDER, reg));

    // Cache this for debugging
    portMemCopy(&pThis->mulDivSettings, sizeof(pThis->mulDivSettings),
                pMulDivSettings, sizeof(*pMulDivSettings));

    return NV_OK;
}
