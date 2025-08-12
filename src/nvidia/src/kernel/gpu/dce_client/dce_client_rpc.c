/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvUnixVersion.h"

#include "gpu/dce_client/dce_client.h"
#include "os/dce_rm_client_ipc.h"

#include "os/os.h"

#include "kernel/core/thread_state.h"
#include "kernel/core/locks.h"

#include "vgpu/rpc.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "class/cl0073.h"
#include "class/clc670.h"
#include "class/clc67b.h"
#include "class/clc67d.h"
#include "class/clc67e.h"
#include "class/cl84a0.h"

#include <rmapi/alloc_size.h>

#include "gpu/disp/kern_disp.h"

#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

#define RPC_MESSAGE_STRUCTURES
#define RPC_MESSAGE_GENERIC_UNION
#include "g_rpc-message-header.h"
#undef RPC_MESSAGE_STRUCTURES
#undef RPC_MESSAGE_GENERIC_UNION

#define DCE_MAX_RPC_MSG_SIZE 4096

NV_STATUS
dceclientInitRpcInfra_IMPL
(
    OBJGPU *pGpu,
    DceClient *pDceClient
)
{
    NV_STATUS nvStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Init RPC Infra Called\n");

    pDceClient->pRpc = initRpcObject(pGpu);
    if (pDceClient->pRpc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "initRpcObject failed\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pDceClient->pRpc->maxRpcSize = DCE_MAX_RPC_MSG_SIZE;

    // Register Synchronous IPC client for RPC to DCE RM
    pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_SYNC] = 0;
    nvStatus = osTegraDceRegisterIpcClient(DCE_CLIENT_RM_IPC_TYPE_SYNC,
                                           NULL,
                                           &pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_SYNC]);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Register dce ipc client failed for DCE_CLIENT_RM_IPC_TYPE_SYNC error 0x%x\n",
                               nvStatus);
        goto ipc_register_fail;
    }

    NV_PRINTF(LEVEL_INFO, "Registered dce ipc client DCE_CLIENT_RM_IPC_TYPE_SYNC handle: 0x%x\n",
                           pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_SYNC]);

    // Register Asynchronous IPC client for event notification from DCE RM
    pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_EVENT] = 0;
    nvStatus = osTegraDceRegisterIpcClient(DCE_CLIENT_RM_IPC_TYPE_EVENT,
                                           pGpu,
                                           &pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_EVENT]);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Register dce ipc client failed for DCE_CLIENT_RM_IPC_TYPE_EVENT error 0x%x\n",
                               nvStatus);
        goto ipc_register_fail;
    }
    NV_PRINTF(LEVEL_INFO, "Register dce ipc client DCE_CLIENT_RM_IPC_TYPE_EVENT: 0x%x\n",
                           pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_EVENT]);

ipc_register_fail:
    if (nvStatus != NV_OK)
    {
        dceclientDeinitRpcInfra(pDceClient);
    }

    return nvStatus;
}

NV_STATUS
dceclientDceRmInit_IMPL
(
    OBJGPU *pGpu,
    DceClient *pDceClient,
    NvBool bInit
)
{
    NV_STATUS nvStatus = NV_OK;
    GPU_MASK  gpusLockedMask = 0;

    if (bInit)
    {
        NV_ASSERT_OK_OR_GOTO(nvStatus,
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                                  GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT, &gpusLockedMask),
            done);
    }

    NV_RM_RPC_DCE_RM_INIT(pGpu, bInit, nvStatus);

done:
    if (gpusLockedMask != 0)
    {
        rmGpuGroupLockRelease(gpusLockedMask, GPUS_LOCK_FLAGS_NONE);
    }

    return nvStatus;
}

void
dceclientDeinitRpcInfra_IMPL
(
    DceClient *pDceClient
)
{
    NvU32 i = 0;

    NV_PRINTF(LEVEL_INFO, "Free RPC Infra Called\n");

    for (i = 0; i < NV_ARRAY_ELEMENTS(pDceClient->clientId); i++)
    {
        osTegraDceUnregisterIpcClient(pDceClient->clientId[i]);
    }

    portMemFree(pDceClient->pRpc);
    pDceClient->pRpc = NULL;
}

