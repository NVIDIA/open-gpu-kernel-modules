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

/*!
 * @file
 * @brief  KernelSchedMgr object module
 *
 * This module is for managing and manipulating state related to KernelSchedMgr
 */

/* -------------------------------- Includes -------------------------------- */

#include "os/os.h"
#include "nvRmReg.h"

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
    NvBool  bSupportSwScheduler = NV_FALSE;
    NvU32   schedPolicy         = SCHED_POLICY_DEFAULT;

    //
    //  Disable OBJSCHED_SW_ENABLE when GPU is older than Pascal.
    //  This is true for WDDM and vGPU scheduling
    if (!IsPASCALorBetter(pGpu))
    {
        bSupportSwScheduler = NV_FALSE;
    }

    //  Disable OBJSCHED_SW_ENABLE when mig is enabled.
    if (bSupportSwScheduler && IS_MIG_ENABLED(pGpu))
    {
        bSupportSwScheduler = NV_FALSE;
        portDbgPrintf("NVRM: Software Scheduler is not supported in MIG mode\n");
    }

    *pSchedPolicy = schedPolicy;

    switch (schedPolicy)
    {
        case SCHED_POLICY_VGPU_RELATIVE:
            return "EQUAL_SHARE";
        case SCHED_POLICY_PGPU_SHARE:
            return  "FIXED_SHARE";
        case SCHED_POLICY_GFN_LSTT:
            return  "GFN_LSTT";
        default:
            if (hypervisorIsVgxHyper())
                return "BEST_EFFORT";
            else // For baremetal and PT
                return "NONE";
    }
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
    if (hypervisorIsVgxHyper() && IsPASCALorBetter(pGpu))
    {
        pKernelSchedMgr->bIsSchedSwEnabled = (pKernelSchedMgr->configSchedPolicy != SCHED_POLICY_DEFAULT);

        portDbgPrintf("NVRM: GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                      domain, bus, device,
                      pKernelSchedMgr->bIsSchedSwEnabled ? "ENABLED" : "DISABLED",
                      schedPolicyName);
    }
    else
    {
        // RM is not yet ready to print this message in release builds on baremetal.
        NV_PRINTF(LEVEL_INFO,
                  "GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                  domain, bus, device,
                  pKernelSchedMgr->bIsSchedSwEnabled ? "ENABLED" : "DISABLED",
                  schedPolicyName);
    }

    // Enabled SWRL Granular locking only if SWRL is enabled on hypervisor.
    if (hypervisorIsVgxHyper() && pKernelSchedMgr->bIsSchedSwEnabled)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING, NV_TRUE);
    }
}
