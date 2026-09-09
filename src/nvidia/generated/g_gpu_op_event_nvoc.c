#define NVOC_GPU_OP_EVENT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_gpu_op_event_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__553cba = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;

// Forward declarations for GpuOperationalEvent
void __nvoc_init__OperationalEvent(OperationalEvent*);
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_init_funcTable_GpuOperationalEvent(GpuOperationalEvent*);
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent*, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
void __nvoc_init_dataField_GpuOperationalEvent(GpuOperationalEvent*);
void __nvoc_dtor_GpuOperationalEvent(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__GpuOperationalEvent;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuOperationalEvent;

// Down-thunk(s) to bridge GpuOperationalEvent methods from ancestors (if any)

// Up-thunk(s) to bridge GpuOperationalEvent methods to ancestors (if any)

// Class-specific details for GpuOperationalEvent
const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent = 
{
    .classInfo.size =               sizeof(GpuOperationalEvent),
    .classInfo.classId =            classId(GpuOperationalEvent),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "GpuOperationalEvent",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_GpuOperationalEvent,
    .pCastInfo =          &__nvoc_castinfo__GpuOperationalEvent,
    .pExportInfo =        &__nvoc_export_info__GpuOperationalEvent
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__GpuOperationalEvent __nvoc_metadata__GpuOperationalEvent = {
    .rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) this
    .rtti.dtor      = &__nvoc_dtor_GpuOperationalEvent,
    .rtti.offset    = 0,
    .metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super
    .metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(GpuOperationalEvent, __nvoc_base_OperationalEvent),
    .metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^2
    .metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(GpuOperationalEvent, __nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(GpuOperationalEvent, __nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__GpuOperationalEvent = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__GpuOperationalEvent.rtti,    // [0]: (gpuopevt) this
        &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [1]: (opevt) super
        &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [2]: (event) super^2
        &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [3]: (obj) super^3
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__GpuOperationalEvent = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct GpuOperationalEvent object.
void __nvoc_dtor_OperationalEvent(Dynamic*);
void __nvoc_dtor_GpuOperationalEvent(Dynamic* pThis) {

    GpuOperationalEvent *__nvoc_this = (GpuOperationalEvent *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_OperationalEvent((Dynamic *) &__nvoc_this->__nvoc_base_OperationalEvent);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_GpuOperationalEvent(GpuOperationalEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct GpuOperationalEvent object.
NV_STATUS __nvoc_ctor_OperationalEvent(OperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_GpuOperationalEvent(GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OperationalEvent(&pEvent->__nvoc_base_OperationalEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuOperationalEvent_fail_OperationalEvent;

    // Initialize data fields.
    __nvoc_init_dataField_GpuOperationalEvent(pEvent);

    // Call the constructor for this class.
    status = __nvoc_gpuopevtConstruct(pEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (status != NV_OK) goto __nvoc_ctor_GpuOperationalEvent_fail__init;
    goto __nvoc_ctor_GpuOperationalEvent_exit; // Success

    // Unwind on error.
__nvoc_ctor_GpuOperationalEvent_fail__init:
    __nvoc_dtor_OperationalEvent((Dynamic *)&pEvent->__nvoc_base_OperationalEvent);
__nvoc_ctor_GpuOperationalEvent_fail_OperationalEvent:
__nvoc_ctor_GpuOperationalEvent_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_GpuOperationalEvent_1(GpuOperationalEvent *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_GpuOperationalEvent_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_GpuOperationalEvent(GpuOperationalEvent *pThis) {
    __nvoc_init_funcTable_GpuOperationalEvent_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__GpuOperationalEvent(GpuOperationalEvent *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^2
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_OperationalEvent;    // (opevt) super
    pThis->__nvoc_pbase_GpuOperationalEvent = pThis;    // (gpuopevt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OperationalEvent(&pThis->__nvoc_base_OperationalEvent);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^2
    pThis->__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__GpuOperationalEvent;    // (gpuopevt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_GpuOperationalEvent(pThis);
}

NV_STATUS __nvoc_objCreate_GpuOperationalEvent(GpuOperationalEvent **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    GpuOperationalEvent *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(GpuOperationalEvent));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(GpuOperationalEvent));

    __nvoc_pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__GpuOperationalEvent(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_GpuOperationalEvent(__nvoc_pThis, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_GpuOperationalEvent_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_GpuOperationalEvent_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(GpuOperationalEvent));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_GpuOperationalEvent(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 category = va_arg(__nvoc_args, NvU32);
    const char *pModuleSignature = va_arg(__nvoc_args, const char *);
    NvU32 eventCode = va_arg(__nvoc_args, NvU32);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 attributes = va_arg(__nvoc_args, NvU32);
    NvU32 scope = va_arg(__nvoc_args, NvU32);
    NvU32 reportingSource = va_arg(__nvoc_args, NvU32);
    NvU32 logLevel = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_GpuOperationalEvent((GpuOperationalEvent **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel);
}

