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

#include "kernel/gpu/rc/kernel_rc.h"

#include "kernel/core/locks.h"
#include "kernel/core/system.h"
#include "kernel/gpu/bif/kernel_bif.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/timer/objtmr.h"
#include "kernel/os/os.h"
#include "kernel/platform/chipset/chipset.h"
#include "kernel/rmapi/client.h"


#include "libraries/utils/nvprintf.h"
#include "nvrm_registry.h"
#include "nverror.h"
#include "nvtypes.h"


static void _krcInitRegistryOverrides(OBJGPU *pGpu, KernelRc *pKernelRc);
static void _krcLogUuidOnce(OBJGPU *pGpu, KernelRc *pKernelRc);


NV_STATUS
krcConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    KernelRc      *pKernelRc,
    ENGDESCRIPTOR  engDescriptor
)
{
    _krcInitRegistryOverrides(pGpu, pKernelRc);

    return NV_OK;
}


void
krcInitRegistryOverridesDelayed_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32 dword = 0;
    (void) dword;


    dword = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ROBUST_CHANNELS, &dword) !=
        NV_OK)
    {
#if RMCFG_FEATURE_PLATFORM_WINDOWS || RMCFG_FEATURE_PLATFORM_GSP || \
    RMCFG_FEATURE_PLATFORM_UNIX
        dword = NV_REG_STR_RM_ROBUST_CHANNELS_ENABLE;
#else
#error "unrecognized platform"
#endif
    }
    pKernelRc->bRobustChannelsEnabled = (dword ==
                                         NV_REG_STR_RM_ROBUST_CHANNELS_ENABLE);


    dword = 0;
    //
    // Force uncached pushbuffers for robust channel.
    //
    // We used to allocate the recovery channel as uncached, which is achieved
    // by allocating physically contiguous memory then remap that uncached.
    // However, this caused allocations issues in cases which shares a channel
    // with the robust channel, and ended up requesting sizeof(RC + pushbuffer)
    // of contiguous memory (bug 73669).
    //
    // We therefore switched to cached allocations, with a few exceptions where
    // an uncached pushbuffer is still needed:
    // - When the system does not support CPU cache snooping (bugs 292461 and
    // 976485).
    //
    if ((osReadRegistryDword(pGpu,
                             NV_REG_STR_USE_UNCACHED_PCI_MAPPINGS,
                             &dword) == NV_OK &&
         dword != 0) ||
        ((pKernelBif != NULL) &&
         !kbifIsSnoopDmaCapable(pGpu, pKernelBif)))
    {
        pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_ALLOC_UNCACHED_PCI;
    }
}


