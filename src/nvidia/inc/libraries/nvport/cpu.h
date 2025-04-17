/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief  CPU module public interface.
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_CPU_H_
#define _NVPORT_CPU_H_
/**
 * @defgroup NVPORT_CPU CPU specifice operations.
 *
 * @brief This module contains cpu specific operations.
 *
 * @{
 */
/**
 * @brief Initialize global CPU module state. This function is called by
 * @ref portInitialize.
 */
void portCpuInitialize(void);

/**
 * @brief Clean up global CPU module state. This function is called by
 * @ref portShutdown
 */
void portCpuShutdown(void);

/**
 * @name Core Functions
 * @{
 */
/**
 * @brief Read requested MSR
 *
 * @param [in]  address    Address of the MSR
 * @param [out] *pValue    Value of MSR
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portReadMsr(NvU32 address, NvU64 *pValue);

/**
 * @brief Write value to requested MSR
 *
 * @param [in]  address    Address of the MSR
 * @param [in]  value      Value to be written
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portWriteMsr(NvU32 address, NvU64 value);

 /// @} End Core functions

 /**
  * @name Extended Functions
  * @{
  */
 /**
  * @name Intel X86 Structures, unions and enums.
  * @{
  */

/**
* @brief Structure representing Intel Processor's general
* features broken down into individual bit fields.
*/
typedef struct PORT_CPU_INTEL_X86_FEATURES
{
    /// @{
    NvU32 SSE3 : 1; /**< Streaming SIMD Extensions 3.*/
    NvU32 PCLMULQDQ : 1; /**< PCLMULQDQ instruction.*/
    NvU32 DTES64 : 1; /**< 64-bit DS Area.*/
    NvU32 MONITOR : 1; /**< MONITOR/MWAIT.*/
    NvU32 DSCPL : 1; /**< CPL Qualified Debug Store.*/
    NvU32 VMX : 1; /**< Virtual Machine Extensions.*/
    NvU32 SMX : 1; /**< Safer Mode Extensions.*/
    NvU32 EIST : 1; /**< Enhanced Intel SpeedStep(R) technology*/
    NvU32 TM2 : 1; /**< Thermal Monitor 2.*/
    NvU32 SSSE3 : 1; /**< Supplemental Streaming SIMD Extensions 3*/
    NvU32 CNXTID : 1; /**< L1 Context ID*/
    NvU32 SDBG : 1; /**< IA32_DEBUG_INTERFACE MSR for silicon debug.*/
    NvU32 FMA : 1; /**< FMA extensions using YMM state.*/
    NvU32 CMPXCHG16B : 1; /**< CMPXCHG8B/CMPXCHG16B Compare and Exchange Bytes*/
    NvU32 xTPRUpdateControl : 1; /** supports changing
                                 IA32_MISC_ENABLE[bit 23].*/
    NvU32 PDCM : 1; /**< Perfmon and Debug Capability: supports the performance
                    and debug feature indication MSR IA32_PERF_CAPABILITIES.*/
    NvU32 PCID : 1; /**< Process-context identifiers: Supports PCIDs and that
                    software may set CR4.PCIDE to 1.*/
    NvU32 DCA : 1; /**< Supports the ability to prefetch data from a memory mapped
                   device.*/
    NvU32 SSE41 : 1; /**< Supports SSE4.1.*/
    NvU32 SSE42 : 1; /**< Supports SSE4.2.*/
    NvU32 x2APIC : 1; /**< Support x2APIC.*/
    NvU32 MOVBE : 1; /**< Supports MOVBE instruction.*/
    NvU32 POPCNT : 1; /**< Supports the POPCNT instruction.*/
    NvU32 TSCDeadline : 1; /**< The processor's local APIC timer supports
                           one-shot operation using a TSC deadline value.*/
    NvU32 AES : 1; /**< Supports the AESNI instruction extensions.*/
    NvU32 XSAVE : 1; /**< Supports the XSAVE/XRSTOR processor extended states
                     feature, the XSETBV/XGETBV instructions, and XCR0.*/
    NvU32 OSXSAVE : 1; /**< the OS has set CR4.OSXSAVE[bit 18] to enable
                       XSETBV/XGETBV instructions to access XCR0 and to support
                       processor extended state management using
                       XSAVE/XRSTOR.*/
    NvU32 AVX : 1; /**< The processor supports the AVX instruction
                   extensions.*/
    NvU32 F16C : 1; /**< processor supports 16-bit floating-point conversion
                    instructions.*/
    NvU32 RDEND : 1; /**< Processor supports RDRAND instruction.*/
    NvU32 FPU : 1; /**< Floating Point Unit On-Chip.*/
    NvU32 VME : 1; /**< Virtual 8086 Mode Enhancements.*/
    NvU32 DE : 1; /**< Debugging Extensions.*/
    NvU32 PSE : 1; /**< Page Size Extension.*/
    NvU32 TSC : 1; /**< Time Stamp Counter.*/
    NvU32 MSR : 1; /**< Model Specific Registers RDMSR and WRMSR
                   Instructions.*/
    NvU32 PAE : 1; /**< Physical Address Extension.*/
    NvU32 MCE : 1; /**< Machine Check Exception.*/
    NvU32 CX8 : 1; /**< CMPXCHG8B Instruction.*/
    NvU32 APIC : 1; /**< APIC On-Chip.*/
    NvU32 SEP : 1; /**< SYSENTER and SYSEXIT Instructions.*/
    NvU32 MTRR : 1; /**< Memory Type Range Registers.*/
    NvU32 PGE : 1; /**< Page Global Bit*/
    NvU32 MCA : 1; /**< Machine Check Architecture.*/
    NvU32 CMOV : 1; /**< Conditional Move Instructions.*/
    NvU32 PAT : 1; /**< Page Attribute Table.*/
    NvU32 PSE36 : 1; /**< 36-Bit Page Size Extension.*/
    NvU32 PSN : 1; /**< 96-Bit Processor Serial Number.*/
    NvU32 CLFSH : 1; /**< CLFLUSH Instruction.*/
    NvU32 DEBUGS : 1; /**< Debug Store.*/
    NvU32 ACPI : 1; /**< Thermal Monitor and Software Controlled Clock
                    Facilities.*/
    NvU32 MMX : 1; /**< Intel MMX Technology.*/
    NvU32 FXSR : 1; /**< FXSAVE and FXRSTOR Instructions.*/
    NvU32 SSE : 1; /**< SSE Extensions.*/
    NvU32 SSE2 : 1; /**< SSE2 extensions.*/
    NvU32 SELFS : 1; /**<  Self Snoop.*/
    NvU32 HTT : 1; /**< Max APIC IDs reserved field is Valid.*/
    NvU32 TM : 1; /**< Thermal Monitor.*/
    NvU32 PBE : 1; /**< Pending Break Enable.*/
    /// @}
} PORT_CPU_INTEL_X86_FEATURES;

