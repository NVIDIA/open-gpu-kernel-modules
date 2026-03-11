#define NVOC_CRASHCAT_WAYFINDER_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_crashcat_wayfinder_nvoc.h"

void __nvoc_init_halspec_CrashCatWayfinderHal(CrashCatWayfinderHal *pCrashCatWayfinderHal, NV_CRASHCAT_WAYFINDER_VERSION version)
{
    // V1
    if(version == 0x1)
    {
        pCrashCatWayfinderHal->__nvoc_HalVarIdx = 0;
    }
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__085e32 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatWayfinder;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for CrashCatWayfinder
void __nvoc_init__Object(Object*);
void __nvoc_init__CrashCatWayfinder(CrashCatWayfinder*,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version);
void __nvoc_init_funcTable_CrashCatWayfinder(CrashCatWayfinder*);
NV_STATUS __nvoc_ctor_CrashCatWayfinder(CrashCatWayfinder*, NvU32 wfl0);
void __nvoc_init_dataField_CrashCatWayfinder(CrashCatWayfinder*);
void __nvoc_dtor_CrashCatWayfinder(CrashCatWayfinder*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatWayfinder;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatWayfinder;

// Down-thunk(s) to bridge CrashCatWayfinder methods from ancestors (if any)

// Up-thunk(s) to bridge CrashCatWayfinder methods to ancestors (if any)

// Class-specific details for CrashCatWayfinder
const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatWayfinder = 
{
    .classInfo.size =               sizeof(CrashCatWayfinder),
    .classInfo.classId =            classId(CrashCatWayfinder),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CrashCatWayfinder",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_CrashCatWayfinder,
    .pCastInfo =          &__nvoc_castinfo__CrashCatWayfinder,
    .pExportInfo =        &__nvoc_export_info__CrashCatWayfinder
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CrashCatWayfinder __nvoc_metadata__CrashCatWayfinder = {
    .rtti.pClassDef = &__nvoc_class_def_CrashCatWayfinder,    // (crashcatWayfinder) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_CrashCatWayfinder,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(CrashCatWayfinder, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CrashCatWayfinder = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__CrashCatWayfinder.rtti,    // [0]: (crashcatWayfinder) this
        &__nvoc_metadata__CrashCatWayfinder.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CrashCatWayfinder = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CrashCatWayfinder object.
void __nvoc_crashcatWayfinderDestruct(CrashCatWayfinder*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_CrashCatWayfinder(CrashCatWayfinder* pThis) {

// Call destructor.
    __nvoc_crashcatWayfinderDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_CrashCatWayfinder(CrashCatWayfinder *pThis) {
    CrashCatWayfinderHal *wayfinderHal = &pThis->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
}


// Construct CrashCatWayfinder object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_CrashCatWayfinder(CrashCatWayfinder *arg_this, NvU32 wfl0) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&arg_this->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatWayfinder_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_CrashCatWayfinder(arg_this);

    // Call the constructor for this class.
    status = __nvoc_crashcatWayfinderConstruct(arg_this, wfl0);
    if (status != NV_OK) goto __nvoc_ctor_CrashCatWayfinder_fail__init;
    goto __nvoc_ctor_CrashCatWayfinder_exit; // Success

    // Unwind on error.
__nvoc_ctor_CrashCatWayfinder_fail__init:
    __nvoc_dtor_Object(&arg_this->__nvoc_base_Object);
__nvoc_ctor_CrashCatWayfinder_fail_Object:
__nvoc_ctor_CrashCatWayfinder_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CrashCatWayfinder_1(CrashCatWayfinder *pThis) {
    CrashCatWayfinderHal *wayfinderHal = &pThis->wayfinderHal;
    const unsigned long wayfinderHal_HalVarIdx = (unsigned long)wayfinderHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal);
    PORT_UNREFERENCED_VARIABLE(wayfinderHal_HalVarIdx);
} // End __nvoc_init_funcTable_CrashCatWayfinder_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CrashCatWayfinder(CrashCatWayfinder *pThis) {
    __nvoc_init_funcTable_CrashCatWayfinder_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CrashCatWayfinder(CrashCatWayfinder *pThis,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_CrashCatWayfinder = pThis;    // (crashcatWayfinder) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatWayfinder.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CrashCatWayfinder;    // (crashcatWayfinder) this

    // Initialize halspec data.
    __nvoc_init_halspec_CrashCatWayfinderHal(&pThis->wayfinderHal, CrashCatWayfinderHal_version);

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CrashCatWayfinder(pThis);
}

NV_STATUS __nvoc_objCreate_CrashCatWayfinder(CrashCatWayfinder **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags,
        NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version, NvU32 wfl0)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CrashCatWayfinder *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CrashCatWayfinder));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CrashCatWayfinder));

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
    __nvoc_init__CrashCatWayfinder(__nvoc_pThis, CrashCatWayfinderHal_version);
    __nvoc_status = __nvoc_ctor_CrashCatWayfinder(__nvoc_pThis, wfl0);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CrashCatWayfinder_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CrashCatWayfinder_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CrashCatWayfinder));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CrashCatWayfinder(CrashCatWayfinder **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    NV_CRASHCAT_WAYFINDER_VERSION CrashCatWayfinderHal_version = va_arg(__nvoc_args, NV_CRASHCAT_WAYFINDER_VERSION);
    NvU32 wfl0 = va_arg(__nvoc_args, NvU32);

    __nvoc_status = __nvoc_objCreate_CrashCatWayfinder(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, CrashCatWayfinderHal_version, wfl0);

    return __nvoc_status;
}

