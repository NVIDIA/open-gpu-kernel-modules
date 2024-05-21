/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "rmapi/rmapi.h"
#include "rmapi/control.h"
#include "rmapi/client.h"
#include "rmapi/rs_utils.h"
#include "diagnostics/tracer.h"
#include "core/locks.h"
#include "core/thread_state.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/device/device.h"

#include "entry_points.h"
#include "resserv/rs_access_map.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "rmapi/rmapi_specific.h"
#include "rmapi/rmapi_utils.h"

#include "ctrl/ctrl0000/ctrl0000gpuacct.h" // NV0000_CTRL_CMD_GPUACCT_*
#include "ctrl/ctrl2080/ctrl2080tmr.h" // NV2080_CTRL_CMD_TIMER_SCHEDULE

static NV_STATUS
releaseDeferRmCtrlBuffer(RmCtrlDeferredCmd* pRmCtrlDeferredCmd)
{
    portMemSet(&pRmCtrlDeferredCmd->paramBuffer, 0, RMCTRL_DEFERRED_MAX_PARAM_SIZE);

    portAtomicSetS32(&pRmCtrlDeferredCmd->pending, RMCTRL_DEFERRED_FREE);

    return NV_OK;
}

//
// This is the rmControl internal handler for deferred calls.
//
//

NV_STATUS
rmControl_Deferred(RmCtrlDeferredCmd* pRmCtrlDeferredCmd)
{
    RmCtrlParams rmCtrlParams;
    NvU8 paramBuffer[RMCTRL_DEFERRED_MAX_PARAM_SIZE];
    NV_STATUS status;
    RS_LOCK_INFO lockInfo = {0};
    RS_CONTROL_COOKIE rmCtrlExecuteCookie = {0};

    // init RmCtrlParams
    portMemCopy(&rmCtrlParams, sizeof(RmCtrlParams), &pRmCtrlDeferredCmd->rmCtrlDeferredParams, sizeof(RmCtrlParams));
    rmCtrlParams.hParent    = NV01_NULL_OBJECT;
    rmCtrlParams.pGpu       = NULL;
    rmCtrlParams.pLockInfo = &lockInfo;
    rmCtrlParams.pCookie = &rmCtrlExecuteCookie;

    // Temporary: tell ResServ not to take any locks
    lockInfo.flags = RM_LOCK_FLAGS_NO_GPUS_LOCK |
                     RM_LOCK_FLAGS_NO_CLIENT_LOCK;

    if (rmapiLockIsOwner())
    {
        lockInfo.state = RM_LOCK_STATES_API_LOCK_ACQUIRED;
    }
    else
    {
        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
    }

    // paramsSize not > _MAX already verified in _rmControlDeferred
    if ((rmCtrlParams.pParams != NvP64_NULL) && (rmCtrlParams.paramsSize != 0))
    {
        // copy param to a local buffer so that pRmCtrlDeferredCmd can be released
        portMemSet(paramBuffer, 0, RMCTRL_DEFERRED_MAX_PARAM_SIZE);
        portMemCopy(paramBuffer, rmCtrlParams.paramsSize, rmCtrlParams.pParams, rmCtrlParams.paramsSize);
        rmCtrlParams.pParams = paramBuffer;
    }

    releaseDeferRmCtrlBuffer(pRmCtrlDeferredCmd);

    // client was checked when we came in through rmControl()
    // but check again to make sure it's still good
    if (serverutilGetClientUnderLock(rmCtrlParams.hClient) == NULL)
    {
        status = NV_ERR_INVALID_CLIENT;
        goto exit;
    }

    status = serverControl(&g_resServ, &rmCtrlParams);

exit:

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_WARNING, "deferred rmctrl %x failed %x!\n",
                  rmCtrlParams.cmd, status);
    }

    return status;
}

