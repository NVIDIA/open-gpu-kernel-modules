#define NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6b9776 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuTimeout
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuTimeout(GpuTimeout*);
void __nvoc_init_funcTable_GpuTimeout(GpuTimeout*);
NV_STATUS __nvoc_ctor_GpuTimeout(GpuTimeout*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);
void __nvoc_init_dataField_GpuTimeout(GpuTimeout*);
void __nvoc_dtor_GpuTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuTimeout;

// Down-thunk(s) to bridge GpuTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge GpuTimeout methods to ancestors (if any)

// Class-specific details for GpuTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout = 
{
    .classInfo.size =               sizeof(GpuTimeout),
    .classInfo.classId =            classId(GpuTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuTimeout,
    .pCastInfo =          &__nvoc_castinfo__GpuTimeout,
    .pExportInfo =        &__nvoc_export_info__GpuTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuTimeout __nvoc_metadata__GpuTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_GpuTimeout,    // (gputimeout) this
    .rtti.dtor      = &__nvoc_dtor_GpuTimeout,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuTimeout, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuTimeout, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuTimeout, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuTimeout, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuTimeout = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuTimeout.rtti,    // [0]: (gputimeout) this
        &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuTimeout object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuTimeout(Dynamic* pThis) {

    GpuTimeout *__nvoc_this = (GpuTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuTimeout(GpuTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuTimeout object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuTimeout(GpuTimeout *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_TIMEOUT, pModuleSignature, eventCode, severity, attributes, scope, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuTimeout_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gputimeoutConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuTimeout_fail__init;
    goto __nvoc_ctor_GpuTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuTimeout_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuTimeout_fail_GpuOperationalEvent:
__nvoc_ctor_GpuTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuTimeout_1(GpuTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuTimeout(GpuTimeout *pThis) {
    __nvoc_init_funcTable_GpuTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuTimeout(GpuTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuTimeout = pThis;    // (gputimeout) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuTimeout.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuTimeout;    // (gputimeout) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_GpuTimeout(GpuTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuTimeout));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuTimeout(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU64 timeoutNs = va_arg(__nvoc_args, NvU64);
    NvU64 elapsedNs = va_arg(__nvoc_args, NvU64);
    const char *pWaitTarget = va_arg(__nvoc_args, const char *);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuTimeout((GpuTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__eb39ad = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuMemoryIntegrityError
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init_funcTable_GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
void __nvoc_init_dataField_GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuMemoryIntegrityError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuMemoryIntegrityError;

// Down-thunk(s) to bridge GpuMemoryIntegrityError methods from ancestors (if any)

// Up-thunk(s) to bridge GpuMemoryIntegrityError methods to ancestors (if any)

// Class-specific details for GpuMemoryIntegrityError
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError = 
{
    .classInfo.size =               sizeof(GpuMemoryIntegrityError),
    .classInfo.classId =            classId(GpuMemoryIntegrityError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuMemoryIntegrityError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuMemoryIntegrityError,
    .pCastInfo =          &__nvoc_castinfo__GpuMemoryIntegrityError,
    .pExportInfo =        &__nvoc_export_info__GpuMemoryIntegrityError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuMemoryIntegrityError __nvoc_metadata__GpuMemoryIntegrityError = {
    .rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) this
    .rtti.dtor      = &__nvoc_dtor_GpuMemoryIntegrityError,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuMemoryIntegrityError, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuMemoryIntegrityError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuMemoryIntegrityError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuMemoryIntegrityError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuMemoryIntegrityError = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuMemoryIntegrityError.rtti,    // [0]: (gpumemintegrityerr) this
        &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuMemoryIntegrityError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuMemoryIntegrityError object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic* pThis) {

    GpuMemoryIntegrityError *__nvoc_this = (GpuMemoryIntegrityError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuMemoryIntegrityError object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_MEMORY_INTEGRITY_ERROR, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuMemoryIntegrityError_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuMemoryIntegrityError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpumemintegrityerrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuMemoryIntegrityError_fail__init;
    goto __nvoc_ctor_GpuMemoryIntegrityError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuMemoryIntegrityError_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuMemoryIntegrityError_fail_GpuOperationalEvent:
__nvoc_ctor_GpuMemoryIntegrityError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuMemoryIntegrityError_1(GpuMemoryIntegrityError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuMemoryIntegrityError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pThis) {
    __nvoc_init_funcTable_GpuMemoryIntegrityError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = pThis;    // (gpumemintegrityerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuMemoryIntegrityError(pThis);
}

NV_STATUS __nvoc_objCreate_GpuMemoryIntegrityError(GpuMemoryIntegrityError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuMemoryIntegrityError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuMemoryIntegrityError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuMemoryIntegrityError));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuMemoryIntegrityError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuMemoryIntegrityError(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuMemoryIntegrityError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuMemoryIntegrityError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuMemoryIntegrityError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuMemoryIntegrityError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 reportingSource = va_arg(__nvoc_args, NvU32);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuMemoryIntegrityError((GpuMemoryIntegrityError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__9050aa = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuInterconnectError
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init_funcTable_GpuInterconnectError(GpuInterconnectError*);
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
void __nvoc_init_dataField_GpuInterconnectError(GpuInterconnectError*);
void __nvoc_dtor_GpuInterconnectError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuInterconnectError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuInterconnectError;

// Down-thunk(s) to bridge GpuInterconnectError methods from ancestors (if any)

// Up-thunk(s) to bridge GpuInterconnectError methods to ancestors (if any)

// Class-specific details for GpuInterconnectError
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError = 
{
    .classInfo.size =               sizeof(GpuInterconnectError),
    .classInfo.classId =            classId(GpuInterconnectError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuInterconnectError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuInterconnectError,
    .pCastInfo =          &__nvoc_castinfo__GpuInterconnectError,
    .pExportInfo =        &__nvoc_export_info__GpuInterconnectError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuInterconnectError __nvoc_metadata__GpuInterconnectError = {
    .rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) this
    .rtti.dtor      = &__nvoc_dtor_GpuInterconnectError,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuInterconnectError, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuInterconnectError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuInterconnectError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuInterconnectError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuInterconnectError = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuInterconnectError.rtti,    // [0]: (gpuinterconnecterr) this
        &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuInterconnectError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuInterconnectError object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuInterconnectError(Dynamic* pThis) {

    GpuInterconnectError *__nvoc_this = (GpuInterconnectError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuInterconnectError(GpuInterconnectError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuInterconnectError object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_INTERCONNECT_ERROR, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuInterconnectError_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuInterconnectError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuinterconnecterrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuInterconnectError_fail__init;
    goto __nvoc_ctor_GpuInterconnectError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuInterconnectError_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuInterconnectError_fail_GpuOperationalEvent:
__nvoc_ctor_GpuInterconnectError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuInterconnectError_1(GpuInterconnectError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuInterconnectError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuInterconnectError(GpuInterconnectError *pThis) {
    __nvoc_init_funcTable_GpuInterconnectError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuInterconnectError(GpuInterconnectError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuInterconnectError = pThis;    // (gpuinterconnecterr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuInterconnectError;    // (gpuinterconnecterr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuInterconnectError(pThis);
}

NV_STATUS __nvoc_objCreate_GpuInterconnectError(GpuInterconnectError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuInterconnectError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuInterconnectError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuInterconnectError));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuInterconnectError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuInterconnectError(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuInterconnectError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuInterconnectError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuInterconnectError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuInterconnectError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 reportingSource = va_arg(__nvoc_args, NvU32);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuInterconnectError((GpuInterconnectError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6f6239 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuFirmwareFault
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_init_funcTable_GpuFirmwareFault(GpuFirmwareFault*);
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
void __nvoc_init_dataField_GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_dtor_GpuFirmwareFault(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuFirmwareFault;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuFirmwareFault;

// Down-thunk(s) to bridge GpuFirmwareFault methods from ancestors (if any)

// Up-thunk(s) to bridge GpuFirmwareFault methods to ancestors (if any)

// Class-specific details for GpuFirmwareFault
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault = 
{
    .classInfo.size =               sizeof(GpuFirmwareFault),
    .classInfo.classId =            classId(GpuFirmwareFault),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuFirmwareFault",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuFirmwareFault,
    .pCastInfo =          &__nvoc_castinfo__GpuFirmwareFault,
    .pExportInfo =        &__nvoc_export_info__GpuFirmwareFault
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuFirmwareFault __nvoc_metadata__GpuFirmwareFault = {
    .rtti.pClassDef = &__nvoc_class_def_GpuFirmwareFault,    // (gpufwfault) this
    .rtti.dtor      = &__nvoc_dtor_GpuFirmwareFault,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuFirmwareFault, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuFirmwareFault, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuFirmwareFault, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuFirmwareFault, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuFirmwareFault = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuFirmwareFault.rtti,    // [0]: (gpufwfault) this
        &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuFirmwareFault = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuFirmwareFault object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuFirmwareFault(Dynamic* pThis) {

    GpuFirmwareFault *__nvoc_this = (GpuFirmwareFault *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuFirmwareFault(GpuFirmwareFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuFirmwareFault object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_FIRMWARE_FAULT, pModuleSignature, eventCode, severity, attributes, scope, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_FW_FAULT, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuFirmwareFault_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuFirmwareFault(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpufwfaultConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuFirmwareFault_fail__init;
    goto __nvoc_ctor_GpuFirmwareFault_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuFirmwareFault_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuFirmwareFault_fail_GpuOperationalEvent:
__nvoc_ctor_GpuFirmwareFault_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuFirmwareFault_1(GpuFirmwareFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuFirmwareFault_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuFirmwareFault(GpuFirmwareFault *pThis) {
    __nvoc_init_funcTable_GpuFirmwareFault_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuFirmwareFault = pThis;    // (gpufwfault) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuFirmwareFault.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuFirmwareFault;    // (gpufwfault) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuFirmwareFault(pThis);
}

NV_STATUS __nvoc_objCreate_GpuFirmwareFault(GpuFirmwareFault **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuFirmwareFault *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuFirmwareFault));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuFirmwareFault));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuFirmwareFault(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuFirmwareFault(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuFirmwareFault_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuFirmwareFault_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuFirmwareFault));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuFirmwareFault(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuFirmwareFault((GpuFirmwareFault **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__07427a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuResourceExhausted
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_init_funcTable_GpuResourceExhausted(GpuResourceExhausted*);
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
void __nvoc_init_dataField_GpuResourceExhausted(GpuResourceExhausted*);
void __nvoc_dtor_GpuResourceExhausted(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuResourceExhausted;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuResourceExhausted;

// Down-thunk(s) to bridge GpuResourceExhausted methods from ancestors (if any)

// Up-thunk(s) to bridge GpuResourceExhausted methods to ancestors (if any)

// Class-specific details for GpuResourceExhausted
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted = 
{
    .classInfo.size =               sizeof(GpuResourceExhausted),
    .classInfo.classId =            classId(GpuResourceExhausted),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuResourceExhausted",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuResourceExhausted,
    .pCastInfo =          &__nvoc_castinfo__GpuResourceExhausted,
    .pExportInfo =        &__nvoc_export_info__GpuResourceExhausted
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuResourceExhausted __nvoc_metadata__GpuResourceExhausted = {
    .rtti.pClassDef = &__nvoc_class_def_GpuResourceExhausted,    // (gpuresexh) this
    .rtti.dtor      = &__nvoc_dtor_GpuResourceExhausted,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuResourceExhausted, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuResourceExhausted, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuResourceExhausted, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuResourceExhausted, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuResourceExhausted = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuResourceExhausted.rtti,    // [0]: (gpuresexh) this
        &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuResourceExhausted = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuResourceExhausted object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuResourceExhausted(Dynamic* pThis) {

    GpuResourceExhausted *__nvoc_this = (GpuResourceExhausted *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuResourceExhausted(GpuResourceExhausted *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuResourceExhausted object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuResourceExhausted(GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_EXHAUSTED, pModuleSignature, eventCode, severity, attributes, scope, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GpuResourceExhausted_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuResourceExhausted(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuresexhConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
    if (status != NV_OK) goto __nvoc_ctor_GpuResourceExhausted_fail__init;
    goto __nvoc_ctor_GpuResourceExhausted_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuResourceExhausted_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuResourceExhausted_fail_GpuOperationalEvent:
__nvoc_ctor_GpuResourceExhausted_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuResourceExhausted_1(GpuResourceExhausted *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuResourceExhausted_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuResourceExhausted(GpuResourceExhausted *pThis) {
    __nvoc_init_funcTable_GpuResourceExhausted_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuResourceExhausted(GpuResourceExhausted *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuResourceExhausted = pThis;    // (gpuresexh) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceExhausted.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceExhausted;    // (gpuresexh) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuResourceExhausted(pThis);
}

NV_STATUS __nvoc_objCreate_GpuResourceExhausted(GpuResourceExhausted **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuResourceExhausted *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuResourceExhausted));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuResourceExhausted));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuResourceExhausted(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuResourceExhausted(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuResourceExhausted_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuResourceExhausted_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuResourceExhausted));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuResourceExhausted(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuResourceExhausted((GpuResourceExhausted **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__3555f8 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuUnclassifiedError
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuUnclassifiedError(GpuUnclassifiedError*);
void __nvoc_init_funcTable_GpuUnclassifiedError(GpuUnclassifiedError*);
NV_STATUS __nvoc_ctor_GpuUnclassifiedError(GpuUnclassifiedError*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
void __nvoc_init_dataField_GpuUnclassifiedError(GpuUnclassifiedError*);
void __nvoc_dtor_GpuUnclassifiedError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuUnclassifiedError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuUnclassifiedError;

// Down-thunk(s) to bridge GpuUnclassifiedError methods from ancestors (if any)

// Up-thunk(s) to bridge GpuUnclassifiedError methods to ancestors (if any)

// Class-specific details for GpuUnclassifiedError
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError = 
{
    .classInfo.size =               sizeof(GpuUnclassifiedError),
    .classInfo.classId =            classId(GpuUnclassifiedError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuUnclassifiedError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuUnclassifiedError,
    .pCastInfo =          &__nvoc_castinfo__GpuUnclassifiedError,
    .pExportInfo =        &__nvoc_export_info__GpuUnclassifiedError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuUnclassifiedError __nvoc_metadata__GpuUnclassifiedError = {
    .rtti.pClassDef = &__nvoc_class_def_GpuUnclassifiedError,    // (gpuunclassifiederr) this
    .rtti.dtor      = &__nvoc_dtor_GpuUnclassifiedError,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuUnclassifiedError, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuUnclassifiedError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuUnclassifiedError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuUnclassifiedError, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuUnclassifiedError = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuUnclassifiedError.rtti,    // [0]: (gpuunclassifiederr) this
        &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuUnclassifiedError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuUnclassifiedError object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuUnclassifiedError(Dynamic* pThis) {

    GpuUnclassifiedError *__nvoc_this = (GpuUnclassifiedError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuUnclassifiedError(GpuUnclassifiedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuUnclassifiedError object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuUnclassifiedError(GpuUnclassifiedError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_UNCLASSIFIED_ERROR, pModuleSignature, eventCode, severity, attributes, scope, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GpuUnclassifiedError_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuUnclassifiedError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuunclassifiederrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
    if (status != NV_OK) goto __nvoc_ctor_GpuUnclassifiedError_fail__init;
    goto __nvoc_ctor_GpuUnclassifiedError_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuUnclassifiedError_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuUnclassifiedError_fail_GpuOperationalEvent:
__nvoc_ctor_GpuUnclassifiedError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuUnclassifiedError_1(GpuUnclassifiedError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuUnclassifiedError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuUnclassifiedError(GpuUnclassifiedError *pThis) {
    __nvoc_init_funcTable_GpuUnclassifiedError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuUnclassifiedError(GpuUnclassifiedError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuUnclassifiedError = pThis;    // (gpuunclassifiederr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuUnclassifiedError.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuUnclassifiedError;    // (gpuunclassifiederr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuUnclassifiedError(pThis);
}

NV_STATUS __nvoc_objCreate_GpuUnclassifiedError(GpuUnclassifiedError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuUnclassifiedError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuUnclassifiedError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuUnclassifiedError));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuUnclassifiedError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuUnclassifiedError(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuUnclassifiedError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuUnclassifiedError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuUnclassifiedError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuUnclassifiedError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuUnclassifiedError((GpuUnclassifiedError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__1cdd99 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInitializationEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuInitializationEvent
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuInitializationEvent(GpuInitializationEvent*);
void __nvoc_init_funcTable_GpuInitializationEvent(GpuInitializationEvent*);
NV_STATUS __nvoc_ctor_GpuInitializationEvent(GpuInitializationEvent*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
void __nvoc_init_dataField_GpuInitializationEvent(GpuInitializationEvent*);
void __nvoc_dtor_GpuInitializationEvent(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuInitializationEvent;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuInitializationEvent;

// Down-thunk(s) to bridge GpuInitializationEvent methods from ancestors (if any)

// Up-thunk(s) to bridge GpuInitializationEvent methods to ancestors (if any)

// Class-specific details for GpuInitializationEvent
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInitializationEvent = 
{
    .classInfo.size =               sizeof(GpuInitializationEvent),
    .classInfo.classId =            classId(GpuInitializationEvent),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuInitializationEvent",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuInitializationEvent,
    .pCastInfo =          &__nvoc_castinfo__GpuInitializationEvent,
    .pExportInfo =        &__nvoc_export_info__GpuInitializationEvent
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuInitializationEvent __nvoc_metadata__GpuInitializationEvent = {
    .rtti.pClassDef = &__nvoc_class_def_GpuInitializationEvent,    // (gpuinitevt) this
    .rtti.dtor      = &__nvoc_dtor_GpuInitializationEvent,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuInitializationEvent, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuInitializationEvent, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuInitializationEvent, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuInitializationEvent, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuInitializationEvent = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuInitializationEvent.rtti,    // [0]: (gpuinitevt) this
        &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuInitializationEvent = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuInitializationEvent object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuInitializationEvent(Dynamic* pThis) {

    GpuInitializationEvent *__nvoc_this = (GpuInitializationEvent *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuInitializationEvent(GpuInitializationEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuInitializationEvent object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuInitializationEvent(GpuInitializationEvent *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_INITIALIZATION, pModuleSignature, eventCode, severity, attributes, scope, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuInitializationEvent_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuInitializationEvent(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuinitevtConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuInitializationEvent_fail__init;
    goto __nvoc_ctor_GpuInitializationEvent_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuInitializationEvent_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuInitializationEvent_fail_GpuOperationalEvent:
__nvoc_ctor_GpuInitializationEvent_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuInitializationEvent_1(GpuInitializationEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuInitializationEvent_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuInitializationEvent(GpuInitializationEvent *pThis) {
    __nvoc_init_funcTable_GpuInitializationEvent_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuInitializationEvent(GpuInitializationEvent *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuInitializationEvent = pThis;    // (gpuinitevt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuInitializationEvent.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuInitializationEvent;    // (gpuinitevt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuInitializationEvent(pThis);
}

NV_STATUS __nvoc_objCreate_GpuInitializationEvent(GpuInitializationEvent **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuInitializationEvent *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuInitializationEvent));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuInitializationEvent));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuInitializationEvent(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuInitializationEvent(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuInitializationEvent_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuInitializationEvent_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuInitializationEvent));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuInitializationEvent(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuInitializationEvent((GpuInitializationEvent **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8cf136 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

// Forward declarations for GpuResourceRetirement
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_init_funcTable_GpuResourceRetirement(GpuResourceRetirement*);
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement*, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);
void __nvoc_init_dataField_GpuResourceRetirement(GpuResourceRetirement*);
void __nvoc_dtor_GpuResourceRetirement(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuResourceRetirement;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuResourceRetirement;

// Down-thunk(s) to bridge GpuResourceRetirement methods from ancestors (if any)

// Up-thunk(s) to bridge GpuResourceRetirement methods to ancestors (if any)

// Class-specific details for GpuResourceRetirement
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement = 
{
    .classInfo.size =               sizeof(GpuResourceRetirement),
    .classInfo.classId =            classId(GpuResourceRetirement),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuResourceRetirement",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuResourceRetirement,
    .pCastInfo =          &__nvoc_castinfo__GpuResourceRetirement,
    .pExportInfo =        &__nvoc_export_info__GpuResourceRetirement
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuResourceRetirement __nvoc_metadata__GpuResourceRetirement = {
    .rtti.pClassDef = &__nvoc_class_def_GpuResourceRetirement,    // (gpuresret) this
    .rtti.dtor      = &__nvoc_dtor_GpuResourceRetirement,
    .rtti.offset    = 0,
    .metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super
    .metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuResourceRetirement, __nvoc_base_GpuOperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^2
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuResourceRetirement, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^3
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuResourceRetirement, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuResourceRetirement, __nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuResourceRetirement = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__GpuResourceRetirement.rtti,    // [0]: (gpuresret) this
        &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.rtti,    // [1]: (gpuopevt) super
        &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [2]: (opevt) super^2
        &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [3]: (event) super^3
        &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [4]: (obj) super^4
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuResourceRetirement = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuResourceRetirement object.
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);
void __nvoc_dtor_GpuResourceRetirement(Dynamic* pThis) {

    GpuResourceRetirement *__nvoc_this = (GpuResourceRetirement *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuOperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuOperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuResourceRetirement(GpuResourceRetirement *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuResourceRetirement object.
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuResourceRetirement(GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuOperationalEvent(&pEvent->__nvoc_base_GpuOperationalEvent, pEventBus, GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_RETIREMENT, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE);
    if (status != NV_OK) goto __nvoc_ctor_GpuResourceRetirement_fail_GpuOperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuResourceRetirement(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuresretConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource);
    if (status != NV_OK) goto __nvoc_ctor_GpuResourceRetirement_fail__init;
    goto __nvoc_ctor_GpuResourceRetirement_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuResourceRetirement_fail__init:
    __nvoc_dtor_GpuOperationalEvent((Dynamic *)&pEvent->__nvoc_base_GpuOperationalEvent);
__nvoc_ctor_GpuResourceRetirement_fail_GpuOperationalEvent:
__nvoc_ctor_GpuResourceRetirement_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuResourceRetirement_1(GpuResourceRetirement *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuResourceRetirement_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuResourceRetirement(GpuResourceRetirement *pThis) {
    __nvoc_init_funcTable_GpuResourceRetirement_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuResourceRetirement(GpuResourceRetirement *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^3
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_pbase_GpuResourceRetirement = pThis;    // (gpuresret) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuOperationalEvent(&pThis->__nvoc_base_GpuOperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^3
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^2
    pThis->__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceRetirement.metadata__GpuOperationalEvent;    // (gpuopevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuResourceRetirement;    // (gpuresret) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuResourceRetirement(pThis);
}

NV_STATUS __nvoc_objCreate_GpuResourceRetirement(GpuResourceRetirement **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuResourceRetirement *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuResourceRetirement));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuResourceRetirement));

    __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuResourceRetirement(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuResourceRetirement(__nvoc_pThis, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuResourceRetirement_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuResourceRetirement_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuResourceRetirement));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuResourceRetirement(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 reportingSource = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuResourceRetirement((GpuResourceRetirement **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__94f24e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDriverInit;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInitializationEvent;

// Forward declarations for GpuDriverInit
void __nvoc_init__GpuInitializationEvent(GpuInitializationEvent*);
void __nvoc_init__GpuDriverInit(GpuDriverInit*);
void __nvoc_init_funcTable_GpuDriverInit(GpuDriverInit*);
NV_STATUS __nvoc_ctor_GpuDriverInit(GpuDriverInit*, struct EventBus *pEventBus);
void __nvoc_init_dataField_GpuDriverInit(GpuDriverInit*);
void __nvoc_dtor_GpuDriverInit(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuDriverInit;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuDriverInit;

// Down-thunk(s) to bridge GpuDriverInit methods from ancestors (if any)

// Up-thunk(s) to bridge GpuDriverInit methods to ancestors (if any)

// Class-specific details for GpuDriverInit
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDriverInit = 
{
    .classInfo.size =               sizeof(GpuDriverInit),
    .classInfo.classId =            classId(GpuDriverInit),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuDriverInit",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuDriverInit,
    .pCastInfo =          &__nvoc_castinfo__GpuDriverInit,
    .pExportInfo =        &__nvoc_export_info__GpuDriverInit
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuDriverInit __nvoc_metadata__GpuDriverInit = {
    .rtti.pClassDef = &__nvoc_class_def_GpuDriverInit,    // (gpudrvinit) this
    .rtti.dtor      = &__nvoc_dtor_GpuDriverInit,
    .rtti.offset    = 0,
    .metadata__GpuInitializationEvent.rtti.pClassDef = &__nvoc_class_def_GpuInitializationEvent,    // (gpuinitevt) super
    .metadata__GpuInitializationEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInitializationEvent.rtti.offset    = NV_OFFSETOF(GpuDriverInit, __nvoc_base_GpuInitializationEvent),
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GpuDriverInit, __nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuDriverInit, __nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuDriverInit, __nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuDriverInit, __nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuDriverInit = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GpuDriverInit.rtti,    // [0]: (gpudrvinit) this
        &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.rtti,    // [1]: (gpuinitevt) super
        &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuDriverInit = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuDriverInit object.
void __nvoc_dtor_GpuInitializationEvent(Dynamic*);
void __nvoc_dtor_GpuDriverInit(Dynamic* pThis) {

    GpuDriverInit *__nvoc_this = (GpuDriverInit *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInitializationEvent((Dynamic *) &__nvoc_this->__nvoc_base_GpuInitializationEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuDriverInit(GpuDriverInit *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuDriverInit object.
NV_STATUS __nvoc_ctor_GpuInitializationEvent(GpuInitializationEvent *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuDriverInit(GpuDriverInit *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInitializationEvent(&pEvent->__nvoc_base_GpuInitializationEvent, pEventBus, "GPU", GPU_OP_EVENT_CODE_DRIVER_INIT, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE);
    if (status != NV_OK) goto __nvoc_ctor_GpuDriverInit_fail_GpuInitializationEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuDriverInit(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpudrvinitConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_GpuDriverInit_fail__init;
    goto __nvoc_ctor_GpuDriverInit_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuDriverInit_fail__init:
    __nvoc_dtor_GpuInitializationEvent((Dynamic *)&pEvent->__nvoc_base_GpuInitializationEvent);
__nvoc_ctor_GpuDriverInit_fail_GpuInitializationEvent:
__nvoc_ctor_GpuDriverInit_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuDriverInit_1(GpuDriverInit *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuDriverInit_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuDriverInit(GpuDriverInit *pThis) {
    __nvoc_init_funcTable_GpuDriverInit_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuDriverInit(GpuDriverInit *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInitializationEvent = &pThis->__nvoc_base_GpuInitializationEvent;    // (gpuinitevt) super
    pThis->__nvoc_pbase_GpuDriverInit = pThis;    // (gpudrvinit) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInitializationEvent(&pThis->__nvoc_base_GpuInitializationEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInitializationEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit.metadata__GpuInitializationEvent;    // (gpuinitevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuDriverInit;    // (gpudrvinit) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuDriverInit(pThis);
}

NV_STATUS __nvoc_objCreate_GpuDriverInit(GpuDriverInit **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuDriverInit *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuDriverInit));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuDriverInit));

    __nvoc_pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuDriverInit(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuDriverInit(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuDriverInit_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuDriverInit_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInitializationEvent.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuDriverInit));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuDriverInit(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_GpuDriverInit((GpuDriverInit **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}

