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

#include "gpu/device/device.h"
#include "gpu/perf/kern_perf.h"
#include "core/locks.h"
#include "vgpu/rpc.h"
#include "nvRmReg.h"
#include "platform/sli/sli.h"

/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Class Interfaces ------------------------ */
/*!
 * Initialize SW state corresponding to SLI GPU Boost synchronization.
 *
 * @param[in]  pGpu         GPU object pointer
 * @param[in]  pKernelPerf  KernelPerf object pointer
 *
 * @return NV_OK
 */
NV_STATUS
kperfGpuBoostSyncStateInit_IMPL
(
    OBJGPU     *pGpu,
    KernelPerf *pKernelPerf
)
{
    NV_STATUS  status = NV_OK;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32      i;
    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO_PARAMS ctrlParams = { 0 };

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_GET_INFO,
                             &ctrlParams,
                             sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to read Sync Gpu Boost init state, status=0x%x\n",
                  status);
        goto kperfGpuBoostSyncStateInit_IMPL_exit;
    }

    pKernelPerf->sliGpuBoostSync.hysteresisus           = ctrlParams.hysteresisus;
    pKernelPerf->sliGpuBoostSync.bHystersisEnable       = ctrlParams.bHystersisEnable;
    pKernelPerf->sliGpuBoostSync.bSliGpuBoostSyncEnable = ctrlParams.bSliGpuBoostSyncEnable;

    // Initialize the GPU Boost synchronization limits.
    for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
    {
        pKernelPerf->sliGpuBoostSync.limits[i] = NV_U32_MAX;
    }

kperfGpuBoostSyncStateInit_IMPL_exit:
    return NV_OK;
}

/*!
 * @copydoc kperfGpuBoostSyncActivate
 */
NV_STATUS
kperfGpuBoostSyncActivate_IMPL
(
    OBJGPU      *pGpu,
    KernelPerf  *pKernelPerf,
    NvBool       bActivate
)
{
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status = NV_OK;
    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_CONTROL_PARAMS ctrlParams = { 0 };

    ctrlParams.bActivate = bActivate;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_CONTROL,
                             &ctrlParams,
                             sizeof(ctrlParams));

    return status;
}

/*!
 * Processes all GPU Boost PERF_LIMITs and applies most restrictive of them for
 * SLI GPU Boost synchronization.
 *
 * @param[in]   pGpu    OBJGPU pointer
 * @param[in]   pParams
 *
 * @return NV_OK
 *      Synchronized PERF_LIMITs successfully applied or removed.
 * @return Other unexpected errors
 *      Unexpected errors propagated from other functions.
 */
