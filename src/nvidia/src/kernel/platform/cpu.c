/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************* CPU Information Gather Routines ***********************\
*                                                                           *
*   One time initialization code to setup the Processor type                *
*                                                                           *
\***************************************************************************/

#include "cpuopsys.h"

#include "Nvcm.h"
#include "os/os.h"
#include "core/system.h"

#include "ctrl/ctrl0000/ctrl0000system.h"


#if NVCPU_IS_AARCH64

#include "cpu_arm_def.h"

#if defined(__GNUC__)

#define CP_READ_REGISTER(reg)                                   \
    ({                                                          \
        NvU32 __res;                                            \
                                                                \
        asm("mrs %0, " reg "\r\t"                               \
            : "=r" (__res)                                      \
           );                                                   \
                                                                \
        __res;                                                  \
    })

#define CP_WRITE_REGISTER(reg, val)                             \
    ({                                                          \
        asm("msr " reg ", %0\r\t"                               \
            :                                                   \
            : "r" (val)                                         \
           );                                                   \
    })

#endif //end defined(__GNUC__)

static void DecodeAarch64Cache(OBJSYS *pSys)
{
    NvU32 val, field, numsets, assoc, linesize;

    // Select level 1 data cache
    CP_WRITE_CSSELR_REGISTER(CP_CSSELR_DATA_CACHE);

    // Retrieve data cache information
    val = CP_READ_CCSIDR_REGISTER();

    field    = GET_BITMASK(CCSIDR_CACHE_NUM_SETS, val);
    numsets  = field + 1;
    field    = GET_BITMASK(CCSIDR_CACHE_ASSOCIATIVITY, val);
    assoc    = field + 1;
    field    = GET_BITMASK(CCSIDR_CACHE_LINE_SIZE, val);
    linesize = 1 << (field + 4);

    pSys->cpuInfo.dataCacheLineSize = linesize;
    pSys->cpuInfo.l1DataCacheSize   = (numsets * assoc * linesize) >> 10;

    // Select level 2 data cache
    CP_WRITE_CSSELR_REGISTER(CP_CSSELR_DATA_CACHE_LEVEL2);

    // Retrieve data cache information
    val = CP_READ_CCSIDR_REGISTER();

    field    = GET_BITMASK(CCSIDR_CACHE_NUM_SETS, val);
    numsets  = field + 1;
    field    = GET_BITMASK(CCSIDR_CACHE_ASSOCIATIVITY, val);
    assoc    = field + 1;
    field    = GET_BITMASK(CCSIDR_CACHE_LINE_SIZE, val);
    linesize = 1 << (field + 4);

    pSys->cpuInfo.l2DataCacheSize = (numsets * assoc * linesize) >> 10;
}

/*
 * ID the CPU.
 */
void RmInitCpuInfo(void)
{
#define AARCH64_VENDOR_PART_NUMBER(v, p) \
        (((v)<<16)|(p))
#define AARCH64_VENDOR_PART(v, p) \
        AARCH64_VENDOR_PART_NUMBER(CP_MIDR_IMPLEMENTER_##v, CP_MIDR_PRIMARY_PART_NUM_##p)

    OBJSYS *pSys = SYS_GET_INSTANCE();

    if (pSys->cpuInfo.bInitialized)
    {
        return;
    }

    // Init structure to default
    portMemSet(&pSys->cpuInfo, 0, sizeof(pSys->cpuInfo));

    // ARM has the equivalent of a fence instruction (DSB)

    // Leave this here for MODS
    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN;
    pSys->cpuInfo.caps = (NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE |
                          NV0000_CTRL_SYSTEM_CPU_CAP_WRITE_COMBINING);

    // Calculate the frequency
    pSys->cpuInfo.clock = osGetCpuFrequency();

    // Number of core is available from SCU configuration.
    pSys->cpuInfo.numPhysicalCpus = osGetCpuCount();

    // There is no hyper-threading on ARM
    pSys->cpuInfo.numLogicalCpus = pSys->cpuInfo.numPhysicalCpus;
    pSys->cpuInfo.maxLogicalCpus = pSys->cpuInfo.numPhysicalCpus;

    // Zero out the vendor-specific family, model & stepping
    pSys->cpuInfo.family = 0;
    pSys->cpuInfo.model  = 0;
    pSys->cpuInfo.stepping = 0;

    NvU32 val;
    NvU32 impl;
    NvU32 part;

    // Retrieve Main ID register
    val = CP_READ_MIDR_REGISTER();

    impl = GET_BITMASK(MIDR_IMPLEMENTER, val);
    part = GET_BITMASK(MIDR_PRIMARY_PART_NUM, val);

    switch(AARCH64_VENDOR_PART_NUMBER(impl, part))
    {
        case AARCH64_VENDOR_PART(NVIDIA, DENVER_1):
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_1_0;
            break;
        case AARCH64_VENDOR_PART(NVIDIA, DENVER_2):
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_NV_DENVER_2_0;
            break;

        case AARCH64_VENDOR_PART(NVIDIA, CARMEL):
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV8A_GENERIC;
            break;
        /*
         * Applied Micro is now Ampere computing, and the Ampere eMag
         * vendor/part ids are the same as AMCC XGENE
         */
        case AARCH64_VENDOR_PART(AMCC, XGENE):
        case AARCH64_VENDOR_PART(ARM, CORTEX_A76):
        case AARCH64_VENDOR_PART(MARVELL, THUNDER_X2):
        case AARCH64_VENDOR_PART(HUAWEI, KUNPENG_920):
        case AARCH64_VENDOR_PART(ARM, BLUEFIELD):
        case AARCH64_VENDOR_PART(ARM, BLUEFIELD3):
        // The Neoverse N1 is the same as Gravitron
        case AARCH64_VENDOR_PART(ARM, GRAVITRON2):
        case AARCH64_VENDOR_PART(FUJITSU, A64FX):
        case AARCH64_VENDOR_PART(PHYTIUM, FT2000):
        case AARCH64_VENDOR_PART(PHYTIUM, S2500):
        case AARCH64_VENDOR_PART(PHYTIUM, S5000):
        case AARCH64_VENDOR_PART(AMPERE, ALTRA):
        case AARCH64_VENDOR_PART(MARVELL, OCTEON_CN96XX):
        case AARCH64_VENDOR_PART(MARVELL, OCTEON_CN98XX):
        case AARCH64_VENDOR_PART(ARM, CORTEX_A57):
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV8A_GENERIC;
            break;
        case AARCH64_VENDOR_PART(ARM, NEOVERSE_N2):
        case AARCH64_VENDOR_PART(ARM, NEOVERSE_V2):
        case AARCH64_VENDOR_PART(AMPERE_2, AMPEREONE):
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV9A_GENERIC;
            break;
        default:
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARMV8A_GENERIC;
            portDbgPrintf(
                "NVRM: CPUID: unknown implementer/part 0x%x/0x%x.\n", impl, part);
            portDbgPrintf(
                "The NVIDIA GPU driver for AArch64 has not been qualified on this CPU\n"
                "and therefore it is not recommended or intended for use in any production\n"
                "environment.\n");
            break;
    }
    DecodeAarch64Cache(pSys);

    // Host native page size
#ifdef PAGE_SIZE
    pSys->cpuInfo.hostPageSize = PAGE_SIZE;
#else
    pSys->cpuInfo.hostPageSize = 4096;
#endif

    pSys->cpuInfo.bInitialized = NV_TRUE;
#undef AARCH64_VENDOR_PART
#undef AARCH64_VENDOR_PART_NUMBER
}

#endif // NVCPU_IS_AARCH64


/***************************************************************************/


#if NVCPU_IS_ARM

#include "cpu_arm_def.h"

#if defined(__GNUC__)

    #define CP_READ_REGISTER(reg)                                   \
        ({                                                          \
            NvU32 __res;                                            \
                                                                    \
            asm("mrc p15, " reg ", %0, c0, c0, 0\r\t"               \
                : "=r" (__res)                                      \
                :                                                   \
                : "cc");                                            \
                                                                    \
            __res;                                                  \
        })

    #define CP_WRITE_REGISTER(reg, val)                             \
        ({                                                          \
            asm("mcr p15, " reg ", %0, c0, c0, 0\r\t"               \
                :                                                   \
                : "r"(val));                                        \
        })

#endif //end defined(__GNUC__)

/*
 * Documentation:
 *
 * https://developer.arm.com/documentation/ddi0388/f/CIHHDACH
 */
static void DecodeCortexA9Cache(OBJSYS *pSys)
{
    NvU32 val, field;

    // Select data cache
    CP_WRITE_CSSELR_REGISTER(CP_CSSELR_DATA_CACHE);

    // Retrieve data cache information
    val = CP_READ_CCSIDR_REGISTER();

    // L1 Data Cache Size (from KB to KB)
    field = GET_BITMASK(CCSIDR_CACHE_NUM_SETS, val);

    if (field == CP_CCSIDR_CACHE_NUM_SETS_16KB)
    {
        pSys->cpuInfo.l1DataCacheSize = 16;
    }
    else if (field == CP_CCSIDR_CACHE_NUM_SETS_32KB)
    {
        pSys->cpuInfo.l1DataCacheSize = 32;
    }
    else if (field == CP_CCSIDR_CACHE_NUM_SETS_64KB)
    {
        pSys->cpuInfo.l1DataCacheSize = 64;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "CPUID: Couldn't find L1DataCacheSize.\n");
    }

    // There is only one level of cache in the Cortex-A9 processor
    pSys->cpuInfo.l2DataCacheSize = 0;

    // Data Cache Line (from W to B)
    field = GET_BITMASK(CCSIDR_CACHE_LINE_SIZE, val);

    if (field & CP_CCSIDR_CACHE_LINE_SIZE_8W)
    {
        pSys->cpuInfo.dataCacheLineSize = 8 * 4;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "CPUID: Couldn't find DataCacheLineSize.\n");
    }
}

