#define NVOC_GPU_MGR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_mgr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__cf1b25 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMGR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJGPUMGR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJGPUMGR(OBJGPUMGR*);
void __nvoc_init_funcTable_OBJGPUMGR(OBJGPUMGR*);
NV_STATUS __nvoc_ctor_OBJGPUMGR(OBJGPUMGR*);
void __nvoc_init_dataField_OBJGPUMGR(OBJGPUMGR*);
void __nvoc_dtor_OBJGPUMGR(OBJGPUMGR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGPUMGR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGPUMGR;

// Down-thunk(s) to bridge OBJGPUMGR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJGPUMGR methods to ancestors (if any)

// Class-specific details for OBJGPUMGR
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMGR = 
{
    .classInfo.size =               sizeof(OBJGPUMGR),
    .classInfo.classId =            classId(OBJGPUMGR),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJGPUMGR",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGPUMGR,
    .pCastInfo =          &__nvoc_castinfo__OBJGPUMGR,
    .pExportInfo =        &__nvoc_export_info__OBJGPUMGR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJGPUMGR __nvoc_metadata__OBJGPUMGR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGPUMGR,    // (gpumgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGPUMGR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGPUMGR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGPUMGR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJGPUMGR.rtti,    // [0]: (gpumgr) this
        &__nvoc_metadata__OBJGPUMGR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGPUMGR = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJGPUMGR object.
void __nvoc_gpumgrDestruct(OBJGPUMGR*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGPUMGR(OBJGPUMGR* pThis) {

// Call destructor.
    __nvoc_gpumgrDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJGPUMGR(OBJGPUMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct OBJGPUMGR object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_OBJGPUMGR(OBJGPUMGR *arg_this) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&arg_this->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMGR_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_OBJGPUMGR(arg_this);

    // Call the constructor for this class.
    status = __nvoc_gpumgrConstruct(arg_this);
    if (status != NV_OK) goto __nvoc_ctor_OBJGPUMGR_fail__init;
    goto __nvoc_ctor_OBJGPUMGR_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJGPUMGR_fail__init:
    __nvoc_dtor_Object(&arg_this->__nvoc_base_Object);
__nvoc_ctor_OBJGPUMGR_fail_Object:
__nvoc_ctor_OBJGPUMGR_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGPUMGR_1(OBJGPUMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGPUMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGPUMGR(OBJGPUMGR *pThis) {
    __nvoc_init_funcTable_OBJGPUMGR_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJGPUMGR(OBJGPUMGR *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJGPUMGR = pThis;    // (gpumgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPUMGR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGPUMGR;    // (gpumgr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGPUMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGPUMGR(OBJGPUMGR **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJGPUMGR *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJGPUMGR));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJGPUMGR));

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
    __nvoc_init__OBJGPUMGR(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJGPUMGR(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJGPUMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJGPUMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJGPUMGR));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMGR(OBJGPUMGR **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJGPUMGR(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

