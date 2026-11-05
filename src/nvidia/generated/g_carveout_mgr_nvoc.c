#define NVOC_CARVEOUT_MGR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_carveout_mgr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e81fb1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CarveoutMgr;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CarveoutMgr
void __nvoc_init__Object(Object*);
void __nvoc_init__CarveoutMgr(CarveoutMgr*);
void __nvoc_init_funcTable_CarveoutMgr(CarveoutMgr*);
NV_STATUS __nvoc_ctor_CarveoutMgr(CarveoutMgr*);
void __nvoc_init_dataField_CarveoutMgr(CarveoutMgr*);
void __nvoc_dtor_CarveoutMgr(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CarveoutMgr;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CarveoutMgr;

// Down-thunk(s) to bridge CarveoutMgr methods from ancestors (if any)

// Up-thunk(s) to bridge CarveoutMgr methods to ancestors (if any)

// Class-specific details for CarveoutMgr
const struct NVOC_CLASS_DEF __nvoc_class_def_CarveoutMgr = 
{
    .classInfo.size =               sizeof(CarveoutMgr),
    .classInfo.classId =            classId(CarveoutMgr),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CarveoutMgr",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CarveoutMgr,
    .pCastInfo =          &__nvoc_castinfo__CarveoutMgr,
    .pExportInfo =        &__nvoc_export_info__CarveoutMgr
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CarveoutMgr __nvoc_metadata__CarveoutMgr = {
    .rtti.pClassDef = &__nvoc_class_def_CarveoutMgr,    // (carveoutmgr) this
    .rtti.dtor      = &__nvoc_dtor_CarveoutMgr,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(CarveoutMgr, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CarveoutMgr = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__CarveoutMgr.rtti,    // [0]: (carveoutmgr) this
        &__nvoc_metadata__CarveoutMgr.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CarveoutMgr = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CarveoutMgr object.
void __nvoc_carveoutmgrDestruct(CarveoutMgr*);
void __nvoc_dtor_Object(Dynamic*);
void __nvoc_dtor_CarveoutMgr(Dynamic* pThis) {

    CarveoutMgr *__nvoc_this = (CarveoutMgr *) pThis;

// Call destructor.
    __nvoc_carveoutmgrDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_Object((Dynamic *) &__nvoc_this->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CarveoutMgr(CarveoutMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CarveoutMgr object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_CarveoutMgr(CarveoutMgr *arg_this) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&arg_this->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CarveoutMgr_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_CarveoutMgr(arg_this);

    // Call the constructor for this class.
    status = __nvoc_carveoutmgrConstruct(arg_this);
    if (status != NV_OK) goto __nvoc_ctor_CarveoutMgr_fail__init;
    goto __nvoc_ctor_CarveoutMgr_exit; // Success

    // Unwind on error.
__nvoc_ctor_CarveoutMgr_fail__init:
    __nvoc_dtor_Object((Dynamic *)&arg_this->__nvoc_base_Object);
__nvoc_ctor_CarveoutMgr_fail_Object:
__nvoc_ctor_CarveoutMgr_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CarveoutMgr_1(CarveoutMgr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CarveoutMgr_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CarveoutMgr(CarveoutMgr *pThis) {
    __nvoc_init_funcTable_CarveoutMgr_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CarveoutMgr(CarveoutMgr *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_CarveoutMgr = pThis;    // (carveoutmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CarveoutMgr.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CarveoutMgr;    // (carveoutmgr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CarveoutMgr(pThis);
}

NV_STATUS __nvoc_objCreate_CarveoutMgr(CarveoutMgr **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CarveoutMgr *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CarveoutMgr));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CarveoutMgr));

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
    __nvoc_init__CarveoutMgr(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CarveoutMgr(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CarveoutMgr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CarveoutMgr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CarveoutMgr));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CarveoutMgr(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_CarveoutMgr((CarveoutMgr **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}

