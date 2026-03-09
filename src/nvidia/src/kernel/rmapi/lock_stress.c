/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_LOCK_STRESS_H_PRIVATE_ACCESS_ALLOWED

#include "core/locks.h"
#include "core/system.h"
#include "rmapi/client.h"
#include "rmapi/lock_stress.h"
#include "rmapi/rs_utils.h"

#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "os/os.h"

#include "class/cl0080.h"
#include "class/cl0100.h"
#include "class/cl0101.h"

#include "g_finn_rm_api.h"

static NvS32 g_LockStressCounter = 0;

NV_STATUS
lockStressObjConstruct_IMPL
(
    LockStressObject             *pResource,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvHandle hClient;

    //
    // This is an off-by-default object since its only use is with testing. Return
    // an error unless the RM test code registry key is turned on by the user.
    //
    if (!pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE))
        return NV_ERR_TEST_ONLY_CODE_NOT_ENABLED;

    //
    // Allocate internal client handle for stressing locks in the internal RM API
    // path if this LockStressObject was externally allocated.
    //
    hClient = RES_GET_CLIENT_HANDLE(pResource);

    if (!serverIsClientInternal(&g_resServ, hClient))
    {
        NV_STATUS status;
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        OBJGPU *pGpu = GPU_RES_GET_GPU(pResource);
        NV0080_ALLOC_PARAMETERS nv0080AllocParams;
        NV2080_ALLOC_PARAMETERS nv2080AllocParams;

        pResource->hInternalClient = NV01_NULL_OBJECT;

        // Allocate internal client
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi,
                NV01_NULL_OBJECT,
                NV01_NULL_OBJECT,
                NV01_NULL_OBJECT,
                NV01_ROOT,
                &pResource->hInternalClient,
                sizeof(pResource->hInternalClient)),
            failed);

        // Allocate a device
        portMemSet(&nv0080AllocParams, 0, sizeof(nv0080AllocParams));
        nv0080AllocParams.deviceId = gpuGetDeviceInstance(pGpu);
        nv0080AllocParams.hClientShare = pResource->hInternalClient;
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Alloc(pRmApi,
                pResource->hInternalClient,
                pResource->hInternalClient,
                &pResource->hInternalDevice,
                NV01_DEVICE_0,
                &nv0080AllocParams,
                sizeof(nv0080AllocParams)),
            failed);

        // Allocate a subdevice
        portMemSet(&nv2080AllocParams, 0, sizeof(nv2080AllocParams));
        nv2080AllocParams.subDeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Alloc(pRmApi,
                pResource->hInternalClient,
                pResource->hInternalDevice,
                &pResource->hInternalSubdevice,
                NV20_SUBDEVICE_0,
                &nv2080AllocParams,
                sizeof(nv2080AllocParams)),
            failed);

        // Allocate the internal lock stress object
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Alloc(pRmApi,
                pResource->hInternalClient,
                pResource->hInternalSubdevice,
                &pResource->hInternalLockStressObject,
                LOCK_STRESS_OBJECT,
                NULL, 0),
            failed);

        return NV_OK;
failed:
        //
        // Free internal client on error, Resource Server will free all other internal
        // objects allocated with it.
        //
        if (pResource->hInternalClient != NV01_NULL_OBJECT)
            pRmApi->Free(pRmApi, pResource->hInternalClient, pResource->hInternalClient);

        return status;
    }

    return NV_OK;
}

void
lockStressObjDestruct_IMPL
(
    LockStressObject *pResource
)
{
    if (!serverIsClientInternal(&g_resServ, RES_GET_CLIENT_HANDLE(pResource)))
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        //
        // Free internal client, Resource Server will free all other internal
        // objects allocated with it.
        //
        pRmApi->Free(pRmApi, pResource->hInternalClient, pResource->hInternalClient);
    }
}

NV_STATUS
lockStressObjCtrlCmdResetLockStressState_IMPL
(
    LockStressObject *pResource
)
{
    RsClient *pClient = RES_GET_CLIENT(pResource);
    RmClient *pRmClient = dynamicCast(pClient, RmClient);
    RmClient *pRmInternalClient;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pResource);

    if (pRmClient == NULL)
        return NV_ERR_INVALID_STATE;

    pRmInternalClient = serverutilGetClientUnderLock(pGpu->hInternalLockStressClient);
    if (pRmInternalClient == NULL)
        return NV_ERR_INVALID_STATE;

    // Reset all lock stress counters to 0
    g_LockStressCounter = 0;

    pGpu->lockStressCounter = 0;
    pRmClient->lockStressCounter = 0;
    pRmInternalClient->lockStressCounter = 0;

    return NV_OK;
}

