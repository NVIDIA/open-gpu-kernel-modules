#define NVOC_KERNEL_RC_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_rc_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x4888db = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelRc;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelRc
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelRc(KernelRc*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelRc(KernelRc*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelRc(KernelRc*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelRc(KernelRc*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelRc(KernelRc*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelRc;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelRc;

// Down-thunk(s) to bridge KernelRc methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelRc_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelRc, ENGDESCRIPTOR engDescriptor);    // this

// Up-thunk(s) to bridge KernelRc methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_krcInitMissing(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreInitLocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateInitLocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateInitUnlocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePostLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePostUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_krcStateDestroy(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_krcIsPresent(struct OBJGPU *pGpu, struct KernelRc *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelRc = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelRc),
        /*classId=*/            classId(KernelRc),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelRc",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelRc,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelRc,
    /*pExportInfo=*/        &__nvoc_export_info__KernelRc
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelRc __nvoc_metadata__KernelRc = {
    .rtti.pClassDef = &__nvoc_class_def_KernelRc,    // (krc) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelRc,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__krcConstructEngine__ = &krcConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelRc_engstateConstructEngine,    // virtual
    .vtable.__krcInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_krcInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__krcStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__krcStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__krcStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_krcStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__krcStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_krcStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__krcStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__krcStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_krcStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__krcStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__krcStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__krcStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_krcStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__krcStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_krcStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__krcStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_krcStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__krcIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_krcIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelRc = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelRc.rtti,    // [0]: (krc) this
        &__nvoc_metadata__KernelRc.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelRc.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in KernelRc from superclasses

// krcConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelRc_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelRc, ENGDESCRIPTOR engDescriptor) {
    return krcConstructEngine(pGpu, (struct KernelRc *)(((unsigned char *) pKernelRc) - NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), engDescriptor);
}


// 13 up-thunk(s) defined to bridge methods in KernelRc to superclasses

// krcInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_krcInitMissing(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreInitLocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateInitLocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateInitUnlocked(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePostLoad(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePreUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStateUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_krcStatePostUnload(struct OBJGPU *pGpu, struct KernelRc *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)), arg3);
}

// krcStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_krcStateDestroy(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}

// krcIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_krcIsPresent(struct OBJGPU *pGpu, struct KernelRc *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelRc, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelRc = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelRc(KernelRc *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KRC_IS_MISSING
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KRC_IS_MISSING, NV_FALSE);
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelRc_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelRc(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelRc_exit; // Success

__nvoc_ctor_KernelRc_fail_OBJENGSTATE:
__nvoc_ctor_KernelRc_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelRc_1(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // krcWatchdogRecovery -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__krcWatchdogRecovery__ = &krcWatchdogRecovery_f2d351;
    }
    else
    {
        pThis->__krcWatchdogRecovery__ = &krcWatchdogRecovery_KERNEL;
    }
} // End __nvoc_init_funcTable_KernelRc_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelRc_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelRc(KernelRc *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelRc = pThis;    // (krc) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelRc.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelRc.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelRc;    // (krc) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelRc(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelRc(KernelRc **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelRc *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelRc), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelRc));

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

    __nvoc_init__KernelRc(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelRc(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelRc_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelRc_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelRc));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelRc(KernelRc **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelRc(ppThis, pParent, createFlags);

    return status;
}