static void
_krcInitRegistryOverrides
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    NvU32 dword = 0;
    (void) dword;

    dword = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_BREAK_ON_RC, &dword) != NV_OK)
    {
        dword = NV_REG_STR_RM_BREAK_ON_RC_DEFAULT;
    }

    pKernelRc->bBreakOnRc = (dword == NV_REG_STR_RM_BREAK_ON_RC_ENABLE);

    // Allow driver registry key RmBreak to override Device Key
    if (DRF_VAL(_DEBUG, _BREAK_FLAGS, _RC, SYS_GET_INSTANCE()->debugFlags) ==
        NV_DEBUG_BREAK_FLAGS_RC_ENABLE)
    {
        pKernelRc->bBreakOnRc = NV_TRUE;
    }

    if (pKernelRc->bBreakOnRc)
    {
        NV_PRINTF(LEVEL_INFO, "Breakpoint on RC Error is enabled\n");
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "Breakpoint on RC Error is disabled\n");
    }


    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_WATCHDOG_TIMEOUT,
                            &pKernelRc->watchdogPersistent.timeoutSecs) !=
            NV_OK ||
        pKernelRc->watchdogPersistent.timeoutSecs == 0)
    {
        pKernelRc->watchdogPersistent.timeoutSecs =
            NV_REG_STR_RM_WATCHDOG_TIMEOUT_DEFAULT;
    }
    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_WATCHDOG_INTERVAL,
                            &pKernelRc->watchdogPersistent.intervalSecs) !=
            NV_OK ||
        pKernelRc->watchdogPersistent.intervalSecs == 0)
    {
        pKernelRc->watchdogPersistent.intervalSecs =
            NV_REG_STR_RM_WATCHDOG_INTERVAL_DEFAULT;
    }

    if (pKernelRc->watchdogPersistent.intervalSecs >
        pKernelRc->watchdogPersistent.timeoutSecs)
    {
        pKernelRc->watchdogPersistent.intervalSecs =
            pKernelRc->watchdogPersistent.timeoutSecs;
    }


    dword = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_RC_WATCHDOG, &dword) == NV_OK)
    {
        if (dword == NV_REG_STR_RM_RC_WATCHDOG_DISABLE)
        {
            pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;
        }
    }
    else if (IS_EMULATION(pGpu) || IS_SIMULATION(pGpu))
    {
        pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;
    }
    else if (gpuIsCCFeatureEnabled(pGpu))
    {
        pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;
    }

    dword = 0;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DO_LOG_RC_EVENTS, &dword) ==
        NV_OK)
    {
        pKernelRc->bLogEvents = (dword == NV_REG_STR_RM_DO_LOG_RC_ENABLE);
        if (pKernelRc->bLogEvents)
        {
            NV_PRINTF(LEVEL_INFO, "RC Error Logging is enabled\n");
#if defined(DEBUG)
            // Don't print out the initialization log on a retail build
            osErrorLog(pGpu, ROBUST_CHANNEL_RC_LOGGING_ENABLED, "");
#endif
        }
    }

    //
    // Do RC on BAR faults by default (For bug 1842228).
    // Only applicable to Volta+ chips.
    //
    pKernelRc->bRcOnBar2Fault = NV_TRUE;

}


static void
_krcLogUuidOnce
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    if (!pKernelRc->bGpuUuidLoggedOnce)
    {
        NvU8 *gidString = NULL;
        NvU32 gidStrlen;

        if (gpuGetGidInfo(pGpu,
                &gidString,
                &gidStrlen,
                (DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _ASCII) |
                 DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE,   _SHA1))) ==
            NV_OK)
        {
            portDbgPrintf("NVRM: GPU at PCI:%04x:%02x:%02x: %s\n",
                          gpuGetDomain(pGpu),
                          gpuGetBus(pGpu),
                          gpuGetDevice(pGpu),
                          gidString);
            portMemFree(gidString);
        }

        if (pGpu->boardInfo != NULL && pGpu->boardInfo->serialNumber[0] != '\0')
        {
            portDbgPrintf("NVRM: GPU Board Serial Number: %s\n",
                          pGpu->boardInfo->serialNumber);
        }

        pKernelRc->bGpuUuidLoggedOnce = NV_TRUE;
    }
}


void
krcGetMigAttributionForError_KERNEL
(
    KernelRc *pKernelRc,
    NvU32     exceptType,
    NvU16    *pGpuPartitionId,
    NvU16    *pComputeInstanceId
)
{
    if (pGpuPartitionId != NULL)
    {
        *pGpuPartitionId = KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID;
    }
    if (pComputeInstanceId != NULL)
    {
        *pComputeInstanceId = KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID;
    }
}


