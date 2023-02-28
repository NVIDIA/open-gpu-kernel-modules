/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <rmconfig.h>

#include <nvlog_inc.h>
#include <nv.h>
#include <nv-priv.h>
#include <nv-reg.h>
#include <nv_ref.h>

#include <osapi.h>

#include <gpu/mem_mgr/mem_mgr.h>
#include <core/locks.h>
#include "kernel/gpu/intr/intr.h"

//
// Function to update fixed fbsr modes to support multiple vairants such as
// GCOFF and cuda S3/resume.
//
static void
RmUpdateFixedFbsrModes(OBJGPU *pGpu)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    nv_state_t    *nv             = NV_GET_NV_STATE(pGpu);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_GCOFF_STATE_ENTERING))
    {
        pMemoryManager->fixedFbsrModesMask = NVBIT(FBSR_TYPE_DMA);
    }
    else if (nv->preserve_vidmem_allocations)
    {
        pMemoryManager->fixedFbsrModesMask = NVBIT(FBSR_TYPE_FILE);
    }
}

static NV_STATUS
RmPowerManagementInternal(
    OBJGPU *pGpu,
    nv_pm_action_t pmAction
)
{
    // default to NV_OK. there may cases where resman is loaded, but
    // no devices are allocated (we're still at the console). in these
    // cases, it's fine to let the system do whatever it wants.
    NV_STATUS rmStatus = NV_OK;

    if (pGpu)
    {
        nv_state_t *nv = NV_GET_NV_STATE(pGpu);
        nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);
        NvBool bcState = gpumgrGetBcEnabledStatus(pGpu);
        Intr *pIntr = GPU_GET_INTR(pGpu);

        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        if ((pmAction == NV_PM_ACTION_HIBERNATE) || (pmAction == NV_PM_ACTION_STANDBY))
        {
            //
            // pFb object store the FBSR mode through which FB state unload has happened,
            // so os layer doesn't need to set FBSR mode on resume.
            //
            RmUpdateFixedFbsrModes(pGpu);
        }

        switch (pmAction)
        {
            case NV_PM_ACTION_HIBERNATE:
                nvp->pm_state.InHibernate = NV_TRUE;
                nvp->pm_state.IntrEn = intrGetIntrEn(pIntr);
                intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                rmStatus = gpuEnterHibernate(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, bcState);

                break;

            case NV_PM_ACTION_STANDBY:
                nvp->pm_state.InHibernate = NV_FALSE;
                nvp->pm_state.IntrEn = intrGetIntrEn(pIntr);
                intrSetIntrEn(pIntr, INTERRUPT_TYPE_DISABLED);
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                rmStatus = gpuEnterStandby(pGpu);
                gpumgrSetBcEnabledStatus(pGpu, bcState);

                break;

            case NV_PM_ACTION_RESUME:
                gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

                if (nvp->pm_state.InHibernate)
                {
                    gpuResumeFromHibernate(pGpu);
                }
                else
                {
                    gpuResumeFromStandby(pGpu);
                }
                intrSetIntrEn(pIntr, nvp->pm_state.IntrEn);
                gpumgrSetBcEnabledStatus(pGpu, bcState);
                NvU32 ac_plugged = 0;

                if (IsMobile(pGpu))
                {
                    if (nv_acpi_get_powersource(&ac_plugged) == NV_OK)
                    {
                        //
                        // As we have already acquired API and GPU lock here, we are
                        // directly calling RmPowerSourceChangeEvent.
                        //
                        RmPowerSourceChangeEvent(nv, !ac_plugged);
                    }
                }
                break;

            default:
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                break;
        }
        pMemoryManager->fixedFbsrModesMask = 0;
    }

    return rmStatus;
}

static NV_STATUS
RmPowerManagement(
    OBJGPU *pGpu,
    nv_pm_action_t pmAction
)
{
    NV_STATUS   rmStatus;

    rmStatus = RmPowerManagementInternal(pGpu, pmAction);

    return rmStatus;
}

NV_STATUS NV_API_CALL rm_power_management(
    nvidia_stack_t *sp,
    nv_state_t *pNv,
    nv_pm_action_t pmAction
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus = NV_OK;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue));

    // LOCK: acquire API lock
    if ((rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER)) == NV_OK)
    {
        OBJGPU *pGpu = NV_GET_NV_PRIV_PGPU(pNv);

        if (pGpu != NULL)
        {
            if ((rmStatus = os_ref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE)) == NV_OK)
            {
                // LOCK: acquire GPUs lock
                if ((rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_DYN_POWER)) == NV_OK)
                {
                    {
                        rmStatus = RmPowerManagement(pGpu, pmAction);
                    }

                    // UNLOCK: release GPUs lock
                    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
                }
                os_unref_dynamic_power(pNv, NV_DYNAMIC_PM_FINE);
            }
        }
        // UNLOCK: release API lock
        rmapiLockRelease();
    }

    NV_ASSERT_OK(os_flush_work_queue(pNv->queue));

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);

    return rmStatus;
}


