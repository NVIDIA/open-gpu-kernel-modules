#ifndef _G_CHIPS2HALSPEC_NVOC_H_
#define _G_CHIPS2HALSPEC_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "g_chips2halspec_nvoc.h"

#ifndef _CHIPS_2_HALSPEC_H_
#define _CHIPS_2_HALSPEC_H_

#include "nvtypes.h"
#include "rmconfig.h"

// Several WARs that only visible by NVOC compiler

#define GPUHAL_ARCH(x)      NV_PMC_BOOT_0_ARCHITECTURE_##x
#define GPUHAL_IMPL(x)      NV_PMC_BOOT_0_IMPLEMENTATION_##x

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