void
krcReportXid_IMPL
(
    OBJGPU     *pGpu,
    KernelRc   *pKernelRc,
    NvU32       exceptType,
    const char *pMsg
)
{
    //
    // Log the RC error to the OS
    //
    // Enforce the policy of gating the log output by "RmLogonRC" regkey.
    // Some of our callers do not abide by this rule.
    // That is how they want it under Windows.
    //
    if (GPU_GET_KERNEL_RC(pGpu)->bLogEvents)
    {
        NvU16          gpuPartitionId;
        NvU16          computeInstanceId;
        KernelChannel *pKernelChannel = krcGetChannelInError(pKernelRc);
        char          *current_procname = NULL;

        // Channels are populated with osGetCurrentProcessName() and pid of
        // their process at creation-time. If no channel was found, mark unknown
        const char *procname = "<unknown>";
        char pid_string[12] = "'<unknown>'";

        //
        // Get PID of channel creator if available, or get the current PID for
        // exception types that never have an associated channel
        //
        // Check for API lock since this can be called from parallel init
        // path without API lock, and RES_GET_CLIENT requires API lock
        //
        if (rmapiLockIsOwner() && (pKernelChannel != NULL))
        {
            RsClient *pClient = RES_GET_CLIENT(pKernelChannel);
            RmClient *pRmClient = dynamicCast(pClient, RmClient);
            procname = pRmClient->name;
            nvDbgSnprintf(pid_string, sizeof(pid_string), "%u", pKernelChannel->ProcessID);
        }
        else if (exceptType == GSP_RPC_TIMEOUT)
        {
            NvU32 current_pid = osGetCurrentProcess();

            nvDbgSnprintf(pid_string, sizeof(pid_string), "%u", current_pid);

            current_procname = portMemAllocNonPaged(NV_PROC_NAME_MAX_LENGTH);
            if (current_procname != NULL)
            {
                osGetCurrentProcessName(current_procname, NV_PROC_NAME_MAX_LENGTH);
                procname = current_procname;
            }
        }

        _krcLogUuidOnce(pGpu, pKernelRc);

        krcGetMigAttributionForError_HAL(pKernelRc,
                                         exceptType,
                                         &gpuPartitionId,
                                         &computeInstanceId);

        if (gpuPartitionId    != KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID &&
            computeInstanceId != KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID)
        {
            // Attribute this XID to both GPU / Compute instance
            portDbgPrintf(
                "NVRM: Xid (PCI:%04x:%02x:%02x GPU-I:%02u GPU-CI:%02u): %d, pid=%s, name=%s, %s\n",
                gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu),
                gpuPartitionId, computeInstanceId,
                exceptType,
                pid_string,
                procname,
                pMsg != NULL ? pMsg : "");
        }
        else if (gpuPartitionId != KMIGMGR_INSTANCE_ATTRIBUTION_ID_INVALID)
        {
            // Attribute this XID to GPU instance only
            portDbgPrintf(
                "NVRM: Xid (PCI:%04x:%02x:%02x GPU-I:%02u): %d, pid=%s, name=%s, %s\n",
                gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu),
                gpuPartitionId,
                exceptType,
                pid_string,
                procname,
                pMsg != NULL ? pMsg : "");
        }
        else
        {
            // Legacy (no attribution) XID reporting
            portDbgPrintf("NVRM: Xid (PCI:%04x:%02x:%02x): %d, pid=%s, name=%s, %s\n",
                gpuGetDomain(pGpu), gpuGetBus(pGpu), gpuGetDevice(pGpu),
                exceptType,
                pid_string,
                procname,
                pMsg != NULL ? pMsg : "");
        }

        portMemFree(current_procname);
    }
}


NvBool
krcTestAllowAlloc_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    NvU32     failMask
)
{
    if (pKernelRc->bRobustChannelsEnabled &&
        (pKernelRc->watchdog.allocFailMask & failMask))
    {
        OBJTMR   *pTmr = GPU_GET_TIMER(pGpu);
        NvU64     time;
        NV_STATUS status = tmrGetCurrentTime(pTmr, &time);

        //
        // randomly fail this alloc based on NV timer
        // assuming here that we don't get allocations within 128ns of each
        // other
        //
        if (status == NV_OK && ((time & 0xff) > (0xffu / 2)))
            return NV_FALSE;
    }

    return NV_TRUE;
}


