/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/rc/kernel_rc.h"

#include "kernel/core/locks.h"
#include "kernel/diagnostics/journal.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "kernel/os/os.h"
#include "rmapi/client.h"

#include "ctrl/ctrl506f.h"

#include "libraries/utils/nvprintf.h"
#include "nverror.h"
#include "nvtypes.h"
#include "objtmr.h"
#include "vgpu/rpc.h"


static NV_STATUS
_vgpuRcResetCallback
(
    NvHandle          hClient,
    NvHandle          hDevice,
    NvHandle          hChannel,
    RC_ERROR_CONTEXT *pRcErrorContext
)
{
    OBJSYS   *pSys   = SYS_GET_INSTANCE();
    NV_STATUS status = NV_OK;

    if (osCondAcquireRmSema(pSys->pSema) == NV_OK)
    {
        if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_COND_ACQUIRE,
                              RM_LOCK_MODULES_RC) == NV_OK)
        {
            THREAD_STATE_NODE                             threadState;
            NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS params = {0};

            threadStateInitISRAndDeferredIntHandler(
                &threadState,
                pRcErrorContext->pGpu,
                THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

            params.engineID   = pRcErrorContext->EngineId;
            params.exceptType = pRcErrorContext->exceptType;

            NV_RM_RPC_CONTROL(pRcErrorContext->pGpu,
                              hClient,
                              hChannel,
                              NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL,
                              &params,
                              sizeof params,
                              status);

            threadStateFreeISRAndDeferredIntHandler(
                &threadState,
                pRcErrorContext->pGpu,
                THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

            portMemFree(pRcErrorContext);
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
        }
        else
        {
            status = NV_ERR_STATE_IN_USE;
        }
        osReleaseRmSema(pSys->pSema, NULL);
    }
    else
    {
        status = NV_ERR_STATE_IN_USE;
    }

    return status;
}


//
// krcResetCallback is called by both LDDM and MODS
// When adding more function parameters make sure to use datatypes that are
// defined in nvtypes.h and also update the RC_RESET_CALLBACK typedef in
// sdk/nvidia/inc/rmcd.h
//
NvU32
krcResetCallback
(
    NvHandle hClient,
    NvHandle hDevice,
    NvHandle hFifo,
    NvHandle hChannel,
    void    *pContext,
    NvBool   bClearRc
)
{
    THREAD_STATE_NODE threadState;
    RC_ERROR_CONTEXT *pRcErrorContext = (RC_ERROR_CONTEXT *)pContext;
    OBJSYS           *pSys            = SYS_GET_INSTANCE();
    NV_STATUS         status          = NV_ERR_GENERIC;

    if (pRcErrorContext != NULL)
    {
        if (bClearRc)
        {
            //
            // This is an error condition encountered where the caller
            // wants to free the RC allocated data structure and nothing
            // else.  Currently, only called by the KMD when a TDR occurs
            // and there are pending RCs that needs to be cancelled.
            //
            portMemFree(pRcErrorContext);
            status = NV_OK;
        }
        else if (IS_VIRTUAL(pRcErrorContext->pGpu))
        {
            status = _vgpuRcResetCallback(hClient,
                                          hDevice,
                                          hChannel,
                                          pRcErrorContext);
        }
        else if (osCondAcquireRmSema(pSys->pSema) == NV_OK)
        {
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_RC) ==
                NV_OK)
            {
                KernelChannel *pKernelChannel = NULL;

                threadStateInitISRAndDeferredIntHandler(
                    &threadState,
                    pRcErrorContext->pGpu,
                    THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

                NV_ASSERT_OK_OR_GOTO(
                    status,
                    serverGetClientUnderLock(&g_resServ, hClient, NULL),
                    error_cleanup);
                NV_ASSERT_OK_OR_GOTO(
                    status,
                    CliGetKernelChannel(hClient, hChannel, &pKernelChannel),
                    error_cleanup);

                NV_ASSERT_OR_ELSE(pKernelChannel != NULL,
                                  status = NV_ERR_INVALID_STATE;
                                  goto error_cleanup);

                if (IS_GSP_CLIENT(pRcErrorContext->pGpu))
                {
                    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS params = {0};
                    NV_RM_RPC_CONTROL(pRcErrorContext->pGpu,
                                      RES_GET_CLIENT_HANDLE(pKernelChannel),
                                      RES_GET_HANDLE(pKernelChannel),
                                      NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL,
                                      &params,
                                      sizeof params,
                                      status);
                }

                threadStateFreeISRAndDeferredIntHandler(
                    &threadState,
                    pRcErrorContext->pGpu,
                    THREAD_STATE_FLAGS_IS_DEFERRED_INT_HANDLER);

                portMemFree(pRcErrorContext);
                rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            }
            else
            {
                status = NV_ERR_STATE_IN_USE;
            }
            osReleaseRmSema(pSys->pSema, NULL);
        }
        else
        {
            status = NV_ERR_STATE_IN_USE;
        }
    }
    else
    {
        // If no context then just skip....
        NV_PRINTF(LEVEL_ERROR, "-- No context skipping reset of channel...\n");
        status = NV_OK;
    }

    return status;

