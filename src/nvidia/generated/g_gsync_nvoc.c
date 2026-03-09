#define NVOC_GSYNC_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gsync_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__d07fd0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

// Forward declarations for OBJGSYNCMGR
void __nvoc_init__Object(Object*);
void __nvoc_init__OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR*);
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJGSYNCMGR;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGSYNCMGR;

// Down-thunk(s) to bridge OBJGSYNCMGR methods from ancestors (if any)

// Up-thunk(s) to bridge OBJGSYNCMGR methods to ancestors (if any)

// Class-specific details for OBJGSYNCMGR
const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGSYNCMGR = 
{
    .classInfo.size =               sizeof(OBJGSYNCMGR),
    .classInfo.classId =            classId(OBJGSYNCMGR),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "OBJGSYNCMGR",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJGSYNCMGR,
    .pCastInfo =          &__nvoc_castinfo__OBJGSYNCMGR,
    .pExportInfo =        &__nvoc_export_info__OBJGSYNCMGR
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__OBJGSYNCMGR __nvoc_metadata__OBJGSYNCMGR = {
    .rtti.pClassDef = &__nvoc_class_def_OBJGSYNCMGR,    // (gsyncmgr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJGSYNCMGR,
    .rtti.offset    = 0,
    .metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super
    .metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Object.rtti.offset    = NV_OFFSETOF(OBJGSYNCMGR, __nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJGSYNCMGR = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__OBJGSYNCMGR.rtti,    // [0]: (gsyncmgr) this
        &__nvoc_metadata__OBJGSYNCMGR.metadata__Object.rtti,    // [1]: (obj) super
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJGSYNCMGR = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct OBJGSYNCMGR object.
void __nvoc_gsyncmgrDestruct(OBJGSYNCMGR*);
void __nvoc_dtor_Object(Object*);
void __nvoc_dtor_OBJGSYNCMGR(OBJGSYNCMGR* pThis) {

// Call destructor.
    __nvoc_gsyncmgrDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_Object(&pThis->__nvoc_base_Object);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct OBJGSYNCMGR object.
NV_STATUS __nvoc_ctor_Object(Object *);
NV_STATUS __nvoc_ctor_OBJGSYNCMGR(OBJGSYNCMGR *pGsyncmgr) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_Object(&pGsyncmgr->__nvoc_base_Object);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail_Object;

    // Initialize data fields.
    __nvoc_init_dataField_OBJGSYNCMGR(pGsyncmgr);

    // Call the constructor for this class.
    status = __nvoc_gsyncmgrConstruct(pGsyncmgr);
    if (status != NV_OK) goto __nvoc_ctor_OBJGSYNCMGR_fail__init;
    goto __nvoc_ctor_OBJGSYNCMGR_exit; // Success

    // Unwind on error.
__nvoc_ctor_OBJGSYNCMGR_fail__init:
    __nvoc_dtor_Object(&pGsyncmgr->__nvoc_base_Object);
__nvoc_ctor_OBJGSYNCMGR_fail_Object:
__nvoc_ctor_OBJGSYNCMGR_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJGSYNCMGR_1(OBJGSYNCMGR *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJGSYNCMGR_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_OBJGSYNCMGR(OBJGSYNCMGR *pThis) {
    __nvoc_init_funcTable_OBJGSYNCMGR_1(pThis);
}

NvBool gsyncmgrIsFirmwareGPUMismatch_STATIC_DISPATCH(struct OBJGPU *pGpu, OBJGSYNC *pGsync) {
    ChipHal *chipHal = &staticCast(pGpu, GpuHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;


    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xb8000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003f9UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B | GB20C */ 
    {
        return gsyncmgrIsFirmwareGPUMismatch_GB100(pGpu, pGsync);
    }
    // default
    else
    {
        return gsyncmgrIsFirmwareGPUMismatch_b2b553(pGpu, pGsync);
    }

    NV_ASSERT_FAILED("No hal impl found for gsyncmgrIsFirmwareGPUMismatch");

    return NV_FALSE;
}

// Initialize newly constructed object.
void __nvoc_init__OBJGSYNCMGR(OBJGSYNCMGR *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_Object;    // (obj) super
    pThis->__nvoc_pbase_OBJGSYNCMGR = pThis;    // (gsyncmgr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__Object(&pThis->__nvoc_base_Object);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJGSYNCMGR.metadata__Object;    // (obj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJGSYNCMGR;    // (gsyncmgr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJGSYNCMGR(pThis);
}

NV_STATUS __nvoc_objCreate_OBJGSYNCMGR(OBJGSYNCMGR **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    OBJGSYNCMGR *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(OBJGSYNCMGR));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(OBJGSYNCMGR));

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
    __nvoc_init__OBJGSYNCMGR(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_OBJGSYNCMGR(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_OBJGSYNCMGR_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_OBJGSYNCMGR_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(OBJGSYNCMGR));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJGSYNCMGR(OBJGSYNCMGR **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_OBJGSYNCMGR(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}