NV_STATUS
dceclientSendRpc_IMPL
(
    DceClient *pDceClient,
    void *msgData,
    NvU32 msgLength
)
{
    NV_STATUS nvStatus = NV_OK;
    NvU32 clientId = pDceClient->clientId[DCE_CLIENT_RM_IPC_TYPE_SYNC];

    NV_PRINTF(LEVEL_INFO, "Send RPC Called, clientid used 0x%x\n", clientId);

    if (msgData)
    {
        nvStatus = osTegraDceClientIpcSendRecv(clientId, msgData, msgLength);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Send RPC failed for clientId %u error %u\n", clientId, nvStatus);
            return nvStatus;
        }
    }

    return nvStatus;
}

NV_STATUS
dceclientReceiveMsg_IMPL
(
    DceClient *pDceClient
)
{
    NV_STATUS nvStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Receive Message Called\n");

    return nvStatus;
}

NV_STATUS
dceclientSendMsg_IMPL
(
    DceClient *pDceClient
)
{
    NV_STATUS nvStatus = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Send Message Called\n");

    return nvStatus;
}

static inline rpc_message_header_v *_dceRpcGetMessageHeader(OBJRPC *pRpc)
{
    return ((rpc_message_header_v*)(pRpc->message_buffer));
}

static inline rpc_generic_union *_dceRpcGetMessageData(OBJRPC *pRpc)
{
    return _dceRpcGetMessageHeader(pRpc)->rpc_message_data;
}

static inline NV_STATUS _dceRpcGetRpcResult(OBJRPC *pRpc)
{
    return _dceRpcGetMessageHeader(pRpc)->rpc_result;
}

/**
 * Prints the header info when _INFO level is enabled.
 */
static void _dceclientrmPrintHdr
(
    OBJRPC *pRpc
)
{
    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE : [msg-buf:0x%p] header_version = 0x%x signature = 0x%x "
              "length = 0x%x function = 0x%x rpc_result = 0x%x\n", pRpc->message_buffer,
              _dceRpcGetMessageHeader(pRpc)->header_version, _dceRpcGetMessageHeader(pRpc)->signature,
              _dceRpcGetMessageHeader(pRpc)->length, _dceRpcGetMessageHeader(pRpc)->function,
              _dceRpcGetMessageHeader(pRpc)->rpc_result);
}

/**
 * Allocate memory for rpc message
 * TODO : Change static allocation of 4K to
 * a better dynamic allocation
 */
static NV_STATUS _dceRpcAllocateMemory
(
    OBJRPC *pRpc
)
{
    NvU32 *message_buffer;

    NV_ASSERT_OR_RETURN(pRpc != NULL, NV_ERR_INVALID_ARGUMENT);
    message_buffer = portMemAllocNonPaged(DCE_MAX_RPC_MSG_SIZE);
    if (message_buffer == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Cannot allocate memory for message_buffer\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pRpc->message_buffer = message_buffer;

    return NV_OK;
}

static void _dceRpcFreeMemory
(
    OBJRPC *pRpc
)
{
    portMemFree(pRpc->message_buffer);
    pRpc->message_buffer = NULL;
}

/**
 * Send RPC msg and check the result.
 */
static NV_STATUS
_dceRpcIssueAndWait
(
    RM_API *pRmApi
)
{
    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    NV_STATUS status = NV_ERR_INVALID_ARGUMENT;
    rpc_message_header_v* message_header = NULL;
    DceClient *pDceclientrm = GPU_GET_DCECLIENTRM(pGpu);

    message_header = _dceRpcGetMessageHeader(pRpc);
    if (message_header)
    {
        _dceclientrmPrintHdr(pRpc);

        status = dceclientSendRpc(pDceclientrm, message_header, message_header->length);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Error while issuing RPC [0x%x]\n", status);
            goto done;
        }
    }

done:
    return status;
}

