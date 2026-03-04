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
 * @brief CPUID macros - X86, X86-64.
 */

#define PORT_CPU_CPUID_STD_FUN_BASIC                            0x00U
#define PORT_CPU_CPUID_STD_FUN_VERSION                          0x01U
#define PORT_CPU_CPUID_STD_FUN_CACHE_TLB                        0x02U
#define PORT_CPU_CPUID_STD_FUN_SERIAL_NO                        0x03U
#define PORT_CPU_CPUID_STD_FUN_DETER_CACHE_PARAMS               0x04U
#define PORT_CPU_CPUID_STD_FUN_MONITOR_MWAIT                    0x05U
#define PORT_CPU_CPUID_STD_FUN_TPM                              0x06U
#define PORT_CPU_CPUID_STD_FUN_EXT_FEATURE_FLAGS                0x07U
#define PORT_CPU_CPUID_STD_FUN_DIRECT_CACHE_ACCESS              0x09U
#define PORT_CPU_CPUID_STD_FUN_ARCH_PERF_MONITOR                0x0AU
#define PORT_CPU_CPUID_STD_FUN_EXT_TOPOLOGY_ENUM                0x0BU
#define PORT_CPU_CPUID_STD_FUN_EXT_STATE_ENUM                   0x0DU

#define PORT_CPU_CPUID_STD_FUN_IRDT_ENUM                        0x0FU
#define PORT_CPU_CPUID_STD_SUBFUN_IRDT_MONITOR_ENUM             0x00U
#define PORT_CPU_CPUID_STD_SUBFUN_IRDT_CAPABILITY_ENUM          0x01U

#define PORT_CPU_CPUID_STD_FUN_IRDT_ALLOCTION_ENUM              0x10U
#define PORT_CPU_CPUID_STD_FUN_IRDT_L3_CACHE_ALLOC_ENUM         0x10U
#define PORT_CPU_CPUID_STD_SUBFUN_L3_CACHE_ALLOC_ENUM           0x00U
#define PORT_CPU_CPUID_STD_SUBFUN_L3_CACHE_ALLOC_TECH_1_ENUM    0x01U
#define PORT_CPU_CPUID_STD_SUBFUN_L3_CACHE_ALLOC_TECH_2_ENUM    0x02U
#define PORT_CPU_CPUID_STD_SUBFUN_L3_CACHE_ALLOC_TECH_3_ENUM    0x03U

#define PORT_CPU_CPUID_STD_FUN_ISGX_CAPABILITY_ENUM             0x12U
#define PORT_CPU_CPUID_STD_SUBFUN_ISGX_CAPABILITY_ENUM          0x00U

#define PORT_CPU_CPUID_STD_FUN_ISGX_ATTRIBUTES_ENUM             0x12U
#define PORT_CPU_CPUID_STD_SUBFUN_ISGX_ATTRIBUTES_ENUM          0x01U

#define PORT_CPU_CPUID_STD_FUN_ISGX_EPC_ENUM                    0x12U
#define PORT_CPU_CPUID_STD_SUBFUN_ISGX_EPC_ENUM                 0x02U

#define PORT_CPU_CPUID_STD_FUN_TRACE_ENUM                       0x14U
#define PORT_CPU_CPUID_STD_SUBFUN_TRACE_0_ENUM                  0x00U
#define PORT_CPU_CPUID_STD_SUBFUN_TRACE_1_ENUM                  0x01U

#define PORT_CPU_CPUID_STD_FUN_TSC_NCCCI                        0x15U
#define PORT_CPU_CPUID_STD_FUN_FREQ_INFO                        0x16U

#define PORT_CPU_CPUID_STD_FUN_SOC_VENDOR_ATTR_ENUM             0x17U
#define PORT_CPU_CPUID_STD_SUBFUN_SOC_VENDOR_ATTR_0_ENUM        0x00U
#define PORT_CPU_CPUID_STD_SUBFUN_SOC_VENDOR_ATTR_1_ENUM        0x01U
#define PORT_CPU_CPUID_STD_SUBFUN_SOC_VENDOR_ATTR_2_ENUM        0x02U
#define PORT_CPU_CPUID_STD_SUBFUN_SOC_VENDOR_ATTR_3_ENUM        0x03U

#define PORT_CPU_CPUID_STD_FUN_ADDRESS_TRANSLATION_PARAMS       0x18U
#define PORT_CPU_CPUID_STD_FUN_EXTENDED_TOPOLOGY_ENUM           0x1FU

#define PORT_CPU_CPUID_EXT_FUN_BASIC                            0x80000000U
#define PORT_CPU_CPUID_EXT_SUBFUN_0                             0x0U

#define PORT_CPU_CPUID_EXT_FUN_FEATURES                         0x80000001U