/**
 * @brief Enum representing Intel processor family information.
 *
 */
typedef enum PORT_CPU_INTEL_FAMILY
{
    PORT_CPU_INTEL_FAMILY_6 = 6,
    PORT_CPU_INTEL_FAMILY_7 = 7
} PORT_CPU_INTEL_FAMILY;

/**
 * @brief Enum representing Intel family 6 processor model information.
 *
 */
typedef enum PORT_CPU_INTEL_FAMILY_6_MODEL
{
    PORT_CPU_INTEL_FAMLLY_6_MODEL_SANDYBRIDGE = 42,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_SANDYBRIDGE_X = 45,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_IVYBRIDGE = 58,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_IVYBRIDGE_X = 62,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_HASWELL = 60,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_HASWELL_X = 63,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_HASWELL_ULT = 69,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_HASWELL_GT3 = 70,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_BROADWELL = 61,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_BROADWELL_GT3 = 71,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_BROADWELL_X = 79,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_SKYLAKE = 94,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_SKYLAKE_MOBILE = 78,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_KABYLAKE = 158,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_KABYLAKE_MOBILE = 142,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_SKYLAKE_X = 85,
    PORT_CPU_INTEL_FAMLLY_6_MODEL_CANNONLAKE_MOBILE = 102,
    PORT_CPU_INTEL_FAMILY_6_MODEL_COMETLAKE_MOBILE = 166,
    PORT_CPU_INTEL_FAMILY_6_MODEL_COMETLAKE = 165,
    PORT_CPU_INTEL_FAMILY_6_MODEL_TIGERLAKE_MOBILE = 140,
    PORT_CPU_INTEL_FAMILY_6_MODEL_TIGERLAKE = 141,
} PORT_CPU_INTEL_FAMILY_6_MODEL;

/**
 * @brief Union representing Intel processor family information.
 *
 */
typedef union PORT_CPU_INTEL_MODEL
{
    PORT_CPU_INTEL_FAMILY_6_MODEL family6;
} PORT_CPU_INTEL_MODEL;