NV_STATUS
kperfDoSyncGpuBoostLimits_IMPL
(
    OBJGPU     *pGpu,
    KernelPerf *pKernelPerf,
    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams
)
{
    OBJSYS                 *pSys                = SYS_GET_INSTANCE();
    OBJGPUMGR              *pGpuMgr             = SYS_GET_GPUMGR(pSys);
    OBJGPUBOOSTMGR         *pBoostMgr           = SYS_GET_GPUBOOSTMGR(pSys);
    NV_STATUS               status              = NV_OK;
    NvU64                   currns              = 0;
    NvU64                   diffns              = 0;
    NvBool                  bUpdate             = NV_FALSE;
    NvBool                  bBridgeless         = NV_FALSE;
    NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS perfGpuBoostSyncParamsSet = { 0 };
    NvBool                  bBcState;
    NvU32                   grpId;
    NvU32                   i;

    bBcState = gpumgrGetBcEnabledStatus(pGpu);

    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    gpuboostmgrGetBoostGrpIdFromGpu(pBoostMgr, pGpu, &grpId);

    if (!gpuboostmgrIsBoostGrpActive(pBoostMgr, grpId))
    {
        return status;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, (pKernelPerf != NULL), NV_ERR_INVALID_POINTER);

    for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
    {
        pKernelPerf->sliGpuBoostSync.limits[i] = pParams->currLimits[i];
    }
    pKernelPerf->sliGpuBoostSync.bBridgeless = pParams->bBridgeless;

    portMemSet(perfGpuBoostSyncParamsSet.currLimits, NV_U8_MAX, sizeof(perfGpuBoostSyncParamsSet.currLimits));

    //
    //  NOTE:
    //  One will see a pattern of forks in this file:
    //      if (boost group active)
    //          Gpu Boost Loop
    //      else
    //          SLI Loop
    //  This is a temporary change to introduce Sync Gpu boost Manager to the SLI Boost framework.
    //  The goal eventually is to replace SLI GPU Boost with Sync GPU Boost.
    //  WDDM KMD and UMDs needs to change for that
    //

    if (gpuboostmgrIsBoostGrpActive(pBoostMgr, grpId))
    {
        OBJGPU *pGpuItr = NULL;

        GPUBOOSTMGR_ITR_START(pBoostMgr, grpId, pGpuItr)
        {
            pKernelPerf = GPU_GET_KERNEL_PERF(pGpuItr);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, (pKernelPerf != NULL), NV_ERR_INVALID_POINTER);

            // Find min of all GPU Boost PERF_LIMITs across all the GPUs.
            for (i = 0; i <  NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
            {
               perfGpuBoostSyncParamsSet.currLimits[i] = NV_MIN(perfGpuBoostSyncParamsSet.currLimits[i], pKernelPerf->sliGpuBoostSync.limits[i]);
            }

            if (pKernelPerf->sliGpuBoostSync.bBridgeless)
            {
               bBridgeless = NV_TRUE;
            }

        }
        GPUBOOSTMGR_ITR_END
    }
    else
    {
    }

    // Enable hysteresis algorithm, if required.
    if ((pKernelPerf != NULL) &&
        (pKernelPerf->sliGpuBoostSync.bHystersisEnable))
    {
        // Get current tick.
        osGetPerformanceCounter(&currns);

        for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
        {
            // If GPU Boost PERF_LIMITs are being lowered, immediately synchronize.
            if (perfGpuBoostSyncParamsSet.currLimits[i] < pGpuMgr->sliGpuBoostSync.prevLimits[i])
            {
                bUpdate = NV_TRUE;
            }
            else if (perfGpuBoostSyncParamsSet.currLimits[i] > pGpuMgr->sliGpuBoostSync.prevLimits[i])
            {
                // Otherwise, synchronize only if specified time has been elapsed.
                diffns = currns - pGpuMgr->sliGpuBoostSync.prevChangeTsns;

                if ((diffns / 1000) > pKernelPerf->sliGpuBoostSync.hysteresisus)
                {
                    bUpdate = NV_TRUE;
                }
            }
        }

        // Update previous history and apply SLI GPU Boost PERF_LIMITs.
        if (bUpdate)
        {
            pGpuMgr->sliGpuBoostSync.prevChangeTsns = currns;

            for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
            {
                pGpuMgr->sliGpuBoostSync.prevLimits[i] = perfGpuBoostSyncParamsSet.currLimits[i];
            }
        }
        else
        {
            return status;
        }
    }

    perfGpuBoostSyncParamsSet.flags       = pParams->flags;
    perfGpuBoostSyncParamsSet.bBridgeless = bBridgeless;

    if (gpuboostmgrIsBoostGrpActive(pBoostMgr, grpId))
    {
        OBJGPU *pGpuItr = NULL;

        GPUBOOSTMGR_ITR_START(pBoostMgr, grpId, pGpuItr)
        {
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

            status = pRmApi->Control(pRmApi,
                                     pGpuItr->hInternalClient,
                                     pGpuItr->hInternalSubdevice,
                                     NV2080_CTRL_CMD_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS,
                                     &perfGpuBoostSyncParamsSet,
                                     sizeof(perfGpuBoostSyncParamsSet));
        }
        GPUBOOSTMGR_ITR_END
    }
    else
    {
    }

    gpumgrSetBcEnabledStatus(pGpu, bBcState);

    return status;
}

/*!
 * Update the bridgeless info.
 *
 * @param[in]  pGpu         GPU object pointer
 * @param[in]  bBridgeless  Latest bridgeless info that should be set
 *
 * @return NV_OK
 */
NV_STATUS
kPerfGpuBoostSyncBridgelessUpdateInfo
(
    OBJGPU *pGpu,
    NvBool  bBridgeless
)
{
    KernelPerf *pKernelPerf = GPU_GET_KERNEL_PERF(pGpu);

    if (pKernelPerf != NULL)
    {
        pKernelPerf->sliGpuBoostSync.bBridgeless = bBridgeless;
    }

    return NV_OK;
}

