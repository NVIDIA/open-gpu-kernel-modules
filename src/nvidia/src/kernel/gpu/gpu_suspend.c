/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bif/kernel_bif.h"
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
static NV_STATUS _gpuPollCFGAndCheckD3Hot(OBJGPU *);

// XXX Needs to be further cleaned up. No new code should be placed in this
// routine. Please use the per-engine StateLoad() and StateUnload() routines
// instead
static NV_STATUS
gpuPowerManagementEnter(OBJGPU *pGpu, NvU32 newLevel, NvU32 flags)
{
    NV_STATUS  status = NV_OK;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    pGpu->powerManagementDepth = NV_PM_DEPTH_OS_LAYER;

    KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    KernelGspUnloadMode unloadMode = KGSP_UNLOAD_MODE_SR_SUSPEND;

    if (IS_GSP_CLIENT(pGpu))
    {
        if (IS_GPU_GC6_STATE_ENTERING(pGpu))
        {
            unloadMode = KGSP_UNLOAD_MODE_GC6_ENTER;
        }
        else
        {
            unloadMode = KGSP_UNLOAD_MODE_SR_SUSPEND;

            // Prepare SR metadata structure for suspend
            status = kgspPrepareSuspendResumeData_HAL(pGpu, pKernelGsp);
            if (status != NV_OK)
            {
                return status;
            }
        }

        pGpu->powerManagementDepth = NV_PM_DEPTH_SR_META;
    }

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementEnterPreUnloadPhysical(pGpu), done);

    NV_ASSERT_OK_OR_GOTO(status, gpuStateUnload(pGpu,
        IS_GPU_GC6_STATE_ENTERING(pGpu) ?
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION | GPU_STATE_FLAGS_GC6_TRANSITION :
        GPU_STATE_FLAGS_PRESERVING | GPU_STATE_FLAGS_PM_TRANSITION), done);

    pGpu->powerManagementDepth = NV_PM_DEPTH_STATE_LOAD;
    pGpu->setProperty(pGpu, PDB_PROP_GPU_VGA_ENABLED, NV_TRUE);

    // This is a no-op in CPU-RM
    NV_ASSERT_OK_OR_GOTO(status, gpuPowerManagementEnterPostUnloadPhysical(pGpu, newLevel), done);

    if (IS_GSP_CLIENT(pGpu))
    {
        status = kgspUnloadRm(pGpu, pKernelGsp, unloadMode, newLevel);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "GSP unload failed at suspend (bootMode 0x%x, newLevel 0x%x): 0x%x\n",
                      unloadMode, newLevel, status);
            goto done;
        }

        if (!IS_GPU_GC6_STATE_ENTERING(pGpu))
        {
            kpmuFreeLibosLoggingStructures(pGpu, GPU_GET_KERNEL_PMU(pGpu));
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

    if (pGpu->powerManagementDepth == NV_PM_DEPTH_OS_LAYER)
    {
        return NV_OK;
    }

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

        OBJTMR    *pTmr = GPU_GET_TIMER(pGpu);
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        KernelGspBootMode bootMode;

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

        if (IS_GPU_GC6_STATE_EXITING(pGpu))
        {
            bootMode = KGSP_BOOT_MODE_GC6_EXIT;
        }
        else
        {
            bootMode = KGSP_BOOT_MODE_SR_RESUME;

            status = kpmuInitLibosLoggingStructures(pGpu, GPU_GET_KERNEL_PMU(pGpu));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "cannot init libOS PMU logging structures: 0x%x\n", status);
                goto done;
            }

            //
            // In suspend/resume case, update the GPU time after GFW boot is complete
            // (to avoid PLM collisions) but before loading GSP-RM ucode (which
            // consumes the updated GPU time).
            //
            tmrSetCurrentTime_HAL(pGpu, pTmr);

            libosLogUpdateTimerDelta(&pKernelGsp->logDecode, pTmr->sysTimerOffsetNs);
        }

        status = kgspPrepareForBootstrap_HAL(pGpu, pKernelGsp, bootMode);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "GSP boot preparation failed at resume (bootMode 0x%x): 0x%x\n", bootMode, status);
            goto done;
        }

        status = kgspBootstrap_HAL(pGpu, pKernelGsp, bootMode);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "GSP boot failed at resume (bootMode 0x%x): 0x%x\n", bootMode, status);
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
        if (IS_GSP_CLIENT(pGpu))
        {
            KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
            kgspFreeSuspendResumeData_HAL(pGpu, pKernelGsp);
        }

        memmgrFreeFbsrMemory(pGpu, pMemoryManager);
    }

    return status;
}

//
// Since the BAR firewall introduced from Blackwell
// Need to poll Device on the bus and CFG BAR firewall first,
// before any PRI register accessing.
// For GCOFF/GC8/GC6 three cases, all will run into standby path,
// So adding this helper function before calling gpu resume.
//
static NV_STATUS
_gpuPollCFGAndCheckD3Hot(OBJGPU *pGpu)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    //
    // GCOFF/GC8/GC6 three cases, all will run into here.
    // Because of BAR firewall introduced from Blackwell,
    // We need to poll CFG for:
    // 1. if GPU is on the bus
    // 2. if BAR firewall disengage only if blackwell and later
    // 3. check GC6 D3Hot case (has PRI register access, must after firewall disengage)
    //

    if (!IS_FMODEL(pGpu))
    {
        // Polling GPU on the bus
        NV_ASSERT_OK_OR_RETURN(kbifPollDeviceOnBus(pGpu, pKernelBif));

        //
        // For V0/V0S fused board, BAR firewall introduced from Blackwell onwards.
        // Polling BAR firewall disengage if FSP released before accessing any PRI access.
        //
        NV_ASSERT_OK_OR_RETURN(kbifPollBarFirewallDisengage_HAL(pGpu, pKernelBif));
    }

    // For GC6 exit, check if D3Hot case
    if(IS_GPU_GC6_STATE_EXITING(pGpu))
    {
        gpuCheckGc6inD3Hot(pGpu);
    }
    return NV_OK;
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

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_SUSPEND, NULL, 0, 0, 0);

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

    resumeStatus = _gpuPollCFGAndCheckD3Hot(pGpu);
    if (resumeStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Polling BAR0 or BAR firewall timeout\n");
    }

    resumeStatus = gpuPowerManagementResume(pGpu, state, GPU_STATE_FLAGS_PM_SUSPEND);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_FALSE);
    pGpu->bInD3Cold = NV_FALSE;
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_FALSE);

    // reset PM depth on resume
    pGpu->powerManagementDepth = NV_PM_DEPTH_NONE;

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

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_RESUME, NULL, 0, 0, 0);

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

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_SUSPEND, NULL, 0, 0, 0);

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

    // reset PM depth on resume
    pGpu->powerManagementDepth = NV_PM_DEPTH_NONE;

    if ((pPfm != NULL) && pPfm->getProperty(pPfm, PDB_PROP_PFM_SUPPORTS_ACPI))
    {
        NV_PRINTF(LEVEL_NOTICE, "Ending transition from D4 to D0\n");
    }
    else
    {
        NV_PRINTF(LEVEL_NOTICE, "End resuming from APM Suspend\n");
    }

    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_RESUME, NULL, 0, 0, 0);

    return resumeStatus;
}
