#define NVOC_KERN_MEM_SYS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_mem_sys_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x7faff1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelMemorySystem
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelMemorySystem(KernelMemorySystem*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelMemorySystem(KernelMemorySystem*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelMemorySystem(KernelMemorySystem*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelMemorySystem(KernelMemorySystem*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelMemorySystem(KernelMemorySystem*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelMemorySystem;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelMemorySystem;

// Down-thunk(s) to bridge KernelMemorySystem methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags);    // this
void __nvoc_down_thunk_KernelMemorySystem_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem);    // this

// Up-thunk(s) to bridge KernelMemorySystem methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kmemsysInitMissing(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePostUnload(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kmemsysIsPresent(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelMemorySystem),
        /*classId=*/            classId(KernelMemorySystem),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelMemorySystem",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMemorySystem,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelMemorySystem,
    /*pExportInfo=*/        &__nvoc_export_info__KernelMemorySystem
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelMemorySystem __nvoc_metadata__KernelMemorySystem = {
    .rtti.pClassDef = &__nvoc_class_def_KernelMemorySystem,    // (kmemsys) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMemorySystem,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kmemsysConstructEngine__ = &kmemsysConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelMemorySystem_engstateConstructEngine,    // virtual
    .vtable.__kmemsysStatePreInitLocked__ = &kmemsysStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreInitLocked,    // virtual
    .vtable.__kmemsysStateInitLocked__ = &kmemsysStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateInitLocked,    // virtual
    .vtable.__kmemsysStatePreLoad__ = &kmemsysStatePreLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreLoad,    // virtual
    .vtable.__kmemsysStatePostLoad__ = &kmemsysStatePostLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePostLoad,    // virtual
    .vtable.__kmemsysStateLoad__ = &kmemsysStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateLoad,    // virtual
    .vtable.__kmemsysStatePreUnload__ = &kmemsysStatePreUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreUnload,    // virtual
    .vtable.__kmemsysStateUnload__ = &kmemsysStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateUnload,    // virtual
    .vtable.__kmemsysStateDestroy__ = &kmemsysStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateDestroy,    // virtual
    .vtable.__kmemsysInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kmemsysStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kmemsysStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kmemsysStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kmemsysIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelMemorySystem = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelMemorySystem.rtti,    // [0]: (kmemsys) this
        &__nvoc_metadata__KernelMemorySystem.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelMemorySystem.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 9 down-thunk(s) defined to bridge methods in KernelMemorySystem from superclasses

// kmemsysConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, ENGDESCRIPTOR arg3) {
    return kmemsysConstructEngine(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmemsysStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    return kmemsysStatePreInitLocked(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}

// kmemsysStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    return kmemsysStateInitLocked(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}

// kmemsysStatePreLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePreLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), flags);
}

// kmemsysStatePostLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePostLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), flags);
}

// kmemsysStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStateLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), flags);
}

// kmemsysStatePreUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePreUnload(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), flags);
}

// kmemsysStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStateUnload(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), flags);
}

// kmemsysStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelMemorySystem_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    kmemsysStateDestroy(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}


// 5 up-thunk(s) defined to bridge methods in KernelMemorySystem to superclasses

// kmemsysInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kmemsysInitMissing(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}

// kmemsysStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}

// kmemsysStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}

// kmemsysStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePostUnload(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)), arg3);
}

// kmemsysIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kmemsysIsPresent(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelMemorySystem = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_kmemsysDestruct(KernelMemorySystem*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMemorySystem(KernelMemorySystem *pThis) {
    __nvoc_kmemsysDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelMemorySystem(KernelMemorySystem *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KMEMSYS_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KMEMSYS_IS_MISSING, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KMEMSYS_IS_MISSING, NV_FALSE);
    }

    // Hal field -- bDisableTiledCachingInvalidatesWithEccBug1521641
    // default
    {
        pThis->bDisableTiledCachingInvalidatesWithEccBug1521641 = NV_FALSE;
    }

    // Hal field -- bGpuCacheEnable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->bGpuCacheEnable = NV_TRUE;
    }

    // Hal field -- bNumaNodesAdded
    pThis->bNumaNodesAdded = NV_FALSE;

    // Hal field -- bL2CleanFbPull
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->bL2CleanFbPull = NV_TRUE;
    }
    // default
    else
    {
        pThis->bL2CleanFbPull = NV_FALSE;
    }

    // Hal field -- l2WriteMode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x71f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->l2WriteMode = FB_CACHE_WRITE_MODE_WRITETHROUGH;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: GB10B | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->l2WriteMode = FB_CACHE_WRITE_MODE_WRITEBACK;
    }

    // Hal field -- bPreserveComptagBackingStoreOnSuspend
    pThis->bPreserveComptagBackingStoreOnSuspend = NV_FALSE;

    // Hal field -- bBug3656943WAR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: GH100 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bBug3656943WAR = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBug3656943WAR = NV_FALSE;
    }

    // Hal field -- overrideToGMK
    pThis->overrideToGMK = 0;

    // Hal field -- bDisablePlcForCertainOffsetsBug3046774
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->bDisablePlcForCertainOffsetsBug3046774 = NV_FALSE;
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->bDisablePlcForCertainOffsetsBug3046774 = NV_FALSE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelMemorySystem(KernelMemorySystem *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMemorySystem_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelMemorySystem(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelMemorySystem_exit; // Success

__nvoc_ctor_KernelMemorySystem_fail_OBJENGSTATE:
__nvoc_ctor_KernelMemorySystem_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelMemorySystem_1(KernelMemorySystem *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // kmemsysGetFbNumaInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x71f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_GV100;
    }
    else
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_56cd7a;
    }

    // kmemsysReadUsableFbSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GP102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0f800UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GA102;
    }
    // default
    else
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_5baef9;
    }

    // kmemsysGetUsableFbSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetUsableFbSize__ = &kmemsysGetUsableFbSize_5baef9;
    }
    else
    {
        pThis->__kmemsysGetUsableFbSize__ = &kmemsysGetUsableFbSize_KERNEL;
    }

    // kmemsysCacheOp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysCacheOp__ = &kmemsysCacheOp_GH100;
    }
    else
    {
        pThis->__kmemsysCacheOp__ = &kmemsysCacheOp_GM200;
    }

    // kmemsysDoCacheOp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysDoCacheOp__ = &kmemsysDoCacheOp_GH100;
    }
    else
    {
        pThis->__kmemsysDoCacheOp__ = &kmemsysDoCacheOp_GM107;
    }

    // kmemsysReadL2SysmemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysReadL2SysmemInvalidateReg__ = &kmemsysReadL2SysmemInvalidateReg_68b109;
    }
    else
    {
        pThis->__kmemsysReadL2SysmemInvalidateReg__ = &kmemsysReadL2SysmemInvalidateReg_TU102;
    }

    // kmemsysWriteL2SysmemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysWriteL2SysmemInvalidateReg__ = &kmemsysWriteL2SysmemInvalidateReg_f2d351;
    }
    else
    {
        pThis->__kmemsysWriteL2SysmemInvalidateReg__ = &kmemsysWriteL2SysmemInvalidateReg_TU102;
    }

    // kmemsysReadL2PeermemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysReadL2PeermemInvalidateReg__ = &kmemsysReadL2PeermemInvalidateReg_68b109;
    }
    else
    {
        pThis->__kmemsysReadL2PeermemInvalidateReg__ = &kmemsysReadL2PeermemInvalidateReg_TU102;
    }

    // kmemsysWriteL2PeermemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysWriteL2PeermemInvalidateReg__ = &kmemsysWriteL2PeermemInvalidateReg_f2d351;
    }
    else
    {
        pThis->__kmemsysWriteL2PeermemInvalidateReg__ = &kmemsysWriteL2PeermemInvalidateReg_TU102;
    }

    // kmemsysInitHshub0Aperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysInitHshub0Aperture__ = &kmemsysInitHshub0Aperture_GB100;
    }
    // default
    else
    {
        pThis->__kmemsysInitHshub0Aperture__ = &kmemsysInitHshub0Aperture_fa6e19;
    }

    // kmemsysDestroyHshub0Aperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysDestroyHshub0Aperture__ = &kmemsysDestroyHshub0Aperture_GB100;
    }
    // default
    else
    {
        pThis->__kmemsysDestroyHshub0Aperture__ = &kmemsysDestroyHshub0Aperture_b3696a;
    }

    // kmemsysInitFlushSysmemBuffer -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_56cd7a;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GM107;
        }
        else
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GA100;
        }
    }

    // kmemsysProgramSysmemFlushBuffer -- halified (7 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_b3696a;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GM107;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GB100;
        }
        else
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GB10B;
        }
    }

    // kmemsysGetFlushSysmemBufferAddrShift -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_4a4dee;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_4a4dee;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_GM107;
        }
        else
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_GB10B;
        }
    }

    // kmemsysIsPagePLCable -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_88bc07;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_GA102;
        }
        // default
        else
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_84161d;
        }
    }

    // kmemsysReadMIGMemoryCfg -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c06UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB20B | GB20C */ 
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_GA100;
        }
        // default
        else
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_46f6a7;
        }
    }

    // kmemsysInitMIGMemoryPartitionTable -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_GA100;
        }
        // default
        else
        {
            pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_56cd7a;
        }
    }

    // kmemsysSwizzIdToVmmuSegmentsRange -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysSwizzIdToVmmuSegmentsRange__ = &kmemsysSwizzIdToVmmuSegmentsRange_GH100;
    }
    else
    {
        pThis->__kmemsysSwizzIdToVmmuSegmentsRange__ = &kmemsysSwizzIdToVmmuSegmentsRange_GA100;
    }

    // kmemsysNumaAddMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysNumaAddMemory__ = &kmemsysNumaAddMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaAddMemory__ = &kmemsysNumaAddMemory_56cd7a;
    }

    // kmemsysNumaRemoveMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysNumaRemoveMemory__ = &kmemsysNumaRemoveMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaRemoveMemory__ = &kmemsysNumaRemoveMemory_b3696a;
    }

    // kmemsysNumaRemoveAllMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysNumaRemoveAllMemory__ = &kmemsysNumaRemoveAllMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaRemoveAllMemory__ = &kmemsysNumaRemoveAllMemory_b3696a;
    }

    // kmemsysPopulateMIGGPUInstanceMemConfig -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysPopulateMIGGPUInstanceMemConfig__ = &kmemsysPopulateMIGGPUInstanceMemConfig_56cd7a;
    }
    else
    {
        // default
        pThis->__kmemsysPopulateMIGGPUInstanceMemConfig__ = &kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL;
    }

    // kmemsysNeedInvalidateGpuCacheOnMap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysNeedInvalidateGpuCacheOnMap__ = &kmemsysNeedInvalidateGpuCacheOnMap_GV100;
    }
    // default
    else
    {
        pThis->__kmemsysNeedInvalidateGpuCacheOnMap__ = &kmemsysNeedInvalidateGpuCacheOnMap_3dd2c9;
    }

    // kmemsysNeedInvalidateGpuCacheOnUnmap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C | T234D | T264D */ 
    {
        pThis->__kmemsysNeedInvalidateGpuCacheOnUnmap__ = &kmemsysNeedInvalidateGpuCacheOnUnmap_T194;
    }
    // default
    else
    {
        pThis->__kmemsysNeedInvalidateGpuCacheOnUnmap__ = &kmemsysNeedInvalidateGpuCacheOnUnmap_3dd2c9;
    }

    // kmemsysSetupAllAtsPeers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kmemsysSetupAllAtsPeers__ = &kmemsysSetupAllAtsPeers_46f6a7;
    }
    else
    {
        pThis->__kmemsysSetupAllAtsPeers__ = &kmemsysSetupAllAtsPeers_GV100;
    }

    // kmemsysRemoveAllAtsPeers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | T234D | T264D */ 
    {
        pThis->__kmemsysRemoveAllAtsPeers__ = &kmemsysRemoveAllAtsPeers_b3696a;
    }
    else
    {
        pThis->__kmemsysRemoveAllAtsPeers__ = &kmemsysRemoveAllAtsPeers_GV100;
    }

    // kmemsysAssertFbAckTimeoutPending -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x90000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe0UL) )) /* ChipHal: GH100 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_3dd2c9;
    }

    // kmemsysGetMaxFbpas -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_TU102;
    }
    // default
    else
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_4a4dee;
    }

    // kmemsysGetEccDedCountSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kmemsysGetEccDedCountSize__ = &kmemsysGetEccDedCountSize_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kmemsysGetEccDedCountSize__ = &kmemsysGetEccDedCountSize_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysGetEccDedCountSize__ = &kmemsysGetEccDedCountSize_4a4dee;
    }

    // kmemsysGetEccDedCountRegAddr -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kmemsysGetEccDedCountRegAddr__ = &kmemsysGetEccDedCountRegAddr_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kmemsysGetEccDedCountRegAddr__ = &kmemsysGetEccDedCountRegAddr_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysGetEccDedCountRegAddr__ = &kmemsysGetEccDedCountRegAddr_4a4dee;
    }

    // kmemsysGetEccCounts -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysGetEccCounts__ = &kmemsysGetEccCounts_TU102;
    }
    // default
    else
    {
        pThis->__kmemsysGetEccCounts__ = &kmemsysGetEccCounts_b3696a;
    }

    // kmemsysGetL2EccDedCountRegAddr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysGetL2EccDedCountRegAddr__ = &kmemsysGetL2EccDedCountRegAddr_GB100;
    }
    else
    {
        pThis->__kmemsysGetL2EccDedCountRegAddr__ = &kmemsysGetL2EccDedCountRegAddr_TU102;
    }

    // kmemsysGetMaximumBlacklistPages -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 3UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000a000UL) )) /* ChipHal: T234D | T264D */ 
    {
        pThis->__kmemsysGetMaximumBlacklistPages__ = &kmemsysGetMaximumBlacklistPages_4a4dee;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetMaximumBlacklistPages__ = &kmemsysGetMaximumBlacklistPages_GM107;
    }
    else
    {
        pThis->__kmemsysGetMaximumBlacklistPages__ = &kmemsysGetMaximumBlacklistPages_GA100;
    }

    // kmemsysIsSwizzIdRejectedByHW -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysIsSwizzIdRejectedByHW__ = &kmemsysIsSwizzIdRejectedByHW_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysIsSwizzIdRejectedByHW__ = &kmemsysIsSwizzIdRejectedByHW_3dd2c9;
    }

    // kmemsysGetFbInfos -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetFbInfos__ = &kmemsysGetFbInfos_VF;
    }
    else
    {
        pThis->__kmemsysGetFbInfos__ = &kmemsysGetFbInfos_ac1694;
    }

    // kmemsysCheckReadoutEccEnablement -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysCheckReadoutEccEnablement__ = &kmemsysCheckReadoutEccEnablement_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kmemsysCheckReadoutEccEnablement__ = &kmemsysCheckReadoutEccEnablement_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000fe6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kmemsysCheckReadoutEccEnablement__ = &kmemsysCheckReadoutEccEnablement_GB100;
    }
    // default
    else
    {
        pThis->__kmemsysCheckReadoutEccEnablement__ = &kmemsysCheckReadoutEccEnablement_3dd2c9;
    }

    // kmemsysIsNonPasidAtsSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000c00UL) )) /* ChipHal: GB10B | GB20B | GB20C */ 
    {
        pThis->__kmemsysIsNonPasidAtsSupported__ = &kmemsysIsNonPasidAtsSupported_88bc07;
    }
    // default
    else
    {
        pThis->__kmemsysIsNonPasidAtsSupported__ = &kmemsysIsNonPasidAtsSupported_3dd2c9;
    }
} // End __nvoc_init_funcTable_KernelMemorySystem_1 with approximately 96 basic block(s).


// Initialize vtable(s) for 51 virtual method(s).
void __nvoc_init_funcTable_KernelMemorySystem(KernelMemorySystem *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 37 per-object function pointer(s).
    __nvoc_init_funcTable_KernelMemorySystem_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelMemorySystem(KernelMemorySystem *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelMemorySystem = pThis;    // (kmemsys) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelMemorySystem.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelMemorySystem.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelMemorySystem;    // (kmemsys) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelMemorySystem(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelMemorySystem *pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(KernelMemorySystem));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelMemorySystem));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMemorySystem_cleanup);

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

    // HALs are defined by the parent or the first super class.
    if ((pGpuhalspecowner = dynamicCast(pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMemorySystem_cleanup);
    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_CHECK_TRUE_OR_GOTO(status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelMemorySystem_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelMemorySystem(pThis, pGpuhalspecowner, pRmhalspecowner);
    status = __nvoc_ctor_KernelMemorySystem(pThis, pGpuhalspecowner, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMemorySystem_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelMemorySystem_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelMemorySystem));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMemorySystem(ppThis, pParent, createFlags);

    return status;
}

