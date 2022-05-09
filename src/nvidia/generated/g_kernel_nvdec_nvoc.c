#define NVOC_KERNEL_NVDEC_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvdec_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xaba9df = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvdec;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_KernelNvdec(KernelNvdec*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelNvdec(KernelNvdec*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelNvdec(KernelNvdec*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelNvdec(KernelNvdec*, RmHalspecOwner* );
void __nvoc_dtor_KernelNvdec(KernelNvdec*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelNvdec;

static const struct NVOC_RTTI __nvoc_rtti_KernelNvdec_KernelNvdec = {
    /*pClassDef=*/          &__nvoc_class_def_KernelNvdec,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelNvdec,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvdec_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvdec, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvdec_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvdec, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvdec_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvdec, __nvoc_base_KernelFalcon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvdec_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvdec, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelNvdec = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_KernelNvdec_KernelNvdec,
        &__nvoc_rtti_KernelNvdec_IntrService,
        &__nvoc_rtti_KernelNvdec_KernelFalcon,
        &__nvoc_rtti_KernelNvdec_OBJENGSTATE,
        &__nvoc_rtti_KernelNvdec_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvdec = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelNvdec),
        /*classId=*/            classId(KernelNvdec),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelNvdec",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelNvdec,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelNvdec,
    /*pExportInfo=*/        &__nvoc_export_info_KernelNvdec
};

