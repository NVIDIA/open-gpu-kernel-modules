/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief CPU functions implementations - X86, X86-64.
 */

#include "cpuopsys.h"

#if NVCPU_IS_X86 || NVCPU_IS_X86_64

#include "nvport/nvport.h"
#include "cpu_x86_amd64.h"

static NV_INLINE NV_STATUS portCpuCpuId(NvU32* pCpuInfo, NvU32 functionId,
    NvU32 subfunctionId)
{
    NV_STATUS status;
#if PORT_IS_FUNC_SUPPORTED(portCpuExCpuId)
    status = portCpuExCpuId(pCpuInfo, functionId, subfunctionId);
#else
    status = NV_ERR_NOT_SUPPORTED;
#endif
    return status;
}

static NV_INLINE NV_STATUS portCpuCheckStdFunSupport(NvU32 function_id)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];

    portMemSet(cpuInfo, 0, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_BASIC,
        PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    if (function_id > cpuInfo[0])
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuIntelGetVersion(PORT_CPU_INTEL *pCpuVersion)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];
    NvU32 model;
    NvU32 family;

    if (pCpuVersion == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = portCpuCheckStdFunSupport(PORT_CPU_CPUID_STD_FUN_VERSION);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_VERSION,
        PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    pCpuVersion->steppingId = PORT_CPU_CPUID_GET_STEPPING_ID(cpuInfo[0]);
    family = PORT_CPU_CPUID_GET_FAMILY(cpuInfo[0]);
    model = PORT_CPU_CPUID_GET_MODEL(cpuInfo[0]);
    if ((family == 0x0FU) || (family == 0x06U))
    {
        model |= PORT_CPU_CPUID_GET_EXT_MODEL(cpuInfo[0])
                 << PORT_CPU_CPUID_MODEL_SHIFT;
    }

    if (family == 0x0FU)
    {
        family += PORT_CPU_CPUID_GET_EXT_FAMILY(cpuInfo[0]);
    }

    pCpuVersion->family = family;
    switch (pCpuVersion->family)
    {
    case PORT_CPU_INTEL_FAMILY_6:
        pCpuVersion->model.family6 = model;
        break;
    default:
        // Unknown famliy, ignore model.
        break;
    }

    pCpuVersion->brandIndex = PORT_CPU_CPUID_GET_BRAND_INDEX(cpuInfo[1]);
    pCpuVersion->localApicId = PORT_CPU_CPUID_GET_LOCAL_APIC_ID(cpuInfo[1]);
    pCpuVersion->processorType = PORT_CPU_CPUID_GET_PROCESSOR_TYPE(cpuInfo[1]);

    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuGetIntelFeatures(PORT_CPU_INTEL_X86_FEATURES
                                                      *pCpuFeatures)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];

    if (pCpuFeatures == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = portCpuCheckStdFunSupport(PORT_CPU_CPUID_STD_FUN_VERSION);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_VERSION,
             PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    pCpuFeatures->SSE3 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_SSE3_BIT);
    pCpuFeatures->PCLMULQDQ = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                              PORT_CPU_CPUID_FEATURE_PCLMULQDQ_BIT);
    pCpuFeatures->DTES64 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                           PORT_CPU_CPUID_FEATURE_DTES64_BIT);
    pCpuFeatures->MONITOR = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                            PORT_CPU_CPUID_FEATURE_MONITOR_BIT);
    pCpuFeatures->DSCPL = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_DSCPL_BIT);
    pCpuFeatures->VMX = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_VMX_BIT);
    pCpuFeatures->SMX = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_SMX_BIT);
    pCpuFeatures->EIST = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_EIST_BIT);
    pCpuFeatures->TM2 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_TM2_BIT);
    pCpuFeatures->SSSE3 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_SSSE3_BIT);
    pCpuFeatures->CNXTID = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                           PORT_CPU_CPUID_FEATURE_CNXTID_BIT);
    pCpuFeatures->SDBG = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_SDBG_BIT);
    pCpuFeatures->FMA = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_FMA_BIT);
    pCpuFeatures->CMPXCHG16B = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                               PORT_CPU_CPUID_FEATURE_CMPXCHG16B_BIT);
    pCpuFeatures->xTPRUpdateControl = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                                      PORT_CPU_CPUID_FEATURE_XTPRUPDATECONTROL_BIT);
    pCpuFeatures->PDCM = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_PDCM_BIT);
    pCpuFeatures->PCID = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_PCID_BIT);
    pCpuFeatures->DCA = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_DCA_BIT);
    pCpuFeatures->SSE41 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_SSE41_BIT);
    pCpuFeatures->SSE42 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_SSE42_BIT);
    pCpuFeatures->x2APIC = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                           PORT_CPU_CPUID_FEATURE_x2APIC_BIT);
    pCpuFeatures->MOVBE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_MOVBE_BIT);
    pCpuFeatures->POPCNT = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                           PORT_CPU_CPUID_FEATURE_POPCNT_BIT);
    pCpuFeatures->TSCDeadline = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                                PORT_CPU_CPUID_FEATURE_TSCDEADLINE_BIT);
    pCpuFeatures->AES = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_AES_BIT);
    pCpuFeatures->XSAVE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_XSAVE_BIT);
    pCpuFeatures->OSXSAVE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                            PORT_CPU_CPUID_FEATURE_OSXSAVE_BIT);
    pCpuFeatures->AVX = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                        PORT_CPU_CPUID_FEATURE_AVX_BIT);
    pCpuFeatures->F16C = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                         PORT_CPU_CPUID_FEATURE_F16C_BIT);
    pCpuFeatures->RDEND = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
                          PORT_CPU_CPUID_FEATURE_RDEND_BIT);


    pCpuFeatures->FPU = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_FPU_BIT);
    pCpuFeatures->VME = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_VME_BIT);
    pCpuFeatures->DE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                       PORT_CPU_CPUID_FEATURE_DE_BIT);
    pCpuFeatures->PSE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PSE_BIT);
    pCpuFeatures->TSC = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_TSC_BIT);
    pCpuFeatures->MSR = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_MSR_BIT);
    pCpuFeatures->PAE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PAE_BIT);
    pCpuFeatures->MCE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_MCE_BIT);
    pCpuFeatures->CX8 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_CX8_BIT);
    pCpuFeatures->APIC = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_APIC_BIT);
    pCpuFeatures->SEP = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_SEP_BIT);
    pCpuFeatures->MTRR = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_MTRR_BIT);
    pCpuFeatures->PGE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PGE_BIT);
    pCpuFeatures->MCA = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_MCA_BIT);
    pCpuFeatures->CMOV = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_CMOV_BIT);
    pCpuFeatures->PAT = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PAT_BIT);
    pCpuFeatures->PSE36 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                          PORT_CPU_CPUID_FEATURE_PSE36_BIT);
    pCpuFeatures->PSN = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PSN_BIT);
    pCpuFeatures->CLFSH = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                          PORT_CPU_CPUID_FEATURE_CLFSH_BIT);
    pCpuFeatures->DEBUGS = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                       PORT_CPU_CPUID_FEATURE_DS_BIT);
    pCpuFeatures->ACPI = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_ACPI_BIT);
    pCpuFeatures->MMX = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_MMX_BIT);
    pCpuFeatures->FXSR = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_FXSR_BIT);
    pCpuFeatures->SSE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_SSE_BIT);
    pCpuFeatures->SSE2 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                         PORT_CPU_CPUID_FEATURE_SSE2_BIT);
    pCpuFeatures->SELFS = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                       PORT_CPU_CPUID_FEATURE_SS_BIT);
    pCpuFeatures->HTT = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_HTT_BIT);
    pCpuFeatures->TM = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                       PORT_CPU_CPUID_FEATURE_TM_BIT);
    pCpuFeatures->PBE = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[3],
                        PORT_CPU_CPUID_FEATURE_PBE_BIT);
    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuGetIntelArchPerfMonitor(
    PORT_CPU_INTEL_ARCH_PERF_MONITOR *pCpuArchPerfMonitor)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];

    if (pCpuArchPerfMonitor == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = portCpuCheckStdFunSupport(PORT_CPU_CPUID_STD_FUN_TPM);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_ARCH_PERF_MONITOR,
        PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    pCpuArchPerfMonitor->versionId =
        PORT_CPU_ARCH_PERF_MONITOR_VERSION_ID(cpuInfo[0]);
    pCpuArchPerfMonitor->noOfGPPerfMonitoringCounters =
        PORT_CPU_ARCH_PERF_MONITOR_COUNTERS(cpuInfo[0]);
    pCpuArchPerfMonitor->bitWidthOfGPCounters =
        PORT_CPU_ARCH_PERF_MONITOR_COUNTERS_BITWIDTH(cpuInfo[0]);
    pCpuArchPerfMonitor->coreCycleEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
                PORT_CPU_ARCH_PERF_MONITOR_CORE_CYCLE_EVENT_BIT);
    pCpuArchPerfMonitor->instructionRetiredEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_INSTR_RET_EVENT_BIT);
    pCpuArchPerfMonitor->referenceCycelEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_REF_CYCLE_EVENT_BIT);
    pCpuArchPerfMonitor->lastLevelCacheRefEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_LL_CACHE_REF_EVENT_BIT);
    pCpuArchPerfMonitor->lastLevelCacheMissEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_LL_CACHE_MIS_EVENT_BIT);
    pCpuArchPerfMonitor->branchInstructionRetiredEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_BRANCH_INSTR_RET_EVENT_BIT);
    pCpuArchPerfMonitor->branchMispredictRetiredEvent =
        !PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
            PORT_CPU_ARCH_PERF_MONITOR_BRANCH_MISPRE_RET_EVENT_BIT);
    pCpuArchPerfMonitor->noOfFixedFuncPerfCounters =
        PORT_CPU_ARCH_PERF_MONITOR_FIX_FUN_COUNTERS(cpuInfo[3]);
    pCpuArchPerfMonitor->bitWidthOfFixedFuncPerfCounters =
        PORT_CPU_ARCH_PERF_MONITOR_FIX_FUN_COUNTERS_BITWIDTH(cpuInfo[3]);

    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuGetIntelTPMFeatures(
    PORT_CPU_INTEL_TPM_FEATURES *pCpuTPMFeatures)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];

    if (pCpuTPMFeatures == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = portCpuCheckStdFunSupport(PORT_CPU_CPUID_STD_FUN_TPM);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_TPM,
        PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    pCpuTPMFeatures->DTS = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_DTS_BIT);
    pCpuTPMFeatures->IntelTurboBoost = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_ITB_BIT);
    pCpuTPMFeatures->ARAT = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_ARAT_BIT);
    pCpuTPMFeatures->PLN = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_PLN_BIT);
    pCpuTPMFeatures->ECMD = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_ECMD_BIT);
    pCpuTPMFeatures->PTM = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_PTM_BIT);
    pCpuTPMFeatures->HWP = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWP_BIT);
    pCpuTPMFeatures->HWPNotification = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWP_NOTIFICATION_BIT);
    pCpuTPMFeatures->HWPActivityWindow = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWP_ACTIVITY_WINDOW_BIT);
    pCpuTPMFeatures->HWPEPP = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWP_ACTIVITY_WINDOW_BIT);
    pCpuTPMFeatures->HWPPLR = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWPEPP_BIT);
    pCpuTPMFeatures->HDC = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HDC_BIT);
    pCpuTPMFeatures->IntelTurboBoostMaxTech30 = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_ITBMT30_BIT);
    pCpuTPMFeatures->HWPCapabilities = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWP_CAPABILITIES_BIT);
    pCpuTPMFeatures->HWPPECI = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_HWPPECI_BIT);
    pCpuTPMFeatures->FLEXHWP = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_FLEXHWP_BIT);
    pCpuTPMFeatures->FAM = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_FAM_BIT);
    pCpuTPMFeatures->ILPHWPRequest = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[0],
        PORT_CPU_CPUID_TPM_FEATURE_ILPHWP_REQUEST_BIT);
    pCpuTPMFeatures->NoOfInterruptThresholdsInDTS =
                                     PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[1],
                                     PORT_CPU_CPUID_TPM_FEATURE_NFITI_DTS_BIT);

    pCpuTPMFeatures->HCFC = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
        PORT_CPU_CPUID_TPM_FEATURE_HCFC_BIT);
    pCpuTPMFeatures->PEBP = PORT_CPU_CPUID_FEATURE_GET_BIT(cpuInfo[2],
        PORT_CPU_CPUID_TPM_FEATURE_PEBP_BIT);

    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuGetIntelTopology(PORT_CPU_INTEL* pCpuInfo)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];
    NvU32 sub = 0;
    NvBool cond = NV_TRUE;

    status = portCpuCheckStdFunSupport(
        PORT_CPU_CPUID_STD_FUN_EXT_TOPOLOGY_ENUM);
    if (status != NV_OK)
    {
       PORT_BREAKPOINT_DEBUG();
       return status;
    }

    do
    {
        portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
        status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_EXT_TOPOLOGY_ENUM,
            sub);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }

        switch (PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LEVEL_TYPE(cpuInfo[2]))
        {
        case PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_SMT:
            pCpuInfo->threadCountPerCore =
                PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LOGICAL_PROC_COUNT(cpuInfo[1]);
            break;
        case PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_CORE:
            pCpuInfo->logicalCoreCount =
                PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LOGICAL_PROC_COUNT(cpuInfo[1]);
            break;
        default:
            cond = NV_FALSE;
            break;
        }

        pCpuInfo->physicalCoreCount = pCpuInfo->logicalCoreCount / pCpuInfo->threadCountPerCore;
        sub++;
    } while (cond == NV_TRUE);

    return NV_OK;
}

