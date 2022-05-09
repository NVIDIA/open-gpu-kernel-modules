#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_gmmu_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x29362f = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGmmu;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

void __nvoc_init_KernelGmmu(KernelGmmu*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelGmmu(KernelGmmu*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelGmmu(KernelGmmu*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelGmmu(KernelGmmu*, RmHalspecOwner* );
void __nvoc_dtor_KernelGmmu(KernelGmmu*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGmmu;

static const struct NVOC_RTTI __nvoc_rtti_KernelGmmu_KernelGmmu = {
    /*pClassDef=*/          &__nvoc_class_def_KernelGmmu,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelGmmu,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGmmu_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGmmu_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGmmu, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelGmmu_IntrService = {
    /*pClassDef=*/          &__nvoc_class_def_IntrService,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelGmmu, __nvoc_base_IntrService),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelGmmu = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_KernelGmmu_KernelGmmu,
        &__nvoc_rtti_KernelGmmu_IntrService,
        &__nvoc_rtti_KernelGmmu_OBJENGSTATE,
        &__nvoc_rtti_KernelGmmu_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGmmu = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelGmmu),
        /*classId=*/            classId(KernelGmmu),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelGmmu",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelGmmu,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelGmmu,
    /*pExportInfo=*/        &__nvoc_export_info_KernelGmmu
};

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, ENGDESCRIPTOR arg0) {
    return kgmmuConstructEngine(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu) {
    return kgmmuStateInitLocked(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 flags) {
    return kgmmuStatePostLoad(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelGmmu_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu) {
    kgmmuStateDestroy(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelGmmu_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceRecord arg0[155]) {
    kgmmuRegisterIntrService(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), arg0);
}

static NvU32 __nvoc_thunk_KernelGmmu_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams) {
    return kgmmuServiceInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuReconcileTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStateLoad(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStateUnload(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_IntrService_kgmmuServiceNotificationInterrupt(struct OBJGPU *pGpu, struct KernelGmmu *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return intrservServiceNotificationInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePreLoad(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePostUnload(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePreUnload(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStateInitUnlocked(POBJGPU pGpu, struct KernelGmmu *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kgmmuInitMissing(POBJGPU pGpu, struct KernelGmmu *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitLocked(POBJGPU pGpu, struct KernelGmmu *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked(POBJGPU pGpu, struct KernelGmmu *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuGetTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuCompareTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kgmmuFreeTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_IntrService_kgmmuClearInterrupt(struct OBJGPU *pGpu, struct KernelGmmu *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return intrservClearInterrupt(pGpu, (struct IntrService *)(((unsigned char *)pIntrService) + __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuAllocTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuSetTunableState(POBJGPU pGpu, struct KernelGmmu *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kgmmuIsPresent(POBJGPU pGpu, struct KernelGmmu *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelGmmu = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_IntrService(IntrService*);
void __nvoc_dtor_KernelGmmu(KernelGmmu *pThis) {
    __nvoc_kgmmuDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_dtor_IntrService(&pThis->__nvoc_base_IntrService);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelGmmu(KernelGmmu *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED, ((NvBool)(0 != 0)));
    }

    // Hal field -- defaultBigPageSize
    if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->defaultBigPageSize = (64 * 1024);
    }
    else if (0)
    {
    }

    // Hal field -- bHugePageSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bHugePageSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bHugePageSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bPageSize512mbSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bPageSize512mbSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bPageSize512mbSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bBug2720120WarEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->bBug2720120WarEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBug2720120WarEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bVaspaceInteropSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bVaspaceInteropSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bVaspaceInteropSupported = ((NvBool)(0 != 0));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_IntrService(IntrService* );
NV_STATUS __nvoc_ctor_KernelGmmu(KernelGmmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelGmmu_fail_OBJENGSTATE;
    status = __nvoc_ctor_IntrService(&pThis->__nvoc_base_IntrService);
    if (status != NV_OK) goto __nvoc_ctor_KernelGmmu_fail_IntrService;
    __nvoc_init_dataField_KernelGmmu(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelGmmu_exit; // Success

__nvoc_ctor_KernelGmmu_fail_IntrService:
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
__nvoc_ctor_KernelGmmu_fail_OBJENGSTATE:
__nvoc_ctor_KernelGmmu_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelGmmu_1(KernelGmmu *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kgmmuConstructEngine__ = &kgmmuConstructEngine_IMPL;

    pThis->__kgmmuStateInitLocked__ = &kgmmuStateInitLocked_IMPL;

    // Hal function -- kgmmuStatePostLoad
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->__kgmmuStatePostLoad__ = &kgmmuStatePostLoad_IMPL;
    }
    else if (0)
    {
    }

    pThis->__kgmmuStateDestroy__ = &kgmmuStateDestroy_IMPL;

    pThis->__kgmmuRegisterIntrService__ = &kgmmuRegisterIntrService_IMPL;

    pThis->__kgmmuServiceInterrupt__ = &kgmmuServiceInterrupt_IMPL;

    // Hal function -- kgmmuInstBlkVaLimitGet
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_f03539;
    }

    // Hal function -- kgmmuSetTlbInvalidateMembarWarParameters
    if (0)
    {
    }
    else if (0)
    {
    }
    else if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_4a4dee;
    }

    // Hal function -- kgmmuSetTlbInvalidationScope
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_46f6a7;
    }

    // Hal function -- kgmmuFmtInitLevels
    if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GP10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GA10X;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- kgmmuSetupWarForBug2720120
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_56cd7a;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelGmmu_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelGmmu_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelGmmu_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelGmmu_engstateStateDestroy;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelGmmu_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_KernelGmmu_intrservServiceInterrupt;

    pThis->__kgmmuReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuReconcileTunableState;

    pThis->__kgmmuStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStateLoad;

    pThis->__kgmmuStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStateUnload;

    pThis->__kgmmuServiceNotificationInterrupt__ = &__nvoc_thunk_IntrService_kgmmuServiceNotificationInterrupt;

    pThis->__kgmmuStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreLoad;

    pThis->__kgmmuStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePostUnload;

    pThis->__kgmmuStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreUnload;

    pThis->__kgmmuStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStateInitUnlocked;

    pThis->__kgmmuInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kgmmuInitMissing;

    pThis->__kgmmuStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitLocked;

    pThis->__kgmmuStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked;

    pThis->__kgmmuGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuGetTunableState;

    pThis->__kgmmuCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuCompareTunableState;

    pThis->__kgmmuFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuFreeTunableState;

    pThis->__kgmmuClearInterrupt__ = &__nvoc_thunk_IntrService_kgmmuClearInterrupt;

    pThis->__kgmmuAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuAllocTunableState;

    pThis->__kgmmuSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kgmmuSetTunableState;

    pThis->__kgmmuIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kgmmuIsPresent;
}

void __nvoc_init_funcTable_KernelGmmu(KernelGmmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelGmmu_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_IntrService(IntrService*);
void __nvoc_init_KernelGmmu(KernelGmmu *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelGmmu = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    pThis->__nvoc_pbase_IntrService = &pThis->__nvoc_base_IntrService;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_IntrService(&pThis->__nvoc_base_IntrService);
    __nvoc_init_funcTable_KernelGmmu(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelGmmu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelGmmu));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelGmmu));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGmmu);

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

    __nvoc_init_KernelGmmu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGmmu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGmmu_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelGmmu_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGmmu(ppThis, pParent, createFlags);

    return status;
}

