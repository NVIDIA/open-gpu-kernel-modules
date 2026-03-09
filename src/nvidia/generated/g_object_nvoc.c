#define NVOC_OBJECT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_object_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__497031 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for Object
void __nvoc_init__Object(Object*);
void __nvoc_init_funcTable_Object(Object*);
NV_STATUS __nvoc_ctor_Object(Object*);
void __nvoc_init_dataField_Object(Object*);
void __nvoc_dtor_Object(Object*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Object;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Object;

// Down-thunk(s) to bridge Object methods from ancestors (if any)

// Up-thunk(s) to bridge Object methods to ancestors (if any)

// Class-specific details for Object
const struct NVOC_CLASS_DEF __nvoc_class_def_Object = 
{
    .classInfo.size =               sizeof(Object),
    .classInfo.classId =            classId(Object),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Object",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Object,
    .pCastInfo =          &__nvoc_castinfo__Object,
    .pExportInfo =        &__nvoc_export_info__Object
};


// Metadata with per-class RTTI
static const struct NVOC_METADATA__Object __nvoc_metadata__Object = {
    .rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Object,
    .rtti.offset    = 0,
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Object = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__Object.rtti,    // [0]: (obj) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Object = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Object object.
void __nvoc_dtor_Object(Object* pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_Object(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct Object object.
NV_STATUS __nvoc_ctor_Object(Object *pThis) {
    NV_STATUS status = NV_OK;

    // Initialize data fields.
    __nvoc_init_dataField_Object(pThis);
    goto __nvoc_ctor_Object_exit; // Success

    // Unwind on error.
__nvoc_ctor_Object_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Object_1(Object *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Object_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Object(Object *pThis) {
    __nvoc_init_funcTable_Object_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Object(Object *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = pThis;    // (obj) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Object;    // (obj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Object(pThis);
}

NV_STATUS __nvoc_objCreate_Object(Object **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Object *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Object));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Object));

    __nvoc_pThis->createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, __nvoc_pThis);
    }
    else
    {
        __nvoc_pThis->pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Object(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_Object(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Object_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Object_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, __nvoc_pThis);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Object));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Object(Object **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_Object(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