static NV_STATUS
_rmControlDeferred(RmCtrlParams *pRmCtrlParams, NvP64 pUserParams, NvU32 paramsSize)
{
    // Schedule a deferred rmctrl call
    OBJGPU     *pGpu;
    NvBool      bBcResource;
    NV_STATUS   rmStatus;
    RsClient   *pClient;

    // We can't allocate memory at DIRQL, so use pre-allocated buffer to store any rmctrl param.
    // The size can't be large than DEFERRED_RMCTRL_MAX_PARAM_SIZE (defined in rmctrl.h), otherwise,
    // fail this call.
    if (paramsSize > RMCTRL_DEFERRED_MAX_PARAM_SIZE)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "rmctrl param size (%d) larger than limit (%d).\n",
                  paramsSize, RMCTRL_DEFERRED_MAX_PARAM_SIZE);
        rmStatus = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    rmStatus = serverGetClientUnderLock(&g_resServ, pRmCtrlParams->hClient, &pClient);
    if (rmStatus != NV_OK)
        return rmStatus;

    rmStatus = gpuGetByHandle(pClient, pRmCtrlParams->hObject, &bBcResource, &pGpu);
    if (rmStatus != NV_OK)
        return rmStatus;

    // Set SLI BC state for thread
    gpuSetThreadBcState(pGpu, bBcResource);

    pRmCtrlParams->pGpu = pGpu;
    pRmCtrlParams->pLockInfo = NULL;

    switch (pRmCtrlParams->cmd)
    {
        // we don't have available bit left in RmCtrlParams.cmd to
        // indicate a rmctrl type as deferrable so use cmd list here
        case NV2080_CTRL_CMD_TIMER_SCHEDULE:
        {
            if (pRmCtrlParams->flags & NVOS54_FLAGS_IRQL_RAISED)
            {
                OBJSYS    *pSys = SYS_GET_INSTANCE();
                NvU32 idx;

                for ( idx = 0; idx < MAX_DEFERRED_CMDS; idx++)
                {
                    if (portAtomicCompareAndSwapS32(&pGpu->pRmCtrlDeferredCmd[idx].pending,
                                                    RMCTRL_DEFERRED_ACQUIRED,
                                                    RMCTRL_DEFERRED_FREE))
                    {
                        portMemCopy(&pGpu->pRmCtrlDeferredCmd[idx].rmCtrlDeferredParams,
                                    sizeof(RmCtrlParams), pRmCtrlParams, sizeof(RmCtrlParams));

                        // copyin param to kernel buffer for deferred rmctrl
                        if (paramsSize != 0 && pUserParams != 0)
                        {
                            portMemCopy(pGpu->pRmCtrlDeferredCmd[idx].paramBuffer, paramsSize,
                                        NvP64_VALUE(pUserParams), paramsSize);

                            if (paramsSize < RMCTRL_DEFERRED_MAX_PARAM_SIZE)
                            {
                                portMemSet(pGpu->pRmCtrlDeferredCmd[idx].paramBuffer +
                                           paramsSize,
                                           0, RMCTRL_DEFERRED_MAX_PARAM_SIZE - paramsSize);
                            }

                            pGpu->pRmCtrlDeferredCmd[idx].rmCtrlDeferredParams.pParams =
                                pGpu->pRmCtrlDeferredCmd[idx].paramBuffer;
                        }

                        portAtomicSetS32(&pGpu->pRmCtrlDeferredCmd[idx].pending,
                                         RMCTRL_DEFERRED_READY);

                        // Make sure there's a release call to trigger the deferred rmctrl.
                        // Previous rmctrl that is holding the lock can already
                        // finished (release its lock) during the period before the pending
                        // flag is set and after this rmctrl failed to acquire lock.

                        // LOCK: try to acquire GPUs lock
                        if (rmGpuLocksAcquire(GPU_LOCK_FLAGS_COND_ACQUIRE,
                                              RM_LOCK_MODULES_CLIENT) == NV_OK)
                        {
                            if (osCondAcquireRmSema(pSys->pSema) == NV_OK)
                            {
                                // In case this is called from device interrupt, use pGpu to queue DPC.
                                osReleaseRmSema(pSys->pSema, pGpu);
                            }
                            // In case this is called from device interrupt, use pGpu to queue DPC.
                            // UNLOCK: release GPUs lock
                            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, pGpu);
                        }

                        rmStatus = NV_OK;
                        goto done;
                    }
                }
            }

            rmStatus = NV_ERR_STATE_IN_USE;
            break;
        }

        default:
            rmStatus = NV_ERR_BUSY_RETRY;
            break;
    }

done:
    return rmStatus;
}

NV_STATUS
serverControlApiCopyIn
(
    RsServer                        *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL  *pRmCtrlParams,
    RS_CONTROL_COOKIE               *pCookie
)
{
    NV_STATUS rmStatus;
    API_STATE *pParamCopy;
    API_STATE *pEmbeddedParamCopies;
    NvP64 pUserParams;
    NvU32 paramsSize;

    NV_ASSERT_OR_RETURN(pCookie != NULL, NV_ERR_INVALID_ARGUMENT);
    pParamCopy = &pCookie->paramCopy;
    pEmbeddedParamCopies = pCookie->embeddedParamCopies;
    pUserParams = NV_PTR_TO_NvP64(pRmCtrlParams->pParams);
    paramsSize = pRmCtrlParams->paramsSize;

    RMAPI_PARAM_COPY_INIT(*pParamCopy, pRmCtrlParams->pParams, pUserParams, 1, paramsSize);

    if (pCookie->apiCopyFlags & RMCTRL_API_COPY_FLAGS_SKIP_COPYIN_ZERO_BUFFER)
    {
        pParamCopy->flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYIN;
        pParamCopy->flags |= RMAPI_PARAM_COPY_FLAGS_ZERO_BUFFER;
    }

    rmStatus = rmapiParamsAcquire(pParamCopy, (pRmCtrlParams->secInfo.paramLocation == PARAM_LOCATION_USER));
    if (rmStatus != NV_OK)
        return rmStatus;
    pCookie->bFreeParamCopy = NV_TRUE;

    rmStatus = embeddedParamCopyIn(pEmbeddedParamCopies, pRmCtrlParams);
    if (rmStatus != NV_OK)
    {
        rmapiParamsRelease(pParamCopy);
        pRmCtrlParams->pParams = NvP64_VALUE(pUserParams);
        pCookie->bFreeParamCopy = NV_FALSE;
        return rmStatus;
    }
    pCookie->bFreeEmbeddedCopy = NV_TRUE;

    return NV_OK;
}

