#define NVOC_JOURNAL_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_journal_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x15dec8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJRCDB;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

void __nvoc_init_OBJRCDB(OBJRCDB*);
void __nvoc_init_funcTable_OBJRCDB(OBJRCDB*);
NV_STATUS __nvoc_ctor_OBJRCDB(OBJRCDB*);
void __nvoc_init_dataField_OBJRCDB(OBJRCDB*);
void __nvoc_dtor_OBJRCDB(OBJRCDB*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJRCDB;

static const struct NVOC_RTTI __nvoc_rtti_OBJRCDB_OBJRCDB = {
    /*pClassDef=*/          &__nvoc_class_def_OBJRCDB,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJRCDB,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJRCDB_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJRCDB, __nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJRCDB_OBJTRACEABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTRACEABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJRCDB, __nvoc_base_OBJTRACEABLE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJRCDB = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_OBJRCDB_OBJRCDB,
        &__nvoc_rtti_OBJRCDB_OBJTRACEABLE,
        &__nvoc_rtti_OBJRCDB_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_OBJRCDB,
    /*pExportInfo=*/        &__nvoc_export_info_OBJRCDB
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJRCDB = 
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
    pThis->setProperty(pThis, PDB_PROP_RCDB_COMPRESS, ((NvBool)(0 == 0)));
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

void __nvoc_init_Object(Object*);
void __nvoc_init_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init_OBJRCDB(OBJRCDB *pThis) {
    pThis->__nvoc_pbase_OBJRCDB = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;
    __nvoc_init_Object(&pThis->__nvoc_base_Object);
    __nvoc_init_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJRCDB);

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

    __nvoc_init_OBJRCDB(pThis);
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

