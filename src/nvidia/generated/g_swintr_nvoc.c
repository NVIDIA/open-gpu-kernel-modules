#define NVOC_SWINTR_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_swintr_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__5ca633 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

// Forward declarations for SwIntr
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__IntrService(IntrService*);
void __nvoc_init__SwIntr(SwIntr*);
void __nvoc_init_funcTable_SwIntr(SwIntr*);
NV_STATUS __nvoc_ctor_SwIntr(SwIntr*);
void __nvoc_init_dataField_SwIntr(SwIntr*);
void __nvoc_dtor_SwIntr(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SwIntr;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SwIntr;

// Down-thunk(s) to bridge SwIntr methods from ancestors (if any)
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[320]);    // this
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams);    // this

// Up-thunk(s) to bridge SwIntr methods to ancestors (if any)

// Class-specific details for SwIntr
const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr = 
{
    .classInfo.size =               sizeof(SwIntr),
    .classInfo.classId =            classId(SwIntr),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "SwIntr",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_SwIntr,
    .pCastInfo =          &__nvoc_castinfo__SwIntr,
    .pExportInfo =        &__nvoc_export_info__SwIntr
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__SwIntr __nvoc_metadata__SwIntr = {
    .rtti.pClassDef = &__nvoc_class_def_SwIntr,    // (swintr) this
    .rtti.dtor      = &__nvoc_dtor_SwIntr,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
    .metadata__IntrService.rtti.pClassDef = &__nvoc_class_def_IntrService,    // (intrserv) super
    .metadata__IntrService.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__IntrService.rtti.offset    = NV_OFFSETOF(SwIntr, __nvoc_base_IntrService),

    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_SwIntr_intrservRegisterIntrService,    // virtual
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_SwIntr_intrservServiceInterrupt,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateConstructApertures__ = &engstateConstructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateDestructApertures__ = &engstateDestructApertures_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .metadata__IntrService.vtable.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__SwIntr = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__SwIntr.rtti,    // [0]: (swintr) this
        &__nvoc_metadata__SwIntr.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__SwIntr.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
        &__nvoc_metadata__SwIntr.metadata__IntrService.rtti,    // [3]: (intrserv) super
    }
};

// 2 down-thunk(s) defined to bridge methods in SwIntr from superclasses

// swintrRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[320]) {
    swintrRegisterIntrService_IMPL(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pRecords);
}

// swintrServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return swintrServiceInterrupt_IMPL(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__SwIntr = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct SwIntr object.
void __nvoc_dtor_OBJENGSTATE(Dynamic*);
void __nvoc_dtor_IntrService(Dynamic*);
void __nvoc_dtor_SwIntr(Dynamic* pThis) {

    SwIntr *__nvoc_this = (SwIntr *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_OBJENGSTATE((Dynamic *) &__nvoc_this->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService((Dynamic *) &__nvoc_this->__nvoc_base_IntrService);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_SwIntr(SwIntr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct SwIntr object.
NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE *);
NV_STATUS __nvoc_ctor_IntrService(IntrService *);
NV_STATUS __nvoc_ctor_SwIntr(SwIntr *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_IntrService;

    // Initialize data fields.
    __nvoc_init_dataField_SwIntr(pThis);
    goto __nvoc_ctor_SwIntr_exit; // Success

    // Unwind on error.
__nvoc_ctor_SwIntr_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE((Dynamic *)&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_SwIntr_fail_OBJENGSTATE:
__nvoc_ctor_SwIntr_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SwIntr_1(SwIntr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SwIntr_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_SwIntr(SwIntr *pThis) {
    __nvoc_init_funcTable_SwIntr_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__SwIntr(SwIntr *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;    // (intrserv) super
    pThis->__nvoc_pbase_SwIntr = pThis;    // (swintr) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init__IntrService(&pThis->__nvoc_base_IntrService);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__SwIntr.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__SwIntr.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_metadata_ptr = &__nvoc_metadata__SwIntr.metadata__IntrService;    // (intrserv) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__SwIntr;    // (swintr) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_SwIntr(pThis);
}

NV_STATUS __nvoc_objCreate_SwIntr(SwIntr **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    SwIntr *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(SwIntr));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(SwIntr));

    __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__SwIntr(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_SwIntr(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_SwIntr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_SwIntr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(SwIntr));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_SwIntr(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_SwIntr((SwIntr **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}