/**
 * @brief Enum representing Intel processor type information.
 *
 */
typedef enum PORT_CPU_INTEL_PROCESSOR_TYPE
{
    PORT_CPU_INTEL_PROCESSOR_TYPE_ORIGINAL_OEM = 0,
    PORT_CPU_INTEL_PROCESSOR_TYPE_OVERDRIVE = 1,
    PORT_CPU_INTEL_PROCESSOR_TYPE_DUAL_PROCESSOR = 2,
    PORT_CPU_INTEL_PROCESSOR_TYPE_RESERVED = 3
} PORT_CPU_INTEL_PROCESSOR_TYPE;

/**
 * @brief Structure representing Intel Processor's Threamal & Power Management
 * features broken down into individual bit fields.
 */
typedef struct PORT_CPU_INTEL_TPM_FEATURES
{
    /// @{
    NvU32 DTS : 1; /**< Digital Temperature Sensor is supported if set.*/
    NvU32 IntelTurboBoost : 1; /**< Intel Turbo Boost Technology available.*/
    NvU32 ARAT : 1; /**< APIC-Timer-always-running feature is supported
                    if set.*/
    NvU32 PLN : 1; /**< Power limit notification controls are supported
                   if set.*/
    NvU32 ECMD : 1; /**< Clock modulation duty cycle extension is supported
                    if set.*/
    NvU32 PTM : 1; /**< Package thermal management is supported if set.*/
    NvU32 HWP : 1; /**< HWP base registers (IA32_PM_ENABLE[bit 0],
                   IA32_HWP_CAPABILITIES, IA32_HWP_REQUEST, IA32_HWP_STATUS)
                   are supported if set.*/
    NvU32 HWPNotification : 1; /**< IA32_HWP_INTERRUPT MSR is supported
                               if set.*/
    NvU32 HWPActivityWindow : 1; /**< IA32_HWP_REQUEST[bits 41:32] is
                                 supported if set.*/
    NvU32 HWPEPP : 1; /**< HWP_Energy_Performance_Preference.
                      IA32_HWP_REQUEST[bits 31:24] is supported if set.*/
    NvU32 HWPPLR : 1; /**< HWP_Package_Level_Request. IA32_HWP_REQUEST_PKG MSR
                      is supported if set.*/
    NvU32 HDC : 1; /**< HDC base registers IA32_PKG_HDC_CTL, IA32_PM_CTL1,
                   IA32_THREAD_STALL MSRs are supported if set.*/
    NvU32 IntelTurboBoostMaxTech30 : 1; /**< Intel(R) Turbo Boost Max Technology
                                        3.0 available.*/
    NvU32 HWPCapabilities : 1; /**< Highest Performance change is supported
                               if set.*/
    NvU32 HWPPECI : 1; /**< HWP PECI override is supported if set.*/
    NvU32 FLEXHWP : 1; /**< Flexible HWP is supported if set.*/
    NvU32 FAM : 1; /**< Fast access mode for the IA32_HWP_REQUEST MSR is
                   supported if set.*/
    NvU32 ILPHWPRequest : 1; /**< Ignoring Idle Logical Processor HWP request
                             is supported if set.*/
    NvU32 NoOfInterruptThresholdsInDTS : 4; /**< Number of Interrupt Thresholds
                                            in Digital Thermal Sensor.*/
    NvU32 HCFC : 1; /**< Hardware Coordination Feedback Capability
                    (Presence of IA32_MPERF and IA32_APERF). The capability to
                    provide a measure of delivered processor performance
                    (since last reset of the counters), as a percentage of the
                    expected processor performance when running at the TSC
                    frequency.*/
    NvU32 PEBP : 1; /**< The processor supports performance-energy bias
                    preference if CPUID.06H:ECX.SETBH[bit 3] is set and it also
                    implies the presence of a new architectural MSR called
                    IA32_ENERGY_PERF_BIAS (1B0H).*/
    /// @}
} PORT_CPU_INTEL_TPM_FEATURES;

/**
 * @brief Structure representing Intel Processor's Architecture Performance
 * monitering features broken down into individual bit fields.
 */
