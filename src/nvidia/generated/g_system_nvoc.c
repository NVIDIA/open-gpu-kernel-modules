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
char __nvoc_class_id_uniqueness_check__40e2c8 = 1;
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

// Class-specific details for OBJSYS
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS = 
{
    .classInfo.size =               sizeof(OBJSYS),
    .classInfo.classId =            classId(OBJSYS),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJSYS",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJSYS,
    .pCastInfo =          &__nvoc_castinfo__OBJSYS,
    .pExportInfo =        &__nvoc_export_info__OBJSYS
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
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJSYS object.
void __nvoc_sysDestruct(OBJSYS*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJTRACEABLE(OBJTRACEABLE*);
void __nvoc_dtor_OBJSYS(OBJSYS* pThis) {

// Call destructor.
    __nvoc_sysDestruct(pThis);

// Recurse to superclass destructors.
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
    pThis->setProperty(pThis, PDB_PROP_SYS_USE_RW_API_LOCK_GET_MEM_ALIGNMENT_BUG_5785851_WAR, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS, NV_TRUE);
    pThis->setProperty(pThis, PDB_PROP_SYS_ENABLE_FORCE_SHARED_LOCK, NV_TRUE);

    pThis->bUseDeferredClientListFree = NV_FALSE;

    pThis->clientListDeferredFreeLimit = 0;

    pThis->bEnableDynamicGranularityPageArrays = NV_FALSE;
    pThis->setProperty(pThis, PDB_PROP_SYS_RECOVERY_REBOOT_REQUIRED, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_SYS_SUPPORTS_S0IX, (0));
}


// Construct OBJSYS object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_OBJTRACEABLE(OBJTRACEABLE *);
NV_STATUS __nvoc_ctor_OBJSYS(OBJSYS *arg_this) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&arg_this->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_Object;
    status = __nvoc_ctor_OBJTRACEABLE(&arg_this->__nvoc_base_OBJTRACEABLE);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail_OBJTRACEABLE;

    // Initialize data fields.
    __nvoc_init_dataField_OBJSYS(arg_this);

    // Call the constructor for this class.
    status = __nvoc_sysConstruct(arg_this);
    if (status != NV_OK) goto __nvoc_ctor_OBJSYS_fail__init;
    goto __nvoc_ctor_OBJSYS_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJSYS_fail__init:
    __nvoc_dtor_OBJTRACEABLE(&arg_this->__nvoc_base_OBJTRACEABLE);
__nvoc_ctor_OBJSYS_fail_OBJTRACEABLE:
    __nvoc_dtor_Object(&arg_this->__nvoc_base_Object);
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

NV_STATUS __nvoc_objCreate_OBJSYS(OBJSYS **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJSYS *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJSYS));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJSYS));

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
    __nvoc_init__OBJSYS(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJSYS(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJSYS_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJSYS_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJSYS));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJSYS(OBJSYS **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJSYS(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

