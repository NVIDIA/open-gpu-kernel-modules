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
*       This is a device resource implementation.
*
******************************************************************************/



#include "resserv/resserv.h"
#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "platform/sli/sli.h"

#include "class/cl0080.h"
#include "core/locks.h"
#include "vgpu/rpc.h"
#include "mem_mgr/mem.h"

#include "rmapi/rs_utils.h"
#include "nvsecurityinfo.h"

#include "gpu/gr/kernel_sm_debugger_session.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "Nvcm.h"
#include "diagnostics/gpu_acct.h"
#include "gpu/perf/kern_cuda_limit.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

static NV_STATUS _deviceTeardown(Device *pDevice, CALL_CONTEXT *pCallContext);
static NV_STATUS _deviceTeardownRef(Device *pDevice, CALL_CONTEXT *pCallContext);
static NV_STATUS _deviceInit(Device *pDevice, CALL_CONTEXT *pCallContext,
                             NvHandle hClient, NvHandle hDevice, NvU32 deviceInst,
                             NvHandle hClientShare, NvHandle hTargetClient, NvHandle hTargetDevice,
                             NvU64 vaSize, NvU64 vaStartInternal, NvU64 vaLimitInternal,
                             NvU32 allocFlags, NvU32 vaMode, NvBool *pbIsFirstDevice);

