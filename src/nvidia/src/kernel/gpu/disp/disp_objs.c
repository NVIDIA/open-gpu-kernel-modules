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

/******************************************************************************
*
*   Description:
*       This file contains functions managing the display - both Disp and DispCommon
*       entries with their insides (DispChannelList and DispDmaControlList)
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "core/locks.h"
#include "resserv/rs_client.h"

#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu/disp/disp_objs.h"
#include "gpu/disp/disp_channel.h"
#include "gpu/disp/kern_disp.h"
#include "gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"

#include "kernel/gpu/intr/intr.h"

#include "class/cl0073.h"                // NV04_DISPLAY_COMMON
#include "class/cl5070.h"                // NV50_DISPLAY
#include "class/clc370.h"                // NVC370_DISPLAY

NV_STATUS
dispapiConstruct_IMPL
(
    DisplayApi                   *pDisplayApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS        status;
    CLASSDESCRIPTOR *pClassDescriptor;
    RsResourceRef   *pResourceRef = pCallContext->pResourceRef;
    OBJGPU          *pGpu;
    KernelDisplay   *pKernelDisplay;
    NvBool           bBcResource;
    NvU32            i;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Use gpuGetByRef instead of GpuResource because it will work even if resource
    // isn't a GpuResource.
    status = gpuGetByRef(pResourceRef, &bBcResource, &pGpu);
    if (status != NV_OK)
        return status;

    // Find class in class db (verifies class is valid for this GPU)
    status = gpuGetClassByClassId(pGpu, pParams->externalClassId, &pClassDescriptor);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "bad class 0x%x\n", pParams->externalClassId);
        return NV_ERR_INVALID_CLASS;
    }

    // Check display is enabled (i.e. not displayless)
    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    if (pKernelDisplay == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    for (i = 0; i < NV2080_MAX_SUBDEVICES; i++)
        pDisplayApi->pNotifyActions[i]    = NULL;

    pDisplayApi->pGpuInRmctrl = NULL;
    pDisplayApi->pGpuGrp = gpumgrGetGpuGrpFromGpu(pGpu);
    pDisplayApi->bBcResource = bBcResource;

    gpuSetThreadBcState(pGpu, bBcResource);

    return status;
}

void
dispapiDestruct_IMPL
(
    DisplayApi *pDisplayApi
)
{
    NvU32 i;

    // Free notify actions memory if it's been allocated
    for (i = 0; i < NV2080_MAX_SUBDEVICES; i++)
    {
        portMemFree(pDisplayApi->pNotifyActions[i]);
        pDisplayApi->pNotifyActions[i] = NULL;
    }
}

static NV_STATUS
_dispapiNotifierInit
(
    DisplayApi *pDisplayApi,
    NvU32       numNotifiers,
    NvU32       disableCmd
)
{
    NvU32     i, j;
    NV_STATUS status = NV_OK;

    pDisplayApi->numNotifiers = numNotifiers;

    for (i = 0; i < NV2080_MAX_SUBDEVICES; i++)
    {
        // get memory for pNotifyActions table
        pDisplayApi->pNotifyActions[i] = portMemAllocNonPaged(
                                   pDisplayApi->numNotifiers * sizeof(NvU32));
        if (pDisplayApi->pNotifyActions[i] != NULL)
        {
            // default actions for each notifier type is disabled
            for (j = 0; j < pDisplayApi->numNotifiers; j++)
            {
                pDisplayApi->pNotifyActions[i][j] = disableCmd;
            }
        }
        else
        {
            goto fail;
        }
    }

    return status;

fail:
    // first release any notifyActions memory
    for (i = 0; i < NV2080_MAX_SUBDEVICES; i++)
    {
        portMemFree(pDisplayApi->pNotifyActions[i]);
        pDisplayApi->pNotifyActions[i] = NULL;
    }

    return NV_ERR_INSUFFICIENT_RESOURCES;
}

NV_STATUS
dispobjConstructHal_IMPL
(
    DispObject                   *pDispObject,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    DisplayApi  *pDisplayApi  = staticCast(pDispObject, DisplayApi);
    Device      *pDevice      = dynamicCast(pCallContext->pResourceRef->pParentRef->pResource, Device);
    GpuResource *pGpuResource = staticCast(pDevice, GpuResource);
    OBJGPU      *pGpu         = pGpuResource->pGpu;
    NV_STATUS    rmStatus     = NV_ERR_INVALID_STATE;

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
    {
        KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

        rmStatus = kdispSelectClass_HAL(pGpu, pKernelDisplay, pCallContext->pResourceRef->externalClassId);

        if (rmStatus != NV_OK)
        {
            // If the operation fails, it should fail on the first try
            NV_ASSERT(gpumgrIsParentGPU(pGpu));
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END;

    if (rmStatus != NV_OK)
        return rmStatus;

    if(dynamicCast(pDisplayApi, NvDispApi))
    {
        rmStatus = _dispapiNotifierInit(pDisplayApi,
                                        NVC370_NOTIFIERS_MAXCOUNT,
                                        NVC370_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE);
    }
    else
    {
        rmStatus = _dispapiNotifierInit(pDisplayApi,
                                        NV5070_NOTIFIERS_MAXCOUNT,
                                        NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE);
    }

    return rmStatus;
}

NV_STATUS
dispobjConstruct_IMPL
(
    DispObject                   *pDispObject,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    pDispObject->rmFreeFlags = NV5070_CTRL_SET_RMFREE_FLAGS_NONE;

    if (pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_USER_ROOT)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failure allocating display class 0x%08x: Only root(admin)/kernel clients are allowed\n",
                  pParams->externalClassId);

        //
        // GPUSWSEC-1560 introduced a central object privilege check in RS. Please mark derived external classes
        // of DispObject privileged in their RS_ENTRY. Since DispObject doesn't have an external class of its own
        // and is used as a base class, leaving this check inline to catch future derivations.
        //
        osAssertFailed();

        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return dispobjConstructHal_HAL(pDispObject, pCallContext, pParams);
}

NV_STATUS
dispobjGetByHandle_IMPL
(
    RsClient    *pClient,
    NvHandle     hDispObject,
    DispObject **ppDispObject
)
{
    NV_STATUS      status;
    RsResourceRef *pResourceRef;

    status = clientGetResourceRef(pClient, hDispObject, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppDispObject = dynamicCast(pResourceRef->pResource, DispObject);

    return (*ppDispObject) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
dispobjGetByDevice_IMPL
(
    RsClient    *pClient,
    Device      *pDevice,
    DispObject **ppDispObject
)
{
    NV_STATUS      status;
    RsResourceRef *pResourceRef;

    status = refFindChildOfType(RES_GET_REF(pDevice), classId(DispObject), NV_FALSE /*bExactMatch*/, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppDispObject = dynamicCast(pResourceRef->pResource, DispObject);

    return (*ppDispObject) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