void dceclientHandleAsyncRpcCallback
(
    NvU32 handle,
    NvU32 interfaceType,
    NvU32 msgLength,
    void *data,
    void *usrCtx
)
{
    NV_PRINTF(LEVEL_INFO, "dceclientHandleAsyncRpcCallback called\n");

    rpc_message_header_v *msg_hdr = NULL;
    rpc_generic_union *rpc_msg_data = NULL;
    OBJGPU *pGpu = (OBJGPU *)usrCtx;

    NV_ASSERT_OR_RETURN_VOID(interfaceType == DCE_CLIENT_RM_IPC_TYPE_EVENT);
    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL && data != NULL);

    msg_hdr = (rpc_message_header_v *)data;
    rpc_msg_data = msg_hdr->rpc_message_data;

    switch (msg_hdr->function)
    {
        case NV_VGPU_MSG_EVENT_POST_EVENT:
        {
            rpc_post_event_v *rpc_params = &rpc_msg_data->post_event_v;

            if (rpc_params->bNotifyList)
            {
                gpuNotifySubDeviceEvent(pGpu, rpc_params->notifyIndex,
                                        rpc_params->eventData,
                                        rpc_params->eventDataSize, 0, 0);
            }
            else
            {
                PEVENTNOTIFICATION pNotifyList  = NULL;
                PEVENTNOTIFICATION pNotifyEvent = NULL;
                Event             *pEvent       = NULL;
                NV_STATUS          nvStatus     = NV_OK;

                // Get the notification list that contains this event.
                NV_ASSERT_OR_RETURN_VOID(CliGetEventInfo(rpc_params->hClient,
                          rpc_params->hEvent, &pEvent));

                if (pEvent->pNotifierShare != NULL)
                    pNotifyList = pEvent->pNotifierShare->pEventList;

                NV_ASSERT_OR_RETURN_VOID(pNotifyList != NULL);

                // Send event to a specific hEvent.  Find hEvent in the notification list.
                for (pNotifyEvent = pNotifyList; pNotifyEvent != NULL; pNotifyEvent = pNotifyEvent->Next)
                {
                    if (pNotifyEvent->hEvent == rpc_params->hEvent)
                    {
                        nvStatus = osNotifyEvent(pGpu, pNotifyEvent, 0,
                                   rpc_params->data, rpc_params->status);
                        if (nvStatus != NV_OK)
                            NV_PRINTF(LEVEL_ERROR, "osNotifyEvent failed with status: %x\n",nvStatus);
                        break;
                    }
                }
                NV_ASSERT(pNotifyEvent != NULL);
            }
            return;
        }
        case NV_VGPU_MSG_EVENT_RG_LINE_INTR:
        {
            rpc_rg_line_intr_v *rpc_params = &rpc_msg_data->rg_line_intr_v;

            KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pKernelDisplay != NULL);

            kdispInvokeRgLineCallback(pKernelDisplay, rpc_params->head, rpc_params->rgIntr, NV_FALSE);
            return;
        }
        case NV_VGPU_MSG_EVENT_DISPLAY_MODESET:
        {
            rpc_display_modeset_v *rpc_params = &rpc_msg_data->display_modeset_v;

            KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
            NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pKernelDisplay != NULL);

            kdispInvokeDisplayModesetCallback(pKernelDisplay,
                                              rpc_params->bModesetStart,
                                              rpc_params->minRequiredIsoBandwidthKBPS,
                                              rpc_params->minRequiredFloorBandwidthKBPS);
            return;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unexpected RPC function 0x%x\n", msg_hdr->function);
            NV_ASSERT_FAILED("Unexpected RPC function");
            return;
        }
    }
}

NV_STATUS rpcRmApiControl_dce
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject,
    NvU32 cmd,
    void *pParamStructPtr,
    NvU32 paramsSize
)
{
    OBJGPU *pGpu = NULL;
    OBJRPC *pRpc = NULL;

    rpc_generic_union *msg_data;
    rpc_gsp_rm_control_v *rpc_params = NULL;
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE : Prepare and send RmApiControl RPC [cmd:0x%x]\n", cmd);

    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    pRpc = GPU_GET_RPC(pGpu);
    NV_ASSERT_OK_OR_RETURN(_dceRpcAllocateMemory(pRpc));

    msg_data = _dceRpcGetMessageData(pRpc);
    rpc_params = &msg_data->gsp_rm_control_v;

    status = rpcWriteCommonHeader(pGpu, pRpc,
                                  NV_VGPU_MSG_FUNCTION_GSP_RM_CONTROL,
                                  (sizeof(rpc_gsp_rm_control_v) +
                                   paramsSize));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Writing RPC Header Failed [0x%x]\n", status);
        goto done;
    }

    rpc_params->hClient    = hClient;
    rpc_params->hObject    = hObject;
    rpc_params->cmd        = cmd;
    rpc_params->paramsSize = paramsSize;
    portMemCopy(rpc_params->params, paramsSize,pParamStructPtr, paramsSize);

    status = _dceRpcIssueAndWait(pRmApi);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _dceRpcGetRpcResult(pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Failed RM ctrl call cmd:0x%x result 0x%x: %s\n",
                  cmd, status, nvstatusToString(status));
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE: RPC for GSP RM Control Successful\n");
    }

    portMemCopy(pParamStructPtr, paramsSize, rpc_params->params, paramsSize);

