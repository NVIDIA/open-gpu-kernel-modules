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

static NV_STATUS __nvoc_thunk_KernelPerf_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, ENGDESCRIPTOR engDesc) {
    return kperfConstructEngine(pGpu, (struct KernelPerf *)(((unsigned char *)pKernelPerf) - __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), engDesc);
}

static NV_STATUS __nvoc_thunk_KernelPerf_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    return kperfStateInitLocked(pGpu, (struct KernelPerf *)(((unsigned char *)pKernelPerf) - __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelPerf_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateLoad(pGpu, (struct KernelPerf *)(((unsigned char *)pKernelPerf) - __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_KernelPerf_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf, NvU32 flags) {
    return kperfStateUnload(pGpu, (struct KernelPerf *)(((unsigned char *)pKernelPerf) - __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelPerf_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPerf) {
    kperfStateDestroy(pGpu, (struct KernelPerf *)(((unsigned char *)pKernelPerf) - __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePreLoad(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePostUnload(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePreUnload(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStateInitUnlocked(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kperfInitMissing(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePreInitLocked(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePreInitUnlocked(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kperfStatePostLoad(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kperfIsPresent(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPerf_OBJENGSTATE.offset));
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

static void __nvoc_init_funcTable_KernelPerf_1(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__kperfConstructEngine__ = &kperfConstructEngine_IMPL;

    pThis->__kperfStateInitLocked__ = &kperfStateInitLocked_IMPL;

    pThis->__kperfStateLoad__ = &kperfStateLoad_IMPL;

    pThis->__kperfStateUnload__ = &kperfStateUnload_IMPL;

    pThis->__kperfStateDestroy__ = &kperfStateDestroy_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelPerf_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelPerf_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelPerf_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelPerf_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelPerf_engstateStateDestroy;

    pThis->__kperfStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePreLoad;

    pThis->__kperfStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePostUnload;

    pThis->__kperfStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePreUnload;

    pThis->__kperfStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kperfStateInitUnlocked;

    pThis->__kperfInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kperfInitMissing;

    pThis->__kperfStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePreInitLocked;

    pThis->__kperfStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePreInitUnlocked;

    pThis->__kperfStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kperfStatePostLoad;

    pThis->__kperfIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kperfIsPresent;
}

void __nvoc_init_funcTable_KernelPerf(KernelPerf *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_KernelPerf(KernelPerf **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelPerf *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelPerf), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelPerf));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelPerf);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
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

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelPerf_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelPerf));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPerf(KernelPerf **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelPerf(ppThis, pParent, createFlags);

    return status;
}

