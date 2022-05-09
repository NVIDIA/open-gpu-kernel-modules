#define NVOC_KERN_BUS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_bus_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd2ac57 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBus;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelBus(KernelBus*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelBus(KernelBus*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelBus(KernelBus*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelBus(KernelBus*, RmHalspecOwner* );
void __nvoc_dtor_KernelBus(KernelBus*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelBus;

static const struct NVOC_RTTI __nvoc_rtti_KernelBus_KernelBus = {
    /*pClassDef=*/          &__nvoc_class_def_KernelBus,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelBus,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelBus_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelBus_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelBus = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelBus_KernelBus,
        &__nvoc_rtti_KernelBus_OBJENGSTATE,
        &__nvoc_rtti_KernelBus_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBus = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelBus),
        /*classId=*/            classId(KernelBus),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelBus",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelBus,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelBus,
    /*pExportInfo=*/        &__nvoc_export_info_KernelBus
};

static NV_STATUS __nvoc_thunk_KernelBus_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, ENGDESCRIPTOR arg0) {
    return kbusConstructEngine(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStatePreInitLocked(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStateInitLocked(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStatePostLoad(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStatePreUnload(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags) {
    return kbusStateUnload(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelBus_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    kbusStateDestroy(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusReconcileTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusStateLoad(POBJGPU pGpu, struct KernelBus *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusStatePreLoad(POBJGPU pGpu, struct KernelBus *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusStatePostUnload(POBJGPU pGpu, struct KernelBus *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusStateInitUnlocked(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kbusInitMissing(POBJGPU pGpu, struct KernelBus *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusStatePreInitUnlocked(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusGetTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusCompareTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kbusFreeTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusAllocTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kbusSetTunableState(POBJGPU pGpu, struct KernelBus *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kbusIsPresent(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelBus = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelBus(KernelBus *pThis) {
    __nvoc_kbusDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bFlaDummyPageEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bFlaDummyPageEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bFlaDummyPageEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bP2pMailboxClientAllocatedBug3466714VoltaAndUp
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bP2pMailboxClientAllocatedBug3466714VoltaAndUp = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bP2pMailboxClientAllocatedBug3466714VoltaAndUp = ((NvBool)(0 != 0));
    }

    // Hal field -- bBug2751296LimitBar2PtSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bBug2751296LimitBar2PtSize = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBug2751296LimitBar2PtSize = ((NvBool)(0 != 0));
    }

    // Hal field -- bAllowReflectedMappingAccess
    if (0)
    {
    }
    // default
    else
    {
        pThis->bAllowReflectedMappingAccess = ((NvBool)(0 != 0));
    }

    // Hal field -- bBar2Tunnelled
    if (0)
    {
    }
    // default
    else
    {
        pThis->bBar2Tunnelled = ((NvBool)(0 != 0));
    }

    // Hal field -- bBar2InternalOnly
    if (0)
    {
    }
    else if (0)
    {
    }
    // default
    else
    {
        pThis->bBar2InternalOnly = ((NvBool)(0 != 0));
    }

    // Hal field -- bReadCpuPointerToFlush
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bReadCpuPointerToFlush = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bReadCpuPointerToFlush = ((NvBool)(0 != 0));
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_IS_MISSING
    if (0)
    {
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_IS_MISSING, ((NvBool)(0 != 0)));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelBus_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelBus(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelBus_exit; // Success

__nvoc_ctor_KernelBus_fail_OBJENGSTATE:
__nvoc_ctor_KernelBus_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelBus_1(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kbusConstructEngine__ = &kbusConstructEngine_IMPL;

    // Hal function -- kbusStatePreInitLocked
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusStatePreInitLocked__ = &kbusStatePreInitLocked_GM107;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    pThis->__kbusStateInitLocked__ = &kbusStateInitLocked_IMPL;

    // Hal function -- kbusStatePostLoad
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusStatePostLoad__ = &kbusStatePostLoad_GM107;
    }
    else if (0)
    {
    }

    // Hal function -- kbusStatePreUnload
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusStatePreUnload__ = &kbusStatePreUnload_GM107;
    }
    else if (0)
    {
    }

    // Hal function -- kbusStateUnload
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kbusStateUnload__ = &kbusStateUnload_GM107;
        }
        else if (0)
        {
        }
    }

    // Hal function -- kbusStateDestroy
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusStateDestroy__ = &kbusStateDestroy_GM107;
    }
    else if (0)
    {
    }

    // Hal function -- kbusGetUnusedPciePeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_TU102;
    }

    // Hal function -- kbusGetNvlinkP2PPeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_56cd7a;
    }

    // Hal function -- kbusRemoveNvlinkPeerMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusRemoveNvlinkPeerMapping__ = &kbusRemoveNvlinkPeerMapping_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusRemoveNvlinkPeerMapping__ = &kbusRemoveNvlinkPeerMapping_56cd7a;
    }

    // Hal function -- kbusUnreserveP2PPeerIds
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_46f6a7;
    }

    // Hal function -- kbusAllocateFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_395e98;
    }

    // Hal function -- kbusAllocateHostManagedFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_395e98;
    }

    // Hal function -- kbusInitFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusInitFla__ = &kbusInitFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusInitFla__ = &kbusInitFla_ac1694;
    }

    // Hal function -- kbusGetFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_395e98;
    }

    // Hal function -- kbusDestroyFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_d44104;
    }

    // Hal function -- kbusDestroyHostManagedFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_d44104;
    }

    // Hal function -- kbusVerifyFlaRange
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_bf6dfa;
    }

    // Hal function -- kbusConstructFlaInstBlk
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_395e98;
    }

    // Hal function -- kbusDestructFlaInstBlk
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_d44104;
    }

    // Hal function -- kbusValidateFlaBaseAddress
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_395e98;
    }

    // Hal function -- kbusIsDirectMappingAllowed
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GA100;
    }

    // Hal function -- kbusUseDirectSysmemMap
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GA100;
    }
    else if (0)
    {
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelBus_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_thunk_KernelBus_engstateStatePreInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelBus_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelBus_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_KernelBus_engstateStatePreUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelBus_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelBus_engstateStateDestroy;

    pThis->__kbusReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusReconcileTunableState;

    pThis->__kbusStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kbusStateLoad;

    pThis->__kbusStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_kbusStatePreLoad;

    pThis->__kbusStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kbusStatePostUnload;

    pThis->__kbusStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbusStateInitUnlocked;

    pThis->__kbusInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kbusInitMissing;

    pThis->__kbusStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbusStatePreInitUnlocked;

    pThis->__kbusGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusGetTunableState;

    pThis->__kbusCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusCompareTunableState;

    pThis->__kbusFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusFreeTunableState;

    pThis->__kbusAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusAllocTunableState;

    pThis->__kbusSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kbusSetTunableState;

    pThis->__kbusIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kbusIsPresent;
}

void __nvoc_init_funcTable_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelBus_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelBus = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelBus(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelBus(KernelBus **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelBus *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelBus));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelBus));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelBus);

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

    __nvoc_init_KernelBus(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelBus(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelBus_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelBus_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBus(KernelBus **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBus(ppThis, pParent, createFlags);

    return status;
}

