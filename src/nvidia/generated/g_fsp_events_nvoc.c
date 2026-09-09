#define NVOC_FSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_fsp_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8a29ef = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FspBootTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout;

// Forward declarations for FspBootTimeout
void __nvoc_init__GpuTimeout(GpuTimeout*);
void __nvoc_init__FspBootTimeout(FspBootTimeout*);
void __nvoc_init_funcTable_FspBootTimeout(FspBootTimeout*);
NV_STATUS __nvoc_ctor_FspBootTimeout(FspBootTimeout*, struct EventBus *pEventBus, NvU64 timeoutNs, NvU64 elapsedNs, NV_STATUS errorStatus, NvU32 fspBootComplete, NvU32 scratch0, NvU32 scratch1, NvU32 scratch2, NvU32 scratch3);
void __nvoc_init_dataField_FspBootTimeout(FspBootTimeout*);
void __nvoc_dtor_FspBootTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FspBootTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FspBootTimeout;

// Down-thunk(s) to bridge FspBootTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge FspBootTimeout methods to ancestors (if any)

// Class-specific details for FspBootTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_FspBootTimeout = 
{
    .classInfo.size =               sizeof(FspBootTimeout),
    .classInfo.classId =            classId(FspBootTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FspBootTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FspBootTimeout,
    .pCastInfo =          &__nvoc_castinfo__FspBootTimeout,
    .pExportInfo =        &__nvoc_export_info__FspBootTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FspBootTimeout __nvoc_metadata__FspBootTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_FspBootTimeout,    // (fspbootto) this
    .rtti.dtor      = &__nvoc_dtor_FspBootTimeout,
    .rtti.offset    = 0,
    .metadata__GpuTimeout.rtti.pClassDef = &__nvoc_class_def_GpuTimeout,    // (gputimeout) super
    .metadata__GpuTimeout.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.rtti.offset    = NV_OFFSETOF(FspBootTimeout, __nvoc_base_GpuTimeout),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FspBootTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FspBootTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FspBootTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FspBootTimeout, __nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FspBootTimeout = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FspBootTimeout.rtti,    // [0]: (fspbootto) this
        &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.rtti,    // [1]: (gputimeout) super
        &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FspBootTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FspBootTimeout object.
void __nvoc_dtor_GpuTimeout(Dynamic*);
void __nvoc_dtor_FspBootTimeout(Dynamic* pThis) {

    FspBootTimeout *__nvoc_this = (FspBootTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuTimeout((Dynamic *) &__nvoc_this->__nvoc_base_GpuTimeout);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FspBootTimeout(FspBootTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FspBootTimeout object.
NV_STATUS __nvoc_ctor_GpuTimeout(GpuTimeout *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FspBootTimeout(FspBootTimeout *pEvent, struct EventBus *pEventBus, NvU64 timeoutNs, NvU64 elapsedNs, NV_STATUS errorStatus, NvU32 fspBootComplete, NvU32 scratch0, NvU32 scratch1, NvU32 scratch2, NvU32 scratch3) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuTimeout(&pEvent->__nvoc_base_GpuTimeout, pEventBus, "GPU-FSP", FSP_OP_EVENT_CODE_BOOT_TIMEOUT, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, timeoutNs, elapsedNs, "FSP boot completion", GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FspBootTimeout_fail_GpuTimeout;

    // Initialize data fields.
    __nvoc_init_dataField_FspBootTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fspboottoConstruct(pEvent, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3);
    if (status != NV_OK) goto __nvoc_ctor_FspBootTimeout_fail__init;
    goto __nvoc_ctor_FspBootTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_FspBootTimeout_fail__init:
    __nvoc_dtor_GpuTimeout((Dynamic *)&pEvent->__nvoc_base_GpuTimeout);
__nvoc_ctor_FspBootTimeout_fail_GpuTimeout:
__nvoc_ctor_FspBootTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FspBootTimeout_1(FspBootTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FspBootTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FspBootTimeout(FspBootTimeout *pThis) {
    __nvoc_init_funcTable_FspBootTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FspBootTimeout(FspBootTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuTimeout = &pThis->__nvoc_base_GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_pbase_FspBootTimeout = pThis;    // (fspbootto) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuTimeout(&pThis->__nvoc_base_GpuTimeout);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuTimeout.__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout.metadata__GpuTimeout;    // (gputimeout) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FspBootTimeout;    // (fspbootto) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FspBootTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_FspBootTimeout(FspBootTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU64 timeoutNs, NvU64 elapsedNs, NV_STATUS errorStatus, NvU32 fspBootComplete, NvU32 scratch0, NvU32 scratch1, NvU32 scratch2, NvU32 scratch3)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FspBootTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FspBootTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FspBootTimeout));

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
    __nvoc_init__FspBootTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FspBootTimeout(__nvoc_pThis, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FspBootTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FspBootTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuTimeout.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FspBootTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FspBootTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU64 timeoutNs = va_arg(__nvoc_args, NvU64);
    NvU64 elapsedNs = va_arg(__nvoc_args, NvU64);
    NV_STATUS errorStatus = va_arg(__nvoc_args, NV_STATUS);
    NvU32 fspBootComplete = va_arg(__nvoc_args, NvU32);
    NvU32 scratch0 = va_arg(__nvoc_args, NvU32);
    NvU32 scratch1 = va_arg(__nvoc_args, NvU32);
    NvU32 scratch2 = va_arg(__nvoc_args, NvU32);
    NvU32 scratch3 = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_FspBootTimeout((FspBootTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5afb18 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FspFuseError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;

// Forward declarations for FspFuseError
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_init__FspFuseError(FspFuseError*);
void __nvoc_init_funcTable_FspFuseError(FspFuseError*);
NV_STATUS __nvoc_ctor_FspFuseError(FspFuseError*, struct EventBus *pEventBus, NvU32 fuseStatus);
void __nvoc_init_dataField_FspFuseError(FspFuseError*);
void __nvoc_dtor_FspFuseError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__FspFuseError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__FspFuseError;

// Down-thunk(s) to bridge FspFuseError methods from ancestors (if any)

// Up-thunk(s) to bridge FspFuseError methods to ancestors (if any)

// Class-specific details for FspFuseError
const struct NVOC_CLASS_DEF __nvoc_class_def_FspFuseError = 
{
    .classInfo.size =               sizeof(FspFuseError),
    .classInfo.classId =            classId(FspFuseError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "FspFuseError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_FspFuseError,
    .pCastInfo =          &__nvoc_castinfo__FspFuseError,
    .pExportInfo =        &__nvoc_export_info__FspFuseError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__FspFuseError __nvoc_metadata__FspFuseError = {
    .rtti.pClassDef = &__nvoc_class_def_FspFuseError,    // (fspfuseerr) this
    .rtti.dtor      = &__nvoc_dtor_FspFuseError,
    .rtti.offset    = 0,
    .metadata__GpuFirmwareFault.rtti.pClassDef = &__nvoc_class_def_GpuFirmwareFault,    // (gpufwfault) super
    .metadata__GpuFirmwareFault.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.rtti.offset    = NV_OFFSETOF(FspFuseError, __nvoc_base_GpuFirmwareFault),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(FspFuseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(FspFuseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(FspFuseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(FspFuseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__FspFuseError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__FspFuseError.rtti,    // [0]: (fspfuseerr) this
        &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.rtti,    // [1]: (gpufwfault) super
        &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__FspFuseError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct FspFuseError object.
void __nvoc_dtor_GpuFirmwareFault(Dynamic*);
void __nvoc_dtor_FspFuseError(Dynamic* pThis) {

    FspFuseError *__nvoc_this = (FspFuseError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuFirmwareFault((Dynamic *) &__nvoc_this->__nvoc_base_GpuFirmwareFault);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_FspFuseError(FspFuseError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct FspFuseError object.
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_FspFuseError(FspFuseError *pEvent, struct EventBus *pEventBus, NvU32 fuseStatus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuFirmwareFault(&pEvent->__nvoc_base_GpuFirmwareFault, pEventBus, "GPU-FSP", FSP_OP_EVENT_CODE_FUSE_ERROR, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_FspFuseError_fail_GpuFirmwareFault;

    // Initialize data fields.
    __nvoc_init_dataField_FspFuseError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_fspfuseerrConstruct(pEvent, pEventBus, fuseStatus);
    if (status != NV_OK) goto __nvoc_ctor_FspFuseError_fail__init;
    goto __nvoc_ctor_FspFuseError_exit; // Success

    // Unwind on error.
__nvoc_ctor_FspFuseError_fail__init:
    __nvoc_dtor_GpuFirmwareFault((Dynamic *)&pEvent->__nvoc_base_GpuFirmwareFault);
__nvoc_ctor_FspFuseError_fail_GpuFirmwareFault:
__nvoc_ctor_FspFuseError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_FspFuseError_1(FspFuseError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_FspFuseError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_FspFuseError(FspFuseError *pThis) {
    __nvoc_init_funcTable_FspFuseError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__FspFuseError(FspFuseError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuFirmwareFault = &pThis->__nvoc_base_GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_pbase_FspFuseError = pThis;    // (fspfuseerr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuFirmwareFault(&pThis->__nvoc_base_GpuFirmwareFault);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError.metadata__GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__FspFuseError;    // (fspfuseerr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_FspFuseError(pThis);
}

NV_STATUS __nvoc_objCreate_FspFuseError(FspFuseError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 fuseStatus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    FspFuseError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(FspFuseError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(FspFuseError));

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
    __nvoc_init__FspFuseError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_FspFuseError(__nvoc_pThis, pEventBus, fuseStatus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_FspFuseError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_FspFuseError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(FspFuseError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_FspFuseError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 fuseStatus = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_FspFuseError((FspFuseError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, fuseStatus);
}

