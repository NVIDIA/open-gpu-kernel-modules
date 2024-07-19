/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*      This file contains the implementations for Suspend to RAM ("Standby")
*      and Suspend to Disk ("Hibernate")
*
******************************************************************************/

#include "platform/platform.h"
#include "platform/chipset/chipset.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/fbsr.h"
#include "gpu/gsp/gsp_init_args.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/pmu/kern_pmu.h"
#include "rmgspseq.h"
#include "core/thread_state.h"
#include "vgpu/rpc.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include <gpu/fsp/kern_fsp.h>

//
// Helper functions
//
static NV_STATUS gpuPowerManagementEnter(OBJGPU *, NvU32 newLevel, NvU32 flags);
static NV_STATUS gpuPowerManagementResume(OBJGPU *, NvU32 oldLevel, NvU32 flags);

// XXX Needs to be further cleaned up. No new code should be placed in this
// routine. Please use the per-engine StateLoad() and StateUnload() routines
// instead
static NV_STATUS
gpuPowerManagementEnter(OBJGPU *pGpu, NvU32 newLevel, NvU32 flags)
{
    NV_STATUS  status = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementEnterPreUnloadPhysical(pGpu), done);

    NV_ASSERT_OK_OR_GOTO(status, gpuStateUnload(pGpu,
        IS_GPU_GC6_STATE_ENTERING(pGpu) ?
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION | GPU_STATE_FLAGS_GC6_TRANSITION :
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION), done);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_VGA_ENABLED, NV_TRUE);

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementEnterPostUnloadPhysical(pGpu, newLevel), done);

    if (IS_GSP_CLIENT(pGpu))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        KernelGspPreparedFwsecCmd preparedCmd;

        NV_RM_RPC_UNLOADING_GUEST_DRIVER(pGpu, status, NV_TRUE, IS_GPU_GC6_STATE_ENTERING(pGpu), newLevel);
        if (status != NV_OK)
            goto done;

        // Wait for GSP-RM to suspend
        kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp);

        // Dump GSP-RM logs before resetting and invoking FWSEC-SB
        kgspDumpGspLogs(pKernelGsp, NV_FALSE);

        if (!IS_GPU_GC6_STATE_ENTERING(pGpu))
        {
            // Because of COT, RM cannot reset GSP-RISCV.
            if (!(pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_COT_ENABLED)))
            {
                kflcnReset_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
            }

            // Invoke FWSEC-SB to load back PreOsApps.
            status = kgspPrepareForFwsecSb_HAL(pGpu, pKernelGsp, pKernelGsp->pFwsecUcode, &preparedCmd);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to prepare for FWSEC-SB for PreOsApps\n");
                goto done;
            }

            status = kgspExecuteFwsec_HAL(pGpu, pKernelGsp, &preparedCmd);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC-SB for PreOsApps\n");
                goto done;
            }

            kpmuFreeLibosLoggingStructures(pGpu, GPU_GET_KERNEL_PMU(pGpu));

            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                kgspSavePowerMgmtState_HAL(pGpu, pKernelGsp), done);
        }
        else
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp, 0), done);
        }
    }

done:
    if ((status != NV_OK) && !IS_GPU_GC6_STATE_ENTERING(pGpu))
    {
        memmgrFreeFbsrMemory(pGpu, pMemoryManager);
    }

    return status;
}

#ifdef DEBUG
int g_BreakOnResume = 0;
#endif