error_cleanup:
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    osReleaseRmSema(pSys->pSema, NULL);
    return status;
}


NvBool
krcErrorInvokeCallback_IMPL
(
    OBJGPU                  *pGpu,
    KernelRc                *pKernelRc,
    KernelChannel           *pKernelChannel,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData,
    NvU32                    exceptType,
    NvU32                    exceptLevel,
    NvU32                    engineId,
    NvU32                    rcDiagRecStart
)
{
    OBJSYS             *pSys              = SYS_GET_INSTANCE();
    Journal            *pRcDB             = SYS_GET_RCDB(pSys);
    OBJOS              *pOS               = SYS_GET_OS(pSys);
    KernelMIGManager   *pKernelMigManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    RmClient           *pClient           = NULL;
    RsClient           *pRsClient;
    RC_CALLBACK_STATUS  clientAction;
    NvU32               localEngineId  = engineId;
    NvU32               rcDiagRecOwner = RCDB_RCDIAG_DEFAULT_OWNER;
    NV_STATUS           status;
    NvBool              bReturn = NV_TRUE;

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), bReturn);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelChannel != NULL, bReturn);

    status = serverGetClientUnderLock(&g_resServ,
                                      RES_GET_CLIENT_HANDLE(pKernelChannel),
                                      &pRsClient);
    if (status != NV_OK)
        return bReturn;

    pClient = dynamicCast(pRsClient, RmClient);
    if (pClient == NULL)
        return bReturn;

    //
    // If SMC is enabled, RM need to notify partition local engineIds.
    // Convert global ID to partition local
    //
    if (IS_MIG_IN_USE(pGpu) && NV2080_ENGINE_TYPE_IS_VALID(engineId) &&
        kmigmgrIsEnginePartitionable(pGpu, pKernelMigManager, engineId))
    {
        MIG_INSTANCE_REF ref;
        status = kmigmgrGetInstanceRefFromClient(pGpu,
                                                 pKernelMigManager,
                                                 pRsClient->hClient,
                                                 &ref);
        if (status != NV_OK)
            return bReturn;

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMigManager, engineId, ref))
        {
            // Notifier is requested for an unsupported engine
            NV_PRINTF(
                LEVEL_ERROR,
                "RcErroCallback requested for an unsupported engine (0x%x)\n",
                localEngineId);
            return bReturn;
        }

        // Override the engine type with the local engine idx
        status = kmigmgrGetGlobalToLocalEngineType(pGpu,
                                                   pKernelMigManager,
                                                   ref,
                                                   engineId,
                                                   &localEngineId);
        if (status != NV_OK)
            return bReturn;
    }

    if (pOS->osCheckCallback(pGpu))
    {
        NvHandle          hDevice, hFifo;
        RC_ERROR_CONTEXT *pRcErrorContext = NULL;
        Device           *pDevice;

        NV_ASSERT_OK_OR_RETURN(
            deviceGetByGpu(pRsClient, pGpu, NV_TRUE, &pDevice));

        hDevice = RES_GET_HANDLE(pDevice);


        if (!pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup
                 ->bAllocatedByRm)
        {
            hFifo = RES_GET_PARENT_HANDLE(pKernelChannel);
        }
        else
        {
            hFifo = RES_GET_HANDLE(pKernelChannel);
        }

        pRcErrorContext = portMemAllocNonPaged(sizeof *pRcErrorContext);
        if (pRcErrorContext != NULL)
        {
            portMemSet(pRcErrorContext, 0, sizeof *pRcErrorContext);

            pRcErrorContext->pGpu       = pGpu;
            pRcErrorContext->ChId       = pKernelChannel->ChID;
            pRcErrorContext->secChId    = 0xFFFFFFFF;
            pRcErrorContext->sechClient = RES_GET_CLIENT_HANDLE(pKernelChannel);
            pRcErrorContext->exceptType = exceptType;
            pRcErrorContext->EngineId   = localEngineId;
            pRcErrorContext->subdeviceInstance = pGpu->subdeviceInstance;

            if (pMmuExceptionData != NULL)
            {
                pRcErrorContext->addrLo    = pMmuExceptionData->addrLo;
                pRcErrorContext->addrHi    = pMmuExceptionData->addrHi;
                pRcErrorContext->faultType = pMmuExceptionData->faultType;
                pRcErrorContext->faultStr  = kgmmuGetFaultTypeString_HAL(
                    GPU_GET_KERNEL_GMMU(pGpu),
                    pMmuExceptionData->faultType);
            }
        }

        clientAction = pOS->osRCCallback(pGpu,
                                         RES_GET_CLIENT_HANDLE(pKernelChannel),
                                         hDevice,
                                         hFifo,
                                         RES_GET_HANDLE(pKernelChannel),
                                         exceptLevel,
                                         exceptType,
                                         (NvU32 *)pRcErrorContext,
                                         &krcResetCallback);

        if (clientAction == RC_CALLBACK_IGNORE ||
            clientAction == RC_CALLBACK_ISOLATE_NO_RESET)
        {
            if (clientAction == RC_CALLBACK_IGNORE)
            {
                NV_PRINTF(LEVEL_ERROR, "-- Drivers tells RM to ignore\n");
            }

            //
            // if osRCCallback returns RC_HANDLER_ISOLATE_NO_RESET or
            // IGNORE, client won't call rcResetChannel to put channel back
            // pRcErrorContext has to be released here
            //
            portMemFree(pRcErrorContext);
        }
        bReturn = (clientAction != RC_CALLBACK_IGNORE);
    }
    else
    {
        // use the new CliNotifyDeviceFifoEvent() notification method
        NvRcNotification       params;
        OBJTMR                *pTmr = GPU_GET_TIMER(pGpu);
        NvU64                  time;
        CLI_CHANNEL_CLASS_INFO classInfo;

        tmrGetCurrentTime(pTmr, &time);

        params.timeStamp.nanoseconds[0] = NvU64_HI32(time);
        params.timeStamp.nanoseconds[1] = NvU64_LO32(time);
        params.exceptLevel              = exceptLevel;
        params.exceptType               = exceptType;

        // Get rc notifier index from class info

        CliGetChannelClassInfo(RES_GET_EXT_CLASS_ID(pKernelChannel),
                               &classInfo);

        // notify the Fifo channel based event listeners
        kchannelNotifyGeneric(pKernelChannel,
                              classInfo.rcNotifierIndex,
                              &params,
                              sizeof(params));
    }

    // update RC diagnostic records with process id and owner
    if (rcDiagRecStart != INVALID_RCDB_RCDIAG_INDEX)
    {
        rcdbUpdateRcDiagRecContext(pRcDB,
                                   rcDiagRecStart,
                                   pRcDB->RcErrRptNextIdx - 1,
                                   pClient->ProcID,
                                   rcDiagRecOwner);
    }
    return bReturn;
}
