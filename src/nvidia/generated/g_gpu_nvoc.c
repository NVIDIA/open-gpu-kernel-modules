#define NVOC_GPU_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x7ef3cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

// Forward declarations for OBJGPU
void __nvoc_init__Object(Object*);
void __nvoc_init__RmHalspecOwner(RmHalspecOwner*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver);
void __nvoc_init__OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init__OBJGPU(OBJGPU*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver);
void __nvoc_init_funcTable_OBJGPU(OBJGPU*);
NV_STATUS __nvoc_ctor_OBJGPU(OBJGPU*, NvU32 arg_gpuInstance, NvU32 arg_gpuId, NvUuid *arg_pUuid);
void __nvoc_init_dataField_OBJGPU(OBJGPU*);
void __nvoc_dtor_OBJGPU(OBJGPU*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGPU;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGPU;

// Down-thunk(s) to bridge OBJGPU methods from ancestors (if any)

// Up-thunk(s) to bridge OBJGPU methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJGPU),
        /*classId=*/            classId(OBJGPU),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJGPU",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPU,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJGPU,
    /*pExportInfo=*/        &__nvoc_export_info__OBJGPU
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJGPU __nvoc_metadata__OBJGPU = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGPU,    // (gpu) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPU,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGPU, __nvoc_base_Object),
    .metadata__RmHalspecOwner.rtti.pClassDef = &__nvoc_class_def_RmHalspecOwner,    // (rmhalspecowner) super
    .metadata__RmHalspecOwner.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmHalspecOwner.rtti.offset    = NV_OFFSETOF(OBJGPU, __nvoc_base_RmHalspecOwner),
    .metadata__OBJTRACEABLE.rtti.pClassDef = &__nvoc_class_def_OBJTRACEABLE,    // (traceable) super
    .metadata__OBJTRACEABLE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJTRACEABLE.rtti.offset    = NV_OFFSETOF(OBJGPU, __nvoc_base_OBJTRACEABLE),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGPU = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__OBJGPU.rtti,    // [0]: (gpu) this
        &__nvoc_metadata__OBJGPU.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__OBJGPU.metadata__RmHalspecOwner.rtti,    // [2]: (rmhalspecowner) super
        &__nvoc_metadata__OBJGPU.metadata__OBJTRACEABLE.rtti,    // [3]: (traceable) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGPU = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RmHalspecOwner(RmHalspecOwner*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJGPU(OBJGPU *pThis) {
    __nvoc_gpuDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJGPU(OBJGPU *pThis) {
    ChipHal *chipHal = &staticCast(pThis, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pThis, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    pThis->setProperty(pThis, PDB_PROP_GPU_IS_CONNECTED, NV_TRUE);

    // NVOC Property Hal field -- PDB_PROP_GPU_KEEP_WPR_ACROSS_GC6_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_KEEP_WPR_ACROSS_GC6_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_KEEP_WPR_ACROSS_GC6_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_TEGRA_SOC_IGPU
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TEGRA_SOC_IGPU, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_ATS_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ATS_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ATS_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_TRIGGER_PCIE_FLR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TRIGGER_PCIE_FLR, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TRIGGER_PCIE_FLR, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CLKS_IN_TEGRA_SOC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CLKS_IN_TEGRA_SOC, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CLKS_IN_TEGRA_SOC, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_ZERO_FB
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ZERO_FB, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ZERO_FB, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000066UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_MIRROR_HOST_CI_ON_GUEST
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_MIRROR_HOST_CI_ON_GUEST, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_MIRROR_HOST_CI_ON_GUEST, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_SUPPORTS_SPLIT_CE_RANGES
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTS_SPLIT_CE_RANGES, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTS_SPLIT_CE_RANGES, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_GFX_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_GFX_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_GFX_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_IS_COT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_COT_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_COT_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_FW_WPR_OFFSET_SET_BY_ACR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_FW_WPR_OFFSET_SET_BY_ACR, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_FW_WPR_OFFSET_SET_BY_ACR, NV_FALSE);
    }

    // Hal field -- bIsFlexibleFlaSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bIsFlexibleFlaSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bIsFlexibleFlaSupported = NV_FALSE;
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0f800UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f00000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK, NV_TRUE);
    }
    pThis->setProperty(pThis, PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE, NV_TRUE);

    // NVOC Property Hal field -- PDB_PROP_GPU_CC_FEATURE_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CC_FEATURE_CAPABLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CC_FEATURE_CAPABLE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_APM_FEATURE_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_APM_FEATURE_CAPABLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_APM_FEATURE_CAPABLE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_IS_SOC_SDM
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_SOC_SDM, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_SOC_SDM, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS, NV_TRUE);
    }
    pThis->setProperty(pThis, PDB_PROP_GPU_FASTPATH_SEQ_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_GPU_RECOVERY_DRAIN_P2P_REQUIRED, NV_FALSE);

    // NVOC Property Hal field -- PDB_PROP_GPU_REUSE_INIT_CONTING_MEM
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_REUSE_INIT_CONTING_MEM, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_REUSE_INIT_CONTING_MEM, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_RUSD_POLLING_SUPPORT_MONOLITHIC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x71f0f800UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RUSD_POLLING_SUPPORT_MONOLITHIC, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RUSD_POLLING_SUPPORT_MONOLITHIC, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_GPU_RECOVERY_REBOOT_REQUIRED, NV_FALSE);

    pThis->deviceInstance = 32;

    // Hal field -- isVirtual
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->isVirtual = NV_TRUE;
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isVirtual = NV_FALSE;
    }

    // Hal field -- isGspClient
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isGspClient = NV_TRUE;
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->isGspClient = NV_FALSE;
    }

    pThis->bIsDebugModeEnabled = NV_FALSE;

    pThis->numOfMclkLockRequests = 0U;

    pThis->bUseRegisterAccessMap = !(0);

    pThis->boardInfo = ((void *)0);

    // Hal field -- gpuGroupCount
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->gpuGroupCount = 2;
    }
    // default
    else
    {
        pThis->gpuGroupCount = 1;
    }

    pThis->bIsMigRm = NV_FALSE;

    // Hal field -- bUnifiedMemorySpaceEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->bUnifiedMemorySpaceEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bUnifiedMemorySpaceEnabled = NV_FALSE;
    }

    // Hal field -- bWarBug200577889SriovHeavyEnabled
    pThis->bWarBug200577889SriovHeavyEnabled = NV_FALSE;

    // Hal field -- bNonPowerOf2ChannelCountSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bNonPowerOf2ChannelCountSupported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bNonPowerOf2ChannelCountSupported = NV_FALSE;
    }

    // Hal field -- bWarBug4347206PowerCycleOnUnload
    // default
    {
        pThis->bWarBug4347206PowerCycleOnUnload = NV_FALSE;
    }

    // Hal field -- bNeed4kPageIsolation
    // default
    {
        pThis->bNeed4kPageIsolation = NV_FALSE;
    }

    // Hal field -- bInstLoc47bitPaWar
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bInstLoc47bitPaWar = NV_TRUE;
    }

    // Hal field -- bIsBarPteInSysmemSupported
    // default
    {
        pThis->bIsBarPteInSysmemSupported = NV_FALSE;
    }

    // Hal field -- bClientRmAllocatedCtxBuffer
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0f800UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bClientRmAllocatedCtxBuffer = NV_TRUE;
    }
    // default
    else
    {
        pThis->bClientRmAllocatedCtxBuffer = NV_FALSE;
    }

    // Hal field -- bInstanceMemoryAlwaysCached
    // default
    {
        pThis->bInstanceMemoryAlwaysCached = NV_FALSE;
    }

    // Hal field -- bComputePolicyTimesliceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bComputePolicyTimesliceSupported = NV_TRUE;
    }

    // Hal field -- bSriovCapable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bSriovCapable = NV_TRUE;
    }

    // Hal field -- bRecheckSliSupportAtResume
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bRecheckSliSupportAtResume = NV_TRUE;
    }

    // Hal field -- bGpuNvEncAv1Supported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc1f00000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e4UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GB102 | GB10B | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bGpuNvEncAv1Supported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bGpuNvEncAv1Supported = NV_FALSE;
    }

    pThis->bIsGspOwnedFaultBuffersEnabled = NV_FALSE;

    // Hal field -- bVfResizableBAR1Supported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bVfResizableBAR1Supported = NV_TRUE;
    }
    // default
    else
    {
        pThis->bVfResizableBAR1Supported = NV_FALSE;
    }

    // Hal field -- bVoltaHubIntrSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bVoltaHubIntrSupported = NV_TRUE;
    }

    // Hal field -- bUsePmcDeviceEnableForHostEngine
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bUsePmcDeviceEnableForHostEngine = NV_TRUE;
    }
    // default
    else
    {
        pThis->bUsePmcDeviceEnableForHostEngine = NV_FALSE;
    }

    pThis->bBlockNewWorkload = NV_FALSE;
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RmHalspecOwner(RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE* );
NV_STATUS __nvoc_ctor_OBJGPU(OBJGPU *pThis, NvU32 arg_gpuInstance, NvU32 arg_gpuId, NvUuid * arg_pUuid) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_Object;
    status = __nvoc_ctor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_RmHalspecOwner;
    status = __nvoc_ctor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail_OBJTRACEABLE;
    __nvoc_init_dataField_OBJGPU(pThis);

    status = __nvoc_gpuConstruct(pThis, arg_gpuInstance, arg_gpuId, arg_pUuid);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPU_fail__init;
    goto __nvoc_ctor_OBJGPU_exit; // Success

