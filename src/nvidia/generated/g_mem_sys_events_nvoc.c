#define NVOC_MEM_SYS_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_mem_sys_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a6fed2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemSysTimeout;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

// Forward declarations for MemSysTimeout
void __nvoc_init__GpuMemoryIntegrityError(GpuMemoryIntegrityError*);
void __nvoc_init__MemSysTimeout(MemSysTimeout*);
void __nvoc_init_funcTable_MemSysTimeout(MemSysTimeout*);
NV_STATUS __nvoc_ctor_MemSysTimeout(MemSysTimeout*, struct EventBus *pEventBus, NvU32 severity, NvU32 maintOpError);
void __nvoc_init_dataField_MemSysTimeout(MemSysTimeout*);
void __nvoc_dtor_MemSysTimeout(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__MemSysTimeout;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__MemSysTimeout;

// Down-thunk(s) to bridge MemSysTimeout methods from ancestors (if any)

// Up-thunk(s) to bridge MemSysTimeout methods to ancestors (if any)

// Class-specific details for MemSysTimeout
const struct NVOC_CLASS_DEF __nvoc_class_def_MemSysTimeout = 
{
    .classInfo.size =               sizeof(MemSysTimeout),
    .classInfo.classId =            classId(MemSysTimeout),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "MemSysTimeout",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_MemSysTimeout,
    .pCastInfo =          &__nvoc_castinfo__MemSysTimeout,
    .pExportInfo =        &__nvoc_export_info__MemSysTimeout
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__MemSysTimeout __nvoc_metadata__MemSysTimeout = {
    .rtti.pClassDef = &__nvoc_class_def_MemSysTimeout,    // (memsysto) this
    .rtti.dtor      = &__nvoc_dtor_MemSysTimeout,
    .rtti.offset    = 0,
    .metadata__GpuMemoryIntegrityError.rtti.pClassDef = &__nvoc_class_def_GpuMemoryIntegrityError,    // (gpumemintegrityerr) super
    .metadata__GpuMemoryIntegrityError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.rtti.offset    = NV_OFFSETOF(MemSysTimeout, __nvoc_base_GpuMemoryIntegrityError),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(MemSysTimeout, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(MemSysTimeout, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(MemSysTimeout, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(MemSysTimeout, __nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__MemSysTimeout = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__MemSysTimeout.rtti,    // [0]: (memsysto) this
        &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.rtti,    // [1]: (gpumemintegrityerr) super
        &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__MemSysTimeout = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct MemSysTimeout object.
void __nvoc_dtor_GpuMemoryIntegrityError(Dynamic*);
void __nvoc_dtor_MemSysTimeout(Dynamic* pThis) {

    MemSysTimeout *__nvoc_this = (MemSysTimeout *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *) &__nvoc_this->__nvoc_base_GpuMemoryIntegrityError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_MemSysTimeout(MemSysTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct MemSysTimeout object.
NV_STATUS __nvoc_ctor_GpuMemoryIntegrityError(GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_MemSysTimeout(MemSysTimeout *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 maintOpError) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuMemoryIntegrityError(&pEvent->__nvoc_base_GpuMemoryIntegrityError, pEventBus, "GPU-MEMSYS", MEM_SYS_OP_EVENT_CODE_MAINT_OP_ERROR, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_MemSysTimeout_fail_GpuMemoryIntegrityError;

    // Initialize data fields.
    __nvoc_init_dataField_MemSysTimeout(pEvent);

    // Call the constructor for this class.
    status = __nvoc_memsystoConstruct(pEvent, pEventBus, severity, maintOpError);
    if (status != NV_OK) goto __nvoc_ctor_MemSysTimeout_fail__init;
    goto __nvoc_ctor_MemSysTimeout_exit; // Success

    // Unwind on error.
__nvoc_ctor_MemSysTimeout_fail__init:
    __nvoc_dtor_GpuMemoryIntegrityError((Dynamic *)&pEvent->__nvoc_base_GpuMemoryIntegrityError);
__nvoc_ctor_MemSysTimeout_fail_GpuMemoryIntegrityError:
__nvoc_ctor_MemSysTimeout_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemSysTimeout_1(MemSysTimeout *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_MemSysTimeout_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_MemSysTimeout(MemSysTimeout *pThis) {
    __nvoc_init_funcTable_MemSysTimeout_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__MemSysTimeout(MemSysTimeout *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuMemoryIntegrityError = &pThis->__nvoc_base_GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_pbase_MemSysTimeout = pThis;    // (memsysto) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuMemoryIntegrityError(&pThis->__nvoc_base_GpuMemoryIntegrityError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout.metadata__GpuMemoryIntegrityError;    // (gpumemintegrityerr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__MemSysTimeout;    // (memsysto) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_MemSysTimeout(pThis);
}

NV_STATUS __nvoc_objCreate_MemSysTimeout(MemSysTimeout **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU32 maintOpError)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    MemSysTimeout *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(MemSysTimeout));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(MemSysTimeout));

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
    __nvoc_init__MemSysTimeout(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_MemSysTimeout(__nvoc_pThis, pEventBus, severity, maintOpError);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_MemSysTimeout_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_MemSysTimeout_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuMemoryIntegrityError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(MemSysTimeout));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_MemSysTimeout(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 maintOpError = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_MemSysTimeout((MemSysTimeout **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, maintOpError);
}