NV_STATUS
krcCheckBusError_KERNEL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    KernelBif        *pKernelBif               = GPU_GET_KERNEL_BIF(pGpu);
    OBJCL            *pCl                      = SYS_GET_CL(SYS_GET_INSTANCE());
    NvU32             clDevCtrlStatusFlags     = 0;
    NvU32             clDevCtrlStatusFlags_Org = 0;
    NvU32             clDevCtrlStatus          = 0;
    PcieAerCapability clAer;


    // PCI-E provides extended error reporting
    if (pKernelBif == NULL || kbifGetBusIntfType_HAL(pKernelBif) !=
                                  NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS)
    {
        return NV_OK;
    }

    // Clear PCIe dev ctrl/status errors and AER errors
    kbifClearConfigErrors(pGpu, pKernelBif, NV_TRUE,
                          KBIF_CLEAR_XVE_AER_ALL_MASK);

    // Corelogic device control status
    if (pCl != NULL &&
        clPcieReadDevCtrlStatus(pGpu, pCl,
                                &clDevCtrlStatusFlags,
                                &clDevCtrlStatus) == NV_OK &&
        clDevCtrlStatusFlags != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
            "PCI-E corelogic status has pending errors (CL_PCIE_DEV_CTRL_STATUS = %08X):\n",
            clDevCtrlStatus);

        clDevCtrlStatusFlags_Org = clDevCtrlStatusFlags;

        if (clDevCtrlStatusFlags &
            NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR)
        {
            NV_PRINTF(LEVEL_ERROR, "     _CORR_ERROR_DETECTED\n");
            // not much interested in this one
            clDevCtrlStatusFlags &=
                ~NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
        }
        if (clDevCtrlStatusFlags &
            NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR)
        {
            NV_PRINTF(LEVEL_ERROR, "     _NON_FATAL_ERROR_DETECTED\n");
        }
        if (clDevCtrlStatusFlags &
            NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR)
        {
            NV_PRINTF(LEVEL_ERROR, "     _FATAL_ERROR_DETECTED\n");
        }
        if (clDevCtrlStatusFlags &
            NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST)
        {
            NV_PRINTF(LEVEL_ERROR, "     _UNSUPP_REQUEST_DETECTED\n");
        }
    }

    // Corelogic AER
    if (pCl != NULL && clPcieReadAerCapability(pGpu, pCl, &clAer) == NV_OK &&
        (clAer.UncorrErrStatusReg != 0 || clAer.RooErrStatus != 0))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "PCE-I Advanced Error Reporting Corelogic Info:\n");
        NV_PRINTF(LEVEL_ERROR,
                  "     Uncorr Error Status Register    : %08X\n",
                  clAer.UncorrErrStatusReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Uncorr Error Mask Register      : %08X\n",
                  clAer.UncorrErrMaskReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Uncorr Error Severity Register  : %08X\n",
                  clAer.UncorrErrSeverityReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Corr Error Status Register      : %08X\n",
                  clAer.CorrErrStatusReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Corr Error Mask Register        : %08X\n",
                  clAer.CorrErrMaskReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Advanced Err Cap & Ctrl Register: %08X\n",
                  clAer.AEcapCrtlReg);
        NV_PRINTF(LEVEL_ERROR,
                  "     Header Log [0-3]                : %08X\n",
                  clAer.HeaderLogReg.Header[0]);
        NV_PRINTF(LEVEL_ERROR,
                  "     Header Log [4-7]                : %08X\n",
                  clAer.HeaderLogReg.Header[1]);
        NV_PRINTF(LEVEL_ERROR,
                  "     Header Log [8-B]                : %08X\n",
                  clAer.HeaderLogReg.Header[2]);
        NV_PRINTF(LEVEL_ERROR,
                  "     Header Log [C-F]                : %08X\n",
                  clAer.HeaderLogReg.Header[3]);
        NV_PRINTF(LEVEL_ERROR,
                  "     Root Error Command Register     : %08X\n",
                  clAer.RootErrCmd);
        NV_PRINTF(LEVEL_ERROR,
                  "     Root Error Status               : %08X\n",
                  clAer.RooErrStatus);
        NV_PRINTF(LEVEL_ERROR,
                  "     Error Source ID Register        : %08X\n",
                  clAer.ErrSrcReg);

        //
        // if you hit this case with some AER errors reported please refer to
        // PCI-E manual for detailed bits spec
        // TODO: add details bits here
        //
    }

    if (clDevCtrlStatusFlags_Org)
    {
        // clear the corelogic status after we had a chance to examine it
        clPcieClearDevCtrlStatus(pGpu, pCl, &clDevCtrlStatus);
    }

    return NV_OK;
}

KernelChannel *
krcGetChannelInError_FWCLIENT
(
    KernelRc *pKernelRc
)
{
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(ENG_GET_GPU(pKernelRc)), NULL);
    return pKernelRc->pPreviousChannelInError;
}