#define PORT_CPU_CPUID_STD_SUBFUN_0                             0x00U

#define PORT_CPU_CPUID_STEPPING_ID_MASK                         0x0000000FU
#define PORT_CPU_CPUID_STEPPING_ID_SHIFT                        0U
#define PORT_CPU_CPUID_GET_STEPPING_ID(a)                       ((a) \
                                                                & 0x0000000FU)

#define PORT_CPU_CPUID_MODEL_MASK                               0x000000F0U
#define PORT_CPU_CPUID_MODEL_SHIFT                              4U
#define PORT_CPU_CPUID_GET_MODEL(a)                             (((a) \
                                                                & 0x000000F0U)\
                                                                >> 4U)

#define PORT_CPU_CPUID_FAMILY_ID_MASK                           0x00000F00U
#define PORT_CPU_CPUID_FAMILY_ID_SHIFT                          8U
#define PORT_CPU_CPUID_GET_FAMILY(a)                            (((a) \
                                                                & 0x00000F00U)\
                                                                >> 8U)

#define PORT_CPU_CPUID_EXT_MODEL_MASK                           0x000F0000U
#define PORT_CPU_CPUID_EXT_MODEL_SHIFT                          16U
#define PORT_CPU_CPUID_GET_EXT_MODEL(a)                         (((a) \
                                                                & 0x000F0000U)\
                                                                >> 16U)

#define PORT_CPU_CPUID_EXT_FAMILY_ID_MASK                       0x0FF00000U
#define PORT_CPU_CPUID_EXT_FAMILY_ID_SHIFT                      20U
#define PORT_CPU_CPUID_GET_EXT_FAMILY(a)                        (((a) \
                                                                & 0x0FF00000U)\
                                                                >> 20U)

#define PORT_CPU_CPUID_PROCESSOR_TYPE_MASK                      0x00003000U
#define PORT_CPU_CPUID_PROCESSOR_TYPE_SHIFT                     12U
#define PORT_CPU_CPUID_GET_PROCESSOR_TYPE(a)                    (((a) \
                                                                & 0x00003000U)\
                                                                >> 12U)

#define PORT_CPU_CPUID_BRAND_INDEX_MASK                         0x000000FFU
#define PORT_CPU_CPUID_BRAND_INDEX_SHIFT                        0U
#define PORT_CPU_CPUID_GET_BRAND_INDEX(a)                       ((a) \
                                                                & 0x000000FFU)

#define PORT_CPU_CPUID_MAX_LOGICAL_PROC_MASK                    0x00FF0000U
#define PORT_CPU_CPUID_MAX_LOGICAL_PROC_SHIFT                   16U

#define PORT_CPU_CPUID_LOCAL_APIC_ID_MASK                       0xFF000000U
#define PORT_CPU_CPUID_LOCAL_APIC_ID_SHIFT                      24U
#define PORT_CPU_CPUID_GET_LOCAL_APIC_ID(a)                     (((a) \
                                                                & 0xFF000000U)\
                                                                >> 24U)

#define PORT_CPU_CPUID_FEATURE_SSE3_BIT                         (1U << 0U)
#define PORT_CPU_CPUID_FEATURE_PCLMULQDQ_BIT                    (1U << 1U)
#define PORT_CPU_CPUID_FEATURE_DTES64_BIT                       (1U << 2U)
#define PORT_CPU_CPUID_FEATURE_MONITOR_BIT                      (1U << 3U)
#define PORT_CPU_CPUID_FEATURE_DSCPL_BIT                        (1U << 4U)
#define PORT_CPU_CPUID_FEATURE_VMX_BIT                          (1U << 5U)
#define PORT_CPU_CPUID_FEATURE_SMX_BIT                          (1U << 6U)
#define PORT_CPU_CPUID_FEATURE_EIST_BIT                         (1U << 7U)
#define PORT_CPU_CPUID_FEATURE_TM2_BIT                          (1U << 8U)
#define PORT_CPU_CPUID_FEATURE_SSSE3_BIT                        (1U << 9U)
#define PORT_CPU_CPUID_FEATURE_CNXTID_BIT                       (1U << 10U)
#define PORT_CPU_CPUID_FEATURE_SDBG_BIT                         (1U << 11U)
#define PORT_CPU_CPUID_FEATURE_FMA_BIT                          (1U << 12U)
#define PORT_CPU_CPUID_FEATURE_CMPXCHG16B_BIT                   (1U << 13U)
#define PORT_CPU_CPUID_FEATURE_XTPRUPDATECONTROL_BIT            (1U << 14U)
#define PORT_CPU_CPUID_FEATURE_PDCM_BIT                         (1U << 15U)
#define PORT_CPU_CPUID_FEATURE_PCID_BIT                         (1U << 17U)
#define PORT_CPU_CPUID_FEATURE_DCA_BIT                          (1U << 18U)
#define PORT_CPU_CPUID_FEATURE_SSE41_BIT                        (1U << 19U)
#define PORT_CPU_CPUID_FEATURE_SSE42_BIT                        (1U << 20U)
#define PORT_CPU_CPUID_FEATURE_x2APIC_BIT                       (1U << 21U)
#define PORT_CPU_CPUID_FEATURE_MOVBE_BIT                        (1U << 22U)
#define PORT_CPU_CPUID_FEATURE_POPCNT_BIT                       (1U << 23U)
#define PORT_CPU_CPUID_FEATURE_TSCDEADLINE_BIT                  (1U << 24U)
#define PORT_CPU_CPUID_FEATURE_AES_BIT                          (1U << 25U)
#define PORT_CPU_CPUID_FEATURE_XSAVE_BIT                        (1U << 26U)
#define PORT_CPU_CPUID_FEATURE_OSXSAVE_BIT                      (1U << 27U)
#define PORT_CPU_CPUID_FEATURE_AVX_BIT                          (1U << 28U)
#define PORT_CPU_CPUID_FEATURE_F16C_BIT                         (1U << 29U)
#define PORT_CPU_CPUID_FEATURE_RDEND_BIT                        (1U << 30U)