static NV_STATUS
updateLockStressCounters
(
    LockStressObject *pResource,
    NvU8 action
)
{
    // Perform increments/decrements as the "action" bitmask dictates
    if (DRF_VAL(0100_CTRL, _GLOBAL_RMAPI, _LOCK_STRESS_COUNTER_ACTION, action) != 0)
    {
        // Assert that we hold the RW API lock here
        NV_ASSERT_OR_RETURN(rmapiLockIsWriteOwner(), NV_ERR_INVALID_LOCK_STATE);

        if (DRF_VAL(0100_CTRL, _GLOBAL_RMAPI, _LOCK_STRESS_COUNTER_INCREMENT, action) != 0)
            g_LockStressCounter++;
        else
            g_LockStressCounter--;
    }

    if (DRF_VAL(0100_CTRL, _GPU, _LOCK_STRESS_COUNTER_ACTION, action) != 0)
    {
        OBJGPU *pGpu;

        NV_ASSERT_OR_RETURN(rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

        pGpu = GPU_RES_GET_GPU(pResource);

        if (DRF_VAL(0100_CTRL, _GPU, _LOCK_STRESS_COUNTER_INCREMENT, action) != 0)
            pGpu->lockStressCounter++;
        else
            pGpu->lockStressCounter--;
    }

    if (DRF_VAL(0100_CTRL, _CLIENT, _LOCK_STRESS_COUNTER_ACTION, action) != 0)
    {
        RsClient *pClient = RES_GET_CLIENT(pResource);
        RmClient *pRmClient;

        //
        // Resource Server currently doesn't attempt to lock the internal client on the
        // internal RM API path if we already hold a client lock to avoid risking lock
        // ordering issues so only assert if we aren't on the internal RM API path.
        //
        if (!serverIsClientInternal(&g_resServ, pClient->hClient))
            NV_ASSERT_OR_RETURN(serverIsClientLocked(&g_resServ, pClient->hClient), NV_ERR_INVALID_LOCK_STATE);

        pRmClient = dynamicCast(pClient, RmClient);

        if (pRmClient == NULL)
            return NV_ERR_INVALID_STATE;

        if (DRF_VAL(0100_CTRL, _CLIENT, _LOCK_STRESS_COUNTER_INCREMENT, action) != 0)
            pRmClient->lockStressCounter++;
        else
            pRmClient->lockStressCounter--;
    }

    if (DRF_VAL(0100_CTRL, _INTERNAL_CLIENT, _LOCK_STRESS_COUNTER_ACTION, action) != 0)
    {
        OBJGPU *pGpu;
        RmClient *pRmInternalClient;

        NV_ASSERT_OR_RETURN(rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

        pGpu = GPU_RES_GET_GPU(pResource);
        pRmInternalClient = serverutilGetClientUnderLock(pGpu->hInternalLockStressClient);

        if (pRmInternalClient == NULL)
            return NV_ERR_INVALID_STATE;

        if (DRF_VAL(0100_CTRL, _INTERNAL_CLIENT, _LOCK_STRESS_COUNTER_INCREMENT, action)
            != 0)
        {
            pRmInternalClient->lockStressCounter++;
        }
        else
            pRmInternalClient->lockStressCounter--;
    }

    return NV_OK;
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams
)
{
    NvU8 rand;

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    // This API has all locks so we can increment/decrement all counters
    pParams->action =
        (rand & DRF_SHIFTMASK(NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_INCREMENT)) |
        DRF_SHIFTMASK(NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_ACTION);

    return updateLockStressCounters(pResource, pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams
)
{
    NvU8 rand;

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // This API has all locks except the GPU lock so we can increment/decrement just
    // the global counter and the per client counter.
    //
    // Internal clients can't be accessed without either the GPU lock or acquiring the
    // internal client's lock (requires dual client locking). Until then, just assume
    // we can only modify the global counter and the per client counter.
    //
    pParams->action =
        (rand & DRF_SHIFTMASK(NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_INCREMENT)) |
        DRF_SHIFTMASK(NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION);

    return updateLockStressCounters(pResource, pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams
)
{
    NvU8 rand;

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // This API takes the API lock in read mode so we can increment/decrement just the
    // per GPU counter and the per client counter.
    //
    pParams->action =
        (rand & DRF_SHIFTMASK(NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_INCREMENT)) |
        DRF_SHIFTMASK(NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_ACTION);

    return updateLockStressCounters(pResource, pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams
)
{
    NvU8 rand;

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // This API takes the API lock in read mode and no GPU lock so we can only
    // increment/decrement the per client counter.
    //
    pParams->action =
        (rand & DRF_SHIFTMASK(NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_INCREMENT)) |
        DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION);

    return updateLockStressCounters(pResource, pParams->action);
}

static NV_STATUS
updateLockStressCountersInternal
(
    LockStressObject *pResource,
    RM_API *pRmApi,
    NvU32   internalCmd,
    NvU8   *pAction
)
{
    NV0100_CTRL_LOCK_STRESS_OUTPUT internalParams;

    // Handle the external client updates here before making the internal RM API call
    if (DRF_VAL(0100_CTRL, _CLIENT, _LOCK_STRESS_COUNTER_ACTION, *pAction) != 0)
    {
        RsClient *pClient = RES_GET_CLIENT(pResource);
        RmClient *pRmClient = dynamicCast(pClient, RmClient);

        //
        // Resource Server currently doesn't attempt to lock the internal client on the
        // internal RM API path if we already hold a client lock to avoid risking lock
        // ordering issues so only assert if we aren't on the internal RM API path.
        //
        if (!serverIsClientInternal(&g_resServ, pClient->hClient))
            NV_ASSERT_OR_RETURN(serverIsClientLocked(&g_resServ, pClient->hClient), NV_ERR_INVALID_LOCK_STATE);

        if (pRmClient == NULL)
            return NV_ERR_INVALID_STATE;

        if (DRF_VAL(0100_CTRL, _CLIENT, _LOCK_STRESS_COUNTER_INCREMENT, *pAction) != 0)
            pRmClient->lockStressCounter++;
        else
            pRmClient->lockStressCounter--;
    }

    // Make internal control
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, pRmApi->Control(pRmApi, pResource->hInternalClient,
        pResource->hInternalLockStressObject, internalCmd,
        &internalParams, sizeof(internalParams)));

    //
    // Capture how the internal control updated the counters. Don't capture the per
    // client counter since the internal client isn't visible to the caller anyway.
    //
    *pAction |=
        (DRF_SHIFTMASK(NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_ACTION) |
        DRF_SHIFTMASK(NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_INCREMENT) |
        DRF_SHIFTMASK(NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_INCREMENT) |
        DRF_SHIFTMASK(NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT))
        & internalParams.action;

    return NV_OK;
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams
)
{
    NvU8 rand;
    NvU32 internalCmd = (FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8);

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // Let the randomly selected internal API update the counters other than the per
    // client counter.
    //
    pParams->action =
        ((rand &
          (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT))) |
         (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION)));

    //
    // Pick a random control call to call internally between the following:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE
    //
    internalCmd |= ((rand & 3) +
        NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS_MESSAGE_ID);

    return updateLockStressCountersInternal(pResource,
        rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL),
        internalCmd, &pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams
)
{
    NvU8 rand;
    NvU32 internalCmd = (FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8);

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // Let the randomly selected internal API update the counters other than the per
    // client counter.
    //
    pParams->action =
        ((rand &
          (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT))) |
         (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION)));

    //
    // Pick a random control call to call internally between the following:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE
    //
    internalCmd |= ((rand & (NVBIT(2)- 1)) +
        NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS_MESSAGE_ID);

    return updateLockStressCountersInternal(pResource,
        rmapiGetInterface(RMAPI_API_LOCK_INTERNAL),
        internalCmd, &pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams
)
{
    NvU8 rand;
    NvU32 internalCmd = (FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8);

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // Let the randomly selected internal API update the counters other than the per
    // client counter.
    //
    pParams->action =
        ((rand &
          (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT))) |
         (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION)));

    //
    // Pick a random control call to call internally between the following:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE
    // We only have the RO API lock so we can't call:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK
    //
    internalCmd |= ((rand & NVBIT(0)) +
        NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID);

    return updateLockStressCountersInternal(pResource, 
        rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL),
        internalCmd, &pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams
)
{
    NvU8 rand;
    NvU32 internalCmd = (FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8);

    // Perform random increments/decrements but report what we did back to caller
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, osGetRandomBytes(&rand, 1));

    //
    // Let the randomly selected internal API update the counters other than the per
    // client counter.
    //
    pParams->action =
        ((rand &
          (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT))) |
         (DRF_SHIFTMASK(NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION)));
    //
    // Pick a random control call to call internally between the following:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE
    // We only have the RO API lock so we can't call:
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS
    // * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK
    //
    internalCmd |= ((rand & NVBIT(0)) +
        NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID);

    return updateLockStressCountersInternal(pResource,
        rmapiGetInterface(RMAPI_API_LOCK_INTERNAL),
        internalCmd, &pParams->action);
}