__nvoc_ctor_OBJGPU_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJGPU_fail_OBJTRACEABLE:
    __nvoc_dtor_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner);
__nvoc_ctor_OBJGPU_fail_RmHalspecOwner:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJGPU_fail_Object:
__nvoc_ctor_OBJGPU_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGPU_1(OBJGPU *pThis) {
    ChipHal *chipHal = &staticCast(pThis, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &staticCast(pThis, RmHalspecOwner)->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // gpuConstructDeviceInfoTable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_VF;
        }
        else
        {
            pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_FWCLIENT;
        }
    }
    // default
    else
    {
        pThis->__gpuConstructDeviceInfoTable__ = &gpuConstructDeviceInfoTable_56cd7a;
    }

    // gpuGetNameString -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetNameString__ = &gpuGetNameString_VF;
    }
    else
    {
        pThis->__gpuGetNameString__ = &gpuGetNameString_FWCLIENT;
    }

    // gpuGetShortNameString -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetShortNameString__ = &gpuGetShortNameString_VF;
    }
    else
    {
        pThis->__gpuGetShortNameString__ = &gpuGetShortNameString_FWCLIENT;
    }

    // gpuInitBranding -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuInitBranding__ = &gpuInitBranding_VF;
    }
    else
    {
        pThis->__gpuInitBranding__ = &gpuInitBranding_FWCLIENT;
    }

    // gpuGetRtd3GC6Data -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetRtd3GC6Data__ = &gpuGetRtd3GC6Data_b3696a;
    }
    else
    {
        pThis->__gpuGetRtd3GC6Data__ = &gpuGetRtd3GC6Data_FWCLIENT;
    }

    // gpuSetResetScratchBit -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuSetResetScratchBit__ = &gpuSetResetScratchBit_46f6a7;
    }
    else
    {
        pThis->__gpuSetResetScratchBit__ = &gpuSetResetScratchBit_GP100;
    }

    // gpuGetResetScratchBit -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetResetScratchBit__ = &gpuGetResetScratchBit_46f6a7;
    }
    else
    {
        pThis->__gpuGetResetScratchBit__ = &gpuGetResetScratchBit_GP100;
    }

    // gpuResetRequiredStateChanged -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuResetRequiredStateChanged__ = &gpuResetRequiredStateChanged_46f6a7;
    }
    else
    {
        pThis->__gpuResetRequiredStateChanged__ = &gpuResetRequiredStateChanged_FWCLIENT;
    }

    // gpuMarkDeviceForReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuMarkDeviceForReset__ = &gpuMarkDeviceForReset_46f6a7;
    }
    else
    {
        pThis->__gpuMarkDeviceForReset__ = &gpuMarkDeviceForReset_IMPL;
    }

    // gpuUnmarkDeviceForReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuUnmarkDeviceForReset__ = &gpuUnmarkDeviceForReset_46f6a7;
    }
    else
    {
        pThis->__gpuUnmarkDeviceForReset__ = &gpuUnmarkDeviceForReset_IMPL;
    }

    // gpuIsDeviceMarkedForReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsDeviceMarkedForReset__ = &gpuIsDeviceMarkedForReset_46f6a7;
    }
    else
    {
        pThis->__gpuIsDeviceMarkedForReset__ = &gpuIsDeviceMarkedForReset_IMPL;
    }

    // gpuSetDrainAndResetScratchBit -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuSetDrainAndResetScratchBit__ = &gpuSetDrainAndResetScratchBit_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__gpuSetDrainAndResetScratchBit__ = &gpuSetDrainAndResetScratchBit_46f6a7;
        }
        else
        {
            pThis->__gpuSetDrainAndResetScratchBit__ = &gpuSetDrainAndResetScratchBit_GA100;
        }
    }

    // gpuGetDrainAndResetScratchBit -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetDrainAndResetScratchBit__ = &gpuGetDrainAndResetScratchBit_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__gpuGetDrainAndResetScratchBit__ = &gpuGetDrainAndResetScratchBit_46f6a7;
        }
        else
        {
            pThis->__gpuGetDrainAndResetScratchBit__ = &gpuGetDrainAndResetScratchBit_GA100;
        }
    }

    // gpuMarkDeviceForDrainAndReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuMarkDeviceForDrainAndReset__ = &gpuMarkDeviceForDrainAndReset_46f6a7;
    }
    else
    {
        pThis->__gpuMarkDeviceForDrainAndReset__ = &gpuMarkDeviceForDrainAndReset_IMPL;
    }

    // gpuUnmarkDeviceForDrainAndReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuUnmarkDeviceForDrainAndReset__ = &gpuUnmarkDeviceForDrainAndReset_46f6a7;
    }
    else
    {
        pThis->__gpuUnmarkDeviceForDrainAndReset__ = &gpuUnmarkDeviceForDrainAndReset_IMPL;
    }

    // gpuIsDeviceMarkedForDrainAndReset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsDeviceMarkedForDrainAndReset__ = &gpuIsDeviceMarkedForDrainAndReset_46f6a7;
    }
    else
    {
        pThis->__gpuIsDeviceMarkedForDrainAndReset__ = &gpuIsDeviceMarkedForDrainAndReset_IMPL;
    }

    // gpuPowerOff -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuPowerOff__ = &gpuPowerOff_46f6a7;
    }
    else
    {
        pThis->__gpuPowerOff__ = &gpuPowerOff_KERNEL;
    }

    // gpuWriteBusConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuWriteBusConfigReg__ = &gpuWriteBusConfigReg_GM107;
    }
    else
    {
        pThis->__gpuWriteBusConfigReg__ = &gpuWriteBusConfigReg_GH100;
    }

    // gpuReadBusConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuReadBusConfigReg__ = &gpuReadBusConfigReg_GM107;
    }
    else
    {
        pThis->__gpuReadBusConfigReg__ = &gpuReadBusConfigReg_GH100;
    }

    // gpuReadBusConfigRegEx -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuReadBusConfigRegEx__ = &gpuReadBusConfigRegEx_GM107;
    }
    else
    {
        pThis->__gpuReadBusConfigRegEx__ = &gpuReadBusConfigRegEx_5baef9;
    }

    // gpuReadFunctionConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuReadFunctionConfigReg__ = &gpuReadFunctionConfigReg_GM107;
    }
    else
    {
        pThis->__gpuReadFunctionConfigReg__ = &gpuReadFunctionConfigReg_5baef9;
    }

    // gpuWriteFunctionConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuWriteFunctionConfigReg__ = &gpuWriteFunctionConfigReg_GM107;
    }
    else
    {
        pThis->__gpuWriteFunctionConfigReg__ = &gpuWriteFunctionConfigReg_5baef9;
    }

    // gpuWriteFunctionConfigRegEx -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuWriteFunctionConfigRegEx__ = &gpuWriteFunctionConfigRegEx_GM107;
    }
    else
    {
        pThis->__gpuWriteFunctionConfigRegEx__ = &gpuWriteFunctionConfigRegEx_5baef9;
    }

    // gpuReadPassThruConfigReg -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GH100 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_GH100;
    }
    // default
    else
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_46f6a7;
    }

    // gpuConfigAccessSanityCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuConfigAccessSanityCheck__ = &gpuConfigAccessSanityCheck_GB100;
    }
    // default
    else
    {
        pThis->__gpuConfigAccessSanityCheck__ = &gpuConfigAccessSanityCheck_56cd7a;
    }

    // gpuReadBusConfigCycle -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__gpuReadBusConfigCycle__ = &gpuReadBusConfigCycle_GB100;
    }
    // default
    else
    {
        pThis->__gpuReadBusConfigCycle__ = &gpuReadBusConfigCycle_GM107;
    }

    // gpuWriteBusConfigCycle -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__gpuWriteBusConfigCycle__ = &gpuWriteBusConfigCycle_GB100;
    }
    // default
    else
    {
        pThis->__gpuWriteBusConfigCycle__ = &gpuWriteBusConfigCycle_GM107;
    }

    // gpuReadPcieConfigCycle -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuReadPcieConfigCycle__ = &gpuReadPcieConfigCycle_GB202;
    }
    // default
    else
    {
        pThis->__gpuReadPcieConfigCycle__ = &gpuReadPcieConfigCycle_46f6a7;
    }

    // gpuWritePcieConfigCycle -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuWritePcieConfigCycle__ = &gpuWritePcieConfigCycle_GB202;
    }
    // default
    else
    {
        pThis->__gpuWritePcieConfigCycle__ = &gpuWritePcieConfigCycle_46f6a7;
    }

    // gpuGetIdInfo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000406UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB20B */ 
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GB100;
    }
    else
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GH100;
    }

    // gpuGenGidData -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGenGidData__ = &gpuGenGidData_VF;
    }
    else
    {
        pThis->__gpuGenGidData__ = &gpuGenGidData_FWCLIENT;
    }

    // gpuGetChipSubRev -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetChipSubRev__ = &gpuGetChipSubRev_4a4dee;
    }
    else
    {
        pThis->__gpuGetChipSubRev__ = &gpuGetChipSubRev_FWCLIENT;
    }

    // gpuGetSkuInfo -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetSkuInfo__ = &gpuGetSkuInfo_VF;
    }
    else
    {
        pThis->__gpuGetSkuInfo__ = &gpuGetSkuInfo_92bfc3;
    }

    // gpuGetRegBaseOffset -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetRegBaseOffset__ = &gpuGetRegBaseOffset_TU102;
    }
    else
    {
        pThis->__gpuGetRegBaseOffset__ = &gpuGetRegBaseOffset_FWCLIENT;
    }

    // gpuHandleSanityCheckRegReadError -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuHandleSanityCheckRegReadError__ = &gpuHandleSanityCheckRegReadError_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__gpuHandleSanityCheckRegReadError__ = &gpuHandleSanityCheckRegReadError_GM107;
        }
        else
        {
            pThis->__gpuHandleSanityCheckRegReadError__ = &gpuHandleSanityCheckRegReadError_GH100;
        }
    }

    // gpuHandleSecFault -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GB100;
    }
    // default
    else
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_b3696a;
    }

    // gpuSanityCheckVirtRegAccess -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_TU102;
        }
        else
        {
            pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_GH100;
        }
    }
    else
    {
        pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_56cd7a;
    }

    // gpuGetChildrenPresent -- halified (12 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000040UL) )) /* ChipHal: TU104 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000080UL) )) /* ChipHal: TU106 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU106;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000320UL) )) /* ChipHal: TU102 | TU116 | TU117 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB100;
    }
    else
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB102;
    }

    // gpuGetClassDescriptorList -- halified (13 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000040UL) )) /* ChipHal: TU104 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000080UL) )) /* ChipHal: TU106 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU106;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000200UL) )) /* ChipHal: TU117 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU117;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB20B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000120UL) )) /* ChipHal: TU102 | TU116 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB100 | GB110 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB100;
    }
    else
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB102;
    }

    // gpuGetPhysAddrWidth -- halified (3 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB10B | GB20B */ 
    {
        pThis->__gpuGetPhysAddrWidth__ = &gpuGetPhysAddrWidth_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuGetPhysAddrWidth__ = &gpuGetPhysAddrWidth_TU102;
    }
    else
    {
        pThis->__gpuGetPhysAddrWidth__ = &gpuGetPhysAddrWidth_GH100;
    }

    // gpuInitSriov -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuInitSriov__ = &gpuInitSriov_VF;
    }
    else
    {
        pThis->__gpuInitSriov__ = &gpuInitSriov_FWCLIENT;
    }

    // gpuDeinitSriov -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuDeinitSriov__ = &gpuDeinitSriov_56cd7a;
    }
    else
    {
        pThis->__gpuDeinitSriov__ = &gpuDeinitSriov_FWCLIENT;
    }

    // gpuCreateDefaultClientShare -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuCreateDefaultClientShare__ = &gpuCreateDefaultClientShare_VF;
    }
    else
    {
        pThis->__gpuCreateDefaultClientShare__ = &gpuCreateDefaultClientShare_56cd7a;
    }

    // gpuDestroyDefaultClientShare -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuDestroyDefaultClientShare__ = &gpuDestroyDefaultClientShare_VF;
    }
    else
    {
        pThis->__gpuDestroyDefaultClientShare__ = &gpuDestroyDefaultClientShare_b3696a;
    }

    // gpuFuseSupportsDisplay -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GB20B */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_88bc07;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000400UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_3dd2c9;
    }
    else
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_GA100;
    }

    // gpuGetActiveFBIOs -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetActiveFBIOs__ = &gpuGetActiveFBIOs_VF;
    }
    else
    {
        pThis->__gpuGetActiveFBIOs__ = &gpuGetActiveFBIOs_FWCLIENT;
    }

    // gpuIsGspToBootInInstInSysMode -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuIsGspToBootInInstInSysMode__ = &gpuIsGspToBootInInstInSysMode_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuIsGspToBootInInstInSysMode__ = &gpuIsGspToBootInInstInSysMode_GH100;
    }
    // default
    else
    {
        pThis->__gpuIsGspToBootInInstInSysMode__ = &gpuIsGspToBootInInstInSysMode_3dd2c9;
    }

    // gpuCheckPageRetirementSupport -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuCheckPageRetirementSupport__ = &gpuCheckPageRetirementSupport_VF;
    }
    else
    {
        pThis->__gpuCheckPageRetirementSupport__ = &gpuCheckPageRetirementSupport_GSPCLIENT;
    }

    // gpuIsInternalSku -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsInternalSku__ = &gpuIsInternalSku_3dd2c9;
    }
    else
    {
        pThis->__gpuIsInternalSku__ = &gpuIsInternalSku_FWCLIENT;
    }

    // gpuClearFbhubPoisonIntrForBug2924523 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_GA100;
    }
    // default
    else
    {
        pThis->__gpuClearFbhubPoisonIntrForBug2924523__ = &gpuClearFbhubPoisonIntrForBug2924523_56cd7a;
    }

    // gpuCheckIfFbhubPoisonIntrPending -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__gpuCheckIfFbhubPoisonIntrPending__ = &gpuCheckIfFbhubPoisonIntrPending_GA100;
    }
    // default
    else
    {
        pThis->__gpuCheckIfFbhubPoisonIntrPending__ = &gpuCheckIfFbhubPoisonIntrPending_3dd2c9;
    }

    // gpuGetSriovCaps -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetSriovCaps__ = &gpuGetSriovCaps_46f6a7;
    }
    else
    {
        pThis->__gpuGetSriovCaps__ = &gpuGetSriovCaps_TU102;
    }

    // gpuCheckIsP2PAllocated -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_86b752;
        }
        else
        {
            pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_GA100;
        }
    }
    else
    {
        pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_3dd2c9;
    }

    // gpuPrePowerOff -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuPrePowerOff__ = &gpuPrePowerOff_46f6a7;
    }
    else
    {
        pThis->__gpuPrePowerOff__ = &gpuPrePowerOff_56cd7a;
    }

    // gpuVerifyExistence -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuVerifyExistence__ = &gpuVerifyExistence_56cd7a;
    }
    else
    {
        pThis->__gpuVerifyExistence__ = &gpuVerifyExistence_IMPL;
    }

    // gpuGetFlaVasSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_474d46;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_GA100;
    }
    else
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_GH100;
    }

    // gpuIsAtsSupportedWithSmcMemPartitioning -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuIsAtsSupportedWithSmcMemPartitioning__ = &gpuIsAtsSupportedWithSmcMemPartitioning_GH100;
    }
    // default
    else
    {
        pThis->__gpuIsAtsSupportedWithSmcMemPartitioning__ = &gpuIsAtsSupportedWithSmcMemPartitioning_3dd2c9;
    }

    // gpuIsGlobalPoisonFuseEnabled -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsGlobalPoisonFuseEnabled__ = &gpuIsGlobalPoisonFuseEnabled_VF;
    }
    else
    {
        pThis->__gpuIsGlobalPoisonFuseEnabled__ = &gpuIsGlobalPoisonFuseEnabled_FWCLIENT;
    }

    // gpuDetermineSelfHostedMode -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuDetermineSelfHostedMode__ = &gpuDetermineSelfHostedMode_KERNEL_GH100;
    }
    // default
    else
    {
        pThis->__gpuDetermineSelfHostedMode__ = &gpuDetermineSelfHostedMode_b3696a;
    }

    // gpuDetermineSelfHostedSocType -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuDetermineSelfHostedSocType__ = &gpuDetermineSelfHostedSocType_GH100;
    }
    // default
    else
    {
        pThis->__gpuDetermineSelfHostedSocType__ = &gpuDetermineSelfHostedSocType_997682;
    }

    // gpuValidateMIGSupport -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuValidateMIGSupport__ = &gpuValidateMIGSupport_VF;
    }
    else
    {
        pThis->__gpuValidateMIGSupport__ = &gpuValidateMIGSupport_KERNEL;
    }

    // gpuInitOptimusSettings -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuInitOptimusSettings__ = &gpuInitOptimusSettings_56cd7a;
    }
    else
    {
        pThis->__gpuInitOptimusSettings__ = &gpuInitOptimusSettings_IMPL;
    }

    // gpuDeinitOptimusSettings -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuDeinitOptimusSettings__ = &gpuDeinitOptimusSettings_56cd7a;
    }
    else
    {
        pThis->__gpuDeinitOptimusSettings__ = &gpuDeinitOptimusSettings_IMPL;
    }

    // gpuIsSliCapableWithoutDisplay -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__gpuIsSliCapableWithoutDisplay__ = &gpuIsSliCapableWithoutDisplay_88bc07;
    }
    // default
    else
    {
        pThis->__gpuIsSliCapableWithoutDisplay__ = &gpuIsSliCapableWithoutDisplay_3dd2c9;
    }

    // gpuIsCCEnabledInHw -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_3dd2c9;
        }
    }

    // gpuIsDevModeEnabledInHw -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_3dd2c9;
        }
    }

    // gpuIsProtectedPcieEnabledInHw -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsProtectedPcieEnabledInHw__ = &gpuIsProtectedPcieEnabledInHw_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsProtectedPcieEnabledInHw__ = &gpuIsProtectedPcieEnabledInHw_GH100;
        }
        // default
        else
        {
            pThis->__gpuIsProtectedPcieEnabledInHw__ = &gpuIsProtectedPcieEnabledInHw_3dd2c9;
        }
    }

    // gpuIsProtectedPcieSupportedInFirmware -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsProtectedPcieSupportedInFirmware__ = &gpuIsProtectedPcieSupportedInFirmware_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsProtectedPcieSupportedInFirmware__ = &gpuIsProtectedPcieSupportedInFirmware_GH100;
        }
        // default
        else
        {
            pThis->__gpuIsProtectedPcieSupportedInFirmware__ = &gpuIsProtectedPcieSupportedInFirmware_3dd2c9;
        }
    }

    // gpuIsMultiGpuNvleEnabledInHw -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsMultiGpuNvleEnabledInHw__ = &gpuIsMultiGpuNvleEnabledInHw_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
        {
            pThis->__gpuIsMultiGpuNvleEnabledInHw__ = &gpuIsMultiGpuNvleEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsMultiGpuNvleEnabledInHw__ = &gpuIsMultiGpuNvleEnabledInHw_3dd2c9;
        }
    }

    // gpuIsNvleModeEnabledInHw -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsNvleModeEnabledInHw__ = &gpuIsNvleModeEnabledInHw_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__gpuIsNvleModeEnabledInHw__ = &gpuIsNvleModeEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsNvleModeEnabledInHw__ = &gpuIsNvleModeEnabledInHw_3dd2c9;
        }
    }

    // gpuIsCtxBufAllocInPmaSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuIsCtxBufAllocInPmaSupported__ = &gpuIsCtxBufAllocInPmaSupported_GA100;
    }
    // default
    else
    {
        pThis->__gpuIsCtxBufAllocInPmaSupported__ = &gpuIsCtxBufAllocInPmaSupported_3dd2c9;
    }

    // gpuGetErrorContStateTableAndSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuGetErrorContStateTableAndSize__ = &gpuGetErrorContStateTableAndSize_11d6dc;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__gpuGetErrorContStateTableAndSize__ = &gpuGetErrorContStateTableAndSize_GA100;
    }
    else
    {
        pThis->__gpuGetErrorContStateTableAndSize__ = &gpuGetErrorContStateTableAndSize_GB100;
    }

    // gpuUpdateErrorContainmentState -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuUpdateErrorContainmentState__ = &gpuUpdateErrorContainmentState_f91eed;
    }
    else
    {
        pThis->__gpuUpdateErrorContainmentState__ = &gpuUpdateErrorContainmentState_GA100;
    }

    // gpuSetPartitionErrorAttribution -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuSetPartitionErrorAttribution__ = &gpuSetPartitionErrorAttribution_KERNEL;
    }
    // default
    else
    {
        pThis->__gpuSetPartitionErrorAttribution__ = &gpuSetPartitionErrorAttribution_c04480;
    }

    // gpuWaitForGfwBootComplete -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuWaitForGfwBootComplete__ = &gpuWaitForGfwBootComplete_5baef9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__gpuWaitForGfwBootComplete__ = &gpuWaitForGfwBootComplete_TU102;
        }
        // default
        else
        {
            pThis->__gpuWaitForGfwBootComplete__ = &gpuWaitForGfwBootComplete_5baef9;
        }
    }

    // gpuGetFirstAsyncLce -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__gpuGetFirstAsyncLce__ = &gpuGetFirstAsyncLce_GB202;
    }
    // default
    else
    {
        pThis->__gpuGetFirstAsyncLce__ = &gpuGetFirstAsyncLce_54c809;
    }

    // gpuIsInternalSkuFuseEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__gpuIsInternalSkuFuseEnabled__ = &gpuIsInternalSkuFuseEnabled_GB202;
    }
    // default
    else
    {
        pThis->__gpuIsInternalSkuFuseEnabled__ = &gpuIsInternalSkuFuseEnabled_3dd2c9;
    }

    // gpuRequireGrCePresence -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuRequireGrCePresence__ = &gpuRequireGrCePresence_3dd2c9;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__gpuRequireGrCePresence__ = &gpuRequireGrCePresence_GB202;
        }
        // default
        else
        {
            pThis->__gpuRequireGrCePresence__ = &gpuRequireGrCePresence_3dd2c9;
        }
    }

    // gpuGetIsCmpSku -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetIsCmpSku__ = &gpuGetIsCmpSku_3dd2c9;
    }
    else
    {
        pThis->__gpuGetIsCmpSku__ = &gpuGetIsCmpSku_72a2e1;
    }
} // End __nvoc_init_funcTable_OBJGPU_1 with approximately 207 basic block(s).


