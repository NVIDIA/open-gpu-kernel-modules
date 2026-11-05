#define NVOC_PCIE_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_pcie_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__2fb654 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieFatalTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieFatalTimeout
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieFatalTimeout(PcieFatalTimeout*);
void __nvoc_init_funcTable_PcieFatalTimeout(PcieFatalTimeout*);
NV_STATUS __nvoc_ctor_PcieFatalTimeout(PcieFatalTimeout*, struct EventBus *pEventBus, NvU32 errorType, NvU32 regVal);
void __nvoc_init_dataField_PcieFatalTimeout(PcieFatalTimeout*);
void __nvoc_dtor_PcieFatalTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieFatalTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieFatalTimeout;

// Down-thunk(s) to bridge PcieFatalTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge PcieFatalTimeout methods to ancestors (if any)

// Class-specific details for PcieFatalTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieFatalTimeout = 
{
    .classInfo.size =               sizeof(PcieFatalTimeout),
    .classInfo.classId =            classId(PcieFatalTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieFatalTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieFatalTimeout,
    .pCastInfo =          &__nvoc_castinfo__PcieFatalTimeout,
    .pExportInfo =        &__nvoc_export_info__PcieFatalTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieFatalTimeout __nvoc_metadata__PcieFatalTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_PcieFatalTimeout,    // (pciefataltimeout) this
    .rtti.dtor      = &__nvoc_dtor_PcieFatalTimeout,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieFatalTimeout, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieFatalTimeout, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieFatalTimeout, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieFatalTimeout, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieFatalTimeout, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieFatalTimeout = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieFatalTimeout.rtti,    // [0]: (pciefataltimeout) this
        &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieFatalTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieFatalTimeout object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieFatalTimeout(Dynamic* pThis) {

    PcieFatalTimeout *__nvoc_this = (PcieFatalTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieFatalTimeout(PcieFatalTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieFatalTimeout object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieFatalTimeout(PcieFatalTimeout *pEvent, struct EventBus *pEventBus, NvU32 errorType, NvU32 regVal) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-PCIE", PCIE_OP_EVENT_CODE_FATAL_TIMEOUT, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieFatalTimeout_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieFatalTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pciefataltimeoutConstruct(pEvent, pEventBus, errorType, regVal);
    if (status != NV_OK) goto __nvoc_ctor_PcieFatalTimeout_fail__init;
    goto __nvoc_ctor_PcieFatalTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieFatalTimeout_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieFatalTimeout_fail_GpuInterconnectError:
__nvoc_ctor_PcieFatalTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieFatalTimeout_1(PcieFatalTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieFatalTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieFatalTimeout(PcieFatalTimeout *pThis) {
    __nvoc_init_funcTable_PcieFatalTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieFatalTimeout(PcieFatalTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieFatalTimeout = pThis;    // (pciefataltimeout) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieFatalTimeout;    // (pciefataltimeout) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieFatalTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_PcieFatalTimeout(PcieFatalTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 errorType, NvU32 regVal)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieFatalTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieFatalTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieFatalTimeout));

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
    __nvoc_init__PcieFatalTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieFatalTimeout(__nvoc_pThis, pEventBus, errorType, regVal);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieFatalTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieFatalTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieFatalTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieFatalTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 errorType = va_arg(__nvoc_args, NvU32);
    NvU32 regVal = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PcieFatalTimeout((PcieFatalTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, errorType, regVal);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__55f461 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieCorrectableErrorFault;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieCorrectableErrorFault
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieCorrectableErrorFault(PcieCorrectableErrorFault*);
void __nvoc_init_funcTable_PcieCorrectableErrorFault(PcieCorrectableErrorFault*);
NV_STATUS __nvoc_ctor_PcieCorrectableErrorFault(PcieCorrectableErrorFault*, struct EventBus *pEventBus, NvU32 ceRateTupleNum, NvU32 ceRateNerdNumber, NvU32 ceRateTupleVerNum);
void __nvoc_init_dataField_PcieCorrectableErrorFault(PcieCorrectableErrorFault*);
void __nvoc_dtor_PcieCorrectableErrorFault(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieCorrectableErrorFault;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieCorrectableErrorFault;

// Down-thunk(s) to bridge PcieCorrectableErrorFault methods from ancestors (if any)

// Up-thunk(s) to bridge PcieCorrectableErrorFault methods to ancestors (if any)

// Class-specific details for PcieCorrectableErrorFault
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieCorrectableErrorFault = 
{
    .classInfo.size =               sizeof(PcieCorrectableErrorFault),
    .classInfo.classId =            classId(PcieCorrectableErrorFault),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieCorrectableErrorFault",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieCorrectableErrorFault,
    .pCastInfo =          &__nvoc_castinfo__PcieCorrectableErrorFault,
    .pExportInfo =        &__nvoc_export_info__PcieCorrectableErrorFault
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieCorrectableErrorFault __nvoc_metadata__PcieCorrectableErrorFault = {
    .rtti.pClassDef = &__nvoc_class_def_PcieCorrectableErrorFault,    // (pciecorrecterr) this
    .rtti.dtor      = &__nvoc_dtor_PcieCorrectableErrorFault,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieCorrectableErrorFault, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieCorrectableErrorFault, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieCorrectableErrorFault, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieCorrectableErrorFault, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieCorrectableErrorFault, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieCorrectableErrorFault = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieCorrectableErrorFault.rtti,    // [0]: (pciecorrecterr) this
        &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieCorrectableErrorFault = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieCorrectableErrorFault object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieCorrectableErrorFault(Dynamic* pThis) {

    PcieCorrectableErrorFault *__nvoc_this = (PcieCorrectableErrorFault *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieCorrectableErrorFault(PcieCorrectableErrorFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieCorrectableErrorFault object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieCorrectableErrorFault(PcieCorrectableErrorFault *pEvent, struct EventBus *pEventBus, NvU32 ceRateTupleNum, NvU32 ceRateNerdNumber, NvU32 ceRateTupleVerNum) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-PCIE", PCIE_OP_EVENT_CODE_CORRECTABLE_ERROR_FAULT, OPERATIONAL_EVENT_SEVERITY_CORRECTED, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE);
    if (status != NV_OK) goto __nvoc_ctor_PcieCorrectableErrorFault_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieCorrectableErrorFault(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pciecorrecterrConstruct(pEvent, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum);
    if (status != NV_OK) goto __nvoc_ctor_PcieCorrectableErrorFault_fail__init;
    goto __nvoc_ctor_PcieCorrectableErrorFault_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieCorrectableErrorFault_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieCorrectableErrorFault_fail_GpuInterconnectError:
__nvoc_ctor_PcieCorrectableErrorFault_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieCorrectableErrorFault_1(PcieCorrectableErrorFault *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieCorrectableErrorFault_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieCorrectableErrorFault(PcieCorrectableErrorFault *pThis) {
    __nvoc_init_funcTable_PcieCorrectableErrorFault_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieCorrectableErrorFault(PcieCorrectableErrorFault *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieCorrectableErrorFault = pThis;    // (pciecorrecterr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieCorrectableErrorFault;    // (pciecorrecterr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieCorrectableErrorFault(pThis);
}

NV_STATUS __nvoc_objCreate_PcieCorrectableErrorFault(PcieCorrectableErrorFault **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 ceRateTupleNum, NvU32 ceRateNerdNumber, NvU32 ceRateTupleVerNum)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieCorrectableErrorFault *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieCorrectableErrorFault));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieCorrectableErrorFault));

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
    __nvoc_init__PcieCorrectableErrorFault(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieCorrectableErrorFault(__nvoc_pThis, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieCorrectableErrorFault_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieCorrectableErrorFault_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieCorrectableErrorFault));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieCorrectableErrorFault(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 ceRateTupleNum = va_arg(__nvoc_args, NvU32);
    NvU32 ceRateNerdNumber = va_arg(__nvoc_args, NvU32);
    NvU32 ceRateTupleVerNum = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PcieCorrectableErrorFault((PcieCorrectableErrorFault **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6c0c10 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieResetFailure;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for PcieResetFailure
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__PcieResetFailure(PcieResetFailure*);
void __nvoc_init_funcTable_PcieResetFailure(PcieResetFailure*);
NV_STATUS __nvoc_ctor_PcieResetFailure(PcieResetFailure*, struct EventBus *pEventBus, NvU32 resetStatus, NvU32 resetRecAction);
void __nvoc_init_dataField_PcieResetFailure(PcieResetFailure*);
void __nvoc_dtor_PcieResetFailure(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PcieResetFailure;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieResetFailure;

// Down-thunk(s) to bridge PcieResetFailure methods from ancestors (if any)

// Up-thunk(s) to bridge PcieResetFailure methods to ancestors (if any)

// Class-specific details for PcieResetFailure
const struct NVOC_CLASS_DEF __nvoc_class_def_PcieResetFailure = 
{
    .classInfo.size =               sizeof(PcieResetFailure),
    .classInfo.classId =            classId(PcieResetFailure),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PcieResetFailure",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PcieResetFailure,
    .pCastInfo =          &__nvoc_castinfo__PcieResetFailure,
    .pExportInfo =        &__nvoc_export_info__PcieResetFailure
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PcieResetFailure __nvoc_metadata__PcieResetFailure = {
    .rtti.pClassDef = &__nvoc_class_def_PcieResetFailure,    // (pcieresetfailure) this
    .rtti.dtor      = &__nvoc_dtor_PcieResetFailure,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(PcieResetFailure, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PcieResetFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PcieResetFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PcieResetFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PcieResetFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PcieResetFailure = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PcieResetFailure.rtti,    // [0]: (pcieresetfailure) this
        &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PcieResetFailure = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PcieResetFailure object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_PcieResetFailure(Dynamic* pThis) {

    PcieResetFailure *__nvoc_this = (PcieResetFailure *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PcieResetFailure(PcieResetFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PcieResetFailure object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PcieResetFailure(PcieResetFailure *pEvent, struct EventBus *pEventBus, NvU32 resetStatus, NvU32 resetRecAction) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-PCIE", PCIE_OP_EVENT_CODE_RESET_FAILURE, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_PcieResetFailure_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_PcieResetFailure(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pcieresetfailureConstruct(pEvent, pEventBus, resetStatus, resetRecAction);
    if (status != NV_OK) goto __nvoc_ctor_PcieResetFailure_fail__init;
    goto __nvoc_ctor_PcieResetFailure_exit; // Success

    // Unwind on error.
__nvoc_ctor_PcieResetFailure_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_PcieResetFailure_fail_GpuInterconnectError:
__nvoc_ctor_PcieResetFailure_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PcieResetFailure_1(PcieResetFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PcieResetFailure_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PcieResetFailure(PcieResetFailure *pThis) {
    __nvoc_init_funcTable_PcieResetFailure_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PcieResetFailure(PcieResetFailure *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_PcieResetFailure = pThis;    // (pcieresetfailure) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PcieResetFailure;    // (pcieresetfailure) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PcieResetFailure(pThis);
}

NV_STATUS __nvoc_objCreate_PcieResetFailure(PcieResetFailure **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 resetStatus, NvU32 resetRecAction)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PcieResetFailure *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PcieResetFailure));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PcieResetFailure));

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
    __nvoc_init__PcieResetFailure(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PcieResetFailure(__nvoc_pThis, pEventBus, resetStatus, resetRecAction);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PcieResetFailure_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PcieResetFailure_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PcieResetFailure));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PcieResetFailure(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 resetStatus = va_arg(__nvoc_args, NvU32);
    NvU32 resetRecAction = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PcieResetFailure((PcieResetFailure **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, resetStatus, resetRecAction);
}

