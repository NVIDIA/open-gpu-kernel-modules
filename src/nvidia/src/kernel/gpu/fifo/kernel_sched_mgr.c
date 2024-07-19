/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief  KernelSchedMgr object module
 *
 * This module is for managing and manipulating state related to KernelSchedMgr
 */

/* -------------------------------- Includes -------------------------------- */

#include "os/os.h"
#include "nvrm_registry.h"

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_sched_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "virtualization/hypervisor/hypervisor.h"

/* -------------------------------- Functions ------------------------------- */

/*!
 *  Obtains the valid scheduling policy for the current platform.
 *  Use: Determine whether software scheduling is required.
 */
static const char *
_kschedmgrGetSchedulerPolicy
(
    KernelSchedMgr  *pKernelSchedMgr,
    OBJGPU          *pGpu,
    NvU32           *pSchedPolicy
)
{
    NvU32   schedPolicy         = SCHED_POLICY_DEFAULT;
    NvU32   regkey              = 0;

    if (hypervisorIsVgxHyper() || (RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))
    {
        //  Disable OBJSCHED_SW_ENABLE when GPU is older than Pascal.
        //  This is true for WDDM and vGPU scheduling
        if (!IsPASCALorBetter(pGpu))
        {
            schedPolicy = SCHED_POLICY_DEFAULT;
        }
        else if (IS_MIG_ENABLED(pGpu))
        {
            schedPolicy = SCHED_POLICY_DEFAULT;
            portDbgPrintf("NVRM: Software Scheduler is not supported in MIG mode\n");
        }
        // Check the PVMRL regkey
        else if (osReadRegistryDword(pGpu, NV_REG_STR_RM_PVMRL, &regkey) == NV_OK &&
                 FLD_TEST_REF(NV_REG_STR_RM_PVMRL_ENABLE, _YES, regkey) )
        {
            NvU32 configSchedPolicy = REF_VAL(NV_REG_STR_RM_PVMRL_SCHED_POLICY, regkey);

            switch (configSchedPolicy)
            {
                case NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_EQUAL_SHARE:
                    schedPolicy = SCHED_POLICY_VGPU_EQUAL_SHARE;
                    break;
                case NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_FIXED_SHARE:
                    schedPolicy = SCHED_POLICY_VGPU_FIXED_SHARE;
                    break;
                default:
                    NV_PRINTF(LEVEL_INFO,
                              "Invalid scheduling policy %d specified by regkey.\n",
                              configSchedPolicy);
                    break;
            }
        }
        else
        {
            schedPolicy = SCHED_POLICY_BEST_EFFORT;
        }
    }

    *pSchedPolicy = schedPolicy;

    switch (schedPolicy)
    {
        case SCHED_POLICY_BEST_EFFORT:
            return MAKE_NV_PRINTF_STR("BEST_EFFORT");
        case SCHED_POLICY_VGPU_EQUAL_SHARE:
            return MAKE_NV_PRINTF_STR("EQUAL_SHARE");
        case SCHED_POLICY_VGPU_FIXED_SHARE:
            return MAKE_NV_PRINTF_STR("FIXED_SHARE");
        case SCHED_POLICY_DEFAULT:
        default:
            // For baremetal and PT
            return MAKE_NV_PRINTF_STR("NONE");
    }
}

void kschedmgrSetConfigPolicyFromUser_IMPL
(
    KernelSchedMgr    *pKernelSchedMgr,
    OBJGPU            *pGpu,
    NvU32              schedSwPolicy
)
{
    NvU32 schedSwPolicyLocal = SCHED_POLICY_DEFAULT;
    switch (schedSwPolicy)
    {
        case NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_BEST_EFFORT:
            schedSwPolicyLocal = SCHED_POLICY_BEST_EFFORT;
            break;
        case NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_EQUAL_SHARE:
            schedSwPolicyLocal =  SCHED_POLICY_VGPU_EQUAL_SHARE;
            break;
        case NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_FIXED_SHARE:
            schedSwPolicyLocal =  SCHED_POLICY_VGPU_FIXED_SHARE;
            break;
        case NV2080_CTRL_CMD_VGPU_SCHEDULER_POLICY_OTHER:
            schedSwPolicyLocal =  SCHED_POLICY_DEFAULT;
            break;
        default:
            NV_PRINTF(LEVEL_WARNING, "Unknown vGPU scheduler policy %u\n", schedSwPolicy);
            schedSwPolicyLocal =  SCHED_POLICY_DEFAULT;
            break;
    }
    pKernelSchedMgr->configSchedPolicy = schedSwPolicyLocal;
    pKernelSchedMgr->bIsSchedSwEnabled = (schedSwPolicyLocal != SCHED_POLICY_DEFAULT);
}

/*!
 * Configure PDBs related to scheduler policy per RM configuration.
 */
void
kschedmgrConstructPolicy_IMPL
(
    KernelSchedMgr     *pKernelSchedMgr,
    OBJGPU             *pGpu
)
{
    const char         *schedPolicyName;
    NvU32               domain          = gpuGetDomain(pGpu);
    NvU32               bus             = gpuGetBus(pGpu);
    NvU32               device          = gpuGetDevice(pGpu);

    schedPolicyName = _kschedmgrGetSchedulerPolicy(pKernelSchedMgr, pGpu, &pKernelSchedMgr->configSchedPolicy);

    // PVMRL is disabled when GPU is older than Pascal
    if (((RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)) || hypervisorIsVgxHyper()) && IsPASCALorBetter(pGpu))
    {
        pKernelSchedMgr->bIsSchedSwEnabled = (pKernelSchedMgr->configSchedPolicy != SCHED_POLICY_DEFAULT);

        portDbgPrintf("NVRM: GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                      domain, bus, device,
                      pKernelSchedMgr->bIsSchedSwEnabled ? MAKE_NV_PRINTF_STR("ENABLED") : MAKE_NV_PRINTF_STR("DISABLED"),
                      schedPolicyName);
    }
    else
    {
        // RM is not yet ready to print this message in release builds on baremetal.
        NV_PRINTF(LEVEL_INFO,
                  "GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                  domain, bus, device,
                  pKernelSchedMgr->bIsSchedSwEnabled ? MAKE_NV_PRINTF_STR("ENABLED") : MAKE_NV_PRINTF_STR("DISABLED"),
                  schedPolicyName);
    }

    // Enabled SWRL Granular locking only if SWRL is enabled on hypervisor or VGPU_GSP_PLUGIN_OFFLOAD is enabled
    if (((RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)) || hypervisorIsVgxHyper()) && pKernelSchedMgr->bIsSchedSwEnabled)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING, NV_TRUE);
    }
}