/*!
 * Helper routine to toggle the state of Sync Gpu Boost Algorithm using SGBG infrastructure
 * @param[in] bActivate  NV_TRUE if we want to turn the algorithm on, NV_FALSE otherwise
 */
NV_STATUS
kperfGpuBoostSyncStateUpdate
(
   OBJGPUBOOSTMGR *pBoostMgr,
   NvU32           boostGroupId,
   NvBool          bActivate
)
{
    OBJSYS        *pSys         = SYS_GET_INSTANCE();
    KernelPerf    *pKernelPerf  = NULL;
    NV_STATUS      status       = NV_OK;
    NvU32          i;

    // No need to activate again if refCount is greater than 1.
    if (1 < pBoostMgr->pBoostGroups[boostGroupId].refCount)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Trying to activate and already active Sync GPU Boost Group = 0x%08x.\n",
                  boostGroupId);
        DBG_BREAKPOINT();
        return status;
    }

    //
    // Trigger state change per GPU
    // We need to acquire GPU locks here as this will tough GPU state.
    //

    // LOCK: acquire GPUs lock
    if (NV_OK == rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_GPU))
    {
        if (NV_OK == osAcquireRmSema(pSys->pSema))
        {
            OBJGPU       *pGpuItr      = NULL;

            GPUBOOSTMGR_ITR_START(pBoostMgr, boostGroupId, pGpuItr)
            {
                pKernelPerf = GPU_GET_KERNEL_PERF(pGpuItr);

                if ((pKernelPerf != NULL) &&
                    (pKernelPerf->sliGpuBoostSync.bSliGpuBoostSyncEnable))
                {
                    status = kperfGpuBoostSyncActivate(pGpuItr, pKernelPerf, bActivate);

                    if (status == NV_OK)
                    {
                       for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
                        {
                            pKernelPerf->sliGpuBoostSync.limits[i] = NV_U32_MAX;
                        }
                    }
                }
                else
                {
                    status = NV_ERR_INVALID_STATE;
                }
                if (NV_OK != status)
                {
                    OBJGPU *pGpuItr2 = NULL;
                    NV_PRINTF(LEVEL_ERROR,
                              "Failed to toggle Sync Gpu Boost state on Gpu 0x%08x\n",
                              pGpuItr->gpuId);
                    DBG_BREAKPOINT();

                    // Toggle back the Sync Gpu Boost state of all the GPUs so far
                    GPUBOOSTMGR_ITR_START(pBoostMgr, boostGroupId, pGpuItr2)
                    {
                        pKernelPerf    = GPU_GET_KERNEL_PERF(pGpuItr2);

                        if ((pKernelPerf != NULL) &&
                            (pKernelPerf->sliGpuBoostSync.bSliGpuBoostSyncEnable))
                        {
                            status = kperfGpuBoostSyncActivate(pGpuItr2, pKernelPerf, !bActivate);

                            if (status == NV_OK)
                            {
                               for (i = 0; i < NV2080_CTRL_INTERNAL_PERF_SYNC_GPU_BOOST_LIMITS_NUM; i++)
                                {
                                    pKernelPerf->sliGpuBoostSync.limits[i] = NV_U32_MAX;
                                }
                            }
                        }
                        // Intentionaly ignoring the status as we want to rollback the algorithm
                        // activation and return previously failing status

                        if (pGpuItr == pGpuItr2)
                        {
                            // break from unwind/cleanup loop
                            break;
                        }
                    }
                    GPUBOOSTMGR_ITR_END

                    // break from outer iterator
                    break;
                }
            }
            GPUBOOSTMGR_ITR_END
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "OS Semaphore acquire failed\n");
            status = NV_ERR_STATE_IN_USE;
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            goto kperfSliGpuBoostSyncStateUpdate_exit;
        }

        // UNLOCK: release GPUs lock
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "GPU lock acquire failed\n");
        status = NV_ERR_STATE_IN_USE;
        goto kperfSliGpuBoostSyncStateUpdate_exit;
    }

kperfSliGpuBoostSyncStateUpdate_exit:
    return status;
}
