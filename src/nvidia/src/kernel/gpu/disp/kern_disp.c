/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "resserv/resserv.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "os/os.h"

#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"
#include "gpu/disp/head/kernel_head.h"
#include "gpu/disp/disp_objs.h"
#include "gpu_mgr/gpu_mgr.h"
#include "objtmr.h"
#include "core/locks.h"
#include "ctrl/ctrl402c.h"
#include "platform/acpi_common.h"
#include "nvrm_registry.h"

#include "kernel/gpu/intr/engine_idx.h"

#include "ctrl/ctrl2080.h"

#include "class/cl5070.h"
#include "class/cl917a.h"
#include "class/cl917b.h"
#include "class/cl917e.h"
#include "class/cl927c.h"
#include "class/cl947d.h"
#include "class/cl957d.h"
#include "class/cl977d.h"
#include "class/cl987d.h"
#include "class/clc37a.h"
#include "class/clc37b.h"
#include "class/clc37d.h"
#include "class/clc37e.h"
#include "class/clc57a.h"
#include "class/clc57b.h"
#include "class/clc57d.h"
#include "class/clc57e.h"
#include "class/clc67a.h"
#include "class/clc67b.h"
#include "class/clc67d.h"
#include "class/clc67e.h"
#include "class/clc77f.h" //NVC77F_ANY_CHANNEL_DMA

#include "class/clc77d.h"

#include "gpu/disp/rg_line_callback/rg_line_callback.h"

#include "rmapi/rmapi_utils.h"
#include "class/cl0073.h"

NV_STATUS
kdispConstructEngine_IMPL(OBJGPU        *pGpu,
                          KernelDisplay *pKernelDisplay,
                          ENGDESCRIPTOR  engDesc)
{
    NV_STATUS status;

    //
    // NOTE: DO NOT call IpVersion _HAL functions in ConstructEngine.
    // IP version based _HAL functions can only be used starting StatePreInit.
    // Long-term: RM offload initialization will be moved earlier so KernelDisplay
    // has the ability to use IP version HAL functions even in construct phase.
    //

    //
    // Sanity check: the only time KERNEL_DISPLAY module should be enabled
    // while DISP is disabled is on KERNEL_ONLY build.
    //
    NV_ASSERT(IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu) || RMCFG_MODULE_DISP);

    //
    // We also need to check if we are in certain configurations which can't
    // even attempt a control call to DISP.
    //
    if (pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_IS_MISSING))
        return NV_ERR_NOT_SUPPORTED;

    // Create children
    pKernelDisplay->pInst = NULL;
    status = kdispConstructInstMem_HAL(pKernelDisplay);
    if (status != NV_OK)
    {
        return status;
    }

    status = kdispConstructKhead(pKernelDisplay);

    // We defer checking whether DISP has been disabled some other way until
    // StateInit, when we can do a control call.

    return status;
}

void
kdispDestruct_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    // Destroy children
    kdispDestructInstMem_HAL(pKernelDisplay);
    kdispDestructKhead(pKernelDisplay);
}

/*! Constructor for DisplayInstanceMemory */
NV_STATUS
kdispConstructInstMem_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    NV_STATUS status;
    DisplayInstanceMemory *pInst;

    status = objCreate(&pInst, pKernelDisplay, DisplayInstanceMemory);
    if (status != NV_OK)
    {
        return status;
    }

    pKernelDisplay->pInst = pInst;
    return NV_OK;
}

/*! Destructor for DisplayInstanceMemory */
void
kdispDestructInstMem_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    objDelete(pKernelDisplay->pInst);
    pKernelDisplay->pInst = NULL;
}

/*! Constructor for Kernel head */
NV_STATUS
kdispConstructKhead_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    NV_STATUS   status;
    KernelHead *pKernelHead;
    NvU8        headIdx;

    for (headIdx = 0; headIdx < OBJ_MAX_HEADS; headIdx++)
    {
        status = objCreate(&pKernelHead, pKernelDisplay, KernelHead);
        if (status != NV_OK)
        {
            return status;
        }

        pKernelDisplay->pKernelHead[headIdx] = pKernelHead;
        pKernelDisplay->pKernelHead[headIdx]->PublicId = headIdx;
    }
    return NV_OK;
}

/*! Destructor for Kernel head */
void
kdispDestructKhead_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    NvU8      headIdx;

    for (headIdx = 0; headIdx < OBJ_MAX_HEADS; headIdx++)
    {
        objDelete(pKernelDisplay->pKernelHead[headIdx]);
        pKernelDisplay->pKernelHead[headIdx] = NULL;
    }
}

