#define NVOC_NV_DEBUG_DUMP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_nv_debug_dump_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7e80a2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_NvDebugDump(NvDebugDump*);
void __nvoc_init_funcTable_NvDebugDump(NvDebugDump*);
NV_STATUS __nvoc_ctor_NvDebugDump(NvDebugDump*);
void __nvoc_init_dataField_NvDebugDump(NvDebugDump*);
void __nvoc_dtor_NvDebugDump(NvDebugDump*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDebugDump;

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_NvDebugDump = {
    /*pClassDef=*/          &__nvoc_class_def_NvDebugDump,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvDebugDump,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvDebugDump_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvDebugDump = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_NvDebugDump_NvDebugDump,
        &__nvoc_rtti_NvDebugDump_OBJENGSTATE,
        &__nvoc_rtti_NvDebugDump_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvDebugDump),
        /*classId=*/            classId(NvDebugDump),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvDebugDump",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvDebugDump,
    /*pCastInfo=*/          &__nvoc_castinfo_NvDebugDump,
    /*pExportInfo=*/        &__nvoc_export_info_NvDebugDump
};

// Down-thunk(s) to bridge NvDebugDump methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_NvDebugDump_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_NvDebugDump_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd);    // this

// 2 down-thunk(s) defined to bridge methods in NvDebugDump from superclasses

// nvdConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_NvDebugDump_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd, ENGDESCRIPTOR arg3) {
    return nvdConstructEngine(pGpu, (struct NvDebugDump *)(((unsigned char *) pNvd) - NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_NvDebugDump_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pNvd) {
    return nvdStateInitLocked(pGpu, (struct NvDebugDump *)(((unsigned char *) pNvd) - NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge NvDebugDump methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_nvdInitMissing(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitLocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitUnlocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateInitUnlocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePostLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePostUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_nvdStateDestroy(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_nvdIsPresent(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate);    // this

// 12 up-thunk(s) defined to bridge methods in NvDebugDump to superclasses

// nvdInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_nvdInitMissing(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}

// nvdStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitLocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}

// nvdStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitUnlocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}

// nvdStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateInitUnlocked(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}

// nvdStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePostLoad(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePreUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStateUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_nvdStatePostUnload(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)), arg3);
}

// nvdStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_nvdStateDestroy(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}

// nvdIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_nvdIsPresent(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(NvDebugDump, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_NvDebugDump = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_NvDebugDump(NvDebugDump *pThis) {
    __nvoc_nvdDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvDebugDump(NvDebugDump *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_NvDebugDump(NvDebugDump *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_NvDebugDump_fail_OBJENGSTATE;
    __nvoc_init_dataField_NvDebugDump(pThis);
    goto __nvoc_ctor_NvDebugDump_exit; // Success

__nvoc_ctor_NvDebugDump_fail_OBJENGSTATE:
__nvoc_ctor_NvDebugDump_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvDebugDump_1(NvDebugDump *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NvDebugDump_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_NvDebugDump(NvDebugDump *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__NvDebugDump vtable = {
        .__nvdConstructEngine__ = &nvdConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_NvDebugDump_engstateConstructEngine,    // virtual
        .__nvdStateInitLocked__ = &nvdStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_NvDebugDump_engstateStateInitLocked,    // virtual
        .__nvdInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_nvdInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__nvdStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__nvdStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__nvdStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__nvdStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__nvdStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__nvdStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__nvdStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__nvdStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__nvdStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__nvdStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_nvdStateDestroy,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
        .__nvdIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_nvdIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (nvd) this
    __nvoc_init_funcTable_NvDebugDump_1(pThis);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_NvDebugDump(NvDebugDump *pThis) {
    pThis->__nvoc_pbase_NvDebugDump = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_NvDebugDump(pThis);
}

NV_STATUS __nvoc_objCreate_NvDebugDump(NvDebugDump **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvDebugDump *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvDebugDump), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvDebugDump));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvDebugDump);

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

    __nvoc_init_NvDebugDump(pThis);
    status = __nvoc_ctor_NvDebugDump(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_NvDebugDump_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvDebugDump_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvDebugDump));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvDebugDump(NvDebugDump **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_NvDebugDump(ppThis, pParent, createFlags);

    return status;
}

