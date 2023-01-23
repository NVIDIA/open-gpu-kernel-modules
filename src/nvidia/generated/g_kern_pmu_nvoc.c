#define NVOC_KERN_PMU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_pmu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xab9d7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelPmu(KernelPmu*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelPmu(KernelPmu*, RmHalspecOwner* );
void __nvoc_dtor_KernelPmu(KernelPmu*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPmu;

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_KernelPmu = {
    /*pClassDef=*/          &__nvoc_class_def_KernelPmu,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelPmu,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelPmu_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelPmu, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelPmu = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelPmu_KernelPmu,
        &__nvoc_rtti_KernelPmu_OBJENGSTATE,
        &__nvoc_rtti_KernelPmu_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelPmu),
        /*classId=*/            classId(KernelPmu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelPmu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelPmu,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelPmu,
    /*pExportInfo=*/        &__nvoc_export_info_KernelPmu
};

static NV_STATUS __nvoc_thunk_KernelPmu_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu, ENGDESCRIPTOR engDesc) {
    return kpmuConstructEngine(pGpu, (struct KernelPmu *)(((unsigned char *)pKernelPmu) - __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), engDesc);
}

static NV_STATUS __nvoc_thunk_KernelPmu_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelPmu) {
    return kpmuStateInitLocked(pGpu, (struct KernelPmu *)(((unsigned char *)pKernelPmu) - __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuReconcileTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStateLoad(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStateUnload(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePreLoad(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePostUnload(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kpmuStateDestroy(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePreUnload(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStateInitUnlocked(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kpmuInitMissing(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePreInitLocked(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuGetTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuCompareTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kpmuFreeTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuStatePostLoad(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuAllocTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kpmuSetTunableState(POBJGPU pGpu, struct KernelPmu *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kpmuIsPresent(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelPmu_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelPmu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelPmu(KernelPmu *pThis) {
    __nvoc_kpmuDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelPmu_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelPmu(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelPmu_exit; // Success

__nvoc_ctor_KernelPmu_fail_OBJENGSTATE:
__nvoc_ctor_KernelPmu_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelPmu_1(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__kpmuConstructEngine__ = &kpmuConstructEngine_IMPL;

    pThis->__kpmuStateInitLocked__ = &kpmuStateInitLocked_IMPL;

    // Hal function -- kpmuPreOsGlobalErotGrantRequest
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kpmuPreOsGlobalErotGrantRequest__ = &kpmuPreOsGlobalErotGrantRequest_AD102;
    }
    // default
    else
    {
        pThis->__kpmuPreOsGlobalErotGrantRequest__ = &kpmuPreOsGlobalErotGrantRequest_56cd7a;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelPmu_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelPmu_engstateStateInitLocked;

    pThis->__kpmuReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuReconcileTunableState;

    pThis->__kpmuStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kpmuStateLoad;

    pThis->__kpmuStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kpmuStateUnload;

    pThis->__kpmuStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePreLoad;

    pThis->__kpmuStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePostUnload;

    pThis->__kpmuStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kpmuStateDestroy;

    pThis->__kpmuStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePreUnload;

    pThis->__kpmuStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kpmuStateInitUnlocked;

    pThis->__kpmuInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kpmuInitMissing;

    pThis->__kpmuStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePreInitLocked;

    pThis->__kpmuStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePreInitUnlocked;

    pThis->__kpmuGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuGetTunableState;

    pThis->__kpmuCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuCompareTunableState;

    pThis->__kpmuFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuFreeTunableState;

    pThis->__kpmuStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kpmuStatePostLoad;

    pThis->__kpmuAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuAllocTunableState;

    pThis->__kpmuSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kpmuSetTunableState;

    pThis->__kpmuIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kpmuIsPresent;
}

void __nvoc_init_funcTable_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelPmu_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelPmu(KernelPmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelPmu = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelPmu(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelPmu(KernelPmu **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelPmu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelPmu));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelPmu));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelPmu);

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

    __nvoc_init_KernelPmu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelPmu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelPmu_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelPmu_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelPmu(KernelPmu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelPmu(ppThis, pParent, createFlags);

    return status;
}