NV_STATUS
deviceConstruct_IMPL
(
    Device *pDevice,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV0080_ALLOC_PARAMETERS         *pNv0080AllocParams = pParams->pAllocParams;
    NvU32                            deviceInst, flags, vaMode;
    NvU32                            deviceClass        = pParams->externalClassId;
    NvHandle                         hClientShare;
    NvHandle                         hTargetClient      = NV01_NULL_OBJECT;
    NvHandle                         hTargetDevice      = NV01_NULL_OBJECT;
    NvU64                            vaSize             = 0;
    NV_STATUS                        rmStatus           = NV_OK;
    OBJSYS                          *pSys               = SYS_GET_INSTANCE();
    OBJOS                           *pOS                = SYS_GET_OS(pSys);
    OBJGPU                          *pGpu;
    NvU64                            vaStartInternal     = 0;
    NvU64                            vaLimitInternal     = 0;
    NvU32                            physicalAllocFlags;
    NvBool                           bIsFirstDevice;

    if (pNv0080AllocParams == NULL)
    {
        deviceInst   = pParams->externalClassId - NV01_DEVICE_0;
        hClientShare = NV01_NULL_OBJECT;
        flags        = 0;
        vaSize       = 0;
        vaMode       = 0;
    }
    else
    {
        deviceInst      = pNv0080AllocParams->deviceId;
        hClientShare    = pNv0080AllocParams->hClientShare;
        hTargetClient   = pNv0080AllocParams->hTargetClient;
        hTargetDevice   = pNv0080AllocParams->hTargetDevice;
        flags           = pNv0080AllocParams->flags;
        vaSize          = pNv0080AllocParams->vaSpaceSize;
        vaMode          = pNv0080AllocParams->vaMode;

        // valid only if NV_DEVICE_ALLOCATION_FLAGS_RESTRICT_RESERVED_VALIMITS is flagged.
        if (flags & NV_DEVICE_ALLOCATION_FLAGS_RESTRICT_RESERVED_VALIMITS)
        {
            vaStartInternal = pNv0080AllocParams->vaStartInternal;
            vaLimitInternal = pNv0080AllocParams->vaLimitInternal;

            if ((vaLimitInternal < vaStartInternal)  || (vaLimitInternal == 0))
            {
                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    // validate device instance
    if (gpumgrIsDeviceInstanceValid(deviceInst) != NV_OK)
    {
        return NV_ERR_INVALID_CLASS;
    }

    // Make sure this device has not been disabled
    if (gpumgrIsDeviceEnabled(deviceInst) == NV_FALSE)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // add new device to client and set the device context
    rmStatus = _deviceInit(pDevice, pCallContext, pParams->hClient, pParams->hResource, deviceInst,
                           hClientShare, hTargetClient, hTargetDevice, vaSize, vaStartInternal, vaLimitInternal,
                           flags, vaMode, &bIsFirstDevice);
    if (rmStatus != NV_OK)
        return rmStatus;

    pGpu = GPU_RES_GET_GPU(pDevice);

    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
    {
        if (!osIsGpuAccessible(pGpu))
        {
            // Delete the device from the client since we should not be allocating it
            _deviceTeardownRef(pDevice, pCallContext);
            _deviceTeardown(pDevice, pCallContext);
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }

    //
    // Make sure this device is not in fullchip reset on OSes where it is
    // restricted.
    //
    if (pOS->getProperty(pOS, PDB_PROP_OS_LIMIT_GPU_RESET) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET))
    {
        // Delete the device from the client since we should not be allocating it
        _deviceTeardownRef(pDevice, pCallContext);
        _deviceTeardown(pDevice, pCallContext);
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }

    {
        //
        // If using thwap to generate an allocation failure here, fail the alloc
        // right away
        //
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);
        if (pKernelRc != NULL &&
            !krcTestAllowAlloc(pGpu, pKernelRc,
                               NV_ROBUST_CHANNEL_ALLOCFAIL_DEVICE))
        {
            _deviceTeardownRef(pDevice, pCallContext);
            _deviceTeardown(pDevice, pCallContext);
            return NV_ERR_GENERIC;
        }
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        physicalAllocFlags = flags & ~(NV_DEVICE_ALLOCATION_FLAGS_PLUGIN_CONTEXT
            | NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE);

        NV_RM_RPC_ALLOC_SHARE_DEVICE(pGpu, pParams->hParent, pParams->hResource, pDevice->hClientShare,
                                     hTargetClient, hTargetDevice, deviceClass,
                                     physicalAllocFlags, vaSize, vaMode, bIsFirstDevice, rmStatus);
        if (rmStatus != NV_OK)
        {
            return rmStatus;
        }
    }

    return rmStatus;
} // end of deviceConstruct_IMPL

void
deviceDestruct_IMPL
(
    Device *pDevice
)
{
    CALL_CONTEXT           *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    NV_STATUS               rmStatus = NV_OK;
    NV_STATUS               tmpStatus;
    NvHandle                hClient;

    resGetFreeParams(staticCast(pDevice, RsResource), &pCallContext, &pParams);

    hClient = pCallContext->pClient->hClient;

    NV_PRINTF(LEVEL_INFO, "    type: device\n");

    LOCK_METER_DATA(FREE_DEVICE, 0, 0, 0);

    // free the device
    if (_deviceTeardownRef(pDevice, pCallContext) != NV_OK ||
        _deviceTeardown(pDevice, pCallContext) != NV_OK)
    {
        tmpStatus = NV_ERR_INVALID_OBJECT_HANDLE;
        if (tmpStatus != NV_OK && rmStatus == NV_OK)
            rmStatus = tmpStatus;
    }

    //
    // If the client was created, but never had any devices successfully
    // attached, we'll get here.  The client's device structure will have
    // been created, but pGpu will be NULL if the device was later found
    // to be non-existent
    //
    if (GPU_RES_GET_GPU(pDevice))
    {
        OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
        // vGpu support
        if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
        {
            RsResourceRef *pResourceRef = pCallContext->pResourceRef;
            NvHandle       hDevice = pResourceRef->hResource;
            NvBool         bClientInUse = NV_FALSE;
            RsClient      *pRsClient = pCallContext->pClient;
            NvBool         bNonOffloadVgpu = (IS_VIRTUAL(pGpu) && !IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu));
            RS_ITERATOR    it;

            if (rmStatus == NV_OK)
            {
                NV_RM_RPC_FREE(pGpu, hClient, hClient, hDevice, rmStatus);
            }

            if (rmStatus != NV_OK)
            {
                pParams->status = rmStatus;
                return;
            }

            // check if there are any more devices in use.
            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

            while (clientRefIterNext(it.pClient, &it))
            {
                Device *pDeviceTest = dynamicCast(it.pResourceRef->pResource, Device);
                NvBool bSameGpu = (GPU_RES_GET_GPU(pDeviceTest) == pGpu);

                if ((pDeviceTest != pDevice) && (bNonOffloadVgpu || bSameGpu))
                {
                    bClientInUse = NV_TRUE;
                    break;
                }
            }

            // check if there are any more KernelSMDebuggerSession in use.
            it = clientRefIter(pRsClient, NULL, classId(KernelSMDebuggerSession), RS_ITERATE_CHILDREN, NV_TRUE);

            while (clientRefIterNext(it.pClient, &it))
            {
                KernelSMDebuggerSession *pKernelSMDebuggerSession = dynamicCast(it.pResourceRef->pResource, KernelSMDebuggerSession);

                if (pKernelSMDebuggerSession != NULL &&
                    (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) || GPU_RES_GET_GPU(pKernelSMDebuggerSession) == pGpu))
                {
                    bClientInUse = NV_TRUE;
                    break;
                }
            }

            //  If neither any devices nor KernelSMDebuggerSession are in use, free up the client on host.
            if (!bClientInUse)
            {
                NV_RM_RPC_FREE(pGpu, hClient, NV01_NULL_OBJECT, hClient, rmStatus);
            }
        }
    }
} // end of deviceDestruct_IMPL

