#define NVOC_KERNEL_GSPLITE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_gsplite_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x927969 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsplite;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelGsplite(KernelGsplite*);
void __nvoc_init_funcTable_KernelGsplite(KernelGsplite*);
NV_STATUS __nvoc_ctor_KernelGsplite(KernelGsplite*);
void __nvoc_init_dataField_KernelGsplite(KernelGsplite*);
void __nvoc_dtor_KernelGsplite(KernelGsplite*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsplite;

static const struct NVOC_RTTI __nvoc_rtti_KernelGsplite_KernelGsplite = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGsplite,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGsplite,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsplite_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsplite_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGsplite = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGsplite_KernelGsplite,
        &__nvoc_rtti_KernelGsplite_OBJENGSTATE,
        &__nvoc_rtti_KernelGsplite_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsplite = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGsplite),
        /*classId=*/            classId(KernelGsplite),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGsplite",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGsplite,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGsplite,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGsplite
};

// Down-thunk(s) to bridge KernelGsplite methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite);    // this
void __nvoc_down_thunk_KernelGsplite_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite);    // this

// 3 down-thunk(s) defined to bridge methods in KernelGsplite from superclasses

// kgspliteConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite, ENGDESCRIPTOR engDesc) {
    return kgspliteConstructEngine(pGpu, (struct KernelGsplite *)(((unsigned char *) pKernelGsplite) - NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kgspliteStateInitUnlocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite) {
    return kgspliteStateInitUnlocked(pGpu, (struct KernelGsplite *)(((unsigned char *) pKernelGsplite) - NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelGsplite_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsplite) {
    kgspliteStateDestroy(pGpu, (struct KernelGsplite *)(((unsigned char *) pKernelGsplite) - NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge KernelGsplite methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate);    // this

// 11 up-thunk(s) defined to bridge methods in KernelGsplite to superclasses

// kgspliteInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}

// kgspliteStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStateLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStateUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)), arg3);
}

// kgspliteIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelGsplite, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsplite = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelGsplite(KernelGsplite *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGsplite(KernelGsplite *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelGsplite(KernelGsplite *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsplite_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelGsplite(pThis);
    goto __nvoc_ctor_KernelGsplite_exit; // Success

__nvoc_ctor_KernelGsplite_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsplite_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelGsplite_1(KernelGsplite *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelGsplite_1


// Initialize vtable(s) for 14 virtual method(s).
void __nvoc_init_funcTable_KernelGsplite(KernelGsplite *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelGsplite vtable = {
        .__kgspliteConstructEngine__ = &kgspliteConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelGsplite_engstateConstructEngine,    // virtual
        .__kgspliteStateInitUnlocked__ = &kgspliteStateInitUnlocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_down_thunk_KernelGsplite_engstateStateInitUnlocked,    // virtual
        .__kgspliteStateDestroy__ = &kgspliteStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelGsplite_engstateStateDestroy,    // virtual
        .__kgspliteInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kgspliteStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kgspliteStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kgspliteStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
        .__kgspliteStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kgspliteStateLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &engstateStateLoad_IMPL,    // virtual
        .__kgspliteStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kgspliteStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kgspliteStateUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStateUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &engstateStateUnload_IMPL,    // virtual
        .__kgspliteStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kgspliteIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kgspliteIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kgsplite) this
    __nvoc_init_funcTable_KernelGsplite_1(pThis);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelGsplite(KernelGsplite *pThis) {
    pThis->__nvoc_pbase_KernelGsplite = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelGsplite(pThis);
}

NV_STATUS __nvoc_objCreate_KernelGsplite(KernelGsplite **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGsplite *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGsplite), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGsplite));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGsplite);

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

    __nvoc_init_KernelGsplite(pThis);
    status = __nvoc_ctor_KernelGsplite(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGsplite_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGsplite_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGsplite));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsplite(KernelGsplite **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsplite(ppThis, pParent, createFlags);

    return status;
}

