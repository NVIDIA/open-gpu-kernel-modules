#define NVOC_KERNEL_CE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_ce_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x242aca = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelCE(KernelCE*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelCE(KernelCE*, RmHalspecOwner* );
void __nvoc_dtor_KernelCE(KernelCE*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCE;

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_KernelCE = {
    /*pClassDef=*/          &__nvoc_class_def_KernelCE,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCE,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelCE_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelCE, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelCE = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_KernelCE_KernelCE,
        &__nvoc_rtti_KernelCE_IntrService,
        &__nvoc_rtti_KernelCE_OBJENGSTATE,
        &__nvoc_rtti_KernelCE_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCE),
        /*classId=*/            classId(KernelCE),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCE",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCE,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelCE,
    /*pExportInfo=*/        &__nvoc_export_info_KernelCE
};

static NV_STATUS __nvoc_thunk_KernelCE_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, ENGDESCRIPTOR arg0) {
    return kceConstructEngine(pGpu, (struct KernelCE *)(((unsigned char *)pKCe) - __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg0);
}

static NvBool __nvoc_thunk_KernelCE_engstateIsPresent(OBJGPU *pGpu, struct OBJENGSTATE *pKCe) {
    return kceIsPresent(pGpu, (struct KernelCE *)(((unsigned char *)pKCe) - __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelCE_engstateStateLoad(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return kceStateLoad(arg0, (struct KernelCE *)(((unsigned char *)arg1) - __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelCE_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKCe, NvU32 flags) {
    return kceStateUnload(pGpu, (struct KernelCE *)(((unsigned char *)pKCe) - __nvoc_rtti_KernelCE_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelCE_intrservRegisterIntrService(OBJGPU *arg0, struct IntrService *arg1, IntrServiceRecord arg2[155]) {
    kceRegisterIntrService(arg0, (struct KernelCE *)(((unsigned char *)arg1) - __nvoc_rtti_KernelCE_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelCE_intrservServiceNotificationInterrupt(OBJGPU *arg0, struct IntrService *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return kceServiceNotificationInterrupt(arg0, (struct KernelCE *)(((unsigned char *)arg1) - __nvoc_rtti_KernelCE_IntrService.offset), arg2);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceReconcileTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStateInitLocked(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePreLoad(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePostUnload(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_kceStateDestroy(POBJGPU pGpu, struct KernelCE *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePreUnload(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStateInitUnlocked(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kceInitMissing(POBJGPU pGpu, struct KernelCE *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePreInitLocked(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePreInitUnlocked(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceGetTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceCompareTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kceFreeTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_IntrService_kceClearInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelCE_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceStatePostLoad(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceAllocTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kceSetTunableState(POBJGPU pGpu, struct KernelCE *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelCE_OBJENGSTATE.offset), pTunableState);
}

static NvU32 __nvoc_thunk_IntrService_kceServiceInterrupt(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return intrservServiceInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelCE_IntrService.offset), pParams);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelCE = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelCE(KernelCE *pThis) {
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelCE_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelCE_fail_IntrService;
    __nvoc_init_dataField_KernelCE(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelCE_exit; // Success

__nvoc_ctor_KernelCE_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelCE_fail_OBJENGSTATE:
__nvoc_ctor_KernelCE_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelCE_1(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kceConstructEngine__ = &kceConstructEngine_IMPL;

    // Hal function -- kceIsPresent
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceIsPresent__ = &kceIsPresent_IMPL;
    }
    else if (0)
    {
    }

    // Hal function -- kceStateLoad
    if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceStateLoad__ = &kceStateLoad_GP100;
    }
    else if (0)
    {
    }

    // Hal function -- kceStateUnload
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceStateUnload__ = &kceStateUnload_GP100;
    }
    // default
    else
    {
        pThis->__kceStateUnload__ = &kceStateUnload_56cd7a;
    }

    pThis->__kceRegisterIntrService__ = &kceRegisterIntrService_IMPL;

    pThis->__kceServiceNotificationInterrupt__ = &kceServiceNotificationInterrupt_IMPL;

    // Hal function -- kceGetNvlinkAutoConfigCeValues
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetNvlinkAutoConfigCeValues__ = &kceGetNvlinkAutoConfigCeValues_GA100;
    }
    else if (0)
    {
    }

    // Hal function -- kceGetNvlinkMaxTopoForTable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkMaxTopoForTable__ = &kceGetNvlinkMaxTopoForTable_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetNvlinkMaxTopoForTable__ = &kceGetNvlinkMaxTopoForTable_491d52;
    }

    // Hal function -- kceIsCurrentMaxTopology
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceIsCurrentMaxTopology__ = &kceIsCurrentMaxTopology_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceIsCurrentMaxTopology__ = &kceIsCurrentMaxTopology_491d52;
    }

    // Hal function -- kceGetGrceConfigSize1
    if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetGrceConfigSize1__ = &kceGetGrceConfigSize1_GA100;
    }
    else if (0)
    {
    }

    // Hal function -- kceGetPce2lceConfigSize1
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetPce2lceConfigSize1__ = &kceGetPce2lceConfigSize1_GA102;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- kceGetMappings
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetMappings__ = &kceGetMappings_46f6a7;
    }

    // Hal function -- kceMapPceLceForSysmemLinks
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceMapPceLceForSysmemLinks__ = &kceMapPceLceForSysmemLinks_46f6a7;
    }

    // Hal function -- kceMapPceLceForNvlinkPeers
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceMapPceLceForNvlinkPeers__ = &kceMapPceLceForNvlinkPeers_46f6a7;
    }

    // Hal function -- kceGetSysmemSupportedLceMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetSysmemSupportedLceMask__ = &kceGetSysmemSupportedLceMask_4a4dee;
    }

    // Hal function -- kceMapAsyncLceDefault
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceMapAsyncLceDefault__ = &kceMapAsyncLceDefault_46f6a7;
    }

    // Hal function -- kceGetNvlinkPeerSupportedLceMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetNvlinkPeerSupportedLceMask__ = &kceGetNvlinkPeerSupportedLceMask_4a4dee;
    }

    // Hal function -- kceGetGrceSupportedLceMask
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kceGetGrceSupportedLceMask__ = &kceGetGrceSupportedLceMask_4a4dee;
    }

    // Hal function -- kceIsGen4orHigherSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceIsGen4orHigherSupported__ = &kceIsGen4orHigherSupported_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceIsGen4orHigherSupported__ = &kceIsGen4orHigherSupported_cbe027;
    }

    // Hal function -- kceApplyGen4orHigherMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kceApplyGen4orHigherMapping__ = &kceApplyGen4orHigherMapping_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kceApplyGen4orHigherMapping__ = &kceApplyGen4orHigherMapping_b3696a;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelCE_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateIsPresent__ = &__nvoc_thunk_KernelCE_engstateIsPresent;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelCE_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelCE_engstateStateUnload;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelCE_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_KernelCE_intrservServiceNotificationInterrupt;

    pThis->__kceReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceReconcileTunableState;

    pThis->__kceStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kceStateInitLocked;

    pThis->__kceStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kceStatePreLoad;

    pThis->__kceStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kceStatePostUnload;

    pThis->__kceStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_kceStateDestroy;

    pThis->__kceStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kceStatePreUnload;

    pThis->__kceStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kceStateInitUnlocked;

    pThis->__kceInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kceInitMissing;

    pThis->__kceStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kceStatePreInitLocked;

    pThis->__kceStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kceStatePreInitUnlocked;

    pThis->__kceGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceGetTunableState;

    pThis->__kceCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceCompareTunableState;

    pThis->__kceFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceFreeTunableState;

    pThis->__kceClearInterrupt__ = &__nvoc_thunk_IntrService_kceClearInterrupt;

    pThis->__kceStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kceStatePostLoad;

    pThis->__kceAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceAllocTunableState;

    pThis->__kceSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kceSetTunableState;

    pThis->__kceServiceInterrupt__ = &__nvoc_thunk_IntrService_kceServiceInterrupt;
}

void __nvoc_init_funcTable_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelCE_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelCE(KernelCE *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelCE = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_KernelCE(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelCE(KernelCE **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelCE *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelCE));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelCE));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelCE);

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

    __nvoc_init_KernelCE(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCE(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCE_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelCE_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCE(KernelCE **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelCE(ppThis, pParent, createFlags);

    return status;
}