static NvU32 DecodeCortexA15CacheSize(NvU32 field)
{
    switch(field)
    {
        case CP_CCSIDR_CACHE_NUM_SETS_A15_32KB:
            return 32;
        case CP_CCSIDR_CACHE_NUM_SETS_A15_512KB:
            return 512;
        case CP_CCSIDR_CACHE_NUM_SETS_A15_1024KB:
            return 1024;
        case CP_CCSIDR_CACHE_NUM_SETS_A15_2048KB:
            return 2048;
        case CP_CCSIDR_CACHE_NUM_SETS_A15_4096KB:
            return 4096;
        default:
            NV_PRINTF(LEVEL_ERROR, "CPUID: Couldn't find DataCacheSize.\n");
            return 0;
    }
}

static void DecodeCortexA15Cache(OBJSYS *pSys)
{
    NvU32 val, field;

    // Select level 1 data cache
    CP_WRITE_CSSELR_REGISTER(CP_CSSELR_DATA_CACHE);

    // Retrieve data cache information
    val = CP_READ_CCSIDR_REGISTER();

    // L1 Data Cache Size (from KB to KB)
    field = GET_BITMASK(CCSIDR_CACHE_NUM_SETS, val);

    pSys->cpuInfo.l1DataCacheSize = DecodeCortexA15CacheSize(field);

    // Data Cache Line (from W to B)
    field = GET_BITMASK(CCSIDR_CACHE_LINE_SIZE, val);

    // line size = 2 ** (field + 2) words
    pSys->cpuInfo.dataCacheLineSize = 4 * (1 << (field + 2));

    // Select level 2 data cache
    CP_WRITE_CSSELR_REGISTER(CP_CSSELR_DATA_CACHE_LEVEL2);

    // Retrieve data cache information
    val = CP_READ_CCSIDR_REGISTER();

    // L2 Data Cache Size (from KB to KB)
    field = GET_BITMASK(CCSIDR_CACHE_NUM_SETS, val);

    pSys->cpuInfo.l2DataCacheSize = DecodeCortexA15CacheSize(field);
}

/*
 * ID the CPU.
 */
void RmInitCpuInfo(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    if (pSys->cpuInfo.bInitialized)
    {
        return;
    }

    // Init structure to default
    portMemSet(&pSys->cpuInfo, 0, sizeof(pSys->cpuInfo));

    // ARM has the equivalent of a fence instruction (DSB)

    // Leave this here for MODS
    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN;
    pSys->cpuInfo.caps = (NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE |
                          NV0000_CTRL_SYSTEM_CPU_CAP_WRITE_COMBINING);

    // Calculate the frequency
    pSys->cpuInfo.clock = osGetCpuFrequency();

    // Number of core is available from SCU configuration.
    pSys->cpuInfo.numPhysicalCpus = osGetCpuCount();
    pSys->cpuInfo.maxLogicalCpus = pSys->cpuInfo.numPhysicalCpus;

    // There is no hyper-threading on ARM
    pSys->cpuInfo.numLogicalCpus = pSys->cpuInfo.numPhysicalCpus;

    // Zero out the vendor-specific family, model & stepping
    pSys->cpuInfo.family = 0;
    pSys->cpuInfo.model  = 0;
    pSys->cpuInfo.stepping = 0;

    NvU32 val;
    NvU32 field;

    // Retrieve Main ID register
    val = CP_READ_MIDR_REGISTER();

    field = GET_BITMASK(MIDR_PRIMARY_PART_NUM, val);

    switch(field)
    {
        case CP_MIDR_PRIMARY_PART_NUM_A9:
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A9;
            DecodeCortexA9Cache(pSys);
            break;
        case CP_MIDR_PRIMARY_PART_NUM_A15:
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_A15;
            DecodeCortexA15Cache(pSys);
            break;
        default:
            // Narrow down to an unknown arm cpu
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ARM_UNKNOWN;
            NV_PRINTF(LEVEL_ERROR, "CPUID: unknown part number 0x%x.\n",
                      field);
            break;
    }

    // Host native page size
#ifdef PAGE_SIZE
    pSys->cpuInfo.hostPageSize = PAGE_SIZE;
#else
    pSys->cpuInfo.hostPageSize = 4096;
#endif

    pSys->cpuInfo.bInitialized = NV_TRUE;
}

#endif // NVCPU_IS_ARM


/***************************************************************************/


#if NVCPU_IS_PPC64LE

/*
 * ID the CPU.
 */
