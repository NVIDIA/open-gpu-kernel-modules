#define NVOC_KERNEL_GSP_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_gsp_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x311d4e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFalcon;

void __nvoc_init_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGsp(KernelGsp*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelGsp(KernelGsp*, RmHalspecOwner* );
void __nvoc_dtor_KernelGsp(KernelGsp*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp;

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelGsp = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGsp,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGsp,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_IntrService),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGsp_KernelFalcon = {
    /*pClassDef=*/          &__nvoc_class_def_KernelFalcon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGsp, __nvoc_base_KernelFalcon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGsp = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGsp_KernelGsp,
        &__nvoc_rtti_KernelGsp_KernelFalcon,
        &__nvoc_rtti_KernelGsp_IntrService,
        &__nvoc_rtti_KernelGsp_OBJENGSTATE,
        &__nvoc_rtti_KernelGsp_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsp = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGsp),
        /*classId=*/            classId(KernelGsp),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGsp",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGsp,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGsp,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGsp
};

static NV_STATUS __nvoc_thunk_KernelGsp_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelGsp, ENGDESCRIPTOR arg0) {
    return kgspConstructEngine(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelGsp_intrservRegisterIntrService(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceRecord pRecords[155]) {
    kgspRegisterIntrService(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pRecords);
}

static NvU32 __nvoc_thunk_KernelGsp_intrservServiceInterrupt(struct OBJGPU *pGpu, struct IntrService *pKernelGsp, IntrServiceServiceInterruptArguments *pParams) {
    return kgspServiceInterrupt(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_KernelGsp_kflcnResetHw(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspResetHw(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_KernelFalcon.offset));
}

static NvBool __nvoc_thunk_KernelGsp_kflcnIsEngineInReset(struct OBJGPU *pGpu, struct KernelFalcon *pKernelGsp) {
    return kgspIsEngineInReset(pGpu, (struct KernelGsp *)(((unsigned char *)pKernelGsp) - __nvoc_rtti_KernelGsp_KernelFalcon.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kgspStateDestroy(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kgspFreeTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspCompareTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static NvBool __nvoc_thunk_IntrService_kgspClearInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kgspIsPresent(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspReconcileTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_IntrService_kgspServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGsp *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGsp_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateInitLocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePostUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreUnload(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspGetTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStateInitUnlocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kgspInitMissing(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreInitLocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePreInitUnlocked(POBJGPU pGpu, struct KernelGsp *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspStatePostLoad(POBJGPU pGpu, struct KernelGsp *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspAllocTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgspSetTunableState(POBJGPU pGpu, struct KernelGsp *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGsp_OBJENGSTATE.offset), pTunableState);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGsp = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelFalcon(KernelFalcon*);
void __nvoc_dtor_KernelGsp(KernelGsp *pThis) {
    __nvoc_kgspDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_dtor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
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
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelFalcon(KernelFalcon* , RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_IntrService;
    status = __nvoc_ctor_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_ctor_KernelGsp_fail_KernelFalcon;
    __nvoc_init_dataField_KernelGsp(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGsp_exit; // Success

__nvoc_ctor_KernelGsp_fail_KernelFalcon:
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
__nvoc_ctor_KernelGsp_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGsp_fail_OBJENGSTATE:
__nvoc_ctor_KernelGsp_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelGsp_1(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kgspConstructEngine__ = &kgspConstructEngine_IMPL;

    pThis->__kgspRegisterIntrService__ = &kgspRegisterIntrService_IMPL;

    pThis->__kgspServiceInterrupt__ = &kgspServiceInterrupt_IMPL;

    // Hal function -- kgspConfigureFalcon
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspConfigureFalcon__ = &kgspConfigureFalcon_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspIsDebugModeEnabled
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspIsDebugModeEnabled__ = &kgspIsDebugModeEnabled_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspBootstrapRiscvOSEarly
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspBootstrapRiscvOSEarly__ = &kgspBootstrapRiscvOSEarly_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspBootstrapRiscvOSEarly__ = &kgspBootstrapRiscvOSEarly_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetGspRmBootUcodeStorage
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetGspRmBootUcodeStorage__ = &kgspGetGspRmBootUcodeStorage_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetBinArchiveGspRmBoot
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveGspRmBoot__ = &kgspGetBinArchiveGspRmBoot_GA102;
        }
        else if (0)
        {
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspExecuteSequencerCommand
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspExecuteSequencerCommand__ = &kgspExecuteSequencerCommand_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspReadUcodeFuseVersion
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_b2b553;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspReadUcodeFuseVersion__ = &kgspReadUcodeFuseVersion_GA100;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspResetHw
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspResetHw__ = &kgspResetHw_TU102;
        }
        else if (0)
        {
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspIsEngineInReset
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspIsEngineInReset__ = &kgspIsEngineInReset_TU102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetFrtsSize
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetFrtsSize__ = &kgspGetFrtsSize_4a4dee;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspExecuteFwsecFrts
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspExecuteFwsecFrts__ = &kgspExecuteFwsecFrts_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspExecuteFwsecFrts__ = &kgspExecuteFwsecFrts_5baef9;
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspExecuteHsFalcon
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspExecuteHsFalcon__ = &kgspExecuteHsFalcon_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetBinArchiveBooterLoadUcode
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterLoadUcode__ = &kgspGetBinArchiveBooterLoadUcode_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetBinArchiveBooterUnloadUcode
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_TU116;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetBinArchiveBooterUnloadUcode__ = &kgspGetBinArchiveBooterUnloadUcode_GA102;
        }
        else if (0)
        {
        }
    }
    else if (0)
    {
    }

    // Hal function -- kgspGetSignatureSectionName
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kgspGetSignatureSectionName__ = &kgspGetSignatureSectionName_63b8e2;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kgspGetSignatureSectionName__ = &kgspGetSignatureSectionName_e46f5b;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000000e0UL) )) /* ChipHal: TU102 | TU104 | TU106 */ 
        {
            pThis->__kgspGetSignatureSectionName__ = &kgspGetSignatureSectionName_cbc19d;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000300UL) )) /* ChipHal: TU116 | TU117 */ 
        {
            pThis->__kgspGetSignatureSectionName__ = &kgspGetSignatureSectionName_ab7237;
        }
        // default
        else
        {
            pThis->__kgspGetSignatureSectionName__ = &kgspGetSignatureSectionName_9e2234;
        }
    }
    else if (0)
    {
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelGsp_engstateConstructEngine;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelGsp_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_KernelGsp_intrservServiceInterrupt;

    pThis->__nvoc_base_KernelFalcon.__kflcnResetHw__ = &__nvoc_thunk_KernelGsp_kflcnResetHw;

    pThis->__nvoc_base_KernelFalcon.__kflcnIsEngineInReset__ = &__nvoc_thunk_KernelGsp_kflcnIsEngineInReset;

    pThis->__kgspStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kgspStateDestroy;

    pThis->__kgspFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspFreeTunableState;

    pThis->__kgspCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspCompareTunableState;

    pThis->__kgspClearInterrupt__ = &__nvoc_thunk_IntrService_kgspClearInterrupt;

    pThis->__kgspIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kgspIsPresent;

    pThis->__kgspReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspReconcileTunableState;

    pThis->__kgspStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStateLoad;

    pThis->__kgspStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStateUnload;

    pThis->__kgspServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_kgspServiceNotificationInterrupt;

    pThis->__kgspStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStateInitLocked;

    pThis->__kgspStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreLoad;

    pThis->__kgspStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePostUnload;

    pThis->__kgspStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreUnload;

    pThis->__kgspGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspGetTunableState;

    pThis->__kgspStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStateInitUnlocked;

    pThis->__kgspInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kgspInitMissing;

    pThis->__kgspStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreInitLocked;

    pThis->__kgspStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePreInitUnlocked;

    pThis->__kgspStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kgspStatePostLoad;

    pThis->__kgspAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspAllocTunableState;

    pThis->__kgspSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgspSetTunableState;
}

void __nvoc_init_funcTable_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelGsp_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*, RmHalspecOwner* );
void __nvoc_init_IntrService(IntrService*, RmHalspecOwner* );
void __nvoc_init_KernelFalcon(KernelFalcon*, RmHalspecOwner* );
void __nvoc_init_KernelGsp(KernelGsp *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGsp = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    pThis->__nvoc_pbase_KernelFalcon = &pThis->__nvoc_base_KernelFalcon;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE, pRmhalspecowner);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService, pRmhalspecowner);
    __nvoc_init_KernelFalcon(&pThis->__nvoc_base_KernelFalcon, pRmhalspecowner);
    __nvoc_init_funcTable_KernelGsp(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelGsp *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelGsp));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelGsp));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGsp);

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

    __nvoc_init_KernelGsp(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGsp(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGsp_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelGsp_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGsp(KernelGsp **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGsp(ppThis, pParent, createFlags);

    return status;
}