NV_STATUS
lockStressObjCtrlCmdGetLockStressCounters_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams
)
{
    RsClient *pClient = RES_GET_CLIENT(pResource);
    RmClient *pRmClient = dynamicCast(pClient, RmClient);
    RmClient *pRmInternalClient;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pResource);

    if (pRmClient == NULL)
        return NV_ERR_INVALID_STATE;

    pRmInternalClient = serverutilGetClientUnderLock(pGpu->hInternalLockStressClient);
    if (pRmInternalClient == NULL)
        return NV_ERR_INVALID_STATE;

    // Fetch all lock stress counter values for user space caller
    pParams->globalLockStressCounter = g_LockStressCounter;

    pParams->gpuLockStressCounter = pGpu->lockStressCounter;
    pParams->clientLockStressCounter = pRmClient->lockStressCounter;
    pParams->internalClientLockStressCounter = pRmInternalClient->lockStressCounter;

    return NV_OK;
}

typedef enum _GPU_LOCK_STATE
{
    _GPU_LOCK_STATE_NONE,
    _GPU_LOCK_STATE_DEVICE_LOCK,
    _GPU_LOCK_STATE_ALL_LOCK
} _GPU_LOCK_STATE;

static NV_STATUS
_verifyGpuLockState
(
    OBJGPU *pGpu,
    NvHandle hClient,
    NvHandle hSubdevice,
    NvHandle hLockStressObject,
    const _GPU_LOCK_STATE lockState
)
{
    NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM internalParam = {0};
    NV0080_ALLOC_PARAMETERS nv0080AllocParams = {0};
    NV00DE_ALLOC_PARAMETERS nv00deAllocParams = {0};
    NvHandle hDevice = NV01_NULL_OBJECT;
    NvHandle hRusd = NV01_NULL_OBJECT;
    NvHandle hRelaxedDupObj = NV01_NULL_OBJECT;
    RM_API *pRmApi = NULL;
    NV_STATUS status = NV_OK;
    NV_STATUS expectedStatus = NV_OK;
    NvU32 gpuMask = 0;
    const NvBool bMultiGpu = (SYS_GET_GPUMGR(SYS_GET_INSTANCE())->gpuAttachCount > 1);

    if (lockState == _GPU_LOCK_STATE_NONE)
    {
        pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    }
    else
    {
        pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    }

    // Test RM Control

    portMemSet(&internalParam, 0, sizeof(internalParam));
    internalParam.bLeaf = NV_TRUE;

    // NO_GPUS_LOCK RMCTRL is always supported regardless of the lock state
    status = pRmApi->Control(pRmApi, hClient, hLockStressObject,
                             NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_NO_LOCK,
                             &internalParam, sizeof(internalParam));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "No GPU lock control failed with: 0x%x\n", status);
        goto cleanup;
    }

    //
    // GPU lock for RMCTRL works differently than other RMAPIs.
    //
    // For API_LOCK_INTERNAL, the resource server takes the GPU lock required for
    // the RMCTRL. Thread holds the exact lock when it reaches the control function.
    //
    // For GPU_LOCK_INTERNAL RMCTRL, the resource server neither check nor change
    // the locking state. _rmapiRmControl set RM_LOCK_FLAGS_NO_GPUS_LOCK, which
    // makes serverControlLookupLockFlags cleans RM_LOCK_FLAGS_GPU_GROUP_LOCK.
    // As a result, serverReslock_Prologue becomes NOP. When the thread reaches
    // the control function body, the GPU lock state for a
    // GPU_LOCK_INTERNAL RMCTRL remains the same as before the call.
    //
    // Therefore, when a thread holds the device only GPU lock and calls this
    // RMCTRL, we reaches the function body with device only GPU lock and fails
    // the check for all GPUs lock when there's multiple GPUs in the system
    // (i.e., when the device only GPU lock does not equal to the all GPUs lock).
    //
    expectedStatus = (bMultiGpu && (lockState == _GPU_LOCK_STATE_DEVICE_LOCK)) ?
                         NV_ERR_INVALID_LOCK_STATE : NV_OK;
    status = pRmApi->Control(pRmApi, hClient, hLockStressObject,
                            NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_ALL_LOCK,
                            &internalParam, sizeof(internalParam));
    if (status != expectedStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "All GPUs lock control failed with: 0x%x, expected: 0x%x\n",
                  status, expectedStatus);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // A device only RMCTRL always succeeds for the lock states we have here.
    // For _GPU_LOCK_STATE_NONE, it goes through API_LOCK_INTERNAL where the
    // resource server takes care the GPU lock acquisition.
    //
    // For _GPU_LOCK_STATE_DEVICE_LOCK/_GPU_LOCK_STATE_ALL_LOCK, the device lock
    // is already held by the caller.
    //
    // This device only GPU lock will only fail when it's called through the
    // _GPU_LOCK_STATE_DEVICE_LOCK interface without holding the device lock for
    // this GPU (e.g., a cross-GPU RMCTRL call).
    //
    status = pRmApi->Control(pRmApi, hClient, hLockStressObject,
                             NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_DEVICE_LOCK,
                             &internalParam, sizeof(internalParam));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Device lock control failed with: 0x%x\n", status);
        goto cleanup;
    }

    // Test RM Alloc

    // Skip since on vGPU only one device with a device instance is allowed
    if (!IS_VIRTUAL(pGpu))
    {
        portMemSet(&nv0080AllocParams, 0, sizeof(nv0080AllocParams));
        nv0080AllocParams.deviceId = gpuGetDeviceInstance(pGpu);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(hClient, &hDevice), cleanup);

        //
        // Test all GPUs lock RM alloc.
        //
        // For alloc, resource server checks the lock state in serverResLock_Prologue.
        // If we hold all GPUs lock, no further lock acqusition required.
        // If we hold GPU device lock in multi-GPU setup, serverResLock_Prologue
        // detects recursive lock acquisition and fails the request.
        //

        expectedStatus = (bMultiGpu && (lockState == _GPU_LOCK_STATE_DEVICE_LOCK)) ?
                            NV_ERR_INVALID_LOCK_STATE : NV_OK;
        status = pRmApi->AllocWithHandle(pRmApi, hClient, hClient, hDevice, NV01_DEVICE_0,
                                        &nv0080AllocParams, sizeof(nv0080AllocParams));
        if (status != NV_OK)
        {
            // No need to free the object
            hDevice = NV01_NULL_OBJECT;
        }
        if (status != expectedStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Failed to allocate device with: 0x%x, expected: 0x%x\n",
                    status, expectedStatus);
            status = NV_ERR_INVALID_LOCK_STATE;
            goto cleanup;
        }
    }

    NV_ASSERT_OK_OR_GOTO(status,
        serverutilGenResourceHandle(hClient, &hRusd), cleanup);

    //
    // Test device only GPU lock RM alloc
    // No further lock acquisition required for either _GPU_LOCK_STATE_DEVICE_LOCK
    // or _GPU_LOCK_STATE_ALL_LOCK.
    //
    // For _GPU_LOCK_STATE_NONE, resource server handles the GPU lock since it
    // goes through API_LOCK_INTERNAL RMAPI.
    //
    // For vGPU, RUSD allocation will return NV_ERR_NOT_SUPPORTED in ctor.
    // We can still verify the resource server locking on vGPU.
    //
    expectedStatus = IS_VIRTUAL(pGpu) ? NV_ERR_NOT_SUPPORTED : NV_OK;
    status = pRmApi->AllocWithHandle(pRmApi, hClient, hSubdevice, hRusd, RM_USER_SHARED_DATA,
                                     &nv00deAllocParams, sizeof(nv00deAllocParams));
    if (status != NV_OK)
    {
        hRusd = NV01_NULL_OBJECT;
    }
    if (status != expectedStatus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to allocate RUSD with: 0x%x, expected: 0x%x\n",
                  status, expectedStatus);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // Allocation to test free with all GPUs locks under _GPU_LOCK_STATE_DEVICE_LOCK.
    //
    // Above device allocation will fail with _GPU_LOCK_STATE_DEVICE_LOCK and
    // multi-GPU; leaving us no object to test free with all GPUs locks.
    //
    // This test-only object requires device only lock to alloc and all GPUs lock
    // to free.
    //
    NV_ASSERT_OK_OR_GOTO(status,
        serverutilGenResourceHandle(hClient, &hRelaxedDupObj), cleanup);
    status = pRmApi->AllocWithHandle(pRmApi, hClient, hSubdevice, hRelaxedDupObj,
                                     LOCK_TEST_RELAXED_DUP_OBJECT, NULL, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to allocate relaxed dup object with: 0x%x\n", status);
        hRelaxedDupObj = NV01_NULL_OBJECT;
        goto cleanup;
    }

    //
    // Test RM free.
    //

    //
    // Test RM Free with device only GPU lock.
    //
    // RM Free goes through serverResLock_Prologue, the same as the RM Alloc.
    // The locking requirements are the same as those for RM alloc.
    //
    if (hRusd != NV01_NULL_OBJECT)
    {
        status = pRmApi->Free(pRmApi, hClient, hRusd);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Failed to free subdevice with: 0x%x\n", status);
            goto cleanup;
        }

        hRusd = NV01_NULL_OBJECT;
    }

    //
    // Test RM Free with all GPUs lock.
    // We won't fail this case. If the device was allocated successfully,
    // it should be able to be freed successfully.
    //
    if (hDevice != NV01_NULL_OBJECT)
    {
        status = pRmApi->Free(pRmApi, hClient, hDevice);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to free device with: 0x%x,\n", status);
            goto cleanup;
        }

        hDevice = NV01_NULL_OBJECT;
    }

    // 
    // Test RM Free with all GPUs lock.
    // Fails if we hold device only lock in multi-GPU setup due to locking order
    // violation.
    //
    if (hRelaxedDupObj != NV01_NULL_OBJECT)
    {
        expectedStatus = (bMultiGpu && (lockState == _GPU_LOCK_STATE_DEVICE_LOCK)) ?
                            NV_ERR_INVALID_LOCK_STATE : NV_OK;
        status = pRmApi->Free(pRmApi, hClient, hRelaxedDupObj);
        if (status == NV_OK)
        {
            hRelaxedDupObj = NV01_NULL_OBJECT;
        }

        if (status != expectedStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Failed to free relaxed dup object with: 0x%x, expected: 0x%x\n",
                    status, expectedStatus);
            status = NV_ERR_INVALID_LOCK_STATE;
            goto cleanup;
        }
    }

    status = NV_OK;

