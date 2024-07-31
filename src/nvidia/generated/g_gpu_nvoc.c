#define NVOC_GPU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_gpu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7ef3cb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmHalspecOwner;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

void __nvoc_init_OBJGPU(OBJGPU*,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver);
void __nvoc_init_funcTable_OBJGPU(OBJGPU*);
NV_STATUS __nvoc_ctor_OBJGPU(OBJGPU*, NvU32 arg_gpuInstance, NvU32 arg_gpuId, NvUuid * arg_pUuid);
void __nvoc_init_dataField_OBJGPU(OBJGPU*);
void __nvoc_dtor_OBJGPU(OBJGPU*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPU;

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_OBJGPU = {
    /*pClassDef=*/          &__nvoc_class_def_OBJGPU,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPU,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_RmHalspecOwner = {
    /*pClassDef=*/          &__nvoc_class_def_RmHalspecOwner,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_RmHalspecOwner),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJGPU_OBJTRACEABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTRACEABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJGPU, __nvoc_base_OBJTRACEABLE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJGPU = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_OBJGPU_OBJGPU,
        &__nvoc_rtti_OBJGPU_OBJTRACEABLE,
        &__nvoc_rtti_OBJGPU_RmHalspecOwner,
        &__nvoc_rtti_OBJGPU_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_OBJGPU,
    /*pExportInfo=*/        &__nvoc_export_info_OBJGPU
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJGPU = 
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
    pThis->setProperty(pThis, PDB_PROP_GPU_IS_CONNECTED, ((NvBool)(0 == 0)));

    // NVOC Property Hal field -- PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_TEGRA_SOC_IGPU
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_TEGRA_SOC_IGPU, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_ATS_SUPPORTED
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ATS_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_ZERO_FB
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_ZERO_FB, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_MIG_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_MIG_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_IS_COT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_COT_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_COT_ENABLED, ((NvBool)(0 != 0)));
    }

    // Hal field -- bIsFlexibleFlaSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->bIsFlexibleFlaSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bIsFlexibleFlaSupported = ((NvBool)(0 != 0));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK, ((NvBool)(0 != 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK, ((NvBool)(0 == 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE, ((NvBool)(0 == 0)));

    // NVOC Property Hal field -- PDB_PROP_GPU_CC_FEATURE_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CC_FEATURE_CAPABLE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CC_FEATURE_CAPABLE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_APM_FEATURE_CAPABLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_APM_FEATURE_CAPABLE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_APM_FEATURE_CAPABLE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_IS_SOC_SDM
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_GPU_IS_SOC_SDM, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS
    pThis->setProperty(pThis, PDB_PROP_GPU_DISP_PB_REQUIRES_SMMU_BYPASS, ((NvBool)(0 == 0)));
    pThis->setProperty(pThis, PDB_PROP_GPU_FASTPATH_SEQ_ENABLED, ((NvBool)(0 != 0)));

    pThis->deviceInstance = 32;

    // Hal field -- isVirtual
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->isVirtual = ((NvBool)(0 == 0));
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isVirtual = ((NvBool)(0 != 0));
    }

    // Hal field -- isGspClient
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->isGspClient = ((NvBool)(0 == 0));
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->isGspClient = ((NvBool)(0 != 0));
    }

    pThis->bIsDebugModeEnabled = ((NvBool)(0 != 0));

    pThis->numOfMclkLockRequests = 0U;

    pThis->bUseRegisterAccessMap = !(0);

    pThis->boardInfo = ((void *)0);

    // Hal field -- gpuGroupCount
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->gpuGroupCount = 2;
    }
    // default
    else
    {
        pThis->gpuGroupCount = 1;
    }

    pThis->bIsMigRm = ((NvBool)(0 != 0));

    // Hal field -- bUnifiedMemorySpaceEnabled
    // default
    {
        pThis->bUnifiedMemorySpaceEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bWarBug200577889SriovHeavyEnabled
    pThis->bWarBug200577889SriovHeavyEnabled = ((NvBool)(0 != 0));

    // Hal field -- bNonPowerOf2ChannelCountSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->bNonPowerOf2ChannelCountSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bNonPowerOf2ChannelCountSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bWarBug4347206PowerCycleOnUnload
    // default
    {
        pThis->bWarBug4347206PowerCycleOnUnload = ((NvBool)(0 != 0));
    }

    // Hal field -- bNeed4kPageIsolation
    // default
    {
        pThis->bNeed4kPageIsolation = ((NvBool)(0 != 0));
    }

    // Hal field -- bInstLoc47bitPaWar
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bInstLoc47bitPaWar = ((NvBool)(0 == 0));
    }

    // Hal field -- bIsBarPteInSysmemSupported
    // default
    {
        pThis->bIsBarPteInSysmemSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bClientRmAllocatedCtxBuffer
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bClientRmAllocatedCtxBuffer = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bClientRmAllocatedCtxBuffer = ((NvBool)(0 != 0));
    }

    pThis->bIterativeMmuWalker = ((NvBool)(0 == 0));

    // Hal field -- bVidmemPreservationBrokenBug3172217
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->bVidmemPreservationBrokenBug3172217 = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bVidmemPreservationBrokenBug3172217 = ((NvBool)(0 != 0));
    }

    // Hal field -- bInstanceMemoryAlwaysCached
    // default
    {
        pThis->bInstanceMemoryAlwaysCached = ((NvBool)(0 != 0));
    }

    // Hal field -- bComputePolicyTimesliceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bComputePolicyTimesliceSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bSriovCapable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bSriovCapable = ((NvBool)(0 == 0));
    }

    // Hal field -- bRecheckSliSupportAtResume
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bRecheckSliSupportAtResume = ((NvBool)(0 == 0));
    }

    // Hal field -- bGpuNvEncAv1Supported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x81f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 | GB102 */ 
    {
        pThis->bGpuNvEncAv1Supported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bGpuNvEncAv1Supported = ((NvBool)(0 != 0));
    }

    pThis->bIsGspOwnedFaultBuffersEnabled = ((NvBool)(0 != 0));

    // Hal field -- bVfResizableBAR1Supported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->bVfResizableBAR1Supported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bVfResizableBAR1Supported = ((NvBool)(0 != 0));
    }

    // Hal field -- bVoltaHubIntrSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bVoltaHubIntrSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bUsePmcDeviceEnableForHostEngine
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bUsePmcDeviceEnableForHostEngine = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bUsePmcDeviceEnableForHostEngine = ((NvBool)(0 != 0));
    }
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

    // gpuGetDeviceInfoTableSectionInfos -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuGetDeviceInfoTableSectionInfos__ = &gpuGetDeviceInfoTableSectionInfos_GA100;
    }
    // default
    else
    {
        pThis->__gpuGetDeviceInfoTableSectionInfos__ = &gpuGetDeviceInfoTableSectionInfos_46f6a7;
    }

    // gpuParseDeviceInfoTableSection -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        // default
        pThis->__gpuParseDeviceInfoTableSection__ = &gpuParseDeviceInfoTableSection_46f6a7;
    }
    // default
    else
    {
        pThis->__gpuParseDeviceInfoTableSection__ = &gpuParseDeviceInfoTableSection_46f6a7;
    }

    // gpuConstructDeviceInfoTable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuWriteBusConfigReg__ = &gpuWriteBusConfigReg_GH100;
    }
    else
    {
        pThis->__gpuWriteBusConfigReg__ = &gpuWriteBusConfigReg_GM107;
    }

    // gpuReadBusConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuReadBusConfigReg__ = &gpuReadBusConfigReg_GH100;
    }
    else
    {
        pThis->__gpuReadBusConfigReg__ = &gpuReadBusConfigReg_GM107;
    }

    // gpuReadBusConfigRegEx -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuReadBusConfigRegEx__ = &gpuReadBusConfigRegEx_5baef9;
    }
    else
    {
        pThis->__gpuReadBusConfigRegEx__ = &gpuReadBusConfigRegEx_GM107;
    }

    // gpuReadFunctionConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuReadFunctionConfigReg__ = &gpuReadFunctionConfigReg_5baef9;
    }
    else
    {
        pThis->__gpuReadFunctionConfigReg__ = &gpuReadFunctionConfigReg_GM107;
    }

    // gpuWriteFunctionConfigReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuWriteFunctionConfigReg__ = &gpuWriteFunctionConfigReg_5baef9;
    }
    else
    {
        pThis->__gpuWriteFunctionConfigReg__ = &gpuWriteFunctionConfigReg_GM107;
    }

    // gpuWriteFunctionConfigRegEx -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuWriteFunctionConfigRegEx__ = &gpuWriteFunctionConfigRegEx_5baef9;
    }
    else
    {
        pThis->__gpuWriteFunctionConfigRegEx__ = &gpuWriteFunctionConfigRegEx_GM107;
    }

    // gpuReadPassThruConfigReg -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_GB100;
    }
    // default
    else
    {
        pThis->__gpuReadPassThruConfigReg__ = &gpuReadPassThruConfigReg_46f6a7;
    }

    // gpuGetIdInfo -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GB100;
    }
    else
    {
        pThis->__gpuGetIdInfo__ = &gpuGetIdInfo_GM107;
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

    // gpuHandleSanityCheckRegReadError -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuHandleSanityCheckRegReadError__ = &gpuHandleSanityCheckRegReadError_GH100;
    }
    else
    {
        pThis->__gpuHandleSanityCheckRegReadError__ = &gpuHandleSanityCheckRegReadError_GM107;
    }

    // gpuHandleSecFault -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__gpuHandleSecFault__ = &gpuHandleSecFault_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
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
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
        {
            pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_GH100;
        }
        else
        {
            pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_TU102;
        }
    }
    else
    {
        pThis->__gpuSanityCheckVirtRegAccess__ = &gpuSanityCheckVirtRegAccess_56cd7a;
    }

    // gpuGetChildrenPresent -- halified (9 hals)
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB102 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GB102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000320UL) )) /* ChipHal: TU102 | TU116 | TU117 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_GA102;
    }
    else
    {
        pThis->__gpuGetChildrenPresent__ = &gpuGetChildrenPresent_AD102;
    }

    // gpuGetClassDescriptorList -- halified (10 hals)
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB102 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GB102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000120UL) )) /* ChipHal: TU102 | TU116 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_GA102;
    }
    else
    {
        pThis->__gpuGetClassDescriptorList__ = &gpuGetClassDescriptorList_AD102;
    }

    // gpuGetPhysAddrWidth -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuGetPhysAddrWidth__ = &gpuGetPhysAddrWidth_GH100;
    }
    else
    {
        pThis->__gpuGetPhysAddrWidth__ = &gpuGetPhysAddrWidth_TU102;
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

    // gpuFuseSupportsDisplay -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_491d52;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuFuseSupportsDisplay__ = &gpuFuseSupportsDisplay_GM107;
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

    // gpuIsGspToBootInInstInSysMode -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuIsGspToBootInInstInSysMode__ = &gpuIsGspToBootInInstInSysMode_GH100;
    }
    // default
    else
    {
        pThis->__gpuIsGspToBootInInstInSysMode__ = &gpuIsGspToBootInInstInSysMode_491d52;
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
        pThis->__gpuIsInternalSku__ = &gpuIsInternalSku_491d52;
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
        pThis->__gpuCheckIfFbhubPoisonIntrPending__ = &gpuCheckIfFbhubPoisonIntrPending_491d52;
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
            pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_108313;
        }
        else
        {
            pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_GA100;
        }
    }
    else
    {
        pThis->__gpuCheckIsP2PAllocated__ = &gpuCheckIsP2PAllocated_491d52;
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_474d46;
    }
    else
    {
        pThis->__gpuGetFlaVasSize__ = &gpuGetFlaVasSize_GA100;
    }

    // gpuIsAtsSupportedWithSmcMemPartitioning -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuIsAtsSupportedWithSmcMemPartitioning__ = &gpuIsAtsSupportedWithSmcMemPartitioning_GH100;
    }
    // default
    else
    {
        pThis->__gpuIsAtsSupportedWithSmcMemPartitioning__ = &gpuIsAtsSupportedWithSmcMemPartitioning_491d52;
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuDetermineSelfHostedMode__ = &gpuDetermineSelfHostedMode_KERNEL_GH100;
    }
    // default
    else
    {
        pThis->__gpuDetermineSelfHostedMode__ = &gpuDetermineSelfHostedMode_b3696a;
    }

    // gpuDetermineMIGSupport -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuDetermineMIGSupport__ = &gpuDetermineMIGSupport_VF;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuDetermineMIGSupport__ = &gpuDetermineMIGSupport_GH100;
        }
        // default
        else
        {
            pThis->__gpuDetermineMIGSupport__ = &gpuDetermineMIGSupport_b3696a;
        }
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuIsSliCapableWithoutDisplay__ = &gpuIsSliCapableWithoutDisplay_cbe027;
    }
    // default
    else
    {
        pThis->__gpuIsSliCapableWithoutDisplay__ = &gpuIsSliCapableWithoutDisplay_491d52;
    }

    // gpuIsCCEnabledInHw -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_491d52;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsCCEnabledInHw__ = &gpuIsCCEnabledInHw_491d52;
        }
    }

    // gpuIsDevModeEnabledInHw -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_491d52;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_GB100;
        }
        // default
        else
        {
            pThis->__gpuIsDevModeEnabledInHw__ = &gpuIsDevModeEnabledInHw_491d52;
        }
    }

    // gpuIsProtectedPcieEnabledInHw -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuIsProtectedPcieEnabledInHw__ = &gpuIsProtectedPcieEnabledInHw_491d52;
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
            pThis->__gpuIsProtectedPcieEnabledInHw__ = &gpuIsProtectedPcieEnabledInHw_491d52;
        }
    }

    // gpuIsCtxBufAllocInPmaSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->__gpuIsCtxBufAllocInPmaSupported__ = &gpuIsCtxBufAllocInPmaSupported_GA100;
    }
    // default
    else
    {
        pThis->__gpuIsCtxBufAllocInPmaSupported__ = &gpuIsCtxBufAllocInPmaSupported_491d52;
    }

    // gpuUpdateErrorContainmentState -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__gpuUpdateErrorContainmentState__ = &gpuUpdateErrorContainmentState_f91eed;
        }
        else
        {
            pThis->__gpuUpdateErrorContainmentState__ = &gpuUpdateErrorContainmentState_GA100;
        }
    }
    else
    {
        pThis->__gpuUpdateErrorContainmentState__ = &gpuUpdateErrorContainmentState_c04480;
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

    // gpuGetIsCmpSku -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__gpuGetIsCmpSku__ = &gpuGetIsCmpSku_491d52;
    }
    else
    {
        pThis->__gpuGetIsCmpSku__ = &gpuGetIsCmpSku_ceaee8;
    }
} // End __nvoc_init_funcTable_OBJGPU_1 with approximately 143 basic block(s).


// Initialize vtable(s) for 56 virtual method(s).
void __nvoc_init_funcTable_OBJGPU(OBJGPU *pThis) {

    // Initialize vtable(s) with 56 per-object function pointer(s).
    __nvoc_init_funcTable_OBJGPU_1(pThis);
}

void __nvoc_init_Object(Object*);
void __nvoc_init_RmHalspecOwner(RmHalspecOwner*, NvU32, NvU32, NvU32, RM_RUNTIME_VARIANT, TEGRA_CHIP_TYPE, NvU32);
void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_OBJGPU(OBJGPU *pThis,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver) {
    pThis->__nvoc_pbase_OBJGPU = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_RmHalspecOwner = &pThis->__nvoc_base_RmHalspecOwner;
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_RmHalspecOwner(&pThis->__nvoc_base_RmHalspecOwner, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver);
    __nvoc_init_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJGPU);

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

    __nvoc_init_OBJGPU(pThis, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver);
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

