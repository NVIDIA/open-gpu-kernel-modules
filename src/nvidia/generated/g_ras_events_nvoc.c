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
char __nvoc_class_id_uniqueness_check__758a69 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingSameGpc;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for TpcRepairPendingSameGpc
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__TpcRepairPendingSameGpc(TpcRepairPendingSameGpc*);
void __nvoc_init_funcTable_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc*);
NV_STATUS __nvoc_ctor_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc*, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
void __nvoc_init_dataField_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc*);
void __nvoc_dtor_TpcRepairPendingSameGpc(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairPendingSameGpc;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairPendingSameGpc;

// Down-thunk(s) to bridge TpcRepairPendingSameGpc methods from ancestors (if any)

// Up-thunk(s) to bridge TpcRepairPendingSameGpc methods to ancestors (if any)

// Class-specific details for TpcRepairPendingSameGpc
const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingSameGpc = 
{
    .classInfo.size =               sizeof(TpcRepairPendingSameGpc),
    .classInfo.classId =            classId(TpcRepairPendingSameGpc),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "TpcRepairPendingSameGpc",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_TpcRepairPendingSameGpc,
    .pCastInfo =          &__nvoc_castinfo__TpcRepairPendingSameGpc,
    .pExportInfo =        &__nvoc_export_info__TpcRepairPendingSameGpc
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__TpcRepairPendingSameGpc __nvoc_metadata__TpcRepairPendingSameGpc = {
    .rtti.pClassDef = &__nvoc_class_def_TpcRepairPendingSameGpc,    // (tpcrepairpendingsamegpc) this
    .rtti.dtor      = &__nvoc_dtor_TpcRepairPendingSameGpc,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(TpcRepairPendingSameGpc, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairPendingSameGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairPendingSameGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(TpcRepairPendingSameGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(TpcRepairPendingSameGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairPendingSameGpc = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__TpcRepairPendingSameGpc.rtti,    // [0]: (tpcrepairpendingsamegpc) this
        &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairPendingSameGpc = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct TpcRepairPendingSameGpc object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_TpcRepairPendingSameGpc(Dynamic* pThis) {

    TpcRepairPendingSameGpc *__nvoc_this = (TpcRepairPendingSameGpc *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct TpcRepairPendingSameGpc object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_TPC_REPAIR_PENDING_SAME_GPC, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairPendingSameGpc_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_TpcRepairPendingSameGpc(pEvent);

    // Call the constructor for this class.
    status = __nvoc_tpcrepairpendingsamegpcConstruct(pEvent, pEventBus, tpcId, gpcId);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairPendingSameGpc_fail__init;
    goto __nvoc_ctor_TpcRepairPendingSameGpc_exit; // Success

    // Unwind on error.
__nvoc_ctor_TpcRepairPendingSameGpc_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_TpcRepairPendingSameGpc_fail_GpuResourceRetirement:
__nvoc_ctor_TpcRepairPendingSameGpc_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_TpcRepairPendingSameGpc_1(TpcRepairPendingSameGpc *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_TpcRepairPendingSameGpc_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc *pThis) {
    __nvoc_init_funcTable_TpcRepairPendingSameGpc_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__TpcRepairPendingSameGpc(TpcRepairPendingSameGpc *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_TpcRepairPendingSameGpc = pThis;    // (tpcrepairpendingsamegpc) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingSameGpc;    // (tpcrepairpendingsamegpc) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_TpcRepairPendingSameGpc(pThis);
}

NV_STATUS __nvoc_objCreate_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    TpcRepairPendingSameGpc *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(TpcRepairPendingSameGpc));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairPendingSameGpc));

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
    __nvoc_init__TpcRepairPendingSameGpc(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_TpcRepairPendingSameGpc(__nvoc_pThis, pEventBus, tpcId, gpcId);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_TpcRepairPendingSameGpc_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_TpcRepairPendingSameGpc_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairPendingSameGpc));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_TpcRepairPendingSameGpc(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 tpcId = va_arg(__nvoc_args, NvU32);
    NvU32 gpcId = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_TpcRepairPendingSameGpc((TpcRepairPendingSameGpc **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__98d3e3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingDifferentGpc;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

// Forward declarations for TpcRepairPendingDifferentGpc
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init__TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc*);
void __nvoc_init_funcTable_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc*);
NV_STATUS __nvoc_ctor_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc*, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
void __nvoc_init_dataField_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc*);
void __nvoc_dtor_TpcRepairPendingDifferentGpc(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairPendingDifferentGpc;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairPendingDifferentGpc;

// Down-thunk(s) to bridge TpcRepairPendingDifferentGpc methods from ancestors (if any)

// Up-thunk(s) to bridge TpcRepairPendingDifferentGpc methods to ancestors (if any)

// Class-specific details for TpcRepairPendingDifferentGpc
const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingDifferentGpc = 
{
    .classInfo.size =               sizeof(TpcRepairPendingDifferentGpc),
    .classInfo.classId =            classId(TpcRepairPendingDifferentGpc),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "TpcRepairPendingDifferentGpc",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_TpcRepairPendingDifferentGpc,
    .pCastInfo =          &__nvoc_castinfo__TpcRepairPendingDifferentGpc,
    .pExportInfo =        &__nvoc_export_info__TpcRepairPendingDifferentGpc
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__TpcRepairPendingDifferentGpc __nvoc_metadata__TpcRepairPendingDifferentGpc = {
    .rtti.pClassDef = &__nvoc_class_def_TpcRepairPendingDifferentGpc,    // (tpcrepairpendingdifferentgpc) this
    .rtti.dtor      = &__nvoc_dtor_TpcRepairPendingDifferentGpc,
    .rtti.offset    = 0,
    .metadata__GpuResourceRetirement.rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) super
    .metadata__GpuResourceRetirement.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.rtti.offset    = NV_OFFSETOF(TpcRepairPendingDifferentGpc, __nvoc_base_GpuResourceRetirement),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairPendingDifferentGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairPendingDifferentGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(TpcRepairPendingDifferentGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(TpcRepairPendingDifferentGpc, __nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairPendingDifferentGpc = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.rtti,    // [0]: (tpcrepairpendingdifferentgpc) this
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.rtti,    // [1]: (gpuresret) super
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairPendingDifferentGpc = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct TpcRepairPendingDifferentGpc object.
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);
void __nvoc_dtor_TpcRepairPendingDifferentGpc(Dynamic* pThis) {

    TpcRepairPendingDifferentGpc *__nvoc_this = (TpcRepairPendingDifferentGpc *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceRetirement((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceRetirement);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct TpcRepairPendingDifferentGpc object.
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);    // inline
NV_STATUS __nvoc_ctor_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceRetirement(&pEvent->__nvoc_base_GpuResourceRetirement, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_TPC_REPAIR_PENDING_DIFFERENT_GPC, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairPendingDifferentGpc_fail_GpuResourceRetirement;

    // Initialize data fields.
    __nvoc_init_dataField_TpcRepairPendingDifferentGpc(pEvent);

    // Call the constructor for this class.
    status = __nvoc_tpcrepairpendingdifferentgpcConstruct(pEvent, pEventBus, tpcId, gpcId);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairPendingDifferentGpc_fail__init;
    goto __nvoc_ctor_TpcRepairPendingDifferentGpc_exit; // Success

    // Unwind on error.
__nvoc_ctor_TpcRepairPendingDifferentGpc_fail__init:
    __nvoc_dtor_GpuResourceRetirement((Dynamic *)&pEvent->__nvoc_base_GpuResourceRetirement);
__nvoc_ctor_TpcRepairPendingDifferentGpc_fail_GpuResourceRetirement:
__nvoc_ctor_TpcRepairPendingDifferentGpc_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_TpcRepairPendingDifferentGpc_1(TpcRepairPendingDifferentGpc *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_TpcRepairPendingDifferentGpc_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc *pThis) {
    __nvoc_init_funcTable_TpcRepairPendingDifferentGpc_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceRetirement = &pThis->__nvoc_base_GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_pbase_TpcRepairPendingDifferentGpc = pThis;    // (tpcrepairpendingdifferentgpc) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceRetirement(&pThis->__nvoc_base_GpuResourceRetirement);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceRetirement.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc.metadata__GpuResourceRetirement;    // (gpuresret) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairPendingDifferentGpc;    // (tpcrepairpendingdifferentgpc) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_TpcRepairPendingDifferentGpc(pThis);
}

NV_STATUS __nvoc_objCreate_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    TpcRepairPendingDifferentGpc *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(TpcRepairPendingDifferentGpc));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairPendingDifferentGpc));

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
    __nvoc_init__TpcRepairPendingDifferentGpc(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_TpcRepairPendingDifferentGpc(__nvoc_pThis, pEventBus, tpcId, gpcId);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_TpcRepairPendingDifferentGpc_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_TpcRepairPendingDifferentGpc_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceRetirement.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairPendingDifferentGpc));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_TpcRepairPendingDifferentGpc(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 tpcId = va_arg(__nvoc_args, NvU32);
    NvU32 gpcId = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_TpcRepairPendingDifferentGpc((TpcRepairPendingDifferentGpc **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__78f4fe = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpare;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for TpcRepairFailureNoSpare
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__TpcRepairFailureNoSpare(TpcRepairFailureNoSpare*);
void __nvoc_init_funcTable_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare*);
NV_STATUS __nvoc_ctor_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare*, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
void __nvoc_init_dataField_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare*);
void __nvoc_dtor_TpcRepairFailureNoSpare(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairFailureNoSpare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairFailureNoSpare;

// Down-thunk(s) to bridge TpcRepairFailureNoSpare methods from ancestors (if any)

// Up-thunk(s) to bridge TpcRepairFailureNoSpare methods to ancestors (if any)

// Class-specific details for TpcRepairFailureNoSpare
const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpare = 
{
    .classInfo.size =               sizeof(TpcRepairFailureNoSpare),
    .classInfo.classId =            classId(TpcRepairFailureNoSpare),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "TpcRepairFailureNoSpare",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_TpcRepairFailureNoSpare,
    .pCastInfo =          &__nvoc_castinfo__TpcRepairFailureNoSpare,
    .pExportInfo =        &__nvoc_export_info__TpcRepairFailureNoSpare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__TpcRepairFailureNoSpare __nvoc_metadata__TpcRepairFailureNoSpare = {
    .rtti.pClassDef = &__nvoc_class_def_TpcRepairFailureNoSpare,    // (tpcrepairfailurenospare) this
    .rtti.dtor      = &__nvoc_dtor_TpcRepairFailureNoSpare,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpare, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpare, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairFailureNoSpare = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__TpcRepairFailureNoSpare.rtti,    // [0]: (tpcrepairfailurenospare) this
        &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairFailureNoSpare = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct TpcRepairFailureNoSpare object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_TpcRepairFailureNoSpare(Dynamic* pThis) {

    TpcRepairFailureNoSpare *__nvoc_this = (TpcRepairFailureNoSpare *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct TpcRepairFailureNoSpare object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_TPC_REPAIR_FAILURE_NO_SPARE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairFailureNoSpare_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_TpcRepairFailureNoSpare(pEvent);

    // Call the constructor for this class.
    status = __nvoc_tpcrepairfailurenospareConstruct(pEvent, pEventBus, tpcId, gpcId);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairFailureNoSpare_fail__init;
    goto __nvoc_ctor_TpcRepairFailureNoSpare_exit; // Success

    // Unwind on error.
__nvoc_ctor_TpcRepairFailureNoSpare_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_TpcRepairFailureNoSpare_fail_GpuResourceExhausted:
__nvoc_ctor_TpcRepairFailureNoSpare_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_TpcRepairFailureNoSpare_1(TpcRepairFailureNoSpare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_TpcRepairFailureNoSpare_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare *pThis) {
    __nvoc_init_funcTable_TpcRepairFailureNoSpare_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__TpcRepairFailureNoSpare(TpcRepairFailureNoSpare *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_TpcRepairFailureNoSpare = pThis;    // (tpcrepairfailurenospare) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpare;    // (tpcrepairfailurenospare) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_TpcRepairFailureNoSpare(pThis);
}

NV_STATUS __nvoc_objCreate_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    TpcRepairFailureNoSpare *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(TpcRepairFailureNoSpare));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairFailureNoSpare));

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
    __nvoc_init__TpcRepairFailureNoSpare(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_TpcRepairFailureNoSpare(__nvoc_pThis, pEventBus, tpcId, gpcId);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_TpcRepairFailureNoSpare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_TpcRepairFailureNoSpare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairFailureNoSpare));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_TpcRepairFailureNoSpare(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 tpcId = va_arg(__nvoc_args, NvU32);
    NvU32 gpcId = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_TpcRepairFailureNoSpare((TpcRepairFailureNoSpare **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b6fda9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpareMig;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

// Forward declarations for TpcRepairFailureNoSpareMig
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init__TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig*);
void __nvoc_init_funcTable_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig*);
NV_STATUS __nvoc_ctor_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig*, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
void __nvoc_init_dataField_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig*);
void __nvoc_dtor_TpcRepairFailureNoSpareMig(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairFailureNoSpareMig;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairFailureNoSpareMig;

// Down-thunk(s) to bridge TpcRepairFailureNoSpareMig methods from ancestors (if any)

// Up-thunk(s) to bridge TpcRepairFailureNoSpareMig methods to ancestors (if any)

// Class-specific details for TpcRepairFailureNoSpareMig
const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpareMig = 
{
    .classInfo.size =               sizeof(TpcRepairFailureNoSpareMig),
    .classInfo.classId =            classId(TpcRepairFailureNoSpareMig),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "TpcRepairFailureNoSpareMig",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_TpcRepairFailureNoSpareMig,
    .pCastInfo =          &__nvoc_castinfo__TpcRepairFailureNoSpareMig,
    .pExportInfo =        &__nvoc_export_info__TpcRepairFailureNoSpareMig
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__TpcRepairFailureNoSpareMig __nvoc_metadata__TpcRepairFailureNoSpareMig = {
    .rtti.pClassDef = &__nvoc_class_def_TpcRepairFailureNoSpareMig,    // (tpcrepairfailurenosparemig) this
    .rtti.dtor      = &__nvoc_dtor_TpcRepairFailureNoSpareMig,
    .rtti.offset    = 0,
    .metadata__GpuResourceExhausted.rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) super
    .metadata__GpuResourceExhausted.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpareMig, __nvoc_base_GpuResourceExhausted),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpareMig, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpareMig, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpareMig, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(TpcRepairFailureNoSpareMig, __nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__TpcRepairFailureNoSpareMig = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.rtti,    // [0]: (tpcrepairfailurenosparemig) this
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.rtti,    // [1]: (gpuresexh) super
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__TpcRepairFailureNoSpareMig = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct TpcRepairFailureNoSpareMig object.
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);
void __nvoc_dtor_TpcRepairFailureNoSpareMig(Dynamic* pThis) {

    TpcRepairFailureNoSpareMig *__nvoc_this = (TpcRepairFailureNoSpareMig *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceExhausted((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceExhausted);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct TpcRepairFailureNoSpareMig object.
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);    // inline
NV_STATUS __nvoc_ctor_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceExhausted(&pEvent->__nvoc_base_GpuResourceExhausted, pEventBus, "GPU-RAS", RAS_OP_EVENT_CODE_TPC_REPAIR_FAILURE_NO_SPARE_MIG, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairFailureNoSpareMig_fail_GpuResourceExhausted;

    // Initialize data fields.
    __nvoc_init_dataField_TpcRepairFailureNoSpareMig(pEvent);

    // Call the constructor for this class.
    status = __nvoc_tpcrepairfailurenosparemigConstruct(pEvent, pEventBus, tpcId, gpcId);
    if (status != NV_OK) goto __nvoc_ctor_TpcRepairFailureNoSpareMig_fail__init;
    goto __nvoc_ctor_TpcRepairFailureNoSpareMig_exit; // Success

    // Unwind on error.
__nvoc_ctor_TpcRepairFailureNoSpareMig_fail__init:
    __nvoc_dtor_GpuResourceExhausted((Dynamic *)&pEvent->__nvoc_base_GpuResourceExhausted);
__nvoc_ctor_TpcRepairFailureNoSpareMig_fail_GpuResourceExhausted:
__nvoc_ctor_TpcRepairFailureNoSpareMig_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_TpcRepairFailureNoSpareMig_1(TpcRepairFailureNoSpareMig *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_TpcRepairFailureNoSpareMig_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig *pThis) {
    __nvoc_init_funcTable_TpcRepairFailureNoSpareMig_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceExhausted = &pThis->__nvoc_base_GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_pbase_TpcRepairFailureNoSpareMig = pThis;    // (tpcrepairfailurenosparemig) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceExhausted(&pThis->__nvoc_base_GpuResourceExhausted);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceExhausted.__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig.metadata__GpuResourceExhausted;    // (gpuresexh) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__TpcRepairFailureNoSpareMig;    // (tpcrepairfailurenosparemig) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_TpcRepairFailureNoSpareMig(pThis);
}

NV_STATUS __nvoc_objCreate_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    TpcRepairFailureNoSpareMig *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(TpcRepairFailureNoSpareMig));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairFailureNoSpareMig));

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
    __nvoc_init__TpcRepairFailureNoSpareMig(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_TpcRepairFailureNoSpareMig(__nvoc_pThis, pEventBus, tpcId, gpcId);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_TpcRepairFailureNoSpareMig_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_TpcRepairFailureNoSpareMig_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceExhausted.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(TpcRepairFailureNoSpareMig));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_TpcRepairFailureNoSpareMig(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 tpcId = va_arg(__nvoc_args, NvU32);
    NvU32 gpcId = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_TpcRepairFailureNoSpareMig((TpcRepairFailureNoSpareMig **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId);
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

