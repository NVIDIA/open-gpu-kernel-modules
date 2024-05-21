/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_lock_stress_nvoc.h"

#ifndef LOCK_STRESS_H
#define LOCK_STRESS_H

#include "gpu/gpu_resource.h"
#include "nvoc/prelude.h"
#include "nvstatus.h"
#include "resserv/resserv.h"

#include "ctrl/ctrl0100.h"

NVOC_PREFIX(lockStressObj) class LockStressObject : GpuResource
{
public:
    NV_STATUS lockStressObjConstruct(LockStressObject *pResource,
                                     CALL_CONTEXT *pCallContext,
                                     RS_RES_ALLOC_PARAMS_INTERNAL *pParams) :
                                     GpuResource(pCallContext, pParams);

    void lockStressObjDestruct(LockStressObject *pResource);

    //
    // RMCTRL Exported methods -- Category: LOCK_STRESS
    //
    RMCTRL_EXPORT(NV0100_CTRL_CMD_RESET_LOCK_STRESS_STATE,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS lockStressObjCtrlCmdResetLockStressState(LockStressObject *pResource);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK,
                  RMCTRL_FLAGS(NON_PRIVILEGED, NO_GPUS_LOCK))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE,
                  RMCTRL_FLAGS(NON_PRIVILEGED, API_LOCK_READONLY))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE,
                  RMCTRL_FLAGS(NON_PRIVILEGED, NO_GPUS_LOCK, API_LOCK_READONLY))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK,
                  RMCTRL_FLAGS(NON_PRIVILEGED, NO_GPUS_LOCK))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE,
                  RMCTRL_FLAGS(NON_PRIVILEGED, API_LOCK_READONLY))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE,
                  RMCTRL_FLAGS(NON_PRIVILEGED, NO_GPUS_LOCK, API_LOCK_READONLY))
    NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode(LockStressObject *pResource,
        NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);

    RMCTRL_EXPORT(NV0100_CTRL_CMD_GET_LOCK_STRESS_COUNTERS,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS lockStressObjCtrlCmdGetLockStressCounters(LockStressObject *pResource,
        NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams);

private:

    // Internal RM objects for internal RM API invocation
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hInternalLockStressObject;
};

#endif // LOCK_STRESS_H