void RmInitCpuInfo(void)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();

    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_POWERN;
    pSys->cpuInfo.caps = 0;

    // Zero out the vendor-specific family, model & stepping
    pSys->cpuInfo.family = 0;
    pSys->cpuInfo.model  = 0;
    pSys->cpuInfo.stepping = 0;

    // Calculate the frequency
    pSys->cpuInfo.clock = osGetCpuFrequency();

    // Number of CPUs.
    // Should maybe take into account SMT, etc.
    pSys->cpuInfo.numPhysicalCpus = osGetCpuCount();
    pSys->cpuInfo.numLogicalCpus = pSys->cpuInfo.numPhysicalCpus;
    pSys->cpuInfo.maxLogicalCpus = pSys->cpuInfo.numPhysicalCpus;

    // host native page size
    pSys->cpuInfo.hostPageSize = 64 * 1024;

    return;
}

#endif // NVCPU_IS_PPC64LE


/***************************************************************************/


#if NVCPU_IS_RISCV64

/*
 * ID the CPU. (stub)
 */
void RmInitCpuInfo(
    void
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();

    // XXX
    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN;

    // Zero out the vendor-specific family, model & stepping
    pSys->cpuInfo.family = 0;
    pSys->cpuInfo.model  = 0;
    pSys->cpuInfo.stepping = 0;

    // Calculate the frequency
    pSys->cpuInfo.clock = 1;

    // host native page size
    pSys->cpuInfo.hostPageSize = 4096;
}

#endif // NVCPU_IS_RISCV64


/***************************************************************************/


#if NVCPU_IS_X86 || NVCPU_IS_X86_64

#include "platform/cpu.h"

#if defined(_M_IX86) || defined(NVCPU_X86) || defined(AMD64) || defined(NVCPU_X86_64)

// bits returned in EDX register by CPUID instruction with EAX=1
#define CPU_STD_TSC                 NVBIT(4)
#define CPU_STD_CMOV                NVBIT(15)   // Supports conditional move instructions.
#define CPU_STD_CLFSH               NVBIT(19)   // Supports CLFLUSH instruction.
#define CPU_STD_MMX                 NVBIT(23)
#define CPU_STD_FXSR                NVBIT(24)   // Indicates CR4.OSFXSR is available.
#define CPU_STD_SSE                 NVBIT(25)   // Katmai
#define CPU_STD_SSE2                NVBIT(26)   // Willamette NI

// bits returned in ECX register by CPUID instruction with EAX=1
#define CPU_STD2_SSE3               NVBIT(0)
#define CPU_STD2_SSE41              NVBIT(19)
#define CPU_STD2_SSE42              NVBIT(20)
#define CPU_STD2_OSXSAVE            NVBIT(27)   // Indicates the OS supports XSAVE/XRESTOR
#define CPU_STD2_AVX                NVBIT(28)

// "Extended Feature Flags" - bits returned in EDX register by CPUID
// instruction with EAX=0x80000001
#define CPU_EXT_3DNOW               NVBIT(31)   // 3DNow
#define CPU_EXT_AMD_3DNOW_EXT       NVBIT(30)   // 3DNow, with Extensions (AMD specific)
#define CPU_EXT_AMD_MMX_EXT         NVBIT(22)   // MMX, with Extensions (AMD specific)

// "Structured Extended Feature Identifiers" - bits returned in EBX
// register by CPUID instruction with EAX=7
#define CPU_EXT2_ERMS               NVBIT(9)

/*
 * Identify chip foundry.
 *      IS_INTEL   = "GenuineIntel"
 *      IS_AMD     = "AuthenticAMD"
 *      IS_WINCHIP = "CentaurHauls"
 *      IS_CYRIX   = "CyrixInstead"
 *      IS_TRANSM  = "GenuineTMx86"  // Transmeta
 */
#define IS_INTEL(fndry)     (((fndry).StrID[0]==0x756E6547)&&((fndry).StrID[1]==0x49656E69)&&((fndry).StrID[2]==0x6C65746E))
#define IS_AMD(fndry)       (((fndry).StrID[0]==0x68747541)&&((fndry).StrID[1]==0x69746E65)&&((fndry).StrID[2]==0x444D4163))
#define IS_WINCHIP(fndry)   (((fndry).StrID[0]==0x746E6543)&&((fndry).StrID[1]==0x48727561)&&((fndry).StrID[2]==0x736C7561))
#define IS_CYRIX(fndry)     (((fndry).StrID[0]==0x69727943)&&((fndry).StrID[1]==0x736E4978)&&((fndry).StrID[2]==0x64616574))
#define IS_TRANSM(fndry)    (((fndry).StrID[0]==0x756E6547)&&((fndry).StrID[1]==0x54656E69)&&((fndry).StrID[2]==0x3638784D))

// CPUID Info
// Used internally in this source.

typedef struct _def_CPUID_info
{
    union
    {
        NvU8 String[12];
        NvU32 StrID[3];
    } Foundry;

    NvU32 StandardFeatures;
    NvU32 ExtendedFeatures;

    NvU16 Family;
    NvU16 ExtFamily;
    NvU16 DisplayedFamily;
    NvU8 Model;
    NvU8 ExtModel;
    NvU8 DisplayedModel;
    NvU8 Stepping;
    NvU32 BrandId;
} CPUIDINFO, *PCPUIDINFO;

// Forward refernces.
//

static void getCpuCounts(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);
static NvBool getEmbeddedProcessorName(char *pName, NvU32 size);
static void cpuidInfoAMD(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);
static void cpuidInfoIntel(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);

#if defined(_M_IX86) || defined(NVCPU_X86)
static void cpuidInfoWinChip(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);
static void cpuidInfoCyrix(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);
static void cpuidInfoTransmeta(OBJSYS *pSys, PCPUIDINFO pCpuidInfo);
#endif


/*
 * ID the CPU.
 */

