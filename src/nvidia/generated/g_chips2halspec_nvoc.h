
#ifndef _G_CHIPS2HALSPEC_NVOC_H_
#define _G_CHIPS2HALSPEC_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include "g_chips2halspec_nvoc.h"

#ifndef _CHIPS_2_HALSPEC_H_
#define _CHIPS_2_HALSPEC_H_

#include "nvtypes.h"
#include "rmconfig.h"

// Several WARs that only visible by NVOC compiler

#define GPUHAL_ARCH(x)      NV_PMC_BOOT_42_ARCHITECTURE_##x
#define GPUHAL_IMPL(x)      NV_PMC_BOOT_42_IMPLEMENTATION_##x

// Create alias 'group' to provide a concise syntax
#define group variant_group

// Use in hal block to indicate that the function isn't wried to any enabled chips 
#define __disabled__ false

struct ChipHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct ChipHal ChipHal;
void __nvoc_init_halspec_ChipHal(ChipHal*, NvU32, NvU32, NvU32);

/*
 * Tegra Chip Type Halspec 
 *
 * For Legacy iGPUs, we have two type Tegra chips in Chips.pm
 *   TEGRA_DGPU : The iGPU Core inside the Tegra Soc Chip with PCIE interface.
 *                The behavior is more like a dGPU.  Such chip is generally
 *                added to dGPU (CLASSIC_GPUS) chip family.  E.g. GA10B
 *                This is generally the test chip used in MODS Arch validation
 *                that shares the test infrastructure with dGPU.
 *
 *   TEGRA      : The SoC chip.  The chips do not share dGPU HAL on PCIE related
 *                implementation.
 * 
 * The Tegra chip after Ampere arch is using PCIE interface which connects
 * iGPU to SoC for BAR and control accesses (interrupt).
 * The code between TEGRA_CHIP_TYPE_PCIE and TEGRA_CHIP_TYPE_SOC
 * shares same dGPU ARCH specific HAL mostly except manual differences due to
 * latency of manual updates between nvgpu (Standlone iGPU/Full Chip Verification)
 * and nvmobile (SOC) trees.
 * */
typedef enum _TEGRA_CHIP_TYPE {
    // Default TEGRA_CHIP_TYPE is TEGRA_PCIE
    TEGRA_CHIP_TYPE_DEFAULT             = 0,
    TEGRA_CHIP_TYPE_SOC                 = 1,
} TEGRA_CHIP_TYPE;

struct TegraChipHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct TegraChipHal TegraChipHal;
void __nvoc_init_halspec_TegraChipHal(TegraChipHal*, TEGRA_CHIP_TYPE);

/*
 * RM Runtime Variant Halspec 
 *
 * One group of Hal Variants that presents two perspectives: 
 *
 *  Operating Environment Perspective: VF / PF / UCODE
 *   VF | PF | UCODE = true
 *   VF & PF & UCODE = false
 * 
 *   VF    : RM is running in VGPU Guest environment.  Equals to IS_VIRTUAL(pGpu)
 *   PF    : RM is running in Host/Baremetal in standard PCIE environment
 *   UCODE : RM is running on microcontroller
 * 
 *  Functionality-Based Perspective: KERNEL_ONLY / PHYSICAL_ONLY / MONOLITHIC
 *   KERNEL_ONLY | PHYSICAL_ONLY | MONOLITHIC = true
 *   KERNEL_ONLY & PHYSICAL_ONLY & MONOLITHIC = false
 * 
 *   KERNEL_ONLY   : RM does not own HW.  The physical part is offloaded to Ucode. 
 *   PHYSICAL_ONLY : RM owns HW but does not expose services to RM Clients
 *   MONOLITHIC    : RM owns both the interface to the client and the underlying HW.
 * 
 *  Note: GSP Client "IS_GSP_CLIENT(pGpu) maps to "PF_KERNEL_ONLY"
 *        DCE Client maps to "PF_KERNEL_ONLY & T234D"
 * 
 *
 *                      HAL Variants
 *  +--------+       +----------------+
 *  |   VF   | <-----|       VF       |--+
 *  +--------+       +----------------+  |    +---------------+
 *                                       |--> |  KERNEL_ONLY  |
 *                   +----------------+  |    +---------------+
 *                +--| PF_KERNEL_ONLY |--+
 *  +--------+    |  +----------------+
 *  |   PF   | <--|
 *  +--------+    |  +----------------+       +---------------+
 *                +--| PF_MONOLITHIC  |-----> |  MONOLITHIC   |
 *                   +----------------+       +---------------+
 *
 *  +--------+       +----------------+       +---------------+
 *  | UCODE  | <-----|     UCODE      |-----> | PHYSICAL_ONLY |
 *  +--------+       +----------------+       +---------------+
 * 
 * */
typedef enum _RM_RUNTIME_VARIANT {
    RM_RUNTIME_VARIANT_VF               = 1,
    RM_RUNTIME_VARIANT_PF_KERNEL_ONLY   = 2,
    RM_RUNTIME_VARIANT_PF_MONOLITHIC    = 3,
    RM_RUNTIME_VARIANT_UCODE            = 4,
} RM_RUNTIME_VARIANT;

struct RmVariantHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct RmVariantHal RmVariantHal;
void __nvoc_init_halspec_RmVariantHal(RmVariantHal*, RM_RUNTIME_VARIANT);

/* DISP IP versions */
struct DispIpHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct DispIpHal DispIpHal;
void __nvoc_init_halspec_DispIpHal(DispIpHal*, NvU32);

/* The 'delete' rules for DispIpHal and ChipHal */
// delete DISPv0400 & ~(TU102 | TU104 | TU106 | TU116 | TU117);
// delete ~DISPv0400 & (TU102 | TU104 | TU106 | TU116 | TU117);
// delete DISPv0401 & ~(GA102 | GA103 | GA104 | GA106 | GA107);
// delete ~DISPv0401 & (GA102 | GA103 | GA104 | GA106 | GA107);
// delete DISPv0404 & ~(AD102 | AD103 | AD104 | AD106 | AD107);
// delete ~DISPv0404 & (AD102 | AD103 | AD104 | AD106 | AD107);
// delete DISPv0501 & ~GB10B;
// delete ~DISPv0501 & GB10B;
// delete DISPv0502 & ~(GB202 | GB203 | GB205 | GB206 | GB207);
// delete ~DISPv0502 & (GB202 | GB203 | GB205 | GB206 | GB207);
// delete DISPv0503 & ~GB20B;
// delete ~DISPv0503 & GB20B;


/* DPU IP versions */
struct DpuIpHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct DpuIpHal DpuIpHal;
void __nvoc_init_halspec_DpuIpHal(DpuIpHal*, NvU32);

/* The 'delete' rules for DpuIpHal and ChipHal */


#undef group
#endif /* _CHIPS_2_HALSPEC_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CHIPS2HALSPEC_NVOC_H_