NV_STATUS
deviceControl_IMPL
(
    Device *pDevice,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{

    //
    // Some assertions to make RMCTRL to NVOC migration smooth
    // Those will be removed at the end of ctrl0080.def migration
    //
    NV_ASSERT_OR_RETURN(pParams->hClient == RES_GET_CLIENT_HANDLE(pDevice), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pParams->hObject == RES_GET_HANDLE(pDevice), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pParams->hParent == RES_GET_PARENT_HANDLE(pDevice), NV_ERR_INVALID_STATE);

    pParams->pGpuGrp      = GPU_RES_GET_GPUGRP(pDevice);
    return gpuresControl_IMPL(staticCast(pDevice, GpuResource),
            pCallContext, pParams);
}

NV_STATUS
deviceInternalControlForward_IMPL
(
    Device *pDevice,
    NvU32 command,
    void *pParams,
    NvU32 size
)
{
    return gpuresInternalControlForward_IMPL(staticCast(pDevice, GpuResource), command, pParams, size);
}

//
// add a device with specified handle, instance num, within a specified client
// (hClientShare also specified)
//
static NV_STATUS
_deviceInit
(
    Device  *pDevice,
    CALL_CONTEXT *pCallContext,
    NvHandle hClient,
    NvHandle hDevice,
    NvU32    deviceInst,
    NvHandle hClientShare,
    NvHandle hTargetClient,
    NvHandle hTargetDevice,
    NvU64    vaSize,
    NvU64    vaStartInternal,
    NvU64    vaLimitInternal,
    NvU32    allocFlags,
    NvU32    vaMode,
    NvBool  *pbIsFirstDevice
)
{
    OBJGPU      *pGpu;
    NV_STATUS    status;
    GpuResource *pGpuResource = staticCast(pDevice, GpuResource);
    Device      *pExistingDevice;
    NvU32        gpuInst;

    if (deviceInst >= NV_MAX_DEVICES)
        return NV_ERR_INVALID_ARGUMENT;

    // Look up GPU and GPU Group
    gpuInst = gpumgrGetPrimaryForDevice(deviceInst);

    if ((pGpu = gpumgrGetGpu(gpuInst)) == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    // Check if device inst already allocated, fail if this call succeeds.
    status = deviceGetByInstance(pCallContext->pClient, deviceInst, &pExistingDevice);
    if (status == NV_OK)
    {
        //
        // RS-TODO: Status code should be NV_ERR_STATE_IN_USE, however keeping
        // existing code from CliAllocElement (for now)
        //
        // Allow many Device objects on the same deviceInst in MIG mode.
        //
        if (!IS_MIG_ENABLED(pGpu) || IS_VIRTUAL(pGpu))
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    *pbIsFirstDevice = (status != NV_OK);

    pDevice->hTargetClient  = hTargetClient;
    pDevice->hTargetDevice  = hTargetDevice;
    pDevice->pKernelHostVgpuDevice = NULL;

    pDevice->deviceInst = deviceInst;

    // Update VA Mode
    pDevice->vaMode = vaMode;

    gpuresSetGpu(pGpuResource, pGpu, NV_TRUE);

    //
    // In case of a SR-IOV enabled guest we create a default client inside
    // the guest whose handle can be used for VAS sharing. Setting hClientShare
    // to 0 on baremetal causes any VA alloc made under this device to use the
    // global vaspace. We do not support use of the global vaspace inside guest.
    // The legacy paravirtualization config also makes use of a default client.
    // But, in the legacy case, the client is created by the plugin and not guest
    // RM . On SR-IOV, vaspace management has been pushed inside the guest. So,
    // having a vaspace only on the plugin side won't help since RmMapMemoryDma
    // calls will no longer be RPCed to host RM.
    //
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        if (hClientShare == NV01_NULL_OBJECT)
        {
            hClientShare = pGpu->hDefaultClientShare;
        }
    }

    status = deviceSetClientShare(pDevice, hClientShare, vaSize,
                                  vaStartInternal, vaLimitInternal, allocFlags);
    if (NV_OK != status)
        goto done;

    {
        OBJSYS      *pSys = SYS_GET_INSTANCE();
        GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
        RsClient *pRsClient = pCallContext->pClient;
        RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
        {
            // Try to start accounting for this procId/SubProcessId.
            // If gpuacctStartGpuAccounting() fails, just assert and print error.
            // gpuacctStartGpuAccounting() is not a major failure, we will continue with deviceInit() as normal.
            if ((pRsClient->type == CLIENT_TYPE_USER) && (gpuacctStartGpuAccounting(pGpuAcct,
                pGpu->gpuInstance, pClient->ProcID, pClient->SubProcessID) != NV_OK))
            {
                NV_ASSERT(0);
                NV_PRINTF(LEVEL_ERROR,
                          "gpuacctStartGpuAccounting() failed for procId : %d and SubProcessID : "
                          "%d. Ignoring the failure and continuing.\n",
                          pClient->ProcID, pClient->SubProcessID);
            }
        }
    }

    if (allocFlags & NV_DEVICE_ALLOCATION_FLAGS_PLUGIN_CONTEXT)
    {
        NV_ASSERT_OR_RETURN(allocFlags & NV_DEVICE_ALLOCATION_FLAGS_HOST_VGPU_DEVICE,
            NV_ERR_INVALID_ARGUMENT);
    }

done:
    if (status != NV_OK)
    {
        deviceRemoveFromClientShare(pDevice);
    }

    return status;
}

//
// delete a device with a specified handle within a client
//
static NV_STATUS
_deviceTeardown
(
    Device  *pDevice,
    CALL_CONTEXT *pCallContext
)
{
    OBJGPU    *pGpu     = GPU_RES_GET_GPU(pDevice);
    PORT_UNREFERENCED_VARIABLE(pGpu);

    deviceRemoveFromClientShare(pDevice);

    // DM-TODO: Force the client to move to Unicast...
    NV_STATUS status = deviceKPerfCudaLimitCliDisable(pDevice, pGpu);

    // Adding status check here, but not returning it as we do not want to
    // introduce any change in functionality.
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,"Disable of Cuda limit activation failed");
        DBG_BREAKPOINT();
    }

    {
        OBJSYS    *pSys = SYS_GET_INSTANCE();
        GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
        RsClient *pRsClient = pCallContext->pClient;
        RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);

        if ((pRsClient->type == CLIENT_TYPE_USER) &&
             pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
        {
            gpuacctStopGpuAccounting(pGpuAcct,
                pGpu->gpuInstance, pClient->ProcID, pClient->SubProcessID);
        }
    }

    return NV_OK;
}