// XXX Needs to be further cleaned up. No new code should be placed in this
// routine. Please use the per-engine StateLoad() and StateUnload() routines
// instead
static NV_STATUS
gpuPowerManagementResume(OBJGPU *pGpu, NvU32 oldLevel, NvU32 flags)
{
    NV_STATUS       status = NV_OK;
    OBJSYS         *pSys   = SYS_GET_INSTANCE();
    OBJCL          *pCl    = SYS_GET_CL(pSys);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

#ifdef DEBUG
    //
    // This is useful for windbg debugging as it frequently doesn't reconnect to the
    // target system until after the majority of the resume code has been ran.
    // Placing an int 3 at the entrypoint to S/R resume code causes the target to
    // halt until the debugger reconnects and the user is then able to place
    // breakpoints / step through code.
    //
    if (g_BreakOnResume)
    {
        DBG_BREAKPOINT();
    }
#endif

    if (pCl != NULL)
    {
        status = clResumeBridge(pCl);
        NV_ASSERT(status == NV_OK);
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        //
        // GSP-RM expects sysmem flush buffer address to be programmed by Kernel-RM.
        // As this buffer is used to perform a system flush while resetting Gpu Falcons.
        //
        kmemsysProgramSysmemFlushBuffer_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        GSP_SR_INIT_ARGUMENTS gspSrInitArgs;

        gspSrInitArgs.oldLevel = oldLevel;
        gspSrInitArgs.flags = flags;
        gspSrInitArgs.bInPMTransition = NV_TRUE;

        kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, &gspSrInitArgs);

        // Wait for GFW_BOOT status
        status = kgspWaitForGfwBootOk_HAL(pGpu, pKernelGsp);
        if (status != NV_OK)
        {
            goto done;
        }

        if (!IS_GPU_GC6_STATE_EXITING(pGpu))
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                kgspRestorePowerMgmtState_HAL(pGpu, pKernelGsp), done);

            status = kpmuInitLibosLoggingStructures(pGpu, GPU_GET_KERNEL_PMU(pGpu));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "cannot init libOS PMU logging structures: 0x%x\n", status);
                goto done;
            }
        }
        else
        {
            status = kgspExecuteBooterLoad_HAL(pGpu, pKernelGsp, 0);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "cannot resume riscv/gsp from GC6: 0x%x\n", status);
                goto done;
            }
        }

        status = kgspWaitForRmInitDone(pGpu, pKernelGsp);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "State load at resume for riscv/gsp failed: 0x%x\n", status);
            goto done;
        }
    }
    else
    {
        // Boot GSP-FMC for monolithic RM
        KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
        if ((pKernelFsp != NULL) && !IS_VIRTUAL(pGpu))
        {
            pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_BOOT_COMMAND_OK, NV_FALSE);

            status = kfspPrepareAndSendBootCommands_HAL(pGpu, pKernelFsp);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "FSP boot command failed during resume.\n");
                goto done;
            }
        }

    }

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementResumePreLoadPhysical(pGpu, oldLevel, flags), done);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_VGA_ENABLED, NV_FALSE);

    NV_ASSERT_OK_OR_GOTO(status, gpuStateLoad(pGpu,
        IS_GPU_GC6_STATE_EXITING(pGpu) ?
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION | GPU_STATE_FLAGS_GC6_TRANSITION :
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION), done);

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementResumePostLoadPhysical(pGpu), done);

    NV_PRINTF(LEVEL_NOTICE, "Adapter now in D0 state\n");

done:
    if (!IS_GPU_GC6_STATE_EXITING(pGpu))
    {
        memmgrFreeFbsrMemory(pGpu, pMemoryManager);
    }

    return status;
}

NV_STATUS
gpuEnterStandby_IMPL(OBJGPU *pGpu)
{
    OBJSYS       *pSys    = SYS_GET_INSTANCE();
    OBJPFM       *pPfm    = SYS_GET_PFM(pSys);
    NV_STATUS     suspendStatus;

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3 Requested\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning transition from D0 to %s\n",
                  IS_GPU_GC6_STATE_ENTERING(pGpu) ? "GC6" : "D3");
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_4 Requested\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning transition to %s\n",
                  IS_GPU_GC6_STATE_ENTERING(pGpu) ? "GC6" : "APM Suspend");
    }

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_TRUE);

    suspendStatus = gpuPowerManagementEnter(pGpu, NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3, GPU_STATE_FLAGS_PM_SUSPEND);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_TRUE);
    pGpu->bInD3Cold = NV_TRUE;

    if ((pPfm !=NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending transition from D0 to %s\n",
                  IS_GPU_GC6_STATE_ENTERING(pGpu) ? "GC6" : "D3");
    }
    else
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending transition to %s\n",
                  IS_GPU_GC6_STATE_ENTERING(pGpu) ? "GC6" : "APM Suspend");
    }

    return suspendStatus;
}

