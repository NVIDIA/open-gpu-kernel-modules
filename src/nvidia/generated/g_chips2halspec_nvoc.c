#define NVOC_CHIPS2HALSPEC_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_chips2halspec_nvoc.h"

void __nvoc_init_halspec_ChipHal(ChipHal *pChipHal, NvU32 arch, NvU32 impl, NvU32 hidrev)
{
    // TU102
    if(arch == 0x16 && impl == 0x2)
    {
        pChipHal->__nvoc_HalVarIdx = 37;
    }
    // TU104
    else if(arch == 0x16 && impl == 0x4)
    {
        pChipHal->__nvoc_HalVarIdx = 38;
    }
    // TU106
    else if(arch == 0x16 && impl == 0x6)
    {
        pChipHal->__nvoc_HalVarIdx = 39;
    }
    // TU116
    else if(arch == 0x16 && impl == 0x8)
    {
        pChipHal->__nvoc_HalVarIdx = 40;
    }
    // TU117
    else if(arch == 0x16 && impl == 0x7)
    {
        pChipHal->__nvoc_HalVarIdx = 41;
    }
    // GA100
    else if(arch == 0x17 && impl == 0x0)
    {
        pChipHal->__nvoc_HalVarIdx = 42;
    }
    // GA102
    else if(arch == 0x17 && impl == 0x2)
    {
        pChipHal->__nvoc_HalVarIdx = 43;
    }
    // GA103
    else if(arch == 0x17 && impl == 0x3)
    {
        pChipHal->__nvoc_HalVarIdx = 44;
    }
    // GA104
    else if(arch == 0x17 && impl == 0x4)
    {
        pChipHal->__nvoc_HalVarIdx = 45;
    }
    // GA106
    else if(arch == 0x17 && impl == 0x6)
    {
        pChipHal->__nvoc_HalVarIdx = 46;
    }
    // GA107
    else if(arch == 0x17 && impl == 0x7)
    {
        pChipHal->__nvoc_HalVarIdx = 47;
    }
    // AD102
    else if(arch == 0x19 && impl == 0x2)
    {
        pChipHal->__nvoc_HalVarIdx = 52;
    }
    // AD103
    else if(arch == 0x19 && impl == 0x3)
    {
        pChipHal->__nvoc_HalVarIdx = 53;
    }
    // AD104
    else if(arch == 0x19 && impl == 0x4)
    {
        pChipHal->__nvoc_HalVarIdx = 54;
    }
    // AD106
    else if(arch == 0x19 && impl == 0x6)
    {
        pChipHal->__nvoc_HalVarIdx = 55;
    }
    // AD107
    else if(arch == 0x19 && impl == 0x7)
    {
        pChipHal->__nvoc_HalVarIdx = 56;
    }
    // GH100
    else if(arch == 0x18 && impl == 0x0)
    {
        pChipHal->__nvoc_HalVarIdx = 60;
    }
    // GB100
    else if(arch == 0x1A && impl == 0x0)
    {
        pChipHal->__nvoc_HalVarIdx = 61;
    }
    // GB102
    else if(arch == 0x1A && impl == 0x2)
    {
        pChipHal->__nvoc_HalVarIdx = 62;
    }
    // GB10B
    else if(arch == 0x1A && impl == 0xB)
    {
        pChipHal->__nvoc_HalVarIdx = 63;
    }
    // GB110
    else if(arch == 0x1A && impl == 0x3)
    {
        pChipHal->__nvoc_HalVarIdx = 65;
    }
    // GB112
    else if(arch == 0x1A && impl == 0x4)
    {
        pChipHal->__nvoc_HalVarIdx = 66;
    }
    // GB202
    else if(arch == 0x1B && impl == 0x2)
    {
        pChipHal->__nvoc_HalVarIdx = 69;
    }
    // GB203
    else if(arch == 0x1B && impl == 0x3)
    {
        pChipHal->__nvoc_HalVarIdx = 70;
    }
    // GB205
    else if(arch == 0x1B && impl == 0x5)
    {
        pChipHal->__nvoc_HalVarIdx = 71;
    }
    // GB206
    else if(arch == 0x1B && impl == 0x6)
    {
        pChipHal->__nvoc_HalVarIdx = 72;
    }
    // GB207
    else if(arch == 0x1B && impl == 0x7)
    {
        pChipHal->__nvoc_HalVarIdx = 73;
    }
    // GB20B
    else if(arch == 0x1B && impl == 0xB)
    {
        pChipHal->__nvoc_HalVarIdx = 74;
    }
}

void __nvoc_init_halspec_TegraChipHal(TegraChipHal *pTegraChipHal, TEGRA_CHIP_TYPE tegraType)
{
    // TEGRA_PCIE
    if(tegraType == 0x0)
    {
        pTegraChipHal->__nvoc_HalVarIdx = 0;
    }
    // TEGRA_SOC
    else if(tegraType == 0x1)
    {
        pTegraChipHal->__nvoc_HalVarIdx = 1;
    }
}

void __nvoc_init_halspec_RmVariantHal(RmVariantHal *pRmVariantHal, RM_RUNTIME_VARIANT rmVariant)
{
    // VF
    if(rmVariant == 0x1)
    {
        pRmVariantHal->__nvoc_HalVarIdx = 0;
    }
    // PF_KERNEL_ONLY
    else if(rmVariant == 0x2)
    {
        pRmVariantHal->__nvoc_HalVarIdx = 1;
    }
}

void __nvoc_init_halspec_DispIpHal(DispIpHal *pDispIpHal, NvU32 ipver)
{
    // DISPv0400
    if(ipver == 0x4000000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 10;
    }
    // DISPv0401
    else if(ipver == 0x4010000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 11;
    }
    // DISPv0404
    else if(ipver == 0x4040000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 14;
    }
    // DISPv0501
    else if(ipver == 0x5010000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 15;
    }
    // DISPv0502
    else if(ipver == 0x5020000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 16;
    }
    // DISPv0503
    else if(ipver == 0x5030000)
    {
        pDispIpHal->__nvoc_HalVarIdx = 17;
    }
    // DISPv0000
    else if(ipver == 0x0)
    {
        pDispIpHal->__nvoc_HalVarIdx = 19;
    }
}

void __nvoc_init_halspec_DpuIpHal(DpuIpHal *pDpuIpHal, NvU32 ipver)
{
    // DPUv0000
    if(ipver == 0x0)
    {
        pDpuIpHal->__nvoc_HalVarIdx = 5;
    }
}

