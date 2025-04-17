#define NVOC_RS_SERVER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_rs_server_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x830542 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for RsShared
void __nvoc_init__Object(Object*);
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init_funcTable_RsShared(RsShared*);
NV_STATUS __nvoc_ctor_RsShared(RsShared*);
void __nvoc_init_dataField_RsShared(RsShared*);
void __nvoc_dtor_RsShared(RsShared*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsShared;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsShared;

// Down-thunk(s) to bridge RsShared methods from ancestors (if any)

// Up-thunk(s) to bridge RsShared methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsShared),
        /*classId=*/            classId(RsShared),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsShared",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsShared,
    /*pCastInfo=*/          &__nvoc_castinfo__RsShared,
    /*pExportInfo=*/        &__nvoc_export_info__RsShared
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RsShared __nvoc_metadata__RsShared = {
    .rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsShared,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(RsShared, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RsShared = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__RsShared.rtti,    // [0]: (shr) this
        &__nvoc_metadata__RsShared.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RsShared = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsShared(RsShared *pThis) {
    __nvoc_shrDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsShared(RsShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_RsShared(RsShared *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail_Object;
    __nvoc_init_dataField_RsShared(pThis);

    status = __nvoc_shrConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail__init;
    goto __nvoc_ctor_RsShared_exit; // Success

__nvoc_ctor_RsShared_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_RsShared_fail_Object:
__nvoc_ctor_RsShared_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsShared_1(RsShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsShared_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RsShared(RsShared *pThis) {
    __nvoc_init_funcTable_RsShared_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RsShared(RsShared *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_RsShared = pThis;    // (shr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RsShared.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RsShared;    // (shr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RsShared(pThis);
}

NV_STATUS __nvoc_objCreate_RsShared(RsShared **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsShared *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsShared), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsShared));

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

    __nvoc_init__RsShared(pThis);
    status = __nvoc_ctor_RsShared(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_RsShared_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsShared_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsShared));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsShared(RsShared **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_RsShared(ppThis, pParent, createFlags);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x830d90 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for RsSession
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__RsSession(RsSession*);
void __nvoc_init_funcTable_RsSession(RsSession*);
NV_STATUS __nvoc_ctor_RsSession(RsSession*);
void __nvoc_init_dataField_RsSession(RsSession*);
void __nvoc_dtor_RsSession(RsSession*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RsSession;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RsSession;

// Down-thunk(s) to bridge RsSession methods from ancestors (if any)

// Up-thunk(s) to bridge RsSession methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RsSession),
        /*classId=*/            classId(RsSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RsSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsSession,
    /*pCastInfo=*/          &__nvoc_castinfo__RsSession,
    /*pExportInfo=*/        &__nvoc_export_info__RsSession
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__RsSession __nvoc_metadata__RsSession = {
    .rtti.pClassDef = &__nvoc_class_def_RsSession,    // (session) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RsSession,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(RsSession, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(RsSession, __nvoc_base_RsShared.__nvoc_base_Object),

    .vtable.__sessionRemoveDependant__ = &sessionRemoveDependant_IMPL,    // virtual
    .vtable.__sessionRemoveDependency__ = &sessionRemoveDependency_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RsSession = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__RsSession.rtti,    // [0]: (session) this
        &__nvoc_metadata__RsSession.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__RsSession.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RsSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_RsSession(RsSession *pThis) {
    __nvoc_sessionDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RsSession(RsSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_RsSession(RsSession *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail_RsShared;
    __nvoc_init_dataField_RsSession(pThis);

    status = __nvoc_sessionConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail__init;
    goto __nvoc_ctor_RsSession_exit; // Success

__nvoc_ctor_RsSession_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
__nvoc_ctor_RsSession_fail_RsShared:
__nvoc_ctor_RsSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RsSession_1(RsSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RsSession_1


// Initialize vtable(s) for 2 virtual method(s).
void __nvoc_init_funcTable_RsSession(RsSession *pThis) {
    __nvoc_init_funcTable_RsSession_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RsSession(RsSession *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_RsSession = pThis;    // (session) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RsSession.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__RsSession.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RsSession;    // (session) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RsSession(pThis);
}

NV_STATUS __nvoc_objCreate_RsSession(RsSession **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RsSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RsSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RsSession));

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__RsSession(pThis);
    status = __nvoc_ctor_RsSession(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_RsSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RsSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RsSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RsSession(RsSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_RsSession(ppThis, pParent, createFlags);

    return status;
}