NV_STATUS
gpuResumeFromStandby_IMPL(OBJGPU *pGpu)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    OBJPFM    *pPfm    = SYS_GET_PFM(pSys);
    NV_STATUS  resumeStatus;
    NvU32      state   = 0;

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        state = NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3;
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState Transitioning from NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning transition from %s to D0\n",
                  IS_GPU_GC6_STATE_EXITING(pGpu) ? "GC6" : "D3");
    }
    else
    {
        state = NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_4;
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState Transitioning from NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_4\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning resume from %s\n",
                  IS_GPU_GC6_STATE_EXITING(pGpu) ? "GC6" : "APM Suspend");
    }

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_TRUE);

    resumeStatus = gpuPowerManagementResume(pGpu, state, GPU_STATE_FLAGS_PM_SUSPEND);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_FALSE);
    pGpu->bInD3Cold = NV_FALSE;
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_FALSE);

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending transition from %s to D0\n",
                  IS_GPU_GC6_STATE_EXITING(pGpu) ? "GC6" : "D3");
    }
    else
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending resume from %s\n",
                  IS_GPU_GC6_STATE_EXITING(pGpu) ? "GC6" : "APM Suspend");
    }
    if (kgraphicsIsBug4208224WARNeeded_HAL(pGpu, GPU_GET_KERNEL_GRAPHICS(pGpu, 0)))
    {
        return kgraphicsInitializeBug4208224WAR_HAL(pGpu, GPU_GET_KERNEL_GRAPHICS(pGpu, 0));
    }

    return resumeStatus;
}

NV_STATUS gpuEnterHibernate_IMPL(OBJGPU *pGpu)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    OBJPFM    *pPfm    = SYS_GET_PFM(pSys);
    NV_STATUS  suspendStatus;

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7 Requested\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning transition from D0 to D4\n");
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuPowerState NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_4 Requested\n");
        NV_PRINTF(LEVEL_NOTICE, "Beginning APM Suspend\n");
    }

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_TRUE);

    suspendStatus = gpuPowerManagementEnter(pGpu, NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7, GPU_STATE_FLAGS_PM_HIBERNATE);

    NV_PRINTF(LEVEL_INFO,
              "gpuPowerState Saving clocks and throttling them down\n");

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE, NV_TRUE);

    NV_PRINTF(LEVEL_NOTICE, "Ending transition from D0 to D4\n");

    return suspendStatus;
}

NV_STATUS gpuResumeFromHibernate_IMPL(OBJGPU *pGpu)
{
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    OBJPFM    *pPfm   = SYS_GET_PFM(pSys);
    NV_STATUS  resumeStatus;

    NV_PRINTF(LEVEL_INFO,
              "gpuPowerState Transitioning from NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7\n");
    NV_PRINTF(LEVEL_NOTICE, "Beginning transition from D4 to D0\n");

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_TRUE);

    resumeStatus = gpuPowerManagementResume(pGpu, NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7, GPU_STATE_FLAGS_PM_HIBERNATE);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_FALSE);

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending transition from D4 to D0\n");
    }
    else
    {
        NV_PRINTF(LEVEL_NOTICE, "End resuming from APM Suspend\n");
    }
    if (kgraphicsIsBug4208224WARNeeded_HAL(pGpu, GPU_GET_KERNEL_GRAPHICS(pGpu, 0)))
    {
        return kgraphicsInitializeBug4208224WAR_HAL(pGpu, GPU_GET_KERNEL_GRAPHICS(pGpu, 0));
    }

    return resumeStatus;
}