typedef struct PORT_CPU_INTEL_ARCH_PERF_MONITOR
{
    /// @{
    NvU32 versionId; /**< Version ID of architectural performance monitoring.*/
    NvU32 noOfGPPerfMonitoringCounters; /**< Number of general-purpose
                                        performance monitoring counter per
                                        logical processor.*/
    NvU32 bitWidthOfGPCounters; /** Bit width of general-purpose, performance
                                monitoring counter.*/
    NvU32 coreCycleEvent : 1; /**<  Core cycle event available if 1.*/
    NvU32 instructionRetiredEvent : 1; /**< Instruction retired event
                                       available if 1.*/
    NvU32 referenceCycelEvent : 1; /**< Reference cycles event available if 1.*/
    NvU32 lastLevelCacheRefEvent : 1; /**< Last-level cache reference event
                                      available if 1.*/
    NvU32 lastLevelCacheMissEvent : 1; /**< Last-level cache misses event not
                                       available if 1.*/
    NvU32 branchInstructionRetiredEvent : 1; /**< Branch instruction retired
                                             event not available if 1.*/
    NvU32 branchMispredictRetiredEvent : 1; /**< Branch mispredict retired event
                                            not available if 1.*/
    NvU32 noOfFixedFuncPerfCounters; /**< Number of fixed-function performance
                                     counters (if Version ID > 1).*/
    NvU32 bitWidthOfFixedFuncPerfCounters; /**< Bit width of fixed-function
                                           performance counters
                                           (if Version ID > 1).*/
    /// @}
} PORT_CPU_INTEL_ARCH_PERF_MONITOR;

/**
 * @brief Structure representing Intel Processor version and features
 * broken down into individual fields.
 */
typedef struct PORT_CPU_INTEL
{
    /// @{
    PORT_CPU_INTEL_FAMILY family; /**< Family of the Processor.*/
    PORT_CPU_INTEL_MODEL model; /**< Model of the Processor.*/
    PORT_CPU_INTEL_PROCESSOR_TYPE processorType; /**< Processor Type.*/
    NvU8  steppingId; /**< Stepping ID of the Processor.*/
    NvU8  brandIndex; /**< Numerical Index of Brand String Index Table
                      entry.*/
    NvU8  localApicId; /** Local APIC ID of the Processor.*/
    NvU32 threadCountPerCore; /**< Threads Per Core.*/
    NvU32 physicalCoreCount; /**< Physical Cores Per Package.*/
    NvU32 logicalCoreCount; /**< Logical Cores Per Package.*/
    PORT_CPU_INTEL_X86_FEATURES features; /**< General Features.*/
    PORT_CPU_INTEL_TPM_FEATURES tpmFeatures; /**< Thermal and Power Management
                                             Features.*/
    PORT_CPU_INTEL_ARCH_PERF_MONITOR archPerfMonitor; /**< Architecture
                                                      Performance Monitoring
                                                      Features.*/
    /// @}
} PORT_CPU_INTEL;

/// @}

/**
 * @name AMD X86 Structures, unions and enums.
 * @{
 */

/**
 * @brief Enum representing AMD processor family information.
 *
 */
typedef enum PORT_CPU_AMD_FAMILY
{
    PORT_CPU_AMD_FAMILY_0 = 0,
    PORT_CPU_AMD_FAMILY_1 = 1,
    PORT_CPU_AMD_FAMILY_ZEN3 = 25
} PORT_CPU_AMD_FAMILY;

/**
 * @brief Enum representing AMD processor family 0 model information.
 *
 */
typedef enum PORT_CPU_AMD_FAMILY_0_MODEL
{
    PORT_CPU_AMD_FAMLLY_0_MODEL_X = 0,
} PORT_CPU_AMD_FAMILY_0_MODEL;

/**
 * @brief Union representing AMD processor family wise model information.
 *
 */
typedef union PORT_CPU_AMD_MODEL
{
    PORT_CPU_AMD_FAMILY_0_MODEL family0;
} PORT_CPU_AMD_MODEL;

/**
 * @brief Structure representing AMD Processor's Threamal & Power Management
 * features broken down into individual bit fields.
 */
typedef struct PORT_CPU_AMD_TPM_FEATURES
{
    /// @{
    NvU32 EffFreq : 1; /**< */
    /// @}
} PORT_CPU_AMD_TPM_FEATURES;

/**
 * @brief Structure representing AMD Processor version and features
 * broken down into individual fields.
 */
