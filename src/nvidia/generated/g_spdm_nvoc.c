#define NVOC_SPDM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_spdm_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__261ee8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for Spdm
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_Spdm(Spdm*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Spdm;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Spdm;

// Down-thunk(s) to bridge Spdm methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_Spdm_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_Spdm_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_Spdm_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, NvU32 flags);    // this

// Up-thunk(s) to bridge Spdm methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_spdmInitMissing(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitLocked(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitUnlocked(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateInitLocked(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateInitUnlocked(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreLoad(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateLoad(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateUnload(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePostUnload(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_spdmStateDestroy(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_spdmIsPresent(struct OBJGPU *pGpu, struct Spdm *pEngstate);    // this

// Class-specific details for Spdm
const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm = 
{
    .classInfo.size =               sizeof(Spdm),
    .classInfo.classId =            classId(Spdm),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Spdm",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Spdm,
    .pCastInfo =          &__nvoc_castinfo__Spdm,
    .pExportInfo =        &__nvoc_export_info__Spdm
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__Spdm __nvoc_metadata__Spdm = {
    .rtti.pClassDef = &__nvoc_class_def_Spdm,    // (spdm) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Spdm,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__spdmConstructEngine__ = &spdmConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_Spdm_engstateConstructEngine,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_Spdm_engstateStatePostLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &__nvoc_down_thunk_Spdm_engstateStatePreUnload,    // virtual
    .vtable.__spdmInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_spdmInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__spdmStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__spdmStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__spdmStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__spdmStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__spdmStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__spdmStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__spdmStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__spdmStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__spdmStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_spdmStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__spdmIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_spdmIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Spdm = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__Spdm.rtti,    // [0]: (spdm) this
        &__nvoc_metadata__Spdm.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__Spdm.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in Spdm from superclasses

// spdmConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_Spdm_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, ENGDESCRIPTOR engDesc) {
    return spdmConstructEngine(pGpu, (struct Spdm *)(((unsigned char *) pSpdm) - NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), engDesc);
}

// spdmStatePostLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_Spdm_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, NvU32 flags) {
    return spdmStatePostLoad(pGpu, (struct Spdm *)(((unsigned char *) pSpdm) - NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), flags);
}

// spdmStatePreUnload: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_Spdm_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pSpdm, NvU32 flags) {
    return spdmStatePreUnload(pGpu, (struct Spdm *)(((unsigned char *) pSpdm) - NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), flags);
}


// 11 up-thunk(s) defined to bridge methods in Spdm to superclasses

// spdmInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_spdmInitMissing(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitLocked(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreInitUnlocked(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateInitLocked(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateInitUnlocked(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePreLoad(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), arg3);
}

// spdmStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateLoad(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), arg3);
}

// spdmStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStateUnload(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), arg3);
}

// spdmStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_spdmStatePostUnload(struct OBJGPU *pGpu, struct Spdm *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)), arg3);
}

// spdmStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_spdmStateDestroy(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}

// spdmIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_spdmIsPresent(struct OBJGPU *pGpu, struct Spdm *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(Spdm, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__Spdm = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Spdm object.
void __nvoc_spdmDestruct(Spdm*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_Spdm(Spdm* pThis) {

// Call destructor.
    __nvoc_spdmDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
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
    pThis->setProperty(pThis, PDB_PROP_SPDM_ENABLED, NV_FALSE);
}


// Construct Spdm object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_Spdm_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_Spdm(pThis, pRmhalspecowner, pGpuhalspecowner);
    goto __nvoc_ctor_Spdm_exit; // Success

    // Unwind on error.
__nvoc_ctor_Spdm_fail_OBJENGSTATE:
__nvoc_ctor_Spdm_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Spdm_1(Spdm *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {
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

    // spdmStatePostLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__spdmStatePostLoad__ = &spdmStatePostLoad_KERNEL;
    }
    // default
    else
    {
        pThis->__spdmStatePostLoad__ = &spdmStatePostLoad_ac1694;
    }

    // spdmStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__spdmStatePreUnload__ = &spdmStatePreUnload_KERNEL;
    }
    // default
    else
    {
        pThis->__spdmStatePreUnload__ = &spdmStatePreUnload_ac1694;
    }

    // spdmGetCertChains -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_395e98;
    }

    // spdmGetAttestationReport -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_395e98;
    }

    // spdmCheckAndExecuteKeyUpdate -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmCheckAndExecuteKeyUpdate__ = &spdmCheckAndExecuteKeyUpdate_GH100;
    }
    // default
    else
    {
        pThis->__spdmCheckAndExecuteKeyUpdate__ = &spdmCheckAndExecuteKeyUpdate_395e98;
    }

    // spdmSendInitRmDataCommand -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmSendInitRmDataCommand__ = &spdmSendInitRmDataCommand_GH100;
    }
    // default
    else
    {
        pThis->__spdmSendInitRmDataCommand__ = &spdmSendInitRmDataCommand_395e98;
    }

    // spdmRegisterForHeartbeats -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmRegisterForHeartbeats__ = &spdmRegisterForHeartbeats_GH100;
    }
    // default
    else
    {
        pThis->__spdmRegisterForHeartbeats__ = &spdmRegisterForHeartbeats_395e98;
    }

    // spdmUnregisterFromHeartbeats -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmUnregisterFromHeartbeats__ = &spdmUnregisterFromHeartbeats_GH100;
    }
    // default
    else
    {
        pThis->__spdmUnregisterFromHeartbeats__ = &spdmUnregisterFromHeartbeats_395e98;
    }

    // spdmMutualAuthSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmMutualAuthSupported__ = &spdmMutualAuthSupported_e661f0;
    }
    // default
    else
    {
        pThis->__spdmMutualAuthSupported__ = &spdmMutualAuthSupported_d69453;
    }

    // spdmSendCtrlCall -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmSendCtrlCall__ = &spdmSendCtrlCall_GH100;
    }
    // default
    else
    {
        pThis->__spdmSendCtrlCall__ = &spdmSendCtrlCall_395e98;
    }

    // spdmDeviceInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_395e98;
    }

    // spdmDeviceDeinit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_395e98;
    }

    // spdmDeviceSecuredSessionSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_395e98;
    }

    // spdmCheckConnection -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_GH100;
    }
    // default
    else
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_395e98;
    }

    // spdmMessageProcess -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_GH100;
    }
    // default
    else
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_395e98;
    }

    // spdmGetCertificates -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_395e98;
    }

    // spdmGetReqEncapCertificates -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetReqEncapCertificates__ = &spdmGetReqEncapCertificates_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetReqEncapCertificates__ = &spdmGetReqEncapCertificates_395e98;
    }

    // spdmGetRequesterCertificateCount -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_395e98;
    }

    // spdmGetBinArchiveIndividualL2Certificate -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f8UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_GB20X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_9e2234;
    }

    // spdmGetBinArchiveIndividualL3Certificate -- halified (11 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x08000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GB102 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x80000000UL) )) /* ChipHal: GB110 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB110;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB112 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB112;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000008UL) )) /* ChipHal: GB202 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB202;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000010UL) )) /* ChipHal: GB203 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB203;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000020UL) )) /* ChipHal: GB205 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB205;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000040UL) )) /* ChipHal: GB206 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB206;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000080UL) )) /* ChipHal: GB207 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB207;
    }
    // default
    else
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_9e2234;
    }

    // spdmGetIndividualCertificate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x04000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_395e98;
    }

    // spdmConvertCertificateToDer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xbc000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__spdmConvertCertificateToDer__ = &spdmConvertCertificateToDer_GH100;
    }
    // default
    else
    {
        pThis->__spdmConvertCertificateToDer__ = &spdmConvertCertificateToDer_395e98;
    }
} // End __nvoc_init_funcTable_Spdm_1 with approximately 57 basic block(s).


// Initialize vtable(s) for 34 virtual method(s).
void __nvoc_init_funcTable_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 22 per-object function pointer(s).
    __nvoc_init_funcTable_Spdm_1(pThis, pRmhalspecowner, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_Spdm = pThis;    // (spdm) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Spdm.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__Spdm.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Spdm;    // (spdm) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Spdm(pThis, pRmhalspecowner, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_Spdm(Spdm **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Spdm *__nvoc_pThis;
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
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Spdm));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Spdm));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Spdm_cleanup);

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
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Spdm_cleanup);
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_Spdm_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Spdm(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_Spdm(__nvoc_pThis, pRmhalspecowner, pGpuhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Spdm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Spdm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Spdm));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Spdm(Spdm **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_Spdm(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

