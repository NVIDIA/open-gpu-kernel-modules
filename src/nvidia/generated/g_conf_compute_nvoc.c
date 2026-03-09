#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_conf_compute_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__9798cc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for ConfidentialCompute
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_ConfidentialCompute(ConfidentialCompute*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_ConfidentialCompute(ConfidentialCompute*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ConfidentialCompute;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ConfidentialCompute;

// Down-thunk(s) to bridge ConfidentialCompute methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute);    // this
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute);    // this
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags);    // this

// Up-thunk(s) to bridge ConfidentialCompute methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_confComputeInitMissing(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePostUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_confComputeStateDestroy(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_confComputeIsPresent(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate);    // this

// Class-specific details for ConfidentialCompute
const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialCompute = 
{
    .classInfo.size =               sizeof(ConfidentialCompute),
    .classInfo.classId =            classId(ConfidentialCompute),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ConfidentialCompute",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_ConfidentialCompute,
    .pCastInfo =          &__nvoc_castinfo__ConfidentialCompute,
    .pExportInfo =        &__nvoc_export_info__ConfidentialCompute
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__ConfidentialCompute __nvoc_metadata__ConfidentialCompute = {
    .rtti.pClassDef = &__nvoc_class_def_ConfidentialCompute,    // (confCompute) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_ConfidentialCompute,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__confComputeConstructEngine__ = &confComputeConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_ConfidentialCompute_engstateConstructEngine,    // virtual
    .vtable.__confComputeStatePreInitLocked__ = &confComputeStatePreInitLocked_ac1694,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePreInitLocked,    // virtual
    .vtable.__confComputeStateInitLocked__ = &confComputeStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStateInitLocked,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePostLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_ConfidentialCompute_engstateStatePreUnload,    // virtual
    .vtable.__confComputeInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__confComputeStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__confComputeStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__confComputeStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__confComputeStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__confComputeStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__confComputeStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__confComputeStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__confComputeIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_confComputeIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ConfidentialCompute = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__ConfidentialCompute.rtti,    // [0]: (confCompute) this
        &__nvoc_metadata__ConfidentialCompute.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__ConfidentialCompute.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 5 down-thunk(s) defined to bridge methods in ConfidentialCompute from superclasses

// confComputeConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, ENGDESCRIPTOR engDesc) {
    return confComputeConstructEngine(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), engDesc);
}

// confComputeStatePreInitLocked: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute) {
    return confComputeStatePreInitLocked(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute) {
    return confComputeStateInitLocked(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags) {
    return confComputeStatePostLoad(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), flags);
}

// confComputeStatePreUnload: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_ConfidentialCompute_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pConfCompute, NvU32 flags) {
    return confComputeStatePreUnload(pGpu, (struct ConfidentialCompute *)(((unsigned char *) pConfCompute) - NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), flags);
}


// 9 up-thunk(s) defined to bridge methods in ConfidentialCompute to superclasses

// confComputeInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_confComputeInitMissing(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateInitUnlocked(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePreLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), arg3);
}

// confComputeStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateLoad(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), arg3);
}

// confComputeStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStateUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), arg3);
}

// confComputeStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_confComputeStatePostUnload(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)), arg3);
}

// confComputeStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_confComputeStateDestroy(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}

// confComputeIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_confComputeIsPresent(struct OBJGPU *pGpu, struct ConfidentialCompute *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(ConfidentialCompute, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__ConfidentialCompute = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ConfidentialCompute object.
void __nvoc_confComputeDestruct(ConfidentialCompute*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_ConfidentialCompute(ConfidentialCompute* pThis) {

// Call destructor.
    __nvoc_confComputeDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_CONFCOMPUTE_IS_MISSING
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_IS_MISSING, NV_TRUE);
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_IS_MISSING, NV_FALSE);
    }
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_ENABLE_EARLY_INIT, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_GPUS_READY_CHECK_ENABLED, NV_TRUE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_SUPPORTED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_KEY_ROTATION_ENABLED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_INTERNAL_KEY_ROTATION_ENABLED, NV_FALSE);

    // NVOC Property Hal field -- PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x18000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_CONFCOMPUTE_WAR_5107790_SYSMEM_FLUSH_ADDR, NV_FALSE);
    }
}


