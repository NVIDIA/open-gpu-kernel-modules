#define NVOC_KERNEL_BIF_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_bif_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xdbe523 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelBif
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelBif(KernelBif*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelBif(KernelBif*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelBif(KernelBif*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelBif(KernelBif*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelBif(KernelBif*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelBif;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelBif;

// Down-thunk(s) to bridge KernelBif methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3);    // this

// Up-thunk(s) to bridge KernelBif methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kbifInitMissing(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kbifIsPresent(struct OBJGPU *pGpu, struct KernelBif *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelBif),
        /*classId=*/            classId(KernelBif),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelBif",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelBif,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelBif,
    /*pExportInfo=*/        &__nvoc_export_info__KernelBif
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelBif __nvoc_metadata__KernelBif = {
    .rtti.pClassDef = &__nvoc_class_def_KernelBif,    // (kbif) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelBif,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kbifConstructEngine__ = &kbifConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelBif_engstateConstructEngine,    // virtual
    .vtable.__kbifStateInitLocked__ = &kbifStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelBif_engstateStateInitLocked,    // virtual
    .vtable.__kbifStateLoad__ = &kbifStateLoad_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelBif_engstateStateLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelBif_engstateStatePostLoad,    // virtual
    .vtable.__kbifStateUnload__ = &kbifStateUnload_IMPL,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelBif_engstateStateUnload,    // virtual
    .vtable.__kbifInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kbifInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kbifStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kbifStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kbifStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kbifStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kbifStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kbifStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kbifStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__kbifIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kbifIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelBif = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelBif.rtti,    // [0]: (kbif) this
        &__nvoc_metadata__KernelBif.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelBif.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 5 down-thunk(s) defined to bridge methods in KernelBif from superclasses

// kbifConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, ENGDESCRIPTOR arg3) {
    return kbifConstructEngine(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif) {
    return kbifStateInitLocked(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStateLoad(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStatePostLoad(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBif_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBif, NvU32 arg3) {
    return kbifStateUnload(pGpu, (struct KernelBif *)(((unsigned char *) pKernelBif) - NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}


// 9 up-thunk(s) defined to bridge methods in KernelBif to superclasses

// kbifInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kbifInitMissing(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitLocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreLoad(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePreUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbifStatePostUnload(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbifStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kbifStateDestroy(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}

// kbifIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kbifIsPresent(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBif, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelBif = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_kbifDestruct(KernelBif*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelBif(KernelBif *pThis) {
    __nvoc_kbifDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelBif(KernelBif *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_KBIF_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_MISSING, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI, NV_FALSE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB10B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_READS_DISABLED
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_READS_DISABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_READS_DISABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_P2P_WRITES_DISABLED
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fbe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_KBIF_SUPPORT_NONCOHERENT, NV_TRUE);

    // NVOC Property Hal field -- PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBIF_FLR_HANDLED_BY_OS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_HANDLED_BY_OS, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBIF_FLR_HANDLED_BY_OS, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelBif(KernelBif *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelBif_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelBif(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelBif_exit; // Success

__nvoc_ctor_KernelBif_fail_OBJENGSTATE:
__nvoc_ctor_KernelBif_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelBif_1(KernelBif *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kbifStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifStatePostLoad__ = &kbifStatePostLoad_56cd7a;
    }
    else
    {
        pThis->__kbifStatePostLoad__ = &kbifStatePostLoad_IMPL;
    }

    // kbifDestruct -- halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDestruct__ = &kbifDestruct_b3696a;
    }
    else
    {
        pThis->__kbifDestruct__ = &kbifDestruct_GM107;
    }

    // kbifInitLtr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifInitLtr__ = &kbifInitLtr_GB202;
    }
    // default
    else
    {
        pThis->__kbifInitLtr__ = &kbifInitLtr_b3696a;
    }

    // kbifInitDmaCaps -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifInitDmaCaps__ = &kbifInitDmaCaps_VF;
    }
    else
    {
        pThis->__kbifInitDmaCaps__ = &kbifInitDmaCaps_IMPL;
    }

    // kbifSavePcieConfigRegisters -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifSavePcieConfigRegisters__ = &kbifSavePcieConfigRegisters_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifSavePcieConfigRegisters__ = &kbifSavePcieConfigRegisters_GM107;
    }
    else
    {
        pThis->__kbifSavePcieConfigRegisters__ = &kbifSavePcieConfigRegisters_GH100;
    }

    // kbifRestorePcieConfigRegisters -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifRestorePcieConfigRegisters__ = &kbifRestorePcieConfigRegisters_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRestorePcieConfigRegisters__ = &kbifRestorePcieConfigRegisters_GM107;
    }
    else
    {
        pThis->__kbifRestorePcieConfigRegisters__ = &kbifRestorePcieConfigRegisters_GH100;
    }

    // kbifSavePcieConfigRegistersFn1 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifSavePcieConfigRegistersFn1__ = &kbifSavePcieConfigRegistersFn1_GB202;
    }
    // default
    else
    {
        pThis->__kbifSavePcieConfigRegistersFn1__ = &kbifSavePcieConfigRegistersFn1_56cd7a;
    }

    // kbifRestorePcieConfigRegistersFn1 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifRestorePcieConfigRegistersFn1__ = &kbifRestorePcieConfigRegistersFn1_GB202;
    }
    // default
    else
    {
        pThis->__kbifRestorePcieConfigRegistersFn1__ = &kbifRestorePcieConfigRegistersFn1_56cd7a;
    }

    // kbifPollBarFirewallDisengage -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifPollBarFirewallDisengage__ = &kbifPollBarFirewallDisengage_GB202;
    }
    // default
    else
    {
        pThis->__kbifPollBarFirewallDisengage__ = &kbifPollBarFirewallDisengage_56cd7a;
    }

    // kbifGetXveStatusBits -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GB100;
    }
    else
    {
        pThis->__kbifGetXveStatusBits__ = &kbifGetXveStatusBits_GH100;
    }

    // kbifClearXveStatus -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GB100;
    }
    else
    {
        pThis->__kbifClearXveStatus__ = &kbifClearXveStatus_GH100;
    }

    // kbifGetXveAerBits -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GB100;
    }
    else
    {
        pThis->__kbifGetXveAerBits__ = &kbifGetXveAerBits_GH100;
    }

    // kbifClearXveAer -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GB100;
    }
    else
    {
        pThis->__kbifClearXveAer__ = &kbifClearXveAer_GH100;
    }

    // kbifGetPcieConfigAccessTestRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetPcieConfigAccessTestRegisters__ = &kbifGetPcieConfigAccessTestRegisters_GM107;
    }
    else
    {
        pThis->__kbifGetPcieConfigAccessTestRegisters__ = &kbifGetPcieConfigAccessTestRegisters_b3696a;
    }

    // kbifVerifyPcieConfigAccessTestRegisters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifVerifyPcieConfigAccessTestRegisters__ = &kbifVerifyPcieConfigAccessTestRegisters_GM107;
    }
    else
    {
        pThis->__kbifVerifyPcieConfigAccessTestRegisters__ = &kbifVerifyPcieConfigAccessTestRegisters_56cd7a;
    }

    // kbifRearmMSI -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifRearmMSI__ = &kbifRearmMSI_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRearmMSI__ = &kbifRearmMSI_GM107;
    }
    else
    {
        pThis->__kbifRearmMSI__ = &kbifRearmMSI_f2d351;
    }

    // kbifIsMSIEnabledInHW -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_GH100;
    }
    else
    {
        pThis->__kbifIsMSIEnabledInHW__ = &kbifIsMSIEnabledInHW_3dd2c9;
    }

    // kbifIsMSIXEnabledInHW -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_GB100;
    }
    else
    {
        pThis->__kbifIsMSIXEnabledInHW__ = &kbifIsMSIXEnabledInHW_GH100;
    }

    // kbifIsPciIoAccessEnabled -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_GB202;
    }
    // default
    else
    {
        pThis->__kbifIsPciIoAccessEnabled__ = &kbifIsPciIoAccessEnabled_3dd2c9;
    }

    // kbifIs3dController -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_3dd2c9;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GB100;
    }
    else
    {
        pThis->__kbifIs3dController__ = &kbifIs3dController_GH100;
    }

    // kbifExecC73War -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifExecC73War__ = &kbifExecC73War_GM107;
    }
    else
    {
        pThis->__kbifExecC73War__ = &kbifExecC73War_b3696a;
    }

    // kbifEnableExtendedTagSupport -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_GH100;
    }
    // default
    else
    {
        pThis->__kbifEnableExtendedTagSupport__ = &kbifEnableExtendedTagSupport_b3696a;
    }

    // kbifPcieConfigEnableRelaxedOrdering -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GB100;
    }
    else
    {
        pThis->__kbifPcieConfigEnableRelaxedOrdering__ = &kbifPcieConfigEnableRelaxedOrdering_GH100;
    }

    // kbifPcieConfigDisableRelaxedOrdering -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GB100;
    }
    else
    {
        pThis->__kbifPcieConfigDisableRelaxedOrdering__ = &kbifPcieConfigDisableRelaxedOrdering_GH100;
    }

    // kbifInitRelaxedOrderingFromEmulatedConfigSpace -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100;
        }
        // default
        else
        {
            pThis->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__ = &kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a;
        }
    }

    // kbifEnableNoSnoop -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GB100;
    }
    else
    {
        pThis->__kbifEnableNoSnoop__ = &kbifEnableNoSnoop_GH100;
    }

    // kbifDisableP2PTransactions -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDisableP2PTransactions__ = &kbifDisableP2PTransactions_b3696a;
    }
    else
    {
        pThis->__kbifDisableP2PTransactions__ = &kbifDisableP2PTransactions_TU102;
    }

    // kbifApplyWARBug3208922 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_GA100;
    }
    else
    {
        pThis->__kbifApplyWARBug3208922__ = &kbifApplyWARBug3208922_b3696a;
    }

    // kbifGetVFSparseMmapRegions -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetVFSparseMmapRegions__ = &kbifGetVFSparseMmapRegions_46f6a7;
    }
    else
    {
        pThis->__kbifGetVFSparseMmapRegions__ = &kbifGetVFSparseMmapRegions_TU102;
    }

    // kbifProbePcieReqAtomicCaps -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_GH100;
        }
        // default
        else
        {
            pThis->__kbifProbePcieReqAtomicCaps__ = &kbifProbePcieReqAtomicCaps_b3696a;
        }
    }

    // kbifEnablePcieAtomics -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_GH100;
    }
    // default
    else
    {
        pThis->__kbifEnablePcieAtomics__ = &kbifEnablePcieAtomics_b3696a;
    }

    // kbifProbePcieCplAtomicCaps -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_GB100;
        }
        // default
        else
        {
            pThis->__kbifProbePcieCplAtomicCaps__ = &kbifProbePcieCplAtomicCaps_b3696a;
        }
    }

    // kbifReadPcieCplCapsFromConfigSpace -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c06UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB20B | GB20C */ 
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_GB10B;
    }
    // default
    else
    {
        pThis->__kbifReadPcieCplCapsFromConfigSpace__ = &kbifReadPcieCplCapsFromConfigSpace_b3696a;
    }

    // kbifDoFunctionLevelReset -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoFunctionLevelReset__ = &kbifDoFunctionLevelReset_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifDoFunctionLevelReset__ = &kbifDoFunctionLevelReset_GH100;
    }
    // default
    else
    {
        pThis->__kbifDoFunctionLevelReset__ = &kbifDoFunctionLevelReset_46f6a7;
    }

    // kbifInitXveRegMap -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_AD102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf000f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_GA102;
    }
    // default
    else
    {
        pThis->__kbifInitXveRegMap__ = &kbifInitXveRegMap_46f6a7;
    }

    // kbifGetMSIXTableVectorControlSize -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_b3787c;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_GB100;
    }
    else
    {
        pThis->__kbifGetMSIXTableVectorControlSize__ = &kbifGetMSIXTableVectorControlSize_GH100;
    }

    // kbifConfigAccessWait -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_GH100;
    }
    // default
    else
    {
        pThis->__kbifConfigAccessWait__ = &kbifConfigAccessWait_46f6a7;
    }

    // kbifGetPciConfigSpacePriMirror -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_48d5a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_GH100;
    }
    // default
    else
    {
        pThis->__kbifGetPciConfigSpacePriMirror__ = &kbifGetPciConfigSpacePriMirror_46f6a7;
    }

    // kbifGetBusOptionsAddr -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GB100;
    }
    else
    {
        pThis->__kbifGetBusOptionsAddr__ = &kbifGetBusOptionsAddr_GH100;
    }

    // kbifPreOsGlobalErotGrantRequest -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPreOsGlobalErotGrantRequest__ = &kbifPreOsGlobalErotGrantRequest_AD102;
    }
    // default
    else
    {
        pThis->__kbifPreOsGlobalErotGrantRequest__ = &kbifPreOsGlobalErotGrantRequest_56cd7a;
    }

    // kbifStopSysMemRequests -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GM107;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_GH100;
        }
        // default
        else
        {
            pThis->__kbifStopSysMemRequests__ = &kbifStopSysMemRequests_46f6a7;
        }
    }

    // kbifDisableSysmemAccess -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDisableSysmemAccess__ = &kbifDisableSysmemAccess_46f6a7;
    }
    else
    {
        pThis->__kbifDisableSysmemAccess__ = &kbifDisableSysmemAccess_GM107;
    }

    // kbifWaitForTransactionsComplete -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_GH100;
    }
    // default
    else
    {
        pThis->__kbifWaitForTransactionsComplete__ = &kbifWaitForTransactionsComplete_46f6a7;
    }

    // kbifTriggerFlr -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_GH100;
    }
    // default
    else
    {
        pThis->__kbifTriggerFlr__ = &kbifTriggerFlr_46f6a7;
    }

    // kbifCacheFlrSupport -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_GH100;
        }
        // default
        else
        {
            pThis->__kbifCacheFlrSupport__ = &kbifCacheFlrSupport_b3696a;
        }
    }

    // kbifCache64bBar0Support -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_GH100;
    }
    // default
    else
    {
        pThis->__kbifCache64bBar0Support__ = &kbifCache64bBar0Support_b3696a;
    }

    // kbifCacheMnocSupport -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifCacheMnocSupport__ = &kbifCacheMnocSupport_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kbifCacheMnocSupport__ = &kbifCacheMnocSupport_GB100;
        }
        // default
        else
        {
            pThis->__kbifCacheMnocSupport__ = &kbifCacheMnocSupport_b3696a;
        }
    }

    // kbifCacheVFInfo -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_GH100;
        }
        // default
        else
        {
            pThis->__kbifCacheVFInfo__ = &kbifCacheVFInfo_b3696a;
        }
    }

    // kbifRestoreBar0 -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x91f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_GA100;
    }
    // default
    else
    {
        pThis->__kbifRestoreBar0__ = &kbifRestoreBar0_b3696a;
    }

    // kbifAnyBarsAreValid -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifAnyBarsAreValid__ = &kbifAnyBarsAreValid_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifAnyBarsAreValid__ = &kbifAnyBarsAreValid_GA100;
    }
    // default
    else
    {
        pThis->__kbifAnyBarsAreValid__ = &kbifAnyBarsAreValid_3dd2c9;
    }

    // kbifRestoreBarsAndCommand -- halified (5 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_GH100;
    }
    // default
    else
    {
        pThis->__kbifRestoreBarsAndCommand__ = &kbifRestoreBarsAndCommand_46f6a7;
    }

    // kbifStoreBarRegOffsets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifStoreBarRegOffsets__ = &kbifStoreBarRegOffsets_GA100;
    }
    // default
    else
    {
        pThis->__kbifStoreBarRegOffsets__ = &kbifStoreBarRegOffsets_b3696a;
    }

    // kbifInit -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifInit__ = &kbifInit_46f6a7;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifInit__ = &kbifInit_GM107;
    }
    else
    {
        pThis->__kbifInit__ = &kbifInit_56cd7a;
    }

    // kbifPrepareForFullChipReset -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_b3696a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GA100;
    }
    else
    {
        pThis->__kbifPrepareForFullChipReset__ = &kbifPrepareForFullChipReset_GH100;
    }

    // kbifIsC2CP2PSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kbifIsC2CP2PSupported__ = &kbifIsC2CP2PSupported_GH100;
    }
    // default
    else
    {
        pThis->__kbifIsC2CP2PSupported__ = &kbifIsC2CP2PSupported_3dd2c9;
    }

    // kbifPrepareForXveReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kbifPrepareForXveReset__ = &kbifPrepareForXveReset_GP100;
    }
    // default
    else
    {
        pThis->__kbifPrepareForXveReset__ = &kbifPrepareForXveReset_56cd7a;
    }

    // kbifDoFullChipReset -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GP10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GA100;
    }
    else
    {
        pThis->__kbifDoFullChipReset__ = &kbifDoFullChipReset_GH100;
    }

    // kbifResetHostEngines -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x91f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifResetHostEngines__ = &kbifResetHostEngines_GA100;
    }
    // default
    else
    {
        pThis->__kbifResetHostEngines__ = &kbifResetHostEngines_b3696a;
    }

    // kbifGetValidEnginesToReset -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifGetValidEnginesToReset__ = &kbifGetValidEnginesToReset_15a734;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbifGetValidEnginesToReset__ = &kbifGetValidEnginesToReset_TU102;
    }
    else
    {
        pThis->__kbifGetValidEnginesToReset__ = &kbifGetValidEnginesToReset_GA100;
    }

    // kbifGetValidDeviceEnginesToReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifGetValidDeviceEnginesToReset__ = &kbifGetValidDeviceEnginesToReset_GA100;
    }
    else
    {
        pThis->__kbifGetValidDeviceEnginesToReset__ = &kbifGetValidDeviceEnginesToReset_15a734;
    }

    // kbifGetMigrationBandwidth -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbifGetMigrationBandwidth__ = &kbifGetMigrationBandwidth_GA100;
    }
    else
    {
        pThis->__kbifGetMigrationBandwidth__ = &kbifGetMigrationBandwidth_GM107;
    }

    // kbifGetEccCounts -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbifGetEccCounts__ = &kbifGetEccCounts_GH100;
    }
    // default
    else
    {
        pThis->__kbifGetEccCounts__ = &kbifGetEccCounts_4a4dee;
    }

    // kbifAllowGpuReqPcieAtomics -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c06UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB20B | GB20C */ 
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_88bc07;
    }
    // default
    else
    {
        pThis->__kbifAllowGpuReqPcieAtomics__ = &kbifAllowGpuReqPcieAtomics_3dd2c9;
    }

    // kbifAllowGpuCplPcieAtomics -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kbifAllowGpuCplPcieAtomics__ = &kbifAllowGpuCplPcieAtomics_88bc07;
    }
    // default
    else
    {
        pThis->__kbifAllowGpuCplPcieAtomics__ = &kbifAllowGpuCplPcieAtomics_3dd2c9;
    }

    // kbifClearDownstreamReadCounter -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifClearDownstreamReadCounter__ = &kbifClearDownstreamReadCounter_GA100;
    }
    else
    {
        pThis->__kbifClearDownstreamReadCounter__ = &kbifClearDownstreamReadCounter_b3696a;
    }

    // kbifDoSecondaryBusResetOrFunctionLevelReset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDoSecondaryBusResetOrFunctionLevelReset__ = &kbifDoSecondaryBusResetOrFunctionLevelReset_46f6a7;
    }
    else
    {
        pThis->__kbifDoSecondaryBusResetOrFunctionLevelReset__ = &kbifDoSecondaryBusResetOrFunctionLevelReset_TU102;
    }

    // kbifDoSecondaryBusHotReset -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00005000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kbifDoSecondaryBusHotReset__ = &kbifDoSecondaryBusHotReset_56cd7a;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbifDoSecondaryBusHotReset__ = &kbifDoSecondaryBusHotReset_GM107;
    }
    else
    {
        pThis->__kbifDoSecondaryBusHotReset__ = &kbifDoSecondaryBusHotReset_GH100;
    }
} // End __nvoc_init_funcTable_KernelBif_1 with approximately 203 basic block(s).


// Initialize vtable(s) for 80 virtual method(s).
void __nvoc_init_funcTable_KernelBif(KernelBif *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 67 per-object function pointer(s).
    __nvoc_init_funcTable_KernelBif_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelBif(KernelBif *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelBif = pThis;    // (kbif) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelBif.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelBif.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelBif;    // (kbif) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelBif(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelBif *pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelBif), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelBif));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelBif(pThis, pGpuhalspecowner, pRmhalspecowner);
    status = __nvoc_ctor_KernelBif(pThis, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelBif_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelBif_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelBif));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBif(KernelBif **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBif(ppThis, pParent, createFlags);

    return status;
}

