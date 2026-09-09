#define NVOC_KERNEL_OOB_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_oob_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__710fb4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelOob;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelOob
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelOob(KernelOob*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelOob(KernelOob*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelOob(KernelOob*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelOob(KernelOob*, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelOob(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelOob;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelOob;

// Down-thunk(s) to bridge KernelOob methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelOob_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelOob_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelOob_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, NvU32 flags);    // this

// Up-thunk(s) to bridge KernelOob methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_koobInitMissing(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreInitLocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStateInitLocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStateInitUnlocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreLoad(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePostLoad(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreUnload(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePostUnload(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_koobStateDestroy(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_koobIsPresent(struct OBJGPU *pGpu, struct KernelOob *pEngstate);    // this

// Class-specific details for KernelOob
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelOob = 
{
    .classInfo.size =               sizeof(KernelOob),
    .classInfo.classId =            classId(KernelOob),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelOob",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelOob,
    .pCastInfo =          &__nvoc_castinfo__KernelOob,
    .pExportInfo =        &__nvoc_export_info__KernelOob
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelOob __nvoc_metadata__KernelOob = {
    .rtti.pClassDef = &__nvoc_class_def_KernelOob,    // (koob) this
    .rtti.dtor      = &__nvoc_dtor_KernelOob,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__koobConstructEngine__ = &koobConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelOob_engstateConstructEngine,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelOob_engstateStateLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelOob_engstateStateUnload,    // virtual
    .vtable.__koobInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_koobInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__koobStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__koobStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__koobStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_koobStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__koobStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_koobStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__koobStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__koobStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__koobStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__koobStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_koobStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__koobStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_koobStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__koobIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_koobIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelOob = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelOob.rtti,    // [0]: (koob) this
        &__nvoc_metadata__KernelOob.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelOob.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelOob from superclasses

// koobConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelOob_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, ENGDESCRIPTOR arg3) {
    return koobConstructEngine(pGpu, (struct KernelOob *)(((unsigned char *) pKernelOob) - NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), arg3);
}

// koobStateLoad: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelOob_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, NvU32 flags) {
    return koobStateLoad(pGpu, (struct KernelOob *)(((unsigned char *) pKernelOob) - NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), flags);
}

// koobStateUnload: virtual halified (2 hals) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelOob_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelOob, NvU32 flags) {
    return koobStateUnload(pGpu, (struct KernelOob *)(((unsigned char *) pKernelOob) - NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), flags);
}


// 11 up-thunk(s) defined to bridge methods in KernelOob to superclasses

// koobInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_koobInitMissing(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreInitLocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStateInitLocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStateInitUnlocked(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreLoad(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), arg3);
}

// koobStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePostLoad(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), arg3);
}

// koobStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePreUnload(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), arg3);
}

// koobStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_koobStatePostUnload(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)), arg3);
}

// koobStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_koobStateDestroy(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}

// koobIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_koobIsPresent(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelOob, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelOob = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelOob object.
void __nvoc_koobDestruct(KernelOob*);
void __nvoc_dtor_OBJENGSTATE(Dynamic*);
void __nvoc_dtor_KernelOob(Dynamic* pThis) {

    KernelOob *__nvoc_this = (KernelOob *) pThis;

// Call destructor.
    __nvoc_koobDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE((Dynamic *) &__nvoc_this->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelOob(KernelOob *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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
}


// Construct KernelOob object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_KernelOob(KernelOob *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelOob_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_KernelOob(pThis, pGpuhalspecowner, pRmhalspecowner);
    goto __nvoc_ctor_KernelOob_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelOob_fail_OBJENGSTATE:
__nvoc_ctor_KernelOob_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelOob_1(KernelOob *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {
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

    // koobStateLoad -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
    {
        pThis->__koobStateLoad__ = &koobStateLoad_GB100;
    }
    // default
    else
    {
        pThis->__koobStateLoad__ = &koobStateLoad_ac1694;
    }

    // koobStateUnload -- virtual halified (2 hals) override (engstate) base (engstate) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
    {
        pThis->__koobStateUnload__ = &koobStateUnload_GB100;
    }
    // default
    else
    {
        pThis->__koobStateUnload__ = &koobStateUnload_ac1694;
    }

    // koobInitAperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
    {
        pThis->__koobInitAperture__ = &koobInitAperture_GB100;
    }
    // default
    else
    {
        pThis->__koobInitAperture__ = &koobInitAperture_395e98;
    }

    // koobCheckSupported -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__koobCheckSupported__ = &koobCheckSupported_d69453;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
        {
            pThis->__koobCheckSupported__ = &koobCheckSupported_GB100;
        }
        // default
        else
        {
            pThis->__koobCheckSupported__ = &koobCheckSupported_d69453;
        }
    }

    // koobSendEvent -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__koobSendEvent__ = &koobSendEvent_d44104;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
        {
            pThis->__koobSendEvent__ = &koobSendEvent_GB100;
        }
        // default
        else
        {
            pThis->__koobSendEvent__ = &koobSendEvent_d44104;
        }
    }

    // koobNsmEncodeEvent -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__koobNsmEncodeEvent__ = &koobNsmEncodeEvent_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
        {
            pThis->__koobNsmEncodeEvent__ = &koobNsmEncodeEvent_GB100;
        }
        // default
        else
        {
            pThis->__koobNsmEncodeEvent__ = &koobNsmEncodeEvent_395e98;
        }
    }

    // koobMnocMboxSend -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__koobMnocMboxSend__ = &koobMnocMboxSend_395e98;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000303fUL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GR100 | GR102 */ 
        {
            pThis->__koobMnocMboxSend__ = &koobMnocMboxSend_GB100;
        }
        // default
        else
        {
            pThis->__koobMnocMboxSend__ = &koobMnocMboxSend_395e98;
        }
    }
} // End __nvoc_init_funcTable_KernelOob_1 with approximately 18 basic block(s).


// Initialize vtable(s) for 19 virtual method(s).
void __nvoc_init_funcTable_KernelOob(KernelOob *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 7 per-object function pointer(s).
    __nvoc_init_funcTable_KernelOob_1(pThis, pGpuhalspecowner, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelOob(KernelOob *pThis, GpuHalspecOwner *pGpuhalspecowner, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelOob = pThis;    // (koob) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelOob.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelOob.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelOob;    // (koob) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelOob(pThis, pGpuhalspecowner, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelOob(KernelOob **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelOob *__nvoc_pThis;
    GpuHalspecOwner *pGpuhalspecowner;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelOob));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelOob));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelOob_cleanup);

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
    if ((pGpuhalspecowner = dynamicCast(__nvoc_pParent, GpuHalspecOwner)) == NULL)
        pGpuhalspecowner = objFindAncestorOfType(GpuHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelOob_cleanup);
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelOob_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelOob(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelOob(__nvoc_pThis, pGpuhalspecowner, pRmhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelOob_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelOob_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelOob));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelOob(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_KernelOob((KernelOob **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}

