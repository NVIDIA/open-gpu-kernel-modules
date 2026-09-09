#define NVOC_GSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gsp_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__3ef5d1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspRpcTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout;

// Forward declarations for GspRpcTimeout
void __nvoc_init__GpuTimeout(GpuTimeout*);
void __nvoc_init__GspRpcTimeout(GspRpcTimeout*);
void __nvoc_init_funcTable_GspRpcTimeout(GspRpcTimeout*);
NV_STATUS __nvoc_ctor_GspRpcTimeout(GspRpcTimeout*, struct EventBus *pEventBus, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs, NvU32 gpuInstance, NvU32 expectedFunc, const char *pRpcName, NvU32 expectedSequence, NvU64 historyData0, NvU64 historyData1, NvU64 waitedSeconds);
void __nvoc_init_dataField_GspRpcTimeout(GspRpcTimeout*);
void __nvoc_dtor_GspRpcTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspRpcTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspRpcTimeout;

// Down-thunk(s) to bridge GspRpcTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge GspRpcTimeout methods to ancestors (if any)

// Class-specific details for GspRpcTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_GspRpcTimeout = 
{
    .classInfo.size =               sizeof(GspRpcTimeout),
    .classInfo.classId =            classId(GspRpcTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspRpcTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspRpcTimeout,
    .pCastInfo =          &__nvoc_castinfo__GspRpcTimeout,
    .pExportInfo =        &__nvoc_export_info__GspRpcTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspRpcTimeout __nvoc_metadata__GspRpcTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_GspRpcTimeout,    // (gsprpcto) this
    .rtti.dtor      = &__nvoc_dtor_GspRpcTimeout,
    .rtti.offset    = 0,
    .metadata__GpuTimeout.rtti.pClassDef = &__nvoc_class_def_GpuTimeout,    // (gputimeout) super
    .metadata__GpuTimeout.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.rtti.offset    = NV_OFFSETOF(GspRpcTimeout, __nvoc_base_GpuTimeout),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspRpcTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspRpcTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspRpcTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspRpcTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspRpcTimeout = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspRpcTimeout.rtti,    // [0]: (gsprpcto) this
        &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.rtti,    // [1]: (gputimeout) super
        &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspRpcTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspRpcTimeout object.
void __nvoc_dtor_GpuTimeout(Dynamic*);
void __nvoc_dtor_GspRpcTimeout(Dynamic* pThis) {

    GspRpcTimeout *__nvoc_this = (GspRpcTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuTimeout((Dynamic *) &__nvoc_this->__nvoc_base_GpuTimeout);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspRpcTimeout(GspRpcTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspRpcTimeout object.
NV_STATUS __nvoc_ctor_GpuTimeout(GpuTimeout *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspRpcTimeout(GspRpcTimeout *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs, NvU32 gpuInstance, NvU32 expectedFunc, const char *pRpcName, NvU32 expectedSequence, NvU64 historyData0, NvU64 historyData1, NvU64 waitedSeconds) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuTimeout(&pEvent->__nvoc_base_GpuTimeout, pEventBus, "GPU-GSP", GSP_OP_EVENT_CODE_RPC_TIMEOUT, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, timeoutNs, elapsedNs, "GSP RPC response", (severity == OPERATIONAL_EVENT_SEVERITY_FATAL) ? GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR : GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_GspRpcTimeout_fail_GpuTimeout;

    // Initialize data fields.
    __nvoc_init_dataField_GspRpcTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gsprpctoConstruct(pEvent, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds);
    if (status != NV_OK) goto __nvoc_ctor_GspRpcTimeout_fail__init;
    goto __nvoc_ctor_GspRpcTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspRpcTimeout_fail__init:
    __nvoc_dtor_GpuTimeout((Dynamic *)&pEvent->__nvoc_base_GpuTimeout);
__nvoc_ctor_GspRpcTimeout_fail_GpuTimeout:
__nvoc_ctor_GspRpcTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspRpcTimeout_1(GspRpcTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspRpcTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspRpcTimeout(GspRpcTimeout *pThis) {
    __nvoc_init_funcTable_GspRpcTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspRpcTimeout(GspRpcTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuTimeout = &pThis->__nvoc_base_GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_pbase_GspRpcTimeout = pThis;    // (gsprpcto) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuTimeout(&pThis->__nvoc_base_GpuTimeout);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuTimeout.__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout.metadata__GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspRpcTimeout;    // (gsprpcto) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspRpcTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_GspRpcTimeout(GspRpcTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs, NvU32 gpuInstance, NvU32 expectedFunc, const char *pRpcName, NvU32 expectedSequence, NvU64 historyData0, NvU64 historyData1, NvU64 waitedSeconds)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspRpcTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspRpcTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspRpcTimeout));

    __nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GspRpcTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspRpcTimeout(__nvoc_pThis, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspRpcTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspRpcTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspRpcTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspRpcTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU64 timeoutNs = va_arg(__nvoc_args, NvU64);
    NvU64 elapsedNs = va_arg(__nvoc_args, NvU64);
    NvU32 gpuInstance = va_arg(__nvoc_args, NvU32);
    NvU32 expectedFunc = va_arg(__nvoc_args, NvU32);
    const char *pRpcName = va_arg(__nvoc_args, const char *);
    NvU32 expectedSequence = va_arg(__nvoc_args, NvU32);
    NvU64 historyData0 = va_arg(__nvoc_args, NvU64);
    NvU64 historyData1 = va_arg(__nvoc_args, NvU64);
    NvU64 waitedSeconds = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_GspRpcTimeout((GspRpcTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__447333 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspHeartbeatTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout;

// Forward declarations for GspHeartbeatTimeout
void __nvoc_init__GpuTimeout(GpuTimeout*);
void __nvoc_init__GspHeartbeatTimeout(GspHeartbeatTimeout*);
void __nvoc_init_funcTable_GspHeartbeatTimeout(GspHeartbeatTimeout*);
NV_STATUS __nvoc_ctor_GspHeartbeatTimeout(GspHeartbeatTimeout*, struct EventBus *pEventBus, NvU32 eventCode, const char *pHeartbeatSource, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs);
void __nvoc_init_dataField_GspHeartbeatTimeout(GspHeartbeatTimeout*);
void __nvoc_dtor_GspHeartbeatTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspHeartbeatTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspHeartbeatTimeout;

// Down-thunk(s) to bridge GspHeartbeatTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge GspHeartbeatTimeout methods to ancestors (if any)

// Class-specific details for GspHeartbeatTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_GspHeartbeatTimeout = 
{
    .classInfo.size =               sizeof(GspHeartbeatTimeout),
    .classInfo.classId =            classId(GspHeartbeatTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspHeartbeatTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspHeartbeatTimeout,
    .pCastInfo =          &__nvoc_castinfo__GspHeartbeatTimeout,
    .pExportInfo =        &__nvoc_export_info__GspHeartbeatTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspHeartbeatTimeout __nvoc_metadata__GspHeartbeatTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_GspHeartbeatTimeout,    // (gsphbto) this
    .rtti.dtor      = &__nvoc_dtor_GspHeartbeatTimeout,
    .rtti.offset    = 0,
    .metadata__GpuTimeout.rtti.pClassDef = &__nvoc_class_def_GpuTimeout,    // (gputimeout) super
    .metadata__GpuTimeout.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.rtti.offset    = NV_OFFSETOF(GspHeartbeatTimeout, __nvoc_base_GpuTimeout),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspHeartbeatTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspHeartbeatTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspHeartbeatTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspHeartbeatTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspHeartbeatTimeout = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspHeartbeatTimeout.rtti,    // [0]: (gsphbto) this
        &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.rtti,    // [1]: (gputimeout) super
        &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspHeartbeatTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspHeartbeatTimeout object.
void __nvoc_dtor_GpuTimeout(Dynamic*);
void __nvoc_dtor_GspHeartbeatTimeout(Dynamic* pThis) {

    GspHeartbeatTimeout *__nvoc_this = (GspHeartbeatTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuTimeout((Dynamic *) &__nvoc_this->__nvoc_base_GpuTimeout);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspHeartbeatTimeout(GspHeartbeatTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspHeartbeatTimeout object.
NV_STATUS __nvoc_ctor_GpuTimeout(GpuTimeout *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspHeartbeatTimeout(GspHeartbeatTimeout *pEvent, struct EventBus *pEventBus, NvU32 eventCode, const char *pHeartbeatSource, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuTimeout(&pEvent->__nvoc_base_GpuTimeout, pEventBus, "GPU-GSP", eventCode, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, timeoutNs, elapsedNs, pHeartbeatSource, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GspHeartbeatTimeout_fail_GpuTimeout;

    // Initialize data fields.
    __nvoc_init_dataField_GspHeartbeatTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gsphbtoConstruct(pEvent, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs);
    if (status != NV_OK) goto __nvoc_ctor_GspHeartbeatTimeout_fail__init;
    goto __nvoc_ctor_GspHeartbeatTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspHeartbeatTimeout_fail__init:
    __nvoc_dtor_GpuTimeout((Dynamic *)&pEvent->__nvoc_base_GpuTimeout);
__nvoc_ctor_GspHeartbeatTimeout_fail_GpuTimeout:
__nvoc_ctor_GspHeartbeatTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspHeartbeatTimeout_1(GspHeartbeatTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspHeartbeatTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspHeartbeatTimeout(GspHeartbeatTimeout *pThis) {
    __nvoc_init_funcTable_GspHeartbeatTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspHeartbeatTimeout(GspHeartbeatTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuTimeout = &pThis->__nvoc_base_GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_pbase_GspHeartbeatTimeout = pThis;    // (gsphbto) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuTimeout(&pThis->__nvoc_base_GpuTimeout);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuTimeout.__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout.metadata__GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspHeartbeatTimeout;    // (gsphbto) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspHeartbeatTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_GspHeartbeatTimeout(GspHeartbeatTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 eventCode, const char *pHeartbeatSource, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspHeartbeatTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspHeartbeatTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspHeartbeatTimeout));

    __nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GspHeartbeatTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspHeartbeatTimeout(__nvoc_pThis, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspHeartbeatTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspHeartbeatTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspHeartbeatTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspHeartbeatTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    const char *pHeartbeatSource = va_arg(__nvoc_args, const char *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU64 timeoutNs = va_arg(__nvoc_args, NvU64);
    NvU64 elapsedNs = va_arg(__nvoc_args, NvU64);

    return __nvoc_objCreate_GspHeartbeatTimeout((GspHeartbeatTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5e0cda = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspFirmwareFault;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;

// Forward declarations for GspFirmwareFault
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_init__GspFirmwareFault(GspFirmwareFault*);
void __nvoc_init_funcTable_GspFirmwareFault(GspFirmwareFault*);
NV_STATUS __nvoc_ctor_GspFirmwareFault(GspFirmwareFault*, struct EventBus *pEventBus);
void __nvoc_init_dataField_GspFirmwareFault(GspFirmwareFault*);
void __nvoc_dtor_GspFirmwareFault(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspFirmwareFault;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspFirmwareFault;

// Down-thunk(s) to bridge GspFirmwareFault methods from ancestors (if any)

// Up-thunk(s) to bridge GspFirmwareFault methods to ancestors (if any)

// Class-specific details for GspFirmwareFault
const struct NVOC_CLASS_DEF __nvoc_class_def_GspFirmwareFault = 
{
    .classInfo.size =               sizeof(GspFirmwareFault),
    .classInfo.classId =            classId(GspFirmwareFault),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspFirmwareFault",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspFirmwareFault,
    .pCastInfo =          &__nvoc_castinfo__GspFirmwareFault,
    .pExportInfo =        &__nvoc_export_info__GspFirmwareFault
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspFirmwareFault __nvoc_metadata__GspFirmwareFault = {
    .rtti.pClassDef = &__nvoc_class_def_GspFirmwareFault,    // (gspfwfault) this
    .rtti.dtor      = &__nvoc_dtor_GspFirmwareFault,
    .rtti.offset    = 0,
    .metadata__GpuFirmwareFault.rtti.pClassDef = &__nvoc_class_def_GpuFirmwareFault,    // (gpufwfault) super
    .metadata__GpuFirmwareFault.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.rtti.offset    = NV_OFFSETOF(GspFirmwareFault, __nvoc_base_GpuFirmwareFault),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspFirmwareFault, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspFirmwareFault, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspFirmwareFault, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspFirmwareFault, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspFirmwareFault = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspFirmwareFault.rtti,    // [0]: (gspfwfault) this
        &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.rtti,    // [1]: (gpufwfault) super
        &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspFirmwareFault = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspFirmwareFault object.
void __nvoc_dtor_GpuFirmwareFault(Dynamic*);
void __nvoc_dtor_GspFirmwareFault(Dynamic* pThis) {

    GspFirmwareFault *__nvoc_this = (GspFirmwareFault *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuFirmwareFault((Dynamic *) &__nvoc_this->__nvoc_base_GpuFirmwareFault);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspFirmwareFault(GspFirmwareFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspFirmwareFault object.
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspFirmwareFault(GspFirmwareFault *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuFirmwareFault(&pEvent->__nvoc_base_GpuFirmwareFault, pEventBus, "GPU-GSP", GSP_OP_EVENT_CODE_FIRMWARE_FAULT, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GspFirmwareFault_fail_GpuFirmwareFault;

    // Initialize data fields.
    __nvoc_init_dataField_GspFirmwareFault(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gspfwfaultConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_GspFirmwareFault_fail__init;
    goto __nvoc_ctor_GspFirmwareFault_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspFirmwareFault_fail__init:
    __nvoc_dtor_GpuFirmwareFault((Dynamic *)&pEvent->__nvoc_base_GpuFirmwareFault);
__nvoc_ctor_GspFirmwareFault_fail_GpuFirmwareFault:
__nvoc_ctor_GspFirmwareFault_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspFirmwareFault_1(GspFirmwareFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspFirmwareFault_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspFirmwareFault(GspFirmwareFault *pThis) {
    __nvoc_init_funcTable_GspFirmwareFault_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspFirmwareFault(GspFirmwareFault *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuFirmwareFault = &pThis->__nvoc_base_GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_pbase_GspFirmwareFault = pThis;    // (gspfwfault) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuFirmwareFault(&pThis->__nvoc_base_GpuFirmwareFault);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault.metadata__GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspFirmwareFault;    // (gspfwfault) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspFirmwareFault(pThis);
}

NV_STATUS __nvoc_objCreate_GspFirmwareFault(GspFirmwareFault **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspFirmwareFault *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspFirmwareFault));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspFirmwareFault));

    __nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GspFirmwareFault(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspFirmwareFault(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspFirmwareFault_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspFirmwareFault_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspFirmwareFault));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspFirmwareFault(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_GspFirmwareFault((GspFirmwareFault **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a610b0 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoison;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for GspPoison
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__GspPoison(GspPoison*);
void __nvoc_init_funcTable_GspPoison(GspPoison*);
NV_STATUS __nvoc_ctor_GspPoison(GspPoison*, struct EventBus *pEventBus);
void __nvoc_init_dataField_GspPoison(GspPoison*);
void __nvoc_dtor_GspPoison(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GspPoison;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GspPoison;

// Down-thunk(s) to bridge GspPoison methods from ancestors (if any)

// Up-thunk(s) to bridge GspPoison methods to ancestors (if any)

// Class-specific details for GspPoison
const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoison = 
{
    .classInfo.size =               sizeof(GspPoison),
    .classInfo.classId =            classId(GspPoison),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GspPoison",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GspPoison,
    .pCastInfo =          &__nvoc_castinfo__GspPoison,
    .pExportInfo =        &__nvoc_export_info__GspPoison
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GspPoison __nvoc_metadata__GspPoison = {
    .rtti.pClassDef = &__nvoc_class_def_GspPoison,    // (gsppoison) this
    .rtti.dtor      = &__nvoc_dtor_GspPoison,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(GspPoison, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(GspPoison, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GspPoison, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GspPoison, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GspPoison, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GspPoison = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__GspPoison.rtti,    // [0]: (gsppoison) this
        &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GspPoison = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GspPoison object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_GspPoison(Dynamic* pThis) {

    GspPoison *__nvoc_this = (GspPoison *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GspPoison(GspPoison *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GspPoison object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GspPoison(GspPoison *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-GSP", GSP_OP_EVENT_CODE_POISON, OPERATIONAL_EVENT_SEVERITY_FATAL, (((0U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_FW_FAULT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_GspPoison_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_GspPoison(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gsppoisonConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_GspPoison_fail__init;
    goto __nvoc_ctor_GspPoison_exit; // Success

    // Unwind on error.
__nvoc_ctor_GspPoison_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_GspPoison_fail_GpuMemoryIntegrityError:
__nvoc_ctor_GspPoison_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GspPoison_1(GspPoison *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GspPoison_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GspPoison(GspPoison *pThis) {
    __nvoc_init_funcTable_GspPoison_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GspPoison(GspPoison *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_GspPoison = pThis;    // (gsppoison) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GspPoison;    // (gsppoison) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GspPoison(pThis);
}

NV_STATUS __nvoc_objCreate_GspPoison(GspPoison **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GspPoison *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GspPoison));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GspPoison));

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
    __nvoc_init__GspPoison(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GspPoison(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GspPoison_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GspPoison_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GspPoison));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GspPoison(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_GspPoison((GspPoison **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}

