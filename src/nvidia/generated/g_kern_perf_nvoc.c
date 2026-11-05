#define NVOC_KERN_PERF_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_perf_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__c53a57 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelPerf
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelPerf(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelPerf;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPerf;

// Down-thunk(s) to bridge KernelPerf methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
void __nvoc_down_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this

// Up-thunk(s) to bridge KernelPerf methods to ancestors (if any)

// Class-specific details for KernelPerf
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf = 
{
    .classInfo.size =               sizeof(KernelPerf),
    .classInfo.classId =            classId(KernelPerf),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelPerf",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelPerf,
    .pCastInfo =          &__nvoc_castinfo__KernelPerf,
    .pExportInfo =        &__nvoc_export_info__KernelPerf
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelPerf __nvoc_metadata__KernelPerf = {
    .rtti.pClassDef = &__nvoc_class_def_KernelPerf,    // (kperf) this
    .rtti.dtor      = &__nvoc_dtor_KernelPerf,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelPerf_engstateConstructEngine,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelPerf_engstateStateInitLocked,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelPerf_engstateStateLoad,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelPerf_engstateStateUnload,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelPerf_engstateStateDestroy,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateConstructApertures__ = &engstateConstructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateDestructApertures__ = &engstateDestructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelPerf = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelPerf.rtti,    // [0]: (kperf) this
        &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 5 down-thunk(s) defined to bridge methods in KernelPerf from superclasses

// kperfConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc) {
    return kperfConstructEngine_IMPL(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kperfStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    return kperfStateInitLocked_IMPL(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateLoad_IMPL(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), flags);
}

// kperfStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateUnload_IMPL(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), flags);
}

// kperfStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    kperfStateDestroy_IMPL(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPerf = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelPerf object.
void __nvoc_dtor_OBJENGSTATE(Dynamic*);
void __nvoc_dtor_KernelPerf(Dynamic* pThis) {

    KernelPerf *__nvoc_this = (KernelPerf *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE((Dynamic *) &__nvoc_this->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct KernelPerf object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelPerf_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_KernelPerf(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelPerf_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelPerf_fail_OBJENGSTATE:
__nvoc_ctor_KernelPerf_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelPerf_1(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kperfGpuBoostSyncStateInit -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kperfGpuBoostSyncStateInit__ = &kperfGpuBoostSyncStateInit_ac1694;
    }
    // default
    else
    {
        pThis->__kperfGpuBoostSyncStateInit__ = &kperfGpuBoostSyncStateInit_IMPL;
    }

    // kperfPerfSetPowerstate -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kperfPerfSetPowerstate__ = &kperfPerfSetPowerstate_ac1694;
    }
    else
    {
        pThis->__kperfPerfSetPowerstate__ = &kperfPerfSetPowerstate_KERNEL;
    }
} // End __nvoc_init_funcTable_KernelPerf_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 2 virtual method(s).
void __nvoc_init_funcTable_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_KernelPerf_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelPerf = pThis;    // (kperf) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf;    // (kperf) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelPerf(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelPerf(KernelPerf **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelPerf *__nvoc_pThis;
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
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelPerf));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelPerf));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelPerf_cleanup);

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
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelPerf_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelPerf(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelPerf(__nvoc_pThis, pRmhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelPerf_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelPerf_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelPerf));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPerf(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_KernelPerf((KernelPerf **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}

