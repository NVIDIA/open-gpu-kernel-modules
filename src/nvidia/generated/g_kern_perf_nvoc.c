#define NVOC_KERN_PERF_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kern_perf_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xc53a57 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelPerf
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelPerf(KernelPerf*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelPerf(KernelPerf*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelPerf;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPerf;

// Down-thunk(s) to bridge KernelPerf methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags);    // this
void __nvoc_down_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelPerf,
    /*pExportInfo=*/        &__nvoc_export_info__KernelPerf
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelPerf __nvoc_metadata__KernelPerf = {
    .rtti.pClassDef = &__nvoc_class_def_KernelPerf,    // (kperf) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelPerf,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelPerf, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__kperfConstructEngine__ = &kperfConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelPerf_engstateConstructEngine,    // virtual
    .vtable.__kperfStateInitLocked__ = &kperfStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelPerf_engstateStateInitLocked,    // virtual
    .vtable.__kperfStateLoad__ = &kperfStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelPerf_engstateStateLoad,    // virtual
    .vtable.__kperfStateUnload__ = &kperfStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelPerf_engstateStateUnload,    // virtual
    .vtable.__kperfStateDestroy__ = &kperfStateDestroy_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelPerf_engstateStateDestroy,    // virtual
    .vtable.__kperfInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kperfInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__kperfStatePreInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &engstateStatePreInitLocked_IMPL,    // virtual
    .vtable.__kperfStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__kperfStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__kperfStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__kperfStatePostLoad__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePostLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &engstateStatePostLoad_IMPL,    // virtual
    .vtable.__kperfStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__kperfStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kperfStatePostUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &engstateStatePostUnload_IMPL,    // virtual
    .vtable.__kperfIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kperfIsPresent,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelPerf = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelPerf.rtti,    // [0]: (kperf) this
        &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

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


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelPerf = 
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

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelPerf_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelPerf = pThis;    // (kperf) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelPerf;    // (kperf) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelPerf(pThis, pRmhalspecowner);
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