static NV_STATUS _deviceTeardownRef
(
    Device *pDevice,
    CALL_CONTEXT *pCallContext
)
{

    return NV_OK;
}

NV_STATUS
deviceGetByHandle_IMPL
(
    RsClient         *pClient,
    NvHandle          hDevice,
    Device          **ppDevice
)
{
    RsResourceRef  *pResourceRef;
    NV_STATUS       status;

    *ppDevice = NULL;

    status = clientGetResourceRef(pClient, hDevice, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppDevice = dynamicCast(pResourceRef->pResource, Device);

    return (*ppDevice) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
deviceGetByInstance_IMPL
(
    RsClient         *pClient,
    NvU32             deviceInstance,
    Device          **ppDevice
)
{
    RS_ITERATOR  it;
    Device      *pDevice;

    *ppDevice = NULL;

    it = clientRefIter(pClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

    while (clientRefIterNext(it.pClient, &it))
    {
        pDevice = dynamicCast(it.pResourceRef->pResource, Device);

        if ((pDevice != NULL) && (deviceInstance == pDevice->deviceInst))
        {
            *ppDevice = pDevice;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
deviceGetByGpu_IMPL
(
    RsClient         *pClient,
    OBJGPU           *pGpu,
    NvBool            bAnyInGroup,
    Device          **ppDevice
)
{
    NvU32     deviceInstance = gpuGetDeviceInstance(pGpu);
    NV_STATUS status;

    status = deviceGetByInstance(pClient, deviceInstance, ppDevice);
    if (status != NV_OK)
        return status;

    // If pGpu is not the primary GPU return failure
    if (!bAnyInGroup && pGpu != GPU_RES_GET_GPU(*ppDevice))
    {
        *ppDevice = NULL;
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    return NV_OK;
}

// ****************************************************************************
//                            Deprecated Functions
// ****************************************************************************

/**
 * WARNING: This function is deprecated and use is *strongly* discouraged
 * (especially for new code!)
 *
 * From the function name (CliSetGpuContext) it appears as a simple accessor but
 * violates expectations by modifying the SLI BC threadstate (calls to
 * GPU_RES_SET_THREAD_BC_STATE). This can be dangerous if not carefully managed
 * by the caller.
 *
 * Instead of using this routine, please use deviceGetByHandle then call
 * GPU_RES_GET_GPU, GPU_RES_GET_GPUGRP, GPU_RES_SET_THREAD_BC_STATE as needed.
 *
 * Note that GPU_RES_GET_GPU supports returning a pGpu for both pDevice,
 * pSubdevice, the base pResource type, and any resource that inherits from
 * GpuResource. That is, instead of using CliSetGpuContext or
 * CliSetSubDeviceContext, please use following pattern to look up the pGpu:
 *
 * OBJGPU *pGpu = GPU_RES_GET_GPU(pResource or pResourceRef->pResource)
 *
 * To set the threadstate, please use:
 *
 * GPU_RES_SET_THREAD_BC_STATE(pResource or pResourceRef->pResource);
 */
NV_STATUS
CliSetGpuContext
(
    NvHandle    hClient,
    NvHandle    hDevice,
    OBJGPU    **ppGpu,
    OBJGPUGRP **ppGpuGrp
)
{
    Device    *pDevice;
    RsClient  *pClient;
    NV_STATUS  status;

    if (ppGpuGrp != NULL)
        *ppGpuGrp = NULL;

    if (ppGpu != NULL)
        *ppGpu = NULL;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return status;

    status = deviceGetByHandle(pClient, hDevice, &pDevice);
    if (status != NV_OK)
        return status;

    if (ppGpu != NULL)
        *ppGpu = GPU_RES_GET_GPU(pDevice);

    if (ppGpuGrp != NULL)
        *ppGpuGrp = GPU_RES_GET_GPUGRP(pDevice);

    GPU_RES_SET_THREAD_BC_STATE(pDevice);

    return NV_OK;
}

/**
 * WARNING: This function is deprecated! Please use gpuGetByRef()
 */
POBJGPU
CliGetGpuFromContext
(
    RsResourceRef *pContextRef,
    NvBool        *pbBroadcast
)
{
    NV_STATUS status;
    OBJGPU   *pGpu;

    status = gpuGetByRef(pContextRef, pbBroadcast, &pGpu);

    return (status == NV_OK) ? pGpu : NULL;
}

/**
 * WARNING: This function is deprecated! Please use gpuGetByHandle()
 */
POBJGPU
CliGetGpuFromHandle
(
    NvHandle hClient,
    NvHandle hResource,
    NvBool *pbBroadcast
)
{
    RsClient    *pClient;
    NV_STATUS    status;
    OBJGPU      *pGpu;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return NULL;

    status = gpuGetByHandle(pClient, hResource, pbBroadcast, &pGpu);

    return (status == NV_OK) ? pGpu : NULL;
}