NV_STATUS
serverControlApiCopyOut
(
    RsServer                       *pServer,
    RS_RES_CONTROL_PARAMS_INTERNAL *pRmCtrlParams,
    RS_CONTROL_COOKIE              *pCookie,
    NV_STATUS                       rmStatus
)
{
    NV_STATUS  cpStatus;
    API_STATE *pParamCopy;
    API_STATE *pEmbeddedParamCopies;
    NvP64      pUserParams;
    NvBool     bFreeEmbeddedCopy;
    NvBool     bFreeParamCopy;

    NV_ASSERT_OR_RETURN(pCookie != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRmCtrlParams != NULL, NV_ERR_INVALID_ARGUMENT);

    if ((pCookie->apiCopyFlags & RMCTRL_API_COPY_FLAGS_SET_CONTROL_CACHE) && rmStatus == NV_OK)
    {
        rmapiControlCacheSet(pRmCtrlParams->hClient,
                             pRmCtrlParams->hObject,
                             pRmCtrlParams->cmd,
                             pRmCtrlParams->pParams,
                             pRmCtrlParams->paramsSize);
    }

    pParamCopy = &pCookie->paramCopy;
    pEmbeddedParamCopies = pCookie->embeddedParamCopies;
    pUserParams = pCookie->paramCopy.pUserParams;
    bFreeParamCopy = pCookie->bFreeParamCopy;
    bFreeEmbeddedCopy = pCookie->bFreeEmbeddedCopy;

    if ((rmStatus != NV_OK) &&
        (!(pCookie->ctrlFlags & RMCTRL_FLAGS_COPYOUT_ON_ERROR) ||
        (pCookie->apiCopyFlags & RMCTRL_API_COPY_FLAGS_FORCE_SKIP_COPYOUT_ON_ERROR)))
    {
        pParamCopy->flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;

        if (bFreeEmbeddedCopy)
        {
            pEmbeddedParamCopies[0].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            pEmbeddedParamCopies[1].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            pEmbeddedParamCopies[2].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
            pEmbeddedParamCopies[3].flags |= RMAPI_PARAM_COPY_FLAGS_SKIP_COPYOUT;
        }
    }

    if (bFreeEmbeddedCopy)
    {
        cpStatus = embeddedParamCopyOut(pEmbeddedParamCopies, pRmCtrlParams);
        if (rmStatus == NV_OK)
            rmStatus = cpStatus;
        pCookie->bFreeEmbeddedCopy = NV_FALSE;
    }

    if (bFreeParamCopy)
    {
        cpStatus = rmapiParamsRelease(pParamCopy);
        if (rmStatus == NV_OK)
            rmStatus = cpStatus;
        pRmCtrlParams->pParams = NvP64_VALUE(pUserParams);
        pCookie->bFreeParamCopy = NV_FALSE;
    }

    return rmStatus;
}

