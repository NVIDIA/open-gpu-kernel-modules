#define NVOC_OBJGRIDDISPLAYLESS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_objgriddisplayless_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__20fd5a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGRIDDISPLAYLESS;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for OBJGRIDDISPLAYLESS
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS*);
void __nvoc_init_funcTable_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS*);
NV_STATUS __nvoc_ctor_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS*);
void __nvoc_init_dataField_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS*);
void __nvoc_dtor_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGRIDDISPLAYLESS;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGRIDDISPLAYLESS;

// Down-thunk(s) to bridge OBJGRIDDISPLAYLESS methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJGRIDDISPLAYLESS_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pGriddisplayless, ENGDESCRIPTOR arg3);    // this

// Up-thunk(s) to bridge OBJGRIDDISPLAYLESS methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_griddisplaylessInitMissing(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitLocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitLocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitUnlocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateDestroy(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_griddisplaylessIsPresent(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate);    // this

// Class-specific details for OBJGRIDDISPLAYLESS
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGRIDDISPLAYLESS = 
{
    .classInfo.size =               sizeof(OBJGRIDDISPLAYLESS),
    .classInfo.classId =            classId(OBJGRIDDISPLAYLESS),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJGRIDDISPLAYLESS",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGRIDDISPLAYLESS,
    .pCastInfo =          &__nvoc_castinfo__OBJGRIDDISPLAYLESS,
    .pExportInfo =        &__nvoc_export_info__OBJGRIDDISPLAYLESS
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJGRIDDISPLAYLESS __nvoc_metadata__OBJGRIDDISPLAYLESS = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGRIDDISPLAYLESS,    // (griddisplayless) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGRIDDISPLAYLESS,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__griddisplaylessConstructEngine__ = &griddisplaylessConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_OBJGRIDDISPLAYLESS_engstateConstructEngine,    // virtual
    .vtable.__griddisplaylessInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__griddisplaylessStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__griddisplaylessStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__griddisplaylessStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__griddisplaylessStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__griddisplaylessStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__griddisplaylessStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__griddisplaylessStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__griddisplaylessStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__griddisplaylessStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__griddisplaylessStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__griddisplaylessStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__griddisplaylessIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_griddisplaylessIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGRIDDISPLAYLESS = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJGRIDDISPLAYLESS.rtti,    // [0]: (griddisplayless) this
        &__nvoc_metadata__OBJGRIDDISPLAYLESS.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__OBJGRIDDISPLAYLESS.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in OBJGRIDDISPLAYLESS from superclasses

// griddisplaylessConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJGRIDDISPLAYLESS_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pGriddisplayless, ENGDESCRIPTOR arg3) {
    return griddisplaylessConstructEngine(pGpu, (struct OBJGRIDDISPLAYLESS *)(((unsigned char *) pGriddisplayless) - NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}


// 13 up-thunk(s) defined to bridge methods in OBJGRIDDISPLAYLESS to superclasses

// griddisplaylessInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_griddisplaylessInitMissing(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitLocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitLocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateInitUnlocked(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostLoad(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePreUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStatePostUnload(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)), arg3);
}

// griddisplaylessStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_griddisplaylessStateDestroy(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}

// griddisplaylessIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_griddisplaylessIsPresent(struct OBJGPU *pGpu, struct OBJGRIDDISPLAYLESS *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJGRIDDISPLAYLESS, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGRIDDISPLAYLESS = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJGRIDDISPLAYLESS object.
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS* pThis) {

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct OBJGRIDDISPLAYLESS object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJGRIDDISPLAYLESS_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_OBJGRIDDISPLAYLESS(pThis);
    goto __nvoc_ctor_OBJGRIDDISPLAYLESS_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJGRIDDISPLAYLESS_fail_OBJENGSTATE:
__nvoc_ctor_OBJGRIDDISPLAYLESS_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGRIDDISPLAYLESS_1(OBJGRIDDISPLAYLESS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGRIDDISPLAYLESS_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS *pThis) {
    __nvoc_init_funcTable_OBJGRIDDISPLAYLESS_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_OBJGRIDDISPLAYLESS = pThis;    // (griddisplayless) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGRIDDISPLAYLESS.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGRIDDISPLAYLESS.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGRIDDISPLAYLESS;    // (griddisplayless) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGRIDDISPLAYLESS(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJGRIDDISPLAYLESS *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJGRIDDISPLAYLESS));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJGRIDDISPLAYLESS));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__OBJGRIDDISPLAYLESS(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJGRIDDISPLAYLESS(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJGRIDDISPLAYLESS_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJGRIDDISPLAYLESS_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJGRIDDISPLAYLESS));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGRIDDISPLAYLESS(OBJGRIDDISPLAYLESS **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJGRIDDISPLAYLESS(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

