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
char __nvoc_class_id_uniqueness_check__0x61649c = 1;
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

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJDCECLIENTRM = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJDCECLIENTRM),
        /*classId=*/            classId(OBJDCECLIENTRM),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJDCECLIENTRM",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJDCECLIENTRM,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJDCECLIENTRM,
    /*pExportInfo=*/        &__nvoc_export_info__OBJDCECLIENTRM
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dceclientDestruct(OBJDCECLIENTRM*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    __nvoc_dceclientDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_OBJDCECLIENTRM(OBJDCECLIENTRM *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJDCECLIENTRM_fail_OBJENGSTATE;
    __nvoc_init_dataField_OBJDCECLIENTRM(pThis);
    goto __nvoc_ctor_OBJDCECLIENTRM_exit; // Success

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

NV_STATUS __nvoc_objCreate_OBJDCECLIENTRM(OBJDCECLIENTRM **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJDCECLIENTRM *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJDCECLIENTRM), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJDCECLIENTRM));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__OBJDCECLIENTRM(pThis);
    status = __nvoc_ctor_OBJDCECLIENTRM(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJDCECLIENTRM_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJDCECLIENTRM_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJDCECLIENTRM));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJDCECLIENTRM(OBJDCECLIENTRM **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJDCECLIENTRM(ppThis, pParent, createFlags);

    return status;
}

