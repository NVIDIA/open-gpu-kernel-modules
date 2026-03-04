#define NVOC_INSTRUMENTATION_MANAGER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_instrumentation_manager_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x30d0c5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_InstrumentationManager;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for InstrumentationManager
void __nvoc_init__Object(Object*);
void __nvoc_init__InstrumentationManager(InstrumentationManager*);
void __nvoc_init_funcTable_InstrumentationManager(InstrumentationManager*);
NV_STATUS __nvoc_ctor_InstrumentationManager(InstrumentationManager*);
void __nvoc_init_dataField_InstrumentationManager(InstrumentationManager*);
void __nvoc_dtor_InstrumentationManager(InstrumentationManager*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__InstrumentationManager;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__InstrumentationManager;

// Down-thunk(s) to bridge InstrumentationManager methods from ancestors (if any)

// Up-thunk(s) to bridge InstrumentationManager methods to ancestors (if any)

const struct NVOC_CLASS_DEF __nvoc_class_def_InstrumentationManager = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(InstrumentationManager),
        /*classId=*/            classId(InstrumentationManager),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "InstrumentationManager",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_InstrumentationManager,
    /*pCastInfo=*/          &__nvoc_castinfo__InstrumentationManager,
    /*pExportInfo=*/        &__nvoc_export_info__InstrumentationManager
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__InstrumentationManager __nvoc_metadata__InstrumentationManager = {
    .rtti.pClassDef = &__nvoc_class_def_InstrumentationManager,    // (instrumentationmanager) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_InstrumentationManager,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(InstrumentationManager, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__InstrumentationManager = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__InstrumentationManager.rtti,    // [0]: (instrumentationmanager) this
        &__nvoc_metadata__InstrumentationManager.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__InstrumentationManager = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_instrumentationmanagerDestruct(InstrumentationManager*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_InstrumentationManager(InstrumentationManager *pThis) {
    __nvoc_instrumentationmanagerDestruct(pThis);
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_InstrumentationManager(InstrumentationManager *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_Object(Object* );
NV_STATUS __nvoc_ctor_InstrumentationManager(InstrumentationManager *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_Object(&pThis->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_InstrumentationManager_fail_Object;
    __nvoc_init_dataField_InstrumentationManager(pThis);

    status = __nvoc_instrumentationmanagerConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_InstrumentationManager_fail__init;
    goto __nvoc_ctor_InstrumentationManager_exit; // Success

__nvoc_ctor_InstrumentationManager_fail__init:
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);
__nvoc_ctor_InstrumentationManager_fail_Object:
__nvoc_ctor_InstrumentationManager_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_InstrumentationManager_1(InstrumentationManager *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_InstrumentationManager_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_InstrumentationManager(InstrumentationManager *pThis) {
    __nvoc_init_funcTable_InstrumentationManager_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__InstrumentationManager(InstrumentationManager *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_InstrumentationManager = pThis;    // (instrumentationmanager) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__InstrumentationManager.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__InstrumentationManager;    // (instrumentationmanager) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_InstrumentationManager(pThis);
}

NV_STATUS __nvoc_objCreate_InstrumentationManager(InstrumentationManager **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    InstrumentationManager *pThis;

    // Don't allocate memory if the caller has already done so.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, ppThis != NULL && *ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        pThis = *ppThis;
    }

    // Allocate memory
    else
    {
        pThis = portMemAllocNonPaged(sizeof(InstrumentationManager));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(InstrumentationManager));

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

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__InstrumentationManager(pThis);
    status = __nvoc_ctor_InstrumentationManager(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_InstrumentationManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_InstrumentationManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(InstrumentationManager));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // Failure
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_InstrumentationManager(InstrumentationManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_InstrumentationManager(ppThis, pParent, createFlags);

    return status;
}