NV_STATUS
kdispAllocateCommonHandle_IMPL
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay
)
{
    NV_STATUS rmStatus;
    NvHandle  hClient;
    NvHandle  hDevice;
    NvHandle  hSubdevice;
    NvHandle  hSubscription = NV01_NULL_OBJECT;
    RM_API   *pRmApi        = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    rmStatus = rmapiutilAllocClientAndDeviceHandles(pRmApi, pGpu, &hClient,
                                                    &hDevice, &hSubdevice);
    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, NV_FALSE);

    rmStatus = pRmApi->AllocWithSecInfo(pRmApi, hClient, hDevice, &hSubscription,
                                        NV04_DISPLAY_COMMON, NULL, 0, RMAPI_ALLOC_FLAGS_NONE,
                                        NULL, &pRmApi->defaultSecInfo);
    NV_ASSERT_OR_RETURN(rmStatus == NV_OK, NV_FALSE);

    pKernelDisplay->hInternalClient = hClient;
    pKernelDisplay->hInternalDevice = hDevice;
    pKernelDisplay->hInternalSubdevice = hSubdevice;
    pKernelDisplay->hDispCommonHandle = hSubscription;

    return NV_OK;
}

void
kdispDestroyCommonHandle_IMPL
(
    KernelDisplay *pKernelDisplay
)
{
    NV_STATUS rmStatus;
    RM_API   *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    rmStatus = pRmApi->FreeWithSecInfo(pRmApi, pKernelDisplay->hInternalClient,
                                        pKernelDisplay->hDispCommonHandle,
                                        RMAPI_ALLOC_FLAGS_NONE, &pRmApi->defaultSecInfo);
    NV_ASSERT(rmStatus == NV_OK);

    rmapiutilFreeClientAndDeviceHandles(pRmApi, &pKernelDisplay->hInternalClient,
                                        &pKernelDisplay->hInternalDevice,
                                        &pKernelDisplay->hInternalSubdevice);

    pKernelDisplay->hInternalClient = 0;
    pKernelDisplay->hInternalDevice = 0;
    pKernelDisplay->hInternalSubdevice = 0;
    pKernelDisplay->hDispCommonHandle = 0;
}

NV_STATUS
kdispStatePreInitLocked_IMPL(OBJGPU        *pGpu,
                             KernelDisplay *pKernelDisplay)
{
    NV_STATUS status;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32   hClient = pGpu->hInternalClient;
    NvU32   hSubdevice = pGpu->hInternalSubdevice;
    NV2080_CTRL_INTERNAL_DISPLAY_GET_IP_VERSION_PARAMS ctrlParams;

    if (!gpuFuseSupportsDisplay_HAL(pGpu))
       return NV_ERR_NOT_SUPPORTED;

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_IP_VERSION,
                             &ctrlParams, sizeof(ctrlParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Failed to read display IP version (FUSE disabled), status=0x%x\n",
                  status);
        return status;
    }

    // NOTE: KernelDisplay IpVersion _HAL functions can only be called after this point.
    status = gpuInitDispIpHal(pGpu, ctrlParams.ipVersion);

    kdispInitRegistryOverrides_HAL(pGpu, pKernelDisplay);

    kdispAllocateCommonHandle(pGpu, pKernelDisplay);

    return status;
}

NV_STATUS
kdispInitBrightcStateLoad_IMPL(OBJGPU *pGpu,
                               KernelDisplay *pKernelDisplay)
{
    NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS *pBrightcInfo = NULL;
    NvU32 status = NV_ERR_NOT_SUPPORTED;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    pBrightcInfo = portMemAllocNonPaged(sizeof(NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS));
    if (pBrightcInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate memory for pBrightcInfo\n");
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pBrightcInfo, 0, sizeof(*pBrightcInfo));

    pBrightcInfo->status = status;
    if ((pKernelDisplay != NULL) && (pKernelDisplay->pStaticInfo->internalDispActiveMask != 0))
    {
        // Fill in the Backlight Method Data.
        pBrightcInfo->backLightDataSize = sizeof(pBrightcInfo->backLightData);
        status = osCallACPI_DSM(pGpu, ACPI_DSM_FUNCTION_CURRENT, NV_ACPI_GENERIC_FUNC_GETBACKLIGHT,
                                (NvU32 *)(pBrightcInfo->backLightData),
                                &pBrightcInfo->backLightDataSize);
        pBrightcInfo->status = status;
    }

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_INTERNAL_INIT_BRIGHTC_STATE_LOAD,
                    pBrightcInfo, sizeof(*pBrightcInfo));

    portMemFree(pBrightcInfo);

    return status;
}