void RmInitCpuInfo(void)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    CPUIDINFO  cpuinfo;
    NvU32      eax, ebx, ecx, edx;

    // Do this only once.
    if (pSys->cpuInfo.bInitialized)
        return;

    // Initialize the processor structure to default values.
    //
    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_UNKNOWN;
    pSys->cpuInfo.caps = 0;
    pSys->cpuInfo.clock = 0;
    pSys->cpuInfo.dataCacheLineSize = 0;
    pSys->cpuInfo.l1DataCacheSize = 0;
    pSys->cpuInfo.l2DataCacheSize = 0;
    pSys->cpuInfo.coresOnDie = 0;
    pSys->cpuInfo.platformID = 0;
    portMemSet(pSys->cpuInfo.name, 0, sizeof(pSys->cpuInfo.name));

    // Init internal structure to default.
    //
    portMemSet(&cpuinfo, 0, sizeof(cpuinfo));

    // Get CPUID stuff for all processors.  We will figure out what to do with it later.

    // if osNv_cpuid returns 0, then this cpu does not support cpuid instruction
    // We just worry about this on the first call...
    if ( ! osNv_cpuid(0, 0, &eax, &cpuinfo.Foundry.StrID[0],
            &cpuinfo.Foundry.StrID[2], &cpuinfo.Foundry.StrID[1]))
        goto Exit;

    osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    cpuinfo.Family = (NvU16)((eax >> 8) & 0x0F);
    cpuinfo.ExtFamily = (NvU16)((eax >> 20) & 0xFF);
    if (cpuinfo.Family != 0xF)
    {
        cpuinfo.DisplayedFamily = cpuinfo.Family;
    }
    else
    {
        cpuinfo.DisplayedFamily = cpuinfo.ExtFamily + cpuinfo.Family;
    }

    cpuinfo.Model = (NvU8)((eax >> 4) & 0x0F);
    cpuinfo.ExtModel = (NvU8)((eax >> 16) & 0x0F);
    if (cpuinfo.Family == 6 || cpuinfo.Family == 0xF)
    {
        cpuinfo.DisplayedModel = (cpuinfo.ExtModel << 4) + cpuinfo.Model;
    }
    else
    {
        cpuinfo.DisplayedModel = cpuinfo.Model;
    }

    cpuinfo.Stepping = (NvU8)(eax & 0x0F);
    cpuinfo.StandardFeatures = edx;
    cpuinfo.BrandId = ((ebx & 0xE0) << 3) | (ebx & 0x1F); // 8bit brandID in 12 bit format

    // Decode the standard features.  Assume that all CPU vendors use the
    // standard feature bits to mean the same thing.  Non-Intel vendors use
    // the extended CPUID to provide non-standard freture bits, so this
    // should be OK.

    if (cpuinfo.StandardFeatures & CPU_STD_MMX)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_MMX;

    if (cpuinfo.StandardFeatures & CPU_STD_CMOV)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_CMOV;

    if (cpuinfo.StandardFeatures & CPU_STD_CLFSH)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_CLFLUSH;

    // Check for Streaming SIMD extensions (Katmai)
    if (cpuinfo.StandardFeatures & CPU_STD_SSE)
    {

        // SFENCE is an SSE instruction, but it does not require CR4.OSFXSR.
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE;

        if (cpuinfo.StandardFeatures & CPU_STD_FXSR)
        {
            NvBool check_osfxsr;
            NvBool check_osxsave;
            // Before setting the NV0000_CTRL_SYSTEM_CPU_CAP_SSE bit, we'll
            // also check that CR4.OSFXSR (bit 9) is set, which means the OS
            // is prepared to switch the additional SSE FP state for us.
            // CPU_STD_FXSR indicates that CR4.OSFXSR is valid.
            check_osfxsr = ((cpuinfo.StandardFeatures & CPU_STD_FXSR) != 0) &&
                           ((osNv_rdcr4() & 0x200) != 0);

            // For NV0000_CTRL_SYSTEM_CPU_CAP_AVX bit, we need:
            // - CPU_STD2_OSXSAVE - CR4.OSXSAVE is valid
            // - CR4.OSXSAVE (bit 18) - The OS will the additional FP state
            //     specified by XCR0
            // - XCR0 - bits 1 and 2 indicate SSE and AVX support respectively
            check_osxsave = ((ecx & CPU_STD2_OSXSAVE) != 0) &&
                            ((osNv_rdcr4() & (1<<18)) != 0) &&
                            ((osNv_rdxcr0() & 0x6) != 0);
            if(check_osfxsr)
            {
                pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SSE;

                // supports SSE2 (Willamette NI) instructions
                if (cpuinfo.StandardFeatures & CPU_STD_SSE2)
                    pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SSE2;

                // Prescott New Instructions
                if (ecx & CPU_STD2_SSE3)
                    pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SSE3;

                // Penryn subset of SSE4
                if (ecx & CPU_STD2_SSE41)
                    pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SSE41;

                // Nehalem subset of SSE4
                if (ecx & CPU_STD2_SSE42)
                    pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SSE42;
            }

            // If the OS setup XSAVE / XRESTOR (and set the AVX bit)
            //   enable AVX
            if (check_osxsave)
            {
                if (ecx & CPU_STD2_AVX)
                    pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_AVX;
            }
        }
    }

    if (osNv_cpuid(7, 0, &eax, &ebx, &ecx, &edx))
    {
        if (ebx & CPU_EXT2_ERMS)
        {
            pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_ERMS;
        }
    }

    // Calculate the frequency
    if (cpuinfo.StandardFeatures & CPU_STD_TSC)
        pSys->cpuInfo.clock = osGetCpuFrequency();

    // Get the extended features (if they exist).
    if (osNv_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx) && eax >= 0x80000001)
    {
        if (osNv_cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx))
        {
            cpuinfo.ExtendedFeatures = edx;
            // if 8 bit brandId is 0
            if (!cpuinfo.BrandId)
            {
                // Check for 12 bit brand ID
                cpuinfo.BrandId = (ebx & 0xfff);
            }
        }
   }

    // Get the embedded processor name (if there is one).
    getEmbeddedProcessorName(pSys->cpuInfo.name, sizeof(pSys->cpuInfo.name));

    if (IS_INTEL(cpuinfo.Foundry))
        cpuidInfoIntel(pSys, &cpuinfo);
    else if (IS_AMD(cpuinfo.Foundry))
        cpuidInfoAMD(pSys, &cpuinfo);
#if defined(_M_IX86) || defined(NVCPU_X86)
    else if (IS_WINCHIP(cpuinfo.Foundry))
        cpuidInfoWinChip(pSys, &cpuinfo);
    else if (IS_CYRIX(cpuinfo.Foundry))
        cpuidInfoCyrix(pSys, &cpuinfo);
    else if (IS_TRANSM(cpuinfo.Foundry))
        cpuidInfoTransmeta(pSys, &cpuinfo);
#endif
    else
    {
        // We are clueless.  If the processor had an embedded name, its already in there.
        // If not, use the foundary name as the processor name.
        if (pSys->cpuInfo.name[0] == 0)
            portMemCopy(pSys->cpuInfo.name, sizeof(cpuinfo.Foundry.String), cpuinfo.Foundry.String, sizeof(cpuinfo.Foundry.String));
    }

    // Pick up the vendor-specific family & model
    pSys->cpuInfo.family = cpuinfo.DisplayedFamily;
    pSys->cpuInfo.model  = cpuinfo.DisplayedModel;

#if defined(AMD64) || defined(NVCPU_X86_64)
    // The WinXP AMD-64 does not context switch the x87/MMX/3DNow registers.  We have to zap the bits
    // even though the CPU supports them.
    // The OS should somehow tell us this, like CR4.OSFXSR above.  Need to find a better way...

    pSys->cpuInfo.caps &= ~(NV0000_CTRL_SYSTEM_CPU_CAP_MMX |
                            NV0000_CTRL_SYSTEM_CPU_CAP_MMX_EXT |
                            NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW |
                            NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW_EXT);
#endif

    pSys->cpuInfo.stepping = cpuinfo.Stepping;
    pSys->cpuInfo.brandId = cpuinfo.BrandId;

 Exit:

    // set physical/logical processor counts
    getCpuCounts(pSys, &cpuinfo);

    // host page size used when allocated host-page-aligned objects in heap
#ifdef PAGE_SIZE
    pSys->cpuInfo.hostPageSize = PAGE_SIZE;
#else
    pSys->cpuInfo.hostPageSize = 4096;
#endif

    pSys->cpuInfo.bInitialized = NV_TRUE;
}

//
// This routine determines the number of physical processors enabled
// on the system as well as the number of logical processors per
// physical processors.  Intel's HyperThreading technology can yield
// a logical processor count of > 1 per physical processor.
//
// This code was more or less lifted from some Intel sample code.
//

#define INTEL_HT_BIT             0x10000000      // EDX[28]
#define INTEL_CORE_CNT           0xFC000000      // EAX[31:26]
#define INTEL_LOGICAL_CNT        0x00FF0000      // EBX[23:16]
#define INTEL_LOGICAL_CNT_LEAFB  0x0000FFFF      // EBX[15:0]
#define AMD_HT_BIT               0x10000000      // EDX[28]
#define AMD_LOGICAL_CNT          0x00FF0000      // EBX[23:16]

