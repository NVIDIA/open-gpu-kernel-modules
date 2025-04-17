#define NVOC_ENG_STATE_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_eng_state_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x7a7ed6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJENGSTATE
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_funcTable_OBJENGSTATE(OBJENGSTATE*);
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_dataField_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJENGSTATE;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJENGSTATE;

// Down-thunk(s) to bridge OBJENGSTATE methods from ancestors (if any)

// Up-thunk(s) to bridge OBJENGSTATE methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJENGSTATE),
        /*classId=*/            classId(OBJENGSTATE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJENGSTATE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJENGSTATE,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJENGSTATE,
    /*pExportInfo=*/        &__nvoc_export_info__OBJENGSTATE
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJENGSTATE __nvoc_metadata__OBJENGSTATE = {
    .rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJENGSTATE,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJENGSTATE, __nvoc_base_Object),

    .vtable.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
    .vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJENGSTATE = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJENGSTATE.rtti,    // [0]: (engstate) this
        &__nvoc_metadata__OBJENGSTATE.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJENGSTATE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE *pThis) {
    __nvoc_engstateDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJENGSTATE(OBJENGSTATE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJENGSTATE_fail_Object;
    __nvoc_init_dataField_OBJENGSTATE(pThis);
    goto __nvoc_ctor_OBJENGSTATE_exit; // Success

__nvoc_ctor_OBJENGSTATE_fail_Object:
__nvoc_ctor_OBJENGSTATE_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJENGSTATE_1(OBJENGSTATE *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJENGSTATE_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_OBJENGSTATE(OBJENGSTATE *pThis) {
    __nvoc_init_funcTable_OBJENGSTATE_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJENGSTATE(OBJENGSTATE *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJENGSTATE = pThis;    // (engstate) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJENGSTATE.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJENGSTATE;    // (engstate) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJENGSTATE(pThis);
}

NV_STATUS __nvoc_objCreate_OBJENGSTATE(OBJENGSTATE **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJENGSTATE *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJENGSTATE), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJENGSTATE));

    pThis->__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__OBJENGSTATE(pThis);
    status = __nvoc_ctor_OBJENGSTATE(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJENGSTATE_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJENGSTATE_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJENGSTATE));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJENGSTATE(OBJENGSTATE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJENGSTATE(ppThis, pParent, createFlags);

    return status;
}