cleanup:
    // Release all locks we have for clean up
    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    pRmApi = rmapiGetInterface(RMAPI_API_LOCK_INTERNAL);
    if (hDevice != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, hClient, hDevice);
        hDevice = NV01_NULL_OBJECT;
    }
    if (hRusd != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, hClient, hRusd);
        hRusd = NV01_NULL_OBJECT;
    }
    if (hRelaxedDupObj != NV01_NULL_OBJECT)
    {
        pRmApi->Free(pRmApi, hClient, hRelaxedDupObj);
        hRelaxedDupObj = NV01_NULL_OBJECT;
    }

    // reset lock state
    if (lockState == _GPU_LOCK_STATE_DEVICE_LOCK)
    {
        if (rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
            GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to reset to device only GPU Lock");
        }
    }
    else if (lockState == _GPU_LOCK_STATE_ALL_LOCK)
    {
        if (rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_ALL,
            GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to reset to all GPUs Lock");
        }
    }

    return status;
}

static NV_STATUS
_verifyGpuLockingOrder
(
    OBJGPU *pGpu
)
{
    NvBool bMultiGpu = (SYS_GET_GPUMGR(SYS_GET_INSTANCE())->gpuAttachCount > 1);
    NvU32 gpuMask = 0;
    OBJGPU *pGpu0 = NULL;
    OBJGPU *pGpu1 = NULL;
    NV_STATUS status = NV_OK;

    // Only support locking order testing under no GPU lock in multi-GPU setup
    if ((rmGpuLocksGetOwnedMask() != 0) || !bMultiGpu)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpu0 = gpumgrGetGpu(0);
    pGpu1 = gpumgrGetGpu(1);

    // Lock GPU0
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire GPU0 device lock with: 0x%x\n", status);
        return status;
    }

    //
    // Lock GPU0 again and expects to fail.
    // Recursivie lock acquisition is not allowed for normal lock acquire.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Recursive GPU0 device lock acquisition succeeded"
                               " with: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // Conditionally lock GPU0 again and expects to fail.
    // Recursivie lock acquisition is not allowed for conditional lock acquire.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPU_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_STATE_IN_USE)
    {
        NV_PRINTF(LEVEL_ERROR, "Recursive GPU0 device lock acquisition succeeded"
                               " with: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // Safe upgrade to lock GPU0 again and expect to succeed.
    // Safe upgrade for recursive lock acquisition became NOP and will succeed.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Safe lock upgrade for recursive GPU0 device lock"
                               " failed with: 0x%x\n", status);
        goto cleanup;
    }

    //
    // Lock GPU1 and expect to succeed.
    // Normal lock acquisition following the locking order should succeed.
    //
    status = rmGpuGroupLockAcquire(pGpu1->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire GPU1 device lock with correct "
                               "locking order, status: 0x%x\n", status);
        goto cleanup;
    }

    // Release all locks
    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    //
    // Test locking order violation
    // Lock GPU1.
    //
    status = rmGpuGroupLockAcquire(pGpu1->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire GPU1 device lock with: 0x%x\n", status);
        goto cleanup;
    }

    //
    // Lock GPU0 and expect to fail.
    // Normal lock acquisition that violates locking order should fail.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU0 lock acqusition that violates locking order "
                               "does not fail. status: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // Conditionally lock GPU0 and expect to get OK or STATE_IN_USE.
    // Conditional lock acquisition that violates locking order may succeed.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPU_LOCK_FLAGS_COND_ACQUIRE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if ((status != NV_OK) && (status != NV_ERR_STATE_IN_USE))
    {
        NV_PRINTF(LEVEL_ERROR, "Conditional lock acquisition that violates locking"
                               " order should not fail. status: 0x%x\n", status);
        goto cleanup;
    }
    else if (status == NV_OK)
    {
        // Unlock GPU0 if conditional lock acquisition succeeded.
        rmGpuGroupLockRelease(0x1, GPUS_LOCK_FLAGS_NONE);
    }

    //
    // Safe upgrade to lock GPU0 and expect to get OK or STATE_IN_USE.
    // Safe lock upgrade that violates locking order becomes a conditoinal acquire.
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if ((status != NV_OK) && (status != NV_ERR_STATE_IN_USE))
    {
        NV_PRINTF(LEVEL_ERROR, "Safe lock upgrade that violates locking order "
                               "should not fail. status: 0x%x\n", status);
        goto cleanup;
    }

    //
    // Testing the locking order violation for alloc lock.
    // Lock alloc lock and expect to fail due to locking order.
    //
    gpuMask = 0;
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_LOCK_ALLOC, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Alloc lock acquisition that violates locking "
                               "order did not fail. status: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    // Conditional alloc lock acquistion that violates locking order can succeed
    gpuMask = 0;
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_LOCK_ALLOC | GPU_LOCK_FLAGS_COND_ACQUIRE,
                RM_LOCK_MODULES_CLIENT, &gpuMask);
    if ((status != NV_ERR_STATE_IN_USE) && (status != NV_OK))
    {
        NV_PRINTF(LEVEL_ERROR, "Alloc lock acquisition that violates locking "
                               "order did not fail. status: 0x%x\n", status);
        goto cleanup;
    }

    //
    // No API to release the alloc lock only (Alloc lock is only used internally)
    // Release all locks.
    //
    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    //
    // Test safe upgrade for alloc lock
    // Lock GPU0
    //
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire GPU0 device lock with: 0x%x\n", status);
        goto cleanup;
    }

    //
    // Safe lock upgrade for alloc lock that violates locking order may succeed
    // Safe lock upgrade that violates locking order becomes a conditoinal acquire.
    //
    gpuMask = 0;
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_LOCK_ALLOC | GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                RM_LOCK_MODULES_CLIENT, &gpuMask);
    if ((status != NV_OK) && (status != NV_ERR_STATE_IN_USE))
    {
        NV_PRINTF(LEVEL_ERROR, "Safe lock upgrade for alloc lock that violates"
                               "locking order failed. status: 0x%x\n", status);
        goto cleanup;
    }

    // Release all locks.
    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    // Lock all GPUs. This also locks the alloc lock.
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_ALL,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire all GPUs lock. status: 0x%x\n", status);
        goto cleanup;
    }

    // Lock the alloc lock and expect to fail due to recursive lock acquisition.
    gpuMask = 0;
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_LOCK_ALLOC, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Recursive alloc lock acquisition does not "
                               "fail with invalid lock state. state: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    //
    // Safe upgrade the alloc lock. Expect to succeed.
    // Safe upgrade for recursive lock acquisition becomes NOP and will succeed.
    //
    gpuMask = 0;
    status = rmGpuGroupLockAcquire(pGpu0->gpuInstance, GPU_LOCK_GRP_MASK,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE | GPU_LOCK_FLAGS_LOCK_ALLOC,
                RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Recursive alloc lock safe upgrade failed with 0x%x\n", status);
        goto cleanup;
    }

    status = NV_OK;

