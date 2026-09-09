#define NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_nvlink_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__6e0b4b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailure;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkAliTrainingFailure
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkAliTrainingFailure(NvlinkAliTrainingFailure*);
void __nvoc_init_funcTable_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure*);
NV_STATUS __nvoc_ctor_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure*, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData);
void __nvoc_init_dataField_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure*);
void __nvoc_dtor_NvlinkAliTrainingFailure(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkAliTrainingFailure;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkAliTrainingFailure;

// Down-thunk(s) to bridge NvlinkAliTrainingFailure methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkAliTrainingFailure methods to ancestors (if any)

// Class-specific details for NvlinkAliTrainingFailure
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailure = 
{
    .classInfo.size =               sizeof(NvlinkAliTrainingFailure),
    .classInfo.classId =            classId(NvlinkAliTrainingFailure),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkAliTrainingFailure",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkAliTrainingFailure,
    .pCastInfo =          &__nvoc_castinfo__NvlinkAliTrainingFailure,
    .pExportInfo =        &__nvoc_export_info__NvlinkAliTrainingFailure
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkAliTrainingFailure __nvoc_metadata__NvlinkAliTrainingFailure = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkAliTrainingFailure,    // (nvlinkalitrainfail) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkAliTrainingFailure,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailure, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailure, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkAliTrainingFailure = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkAliTrainingFailure.rtti,    // [0]: (nvlinkalitrainfail) this
        &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkAliTrainingFailure = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkAliTrainingFailure object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkAliTrainingFailure(Dynamic* pThis) {

    NvlinkAliTrainingFailure *__nvoc_this = (NvlinkAliTrainingFailure *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkAliTrainingFailure object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_ALI_TRAINING_FAILURE, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkAliTrainingFailure_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkAliTrainingFailure(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkalitrainfailConstruct(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkAliTrainingFailure_fail__init;
    goto __nvoc_ctor_NvlinkAliTrainingFailure_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkAliTrainingFailure_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkAliTrainingFailure_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkAliTrainingFailure_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkAliTrainingFailure_1(NvlinkAliTrainingFailure *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkAliTrainingFailure_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure *pThis) {
    __nvoc_init_funcTable_NvlinkAliTrainingFailure_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkAliTrainingFailure(NvlinkAliTrainingFailure *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkAliTrainingFailure = pThis;    // (nvlinkalitrainfail) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailure;    // (nvlinkalitrainfail) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkAliTrainingFailure(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkAliTrainingFailure *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkAliTrainingFailure));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkAliTrainingFailure));

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
    __nvoc_init__NvlinkAliTrainingFailure(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkAliTrainingFailure(__nvoc_pThis, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkAliTrainingFailure_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkAliTrainingFailure_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkAliTrainingFailure));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkAliTrainingFailure(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 entryCount = va_arg(__nvoc_args, NvU32);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    const NvU8 *pLinkIds = va_arg(__nvoc_args, const NvU8 *);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);

    return __nvoc_objCreate_NvlinkAliTrainingFailure((NvlinkAliTrainingFailure **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__79d043 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailureLegacy;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkAliTrainingFailureLegacy
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy*);
void __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy*);
NV_STATUS __nvoc_ctor_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy*, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData, NvU32 xidCode);
void __nvoc_init_dataField_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy*);
void __nvoc_dtor_NvlinkAliTrainingFailureLegacy(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkAliTrainingFailureLegacy;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkAliTrainingFailureLegacy;

// Down-thunk(s) to bridge NvlinkAliTrainingFailureLegacy methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkAliTrainingFailureLegacy methods to ancestors (if any)

// Class-specific details for NvlinkAliTrainingFailureLegacy
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailureLegacy = 
{
    .classInfo.size =               sizeof(NvlinkAliTrainingFailureLegacy),
    .classInfo.classId =            classId(NvlinkAliTrainingFailureLegacy),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkAliTrainingFailureLegacy",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkAliTrainingFailureLegacy,
    .pCastInfo =          &__nvoc_castinfo__NvlinkAliTrainingFailureLegacy,
    .pExportInfo =        &__nvoc_export_info__NvlinkAliTrainingFailureLegacy
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkAliTrainingFailureLegacy __nvoc_metadata__NvlinkAliTrainingFailureLegacy = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkAliTrainingFailureLegacy,    // (nvlinkalitrainfaillegacy) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkAliTrainingFailureLegacy,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailureLegacy, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailureLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailureLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailureLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkAliTrainingFailureLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkAliTrainingFailureLegacy = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.rtti,    // [0]: (nvlinkalitrainfaillegacy) this
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkAliTrainingFailureLegacy = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkAliTrainingFailureLegacy object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkAliTrainingFailureLegacy(Dynamic* pThis) {

    NvlinkAliTrainingFailureLegacy *__nvoc_this = (NvlinkAliTrainingFailureLegacy *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkAliTrainingFailureLegacy object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData, NvU32 xidCode) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_ALI_TRAINING_FAILURE, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkAliTrainingFailureLegacy_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkAliTrainingFailureLegacy(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkalitrainfaillegacyConstruct(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkAliTrainingFailureLegacy_fail__init;
    goto __nvoc_ctor_NvlinkAliTrainingFailureLegacy_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkAliTrainingFailureLegacy_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkAliTrainingFailureLegacy_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkAliTrainingFailureLegacy_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy_1(NvlinkAliTrainingFailureLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy *pThis) {
    __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkAliTrainingFailureLegacy = pThis;    // (nvlinkalitrainfaillegacy) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkAliTrainingFailureLegacy;    // (nvlinkalitrainfaillegacy) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkAliTrainingFailureLegacy(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData, NvU32 xidCode)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkAliTrainingFailureLegacy *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkAliTrainingFailureLegacy));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkAliTrainingFailureLegacy));

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
    __nvoc_init__NvlinkAliTrainingFailureLegacy(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkAliTrainingFailureLegacy(__nvoc_pThis, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkAliTrainingFailureLegacy_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkAliTrainingFailureLegacy_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkAliTrainingFailureLegacy));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkAliTrainingFailureLegacy(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 entryCount = va_arg(__nvoc_args, NvU32);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    const NvU8 *pLinkIds = va_arg(__nvoc_args, const NvU8 *);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 xidCode = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkAliTrainingFailureLegacy((NvlinkAliTrainingFailureLegacy **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__328f75 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;

// Forward declarations for NvlinkMseError
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_init__NvlinkMseError(NvlinkMseError*);
void __nvoc_init_funcTable_NvlinkMseError(NvlinkMseError*);
NV_STATUS __nvoc_ctor_NvlinkMseError(NvlinkMseError*, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, NvU32 errorStatus, const NvU32 *pDebugData);
void __nvoc_init_dataField_NvlinkMseError(NvlinkMseError*);
void __nvoc_dtor_NvlinkMseError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkMseError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkMseError;

// Down-thunk(s) to bridge NvlinkMseError methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkMseError methods to ancestors (if any)

// Class-specific details for NvlinkMseError
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseError = 
{
    .classInfo.size =               sizeof(NvlinkMseError),
    .classInfo.classId =            classId(NvlinkMseError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkMseError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkMseError,
    .pCastInfo =          &__nvoc_castinfo__NvlinkMseError,
    .pExportInfo =        &__nvoc_export_info__NvlinkMseError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkMseError __nvoc_metadata__NvlinkMseError = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkMseError,    // (nvlinkmseerror) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkMseError,
    .rtti.offset    = 0,
    .metadata__GpuFirmwareFault.rtti.pClassDef = &__nvoc_class_def_GpuFirmwareFault,    // (gpufwfault) super
    .metadata__GpuFirmwareFault.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.rtti.offset    = NV_OFFSETOF(NvlinkMseError, __nvoc_base_GpuFirmwareFault),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkMseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkMseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkMseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkMseError, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkMseError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkMseError.rtti,    // [0]: (nvlinkmseerror) this
        &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.rtti,    // [1]: (gpufwfault) super
        &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkMseError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkMseError object.
void __nvoc_dtor_GpuFirmwareFault(Dynamic*);
void __nvoc_dtor_NvlinkMseError(Dynamic* pThis) {

    NvlinkMseError *__nvoc_this = (NvlinkMseError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuFirmwareFault((Dynamic *) &__nvoc_this->__nvoc_base_GpuFirmwareFault);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkMseError(NvlinkMseError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkMseError object.
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkMseError(NvlinkMseError *pEvent, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, NvU32 errorStatus, const NvU32 *pDebugData) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuFirmwareFault(&pEvent->__nvoc_base_GpuFirmwareFault, pEventBus, "GPU-NVLINK", eventCode, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkMseError_fail_GpuFirmwareFault;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkMseError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkmseerrorConstruct(pEvent, pEventBus, eventCode, severity, errorStatus, pDebugData);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkMseError_fail__init;
    goto __nvoc_ctor_NvlinkMseError_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkMseError_fail__init:
    __nvoc_dtor_GpuFirmwareFault((Dynamic *)&pEvent->__nvoc_base_GpuFirmwareFault);
__nvoc_ctor_NvlinkMseError_fail_GpuFirmwareFault:
__nvoc_ctor_NvlinkMseError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkMseError_1(NvlinkMseError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkMseError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkMseError(NvlinkMseError *pThis) {
    __nvoc_init_funcTable_NvlinkMseError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkMseError(NvlinkMseError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuFirmwareFault = &pThis->__nvoc_base_GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_pbase_NvlinkMseError = pThis;    // (nvlinkmseerror) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuFirmwareFault(&pThis->__nvoc_base_GpuFirmwareFault);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError.metadata__GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseError;    // (nvlinkmseerror) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkMseError(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkMseError(NvlinkMseError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, NvU32 errorStatus, const NvU32 *pDebugData)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkMseError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkMseError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkMseError));

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
    __nvoc_init__NvlinkMseError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkMseError(__nvoc_pThis, pEventBus, eventCode, severity, errorStatus, pDebugData);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkMseError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkMseError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkMseError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkMseError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);

    return __nvoc_objCreate_NvlinkMseError((NvlinkMseError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, severity, errorStatus, pDebugData);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5edd3f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseErrorLegacy;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;

// Forward declarations for NvlinkMseErrorLegacy
void __nvoc_init__GpuFirmwareFault(GpuFirmwareFault*);
void __nvoc_init__NvlinkMseErrorLegacy(NvlinkMseErrorLegacy*);
void __nvoc_init_funcTable_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy*);
NV_STATUS __nvoc_ctor_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy*, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, const NvU32 *pDebugData, NvU32 xidCode, NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName, NvBool bFatal, NvBool bXContain, NvBool bInjected, NvS32 linkId, NvU32 intrInfo, NvU32 legacyErrorStatus);
void __nvoc_init_dataField_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy*);
void __nvoc_dtor_NvlinkMseErrorLegacy(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkMseErrorLegacy;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkMseErrorLegacy;

// Down-thunk(s) to bridge NvlinkMseErrorLegacy methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkMseErrorLegacy methods to ancestors (if any)

// Class-specific details for NvlinkMseErrorLegacy
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseErrorLegacy = 
{
    .classInfo.size =               sizeof(NvlinkMseErrorLegacy),
    .classInfo.classId =            classId(NvlinkMseErrorLegacy),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkMseErrorLegacy",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkMseErrorLegacy,
    .pCastInfo =          &__nvoc_castinfo__NvlinkMseErrorLegacy,
    .pExportInfo =        &__nvoc_export_info__NvlinkMseErrorLegacy
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkMseErrorLegacy __nvoc_metadata__NvlinkMseErrorLegacy = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkMseErrorLegacy,    // (nvlinkmseerrorlegacy) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkMseErrorLegacy,
    .rtti.offset    = 0,
    .metadata__GpuFirmwareFault.rtti.pClassDef = &__nvoc_class_def_GpuFirmwareFault,    // (gpufwfault) super
    .metadata__GpuFirmwareFault.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.rtti.offset    = NV_OFFSETOF(NvlinkMseErrorLegacy, __nvoc_base_GpuFirmwareFault),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkMseErrorLegacy, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkMseErrorLegacy, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkMseErrorLegacy, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkMseErrorLegacy, __nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkMseErrorLegacy = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkMseErrorLegacy.rtti,    // [0]: (nvlinkmseerrorlegacy) this
        &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.rtti,    // [1]: (gpufwfault) super
        &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkMseErrorLegacy = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkMseErrorLegacy object.
void __nvoc_dtor_GpuFirmwareFault(Dynamic*);
void __nvoc_dtor_NvlinkMseErrorLegacy(Dynamic* pThis) {

    NvlinkMseErrorLegacy *__nvoc_this = (NvlinkMseErrorLegacy *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuFirmwareFault((Dynamic *) &__nvoc_this->__nvoc_base_GpuFirmwareFault);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkMseErrorLegacy object.
NV_STATUS __nvoc_ctor_GpuFirmwareFault(GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy *pEvent, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, const NvU32 *pDebugData, NvU32 xidCode, NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName, NvBool bFatal, NvBool bXContain, NvBool bInjected, NvS32 linkId, NvU32 intrInfo, NvU32 legacyErrorStatus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuFirmwareFault(&pEvent->__nvoc_base_GpuFirmwareFault, pEventBus, "GPU-NVLINK", eventCode, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkMseErrorLegacy_fail_GpuFirmwareFault;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkMseErrorLegacy(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkmseerrorlegacyConstruct(pEvent, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkMseErrorLegacy_fail__init;
    goto __nvoc_ctor_NvlinkMseErrorLegacy_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkMseErrorLegacy_fail__init:
    __nvoc_dtor_GpuFirmwareFault((Dynamic *)&pEvent->__nvoc_base_GpuFirmwareFault);
__nvoc_ctor_NvlinkMseErrorLegacy_fail_GpuFirmwareFault:
__nvoc_ctor_NvlinkMseErrorLegacy_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkMseErrorLegacy_1(NvlinkMseErrorLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkMseErrorLegacy_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy *pThis) {
    __nvoc_init_funcTable_NvlinkMseErrorLegacy_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkMseErrorLegacy(NvlinkMseErrorLegacy *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuFirmwareFault = &pThis->__nvoc_base_GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_pbase_NvlinkMseErrorLegacy = pThis;    // (nvlinkmseerrorlegacy) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuFirmwareFault(&pThis->__nvoc_base_GpuFirmwareFault);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuFirmwareFault.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy.metadata__GpuFirmwareFault;    // (gpufwfault) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkMseErrorLegacy;    // (nvlinkmseerrorlegacy) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkMseErrorLegacy(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, const NvU32 *pDebugData, NvU32 xidCode, NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName, NvBool bFatal, NvBool bXContain, NvBool bInjected, NvS32 linkId, NvU32 intrInfo, NvU32 legacyErrorStatus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkMseErrorLegacy *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkMseErrorLegacy));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkMseErrorLegacy));

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
    __nvoc_init__NvlinkMseErrorLegacy(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkMseErrorLegacy(__nvoc_pThis, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkMseErrorLegacy_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkMseErrorLegacy_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuFirmwareFault.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkMseErrorLegacy));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkMseErrorLegacy(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 xidCode = va_arg(__nvoc_args, NvU32);
    NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName = va_arg(__nvoc_args, NVLINK_MSE_LEGACY_XID_ERROR_NAME);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    NvU32 legacyErrorStatus = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkMseErrorLegacy((NvlinkMseErrorLegacy **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__b7f16f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDown;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkSwLinkDown
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkSwLinkDown(NvlinkSwLinkDown*);
void __nvoc_init_funcTable_NvlinkSwLinkDown(NvlinkSwLinkDown*);
NV_STATUS __nvoc_ctor_NvlinkSwLinkDown(NvlinkSwLinkDown*, struct EventBus *pEventBus, NvU32 linkId, const NvU32 *pDebugData);
void __nvoc_init_dataField_NvlinkSwLinkDown(NvlinkSwLinkDown*);
void __nvoc_dtor_NvlinkSwLinkDown(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSwLinkDown;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSwLinkDown;

// Down-thunk(s) to bridge NvlinkSwLinkDown methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkSwLinkDown methods to ancestors (if any)

// Class-specific details for NvlinkSwLinkDown
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDown = 
{
    .classInfo.size =               sizeof(NvlinkSwLinkDown),
    .classInfo.classId =            classId(NvlinkSwLinkDown),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkSwLinkDown",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkSwLinkDown,
    .pCastInfo =          &__nvoc_castinfo__NvlinkSwLinkDown,
    .pExportInfo =        &__nvoc_export_info__NvlinkSwLinkDown
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkSwLinkDown __nvoc_metadata__NvlinkSwLinkDown = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkSwLinkDown,    // (nvlinkswlinkdown) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkSwLinkDown,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDown, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDown, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDown, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDown, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDown, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSwLinkDown = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkSwLinkDown.rtti,    // [0]: (nvlinkswlinkdown) this
        &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSwLinkDown = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkSwLinkDown object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkSwLinkDown(Dynamic* pThis) {

    NvlinkSwLinkDown *__nvoc_this = (NvlinkSwLinkDown *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkSwLinkDown(NvlinkSwLinkDown *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkSwLinkDown object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkSwLinkDown(NvlinkSwLinkDown *pEvent, struct EventBus *pEventBus, NvU32 linkId, const NvU32 *pDebugData) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_SW_LINK_DOWN, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSwLinkDown_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkSwLinkDown(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkswlinkdownConstruct(pEvent, pEventBus, linkId, pDebugData);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSwLinkDown_fail__init;
    goto __nvoc_ctor_NvlinkSwLinkDown_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkSwLinkDown_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkSwLinkDown_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkSwLinkDown_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkSwLinkDown_1(NvlinkSwLinkDown *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkSwLinkDown_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkSwLinkDown(NvlinkSwLinkDown *pThis) {
    __nvoc_init_funcTable_NvlinkSwLinkDown_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkSwLinkDown(NvlinkSwLinkDown *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkSwLinkDown = pThis;    // (nvlinkswlinkdown) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDown;    // (nvlinkswlinkdown) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkSwLinkDown(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkSwLinkDown(NvlinkSwLinkDown **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 linkId, const NvU32 *pDebugData)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkSwLinkDown *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkSwLinkDown));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSwLinkDown));

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
    __nvoc_init__NvlinkSwLinkDown(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkSwLinkDown(__nvoc_pThis, pEventBus, linkId, pDebugData);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkSwLinkDown_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkSwLinkDown_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSwLinkDown));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkSwLinkDown(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 linkId = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);

    return __nvoc_objCreate_NvlinkSwLinkDown((NvlinkSwLinkDown **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, pDebugData);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__555ef5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDownLegacy;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkSwLinkDownLegacy
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy*);
void __nvoc_init_funcTable_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy*);
NV_STATUS __nvoc_ctor_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy*, struct EventBus *pEventBus, NvU32 linkId, NvU32 xidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected);
void __nvoc_init_dataField_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy*);
void __nvoc_dtor_NvlinkSwLinkDownLegacy(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSwLinkDownLegacy;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSwLinkDownLegacy;

// Down-thunk(s) to bridge NvlinkSwLinkDownLegacy methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkSwLinkDownLegacy methods to ancestors (if any)

// Class-specific details for NvlinkSwLinkDownLegacy
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDownLegacy = 
{
    .classInfo.size =               sizeof(NvlinkSwLinkDownLegacy),
    .classInfo.classId =            classId(NvlinkSwLinkDownLegacy),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkSwLinkDownLegacy",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkSwLinkDownLegacy,
    .pCastInfo =          &__nvoc_castinfo__NvlinkSwLinkDownLegacy,
    .pExportInfo =        &__nvoc_export_info__NvlinkSwLinkDownLegacy
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkSwLinkDownLegacy __nvoc_metadata__NvlinkSwLinkDownLegacy = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkSwLinkDownLegacy,    // (nvlinkswlinkdownlegacy) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkSwLinkDownLegacy,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDownLegacy, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDownLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDownLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDownLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkSwLinkDownLegacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSwLinkDownLegacy = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.rtti,    // [0]: (nvlinkswlinkdownlegacy) this
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSwLinkDownLegacy = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkSwLinkDownLegacy object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkSwLinkDownLegacy(Dynamic* pThis) {

    NvlinkSwLinkDownLegacy *__nvoc_this = (NvlinkSwLinkDownLegacy *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkSwLinkDownLegacy object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 xidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_SW_LINK_DOWN, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSwLinkDownLegacy_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkSwLinkDownLegacy(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkswlinkdownlegacyConstruct(pEvent, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSwLinkDownLegacy_fail__init;
    goto __nvoc_ctor_NvlinkSwLinkDownLegacy_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkSwLinkDownLegacy_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkSwLinkDownLegacy_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkSwLinkDownLegacy_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkSwLinkDownLegacy_1(NvlinkSwLinkDownLegacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkSwLinkDownLegacy_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy *pThis) {
    __nvoc_init_funcTable_NvlinkSwLinkDownLegacy_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkSwLinkDownLegacy = pThis;    // (nvlinkswlinkdownlegacy) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSwLinkDownLegacy;    // (nvlinkswlinkdownlegacy) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkSwLinkDownLegacy(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 linkId, NvU32 xidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkSwLinkDownLegacy *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkSwLinkDownLegacy));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSwLinkDownLegacy));

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
    __nvoc_init__NvlinkSwLinkDownLegacy(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkSwLinkDownLegacy(__nvoc_pThis, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkSwLinkDownLegacy_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkSwLinkDownLegacy_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSwLinkDownLegacy));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkSwLinkDownLegacy(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 linkId = va_arg(__nvoc_args, NvU32);
    NvU32 xidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkSwLinkDownLegacy((NvlinkSwLinkDownLegacy **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected);
}

