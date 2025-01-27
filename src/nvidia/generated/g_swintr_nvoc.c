#define NVOC_SWINTR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_swintr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5ca633 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_SwIntr(SwIntr*);
void __nvoc_init_funcTable_SwIntr(SwIntr*);
NV_STATUS __nvoc_ctor_SwIntr(SwIntr*);
void __nvoc_init_dataField_SwIntr(SwIntr*);
void __nvoc_dtor_SwIntr(SwIntr*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_SwIntr;

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_SwIntr = {
    /*pClassDef=*/          &__nvoc_class_def_SwIntr,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_SwIntr,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_SwIntr_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(SwIntr, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_SwIntr = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_SwIntr_SwIntr,
        &__nvoc_rtti_SwIntr_IntrService,
        &__nvoc_rtti_SwIntr_OBJENGSTATE,
        &__nvoc_rtti_SwIntr_Object,
    },
};

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
    /*pCastInfo=*/          &__nvoc_castinfo_SwIntr,
    /*pExportInfo=*/        &__nvoc_export_info_SwIntr
};

// Down-thunk(s) to bridge SwIntr methods from ancestors (if any)
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[177]);    // this
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams);    // this

// 2 down-thunk(s) defined to bridge methods in SwIntr from superclasses

// swintrRegisterIntrService: virtual override (intrserv) base (intrserv)
void __nvoc_down_thunk_SwIntr_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceRecord pRecords[177]) {
    swintrRegisterIntrService(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pRecords);
}

// swintrServiceInterrupt: virtual override (intrserv) base (intrserv)
NvU32 __nvoc_down_thunk_SwIntr_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return swintrServiceInterrupt(pGpu, (struct SwIntr *)(((unsigned char *) pSwIntr) - NV_OFFSETOF(SwIntr, __nvoc_base_IntrService)), pParams);
}


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


const struct NVOC_EXPORT_INFO __nvoc_export_info_SwIntr = 
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

    // Per-class vtable definition
    static const struct NVOC_VTABLE__SwIntr vtable = {
        .__swintrRegisterIntrService__ = &swintrRegisterIntrService_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservRegisterIntrService__ = &__nvoc_down_thunk_SwIntr_intrservRegisterIntrService,    // virtual
        .__swintrServiceInterrupt__ = &swintrServiceInterrupt_IMPL,    // virtual override (intrserv) base (intrserv)
        .IntrService.__intrservServiceInterrupt__ = &__nvoc_down_thunk_SwIntr_intrservServiceInterrupt,    // virtual
        .__swintrConstructEngine__ = &__nvoc_up_thunk_OBJENGSTATE_swintrConstructEngine,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &engstateConstructEngine_IMPL,    // virtual
        .__swintrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_swintrInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__swintrStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__swintrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__swintrStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__swintrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__swintrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__swintrStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__swintrStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__swintrStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__swintrStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__swintrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__swintrStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_swintrStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__swintrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_swintrIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
        .__swintrClearInterrupt__ = &__nvoc_up_thunk_IntrService_swintrClearInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservClearInterrupt__ = &intrservClearInterrupt_IMPL,    // virtual
        .__swintrServiceNotificationInterrupt__ = &__nvoc_up_thunk_IntrService_swintrServiceNotificationInterrupt,    // virtual inherited (intrserv) base (intrserv)
        .IntrService.__intrservServiceNotificationInterrupt__ = &intrservServiceNotificationInterrupt_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_base_IntrService.__nvoc_vtable = &vtable.IntrService;    // (intrserv) super
    pThis->__nvoc_vtable = &vtable;    // (swintr) this
    __nvoc_init_funcTable_SwIntr_1(pThis);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_SwIntr(SwIntr *pThis) {
    pThis->__nvoc_pbase_SwIntr = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
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

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_SwIntr);

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

    __nvoc_init_SwIntr(pThis);
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