cleanup:
    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    return status;

}

NV_STATUS
lockStressObjCtrlCmdRecursiveGpuLockTestNoLock_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pResource);
    NV_STATUS status = NV_OK;
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pResource);
    NvHandle hSubdevice = RES_GET_PARENT_HANDLE(pResource);
    NvHandle hLockStressObject = RES_GET_HANDLE(pResource);
    NvBool bMultiGpu = (SYS_GET_GPUMGR(SYS_GET_INSTANCE())->gpuAttachCount > 1);

    NvU32 gpuMask = 0;

    // Basic Lock Model is not supported for GSP.
    if(RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // if this is leaf function, verify that we hold at least the required lock
    if (pParams->bLeaf)
    {
        // no GPU lock needed, nothing to check
        return NV_OK;
    }

    // Only testing locking order for multi-GPU setup.
    if (bMultiGpu)
    {
        NV_ASSERT_OK_OR_RETURN(
            _verifyGpuLockingOrder(pGpu));
    }

    NV_ASSERT_OK_OR_RETURN(
        _verifyGpuLockState(pGpu, hClient, hSubdevice, hLockStressObject, _GPU_LOCK_STATE_NONE));

    // Try acquire GPU device lock. Expected to work.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire GPU device lock with: 0x%x\n", status);
        goto cleanup;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        _verifyGpuLockState(pGpu, hClient, hSubdevice, hLockStressObject,
                            _GPU_LOCK_STATE_DEVICE_LOCK),
        cleanup);

    // Try acquire all GPUs lock. Expected to fail since we hold device only lock.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_ALL,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "All GPUs lock attempt under GPU Device Lock "
                               " should be invalid, status: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    // Re-acquire all GPUs lock. Expected to work.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_ALL,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire all GPUs lock with: 0x%x\n", status);
        goto cleanup;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        _verifyGpuLockState(pGpu, hClient, hSubdevice, hLockStressObject,
                            _GPU_LOCK_STATE_ALL_LOCK),
        cleanup);

    // Try acquire device lock. Expected to fail since we hold all GPUs lock.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Device lock attempt under All GPUs Lock "
                               " should be invalid, status: 0x%x\n", status);
        status = NV_ERR_INVALID_LOCK_STATE;
        goto cleanup;
    }

    status = NV_OK;

