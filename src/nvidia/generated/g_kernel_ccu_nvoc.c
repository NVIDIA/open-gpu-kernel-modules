#define NVOC_KERNEL_CCU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ccu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x5d5b68 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCcu(KernelCcu*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCcu(KernelCcu*, RmHalspecOwner* );
void __nvoc_dtor_KernelCcu(KernelCcu*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcu;

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_KernelCcu = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCcu,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCcu,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCcu_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCcu, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCcu = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCcu_KernelCcu,
        &__nvoc_rtti_KernelCcu_OBJENGSTATE,
        &__nvoc_rtti_KernelCcu_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCcu),
        /*classId=*/            classId(KernelCcu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCcu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCcu,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCcu,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCcu
};

static NV_STATUS __nvoc_thunk_KernelCcu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelCcu, ENGDESCRIPTOR engDesc) {
    return kccuConstructEngine(pGpu, (struct KernelCcu *)(((unsigned char *)pKernelCcu) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), engDesc);
}

static NV_STATUS __nvoc_thunk_KernelCcu_engstateStateLoad(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 flags) {
    return kccuStateLoad(arg0, (struct KernelCcu *)(((unsigned char *)arg1) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_KernelCcu_engstateStateUnload(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 flags) {
    return kccuStateUnload(arg0, (struct KernelCcu *)(((unsigned char *)arg1) - __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuReconcileTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStateInitLocked(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePreLoad(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePostUnload(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kccuStateDestroy(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePreUnload(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStateInitUnlocked(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kccuInitMissing(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePreInitLocked(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePreInitUnlocked(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuGetTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuCompareTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kccuFreeTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuStatePostLoad(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuAllocTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kccuSetTunableState(POBJGPU pGpu, struct KernelCcu *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kccuIsPresent(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCcu_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCcu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelCcu(KernelCcu *pThis) {
    __nvoc_kccuDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelCcu_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelCcu(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelCcu_exit; // Success

__nvoc_ctor_KernelCcu_fail_OBJENGSTATE:
__nvoc_ctor_KernelCcu_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelCcu_1(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // Hal function -- kccuConstructEngine
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__kccuConstructEngine__ = &kccuConstructEngine_IMPL;
    }
    else if (0)
    {
    }

    pThis->__kccuStateLoad__ = &kccuStateLoad_IMPL;

    pThis->__kccuStateUnload__ = &kccuStateUnload_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelCcu_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelCcu_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelCcu_engstateStateUnload;

    pThis->__kccuReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuReconcileTunableState;

    pThis->__kccuStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kccuStateInitLocked;

    pThis->__kccuStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePreLoad;

    pThis->__kccuStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePostUnload;

    pThis->__kccuStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kccuStateDestroy;

    pThis->__kccuStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePreUnload;

    pThis->__kccuStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kccuStateInitUnlocked;

    pThis->__kccuInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kccuInitMissing;

    pThis->__kccuStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePreInitLocked;

    pThis->__kccuStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePreInitUnlocked;

    pThis->__kccuGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuGetTunableState;

    pThis->__kccuCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuCompareTunableState;

    pThis->__kccuFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuFreeTunableState;

    pThis->__kccuStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kccuStatePostLoad;

    pThis->__kccuAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuAllocTunableState;

    pThis->__kccuSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kccuSetTunableState;

    pThis->__kccuIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kccuIsPresent;
}

void __nvoc_init_funcTable_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelCcu_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelCcu(KernelCcu *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCcu = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelCcu(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelCcu(KernelCcu **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelCcu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelCcu));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelCcu));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCcu);

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

    __nvoc_init_KernelCcu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCcu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCcu_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelCcu_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCcu(KernelCcu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelCcu(ppThis, pParent, createFlags);

    return status;
}