// Construct ConfidentialCompute object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_ConfidentialCompute_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_ConfidentialCompute(pThis, pRmhalspecowner, pGpuhalspecowner);
    goto __nvoc_ctor_ConfidentialCompute_exit; // Success

    // Unwind on error.
__nvoc_ctor_ConfidentialCompute_fail_OBJENGSTATE:
__nvoc_ctor_ConfidentialCompute_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ConfidentialCompute_1(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // confComputeDestruct -- halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeDestruct__ = &confComputeDestruct_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeDestruct__ = &confComputeDestruct_d44104;
    }

    // confComputeStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeStatePostLoad__ = &confComputeStatePostLoad_IMPL;
    }
    // default
    else
    {
        pThis->__confComputeStatePostLoad__ = &confComputeStatePostLoad_ac1694;
    }

    // confComputeStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeStatePreUnload__ = &confComputeStatePreUnload_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeStatePreUnload__ = &confComputeStatePreUnload_ac1694;
    }

    // confComputeSetErrorState -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeSetErrorState__ = &confComputeSetErrorState_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeSetErrorState__ = &confComputeSetErrorState_d44104;
    }

    // confComputeKeyStoreDeriveViaChannel -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeKeyStoreDeriveViaChannel__ = &confComputeKeyStoreDeriveViaChannel_ac1694;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreDeriveViaChannel__ = &confComputeKeyStoreDeriveViaChannel_GB100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDeriveViaChannel__ = &confComputeKeyStoreDeriveViaChannel_395e98;
    }

    // confComputeKeyStoreRetrieveViaChannel -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeKeyStoreRetrieveViaChannel__ = &confComputeKeyStoreRetrieveViaChannel_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreRetrieveViaChannel__ = &confComputeKeyStoreRetrieveViaChannel_GB100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreRetrieveViaChannel__ = &confComputeKeyStoreRetrieveViaChannel_395e98;
    }

    // confComputeKeyStoreRetrieveViaKeyId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreRetrieveViaKeyId__ = &confComputeKeyStoreRetrieveViaKeyId_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreRetrieveViaKeyId__ = &confComputeKeyStoreRetrieveViaKeyId_395e98;
    }

    // confComputeDeriveSecretsForCEKeySpace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f8UL) )) /* ChipHal: GH100 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeDeriveSecretsForCEKeySpace__ = &confComputeDeriveSecretsForCEKeySpace_GH100;
    }
    // default
    else
    {
        pThis->__confComputeDeriveSecretsForCEKeySpace__ = &confComputeDeriveSecretsForCEKeySpace_395e98;
    }

    // confComputeDeriveInitialKeySeed -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeDeriveInitialKeySeed__ = &confComputeDeriveInitialKeySeed_ac1694;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeDeriveInitialKeySeed__ = &confComputeDeriveInitialKeySeed_GB100;
    }
    // default
    else
    {
        pThis->__confComputeDeriveInitialKeySeed__ = &confComputeDeriveInitialKeySeed_395e98;
    }

    // confComputeGetAndUpdateCurrentKeySeed -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetAndUpdateCurrentKeySeed__ = &confComputeGetAndUpdateCurrentKeySeed_GB100;
    }
    // default
    else
    {
        pThis->__confComputeGetAndUpdateCurrentKeySeed__ = &confComputeGetAndUpdateCurrentKeySeed_395e98;
    }

    // confComputeDeriveSecrets -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeDeriveSecrets__ = &confComputeDeriveSecrets_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeDeriveSecrets__ = &confComputeDeriveSecrets_GB100;
    }
    // default
    else
    {
        pThis->__confComputeDeriveSecrets__ = &confComputeDeriveSecrets_395e98;
    }

    // confComputeRotationNotifHandler -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeRotationNotifHandler__ = &confComputeRotationNotifHandler_GB100;
    }
    // default
    else
    {
        pThis->__confComputeRotationNotifHandler__ = &confComputeRotationNotifHandler_GH100;
    }

    // confComputeUpdateSecrets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeUpdateSecrets__ = &confComputeUpdateSecrets_GH100;
    }
    // default
    else
    {
        pThis->__confComputeUpdateSecrets__ = &confComputeUpdateSecrets_395e98;
    }

    // confComputeUpdatePerChannelSecrets -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeUpdatePerChannelSecrets__ = &confComputeUpdatePerChannelSecrets_GB100;
    }
    // default
    else
    {
        pThis->__confComputeUpdatePerChannelSecrets__ = &confComputeUpdatePerChannelSecrets_395e98;
    }

    // confComputeIsSpdmEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeIsSpdmEnabled__ = &confComputeIsSpdmEnabled_e661f0;
    }
    // default
    else
    {
        pThis->__confComputeIsSpdmEnabled__ = &confComputeIsSpdmEnabled_d69453;
    }

    // confComputeGetEngineIdFromKeySpace -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeGetEngineIdFromKeySpace__ = &confComputeGetEngineIdFromKeySpace_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetEngineIdFromKeySpace__ = &confComputeGetEngineIdFromKeySpace_GB100;
    }
    // default
    else
    {
        pThis->__confComputeGetEngineIdFromKeySpace__ = &confComputeGetEngineIdFromKeySpace_4f6cff;
    }

    // confComputeGetKeySpaceFromKChannel -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeGetKeySpaceFromKChannel__ = &confComputeGetKeySpaceFromKChannel_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetKeySpaceFromKChannel__ = &confComputeGetKeySpaceFromKChannel_GB100;
    }
    // default
    else
    {
        pThis->__confComputeGetKeySpaceFromKChannel__ = &confComputeGetKeySpaceFromKChannel_395e98;
    }

    // confComputeGetLceKeyIdFromKChannel -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeGetLceKeyIdFromKChannel__ = &confComputeGetLceKeyIdFromKChannel_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetLceKeyIdFromKChannel__ = &confComputeGetLceKeyIdFromKChannel_GB100;
    }
    // default
    else
    {
        pThis->__confComputeGetLceKeyIdFromKChannel__ = &confComputeGetLceKeyIdFromKChannel_395e98;
    }

    // confComputeGetMaxCeKeySpaceIdx -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeGetMaxCeKeySpaceIdx__ = &confComputeGetMaxCeKeySpaceIdx_a3eb5b;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetMaxCeKeySpaceIdx__ = &confComputeGetMaxCeKeySpaceIdx_ca9aa5;
    }
    // default
    else
    {
        pThis->__confComputeGetMaxCeKeySpaceIdx__ = &confComputeGetMaxCeKeySpaceIdx_b2b553;
    }

    // confComputeGlobalKeyIsKernelPriv -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGlobalKeyIsKernelPriv__ = &confComputeGlobalKeyIsKernelPriv_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGlobalKeyIsKernelPriv__ = &confComputeGlobalKeyIsKernelPriv_d69453;
    }

    // confComputeGlobalKeyIsUvmKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGlobalKeyIsUvmKey__ = &confComputeGlobalKeyIsUvmKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGlobalKeyIsUvmKey__ = &confComputeGlobalKeyIsUvmKey_d69453;
    }

    // confComputeGetKeyPairByChannel -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetKeyPairByChannel__ = &confComputeGetKeyPairByChannel_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGetKeyPairByChannel__ = &confComputeGetKeyPairByChannel_395e98;
    }

    // confComputeTriggerKeyRotation -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_GH100;
        }
        // default
        else
        {
            pThis->__confComputeTriggerKeyRotation__ = &confComputeTriggerKeyRotation_ac1694;
        }
    }

    // confComputeGetKeyPairForKeySpace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetKeyPairForKeySpace__ = &confComputeGetKeyPairForKeySpace_GH100;
    }
    // default
    else
    {
        pThis->__confComputeGetKeyPairForKeySpace__ = &confComputeGetKeyPairForKeySpace_d44104;
    }

    // confComputeEnableKeyRotationCallback -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_ac1694;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_GH100;
        }
        // default
        else
        {
            pThis->__confComputeEnableKeyRotationCallback__ = &confComputeEnableKeyRotationCallback_ac1694;
        }
    }

    // confComputeEnableKeyRotationSupport -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_ac1694;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
                 ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
        {
            pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_GB100;
        }
        // default
        else
        {
            pThis->__confComputeEnableKeyRotationSupport__ = &confComputeEnableKeyRotationSupport_ac1694;
        }
    }

    // confComputeEnableInternalKeyRotationSupport -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_ac1694;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_GH100;
        }
        // default
        else
        {
            pThis->__confComputeEnableInternalKeyRotationSupport__ = &confComputeEnableInternalKeyRotationSupport_ac1694;
        }
    }

    // confComputeIsDebugModeEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeIsDebugModeEnabled__ = &confComputeIsDebugModeEnabled_GH100;
    }
    // default
    else
    {
        pThis->__confComputeIsDebugModeEnabled__ = &confComputeIsDebugModeEnabled_e661f0;
    }

    // confComputeIsGpuCcCapable -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_GB100;
    }
    // default
    else
    {
        pThis->__confComputeIsGpuCcCapable__ = &confComputeIsGpuCcCapable_d69453;
    }

    // confComputeTestPlatformSupport -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__confComputeTestPlatformSupport__ = &confComputeTestPlatformSupport_GH100;
    }
    // default
    else
    {
        pThis->__confComputeTestPlatformSupport__ = &confComputeTestPlatformSupport_ac1694;
    }

    // confComputeDeriveSessionKeys -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__confComputeDeriveSessionKeys__ = &confComputeDeriveSessionKeys_KERNEL;
    }
    // default
    else
    {
        pThis->__confComputeDeriveSessionKeys__ = &confComputeDeriveSessionKeys_395e98;
    }

    // confComputeKeyStoreDepositIvMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreDepositIvMask__ = &confComputeKeyStoreDepositIvMask_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDepositIvMask__ = &confComputeKeyStoreDepositIvMask_d44104;
    }

    // confComputeKeyStoreUpdateKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreUpdateKey__ = &confComputeKeyStoreUpdateKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreUpdateKey__ = &confComputeKeyStoreUpdateKey_395e98;
    }

    // confComputeKeyStoreIsValidGlobalKeyId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreIsValidGlobalKeyId__ = &confComputeKeyStoreIsValidGlobalKeyId_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreIsValidGlobalKeyId__ = &confComputeKeyStoreIsValidGlobalKeyId_d69453;
    }

    // confComputeKeyStoreInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreInit__ = &confComputeKeyStoreInit_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreInit__ = &confComputeKeyStoreInit_395e98;
    }

    // confComputeKeyStoreDeinit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreDeinit__ = &confComputeKeyStoreDeinit_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDeinit__ = &confComputeKeyStoreDeinit_d44104;
    }

    // confComputeKeyStoreGetExportMasterKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreGetExportMasterKey__ = &confComputeKeyStoreGetExportMasterKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreGetExportMasterKey__ = &confComputeKeyStoreGetExportMasterKey_9e2234;
    }

    // confComputeGetCurrentKeySeed -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeGetCurrentKeySeed__ = &confComputeGetCurrentKeySeed_GB100;
    }
    // default
    else
    {
        pThis->__confComputeGetCurrentKeySeed__ = &confComputeGetCurrentKeySeed_9e2234;
    }

    // confComputeKeyStoreDeriveKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreDeriveKey__ = &confComputeKeyStoreDeriveKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreDeriveKey__ = &confComputeKeyStoreDeriveKey_395e98;
    }

    // confComputeKeyStoreClearExportMasterKey -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__confComputeKeyStoreClearExportMasterKey__ = &confComputeKeyStoreClearExportMasterKey_GH100;
    }
    // default
    else
    {
        pThis->__confComputeKeyStoreClearExportMasterKey__ = &confComputeKeyStoreClearExportMasterKey_d44104;
    }
} // End __nvoc_init_funcTable_ConfidentialCompute_1 with approximately 94 basic block(s).


// Initialize vtable(s) for 52 virtual method(s).
void __nvoc_init_funcTable_ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 40 per-object function pointer(s).
    __nvoc_init_funcTable_ConfidentialCompute_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__ConfidentialCompute(ConfidentialCompute *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_ConfidentialCompute = pThis;    // (confCompute) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialCompute.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialCompute.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ConfidentialCompute;    // (confCompute) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ConfidentialCompute(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_ConfidentialCompute(ConfidentialCompute **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ConfidentialCompute *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;
    GpuHalspecOwner *pGpuhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ConfidentialCompute));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ConfidentialCompute));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ConfidentialCompute_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ConfidentialCompute_cleanup);
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_ConfidentialCompute_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ConfidentialCompute(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_ConfidentialCompute(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ConfidentialCompute_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ConfidentialCompute_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ConfidentialCompute));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ConfidentialCompute(ConfidentialCompute **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_ConfidentialCompute(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