static void
getCpuCounts(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    NvU32  numPhysicalCpus, numLogicalCpus, maxLogicalCpus;
    NvU32  eax = 0;
    NvU32  ebx = 0;
    NvU32  ecx = 0;
    NvU32  edx = 0;

    //
    // First use OS call to get number of logical CPUs.
    //
    numLogicalCpus = osGetCpuCount();

    //
    // Assume the number of physical CPUs is the same as the number of logical CPUs.
    //
    numPhysicalCpus = numLogicalCpus;
    maxLogicalCpus = numLogicalCpus;

    // There is no reliable way to tell if hyper-threading is enabled.  So, if
    // there is more than 1 logical CPUs AND the CPU is hyperthreading capable,
    // then assume that HT is enabled.
    //
    // This should give the right answer for most cases.  Some HT capable dual
    // CPU systems with HT disabled will be detected as single GPU systems with
    // HT enabled.  While less than ideal, this should be OK, since logical CPUs
    // is 2 in both cases.
    //
#if defined(_M_IX86) || defined(NVCPU_X86) || defined(NVCPU_X86_64)
    if (IS_INTEL(pCpuidInfo->Foundry))
    {
        NvBool cpuHasLeafB = NV_FALSE;

        osNv_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
        if (eax >= 0xB)
        {
            osNv_cpuid(0xB, 0, &eax, &ebx, &ecx, &edx);
            if (ebx != 0)
            {
                cpuHasLeafB = NV_TRUE;
            }
        }

        osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx);

        if (edx & INTEL_HT_BIT)
        {
            NvU32 CpuHT;

            if (cpuHasLeafB)
            {
                osNv_cpuid(0xB, 0, &eax, &ebx, &ecx, &edx);
                CpuHT = (ebx & INTEL_LOGICAL_CNT_LEAFB);
                osNv_cpuid(0xB, 1, &eax, &ebx, &ecx, &edx);
                maxLogicalCpus = (ebx & INTEL_LOGICAL_CNT_LEAFB);
                numPhysicalCpus = maxLogicalCpus/CpuHT;
            }
            else
            {
                osNv_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
                if (eax >=4)
                {
                    osNv_cpuid(4, 0, &eax, &ebx, &ecx, &edx);
                    numPhysicalCpus = ((eax & INTEL_CORE_CNT) >> 26) + 1;
                    osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
                    maxLogicalCpus = (ebx & INTEL_LOGICAL_CNT) >> 16;
                    CpuHT = maxLogicalCpus/numPhysicalCpus;
                }
            }

            if (numPhysicalCpus > numLogicalCpus)
                numPhysicalCpus = numLogicalCpus;

            if (numPhysicalCpus < 1)
                numPhysicalCpus = 1;

            pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_HT_CAPABLE;
        }
    }
    else if(IS_AMD(pCpuidInfo->Foundry))
    {
        osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
        if( edx & AMD_HT_BIT )
        {
            maxLogicalCpus = (ebx & AMD_LOGICAL_CNT) >> 16;
        }
    }

    NV_PRINTF(LEVEL_INFO, "RmInitCpuCounts: physical 0x%x logical 0x%x\n",
              numPhysicalCpus, numLogicalCpus);
#endif

    if(maxLogicalCpus < numLogicalCpus)
        maxLogicalCpus = numLogicalCpus;

#if NVCPU_IS_FAMILY_X86
    // bug1974464: Ryzen physical CPU count is getting misreported
    if (IS_AMD(pCpuidInfo->Foundry) && (pCpuidInfo->DisplayedFamily == 0x17))
    {
        numPhysicalCpus = NV_MAX(maxLogicalCpus/2, 1);
    }
#endif

    pSys->cpuInfo.numPhysicalCpus = numPhysicalCpus;
    pSys->cpuInfo.numLogicalCpus = numLogicalCpus;
    pSys->cpuInfo.maxLogicalCpus = maxLogicalCpus;

    return;
}


// getEmbeddedProcessorName
//
// All processors that have extended CPUID info up through 0x80000004 have an embedded name.
//
static NvBool getEmbeddedProcessorName(char *pName, NvU32 size)
{
    NvU32       op, eax, ebx, ecx, edx;
    char       *p       = pName;
    const NvU32 maxSize = 48; // max 48 bytes on x86 CPUs

    NV_ASSERT_OR_RETURN(size >= maxSize, 0);

    pName[size > maxSize ? maxSize : size-1] = 0;  // Make sure it has a zero at the end.

    // Is there is a enough data?  If not bail.
    if (osNv_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx) == 0 || eax < 0x80000004)
        return NV_FALSE;

    // Yes, get 48 bytes of CPU name.
    for (op = 0x80000002; op < 0x80000005; op++, p += 16)
        osNv_cpuid(op, 0, (NvU32 *)&p[0], (NvU32 *)&p[4], (NvU32 *)&p[8], (NvU32 *)&p[12]);

    // Kill leading spaces. (Intel's string is right justified.)
    if (*pName == ' ')
    {
        p = pName;
        while (*p == ' ')
            p++;
        do
            *(pName++) = *(p++);
        while (*p);
    }

    return NV_TRUE;
}