typedef struct PORT_CPU_AMD
{
    /// @{
    PORT_CPU_AMD_FAMILY family; /**< Family of the Processor.*/
    PORT_CPU_AMD_MODEL model; /**< Model of the Processor.*/
    NvU8  steppingId; /**< Stepping ID of the Processor.*/
    NvU8  brandIndex; /**< Numerical Index of Brand String Index Table
                      entry.*/
    NvU8  localApicId; /** Local APIC ID of the Processor.*/
    NvU32 threadCountPerCore; /**< Threads Per Core.*/
    NvU32 physicalCoreCount; /**< Physical Cores Per Package.*/
    NvU32 logicalCoreCount; /**< Logical Cores Per Package.*/
    PORT_CPU_AMD_TPM_FEATURES tpmFeatures; /**< Thermal and Power Management
                                             Features.*/
    /// @}
} PORT_CPU_AMD;

/// @}

/**
 * @name Generic CPU Information Structures, unions and enums.
 * @{
 */

/**
*@brief Maximum length of Vendor ID Null terminated string.
*/
#define PORT_CPU_VENDOR_ID_LENGTH 20

/**
*@brief Enum represening the Processor Architecture Type.
*/
typedef enum PORT_CPU_TYPE
{
    /// @{
    PORT_CPU_TYPE_INTEL_X86 = 0, /**< Intel X86/X86-64 Architecture.*/
    PORT_CPU_TYPE_AMD_X86 = 1, /**< AMD X86/AMD64 Architecture.*/
    PORT_CPU_TYPE_ARM = 2 /**< ARM Architecture.*/
    /// @}
} PORT_CPU_TYPE;

/**
*@brief Union represening the Abstract Processor data structure.
*/
typedef union PORT_CPU
{
    PORT_CPU_AMD amd;
    PORT_CPU_INTEL intel;
} PORT_CPU;

/**
 * @brief Structure representing processor information broken down into
 * individual fields.
 */
typedef struct PORT_CPU_INFORMATION
{
    /// @{
    PORT_CPU_TYPE type; /**< Type of Architecture/CPU.*/
    char vendorId[PORT_CPU_VENDOR_ID_LENGTH]; /**< Null terminated Vendor Id
                                              String.*/
    NvLength vendorIdLength; /**< Actual length of the null terminated Vendor
                             Id string.*/
    PORT_CPU cpu; /**< CPU specifice information.*/
    /// @}
} PORT_CPU_INFORMATION;

/**
 * @brief Structure representing processor logical topology information broken
 * down into individual fields.
 */
typedef struct PORT_CPU_LOGICAL_TOPOLOGY
{
    /// @{
    NvU64 activeCpuCount; /**< Active Logical CPUs.*/
    NvU64 activeGroupCount; /**< Active Logical CPU Group count.*/
    NvU64 maxCpuCount; /**< Maximum Logical CPUs system can support*/
    NvU64 maxGroupCount; /**< Maximum Logical CPUs Groups system can support*/
    NvU64 maxCpuPerGroup; /**< Maximum Logical CPUs system can support per group*/
    /// @}
} PORT_CPU_LOGICAL_TOPOLOGY;

/**
 * @brief Structure representing a BAR descriptor for a PCIe device
 */
typedef struct PORT_CPU_BAR_DESC
{
    /// @{
    void *pBarAddr; /**< Starting virtual address of the BAR space */
    NvU64 physAddr; /**< Starting physical address of the BAR space */
    NvU32 barSize; /**< Size of BAR space */
    /// @}
} PORT_CPU_BAR_DESC;

/// @} End Generic CPU Information Structures, unions and enums.

/**
 * @brief Get Logical Topology of CPU.
 * @param[out] pCpuTopology PORT_CPU_LOGICAL_TOPOLOGY pointer.
 * @return NV_OK If successful and cpu logical topology information
 *         in pCpuInfo structure.
 */
NV_STATUS portCpuGetLogicalTopology(PORT_CPU_LOGICAL_TOPOLOGY *pCpuTopology);
#define portCpuGetLogicalTopology_SUPPORTED (NVOS_IS_WINDOWS)

/**
 * @brief Get CPU Logical Topology Information.
 * @param[out] pCpuInfo PORT_CPU_INFORMATION pointer.
 * @return NV_OK If successful and CPU Information in pCpuInfo structure.
 */
NV_STATUS portCpuGetInfo(PORT_CPU_INFORMATION* pCpuInfo);
#define portCpuGetInfo_SUPPORTED (_X86_ || _AMD64_)

