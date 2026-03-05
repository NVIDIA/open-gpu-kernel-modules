#define NVOC_DCE_CLIENT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_dce_client_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__61649c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJDCECLIENTRM;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for OBJDCECLIENTRM
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__OBJDCECLIENTRM(OBJDCECLIENTRM*);
void __nvoc_init_funcTable_OBJDCECLIENTRM(OBJDCECLIENTRM*);
NV_STATUS __nvoc_ctor_OBJDCECLIENTRM(OBJDCECLIENTRM*);
void __nvoc_init_dataField_OBJDCECLIENTRM(OBJDCECLIENTRM*);
void __nvoc_dtor_OBJDCECLIENTRM(OBJDCECLIENTRM*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJDCECLIENTRM;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJDCECLIENTRM;

// Down-thunk(s) to bridge OBJDCECLIENTRM methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3);    // this
void __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateDestroy(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this

// Up-thunk(s) to bridge OBJDCECLIENTRM methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_dceclientInitMissing(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitLocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStateInitLocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStateInitUnlocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreLoad(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePostLoad(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreUnload(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePostUnload(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_dceclientIsPresent(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate);    // this

// Class-specific details for OBJDCECLIENTRM
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJDCECLIENTRM = 
{
    .classInfo.size =               sizeof(OBJDCECLIENTRM),
    .classInfo.classId =            classId(OBJDCECLIENTRM),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJDCECLIENTRM",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJDCECLIENTRM,
    .pCastInfo =          &__nvoc_castinfo__OBJDCECLIENTRM,
    .pExportInfo =        &__nvoc_export_info__OBJDCECLIENTRM
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJDCECLIENTRM __nvoc_metadata__OBJDCECLIENTRM = {
    .rtti.pClassDef = &__nvoc_class_def_OBJDCECLIENTRM,    // (dceclient) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJDCECLIENTRM,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__dceclientConstructEngine__ = &dceclientConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_OBJDCECLIENTRM_engstateConstructEngine,    // virtual
    .vtable.__dceclientStateDestroy__ = &dceclientStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_OBJDCECLIENTRM_engstateStateDestroy,    // virtual
    .vtable.__dceclientStateLoad__ = &dceclientStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_OBJDCECLIENTRM_engstateStateLoad,    // virtual
    .vtable.__dceclientStateUnload__ = &dceclientStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_OBJDCECLIENTRM_engstateStateUnload,    // virtual
    .vtable.__dceclientInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__dceclientStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__dceclientStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__dceclientStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__dceclientStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__dceclientStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__dceclientStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__dceclientStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__dceclientStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__dceclientIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_dceclientIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJDCECLIENTRM = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJDCECLIENTRM.rtti,    // [0]: (dceclient) this
        &__nvoc_metadata__OBJDCECLIENTRM.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__OBJDCECLIENTRM.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 4 down-thunk(s) defined to bridge methods in OBJDCECLIENTRM from superclasses

// dceclientConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return dceclientConstructEngine(arg1, (struct OBJDCECLIENTRM *)(((unsigned char *) arg_this) - NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateDestroy(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    dceclientStateDestroy(arg1, (struct OBJDCECLIENTRM *)(((unsigned char *) arg_this) - NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return dceclientStateLoad(arg1, (struct OBJDCECLIENTRM *)(((unsigned char *) arg_this) - NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJDCECLIENTRM_engstateStateUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return dceclientStateUnload(arg1, (struct OBJDCECLIENTRM *)(((unsigned char *) arg_this) - NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}


// 10 up-thunk(s) defined to bridge methods in OBJDCECLIENTRM to superclasses

// dceclientInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_dceclientInitMissing(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitLocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStateInitLocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStateInitUnlocked(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}

// dceclientStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreLoad(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePostLoad(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePreUnload(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_dceclientStatePostUnload(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)), arg3);
}

// dceclientIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_dceclientIsPresent(struct OBJGPU *pGpu, struct OBJDCECLIENTRM *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJDCECLIENTRM, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJDCECLIENTRM = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJDCECLIENTRM object.
void __nvoc_dceclientDestruct(OBJDCECLIENTRM*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJDCECLIENTRM(OBJDCECLIENTRM* pThis) {

// Call destructor.
    __nvoc_dceclientDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct OBJDCECLIENTRM object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJDCECLIENTRM_fail_OBJENGSTATE;

    // Initialize data fields.
    __nvoc_init_dataField_OBJDCECLIENTRM(pThis);
    goto __nvoc_ctor_OBJDCECLIENTRM_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJDCECLIENTRM_fail_OBJENGSTATE:
__nvoc_ctor_OBJDCECLIENTRM_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJDCECLIENTRM_1(OBJDCECLIENTRM *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJDCECLIENTRM_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    __nvoc_init_funcTable_OBJDCECLIENTRM_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_OBJDCECLIENTRM = pThis;    // (dceclient) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJDCECLIENTRM.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJDCECLIENTRM.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJDCECLIENTRM;    // (dceclient) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJDCECLIENTRM(pThis);
}

NV_STATUS __nvoc_objCreate_OBJDCECLIENTRM(OBJDCECLIENTRM **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJDCECLIENTRM *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJDCECLIENTRM));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJDCECLIENTRM));

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
    __nvoc_init__OBJDCECLIENTRM(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJDCECLIENTRM(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJDCECLIENTRM_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJDCECLIENTRM_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJDCECLIENTRM));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJDCECLIENTRM(OBJDCECLIENTRM **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJDCECLIENTRM(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

