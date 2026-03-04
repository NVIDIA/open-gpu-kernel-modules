/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/deferred_api.h"

#include "kernel/core/locks.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/mem_mgr/vaspace.h"
#include "kernel/rmapi/control.h"
#include "kernel/rmapi/rs_utils.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "kernel/gpu/fifo/kernel_channel.h"

#include "class/cl5080.h"

#include "ctrl/ctrl2080.h"

#include "libraries/resserv/rs_server.h"
#include "vgpu/rpc.h"


static NV_STATUS _Class5080DelDeferredApi(DeferredApiObject *pDeferredApiObject,
                                          NvHandle           hDeferredApi);

static NV_STATUS _class5080DeferredApiV2(OBJGPU            *pGpu,
                                         ChannelDescendant *Object,
                                         NvU32              Offset,
                                         NvU32              Data);

static NV_STATUS
_Class5080UpdateTLBFlushState(DeferredApiObject *pDeferredApiObject);

static NV_STATUS
_Class5080GetDeferredApiInfo(DeferredApiObject  *pDeferredApiObject,
                             NvHandle            hDeferredApi,
                             DEFERRED_API_INFO **ppCliDeferredApi);

static NV_STATUS
_Class5080AddDeferredApi
(
    DeferredApiObject *pDeferredApiObject,
    NvHandle hDeferredApi,
    void *pDeferredApi,
    NvU32 size
)
{
    NV_STATUS          rmStatus = NV_OK;
    DEFERRED_API_INFO *pCliDeferredApi;
    CALL_CONTEXT      *pCallContext = resservGetTlsCallContext();
    NvHandle           hClient = RES_GET_CLIENT_HANDLE(pDeferredApiObject);

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    // validate handle
    if (!serverutilValidateNewResourceHandle(hClient, hDeferredApi))
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    rmStatus = _Class5080GetDeferredApiInfo(pDeferredApiObject, hDeferredApi, &pCliDeferredApi);

    // Object already exists
    if (NV_OK == rmStatus)
    {
        return NV_ERR_INVALID_OBJECT_HANDLE;
    }

    // allocate a new Deferred Api and add to the client
    pCliDeferredApi = portMemAllocNonPaged(sizeof(DEFERRED_API_INFO) + size);
    if (NULL != pCliDeferredApi)
    {
        // initialize the entry
        pCliDeferredApi->Handle      = hDeferredApi;
        pCliDeferredApi->pDeferredApiInfo = (NvU8 *)pCliDeferredApi + sizeof(DEFERRED_API_INFO);
        pCliDeferredApi->Node.keyStart = pCliDeferredApi->Handle;
        pCliDeferredApi->Node.keyEnd = pCliDeferredApi->Handle;
        pCliDeferredApi->Node.Data   = pCliDeferredApi;
        pCliDeferredApi->Flags       = 0;
        pCliDeferredApi->privLevel   = pCallContext->secInfo.privLevel;
        portMemCopy(pCliDeferredApi->pDeferredApiInfo, size, pDeferredApi, size);

        // link in the new entry
        rmStatus = btreeInsert(&pCliDeferredApi->Node, &pDeferredApiObject->DeferredApiList);

        if (rmStatus != NV_OK)
        {
            portMemFree(pCliDeferredApi);
            pCliDeferredApi = NULL;
        }
    }
    else
        return NV_ERR_NO_MEMORY;

    return rmStatus;
}


static NV_STATUS
_Class5080GetDeferredApiInfo
(
    DeferredApiObject  *pDeferredApiObject,
    NvHandle            hDeferredApi,
    DEFERRED_API_INFO **ppCliDeferredApi
)
{
    NODE *pNode;

    if (btreeSearch(hDeferredApi, &pNode, pDeferredApiObject->DeferredApiList) == NV_OK)
    {
        *ppCliDeferredApi = pNode->Data;
        return NV_OK;
    }

    return NV_ERR_INVALID_DATA;

}


