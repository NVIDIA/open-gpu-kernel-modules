#define NVOC_CHIPSET_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_chipset_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__547dbb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJCL;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJCL
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJCL(OBJCL*);
void __nvoc_init_funcTable_OBJCL(OBJCL*);
NV_STATUS __nvoc_ctor_OBJCL(OBJCL*);
void __nvoc_init_dataField_OBJCL(OBJCL*);
void __nvoc_dtor_OBJCL(OBJCL*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJCL;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJCL;

// Down-thunk(s) to bridge OBJCL methods from ancestors (if any)

// Up-thunk(s) to bridge OBJCL methods to ancestors (if any)

// Class-specific details for OBJCL
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJCL = 
{
    .classInfo.size =               sizeof(OBJCL),
    .classInfo.classId =            classId(OBJCL),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJCL",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJCL,
    .pCastInfo =          &__nvoc_castinfo__OBJCL,
    .pExportInfo =        &__nvoc_export_info__OBJCL
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJCL __nvoc_metadata__OBJCL = {
    .rtti.pClassDef = &__nvoc_class_def_OBJCL,    // (cl) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJCL,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJCL, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJCL = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJCL.rtti,    // [0]: (cl) this
        &__nvoc_metadata__OBJCL.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJCL = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJCL object.
void __nvoc_clDestruct(OBJCL*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJCL(OBJCL* pThis) {

// Call destructor.
    __nvoc_clDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJCL(OBJCL *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
    pThis->setProperty(pThis, PDB_PROP_CL_HAS_RESIZABLE_BAR_ISSUE, NV_FALSE);
    pThis->setProperty(pThis, PDB_PROP_CL_BUG_3751839_GEN_SPEED_WAR, NV_FALSE);
}


// Construct OBJCL object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_OBJCL(OBJCL *pCl) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pCl->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJCL_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_OBJCL(pCl);

    // Call the constructor for this class.
    status = __nvoc_clConstruct(pCl);
    if (status != NV_OK) goto __nvoc_ctor_OBJCL_fail__init;
    goto __nvoc_ctor_OBJCL_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJCL_fail__init:
    __nvoc_dtor_Object(&pCl->__nvoc_base_Object);
__nvoc_ctor_OBJCL_fail_Object:
__nvoc_ctor_OBJCL_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJCL_1(OBJCL *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJCL_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJCL(OBJCL *pThis) {
    __nvoc_init_funcTable_OBJCL_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJCL(OBJCL *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJCL = pThis;    // (cl) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJCL.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJCL;    // (cl) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJCL(pThis);
}

NV_STATUS __nvoc_objCreate_OBJCL(OBJCL **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJCL *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJCL));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJCL));

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
    __nvoc_init__OBJCL(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJCL(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJCL_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJCL_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJCL));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJCL(OBJCL **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJCL(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