done:
    if (pRpc->message_buffer)
        _dceRpcFreeMemory(pRpc);
    return status;
}

NV_STATUS rpcRmApiAlloc_dce
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle hObject,
    NvU32 hClass,
    void *pAllocParams,
    NvU32 allocParamSize
)
{
    OBJGPU *pGpu = NULL;
    OBJRPC *pRpc = NULL;

    rpc_generic_union *msg_data;
    rpc_gsp_rm_alloc_v *rpc_params;
    NV_STATUS status;
    NvU32 paramsSize;
    NvBool bNullAllowed;

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE: Prepare and send RmApiAlloc RPC\n");

    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    pRpc = GPU_GET_RPC(pGpu);
    NV_ASSERT_OK_OR_RETURN(_dceRpcAllocateMemory(pRpc));

    msg_data = _dceRpcGetMessageData(pRpc);
    rpc_params = &msg_data->gsp_rm_alloc_v;

    NV_ASSERT_OK_OR_GOTO(status,
                         rmapiGetClassAllocParamSize(&paramsSize, pAllocParams, &bNullAllowed, hClass),
                         done);

    if (pAllocParams == NULL && !bNullAllowed)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: NULL allocation params not allowed for class 0x%x\n", hClass);
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc,
                                  NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC,
                                  (sizeof(rpc_gsp_rm_alloc_v) + paramsSize));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Writing RPC Header Failed [0x%x]\n", status);
        goto done;
    }

    rpc_params->hClient    = hClient;
    rpc_params->hParent    = hParent;
    rpc_params->hObject    = hObject;
    rpc_params->hClass     = hClass;
    rpc_params->paramsSize = pAllocParams ? paramsSize : 0;
    portMemCopy(rpc_params->params, rpc_params->paramsSize, pAllocParams, paramsSize);

    status = _dceRpcIssueAndWait(pRmApi);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _dceRpcGetRpcResult(pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Failed RM Alloc Object 0x%x result 0x%x: %s\n",
                  hClass, status, nvstatusToString(status));
    }

    // Deserialize the response
    // We do not deserialize the variable length data as we do not expect it to be modified
    if (paramsSize > 0)
    {
        portMemCopy(pAllocParams, paramsSize, rpc_params->params, rpc_params->paramsSize);
    }

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE: RPC for GSP RM Alloc Successful\n");

done:
    if (pRpc->message_buffer)
        _dceRpcFreeMemory(pRpc);
    return status;
}

NV_STATUS rpcRmApiFree_dce(RM_API *pRmApi, NvHandle hClient, NvHandle hObject)
{
    OBJGPU *pGpu = NULL;
    OBJRPC *pRpc = NULL;

    rpc_generic_union *msg_data;
    NVOS00_PARAMETERS_v *rpc_params;
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE Free "
              "RPC Called for hClient: 0x%x\n", hClient);

    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    pRpc = GPU_GET_RPC(pGpu);
    NV_ASSERT_OK_OR_RETURN(_dceRpcAllocateMemory(pRpc));

    msg_data = _dceRpcGetMessageData(pRpc);
    rpc_params = &msg_data->free_v.params;

    status = rpcWriteCommonHeader(pGpu, pRpc,
                                  NV_VGPU_MSG_FUNCTION_FREE,
                                  sizeof(rpc_free_v));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Writing RPC Header Failed [0x%x]\n", status);
        goto done;
    }

    rpc_params->hRoot = hClient;
    rpc_params->hObjectParent = NV01_NULL_OBJECT;
    rpc_params->hObjectOld = hObject;

    status = _dceRpcIssueAndWait(pRmApi);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _dceRpcGetRpcResult(pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Failed RM Free Object result 0x%x:\n", status);
    }

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE: RPC for Free Successful\n");

done:
    if (pRpc->message_buffer)
        _dceRpcFreeMemory(pRpc);

    return status;
}

