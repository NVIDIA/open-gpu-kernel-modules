#define NVOC_KERN_PERF_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_perf_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xc53a57 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelPerf(KernelPerf*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelPerf(KernelPerf*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelPerf(KernelPerf*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelPerf(KernelPerf*, RmHalspecOwner* );
void __nvoc_dtor_KernelPerf(KernelPerf*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPerf;

static const struct NVOC_RTTI __nvoc_rtti_KernelPerf_KernelPerf = {
    /*pClassDef=*/          &__nvoc_class_def_KernelPerf,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelPerf,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPerf_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPerf_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelPerf = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelPerf_KernelPerf,
        &__nvoc_rtti_KernelPerf_OBJENGSTATE,
        &__nvoc_rtti_KernelPerf_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelPerf),
        /*classId=*/            classId(KernelPerf),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelPerf",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelPerf,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelPerf,
    /*pExportInfo=*/        &__nvoc_export_info_KernelPerf
};

// Down-thunk(s) to bridge KernelPerf methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
void __nvoc_down_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this

// 5 down-thunk(s) defined to bridge methods in KernelPerf from superclasses

// kperfConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc) {
    return kperfConstructEngine(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), engDesc);
}

// kperfStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    return kperfStateInitLocked(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateLoad(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), flags);
}

// kperfStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateUnload(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), flags);
}

// kperfStateDestroy: virtual override (engstate) base (engstate)
void __nvoc_down_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    kperfStateDestroy(pGpu, (struct KernelPerf *)(((unsigned char *) pKernelPerf) - NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge KernelPerf methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kperfInitMissing(struct OBJGPU *pGpu, struct KernelPerf *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreLoad(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePostLoad(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreUnload(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePostUnload(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kperfIsPresent(struct OBJGPU *pGpu, struct KernelPerf *pEngstate);    // this

// 9 up-thunk(s) defined to bridge methods in KernelPerf to superclasses

// kperfInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kperfInitMissing(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStatePreInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitLocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStateInitUnlocked(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}

// kperfStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreLoad(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), arg3);
}

// kperfStatePostLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePostLoad(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), arg3);
}

// kperfStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePreUnload(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), arg3);
}

// kperfStatePostUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kperfStatePostUnload(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)), arg3);
}

// kperfIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kperfIsPresent(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPerf = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelPerf(KernelPerf *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelPerf_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelPerf(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelPerf_exit; // Success

__nvoc_ctor_KernelPerf_fail_OBJENGSTATE:
__nvoc_ctor_KernelPerf_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelPerf_1(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // kperfGpuBoostSyncStateInit -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kperfGpuBoostSyncStateInit__ = &kperfGpuBoostSyncStateInit_56cd7a;
    }
    // default
    else
    {
        pThis->__kperfGpuBoostSyncStateInit__ = &kperfGpuBoostSyncStateInit_IMPL;
    }
} // End __nvoc_init_funcTable_KernelPerf_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 15 virtual method(s).
void __nvoc_init_funcTable_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelPerf vtable = {
        .__kperfConstructEngine__ = &kperfConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelPerf_engstateConstructEngine,    // virtual
        .__kperfStateInitLocked__ = &kperfStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelPerf_engstateStateInitLocked,    // virtual
        .__kperfStateLoad__ = &kperfStateLoad_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelPerf_engstateStateLoad,    // virtual
        .__kperfStateUnload__ = &kperfStateUnload_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelPerf_engstateStateUnload,    // virtual
        .__kperfStateDestroy__ = &kperfStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelPerf_engstateStateDestroy,    // virtual
        .__kperfInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kperfInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kperfStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
        .__kperfStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kperfStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kperfStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
        .__kperfStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePostLoad,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
        .__kperfStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
        .__kperfStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePostUnload,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
        .__kperfIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kperfIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kperf) this

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelPerf_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelPerf = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelPerf(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelPerf(KernelPerf **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelPerf *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelPerf), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelPerf));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelPerf);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_KernelPerf(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelPerf(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelPerf_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelPerf_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelPerf));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPerf(KernelPerf **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelPerf(ppThis, pParent, createFlags);

    return status;
}

