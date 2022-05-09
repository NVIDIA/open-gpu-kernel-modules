#define NVOC_OBJTMR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_objtmr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x9ddede = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTMR;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJINTRABLE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_init_funcTable_OBJTMR(OBJTMR*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_init_dataField_OBJTMR(OBJTMR*, RmHalspecOwner* );
void __nvoc_dtor_OBJTMR(OBJTMR*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTMR;

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_OBJTMR = {
    /*pClassDef=*/          &__nvoc_class_def_OBJTMR,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_OBJTMR,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_OBJINTRABLE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJINTRABLE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_OBJINTRABLE),
};

static const struct NVOC_RTTI __nvoc_rtti_OBJTMR_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(OBJTMR, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_OBJTMR = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_OBJTMR_OBJTMR,
        &__nvoc_rtti_OBJTMR_IntrService,
        &__nvoc_rtti_OBJTMR_OBJINTRABLE,
        &__nvoc_rtti_OBJTMR_OBJENGSTATE,
        &__nvoc_rtti_OBJTMR_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_OBJTMR = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(OBJTMR),
        /*classId=*/            classId(OBJTMR),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "OBJTMR",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_OBJTMR,
    /*pCastInfo=*/          &__nvoc_castinfo_OBJTMR,
    /*pExportInfo=*/        &__nvoc_export_info_OBJTMR
};

static void __nvoc_thunk_OBJTMR_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceRecord pRecords[155]) {
    tmrRegisterIntrService(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_IntrService.offset), pRecords);
}

static NvBool __nvoc_thunk_OBJTMR_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceClearInterruptArguments *pParams) {
    return tmrClearInterrupt(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_IntrService.offset), pParams);
}

