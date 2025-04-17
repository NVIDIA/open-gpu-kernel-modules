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
char __nvoc_class_id_uniqueness_check__0x15dec8 = 1;
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

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJRCDB = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJRCDB),
        /*classId=*/            classId(OBJRCDB),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJRCDB",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJRCDB,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJRCDB,
    /*pExportInfo=*/        &__nvoc_export_info__OBJRCDB
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
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJRCDB(OBJRCDB *pThis) {
    __nvoc_rcdbDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJRCDB(OBJRCDB *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_RCDB_COMPRESS, NV_TRUE);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE* );
NV_STATUS __nvoc_ctor_OBJRCDB(OBJRCDB *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail_Object;
    status = __nvoc_ctor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail_OBJTRACEABLE;
    __nvoc_init_dataField_OBJRCDB(pThis);

    status = __nvoc_rcdbConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJRCDB_fail__init;
    goto __nvoc_ctor_OBJRCDB_exit; // Success

__nvoc_ctor_OBJRCDB_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJRCDB_fail_OBJTRACEABLE:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
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

NV_STATUS __nvoc_objCreate_OBJRCDB(OBJRCDB **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJRCDB *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJRCDB), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJRCDB));

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

    __nvoc_init__OBJRCDB(pThis);
    status = __nvoc_ctor_OBJRCDB(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJRCDB_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJRCDB_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJRCDB));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJRCDB(OBJRCDB **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJRCDB(ppThis, pParent, createFlags);

    return status;
}