static NV_INLINE NV_STATUS portCpuGetVendorId(char* vendorId, NvLength length,
    NvLength* pVendorIdLength)
{
    NV_STATUS status;
    NvU32 cpuInfo[4];

    if (length < PORT_CPU_CPUID_VENDOR_ID_LENGTH)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if (pVendorIdLength == NULL || vendorId == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(cpuInfo, 0U, sizeof(cpuInfo));
    status = portCpuCpuId(cpuInfo, PORT_CPU_CPUID_STD_FUN_BASIC,
        PORT_CPU_CPUID_STD_SUBFUN_0);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    portMemSet(vendorId, 0U, length);
    NV_SWAP_U32(cpuInfo[2], cpuInfo[3]);
    portMemCopy(vendorId, length, &cpuInfo[1],
        PORT_CPU_CPUID_VENDOR_ID_LENGTH - 1);
    *pVendorIdLength = PORT_CPU_CPUID_VENDOR_ID_LENGTH;

    return NV_OK;
}

NV_STATUS portCpuGetInfo(PORT_CPU_INFORMATION *pCpuInfo)
{
    NV_STATUS status;

    if (pCpuInfo == NULL)
    {
        PORT_BREAKPOINT_DEBUG();
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(pCpuInfo, 0U, sizeof(PORT_CPU_INFORMATION));
    status = portCpuGetVendorId(pCpuInfo->vendorId, PORT_CPU_VENDOR_ID_LENGTH,
                       &pCpuInfo->vendorIdLength);
    if (status != NV_OK)
    {
        PORT_BREAKPOINT_DEBUG();
        return status;
    }

    if (portStringCompare(pCpuInfo->vendorId,
        PORT_CPU_CPUID_VENDOR_ID_INTEL, PORT_CPU_CPUID_VENDOR_ID_LENGTH) == 0)
    {
        pCpuInfo->type = PORT_CPU_TYPE_INTEL_X86;
        status = portCpuIntelGetVersion(&pCpuInfo->cpu.intel);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }

        status = portCpuGetIntelTopology(&pCpuInfo->cpu.intel);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }

        status = portCpuGetIntelTPMFeatures(
            &pCpuInfo->cpu.intel.tpmFeatures);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }

        status = portCpuGetIntelArchPerfMonitor(
            &pCpuInfo->cpu.intel.archPerfMonitor);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }

        status = portCpuGetIntelFeatures(&pCpuInfo->cpu.intel.features);
        if (status != NV_OK)
        {
            PORT_BREAKPOINT_DEBUG();
            return status;
        }
    }
    else if (portStringCompare(pCpuInfo->vendorId,
        PORT_CPU_CPUID_VENDOR_ID_AMD, PORT_CPU_CPUID_VENDOR_ID_LENGTH) == 0)
    {
        pCpuInfo->type = PORT_CPU_TYPE_AMD_X86;
    }

    return NV_OK;
}
#endif