// Decode Prescott style cache descriptors.
//
static NvBool DecodePrescottCache(OBJSYS *pSys)
{
    NvU32   eax, ebx, ecx, edx;

    // Decode the cache desciptors.

    if (osNv_cpuid(0, 0, &eax, &ebx, &ecx, &edx))
    {
        if (eax >= 4 && eax < 0x80000000)     // CPU support new (Prescott) cache descrtiptors?
        {
            // From Prescot New Instructions Software Developers Guide 252490-003

            NvU32 uLevel;
            NvU32 uLineSize;
            NvU32 uCacheSize;
            int i;

            // Loop over the cache descriptors by incrementing sub-function.  This will never get
            // get run on pre-Prescott CPUs since they do not support CPUID 4, but limit number of
            // cache descriptors to 20 just in case, so it does not get in an infinite loop.
            //
            for (i = 0; i < 20; i++)
            {
                osNv_cpuid(4, i, &eax, &ebx, &ecx, &edx);

                if (i == 0)
                {
                    pSys->cpuInfo.coresOnDie = (eax >> 26) + 1;// eax[31:26] Processor cores on the chip
                }

                switch (eax & 0x1f)      // Cache type.
                {
                    case 0:              // No more cache descriptors.
                        i = 100;         // Break out of loop.
                        break;

                    case 1:              // Data cache.
                    case 3:              // Unified cache.
                        uLevel =     (eax >> 5) & 0x7;             // eax[7:5]    Cache level
                        uLineSize =  (ebx & 0xfff) + 1;            // ebx[11:0]   System Coherency Line Size

                        uCacheSize = uLineSize                     // ebx[11:0]   System Coherency Line Size
                                     * (((ebx >> 12) & 0x3FF) + 1) // ebx[21:12]  Physical line partitions
                                     * (((ebx >> 22) & 0x3FF) + 1) // ebx[21:12]  Ways of associativity
                                     * (ecx + 1)                   // ecx[31:0]   Number of sets
                                     / 1024;                       // Put it in KB.

                        pSys->cpuInfo.dataCacheLineSize = uLineSize;

                        if (uLevel == 1)
                            pSys->cpuInfo.l1DataCacheSize = uCacheSize;
                        else if (pSys->cpuInfo.l2DataCacheSize < uCacheSize)
                            pSys->cpuInfo.l2DataCacheSize = uCacheSize;
                        break;

                    default:             // Instruction of unknown cache type.
                        break;           // Do nothing.
                }
            }

            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

#if defined(_M_IX86) || defined(NVCPU_X86)
static void DecodeIntelCacheEntry(OBJSYS *pSys, NvU8 cacheEntry)
{
    // From Intel's AP-485 (11/03).
    //
    // 00h Null
    // 01h Instruction TLB: 4K-byte Pages, 4-way set associative, 32 entries
    // 02h Instruction TLB: 4M-byte Pages, fully associative, 2 entries
    // 03h Data TLB: 4K-byte Pages, 4-way set associative, 64 entries
    // 04h Data TLB: 4M-byte Pages, 4-way set associative, 8 entries
    // 06h 1st-level instruction cache: 8K-bytes, 4-way set associative, 32 byte line size
    // 08h 1st-level instruction cache: 16K-bytes, 4-way set associative, 32 byte line size
    // 0Ah 1st-level data cache: 8K-bytes, 2-way set associative, 32 byte line size
    // 0Ch 1st-level data cache: 16K-bytes, 4-way set associative, 32 byte line size
    // 22h 3rd-level cache: 512K-bytes, 4-way set associative, sectored cache, 64-byte line size
    // 23h 3rd-level cache: 1M-bytes, 8-way set associative, sectored cache, 64-byte line size
    // 25h 3rd-level cache: 2MB, 8-way set associative, sectored cache, 64-byte line size
    // 29h 3rd-level cache: 4MB, 8-way set associative, sectored cache, 64-byte line size
    // 2Ch 1st-level data cache: 32K-bytes, 8-way set associative, 64-byte line size
    // 30h 1st-level instruction cache: 32K-bytes, 8-way set associative, 64-byte line size
    // 39h 2nd-level cache: 128K-bytes, 4-way set associative, sectored cache, 64-byte line size
    // 3Bh 2nd-level cache: 128KB, 2-way set associative, sectored cache, 64-byte line size
    // 3Ch 2nd-level cache: 256K-bytes, 4-way set associative, sectored cache, 64-byte line size
    // 40h No 2nd-level cache or, if processor contains a valid 2nd-level cache, no3rd-level cache
    // 41h 2nd-level cache: 128K-bytes, 4-way set associative, 32 byte line size
    // 42h 2nd-level cache: 256K-bytes, 4-way set associative, 32 byte line size
    // 43h 2nd-level cache: 512K-bytes, 4-way set associative, 32 byte line size
    // 44h 2nd-level cache: 1M-bytes, 4-way set associative, 32 byte line size
    // 45h 2nd-level cache: 2M-bytes, 4-way set associative, 32 byte line size
    // 50h Instruction TLB: 4K, 2M or 4M pages, fully associative, 64 entries
    // 51h Instruction TLB: 4K, 2M or 4M pages, fully associative, 128 entries
    // 52h Instruction TLB: 4K, 2M or 4M pages, fully associative, 256 entries
    // 5Bh Data TLB: 4K or 4M pages, fully associative, 64 entries
    // 5Ch Data TLB: 4K or 4M pages, fully associative, 128 entries
    // 5Dh Data TLB: 4K or 4M pages, fully associative, 256 entries
    // 66h 1st-level data cache: 8K-bytes, 4-way set associative, sectored cache, 64-byte line size
    // 67h 1st-level data cache: 16K-bytes, 4-way set associative, sectored cache, 64-byte line size
    // 68h 1st-level data cache: 32K-bytes, 4 way set associative, sectored cache, 64-byte line size
    // 70h Trace cache: 12K-uops, 8-way set associative
    // 71h Trace cache: 16K-uops, 8-way set associative
    // 72h Trace cache: 32K-uops, 8-way set associative
    // 79h 2nd-level cache: 128K-bytes, 8-way set associative, sectored cache, 64-byte line size
    // 7Ah 2nd-level cache: 256K-bytes, 8-way set associative, sectored cache, 64-byte line size
    // 7Bh 2nd-level cache: 512K-bytes, 8-way set associative, sectored cache, 64-byte line size
    // 7Ch 2nd-level cache: 1M-bytes, 8-way set associative, sectored cache, 64-byte line size
    // 82h 2nd-level cache: 256K-bytes, 8-way set associative, 32 byte line size
    // 83h 2nd-level cache: 512K-bytes, 8-way set associative, 32 byte line size
    // 84h 2nd-level cache: 1M-bytes, 8-way set associative, 32 byte line size
    // 85h 2nd-level cache: 2M-bytes, 8-way set associative, 32 byte line size
    // 86h 2nd-level cache: 512K-bytes, 4-way set associative, 64 byte line size
    // 87h 2nd-level cache: 1M-bytes, 8-way set associative, 64 byte line size
    // B0h Instruction TLB: 4K-byte Pages, 4-way set associative, 128 entries
    // B3h Data TLB: 4K-byte Pages, 4-way set associative, 128 entries
    //
    // From Intel via Michael Diamond (under NDA):
    // Fixes bug 75982 - Reporting incorrect cache info on Banias mobile platform.
    //
    // 7D 2M; 8 way; 64 byte line size; unified on-die
    // 78 1M; 8 way; 64 byte line size, unified on-die
    //
    // Note: Newer GPUs have added an additional cache level.  What used to be L2 is
    // now L3.  Set the L2 cache to the largest L2 or L3 descriptor found.

    switch (cacheEntry)
    {
        case 0x0A: // 1st-level data cache: 8K-bytes, 2-way set associative, 32 byte line size
            pSys->cpuInfo.l1DataCacheSize = 8;
            pSys->cpuInfo.dataCacheLineSize = 32;
            break;

        case 0x0C: // 1st-level data cache: 16K-bytes, 4-way set associative, 32 byte line size
            pSys->cpuInfo.l1DataCacheSize = 16;
            pSys->cpuInfo.dataCacheLineSize = 32;
            break;

        case 0x66: // 1st-level data cache: 8K-bytes, 4-way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.l1DataCacheSize = 8;
            pSys->cpuInfo.dataCacheLineSize = 64;
            break;

        case 0x67: // 1st-level data cache: 16K-bytes, 4-way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.l1DataCacheSize = 16;
            pSys->cpuInfo.dataCacheLineSize = 64;
            break;

        case 0x2C: // 1st-level data cache: 32K-bytes, 8-way set associative, 64-byte line size
        case 0x68: // 1st-level data cache: 32K-bytes, 4 way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.l1DataCacheSize = 32;
            pSys->cpuInfo.dataCacheLineSize = 64;
            break;

        case 0x41: // 2nd-level cache: 128K-bytes, 4-way set associative, 32 byte line size
            pSys->cpuInfo.dataCacheLineSize = 32;
            if (pSys->cpuInfo.l2DataCacheSize < 128)
                pSys->cpuInfo.l2DataCacheSize = 128;
            break;

        case 0x39: // 2nd-level cache: 128K-bytes, 4-way set associative, sectored cache, 64-byte line size
        case 0x3B: // 2nd-level cache: 128KB, 2-way set associative, sectored cache, 64-byte line size
        case 0x79: // 2nd-level cache: 128K-bytes, 8-way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 128)
                pSys->cpuInfo.l2DataCacheSize = 128;
            break;

        case 0x42: // 2nd-level cache: 256K-bytes, 4-way set associative, 32 byte line size
        case 0x82: // 2nd-level cache: 256K-bytes, 8-way set associative, 32 byte line size
            pSys->cpuInfo.dataCacheLineSize = 32;
            if (pSys->cpuInfo.l2DataCacheSize < 256)
                pSys->cpuInfo.l2DataCacheSize = 256;
            break;

        case 0x3C: // 2nd-level cache: 256K-bytes, 4-way set associative, sectored cache, 64-byte line size
        case 0x7A: // 2nd-level cache: 256K-bytes, 8-way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 256)
                pSys->cpuInfo.l2DataCacheSize = 256;
            break;

        case 0x43: // 2nd-level cache: 512K-bytes, 4-way set associative, 32 byte line size
        case 0x83: // 2nd-level cache: 512K-bytes, 8-way set associative, 32 byte line size
            pSys->cpuInfo.dataCacheLineSize = 32;
            if (pSys->cpuInfo.l2DataCacheSize < 512)
                pSys->cpuInfo.l2DataCacheSize = 512;
            break;

        case 0x22: // 3rd-level cache: 512K-bytes, 4-way set associative, sectored cache, 64-byte line size
        case 0x7B: // 2nd-level cache: 512K-bytes, 8-way set associative, sectored cache, 64-byte line size
        case 0x86: // 2nd-level cache: 512K-bytes, 4-way set associative, 64 byte line size
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 512)
                pSys->cpuInfo.l2DataCacheSize = 512;
            break;

        case 0x44: // 2nd-level cache: 1M-bytes, 4-way set associative, 32 byte line size
        case 0x84: // 2nd-level cache: 1M-bytes, 8-way set associative, 32 byte line size
            pSys->cpuInfo.dataCacheLineSize = 32;
            if (pSys->cpuInfo.l2DataCacheSize < 1024)
                pSys->cpuInfo.l2DataCacheSize = 1024;
            break;

        case 0x23: // 3rd-level cache: 1M-bytes, 8-way set associative, sectored cache, 64-byte line size
        case 0x78: // 1M; 8 way; 64 byte line size, unified on-die
        case 0x7C: // 2nd-level cache: 1M-bytes, 8-way set associative, sectored cache, 64-byte line size
        case 0x87: // 2nd-level cache: 1M-bytes, 8-way set associative, 64 byte line size
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 1024)
                pSys->cpuInfo.l2DataCacheSize = 1024;
            break;

        case 0x45: // 2nd-level cache: 2M-bytes, 4-way set associative, 32 byte line size
        case 0x85: // 2nd-level cache: 2M-bytes, 8-way set associative, 32 byte line size
            pSys->cpuInfo.dataCacheLineSize = 32;
            if (pSys->cpuInfo.l2DataCacheSize < 2048)
                pSys->cpuInfo.l2DataCacheSize = 2048;
            break;

        case 0x25: // 3rd-level cache: 2MB, 8-way set associative, sectored cache, 64-byte line size
        case 0x7D: // 2M; 8 way; 64 byte line size; unified on-die
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 2048)
                pSys->cpuInfo.l2DataCacheSize = 2048;
            break;

        case 0x29: // 3rd-level cache: 4MB, 8-way set associative, sectored cache, 64-byte line size
            pSys->cpuInfo.dataCacheLineSize = 64;
            if (pSys->cpuInfo.l2DataCacheSize < 4096)
                pSys->cpuInfo.l2DataCacheSize = 4096;
            break;
    }
}