NV_STATUS
kdispSetupAcpiEdid_IMPL
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay
)
{
    NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS *pEdidParams = NULL;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32 status = NV_ERR_GENERIC;
    NvU32 index;

    pEdidParams = portMemAllocNonPaged(sizeof(NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA_PARAMS));
    if (pEdidParams == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate memory for pEdidParams\n");
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pEdidParams, 0, sizeof(*pEdidParams));

    pEdidParams->tableLen = pGpu->acpiMethodData.dodMethodData.acpiIdListLen / sizeof(NvU32);

    for (index = 0; index < pEdidParams->tableLen; index++)
    {
        pEdidParams->edidTable[index].bufferSize = MAX_EDID_SIZE_FROM_SBIOS;
        status = osCallACPI_DDC(pGpu, pGpu->acpiMethodData.dodMethodData.acpiIdList[index],
                                    pEdidParams->edidTable[index].edidBuffer,
                                    &pEdidParams->edidTable[index].bufferSize, NV_TRUE);
        pEdidParams->edidTable[index].acpiId = pGpu->acpiMethodData.dodMethodData.acpiIdList[index];
        pEdidParams->edidTable[index].status = status;
    }

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_INTERNAL_SET_STATIC_EDID_DATA,
                    pEdidParams, sizeof(*pEdidParams));

    portMemFree(pEdidParams);

    return status;
}

void
kdispInitRegistryOverrides_IMPL(OBJGPU        *pGpu,
                                KernelDisplay *pKernelDisplay)
{
    NvU32 data32 = 0;

    if (pKernelDisplay == NULL)
    {
        return;
    }

    if (NV_OK == osReadRegistryDword(pGpu, NV_REG_STR_RM_BUG_2089053_WAR, &data32))
    {
        if (data32 == NV_REG_STR_RM_BUG_2089053_WAR_DISABLE)
        {
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS, NV_FALSE);
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE, NV_FALSE);
        }
        else if (data32 == NV_REG_STR_RM_BUG_2089053_WAR_ENABLE_ALWAYS)
        {
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS, NV_TRUE);
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE, NV_FALSE);
        }
        else if (data32 == NV_REG_STR_RM_BUG_2089053_WAR_ENABLE_ON_HMD_ACTIVE_ONLY)
        {
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE, NV_TRUE);
            pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS, NV_FALSE);
        }
    }
}

NV_STATUS
kdispStateInitLocked_IMPL(OBJGPU        *pGpu,
                          KernelDisplay *pKernelDisplay)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    KernelDisplayStaticInfo *pStaticInfo;

    pStaticInfo = portMemAllocNonPaged(sizeof(KernelDisplayStaticInfo));
    if (pStaticInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate KernelDisplayStaticInfo");
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }
    portMemSet(pStaticInfo, 0, sizeof(*pStaticInfo));

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_DISPLAY_GET_STATIC_INFO,
                        pStaticInfo, sizeof(*pStaticInfo)),
        exit);

    pKernelDisplay->pStaticInfo = pStaticInfo;
    pKernelDisplay->numHeads = pStaticInfo->numHeads;
    pStaticInfo = NULL;

    // Initiate Brightc module state load
    status = kdispInitBrightcStateLoad_HAL(pGpu, pKernelDisplay);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rmapi control call for brightc state load failed\n");
        goto exit;
    }

    // Set up ACPI DDC data in Physical RM for future usage
    status = kdispSetupAcpiEdid_HAL(pGpu, pKernelDisplay);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "rmapi control call for acpi child device init failed\n");
        goto exit;
    }

    if (pKernelDisplay->pInst != NULL)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                instmemStateInitLocked(pGpu, pKernelDisplay->pInst), exit);
    }

    // Initialize any external daughterboards that
    // might be out there.

    pGpu->i2cPortForExtdev = NV402C_CTRL_NUM_I2C_PORTS;

    if (pKernelDisplay->pStaticInfo->i2cPort == NV402C_CTRL_NUM_I2C_PORTS)
    {
        NV_PRINTF(LEVEL_INFO, "Error in getting valid I2Cport for Extdevice or extdevice doesn't exist\n");
    }
    else
    {
        pGpu->i2cPortForExtdev = pKernelDisplay->pStaticInfo->i2cPort;

        if (NV_OK != gpuExtdevConstruct_HAL(pGpu))
        {
            NV_PRINTF(LEVEL_INFO, "gpuExtdevConstruct() failed or not supported\n");
        }
    }

    if (pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_IMP_ENABLE))
    {
        // NOTE: Fills IMP parameters and populate those to disp object in Tegra
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                kdispImportImpData_HAL(pKernelDisplay), exit);
    }

exit:
    portMemFree(pStaticInfo);

    return status;
}

void
kdispStateDestroy_IMPL(OBJGPU *pGpu,
                       KernelDisplay *pKernelDisplay)
{
    if (pKernelDisplay->pInst != NULL)
    {
        instmemStateDestroy(pGpu, pKernelDisplay->pInst);
    }

    portMemFree((void*) pKernelDisplay->pStaticInfo);
    pKernelDisplay->pStaticInfo = NULL;

    kdispDestroyCommonHandle(pKernelDisplay);
}

