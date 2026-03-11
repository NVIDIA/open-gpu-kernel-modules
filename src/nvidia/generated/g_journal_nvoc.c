#define NVOC_JOURNAL_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_journal_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__15dec8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJRCDB;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

// Forward declarations for OBJRCDB
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init__OBJRCDB(OBJRCDB*);
void __nvoc_init_funcTable_OBJRCDB(OBJRCDB*);
NV_STATUS __nvoc_ctor_OBJRCDB(OBJRCDB*);
void __nvoc_init_dataField_OBJRCDB(OBJRCDB*);
void __nvoc_dtor_OBJRCDB(OBJRCDB*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJRCDB;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJRCDB;

// Down-thunk(s) to bridge OBJRCDB methods from ancestors (if any)

// Up-thunk(s) to bridge OBJRCDB methods to ancestors (if any)

// Class-specific details for OBJRCDB
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJRCDB = 
{
    .classInfo.size =               sizeof(OBJRCDB),
    .classInfo.classId =            classId(OBJRCDB),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJRCDB",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJRCDB,
    .pCastInfo =          &__nvoc_castinfo__OBJRCDB,
    .pExportInfo =        &__nvoc_export_info__OBJRCDB
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJRCDB __nvoc_metadata__OBJRCDB = {
    .rtti.pClassDef = &__nvoc_class_def_OBJRCDB,    // (rcdb) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJRCDB,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJRCDB, __nvoc_base_Object),
    .metadata__OBJTRACEABLE.rtti.pClassDef = &__nvoc_class_def_OBJTRACEABLE,    // (traceable) super
    .metadata__OBJTRACEABLE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJTRACEABLE.rtti.offset    = NV_OFFSETOF(OBJRCDB, __nvoc_base_OBJTRACEABLE),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJRCDB = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJRCDB.rtti,    // [0]: (rcdb) this
        &__nvoc_metadata__OBJRCDB.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__OBJRCDB.metadata__OBJTRACEABLE.rtti,    // [2]: (traceable) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJRCDB = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJRCDB object.
void __nvoc_rcdbDestruct(OBJRCDB*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJRCDB(OBJRCDB* pThis) {

// Call destructor.
    __nvoc_rcdbDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJRCDB(OBJRCDB *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_RCDB_COMPRESS, NV_TRUE);
}


// Construct OBJRCDB object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE *);
NV_STATUS __nvoc_ctor_OBJRCDB(OBJRCDB *pRcdb) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pRcdb->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail_Object;
    status = __nvoc_ctor_OBJTRACEABLE(&pRcdb->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail_OBJTRACEABLE;

    // Initialize data fields.
    __nvoc_init_dataField_OBJRCDB(pRcdb);

    // Call the constructor for this class.
    status = __nvoc_rcdbConstruct(pRcdb);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail__init;
    goto __nvoc_ctor_OBJRCDB_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJRCDB_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pRcdb->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJRCDB_fail_OBJTRACEABLE:
    __nvoc_dtor_Object(&pRcdb->__nvoc_base_Object);
__nvoc_ctor_OBJRCDB_fail_Object:
__nvoc_ctor_OBJRCDB_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJRCDB_1(OBJRCDB *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJRCDB_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJRCDB(OBJRCDB *pThis) {
    __nvoc_init_funcTable_OBJRCDB_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJRCDB(OBJRCDB *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_pbase_OBJRCDB = pThis;    // (rcdb) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJRCDB.metadata__Object;    // (obj) super
    pThis->__nvoc_base_OBJTRACEABLE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJRCDB.metadata__OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJRCDB;    // (rcdb) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJRCDB(pThis);
}

NV_STATUS __nvoc_objCreate_OBJRCDB(OBJRCDB **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJRCDB *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJRCDB));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJRCDB));

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
    __nvoc_init__OBJRCDB(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJRCDB(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJRCDB_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJRCDB_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJRCDB));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJRCDB(OBJRCDB **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJRCDB(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