/**
 * @brief Get CPU information using CPUID Instruction (X86-64 Specifice)
 * @param[out] pCpuInfo     Pointer to array which return value
 *                          cpuInfo[0] = EAX,
 *                          cpuInfo[1] = EBX,
 *                          cpuInfo[2] = ECX,
 *                          cpuInfo[3] = EDX.
 * @param[in] functionId    Function Id of CPUID instruction to execute.
 * @param[in] subfunctionId Sub-Function Id of CPUID instruction to execute.
 *            subfunctionId enables you to gather additional information about
 *            the processor

 * @return NV_OK if successful, otherwise return errors.
 */
NV_STATUS portCpuExCpuId(NvU32* pCpuInfo, NvU32 functionId,
    NvU32 subfunctionId);
#define portCpuExCpuId_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))
/// @} End extended functions

/**
 * @brief Retrieve the current value and frequency of the performance counter
 *
 * @param[out] pFreq  A pointer to a variable to which this routine writes the
 *                    performance counter frequency, in ticks per second.
 *                    This parameter is optional and can be NULL if the caller
 *                    does not need the counter frequency value.
 *
 * @return  The performance counter value in units of ticks
 */
NvS64 portCpuExQueryPerformanceCounter(NvS64 *pFreq);
#define portCpuExQueryPerformanceCounter_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_ || _ARM64_))

/**
 * @brief Enable PMC read feature
 */
void portCpuExEnablePmc(void);
#define portCpuExEnablePmc_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Read requested PMC register
 *
 * @param [in]  address  Address of the PMC register
 * @param [out] *pValue  Value of PMC register
 *
 * @return  NV_OK  If successful.
 */
NV_STATUS portCpuExReadPmc(NvU32 address, NvU64 *pValue);
#define portCpuExReadPmc_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Fill in BAR descriptor of Integrated memory controller
 *
 * @param [in]  pImcBarDesc    Pointer to BAR descriptor structure
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExAllocImcBarDesc(PORT_CPU_BAR_DESC *pImcBarDesc);
#define portCpuExAllocImcBarDesc_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Free BAR descriptor of Integrated memory controller
 *
 * @param [in]  pImcBarDesc    Pointer to BAR descriptor structure
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExFreeImcBarDesc(PORT_CPU_BAR_DESC *pImcBarDesc);
#define portCpuExFreeImcBarDesc_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Reset Performance monitoring counters
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExResetPmu(void);
#define portCpuExResetPmu_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Program Performance monitoring counters
 *
 * @param [in]  numValidEvents  Number of valid events in array pPerfEvents
 * @param [in]  pPerfEvents     Array of events to be configured into general
 *                              purpose performance monitoring counters(PMCs)
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExProgramPmu(NvU32 numValidEvents, NvU32 *pPerfEvents);
#define portCpuExProgramPmu_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Get number of DRAM reads in terms of bytes
 *
 * @param [out]  pNumReads
 *
 * @return  NV_OK  If successful
 */
NV_STATUS portCpuExGetDRamReads(NvU64 *pNumReads);
#define portCpuExGetDRamReads_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Get number of DRAM writes in terms of bytes
 *
 * @param [out]  pNumWrites
 *
 * @return  NV_OK  If successful
 */
NV_STATUS portCpuExGetDRamWrites(NvU64 *pNumWrites);
#define portCpuExGetDRamWrites_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Check if the given MSR is supported on the current processor
 *
 * @param [in]  address  Address of the MSR that needs to be checked
 *
 * @return NV_TRUE  If MSR is supported
 *         NV_FALSE If MSR is not supported
 */
NvBool portCpuExIsMsrSupported(NvU32 address);
#define portCpuExIsMsrSupported_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_ || _ARM64_))

/**
 * @brief Check if the current processor supports DRAM read/write request counting
 *
 * @return NV_TRUE  If supported
 *         NV_FALSE If not supported
 */
NvBool portCpuExIsDramRwCountingSupported(void);
#define portCpuExIsDramRwCountingSupported_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS && (_X86_ || _AMD64_))

/**
 * @brief Acquire CPU counters resource before use
 *
 * @param [out] pResourceHandle  handle returned from allocation
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExAcquireHardwareCounters(NvP64* pResourceHandle);
#define portCpuExAcquireHardwareCounters_SUPPORTED (NVOS_IS_WINDOWS)

/**
 * @brief Release CPU counters resource after use
 *
 * @param [in]  resourceHandle  handle used to free the allocation
 *
 * @return  NV_OK    If successful.
 */
NV_STATUS portCpuExReleaseHardwareCounters(NvP64 resourceHandle);
#define portCpuExReleaseHardwareCounters_SUPPORTED (NVOS_IS_WINDOWS)

#endif // _NVPORT_CPU_H_
/// @}