static NV_STATUS _Class5080DelDeferredApi
(
    DeferredApiObject *pDeferredApiObject,
    NvHandle           hDeferredApi
)
{
    DEFERRED_API_INFO *pDeferredApi = NULL;
    NV_STATUS          status;
    NODE              *pNode;

    // remove the event from the client database
    if (NV_OK == _Class5080GetDeferredApiInfo(pDeferredApiObject,
                                              hDeferredApi, &pDeferredApi))
    {
        status = btreeSearch(hDeferredApi, &pNode, pDeferredApiObject->DeferredApiList);
        if (status != NV_OK)
            return NV_ERR_GENERIC;

        status = btreeUnlink(pNode, &pDeferredApiObject->DeferredApiList);
        if (status == NV_OK)
        {
            NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApiParams;
            pDeferredApiParams = (NV5080_CTRL_DEFERRED_API_PARAMS *)pDeferredApi->pDeferredApiInfo;

            if (DRF_VAL(5080_CTRL, _CMD_DEFERRED_API, _FLAGS_WAIT_FOR_TLB_FLUSH, pDeferredApiParams->flags) ==
                  NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH_TRUE)
            {
                // decrement count, if API was waiting on a TLB flush, but never saw one
                if ((pDeferredApi->Flags & DEFERRED_API_INFO_FLAGS_HAS_EXECUTED) &&
                    !(pDeferredApi->Flags & DEFERRED_API_INFO_FLAGS_HAS_TLB_FLUSHED))
                {
                    pDeferredApiObject->NumWaitingOnTLBFlush--;
                }
            }

            // free the list element
            portMemFree(pDeferredApi);
            return NV_OK;
        }
    }

    return NV_ERR_GENERIC;
}


static NV_STATUS _Class5080UpdateTLBFlushState
(
    DeferredApiObject *pDeferredApiObject
)
{
    NODE                            *pNode;
    DEFERRED_API_INFO               *pCliDeferredApi;
    NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApi;

    btreeEnumStart(0, &pNode, pDeferredApiObject->DeferredApiList);
    while (pNode &&
           pDeferredApiObject->NumWaitingOnTLBFlush)
    {
        pCliDeferredApi = pNode->Data;
        pDeferredApi    = (NV5080_CTRL_DEFERRED_API_PARAMS *) pCliDeferredApi->pDeferredApiInfo;

        // update any APIs with WAIT_FOR_TLB_FLUSH set
        if (DRF_VAL(5080_CTRL, _CMD_DEFERRED_API, _FLAGS_WAIT_FOR_TLB_FLUSH, pDeferredApi->flags) ==
            NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH_TRUE)
        {
            // check if API has EXECUTED and newly TLB_FLUSHED
            if ((pCliDeferredApi->Flags & DEFERRED_API_INFO_FLAGS_HAS_EXECUTED) &&
                !(pCliDeferredApi->Flags & DEFERRED_API_INFO_FLAGS_HAS_TLB_FLUSHED))
            {
                pCliDeferredApi->Flags |= DEFERRED_API_INFO_FLAGS_HAS_TLB_FLUSHED;
                pDeferredApiObject->NumWaitingOnTLBFlush--;

                btreeEnumNext(&pNode, pDeferredApiObject->DeferredApiList);

                // check if API can now be implicitly deleted
                if (DRF_VAL(5080_CTRL, _CMD_DEFERRED_API, _FLAGS_DELETE, pDeferredApi->flags) ==
                    NV5080_CTRL_CMD_DEFERRED_API_FLAGS_DELETE_IMPLICIT)
                {
                    _Class5080DelDeferredApi(pDeferredApiObject, pCliDeferredApi->Handle);
                }
                continue;
            }
        }
        btreeEnumNext(&pNode, pDeferredApiObject->DeferredApiList);
    }

    return NV_OK;
}


//---------------------------------------------------------------------------
//
//  Class object creation and destruction
//
//---------------------------------------------------------------------------

NV_STATUS
defapiConstruct_IMPL
(
    DeferredApiObject            *pDeferredApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    if (pParams->pAllocParams != NULL)
    {
        NV5080_ALLOC_PARAMS *pAllocParams = pParams->pAllocParams;
        if (pAllocParams->notifyCompletion)
        {
            staticCast(pDeferredApi, ChannelDescendant)->notifyAction  = NV_OS_WRITE_THEN_AWAKEN;
            staticCast(pDeferredApi, ChannelDescendant)->bNotifyTrigger = NV_TRUE;
        }
    }

    return NV_OK;
}