static NV_STATUS
_rmapiRmControl(NvHandle hClient, NvHandle hObject, NvU32 cmd, NvP64 pUserParams, NvU32 paramsSize, NvU32 flags, RM_API *pRmApi, API_SECURITY_INFO *pSecInfo)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    RmCtrlParams rmCtrlParams;
    RS_CONTROL_COOKIE rmCtrlExecuteCookie = {0};
    NvBool bIsRaisedIrqlCmd;
    NvBool bIsLockBypassCmd;
    NvBool bInternalRequest;
    NV_STATUS  rmStatus = NV_OK;
    RS_LOCK_INFO        lockInfo = {0};
    NvU32 ctrlFlags = 0;
    NvU32 ctrlAccessRight = 0;
    NvU32 ctrlParamsSize = 0;
    NV_STATUS getCtrlInfoStatus;

    RMTRACE_RMAPI(_RMCTRL_ENTRY, cmd);

    // Check first for the NULL command.
    // Return NV_OK immediately for NVXXXX_CTRL_CMD_NULL (0x00000000)
    // as well as the per-class NULL cmd ( _CATEGORY==0x00 and _INDEX==0x00 )
    if ((cmd == NVXXXX_CTRL_CMD_NULL) ||
        (FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _CATEGORY, 0x00, cmd) &&
         FLD_TEST_DRF_NUM(XXXX, _CTRL_CMD, _INDEX,    0x00, cmd)))
    {
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO,
              "rmControl: hClient 0x%x hObject 0x%x cmd 0x%x\n", hClient,
              hObject, cmd);

    NV_PRINTF(LEVEL_INFO, "rmControl: pUserParams 0x%p paramSize 0x%x\n",
              NvP64_VALUE(pUserParams), paramsSize);

    // If we're behind either API lock or GPU lock treat as internal.
    bInternalRequest = pRmApi->bApiLockInternal || pRmApi->bGpuLockInternal;

    // is this a raised IRQL cmd?
    bIsRaisedIrqlCmd = (flags & NVOS54_FLAGS_IRQL_RAISED);

    // is this a lock bypass cmd?
    bIsLockBypassCmd = ((flags & NVOS54_FLAGS_LOCK_BYPASS) || pRmApi->bGpuLockInternal);

    // NVOS54_FLAGS_IRQL_RAISED cmds are only allowed to be called in raised irq level.
    if (bIsRaisedIrqlCmd)
    {
        // Check that we support this control call at raised IRQL
        if (!rmapiRmControlCanBeRaisedIrql(cmd))
        {
            NV_PRINTF(LEVEL_WARNING,
                      "rmControl:  cmd 0x%x cannot be called at raised irq level\n", cmd);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (!osIsRaisedIRQL())
        {
            NV_PRINTF(LEVEL_WARNING,
                      "rmControl:  raised cmd 0x%x at normal irq level\n", cmd);
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

    if (bIsLockBypassCmd)
    {
        flags |= NVOS54_FLAGS_LOCK_BYPASS;

        if (!bInternalRequest)
        {
            // Check that we support bypassing locks with this control call
            if (!rmapiRmControlCanBeBypassLock(cmd))
            {
                NV_PRINTF(LEVEL_WARNING,
                          "rmControl:  cmd 0x%x cannot bypass locks\n", cmd);
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }
        }
    }

    // Potential race condition if run lockless?
    if (serverutilGetClientUnderLock(hClient) == NULL)
    {
        rmStatus = NV_ERR_INVALID_CLIENT;
        goto done;
    }

    // only kernel clients can issue raised IRQL or lock bypass cmds
    // bypass client priv check for internal calls done on behalf of lower priv
    // clients
    if ((bIsRaisedIrqlCmd || bIsLockBypassCmd) && !bInternalRequest)
    {
        if (pSecInfo->privLevel < RS_PRIV_LEVEL_KERNEL)
        {
            rmStatus = NV_ERR_INVALID_CLIENT;
            goto done;
        }
    }

    getCtrlInfoStatus = rmapiutilGetControlInfo(cmd, &ctrlFlags, &ctrlAccessRight, &ctrlParamsSize);

    // error check parameters
    if (((paramsSize != 0) && (pUserParams == (NvP64) 0)) ||
        ((paramsSize == 0) && (pUserParams != (NvP64) 0))
        || ((getCtrlInfoStatus == NV_OK) && (paramsSize != ctrlParamsSize))
        )
    {
        NV_PRINTF(LEVEL_WARNING,
                  "bad params: cmd:0x%x ptr " NvP64_fmt " size: 0x%x expect size: 0x%x\n",
                  cmd, pUserParams, paramsSize, ctrlParamsSize);
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    // init RmCtrlParams
    portMemSet(&rmCtrlParams, 0, sizeof(rmCtrlParams));
    rmCtrlParams.hClient = hClient;
    rmCtrlParams.hObject = hObject;
    rmCtrlParams.cmd = cmd;
    rmCtrlParams.flags = flags;
    rmCtrlParams.pParams = NvP64_VALUE(pUserParams);
    rmCtrlParams.paramsSize = paramsSize;
    rmCtrlParams.hParent = NV01_NULL_OBJECT;
    rmCtrlParams.pGpu = NULL;
    rmCtrlParams.pResourceRef = NULL;
    rmCtrlParams.secInfo = *pSecInfo;
    rmCtrlParams.pLockInfo = &lockInfo;
    rmCtrlParams.pCookie = &rmCtrlExecuteCookie;
    rmCtrlParams.bInternal = bInternalRequest;

    if (pRmApi->bApiLockInternal)
    {
        lockInfo.state |= RM_LOCK_STATES_API_LOCK_ACQUIRED;
        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
    }

    if (getCtrlInfoStatus == NV_OK)
    {
        //
        // The output of CACHEABLE RMCTRL do not depend on the input.
        // Skip param copy and clear the buffer in case the uninitialized
        // buffer leaks information to clients.
        //
        if (ctrlFlags & RMCTRL_FLAGS_CACHEABLE)
            rmCtrlParams.pCookie->apiCopyFlags |= RMCTRL_API_COPY_FLAGS_SKIP_COPYIN_ZERO_BUFFER;
    }

    rmCtrlParams.pCookie->ctrlFlags = ctrlFlags;

    //
    // Three separate rmctrl command modes:
    //
    //  mode#1: lock bypass rmctrl request
    //  mode#2: raised-irql rmctrl request
    //  mode#3: normal rmctrl request
    //
    if (bIsLockBypassCmd)
    {
        lockInfo.state |= RM_LOCK_STATES_API_LOCK_ACQUIRED;
        lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK |
                          RM_LOCK_FLAGS_NO_GPUS_LOCK |
                          RM_LOCK_FLAGS_NO_CLIENT_LOCK;

        //
        // Lock bypass rmctrl request.
        //
        rmStatus = serverControl(&g_resServ, &rmCtrlParams);
    }
    else if (bIsRaisedIrqlCmd)
    {
        //
        // Raised IRQL rmctrl request.
        //
        // Try to get lock; if we cannot get it then place on deferred queue.
        //

        // LOCK: try to acquire GPUs lock
        if (osCondAcquireRmSema(pSys->pSema) == NV_OK)
        {
            if (rmGpuLocksAcquire(GPU_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_CLIENT) == NV_OK)
            {
                lockInfo.state |= RM_LOCK_STATES_GPUS_LOCK_ACQUIRED;
                lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK |
                                  RM_LOCK_FLAGS_NO_GPUS_LOCK |
                                  RM_LOCK_FLAGS_NO_CLIENT_LOCK;
                rmStatus = serverControl(&g_resServ, &rmCtrlParams);

                // UNLOCK: release GPUs lock
                rmGpuLocksRelease(GPU_LOCK_FLAGS_COND_ACQUIRE, osIsISR() ? rmCtrlParams.pGpu : NULL);
            }
            else
            {
                rmStatus = _rmControlDeferred(&rmCtrlParams, pUserParams, paramsSize);
            }
            // we must have a pGpu here for queuing of a DPC.
            NV_ASSERT(!osIsISR() || rmCtrlParams.pGpu);
            osReleaseRmSema(pSys->pSema, osIsISR() ? rmCtrlParams.pGpu : NULL);
        }
        else
        {
            rmStatus = _rmControlDeferred(&rmCtrlParams, pUserParams, paramsSize);
        }
    }
    else
    {
        //
        // Normal rmctrl request.
        //

        if (getCtrlInfoStatus == NV_OK)
        {
            if (rmapiControlIsCacheable(ctrlFlags, ctrlAccessRight, NV_FALSE))
            {
                rmCtrlParams.pCookie->apiCopyFlags |= RMCTRL_API_COPY_FLAGS_FORCE_SKIP_COPYOUT_ON_ERROR;

                rmStatus = serverControlApiCopyIn(&g_resServ, &rmCtrlParams,
                                                  rmCtrlParams.pCookie);
                if (rmStatus == NV_OK)
                {
                    rmStatus = rmapiControlCacheGet(hClient, hObject, cmd,
                                                    rmCtrlParams.pParams,
                                                    paramsSize);

                    // rmStatus is passed in for error handling
                    rmStatus = serverControlApiCopyOut(&g_resServ,
                                                       &rmCtrlParams,
                                                       rmCtrlParams.pCookie,
                                                       rmStatus);
                }

                if (rmStatus == NV_OK)
                {
                    goto done;
                }
                else
                {
                    // reset cookie if cache get failed
                    portMemSet(rmCtrlParams.pCookie, 0, sizeof(RS_CONTROL_COOKIE));
                    rmCtrlParams.pCookie->apiCopyFlags |= RMCTRL_API_COPY_FLAGS_SET_CONTROL_CACHE;

                    // re-initialize the flag if it's cleaned
                    if (ctrlFlags & RMCTRL_FLAGS_CACHEABLE)
                        rmCtrlParams.pCookie->apiCopyFlags |= RMCTRL_API_COPY_FLAGS_SKIP_COPYIN_ZERO_BUFFER;
                }
            }
        }

        RM_API_CONTEXT rmApiContext = {0};
        rmStatus = rmapiPrologue(pRmApi, &rmApiContext);
        if (rmStatus != NV_OK)
            goto epilogue;

        //
        // If this is an internal request within the same RM instance, make
        // sure we don't double lock clients and preserve previous lock state.
        //
        if (bInternalRequest && resservGetTlsCallContext() != NULL)
        {
            NvHandle hSecondClient = NV01_NULL_OBJECT;

            if (pSecInfo->paramLocation == PARAM_LOCATION_KERNEL)
            {
                rmStatus = serverControlLookupSecondClient(cmd,
                    NvP64_VALUE(pUserParams), rmCtrlParams.pCookie, &hSecondClient);

                if (rmStatus != NV_OK)
                    goto epilogue;
            }

            rmStatus = rmapiInitLockInfo(pRmApi, hClient, hSecondClient, &lockInfo);
            if (rmStatus != NV_OK)
                goto epilogue;

            //
            // rmapiInitLockInfo overwrites lockInfo.flags, re-add
            // RM_LOCK_FLAGS_NO_API_LOCK if it was originally added.
            //
            if (pRmApi->bApiLockInternal)
                lockInfo.flags |= RM_LOCK_FLAGS_NO_API_LOCK;
        }

        lockInfo.flags |= RM_LOCK_FLAGS_RM_SEMA;
        rmStatus = serverControl(&g_resServ, &rmCtrlParams);
epilogue:
        rmapiEpilogue(pRmApi, &rmApiContext);
    }
done:

    RMTRACE_RMAPI(_RMCTRL_EXIT, cmd);
    return rmStatus;
}

static NvBool
serverControl_ValidateVgpu
(
    OBJGPU *pGpu,
    NvU32 cmd,
    RS_PRIV_LEVEL privLevel,
    const NvU32 cookieFlags
)
{
    NvBool bPermissionGranted = NV_FALSE;

    // Check if context is already sufficiently admin privileged
    if (cookieFlags & RMCTRL_FLAGS_PRIVILEGED)
    {
        if (privLevel >= RS_PRIV_LEVEL_USER_ROOT)
        {
            bPermissionGranted = NV_TRUE;
        }
    }

    //
    // If context is not privileged enough, check whether this
    // control call is allowed in current hypervisor environment
    //
    if (!bPermissionGranted)
    {
        {
            // For non-NV0000, identify current hypervisor environment and check for allow flag
            if
            (
                (IS_SRIOV_WITH_VGPU_GSP_ENABLED(pGpu) && (cookieFlags & RMCTRL_FLAGS_CPU_PLUGIN_FOR_VGPU_GSP))
            )
            {
                bPermissionGranted = NV_TRUE;
            }
        }
    }

    return bPermissionGranted;
}

// validate rmctrl flags
NV_STATUS serverControl_ValidateCookie
(
    RS_RES_CONTROL_PARAMS_INTERNAL *pRmCtrlParams,
    RS_CONTROL_COOKIE *pRmCtrlExecuteCookie
)
{
    NV_STATUS status;
    OBJGPU *pGpu;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    if (pCallContext == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Calling context is NULL!\n");
        return NV_ERR_INVALID_PARAMETER;
    }

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        pGpu = gpumgrGetSomeGpu();
        if (pGpu == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU is not found\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    if (g_resServ.bRsAccessEnabled)
    {
        if (pRmCtrlParams->pResourceRef != NULL)
        {
            //
            // Check that the invoking client has appropriate access rights
            // For control calls, the invoking client is the owner of the ref
            //
            status = rsAccessCheckRights(pRmCtrlParams->pResourceRef,
                                         pRmCtrlParams->pResourceRef->pClient,
                                         &pRmCtrlExecuteCookie->rightsRequired);
            if (status != NV_OK)
                return status;
        }
        else
        {
            // pResourceRef can be NULL when rmControlCmdExecute is manually
            // invoked from the deferred API path (see class5080DeferredApiV2).
            // For now, we skip performing any access right checks in this case.
        }
    }
    else
    {
        //
        // When access rights are disabled, any control calls that have the
        // *_IF_RS_ACCESS_DISABLED flags should be treated as if they were declared
        // with the corresponding flags
        //
        if ((pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_PRIVILEGED_IF_RS_ACCESS_DISABLED) != 0)
        {
            pRmCtrlExecuteCookie->ctrlFlags |= RMCTRL_FLAGS_PRIVILEGED;
        }
    }

    if (pRmCtrlParams->pGpu != NULL && IS_VIRTUAL(pRmCtrlParams->pGpu) &&
        (pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL) &&
        !(pRmCtrlExecuteCookie->ctrlFlags & (RMCTRL_FLAGS_ROUTE_TO_VGPU_HOST | RMCTRL_FLAGS_PHYSICAL_IMPLEMENTED_ON_VGPU_GUEST)))
    {
        if (!rmapiutilSkipErrorMessageForUnsupportedVgpuGuestControl(pRmCtrlParams->cmd))
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported ROUTE_TO_PHYSICAL control 0x%x was called on vGPU guest\n", pRmCtrlParams->cmd);
        }

        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_INTERNAL))
    {
        NvBool bInternalCall = pRmCtrlParams->bInternal;
        if (!bInternalCall)
            return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Narrow down usecase as much as possible to CPU-plugin.
    // Must be running in hypervisor, at least cached privileged, not a kernel context and
    // accessing a privileged or kernel privileged control call.
    //
    if (hypervisorIsVgxHyper() &&
        clientIsAdmin(pCallContext->pClient, clientGetCachedPrivilege(pCallContext->pClient)) &&
        (pRmCtrlParams->secInfo.privLevel != RS_PRIV_LEVEL_KERNEL) &&
        !(pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_NON_PRIVILEGED))
    {
        // VGPU CPU-Plugin (Legacy Non-SRIOV, SRIOV-HYPERV, SRIOV-LEGACY, SRIOV-Offload), and Admin or kernel clients running in hypervisor
        NvBool bPermissionGranted = serverControl_ValidateVgpu(pRmCtrlParams->pGpu,
                                                               pRmCtrlParams->cmd,
                                                               pRmCtrlParams->secInfo.privLevel,
                                                               pRmCtrlExecuteCookie->ctrlFlags);
        if (!bPermissionGranted)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "hClient: 0x%08x, hObject 0x%08x, cmd 0x%08x: non-privileged hypervisor context issued privileged cmd\n",
                      pRmCtrlParams->hClient, pRmCtrlParams->hObject,
                      pRmCtrlParams->cmd);
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }
    else
    {
        //
        // Non-Hypervisor clients
        // PF clients
        // Unprivileged processes running in Hypervisor
        // Privileged processes running in Hypervisor, executing an unprivileged control call.
        // Kernel privileged processes running in Hypervisor
        //

        // permissions check for PRIVILEGED controls
        if (pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_PRIVILEGED)
        {
            //
            // Calls originating from usermode require admin perms while calls
            // originating from other kernel drivers are always allowed.
            //
            if (pRmCtrlParams->secInfo.privLevel < RS_PRIV_LEVEL_USER_ROOT)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "hClient: 0x%08x, hObject 0x%08x, cmd 0x%08x: non-privileged context issued privileged cmd\n",
                          pRmCtrlParams->hClient, pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd);
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }

        // permissions check for KERNEL_PRIVILEGED (default) unless NON_PRIVILEGED, PRIVILEGED or INTERNAL is specified
        if (!(pRmCtrlExecuteCookie->ctrlFlags & (RMCTRL_FLAGS_NON_PRIVILEGED | RMCTRL_FLAGS_PRIVILEGED | RMCTRL_FLAGS_INTERNAL)))
        {
            if (pRmCtrlParams->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
            {
                NV_PRINTF(LEVEL_WARNING,
                          "hClient: 0x%08x, hObject 0x%08x, cmd 0x%08x: non-kernel client issued kernel-only cmd\n",
                          pRmCtrlParams->hClient, pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd);
                return NV_ERR_INSUFFICIENT_PERMISSIONS;
            }
        }
    }

    // fail if GPU isn't ready
    if ((!(pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_NO_GPUS_ACCESS)) && (pRmCtrlParams->pGpu != NULL))
    {
        API_GPU_FULL_POWER_SANITY_CHECK(pRmCtrlParams->pGpu, NV_FALSE,
            pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_ALLOW_WITHOUT_SYSMEM_ACCESS);

        if ( ! API_GPU_ATTACHED_SANITY_CHECK(pRmCtrlParams->pGpu))
            return NV_ERR_GPU_IS_LOST;
    }

    if ((pRmCtrlParams->flags & NVOS54_FLAGS_IRQL_RAISED) &&
        (pRmCtrlParams->secInfo.paramLocation != PARAM_LOCATION_KERNEL))
    {
        return NV_ERR_INVALID_PARAMETER;
    }

    if (pRmCtrlExecuteCookie->ctrlFlags & RMCTRL_FLAGS_RM_TEST_ONLY_CODE)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        if (!pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE))
        {
            return NV_ERR_TEST_ONLY_CODE_NOT_ENABLED;
        }
    }

    return NV_OK;
}