cleanup:

    rmGpuGroupLockRelease(rmGpuLocksGetOwnedMask(), GPUS_LOCK_FLAGS_NONE);

    return status;
}

NV_STATUS
lockStressObjCtrlCmdRecursiveGpuLockTestAllLock_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pResource);
    NV_STATUS status = NV_OK;
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pResource);
    NvHandle hSubdevice = RES_GET_PARENT_HANDLE(pResource);
    NvHandle hLockStressObject = RES_GET_HANDLE(pResource);

    NvU32 gpuMask = 0;
    if(RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // if this is leaf function, verify that we hold at least the required lock
    if (pParams->bLeaf)
    {
        if (rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_ALL, &gpuMask))
        {
            return NV_OK;
        }
        else
        {
            //
            // This can happen because resource server GPU lock checking is
            // skipped for GPU_LOCK_INTERNAL RMAPI. 
            //
            NV_PRINTF(LEVEL_ERROR, "Failed to hold all GPUs lock\n");
            return NV_ERR_INVALID_LOCK_STATE;
        }
    }

    NV_ASSERT_OK_OR_RETURN(_verifyGpuLockState(pGpu, hClient, hSubdevice, hLockStressObject, _GPU_LOCK_STATE_ALL_LOCK));

    // Test taking all GPUs lock when we already hold all GPUs lock. Expected to fail.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_ALL,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "All GPUs lock attempt succeeded, status: 0x%x\n", status);
        return NV_ERR_INVALID_LOCK_STATE;
    }

    // Test taking device lock when we already hold all GPUs lock. Expected to fail.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Device lock attempt succeeded, status: 0x%x\n", status);
        return NV_ERR_INVALID_LOCK_STATE;
    }

    return NV_OK;
}