static void DecodeIntelCacheRegister(OBJSYS *pSys, NvU32 cacheRegister /* punny, huh? */)
{
    if ((cacheRegister & NVBIT(31)) == 0)  // If bit 31 is set, it is reserved.
    {
        DecodeIntelCacheEntry(pSys, (NvU8)(cacheRegister >> 24));
        DecodeIntelCacheEntry(pSys, (NvU8)(cacheRegister >> 16));
        DecodeIntelCacheEntry(pSys, (NvU8)(cacheRegister >> 8));
        DecodeIntelCacheEntry(pSys, (NvU8)cacheRegister);
    }
}
#endif

static void cpuidInfoIntel(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    NvU32   eax, ebx, ecx, edx;

    if (pCpuidInfo->Family == 5)
    {
        if (pCpuidInfo->Model == 4)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P55;
        else
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P5;
    }
    else if (pCpuidInfo->Family == 6)
    {
        switch (pCpuidInfo->DisplayedModel)
        {
            case 1:                                    // Pentium Pro
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P6;
                break;

            case 3:                                    // Pentium II
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P2;
                break;

            case 5:                                    // Pentium II, Pentium II Xeon, or Celeron
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P2XC;
                break;

            case 6:                                    // Pentium II Celeron-A
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_CELA;
                break;

            case 7:                                    // Pentium III or Pentium III Xeon (Katmai)
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P3;
                break;

            case 15:                                   // Conroe, Core2 Duo
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2;
                break;

            case 22:                                   // Celeron model 16h (65nm)
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_CELN_M16H;
                break;

            case 23:                                   // Intel Core2 Extreme/Intel Xeon model 17h (45nm)
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_CORE2_EXTRM;
                break;

            case 28:
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_ATOM;
                break;

            case 143:                                  // Intel Xeon Sapphire Rapids(SPR) model 143
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_XEON_SPR;
                break;

            case 8:                                    // Pentium III, Pentium III Xeon, or Celeron (Coppermine, 0.18 micron)
            case 10:                                   // Pentium III Xeon (Tualatin, 0.13 micron)
            case 11:                                   // Pentium III, or Celeron (Tualatin, 0.13 micron)
            default:                                   // If it is a new family 6, it is a Pentium III of some type.
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P3_INTL2;
                break;
        }
        // Flag processors that may be affected by bug 124888.  At this time,
        // we believe these are Pentium III and Pentium M processors.  The
        // model numbers for these processors in Family 6 are:
        //   7 - Pentium III or Pentium III Xeon
        //   8 - Pentium III, Pentium III Xeon, or Celeron
        //   9 - Pentium M
        //  10 - Pentium III Xeon
        //  11 - Pentium III
        //  12 - ???
        //  13 - Pentium M ("Dothan")
        //  14 - ???
        //  15 - Core 2 (bug 272047)
        if (pCpuidInfo->Model >= 7)
        {
            pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WAR_124888;
        }
    }
    else if (pCpuidInfo->Family == 0x0F)
    {
        // Model 0 & 1 == Pentium 4 or Pentium 4 Xeon (Willamette, 423 or 478-pin packages, 0.18 micron)
        // Model 2 == Pentium 4 or Pentium 4 Xeon (Northwood, 478-pin package for brookdale, 0.13 micron)
        //
        // Be careful if you change this.  Both D3D and OpenGL are enabling
        // performance options based on NV0000_CTRL_SYSTEM_CPU_TYPE_P4.
        //
        pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_P4;

        // The first P4s (pre-Northwood ones) have a performance problem
        // when mixing write combined and cached writes. This is fixed
        // with model revision 2.
        if ((pCpuidInfo->Model == 0) || (pCpuidInfo->Model == 1))
        {
            pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_NEEDS_WC_WORKAROUND;
        }
    }

    if (pCpuidInfo->Family == 0xF || (pCpuidInfo->Family == 6 && pCpuidInfo->Model >= 7))
    {
        if (osNv_cpuid(0x17, 0, &eax, &ebx, &ecx, &edx))
            pSys->cpuInfo.platformID = (edx >> 18) & 7;        // edx[20:18]   PlatformID (package type)
    }

    // Decode the cache desciptors.
    if (!DecodePrescottCache(pSys))
    {
#if defined(_M_IX86) || defined(NVCPU_X86)

        // Prescott style cache descriptors are not supported.  Fall back to older style.
        //
        if (osNv_cpuid(0, 0, &eax, &ebx, &ecx, &edx))
        {
            if (eax >= 2)                    // CPU support old cache descrtiptors?
            {
                osNv_cpuid(2, 0, &eax, &ebx, &ecx, &edx);

                if ((eax & 0xff) == 1)  // AL contains number of times CPU must be called.  This will be 1 forever.
                {
                    DecodeIntelCacheRegister(pSys, eax & 0xffffff00);
                    DecodeIntelCacheRegister(pSys, ebx);
                    DecodeIntelCacheRegister(pSys, ecx);
                    DecodeIntelCacheRegister(pSys, edx);
                }
            }
        }
#endif
    }
}