void
defapiDestruct_IMPL
(
    DeferredApiObject *pDeferredApi
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pDeferredApi, ChannelDescendant);
    NODE              *pNode;
    DEFERRED_API_INFO *pCliDeferredApi;

    chandesIsolateOnDestruct(pChannelDescendant);

    // Free All Outstanding API on the btree
    btreeEnumStart(0, &pNode, pDeferredApi->DeferredApiList);
    while (pNode)
    {
        pCliDeferredApi = pNode->Data;

        btreeEnumNext(&pNode, pDeferredApi->DeferredApiList);
        _Class5080DelDeferredApi(pDeferredApi, pCliDeferredApi->Handle);
    }
}

NV_STATUS
defapiCtrlCmdDeferredApi_IMPL
(
    DeferredApiObject *pDeferredApiObj,
    NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApi
)
{
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pDeferredApiObj);
    NV_STATUS status = NV_OK;

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, there is nothing to do at this point in the
    // guest OS (where IS_VIRTUAL(pGpu) is true).
    //
    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_DEFERRED_API_CONTROL(pGpu,
                                       RES_GET_CLIENT_HANDLE(pDeferredApiObj),
                                       RES_GET_PARENT_HANDLE(pDeferredApiObj),
                                       RES_GET_HANDLE(pDeferredApiObj),
                                       pDeferredApi,
                                       sizeof(*pDeferredApi),
                                       status);
        return status;
    }

    return _Class5080AddDeferredApi(pDeferredApiObj,
                                    pDeferredApi->hApiHandle,
                                    pDeferredApi,
                                    sizeof(*pDeferredApi));
}

NV_STATUS
defapiCtrlCmdDeferredApiV2_IMPL
(
    DeferredApiObject                  *pDeferredApiObj,
    NV5080_CTRL_DEFERRED_API_V2_PARAMS *pDeferredApi
)
{
    NV_STATUS status = NV_OK;
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pDeferredApiObj);
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, there is nothing to do at this point in the
    // guest OS (where IS_VIRTUAL(pGpu) is true).
    //
    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_DEFERRED_API_CONTROL(pGpu,
                                       RES_GET_CLIENT_HANDLE(pDeferredApiObj),
                                       RES_GET_PARENT_HANDLE(pDeferredApiObj),
                                       RES_GET_HANDLE(pDeferredApiObj),
                                       pDeferredApi,
                                       sizeof(*pDeferredApi),
                                       status);
        return status;
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        switch (pDeferredApi->cmd)
        {
            case NV2080_CTRL_CMD_FIFO_UPDATE_CHANNEL_INFO:
            {
                RsClient *pClient = NULL;
                RsResourceRef *pResourceRef = NULL;
                NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS *pParams =
                    &pDeferredApi->api_bundle.ChannelInfoUpdate;

                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    serverGetClientUnderLock(&g_resServ, pParams->hClient, &pClient));

                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    clientGetResourceRef(pClient, pParams->hUserdMemory, &pResourceRef));

                NV_CHECK_OR_RETURN(LEVEL_ERROR,
                                   pResourceRef->pParentRef != NULL,
                                   NV_ERR_INVALID_OBJECT_PARENT);

                //
                // Register the client-specified USERD hMemory with GSP-RM before
                // calling physical RM.
                //
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                    memRegisterWithGsp(pGpu, pClient,
                                       pResourceRef->pParentRef->hResource,
                                       pResourceRef->hResource));

                break;
            }
            default:
                // Nothing to do in kernel RM; just forward to physical RM.
                break;
        }
    }

    //
    // NV5080_CTRL_DEFERRED_API_V2_PARAMS and
    // NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS are equivalent, so just pass
    // pDeferredApi through.
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        RES_GET_CLIENT_HANDLE(pDeferredApiObj),
                        RES_GET_HANDLE(pDeferredApiObj),
                        NV5080_CTRL_CMD_DEFERRED_API_INTERNAL,
                        pDeferredApi,
                        sizeof(*pDeferredApi)));

    return NV_OK;
}

