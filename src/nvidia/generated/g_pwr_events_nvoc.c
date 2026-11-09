#define NVOC_PWR_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_pwr_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f59a1f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcReengaged;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuStateChanged;

// Forward declarations for PshcReengaged
void __nvoc_init__GpuStateChanged(GpuStateChanged*);
void __nvoc_init__PshcReengaged(PshcReengaged*);
void __nvoc_init_funcTable_PshcReengaged(PshcReengaged*);
NV_STATUS __nvoc_ctor_PshcReengaged(PshcReengaged*, struct EventBus *pEventBus);
void __nvoc_init_dataField_PshcReengaged(PshcReengaged*);
void __nvoc_dtor_PshcReengaged(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PshcReengaged;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcReengaged;

// Down-thunk(s) to bridge PshcReengaged methods from ancestors (if any)

// Up-thunk(s) to bridge PshcReengaged methods to ancestors (if any)

// Class-specific details for PshcReengaged
const struct NVOC_CLASS_DEF __nvoc_class_def_PshcReengaged = 
{
    .classInfo.size =               sizeof(PshcReengaged),
    .classInfo.classId =            classId(PshcReengaged),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PshcReengaged",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PshcReengaged,
    .pCastInfo =          &__nvoc_castinfo__PshcReengaged,
    .pExportInfo =        &__nvoc_export_info__PshcReengaged
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PshcReengaged __nvoc_metadata__PshcReengaged = {
    .rtti.pClassDef = &__nvoc_class_def_PshcReengaged,    // (pshcreengaged) this
    .rtti.dtor      = &__nvoc_dtor_PshcReengaged,
    .rtti.offset    = 0,
    .metadata__GpuStateChanged.rtti.pClassDef = &__nvoc_class_def_GpuStateChanged,    // (gpustatechanged) super
    .metadata__GpuStateChanged.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.rtti.offset    = NV_OFFSETOF(PshcReengaged, __nvoc_base_GpuStateChanged),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PshcReengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PshcReengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PshcReengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PshcReengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PshcReengaged = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PshcReengaged.rtti,    // [0]: (pshcreengaged) this
        &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.rtti,    // [1]: (gpustatechanged) super
        &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcReengaged = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PshcReengaged object.
void __nvoc_dtor_GpuStateChanged(Dynamic*);
void __nvoc_dtor_PshcReengaged(Dynamic* pThis) {

    PshcReengaged *__nvoc_this = (PshcReengaged *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuStateChanged((Dynamic *) &__nvoc_this->__nvoc_base_GpuStateChanged);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PshcReengaged(PshcReengaged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PshcReengaged object.
NV_STATUS __nvoc_ctor_GpuStateChanged(GpuStateChanged *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PshcReengaged(PshcReengaged *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuStateChanged(&pEvent->__nvoc_base_GpuStateChanged, pEventBus, "GPU-PSHC", PWR_OP_EVENT_CODE_PSHC_REENGAGED, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_DIAGNOSTIC);
    if (status != NV_OK) goto __nvoc_ctor_PshcReengaged_fail_GpuStateChanged;

    // Initialize data fields.
    __nvoc_init_dataField_PshcReengaged(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pshcreengagedConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_PshcReengaged_fail__init;
    goto __nvoc_ctor_PshcReengaged_exit; // Success

    // Unwind on error.
__nvoc_ctor_PshcReengaged_fail__init:
    __nvoc_dtor_GpuStateChanged((Dynamic *)&pEvent->__nvoc_base_GpuStateChanged);
__nvoc_ctor_PshcReengaged_fail_GpuStateChanged:
__nvoc_ctor_PshcReengaged_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PshcReengaged_1(PshcReengaged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PshcReengaged_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PshcReengaged(PshcReengaged *pThis) {
    __nvoc_init_funcTable_PshcReengaged_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PshcReengaged(PshcReengaged *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuStateChanged = &pThis->__nvoc_base_GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_pbase_PshcReengaged = pThis;    // (pshcreengaged) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuStateChanged(&pThis->__nvoc_base_GpuStateChanged);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuStateChanged.__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged.metadata__GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PshcReengaged;    // (pshcreengaged) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PshcReengaged(pThis);
}

NV_STATUS __nvoc_objCreate_PshcReengaged(PshcReengaged **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PshcReengaged *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PshcReengaged));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PshcReengaged));

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
    __nvoc_init__PshcReengaged(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PshcReengaged(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PshcReengaged_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PshcReengaged_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PshcReengaged));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PshcReengaged(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_PshcReengaged((PshcReengaged **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__fa9cd6 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcDisengaged;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuStateChanged;

// Forward declarations for PshcDisengaged
void __nvoc_init__GpuStateChanged(GpuStateChanged*);
void __nvoc_init__PshcDisengaged(PshcDisengaged*);
void __nvoc_init_funcTable_PshcDisengaged(PshcDisengaged*);
NV_STATUS __nvoc_ctor_PshcDisengaged(PshcDisengaged*, struct EventBus *pEventBus, NvU32 reason, NvU32 slowdownMask);
void __nvoc_init_dataField_PshcDisengaged(PshcDisengaged*);
void __nvoc_dtor_PshcDisengaged(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PshcDisengaged;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcDisengaged;

// Down-thunk(s) to bridge PshcDisengaged methods from ancestors (if any)

// Up-thunk(s) to bridge PshcDisengaged methods to ancestors (if any)

// Class-specific details for PshcDisengaged
const struct NVOC_CLASS_DEF __nvoc_class_def_PshcDisengaged = 
{
    .classInfo.size =               sizeof(PshcDisengaged),
    .classInfo.classId =            classId(PshcDisengaged),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PshcDisengaged",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PshcDisengaged,
    .pCastInfo =          &__nvoc_castinfo__PshcDisengaged,
    .pExportInfo =        &__nvoc_export_info__PshcDisengaged
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PshcDisengaged __nvoc_metadata__PshcDisengaged = {
    .rtti.pClassDef = &__nvoc_class_def_PshcDisengaged,    // (pshcdisengaged) this
    .rtti.dtor      = &__nvoc_dtor_PshcDisengaged,
    .rtti.offset    = 0,
    .metadata__GpuStateChanged.rtti.pClassDef = &__nvoc_class_def_GpuStateChanged,    // (gpustatechanged) super
    .metadata__GpuStateChanged.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.rtti.offset    = NV_OFFSETOF(PshcDisengaged, __nvoc_base_GpuStateChanged),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PshcDisengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PshcDisengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PshcDisengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PshcDisengaged, __nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PshcDisengaged = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PshcDisengaged.rtti,    // [0]: (pshcdisengaged) this
        &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.rtti,    // [1]: (gpustatechanged) super
        &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcDisengaged = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PshcDisengaged object.
void __nvoc_dtor_GpuStateChanged(Dynamic*);
void __nvoc_dtor_PshcDisengaged(Dynamic* pThis) {

    PshcDisengaged *__nvoc_this = (PshcDisengaged *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuStateChanged((Dynamic *) &__nvoc_this->__nvoc_base_GpuStateChanged);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PshcDisengaged(PshcDisengaged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PshcDisengaged object.
NV_STATUS __nvoc_ctor_GpuStateChanged(GpuStateChanged *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PshcDisengaged(PshcDisengaged *pEvent, struct EventBus *pEventBus, NvU32 reason, NvU32 slowdownMask) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuStateChanged(&pEvent->__nvoc_base_GpuStateChanged, pEventBus, "GPU-PSHC", PWR_OP_EVENT_CODE_PSHC_DISENGAGED, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_DIAGNOSTIC);
    if (status != NV_OK) goto __nvoc_ctor_PshcDisengaged_fail_GpuStateChanged;

    // Initialize data fields.
    __nvoc_init_dataField_PshcDisengaged(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pshcdisengagedConstruct(pEvent, pEventBus, reason, slowdownMask);
    if (status != NV_OK) goto __nvoc_ctor_PshcDisengaged_fail__init;
    goto __nvoc_ctor_PshcDisengaged_exit; // Success

    // Unwind on error.
__nvoc_ctor_PshcDisengaged_fail__init:
    __nvoc_dtor_GpuStateChanged((Dynamic *)&pEvent->__nvoc_base_GpuStateChanged);
__nvoc_ctor_PshcDisengaged_fail_GpuStateChanged:
__nvoc_ctor_PshcDisengaged_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PshcDisengaged_1(PshcDisengaged *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PshcDisengaged_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PshcDisengaged(PshcDisengaged *pThis) {
    __nvoc_init_funcTable_PshcDisengaged_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PshcDisengaged(PshcDisengaged *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuStateChanged = &pThis->__nvoc_base_GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_pbase_PshcDisengaged = pThis;    // (pshcdisengaged) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuStateChanged(&pThis->__nvoc_base_GpuStateChanged);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuStateChanged.__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged.metadata__GpuStateChanged;    // (gpustatechanged) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PshcDisengaged;    // (pshcdisengaged) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PshcDisengaged(pThis);
}

NV_STATUS __nvoc_objCreate_PshcDisengaged(PshcDisengaged **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 reason, NvU32 slowdownMask)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PshcDisengaged *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PshcDisengaged));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PshcDisengaged));

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
    __nvoc_init__PshcDisengaged(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PshcDisengaged(__nvoc_pThis, pEventBus, reason, slowdownMask);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PshcDisengaged_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PshcDisengaged_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuStateChanged.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PshcDisengaged));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PshcDisengaged(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 reason = va_arg(__nvoc_args, NvU32);
    NvU32 slowdownMask = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PshcDisengaged((PshcDisengaged **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, reason, slowdownMask);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a3f2ac = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcLowLifetime;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceThreshold;

// Forward declarations for PshcLowLifetime
void __nvoc_init__GpuResourceThreshold(GpuResourceThreshold*);
void __nvoc_init__PshcLowLifetime(PshcLowLifetime*);
void __nvoc_init_funcTable_PshcLowLifetime(PshcLowLifetime*);
NV_STATUS __nvoc_ctor_PshcLowLifetime(PshcLowLifetime*, struct EventBus *pEventBus, NvU32 thresholdPct);
void __nvoc_init_dataField_PshcLowLifetime(PshcLowLifetime*);
void __nvoc_dtor_PshcLowLifetime(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PshcLowLifetime;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcLowLifetime;

// Down-thunk(s) to bridge PshcLowLifetime methods from ancestors (if any)

// Up-thunk(s) to bridge PshcLowLifetime methods to ancestors (if any)

// Class-specific details for PshcLowLifetime
const struct NVOC_CLASS_DEF __nvoc_class_def_PshcLowLifetime = 
{
    .classInfo.size =               sizeof(PshcLowLifetime),
    .classInfo.classId =            classId(PshcLowLifetime),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PshcLowLifetime",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PshcLowLifetime,
    .pCastInfo =          &__nvoc_castinfo__PshcLowLifetime,
    .pExportInfo =        &__nvoc_export_info__PshcLowLifetime
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PshcLowLifetime __nvoc_metadata__PshcLowLifetime = {
    .rtti.pClassDef = &__nvoc_class_def_PshcLowLifetime,    // (pshclowlifetime) this
    .rtti.dtor      = &__nvoc_dtor_PshcLowLifetime,
    .rtti.offset    = 0,
    .metadata__GpuResourceThreshold.rtti.pClassDef = &__nvoc_class_def_GpuResourceThreshold,    // (gpuresthreshold) super
    .metadata__GpuResourceThreshold.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.rtti.offset    = NV_OFFSETOF(PshcLowLifetime, __nvoc_base_GpuResourceThreshold),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PshcLowLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PshcLowLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PshcLowLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PshcLowLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PshcLowLifetime = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PshcLowLifetime.rtti,    // [0]: (pshclowlifetime) this
        &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.rtti,    // [1]: (gpuresthreshold) super
        &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcLowLifetime = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PshcLowLifetime object.
void __nvoc_dtor_GpuResourceThreshold(Dynamic*);
void __nvoc_dtor_PshcLowLifetime(Dynamic* pThis) {

    PshcLowLifetime *__nvoc_this = (PshcLowLifetime *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceThreshold((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceThreshold);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PshcLowLifetime(PshcLowLifetime *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PshcLowLifetime object.
NV_STATUS __nvoc_ctor_GpuResourceThreshold(GpuResourceThreshold *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PshcLowLifetime(PshcLowLifetime *pEvent, struct EventBus *pEventBus, NvU32 thresholdPct) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceThreshold(&pEvent->__nvoc_base_GpuResourceThreshold, pEventBus, "GPU-PSHC", PWR_OP_EVENT_CODE_PSHC_LOW_LIFETIME, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_PshcLowLifetime_fail_GpuResourceThreshold;

    // Initialize data fields.
    __nvoc_init_dataField_PshcLowLifetime(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pshclowlifetimeConstruct(pEvent, pEventBus, thresholdPct);
    if (status != NV_OK) goto __nvoc_ctor_PshcLowLifetime_fail__init;
    goto __nvoc_ctor_PshcLowLifetime_exit; // Success

    // Unwind on error.
__nvoc_ctor_PshcLowLifetime_fail__init:
    __nvoc_dtor_GpuResourceThreshold((Dynamic *)&pEvent->__nvoc_base_GpuResourceThreshold);
__nvoc_ctor_PshcLowLifetime_fail_GpuResourceThreshold:
__nvoc_ctor_PshcLowLifetime_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PshcLowLifetime_1(PshcLowLifetime *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PshcLowLifetime_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PshcLowLifetime(PshcLowLifetime *pThis) {
    __nvoc_init_funcTable_PshcLowLifetime_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PshcLowLifetime(PshcLowLifetime *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceThreshold = &pThis->__nvoc_base_GpuResourceThreshold;    // (gpuresthreshold) super
    pThis->__nvoc_pbase_PshcLowLifetime = pThis;    // (pshclowlifetime) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceThreshold(&pThis->__nvoc_base_GpuResourceThreshold);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime.metadata__GpuResourceThreshold;    // (gpuresthreshold) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PshcLowLifetime;    // (pshclowlifetime) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PshcLowLifetime(pThis);
}

NV_STATUS __nvoc_objCreate_PshcLowLifetime(PshcLowLifetime **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 thresholdPct)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PshcLowLifetime *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PshcLowLifetime));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PshcLowLifetime));

    __nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PshcLowLifetime(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PshcLowLifetime(__nvoc_pThis, pEventBus, thresholdPct);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PshcLowLifetime_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PshcLowLifetime_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PshcLowLifetime));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PshcLowLifetime(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 thresholdPct = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_PshcLowLifetime((PshcLowLifetime **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, thresholdPct);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__da3388 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcZeroLifetime;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceThreshold;

// Forward declarations for PshcZeroLifetime
void __nvoc_init__GpuResourceThreshold(GpuResourceThreshold*);
void __nvoc_init__PshcZeroLifetime(PshcZeroLifetime*);
void __nvoc_init_funcTable_PshcZeroLifetime(PshcZeroLifetime*);
NV_STATUS __nvoc_ctor_PshcZeroLifetime(PshcZeroLifetime*, struct EventBus *pEventBus);
void __nvoc_init_dataField_PshcZeroLifetime(PshcZeroLifetime*);
void __nvoc_dtor_PshcZeroLifetime(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__PshcZeroLifetime;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcZeroLifetime;

// Down-thunk(s) to bridge PshcZeroLifetime methods from ancestors (if any)

// Up-thunk(s) to bridge PshcZeroLifetime methods to ancestors (if any)

// Class-specific details for PshcZeroLifetime
const struct NVOC_CLASS_DEF __nvoc_class_def_PshcZeroLifetime = 
{
    .classInfo.size =               sizeof(PshcZeroLifetime),
    .classInfo.classId =            classId(PshcZeroLifetime),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "PshcZeroLifetime",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_PshcZeroLifetime,
    .pCastInfo =          &__nvoc_castinfo__PshcZeroLifetime,
    .pExportInfo =        &__nvoc_export_info__PshcZeroLifetime
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__PshcZeroLifetime __nvoc_metadata__PshcZeroLifetime = {
    .rtti.pClassDef = &__nvoc_class_def_PshcZeroLifetime,    // (pshczerolifetime) this
    .rtti.dtor      = &__nvoc_dtor_PshcZeroLifetime,
    .rtti.offset    = 0,
    .metadata__GpuResourceThreshold.rtti.pClassDef = &__nvoc_class_def_GpuResourceThreshold,    // (gpuresthreshold) super
    .metadata__GpuResourceThreshold.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.rtti.offset    = NV_OFFSETOF(PshcZeroLifetime, __nvoc_base_GpuResourceThreshold),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(PshcZeroLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(PshcZeroLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(PshcZeroLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(PshcZeroLifetime, __nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__PshcZeroLifetime = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__PshcZeroLifetime.rtti,    // [0]: (pshczerolifetime) this
        &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.rtti,    // [1]: (gpuresthreshold) super
        &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__PshcZeroLifetime = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct PshcZeroLifetime object.
void __nvoc_dtor_GpuResourceThreshold(Dynamic*);
void __nvoc_dtor_PshcZeroLifetime(Dynamic* pThis) {

    PshcZeroLifetime *__nvoc_this = (PshcZeroLifetime *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResourceThreshold((Dynamic *) &__nvoc_this->__nvoc_base_GpuResourceThreshold);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_PshcZeroLifetime(PshcZeroLifetime *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct PshcZeroLifetime object.
NV_STATUS __nvoc_ctor_GpuResourceThreshold(GpuResourceThreshold *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_PshcZeroLifetime(PshcZeroLifetime *pEvent, struct EventBus *pEventBus) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResourceThreshold(&pEvent->__nvoc_base_GpuResourceThreshold, pEventBus, "GPU-PSHC", PWR_OP_EVENT_CODE_PSHC_ZERO_LIFETIME, OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_WARNING);
    if (status != NV_OK) goto __nvoc_ctor_PshcZeroLifetime_fail_GpuResourceThreshold;

    // Initialize data fields.
    __nvoc_init_dataField_PshcZeroLifetime(pEvent);

    // Call the constructor for this class.
    status = __nvoc_pshczerolifetimeConstruct(pEvent, pEventBus);
    if (status != NV_OK) goto __nvoc_ctor_PshcZeroLifetime_fail__init;
    goto __nvoc_ctor_PshcZeroLifetime_exit; // Success

    // Unwind on error.
__nvoc_ctor_PshcZeroLifetime_fail__init:
    __nvoc_dtor_GpuResourceThreshold((Dynamic *)&pEvent->__nvoc_base_GpuResourceThreshold);
__nvoc_ctor_PshcZeroLifetime_fail_GpuResourceThreshold:
__nvoc_ctor_PshcZeroLifetime_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_PshcZeroLifetime_1(PshcZeroLifetime *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_PshcZeroLifetime_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_PshcZeroLifetime(PshcZeroLifetime *pThis) {
    __nvoc_init_funcTable_PshcZeroLifetime_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__PshcZeroLifetime(PshcZeroLifetime *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuResourceThreshold = &pThis->__nvoc_base_GpuResourceThreshold;    // (gpuresthreshold) super
    pThis->__nvoc_pbase_PshcZeroLifetime = pThis;    // (pshczerolifetime) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResourceThreshold(&pThis->__nvoc_base_GpuResourceThreshold);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuResourceThreshold.__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime.metadata__GpuResourceThreshold;    // (gpuresthreshold) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__PshcZeroLifetime;    // (pshczerolifetime) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_PshcZeroLifetime(pThis);
}

NV_STATUS __nvoc_objCreate_PshcZeroLifetime(PshcZeroLifetime **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    PshcZeroLifetime *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(PshcZeroLifetime));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(PshcZeroLifetime));

    __nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__PshcZeroLifetime(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_PshcZeroLifetime(__nvoc_pThis, pEventBus);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_PshcZeroLifetime_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_PshcZeroLifetime_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResourceThreshold.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(PshcZeroLifetime));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_PshcZeroLifetime(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);

    return __nvoc_objCreate_PshcZeroLifetime((PshcZeroLifetime **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus);
}

