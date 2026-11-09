#define NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_ras_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__670eb2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemapPending;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for RowRemapPending
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__RowRemapPending(RowRemapPending*);
void __nvoc_init_funcTable_RowRemapPending(RowRemapPending*);
NV_STATUS __nvoc_ctor_RowRemapPending(RowRemapPending*, struct EventBus *pEventBus, NvU32 source, NvU64 physicalAddress);
void __nvoc_init_dataField_RowRemapPending(RowRemapPending*);
void __nvoc_dtor_RowRemapPending(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemapPending;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemapPending;

// Down-thunk(s) to bridge RowRemapPending methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemapPending methods to ancestors (if any)

// Class-specific details for RowRemapPending
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemapPending = 
{
    .classInfo.size =               sizeof(RowRemapPending),
    .classInfo.classId =            classId(RowRemapPending),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemapPending",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemapPending,
    .pCastInfo =          &__nvoc_castinfo__RowRemapPending,
    .pExportInfo =        &__nvoc_export_info__RowRemapPending
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemapPending __nvoc_metadata__RowRemapPending = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemapPending,    // (rrpending) this
    .rtti.dtor      = &__nvoc_dtor_RowRemapPending,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(RowRemapPending, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemapPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemapPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemapPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemapPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemapPending = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemapPending.rtti,    // [0]: (rrpending) this
        &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemapPending = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemapPending object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_RowRemapPending(Dynamic* pThis) {

    RowRemapPending *__nvoc_this = (RowRemapPending *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemapPending(RowRemapPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemapPending object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_RowRemapPending(RowRemapPending *pEvent, struct EventBus *pEventBus, NvU32 source, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_PENDING, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, source);
    if (status != NV_OK) goto __nvoc_ctor_RowRemapPending_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemapPending(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrpendingConstruct(pEvent, pEventBus, source, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_RowRemapPending_fail__init;
    goto __nvoc_ctor_RowRemapPending_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemapPending_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_RowRemapPending_fail_GpuResourceRetirement:
__nvoc_ctor_RowRemapPending_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemapPending_1(RowRemapPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemapPending_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemapPending(RowRemapPending *pThis) {
    __nvoc_init_funcTable_RowRemapPending_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemapPending(RowRemapPending *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_RowRemapPending = pThis;    // (rrpending) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemapPending;    // (rrpending) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemapPending(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemapPending(RowRemapPending **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 source, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemapPending *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemapPending));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemapPending));

    __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemapPending(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemapPending(__nvoc_pThis, pEventBus, source, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemapPending_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemapPending_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemapPending));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemapPending(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 source = va_arg(__nvoc_args, NvU32);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_RowRemapPending((RowRemapPending **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, source, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0f7b49 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureTableFull;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for RowRemappingFailureTableFull
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__RowRemappingFailureTableFull(RowRemappingFailureTableFull*);
void __nvoc_init_funcTable_RowRemappingFailureTableFull(RowRemappingFailureTableFull*);
NV_STATUS __nvoc_ctor_RowRemappingFailureTableFull(RowRemappingFailureTableFull*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_RowRemappingFailureTableFull(RowRemappingFailureTableFull*);
void __nvoc_dtor_RowRemappingFailureTableFull(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureTableFull;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureTableFull;

// Down-thunk(s) to bridge RowRemappingFailureTableFull methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemappingFailureTableFull methods to ancestors (if any)

// Class-specific details for RowRemappingFailureTableFull
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureTableFull = 
{
    .classInfo.size =               sizeof(RowRemappingFailureTableFull),
    .classInfo.classId =            classId(RowRemappingFailureTableFull),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemappingFailureTableFull",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemappingFailureTableFull,
    .pCastInfo =          &__nvoc_castinfo__RowRemappingFailureTableFull,
    .pExportInfo =        &__nvoc_export_info__RowRemappingFailureTableFull
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemappingFailureTableFull __nvoc_metadata__RowRemappingFailureTableFull = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemappingFailureTableFull,    // (rrtablefull) this
    .rtti.dtor      = &__nvoc_dtor_RowRemappingFailureTableFull,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(RowRemappingFailureTableFull, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureTableFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureTableFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemappingFailureTableFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemappingFailureTableFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureTableFull = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemappingFailureTableFull.rtti,    // [0]: (rrtablefull) this
        &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureTableFull = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemappingFailureTableFull object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_RowRemappingFailureTableFull(Dynamic* pThis) {

    RowRemappingFailureTableFull *__nvoc_this = (RowRemappingFailureTableFull *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemappingFailureTableFull(RowRemappingFailureTableFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemappingFailureTableFull object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_RowRemappingFailureTableFull(RowRemappingFailureTableFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_TABLE_FULL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureTableFull_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemappingFailureTableFull(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrtablefullConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureTableFull_fail__init;
    goto __nvoc_ctor_RowRemappingFailureTableFull_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemappingFailureTableFull_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_RowRemappingFailureTableFull_fail_GpuResourceExhausted:
__nvoc_ctor_RowRemappingFailureTableFull_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemappingFailureTableFull_1(RowRemappingFailureTableFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemappingFailureTableFull_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemappingFailureTableFull(RowRemappingFailureTableFull *pThis) {
    __nvoc_init_funcTable_RowRemappingFailureTableFull_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemappingFailureTableFull(RowRemappingFailureTableFull *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_RowRemappingFailureTableFull = pThis;    // (rrtablefull) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureTableFull;    // (rrtablefull) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemappingFailureTableFull(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemappingFailureTableFull(RowRemappingFailureTableFull **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemappingFailureTableFull *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemappingFailureTableFull));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureTableFull));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemappingFailureTableFull(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemappingFailureTableFull(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemappingFailureTableFull_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemappingFailureTableFull_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureTableFull));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureTableFull(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_RowRemappingFailureTableFull((RowRemappingFailureTableFull **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__1e6334 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureBankFull;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for RowRemappingFailureBankFull
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__RowRemappingFailureBankFull(RowRemappingFailureBankFull*);
void __nvoc_init_funcTable_RowRemappingFailureBankFull(RowRemappingFailureBankFull*);
NV_STATUS __nvoc_ctor_RowRemappingFailureBankFull(RowRemappingFailureBankFull*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_RowRemappingFailureBankFull(RowRemappingFailureBankFull*);
void __nvoc_dtor_RowRemappingFailureBankFull(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureBankFull;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureBankFull;

// Down-thunk(s) to bridge RowRemappingFailureBankFull methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemappingFailureBankFull methods to ancestors (if any)

// Class-specific details for RowRemappingFailureBankFull
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureBankFull = 
{
    .classInfo.size =               sizeof(RowRemappingFailureBankFull),
    .classInfo.classId =            classId(RowRemappingFailureBankFull),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemappingFailureBankFull",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemappingFailureBankFull,
    .pCastInfo =          &__nvoc_castinfo__RowRemappingFailureBankFull,
    .pExportInfo =        &__nvoc_export_info__RowRemappingFailureBankFull
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemappingFailureBankFull __nvoc_metadata__RowRemappingFailureBankFull = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemappingFailureBankFull,    // (rrbankfull) this
    .rtti.dtor      = &__nvoc_dtor_RowRemappingFailureBankFull,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(RowRemappingFailureBankFull, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureBankFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureBankFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemappingFailureBankFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemappingFailureBankFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureBankFull = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemappingFailureBankFull.rtti,    // [0]: (rrbankfull) this
        &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureBankFull = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemappingFailureBankFull object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_RowRemappingFailureBankFull(Dynamic* pThis) {

    RowRemappingFailureBankFull *__nvoc_this = (RowRemappingFailureBankFull *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemappingFailureBankFull(RowRemappingFailureBankFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemappingFailureBankFull object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_RowRemappingFailureBankFull(RowRemappingFailureBankFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_BANK_FULL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureBankFull_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemappingFailureBankFull(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrbankfullConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureBankFull_fail__init;
    goto __nvoc_ctor_RowRemappingFailureBankFull_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemappingFailureBankFull_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_RowRemappingFailureBankFull_fail_GpuResourceExhausted:
__nvoc_ctor_RowRemappingFailureBankFull_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemappingFailureBankFull_1(RowRemappingFailureBankFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemappingFailureBankFull_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemappingFailureBankFull(RowRemappingFailureBankFull *pThis) {
    __nvoc_init_funcTable_RowRemappingFailureBankFull_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemappingFailureBankFull(RowRemappingFailureBankFull *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_RowRemappingFailureBankFull = pThis;    // (rrbankfull) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureBankFull;    // (rrbankfull) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemappingFailureBankFull(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemappingFailureBankFull(RowRemappingFailureBankFull **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemappingFailureBankFull *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemappingFailureBankFull));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureBankFull));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemappingFailureBankFull(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemappingFailureBankFull(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemappingFailureBankFull_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemappingFailureBankFull_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureBankFull));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureBankFull(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_RowRemappingFailureBankFull((RowRemappingFailureBankFull **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__fa1d6a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureReservedRow;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for RowRemappingFailureReservedRow
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__RowRemappingFailureReservedRow(RowRemappingFailureReservedRow*);
void __nvoc_init_funcTable_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow*);
NV_STATUS __nvoc_ctor_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow*);
void __nvoc_dtor_RowRemappingFailureReservedRow(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureReservedRow;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureReservedRow;

// Down-thunk(s) to bridge RowRemappingFailureReservedRow methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemappingFailureReservedRow methods to ancestors (if any)

// Class-specific details for RowRemappingFailureReservedRow
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureReservedRow = 
{
    .classInfo.size =               sizeof(RowRemappingFailureReservedRow),
    .classInfo.classId =            classId(RowRemappingFailureReservedRow),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemappingFailureReservedRow",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemappingFailureReservedRow,
    .pCastInfo =          &__nvoc_castinfo__RowRemappingFailureReservedRow,
    .pExportInfo =        &__nvoc_export_info__RowRemappingFailureReservedRow
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemappingFailureReservedRow __nvoc_metadata__RowRemappingFailureReservedRow = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemappingFailureReservedRow,    // (rrreservedrow) this
    .rtti.dtor      = &__nvoc_dtor_RowRemappingFailureReservedRow,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(RowRemappingFailureReservedRow, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureReservedRow, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureReservedRow, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemappingFailureReservedRow, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemappingFailureReservedRow, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureReservedRow = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemappingFailureReservedRow.rtti,    // [0]: (rrreservedrow) this
        &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureReservedRow = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemappingFailureReservedRow object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_RowRemappingFailureReservedRow(Dynamic* pThis) {

    RowRemappingFailureReservedRow *__nvoc_this = (RowRemappingFailureReservedRow *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemappingFailureReservedRow object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_RESERVED_ROW, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureReservedRow_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemappingFailureReservedRow(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrreservedrowConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureReservedRow_fail__init;
    goto __nvoc_ctor_RowRemappingFailureReservedRow_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemappingFailureReservedRow_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_RowRemappingFailureReservedRow_fail_GpuResourceExhausted:
__nvoc_ctor_RowRemappingFailureReservedRow_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemappingFailureReservedRow_1(RowRemappingFailureReservedRow *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemappingFailureReservedRow_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow *pThis) {
    __nvoc_init_funcTable_RowRemappingFailureReservedRow_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemappingFailureReservedRow(RowRemappingFailureReservedRow *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_RowRemappingFailureReservedRow = pThis;    // (rrreservedrow) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureReservedRow;    // (rrreservedrow) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemappingFailureReservedRow(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemappingFailureReservedRow *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemappingFailureReservedRow));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureReservedRow));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemappingFailureReservedRow(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemappingFailureReservedRow(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemappingFailureReservedRow_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemappingFailureReservedRow_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureReservedRow));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureReservedRow(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_RowRemappingFailureReservedRow((RowRemappingFailureReservedRow **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__fd614f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureInternalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError;

// Forward declarations for RowRemappingFailureInternalError
void __nvoc_init__GpuUnclassifiedError(GpuUnclassifiedError*);
void __nvoc_init__RowRemappingFailureInternalError(RowRemappingFailureInternalError*);
void __nvoc_init_funcTable_RowRemappingFailureInternalError(RowRemappingFailureInternalError*);
NV_STATUS __nvoc_ctor_RowRemappingFailureInternalError(RowRemappingFailureInternalError*, struct EventBus *pEventBus, NvU64 physicalAddress, const char *pErrorString);
void __nvoc_init_dataField_RowRemappingFailureInternalError(RowRemappingFailureInternalError*);
void __nvoc_dtor_RowRemappingFailureInternalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureInternalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureInternalError;

// Down-thunk(s) to bridge RowRemappingFailureInternalError methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemappingFailureInternalError methods to ancestors (if any)

// Class-specific details for RowRemappingFailureInternalError
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureInternalError = 
{
    .classInfo.size =               sizeof(RowRemappingFailureInternalError),
    .classInfo.classId =            classId(RowRemappingFailureInternalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemappingFailureInternalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemappingFailureInternalError,
    .pCastInfo =          &__nvoc_castinfo__RowRemappingFailureInternalError,
    .pExportInfo =        &__nvoc_export_info__RowRemappingFailureInternalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemappingFailureInternalError __nvoc_metadata__RowRemappingFailureInternalError = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemappingFailureInternalError,    // (rrinternalerr) this
    .rtti.dtor      = &__nvoc_dtor_RowRemappingFailureInternalError,
    .rtti.offset    = 0,
    .metadata__GpuUnclassifiedError.rtti.pClassDef = &__nvoc_class_def_GpuUnclassifiedError,    // (gpuunclassifiederr) super
    .metadata__GpuUnclassifiedError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.rtti.offset    = NV_OFFSETOF(RowRemappingFailureInternalError, __nvoc_base_GpuUnclassifiedError),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemappingFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemappingFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingFailureInternalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemappingFailureInternalError.rtti,    // [0]: (rrinternalerr) this
        &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.rtti,    // [1]: (gpuunclassifiederr) super
        &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingFailureInternalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemappingFailureInternalError object.
void __nvoc_dtor_GpuUnclassifiedError(Dynamic*);
void __nvoc_dtor_RowRemappingFailureInternalError(Dynamic* pThis) {

    RowRemappingFailureInternalError *__nvoc_this = (RowRemappingFailureInternalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *) &__nvoc_this->__nvoc_base_GpuUnclassifiedError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemappingFailureInternalError(RowRemappingFailureInternalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemappingFailureInternalError object.
NV_STATUS __nvoc_ctor_GpuUnclassifiedError(GpuUnclassifiedError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_RowRemappingFailureInternalError(RowRemappingFailureInternalError *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress, const char *pErrorString) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuUnclassifiedError(&pEvent->__nvoc_base_GpuUnclassifiedError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_INTERNAL_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureInternalError_fail_GpuUnclassifiedError;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemappingFailureInternalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrinternalerrConstruct(pEvent, pEventBus, physicalAddress, pErrorString);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingFailureInternalError_fail__init;
    goto __nvoc_ctor_RowRemappingFailureInternalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemappingFailureInternalError_fail__init:
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *)&pEvent->__nvoc_base_GpuUnclassifiedError);
__nvoc_ctor_RowRemappingFailureInternalError_fail_GpuUnclassifiedError:
__nvoc_ctor_RowRemappingFailureInternalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemappingFailureInternalError_1(RowRemappingFailureInternalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemappingFailureInternalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemappingFailureInternalError(RowRemappingFailureInternalError *pThis) {
    __nvoc_init_funcTable_RowRemappingFailureInternalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemappingFailureInternalError(RowRemappingFailureInternalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuUnclassifiedError = &pThis->__nvoc_base_GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_pbase_RowRemappingFailureInternalError = pThis;    // (rrinternalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuUnclassifiedError(&pThis->__nvoc_base_GpuUnclassifiedError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError.metadata__GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingFailureInternalError;    // (rrinternalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemappingFailureInternalError(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemappingFailureInternalError(RowRemappingFailureInternalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress, const char *pErrorString)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemappingFailureInternalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemappingFailureInternalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureInternalError));

    __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemappingFailureInternalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemappingFailureInternalError(__nvoc_pThis, pEventBus, physicalAddress, pErrorString);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemappingFailureInternalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemappingFailureInternalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingFailureInternalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureInternalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);
    const char *pErrorString = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_RowRemappingFailureInternalError((RowRemappingFailureInternalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress, pErrorString);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a09afc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingPageOfflineFailure;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError;

// Forward declarations for RowRemappingPageOfflineFailure
void __nvoc_init__GpuUnclassifiedError(GpuUnclassifiedError*);
void __nvoc_init__RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure*);
void __nvoc_init_funcTable_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure*);
NV_STATUS __nvoc_ctor_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure*);
void __nvoc_dtor_RowRemappingPageOfflineFailure(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingPageOfflineFailure;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingPageOfflineFailure;

// Down-thunk(s) to bridge RowRemappingPageOfflineFailure methods from ancestors (if any)

// Up-thunk(s) to bridge RowRemappingPageOfflineFailure methods to ancestors (if any)

// Class-specific details for RowRemappingPageOfflineFailure
const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingPageOfflineFailure = 
{
    .classInfo.size =               sizeof(RowRemappingPageOfflineFailure),
    .classInfo.classId =            classId(RowRemappingPageOfflineFailure),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RowRemappingPageOfflineFailure",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RowRemappingPageOfflineFailure,
    .pCastInfo =          &__nvoc_castinfo__RowRemappingPageOfflineFailure,
    .pExportInfo =        &__nvoc_export_info__RowRemappingPageOfflineFailure
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RowRemappingPageOfflineFailure __nvoc_metadata__RowRemappingPageOfflineFailure = {
    .rtti.pClassDef = &__nvoc_class_def_RowRemappingPageOfflineFailure,    // (rrpageofflinefailure) this
    .rtti.dtor      = &__nvoc_dtor_RowRemappingPageOfflineFailure,
    .rtti.offset    = 0,
    .metadata__GpuUnclassifiedError.rtti.pClassDef = &__nvoc_class_def_GpuUnclassifiedError,    // (gpuunclassifiederr) super
    .metadata__GpuUnclassifiedError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.rtti.offset    = NV_OFFSETOF(RowRemappingPageOfflineFailure, __nvoc_base_GpuUnclassifiedError),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingPageOfflineFailure, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(RowRemappingPageOfflineFailure, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(RowRemappingPageOfflineFailure, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(RowRemappingPageOfflineFailure, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RowRemappingPageOfflineFailure = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__RowRemappingPageOfflineFailure.rtti,    // [0]: (rrpageofflinefailure) this
        &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.rtti,    // [1]: (gpuunclassifiederr) super
        &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__RowRemappingPageOfflineFailure = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RowRemappingPageOfflineFailure object.
void __nvoc_dtor_GpuUnclassifiedError(Dynamic*);
void __nvoc_dtor_RowRemappingPageOfflineFailure(Dynamic* pThis) {

    RowRemappingPageOfflineFailure *__nvoc_this = (RowRemappingPageOfflineFailure *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *) &__nvoc_this->__nvoc_base_GpuUnclassifiedError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RowRemappingPageOfflineFailure object.
NV_STATUS __nvoc_ctor_GpuUnclassifiedError(GpuUnclassifiedError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuUnclassifiedError(&pEvent->__nvoc_base_GpuUnclassifiedError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_PAGE_OFFLINE_FAILURE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingPageOfflineFailure_fail_GpuUnclassifiedError;

    // Initialize data fields.
    __nvoc_init_dataField_RowRemappingPageOfflineFailure(pEvent);

    // Call the constructor for this class.
    status = __nvoc_rrpageofflinefailureConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_RowRemappingPageOfflineFailure_fail__init;
    goto __nvoc_ctor_RowRemappingPageOfflineFailure_exit; // Success

    // Unwind on error.
__nvoc_ctor_RowRemappingPageOfflineFailure_fail__init:
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *)&pEvent->__nvoc_base_GpuUnclassifiedError);
__nvoc_ctor_RowRemappingPageOfflineFailure_fail_GpuUnclassifiedError:
__nvoc_ctor_RowRemappingPageOfflineFailure_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RowRemappingPageOfflineFailure_1(RowRemappingPageOfflineFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RowRemappingPageOfflineFailure_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure *pThis) {
    __nvoc_init_funcTable_RowRemappingPageOfflineFailure_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuUnclassifiedError = &pThis->__nvoc_base_GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_pbase_RowRemappingPageOfflineFailure = pThis;    // (rrpageofflinefailure) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuUnclassifiedError(&pThis->__nvoc_base_GpuUnclassifiedError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure.metadata__GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RowRemappingPageOfflineFailure;    // (rrpageofflinefailure) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RowRemappingPageOfflineFailure(pThis);
}

NV_STATUS __nvoc_objCreate_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RowRemappingPageOfflineFailure *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RowRemappingPageOfflineFailure));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingPageOfflineFailure));

    __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RowRemappingPageOfflineFailure(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RowRemappingPageOfflineFailure(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RowRemappingPageOfflineFailure_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RowRemappingPageOfflineFailure_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RowRemappingPageOfflineFailure));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RowRemappingPageOfflineFailure(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_RowRemappingPageOfflineFailure((RowRemappingPageOfflineFailure **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ffd4de = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInternalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError;

// Forward declarations for DramRetirementFailureInternalError
void __nvoc_init__GpuUnclassifiedError(GpuUnclassifiedError*);
void __nvoc_init__DramRetirementFailureInternalError(DramRetirementFailureInternalError*);
void __nvoc_init_funcTable_DramRetirementFailureInternalError(DramRetirementFailureInternalError*);
NV_STATUS __nvoc_ctor_DramRetirementFailureInternalError(DramRetirementFailureInternalError*, struct EventBus *pEventBus, NvU64 physicalAddress, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason);
void __nvoc_init_dataField_DramRetirementFailureInternalError(DramRetirementFailureInternalError*);
void __nvoc_dtor_DramRetirementFailureInternalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureInternalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureInternalError;

// Down-thunk(s) to bridge DramRetirementFailureInternalError methods from ancestors (if any)

// Up-thunk(s) to bridge DramRetirementFailureInternalError methods to ancestors (if any)

// Class-specific details for DramRetirementFailureInternalError
const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInternalError = 
{
    .classInfo.size =               sizeof(DramRetirementFailureInternalError),
    .classInfo.classId =            classId(DramRetirementFailureInternalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramRetirementFailureInternalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramRetirementFailureInternalError,
    .pCastInfo =          &__nvoc_castinfo__DramRetirementFailureInternalError,
    .pExportInfo =        &__nvoc_export_info__DramRetirementFailureInternalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramRetirementFailureInternalError __nvoc_metadata__DramRetirementFailureInternalError = {
    .rtti.pClassDef = &__nvoc_class_def_DramRetirementFailureInternalError,    // (drinternalerr) this
    .rtti.dtor      = &__nvoc_dtor_DramRetirementFailureInternalError,
    .rtti.offset    = 0,
    .metadata__GpuUnclassifiedError.rtti.pClassDef = &__nvoc_class_def_GpuUnclassifiedError,    // (gpuunclassifiederr) super
    .metadata__GpuUnclassifiedError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInternalError, __nvoc_base_GpuUnclassifiedError),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInternalError, __nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureInternalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramRetirementFailureInternalError.rtti,    // [0]: (drinternalerr) this
        &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.rtti,    // [1]: (gpuunclassifiederr) super
        &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureInternalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramRetirementFailureInternalError object.
void __nvoc_dtor_GpuUnclassifiedError(Dynamic*);
void __nvoc_dtor_DramRetirementFailureInternalError(Dynamic* pThis) {

    DramRetirementFailureInternalError *__nvoc_this = (DramRetirementFailureInternalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *) &__nvoc_this->__nvoc_base_GpuUnclassifiedError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramRetirementFailureInternalError(DramRetirementFailureInternalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramRetirementFailureInternalError object.
NV_STATUS __nvoc_ctor_GpuUnclassifiedError(GpuUnclassifiedError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_DramRetirementFailureInternalError(DramRetirementFailureInternalError *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuUnclassifiedError(&pEvent->__nvoc_base_GpuUnclassifiedError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureInternalError_fail_GpuUnclassifiedError;

    // Initialize data fields.
    __nvoc_init_dataField_DramRetirementFailureInternalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_drinternalerrConstruct(pEvent, pEventBus, physicalAddress, errorReason);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureInternalError_fail__init;
    goto __nvoc_ctor_DramRetirementFailureInternalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramRetirementFailureInternalError_fail__init:
    __nvoc_dtor_GpuUnclassifiedError((Dynamic *)&pEvent->__nvoc_base_GpuUnclassifiedError);
__nvoc_ctor_DramRetirementFailureInternalError_fail_GpuUnclassifiedError:
__nvoc_ctor_DramRetirementFailureInternalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramRetirementFailureInternalError_1(DramRetirementFailureInternalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramRetirementFailureInternalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramRetirementFailureInternalError(DramRetirementFailureInternalError *pThis) {
    __nvoc_init_funcTable_DramRetirementFailureInternalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramRetirementFailureInternalError(DramRetirementFailureInternalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuUnclassifiedError = &pThis->__nvoc_base_GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_pbase_DramRetirementFailureInternalError = pThis;    // (drinternalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuUnclassifiedError(&pThis->__nvoc_base_GpuUnclassifiedError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError.metadata__GpuUnclassifiedError;    // (gpuunclassifiederr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInternalError;    // (drinternalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramRetirementFailureInternalError(pThis);
}

NV_STATUS __nvoc_objCreate_DramRetirementFailureInternalError(DramRetirementFailureInternalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramRetirementFailureInternalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramRetirementFailureInternalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureInternalError));

    __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramRetirementFailureInternalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramRetirementFailureInternalError(__nvoc_pThis, pEventBus, physicalAddress, errorReason);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramRetirementFailureInternalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramRetirementFailureInternalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuUnclassifiedError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureInternalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureInternalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason = va_arg(__nvoc_args, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON);

    return __nvoc_objCreate_DramRetirementFailureInternalError((DramRetirementFailureInternalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress, errorReason);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__aeaec2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInforomFull;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for DramRetirementFailureInforomFull
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__DramRetirementFailureInforomFull(DramRetirementFailureInforomFull*);
void __nvoc_init_funcTable_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull*);
NV_STATUS __nvoc_ctor_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull*);
void __nvoc_dtor_DramRetirementFailureInforomFull(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureInforomFull;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureInforomFull;

// Down-thunk(s) to bridge DramRetirementFailureInforomFull methods from ancestors (if any)

// Up-thunk(s) to bridge DramRetirementFailureInforomFull methods to ancestors (if any)

// Class-specific details for DramRetirementFailureInforomFull
const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInforomFull = 
{
    .classInfo.size =               sizeof(DramRetirementFailureInforomFull),
    .classInfo.classId =            classId(DramRetirementFailureInforomFull),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramRetirementFailureInforomFull",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramRetirementFailureInforomFull,
    .pCastInfo =          &__nvoc_castinfo__DramRetirementFailureInforomFull,
    .pExportInfo =        &__nvoc_export_info__DramRetirementFailureInforomFull
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramRetirementFailureInforomFull __nvoc_metadata__DramRetirementFailureInforomFull = {
    .rtti.pClassDef = &__nvoc_class_def_DramRetirementFailureInforomFull,    // (drinforomfull) this
    .rtti.dtor      = &__nvoc_dtor_DramRetirementFailureInforomFull,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInforomFull, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInforomFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInforomFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInforomFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramRetirementFailureInforomFull, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureInforomFull = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramRetirementFailureInforomFull.rtti,    // [0]: (drinforomfull) this
        &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureInforomFull = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramRetirementFailureInforomFull object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_DramRetirementFailureInforomFull(Dynamic* pThis) {

    DramRetirementFailureInforomFull *__nvoc_this = (DramRetirementFailureInforomFull *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramRetirementFailureInforomFull object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_INFOROM_FULL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureInforomFull_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_DramRetirementFailureInforomFull(pEvent);

    // Call the constructor for this class.
    status = __nvoc_drinforomfullConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureInforomFull_fail__init;
    goto __nvoc_ctor_DramRetirementFailureInforomFull_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramRetirementFailureInforomFull_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_DramRetirementFailureInforomFull_fail_GpuResourceExhausted:
__nvoc_ctor_DramRetirementFailureInforomFull_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramRetirementFailureInforomFull_1(DramRetirementFailureInforomFull *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramRetirementFailureInforomFull_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull *pThis) {
    __nvoc_init_funcTable_DramRetirementFailureInforomFull_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramRetirementFailureInforomFull(DramRetirementFailureInforomFull *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_DramRetirementFailureInforomFull = pThis;    // (drinforomfull) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureInforomFull;    // (drinforomfull) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramRetirementFailureInforomFull(pThis);
}

NV_STATUS __nvoc_objCreate_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramRetirementFailureInforomFull *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramRetirementFailureInforomFull));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureInforomFull));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramRetirementFailureInforomFull(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramRetirementFailureInforomFull(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramRetirementFailureInforomFull_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramRetirementFailureInforomFull_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureInforomFull));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureInforomFull(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_DramRetirementFailureInforomFull((DramRetirementFailureInforomFull **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__84944c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureHwLimit;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for DramRetirementFailureHwLimit
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__DramRetirementFailureHwLimit(DramRetirementFailureHwLimit*);
void __nvoc_init_funcTable_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit*);
NV_STATUS __nvoc_ctor_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit*);
void __nvoc_dtor_DramRetirementFailureHwLimit(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureHwLimit;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureHwLimit;

// Down-thunk(s) to bridge DramRetirementFailureHwLimit methods from ancestors (if any)

// Up-thunk(s) to bridge DramRetirementFailureHwLimit methods to ancestors (if any)

// Class-specific details for DramRetirementFailureHwLimit
const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureHwLimit = 
{
    .classInfo.size =               sizeof(DramRetirementFailureHwLimit),
    .classInfo.classId =            classId(DramRetirementFailureHwLimit),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramRetirementFailureHwLimit",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramRetirementFailureHwLimit,
    .pCastInfo =          &__nvoc_castinfo__DramRetirementFailureHwLimit,
    .pExportInfo =        &__nvoc_export_info__DramRetirementFailureHwLimit
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramRetirementFailureHwLimit __nvoc_metadata__DramRetirementFailureHwLimit = {
    .rtti.pClassDef = &__nvoc_class_def_DramRetirementFailureHwLimit,    // (drhwlimit) this
    .rtti.dtor      = &__nvoc_dtor_DramRetirementFailureHwLimit,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(DramRetirementFailureHwLimit, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureHwLimit, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureHwLimit, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramRetirementFailureHwLimit, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramRetirementFailureHwLimit, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureHwLimit = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramRetirementFailureHwLimit.rtti,    // [0]: (drhwlimit) this
        &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureHwLimit = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramRetirementFailureHwLimit object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_DramRetirementFailureHwLimit(Dynamic* pThis) {

    DramRetirementFailureHwLimit *__nvoc_this = (DramRetirementFailureHwLimit *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramRetirementFailureHwLimit object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_HW_LIMIT, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureHwLimit_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_DramRetirementFailureHwLimit(pEvent);

    // Call the constructor for this class.
    status = __nvoc_drhwlimitConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureHwLimit_fail__init;
    goto __nvoc_ctor_DramRetirementFailureHwLimit_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramRetirementFailureHwLimit_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_DramRetirementFailureHwLimit_fail_GpuResourceExhausted:
__nvoc_ctor_DramRetirementFailureHwLimit_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramRetirementFailureHwLimit_1(DramRetirementFailureHwLimit *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramRetirementFailureHwLimit_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit *pThis) {
    __nvoc_init_funcTable_DramRetirementFailureHwLimit_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramRetirementFailureHwLimit(DramRetirementFailureHwLimit *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_DramRetirementFailureHwLimit = pThis;    // (drhwlimit) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureHwLimit;    // (drhwlimit) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramRetirementFailureHwLimit(pThis);
}

NV_STATUS __nvoc_objCreate_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramRetirementFailureHwLimit *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramRetirementFailureHwLimit));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureHwLimit));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramRetirementFailureHwLimit(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramRetirementFailureHwLimit(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramRetirementFailureHwLimit_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramRetirementFailureHwLimit_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureHwLimit));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureHwLimit(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_DramRetirementFailureHwLimit((DramRetirementFailureHwLimit **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4da508 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureNoSpare;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for DramRetirementFailureNoSpare
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__DramRetirementFailureNoSpare(DramRetirementFailureNoSpare*);
void __nvoc_init_funcTable_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare*);
NV_STATUS __nvoc_ctor_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare*, struct EventBus *pEventBus, NvU64 physicalAddress);
void __nvoc_init_dataField_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare*);
void __nvoc_dtor_DramRetirementFailureNoSpare(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureNoSpare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureNoSpare;

// Down-thunk(s) to bridge DramRetirementFailureNoSpare methods from ancestors (if any)

// Up-thunk(s) to bridge DramRetirementFailureNoSpare methods to ancestors (if any)

// Class-specific details for DramRetirementFailureNoSpare
const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureNoSpare = 
{
    .classInfo.size =               sizeof(DramRetirementFailureNoSpare),
    .classInfo.classId =            classId(DramRetirementFailureNoSpare),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramRetirementFailureNoSpare",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramRetirementFailureNoSpare,
    .pCastInfo =          &__nvoc_castinfo__DramRetirementFailureNoSpare,
    .pExportInfo =        &__nvoc_export_info__DramRetirementFailureNoSpare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramRetirementFailureNoSpare __nvoc_metadata__DramRetirementFailureNoSpare = {
    .rtti.pClassDef = &__nvoc_class_def_DramRetirementFailureNoSpare,    // (drnospare) this
    .rtti.dtor      = &__nvoc_dtor_DramRetirementFailureNoSpare,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(DramRetirementFailureNoSpare, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramRetirementFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramRetirementFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramRetirementFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramRetirementFailureNoSpare = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramRetirementFailureNoSpare.rtti,    // [0]: (drnospare) this
        &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramRetirementFailureNoSpare = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramRetirementFailureNoSpare object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_DramRetirementFailureNoSpare(Dynamic* pThis) {

    DramRetirementFailureNoSpare *__nvoc_this = (DramRetirementFailureNoSpare *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramRetirementFailureNoSpare object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_RETIREMENT_FAILURE_NO_SPARE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureNoSpare_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_DramRetirementFailureNoSpare(pEvent);

    // Call the constructor for this class.
    status = __nvoc_drnospareConstruct(pEvent, pEventBus, physicalAddress);
    if (status != NV_OK) goto __nvoc_ctor_DramRetirementFailureNoSpare_fail__init;
    goto __nvoc_ctor_DramRetirementFailureNoSpare_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramRetirementFailureNoSpare_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_DramRetirementFailureNoSpare_fail_GpuResourceExhausted:
__nvoc_ctor_DramRetirementFailureNoSpare_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramRetirementFailureNoSpare_1(DramRetirementFailureNoSpare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramRetirementFailureNoSpare_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare *pThis) {
    __nvoc_init_funcTable_DramRetirementFailureNoSpare_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramRetirementFailureNoSpare(DramRetirementFailureNoSpare *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_DramRetirementFailureNoSpare = pThis;    // (drnospare) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramRetirementFailureNoSpare;    // (drnospare) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramRetirementFailureNoSpare(pThis);
}

NV_STATUS __nvoc_objCreate_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 physicalAddress)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramRetirementFailureNoSpare *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramRetirementFailureNoSpare));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureNoSpare));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramRetirementFailureNoSpare(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramRetirementFailureNoSpare(__nvoc_pThis, pEventBus, physicalAddress);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramRetirementFailureNoSpare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramRetirementFailureNoSpare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramRetirementFailureNoSpare));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureNoSpare(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_DramRetirementFailureNoSpare((DramRetirementFailureNoSpare **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__25375b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BankRemappingPending;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for BankRemappingPending
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__BankRemappingPending(BankRemappingPending*);
void __nvoc_init_funcTable_BankRemappingPending(BankRemappingPending*);
NV_STATUS __nvoc_ctor_BankRemappingPending(BankRemappingPending*, struct EventBus *pEventBus, NvU32 source);
void __nvoc_init_dataField_BankRemappingPending(BankRemappingPending*);
void __nvoc_dtor_BankRemappingPending(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__BankRemappingPending;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__BankRemappingPending;

// Down-thunk(s) to bridge BankRemappingPending methods from ancestors (if any)

// Up-thunk(s) to bridge BankRemappingPending methods to ancestors (if any)

// Class-specific details for BankRemappingPending
const struct NVOC_CLASS_DEF __nvoc_class_def_BankRemappingPending = 
{
    .classInfo.size =               sizeof(BankRemappingPending),
    .classInfo.classId =            classId(BankRemappingPending),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "BankRemappingPending",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_BankRemappingPending,
    .pCastInfo =          &__nvoc_castinfo__BankRemappingPending,
    .pExportInfo =        &__nvoc_export_info__BankRemappingPending
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__BankRemappingPending __nvoc_metadata__BankRemappingPending = {
    .rtti.pClassDef = &__nvoc_class_def_BankRemappingPending,    // (brpending) this
    .rtti.dtor      = &__nvoc_dtor_BankRemappingPending,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(BankRemappingPending, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(BankRemappingPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(BankRemappingPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(BankRemappingPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(BankRemappingPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__BankRemappingPending = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__BankRemappingPending.rtti,    // [0]: (brpending) this
        &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__BankRemappingPending = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct BankRemappingPending object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_BankRemappingPending(Dynamic* pThis) {

    BankRemappingPending *__nvoc_this = (BankRemappingPending *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_BankRemappingPending(BankRemappingPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct BankRemappingPending object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_BankRemappingPending(BankRemappingPending *pEvent, struct EventBus *pEventBus, NvU32 source) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_BANK_REMAPPING_PENDING, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, source);
    if (status != NV_OK) goto __nvoc_ctor_BankRemappingPending_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_BankRemappingPending(pEvent);

    // Call the constructor for this class.
    status = __nvoc_brpendingConstruct(pEvent, pEventBus, source);
    if (status != NV_OK) goto __nvoc_ctor_BankRemappingPending_fail__init;
    goto __nvoc_ctor_BankRemappingPending_exit; // Success

    // Unwind on error.
__nvoc_ctor_BankRemappingPending_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_BankRemappingPending_fail_GpuResourceRetirement:
__nvoc_ctor_BankRemappingPending_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_BankRemappingPending_1(BankRemappingPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_BankRemappingPending_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_BankRemappingPending(BankRemappingPending *pThis) {
    __nvoc_init_funcTable_BankRemappingPending_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__BankRemappingPending(BankRemappingPending *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_BankRemappingPending = pThis;    // (brpending) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__BankRemappingPending;    // (brpending) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_BankRemappingPending(pThis);
}

NV_STATUS __nvoc_objCreate_BankRemappingPending(BankRemappingPending **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 source)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    BankRemappingPending *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(BankRemappingPending));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(BankRemappingPending));

    __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__BankRemappingPending(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_BankRemappingPending(__nvoc_pThis, pEventBus, source);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_BankRemappingPending_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_BankRemappingPending_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(BankRemappingPending));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_BankRemappingPending(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 source = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_BankRemappingPending((BankRemappingPending **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, source);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ff6030 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairPending;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for LtsRepairPending
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__LtsRepairPending(LtsRepairPending*);
void __nvoc_init_funcTable_LtsRepairPending(LtsRepairPending*);
NV_STATUS __nvoc_ctor_LtsRepairPending(LtsRepairPending*, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
void __nvoc_init_dataField_LtsRepairPending(LtsRepairPending*);
void __nvoc_dtor_LtsRepairPending(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtsRepairPending;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtsRepairPending;

// Down-thunk(s) to bridge LtsRepairPending methods from ancestors (if any)

// Up-thunk(s) to bridge LtsRepairPending methods to ancestors (if any)

// Class-specific details for LtsRepairPending
const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairPending = 
{
    .classInfo.size =               sizeof(LtsRepairPending),
    .classInfo.classId =            classId(LtsRepairPending),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtsRepairPending",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtsRepairPending,
    .pCastInfo =          &__nvoc_castinfo__LtsRepairPending,
    .pExportInfo =        &__nvoc_export_info__LtsRepairPending
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtsRepairPending __nvoc_metadata__LtsRepairPending = {
    .rtti.pClassDef = &__nvoc_class_def_LtsRepairPending,    // (ltsrepairpending) this
    .rtti.dtor      = &__nvoc_dtor_LtsRepairPending,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(LtsRepairPending, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtsRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtsRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtsRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtsRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtsRepairPending = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtsRepairPending.rtti,    // [0]: (ltsrepairpending) this
        &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtsRepairPending = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtsRepairPending object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_LtsRepairPending(Dynamic* pThis) {

    LtsRepairPending *__nvoc_this = (LtsRepairPending *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtsRepairPending(LtsRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtsRepairPending object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_LtsRepairPending(LtsRepairPending *pEvent, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTS_REPAIR_PENDING, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT);
    if (status != NV_OK) goto __nvoc_ctor_LtsRepairPending_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_LtsRepairPending(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltsrepairpendingConstruct(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString);
    if (status != NV_OK) goto __nvoc_ctor_LtsRepairPending_fail__init;
    goto __nvoc_ctor_LtsRepairPending_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtsRepairPending_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_LtsRepairPending_fail_GpuResourceRetirement:
__nvoc_ctor_LtsRepairPending_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtsRepairPending_1(LtsRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtsRepairPending_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtsRepairPending(LtsRepairPending *pThis) {
    __nvoc_init_funcTable_LtsRepairPending_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtsRepairPending(LtsRepairPending *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_LtsRepairPending = pThis;    // (ltsrepairpending) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairPending;    // (ltsrepairpending) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtsRepairPending(pThis);
}

NV_STATUS __nvoc_objCreate_LtsRepairPending(LtsRepairPending **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtsRepairPending *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtsRepairPending));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtsRepairPending));

    __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtsRepairPending(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtsRepairPending(__nvoc_pThis, pEventBus, sublocation, location, recoveryAction, pErrorString);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtsRepairPending_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtsRepairPending_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtsRepairPending));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtsRepairPending(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 sublocation = va_arg(__nvoc_args, NvU32);
    NvU32 location = va_arg(__nvoc_args, NvU32);
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction = va_arg(__nvoc_args, NV2080_CTRL_GPU_RECOVERY_ACTION);
    const char *pErrorString = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_LtsRepairPending((LtsRepairPending **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, sublocation, location, recoveryAction, pErrorString);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e88f92 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairPending;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for MemoryChannelRepairPending
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__MemoryChannelRepairPending(MemoryChannelRepairPending*);
void __nvoc_init_funcTable_MemoryChannelRepairPending(MemoryChannelRepairPending*);
NV_STATUS __nvoc_ctor_MemoryChannelRepairPending(MemoryChannelRepairPending*, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
void __nvoc_init_dataField_MemoryChannelRepairPending(MemoryChannelRepairPending*);
void __nvoc_dtor_MemoryChannelRepairPending(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryChannelRepairPending;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryChannelRepairPending;

// Down-thunk(s) to bridge MemoryChannelRepairPending methods from ancestors (if any)

// Up-thunk(s) to bridge MemoryChannelRepairPending methods to ancestors (if any)

// Class-specific details for MemoryChannelRepairPending
const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairPending = 
{
    .classInfo.size =               sizeof(MemoryChannelRepairPending),
    .classInfo.classId =            classId(MemoryChannelRepairPending),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MemoryChannelRepairPending",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MemoryChannelRepairPending,
    .pCastInfo =          &__nvoc_castinfo__MemoryChannelRepairPending,
    .pExportInfo =        &__nvoc_export_info__MemoryChannelRepairPending
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__MemoryChannelRepairPending __nvoc_metadata__MemoryChannelRepairPending = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryChannelRepairPending,    // (memorychannelrepairpending) this
    .rtti.dtor      = &__nvoc_dtor_MemoryChannelRepairPending,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairPending, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryChannelRepairPending = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemoryChannelRepairPending.rtti,    // [0]: (memorychannelrepairpending) this
        &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryChannelRepairPending = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct MemoryChannelRepairPending object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_MemoryChannelRepairPending(Dynamic* pThis) {

    MemoryChannelRepairPending *__nvoc_this = (MemoryChannelRepairPending *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MemoryChannelRepairPending(MemoryChannelRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MemoryChannelRepairPending object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_MemoryChannelRepairPending(MemoryChannelRepairPending *pEvent, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_MEMORY_CHANNEL_REPAIR_PENDING, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT);
    if (status != NV_OK) goto __nvoc_ctor_MemoryChannelRepairPending_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_MemoryChannelRepairPending(pEvent);

    // Call the constructor for this class.
    status = __nvoc_memorychannelrepairpendingConstruct(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString);
    if (status != NV_OK) goto __nvoc_ctor_MemoryChannelRepairPending_fail__init;
    goto __nvoc_ctor_MemoryChannelRepairPending_exit; // Success

    // Unwind on error.
__nvoc_ctor_MemoryChannelRepairPending_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_MemoryChannelRepairPending_fail_GpuResourceRetirement:
__nvoc_ctor_MemoryChannelRepairPending_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryChannelRepairPending_1(MemoryChannelRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MemoryChannelRepairPending_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_MemoryChannelRepairPending(MemoryChannelRepairPending *pThis) {
    __nvoc_init_funcTable_MemoryChannelRepairPending_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryChannelRepairPending(MemoryChannelRepairPending *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_MemoryChannelRepairPending = pThis;    // (memorychannelrepairpending) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairPending;    // (memorychannelrepairpending) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryChannelRepairPending(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryChannelRepairPending(MemoryChannelRepairPending **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MemoryChannelRepairPending *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MemoryChannelRepairPending));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MemoryChannelRepairPending));

    __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MemoryChannelRepairPending(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MemoryChannelRepairPending(__nvoc_pThis, pEventBus, sublocation, location, recoveryAction, pErrorString);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MemoryChannelRepairPending_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemoryChannelRepairPending_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MemoryChannelRepairPending));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryChannelRepairPending(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 sublocation = va_arg(__nvoc_args, NvU32);
    NvU32 location = va_arg(__nvoc_args, NvU32);
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction = va_arg(__nvoc_args, NV2080_CTRL_GPU_RECOVERY_ACTION);
    const char *pErrorString = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_MemoryChannelRepairPending((MemoryChannelRepairPending **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, sublocation, location, recoveryAction, pErrorString);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__7df945 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairFailure;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for LtsRepairFailure
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__LtsRepairFailure(LtsRepairFailure*);
void __nvoc_init_funcTable_LtsRepairFailure(LtsRepairFailure*);
NV_STATUS __nvoc_ctor_LtsRepairFailure(LtsRepairFailure*, struct EventBus *pEventBus);
void __nvoc_init_dataField_LtsRepairFailure(LtsRepairFailure*);
void __nvoc_dtor_LtsRepairFailure(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtsRepairFailure;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtsRepairFailure;

// Down-thunk(s) to bridge LtsRepairFailure methods from ancestors (if any)

// Up-thunk(s) to bridge LtsRepairFailure methods to ancestors (if any)

// Class-specific details for LtsRepairFailure
const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairFailure = 
{
    .classInfo.size =               sizeof(LtsRepairFailure),
    .classInfo.classId =            classId(LtsRepairFailure),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtsRepairFailure",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtsRepairFailure,
    .pCastInfo =          &__nvoc_castinfo__LtsRepairFailure,
    .pExportInfo =        &__nvoc_export_info__LtsRepairFailure
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtsRepairFailure __nvoc_metadata__LtsRepairFailure = {
    .rtti.pClassDef = &__nvoc_class_def_LtsRepairFailure,    // (ltsrepairfailure) this
    .rtti.dtor      = &__nvoc_dtor_LtsRepairFailure,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(LtsRepairFailure, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtsRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtsRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtsRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtsRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtsRepairFailure = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtsRepairFailure.rtti,    // [0]: (ltsrepairfailure) this
        &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtsRepairFailure = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtsRepairFailure object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_LtsRepairFailure(Dynamic* pThis) {

    LtsRepairFailure *__nvoc_this = (LtsRepairFailure *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtsRepairFailure(LtsRepairFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtsRepairFailure object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_LtsRepairFailure(LtsRepairFailure *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTS_REPAIR_FAILURE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_LtsRepairFailure_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_LtsRepairFailure(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltsrepairfailureConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_LtsRepairFailure_fail__init;
    goto __nvoc_ctor_LtsRepairFailure_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtsRepairFailure_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_LtsRepairFailure_fail_GpuResourceExhausted:
__nvoc_ctor_LtsRepairFailure_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtsRepairFailure_1(LtsRepairFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtsRepairFailure_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtsRepairFailure(LtsRepairFailure *pThis) {
    __nvoc_init_funcTable_LtsRepairFailure_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtsRepairFailure(LtsRepairFailure *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_LtsRepairFailure = pThis;    // (ltsrepairfailure) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtsRepairFailure;    // (ltsrepairfailure) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtsRepairFailure(pThis);
}

NV_STATUS __nvoc_objCreate_LtsRepairFailure(LtsRepairFailure **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtsRepairFailure *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtsRepairFailure));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtsRepairFailure));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtsRepairFailure(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtsRepairFailure(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtsRepairFailure_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtsRepairFailure_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtsRepairFailure));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtsRepairFailure(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_LtsRepairFailure((LtsRepairFailure **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a9a3e9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairFailure;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for MemoryChannelRepairFailure
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__MemoryChannelRepairFailure(MemoryChannelRepairFailure*);
void __nvoc_init_funcTable_MemoryChannelRepairFailure(MemoryChannelRepairFailure*);
NV_STATUS __nvoc_ctor_MemoryChannelRepairFailure(MemoryChannelRepairFailure*, struct EventBus *pEventBus);
void __nvoc_init_dataField_MemoryChannelRepairFailure(MemoryChannelRepairFailure*);
void __nvoc_dtor_MemoryChannelRepairFailure(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemoryChannelRepairFailure;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryChannelRepairFailure;

// Down-thunk(s) to bridge MemoryChannelRepairFailure methods from ancestors (if any)

// Up-thunk(s) to bridge MemoryChannelRepairFailure methods to ancestors (if any)

// Class-specific details for MemoryChannelRepairFailure
const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairFailure = 
{
    .classInfo.size =               sizeof(MemoryChannelRepairFailure),
    .classInfo.classId =            classId(MemoryChannelRepairFailure),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MemoryChannelRepairFailure",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MemoryChannelRepairFailure,
    .pCastInfo =          &__nvoc_castinfo__MemoryChannelRepairFailure,
    .pExportInfo =        &__nvoc_export_info__MemoryChannelRepairFailure
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__MemoryChannelRepairFailure __nvoc_metadata__MemoryChannelRepairFailure = {
    .rtti.pClassDef = &__nvoc_class_def_MemoryChannelRepairFailure,    // (memorychannelrepairfailure) this
    .rtti.dtor      = &__nvoc_dtor_MemoryChannelRepairFailure,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairFailure, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(MemoryChannelRepairFailure, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemoryChannelRepairFailure = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemoryChannelRepairFailure.rtti,    // [0]: (memorychannelrepairfailure) this
        &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__MemoryChannelRepairFailure = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct MemoryChannelRepairFailure object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_MemoryChannelRepairFailure(Dynamic* pThis) {

    MemoryChannelRepairFailure *__nvoc_this = (MemoryChannelRepairFailure *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MemoryChannelRepairFailure(MemoryChannelRepairFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MemoryChannelRepairFailure object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_MemoryChannelRepairFailure(MemoryChannelRepairFailure *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_MEMORY_CHANNEL_REPAIR_FAILURE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_MemoryChannelRepairFailure_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_MemoryChannelRepairFailure(pEvent);

    // Call the constructor for this class.
    status = __nvoc_memorychannelrepairfailureConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_MemoryChannelRepairFailure_fail__init;
    goto __nvoc_ctor_MemoryChannelRepairFailure_exit; // Success

    // Unwind on error.
__nvoc_ctor_MemoryChannelRepairFailure_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_MemoryChannelRepairFailure_fail_GpuResourceExhausted:
__nvoc_ctor_MemoryChannelRepairFailure_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryChannelRepairFailure_1(MemoryChannelRepairFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MemoryChannelRepairFailure_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_MemoryChannelRepairFailure(MemoryChannelRepairFailure *pThis) {
    __nvoc_init_funcTable_MemoryChannelRepairFailure_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemoryChannelRepairFailure(MemoryChannelRepairFailure *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_MemoryChannelRepairFailure = pThis;    // (memorychannelrepairfailure) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemoryChannelRepairFailure;    // (memorychannelrepairfailure) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemoryChannelRepairFailure(pThis);
}

NV_STATUS __nvoc_objCreate_MemoryChannelRepairFailure(MemoryChannelRepairFailure **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MemoryChannelRepairFailure *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MemoryChannelRepairFailure));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MemoryChannelRepairFailure));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MemoryChannelRepairFailure(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MemoryChannelRepairFailure(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MemoryChannelRepairFailure_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemoryChannelRepairFailure_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MemoryChannelRepairFailure));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryChannelRepairFailure(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_MemoryChannelRepairFailure((MemoryChannelRepairFailure **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__be89e4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairPending;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for SparingRepairPending
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__SparingRepairPending(SparingRepairPending*);
void __nvoc_init_funcTable_SparingRepairPending(SparingRepairPending*);
NV_STATUS __nvoc_ctor_SparingRepairPending(SparingRepairPending*, struct EventBus *pEventBus);
void __nvoc_init_dataField_SparingRepairPending(SparingRepairPending*);
void __nvoc_dtor_SparingRepairPending(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SparingRepairPending;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingRepairPending;

// Down-thunk(s) to bridge SparingRepairPending methods from ancestors (if any)

// Up-thunk(s) to bridge SparingRepairPending methods to ancestors (if any)

// Class-specific details for SparingRepairPending
const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairPending = 
{
    .classInfo.size =               sizeof(SparingRepairPending),
    .classInfo.classId =            classId(SparingRepairPending),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SparingRepairPending",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SparingRepairPending,
    .pCastInfo =          &__nvoc_castinfo__SparingRepairPending,
    .pExportInfo =        &__nvoc_export_info__SparingRepairPending
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SparingRepairPending __nvoc_metadata__SparingRepairPending = {
    .rtti.pClassDef = &__nvoc_class_def_SparingRepairPending,    // (sparingrepairpending) this
    .rtti.dtor      = &__nvoc_dtor_SparingRepairPending,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(SparingRepairPending, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SparingRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SparingRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SparingRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SparingRepairPending, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SparingRepairPending = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SparingRepairPending.rtti,    // [0]: (sparingrepairpending) this
        &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingRepairPending = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SparingRepairPending object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_SparingRepairPending(Dynamic* pThis) {

    SparingRepairPending *__nvoc_this = (SparingRepairPending *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SparingRepairPending(SparingRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SparingRepairPending object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_SparingRepairPending(SparingRepairPending *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SPARING_REPAIR_PENDING, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT);
    if (status != NV_OK) goto __nvoc_ctor_SparingRepairPending_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_SparingRepairPending(pEvent);

    // Call the constructor for this class.
    status = __nvoc_sparingrepairpendingConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_SparingRepairPending_fail__init;
    goto __nvoc_ctor_SparingRepairPending_exit; // Success

    // Unwind on error.
__nvoc_ctor_SparingRepairPending_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_SparingRepairPending_fail_GpuResourceRetirement:
__nvoc_ctor_SparingRepairPending_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SparingRepairPending_1(SparingRepairPending *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SparingRepairPending_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SparingRepairPending(SparingRepairPending *pThis) {
    __nvoc_init_funcTable_SparingRepairPending_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SparingRepairPending(SparingRepairPending *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_SparingRepairPending = pThis;    // (sparingrepairpending) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairPending;    // (sparingrepairpending) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SparingRepairPending(pThis);
}

NV_STATUS __nvoc_objCreate_SparingRepairPending(SparingRepairPending **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SparingRepairPending *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SparingRepairPending));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SparingRepairPending));

    __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SparingRepairPending(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SparingRepairPending(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SparingRepairPending_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SparingRepairPending_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SparingRepairPending));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SparingRepairPending(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_SparingRepairPending((SparingRepairPending **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a29d84 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairNotPossible;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for SparingRepairNotPossible
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__SparingRepairNotPossible(SparingRepairNotPossible*);
void __nvoc_init_funcTable_SparingRepairNotPossible(SparingRepairNotPossible*);
NV_STATUS __nvoc_ctor_SparingRepairNotPossible(SparingRepairNotPossible*, struct EventBus *pEventBus);
void __nvoc_init_dataField_SparingRepairNotPossible(SparingRepairNotPossible*);
void __nvoc_dtor_SparingRepairNotPossible(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SparingRepairNotPossible;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingRepairNotPossible;

// Down-thunk(s) to bridge SparingRepairNotPossible methods from ancestors (if any)

// Up-thunk(s) to bridge SparingRepairNotPossible methods to ancestors (if any)

// Class-specific details for SparingRepairNotPossible
const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairNotPossible = 
{
    .classInfo.size =               sizeof(SparingRepairNotPossible),
    .classInfo.classId =            classId(SparingRepairNotPossible),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SparingRepairNotPossible",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SparingRepairNotPossible,
    .pCastInfo =          &__nvoc_castinfo__SparingRepairNotPossible,
    .pExportInfo =        &__nvoc_export_info__SparingRepairNotPossible
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SparingRepairNotPossible __nvoc_metadata__SparingRepairNotPossible = {
    .rtti.pClassDef = &__nvoc_class_def_SparingRepairNotPossible,    // (sparingrepairnotpossible) this
    .rtti.dtor      = &__nvoc_dtor_SparingRepairNotPossible,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(SparingRepairNotPossible, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SparingRepairNotPossible, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SparingRepairNotPossible, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SparingRepairNotPossible, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SparingRepairNotPossible, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SparingRepairNotPossible = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SparingRepairNotPossible.rtti,    // [0]: (sparingrepairnotpossible) this
        &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingRepairNotPossible = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SparingRepairNotPossible object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_SparingRepairNotPossible(Dynamic* pThis) {

    SparingRepairNotPossible *__nvoc_this = (SparingRepairNotPossible *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SparingRepairNotPossible(SparingRepairNotPossible *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SparingRepairNotPossible object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_SparingRepairNotPossible(SparingRepairNotPossible *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SPARING_REPAIR_NOT_POSSIBLE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_SparingRepairNotPossible_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_SparingRepairNotPossible(pEvent);

    // Call the constructor for this class.
    status = __nvoc_sparingrepairnotpossibleConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_SparingRepairNotPossible_fail__init;
    goto __nvoc_ctor_SparingRepairNotPossible_exit; // Success

    // Unwind on error.
__nvoc_ctor_SparingRepairNotPossible_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_SparingRepairNotPossible_fail_GpuResourceExhausted:
__nvoc_ctor_SparingRepairNotPossible_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SparingRepairNotPossible_1(SparingRepairNotPossible *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SparingRepairNotPossible_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SparingRepairNotPossible(SparingRepairNotPossible *pThis) {
    __nvoc_init_funcTable_SparingRepairNotPossible_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SparingRepairNotPossible(SparingRepairNotPossible *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_SparingRepairNotPossible = pThis;    // (sparingrepairnotpossible) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SparingRepairNotPossible;    // (sparingrepairnotpossible) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SparingRepairNotPossible(pThis);
}

NV_STATUS __nvoc_objCreate_SparingRepairNotPossible(SparingRepairNotPossible **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SparingRepairNotPossible *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SparingRepairNotPossible));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SparingRepairNotPossible));

    __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SparingRepairNotPossible(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SparingRepairNotPossible(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SparingRepairNotPossible_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SparingRepairNotPossible_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SparingRepairNotPossible));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SparingRepairNotPossible(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_SparingRepairNotPossible((SparingRepairNotPossible **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e78edd = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuStateChanged;

// Forward declarations for SparingEvent
void __nvoc_init__GpuStateChanged(GpuStateChanged*);
void __nvoc_init__SparingEvent(SparingEvent*);
void __nvoc_init_funcTable_SparingEvent(SparingEvent*);
NV_STATUS __nvoc_ctor_SparingEvent(SparingEvent*, struct EventBus *pEventBus);
void __nvoc_init_dataField_SparingEvent(SparingEvent*);
void __nvoc_dtor_SparingEvent(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SparingEvent;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingEvent;

// Down-thunk(s) to bridge SparingEvent methods from ancestors (if any)

// Up-thunk(s) to bridge SparingEvent methods to ancestors (if any)

// Class-specific details for SparingEvent
const struct NVOC_CLASS_DEF __nvoc_class_def_SparingEvent = 
{
    .classInfo.size =               sizeof(SparingEvent),
    .classInfo.classId =            classId(SparingEvent),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SparingEvent",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SparingEvent,
    .pCastInfo =          &__nvoc_castinfo__SparingEvent,
    .pExportInfo =        &__nvoc_export_info__SparingEvent
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SparingEvent __nvoc_metadata__SparingEvent = {
    .rtti.pClassDef = &__nvoc_class_def_SparingEvent,    // (sparingevent) this
    .rtti.dtor      = &__nvoc_dtor_SparingEvent,
    .rtti.offset    = 0,
    .metadata__GpuStateChanged.rtti.pClassDef = &__nvoc_class_def_GpuStateChanged,    // (gpustatechanged) super
    .metadata__GpuStateChanged.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.rtti.offset    = NV_OFFSETOF(SparingEvent, __nvoc_base_GpuStateChanged),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SparingEvent, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SparingEvent, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SparingEvent, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SparingEvent, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SparingEvent = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SparingEvent.rtti,    // [0]: (sparingevent) this
        &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.rtti,    // [1]: (gpustatechanged) super
        &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SparingEvent = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SparingEvent object.
void __nvoc_dtor_GpuStateChanged(Dynamic*);
void __nvoc_dtor_SparingEvent(Dynamic* pThis) {

    SparingEvent *__nvoc_this = (SparingEvent *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuStateChanged((Dynamic *) &__nvoc_this->__nvoc_base_GpuStateChanged);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SparingEvent(SparingEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SparingEvent object.
NV_STATUS __nvoc_ctor_GpuStateChanged(GpuStateChanged *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_SparingEvent(SparingEvent *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuStateChanged(&pEvent->__nvoc_base_GpuStateChanged, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SPARING_EVENT, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE);
    if (status != NV_OK) goto __nvoc_ctor_SparingEvent_fail_GpuStateChanged;

    // Initialize data fields.
    __nvoc_init_dataField_SparingEvent(pEvent);

    // Call the constructor for this class.
    status = __nvoc_sparingeventConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_SparingEvent_fail__init;
    goto __nvoc_ctor_SparingEvent_exit; // Success

    // Unwind on error.
__nvoc_ctor_SparingEvent_fail__init:
    __nvoc_dtor_GpuStateChanged((Dynamic *)&pEvent->__nvoc_base_GpuStateChanged);
__nvoc_ctor_SparingEvent_fail_GpuStateChanged:
__nvoc_ctor_SparingEvent_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SparingEvent_1(SparingEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SparingEvent_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SparingEvent(SparingEvent *pThis) {
    __nvoc_init_funcTable_SparingEvent_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SparingEvent(SparingEvent *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuStateChanged = &pThis->__nvoc_base_GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_pbase_SparingEvent = pThis;    // (sparingevent) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuStateChanged(&pThis->__nvoc_base_GpuStateChanged);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuStateChanged.__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent.metadata__GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SparingEvent;    // (sparingevent) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SparingEvent(pThis);
}

NV_STATUS __nvoc_objCreate_SparingEvent(SparingEvent **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SparingEvent *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SparingEvent));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SparingEvent));

    __nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SparingEvent(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SparingEvent(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SparingEvent_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SparingEvent_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SparingEvent));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SparingEvent(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_SparingEvent((SparingEvent **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__62b176 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LtcError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LtcError(LtcError*);
void __nvoc_init_funcTable_LtcError(LtcError*);
NV_STATUS __nvoc_ctor_LtcError(LtcError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
void __nvoc_init_dataField_LtcError(LtcError*);
void __nvoc_dtor_LtcError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtcError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcError;

// Down-thunk(s) to bridge LtcError methods from ancestors (if any)

// Up-thunk(s) to bridge LtcError methods to ancestors (if any)

// Class-specific details for LtcError
const struct NVOC_CLASS_DEF __nvoc_class_def_LtcError = 
{
    .classInfo.size =               sizeof(LtcError),
    .classInfo.classId =            classId(LtcError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtcError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtcError,
    .pCastInfo =          &__nvoc_castinfo__LtcError,
    .pExportInfo =        &__nvoc_export_info__LtcError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtcError __nvoc_metadata__LtcError = {
    .rtti.pClassDef = &__nvoc_class_def_LtcError,    // (ltcerr) this
    .rtti.dtor      = &__nvoc_dtor_LtcError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LtcError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtcError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtcError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtcError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtcError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtcError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtcError.rtti,    // [0]: (ltcerr) this
        &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtcError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LtcError(Dynamic* pThis) {

    LtcError *__nvoc_this = (LtcError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtcError(LtcError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtcError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LtcError(LtcError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTC_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LtcError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LtcError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltcerrConstruct(pEvent, pEventBus, loc);
    if (status != NV_OK) goto __nvoc_ctor_LtcError_fail__init;
    goto __nvoc_ctor_LtcError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtcError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LtcError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LtcError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtcError_1(LtcError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtcError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtcError(LtcError *pThis) {
    __nvoc_init_funcTable_LtcError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtcError(LtcError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LtcError = pThis;    // (ltcerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LtcError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtcError;    // (ltcerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtcError(pThis);
}

NV_STATUS __nvoc_objCreate_LtcError(LtcError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtcError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtcError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtcError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtcError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtcError(__nvoc_pThis, pEventBus, loc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtcError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtcError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtcError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtcError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);

    return __nvoc_objCreate_LtcError((LtcError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__3080d3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for DramError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__DramError(DramError*);
void __nvoc_init_funcTable_DramError(DramError*);
NV_STATUS __nvoc_ctor_DramError(DramError*, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvBool bHbm, NvU32 encodedBankRemapRegVal);
void __nvoc_init_dataField_DramError(DramError*);
void __nvoc_dtor_DramError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramError;

// Down-thunk(s) to bridge DramError methods from ancestors (if any)

// Up-thunk(s) to bridge DramError methods to ancestors (if any)

// Class-specific details for DramError
const struct NVOC_CLASS_DEF __nvoc_class_def_DramError = 
{
    .classInfo.size =               sizeof(DramError),
    .classInfo.classId =            classId(DramError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramError,
    .pCastInfo =          &__nvoc_castinfo__DramError,
    .pExportInfo =        &__nvoc_export_info__DramError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramError __nvoc_metadata__DramError = {
    .rtti.pClassDef = &__nvoc_class_def_DramError,    // (dramerr) this
    .rtti.dtor      = &__nvoc_dtor_DramError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(DramError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramError.rtti,    // [0]: (dramerr) this
        &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_DramError(Dynamic* pThis) {

    DramError *__nvoc_this = (DramError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramError(DramError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_DramError(DramError *pEvent, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvBool bHbm, NvU32 encodedBankRemapRegVal) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_DramError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_DramError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_dramerrConstruct(pEvent, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal);
    if (status != NV_OK) goto __nvoc_ctor_DramError_fail__init;
    goto __nvoc_ctor_DramError_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_DramError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_DramError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramError_1(DramError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramError(DramError *pThis) {
    __nvoc_init_funcTable_DramError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramError(DramError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_DramError = pThis;    // (dramerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__DramError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramError;    // (dramerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramError(pThis);
}

NV_STATUS __nvoc_objCreate_DramError(DramError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvBool bHbm, NvU32 encodedBankRemapRegVal)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramError(__nvoc_pThis, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 fbpa = va_arg(__nvoc_args, NvU32);
    NvU32 sublocation = va_arg(__nvoc_args, NvU32);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);
    NvBool bHbm = (NvBool) va_arg(__nvoc_args, NvU32);
    NvU32 encodedBankRemapRegVal = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_DramError((DramError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a8a87c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramSidebandError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for DramSidebandError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__DramSidebandError(DramSidebandError*);
void __nvoc_init_funcTable_DramSidebandError(DramSidebandError*);
NV_STATUS __nvoc_ctor_DramSidebandError(DramSidebandError*, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvU32 encodedBankRemapRegVal);
void __nvoc_init_dataField_DramSidebandError(DramSidebandError*);
void __nvoc_dtor_DramSidebandError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramSidebandError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramSidebandError;

// Down-thunk(s) to bridge DramSidebandError methods from ancestors (if any)

// Up-thunk(s) to bridge DramSidebandError methods to ancestors (if any)

// Class-specific details for DramSidebandError
const struct NVOC_CLASS_DEF __nvoc_class_def_DramSidebandError = 
{
    .classInfo.size =               sizeof(DramSidebandError),
    .classInfo.classId =            classId(DramSidebandError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramSidebandError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramSidebandError,
    .pCastInfo =          &__nvoc_castinfo__DramSidebandError,
    .pExportInfo =        &__nvoc_export_info__DramSidebandError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramSidebandError __nvoc_metadata__DramSidebandError = {
    .rtti.pClassDef = &__nvoc_class_def_DramSidebandError,    // (dramsidebanderr) this
    .rtti.dtor      = &__nvoc_dtor_DramSidebandError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(DramSidebandError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramSidebandError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramSidebandError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramSidebandError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramSidebandError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramSidebandError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramSidebandError.rtti,    // [0]: (dramsidebanderr) this
        &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramSidebandError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramSidebandError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_DramSidebandError(Dynamic* pThis) {

    DramSidebandError *__nvoc_this = (DramSidebandError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramSidebandError(DramSidebandError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramSidebandError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_DramSidebandError(DramSidebandError *pEvent, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvU32 encodedBankRemapRegVal) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_SIDEBAND_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_DramSidebandError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_DramSidebandError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_dramsidebanderrConstruct(pEvent, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal);
    if (status != NV_OK) goto __nvoc_ctor_DramSidebandError_fail__init;
    goto __nvoc_ctor_DramSidebandError_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramSidebandError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_DramSidebandError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_DramSidebandError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramSidebandError_1(DramSidebandError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramSidebandError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramSidebandError(DramSidebandError *pThis) {
    __nvoc_init_funcTable_DramSidebandError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramSidebandError(DramSidebandError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_DramSidebandError = pThis;    // (dramsidebanderr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramSidebandError;    // (dramsidebanderr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramSidebandError(pThis);
}

NV_STATUS __nvoc_objCreate_DramSidebandError(DramSidebandError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvU32 encodedBankRemapRegVal)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramSidebandError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramSidebandError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramSidebandError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramSidebandError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramSidebandError(__nvoc_pThis, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramSidebandError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramSidebandError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramSidebandError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramSidebandError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 fbpa = va_arg(__nvoc_args, NvU32);
    NvU32 sublocation = va_arg(__nvoc_args, NvU32);
    NvU64 physicalAddress = va_arg(__nvoc_args, NvU64);
    NvU32 encodedBankRemapRegVal = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_DramSidebandError((DramSidebandError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__1689b9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcFatalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for DramCbcFatalError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__DramCbcFatalError(DramCbcFatalError*);
void __nvoc_init_funcTable_DramCbcFatalError(DramCbcFatalError*);
NV_STATUS __nvoc_ctor_DramCbcFatalError(DramCbcFatalError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
void __nvoc_init_dataField_DramCbcFatalError(DramCbcFatalError*);
void __nvoc_dtor_DramCbcFatalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramCbcFatalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramCbcFatalError;

// Down-thunk(s) to bridge DramCbcFatalError methods from ancestors (if any)

// Up-thunk(s) to bridge DramCbcFatalError methods to ancestors (if any)

// Class-specific details for DramCbcFatalError
const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcFatalError = 
{
    .classInfo.size =               sizeof(DramCbcFatalError),
    .classInfo.classId =            classId(DramCbcFatalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramCbcFatalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramCbcFatalError,
    .pCastInfo =          &__nvoc_castinfo__DramCbcFatalError,
    .pExportInfo =        &__nvoc_export_info__DramCbcFatalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramCbcFatalError __nvoc_metadata__DramCbcFatalError = {
    .rtti.pClassDef = &__nvoc_class_def_DramCbcFatalError,    // (dramcbcfatalerr) this
    .rtti.dtor      = &__nvoc_dtor_DramCbcFatalError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(DramCbcFatalError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramCbcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramCbcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramCbcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramCbcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramCbcFatalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramCbcFatalError.rtti,    // [0]: (dramcbcfatalerr) this
        &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramCbcFatalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramCbcFatalError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_DramCbcFatalError(Dynamic* pThis) {

    DramCbcFatalError *__nvoc_this = (DramCbcFatalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramCbcFatalError(DramCbcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramCbcFatalError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_DramCbcFatalError(DramCbcFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_CBC_FATAL_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_DramCbcFatalError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_DramCbcFatalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_dramcbcfatalerrConstruct(pEvent, pEventBus, loc, bHbm);
    if (status != NV_OK) goto __nvoc_ctor_DramCbcFatalError_fail__init;
    goto __nvoc_ctor_DramCbcFatalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramCbcFatalError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_DramCbcFatalError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_DramCbcFatalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramCbcFatalError_1(DramCbcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramCbcFatalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramCbcFatalError(DramCbcFatalError *pThis) {
    __nvoc_init_funcTable_DramCbcFatalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramCbcFatalError(DramCbcFatalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_DramCbcFatalError = pThis;    // (dramcbcfatalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcFatalError;    // (dramcbcfatalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramCbcFatalError(pThis);
}

NV_STATUS __nvoc_objCreate_DramCbcFatalError(DramCbcFatalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramCbcFatalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramCbcFatalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramCbcFatalError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramCbcFatalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramCbcFatalError(__nvoc_pThis, pEventBus, loc, bHbm);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramCbcFatalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramCbcFatalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramCbcFatalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramCbcFatalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);
    NvBool bHbm = (NvBool) va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_DramCbcFatalError((DramCbcFatalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, bHbm);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b1540a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcNonFatalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for DramCbcNonFatalError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__DramCbcNonFatalError(DramCbcNonFatalError*);
void __nvoc_init_funcTable_DramCbcNonFatalError(DramCbcNonFatalError*);
NV_STATUS __nvoc_ctor_DramCbcNonFatalError(DramCbcNonFatalError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
void __nvoc_init_dataField_DramCbcNonFatalError(DramCbcNonFatalError*);
void __nvoc_dtor_DramCbcNonFatalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramCbcNonFatalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramCbcNonFatalError;

// Down-thunk(s) to bridge DramCbcNonFatalError methods from ancestors (if any)

// Up-thunk(s) to bridge DramCbcNonFatalError methods to ancestors (if any)

// Class-specific details for DramCbcNonFatalError
const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcNonFatalError = 
{
    .classInfo.size =               sizeof(DramCbcNonFatalError),
    .classInfo.classId =            classId(DramCbcNonFatalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramCbcNonFatalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramCbcNonFatalError,
    .pCastInfo =          &__nvoc_castinfo__DramCbcNonFatalError,
    .pExportInfo =        &__nvoc_export_info__DramCbcNonFatalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramCbcNonFatalError __nvoc_metadata__DramCbcNonFatalError = {
    .rtti.pClassDef = &__nvoc_class_def_DramCbcNonFatalError,    // (dramcbcnonfatalerr) this
    .rtti.dtor      = &__nvoc_dtor_DramCbcNonFatalError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(DramCbcNonFatalError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramCbcNonFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramCbcNonFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramCbcNonFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramCbcNonFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramCbcNonFatalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramCbcNonFatalError.rtti,    // [0]: (dramcbcnonfatalerr) this
        &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramCbcNonFatalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramCbcNonFatalError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_DramCbcNonFatalError(Dynamic* pThis) {

    DramCbcNonFatalError *__nvoc_this = (DramCbcNonFatalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramCbcNonFatalError(DramCbcNonFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramCbcNonFatalError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_DramCbcNonFatalError(DramCbcNonFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_CBC_NON_FATAL_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_DramCbcNonFatalError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_DramCbcNonFatalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_dramcbcnonfatalerrConstruct(pEvent, pEventBus, loc, bHbm);
    if (status != NV_OK) goto __nvoc_ctor_DramCbcNonFatalError_fail__init;
    goto __nvoc_ctor_DramCbcNonFatalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramCbcNonFatalError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_DramCbcNonFatalError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_DramCbcNonFatalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramCbcNonFatalError_1(DramCbcNonFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramCbcNonFatalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramCbcNonFatalError(DramCbcNonFatalError *pThis) {
    __nvoc_init_funcTable_DramCbcNonFatalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramCbcNonFatalError(DramCbcNonFatalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_DramCbcNonFatalError = pThis;    // (dramcbcnonfatalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramCbcNonFatalError;    // (dramcbcnonfatalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramCbcNonFatalError(pThis);
}

NV_STATUS __nvoc_objCreate_DramCbcNonFatalError(DramCbcNonFatalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramCbcNonFatalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramCbcNonFatalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramCbcNonFatalError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramCbcNonFatalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramCbcNonFatalError(__nvoc_pThis, pEventBus, loc, bHbm);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramCbcNonFatalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramCbcNonFatalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramCbcNonFatalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramCbcNonFatalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);
    NvBool bHbm = (NvBool) va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_DramCbcNonFatalError((DramCbcNonFatalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, bHbm);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__9cb3f5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcFatalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LtcFatalError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LtcFatalError(LtcFatalError*);
void __nvoc_init_funcTable_LtcFatalError(LtcFatalError*);
NV_STATUS __nvoc_ctor_LtcFatalError(LtcFatalError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, L2_ERROR_TYPE errorType);
void __nvoc_init_dataField_LtcFatalError(LtcFatalError*);
void __nvoc_dtor_LtcFatalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtcFatalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcFatalError;

// Down-thunk(s) to bridge LtcFatalError methods from ancestors (if any)

// Up-thunk(s) to bridge LtcFatalError methods to ancestors (if any)

// Class-specific details for LtcFatalError
const struct NVOC_CLASS_DEF __nvoc_class_def_LtcFatalError = 
{
    .classInfo.size =               sizeof(LtcFatalError),
    .classInfo.classId =            classId(LtcFatalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtcFatalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtcFatalError,
    .pCastInfo =          &__nvoc_castinfo__LtcFatalError,
    .pExportInfo =        &__nvoc_export_info__LtcFatalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtcFatalError __nvoc_metadata__LtcFatalError = {
    .rtti.pClassDef = &__nvoc_class_def_LtcFatalError,    // (ltcfatalerr) this
    .rtti.dtor      = &__nvoc_dtor_LtcFatalError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LtcFatalError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtcFatalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtcFatalError.rtti,    // [0]: (ltcfatalerr) this
        &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcFatalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtcFatalError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LtcFatalError(Dynamic* pThis) {

    LtcFatalError *__nvoc_this = (LtcFatalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtcFatalError(LtcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtcFatalError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LtcFatalError(LtcFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, L2_ERROR_TYPE errorType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTC_FATAL_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LtcFatalError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LtcFatalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltcfatalerrConstruct(pEvent, pEventBus, loc, errorType);
    if (status != NV_OK) goto __nvoc_ctor_LtcFatalError_fail__init;
    goto __nvoc_ctor_LtcFatalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtcFatalError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LtcFatalError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LtcFatalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtcFatalError_1(LtcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtcFatalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtcFatalError(LtcFatalError *pThis) {
    __nvoc_init_funcTable_LtcFatalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtcFatalError(LtcFatalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LtcFatalError = pThis;    // (ltcfatalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtcFatalError;    // (ltcfatalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtcFatalError(pThis);
}

NV_STATUS __nvoc_objCreate_LtcFatalError(LtcFatalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, L2_ERROR_TYPE errorType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtcFatalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtcFatalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtcFatalError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtcFatalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtcFatalError(__nvoc_pThis, pEventBus, loc, errorType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtcFatalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtcFatalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtcFatalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtcFatalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);
    L2_ERROR_TYPE errorType = va_arg(__nvoc_args, L2_ERROR_TYPE);

    return __nvoc_objCreate_LtcFatalError((LtcFatalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, errorType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__d5e463 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonUncontainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LtcUnsupportedClientPoisonUncontainedError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError*);
void __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError*);
NV_STATUS __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
void __nvoc_init_dataField_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError*);
void __nvoc_dtor_LtcUnsupportedClientPoisonUncontainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtcUnsupportedClientPoisonUncontainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcUnsupportedClientPoisonUncontainedError;

// Down-thunk(s) to bridge LtcUnsupportedClientPoisonUncontainedError methods from ancestors (if any)

// Up-thunk(s) to bridge LtcUnsupportedClientPoisonUncontainedError methods to ancestors (if any)

// Class-specific details for LtcUnsupportedClientPoisonUncontainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonUncontainedError = 
{
    .classInfo.size =               sizeof(LtcUnsupportedClientPoisonUncontainedError),
    .classInfo.classId =            classId(LtcUnsupportedClientPoisonUncontainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtcUnsupportedClientPoisonUncontainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtcUnsupportedClientPoisonUncontainedError,
    .pCastInfo =          &__nvoc_castinfo__LtcUnsupportedClientPoisonUncontainedError,
    .pExportInfo =        &__nvoc_export_info__LtcUnsupportedClientPoisonUncontainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtcUnsupportedClientPoisonUncontainedError __nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError = {
    .rtti.pClassDef = &__nvoc_class_def_LtcUnsupportedClientPoisonUncontainedError,    // (ltcclientpoisonuncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_LtcUnsupportedClientPoisonUncontainedError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtcUnsupportedClientPoisonUncontainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.rtti,    // [0]: (ltcclientpoisonuncontainederr) this
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcUnsupportedClientPoisonUncontainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtcUnsupportedClientPoisonUncontainedError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LtcUnsupportedClientPoisonUncontainedError(Dynamic* pThis) {

    LtcUnsupportedClientPoisonUncontainedError *__nvoc_this = (LtcUnsupportedClientPoisonUncontainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtcUnsupportedClientPoisonUncontainedError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTC_UNSUPPORTED_CLIENT_POISON_UNCONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LtcUnsupportedClientPoisonUncontainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltcclientpoisonuncontainederrConstruct(pEvent, pEventBus, loc);
    if (status != NV_OK) goto __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_fail__init;
    goto __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError_1(LtcUnsupportedClientPoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError *pThis) {
    __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LtcUnsupportedClientPoisonUncontainedError = pThis;    // (ltcclientpoisonuncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonUncontainedError;    // (ltcclientpoisonuncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtcUnsupportedClientPoisonUncontainedError(pThis);
}

NV_STATUS __nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtcUnsupportedClientPoisonUncontainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtcUnsupportedClientPoisonUncontainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtcUnsupportedClientPoisonUncontainedError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtcUnsupportedClientPoisonUncontainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtcUnsupportedClientPoisonUncontainedError(__nvoc_pThis, pEventBus, loc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtcUnsupportedClientPoisonUncontainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtcUnsupportedClientPoisonUncontainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);

    return __nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError((LtcUnsupportedClientPoisonUncontainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__355a88 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonContainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LtcUnsupportedClientPoisonContainedError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError*);
void __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError*);
NV_STATUS __nvoc_ctor_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError*, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
void __nvoc_init_dataField_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError*);
void __nvoc_dtor_LtcUnsupportedClientPoisonContainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LtcUnsupportedClientPoisonContainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcUnsupportedClientPoisonContainedError;

// Down-thunk(s) to bridge LtcUnsupportedClientPoisonContainedError methods from ancestors (if any)

// Up-thunk(s) to bridge LtcUnsupportedClientPoisonContainedError methods to ancestors (if any)

// Class-specific details for LtcUnsupportedClientPoisonContainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonContainedError = 
{
    .classInfo.size =               sizeof(LtcUnsupportedClientPoisonContainedError),
    .classInfo.classId =            classId(LtcUnsupportedClientPoisonContainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LtcUnsupportedClientPoisonContainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LtcUnsupportedClientPoisonContainedError,
    .pCastInfo =          &__nvoc_castinfo__LtcUnsupportedClientPoisonContainedError,
    .pExportInfo =        &__nvoc_export_info__LtcUnsupportedClientPoisonContainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LtcUnsupportedClientPoisonContainedError __nvoc_metadata__LtcUnsupportedClientPoisonContainedError = {
    .rtti.pClassDef = &__nvoc_class_def_LtcUnsupportedClientPoisonContainedError,    // (ltcclientpoisoncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_LtcUnsupportedClientPoisonContainedError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonContainedError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LtcUnsupportedClientPoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LtcUnsupportedClientPoisonContainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.rtti,    // [0]: (ltcclientpoisoncontainederr) this
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LtcUnsupportedClientPoisonContainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LtcUnsupportedClientPoisonContainedError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LtcUnsupportedClientPoisonContainedError(Dynamic* pThis) {

    LtcUnsupportedClientPoisonContainedError *__nvoc_this = (LtcUnsupportedClientPoisonContainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LtcUnsupportedClientPoisonContainedError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LTC_UNSUPPORTED_CLIENT_POISON_CONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LtcUnsupportedClientPoisonContainedError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LtcUnsupportedClientPoisonContainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ltcclientpoisoncontainederrConstruct(pEvent, pEventBus, loc);
    if (status != NV_OK) goto __nvoc_ctor_LtcUnsupportedClientPoisonContainedError_fail__init;
    goto __nvoc_ctor_LtcUnsupportedClientPoisonContainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LtcUnsupportedClientPoisonContainedError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LtcUnsupportedClientPoisonContainedError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LtcUnsupportedClientPoisonContainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError_1(LtcUnsupportedClientPoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError *pThis) {
    __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LtcUnsupportedClientPoisonContainedError = pThis;    // (ltcclientpoisoncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LtcUnsupportedClientPoisonContainedError;    // (ltcclientpoisoncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LtcUnsupportedClientPoisonContainedError(pThis);
}

NV_STATUS __nvoc_objCreate_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LtcUnsupportedClientPoisonContainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LtcUnsupportedClientPoisonContainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LtcUnsupportedClientPoisonContainedError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LtcUnsupportedClientPoisonContainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LtcUnsupportedClientPoisonContainedError(__nvoc_pThis, pEventBus, loc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LtcUnsupportedClientPoisonContainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LtcUnsupportedClientPoisonContainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LtcUnsupportedClientPoisonContainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LtcUnsupportedClientPoisonContainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NV_ERROR_CONT_LOCATION loc = va_arg(__nvoc_args, NV_ERROR_CONT_LOCATION);

    return __nvoc_objCreate_LtcUnsupportedClientPoisonContainedError((LtcUnsupportedClientPoisonContainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__075c90 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LrcFatalError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LrcFatalError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LrcFatalError(LrcFatalError*);
void __nvoc_init_funcTable_LrcFatalError(LrcFatalError*);
NV_STATUS __nvoc_ctor_LrcFatalError(LrcFatalError*, struct EventBus *pEventBus, NvU32 partition, NvU32 slice);
void __nvoc_init_dataField_LrcFatalError(LrcFatalError*);
void __nvoc_dtor_LrcFatalError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LrcFatalError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LrcFatalError;

// Down-thunk(s) to bridge LrcFatalError methods from ancestors (if any)

// Up-thunk(s) to bridge LrcFatalError methods to ancestors (if any)

// Class-specific details for LrcFatalError
const struct NVOC_CLASS_DEF __nvoc_class_def_LrcFatalError = 
{
    .classInfo.size =               sizeof(LrcFatalError),
    .classInfo.classId =            classId(LrcFatalError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LrcFatalError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LrcFatalError,
    .pCastInfo =          &__nvoc_castinfo__LrcFatalError,
    .pExportInfo =        &__nvoc_export_info__LrcFatalError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LrcFatalError __nvoc_metadata__LrcFatalError = {
    .rtti.pClassDef = &__nvoc_class_def_LrcFatalError,    // (lrcfatalerr) this
    .rtti.dtor      = &__nvoc_dtor_LrcFatalError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LrcFatalError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LrcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LrcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LrcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LrcFatalError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LrcFatalError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LrcFatalError.rtti,    // [0]: (lrcfatalerr) this
        &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LrcFatalError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LrcFatalError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LrcFatalError(Dynamic* pThis) {

    LrcFatalError *__nvoc_this = (LrcFatalError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LrcFatalError(LrcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LrcFatalError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LrcFatalError(LrcFatalError *pEvent, struct EventBus *pEventBus, NvU32 partition, NvU32 slice) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LRC_FATAL_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LrcFatalError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LrcFatalError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_lrcfatalerrConstruct(pEvent, pEventBus, partition, slice);
    if (status != NV_OK) goto __nvoc_ctor_LrcFatalError_fail__init;
    goto __nvoc_ctor_LrcFatalError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LrcFatalError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LrcFatalError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LrcFatalError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LrcFatalError_1(LrcFatalError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LrcFatalError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LrcFatalError(LrcFatalError *pThis) {
    __nvoc_init_funcTable_LrcFatalError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LrcFatalError(LrcFatalError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LrcFatalError = pThis;    // (lrcfatalerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LrcFatalError;    // (lrcfatalerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LrcFatalError(pThis);
}

NV_STATUS __nvoc_objCreate_LrcFatalError(LrcFatalError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 partition, NvU32 slice)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LrcFatalError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LrcFatalError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LrcFatalError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LrcFatalError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LrcFatalError(__nvoc_pThis, pEventBus, partition, slice);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LrcFatalError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LrcFatalError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LrcFatalError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LrcFatalError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 partition = va_arg(__nvoc_args, NvU32);
    NvU32 slice = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_LrcFatalError((LrcFatalError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, partition, slice);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__fce4f3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonContainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieIngressPoisonContainedError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieIngressPoisonContainedError(PcieIngressPoisonContainedError*);
void __nvoc_init_funcTable_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError*);
NV_STATUS __nvoc_ctor_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError*, struct EventBus *pEventBus, NvU32 gfid);
void __nvoc_init_dataField_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError*);
void __nvoc_dtor_PcieIngressPoisonContainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieIngressPoisonContainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieIngressPoisonContainedError;

// Down-thunk(s) to bridge PcieIngressPoisonContainedError methods from ancestors (if any)

// Up-thunk(s) to bridge PcieIngressPoisonContainedError methods to ancestors (if any)

// Class-specific details for PcieIngressPoisonContainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonContainedError = 
{
    .classInfo.size =               sizeof(PcieIngressPoisonContainedError),
    .classInfo.classId =            classId(PcieIngressPoisonContainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieIngressPoisonContainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieIngressPoisonContainedError,
    .pCastInfo =          &__nvoc_castinfo__PcieIngressPoisonContainedError,
    .pExportInfo =        &__nvoc_export_info__PcieIngressPoisonContainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieIngressPoisonContainedError __nvoc_metadata__PcieIngressPoisonContainedError = {
    .rtti.pClassDef = &__nvoc_class_def_PcieIngressPoisonContainedError,    // (pcieingresspoisoncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_PcieIngressPoisonContainedError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonContainedError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonContainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonContainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonContainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonContainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieIngressPoisonContainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieIngressPoisonContainedError.rtti,    // [0]: (pcieingresspoisoncontainederr) this
        &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieIngressPoisonContainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieIngressPoisonContainedError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieIngressPoisonContainedError(Dynamic* pThis) {

    PcieIngressPoisonContainedError *__nvoc_this = (PcieIngressPoisonContainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieIngressPoisonContainedError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError *pEvent, struct EventBus *pEventBus, NvU32 gfid) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PCIE_INGRESS_POISON_CONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieIngressPoisonContainedError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieIngressPoisonContainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pcieingresspoisoncontainederrConstruct(pEvent, pEventBus, gfid);
    if (status != NV_OK) goto __nvoc_ctor_PcieIngressPoisonContainedError_fail__init;
    goto __nvoc_ctor_PcieIngressPoisonContainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieIngressPoisonContainedError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieIngressPoisonContainedError_fail_GpuInterconnectError:
__nvoc_ctor_PcieIngressPoisonContainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieIngressPoisonContainedError_1(PcieIngressPoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieIngressPoisonContainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError *pThis) {
    __nvoc_init_funcTable_PcieIngressPoisonContainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieIngressPoisonContainedError(PcieIngressPoisonContainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieIngressPoisonContainedError = pThis;    // (pcieingresspoisoncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonContainedError;    // (pcieingresspoisoncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieIngressPoisonContainedError(pThis);
}

NV_STATUS __nvoc_objCreate_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 gfid)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieIngressPoisonContainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieIngressPoisonContainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieIngressPoisonContainedError));

    __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PcieIngressPoisonContainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieIngressPoisonContainedError(__nvoc_pThis, pEventBus, gfid);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieIngressPoisonContainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieIngressPoisonContainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieIngressPoisonContainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieIngressPoisonContainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 gfid = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PcieIngressPoisonContainedError((PcieIngressPoisonContainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, gfid);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f20fad = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonUncontainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieIngressPoisonUncontainedError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError*);
void __nvoc_init_funcTable_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError*);
NV_STATUS __nvoc_ctor_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError*);
void __nvoc_dtor_PcieIngressPoisonUncontainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieIngressPoisonUncontainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieIngressPoisonUncontainedError;

// Down-thunk(s) to bridge PcieIngressPoisonUncontainedError methods from ancestors (if any)

// Up-thunk(s) to bridge PcieIngressPoisonUncontainedError methods to ancestors (if any)

// Class-specific details for PcieIngressPoisonUncontainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonUncontainedError = 
{
    .classInfo.size =               sizeof(PcieIngressPoisonUncontainedError),
    .classInfo.classId =            classId(PcieIngressPoisonUncontainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieIngressPoisonUncontainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieIngressPoisonUncontainedError,
    .pCastInfo =          &__nvoc_castinfo__PcieIngressPoisonUncontainedError,
    .pExportInfo =        &__nvoc_export_info__PcieIngressPoisonUncontainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieIngressPoisonUncontainedError __nvoc_metadata__PcieIngressPoisonUncontainedError = {
    .rtti.pClassDef = &__nvoc_class_def_PcieIngressPoisonUncontainedError,    // (pcieingresspoisonuncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_PcieIngressPoisonUncontainedError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonUncontainedError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonUncontainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonUncontainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonUncontainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieIngressPoisonUncontainedError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieIngressPoisonUncontainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.rtti,    // [0]: (pcieingresspoisonuncontainederr) this
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieIngressPoisonUncontainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieIngressPoisonUncontainedError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieIngressPoisonUncontainedError(Dynamic* pThis) {

    PcieIngressPoisonUncontainedError *__nvoc_this = (PcieIngressPoisonUncontainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieIngressPoisonUncontainedError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PCIE_INGRESS_POISON_UNCONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieIngressPoisonUncontainedError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieIngressPoisonUncontainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pcieingresspoisonuncontainederrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_PcieIngressPoisonUncontainedError_fail__init;
    goto __nvoc_ctor_PcieIngressPoisonUncontainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieIngressPoisonUncontainedError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieIngressPoisonUncontainedError_fail_GpuInterconnectError:
__nvoc_ctor_PcieIngressPoisonUncontainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieIngressPoisonUncontainedError_1(PcieIngressPoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieIngressPoisonUncontainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError *pThis) {
    __nvoc_init_funcTable_PcieIngressPoisonUncontainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieIngressPoisonUncontainedError = pThis;    // (pcieingresspoisonuncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieIngressPoisonUncontainedError;    // (pcieingresspoisonuncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieIngressPoisonUncontainedError(pThis);
}

NV_STATUS __nvoc_objCreate_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieIngressPoisonUncontainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieIngressPoisonUncontainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieIngressPoisonUncontainedError));

    __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PcieIngressPoisonUncontainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieIngressPoisonUncontainedError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieIngressPoisonUncontainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieIngressPoisonUncontainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieIngressPoisonUncontainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieIngressPoisonUncontainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_PcieIngressPoisonUncontainedError((PcieIngressPoisonUncontainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5f1cc9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieEgressPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieEgressPoisonError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieEgressPoisonError(PcieEgressPoisonError*);
void __nvoc_init_funcTable_PcieEgressPoisonError(PcieEgressPoisonError*);
NV_STATUS __nvoc_ctor_PcieEgressPoisonError(PcieEgressPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_PcieEgressPoisonError(PcieEgressPoisonError*);
void __nvoc_dtor_PcieEgressPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieEgressPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieEgressPoisonError;

// Down-thunk(s) to bridge PcieEgressPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge PcieEgressPoisonError methods to ancestors (if any)

// Class-specific details for PcieEgressPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieEgressPoisonError = 
{
    .classInfo.size =               sizeof(PcieEgressPoisonError),
    .classInfo.classId =            classId(PcieEgressPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieEgressPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieEgressPoisonError,
    .pCastInfo =          &__nvoc_castinfo__PcieEgressPoisonError,
    .pExportInfo =        &__nvoc_export_info__PcieEgressPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieEgressPoisonError __nvoc_metadata__PcieEgressPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_PcieEgressPoisonError,    // (pcieegresspoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_PcieEgressPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieEgressPoisonError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieEgressPoisonError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieEgressPoisonError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieEgressPoisonError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieEgressPoisonError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieEgressPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieEgressPoisonError.rtti,    // [0]: (pcieegresspoisonerr) this
        &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieEgressPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieEgressPoisonError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieEgressPoisonError(Dynamic* pThis) {

    PcieEgressPoisonError *__nvoc_this = (PcieEgressPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieEgressPoisonError(PcieEgressPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieEgressPoisonError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieEgressPoisonError(PcieEgressPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PCIE_EGRESS_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieEgressPoisonError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieEgressPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pcieegresspoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_PcieEgressPoisonError_fail__init;
    goto __nvoc_ctor_PcieEgressPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieEgressPoisonError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieEgressPoisonError_fail_GpuInterconnectError:
__nvoc_ctor_PcieEgressPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieEgressPoisonError_1(PcieEgressPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieEgressPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieEgressPoisonError(PcieEgressPoisonError *pThis) {
    __nvoc_init_funcTable_PcieEgressPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieEgressPoisonError(PcieEgressPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieEgressPoisonError = pThis;    // (pcieegresspoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieEgressPoisonError;    // (pcieegresspoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieEgressPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_PcieEgressPoisonError(PcieEgressPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieEgressPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieEgressPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieEgressPoisonError));

    __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PcieEgressPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieEgressPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieEgressPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieEgressPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieEgressPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieEgressPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_PcieEgressPoisonError((PcieEgressPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4823f5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonContainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for CopyEnginePoisonContainedError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__CopyEnginePoisonContainedError(CopyEnginePoisonContainedError*);
void __nvoc_init_funcTable_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError*);
NV_STATUS __nvoc_ctor_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError*);
void __nvoc_dtor_CopyEnginePoisonContainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CopyEnginePoisonContainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEnginePoisonContainedError;

// Down-thunk(s) to bridge CopyEnginePoisonContainedError methods from ancestors (if any)

// Up-thunk(s) to bridge CopyEnginePoisonContainedError methods to ancestors (if any)

// Class-specific details for CopyEnginePoisonContainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonContainedError = 
{
    .classInfo.size =               sizeof(CopyEnginePoisonContainedError),
    .classInfo.classId =            classId(CopyEnginePoisonContainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CopyEnginePoisonContainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CopyEnginePoisonContainedError,
    .pCastInfo =          &__nvoc_castinfo__CopyEnginePoisonContainedError,
    .pExportInfo =        &__nvoc_export_info__CopyEnginePoisonContainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CopyEnginePoisonContainedError __nvoc_metadata__CopyEnginePoisonContainedError = {
    .rtti.pClassDef = &__nvoc_class_def_CopyEnginePoisonContainedError,    // (copyenginepoisoncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_CopyEnginePoisonContainedError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonContainedError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonContainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CopyEnginePoisonContainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__CopyEnginePoisonContainedError.rtti,    // [0]: (copyenginepoisoncontainederr) this
        &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEnginePoisonContainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CopyEnginePoisonContainedError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_CopyEnginePoisonContainedError(Dynamic* pThis) {

    CopyEnginePoisonContainedError *__nvoc_this = (CopyEnginePoisonContainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CopyEnginePoisonContainedError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_COPY_ENGINE_POISON_CONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_CopyEnginePoisonContainedError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_CopyEnginePoisonContainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_copyenginepoisoncontainederrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_CopyEnginePoisonContainedError_fail__init;
    goto __nvoc_ctor_CopyEnginePoisonContainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_CopyEnginePoisonContainedError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_CopyEnginePoisonContainedError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_CopyEnginePoisonContainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CopyEnginePoisonContainedError_1(CopyEnginePoisonContainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CopyEnginePoisonContainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError *pThis) {
    __nvoc_init_funcTable_CopyEnginePoisonContainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CopyEnginePoisonContainedError(CopyEnginePoisonContainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_CopyEnginePoisonContainedError = pThis;    // (copyenginepoisoncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonContainedError;    // (copyenginepoisoncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CopyEnginePoisonContainedError(pThis);
}

NV_STATUS __nvoc_objCreate_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CopyEnginePoisonContainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CopyEnginePoisonContainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CopyEnginePoisonContainedError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__CopyEnginePoisonContainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CopyEnginePoisonContainedError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CopyEnginePoisonContainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CopyEnginePoisonContainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CopyEnginePoisonContainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CopyEnginePoisonContainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_CopyEnginePoisonContainedError((CopyEnginePoisonContainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__cd5e25 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonUncontainedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for CopyEnginePoisonUncontainedError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError*);
void __nvoc_init_funcTable_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError*);
NV_STATUS __nvoc_ctor_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError*);
void __nvoc_dtor_CopyEnginePoisonUncontainedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CopyEnginePoisonUncontainedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEnginePoisonUncontainedError;

// Down-thunk(s) to bridge CopyEnginePoisonUncontainedError methods from ancestors (if any)

// Up-thunk(s) to bridge CopyEnginePoisonUncontainedError methods to ancestors (if any)

// Class-specific details for CopyEnginePoisonUncontainedError
const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonUncontainedError = 
{
    .classInfo.size =               sizeof(CopyEnginePoisonUncontainedError),
    .classInfo.classId =            classId(CopyEnginePoisonUncontainedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CopyEnginePoisonUncontainedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CopyEnginePoisonUncontainedError,
    .pCastInfo =          &__nvoc_castinfo__CopyEnginePoisonUncontainedError,
    .pExportInfo =        &__nvoc_export_info__CopyEnginePoisonUncontainedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CopyEnginePoisonUncontainedError __nvoc_metadata__CopyEnginePoisonUncontainedError = {
    .rtti.pClassDef = &__nvoc_class_def_CopyEnginePoisonUncontainedError,    // (copyenginepoisonuncontainederr) this
    .rtti.dtor      = &__nvoc_dtor_CopyEnginePoisonUncontainedError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(CopyEnginePoisonUncontainedError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CopyEnginePoisonUncontainedError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.rtti,    // [0]: (copyenginepoisonuncontainederr) this
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEnginePoisonUncontainedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CopyEnginePoisonUncontainedError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_CopyEnginePoisonUncontainedError(Dynamic* pThis) {

    CopyEnginePoisonUncontainedError *__nvoc_this = (CopyEnginePoisonUncontainedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CopyEnginePoisonUncontainedError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_COPY_ENGINE_POISON_UNCONTAINED_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_CopyEnginePoisonUncontainedError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_CopyEnginePoisonUncontainedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_copyenginepoisonuncontainederrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_CopyEnginePoisonUncontainedError_fail__init;
    goto __nvoc_ctor_CopyEnginePoisonUncontainedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_CopyEnginePoisonUncontainedError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_CopyEnginePoisonUncontainedError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_CopyEnginePoisonUncontainedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CopyEnginePoisonUncontainedError_1(CopyEnginePoisonUncontainedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CopyEnginePoisonUncontainedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError *pThis) {
    __nvoc_init_funcTable_CopyEnginePoisonUncontainedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_CopyEnginePoisonUncontainedError = pThis;    // (copyenginepoisonuncontainederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CopyEnginePoisonUncontainedError;    // (copyenginepoisonuncontainederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CopyEnginePoisonUncontainedError(pThis);
}

NV_STATUS __nvoc_objCreate_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CopyEnginePoisonUncontainedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CopyEnginePoisonUncontainedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CopyEnginePoisonUncontainedError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__CopyEnginePoisonUncontainedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CopyEnginePoisonUncontainedError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CopyEnginePoisonUncontainedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CopyEnginePoisonUncontainedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CopyEnginePoisonUncontainedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CopyEnginePoisonUncontainedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_CopyEnginePoisonUncontainedError((CopyEnginePoisonUncontainedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ec940d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for SmPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__SmPoisonError(SmPoisonError*);
void __nvoc_init_funcTable_SmPoisonError(SmPoisonError*);
NV_STATUS __nvoc_ctor_SmPoisonError(SmPoisonError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_SmPoisonError(SmPoisonError*);
void __nvoc_dtor_SmPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SmPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SmPoisonError;

// Down-thunk(s) to bridge SmPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge SmPoisonError methods to ancestors (if any)

// Class-specific details for SmPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_SmPoisonError = 
{
    .classInfo.size =               sizeof(SmPoisonError),
    .classInfo.classId =            classId(SmPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SmPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SmPoisonError,
    .pCastInfo =          &__nvoc_castinfo__SmPoisonError,
    .pExportInfo =        &__nvoc_export_info__SmPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SmPoisonError __nvoc_metadata__SmPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_SmPoisonError,    // (smpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_SmPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(SmPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SmPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SmPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SmPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SmPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SmPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SmPoisonError.rtti,    // [0]: (smpoisonerr) this
        &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SmPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SmPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_SmPoisonError(Dynamic* pThis) {

    SmPoisonError *__nvoc_this = (SmPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SmPoisonError(SmPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SmPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_SmPoisonError(SmPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SM_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_SmPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_SmPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_smpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_SmPoisonError_fail__init;
    goto __nvoc_ctor_SmPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_SmPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_SmPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_SmPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SmPoisonError_1(SmPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SmPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SmPoisonError(SmPoisonError *pThis) {
    __nvoc_init_funcTable_SmPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SmPoisonError(SmPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_SmPoisonError = pThis;    // (smpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SmPoisonError;    // (smpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SmPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_SmPoisonError(SmPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SmPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SmPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SmPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SmPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SmPoisonError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SmPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SmPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SmPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SmPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_SmPoisonError((SmPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__055ca3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GccPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GccPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GccPoisonError(GccPoisonError*);
void __nvoc_init_funcTable_GccPoisonError(GccPoisonError*);
NV_STATUS __nvoc_ctor_GccPoisonError(GccPoisonError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_GccPoisonError(GccPoisonError*);
void __nvoc_dtor_GccPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GccPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GccPoisonError;

// Down-thunk(s) to bridge GccPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge GccPoisonError methods to ancestors (if any)

// Class-specific details for GccPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_GccPoisonError = 
{
    .classInfo.size =               sizeof(GccPoisonError),
    .classInfo.classId =            classId(GccPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GccPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GccPoisonError,
    .pCastInfo =          &__nvoc_castinfo__GccPoisonError,
    .pExportInfo =        &__nvoc_export_info__GccPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GccPoisonError __nvoc_metadata__GccPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_GccPoisonError,    // (gccpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_GccPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GccPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GccPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GccPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GccPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GccPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GccPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GccPoisonError.rtti,    // [0]: (gccpoisonerr) this
        &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GccPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GccPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GccPoisonError(Dynamic* pThis) {

    GccPoisonError *__nvoc_this = (GccPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GccPoisonError(GccPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GccPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GccPoisonError(GccPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_GCC_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GccPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GccPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gccpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_GccPoisonError_fail__init;
    goto __nvoc_ctor_GccPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GccPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GccPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GccPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GccPoisonError_1(GccPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GccPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GccPoisonError(GccPoisonError *pThis) {
    __nvoc_init_funcTable_GccPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GccPoisonError(GccPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GccPoisonError = pThis;    // (gccpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GccPoisonError;    // (gccpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GccPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_GccPoisonError(GccPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GccPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GccPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GccPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GccPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GccPoisonError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GccPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GccPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GccPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GccPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GccPoisonError((GccPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8f50d0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for ContextSwitchPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__ContextSwitchPoisonError(ContextSwitchPoisonError*);
void __nvoc_init_funcTable_ContextSwitchPoisonError(ContextSwitchPoisonError*);
NV_STATUS __nvoc_ctor_ContextSwitchPoisonError(ContextSwitchPoisonError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_ContextSwitchPoisonError(ContextSwitchPoisonError*);
void __nvoc_dtor_ContextSwitchPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ContextSwitchPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ContextSwitchPoisonError;

// Down-thunk(s) to bridge ContextSwitchPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge ContextSwitchPoisonError methods to ancestors (if any)

// Class-specific details for ContextSwitchPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchPoisonError = 
{
    .classInfo.size =               sizeof(ContextSwitchPoisonError),
    .classInfo.classId =            classId(ContextSwitchPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ContextSwitchPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ContextSwitchPoisonError,
    .pCastInfo =          &__nvoc_castinfo__ContextSwitchPoisonError,
    .pExportInfo =        &__nvoc_export_info__ContextSwitchPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ContextSwitchPoisonError __nvoc_metadata__ContextSwitchPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_ContextSwitchPoisonError,    // (ctxswpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_ContextSwitchPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(ContextSwitchPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(ContextSwitchPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(ContextSwitchPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(ContextSwitchPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(ContextSwitchPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ContextSwitchPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ContextSwitchPoisonError.rtti,    // [0]: (ctxswpoisonerr) this
        &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ContextSwitchPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ContextSwitchPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_ContextSwitchPoisonError(Dynamic* pThis) {

    ContextSwitchPoisonError *__nvoc_this = (ContextSwitchPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ContextSwitchPoisonError(ContextSwitchPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ContextSwitchPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_ContextSwitchPoisonError(ContextSwitchPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_CONTEXT_SWITCH_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_ContextSwitchPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_ContextSwitchPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ctxswpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_ContextSwitchPoisonError_fail__init;
    goto __nvoc_ctor_ContextSwitchPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_ContextSwitchPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_ContextSwitchPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_ContextSwitchPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ContextSwitchPoisonError_1(ContextSwitchPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ContextSwitchPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_ContextSwitchPoisonError(ContextSwitchPoisonError *pThis) {
    __nvoc_init_funcTable_ContextSwitchPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ContextSwitchPoisonError(ContextSwitchPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_ContextSwitchPoisonError = pThis;    // (ctxswpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchPoisonError;    // (ctxswpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ContextSwitchPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_ContextSwitchPoisonError(ContextSwitchPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ContextSwitchPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ContextSwitchPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ContextSwitchPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ContextSwitchPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ContextSwitchPoisonError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ContextSwitchPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ContextSwitchPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ContextSwitchPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ContextSwitchPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_ContextSwitchPoisonError((ContextSwitchPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__9ef007 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for MmuPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__MmuPoisonError(MmuPoisonError*);
void __nvoc_init_funcTable_MmuPoisonError(MmuPoisonError*);
NV_STATUS __nvoc_ctor_MmuPoisonError(MmuPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_MmuPoisonError(MmuPoisonError*);
void __nvoc_dtor_MmuPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MmuPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MmuPoisonError;

// Down-thunk(s) to bridge MmuPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge MmuPoisonError methods to ancestors (if any)

// Class-specific details for MmuPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_MmuPoisonError = 
{
    .classInfo.size =               sizeof(MmuPoisonError),
    .classInfo.classId =            classId(MmuPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MmuPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MmuPoisonError,
    .pCastInfo =          &__nvoc_castinfo__MmuPoisonError,
    .pExportInfo =        &__nvoc_export_info__MmuPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__MmuPoisonError __nvoc_metadata__MmuPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_MmuPoisonError,    // (mmupoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_MmuPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(MmuPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(MmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(MmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(MmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(MmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MmuPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MmuPoisonError.rtti,    // [0]: (mmupoisonerr) this
        &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__MmuPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct MmuPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_MmuPoisonError(Dynamic* pThis) {

    MmuPoisonError *__nvoc_this = (MmuPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MmuPoisonError(MmuPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MmuPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_MmuPoisonError(MmuPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_MMU_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_MmuPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_MmuPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_mmupoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_MmuPoisonError_fail__init;
    goto __nvoc_ctor_MmuPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_MmuPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_MmuPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_MmuPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MmuPoisonError_1(MmuPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MmuPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_MmuPoisonError(MmuPoisonError *pThis) {
    __nvoc_init_funcTable_MmuPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MmuPoisonError(MmuPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_MmuPoisonError = pThis;    // (mmupoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MmuPoisonError;    // (mmupoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MmuPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_MmuPoisonError(MmuPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MmuPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MmuPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MmuPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MmuPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MmuPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MmuPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MmuPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MmuPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MmuPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_MmuPoisonError((MmuPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__2e9cc4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubFatalPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for FbHubFatalPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__FbHubFatalPoisonError(FbHubFatalPoisonError*);
void __nvoc_init_funcTable_FbHubFatalPoisonError(FbHubFatalPoisonError*);
NV_STATUS __nvoc_ctor_FbHubFatalPoisonError(FbHubFatalPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_FbHubFatalPoisonError(FbHubFatalPoisonError*);
void __nvoc_dtor_FbHubFatalPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FbHubFatalPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FbHubFatalPoisonError;

// Down-thunk(s) to bridge FbHubFatalPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge FbHubFatalPoisonError methods to ancestors (if any)

// Class-specific details for FbHubFatalPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubFatalPoisonError = 
{
    .classInfo.size =               sizeof(FbHubFatalPoisonError),
    .classInfo.classId =            classId(FbHubFatalPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FbHubFatalPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FbHubFatalPoisonError,
    .pCastInfo =          &__nvoc_castinfo__FbHubFatalPoisonError,
    .pExportInfo =        &__nvoc_export_info__FbHubFatalPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FbHubFatalPoisonError __nvoc_metadata__FbHubFatalPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_FbHubFatalPoisonError,    // (fbhubfatalpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_FbHubFatalPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(FbHubFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FbHubFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FbHubFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FbHubFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FbHubFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FbHubFatalPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FbHubFatalPoisonError.rtti,    // [0]: (fbhubfatalpoisonerr) this
        &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FbHubFatalPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FbHubFatalPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_FbHubFatalPoisonError(Dynamic* pThis) {

    FbHubFatalPoisonError *__nvoc_this = (FbHubFatalPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FbHubFatalPoisonError(FbHubFatalPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FbHubFatalPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FbHubFatalPoisonError(FbHubFatalPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_FBHUB_FATAL_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FbHubFatalPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_FbHubFatalPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fbhubfatalpoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_FbHubFatalPoisonError_fail__init;
    goto __nvoc_ctor_FbHubFatalPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_FbHubFatalPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_FbHubFatalPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_FbHubFatalPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FbHubFatalPoisonError_1(FbHubFatalPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FbHubFatalPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FbHubFatalPoisonError(FbHubFatalPoisonError *pThis) {
    __nvoc_init_funcTable_FbHubFatalPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FbHubFatalPoisonError(FbHubFatalPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_FbHubFatalPoisonError = pThis;    // (fbhubfatalpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FbHubFatalPoisonError;    // (fbhubfatalpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FbHubFatalPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_FbHubFatalPoisonError(FbHubFatalPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FbHubFatalPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FbHubFatalPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FbHubFatalPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__FbHubFatalPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FbHubFatalPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FbHubFatalPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FbHubFatalPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FbHubFatalPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FbHubFatalPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_FbHubFatalPoisonError((FbHubFatalPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__148a19 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubNonFatalPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for FbHubNonFatalPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__FbHubNonFatalPoisonError(FbHubNonFatalPoisonError*);
void __nvoc_init_funcTable_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError*);
NV_STATUS __nvoc_ctor_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError*);
void __nvoc_dtor_FbHubNonFatalPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FbHubNonFatalPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FbHubNonFatalPoisonError;

// Down-thunk(s) to bridge FbHubNonFatalPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge FbHubNonFatalPoisonError methods to ancestors (if any)

// Class-specific details for FbHubNonFatalPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubNonFatalPoisonError = 
{
    .classInfo.size =               sizeof(FbHubNonFatalPoisonError),
    .classInfo.classId =            classId(FbHubNonFatalPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FbHubNonFatalPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FbHubNonFatalPoisonError,
    .pCastInfo =          &__nvoc_castinfo__FbHubNonFatalPoisonError,
    .pExportInfo =        &__nvoc_export_info__FbHubNonFatalPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FbHubNonFatalPoisonError __nvoc_metadata__FbHubNonFatalPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_FbHubNonFatalPoisonError,    // (fbhubnonfatalpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_FbHubNonFatalPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(FbHubNonFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FbHubNonFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FbHubNonFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FbHubNonFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FbHubNonFatalPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FbHubNonFatalPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FbHubNonFatalPoisonError.rtti,    // [0]: (fbhubnonfatalpoisonerr) this
        &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FbHubNonFatalPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FbHubNonFatalPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_FbHubNonFatalPoisonError(Dynamic* pThis) {

    FbHubNonFatalPoisonError *__nvoc_this = (FbHubNonFatalPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FbHubNonFatalPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_FBHUB_NON_FATAL_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)) | (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 0 : 0)) % 32) + ((((0 != 0) ? 0 : 0)) % 32)))) << ((((0 != 0) ? 0 : 0)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FbHubNonFatalPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_FbHubNonFatalPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fbhubnonfatalpoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_FbHubNonFatalPoisonError_fail__init;
    goto __nvoc_ctor_FbHubNonFatalPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_FbHubNonFatalPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_FbHubNonFatalPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_FbHubNonFatalPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FbHubNonFatalPoisonError_1(FbHubNonFatalPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FbHubNonFatalPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError *pThis) {
    __nvoc_init_funcTable_FbHubNonFatalPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FbHubNonFatalPoisonError(FbHubNonFatalPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_FbHubNonFatalPoisonError = pThis;    // (fbhubnonfatalpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FbHubNonFatalPoisonError;    // (fbhubnonfatalpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FbHubNonFatalPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FbHubNonFatalPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FbHubNonFatalPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FbHubNonFatalPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__FbHubNonFatalPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FbHubNonFatalPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FbHubNonFatalPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FbHubNonFatalPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FbHubNonFatalPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FbHubNonFatalPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_FbHubNonFatalPoisonError((FbHubNonFatalPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__690d4b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GspPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GspPoisonError(GspPoisonError*);
void __nvoc_init_funcTable_GspPoisonError(GspPoisonError*);
NV_STATUS __nvoc_ctor_GspPoisonError(GspPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_GspPoisonError(GspPoisonError*);
void __nvoc_dtor_GspPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspPoisonError;

// Down-thunk(s) to bridge GspPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge GspPoisonError methods to ancestors (if any)

// Class-specific details for GspPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoisonError = 
{
    .classInfo.size =               sizeof(GspPoisonError),
    .classInfo.classId =            classId(GspPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspPoisonError,
    .pCastInfo =          &__nvoc_castinfo__GspPoisonError,
    .pExportInfo =        &__nvoc_export_info__GspPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspPoisonError __nvoc_metadata__GspPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_GspPoisonError,    // (gsppoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_GspPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GspPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspPoisonError.rtti,    // [0]: (gsppoisonerr) this
        &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GspPoisonError(Dynamic* pThis) {

    GspPoisonError *__nvoc_this = (GspPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspPoisonError(GspPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspPoisonError(GspPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_GSP_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GspPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GspPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gsppoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_GspPoisonError_fail__init;
    goto __nvoc_ctor_GspPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GspPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GspPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspPoisonError_1(GspPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspPoisonError(GspPoisonError *pThis) {
    __nvoc_init_funcTable_GspPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspPoisonError(GspPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GspPoisonError = pThis;    // (gsppoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspPoisonError;    // (gsppoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_GspPoisonError(GspPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GspPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_GspPoisonError((GspPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f3d5df = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PmuPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for PmuPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__PmuPoisonError(PmuPoisonError*);
void __nvoc_init_funcTable_PmuPoisonError(PmuPoisonError*);
NV_STATUS __nvoc_ctor_PmuPoisonError(PmuPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_PmuPoisonError(PmuPoisonError*);
void __nvoc_dtor_PmuPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PmuPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PmuPoisonError;

// Down-thunk(s) to bridge PmuPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge PmuPoisonError methods to ancestors (if any)

// Class-specific details for PmuPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_PmuPoisonError = 
{
    .classInfo.size =               sizeof(PmuPoisonError),
    .classInfo.classId =            classId(PmuPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PmuPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PmuPoisonError,
    .pCastInfo =          &__nvoc_castinfo__PmuPoisonError,
    .pExportInfo =        &__nvoc_export_info__PmuPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PmuPoisonError __nvoc_metadata__PmuPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_PmuPoisonError,    // (pmupoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_PmuPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(PmuPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PmuPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PmuPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PmuPoisonError.rtti,    // [0]: (pmupoisonerr) this
        &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PmuPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PmuPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_PmuPoisonError(Dynamic* pThis) {

    PmuPoisonError *__nvoc_this = (PmuPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PmuPoisonError(PmuPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PmuPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PmuPoisonError(PmuPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PMU_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PmuPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_PmuPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pmupoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_PmuPoisonError_fail__init;
    goto __nvoc_ctor_PmuPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PmuPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_PmuPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_PmuPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PmuPoisonError_1(PmuPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PmuPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PmuPoisonError(PmuPoisonError *pThis) {
    __nvoc_init_funcTable_PmuPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PmuPoisonError(PmuPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_PmuPoisonError = pThis;    // (pmupoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PmuPoisonError;    // (pmupoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PmuPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_PmuPoisonError(PmuPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PmuPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PmuPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PmuPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PmuPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PmuPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PmuPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PmuPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PmuPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PmuPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_PmuPoisonError((PmuPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b507fb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2PoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for Sec2PoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__Sec2PoisonError(Sec2PoisonError*);
void __nvoc_init_funcTable_Sec2PoisonError(Sec2PoisonError*);
NV_STATUS __nvoc_ctor_Sec2PoisonError(Sec2PoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_Sec2PoisonError(Sec2PoisonError*);
void __nvoc_dtor_Sec2PoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Sec2PoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2PoisonError;

// Down-thunk(s) to bridge Sec2PoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge Sec2PoisonError methods to ancestors (if any)

// Class-specific details for Sec2PoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2PoisonError = 
{
    .classInfo.size =               sizeof(Sec2PoisonError),
    .classInfo.classId =            classId(Sec2PoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Sec2PoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_Sec2PoisonError,
    .pCastInfo =          &__nvoc_castinfo__Sec2PoisonError,
    .pExportInfo =        &__nvoc_export_info__Sec2PoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__Sec2PoisonError __nvoc_metadata__Sec2PoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_Sec2PoisonError,    // (sec2poisonerr) this
    .rtti.dtor      = &__nvoc_dtor_Sec2PoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(Sec2PoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(Sec2PoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(Sec2PoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(Sec2PoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(Sec2PoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Sec2PoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__Sec2PoisonError.rtti,    // [0]: (sec2poisonerr) this
        &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2PoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Sec2PoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_Sec2PoisonError(Dynamic* pThis) {

    Sec2PoisonError *__nvoc_this = (Sec2PoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_Sec2PoisonError(Sec2PoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct Sec2PoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_Sec2PoisonError(Sec2PoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SEC2_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_Sec2PoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_Sec2PoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_sec2poisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_Sec2PoisonError_fail__init;
    goto __nvoc_ctor_Sec2PoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_Sec2PoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_Sec2PoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_Sec2PoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Sec2PoisonError_1(Sec2PoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Sec2PoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Sec2PoisonError(Sec2PoisonError *pThis) {
    __nvoc_init_funcTable_Sec2PoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Sec2PoisonError(Sec2PoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_Sec2PoisonError = pThis;    // (sec2poisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Sec2PoisonError;    // (sec2poisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Sec2PoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_Sec2PoisonError(Sec2PoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Sec2PoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Sec2PoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Sec2PoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Sec2PoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_Sec2PoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Sec2PoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Sec2PoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Sec2PoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Sec2PoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_Sec2PoisonError((Sec2PoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__04f443 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbFalconPoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for FbFalconPoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__FbFalconPoisonError(FbFalconPoisonError*);
void __nvoc_init_funcTable_FbFalconPoisonError(FbFalconPoisonError*);
NV_STATUS __nvoc_ctor_FbFalconPoisonError(FbFalconPoisonError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_FbFalconPoisonError(FbFalconPoisonError*);
void __nvoc_dtor_FbFalconPoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FbFalconPoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FbFalconPoisonError;

// Down-thunk(s) to bridge FbFalconPoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge FbFalconPoisonError methods to ancestors (if any)

// Class-specific details for FbFalconPoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_FbFalconPoisonError = 
{
    .classInfo.size =               sizeof(FbFalconPoisonError),
    .classInfo.classId =            classId(FbFalconPoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FbFalconPoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FbFalconPoisonError,
    .pCastInfo =          &__nvoc_castinfo__FbFalconPoisonError,
    .pExportInfo =        &__nvoc_export_info__FbFalconPoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FbFalconPoisonError __nvoc_metadata__FbFalconPoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_FbFalconPoisonError,    // (fbfalconpoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_FbFalconPoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(FbFalconPoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FbFalconPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FbFalconPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FbFalconPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FbFalconPoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FbFalconPoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FbFalconPoisonError.rtti,    // [0]: (fbfalconpoisonerr) this
        &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FbFalconPoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FbFalconPoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_FbFalconPoisonError(Dynamic* pThis) {

    FbFalconPoisonError *__nvoc_this = (FbFalconPoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FbFalconPoisonError(FbFalconPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FbFalconPoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FbFalconPoisonError(FbFalconPoisonError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_FBFALCON_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FbFalconPoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_FbFalconPoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fbfalconpoisonerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_FbFalconPoisonError_fail__init;
    goto __nvoc_ctor_FbFalconPoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_FbFalconPoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_FbFalconPoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_FbFalconPoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FbFalconPoisonError_1(FbFalconPoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FbFalconPoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FbFalconPoisonError(FbFalconPoisonError *pThis) {
    __nvoc_init_funcTable_FbFalconPoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FbFalconPoisonError(FbFalconPoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_FbFalconPoisonError = pThis;    // (fbfalconpoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FbFalconPoisonError;    // (fbfalconpoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FbFalconPoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_FbFalconPoisonError(FbFalconPoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FbFalconPoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FbFalconPoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FbFalconPoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__FbFalconPoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FbFalconPoisonError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FbFalconPoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FbFalconPoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FbFalconPoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FbFalconPoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_FbFalconPoisonError((FbFalconPoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0e1cb3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VideoEnginePoisonError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for VideoEnginePoisonError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__VideoEnginePoisonError(VideoEnginePoisonError*);
void __nvoc_init_funcTable_VideoEnginePoisonError(VideoEnginePoisonError*);
NV_STATUS __nvoc_ctor_VideoEnginePoisonError(VideoEnginePoisonError*, struct EventBus *pEventBus, NvU32 nv2080EngineType);
void __nvoc_init_dataField_VideoEnginePoisonError(VideoEnginePoisonError*);
void __nvoc_dtor_VideoEnginePoisonError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__VideoEnginePoisonError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__VideoEnginePoisonError;

// Down-thunk(s) to bridge VideoEnginePoisonError methods from ancestors (if any)

// Up-thunk(s) to bridge VideoEnginePoisonError methods to ancestors (if any)

// Class-specific details for VideoEnginePoisonError
const struct NVOC_CLASS_DEF __nvoc_class_def_VideoEnginePoisonError = 
{
    .classInfo.size =               sizeof(VideoEnginePoisonError),
    .classInfo.classId =            classId(VideoEnginePoisonError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "VideoEnginePoisonError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_VideoEnginePoisonError,
    .pCastInfo =          &__nvoc_castinfo__VideoEnginePoisonError,
    .pExportInfo =        &__nvoc_export_info__VideoEnginePoisonError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__VideoEnginePoisonError __nvoc_metadata__VideoEnginePoisonError = {
    .rtti.pClassDef = &__nvoc_class_def_VideoEnginePoisonError,    // (videoenginepoisonerr) this
    .rtti.dtor      = &__nvoc_dtor_VideoEnginePoisonError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(VideoEnginePoisonError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(VideoEnginePoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(VideoEnginePoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(VideoEnginePoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(VideoEnginePoisonError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__VideoEnginePoisonError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__VideoEnginePoisonError.rtti,    // [0]: (videoenginepoisonerr) this
        &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__VideoEnginePoisonError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct VideoEnginePoisonError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_VideoEnginePoisonError(Dynamic* pThis) {

    VideoEnginePoisonError *__nvoc_this = (VideoEnginePoisonError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_VideoEnginePoisonError(VideoEnginePoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct VideoEnginePoisonError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_VideoEnginePoisonError(VideoEnginePoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_VIDEO_ENGINE_POISON_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 2 : 2)) % 32) + ((((0 != 0) ? 2 : 2)) % 32)))) << ((((0 != 0) ? 2 : 2)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_VideoEnginePoisonError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_VideoEnginePoisonError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_videoenginepoisonerrConstruct(pEvent, pEventBus, nv2080EngineType);
    if (status != NV_OK) goto __nvoc_ctor_VideoEnginePoisonError_fail__init;
    goto __nvoc_ctor_VideoEnginePoisonError_exit; // Success

    // Unwind on error.
__nvoc_ctor_VideoEnginePoisonError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_VideoEnginePoisonError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_VideoEnginePoisonError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_VideoEnginePoisonError_1(VideoEnginePoisonError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_VideoEnginePoisonError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_VideoEnginePoisonError(VideoEnginePoisonError *pThis) {
    __nvoc_init_funcTable_VideoEnginePoisonError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__VideoEnginePoisonError(VideoEnginePoisonError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_VideoEnginePoisonError = pThis;    // (videoenginepoisonerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__VideoEnginePoisonError;    // (videoenginepoisonerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_VideoEnginePoisonError(pThis);
}

NV_STATUS __nvoc_objCreate_VideoEnginePoisonError(VideoEnginePoisonError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080EngineType)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    VideoEnginePoisonError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(VideoEnginePoisonError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(VideoEnginePoisonError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__VideoEnginePoisonError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_VideoEnginePoisonError(__nvoc_pThis, pEventBus, nv2080EngineType);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_VideoEnginePoisonError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_VideoEnginePoisonError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(VideoEnginePoisonError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_VideoEnginePoisonError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080EngineType = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_VideoEnginePoisonError((VideoEnginePoisonError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__51ad4e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramEccIntrStorm;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for DramEccIntrStorm
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__DramEccIntrStorm(DramEccIntrStorm*);
void __nvoc_init_funcTable_DramEccIntrStorm(DramEccIntrStorm*);
NV_STATUS __nvoc_ctor_DramEccIntrStorm(DramEccIntrStorm*, struct EventBus *pEventBus, NvU32 fbpaIndexVirt);
void __nvoc_init_dataField_DramEccIntrStorm(DramEccIntrStorm*);
void __nvoc_dtor_DramEccIntrStorm(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DramEccIntrStorm;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DramEccIntrStorm;

// Down-thunk(s) to bridge DramEccIntrStorm methods from ancestors (if any)

// Up-thunk(s) to bridge DramEccIntrStorm methods to ancestors (if any)

// Class-specific details for DramEccIntrStorm
const struct NVOC_CLASS_DEF __nvoc_class_def_DramEccIntrStorm = 
{
    .classInfo.size =               sizeof(DramEccIntrStorm),
    .classInfo.classId =            classId(DramEccIntrStorm),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "DramEccIntrStorm",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_DramEccIntrStorm,
    .pCastInfo =          &__nvoc_castinfo__DramEccIntrStorm,
    .pExportInfo =        &__nvoc_export_info__DramEccIntrStorm
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__DramEccIntrStorm __nvoc_metadata__DramEccIntrStorm = {
    .rtti.pClassDef = &__nvoc_class_def_DramEccIntrStorm,    // (cedramstorm) this
    .rtti.dtor      = &__nvoc_dtor_DramEccIntrStorm,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(DramEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(DramEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(DramEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(DramEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(DramEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DramEccIntrStorm = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__DramEccIntrStorm.rtti,    // [0]: (cedramstorm) this
        &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__DramEccIntrStorm = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct DramEccIntrStorm object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_DramEccIntrStorm(Dynamic* pThis) {

    DramEccIntrStorm *__nvoc_this = (DramEccIntrStorm *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_DramEccIntrStorm(DramEccIntrStorm *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct DramEccIntrStorm object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_DramEccIntrStorm(DramEccIntrStorm *pEvent, struct EventBus *pEventBus, NvU32 fbpaIndexVirt) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_DRAM_ECC_INTR_STORM, OPERATIONAL_EVENT_SEVERITY_CORRECTED, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_DramEccIntrStorm_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_DramEccIntrStorm(pEvent);

    // Call the constructor for this class.
    status = __nvoc_cedramstormConstruct(pEvent, pEventBus, fbpaIndexVirt);
    if (status != NV_OK) goto __nvoc_ctor_DramEccIntrStorm_fail__init;
    goto __nvoc_ctor_DramEccIntrStorm_exit; // Success

    // Unwind on error.
__nvoc_ctor_DramEccIntrStorm_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_DramEccIntrStorm_fail_GpuMemoryIntegrityError:
__nvoc_ctor_DramEccIntrStorm_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DramEccIntrStorm_1(DramEccIntrStorm *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DramEccIntrStorm_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_DramEccIntrStorm(DramEccIntrStorm *pThis) {
    __nvoc_init_funcTable_DramEccIntrStorm_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DramEccIntrStorm(DramEccIntrStorm *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_DramEccIntrStorm = pThis;    // (cedramstorm) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DramEccIntrStorm;    // (cedramstorm) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DramEccIntrStorm(pThis);
}

NV_STATUS __nvoc_objCreate_DramEccIntrStorm(DramEccIntrStorm **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 fbpaIndexVirt)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    DramEccIntrStorm *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(DramEccIntrStorm));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(DramEccIntrStorm));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__DramEccIntrStorm(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_DramEccIntrStorm(__nvoc_pThis, pEventBus, fbpaIndexVirt);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_DramEccIntrStorm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_DramEccIntrStorm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(DramEccIntrStorm));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_DramEccIntrStorm(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 fbpaIndexVirt = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_DramEccIntrStorm((DramEccIntrStorm **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpaIndexVirt);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b4edfd = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmEccIntrStorm;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for SmEccIntrStorm
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__SmEccIntrStorm(SmEccIntrStorm*);
void __nvoc_init_funcTable_SmEccIntrStorm(SmEccIntrStorm*);
NV_STATUS __nvoc_ctor_SmEccIntrStorm(SmEccIntrStorm*, struct EventBus *pEventBus);
void __nvoc_init_dataField_SmEccIntrStorm(SmEccIntrStorm*);
void __nvoc_dtor_SmEccIntrStorm(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SmEccIntrStorm;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SmEccIntrStorm;

// Down-thunk(s) to bridge SmEccIntrStorm methods from ancestors (if any)

// Up-thunk(s) to bridge SmEccIntrStorm methods to ancestors (if any)

// Class-specific details for SmEccIntrStorm
const struct NVOC_CLASS_DEF __nvoc_class_def_SmEccIntrStorm = 
{
    .classInfo.size =               sizeof(SmEccIntrStorm),
    .classInfo.classId =            classId(SmEccIntrStorm),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SmEccIntrStorm",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SmEccIntrStorm,
    .pCastInfo =          &__nvoc_castinfo__SmEccIntrStorm,
    .pExportInfo =        &__nvoc_export_info__SmEccIntrStorm
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SmEccIntrStorm __nvoc_metadata__SmEccIntrStorm = {
    .rtti.pClassDef = &__nvoc_class_def_SmEccIntrStorm,    // (cesmstorm) this
    .rtti.dtor      = &__nvoc_dtor_SmEccIntrStorm,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(SmEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SmEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SmEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SmEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SmEccIntrStorm, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SmEccIntrStorm = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SmEccIntrStorm.rtti,    // [0]: (cesmstorm) this
        &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SmEccIntrStorm = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SmEccIntrStorm object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_SmEccIntrStorm(Dynamic* pThis) {

    SmEccIntrStorm *__nvoc_this = (SmEccIntrStorm *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SmEccIntrStorm(SmEccIntrStorm *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SmEccIntrStorm object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_SmEccIntrStorm(SmEccIntrStorm *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SM_ECC_INTR_STORM, OPERATIONAL_EVENT_SEVERITY_CORRECTED, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_SmEccIntrStorm_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_SmEccIntrStorm(pEvent);

    // Call the constructor for this class.
    status = __nvoc_cesmstormConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_SmEccIntrStorm_fail__init;
    goto __nvoc_ctor_SmEccIntrStorm_exit; // Success

    // Unwind on error.
__nvoc_ctor_SmEccIntrStorm_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_SmEccIntrStorm_fail_GpuMemoryIntegrityError:
__nvoc_ctor_SmEccIntrStorm_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SmEccIntrStorm_1(SmEccIntrStorm *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SmEccIntrStorm_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SmEccIntrStorm(SmEccIntrStorm *pThis) {
    __nvoc_init_funcTable_SmEccIntrStorm_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SmEccIntrStorm(SmEccIntrStorm *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_SmEccIntrStorm = pThis;    // (cesmstorm) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SmEccIntrStorm;    // (cesmstorm) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SmEccIntrStorm(pThis);
}

NV_STATUS __nvoc_objCreate_SmEccIntrStorm(SmEccIntrStorm **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SmEccIntrStorm *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SmEccIntrStorm));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SmEccIntrStorm));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SmEccIntrStorm(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SmEccIntrStorm(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SmEccIntrStorm_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SmEccIntrStorm_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SmEccIntrStorm));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SmEccIntrStorm(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_SmEccIntrStorm((SmEccIntrStorm **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__162294 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_EccResidualUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for EccResidualUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__EccResidualUncorrectableError(EccResidualUncorrectableError*);
void __nvoc_init_funcTable_EccResidualUncorrectableError(EccResidualUncorrectableError*);
NV_STATUS __nvoc_ctor_EccResidualUncorrectableError(EccResidualUncorrectableError*, struct EventBus *pEventBus, NvU32 severity, NvU32 dramCount, NvU32 ltcCount, NvU32 mmuCount, NvU32 pcieCount);
void __nvoc_init_dataField_EccResidualUncorrectableError(EccResidualUncorrectableError*);
void __nvoc_dtor_EccResidualUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__EccResidualUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__EccResidualUncorrectableError;

// Down-thunk(s) to bridge EccResidualUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge EccResidualUncorrectableError methods to ancestors (if any)

// Class-specific details for EccResidualUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_EccResidualUncorrectableError = 
{
    .classInfo.size =               sizeof(EccResidualUncorrectableError),
    .classInfo.classId =            classId(EccResidualUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "EccResidualUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_EccResidualUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__EccResidualUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__EccResidualUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__EccResidualUncorrectableError __nvoc_metadata__EccResidualUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_EccResidualUncorrectableError,    // (eccresidual) this
    .rtti.dtor      = &__nvoc_dtor_EccResidualUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(EccResidualUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(EccResidualUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(EccResidualUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(EccResidualUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(EccResidualUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__EccResidualUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__EccResidualUncorrectableError.rtti,    // [0]: (eccresidual) this
        &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__EccResidualUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct EccResidualUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_EccResidualUncorrectableError(Dynamic* pThis) {

    EccResidualUncorrectableError *__nvoc_this = (EccResidualUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_EccResidualUncorrectableError(EccResidualUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct EccResidualUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_EccResidualUncorrectableError(EccResidualUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 dramCount, NvU32 ltcCount, NvU32 mmuCount, NvU32 pcieCount) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_ECC_RESIDUAL_UNCORRECTABLE_ERROR, severity, (((0U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_EccResidualUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_EccResidualUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_eccresidualConstruct(pEvent, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount);
    if (status != NV_OK) goto __nvoc_ctor_EccResidualUncorrectableError_fail__init;
    goto __nvoc_ctor_EccResidualUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_EccResidualUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_EccResidualUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_EccResidualUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_EccResidualUncorrectableError_1(EccResidualUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_EccResidualUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_EccResidualUncorrectableError(EccResidualUncorrectableError *pThis) {
    __nvoc_init_funcTable_EccResidualUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__EccResidualUncorrectableError(EccResidualUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_EccResidualUncorrectableError = pThis;    // (eccresidual) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__EccResidualUncorrectableError;    // (eccresidual) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_EccResidualUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_EccResidualUncorrectableError(EccResidualUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU32 dramCount, NvU32 ltcCount, NvU32 mmuCount, NvU32 pcieCount)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    EccResidualUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(EccResidualUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(EccResidualUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__EccResidualUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_EccResidualUncorrectableError(__nvoc_pThis, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_EccResidualUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_EccResidualUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(EccResidualUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_EccResidualUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 dramCount = va_arg(__nvoc_args, NvU32);
    NvU32 ltcCount = va_arg(__nvoc_args, NvU32);
    NvU32 mmuCount = va_arg(__nvoc_args, NvU32);
    NvU32 pcieCount = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_EccResidualUncorrectableError((EccResidualUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__52be0a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieUncorrectableError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieUncorrectableError(PcieUncorrectableError*);
void __nvoc_init_funcTable_PcieUncorrectableError(PcieUncorrectableError*);
NV_STATUS __nvoc_ctor_PcieUncorrectableError(PcieUncorrectableError*, struct EventBus *pEventBus, NvU32 unit);
void __nvoc_init_dataField_PcieUncorrectableError(PcieUncorrectableError*);
void __nvoc_dtor_PcieUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieUncorrectableError;

// Down-thunk(s) to bridge PcieUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge PcieUncorrectableError methods to ancestors (if any)

// Class-specific details for PcieUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieUncorrectableError = 
{
    .classInfo.size =               sizeof(PcieUncorrectableError),
    .classInfo.classId =            classId(PcieUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__PcieUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__PcieUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieUncorrectableError __nvoc_metadata__PcieUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_PcieUncorrectableError,    // (pcieuceerr) this
    .rtti.dtor      = &__nvoc_dtor_PcieUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieUncorrectableError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieUncorrectableError.rtti,    // [0]: (pcieuceerr) this
        &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieUncorrectableError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieUncorrectableError(Dynamic* pThis) {

    PcieUncorrectableError *__nvoc_this = (PcieUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieUncorrectableError(PcieUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieUncorrectableError(PcieUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PCIE_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieUncorrectableError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pcieuceerrConstruct(pEvent, pEventBus, unit);
    if (status != NV_OK) goto __nvoc_ctor_PcieUncorrectableError_fail__init;
    goto __nvoc_ctor_PcieUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieUncorrectableError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieUncorrectableError_fail_GpuInterconnectError:
__nvoc_ctor_PcieUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieUncorrectableError_1(PcieUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieUncorrectableError(PcieUncorrectableError *pThis) {
    __nvoc_init_funcTable_PcieUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieUncorrectableError(PcieUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieUncorrectableError = pThis;    // (pcieuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieUncorrectableError;    // (pcieuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_PcieUncorrectableError(PcieUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 unit)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PcieUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieUncorrectableError(__nvoc_pThis, pEventBus, unit);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 unit = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PcieUncorrectableError((PcieUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__973621 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEngineUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for CopyEngineUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__CopyEngineUncorrectableError(CopyEngineUncorrectableError*);
void __nvoc_init_funcTable_CopyEngineUncorrectableError(CopyEngineUncorrectableError*);
NV_STATUS __nvoc_ctor_CopyEngineUncorrectableError(CopyEngineUncorrectableError*, struct EventBus *pEventBus, NvU32 dieletType, NvU32 dieletId, NvU32 pce);
void __nvoc_init_dataField_CopyEngineUncorrectableError(CopyEngineUncorrectableError*);
void __nvoc_dtor_CopyEngineUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__CopyEngineUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEngineUncorrectableError;

// Down-thunk(s) to bridge CopyEngineUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge CopyEngineUncorrectableError methods to ancestors (if any)

// Class-specific details for CopyEngineUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEngineUncorrectableError = 
{
    .classInfo.size =               sizeof(CopyEngineUncorrectableError),
    .classInfo.classId =            classId(CopyEngineUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "CopyEngineUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_CopyEngineUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__CopyEngineUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__CopyEngineUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__CopyEngineUncorrectableError __nvoc_metadata__CopyEngineUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_CopyEngineUncorrectableError,    // (copyengineuce) this
    .rtti.dtor      = &__nvoc_dtor_CopyEngineUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(CopyEngineUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEngineUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(CopyEngineUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(CopyEngineUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(CopyEngineUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__CopyEngineUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__CopyEngineUncorrectableError.rtti,    // [0]: (copyengineuce) this
        &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__CopyEngineUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct CopyEngineUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_CopyEngineUncorrectableError(Dynamic* pThis) {

    CopyEngineUncorrectableError *__nvoc_this = (CopyEngineUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_CopyEngineUncorrectableError(CopyEngineUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct CopyEngineUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_CopyEngineUncorrectableError(CopyEngineUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 dieletType, NvU32 dieletId, NvU32 pce) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_COPY_ENGINE_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_CopyEngineUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_CopyEngineUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_copyengineuceConstruct(pEvent, pEventBus, dieletType, dieletId, pce);
    if (status != NV_OK) goto __nvoc_ctor_CopyEngineUncorrectableError_fail__init;
    goto __nvoc_ctor_CopyEngineUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_CopyEngineUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_CopyEngineUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_CopyEngineUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_CopyEngineUncorrectableError_1(CopyEngineUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_CopyEngineUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_CopyEngineUncorrectableError(CopyEngineUncorrectableError *pThis) {
    __nvoc_init_funcTable_CopyEngineUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__CopyEngineUncorrectableError(CopyEngineUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_CopyEngineUncorrectableError = pThis;    // (copyengineuce) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__CopyEngineUncorrectableError;    // (copyengineuce) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_CopyEngineUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_CopyEngineUncorrectableError(CopyEngineUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 dieletType, NvU32 dieletId, NvU32 pce)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    CopyEngineUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(CopyEngineUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(CopyEngineUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__CopyEngineUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_CopyEngineUncorrectableError(__nvoc_pThis, pEventBus, dieletType, dieletId, pce);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_CopyEngineUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_CopyEngineUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(CopyEngineUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_CopyEngineUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 dieletType = va_arg(__nvoc_args, NvU32);
    NvU32 dieletId = va_arg(__nvoc_args, NvU32);
    NvU32 pce = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_CopyEngineUncorrectableError((CopyEngineUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, dieletType, dieletId, pce);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__7346b4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for MmuUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__MmuUncorrectableError(MmuUncorrectableError*);
void __nvoc_init_funcTable_MmuUncorrectableError(MmuUncorrectableError*);
NV_STATUS __nvoc_ctor_MmuUncorrectableError(MmuUncorrectableError*, struct EventBus *pEventBus, NvU32 unit);
void __nvoc_init_dataField_MmuUncorrectableError(MmuUncorrectableError*);
void __nvoc_dtor_MmuUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MmuUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MmuUncorrectableError;

// Down-thunk(s) to bridge MmuUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge MmuUncorrectableError methods to ancestors (if any)

// Class-specific details for MmuUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_MmuUncorrectableError = 
{
    .classInfo.size =               sizeof(MmuUncorrectableError),
    .classInfo.classId =            classId(MmuUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MmuUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MmuUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__MmuUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__MmuUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__MmuUncorrectableError __nvoc_metadata__MmuUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_MmuUncorrectableError,    // (mmuuceerr) this
    .rtti.dtor      = &__nvoc_dtor_MmuUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(MmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(MmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(MmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(MmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(MmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MmuUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MmuUncorrectableError.rtti,    // [0]: (mmuuceerr) this
        &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__MmuUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct MmuUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_MmuUncorrectableError(Dynamic* pThis) {

    MmuUncorrectableError *__nvoc_this = (MmuUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MmuUncorrectableError(MmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MmuUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_MmuUncorrectableError(MmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_MMU_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_MmuUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_MmuUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_mmuuceerrConstruct(pEvent, pEventBus, unit);
    if (status != NV_OK) goto __nvoc_ctor_MmuUncorrectableError_fail__init;
    goto __nvoc_ctor_MmuUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_MmuUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_MmuUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_MmuUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MmuUncorrectableError_1(MmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MmuUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_MmuUncorrectableError(MmuUncorrectableError *pThis) {
    __nvoc_init_funcTable_MmuUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MmuUncorrectableError(MmuUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_MmuUncorrectableError = pThis;    // (mmuuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MmuUncorrectableError;    // (mmuuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MmuUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_MmuUncorrectableError(MmuUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 unit)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MmuUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MmuUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MmuUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__MmuUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MmuUncorrectableError(__nvoc_pThis, pEventBus, unit);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MmuUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MmuUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MmuUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MmuUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 unit = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_MmuUncorrectableError((MmuUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__de7383 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FecsUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for FecsUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__FecsUncorrectableError(FecsUncorrectableError*);
void __nvoc_init_funcTable_FecsUncorrectableError(FecsUncorrectableError*);
NV_STATUS __nvoc_ctor_FecsUncorrectableError(FecsUncorrectableError*, struct EventBus *pEventBus);
void __nvoc_init_dataField_FecsUncorrectableError(FecsUncorrectableError*);
void __nvoc_dtor_FecsUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FecsUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FecsUncorrectableError;

// Down-thunk(s) to bridge FecsUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge FecsUncorrectableError methods to ancestors (if any)

// Class-specific details for FecsUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_FecsUncorrectableError = 
{
    .classInfo.size =               sizeof(FecsUncorrectableError),
    .classInfo.classId =            classId(FecsUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FecsUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FecsUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__FecsUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__FecsUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FecsUncorrectableError __nvoc_metadata__FecsUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_FecsUncorrectableError,    // (fecsuceerr) this
    .rtti.dtor      = &__nvoc_dtor_FecsUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(FecsUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FecsUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FecsUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FecsUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FecsUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FecsUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FecsUncorrectableError.rtti,    // [0]: (fecsuceerr) this
        &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FecsUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FecsUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_FecsUncorrectableError(Dynamic* pThis) {

    FecsUncorrectableError *__nvoc_this = (FecsUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FecsUncorrectableError(FecsUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FecsUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FecsUncorrectableError(FecsUncorrectableError *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_FECS_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FecsUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_FecsUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fecsuceerrConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_FecsUncorrectableError_fail__init;
    goto __nvoc_ctor_FecsUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_FecsUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_FecsUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_FecsUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FecsUncorrectableError_1(FecsUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FecsUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FecsUncorrectableError(FecsUncorrectableError *pThis) {
    __nvoc_init_funcTable_FecsUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FecsUncorrectableError(FecsUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_FecsUncorrectableError = pThis;    // (fecsuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FecsUncorrectableError;    // (fecsuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FecsUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_FecsUncorrectableError(FecsUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FecsUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FecsUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FecsUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__FecsUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FecsUncorrectableError(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FecsUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FecsUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FecsUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FecsUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_FecsUncorrectableError((FecsUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__66dff4 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for SmUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__SmUncorrectableError(SmUncorrectableError*);
void __nvoc_init_funcTable_SmUncorrectableError(SmUncorrectableError*);
NV_STATUS __nvoc_ctor_SmUncorrectableError(SmUncorrectableError*, struct EventBus *pEventBus, NvU32 unit, NvU32 gpc, NvU32 tpc);
void __nvoc_init_dataField_SmUncorrectableError(SmUncorrectableError*);
void __nvoc_dtor_SmUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SmUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SmUncorrectableError;

// Down-thunk(s) to bridge SmUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge SmUncorrectableError methods to ancestors (if any)

// Class-specific details for SmUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_SmUncorrectableError = 
{
    .classInfo.size =               sizeof(SmUncorrectableError),
    .classInfo.classId =            classId(SmUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SmUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SmUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__SmUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__SmUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SmUncorrectableError __nvoc_metadata__SmUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_SmUncorrectableError,    // (smuceerr) this
    .rtti.dtor      = &__nvoc_dtor_SmUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(SmUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(SmUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(SmUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(SmUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(SmUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SmUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__SmUncorrectableError.rtti,    // [0]: (smuceerr) this
        &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__SmUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SmUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_SmUncorrectableError(Dynamic* pThis) {

    SmUncorrectableError *__nvoc_this = (SmUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SmUncorrectableError(SmUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SmUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_SmUncorrectableError(SmUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit, NvU32 gpc, NvU32 tpc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SM_CONTAINED_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_SmUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_SmUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_smuceerrConstruct(pEvent, pEventBus, unit, gpc, tpc);
    if (status != NV_OK) goto __nvoc_ctor_SmUncorrectableError_fail__init;
    goto __nvoc_ctor_SmUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_SmUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_SmUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_SmUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SmUncorrectableError_1(SmUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SmUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SmUncorrectableError(SmUncorrectableError *pThis) {
    __nvoc_init_funcTable_SmUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SmUncorrectableError(SmUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_SmUncorrectableError = pThis;    // (smuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SmUncorrectableError;    // (smuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SmUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_SmUncorrectableError(SmUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 unit, NvU32 gpc, NvU32 tpc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SmUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SmUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SmUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SmUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SmUncorrectableError(__nvoc_pThis, pEventBus, unit, gpc, tpc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SmUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SmUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SmUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SmUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 unit = va_arg(__nvoc_args, NvU32);
    NvU32 gpc = va_arg(__nvoc_args, NvU32);
    NvU32 tpc = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_SmUncorrectableError((SmUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit, gpc, tpc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__46e41f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GccUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GccUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GccUncorrectableError(GccUncorrectableError*);
void __nvoc_init_funcTable_GccUncorrectableError(GccUncorrectableError*);
NV_STATUS __nvoc_ctor_GccUncorrectableError(GccUncorrectableError*, struct EventBus *pEventBus, NvU32 gpc);
void __nvoc_init_dataField_GccUncorrectableError(GccUncorrectableError*);
void __nvoc_dtor_GccUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GccUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GccUncorrectableError;

// Down-thunk(s) to bridge GccUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge GccUncorrectableError methods to ancestors (if any)

// Class-specific details for GccUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_GccUncorrectableError = 
{
    .classInfo.size =               sizeof(GccUncorrectableError),
    .classInfo.classId =            classId(GccUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GccUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GccUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__GccUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__GccUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GccUncorrectableError __nvoc_metadata__GccUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_GccUncorrectableError,    // (gccuceerr) this
    .rtti.dtor      = &__nvoc_dtor_GccUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GccUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GccUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GccUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GccUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GccUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GccUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GccUncorrectableError.rtti,    // [0]: (gccuceerr) this
        &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GccUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GccUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GccUncorrectableError(Dynamic* pThis) {

    GccUncorrectableError *__nvoc_this = (GccUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GccUncorrectableError(GccUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GccUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GccUncorrectableError(GccUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_GCC_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GccUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GccUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gccuceerrConstruct(pEvent, pEventBus, gpc);
    if (status != NV_OK) goto __nvoc_ctor_GccUncorrectableError_fail__init;
    goto __nvoc_ctor_GccUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GccUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GccUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GccUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GccUncorrectableError_1(GccUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GccUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GccUncorrectableError(GccUncorrectableError *pThis) {
    __nvoc_init_funcTable_GccUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GccUncorrectableError(GccUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GccUncorrectableError = pThis;    // (gccuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GccUncorrectableError;    // (gccuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GccUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_GccUncorrectableError(GccUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 gpc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GccUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GccUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GccUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GccUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GccUncorrectableError(__nvoc_pThis, pEventBus, gpc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GccUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GccUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GccUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GccUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 gpc = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GccUncorrectableError((GccUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e36111 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for ContextSwitchUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__ContextSwitchUncorrectableError(ContextSwitchUncorrectableError*);
void __nvoc_init_funcTable_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError*);
NV_STATUS __nvoc_ctor_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError*, struct EventBus *pEventBus, NvU32 gpc);
void __nvoc_init_dataField_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError*);
void __nvoc_dtor_ContextSwitchUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__ContextSwitchUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__ContextSwitchUncorrectableError;

// Down-thunk(s) to bridge ContextSwitchUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge ContextSwitchUncorrectableError methods to ancestors (if any)

// Class-specific details for ContextSwitchUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchUncorrectableError = 
{
    .classInfo.size =               sizeof(ContextSwitchUncorrectableError),
    .classInfo.classId =            classId(ContextSwitchUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "ContextSwitchUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_ContextSwitchUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__ContextSwitchUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__ContextSwitchUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__ContextSwitchUncorrectableError __nvoc_metadata__ContextSwitchUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_ContextSwitchUncorrectableError,    // (ctxswuceerr) this
    .rtti.dtor      = &__nvoc_dtor_ContextSwitchUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(ContextSwitchUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(ContextSwitchUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(ContextSwitchUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(ContextSwitchUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(ContextSwitchUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__ContextSwitchUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__ContextSwitchUncorrectableError.rtti,    // [0]: (ctxswuceerr) this
        &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__ContextSwitchUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct ContextSwitchUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_ContextSwitchUncorrectableError(Dynamic* pThis) {

    ContextSwitchUncorrectableError *__nvoc_this = (ContextSwitchUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct ContextSwitchUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_CONTEXT_SWITCH_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_ContextSwitchUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_ContextSwitchUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_ctxswuceerrConstruct(pEvent, pEventBus, gpc);
    if (status != NV_OK) goto __nvoc_ctor_ContextSwitchUncorrectableError_fail__init;
    goto __nvoc_ctor_ContextSwitchUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_ContextSwitchUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_ContextSwitchUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_ContextSwitchUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_ContextSwitchUncorrectableError_1(ContextSwitchUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_ContextSwitchUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError *pThis) {
    __nvoc_init_funcTable_ContextSwitchUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__ContextSwitchUncorrectableError(ContextSwitchUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_ContextSwitchUncorrectableError = pThis;    // (ctxswuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__ContextSwitchUncorrectableError;    // (ctxswuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_ContextSwitchUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 gpc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    ContextSwitchUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(ContextSwitchUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(ContextSwitchUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__ContextSwitchUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_ContextSwitchUncorrectableError(__nvoc_pThis, pEventBus, gpc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_ContextSwitchUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_ContextSwitchUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(ContextSwitchUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_ContextSwitchUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 gpc = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_ContextSwitchUncorrectableError((ContextSwitchUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f31a41 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpcMmuUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GpcMmuUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GpcMmuUncorrectableError(GpcMmuUncorrectableError*);
void __nvoc_init_funcTable_GpcMmuUncorrectableError(GpcMmuUncorrectableError*);
NV_STATUS __nvoc_ctor_GpcMmuUncorrectableError(GpcMmuUncorrectableError*, struct EventBus *pEventBus, NvU32 gpc);
void __nvoc_init_dataField_GpcMmuUncorrectableError(GpcMmuUncorrectableError*);
void __nvoc_dtor_GpcMmuUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpcMmuUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpcMmuUncorrectableError;

// Down-thunk(s) to bridge GpcMmuUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge GpcMmuUncorrectableError methods to ancestors (if any)

// Class-specific details for GpcMmuUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_GpcMmuUncorrectableError = 
{
    .classInfo.size =               sizeof(GpcMmuUncorrectableError),
    .classInfo.classId =            classId(GpcMmuUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpcMmuUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpcMmuUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__GpcMmuUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__GpcMmuUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpcMmuUncorrectableError __nvoc_metadata__GpcMmuUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_GpcMmuUncorrectableError,    // (gpcmmuuceerr) this
    .rtti.dtor      = &__nvoc_dtor_GpcMmuUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GpcMmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpcMmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpcMmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpcMmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpcMmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpcMmuUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GpcMmuUncorrectableError.rtti,    // [0]: (gpcmmuuceerr) this
        &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpcMmuUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpcMmuUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GpcMmuUncorrectableError(Dynamic* pThis) {

    GpcMmuUncorrectableError *__nvoc_this = (GpcMmuUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpcMmuUncorrectableError(GpcMmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpcMmuUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpcMmuUncorrectableError(GpcMmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_GPCMMU_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_ENGINE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GpcMmuUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GpcMmuUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpcmmuuceerrConstruct(pEvent, pEventBus, gpc);
    if (status != NV_OK) goto __nvoc_ctor_GpcMmuUncorrectableError_fail__init;
    goto __nvoc_ctor_GpcMmuUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpcMmuUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GpcMmuUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GpcMmuUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpcMmuUncorrectableError_1(GpcMmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpcMmuUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpcMmuUncorrectableError(GpcMmuUncorrectableError *pThis) {
    __nvoc_init_funcTable_GpcMmuUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpcMmuUncorrectableError(GpcMmuUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GpcMmuUncorrectableError = pThis;    // (gpcmmuuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpcMmuUncorrectableError;    // (gpcmmuuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpcMmuUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_GpcMmuUncorrectableError(GpcMmuUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 gpc)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpcMmuUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpcMmuUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpcMmuUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpcMmuUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpcMmuUncorrectableError(__nvoc_pThis, pEventBus, gpc);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpcMmuUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpcMmuUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpcMmuUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpcMmuUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 gpc = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpcMmuUncorrectableError((GpcMmuUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b8abcb = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GspUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GspUncorrectableError(GspUncorrectableError*);
void __nvoc_init_funcTable_GspUncorrectableError(GspUncorrectableError*);
NV_STATUS __nvoc_ctor_GspUncorrectableError(GspUncorrectableError*, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
void __nvoc_init_dataField_GspUncorrectableError(GspUncorrectableError*);
void __nvoc_dtor_GspUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspUncorrectableError;

// Down-thunk(s) to bridge GspUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge GspUncorrectableError methods to ancestors (if any)

// Class-specific details for GspUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_GspUncorrectableError = 
{
    .classInfo.size =               sizeof(GspUncorrectableError),
    .classInfo.classId =            classId(GspUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__GspUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__GspUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspUncorrectableError __nvoc_metadata__GspUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_GspUncorrectableError,    // (gspuceerr) this
    .rtti.dtor      = &__nvoc_dtor_GspUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GspUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspUncorrectableError.rtti,    // [0]: (gspuceerr) this
        &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GspUncorrectableError(Dynamic* pThis) {

    GspUncorrectableError *__nvoc_this = (GspUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspUncorrectableError(GspUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspUncorrectableError(GspUncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_GSP_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GspUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GspUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gspuceerrConstruct(pEvent, pEventBus, bHasLocation, location);
    if (status != NV_OK) goto __nvoc_ctor_GspUncorrectableError_fail__init;
    goto __nvoc_ctor_GspUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GspUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GspUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspUncorrectableError_1(GspUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspUncorrectableError(GspUncorrectableError *pThis) {
    __nvoc_init_funcTable_GspUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspUncorrectableError(GspUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GspUncorrectableError = pThis;    // (gspuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspUncorrectableError;    // (gspuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_GspUncorrectableError(GspUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GspUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspUncorrectableError(__nvoc_pThis, pEventBus, bHasLocation, location);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvBool bHasLocation = (NvBool) va_arg(__nvoc_args, NvU32);
    NvU32 location = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GspUncorrectableError((GspUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, bHasLocation, location);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6a47f6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LpsecUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for LpsecUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__LpsecUncorrectableError(LpsecUncorrectableError*);
void __nvoc_init_funcTable_LpsecUncorrectableError(LpsecUncorrectableError*);
NV_STATUS __nvoc_ctor_LpsecUncorrectableError(LpsecUncorrectableError*, struct EventBus *pEventBus, NvU32 sequencer);
void __nvoc_init_dataField_LpsecUncorrectableError(LpsecUncorrectableError*);
void __nvoc_dtor_LpsecUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LpsecUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LpsecUncorrectableError;

// Down-thunk(s) to bridge LpsecUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge LpsecUncorrectableError methods to ancestors (if any)

// Class-specific details for LpsecUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_LpsecUncorrectableError = 
{
    .classInfo.size =               sizeof(LpsecUncorrectableError),
    .classInfo.classId =            classId(LpsecUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LpsecUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LpsecUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__LpsecUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__LpsecUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LpsecUncorrectableError __nvoc_metadata__LpsecUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_LpsecUncorrectableError,    // (lpsecuceerr) this
    .rtti.dtor      = &__nvoc_dtor_LpsecUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(LpsecUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(LpsecUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(LpsecUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(LpsecUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(LpsecUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LpsecUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LpsecUncorrectableError.rtti,    // [0]: (lpsecuceerr) this
        &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LpsecUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct LpsecUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_LpsecUncorrectableError(Dynamic* pThis) {

    LpsecUncorrectableError *__nvoc_this = (LpsecUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LpsecUncorrectableError(LpsecUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LpsecUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_LpsecUncorrectableError(LpsecUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 sequencer) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_LPSEC_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_LpsecUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_LpsecUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_lpsecuceerrConstruct(pEvent, pEventBus, sequencer);
    if (status != NV_OK) goto __nvoc_ctor_LpsecUncorrectableError_fail__init;
    goto __nvoc_ctor_LpsecUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_LpsecUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_LpsecUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_LpsecUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LpsecUncorrectableError_1(LpsecUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LpsecUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LpsecUncorrectableError(LpsecUncorrectableError *pThis) {
    __nvoc_init_funcTable_LpsecUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LpsecUncorrectableError(LpsecUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_LpsecUncorrectableError = pThis;    // (lpsecuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LpsecUncorrectableError;    // (lpsecuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LpsecUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_LpsecUncorrectableError(LpsecUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 sequencer)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LpsecUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LpsecUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LpsecUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LpsecUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LpsecUncorrectableError(__nvoc_pThis, pEventBus, sequencer);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LpsecUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LpsecUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LpsecUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LpsecUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 sequencer = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_LpsecUncorrectableError((LpsecUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, sequencer);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b35a90 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkUncorrectableError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkUncorrectableError(NvlinkUncorrectableError*);
void __nvoc_init_funcTable_NvlinkUncorrectableError(NvlinkUncorrectableError*);
NV_STATUS __nvoc_ctor_NvlinkUncorrectableError(NvlinkUncorrectableError*, struct EventBus *pEventBus, NvU32 nv2080GpuEccUnit);
void __nvoc_init_dataField_NvlinkUncorrectableError(NvlinkUncorrectableError*);
void __nvoc_dtor_NvlinkUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkUncorrectableError;

// Down-thunk(s) to bridge NvlinkUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkUncorrectableError methods to ancestors (if any)

// Class-specific details for NvlinkUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkUncorrectableError = 
{
    .classInfo.size =               sizeof(NvlinkUncorrectableError),
    .classInfo.classId =            classId(NvlinkUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__NvlinkUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__NvlinkUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkUncorrectableError __nvoc_metadata__NvlinkUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkUncorrectableError,    // (nvlinkuceerr) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkUncorrectableError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkUncorrectableError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkUncorrectableError.rtti,    // [0]: (nvlinkuceerr) this
        &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkUncorrectableError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkUncorrectableError(Dynamic* pThis) {

    NvlinkUncorrectableError *__nvoc_this = (NvlinkUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkUncorrectableError(NvlinkUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkUncorrectableError(NvlinkUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 nv2080GpuEccUnit) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_NVLINK_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkUncorrectableError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkuceerrConstruct(pEvent, pEventBus, nv2080GpuEccUnit);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkUncorrectableError_fail__init;
    goto __nvoc_ctor_NvlinkUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkUncorrectableError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkUncorrectableError_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkUncorrectableError_1(NvlinkUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkUncorrectableError(NvlinkUncorrectableError *pThis) {
    __nvoc_init_funcTable_NvlinkUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkUncorrectableError(NvlinkUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkUncorrectableError = pThis;    // (nvlinkuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkUncorrectableError;    // (nvlinkuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkUncorrectableError(NvlinkUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 nv2080GpuEccUnit)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__NvlinkUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkUncorrectableError(__nvoc_pThis, pEventBus, nv2080GpuEccUnit);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 nv2080GpuEccUnit = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkUncorrectableError((NvlinkUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080GpuEccUnit);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__c6a5e0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PmuUncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for PmuUncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__PmuUncorrectableError(PmuUncorrectableError*);
void __nvoc_init_funcTable_PmuUncorrectableError(PmuUncorrectableError*);
NV_STATUS __nvoc_ctor_PmuUncorrectableError(PmuUncorrectableError*, struct EventBus *pEventBus, NvBool bIsFalcon, NvBool bHasLocation, NvU32 location);
void __nvoc_init_dataField_PmuUncorrectableError(PmuUncorrectableError*);
void __nvoc_dtor_PmuUncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PmuUncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PmuUncorrectableError;

// Down-thunk(s) to bridge PmuUncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge PmuUncorrectableError methods to ancestors (if any)

// Class-specific details for PmuUncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_PmuUncorrectableError = 
{
    .classInfo.size =               sizeof(PmuUncorrectableError),
    .classInfo.classId =            classId(PmuUncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PmuUncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PmuUncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__PmuUncorrectableError,
    .pExportInfo =        &__nvoc_export_info__PmuUncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PmuUncorrectableError __nvoc_metadata__PmuUncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_PmuUncorrectableError,    // (pmuuceerr) this
    .rtti.dtor      = &__nvoc_dtor_PmuUncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(PmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PmuUncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PmuUncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PmuUncorrectableError.rtti,    // [0]: (pmuuceerr) this
        &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PmuUncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PmuUncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_PmuUncorrectableError(Dynamic* pThis) {

    PmuUncorrectableError *__nvoc_this = (PmuUncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PmuUncorrectableError(PmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PmuUncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PmuUncorrectableError(PmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bIsFalcon, NvBool bHasLocation, NvU32 location) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_PMU_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PmuUncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_PmuUncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pmuuceerrConstruct(pEvent, pEventBus, bIsFalcon, bHasLocation, location);
    if (status != NV_OK) goto __nvoc_ctor_PmuUncorrectableError_fail__init;
    goto __nvoc_ctor_PmuUncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_PmuUncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_PmuUncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_PmuUncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PmuUncorrectableError_1(PmuUncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PmuUncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PmuUncorrectableError(PmuUncorrectableError *pThis) {
    __nvoc_init_funcTable_PmuUncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PmuUncorrectableError(PmuUncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_PmuUncorrectableError = pThis;    // (pmuuceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PmuUncorrectableError;    // (pmuuceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PmuUncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_PmuUncorrectableError(PmuUncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvBool bIsFalcon, NvBool bHasLocation, NvU32 location)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PmuUncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PmuUncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PmuUncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PmuUncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PmuUncorrectableError(__nvoc_pThis, pEventBus, bIsFalcon, bHasLocation, location);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PmuUncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PmuUncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PmuUncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PmuUncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvBool bIsFalcon = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bHasLocation = (NvBool) va_arg(__nvoc_args, NvU32);
    NvU32 location = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PmuUncorrectableError((PmuUncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, bIsFalcon, bHasLocation, location);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__260c4b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2UncorrectableError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for Sec2UncorrectableError
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__Sec2UncorrectableError(Sec2UncorrectableError*);
void __nvoc_init_funcTable_Sec2UncorrectableError(Sec2UncorrectableError*);
NV_STATUS __nvoc_ctor_Sec2UncorrectableError(Sec2UncorrectableError*, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
void __nvoc_init_dataField_Sec2UncorrectableError(Sec2UncorrectableError*);
void __nvoc_dtor_Sec2UncorrectableError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Sec2UncorrectableError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2UncorrectableError;

// Down-thunk(s) to bridge Sec2UncorrectableError methods from ancestors (if any)

// Up-thunk(s) to bridge Sec2UncorrectableError methods to ancestors (if any)

// Class-specific details for Sec2UncorrectableError
const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2UncorrectableError = 
{
    .classInfo.size =               sizeof(Sec2UncorrectableError),
    .classInfo.classId =            classId(Sec2UncorrectableError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Sec2UncorrectableError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_Sec2UncorrectableError,
    .pCastInfo =          &__nvoc_castinfo__Sec2UncorrectableError,
    .pExportInfo =        &__nvoc_export_info__Sec2UncorrectableError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__Sec2UncorrectableError __nvoc_metadata__Sec2UncorrectableError = {
    .rtti.pClassDef = &__nvoc_class_def_Sec2UncorrectableError,    // (sec2uceerr) this
    .rtti.dtor      = &__nvoc_dtor_Sec2UncorrectableError,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(Sec2UncorrectableError, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(Sec2UncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(Sec2UncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(Sec2UncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(Sec2UncorrectableError, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Sec2UncorrectableError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__Sec2UncorrectableError.rtti,    // [0]: (sec2uceerr) this
        &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__Sec2UncorrectableError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Sec2UncorrectableError object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_Sec2UncorrectableError(Dynamic* pThis) {

    Sec2UncorrectableError *__nvoc_this = (Sec2UncorrectableError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_Sec2UncorrectableError(Sec2UncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct Sec2UncorrectableError object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_Sec2UncorrectableError(Sec2UncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_SEC2_UNCORRECTABLE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_Sec2UncorrectableError_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_Sec2UncorrectableError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_sec2uceerrConstruct(pEvent, pEventBus, bHasLocation, location);
    if (status != NV_OK) goto __nvoc_ctor_Sec2UncorrectableError_fail__init;
    goto __nvoc_ctor_Sec2UncorrectableError_exit; // Success

    // Unwind on error.
__nvoc_ctor_Sec2UncorrectableError_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_Sec2UncorrectableError_fail_GpuMemoryIntegrityError:
__nvoc_ctor_Sec2UncorrectableError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Sec2UncorrectableError_1(Sec2UncorrectableError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Sec2UncorrectableError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_Sec2UncorrectableError(Sec2UncorrectableError *pThis) {
    __nvoc_init_funcTable_Sec2UncorrectableError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Sec2UncorrectableError(Sec2UncorrectableError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_Sec2UncorrectableError = pThis;    // (sec2uceerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Sec2UncorrectableError;    // (sec2uceerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Sec2UncorrectableError(pThis);
}

NV_STATUS __nvoc_objCreate_Sec2UncorrectableError(Sec2UncorrectableError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    Sec2UncorrectableError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(Sec2UncorrectableError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(Sec2UncorrectableError));

    __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__Sec2UncorrectableError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_Sec2UncorrectableError(__nvoc_pThis, pEventBus, bHasLocation, location);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_Sec2UncorrectableError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_Sec2UncorrectableError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(Sec2UncorrectableError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_Sec2UncorrectableError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvBool bHasLocation = (NvBool) va_arg(__nvoc_args, NvU32);
    NvU32 location = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_Sec2UncorrectableError((Sec2UncorrectableError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, bHasLocation, location);
}