NV_STATUS
defapiCtrlCmdDeferredApiInternal_IMPL
(
    DeferredApiObject                  *pDeferredApiObj,
    NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS *pDeferredApi
)
{
    return _Class5080AddDeferredApi(pDeferredApiObj,
                                    pDeferredApi->hApiHandle,
                                    pDeferredApi,
                                    sizeof(*pDeferredApi));
}

NV_STATUS
defapiCtrlCmdRemoveApi_IMPL
(
    DeferredApiObject             *pDeferredApiObj,
    NV5080_CTRL_REMOVE_API_PARAMS *pRemoveApi
)
{
    NV_STATUS status = NV_OK;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pDeferredApiObj);

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, there is nothing to do at this point in the
    // guest OS (where IS_VIRTUAL(pGpu) is true).
    //
    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_REMOVE_DEFERRED_API(pGpu,
                                      RES_GET_CLIENT_HANDLE(pDeferredApiObj),
                                      RES_GET_PARENT_HANDLE(pDeferredApiObj),
                                      RES_GET_HANDLE(pDeferredApiObj),
                                      pRemoveApi->hApiHandle, status);
        return status;
    }

    return _Class5080DelDeferredApi(pDeferredApiObj,
                                    pRemoveApi->hApiHandle);
}

static NV_STATUS
_class5080DeferredApiV2
(
    OBJGPU *pGpu,
    ChannelDescendant *Object,
    NvU32 Offset,
    NvU32 Data
)
{
    DeferredApiObject   *pDeferredApiObject = dynamicCast(Object, DeferredApiObject);
    DEFERRED_API_INFO   *pCliDeferredApi    = NULL;
    NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApi;
    NV_STATUS            rmStatus           = NV_OK;
    NvU32                paramSize          = 0;
    NvBool               bIsCtrlCall        = NV_TRUE;

    rmStatus = _Class5080GetDeferredApiInfo(pDeferredApiObject,
                                            Data, &pCliDeferredApi);
    if (rmStatus == NV_OK)
    {
        pDeferredApi = pCliDeferredApi->pDeferredApiInfo;

        switch (pDeferredApi->cmd)
        {
        case NV2080_CTRL_CMD_GPU_INITIALIZE_CTX:
            paramSize = sizeof(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS);
            break;

        case NV2080_CTRL_CMD_GPU_PROMOTE_CTX:
            paramSize = sizeof(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS);
            break;

        case NV2080_CTRL_CMD_GPU_EVICT_CTX:
            paramSize = sizeof(NV2080_CTRL_GPU_EVICT_CTX_PARAMS);
            break;

        case NV2080_CTRL_CMD_FIFO_UPDATE_CHANNEL_INFO:
            paramSize = sizeof(NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS);
            break;

        case NV2080_CTRL_CMD_DMA_INVALIDATE_TLB:
        {
            OBJGPU *pTgtGpu;
            RsClient *pClientVA;
            Subdevice *pSubdevice;

            bIsCtrlCall = NV_FALSE;

            rmStatus = serverGetClientUnderLock(&g_resServ, pDeferredApi->hClientVA,
                    &pClientVA);
            if (rmStatus != NV_OK)
                break;

            rmStatus = subdeviceGetByHandle(pClientVA, pDeferredApi->hDeviceVA,
                    &pSubdevice);

            if (rmStatus != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to find target gpu from hClient(%x), hDevice(%x)\n",
                          pDeferredApi->hClientVA, pDeferredApi->hDeviceVA);
            }
            else
            {
                NvHandle hDevice;
                OBJVASPACE *pVAS = NULL;

                // Fetch target GPU and set threadstate
                pTgtGpu = GPU_RES_GET_GPU(pSubdevice);

                hDevice = RES_GET_HANDLE(pSubdevice->pDevice);

                GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

                rmStatus = vaspaceGetByHandleOrDeviceDefault(pClientVA,
                                                             hDevice,
                                                             pDeferredApi->api_bundle.InvalidateTlb.hVASpace,
                                                             &pVAS);
                if (NV_OK == rmStatus)
                {
                    vaspaceInvalidateTlb(pVAS, pTgtGpu, PTE_UPGRADE);

                    if (pDeferredApiObject->NumWaitingOnTLBFlush)
                        rmStatus = _Class5080UpdateTLBFlushState(pDeferredApiObject);
                }
            }
            break;
        }

        case NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND:
            paramSize = sizeof(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS);
            break;

        case NV2080_CTRL_CMD_GR_CTXSW_PM_BIND:
            paramSize = sizeof(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS);
            break;

        case NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND:
            paramSize = sizeof(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS);
            break;

        default:
            bIsCtrlCall = NV_FALSE;
            paramSize = 0;
            NV_PRINTF(LEVEL_ERROR, "Unknown or Unimplemented Command %x\n",
                      pDeferredApi->cmd);
                NV_ASSERT(0);
                rmStatus = NV_ERR_INVALID_ARGUMENT;
            break;
        }

        if (bIsCtrlCall)
        {
            RmCtrlParams        rmCtrlParams;
            RmCtrlExecuteCookie rmCtrlExecuteCookie = {0};
            RS_LOCK_INFO        lockInfo = {0};
            Subdevice          *pSubdevice;
            RsClient           *pRsClient = RES_GET_CLIENT(pDeferredApiObject);
            Device             *pDevice = GPU_RES_GET_DEVICE(pDeferredApiObject);
            const struct NVOC_EXPORTED_METHOD_DEF *pEntry;
            LOCK_ACCESS_TYPE access;
            NvU32 releaseFlags = 0;
            CALL_CONTEXT  callContext;
            CALL_CONTEXT *pOldContext = NULL;

            portMemSet(&rmCtrlParams, 0, sizeof(RmCtrlParams));
            rmCtrlParams.hClient    = pRsClient->hClient;
            rmCtrlParams.pGpu       = pGpu;
            rmCtrlParams.cmd        = pDeferredApi->cmd;
            rmCtrlParams.flags      = 0;
            rmCtrlParams.pParams    = &pDeferredApi->api_bundle;
            rmCtrlParams.paramsSize = paramSize;
            rmCtrlParams.secInfo.privLevel = pCliDeferredApi->privLevel;
            rmCtrlParams.secInfo.paramLocation = PARAM_LOCATION_KERNEL;
            rmCtrlParams.pCookie    = &rmCtrlExecuteCookie;
            rmCtrlParams.pLockInfo  = &lockInfo;
            rmCtrlParams.bDeferredApi = NV_TRUE;

            lockInfo.flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK |
                              RM_LOCK_FLAGS_NO_CLIENT_LOCK;

            rmCtrlParams.flags |= NVOS54_FLAGS_LOCK_BYPASS;

            // In case of deferred API, the parameters are already copied
            // from user space to kernel space when the deferred API is registered
            // So the IRQL_RAISED flag is set to avoid to second copy of paramaters
            if ((RMCFG_FEATURE_RM_BASIC_LOCK_MODEL && osIsRaisedIRQL()) ||
                hypervisorIsVgxHyper())
            {
                rmCtrlParams.flags |= NVOS54_FLAGS_IRQL_RAISED;
            }

            rmStatus = subdeviceGetByDeviceAndGpu(pRsClient, pDevice, pGpu,
                                                  &pSubdevice);
            if (rmStatus != NV_OK)
            {
                goto cleanup;
            }

            rmStatus = resControlLookup(staticCast(pSubdevice, RsResource), &rmCtrlParams, &pEntry);
            if (rmStatus != NV_OK)
            {
                goto cleanup;
            }

            NV_ASSERT(pEntry != NULL);
            // Initialize the execution cookie
            serverControl_InitCookie(pEntry, &rmCtrlExecuteCookie);

            // Set the call context as we use that to validate call permissions
            // in some cases
            portMemSet(&callContext, 0, sizeof(callContext));
            callContext.pResourceRef   = RES_GET_REF(pSubdevice);
            callContext.pClient        = pRsClient;
            callContext.secInfo        = rmCtrlParams.secInfo;
            callContext.pServer        = &g_resServ;
            callContext.pControlParams = &rmCtrlParams;
            callContext.pLockInfo      = rmCtrlParams.pLockInfo;

            if (RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
            {
                NvU32 gfid = kchannelGetGfid(Object->pKernelChannel);
                callContext.secInfo.pProcessToken = (void *)(NvU64) gfid;
            }

            NV_ASSERT_OK_OR_GOTO(rmStatus,
                resservSwapTlsCallContext(&pOldContext, &callContext), cleanup);

            rmStatus = serverControl_Prologue(&g_resServ, &rmCtrlParams, &access, &releaseFlags);

            if (rmStatus == NV_OK)
            {
                if (pEntry->paramSize == 0)
                {
                    typedef NV_STATUS (*CONTROL_EXPORT_FNPTR_NO_PARAMS)(void*);
                    CONTROL_EXPORT_FNPTR_NO_PARAMS pFunc = ((CONTROL_EXPORT_FNPTR_NO_PARAMS) pEntry->pFunc);
                    rmStatus = pFunc((void*)pSubdevice);
                }
                else
                {
                    typedef NV_STATUS (*CONTROL_EXPORT_FNPTR)(void*, void*);
                    CONTROL_EXPORT_FNPTR pFunc = ((CONTROL_EXPORT_FNPTR) pEntry->pFunc);
                    rmStatus = pFunc((void*)pSubdevice, rmCtrlParams.pParams);
                }
            }

            NV_ASSERT_OK(resservRestoreTlsCallContext(pOldContext));
            rmStatus = serverControl_Epilogue(&g_resServ, &rmCtrlParams, access, &releaseFlags, rmStatus);
        }

cleanup:

        pCliDeferredApi->Flags |= DEFERRED_API_INFO_FLAGS_HAS_EXECUTED;

        if (DRF_VAL(5080_CTRL, _CMD_DEFERRED_API, _FLAGS_DELETE, pDeferredApi->flags) ==
            NV5080_CTRL_CMD_DEFERRED_API_FLAGS_DELETE_IMPLICIT)
        {
            // delete implicitly, unless WAIT_FOR_TLB_FLUSH is also NV_TRUE
            if (DRF_VAL(5080_CTRL, _CMD_DEFERRED_API, _FLAGS_WAIT_FOR_TLB_FLUSH, pDeferredApi->flags) ==
                NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH_TRUE)
            {
                pDeferredApiObject->NumWaitingOnTLBFlush++;
            }
            else
            {
                _Class5080DelDeferredApi(pDeferredApiObject, Data);
            }
        }
    }

    return rmStatus;
}