static NV_STATUS __nvoc_thunk_KernelNvdec_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelNvdec, ENGDESCRIPTOR arg0) {
    return knvdecConstructEngine(pGpu, (struct KernelNvdec *)(((unsigned char *)pKernelNvdec) - __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelNvdec_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelNvdec) {
    return knvdecResetHw(pGpu, (struct KernelNvdec *)(((unsigned char *)pKernelNvdec) - __nvoc_rtti_KernelNvdec_KernelFalcon.offset));
}

static NvBool __nvoc_thunk_KernelNvdec_kflcnIsEngineInReset(struct OBJGPU *pGpu, struct KernelFalcon *pKernelNvdec) {
    return knvdecIsEngineInReset(pGpu, (struct KernelNvdec *)(((unsigned char *)pKernelNvdec) - __nvoc_rtti_KernelNvdec_KernelFalcon.offset));
}

static void __nvoc_thunk_KernelNvdec_intrservRegisterIntrService(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceRecord arg2[155]) {
    knvdecRegisterIntrService(arg0, (struct KernelNvdec *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvdec_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelNvdec_intrservServiceNotificationInterrupt(struct OBJGPU *arg0, struct IntrService *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return knvdecServiceNotificationInterrupt(arg0, (struct KernelNvdec *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvdec_IntrService.offset), arg2);
}

static void __nvoc_thunk_OBJENGSTATE_knvdecStateDestroy(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_knvdecFreeTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecCompareTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static NvBool __nvoc_thunk_IntrService_knvdecClearInterrupt(struct OBJGPU *pGpu, struct KernelNvdec *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelNvdec_IntrService.offset), pParams);
}

static NvBool __nvoc_thunk_OBJENGSTATE_knvdecIsPresent(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static NvU32 __nvoc_thunk_IntrService_knvdecServiceInterrupt(struct OBJGPU *pGpu, struct KernelNvdec *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelNvdec_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecReconcileTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStateLoad(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStateUnload(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStateInitLocked(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePreLoad(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePostUnload(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePreUnload(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStateInitUnlocked(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_knvdecInitMissing(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePreInitLocked(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePreInitUnlocked(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecGetTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecStatePostLoad(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecAllocTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvdecSetTunableState(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvdec_OBJENGSTATE.offset), pTunableState);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelNvdec = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelNvdec(KernelNvdec *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelNvdec(KernelNvdec *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelNvdec(KernelNvdec *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelNvdec_fail_OBJENGSTATE;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelNvdec_fail_KernelFalcon;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelNvdec_fail_IntrService;
    __nvoc_init_dataField_KernelNvdec(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelNvdec_exit; // Success

__nvoc_ctor_KernelNvdec_fail_IntrService:
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
__nvoc_ctor_KernelNvdec_fail_KernelFalcon:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelNvdec_fail_OBJENGSTATE:
__nvoc_ctor_KernelNvdec_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelNvdec_1(KernelNvdec *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    ChipHal *chipHal = &pRmhalspecowner->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
    PORT_UNREFERENCED_VARIABLE(chipHal);
    PORT_UNREFERENCED_VARIABLE(chipHal_HalVarIdx);

    // Hal function -- knvdecConstructEngine
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__knvdecConstructEngine__ = &knvdecConstructEngine_IMPL;
    }
    else if (0)
    {
    }

    // Hal function -- knvdecConfigureFalcon
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__knvdecConfigureFalcon__ = &knvdecConfigureFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__knvdecConfigureFalcon__ = &knvdecConfigureFalcon_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvdecConfigureFalcon__ = &knvdecConfigureFalcon_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- knvdecResetHw
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvdecResetHw__ = &knvdecResetHw_ac1694;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    pThis->__knvdecIsEngineInReset__ = &knvdecIsEngineInReset_167f46;

    // Hal function -- knvdecIsEngineDisabled
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__knvdecIsEngineDisabled__ = &knvdecIsEngineDisabled_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__knvdecIsEngineDisabled__ = &knvdecIsEngineDisabled_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvdecIsEngineDisabled__ = &knvdecIsEngineDisabled_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- knvdecReadUcodeFuseVersion
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__knvdecReadUcodeFuseVersion__ = &knvdecReadUcodeFuseVersion_b2b553;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvdecReadUcodeFuseVersion__ = &knvdecReadUcodeFuseVersion_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    pThis->__knvdecRegisterIntrService__ = &knvdecRegisterIntrService_IMPL;

    pThis->__knvdecServiceNotificationInterrupt__ = &knvdecServiceNotificationInterrupt_IMPL;

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelNvdec_engstateConstructEngine;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_KernelNvdec_kflcnResetHw;

    pThis->__nvoc_base_KernelFalcon.__kflcnIsEngineInReset__ = &__nvoc_thunk_KernelNvdec_kflcnIsEngineInReset;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelNvdec_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_KernelNvdec_intrservServiceNotificationInterrupt;

    pThis->__knvdecStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_knvdecStateDestroy;

    pThis->__knvdecFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecFreeTunableState;

    pThis->__knvdecCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecCompareTunableState;

    pThis->__knvdecClearInterrupt__ = &__nvoc_thunk_IntrService_knvdecClearInterrupt;

    pThis->__knvdecIsPresent__ = &__nvoc_thunk_OBJENGSTATE_knvdecIsPresent;

    pThis->__knvdecServiceInterrupt__ = &__nvoc_thunk_IntrService_knvdecServiceInterrupt;

    pThis->__knvdecReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecReconcileTunableState;

    pThis->__knvdecStateLoad__ = &__nvoc_thunk_OBJENGSTATE_knvdecStateLoad;

    pThis->__knvdecStateUnload__ = &__nvoc_thunk_OBJENGSTATE_knvdecStateUnload;

    pThis->__knvdecStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_knvdecStateInitLocked;

    pThis->__knvdecStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePreLoad;

    pThis->__knvdecStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePostUnload;

    pThis->__knvdecStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePreUnload;

    pThis->__knvdecStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_knvdecStateInitUnlocked;

    pThis->__knvdecInitMissing__ = &__nvoc_thunk_OBJENGSTATE_knvdecInitMissing;

    pThis->__knvdecStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePreInitLocked;

    pThis->__knvdecStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePreInitUnlocked;

    pThis->__knvdecGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecGetTunableState;

    pThis->__knvdecStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_knvdecStatePostLoad;

    pThis->__knvdecAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecAllocTunableState;

    pThis->__knvdecSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvdecSetTunableState;
}

void __nvoc_init_funcTable_KernelNvdec(KernelNvdec *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelNvdec_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*, RmHalspecOwner* );
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*, RmHalspecOwner* );
void __nvoc_init_KernelNvdec(KernelNvdec *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelNvdec = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    __nvoc_init_funcTable_KernelNvdec(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelNvdec(KernelNvdec **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelNvdec *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelNvdec));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelNvdec));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelNvdec);

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

    __nvoc_init_KernelNvdec(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelNvdec(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelNvdec_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelNvdec_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelNvdec(KernelNvdec **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelNvdec(ppThis, pParent, createFlags);

    return status;
}

