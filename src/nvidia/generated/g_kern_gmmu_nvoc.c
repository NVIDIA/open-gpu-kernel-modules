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

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg0) {
    return kgmmuStateLoad(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg0) {
    return kgmmuStateUnload(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg0) {
    return kgmmuStatePostLoad(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelGmmu_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu, NvU32 arg0) {
    return kgmmuStatePreUnload(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_KernelGmmu_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelGmmu) {
    kgmmuStateDestroy(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset));
}

static void __nvoc_thunk_KernelGmmu_intrservRegisterIntrService(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceRecord arg0[171]) {
    kgmmuRegisterIntrService(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), arg0);
}

static NvBool __nvoc_thunk_KernelGmmu_intrservClearInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceClearInterruptArguments *pParams) {
    return kgmmuClearInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NvU32 __nvoc_thunk_KernelGmmu_intrservServiceInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceInterruptArguments *pParams) {
    return kgmmuServiceInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_KernelGmmu_intrservServiceNotificationInterrupt(OBJGPU *pGpu, struct IntrService *pKernelGmmu, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return kgmmuServiceNotificationInterrupt(pGpu, (struct KernelGmmu *)(((unsigned char *)pKernelGmmu) - __nvoc_rtti_KernelGmmu_IntrService.offset), pParams);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePreLoad(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kgmmuStatePostUnload(POBJGPU pGpu, struct KernelGmmu *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelGmmu_OBJENGSTATE.offset), arg0);
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED, ((NvBool)(0 == 0)));
    }
    pThis->setProperty(pThis, PDB_PROP_KGMMU_REDUCE_NR_FAULT_BUFFER_SIZE, ((NvBool)(0 != 0)));

    // Hal field -- defaultBigPageSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->defaultBigPageSize = (64 * 1024);
    }

    // Hal field -- bHugePageSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bHugePageSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bPageSize512mbSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bVaspaceInteropSupported = ((NvBool)(0 == 0));
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

    pThis->__kgmmuStateLoad__ = &kgmmuStateLoad_IMPL;

    pThis->__kgmmuStateUnload__ = &kgmmuStateUnload_IMPL;

    // Hal function -- kgmmuStatePostLoad
    pThis->__kgmmuStatePostLoad__ = &kgmmuStatePostLoad_IMPL;

    // Hal function -- kgmmuStatePreUnload
    pThis->__kgmmuStatePreUnload__ = &kgmmuStatePreUnload_IMPL;

    pThis->__kgmmuStateDestroy__ = &kgmmuStateDestroy_IMPL;

    pThis->__kgmmuRegisterIntrService__ = &kgmmuRegisterIntrService_IMPL;

    pThis->__kgmmuClearInterrupt__ = &kgmmuClearInterrupt_IMPL;

    pThis->__kgmmuServiceInterrupt__ = &kgmmuServiceInterrupt_IMPL;

    // Hal function -- kgmmuServiceNotificationInterrupt
    pThis->__kgmmuServiceNotificationInterrupt__ = &kgmmuServiceNotificationInterrupt_56cd7a;

    // Hal function -- kgmmuInstBlkVaLimitGet
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_GV100;
    }
    else
    {
        pThis->__kgmmuInstBlkVaLimitGet__ = &kgmmuInstBlkVaLimitGet_f03539;
    }

    // Hal function -- kgmmuSetTlbInvalidateMembarWarParameters
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_TU102;
    }
    else
    {
        pThis->__kgmmuSetTlbInvalidateMembarWarParameters__ = &kgmmuSetTlbInvalidateMembarWarParameters_4a4dee;
    }

    // Hal function -- kgmmuSetTlbInvalidationScope
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_46f6a7;
    }
    else
    {
        pThis->__kgmmuSetTlbInvalidationScope__ = &kgmmuSetTlbInvalidationScope_GA100;
    }

    // Hal function -- kgmmuFmtInitPteComptagLine
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitPteComptagLine__ = &kgmmuFmtInitPteComptagLine_b3696a;
    }
    else
    {
        pThis->__kgmmuFmtInitPteComptagLine__ = &kgmmuFmtInitPteComptagLine_TU10X;
    }

    // Hal function -- kgmmuFmtInitPeerPteFld
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitPeerPteFld__ = &kgmmuFmtInitPeerPteFld_b3696a;
    }
    else
    {
        pThis->__kgmmuFmtInitPeerPteFld__ = &kgmmuFmtInitPeerPteFld_TU10X;
    }

    // Hal function -- kgmmuFmtInitPte
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitPte__ = &kgmmuFmtInitPte_GH10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPte__ = &kgmmuFmtInitPte_GP10X;
    }

    // Hal function -- kgmmuFmtInitPde
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitPde__ = &kgmmuFmtInitPde_GH10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPde__ = &kgmmuFmtInitPde_GP10X;
    }

    // Hal function -- kgmmuFmtIsVersionSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtIsVersionSupported__ = &kgmmuFmtIsVersionSupported_GH10X;
    }
    else
    {
        pThis->__kgmmuFmtIsVersionSupported__ = &kgmmuFmtIsVersionSupported_GP10X;
    }

    // Hal function -- kgmmuFmtInitLevels
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GH10X;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GP10X;
    }
    else
    {
        pThis->__kgmmuFmtInitLevels__ = &kgmmuFmtInitLevels_GA10X;
    }

    // Hal function -- kgmmuFmtInitPdeMulti
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtInitPdeMulti__ = &kgmmuFmtInitPdeMulti_GH10X;
    }
    else
    {
        pThis->__kgmmuFmtInitPdeMulti__ = &kgmmuFmtInitPdeMulti_GP10X;
    }

    // Hal function -- kgmmuFmtFamiliesInit
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFmtFamiliesInit__ = &kgmmuFmtFamiliesInit_GH100;
    }
    else
    {
        pThis->__kgmmuFmtFamiliesInit__ = &kgmmuFmtFamiliesInit_TU102;
    }

    // Hal function -- kgmmuTranslatePtePcfFromSw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuTranslatePtePcfFromSw__ = &kgmmuTranslatePtePcfFromSw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePtePcfFromSw__ = &kgmmuTranslatePtePcfFromSw_56cd7a;
    }

    // Hal function -- kgmmuTranslatePtePcfFromHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuTranslatePtePcfFromHw__ = &kgmmuTranslatePtePcfFromHw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePtePcfFromHw__ = &kgmmuTranslatePtePcfFromHw_56cd7a;
    }

    // Hal function -- kgmmuTranslatePdePcfFromSw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuTranslatePdePcfFromSw__ = &kgmmuTranslatePdePcfFromSw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePdePcfFromSw__ = &kgmmuTranslatePdePcfFromSw_56cd7a;
    }

    // Hal function -- kgmmuTranslatePdePcfFromHw
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuTranslatePdePcfFromHw__ = &kgmmuTranslatePdePcfFromHw_GH100;
    }
    else
    {
        pThis->__kgmmuTranslatePdePcfFromHw__ = &kgmmuTranslatePdePcfFromHw_56cd7a;
    }

    // Hal function -- kgmmuGetFaultRegisterMappings
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuGetFaultRegisterMappings__ = &kgmmuGetFaultRegisterMappings_GH100;
    }
    else
    {
        pThis->__kgmmuGetFaultRegisterMappings__ = &kgmmuGetFaultRegisterMappings_TU102;
    }

    // Hal function -- kgmmuIssueReplayableFaultBufferFlush
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuIssueReplayableFaultBufferFlush__ = &kgmmuIssueReplayableFaultBufferFlush_GH100;
    }
    else
    {
        pThis->__kgmmuIssueReplayableFaultBufferFlush__ = &kgmmuIssueReplayableFaultBufferFlush_46f6a7;
    }

    // Hal function -- kgmmuToggleFaultOnPrefetch
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuToggleFaultOnPrefetch__ = &kgmmuToggleFaultOnPrefetch_GH100;
    }
    else
    {
        pThis->__kgmmuToggleFaultOnPrefetch__ = &kgmmuToggleFaultOnPrefetch_46f6a7;
    }

    // Hal function -- kgmmuFaultBufferAllocSharedMemory
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFaultBufferAllocSharedMemory__ = &kgmmuFaultBufferAllocSharedMemory_GH100;
    }
    else
    {
        pThis->__kgmmuFaultBufferAllocSharedMemory__ = &kgmmuFaultBufferAllocSharedMemory_56cd7a;
    }

    // Hal function -- kgmmuFaultBufferFreeSharedMemory
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuFaultBufferFreeSharedMemory__ = &kgmmuFaultBufferFreeSharedMemory_GH100;
    }
    else
    {
        pThis->__kgmmuFaultBufferFreeSharedMemory__ = &kgmmuFaultBufferFreeSharedMemory_b3696a;
    }

    // Hal function -- kgmmuSetupWarForBug2720120
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_GA100;
    }
    else
    {
        pThis->__kgmmuSetupWarForBug2720120__ = &kgmmuSetupWarForBug2720120_56cd7a;
    }

    // Hal function -- kgmmuGetGraphicsEngineId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuGetGraphicsEngineId__ = &kgmmuGetGraphicsEngineId_GH100;
    }
    else
    {
        pThis->__kgmmuGetGraphicsEngineId__ = &kgmmuGetGraphicsEngineId_GV100;
    }

    // Hal function -- kgmmuReadShadowBufPutIndex
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuReadShadowBufPutIndex__ = &kgmmuReadShadowBufPutIndex_GH100;
    }
    // default
    else
    {
        pThis->__kgmmuReadShadowBufPutIndex__ = &kgmmuReadShadowBufPutIndex_4a4dee;
    }

    // Hal function -- kgmmuIsFaultEngineBar1
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuIsFaultEngineBar1__ = &kgmmuIsFaultEngineBar1_GH100;
    }
    else
    {
        pThis->__kgmmuIsFaultEngineBar1__ = &kgmmuIsFaultEngineBar1_TU102;
    }

    // Hal function -- kgmmuIsFaultEngineBar2
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuIsFaultEngineBar2__ = &kgmmuIsFaultEngineBar2_GH100;
    }
    else
    {
        pThis->__kgmmuIsFaultEngineBar2__ = &kgmmuIsFaultEngineBar2_TU102;
    }

    // Hal function -- kgmmuIsFaultEnginePhysical
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuIsFaultEnginePhysical__ = &kgmmuIsFaultEnginePhysical_GH100;
    }
    else
    {
        pThis->__kgmmuIsFaultEnginePhysical__ = &kgmmuIsFaultEnginePhysical_GV100;
    }

    // Hal function -- kgmmuCopyMmuFaults
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuCopyMmuFaults__ = &kgmmuCopyMmuFaults_GV100;
    }
    else
    {
        pThis->__kgmmuCopyMmuFaults__ = &kgmmuCopyMmuFaults_92bfc3;
    }

    // Hal function -- kgmmuParseFaultPacket
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuParseFaultPacket__ = &kgmmuParseFaultPacket_GV100;
    }
    else
    {
        pThis->__kgmmuParseFaultPacket__ = &kgmmuParseFaultPacket_92bfc3;
    }

    // Hal function -- kgmmuFaultBufferClearPackets
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferClearPackets__ = &kgmmuFaultBufferClearPackets_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferClearPackets__ = &kgmmuFaultBufferClearPackets_f2d351;
    }

    // Hal function -- kgmmuFaultBufferGetFault
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferGetFault__ = &kgmmuFaultBufferGetFault_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferGetFault__ = &kgmmuFaultBufferGetFault_dc3e6c;
    }

    // Hal function -- kgmmuCopyFaultPacketToClientShadowBuffer
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_GH100;
        }
        else
        {
            pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_GV100;
        }
    }
    else
    {
        pThis->__kgmmuCopyFaultPacketToClientShadowBuffer__ = &kgmmuCopyFaultPacketToClientShadowBuffer_13cd8d;
    }

    // Hal function -- kgmmuIsReplayableShadowFaultBufferFull
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_GH100;
        }
        // default
        else
        {
            pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_491d52;
        }
    }
    else
    {
        pThis->__kgmmuIsReplayableShadowFaultBufferFull__ = &kgmmuIsReplayableShadowFaultBufferFull_ceaee8;
    }

    // Hal function -- kgmmuReadClientShadowBufPutIndex
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_GH100;
        }
        // default
        else
        {
            pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_4a4dee;
        }
    }
    else
    {
        pThis->__kgmmuReadClientShadowBufPutIndex__ = &kgmmuReadClientShadowBufPutIndex_13cd8d;
    }

    // Hal function -- kgmmuWriteClientShadowBufPutIndex
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_GH100;
        }
        // default
        else
        {
            pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_b3696a;
        }
    }
    else
    {
        pThis->__kgmmuWriteClientShadowBufPutIndex__ = &kgmmuWriteClientShadowBufPutIndex_f2d351;
    }

    // Hal function -- kgmmuGetMinCeEngineId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuGetMinCeEngineId__ = &kgmmuGetMinCeEngineId_GH100;
    }
    else
    {
        pThis->__kgmmuGetMinCeEngineId__ = &kgmmuGetMinCeEngineId_GV100;
    }

    // Hal function -- kgmmuGetMaxCeEngineId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kgmmuGetMaxCeEngineId__ = &kgmmuGetMaxCeEngineId_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuGetMaxCeEngineId__ = &kgmmuGetMaxCeEngineId_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kgmmuGetMaxCeEngineId__ = &kgmmuGetMaxCeEngineId_AD102;
    }
    else
    {
        pThis->__kgmmuGetMaxCeEngineId__ = &kgmmuGetMaxCeEngineId_GA100;
    }

    // Hal function -- kgmmuFaultBufferMap
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferMap__ = &kgmmuFaultBufferMap_IMPL;
    }
    else
    {
        pThis->__kgmmuFaultBufferMap__ = &kgmmuFaultBufferMap_92bfc3;
    }

    // Hal function -- kgmmuFaultBufferUnmap
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferUnmap__ = &kgmmuFaultBufferUnmap_IMPL;
    }
    else
    {
        pThis->__kgmmuFaultBufferUnmap__ = &kgmmuFaultBufferUnmap_92bfc3;
    }

    // Hal function -- kgmmuFaultBufferInit
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferInit__ = &kgmmuFaultBufferInit_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferInit__ = &kgmmuFaultBufferInit_56cd7a;
    }

    // Hal function -- kgmmuFaultBufferDestroy
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferDestroy__ = &kgmmuFaultBufferDestroy_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferDestroy__ = &kgmmuFaultBufferDestroy_56cd7a;
    }

    // Hal function -- kgmmuFaultBufferLoad
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferLoad__ = &kgmmuFaultBufferLoad_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferLoad__ = &kgmmuFaultBufferLoad_ac1694;
    }

    // Hal function -- kgmmuFaultBufferUnload
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultBufferUnload__ = &kgmmuFaultBufferUnload_GV100;
    }
    else
    {
        pThis->__kgmmuFaultBufferUnload__ = &kgmmuFaultBufferUnload_ac1694;
    }

    // Hal function -- kgmmuEnableFaultBuffer
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableFaultBuffer__ = &kgmmuEnableFaultBuffer_GV100;
    }
    else
    {
        pThis->__kgmmuEnableFaultBuffer__ = &kgmmuEnableFaultBuffer_395e98;
    }

    // Hal function -- kgmmuDisableFaultBuffer
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuDisableFaultBuffer__ = &kgmmuDisableFaultBuffer_GV100;
    }
    else
    {
        pThis->__kgmmuDisableFaultBuffer__ = &kgmmuDisableFaultBuffer_92bfc3;
    }

    // Hal function -- kgmmuSetAndGetDefaultFaultBufferSize
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuSetAndGetDefaultFaultBufferSize__ = &kgmmuSetAndGetDefaultFaultBufferSize_TU102;
    }
    else
    {
        pThis->__kgmmuSetAndGetDefaultFaultBufferSize__ = &kgmmuSetAndGetDefaultFaultBufferSize_13cd8d;
    }

    // Hal function -- kgmmuReadMmuFaultInstHiLo
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuReadMmuFaultInstHiLo__ = &kgmmuReadMmuFaultInstHiLo_TU102;
    }
    else
    {
        pThis->__kgmmuReadMmuFaultInstHiLo__ = &kgmmuReadMmuFaultInstHiLo_f2d351;
    }

    // Hal function -- kgmmuReadMmuFaultAddrHiLo
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuReadMmuFaultAddrHiLo__ = &kgmmuReadMmuFaultAddrHiLo_TU102;
    }
    else
    {
        pThis->__kgmmuReadMmuFaultAddrHiLo__ = &kgmmuReadMmuFaultAddrHiLo_f2d351;
    }

    // Hal function -- kgmmuReadMmuFaultInfo
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuReadMmuFaultInfo__ = &kgmmuReadMmuFaultInfo_TU102;
    }
    else
    {
        pThis->__kgmmuReadMmuFaultInfo__ = &kgmmuReadMmuFaultInfo_a547a8;
    }

    // Hal function -- kgmmuWriteMmuFaultBufferSize
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuWriteMmuFaultBufferSize__ = &kgmmuWriteMmuFaultBufferSize_TU102;
    }
    else
    {
        pThis->__kgmmuWriteMmuFaultBufferSize__ = &kgmmuWriteMmuFaultBufferSize_f2d351;
    }

    // Hal function -- kgmmuWriteMmuFaultBufferHiLo
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuWriteMmuFaultBufferHiLo__ = &kgmmuWriteMmuFaultBufferHiLo_TU102;
    }
    else
    {
        pThis->__kgmmuWriteMmuFaultBufferHiLo__ = &kgmmuWriteMmuFaultBufferHiLo_f2d351;
    }

    // Hal function -- kgmmuEnableMmuFaultInterrupts
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableMmuFaultInterrupts__ = &kgmmuEnableMmuFaultInterrupts_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableMmuFaultInterrupts__ = &kgmmuEnableMmuFaultInterrupts_92bfc3;
    }

    // Hal function -- kgmmuDisableMmuFaultInterrupts
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuDisableMmuFaultInterrupts__ = &kgmmuDisableMmuFaultInterrupts_46f6a7;
    }
    else
    {
        pThis->__kgmmuDisableMmuFaultInterrupts__ = &kgmmuDisableMmuFaultInterrupts_92bfc3;
    }

    // Hal function -- kgmmuEnableMmuFaultOverflowIntr
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableMmuFaultOverflowIntr__ = &kgmmuEnableMmuFaultOverflowIntr_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableMmuFaultOverflowIntr__ = &kgmmuEnableMmuFaultOverflowIntr_92bfc3;
    }

    // Hal function -- kgmmuSignExtendFaultAddress
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_GH100;
        }
        else
        {
            pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_GV100;
        }
    }
    else
    {
        pThis->__kgmmuSignExtendFaultAddress__ = &kgmmuSignExtendFaultAddress_f2d351;
    }

    // Hal function -- kgmmuGetFaultType
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_GV100;
    }
    else
    {
        pThis->__kgmmuGetFaultType__ = &kgmmuGetFaultType_92bfc3;
    }

    // Hal function -- kgmmuIsP2PUnboundInstFault
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_491d52;
        }
        else
        {
            pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_GA100;
        }
    }
    else
    {
        pThis->__kgmmuIsP2PUnboundInstFault__ = &kgmmuIsP2PUnboundInstFault_92bfc3;
    }

    // Hal function -- kgmmuServiceVfPriFaults
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceVfPriFaults__ = &kgmmuServiceVfPriFaults_IMPL;
    }
    else
    {
        pThis->__kgmmuServiceVfPriFaults__ = &kgmmuServiceVfPriFaults_92bfc3;
    }

    // Hal function -- kgmmuTestVidmemAccessBitBufferError
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuTestVidmemAccessBitBufferError__ = &kgmmuTestVidmemAccessBitBufferError_491d52;
    }
    else
    {
        pThis->__kgmmuTestVidmemAccessBitBufferError__ = &kgmmuTestVidmemAccessBitBufferError_ceaee8;
    }

    // Hal function -- kgmmuDisableVidmemAccessBitBuf
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuDisableVidmemAccessBitBuf__ = &kgmmuDisableVidmemAccessBitBuf_b3696a;
    }
    else
    {
        pThis->__kgmmuDisableVidmemAccessBitBuf__ = &kgmmuDisableVidmemAccessBitBuf_e426af;
    }

    // Hal function -- kgmmuEnableVidmemAccessBitBuf
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuEnableVidmemAccessBitBuf__ = &kgmmuEnableVidmemAccessBitBuf_46f6a7;
    }
    else
    {
        pThis->__kgmmuEnableVidmemAccessBitBuf__ = &kgmmuEnableVidmemAccessBitBuf_92bfc3;
    }

    // Hal function -- kgmmuClearAccessCounterWriteNak
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuClearAccessCounterWriteNak__ = &kgmmuClearAccessCounterWriteNak_b3696a;
    }
    else
    {
        pThis->__kgmmuClearAccessCounterWriteNak__ = &kgmmuClearAccessCounterWriteNak_e426af;
    }

    // Hal function -- kgmmuServiceMthdBuffFaultInBar2Fault
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceMthdBuffFaultInBar2Fault__ = &kgmmuServiceMthdBuffFaultInBar2Fault_56cd7a;
    }
    else
    {
        pThis->__kgmmuServiceMthdBuffFaultInBar2Fault__ = &kgmmuServiceMthdBuffFaultInBar2Fault_92bfc3;
    }

    // Hal function -- kgmmuFaultCancelTargeted
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultCancelTargeted__ = &kgmmuFaultCancelTargeted_VF;
    }
    else
    {
        pThis->__kgmmuFaultCancelTargeted__ = &kgmmuFaultCancelTargeted_92bfc3;
    }

    // Hal function -- kgmmuFaultCancelIssueInvalidate
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuFaultCancelIssueInvalidate__ = &kgmmuFaultCancelIssueInvalidate_GP100;
    }
    else
    {
        pThis->__kgmmuFaultCancelIssueInvalidate__ = &kgmmuFaultCancelIssueInvalidate_92bfc3;
    }

    // Hal function -- kgmmuServiceMmuFault
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kgmmuServiceMmuFault__ = &kgmmuServiceMmuFault_GV100;
    }
    else
    {
        pThis->__kgmmuServiceMmuFault__ = &kgmmuServiceMmuFault_GA100;
    }

    // Hal function -- kgmmuServiceUnboundInstBlockFault
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kgmmuServiceUnboundInstBlockFault__ = &kgmmuServiceUnboundInstBlockFault_56cd7a;
    }
    else
    {
        pThis->__kgmmuServiceUnboundInstBlockFault__ = &kgmmuServiceUnboundInstBlockFault_92bfc3;
    }

    // Hal function -- kgmmuGetEccCounts
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000420UL) )) /* ChipHal: TU102 | GA100 | GH100 */ 
    {
        pThis->__kgmmuGetEccCounts__ = &kgmmuGetEccCounts_TU102;
    }
    // default
    else
    {
        pThis->__kgmmuGetEccCounts__ = &kgmmuGetEccCounts_4a4dee;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelGmmu_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelGmmu_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelGmmu_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelGmmu_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelGmmu_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_KernelGmmu_engstateStatePreUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelGmmu_engstateStateDestroy;

    pThis->__nvoc_base_IntrService.__intrservRegisterIntrService__ = &__nvoc_thunk_KernelGmmu_intrservRegisterIntrService;

    pThis->__nvoc_base_IntrService.__intrservClearInterrupt__ = &__nvoc_thunk_KernelGmmu_intrservClearInterrupt;

    pThis->__nvoc_base_IntrService.__intrservServiceInterrupt__ = &__nvoc_thunk_KernelGmmu_intrservServiceInterrupt;

    pThis->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__ = &__nvoc_thunk_KernelGmmu_intrservServiceNotificationInterrupt;

    pThis->__kgmmuStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreLoad;

    pThis->__kgmmuStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePostUnload;

    pThis->__kgmmuStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStateInitUnlocked;

    pThis->__kgmmuInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kgmmuInitMissing;

    pThis->__kgmmuStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitLocked;

    pThis->__kgmmuStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kgmmuStatePreInitUnlocked;

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

NV_STATUS __nvoc_objCreate_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelGmmu *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelGmmu), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelGmmu));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelGmmu);

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

    __nvoc_init_KernelGmmu(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelGmmu(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelGmmu_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelGmmu_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelGmmu));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelGmmu(KernelGmmu **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelGmmu(ppThis, pParent, createFlags);

    return status;
}