//
// Most display control calls take a subDeviceInstance argument.
// We need to verify that this argument is valid and then use it to
// locate the correct OBJGPU for the particular subdevice.
//
NV_STATUS
dispapiSetUnicastAndSynchronize_KERNEL
(
    DisplayApi      *pDisplayApi,
    OBJGPUGRP       *pGpuGroup,
    OBJGPU         **ppGpu,
    OBJDISP        **ppDisp,
    NvU32            subDeviceInstance
)
{
    NV_STATUS   nvStatus    = NV_OK;

    nvStatus = gpugrpGetGpuFromSubDeviceInstance(pGpuGroup, subDeviceInstance, ppGpu);
    if (nvStatus != NV_OK)
        return nvStatus;

    gpumgrSetBcEnabledStatus(*ppGpu, NV_FALSE);

    //
    // The _KERNEL version of this function is only called from Kernel RM, but
    // in Kernel RM, OBJDISP is not available, so ppDisp must be NULL. If the
    // caller needs to access OBJDISP, either the caller code must remove the
    // OBJDISP dependency, or the caller code must be changed so that
    // dispapiSetUnicastAndSynchronize is called only from physical or
    // monolithic RM, never Kernel RM.
    //
    if (ppDisp != NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    return nvStatus;
}

NV_STATUS
dispapiControl_Prologue_IMPL
(
    DisplayApi                     *pDisplayApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRmCtrlParams
)
{
    NvU32       subdeviceIndex;
    NV_STATUS   status;
    RmResource *pResource = staticCast(pDisplayApi, RmResource);

    if (dynamicCast(pDisplayApi, DispCommon))
    {
        Device      *pDevice = dynamicCast(pCallContext->pResourceRef->pParentRef->pResource, Device);
        GpuResource *pGpuResource = staticCast(pDevice, GpuResource);

        pResource->rpcGpuInstance = gpuGetInstance(pGpuResource->pGpu);
        pDisplayApi->pGpuInRmctrl = pGpuResource->pGpu;
        return rmresControl_Prologue_IMPL(pResource, pCallContext, pRmCtrlParams);
    }

    // Read the subdevice ID out and swap GPU pointer
    if (dynamicCast(pDisplayApi, NvDispApi))
    {
        NVC370_CTRL_CMD_BASE_PARAMS *pBaseParameters = pRmCtrlParams->pParams;

        //
        // All non-NULL disp control 5070 methods have
        // NVC370_CTRL_CMD_BASE_PARAMS as their first member.
        //
        if ((pBaseParameters == NULL) || (pRmCtrlParams->paramsSize < sizeof(NVC370_CTRL_CMD_BASE_PARAMS)))
        {
            status = NV_ERR_INVALID_PARAM_STRUCT;
            goto done;
        }
        subdeviceIndex = pBaseParameters->subdeviceIndex;
    }
    else if (dynamicCast(pDisplayApi, DispSwObj))
    {
        NVC372_CTRL_CMD_BASE_PARAMS *pBaseParameters = pRmCtrlParams->pParams;

        //
        // All non-NULL disp control C372 methods have
        // NVC372_CTRL_CMD_BASE_PARAMS as their first member.
        //
        if ((pBaseParameters == NULL) || (pRmCtrlParams->paramsSize < sizeof(NVC372_CTRL_CMD_BASE_PARAMS)))
        {
            status = NV_ERR_INVALID_PARAM_STRUCT;
            goto done;
        }
        subdeviceIndex = pBaseParameters->subdeviceIndex;
    }
    else
    {
        NV5070_CTRL_CMD_BASE_PARAMS *pBaseParameters = pRmCtrlParams->pParams;

        //
        // All non-NULL disp control 5070 methods have
        // NV5070_CTRL_CMD_BASE_PARAMS as their first member.
        //
        if ((pBaseParameters == NULL) || (pRmCtrlParams->paramsSize < sizeof(NV5070_CTRL_CMD_BASE_PARAMS)))
        {
            status = NV_ERR_INVALID_PARAM_STRUCT;
            goto done;
        }
        subdeviceIndex = pBaseParameters->subdeviceIndex;
    }

    status = dispapiSetUnicastAndSynchronize_HAL(pDisplayApi,
                                             pRmCtrlParams->pGpuGrp,
                                             &pRmCtrlParams->pGpu,
                                             NULL,
                                             subdeviceIndex);

    if (status == NV_OK)
    {
        pResource->rpcGpuInstance = gpuGetInstance(pRmCtrlParams->pGpu);
        pDisplayApi->pGpuInRmctrl = pRmCtrlParams->pGpu;
        return rmresControl_Prologue_IMPL(pResource, pCallContext, pRmCtrlParams);
    }

done:
    return status;
}

void
dispapiControl_Epilogue_IMPL
(
    DisplayApi                     *pDisplayApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRmCtrlParams
)
{
    if (dynamicCast(pDisplayApi, DispCommon) == NULL)
    {
        RmResource *pResource = staticCast(pDisplayApi, RmResource);
        pResource->rpcGpuInstance = ~0;
    }

    pDisplayApi->pGpuInRmctrl = NULL;
}

NV_STATUS
dispapiControl_IMPL
(
    DisplayApi                     *pDisplayApi,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    Intr             *pIntr;
    NV_STATUS         status        = NV_OK;
    Device           *pDevice       = dynamicCast(pCallContext->pResourceRef->pParentRef->pResource, Device);
    GpuResource      *pGpuResource  = staticCast(pDevice, GpuResource);
    RmCtrlParams     *pRmCtrlParams = pParams->pLegacyParams;
    OBJGPU           *pGpu          = pGpuResource->pGpu;

    NV_PRINTF(LEVEL_INFO, "class: 0x%x cmd 0x%x\n",
              RES_GET_EXT_CLASS_ID(pDisplayApi),
              pRmCtrlParams->cmd);

    pRmCtrlParams->pGpu    = pGpu;
    pRmCtrlParams->pGpuGrp = pGpuResource->pGpuGrp;

    gpuSetThreadBcState(pGpu, NV_TRUE);

    pIntr = GPU_GET_INTR(pGpu);
    if (pIntr != NULL)
    {
        bitVectorClrAll(&pIntr->helperEngineMask);
        bitVectorSet(&pIntr->helperEngineMask, MC_ENGINE_IDX_GR);
        bitVectorSet(&pIntr->helperEngineMask, MC_ENGINE_IDX_DISP);
        bitVectorSet(&pIntr->helperEngineMask, MC_ENGINE_IDX_FIFO);
    }

    status = resControl_IMPL(staticCast(pDisplayApi, RsResource),
                                pCallContext, pParams);

    if (pIntr != NULL)
    {
        bitVectorClrAll(&pIntr->helperEngineMask);
    }

    return status;
}

NV_STATUS
dispswobjConstruct_IMPL
(
    DispSwObj                    *pDispSwObj,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
dispcmnConstruct_IMPL
(
    DispCommon                   *pDispCommon,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    DisplayApi *pDisplayApi = staticCast(pDispCommon, DisplayApi);

    //
    // Not adding the priv-level check for this class
    // as it is being used by OpenGL from userspace.Once the Cleanup is done from the OpenGL
    // we can add the priv level check here  below
    //

    pDispCommon->hotPlugMaskToBeReported = 0;
    pDispCommon->hotUnplugMaskToBeReported = 0;

    return _dispapiNotifierInit(pDisplayApi,
                                NV0073_NOTIFIERS_MAXCOUNT,
                                NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE);
}

NV_STATUS
dispcmnGetByHandle_IMPL
(
    RsClient    *pClient,
    NvHandle     hDispCommon,
    DispCommon **ppDispCommon
)
{
    NV_STATUS      status;
    RsResourceRef *pResourceRef;

    status = clientGetResourceRef(pClient, hDispCommon, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppDispCommon = dynamicCast(pResourceRef->pResource, DispCommon);

    return (*ppDispCommon) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

void
dispcmnGetByDevice_IMPL
(
    RsClient    *pClient,
    NvHandle     hDevice,
    DispCommon **ppDispCommon
)
{
    Device        *pDevice;
    RsResourceRef *pResourceRef;

    *ppDispCommon = NULL; /* return failure by default */

    if (deviceGetByHandle(pClient, hDevice, &pDevice) != NV_OK)
        return;

    if (refFindChildOfType(RES_GET_REF(pDevice),
                           classId(DispCommon),
                           NV_FALSE,
                           &pResourceRef) != NV_OK)
        return;

    *ppDispCommon = dynamicCast(pResourceRef->pResource, DispCommon);
}

/**
 * @brief Return NV_TRUE if RmFree() needs to preserve the HW, otherwise NV_FALSE
 *
 * @param[in] DispObject Pointer
 */
NvBool dispobjGetRmFreeFlags_IMPL(DispObject *pDispObject)
{
    return !!(pDispObject->rmFreeFlags & NV5070_CTRL_SET_RMFREE_FLAGS_PRESERVE_HW);
}

/**
 * @brief Clears the RmFree() temporary flags
 *
 * @param[in] DispObject Pointer
 *
 * @return void
 */
void dispobjClearRmFreeFlags_IMPL(DispObject *pDispObject)
{
    pDispObject->rmFreeFlags  = NV5070_CTRL_SET_RMFREE_FLAGS_NONE;
}

NV_STATUS
nvdispapiConstruct_IMPL
(
    NvDispApi                    *pNvdispApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
nvdispapiCtrlCmdSetAccl_IMPL
(
    NvDispApi *pNvDispApi,
    NVC370_CTRL_SET_ACCL_PARAMS *pParams
)
{
    POBJGPU pGpu = DISPAPI_GET_GPU(pNvDispApi);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pNvDispApi);
    DISPCHNCLASS internalChnClass = dispChnClass_Supported;
    NvU32 accelerators = DISP_ACCL_NONE;
    NvU32 accelMask = DISP_ACCL_NONE;
    NV_STATUS status = NV_OK;

    status = kdispGetIntChnClsForHwCls(GPU_GET_KERNEL_DISPLAY(pGpu), pParams->channelClass, &internalChnClass);
    if (status != NV_OK)
        return status;

    if (pParams->accelerators & NVC370_CTRL_ACCL_IGNORE_PI)
        accelerators |= DISP_ACCL_IGNORE_PI;
    if (pParams->accelerators & NVC370_CTRL_ACCL_SKIP_NOTIF)
        accelerators |= DISP_ACCL_SKIP_NOTIF;
    if (pParams->accelerators & NVC370_CTRL_ACCL_SKIP_SEMA)
        accelerators |= DISP_ACCL_SKIP_SEMA;
    if (pParams->accelerators & NVC370_CTRL_ACCL_IGNORE_INTERLOCK)
        accelerators |= DISP_ACCL_IGNORE_INTERLOCK;
    if (pParams->accelerators & NVC370_CTRL_ACCL_IGNORE_FLIPLOCK)
        accelerators |= DISP_ACCL_IGNORE_FLIPLOCK;
    if (pParams->accelerators & NVC370_CTRL_ACCL_TRASH_ONLY)
        accelerators |= DISP_ACCL_TRASH_ONLY;
    if (pParams->accelerators & NVC370_CTRL_ACCL_TRASH_AND_ABORT)
        accelerators |= DISP_ACCL_TRASH_AND_ABORT;
    if (pParams->accelerators & NVC370_CTRL_ACCL_SKIP_SYNCPOINT)
        accelerators |= DISP_ACCL_SKIP_SYNCPOINT;
    if (pParams->accelerators & NVC370_CTRL_ACCL_IGNORE_TIMESTAMP)
        accelerators |= DISP_ACCL_IGNORE_TIMESTAMP;
    if (pParams->accelerators & NVC370_CTRL_ACCL_IGNORE_MGI)
        accelerators |= DISP_ACCL_IGNORE_MGI;

    if (pParams->accelMask & NVC370_CTRL_ACCL_IGNORE_PI)
        accelMask |= DISP_ACCL_IGNORE_PI;
    if (pParams->accelMask & NVC370_CTRL_ACCL_SKIP_NOTIF)
        accelMask |= DISP_ACCL_SKIP_NOTIF;
    if (pParams->accelMask & NVC370_CTRL_ACCL_SKIP_SEMA)
        accelMask |= DISP_ACCL_SKIP_SEMA;
    if (pParams->accelMask & NVC370_CTRL_ACCL_IGNORE_INTERLOCK)
        accelMask |= DISP_ACCL_IGNORE_INTERLOCK;
    if (pParams->accelMask & NVC370_CTRL_ACCL_IGNORE_FLIPLOCK)
        accelMask |= DISP_ACCL_IGNORE_FLIPLOCK;
    if (pParams->accelMask & NVC370_CTRL_ACCL_TRASH_ONLY)
        accelMask |= DISP_ACCL_TRASH_ONLY;
    if (pParams->accelMask & NVC370_CTRL_ACCL_TRASH_AND_ABORT)
        accelMask |= DISP_ACCL_TRASH_AND_ABORT;
    if (pParams->accelMask & NVC370_CTRL_ACCL_SKIP_SYNCPOINT)
        accelMask |= DISP_ACCL_SKIP_SYNCPOINT;
    if (pParams->accelMask & NVC370_CTRL_ACCL_IGNORE_TIMESTAMP)
        accelMask |= DISP_ACCL_IGNORE_TIMESTAMP;
    if (pParams->accelMask & NVC370_CTRL_ACCL_IGNORE_MGI)
        accelMask |= DISP_ACCL_IGNORE_MGI;

    return kdispSetAccel_HAL(pGpu, pKernelDisplay,
                            hClient,
                            internalChnClass,
                            pParams->channelInstance,
                            accelerators,
                            accelMask);
}

NV_STATUS
nvdispapiCtrlCmdGetAccl_IMPL
(
    NvDispApi *pNvDispApi,
    NVC370_CTRL_GET_ACCL_PARAMS *pParams
)
{
    POBJGPU pGpu = DISPAPI_GET_GPU(pNvDispApi);
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pNvDispApi);
    DISPCHNCLASS internalChnClass = dispChnClass_Supported;
    NvU32 accelerators = DISP_ACCL_NONE;
    NV_STATUS status = NV_OK;

    status = kdispGetIntChnClsForHwCls(GPU_GET_KERNEL_DISPLAY(pGpu), pParams->channelClass, &internalChnClass);
    if (status != NV_OK)
        return status;

    status = kdispGetAccel_HAL(pGpu, GPU_GET_KERNEL_DISPLAY(pGpu),
                              hClient,
                              internalChnClass,
                              pParams->channelInstance,
                              &accelerators);

    pParams->accelerators = NVC370_CTRL_ACCL_NONE;

    if (accelerators & DISP_ACCL_IGNORE_PI)
        pParams->accelerators |= NVC370_CTRL_ACCL_IGNORE_PI;
    if (accelerators & DISP_ACCL_SKIP_NOTIF)
        pParams->accelerators |= NVC370_CTRL_ACCL_SKIP_NOTIF;
    if (accelerators & DISP_ACCL_SKIP_SEMA)
        pParams->accelerators |= NVC370_CTRL_ACCL_SKIP_SEMA;
    if (accelerators & DISP_ACCL_IGNORE_INTERLOCK)
        pParams->accelerators |= NVC370_CTRL_ACCL_IGNORE_INTERLOCK;
    if (accelerators & DISP_ACCL_IGNORE_FLIPLOCK)
        pParams->accelerators |= NVC370_CTRL_ACCL_IGNORE_FLIPLOCK;
    if (accelerators & DISP_ACCL_TRASH_ONLY)
        pParams->accelerators |= NVC370_CTRL_ACCL_TRASH_ONLY;
    if (accelerators & DISP_ACCL_TRASH_AND_ABORT)
        pParams->accelerators |= NVC370_CTRL_ACCL_TRASH_AND_ABORT;
    if (accelerators & DISP_ACCL_SKIP_SYNCPOINT)
        pParams->accelerators |= NVC370_CTRL_ACCL_SKIP_SYNCPOINT;
    if (accelerators & DISP_ACCL_IGNORE_TIMESTAMP)
        pParams->accelerators |= NVC370_CTRL_ACCL_IGNORE_TIMESTAMP;
    if (accelerators & DISP_ACCL_IGNORE_MGI)
        pParams->accelerators |= NVC370_CTRL_ACCL_IGNORE_MGI;

    return status;
}

NV_STATUS
nvdispapiCtrlCmdGetChannelInfo_IMPL
(
    NvDispApi *pNvDispApi,
    NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *pParams
)
{
    POBJGPU pGpu = DISPAPI_GET_GPU(pNvDispApi);
    DISPCHNSTATE channelState = dispChnState_Supported;
    DISPCHNCLASS internalChnClass = dispChnClass_Supported;
    NV_STATUS status = NV_OK;
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    status = kdispGetIntChnClsForHwCls(pKernelDisplay, pParams->channelClass, &internalChnClass);
    if (status != NV_OK)
    {
        return status;
    }

    status = kdispReadChannelState_HAL(pGpu, pKernelDisplay,
                                      internalChnClass,
                                      pParams->channelInstance,
                                      &channelState);
    if (status != NV_OK)
    {
        return status;
    }

    switch (channelState)
    {
        case dispChnState_Idle:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_IDLE;
            break;

        case dispChnState_Busy:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_BUSY;
            break;

        case dispChnState_Dealloc:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_DEALLOC;
            break;

        case dispChnState_DeallocLimbo:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_DEALLOC_LIMBO;
            break;

        case dispChnState_Fcodeinit1:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_EFI_INIT1;
            break;

        case dispChnState_Fcodeinit2:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_EFI_INIT2;
            break;

        case dispChnState_Fcode:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_EFI_OPERATION;
            break;

        case dispChnState_Vbiosinit1:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_VBIOS_INIT1;
            break;

        case dispChnState_Vbiosinit2:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_VBIOS_INIT2;
            break;

        case dispChnState_Vbiosoper:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_VBIOS_OPERATION;
            break;

        case dispChnState_Unconnected:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_UNCONNECTED;
            break;

        case dispChnState_Initialize1:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_INIT1;
            break;

        case dispChnState_Initialize2:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_INIT2;
            break;

        case dispChnState_Shutdown1:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_SHUTDOWN1;
            break;

        case dispChnState_Shutdown2:
            pParams->channelState = NVC370_CTRL_GET_CHANNEL_INFO_STATE_SHUTDOWN2;
            break;

        default:
            DBG_BREAKPOINT(); // We should never hit this when status is OK
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

    status = kdispReadDebugStatus_HAL(pGpu, pKernelDisplay,
                                     internalChnClass,
                                     pParams->channelInstance,
                                     &pParams->IsChannelInDebugMode);

    return status;
}

// ****************************************************************************
//                            Deprecated Functions
// ****************************************************************************

/**
 * @warning This function is deprecated! Please use dispchnGetByHandle.
 */
NV_STATUS
CliFindDispChannelInfo
(
    NvHandle       hClient,
    NvHandle       hDispChannel,
    DispChannel  **ppDispChannel,
    NvHandle      *phParent
)
{
    RsClient   *pClient;
    NV_STATUS   status;

    *ppDispChannel = NULL;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return NV_ERR_INVALID_CLIENT;

    status = dispchnGetByHandle(pClient, hDispChannel, ppDispChannel);
    if (status != NV_OK)
        return status;

    if (phParent)
        *phParent = RES_GET_PARENT_HANDLE(*ppDispChannel);

    return NV_OK;
}

//
// DISP Event RM Controls
//
NV_STATUS
dispapiCtrlCmdEventSetNotification_IMPL
(
    DisplayApi *pDisplayApi,
    NV0073_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams
)
{
    OBJGPU *pGpu = DISPAPI_GET_GPU(pDisplayApi);
    NvU32 *pNotifyActions;
    NV_STATUS status = NV_OK;
    PEVENTNOTIFICATION pEventNotifications = inotifyGetNotificationList(staticCast(pDisplayApi, INotifier));

    // NV01_EVENT must have been plugged into this subdevice
    if (pEventNotifications == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "cmd 0x%x: no event list\n", NV0073_CTRL_CMD_EVENT_SET_NOTIFICATION);
        return NV_ERR_INVALID_STATE;
    }

    // error check event index
    if (pSetEventParams->event >= pDisplayApi->numNotifiers)
    {
        NV_PRINTF(LEVEL_INFO, "bad event 0x%x\n", pSetEventParams->event);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // error check subDeviceInstance
    if (pSetEventParams->subDeviceInstance >= gpumgrGetSubDeviceMaxValuePlus1(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "bad subDeviceInstance 0x%x\n",
                  pSetEventParams->subDeviceInstance);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pNotifyActions = pDisplayApi->pNotifyActions[pSetEventParams->subDeviceInstance];

    switch (pSetEventParams->action)
    {
        case NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE:
        case NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT:
        {
            // must be in disabled state to transition to an active state
            if (pNotifyActions[pSetEventParams->event] != NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
            {
                status = NV_ERR_INVALID_STATE;
                break;
            }

            // bind hEvent to particular subdeviceInst
            status = bindEventNotificationToSubdevice(pEventNotifications,
                                                      pSetEventParams->hEvent,
                                                      pSetEventParams->subDeviceInstance);
            if (status != NV_OK)
                return status;

            pNotifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }

        case NV0073_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE:
        {
            pNotifyActions[pSetEventParams->event] = pSetEventParams->action;
            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return status;
}
