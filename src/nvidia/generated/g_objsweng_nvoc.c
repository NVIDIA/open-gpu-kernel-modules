#define NVOC_OBJSWENG_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_objsweng_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x95a6f5 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for OBJSWENG
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__OBJSWENG(OBJSWENG*);
void __nvoc_init_funcTable_OBJSWENG(OBJSWENG*);
NV_STATUS __nvoc_ctor_OBJSWENG(OBJSWENG*);
void __nvoc_init_dataField_OBJSWENG(OBJSWENG*);
void __nvoc_dtor_OBJSWENG(OBJSWENG*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__OBJSWENG;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJSWENG;

// Down-thunk(s) to bridge OBJSWENG methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_OBJSWENG_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pSweng, ENGDESCRIPTOR arg3);    // this

// Up-thunk(s) to bridge OBJSWENG methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_swengInitMissing(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreInitLocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateInitLocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateInitUnlocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePostLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePostUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_swengStateDestroy(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_swengIsPresent(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJSWENG),
        /*classId=*/            classId(OBJSWENG),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJSWENG",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJSWENG,
    /*pCastInfo=*/          &__nvoc_castinfo__OBJSWENG,
    /*pExportInfo=*/        &__nvoc_export_info__OBJSWENG
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__OBJSWENG __nvoc_metadata__OBJSWENG = {
    .rtti.pClassDef = &__nvoc_class_def_OBJSWENG,    // (sweng) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJSWENG,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__swengConstructEngine__ = &swengConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_OBJSWENG_engstateConstructEngine,    // virtual
    .vtable.__swengInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_swengInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__swengStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__swengStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__swengStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_swengStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__swengStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_swengStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__swengStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__swengStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swengStateLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
    .vtable.__swengStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__swengStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__swengStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swengStateUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
    .vtable.__swengStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_swengStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__swengStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_swengStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
    .vtable.__swengIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_swengIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__OBJSWENG = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__OBJSWENG.rtti,    // [0]: (sweng) this
        &__nvoc_metadata__OBJSWENG.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__OBJSWENG.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in OBJSWENG from superclasses

// swengConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_OBJSWENG_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pSweng, ENGDESCRIPTOR arg3) {
    return swengConstructEngine(pGpu, (struct OBJSWENG *)(((unsigned char *) pSweng) - NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}


// 13 up-thunk(s) defined to bridge methods in OBJSWENG to superclasses

// swengInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_swengInitMissing(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreInitLocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreInitUnlocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateInitLocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateInitUnlocked(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePostLoad(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePreUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStateUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_swengStatePostUnload(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)), arg3);
}

// swengStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_swengStateDestroy(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}

// swengIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_swengIsPresent(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(OBJSWENG, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__OBJSWENG = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJSWENG(OBJSWENG *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJSWENG(OBJSWENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_OBJSWENG(OBJSWENG *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_OBJSWENG_fail_OBJENGSTATE;
    __nvoc_init_dataField_OBJSWENG(pThis);
    goto __nvoc_ctor_OBJSWENG_exit; // Success

__nvoc_ctor_OBJSWENG_fail_OBJENGSTATE:
__nvoc_ctor_OBJSWENG_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_OBJSWENG_1(OBJSWENG *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_OBJSWENG_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_OBJSWENG(OBJSWENG *pThis) {
    __nvoc_init_funcTable_OBJSWENG_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__OBJSWENG(OBJSWENG *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_OBJSWENG = pThis;    // (sweng) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__OBJSWENG.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__OBJSWENG.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__OBJSWENG;    // (sweng) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_OBJSWENG(pThis);
}

NV_STATUS __nvoc_objCreate_OBJSWENG(OBJSWENG **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    OBJSWENG *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(OBJSWENG), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(OBJSWENG));

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

    __nvoc_init__OBJSWENG(pThis);
    status = __nvoc_ctor_OBJSWENG(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_OBJSWENG_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_OBJSWENG_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(OBJSWENG));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJSWENG(OBJSWENG **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJSWENG(ppThis, pParent, createFlags);

    return status;
}

