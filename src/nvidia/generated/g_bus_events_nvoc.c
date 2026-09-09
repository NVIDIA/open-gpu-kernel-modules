#define NVOC_BUS_EVENTS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_bus_events_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0edf96 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_BusC2cContainment;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

// Forward declarations for BusC2cContainment
void __nvoc_init__GpuInterconnectError(GpuInterconnectError*);
void __nvoc_init__BusC2cContainment(BusC2cContainment*);
void __nvoc_init_funcTable_BusC2cContainment(BusC2cContainment*);
NV_STATUS __nvoc_ctor_BusC2cContainment(BusC2cContainment*, struct EventBus *pEventBus, NvU32 severity, NvU32 containmentCode);
void __nvoc_init_dataField_BusC2cContainment(BusC2cContainment*);
void __nvoc_dtor_BusC2cContainment(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__BusC2cContainment;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__BusC2cContainment;

// Down-thunk(s) to bridge BusC2cContainment methods from ancestors (if any)

// Up-thunk(s) to bridge BusC2cContainment methods to ancestors (if any)

// Class-specific details for BusC2cContainment
const struct NVOC_CLASS_DEF __nvoc_class_def_BusC2cContainment = 
{
    .classInfo.size =               sizeof(BusC2cContainment),
    .classInfo.classId =            classId(BusC2cContainment),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_EXPORT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "BusC2cContainment",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_BusC2cContainment,
    .pCastInfo =          &__nvoc_castinfo__BusC2cContainment,
    .pExportInfo =        &__nvoc_export_info__BusC2cContainment
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__BusC2cContainment __nvoc_metadata__BusC2cContainment = {
    .rtti.pClassDef = &__nvoc_class_def_BusC2cContainment,    // (busc2ccontnmt) this
    .rtti.dtor      = &__nvoc_dtor_BusC2cContainment,
    .rtti.offset    = 0,
    .metadata__GpuInterconnectError.rtti.pClassDef = &__nvoc_class_def_GpuInterconnectError,    // (gpuinterconnecterr) super
    .metadata__GpuInterconnectError.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.rtti.offset    = NV_OFFSETOF(BusC2cContainment, __nvoc_base_GpuInterconnectError),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.pClassDef = &__nvoc_class_def_GpuOperationalEvent,    // (gpuopevt) super^2
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti.offset    = NV_OFFSETOF(BusC2cContainment, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.pClassDef = &__nvoc_class_def_OperationalEvent,    // (opevt) super^3
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti.offset    = NV_OFFSETOF(BusC2cContainment, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.pClassDef = &__nvoc_class_def_Event,    // (event) super^4
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti.offset    = NV_OFFSETOF(BusC2cContainment, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event),
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti.offset    = NV_OFFSETOF(BusC2cContainment, __nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__BusC2cContainment = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__BusC2cContainment.rtti,    // [0]: (busc2ccontnmt) this
        &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.rtti,    // [1]: (gpuinterconnecterr) super
        &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.rtti,    // [2]: (gpuopevt) super^2
        &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.rtti,    // [3]: (opevt) super^3
        &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.rtti,    // [4]: (event) super^4
        &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object.rtti,    // [5]: (obj) super^5
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__BusC2cContainment = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct BusC2cContainment object.
void __nvoc_dtor_GpuInterconnectError(Dynamic*);
void __nvoc_dtor_BusC2cContainment(Dynamic* pThis) {

    BusC2cContainment *__nvoc_this = (BusC2cContainment *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_GpuInterconnectError((Dynamic *) &__nvoc_this->__nvoc_base_GpuInterconnectError);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_BusC2cContainment(BusC2cContainment *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct BusC2cContainment object.
NV_STATUS __nvoc_ctor_GpuInterconnectError(GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);    // inline
NV_STATUS __nvoc_ctor_BusC2cContainment(BusC2cContainment *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 containmentCode) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuInterconnectError(&pEvent->__nvoc_base_GpuInterconnectError, pEventBus, "GPU-BUS", BUS_OP_EVENT_CODE_C2C_CONTAINMENT, severity, (((1U) & (4294967295U >> (31 - ((((1 != 0) ? 5 : 5)) % 32) + ((((0 != 0) ? 5 : 5)) % 32)))) << ((((0 != 0) ? 5 : 5)) % 32)), GPU_OPERATIONAL_EVENT_SCOPE_DEVICE, GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT, GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR);
    if (status != NV_OK) goto __nvoc_ctor_BusC2cContainment_fail_GpuInterconnectError;

    // Initialize data fields.
    __nvoc_init_dataField_BusC2cContainment(pEvent);

    // Call the constructor for this class.
    status = __nvoc_busc2ccontnmtConstruct(pEvent, pEventBus, severity, containmentCode);
    if (status != NV_OK) goto __nvoc_ctor_BusC2cContainment_fail__init;
    goto __nvoc_ctor_BusC2cContainment_exit; // Success

    // Unwind on error.
__nvoc_ctor_BusC2cContainment_fail__init:
    __nvoc_dtor_GpuInterconnectError((Dynamic *)&pEvent->__nvoc_base_GpuInterconnectError);
__nvoc_ctor_BusC2cContainment_fail_GpuInterconnectError:
__nvoc_ctor_BusC2cContainment_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_BusC2cContainment_1(BusC2cContainment *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_BusC2cContainment_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_BusC2cContainment(BusC2cContainment *pThis) {
    __nvoc_init_funcTable_BusC2cContainment_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__BusC2cContainment(BusC2cContainment *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_Event = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event;    // (event) super^4
    pThis->__nvoc_pbase_OperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_pbase_GpuOperationalEvent = &pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_pbase_GpuInterconnectError = &pThis->__nvoc_base_GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_pbase_BusC2cContainment = pThis;    // (busc2ccontnmt) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuInterconnectError(&pThis->__nvoc_base_GpuInterconnectError);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent.metadata__Event;    // (event) super^4
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent.metadata__OperationalEvent;    // (opevt) super^3
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError.metadata__GpuOperationalEvent;    // (gpuopevt) super^2
    pThis->__nvoc_base_GpuInterconnectError.__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment.metadata__GpuInterconnectError;    // (gpuinterconnecterr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__BusC2cContainment;    // (busc2ccontnmt) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_BusC2cContainment(pThis);
}

NV_STATUS __nvoc_objCreate_BusC2cContainment(BusC2cContainment **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct EventBus *pEventBus, NvU32 severity, NvU32 containmentCode)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    BusC2cContainment *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(BusC2cContainment));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(BusC2cContainment));

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
    __nvoc_init__BusC2cContainment(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_BusC2cContainment(__nvoc_pThis, pEventBus, severity, containmentCode);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_BusC2cContainment_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_BusC2cContainment_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuInterconnectError.__nvoc_base_GpuOperationalEvent.__nvoc_base_OperationalEvent.__nvoc_base_Event.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(BusC2cContainment));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_BusC2cContainment(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct EventBus *pEventBus = va_arg(__nvoc_args, struct EventBus *);
    NvU32 severity = va_arg(__nvoc_args, NvU32);
    NvU32 containmentCode = va_arg(__nvoc_args, NvU32);

    return __nvoc_objCreate_BusC2cContainment((BusC2cContainment **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, containmentCode);
}

