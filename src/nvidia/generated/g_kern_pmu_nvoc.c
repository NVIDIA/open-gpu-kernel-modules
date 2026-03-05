#define NVOC_KERN_PMU_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_pmu_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ab9d7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelPmu
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelPmu(KernelPmu*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_funcTable_KernelPmu(KernelPmu*, GpuHalspecOwner *pGpuhalspecowner);
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_init_dataField_KernelPmu(KernelPmu*, GpuHalspecOwner *pGpuhalspecowner);
void __nvoc_dtor_KernelPmu(KernelPmu*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelPmu;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPmu;

// Down-thunk(s) to bridge KernelPmu methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu, ENGDESCRIPTOR engDesc);    // this
void __nvoc_down_thunk_KernelPmu_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu);    // this
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu);    // this

// Up-thunk(s) to bridge KernelPmu methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent(struct OBJGPU *pGpu, struct KernelPmu *pEngstate);    // this

// Class-specific details for KernelPmu
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu = 
{
    .classInfo.size =               sizeof(KernelPmu),
    .classInfo.classId =            classId(KernelPmu),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelPmu",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelPmu,
    .pCastInfo =          &__nvoc_castinfo__KernelPmu,
    .pExportInfo =        &__nvoc_export_info__KernelPmu
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelPmu __nvoc_metadata__KernelPmu = {
    .rtti.pClassDef = &__nvoc_class_def_KernelPmu,    // (kpmu) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelPmu,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kpmuConstructEngine__ = &kpmuConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelPmu_engstateConstructEngine,    // virtual
    .vtable.__kpmuStateDestroy__ = &kpmuStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelPmu_engstateStateDestroy,    // virtual
    .vtable.__kpmuStateInitLocked__ = &kpmuStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelPmu_engstateStateInitLocked,    // virtual
    .vtable.__kpmuInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kpmuStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kpmuStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kpmuStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kpmuStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kpmuStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__kpmuStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kpmuStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kpmuStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__kpmuStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kpmuIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelPmu = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelPmu.rtti,    // [0]: (kpmu) this
        &__nvoc_metadata__KernelPmu.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelPmu.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in KernelPmu from superclasses

// kpmuConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu, ENGDESCRIPTOR engDesc) {
    return kpmuConstructEngine(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kpmuStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelPmu_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu) {
    kpmuStateDestroy(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPmu_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu) {
    return kpmuStateInitLocked(pGpu, (struct KernelPmu *)(((unsigned char *) pKernelPmu) - NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}


// 11 up-thunk(s) defined to bridge methods in KernelPmu to superclasses

// kpmuInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kpmuInitMissing(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}

// kpmuStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostLoad(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePreUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStateUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kpmuStatePostUnload(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)), arg3);
}

// kpmuIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kpmuIsPresent(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPmu = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct KernelPmu object.
void __nvoc_kpmuDestruct(KernelPmu*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelPmu(KernelPmu* pThis) {

// Call destructor.
    __nvoc_kpmuDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_KernelPmu(KernelPmu *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}


// Construct KernelPmu object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelPmu_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_KernelPmu(pThis, pGpuhalspecowner);
    goto __nvoc_ctor_KernelPmu_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelPmu_fail_OBJENGSTATE:
__nvoc_ctor_KernelPmu_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelPmu_1(KernelPmu *pThis, GpuHalspecOwner *pGpuhalspecowner) {
    ChipHal *chipHal = &pGpuhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pGpuhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // kpmuGetIsSelfInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x98000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        pThis->__kpmuGetIsSelfInit__ = &kpmuGetIsSelfInit_e661f0;
    }
    // default
    else
    {
        pThis->__kpmuGetIsSelfInit__ = &kpmuGetIsSelfInit_d69453;
    }
} // End __nvoc_init_funcTable_KernelPmu_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelPmu(KernelPmu *pThis, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelPmu_1(pThis, pGpuhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelPmu(KernelPmu *pThis, GpuHalspecOwner *pGpuhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelPmu = pThis;    // (kpmu) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPmu.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPmu.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelPmu;    // (kpmu) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelPmu(pThis, pGpuhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelPmu(KernelPmu **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelPmu *__nvoc_pThis;
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
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelPmu));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelPmu));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelPmu_cleanup);

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
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pGpuhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelPmu_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelPmu(__nvoc_pThis, pGpuhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelPmu(__nvoc_pThis, pGpuhalspecowner);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelPmu_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelPmu_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelPmu));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPmu(KernelPmu **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_KernelPmu(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