NV_STATUS
lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock_IMPL
(
    LockStressObject *pResource,
    NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pResource);
    NV_STATUS status = NV_OK;
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pResource);
    NvHandle hSubdevice = RES_GET_PARENT_HANDLE(pResource);
    NvHandle hLockStressObject = RES_GET_HANDLE(pResource);
    NvU32 gpuMask = 0;

    if(RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // if this is leaf function, verify that we hold at least the required lock
    if (pParams->bLeaf)
    {
        if (rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE, &gpuMask))
        {
            return NV_OK;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to hold the GPU device lock\n");
            return NV_ERR_INVALID_LOCK_STATE;
        }
    }

    NV_ASSERT_OK_OR_RETURN(
        _verifyGpuLockState(pGpu, hClient, hSubdevice, hLockStressObject, _GPU_LOCK_STATE_DEVICE_LOCK));

    // Test taking all GPUs lock when we already hold GPU device lock. Expected to fail.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_ALL,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "All GPUs lock attempt succeeded, status: 0x%x\n", status);

        // In case we did succeeded, release the lock and return error.
        if (status == NV_OK)
        {
            rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
        }

        return NV_ERR_INVALID_LOCK_STATE;
    }

    // Test taking device lock when we already hold GPU device lock. Expected to fail.
    status = rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_DEVICE,
                GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_CLIENT, &gpuMask);
    if (status != NV_ERR_INVALID_LOCK_STATE)
    {
        NV_PRINTF(LEVEL_ERROR, "Device lock attempt succeeded, status: 0x%x\n", status);

        // In case we did succeeded, release the lock and return error.
        if (status == NV_OK)
        {
            rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
        }

        return NV_ERR_INVALID_LOCK_STATE;
    }

    return NV_OK;
}