static NvU32 __nvoc_thunk_OBJTMR_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pTmr, IntrServiceServiceInterruptArguments *pParams) {
    return tmrServiceInterrupt(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJTMR_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, ENGDESCRIPTOR arg0) {
    return tmrConstructEngine(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJTMR_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    return tmrStateInitLocked(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJTMR_engstateStateInitUnlocked(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    return tmrStateInitUnlocked(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJTMR_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg0) {
    return tmrStateLoad(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJTMR_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pTmr, NvU32 arg0) {
    return tmrStateUnload(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJTMR_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pTmr) {
    tmrStateDestroy(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJTMR_intrableGetPhysicalIntrVectors(OBJGPU *pGpu, struct OBJINTRABLE *pTmr, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return tmrGetPhysicalIntrVectors(pGpu, (struct OBJTMR *)(((unsigned char *)pTmr) - __nvoc_rtti_OBJTMR_OBJINTRABLE.offset), maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static void __nvoc_thunk_OBJENGSTATE_tmrFreeTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrCompareTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_tmrGetNotificationIntrVector(struct OBJGPU *pGpu, struct OBJTMR *pIntrable, NvU32 *pIntrVector) {
    return intrableGetNotificationIntrVector(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_OBJTMR_OBJINTRABLE.offset), pIntrVector);
}

static NvBool __nvoc_thunk_OBJENGSTATE_tmrIsPresent(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrReconcileTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_tmrGetKernelIntrVectors(struct OBJGPU *pGpu, struct OBJTMR *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return intrableGetKernelIntrVectors(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_OBJTMR_OBJINTRABLE.offset), maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static NV_STATUS __nvoc_thunk_OBJINTRABLE_tmrSetNotificationIntrVector(struct OBJGPU *pGpu, struct OBJTMR *pIntrable, NvU32 intrVector) {
    return intrableSetNotificationIntrVector(pGpu, (struct OBJINTRABLE *)(((unsigned char *)pIntrable) + __nvoc_rtti_OBJTMR_OBJINTRABLE.offset), intrVector);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePreLoad(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePostUnload(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePreUnload(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrGetTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), pTunableState);
}

static void __nvoc_thunk_OBJENGSTATE_tmrInitMissing(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePreInitLocked(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePreInitUnlocked(POBJGPU pGpu, struct OBJTMR *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_IntrService_tmrServiceNotificationInterrupt(OBJGPU *pGpu, struct OBJTMR *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_OBJTMR_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrStatePostLoad(POBJGPU pGpu, struct OBJTMR *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrAllocTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_tmrSetTunableState(POBJGPU pGpu, struct OBJTMR *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_OBJTMR_OBJENGSTATE.offset), pTunableState);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_OBJTMR = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_OBJINTRABLE(OBJINTRABLE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_OBJTMR(OBJTMR *pThis) {
    __nvoc_tmrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_ALARM_INTR_REMOVED_FROM_PMC_TREE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS, ((NvBool)(0 != 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_TMR_USE_PTIMER_FOR_OSTIMER_CALLBACKS, (0));
    pThis->setProperty(pThis, PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS, (0));

    // NVOC Property Hal field -- PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_TMR_USE_SECOND_COUNTDOWN_TIMER_FOR_SWRL, ((NvBool)(0 != 0)));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJINTRABLE(OBJINTRABLE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_OBJTMR_fail_OBJENGSTATE;
    status = __nvoc_ctor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_OBJTMR_fail_OBJINTRABLE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_OBJTMR_fail_IntrService;
    __nvoc_init_dataField_OBJTMR(pThis, pRmhalspecowner);
    goto __nvoc_ctor_OBJTMR_exit; // Success

__nvoc_ctor_OBJTMR_fail_IntrService:
    __nvoc_dtor_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE);
__nvoc_ctor_OBJTMR_fail_OBJINTRABLE:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_OBJTMR_fail_OBJENGSTATE:
__nvoc_ctor_OBJTMR_exit:

    return status;
}

static void __nvoc_init_funcTable_OBJTMR_1(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    pThis->__tmrRegisterIntrService__ = &tmrRegisterIntrService_IMPL;

    pThis->__tmrClearInterrupt__ = &tmrClearInterrupt_IMPL;

    // Hal function -- tmrServiceInterrupt
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_56cd7a;
    }
    else if (0)
    {
#if 0
        if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__tmrServiceInterrupt__ = &tmrServiceInterrupt_GA100;
        }
        else if (0)
        {
        }
#endif
    }

    pThis->__tmrConstructEngine__ = &tmrConstructEngine_IMPL;

    pThis->__tmrStateInitLocked__ = &tmrStateInitLocked_IMPL;

    pThis->__tmrStateInitUnlocked__ = &tmrStateInitUnlocked_IMPL;

    pThis->__tmrStateLoad__ = &tmrStateLoad_IMPL;

    pThis->__tmrStateUnload__ = &tmrStateUnload_IMPL;

    pThis->__tmrStateDestroy__ = &tmrStateDestroy_IMPL;

    // Hal function -- tmrGetGpuPtimerOffset
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__tmrGetGpuPtimerOffset__ = &tmrGetGpuPtimerOffset_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__tmrGetGpuPtimerOffset__ = &tmrGetGpuPtimerOffset_GA100;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- tmrGetPhysicalIntrVectors
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__tmrGetPhysicalIntrVectors__ = &tmrGetPhysicalIntrVectors_46f6a7;
    }
    else if (0)
    {
#if 0
        if (0)
        {
        }
        // default
        else
        {
            pThis->__tmrGetPhysicalIntrVectors__ = &tmrGetPhysicalIntrVectors_46f6a7;
        }
#endif
    }

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_OBJTMR_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservClearInterrupt__ = &__nvoc_thunk_OBJTMR_intrservClearInterrupt;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_OBJTMR_intrservServiceInterrupt;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_OBJTMR_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_OBJTMR_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__ = &__nvoc_thunk_OBJTMR_engstateStateInitUnlocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_OBJTMR_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_OBJTMR_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_OBJTMR_engstateStateDestroy;

    pThis->__nvoc_base_OBJINTRABLE.__intrableGetPhysicalIntrVectors__ = &__nvoc_thunk_OBJTMR_intrableGetPhysicalIntrVectors;

    pThis->__tmrFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrFreeTunableState;

    pThis->__tmrCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrCompareTunableState;

    pThis->__tmrGetNotificationIntrVector__ = &__nvoc_thunk_OBJINTRABLE_tmrGetNotificationIntrVector;

    pThis->__tmrIsPresent__ = &__nvoc_thunk_OBJENGSTATE_tmrIsPresent;

    pThis->__tmrReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrReconcileTunableState;

    pThis->__tmrGetKernelIntrVectors__ = &__nvoc_thunk_OBJINTRABLE_tmrGetKernelIntrVectors;

    pThis->__tmrSetNotificationIntrVector__ = &__nvoc_thunk_OBJINTRABLE_tmrSetNotificationIntrVector;

    pThis->__tmrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePreLoad;

    pThis->__tmrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePostUnload;

    pThis->__tmrStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePreUnload;

    pThis->__tmrGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrGetTunableState;

    pThis->__tmrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_tmrInitMissing;

    pThis->__tmrStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePreInitLocked;

    pThis->__tmrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePreInitUnlocked;

    pThis->__tmrServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_tmrServiceNotificationInterrupt;

    pThis->__tmrStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_tmrStatePostLoad;

    pThis->__tmrAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrAllocTunableState;

    pThis->__tmrSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_tmrSetTunableState;
}

void __nvoc_init_funcTable_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_OBJTMR_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*, RmHalspecOwner* );
void __nvoc_init_OBJINTRABLE(OBJINTRABLE*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*, RmHalspecOwner* );
void __nvoc_init_OBJTMR(OBJTMR *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_OBJTMR = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_OBJINTRABLE = &pThis->__nvoc_base_OBJINTRABLE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    __nvoc_init_OBJINTRABLE(&pThis->__nvoc_base_OBJINTRABLE, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    __nvoc_init_funcTable_OBJTMR(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_OBJTMR(OBJTMR **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    OBJTMR *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(OBJTMR));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(OBJTMR));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_OBJTMR);

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

    __nvoc_init_OBJTMR(pThis, pRmhalspecowner);
    status = __nvoc_ctor_OBJTMR(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_OBJTMR_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_OBJTMR_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_OBJTMR(OBJTMR **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_OBJTMR(ppThis, pParent, createFlags);

    return status;
}

