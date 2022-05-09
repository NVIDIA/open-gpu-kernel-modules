#define NVOC_KERNEL_MC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_mc_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3827ff = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelMc(KernelMc*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelMc(KernelMc*, RmHalspecOwner* );
void __nvoc_dtor_KernelMc(KernelMc*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMc;

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_KernelMc = {
    /*pClassDef=*/          &__nvoc_class_def_KernelMc,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMc,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMc_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMc, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelMc = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelMc_KernelMc,
        &__nvoc_rtti_KernelMc_OBJENGSTATE,
        &__nvoc_rtti_KernelMc_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelMc),
        /*classId=*/            classId(KernelMc),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelMc",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMc,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelMc,
    /*pExportInfo=*/        &__nvoc_export_info_KernelMc
};

static NV_STATUS __nvoc_thunk_KernelMc_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelMc) {
    return kmcStateInitLocked(pGpu, (struct KernelMc *)(((unsigned char *)pKernelMc) - __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcReconcileTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStateLoad(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStateUnload(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePreLoad(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePostUnload(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kmcStateDestroy(POBJGPU pGpu, struct KernelMc *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePreUnload(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStateInitUnlocked(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kmcInitMissing(POBJGPU pGpu, struct KernelMc *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePreInitLocked(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePreInitUnlocked(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcGetTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcCompareTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kmcFreeTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcStatePostLoad(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcAllocTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcSetTunableState(POBJGPU pGpu, struct KernelMc *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmcConstructEngine(POBJGPU pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg0) {
    return engstateConstructEngine(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kmcIsPresent(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMc_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMc = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMc(KernelMc *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMc_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelMc(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelMc_exit; // Success

__nvoc_ctor_KernelMc_fail_OBJENGSTATE:
__nvoc_ctor_KernelMc_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelMc_1(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    pThis->__kmcStateInitLocked__ = &kmcStateInitLocked_IMPL;

    // Hal function -- kmcWritePmcEnableReg
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmcWritePmcEnableReg__ = &kmcWritePmcEnableReg_GK104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kmcWritePmcEnableReg__ = &kmcWritePmcEnableReg_GA100;
    }
    else if (0)
    {
    }

    // Hal function -- kmcReadPmcEnableReg
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmcReadPmcEnableReg__ = &kmcReadPmcEnableReg_GK104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kmcReadPmcEnableReg__ = &kmcReadPmcEnableReg_GA100;
    }
    else if (0)
    {
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelMc_engstateStateInitLocked;

    pThis->__kmcReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcReconcileTunableState;

    pThis->__kmcStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kmcStateLoad;

    pThis->__kmcStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kmcStateUnload;

    pThis->__kmcStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePreLoad;

    pThis->__kmcStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePostUnload;

    pThis->__kmcStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kmcStateDestroy;

    pThis->__kmcStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePreUnload;

    pThis->__kmcStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmcStateInitUnlocked;

    pThis->__kmcInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kmcInitMissing;

    pThis->__kmcStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePreInitLocked;

    pThis->__kmcStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePreInitUnlocked;

    pThis->__kmcGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcGetTunableState;

    pThis->__kmcCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcCompareTunableState;

    pThis->__kmcFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcFreeTunableState;

    pThis->__kmcStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kmcStatePostLoad;

    pThis->__kmcAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcAllocTunableState;

    pThis->__kmcSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmcSetTunableState;

    pThis->__kmcConstructEngine__ = &__nvoc_thunk_OBJENGSTATE_kmcConstructEngine;

    pThis->__kmcIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kmcIsPresent;
}

void __nvoc_init_funcTable_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelMc_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelMc(KernelMc *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelMc = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelMc(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMc(KernelMc **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelMc *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelMc));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelMc));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMc);

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

    __nvoc_init_KernelMc(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMc(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMc_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelMc_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMc(KernelMc **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMc(ppThis, pParent, createFlags);

    return status;
}