#define PORT_CPU_CPUID_FEATURE_FPU_BIT                          (1U << 0U)
#define PORT_CPU_CPUID_FEATURE_VME_BIT                          (1U << 1U)
#define PORT_CPU_CPUID_FEATURE_DE_BIT                           (1U << 2U)
#define PORT_CPU_CPUID_FEATURE_PSE_BIT                          (1U << 3U)
#define PORT_CPU_CPUID_FEATURE_TSC_BIT                          (1U << 4U)
#define PORT_CPU_CPUID_FEATURE_MSR_BIT                          (1U << 5U)
#define PORT_CPU_CPUID_FEATURE_PAE_BIT                          (1U << 6U)
#define PORT_CPU_CPUID_FEATURE_MCE_BIT                          (1U << 7U)
#define PORT_CPU_CPUID_FEATURE_CX8_BIT                          (1U << 8U)
#define PORT_CPU_CPUID_FEATURE_APIC_BIT                         (1U << 9U)
#define PORT_CPU_CPUID_FEATURE_SEP_BIT                          (1U << 11U)
#define PORT_CPU_CPUID_FEATURE_MTRR_BIT                         (1U << 12U)
#define PORT_CPU_CPUID_FEATURE_PGE_BIT                          (1U << 13U)
#define PORT_CPU_CPUID_FEATURE_MCA_BIT                          (1U << 14U)
#define PORT_CPU_CPUID_FEATURE_CMOV_BIT                         (1U << 15U)
#define PORT_CPU_CPUID_FEATURE_PAT_BIT                          (1U << 16U)
#define PORT_CPU_CPUID_FEATURE_PSE36_BIT                        (1U << 17U)
#define PORT_CPU_CPUID_FEATURE_PSN_BIT                          (1U << 18U)
#define PORT_CPU_CPUID_FEATURE_CLFSH_BIT                        (1U << 19U)
#define PORT_CPU_CPUID_FEATURE_DS_BIT                           (1U << 21U)
#define PORT_CPU_CPUID_FEATURE_ACPI_BIT                         (1U << 22U)
#define PORT_CPU_CPUID_FEATURE_MMX_BIT                          (1U << 23U)
#define PORT_CPU_CPUID_FEATURE_FXSR_BIT                         (1U << 24U)
#define PORT_CPU_CPUID_FEATURE_SSE_BIT                          (1U << 25U)
#define PORT_CPU_CPUID_FEATURE_SSE2_BIT                         (1U << 26U)
#define PORT_CPU_CPUID_FEATURE_SS_BIT                           (1U << 27U)
#define PORT_CPU_CPUID_FEATURE_HTT_BIT                          (1U << 28U)
#define PORT_CPU_CPUID_FEATURE_TM_BIT                           (1U << 29U)
#define PORT_CPU_CPUID_FEATURE_PBE_BIT                          (1U << 31U)

