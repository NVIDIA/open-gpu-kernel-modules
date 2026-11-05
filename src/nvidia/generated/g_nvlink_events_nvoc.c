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


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__2a057a = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkFatalErrorNvl3
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3*);
void __nvoc_init_funcTable_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3*);
NV_STATUS __nvoc_ctor_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3*, struct EventBus *pEventBus, NvU32 linkId, NvU32 debugDataCount, const NvU32 *pDebugData);
void __nvoc_init_dataField_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3*);
void __nvoc_dtor_NvlinkFatalErrorNvl3(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkFatalErrorNvl3;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkFatalErrorNvl3;

// Down-thunk(s) to bridge NvlinkFatalErrorNvl3 methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkFatalErrorNvl3 methods to ancestors (if any)

// Class-specific details for NvlinkFatalErrorNvl3
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3 = 
{
    .classInfo.size =               sizeof(NvlinkFatalErrorNvl3),
    .classInfo.classId =            classId(NvlinkFatalErrorNvl3),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkFatalErrorNvl3",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkFatalErrorNvl3,
    .pCastInfo =          &__nvoc_castinfo__NvlinkFatalErrorNvl3,
    .pExportInfo =        &__nvoc_export_info__NvlinkFatalErrorNvl3
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkFatalErrorNvl3 __nvoc_metadata__NvlinkFatalErrorNvl3 = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkFatalErrorNvl3,    // (nvlinkfatalerrornvl3) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkFatalErrorNvl3,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkFatalErrorNvl3 = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkFatalErrorNvl3.rtti,    // [0]: (nvlinkfatalerrornvl3) this
        &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkFatalErrorNvl3 = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkFatalErrorNvl3 object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkFatalErrorNvl3(Dynamic* pThis) {

    NvlinkFatalErrorNvl3 *__nvoc_this = (NvlinkFatalErrorNvl3 *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkFatalErrorNvl3 object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3 *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 debugDataCount, const NvU32 *pDebugData) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_FATAL_ERROR_NVL3, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkFatalErrorNvl3_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkFatalErrorNvl3(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkfatalerrornvl3Construct(pEvent, pEventBus, linkId, debugDataCount, pDebugData);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkFatalErrorNvl3_fail__init;
    goto __nvoc_ctor_NvlinkFatalErrorNvl3_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkFatalErrorNvl3_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkFatalErrorNvl3_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkFatalErrorNvl3_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkFatalErrorNvl3_1(NvlinkFatalErrorNvl3 *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkFatalErrorNvl3_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3 *pThis) {
    __nvoc_init_funcTable_NvlinkFatalErrorNvl3_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3 *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkFatalErrorNvl3 = pThis;    // (nvlinkfatalerrornvl3) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3;    // (nvlinkfatalerrornvl3) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkFatalErrorNvl3(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3 **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 linkId, NvU32 debugDataCount, const NvU32 *pDebugData)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkFatalErrorNvl3 *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkFatalErrorNvl3));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkFatalErrorNvl3));

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
    __nvoc_init__NvlinkFatalErrorNvl3(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkFatalErrorNvl3(__nvoc_pThis, pEventBus, linkId, debugDataCount, pDebugData);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkFatalErrorNvl3_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkFatalErrorNvl3_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkFatalErrorNvl3));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkFatalErrorNvl3(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 linkId = va_arg(__nvoc_args, NvU32);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);

    return __nvoc_objCreate_NvlinkFatalErrorNvl3((NvlinkFatalErrorNvl3 **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, debugDataCount, pDebugData);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ded290 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3Legacy;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkFatalErrorNvl3Legacy
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy*);
void __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy*);
NV_STATUS __nvoc_ctor_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy*, struct EventBus *pEventBus, NvU32 linkId, NvU32 dword0, NvU32 dword1, NvU32 dword2, NvU32 dword3, NvU32 dword4, NvU32 dword5, NvU32 dword6);
void __nvoc_init_dataField_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy*);
void __nvoc_dtor_NvlinkFatalErrorNvl3Legacy(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkFatalErrorNvl3Legacy;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkFatalErrorNvl3Legacy;

// Down-thunk(s) to bridge NvlinkFatalErrorNvl3Legacy methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkFatalErrorNvl3Legacy methods to ancestors (if any)

// Class-specific details for NvlinkFatalErrorNvl3Legacy
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3Legacy = 
{
    .classInfo.size =               sizeof(NvlinkFatalErrorNvl3Legacy),
    .classInfo.classId =            classId(NvlinkFatalErrorNvl3Legacy),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkFatalErrorNvl3Legacy",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkFatalErrorNvl3Legacy,
    .pCastInfo =          &__nvoc_castinfo__NvlinkFatalErrorNvl3Legacy,
    .pExportInfo =        &__nvoc_export_info__NvlinkFatalErrorNvl3Legacy
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkFatalErrorNvl3Legacy __nvoc_metadata__NvlinkFatalErrorNvl3Legacy = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkFatalErrorNvl3Legacy,    // (nvlinkfatalerrornvl3legacy) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkFatalErrorNvl3Legacy,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3Legacy, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3Legacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3Legacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3Legacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkFatalErrorNvl3Legacy, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkFatalErrorNvl3Legacy = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.rtti,    // [0]: (nvlinkfatalerrornvl3legacy) this
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkFatalErrorNvl3Legacy = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkFatalErrorNvl3Legacy object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkFatalErrorNvl3Legacy(Dynamic* pThis) {

    NvlinkFatalErrorNvl3Legacy *__nvoc_this = (NvlinkFatalErrorNvl3Legacy *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkFatalErrorNvl3Legacy object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 dword0, NvU32 dword1, NvU32 dword2, NvU32 dword3, NvU32 dword4, NvU32 dword5, NvU32 dword6) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_FATAL_ERROR_NVL3, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkFatalErrorNvl3Legacy_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkFatalErrorNvl3Legacy(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkfatalerrornvl3legacyConstruct(pEvent, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkFatalErrorNvl3Legacy_fail__init;
    goto __nvoc_ctor_NvlinkFatalErrorNvl3Legacy_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkFatalErrorNvl3Legacy_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkFatalErrorNvl3Legacy_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkFatalErrorNvl3Legacy_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy_1(NvlinkFatalErrorNvl3Legacy *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy *pThis) {
    __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkFatalErrorNvl3Legacy = pThis;    // (nvlinkfatalerrornvl3legacy) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkFatalErrorNvl3Legacy;    // (nvlinkfatalerrornvl3legacy) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkFatalErrorNvl3Legacy(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 linkId, NvU32 dword0, NvU32 dword1, NvU32 dword2, NvU32 dword3, NvU32 dword4, NvU32 dword5, NvU32 dword6)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkFatalErrorNvl3Legacy *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkFatalErrorNvl3Legacy));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkFatalErrorNvl3Legacy));

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
    __nvoc_init__NvlinkFatalErrorNvl3Legacy(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkFatalErrorNvl3Legacy(__nvoc_pThis, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkFatalErrorNvl3Legacy_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkFatalErrorNvl3Legacy_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkFatalErrorNvl3Legacy));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkFatalErrorNvl3Legacy(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 linkId = va_arg(__nvoc_args, NvU32);
    NvU32 dword0 = va_arg(__nvoc_args, NvU32);
    NvU32 dword1 = va_arg(__nvoc_args, NvU32);
    NvU32 dword2 = va_arg(__nvoc_args, NvU32);
    NvU32 dword3 = va_arg(__nvoc_args, NvU32);
    NvU32 dword4 = va_arg(__nvoc_args, NvU32);
    NvU32 dword5 = va_arg(__nvoc_args, NvU32);
    NvU32 dword6 = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkFatalErrorNvl3Legacy((NvlinkFatalErrorNvl3Legacy **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__cb3e3f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkPrivError;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkPrivError
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkPrivError(NvlinkPrivError*);
void __nvoc_init_funcTable_NvlinkPrivError(NvlinkPrivError*);
NV_STATUS __nvoc_ctor_NvlinkPrivError(NvlinkPrivError*, struct EventBus *pEventBus, NvU32 linkId, NvU32 gpuId);
void __nvoc_init_dataField_NvlinkPrivError(NvlinkPrivError*);
void __nvoc_dtor_NvlinkPrivError(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkPrivError;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkPrivError;

// Down-thunk(s) to bridge NvlinkPrivError methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkPrivError methods to ancestors (if any)

// Class-specific details for NvlinkPrivError
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkPrivError = 
{
    .classInfo.size =               sizeof(NvlinkPrivError),
    .classInfo.classId =            classId(NvlinkPrivError),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkPrivError",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkPrivError,
    .pCastInfo =          &__nvoc_castinfo__NvlinkPrivError,
    .pExportInfo =        &__nvoc_export_info__NvlinkPrivError
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkPrivError __nvoc_metadata__NvlinkPrivError = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkPrivError,    // (nvlinkpriverror) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkPrivError,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkPrivError, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkPrivError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkPrivError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkPrivError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkPrivError, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkPrivError = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkPrivError.rtti,    // [0]: (nvlinkpriverror) this
        &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkPrivError = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkPrivError object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkPrivError(Dynamic* pThis) {

    NvlinkPrivError *__nvoc_this = (NvlinkPrivError *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkPrivError(NvlinkPrivError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkPrivError object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkPrivError(NvlinkPrivError *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 gpuId) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_PRIV_ERROR, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkPrivError_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkPrivError(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkpriverrorConstruct(pEvent, pEventBus, linkId, gpuId);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkPrivError_fail__init;
    goto __nvoc_ctor_NvlinkPrivError_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkPrivError_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkPrivError_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkPrivError_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkPrivError_1(NvlinkPrivError *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkPrivError_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkPrivError(NvlinkPrivError *pThis) {
    __nvoc_init_funcTable_NvlinkPrivError_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkPrivError(NvlinkPrivError *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkPrivError = pThis;    // (nvlinkpriverror) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkPrivError;    // (nvlinkpriverror) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkPrivError(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkPrivError(NvlinkPrivError **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 linkId, NvU32 gpuId)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkPrivError *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkPrivError));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkPrivError));

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
    __nvoc_init__NvlinkPrivError(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkPrivError(__nvoc_pThis, pEventBus, linkId, gpuId);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkPrivError_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkPrivError_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkPrivError));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkPrivError(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 linkId = va_arg(__nvoc_args, NvU32);
    NvU32 gpuId = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_NvlinkPrivError((NvlinkPrivError **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, gpuId);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4cca06 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorGpuFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkSawErrorGpuFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal*);
void __nvoc_init_funcTable_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal*);
NV_STATUS __nvoc_ctor_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal*);
void __nvoc_dtor_NvlinkSawErrorGpuFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorGpuFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorGpuFatal;

// Down-thunk(s) to bridge NvlinkSawErrorGpuFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkSawErrorGpuFatal methods to ancestors (if any)

// Class-specific details for NvlinkSawErrorGpuFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorGpuFatal = 
{
    .classInfo.size =               sizeof(NvlinkSawErrorGpuFatal),
    .classInfo.classId =            classId(NvlinkSawErrorGpuFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkSawErrorGpuFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkSawErrorGpuFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkSawErrorGpuFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkSawErrorGpuFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkSawErrorGpuFatal __nvoc_metadata__NvlinkSawErrorGpuFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkSawErrorGpuFatal,    // (nvlinksawerrorgpufatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkSawErrorGpuFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorGpuFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorGpuFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.rtti,    // [0]: (nvlinksawerrorgpufatal) this
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorGpuFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkSawErrorGpuFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkSawErrorGpuFatal(Dynamic* pThis) {

    NvlinkSawErrorGpuFatal *__nvoc_this = (NvlinkSawErrorGpuFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkSawErrorGpuFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_SAW_ERROR_GPU_FATAL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorGpuFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkSawErrorGpuFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinksawerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorGpuFatal_fail__init;
    goto __nvoc_ctor_NvlinkSawErrorGpuFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkSawErrorGpuFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkSawErrorGpuFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkSawErrorGpuFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkSawErrorGpuFatal_1(NvlinkSawErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkSawErrorGpuFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal *pThis) {
    __nvoc_init_funcTable_NvlinkSawErrorGpuFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkSawErrorGpuFatal = pThis;    // (nvlinksawerrorgpufatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorGpuFatal;    // (nvlinksawerrorgpufatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkSawErrorGpuFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkSawErrorGpuFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkSawErrorGpuFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorGpuFatal));

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
    __nvoc_init__NvlinkSawErrorGpuFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkSawErrorGpuFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkSawErrorGpuFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkSawErrorGpuFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorGpuFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorGpuFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkSawErrorGpuFatal((NvlinkSawErrorGpuFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__499ff1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkSawErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkSawErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkSawErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkSawErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkSawErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkSawErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkSawErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkSawErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkSawErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkSawErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkSawErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkSawErrorNonFatalWarning __nvoc_metadata__NvlinkSawErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkSawErrorNonFatalWarning,    // (nvlinksawerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkSawErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.rtti,    // [0]: (nvlinksawerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkSawErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkSawErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkSawErrorNonFatalWarning *__nvoc_this = (NvlinkSawErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkSawErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_SAW_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkSawErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinksawerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkSawErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkSawErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkSawErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkSawErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning_1(NvlinkSawErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkSawErrorNonFatalWarning = pThis;    // (nvlinksawerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalWarning;    // (nvlinksawerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkSawErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkSawErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkSawErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorNonFatalWarning));

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
    __nvoc_init__NvlinkSawErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkSawErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkSawErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkSawErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkSawErrorNonFatalWarning((NvlinkSawErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ed1b66 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalContained;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkSawErrorNonFatalContained
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained*);
void __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained*);
NV_STATUS __nvoc_ctor_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained*);
void __nvoc_dtor_NvlinkSawErrorNonFatalContained(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorNonFatalContained;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorNonFatalContained;

// Down-thunk(s) to bridge NvlinkSawErrorNonFatalContained methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkSawErrorNonFatalContained methods to ancestors (if any)

// Class-specific details for NvlinkSawErrorNonFatalContained
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalContained = 
{
    .classInfo.size =               sizeof(NvlinkSawErrorNonFatalContained),
    .classInfo.classId =            classId(NvlinkSawErrorNonFatalContained),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkSawErrorNonFatalContained",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkSawErrorNonFatalContained,
    .pCastInfo =          &__nvoc_castinfo__NvlinkSawErrorNonFatalContained,
    .pExportInfo =        &__nvoc_export_info__NvlinkSawErrorNonFatalContained
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkSawErrorNonFatalContained __nvoc_metadata__NvlinkSawErrorNonFatalContained = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkSawErrorNonFatalContained,    // (nvlinksawerrornonfatalcontained) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkSawErrorNonFatalContained,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalContained, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkSawErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkSawErrorNonFatalContained = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.rtti,    // [0]: (nvlinksawerrornonfatalcontained) this
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkSawErrorNonFatalContained = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkSawErrorNonFatalContained object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkSawErrorNonFatalContained(Dynamic* pThis) {

    NvlinkSawErrorNonFatalContained *__nvoc_this = (NvlinkSawErrorNonFatalContained *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkSawErrorNonFatalContained object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_SAW_ERROR_NON_FATAL_CONTAINED, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorNonFatalContained_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkSawErrorNonFatalContained(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinksawerrornonfatalcontainedConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkSawErrorNonFatalContained_fail__init;
    goto __nvoc_ctor_NvlinkSawErrorNonFatalContained_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkSawErrorNonFatalContained_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkSawErrorNonFatalContained_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkSawErrorNonFatalContained_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained_1(NvlinkSawErrorNonFatalContained *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained *pThis) {
    __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkSawErrorNonFatalContained = pThis;    // (nvlinksawerrornonfatalcontained) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkSawErrorNonFatalContained;    // (nvlinksawerrornonfatalcontained) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkSawErrorNonFatalContained(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkSawErrorNonFatalContained *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkSawErrorNonFatalContained));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorNonFatalContained));

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
    __nvoc_init__NvlinkSawErrorNonFatalContained(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkSawErrorNonFatalContained(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkSawErrorNonFatalContained_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkSawErrorNonFatalContained_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkSawErrorNonFatalContained));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorNonFatalContained(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkSawErrorNonFatalContained((NvlinkSawErrorNonFatalContained **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__888655 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorGpuFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkRlwErrorGpuFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal*);
void __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal*);
NV_STATUS __nvoc_ctor_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal*);
void __nvoc_dtor_NvlinkRlwErrorGpuFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorGpuFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorGpuFatal;

// Down-thunk(s) to bridge NvlinkRlwErrorGpuFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkRlwErrorGpuFatal methods to ancestors (if any)

// Class-specific details for NvlinkRlwErrorGpuFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorGpuFatal = 
{
    .classInfo.size =               sizeof(NvlinkRlwErrorGpuFatal),
    .classInfo.classId =            classId(NvlinkRlwErrorGpuFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkRlwErrorGpuFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkRlwErrorGpuFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkRlwErrorGpuFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkRlwErrorGpuFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkRlwErrorGpuFatal __nvoc_metadata__NvlinkRlwErrorGpuFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkRlwErrorGpuFatal,    // (nvlinkrlwerrorgpufatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkRlwErrorGpuFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorGpuFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorGpuFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.rtti,    // [0]: (nvlinkrlwerrorgpufatal) this
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorGpuFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkRlwErrorGpuFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkRlwErrorGpuFatal(Dynamic* pThis) {

    NvlinkRlwErrorGpuFatal *__nvoc_this = (NvlinkRlwErrorGpuFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkRlwErrorGpuFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_RLW_ERROR_GPU_FATAL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorGpuFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkRlwErrorGpuFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkrlwerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorGpuFatal_fail__init;
    goto __nvoc_ctor_NvlinkRlwErrorGpuFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkRlwErrorGpuFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkRlwErrorGpuFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkRlwErrorGpuFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal_1(NvlinkRlwErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal *pThis) {
    __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkRlwErrorGpuFatal = pThis;    // (nvlinkrlwerrorgpufatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorGpuFatal;    // (nvlinkrlwerrorgpufatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkRlwErrorGpuFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkRlwErrorGpuFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkRlwErrorGpuFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorGpuFatal));

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
    __nvoc_init__NvlinkRlwErrorGpuFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkRlwErrorGpuFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkRlwErrorGpuFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkRlwErrorGpuFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorGpuFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorGpuFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkRlwErrorGpuFatal((NvlinkRlwErrorGpuFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__2fbc0d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkRlwErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkRlwErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkRlwErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkRlwErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkRlwErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkRlwErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkRlwErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkRlwErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkRlwErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkRlwErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkRlwErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkRlwErrorNonFatalWarning __nvoc_metadata__NvlinkRlwErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkRlwErrorNonFatalWarning,    // (nvlinkrlwerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkRlwErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.rtti,    // [0]: (nvlinkrlwerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkRlwErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkRlwErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkRlwErrorNonFatalWarning *__nvoc_this = (NvlinkRlwErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkRlwErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_RLW_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkRlwErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkrlwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkRlwErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkRlwErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkRlwErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkRlwErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning_1(NvlinkRlwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkRlwErrorNonFatalWarning = pThis;    // (nvlinkrlwerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorNonFatalWarning;    // (nvlinkrlwerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkRlwErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkRlwErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkRlwErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorNonFatalWarning));

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
    __nvoc_init__NvlinkRlwErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkRlwErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkRlwErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkRlwErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkRlwErrorNonFatalWarning((NvlinkRlwErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0d9cba = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorApplicationFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkRlwErrorApplicationFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal*);
void __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal*);
NV_STATUS __nvoc_ctor_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal*);
void __nvoc_dtor_NvlinkRlwErrorApplicationFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorApplicationFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorApplicationFatal;

// Down-thunk(s) to bridge NvlinkRlwErrorApplicationFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkRlwErrorApplicationFatal methods to ancestors (if any)

// Class-specific details for NvlinkRlwErrorApplicationFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorApplicationFatal = 
{
    .classInfo.size =               sizeof(NvlinkRlwErrorApplicationFatal),
    .classInfo.classId =            classId(NvlinkRlwErrorApplicationFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkRlwErrorApplicationFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkRlwErrorApplicationFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkRlwErrorApplicationFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkRlwErrorApplicationFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkRlwErrorApplicationFatal __nvoc_metadata__NvlinkRlwErrorApplicationFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkRlwErrorApplicationFatal,    // (nvlinkrlwerrorapplicationfatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkRlwErrorApplicationFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorApplicationFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorApplicationFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorApplicationFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorApplicationFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkRlwErrorApplicationFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkRlwErrorApplicationFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.rtti,    // [0]: (nvlinkrlwerrorapplicationfatal) this
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkRlwErrorApplicationFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkRlwErrorApplicationFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkRlwErrorApplicationFatal(Dynamic* pThis) {

    NvlinkRlwErrorApplicationFatal *__nvoc_this = (NvlinkRlwErrorApplicationFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkRlwErrorApplicationFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_RLW_ERROR_APPLICATION_FATAL, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorApplicationFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkRlwErrorApplicationFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinkrlwerrorapplicationfatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkRlwErrorApplicationFatal_fail__init;
    goto __nvoc_ctor_NvlinkRlwErrorApplicationFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkRlwErrorApplicationFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkRlwErrorApplicationFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkRlwErrorApplicationFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal_1(NvlinkRlwErrorApplicationFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal *pThis) {
    __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkRlwErrorApplicationFatal = pThis;    // (nvlinkrlwerrorapplicationfatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkRlwErrorApplicationFatal;    // (nvlinkrlwerrorapplicationfatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkRlwErrorApplicationFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkRlwErrorApplicationFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkRlwErrorApplicationFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorApplicationFatal));

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
    __nvoc_init__NvlinkRlwErrorApplicationFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkRlwErrorApplicationFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkRlwErrorApplicationFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkRlwErrorApplicationFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkRlwErrorApplicationFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorApplicationFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkRlwErrorApplicationFatal((NvlinkRlwErrorApplicationFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5c1052 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorGpuFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkTlwErrorGpuFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal*);
void __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal*);
NV_STATUS __nvoc_ctor_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal*);
void __nvoc_dtor_NvlinkTlwErrorGpuFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorGpuFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorGpuFatal;

// Down-thunk(s) to bridge NvlinkTlwErrorGpuFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkTlwErrorGpuFatal methods to ancestors (if any)

// Class-specific details for NvlinkTlwErrorGpuFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorGpuFatal = 
{
    .classInfo.size =               sizeof(NvlinkTlwErrorGpuFatal),
    .classInfo.classId =            classId(NvlinkTlwErrorGpuFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkTlwErrorGpuFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkTlwErrorGpuFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkTlwErrorGpuFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkTlwErrorGpuFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkTlwErrorGpuFatal __nvoc_metadata__NvlinkTlwErrorGpuFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkTlwErrorGpuFatal,    // (nvlinktlwerrorgpufatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkTlwErrorGpuFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorGpuFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorGpuFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.rtti,    // [0]: (nvlinktlwerrorgpufatal) this
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorGpuFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkTlwErrorGpuFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkTlwErrorGpuFatal(Dynamic* pThis) {

    NvlinkTlwErrorGpuFatal *__nvoc_this = (NvlinkTlwErrorGpuFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkTlwErrorGpuFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_TLW_ERROR_GPU_FATAL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorGpuFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkTlwErrorGpuFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinktlwerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorGpuFatal_fail__init;
    goto __nvoc_ctor_NvlinkTlwErrorGpuFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkTlwErrorGpuFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkTlwErrorGpuFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkTlwErrorGpuFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal_1(NvlinkTlwErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal *pThis) {
    __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkTlwErrorGpuFatal = pThis;    // (nvlinktlwerrorgpufatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorGpuFatal;    // (nvlinktlwerrorgpufatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkTlwErrorGpuFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkTlwErrorGpuFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkTlwErrorGpuFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorGpuFatal));

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
    __nvoc_init__NvlinkTlwErrorGpuFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkTlwErrorGpuFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkTlwErrorGpuFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkTlwErrorGpuFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorGpuFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorGpuFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkTlwErrorGpuFatal((NvlinkTlwErrorGpuFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__da4760 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalContained;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkTlwErrorNonFatalContained
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained*);
void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained*);
NV_STATUS __nvoc_ctor_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained*);
void __nvoc_dtor_NvlinkTlwErrorNonFatalContained(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorNonFatalContained;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorNonFatalContained;

// Down-thunk(s) to bridge NvlinkTlwErrorNonFatalContained methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkTlwErrorNonFatalContained methods to ancestors (if any)

// Class-specific details for NvlinkTlwErrorNonFatalContained
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalContained = 
{
    .classInfo.size =               sizeof(NvlinkTlwErrorNonFatalContained),
    .classInfo.classId =            classId(NvlinkTlwErrorNonFatalContained),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkTlwErrorNonFatalContained",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalContained,
    .pCastInfo =          &__nvoc_castinfo__NvlinkTlwErrorNonFatalContained,
    .pExportInfo =        &__nvoc_export_info__NvlinkTlwErrorNonFatalContained
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkTlwErrorNonFatalContained __nvoc_metadata__NvlinkTlwErrorNonFatalContained = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkTlwErrorNonFatalContained,    // (nvlinktlwerrornonfatalcontained) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkTlwErrorNonFatalContained,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalContained, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalContained, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorNonFatalContained = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.rtti,    // [0]: (nvlinktlwerrornonfatalcontained) this
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorNonFatalContained = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkTlwErrorNonFatalContained object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkTlwErrorNonFatalContained(Dynamic* pThis) {

    NvlinkTlwErrorNonFatalContained *__nvoc_this = (NvlinkTlwErrorNonFatalContained *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkTlwErrorNonFatalContained object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_TLW_ERROR_NON_FATAL_CONTAINED, OPERATIONAL_EVENT_SEVERITY_RECOVERABLE, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorNonFatalContained_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkTlwErrorNonFatalContained(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinktlwerrornonfatalcontainedConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorNonFatalContained_fail__init;
    goto __nvoc_ctor_NvlinkTlwErrorNonFatalContained_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkTlwErrorNonFatalContained_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkTlwErrorNonFatalContained_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkTlwErrorNonFatalContained_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained_1(NvlinkTlwErrorNonFatalContained *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained *pThis) {
    __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkTlwErrorNonFatalContained = pThis;    // (nvlinktlwerrornonfatalcontained) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalContained;    // (nvlinktlwerrornonfatalcontained) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkTlwErrorNonFatalContained(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkTlwErrorNonFatalContained *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkTlwErrorNonFatalContained));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorNonFatalContained));

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
    __nvoc_init__NvlinkTlwErrorNonFatalContained(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkTlwErrorNonFatalContained(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkTlwErrorNonFatalContained_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkTlwErrorNonFatalContained_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorNonFatalContained));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalContained(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkTlwErrorNonFatalContained((NvlinkTlwErrorNonFatalContained **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__80867f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkTlwErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkTlwErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkTlwErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkTlwErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkTlwErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkTlwErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkTlwErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkTlwErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkTlwErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkTlwErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkTlwErrorNonFatalWarning __nvoc_metadata__NvlinkTlwErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkTlwErrorNonFatalWarning,    // (nvlinktlwerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkTlwErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkTlwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTlwErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.rtti,    // [0]: (nvlinktlwerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTlwErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkTlwErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkTlwErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkTlwErrorNonFatalWarning *__nvoc_this = (NvlinkTlwErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkTlwErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_TLW_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkTlwErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinktlwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTlwErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkTlwErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkTlwErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkTlwErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkTlwErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning_1(NvlinkTlwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkTlwErrorNonFatalWarning = pThis;    // (nvlinktlwerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTlwErrorNonFatalWarning;    // (nvlinktlwerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkTlwErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkTlwErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkTlwErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorNonFatalWarning));

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
    __nvoc_init__NvlinkTlwErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkTlwErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkTlwErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkTlwErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTlwErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkTlwErrorNonFatalWarning((NvlinkTlwErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__c7edc3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTrexErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkTrexErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkTrexErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTrexErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTrexErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkTrexErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkTrexErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkTrexErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTrexErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkTrexErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkTrexErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkTrexErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkTrexErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkTrexErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkTrexErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkTrexErrorNonFatalWarning __nvoc_metadata__NvlinkTrexErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkTrexErrorNonFatalWarning,    // (nvlinktrexerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkTrexErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkTrexErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTrexErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkTrexErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkTrexErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkTrexErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkTrexErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.rtti,    // [0]: (nvlinktrexerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkTrexErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkTrexErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkTrexErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkTrexErrorNonFatalWarning *__nvoc_this = (NvlinkTrexErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkTrexErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_TREX_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTrexErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkTrexErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinktrexerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkTrexErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkTrexErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkTrexErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkTrexErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkTrexErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning_1(NvlinkTrexErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkTrexErrorNonFatalWarning = pThis;    // (nvlinktrexerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkTrexErrorNonFatalWarning;    // (nvlinktrexerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkTrexErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkTrexErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkTrexErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTrexErrorNonFatalWarning));

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
    __nvoc_init__NvlinkTrexErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkTrexErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkTrexErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkTrexErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkTrexErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkTrexErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkTrexErrorNonFatalWarning((NvlinkTrexErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__e8f092 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNvlpwErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkNvlpwErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkNvlpwErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNvlpwErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNvlpwErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkNvlpwErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkNvlpwErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkNvlpwErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNvlpwErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkNvlpwErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkNvlpwErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkNvlpwErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkNvlpwErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkNvlpwErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkNvlpwErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkNvlpwErrorNonFatalWarning __nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkNvlpwErrorNonFatalWarning,    // (nvlinknvlpwerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkNvlpwErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkNvlpwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNvlpwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNvlpwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkNvlpwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkNvlpwErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNvlpwErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.rtti,    // [0]: (nvlinknvlpwerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNvlpwErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkNvlpwErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkNvlpwErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkNvlpwErrorNonFatalWarning *__nvoc_this = (NvlinkNvlpwErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkNvlpwErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_NVLPW_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkNvlpwErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinknvlpwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning_1(NvlinkNvlpwErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkNvlpwErrorNonFatalWarning = pThis;    // (nvlinknvlpwerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNvlpwErrorNonFatalWarning;    // (nvlinknvlpwerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkNvlpwErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkNvlpwErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkNvlpwErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNvlpwErrorNonFatalWarning));

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
    __nvoc_init__NvlinkNvlpwErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkNvlpwErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNvlpwErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkNvlpwErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning((NvlinkNvlpwErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__dfc4a7 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorNonFatalWarning;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkNetirErrorNonFatalWarning
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning*);
void __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning*);
NV_STATUS __nvoc_ctor_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning*);
void __nvoc_dtor_NvlinkNetirErrorNonFatalWarning(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorNonFatalWarning;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorNonFatalWarning;

// Down-thunk(s) to bridge NvlinkNetirErrorNonFatalWarning methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkNetirErrorNonFatalWarning methods to ancestors (if any)

// Class-specific details for NvlinkNetirErrorNonFatalWarning
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorNonFatalWarning = 
{
    .classInfo.size =               sizeof(NvlinkNetirErrorNonFatalWarning),
    .classInfo.classId =            classId(NvlinkNetirErrorNonFatalWarning),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkNetirErrorNonFatalWarning",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkNetirErrorNonFatalWarning,
    .pCastInfo =          &__nvoc_castinfo__NvlinkNetirErrorNonFatalWarning,
    .pExportInfo =        &__nvoc_export_info__NvlinkNetirErrorNonFatalWarning
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkNetirErrorNonFatalWarning __nvoc_metadata__NvlinkNetirErrorNonFatalWarning = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkNetirErrorNonFatalWarning,    // (nvlinknetirerrornonfatalwarning) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkNetirErrorNonFatalWarning,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorNonFatalWarning, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorNonFatalWarning, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorNonFatalWarning = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.rtti,    // [0]: (nvlinknetirerrornonfatalwarning) this
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorNonFatalWarning = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkNetirErrorNonFatalWarning object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkNetirErrorNonFatalWarning(Dynamic* pThis) {

    NvlinkNetirErrorNonFatalWarning *__nvoc_this = (NvlinkNetirErrorNonFatalWarning *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkNetirErrorNonFatalWarning object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_NETIR_ERROR_NON_FATAL_WARNING, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorNonFatalWarning_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkNetirErrorNonFatalWarning(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinknetirerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorNonFatalWarning_fail__init;
    goto __nvoc_ctor_NvlinkNetirErrorNonFatalWarning_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkNetirErrorNonFatalWarning_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkNetirErrorNonFatalWarning_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkNetirErrorNonFatalWarning_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning_1(NvlinkNetirErrorNonFatalWarning *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning *pThis) {
    __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkNetirErrorNonFatalWarning = pThis;    // (nvlinknetirerrornonfatalwarning) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorNonFatalWarning;    // (nvlinknetirerrornonfatalwarning) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkNetirErrorNonFatalWarning(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkNetirErrorNonFatalWarning *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkNetirErrorNonFatalWarning));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorNonFatalWarning));

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
    __nvoc_init__NvlinkNetirErrorNonFatalWarning(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkNetirErrorNonFatalWarning(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkNetirErrorNonFatalWarning_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkNetirErrorNonFatalWarning_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorNonFatalWarning));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorNonFatalWarning(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkNetirErrorNonFatalWarning((NvlinkNetirErrorNonFatalWarning **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__aa3699 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorGpuFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkNetirErrorGpuFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal*);
void __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal*);
NV_STATUS __nvoc_ctor_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal*);
void __nvoc_dtor_NvlinkNetirErrorGpuFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorGpuFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorGpuFatal;

// Down-thunk(s) to bridge NvlinkNetirErrorGpuFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkNetirErrorGpuFatal methods to ancestors (if any)

// Class-specific details for NvlinkNetirErrorGpuFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorGpuFatal = 
{
    .classInfo.size =               sizeof(NvlinkNetirErrorGpuFatal),
    .classInfo.classId =            classId(NvlinkNetirErrorGpuFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkNetirErrorGpuFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkNetirErrorGpuFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkNetirErrorGpuFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkNetirErrorGpuFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkNetirErrorGpuFatal __nvoc_metadata__NvlinkNetirErrorGpuFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkNetirErrorGpuFatal,    // (nvlinknetirerrorgpufatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkNetirErrorGpuFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorGpuFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorGpuFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorGpuFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.rtti,    // [0]: (nvlinknetirerrorgpufatal) this
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorGpuFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkNetirErrorGpuFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkNetirErrorGpuFatal(Dynamic* pThis) {

    NvlinkNetirErrorGpuFatal *__nvoc_this = (NvlinkNetirErrorGpuFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkNetirErrorGpuFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_NETIR_ERROR_GPU_FATAL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorGpuFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkNetirErrorGpuFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinknetirerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorGpuFatal_fail__init;
    goto __nvoc_ctor_NvlinkNetirErrorGpuFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkNetirErrorGpuFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkNetirErrorGpuFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkNetirErrorGpuFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal_1(NvlinkNetirErrorGpuFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal *pThis) {
    __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkNetirErrorGpuFatal = pThis;    // (nvlinknetirerrorgpufatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorGpuFatal;    // (nvlinknetirerrorgpufatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkNetirErrorGpuFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkNetirErrorGpuFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkNetirErrorGpuFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorGpuFatal));

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
    __nvoc_init__NvlinkNetirErrorGpuFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkNetirErrorGpuFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkNetirErrorGpuFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkNetirErrorGpuFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorGpuFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorGpuFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkNetirErrorGpuFatal((NvlinkNetirErrorGpuFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__8b8882 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorLinkFatal;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for NvlinkNetirErrorLinkFatal
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal*);
void __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal*);
NV_STATUS __nvoc_ctor_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal*, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
void __nvoc_init_dataField_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal*);
void __nvoc_dtor_NvlinkNetirErrorLinkFatal(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorLinkFatal;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorLinkFatal;

// Down-thunk(s) to bridge NvlinkNetirErrorLinkFatal methods from ancestors (if any)

// Up-thunk(s) to bridge NvlinkNetirErrorLinkFatal methods to ancestors (if any)

// Class-specific details for NvlinkNetirErrorLinkFatal
const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorLinkFatal = 
{
    .classInfo.size =               sizeof(NvlinkNetirErrorLinkFatal),
    .classInfo.classId =            classId(NvlinkNetirErrorLinkFatal),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NvlinkNetirErrorLinkFatal",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_NvlinkNetirErrorLinkFatal,
    .pCastInfo =          &__nvoc_castinfo__NvlinkNetirErrorLinkFatal,
    .pExportInfo =        &__nvoc_export_info__NvlinkNetirErrorLinkFatal
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NvlinkNetirErrorLinkFatal __nvoc_metadata__NvlinkNetirErrorLinkFatal = {
    .rtti.pClassDef = &__nvoc_class_def_NvlinkNetirErrorLinkFatal,    // (nvlinknetirerrorlinkfatal) this
    .rtti.dtor      = &__nvoc_dtor_NvlinkNetirErrorLinkFatal,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorLinkFatal, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorLinkFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorLinkFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorLinkFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(NvlinkNetirErrorLinkFatal, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvlinkNetirErrorLinkFatal = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.rtti,    // [0]: (nvlinknetirerrorlinkfatal) this
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NvlinkNetirErrorLinkFatal = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NvlinkNetirErrorLinkFatal object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_NvlinkNetirErrorLinkFatal(Dynamic* pThis) {

    NvlinkNetirErrorLinkFatal *__nvoc_this = (NvlinkNetirErrorLinkFatal *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NvlinkNetirErrorLinkFatal object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-NVLINK", NVLINK_OP_EVENT_CODE_NETIR_ERROR_LINK_FATAL, OPERATIONAL_EVENT_SEVERITY_FATAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorLinkFatal_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_NvlinkNetirErrorLinkFatal(pEvent);

    // Call the constructor for this class.
    status = __nvoc_nvlinknetirerrorlinkfatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (status != NV_OK) goto __nvoc_ctor_NvlinkNetirErrorLinkFatal_fail__init;
    goto __nvoc_ctor_NvlinkNetirErrorLinkFatal_exit; // Success

    // Unwind on error.
__nvoc_ctor_NvlinkNetirErrorLinkFatal_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_NvlinkNetirErrorLinkFatal_fail_GpuInterconnectError:
__nvoc_ctor_NvlinkNetirErrorLinkFatal_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal_1(NvlinkNetirErrorLinkFatal *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal *pThis) {
    __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_NvlinkNetirErrorLinkFatal = pThis;    // (nvlinknetirerrorlinkfatal) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvlinkNetirErrorLinkFatal;    // (nvlinknetirerrorlinkfatal) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvlinkNetirErrorLinkFatal(pThis);
}

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NvlinkNetirErrorLinkFatal *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NvlinkNetirErrorLinkFatal));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorLinkFatal));

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
    __nvoc_init__NvlinkNetirErrorLinkFatal(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NvlinkNetirErrorLinkFatal(__nvoc_pThis, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NvlinkNetirErrorLinkFatal_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NvlinkNetirErrorLinkFatal_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NvlinkNetirErrorLinkFatal));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorLinkFatal(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvS32 linkId = va_arg(__nvoc_args, NvS32);
    NvU32 errorStatus = va_arg(__nvoc_args, NvU32);
    NvU32 intrInfo = va_arg(__nvoc_args, NvU32);
    const NvU32 *pDebugData = va_arg(__nvoc_args, const NvU32 *);
    NvU32 debugDataCount = va_arg(__nvoc_args, NvU32);
    NvU32 legacyXidCode = va_arg(__nvoc_args, NvU32);
    NvBool bFatal = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bXContain = (NvBool) va_arg(__nvoc_args, NvU32);
    NvBool bInjected = (NvBool) va_arg(__nvoc_args, NvU32);
    const char *pSeverityString = va_arg(__nvoc_args, const char *);
    const char *pLegacyXidErrorName = va_arg(__nvoc_args, const char *);

    return __nvoc_objCreate_NvlinkNetirErrorLinkFatal((NvlinkNetirErrorLinkFatal **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName);
}

