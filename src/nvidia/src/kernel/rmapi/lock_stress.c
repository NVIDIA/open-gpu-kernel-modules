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
