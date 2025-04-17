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
char __nvoc_class_id_uniqueness_check__0x5ca633 = 1;
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
void __nvoc_dtor_SwIntr(SwIntr*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__SwIntr;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__SwIntr;

// Down-thunk(s) to bridge SwIntr methods from ancestors (if any)
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[179]);    // this
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams);    // this

// Up-thunk(s) to bridge SwIntr methods to ancestors (if any)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrConstructEngine(struct OBJGPU *pGpu, struct SwIntr *pEngstate, ENGDESCRIPTOR arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_swintrInitMissing(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitLocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitUnlocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateInitLocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateInitUnlocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePostLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePostUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_swintrStateDestroy(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_swintrIsPresent(struct OBJGPU *pGpu, struct SwIntr *pEngstate);    // this
NvBool __nvoc_up_thunk_IntrService_swintrClearInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceClearInterruptArguments *pParams);    // this
NV_STATUS __nvoc_up_thunk_IntrService_swintrServiceNotificationInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(SwIntr),
        /*classId=*/            classId(SwIntr),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "SwIntr",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_SwIntr,
    /*pCastInfo=*/          &__nvoc_castinfo__SwIntr,
    /*pExportInfo=*/        &__nvoc_export_info__SwIntr
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__SwIntr __nvoc_metadata__SwIntr = {
    .rtti.pClassDef = &__nvoc_class_def_SwIntr,    // (swintr) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SwIntr,
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

    .vtable.__swintrRegisterIntrService__ = &swintrRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservRegisterIntrService__ = &__nvoc_down_thunk_SwIntr_intrservRegisterIntrService,    // virtual
    .vtable.__swintrServiceInterrupt__ = &swintrServiceInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservServiceInterrupt__ = &__nvoc_down_thunk_SwIntr_intrservServiceInterrupt,    // virtual
    .vtable.__swintrConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_swintrConstructEngine,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
    .vtable.__swintrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_swintrInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__swintrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__swintrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__swintrStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__swintrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__swintrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__swintrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__swintrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__swintrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__swintrStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__swintrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__swintrStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__swintrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_swintrIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    .vtable.__swintrClearInterrupt__ = &__nvoc_up_thunk_IntrService_swintrClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
    .metadata__IntrService.vtable.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
    .vtable.__swintrServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_swintrServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
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
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[179]) {
    swintrRegisterIntrService(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pRecords);
}

// swintrServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return swintrServiceInterrupt(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pParams);
}


// 16 up-thunk(s) defined to bridge methods in SwIntr to superclasses

// swintrConstructEngine: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrConstructEngine(struct OBJGPU *pGpu, struct SwIntr *pEngstate, ENGDESCRIPTOR arg3) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_swintrInitMissing(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitLocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitUnlocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateInitLocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateInitUnlocked(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePostLoad(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePreUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStateUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swintrStatePostUnload(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)), arg3);
}

// swintrStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_swintrStateDestroy(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_swintrIsPresent(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE)));
}

// swintrClearInterrupt: virtual inherited (intrserv) base (intrserv)
NvBool __nvoc_up_thunk_IntrService_swintrClearInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pParams);
}

// swintrServiceNotificationInterrupt: virtual inherited (intrserv) base (intrserv)
NV_STATUS __nvoc_up_thunk_IntrService_swintrServiceNotificationInterrupt(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *) pIntrService) + NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pParams);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__SwIntr = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_SwIntr(SwIntr *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_SwIntr(SwIntr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_SwIntr(SwIntr *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_SwIntr_fail_IntrService;
    __nvoc_init_dataField_SwIntr(pThis);
    goto __nvoc_ctor_SwIntr_exit; // Success

__nvoc_ctor_SwIntr_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_SwIntr_fail_OBJENGSTATE:
__nvoc_ctor_SwIntr_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_SwIntr_1(SwIntr *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_SwIntr_1


// Initialize vtable(s) for 18 virtual method(s).
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

NV_STATUS __nvoc_objCreate_SwIntr(SwIntr **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    SwIntr *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(SwIntr), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(SwIntr));

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__SwIntr(pThis);
    status = __nvoc_ctor_SwIntr(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_SwIntr_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_SwIntr_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(SwIntr));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_SwIntr(SwIntr **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_SwIntr(ppThis, pParent, createFlags);

    return status;
}

