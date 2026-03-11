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
char __nvoc_class_id_uniqueness_check__830542 = 1;
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

// Class-specific details for RsShared
const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared = 
{
    .classInfo.size =               sizeof(RsShared),
    .classInfo.classId =            classId(RsShared),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsShared",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsShared,
    .pCastInfo =          &__nvoc_castinfo__RsShared,
    .pExportInfo =        &__nvoc_export_info__RsShared
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
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsShared object.
void __nvoc_shrDestruct(RsShared*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_RsShared(RsShared* pThis) {

// Call destructor.
    __nvoc_shrDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_RsShared(RsShared *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RsShared object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_RsShared(RsShared *pShared) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pShared->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_RsShared(pShared);

    // Call the constructor for this class.
    status = __nvoc_shrConstruct(pShared);
    if (status != NV_OK) goto __nvoc_ctor_RsShared_fail__init;
    goto __nvoc_ctor_RsShared_exit; // Success

    // Unwind on error.
__nvoc_ctor_RsShared_fail__init:
    __nvoc_dtor_Object(&pShared->__nvoc_base_Object);
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

NV_STATUS __nvoc_objCreate_RsShared(RsShared **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RsShared *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RsShared));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RsShared));

    __nvoc_pThis->__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RsShared(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RsShared(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RsShared_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RsShared_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RsShared));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RsShared(RsShared **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_RsShared(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__830d90 = 1;
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

// Class-specific details for RsSession
const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession = 
{
    .classInfo.size =               sizeof(RsSession),
    .classInfo.classId =            classId(RsSession),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RsSession",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RsSession,
    .pCastInfo =          &__nvoc_castinfo__RsSession,
    .pExportInfo =        &__nvoc_export_info__RsSession
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
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RsSession object.
void __nvoc_sessionDestruct(RsSession*);
void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_RsSession(RsSession* pThis) {

// Call destructor.
    __nvoc_sessionDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_RsSession(RsSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RsSession object.
NV_STATUS __nvoc_ctor_RsShared(RsShared *pShared);
NV_STATUS __nvoc_ctor_RsSession(RsSession *pSession) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsShared(&pSession->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail_RsShared;

    // Initialize data fields.
    __nvoc_init_dataField_RsSession(pSession);

    // Call the constructor for this class.
    status = __nvoc_sessionConstruct(pSession);
    if (status != NV_OK) goto __nvoc_ctor_RsSession_fail__init;
    goto __nvoc_ctor_RsSession_exit; // Success

    // Unwind on error.
__nvoc_ctor_RsSession_fail__init:
    __nvoc_dtor_RsShared(&pSession->__nvoc_base_RsShared);
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

NV_STATUS __nvoc_objCreate_RsSession(RsSession **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RsSession *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RsSession));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RsSession));

    __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RsSession(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RsSession(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RsSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RsSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RsSession));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RsSession(RsSession **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_RsSession(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