// Initialize vtable(s) for 79 virtual method(s).
void __nvoc_init_funcTable_OBJGPU(OBJGPU *pThis) {

    // Initialize vtable(s) with 79 per-object function pointer(s).
    __nvoc_init_funcTable_OBJGPU_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJGPU(OBJGPU *pThis,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RmHalspecOwner = &pThis->__nvoc_base_RmHalspecOwner;    // (rmhalspecowner) super
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_pbase_OBJGPU = pThis;    // (gpu) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver);
    __nvoc_init__OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPU.metadata__Object;    // (obj) super
    pThis->__nvoc_base_RmHalspecOwner.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPU.metadata__RmHalspecOwner;    // (rmhalspecowner) super
    pThis->__nvoc_base_OBJTRACEABLE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPU.metadata__OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPU;    // (gpu) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGPU(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPU(OBJGPU **ppThis, Dynamic *pParent, NvU32 createFlags,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver, NvU32 arg_gpuInstance, NvU32 arg_gpuId, NvUuid * arg_pUuid)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJGPU *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJGPU), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJGPU));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__OBJGPU(pThis, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver);
    status = __nvoc_ctor_OBJGPU(pThis, arg_gpuInstance, arg_gpuId, arg_pUuid);
    if (status != NV_OK) goto __nvoc_objCreate_OBJGPU_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJGPU_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJGPU));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPU(OBJGPU **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    NvU32 ChipHal_arch = va_arg(args, NvU32);
    NvU32 ChipHal_impl = va_arg(args, NvU32);
    NvU32 ChipHal_hidrev = va_arg(args, NvU32);
    RM_RUNTIME_VARIANT RmVariantHal_rmVariant = va_arg(args, RM_RUNTIME_VARIANT);
    TEGRA_CHIP_TYPE TegraChipHal_tegraType = va_arg(args, TEGRA_CHIP_TYPE);
    NvU32 DispIpHal_ipver = va_arg(args, NvU32);
    NvU32 arg_gpuInstance = va_arg(args, NvU32);
    NvU32 arg_gpuId = va_arg(args, NvU32);
    NvUuid * arg_pUuid = va_arg(args, NvUuid *);

    status = __nvoc_objCreate_OBJGPU(ppThis, pParent, createFlags, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver, arg_gpuInstance, arg_gpuId, arg_pUuid);

    return status;
}