NV_STATUS
kdispStateLoad_IMPL
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         flags
)
{
    NV_STATUS status = NV_OK;

    if (pKernelDisplay->pInst != NULL)
        status = instmemStateLoad(pGpu, pKernelDisplay->pInst, flags);

    return status;
}

NV_STATUS
kdispStateUnload_IMPL
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32         flags
)
{
    NV_STATUS status = NV_OK;

    if (pKernelDisplay->pInst != NULL)
        status = instmemStateUnload(pGpu, pKernelDisplay->pInst, flags);

    return status;
}

/*! Get and Populate IMP init data for Tegra */
NV_STATUS
kdispImportImpData_IMPL(KernelDisplay *pKernelDisplay)
{
    OBJGPU *pGpu = ENG_GET_GPU(pKernelDisplay);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32   hClient = pGpu->hInternalClient;
    NvU32   hSubdevice = pGpu->hInternalSubdevice;
    NV2080_CTRL_INTERNAL_DISPLAY_SET_IMP_INIT_INFO_PARAMS params;
    NvU32   simulationMode;

    //
    // FPGA has different latency characteristics, and the current code latency
    // models that IMP uses for silicon will not work for FPGA, so keep IMP
    // disabled by default on Tegra FPGA.
    //
    simulationMode = osGetSimulationMode();
    if (simulationMode == NV_SIM_MODE_TEGRA_FPGA)
    {
        pKernelDisplay->setProperty(pKernelDisplay, PDB_PROP_KDISP_IMP_ENABLE, NV_FALSE);
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(osTegraSocGetImpImportData(&params.tegraImpImportData));

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, hClient, hSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_DISPLAY_SET_IMP_INIT_INFO,
                           &params, sizeof(params)));

    return NV_OK;
}