NV_STATUS rpcRmApiDupObject_dce
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle *phObject,
    NvHandle hClientSrc,
    NvHandle hObjectSrc,
    NvU32 flags
)
{
    OBJGPU *pGpu = NULL;
    OBJRPC *pRpc = NULL;

    rpc_generic_union *msg_data;
    NVOS55_PARAMETERS_v03_00 *rpc_params = NULL;
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE Dup Object "
              "RPC Called for hClient: 0x%x\n", hClient);


    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    pRpc = GPU_GET_RPC(pGpu);
    NV_ASSERT_OK_OR_RETURN(_dceRpcAllocateMemory(pRpc));

    msg_data = _dceRpcGetMessageData(pRpc);
    rpc_params = &msg_data->dup_object_v.params;

    status = rpcWriteCommonHeader(pGpu, pRpc,
                                  NV_VGPU_MSG_FUNCTION_DUP_OBJECT,
                                  sizeof(rpc_dup_object_v));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Writing RPC Header Failed [0x%x]\n", status);
        goto done;
    }

    rpc_params->hClient     = hClient;
    rpc_params->hParent     = hParent;
    rpc_params->hObject     = *phObject;
    rpc_params->hClientSrc  = hClientSrc;
    rpc_params->hObjectSrc  = hObjectSrc;
    rpc_params->flags       = flags;

    status = _dceRpcIssueAndWait(pRmApi);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _dceRpcGetRpcResult(pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Failed RM Dup Object result 0x%x:\n", status);
    }

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE: RPC for DUP OBJECT Successful\n");

done:
    portMemFree(pRpc->message_buffer);
    return status;
}

NV_STATUS
rpcDceRmInit_dce
(
    RM_API *pRmApi,
    NvBool bInit
)
{
    OBJGPU *pGpu = NULL;
    OBJRPC *pRpc = NULL;
    DceClient *pDceClientrm = NULL;

    rpc_generic_union *msg_data;
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    rpc_dce_rm_init_v *rpc_params = NULL;

    NV_PRINTF(LEVEL_INFO, "NVRM_RPC_DCE RPC to trigger %s called\n",
              bInit ? "RmInitAdapter" : "RmShutdownAdapter");

    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_STATE);
    pRpc = GPU_GET_RPC(pGpu);
    pDceClientrm = GPU_GET_DCECLIENTRM(pGpu);
    NV_ASSERT_OK_OR_RETURN(_dceRpcAllocateMemory(pRpc));

    msg_data = _dceRpcGetMessageData(pRpc);
    rpc_params = &msg_data->dce_rm_init_v;

    status = rpcWriteCommonHeader(pGpu, pRpc,
                                  NV_VGPU_MSG_FUNCTION_DCE_RM_INIT,
                                  sizeof(rpc_dce_rm_init_v));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Writing RPC Header Failed [0x%x]\n", status);
        goto done;
    }

    rpc_params->bInit     = bInit;
    status = _dceRpcIssueAndWait(pRmApi);
    if (status != NV_OK)
    {
        goto done;
    }

    status = _dceRpcGetRpcResult(pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC_DCE: Failed RM init/deinit result 0x%x:\n", status);
    }

    pDceClientrm->hInternalClient = rpc_params->hInternalClient;

    if (bInit)
    {
        NV0000_CTRL_SYSTEM_GET_BUILD_VERSION_V2_PARAMS *pGbvParams = portMemAllocNonPaged(sizeof(*pGbvParams));

        NV_ASSERT_TRUE_OR_GOTO(status, pGbvParams != NULL, NV_ERR_NO_MEMORY, done);

        status = rpcRmApiControl_dce(pRmApi, pDceClientrm->hInternalClient, pDceClientrm->hInternalClient,
                            NV0000_CTRL_CMD_SYSTEM_GET_BUILD_VERSION_V2, pGbvParams, sizeof(*pGbvParams));
        NV_ASSERT_OK(status);

        if (status == NV_OK &&
            portStringCompare(NV_VERSION_STRING, pGbvParams->driverVersionBuffer, sizeof(NV_VERSION_STRING)) != 0)
        {
            NV_PRINTF(LEVEL_WARNING, "Possibly incompatible DCE RM version! CPU RM: %s DCE RM: %s\n",
                      NV_VERSION_STRING, pGbvParams->driverVersionBuffer);
        }

        portMemFree(pGbvParams);
    }

done:
    portMemFree(pRpc->message_buffer);
    return status;
}