NV_STATUS
serverControlLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RmCtrlParams *pRmCtrlParams,
    RmCtrlExecuteCookie *pRmCtrlExecuteCookie,
    LOCK_ACCESS_TYPE *pAccess
)
{
    //
    // Calls with LOCK_TOP doesn't fill in the cookie param correctly.
    // This is just a WAR for this.
    //
    NvU32 controlFlags = pRmCtrlExecuteCookie->ctrlFlags;
    if (controlFlags == 0 && !RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_STATUS status = rmapiutilGetControlInfo(pRmCtrlParams->cmd, &controlFlags, NULL, NULL);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "rmapiutilGetControlInfo(cmd=0x%x, out flags=0x%x, NULL) = status=0x%x\n",
                      pRmCtrlParams->cmd, controlFlags, status);
        }

        pRmCtrlExecuteCookie->ctrlFlags = controlFlags;
    }

    NvBool areAllGpusInOffloadMode = gpumgrAreAllGpusInOffloadMode();

    //
    // If the control is ROUTE_TO_PHYSICAL, and we're in GSP offload mode,
    // we can use a more relaxed locking mode:
    //    1. Only lock the single device and not all GPUs
    //    2. Take the API lock for READ instead of WRITE.
    // Unfortunately, at this point we don't have the pGpu yet to check if it
    // is in offload mode or not. So, instead, these optimizations are only
    // done if *all* GPUs in the system are in offload mode.
    //
    NvBool bUseGspLockingMode = areAllGpusInOffloadMode &&
                                (controlFlags & RMCTRL_FLAGS_ROUTE_TO_PHYSICAL);

    if (pAccess == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *pAccess = LOCK_ACCESS_WRITE;

    if (lock == RS_LOCK_TOP)
    {
        if (controlFlags & RMCTRL_FLAGS_NO_API_LOCK)
        {
            // NO_API_LOCK requires no access to GPU lock protected data
            NV_ASSERT_OR_RETURN(((controlFlags & RMCTRL_FLAGS_NO_GPUS_LOCK) != 0),
                NV_ERR_INVALID_LOCK_STATE);

            // NO_API_LOCK used in combination with API_LOCK_READONLY does not make sense
            NV_ASSERT_OR_RETURN(((controlFlags & RMCTRL_FLAGS_API_LOCK_READONLY) == 0),
                NV_ERR_INVALID_LOCK_STATE);

            RS_LOCK_INFO *pLockInfo = pRmCtrlParams->pLockInfo;

            pLockInfo->flags |= RM_LOCK_FLAGS_NO_API_LOCK;
            return NV_OK;
        }

        if (!serverSupportsReadOnlyLock(&g_resServ, RS_LOCK_TOP, RS_API_CTRL))
        {
            *pAccess = LOCK_ACCESS_WRITE;
            return NV_OK;
        }

        if (controlFlags & RMCTRL_FLAGS_API_LOCK_READONLY)
        {
            *pAccess = LOCK_ACCESS_READ;
        }

        //
        // ROUTE_TO_PHYSICAL controls always take the READ API lock. This only applies
        // to GSP clients: Only there can we guarantee per-gpu execution of commands.
        //
        if (g_resServ.bRouteToPhysicalLockBypass && bUseGspLockingMode)
        {
            *pAccess = LOCK_ACCESS_READ;
        }

        return NV_OK;
    }

    if (lock == RS_LOCK_RESOURCE)
    {
        RS_LOCK_INFO *pLockInfo = pRmCtrlParams->pLockInfo;

        //
        // Do not acquire the GPU lock if we were explicitly told
        // not to or if this is an Internal Call meaning that
        // we already own the GPUs Lock.
        //
        if  ((pLockInfo->state & RM_LOCK_STATES_GPUS_LOCK_ACQUIRED) ||
             (controlFlags & RMCTRL_FLAGS_NO_GPUS_LOCK) ||
             (pRmCtrlParams->flags & NVOS54_FLAGS_IRQL_RAISED) ||
             (pRmCtrlParams->flags & NVOS54_FLAGS_LOCK_BYPASS))
        {
            pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK;
            pLockInfo->flags &= ~RM_LOCK_FLAGS_GPU_GROUP_LOCK;
        }
        else
        {
            if ((controlFlags & RMCTRL_FLAGS_GPU_LOCK_DEVICE_ONLY) ||
                (g_resServ.bRouteToPhysicalLockBypass && bUseGspLockingMode))
            {
                pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK;
                pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
            }
            else
            {
                pLockInfo->flags &= ~RM_LOCK_FLAGS_NO_GPUS_LOCK;
                pLockInfo->flags &= ~RM_LOCK_FLAGS_GPU_GROUP_LOCK;
            }
        }

        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
serverControlLookupClientLockFlags
(
    RmCtrlExecuteCookie *pRmCtrlExecuteCookie,
    enum CLIENT_LOCK_TYPE *pClientLockType
)
{
    NvU32 controlFlags = pRmCtrlExecuteCookie->ctrlFlags;

    if ((controlFlags & RMCTRL_FLAGS_ALL_CLIENT_LOCK) != 0)
    {
        *pClientLockType = CLIENT_LOCK_ALL;

        // Locking all clients requires the RW API lock
        NV_ASSERT_OR_RETURN(rmapiLockIsWriteOwner(), NV_ERR_INVALID_LOCK_STATE);
    }
    else
        *pClientLockType = CLIENT_LOCK_SPECIFIC;

    return NV_OK;
}

NV_STATUS
rmapiControl
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hObject,
    NvU32     cmd,
    void     *pParams,
    NvU32     paramsSize
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->ControlWithSecInfo(pRmApi, hClient, hObject, cmd, NV_PTR_TO_NvP64(pParams),
                                      paramsSize, 0, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiControlWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              cmd,
    NvP64              pParams,
    NvU32              paramsSize,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;

    NV_PRINTF(LEVEL_INFO,
              "Nv04Control: hClient:0x%x hObject:0x%x cmd:0x%x params:" NvP64_fmt " paramSize:0x%x flags:0x%x\n",
              hClient, hObject, cmd, pParams, paramsSize, flags);

    status = _rmapiRmControl(hClient, hObject, cmd, pParams, paramsSize, flags, pRmApi, pSecInfo);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Nv04Control: control complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Nv04Control: control failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
        NV_PRINTF(LEVEL_INFO,
                  "Nv04Control:  hClient:0x%x hObject:0x%x cmd:0x%x params:" NvP64_fmt " paramSize:0x%x flags:0x%x\n",
                  hClient, hObject, cmd, pParams, paramsSize, flags);
    }

    return status;
}


//
// Called at DIRQL, where we can't do memory allocations
// Do not inline that function to save stack space
//
static NV_NOINLINE NV_STATUS
_rmapiControlWithSecInfoTlsIRQL
(
    RM_API* pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              cmd,
    NvP64              pParams,
    NvU32              paramsSize,
    NvU32              flags,
    API_SECURITY_INFO* pSecInfo
)
{
    NV_STATUS           status;
    THREAD_STATE_NODE   threadState;

    NvU8                stackAllocator[2*TLS_ISR_ALLOCATOR_SIZE];
    PORT_MEM_ALLOCATOR* pIsrAllocator = portMemAllocatorCreateOnExistingBlock(stackAllocator, sizeof(stackAllocator));
    tlsIsrInit(pIsrAllocator);

    //
    // SMP synchronization for Nv04Control is handled lower in the
    // call sequence to accommodate the various operation-specific
    // lock requirements (e.g. some operations can run locklessly).
    //
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiControlWithSecInfo(pRmApi, hClient, hObject, cmd, pParams, paramsSize, flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    tlsIsrDestroy(pIsrAllocator);
    portMemAllocatorRelease(pIsrAllocator);

    return status;
}


NV_STATUS
rmapiControlWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              cmd,
    NvP64              pParams,
    NvU32              paramsSize,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS           status;
    THREAD_STATE_NODE   threadState;

    if (!portMemExSafeForNonPagedAlloc())
    {
        return _rmapiControlWithSecInfoTlsIRQL(pRmApi, hClient, hObject, cmd, pParams, paramsSize, flags, pSecInfo);
    }

    //
    // SMP synchronization for Nv04Control is handled lower in the
    // call sequence to accommodate the various operation-specific
    // lock requirements (e.g. some operations can run locklessly).
    //
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiControlWithSecInfo(pRmApi, hClient, hObject, cmd, pParams, paramsSize, flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

