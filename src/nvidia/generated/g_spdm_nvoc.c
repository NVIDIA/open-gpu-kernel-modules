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
char __nvoc_class_id_uniqueness_check__0x261ee8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for Spdm
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_Spdm(Spdm*, RmHalspecOwner *pRmhalspecowner);
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

const struct NVOC_CLASS_DEF __nvoc_class_def_Spdm = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Spdm),
        /*classId=*/            classId(Spdm),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Spdm",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Spdm,
    /*pCastInfo=*/          &__nvoc_castinfo__Spdm,
    /*pExportInfo=*/        &__nvoc_export_info__Spdm
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_Spdm(Spdm *pThis) {
    __nvoc_spdmDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    pThis->setProperty(pThis, PDB_PROP_SPDM_ENABLED, NV_FALSE);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_Spdm_fail_OBJENGSTATE;
    __nvoc_init_dataField_Spdm(pThis, pRmhalspecowner);
    goto __nvoc_ctor_Spdm_exit; // Success

__nvoc_ctor_Spdm_fail_OBJENGSTATE:
__nvoc_ctor_Spdm_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Spdm_1(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
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
        pThis->__spdmStatePostLoad__ = &spdmStatePostLoad_56cd7a;
    }

    // spdmStatePreUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__spdmStatePreUnload__ = &spdmStatePreUnload_KERNEL;
    }
    // default
    else
    {
        pThis->__spdmStatePreUnload__ = &spdmStatePreUnload_56cd7a;
    }

    // spdmGetCertChains -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertChains__ = &spdmGetCertChains_46f6a7;
    }

    // spdmGetAttestationReport -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetAttestationReport__ = &spdmGetAttestationReport_46f6a7;
    }

    // spdmCheckAndExecuteKeyUpdate -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmCheckAndExecuteKeyUpdate__ = &spdmCheckAndExecuteKeyUpdate_GH100;
    }
    // default
    else
    {
        pThis->__spdmCheckAndExecuteKeyUpdate__ = &spdmCheckAndExecuteKeyUpdate_46f6a7;
    }

    // spdmSendInitRmDataCommand -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmSendInitRmDataCommand__ = &spdmSendInitRmDataCommand_GH100;
    }
    // default
    else
    {
        pThis->__spdmSendInitRmDataCommand__ = &spdmSendInitRmDataCommand_46f6a7;
    }

    // spdmRegisterForHeartbeats -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmRegisterForHeartbeats__ = &spdmRegisterForHeartbeats_GH100;
    }
    // default
    else
    {
        pThis->__spdmRegisterForHeartbeats__ = &spdmRegisterForHeartbeats_46f6a7;
    }

    // spdmUnregisterFromHeartbeats -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmUnregisterFromHeartbeats__ = &spdmUnregisterFromHeartbeats_GH100;
    }
    // default
    else
    {
        pThis->__spdmUnregisterFromHeartbeats__ = &spdmUnregisterFromHeartbeats_46f6a7;
    }

    // spdmMutualAuthSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmMutualAuthSupported__ = &spdmMutualAuthSupported_88bc07;
    }
    // default
    else
    {
        pThis->__spdmMutualAuthSupported__ = &spdmMutualAuthSupported_3dd2c9;
    }

    // spdmSendCtrlCall -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmSendCtrlCall__ = &spdmSendCtrlCall_GH100;
    }
    // default
    else
    {
        pThis->__spdmSendCtrlCall__ = &spdmSendCtrlCall_46f6a7;
    }

    // spdmDeviceInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceInit__ = &spdmDeviceInit_46f6a7;
    }

    // spdmDeviceDeinit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceDeinit__ = &spdmDeviceDeinit_46f6a7;
    }

    // spdmDeviceSecuredSessionSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_GH100;
    }
    // default
    else
    {
        pThis->__spdmDeviceSecuredSessionSupported__ = &spdmDeviceSecuredSessionSupported_46f6a7;
    }

    // spdmCheckConnection -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_GH100;
    }
    // default
    else
    {
        pThis->__spdmCheckConnection__ = &spdmCheckConnection_46f6a7;
    }

    // spdmMessageProcess -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_GH100;
    }
    // default
    else
    {
        pThis->__spdmMessageProcess__ = &spdmMessageProcess_46f6a7;
    }

    // spdmGetCertificates -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetCertificates__ = &spdmGetCertificates_46f6a7;
    }

    // spdmGetReqEncapCertificates -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 */ 
    {
        pThis->__spdmGetReqEncapCertificates__ = &spdmGetReqEncapCertificates_GH100;
    }
    // default
    else
    {
        pThis->__spdmGetReqEncapCertificates__ = &spdmGetReqEncapCertificates_46f6a7;
    }

    // spdmGetRequesterCertificateCount -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetRequesterCertificateCount__ = &spdmGetRequesterCertificateCount_46f6a7;
    }

    // spdmGetBinArchiveIndividualL2Certificate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetBinArchiveIndividualL2Certificate__ = &spdmGetBinArchiveIndividualL2Certificate_fa6e19;
    }

    // spdmGetBinArchiveIndividualL3Certificate -- halified (6 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x20000000UL) )) /* ChipHal: GB100 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x40000000UL) )) /* ChipHal: GB102 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* ChipHal: GB110 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB110;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000004UL) )) /* ChipHal: GB112 */ 
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_GB112;
    }
    // default
    else
    {
        pThis->__spdmGetBinArchiveIndividualL3Certificate__ = &spdmGetBinArchiveIndividualL3Certificate_fa6e19;
    }

    // spdmGetIndividualCertificate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_GB100;
    }
    // default
    else
    {
        pThis->__spdmGetIndividualCertificate__ = &spdmGetIndividualCertificate_46f6a7;
    }
} // End __nvoc_init_funcTable_Spdm_1 with approximately 49 basic block(s).


// Initialize vtable(s) for 33 virtual method(s).
void __nvoc_init_funcTable_Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 21 per-object function pointer(s).
    __nvoc_init_funcTable_Spdm_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__Spdm(Spdm *pThis, RmHalspecOwner *pRmhalspecowner) {

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
    __nvoc_init_funcTable_Spdm(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_Spdm(Spdm **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Spdm *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Spdm), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Spdm));

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

    __nvoc_init__Spdm(pThis, pRmhalspecowner);
    status = __nvoc_ctor_Spdm(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_Spdm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Spdm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Spdm));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Spdm(Spdm **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_Spdm(ppThis, pParent, createFlags);

    return status;
}