#define PORT_CPU_CPUID_TPM_FEATURE_DTS_BIT                      (1U << 0U)
#define PORT_CPU_CPUID_TPM_FEATURE_ITB_BIT                      (1U << 1U)
#define PORT_CPU_CPUID_TPM_FEATURE_ARAT_BIT                     (1U << 2U)
#define PORT_CPU_CPUID_TPM_FEATURE_PLN_BIT                      (1U << 4U)
#define PORT_CPU_CPUID_TPM_FEATURE_ECMD_BIT                     (1U << 5U)
#define PORT_CPU_CPUID_TPM_FEATURE_PTM_BIT                      (1U << 6U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWP_BIT                      (1U << 7U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWP_NOTIFICATION_BIT         (1U << 8U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWP_ACTIVITY_WINDOW_BIT      (1U << 9U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWPEPP_BIT                   (1U << 10U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWPPLR_BIT                   (1U << 11U)
#define PORT_CPU_CPUID_TPM_FEATURE_HDC_BIT                      (1U << 12U)
#define PORT_CPU_CPUID_TPM_FEATURE_ITBMT30_BIT                  (1U << 13U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWP_CAPABILITIES_BIT         (1U << 14U)
#define PORT_CPU_CPUID_TPM_FEATURE_HWPPECI_BIT                  (1U << 15U)
#define PORT_CPU_CPUID_TPM_FEATURE_FLEXHWP_BIT                  (1U << 16U)
#define PORT_CPU_CPUID_TPM_FEATURE_FAM_BIT                      (1U << 17U)
#define PORT_CPU_CPUID_TPM_FEATURE_ILPHWP_REQUEST_BIT           (1U << 18U)

#define PORT_CPU_CPUID_TPM_FEATURE_NFITI_DTS_BIT                (1U << 0U)

#define PORT_CPU_CPUID_TPM_FEATURE_HCFC_BIT                     (1U << 0U)
#define PORT_CPU_CPUID_TPM_FEATURE_PEBP_BIT                     (1U << 3U)

//
// Refer chapter 18 of 64-ia-32-architectures-software-developer-vol-3b-part-2
// manual for Perf Monitor defines below
//
// Bits 7:0 of CPUID.0AH.EAX indicate the version of the Performance Monitor
#define PORT_CPU_ARCH_PERF_MONITOR_VERSION_ID(a)                ((a) & 0xFFU)
//
// Bits 15:8 of CPUID.0AH.EAX indicate the number of performance
// counters available on the logical processor
//
#define PORT_CPU_ARCH_PERF_MONITOR_COUNTERS(a)                  (((a) & 0xFF00U)\
                                                                >> 8U)
// Bits 23:16 of CPUID.0AH.EAX indicate the width of counters
#define PORT_CPU_ARCH_PERF_MONITOR_COUNTERS_BITWIDTH(a)         (((a) & 0xFF0000U)\
                                                                >> 16U)
//
// Bits 4:0 of CPUID.0AH.EDX indicate the number of fixed-function performance
// counters available per thread
//
#define PORT_CPU_ARCH_PERF_MONITOR_FIX_FUN_COUNTERS(a)          ((a) & 0x1FU)
// Bits 12:5 of CPUID.0AH.EDX indicate width of fixed-function counters
#define PORT_CPU_ARCH_PERF_MONITOR_FIX_FUN_COUNTERS_BITWIDTH(a) (((a) & 0x1FE0U) \
                                                                >> 5U)

#define PORT_CPU_ARCH_PERF_MONITOR_CORE_CYCLE_EVENT_BIT         (1U << 0U)
#define PORT_CPU_ARCH_PERF_MONITOR_INSTR_RET_EVENT_BIT          (1U << 1U)
#define PORT_CPU_ARCH_PERF_MONITOR_REF_CYCLE_EVENT_BIT          (1U << 2U)
#define PORT_CPU_ARCH_PERF_MONITOR_LL_CACHE_REF_EVENT_BIT       (1U << 3U)
#define PORT_CPU_ARCH_PERF_MONITOR_LL_CACHE_MIS_EVENT_BIT       (1U << 4U)
#define PORT_CPU_ARCH_PERF_MONITOR_BRANCH_INSTR_RET_EVENT_BIT   (1U << 5U)
#define PORT_CPU_ARCH_PERF_MONITOR_BRANCH_MISPRE_RET_EVENT_BIT  (1U << 6U)

#define PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LOGICAL_PROC_COUNT(a)    ((a) & 0xFFFFU)
#define PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LEVEL(a)                 ((a) & 0xFFU)
#define PORT_CPU_EXT_TOPOLOGY_ENUM_GET_LEVEL_TYPE(a)            (((a) & 0xFF00U) >> 8)

#define PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_SMT               1
#define PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_CORE              2
#define PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_MODULE            3
#define PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_TILE              4
#define PORT_CPU_EXT_TOPOLOGY_ENUM_LEVEL_TYPE_DIE               5

#define PORT_CPU_CPUID_FEATURE_GET_BIT(featureBitMap, bit)      (((featureBitMap) \
                                                                & (bit)) == (bit))

#define PORT_CPU_CPUID_VENDOR_ID_LENGTH                         13U
#define PORT_CPU_CPUID_VENDOR_ID_INTEL                          "GenuineIntel"
#define PORT_CPU_CPUID_VENDOR_ID_AMD                            "AuthenticAMD"
