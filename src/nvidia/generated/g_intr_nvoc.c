#define NVOC_INTR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_intr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xc06e44 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Intr;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for Intr
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__Intr(Intr*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_Intr(Intr*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_Intr(Intr*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_Intr(Intr*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_Intr(Intr*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Intr;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Intr;

// Down-thunk(s) to bridge Intr methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_Intr_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_Intr_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr);    // this
NV_STATUS __nvoc_down_thunk_Intr_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr);    // this
NV_STATUS __nvoc_down_thunk_Intr_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr);    // this
void __nvoc_down_thunk_Intr_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pIntr);    // this
NV_STATUS __nvoc_down_thunk_Intr_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_Intr_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg3);    // this

// Up-thunk(s) to bridge Intr methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_intrInitMissing(struct OBJGPU *pGpu, struct Intr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreInitUnlocked(struct OBJGPU *pGpu, struct Intr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreLoad(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePostLoad(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreUnload(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePostUnload(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_intrIsPresent(struct OBJGPU *pGpu, struct Intr *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_Intr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Intr),
        /*classId=*/            classId(Intr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Intr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Intr,
    /*pCastInfo=*/          &__nvoc_castinfo__Intr,
    /*pExportInfo=*/        &__nvoc_export_info__Intr
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__Intr __nvoc_metadata__Intr = {
    .rtti.pClassDef = &__nvoc_class_def_Intr,    // (intr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Intr,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__intrConstructEngine__ = &intrConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_Intr_engstateConstructEngine,    // virtual
    .vtable.__intrStatePreInitLocked__ = &intrStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_Intr_engstateStatePreInitLocked,    // virtual
    .vtable.__intrStateInitUnlocked__ = &intrStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_Intr_engstateStateInitUnlocked,    // virtual
    .vtable.__intrStateInitLocked__ = &intrStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_Intr_engstateStateInitLocked,    // virtual
    .vtable.__intrStateDestroy__ = &intrStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_Intr_engstateStateDestroy,    // virtual
    .vtable.__intrStateLoad__ = &intrStateLoad_TU102,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_Intr_engstateStateLoad,    // virtual
    .vtable.__intrStateUnload__ = &intrStateUnload_TU102,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_Intr_engstateStateUnload,    // virtual
    .vtable.__intrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_intrInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__intrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_intrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__intrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_intrStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__intrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_intrStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__intrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_intrStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__intrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_intrStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__intrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_intrIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Intr = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__Intr.rtti,    // [0]: (intr) this
        &__nvoc_metadata__Intr.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__Intr.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 7 down-thunk(s) defined to bridge methods in Intr from superclasses

// intrConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_Intr_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, ENGDESCRIPTOR arg3) {
    return intrConstructEngine(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_Intr_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    return intrStatePreInitLocked(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_Intr_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    return intrStateInitUnlocked(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_Intr_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    return intrStateInitLocked(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_Intr_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pIntr) {
    intrStateDestroy(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_Intr_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg3) {
    return intrStateLoad(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_Intr_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pIntr, NvU32 arg3) {
    return intrStateUnload(pGpu, (struct Intr *)(((unsigned char *) pIntr) - NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}


// 7 up-thunk(s) defined to bridge methods in Intr to superclasses

// intrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_intrInitMissing(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreInitUnlocked(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}

// intrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreLoad(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePostLoad(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePreUnload(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_intrStatePostUnload(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)), arg3);
}

// intrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_intrIsPresent(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Intr, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__Intr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_Intr(Intr *pThis) {
    __nvoc_intrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_INTR_IS_MISSING
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_USE_TOP_EN_FOR_VBLANK_HANDLING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_USE_TOP_EN_FOR_VBLANK_HANDLING, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_INTR_MASK_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_INTR_MASK_SUPPORTED, NV_TRUE);
    }

    // Hal field -- bDefaultNonstallNotify
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bDefaultNonstallNotify = NV_TRUE;
    }

    // Hal field -- bUseLegacyVectorAssignment
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bUseLegacyVectorAssignment = NV_FALSE;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->bUseLegacyVectorAssignment = NV_FALSE;
    }
    // default
    else
    {
        pThis->bUseLegacyVectorAssignment = NV_TRUE;
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_Intr_fail_OBJENGSTATE;
    __nvoc_init_dataField_Intr(pThis, pRmhalspecowner);
    goto __nvoc_ctor_Intr_exit; // Success

__nvoc_ctor_Intr_fail_OBJENGSTATE:
__nvoc_ctor_Intr_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Intr_1(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // intrDecodeStallIntrEn -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrDecodeStallIntrEn__ = &intrDecodeStallIntrEn_TU102;
    }
    else
    {
        pThis->__intrDecodeStallIntrEn__ = &intrDecodeStallIntrEn_4a4dee;
    }

    // intrServiceVirtual -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__intrServiceVirtual__ = &intrServiceVirtual_f2d351;
    }
    else
    {
        pThis->__intrServiceVirtual__ = &intrServiceVirtual_TU102;
    }

    // intrTriggerPrivDoorbell -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__intrTriggerPrivDoorbell__ = &intrTriggerPrivDoorbell_5baef9;
    }
    else
    {
        pThis->__intrTriggerPrivDoorbell__ = &intrTriggerPrivDoorbell_TU102;
    }

    // intrGetLocklessVectorsInRmSubtree -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrGetLocklessVectorsInRmSubtree__ = &intrGetLocklessVectorsInRmSubtree_TU102;
    }
    else
    {
        pThis->__intrGetLocklessVectorsInRmSubtree__ = &intrGetLocklessVectorsInRmSubtree_GA100;
    }

    // intrGetPendingLowLatencyHwDisplayIntr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__intrGetPendingLowLatencyHwDisplayIntr__ = &intrGetPendingLowLatencyHwDisplayIntr_GB202;
    }
    else
    {
        pThis->__intrGetPendingLowLatencyHwDisplayIntr__ = &intrGetPendingLowLatencyHwDisplayIntr_TU102;
    }

    // intrSetDisplayInterruptEnable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__intrSetDisplayInterruptEnable__ = &intrSetDisplayInterruptEnable_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x81f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB10B */ 
    {
        pThis->__intrSetDisplayInterruptEnable__ = &intrSetDisplayInterruptEnable_TU102;
    }
    // default
    else
    {
        pThis->__intrSetDisplayInterruptEnable__ = &intrSetDisplayInterruptEnable_b3696a;
    }

    // intrCacheDispIntrVectors -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__intrCacheDispIntrVectors__ = &intrCacheDispIntrVectors_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x81f0fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GB10B */ 
    {
        pThis->__intrCacheDispIntrVectors__ = &intrCacheDispIntrVectors_TU102;
    }
    // default
    else
    {
        pThis->__intrCacheDispIntrVectors__ = &intrCacheDispIntrVectors_b3696a;
    }

    // intrReadRegTopEnSet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrReadRegTopEnSet__ = &intrReadRegTopEnSet_CPU_TU102;
    }
    else
    {
        pThis->__intrReadRegTopEnSet__ = &intrReadRegTopEnSet_CPU_GA102;
    }

    // intrWriteRegTopEnSet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrWriteRegTopEnSet__ = &intrWriteRegTopEnSet_CPU_TU102;
    }
    else
    {
        pThis->__intrWriteRegTopEnSet__ = &intrWriteRegTopEnSet_CPU_GA102;
    }

    // intrWriteRegTopEnClear -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__intrWriteRegTopEnClear__ = &intrWriteRegTopEnClear_CPU_TU102;
    }
    else
    {
        pThis->__intrWriteRegTopEnClear__ = &intrWriteRegTopEnClear_CPU_GA102;
    }

    // intrGetNumLeaves -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrGetNumLeaves__ = &intrGetNumLeaves_TU102;
    }
    else
    {
        pThis->__intrGetNumLeaves__ = &intrGetNumLeaves_GH100;
    }

    // intrGetLeafSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrGetLeafSize__ = &intrGetLeafSize_TU102;
    }
    else
    {
        pThis->__intrGetLeafSize__ = &intrGetLeafSize_GH100;
    }

    // intrGetIntrTopNonStallMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrGetIntrTopNonStallMask__ = &intrGetIntrTopNonStallMask_TU102;
    }
    else
    {
        pThis->__intrGetIntrTopNonStallMask__ = &intrGetIntrTopNonStallMask_GH100;
    }

    // intrSanityCheckEngineIntrStallVector -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_GH100;
    }
    // default
    else
    {
        pThis->__intrSanityCheckEngineIntrStallVector__ = &intrSanityCheckEngineIntrStallVector_b3696a;
    }

    // intrSanityCheckEngineIntrNotificationVector -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_GH100;
    }
    // default
    else
    {
        pThis->__intrSanityCheckEngineIntrNotificationVector__ = &intrSanityCheckEngineIntrNotificationVector_b3696a;
    }

    // intrInitSubtreeMap -- halified (3 hals) body
    if (((( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */  && (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ ))
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__intrInitSubtreeMap__ = &intrInitSubtreeMap_TU102;
        }
        else
        {
            pThis->__intrInitSubtreeMap__ = &intrInitSubtreeMap_GH100;
        }
    }
    else
    {
        pThis->__intrInitSubtreeMap__ = &intrInitSubtreeMap_395e98;
    }

    // intrInitInterruptTable -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__intrInitInterruptTable__ = &intrInitInterruptTable_VF;
    }
    else
    {
        pThis->__intrInitInterruptTable__ = &intrInitInterruptTable_KERNEL;
    }

    // intrSetIntrMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrSetIntrMask__ = &intrSetIntrMask_GP100;
    }
    else
    {
        pThis->__intrSetIntrMask__ = &intrSetIntrMask_46f6a7;
    }

    // intrSetIntrEnInHw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrSetIntrEnInHw__ = &intrSetIntrEnInHw_GP100;
    }
    else
    {
        pThis->__intrSetIntrEnInHw__ = &intrSetIntrEnInHw_d44104;
    }

    // intrGetIntrEnFromHw -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__intrGetIntrEnFromHw__ = &intrGetIntrEnFromHw_GP100;
    }
    else
    {
        pThis->__intrGetIntrEnFromHw__ = &intrGetIntrEnFromHw_b2b553;
    }
} // End __nvoc_init_funcTable_Intr_1 with approximately 45 basic block(s).


// Initialize vtable(s) for 34 virtual method(s).
void __nvoc_init_funcTable_Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 20 per-object function pointer(s).
    __nvoc_init_funcTable_Intr_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__Intr(Intr *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_Intr = pThis;    // (intr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Intr.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__Intr.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Intr;    // (intr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Intr(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Intr(Intr **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Intr *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Intr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Intr));

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

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__Intr(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Intr(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_Intr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Intr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Intr));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Intr(Intr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Intr(ppThis, pParent, createFlags);

    return status;
}