static const METHOD Nv50DeferredApi[] =
{
    { mthdNoOperation,                  0x0100, 0x0103 },
    { _class5080DeferredApiV2,          0x0200, 0x0203 },
};

NV_STATUS defapiGetSwMethods_IMPL
(
    DeferredApiObject  *pDeferredApi,
    const METHOD      **ppMethods,
    NvU32              *pNumMethods
)
{
    *ppMethods = Nv50DeferredApi;
    *pNumMethods = NV_ARRAY_ELEMENTS(Nv50DeferredApi);
    return NV_OK;
}

NvBool defapiIsSwMethodStalling_IMPL
(
    DeferredApiObject *pDeferredApi,
    NvU32              hDeferredApi
)
{
    DEFERRED_API_INFO               *pCliDeferredApi = NULL;
    NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApiParams;

    NV_STATUS rmStatus = _Class5080GetDeferredApiInfo(pDeferredApi,
                                                      hDeferredApi,
                                                      &pCliDeferredApi);
    if (rmStatus == NV_OK)
    {
        pDeferredApiParams = pCliDeferredApi->pDeferredApiInfo;

        // Clear the PBDMA interrupt before executing the software method.
        if (pDeferredApiParams->cmd == NV2080_CTRL_CMD_FIFO_UPDATE_CHANNEL_INFO)
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