static void cpuidInfoAMD(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    NvU32   eax = 0;
    NvU32   ebx = 0;
    NvU32   ecx = 0;
    NvU32   edx = 0;

    NvU32 largestExtendedFunctionNumberSupported = 0x80000000;

    if (pCpuidInfo->Family == 5)                // K5, K6, K6-2 with 3DNow, K6-3
    {
        if (pCpuidInfo->Model < 6)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K5;
        else if (pCpuidInfo->Model < 8)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K6;
        else if (pCpuidInfo->Model == 8)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K62;
        else if (pCpuidInfo->Model == 9)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K63;
    }
    else if (pCpuidInfo->Family == 6)           // K7
    {
        // Family 6 is a mixture of Athlon and Duron processors.  Just set the
        // processor type to Athlon.  The processor name will show the branding.
        pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K7;
    }
    else if (pCpuidInfo->Family == 15)          // K8
    {
        // If family is 15, we need to use AMD's extended family/model information.
        osNv_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
        pCpuidInfo->Family = (NvU16)(((eax >> 8) & 0x0F) + ((eax >> 16) & 0xFF0));  // 27:20 concat 11:8
        pCpuidInfo->Model  = (NvU8) (((eax >> 4) & 0x0F) + ((eax >> 12) & 0xF0));   // 19:16 concat 7:4

        // Differentiate K8, K10, K11, RYZEN, etc
        switch( pCpuidInfo->Family & 0xFF0)
        {
            case 0x000:
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K8;
                break;
            case 0x010:
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K10;
                break;
            case 0x020:
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K11;
                break;
            case 0x080:
                // Zen, Zen+, Zen 2
            case 0x0A0:
                // Zen 3, Zen 4
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_RYZEN;
                break;
            default:
                NV_PRINTF(LEVEL_ERROR,
                          "Unrecognized AMD processor in cpuidInfoAMD\n");
                pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_K8;
                break;
        }
    }

    if (pCpuidInfo->ExtendedFeatures & CPU_EXT_3DNOW)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW;      // 3DNow

    if (pCpuidInfo->ExtendedFeatures & CPU_EXT_AMD_3DNOW_EXT)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW_EXT;  // 3DNow, with Extensions (AMD specific)

    if (pCpuidInfo->ExtendedFeatures & CPU_EXT_AMD_MMX_EXT)
    {
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_MMX_EXT;    // MMX, with Extensions (AMD specific)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_SFENCE;
    }

    // Get the cache info.
    if (osNv_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx))
    {
        largestExtendedFunctionNumberSupported = eax;

        if (largestExtendedFunctionNumberSupported >= 0x80000006)
        {
            // L1 cache
            if (osNv_cpuid(0x80000005, 0, &eax, &ebx, &ecx, &edx))
            {
                pSys->cpuInfo.dataCacheLineSize = ecx & 0xff;
                pSys->cpuInfo.l1DataCacheSize = ecx >> 24;
            }

            // L2 cache
            if (osNv_cpuid(0x80000006, 0, &eax, &ebx, &ecx, &edx))
                pSys->cpuInfo.l2DataCacheSize = ecx >> 16;
        }

        // Get the SEV capability info
        if ((largestExtendedFunctionNumberSupported >= 0x8000001f) &&
            osNv_cpuid(0x8000001f, 0, &eax, &ebx, &ecx, &edx))
        {
            //
            // EAX[1] stores capability info
            // ECX[31:0] stores # of encrypted guests supported simultaneously
            //
            if (eax & 0x2)
            {
                pSys->cpuInfo.bSEVCapable = NV_TRUE;
                pSys->cpuInfo.maxEncryptedGuests = ecx;
            }
        }
    }
}


#if defined(_M_IX86) || defined(NVCPU_X86)

static void cpuidInfoWinChip(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    if (pCpuidInfo->Family == 5)                // Winchip C6, Winchip2 w/ 3DNow
    {
        if (pCpuidInfo->Model == 4)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_C6;
        if (pCpuidInfo->Model == 8)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_C62;
    }

    if (pCpuidInfo->ExtendedFeatures & CPU_EXT_3DNOW)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW;
}

static void cpuidInfoCyrix(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    if (pCpuidInfo->Family == 4)                // MediaGX
        pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_GX;
    if (pCpuidInfo->Family == 5)                // Cyrix 6x86 or MediaGX w/ MMX
    {
        if (pCpuidInfo->Model == 2)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_M1;
        if (pCpuidInfo->Model == 4)
            pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_MGX;
    }
    if (pCpuidInfo->Family == 6)                // Cyrix MII
        pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_M2;

    if (pCpuidInfo->ExtendedFeatures & CPU_EXT_3DNOW)
        pSys->cpuInfo.caps |= NV0000_CTRL_SYSTEM_CPU_CAP_3DNOW;
}

static void cpuidInfoTransmeta(OBJSYS *pSys, PCPUIDINFO pCpuidInfo)
{
    NvU32 eax, ebx, ecx, edx;

    //
    // Transmeta allows the OEM to program the foundry, family, model, and stepping.  Arrrrgh...
    // If this turns out to be a problem, we will need to use one of the extended CPUID calls to
    // get the real info.
    //

    // Docs were not real clear on which family/model.  Just assume it's a Crusoe
    pSys->cpuInfo.type = NV0000_CTRL_SYSTEM_CPU_TYPE_TM_CRUSOE;

    //
    // Get the cache info.  From preliminary TM8000 programming and config guide, 2/19/03
    // This appears to match AMD's cache CPUID definitions.
    //
    if (osNv_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx) && eax >= 0x80000006)
    {
        // L1 Cache
        if (osNv_cpuid(0x80000005, 0, &eax, &ebx, &ecx, &edx))
        {
            pSys->cpuInfo.dataCacheLineSize = ecx & 0xff;
            pSys->cpuInfo.l1DataCacheSize = ecx >> 24;
        }

        // L2 Cache
        if (osNv_cpuid(0x80000006, 0, &eax, &ebx, &ecx, &edx))
            pSys->cpuInfo.l2DataCacheSize = ecx >> 16;
    }
}

#endif // defined(_M_IX86) || defined(NVCPU_X86)

#endif // defined(_M_IX86) || defined(NVCPU_X86) || defined(AMD64) || defined(NVCPU_X86_64)

#endif // NVCPU_IS_X86 || NVCPU_IS_X86_64
