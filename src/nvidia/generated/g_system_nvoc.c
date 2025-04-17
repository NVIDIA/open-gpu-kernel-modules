#define NVOC_SYSTEM_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_system_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x40e2c8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTRACEABLE;

// Forward declarations for OBJSYS
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_init__OBJSYS(OBJSYS*);
void __nvoc_init_funcTable_OBJSYS(OBJSYS*);
NV_STATUS __nvoc_ctor_OBJSYS(OBJSYS*);
void __nvoc_init_dataField_OBJSYS(OBJSYS*);
void __nvoc_dtor_OBJSYS(OBJSYS*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJSYS;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJSYS;

// Down-thunk(s) to bridge OBJSYS methods from ancestors (if any)

// Up-thunk(s) to bridge OBJSYS methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJSYS),
        /*classId=*/            classId(OBJSYS),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJSYS",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJSYS,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJSYS,
    /*pExportInfo=*/        &__nvoc_export_info__OBJSYS
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJSYS __nvoc_metadata__OBJSYS = {
    .rtti.pClassDef = &__nvoc_class_def_OBJSYS,    // (sys) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJSYS,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJSYS, __nvoc_base_Object),
    .metadata__OBJTRACEABLE.rtti.pClassDef = &__nvoc_class_def_OBJTRACEABLE,    // (traceable) super
    .metadata__OBJTRACEABLE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJTRACEABLE.rtti.offset    = NV_OFFSETOF(OBJSYS, __nvoc_base_OBJTRACEABLE),

    .vtable.__sysCaptureState__ = &sysCaptureState_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJSYS = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJSYS.rtti,    // [0]: (sys) this
        &__nvoc_metadata__OBJSYS.metadata__Object.rtti,    // [1]: (obj) super
        &__nvoc_metadata__OBJSYS.metadata__OBJTRACEABLE.rtti,    // [2]: (traceable) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJSYS = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJSYS(OBJSYS *pThis) {
    __nvoc_sysDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJSYS(OBJSYS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, ((0) || (1) || (0)));
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, ((1) && !0));
    pThis->setProperty(pThis, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED, ((0) || (0)));
    pThis->setProperty(pThis, PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_PRIORITY_BOOST, (0));
    pThis->setProperty(pThis, PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US, 16 * 1000);
    pThis->setProperty(pThis, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS, NV_TRUE);
    pThis->setProperty(pThis, PDB_PROP_SYS_ENABLE_FORCE_SHARED_LOCK, NV_TRUE);

    pThis->bUseDeferredClientListFree = NV_FALSE;

    pThis->clientListDeferredFreeLimit = 0;
    pThis->setProperty(pThis, PDB_PROP_SYS_RECOVERY_REBOOT_REQUIRED, NV_FALSE);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE* );
NV_STATUS __nvoc_ctor_OBJSYS(OBJSYS *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_Object;
    status = __nvoc_ctor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_OBJTRACEABLE;
    __nvoc_init_dataField_OBJSYS(pThis);

    status = __nvoc_sysConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail__init;
    goto __nvoc_ctor_OBJSYS_exit; // Success

__nvoc_ctor_OBJSYS_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJSYS_fail_OBJTRACEABLE:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_OBJSYS_fail_Object:
__nvoc_ctor_OBJSYS_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJSYS_1(OBJSYS *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJSYS_1


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_OBJSYS(OBJSYS *pThis) {
    __nvoc_init_funcTable_OBJSYS_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJSYS(OBJSYS *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJTRACEABLE = &pThis->__nvoc_base_OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_pbase_OBJSYS = pThis;    // (sys) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);
    __nvoc_init__OBJTRACEABLE(&pThis->__nvoc_base_OBJTRACEABLE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJSYS.metadata__Object;    // (obj) super
    pThis->__nvoc_base_OBJTRACEABLE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJSYS.metadata__OBJTRACEABLE;    // (traceable) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJSYS;    // (sys) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJSYS(pThis);
}

NV_STATUS __nvoc_objCreate_OBJSYS(OBJSYS **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJSYS *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJSYS), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJSYS));

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

    __nvoc_init__OBJSYS(pThis);
    status = __nvoc_ctor_OBJSYS(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJSYS_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJSYS_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJSYS));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJSYS(OBJSYS **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJSYS(ppThis, pParent, createFlags);

    return status;
}