/*! Get internal enum equivalent of the HW class number */
NV_STATUS
kdispGetIntChnClsForHwCls_IMPL
(
    KernelDisplay *pKernelDisplay,
    NvU32          hwClass,
    DISPCHNCLASS  *pDispChnClass
)
{
    // sanity check
    if (pDispChnClass == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    switch (hwClass)
    {
        case NV917A_CURSOR_CHANNEL_PIO:
        case NVC37A_CURSOR_IMM_CHANNEL_PIO:
        case NVC57A_CURSOR_IMM_CHANNEL_PIO:
        case NVC67A_CURSOR_IMM_CHANNEL_PIO:
            *pDispChnClass = dispChnClass_Curs;
            break;

        case NV917B_OVERLAY_IMM_CHANNEL_PIO:
            *pDispChnClass = dispChnClass_Ovim;
            break;

        case NV927C_BASE_CHANNEL_DMA:
            *pDispChnClass = dispChnClass_Base;
            break;

        case NV947D_CORE_CHANNEL_DMA:
        case NV957D_CORE_CHANNEL_DMA:
        case NV977D_CORE_CHANNEL_DMA:
        case NV987D_CORE_CHANNEL_DMA:
        case NVC37D_CORE_CHANNEL_DMA:
        case NVC57D_CORE_CHANNEL_DMA:
        case NVC67D_CORE_CHANNEL_DMA:
        case NVC77D_CORE_CHANNEL_DMA:
            *pDispChnClass = dispChnClass_Core;
            break;

        case NV917E_OVERLAY_CHANNEL_DMA:
            *pDispChnClass = dispChnClass_Ovly;
            break;

        case NVC37B_WINDOW_IMM_CHANNEL_DMA:
        case NVC57B_WINDOW_IMM_CHANNEL_DMA:
        case NVC67B_WINDOW_IMM_CHANNEL_DMA:
            *pDispChnClass = dispChnClass_Winim;
            break;

        case NVC37E_WINDOW_CHANNEL_DMA:
        case NVC57E_WINDOW_CHANNEL_DMA:
        case NVC67E_WINDOW_CHANNEL_DMA:
            *pDispChnClass = dispChnClass_Win;
            break;

        case NVC77F_ANY_CHANNEL_DMA:
            // Assert incase of physical RM, Any channel is kernel only channel.
            NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_INVALID_CHANNEL);
            *pDispChnClass = dispChnClass_Any;
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown channel class %x\n", hwClass);
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

void
kdispNotifyEvent_IMPL
(
    OBJGPU        *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32          notifyIndex,
    void          *pNotifyParams,
    NvU32          notifyParamsSize,
    NvV32          info32,
    NvV16          info16
)
{
    PEVENTNOTIFICATION pEventNotifications;
    NvU32             *pNotifyActions;
    NvU32              disableCmd, singleCmd;
    NvU32              subDeviceInst;
    RS_SHARE_ITERATOR  it = serverutilShareIter(classId(NotifShare));

    // search notifiers with events hooked up for this gpu
    while (serverutilShareIterNext(&it))
    {
        RsShared   *pShared = it.pShared;
        DisplayApi *pDisplayApi;
        INotifier  *pNotifier;
        Device     *pDevice;
        NotifShare *pNotifierShare = dynamicCast(pShared, NotifShare);

        if ((pNotifierShare == NULL) || (pNotifierShare->pNotifier == NULL))
            continue;

        pNotifier = pNotifierShare->pNotifier;
        pDisplayApi = dynamicCast(pNotifier, DisplayApi);

        // Only notify matching GPUs
        if (pDisplayApi == NULL)
            continue;

        pDevice = dynamicCast(RES_GET_REF(pDisplayApi)->pParentRef->pResource, Device);

        if (GPU_RES_GET_GPU(pDevice) != pGpu)
            continue;

        gpuSetThreadBcState(GPU_RES_GET_GPU(pDevice), pDisplayApi->bBcResource);

        disableCmd = NV5070_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
        singleCmd = NV5070_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE;

        // get notify actions list
        subDeviceInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        pNotifyActions = pDisplayApi->pNotifyActions[subDeviceInst];
        if (pNotifyActions == NULL)
        {
            continue;
        }

        // get event list
        pEventNotifications = inotifyGetNotificationList(pNotifier);
        if (pEventNotifications == NULL)
        {
            continue;
        }

        // skip if client not "listening" to events of this type
        if (pNotifyActions[notifyIndex] == disableCmd)
        {
            continue;
        }

        if (pDisplayApi->hNotifierMemory != NV01_NULL_OBJECT &&
            pDisplayApi->pNotifierMemory != NULL)
        {
            notifyFillNotifierMemory(pGpu, pDisplayApi->pNotifierMemory, info32, info16,
                                     NV5070_NOTIFICATION_STATUS_DONE_SUCCESS, notifyIndex);
        }

        // ping events bound to subdevice associated with pGpu
        osEventNotification(pGpu, pEventNotifications,
                            (notifyIndex | OS_EVENT_NOTIFICATION_INDEX_MATCH_SUBDEV),
                            pNotifyParams, notifyParamsSize);

        // reset if single shot notify action
        if (pNotifyActions[notifyIndex] == singleCmd)
        {
            pNotifyActions[notifyIndex] = disableCmd;
        }
    }
}

void
kdispSetWarPurgeSatellitesOnCoreFree_IMPL
(
    KernelDisplay *pKernelDisplay,
    NvBool value
)
{
    pKernelDisplay->bWarPurgeSatellitesOnCoreFree = value;
}

NV_STATUS
kdispRegisterRgLineCallback_IMPL
(
    KernelDisplay *pKernelDisplay,
    RgLineCallback *pRgLineCallback,
    NvU32 head,
    NvU32 rgIntrLine,
    NvBool bEnable
)
{
    NV_ASSERT_OR_RETURN(head < OBJ_MAX_HEADS, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rgIntrLine < MAX_RG_LINE_CALLBACKS_PER_HEAD, NV_ERR_INVALID_ARGUMENT);

    RgLineCallback **slot = &pKernelDisplay->rgLineCallbackPerHead[head][rgIntrLine];

    if (bEnable && *slot == NULL)
    {
        *slot = pRgLineCallback;
    }
    else if (!bEnable && *slot == pRgLineCallback)
    {
        *slot = NULL;
    }
    else
    {
        //
        // OBJDISP is the authority for *allocating* these "slots";
        // KernelDisplay trusts it as an allocator.
        // If we try to register a callback in an existing slot, or free an
        // empty slot, it means OBJDISP has created conflicting allocations or
        // has allowed a double-free. (Or RgLineCallback has provided invalid
        // parameters.)
        //
        NV_ASSERT_FAILED("Invalid KernelDisplay state for RgLineCallback");
        return NV_ERR_INVALID_STATE;
    }

    return NV_OK;
}

void
kdispInvokeRgLineCallback_KERNEL
(
    KernelDisplay *pKernelDisplay,
    NvU32 head,
    NvU32 rgIntrLine,
    NvBool bIsIrqlIsr
)
{
    NV_ASSERT_OR_RETURN_VOID(head < OBJ_MAX_HEADS);
    NV_ASSERT_OR_RETURN_VOID(rgIntrLine < MAX_RG_LINE_CALLBACKS_PER_HEAD);

    RgLineCallback *pCallbackObject = pKernelDisplay->rgLineCallbackPerHead[head][rgIntrLine];

    if (pCallbackObject != NULL)
    {
        rglcbInvoke(pCallbackObject, bIsIrqlIsr);
    }
    else if (IS_GSP_CLIENT(ENG_GET_GPU(pKernelDisplay)))
    {
        //
        // For offloaded RM case, getting a callback invocation without a registered callback could
        // happen during or after deregistration: there might already have been an event in the
        // queue by the time we asked physical RM to deconfigure the interrupt.
        //
        // Because this could lead to an A-B-A situation where a new callback is registered to the
        // same slot and invoked in place of the old callback, we must assert against this case.
        // To avoid this, RgLineCallback must drain the client RM event queue after deconfiguring
        // the interrupt and before calling kdispRegisterRgLineCallback to deregister the callback.
        //
        NV_ASSERT_FAILED("got RgLineCallback invocation for null callback");
    }
    else
    {
        //
        // For the monolithic RM case, getting a callback invocation without a registered callback
        // could happen during registration: after configuring hardware for the interrupt, but
        // before registering the callback with KernelDisplay, the interrupt could be handled.
        //
        // This is not a bug in and of itself as it is harmless and expected. On the other hand we
        // would not expect to see this warning in the log more than a few times per registration,
        // e.g. if it were printed for every single interrupt, as the callback ought to be fully
        // registered before excessively many interrupts are handled.
        //
        NV_PRINTF(LEVEL_WARNING, "got RgLineCallback invocation for null callback\n");
    }
}

#define HOTPLUG_PROFILE 0

#if HOTPLUG_PROFILE

    #define ISR_TSTAMP_SIZE 18000 /* 5 minutes (5*60Hz*60)*/

    NvU32 timeStampIndexISR = ISR_TSTAMP_SIZE-1;

    tmr_tstamp_u timeStampStartISR[ISR_TSTAMP_SIZE];
    tmr_tstamp_u timeStampDeltaISR[ISR_TSTAMP_SIZE];

#endif

void
kdispServiceVblank_KERNEL
(
    OBJGPU            *pGpu,
    KernelDisplay     *pKernelDisplay,
    NvU32              headmask,
    NvU32              state,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32      pending, check_pending, pending_checked;
    NvU32      Head;
    NvU32      maskNonEmptyQueues[OBJ_MAX_HEADS];  // array of masks of VBLANK_STATE_PROCESS_XXX_LATENCY bits, indicating which queues are non-empty
    NvU32      unionNonEmptyQueues = 0;            // mask of VBLANK_STATE_PROCESS_XXX_LATENCY bits, union of queue states of all heads w/ pending vblank ints
    NvU32      Count = 0;
    NvU32      i, skippedcallbacks;
    NvU32      maskCallbacksStillPending = 0;
    KernelHead    *pKernelHead = NULL;

#if HOTPLUG_PROFILE
    OBJTMR    *pTmr;
    pTmr = GPU_GET_TIMER(pGpu);
    if (++timeStampIndexISR >= ISR_TSTAMP_SIZE)
        timeStampIndexISR = 0;

    tmrGetCurrentTime(pTmr, &timeStampStartISR[timeStampIndexISR].time32.hi, &timeStampStartISR[timeStampIndexISR].time32.lo);

    // For the ISR we want to know how much time since the last ISR.
    if (timeStampIndexISR)
    {
        NvU64 temp64;

        temp64 = timeStampStartISR[timeStampIndexISR].time64;
        temp64 -= timeStampStartISR[timeStampIndexISR-1].time64;

        timeStampDeltaISR[timeStampIndexISR].time64 = temp64;
    }
#endif


    // If the caller failed to spec which queue, figure they wanted all of them
    if (!(state & VBLANK_STATE_PROCESS_ALL_CALLBACKS) )
    {
        state |= VBLANK_STATE_PROCESS_ALL_CALLBACKS;
    }

    // If the headmask is 0, we should process all heads
    if (headmask == 0)
    {
        headmask = 0xFFFFFFFF;
    }

    //
    // If we are being asked to process the callbacks now, regardless of the true irqspending,
    // we force the pending mask to the head mask passed in.
    //
    if (state & VBLANK_STATE_PROCESS_IMMEDIATE)
    {
        pending = headmask;
    }
    else
    {
        // We're here because at least one of the PCRTC bits MAY be pending.
        pending = kdispReadPendingVblank_HAL(pGpu, pKernelDisplay, pThreadState);
    }

    //  No sense in doing anything if there is nothing pending.
    if (pending == 0)
    {
        return;
    }

    //
    // We want to check for pending service now and then we check again each
    // time through the loop. Keep these seperate.
    //
    check_pending = pending;

    // We have not checked anything yet
    pending_checked = 0;

    // Start with head 0
    Head = 0;

    //
    // We keep scanning all supported heads, and if we have something pending,
    // check the associated queues
    //
    while(pending_checked != pending)
    {
        pKernelHead = KDISP_GET_HEAD(pKernelDisplay, Head);

        // Move on if this crtc's interrupt isn't pending...
        if ( (headmask & check_pending & ~pending_checked) & NVBIT(Head))
        {
            // Track that we have now checked this head
            pending_checked |= NVBIT(Head);

            // If our queues are empty, we can bail early
            maskNonEmptyQueues[Head]  = kheadCheckVblankCallbacksQueued(pGpu, pKernelHead, state, NULL);
            unionNonEmptyQueues      |= maskNonEmptyQueues[Head];

            // This function will check to see if there are callback states in which the
            // caller has skipped execution.
            skippedcallbacks = ((state & VBLANK_STATE_PROCESS_ALL_CALLBACKS) ^ VBLANK_STATE_PROCESS_ALL_CALLBACKS);
            skippedcallbacks |= (state & (VBLANK_STATE_PROCESS_CALLED_FROM_ISR | VBLANK_STATE_PROCESS_IMMEDIATE));

            // now lets see if there's callbacks pending on the skipped callbacks
            maskCallbacksStillPending |= NVBIT(Head) * !!kheadCheckVblankCallbacksQueued(pGpu, pKernelHead, skippedcallbacks, NULL);
        }

        // Don't check for new interrupts if we are in immediate mode
        if (!(state & VBLANK_STATE_PROCESS_IMMEDIATE) )
        {
            pending = kdispReadPendingVblank_HAL(pGpu, pKernelDisplay, pThreadState);
        }

        // if there was a change in the pending state, we should recheck everything
        if (check_pending != pending)
        {
            // We need to recheck heads that were not pending before
            check_pending = pending;
            Head = 0;
        }
        else
        {
            // Nothing changed, so move on to the next head
            Head++;
        }

        // Make sure we dont waste time on heads that dont exist
        if (Head >= OBJ_MAX_HEADS)
        {
            break;
        }
    }

    if (state & VBLANK_STATE_PROCESS_CALLED_FROM_ISR)
    {
        // store off which heads have pending vblank interrupts, for comparison at the next DPC time.
        pKernelDisplay->isrVblankHeads = pending;

    }

    // increment the per-head vblank total counter, for any head with a pending vblank intr
    for (Head=0; Head < OBJ_MAX_HEADS; Head++)
    {
        // Move on if this crtc's interrupt isn't pending...
        if ((pending & NVBIT(Head)) == 0)
        {
            continue;
        }

        pKernelHead = KDISP_GET_HEAD(pKernelDisplay, Head);
        //
        // increment vblank counters, as appropriate.
        //

        // Track the fact that we passed through here. This keeps the RC manager happy.
        Count = kheadGetVblankTotalCounter_HAL(pKernelHead) + 1;
        kheadSetVblankTotalCounter_HAL(pKernelHead, Count);

        //
        // Update the vblank counter if we are single chip or multichip master.
        // We now have two queues, so we need to have two vblank counters.
        //

        // did they ask for processing of low-latency work?
        if (state & VBLANK_STATE_PROCESS_LOW_LATENCY /* & maskNonEmptyQueues[Head]*/)
        {
            //
            // don't let the DPC thread increment the low-latency counter.
            // otherwise, the counter will frequently increment at double the
            // expected rate, breaking things like swapInterval.
            //
            // XXX actually, there is one case where it would be OK for the DPC
            // thread to increment this counter:  if the DPC thread could ascertain
            // that 'pending & NVBIT(Head)' represented a new interrupt event, and
            // not simply the one that the ISR left uncleared in PCRTC_INTR_0, for
            // the purpose of causing this DPC thread to get queued.
            // Not sure how to do that.
            //
            if ( !(state & VBLANK_STATE_PROCESS_CALLED_FROM_DPC) || (pending & NVBIT(Head) & ~pKernelDisplay->isrVblankHeads) )
            {
                // either we were called from the ISR, or vblank is asserted in DPC when it wasn't in the ISR

                // low latency queue requested, and this isn't a DPC thread.
                Count = kheadGetVblankLowLatencyCounter_HAL(pKernelHead) + 1;
                kheadSetVblankLowLatencyCounter_HAL(pKernelHead, Count);
           }
        }

        // did they ask for processing of normal-latency work?
        if (state & VBLANK_STATE_PROCESS_NORMAL_LATENCY /* & maskNonEmptyQueues[Head]*/)
        {
            // processing of the normal latency queue requested
            Count = kheadGetVblankNormLatencyCounter_HAL(pKernelHead) + 1;
            kheadSetVblankNormLatencyCounter_HAL(pKernelHead, Count);
        }
    }

    //
    // If we have nothing to process (no work to do in queue),
    // we can bail early. We got here for some reason, so make
    // sure we clear the interrupts.
    //

    if (!unionNonEmptyQueues)
    {
        // all queues (belonging to heads with pending vblank ints) are empty.
        if (IS_GSP_CLIENT(pGpu))
        {
            kheadResetPendingVblank_HAL(pGpu, pKernelHead, pThreadState);
        }
        return;
    }

    //
    // Although we have separate handlers for each head, attempt to process all
    // interrupting heads now. What about DPCs schedule already?
    //
    for (Head = 0; Head < OBJ_MAX_HEADS; Head++)
    {
        pKernelHead = KDISP_GET_HEAD(pKernelDisplay, Head);
        // Move on if this crtc's interrupt isn't pending...
        if ((pending & NVBIT(Head)) == 0)
        {
            continue;
        }

        // Process the callback list for this Head...
        kheadProcessVblankCallbacks(pGpu, pKernelHead, state);
    }

    //
    // if there are still callbacks pending, and we are in an ISR,
    // then don't clear PCRTC_INTR; XXXar why would we *ever* want
    // to clear PCRTC_INTR if there are still things pending?
    //
    if ( (maskCallbacksStillPending) &&
         (state & VBLANK_STATE_PROCESS_CALLED_FROM_ISR) )
    {
        //
        // there are still callbacks pending; don't clear
        // PCRTC_INTR, yet. The expectation is that the OS layer
        // will see that interrupts are still pending and queue a
        // DPC/BottomHalf/whatever to service the rest of the
        // vblank callback queues
        //
        for(i=0; i< OBJ_MAX_HEADS; i++)
        {
            pKernelHead = KDISP_GET_HEAD(pKernelDisplay, i);
            if (IS_GSP_CLIENT(pGpu))
            {
                kheadResetPendingVblank_HAL(pGpu, pKernelHead, pThreadState);
            }
        }
    }
    else
    {
        // reset the VBlank intrs we've handled, and don't reset the vblank intrs we haven't.
        for(i=0; i< OBJ_MAX_HEADS; i++)
        {
            pKernelHead = KDISP_GET_HEAD(pKernelDisplay, i);
            if (pending & NVBIT(i) & ~maskCallbacksStillPending)
            {
                kheadResetPendingVblank_HAL(pGpu, pKernelHead, pThreadState);
            }
        }
    }

    return;
}

NvU32 kdispReadPendingVblank_IMPL(OBJGPU *pGpu, KernelDisplay *pKernelDisplay, THREAD_STATE_NODE *pThreadState)
{
    KernelHead *pKernelHead;
    NvU32       headIdx, pending = 0;

    for (headIdx = 0; headIdx < kdispGetNumHeads(pKernelDisplay); headIdx++)
    {
        pKernelHead = KDISP_GET_HEAD(pKernelDisplay, headIdx);

        if (kheadReadPendingVblank_HAL(pGpu, pKernelHead, NULL, pThreadState))
        {
            pending |= NVBIT(headIdx);
        }
    }

    return pending;
}

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kdispRegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU32 engineIdx = MC_ENGINE_IDX_DISP;
    NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
    pRecords[engineIdx].pInterruptService = staticCast(pKernelDisplay, IntrService);
}

/*!
 * @brief Route modeset start/end notification to kernel RM
 *
 * Physical RM is expected to send a "start" notification at the beginning of
 * every display modeset (supervisor interrupt sequence), and an "end"
 * notification at the end.  However, if physical RM detects back-to-back
 * modesets, the intervening "end" notification MAY be skipped; in this case,
 * the "start" notification for the next modeset serves as the "end notification
 * for the previous modeset.
 *
 * Kernel RM will use the notification to update the BW allocation for display.
 * The ICC call that is required to update the BW allocation cannot be made
 * from physical RM.
 *
 * @param[in] pKernelDisplay                KernelDisplay pointer
 * @param[in] bModesetStart                 NV_TRUE -> start of modeset;
 *                                          NV_FALSE -> end of modeset
 * @param[in] minRequiredIsoBandwidthKBPS   Min ISO BW required by IMP (KB/sec)
 * @param[in] minRequiredFloorBandwidthKBPS Min dramclk freq * pipe width (KB/sec)
 */
void
kdispInvokeDisplayModesetCallback_KERNEL
(
    KernelDisplay *pKernelDisplay,
    NvBool bModesetStart,
    NvU32 minRequiredIsoBandwidthKBPS,
    NvU32 minRequiredFloorBandwidthKBPS
)
{
    NV_STATUS   status;

    NV_PRINTF(LEVEL_INFO,
              "Kernel RM received \"%s of modeset\" notification "
              "(minRequiredIsoBandwidthKBPS = %u, minRequiredFloorBandwidthKBPS = %u)\n",
              bModesetStart ? "start" : "end",
              minRequiredIsoBandwidthKBPS,
              minRequiredFloorBandwidthKBPS);

    OBJGPU *pGpu = ENG_GET_GPU(pKernelDisplay);
    status =
        kdispArbAndAllocDisplayBandwidth_HAL(pGpu,
                                             pKernelDisplay,
                                             DISPLAY_ICC_BW_CLIENT_RM,
                                             minRequiredIsoBandwidthKBPS,
                                             minRequiredFloorBandwidthKBPS);
    //
    // The modeset cannot be aborted, so, if there is an error, no recovery
    // is possible.
    //
    NV_ASSERT_OK(status);
}
