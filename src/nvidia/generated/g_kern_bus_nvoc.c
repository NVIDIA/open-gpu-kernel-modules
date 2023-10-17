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

static NV_STATUS __nvoc_thunk_KernelBus_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, ENGDESCRIPTOR arg0) {
    return kbusConstructEngine(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStatePreInitLocked(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStateInitLocked(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePreLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStatePreLoad(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStateLoad(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStatePostLoad(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg0) {
    return kbusStatePreUnload(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags) {
    return kbusStateUnload(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), flags);
}

static NV_STATUS __nvoc_thunk_KernelBus_engstateStatePostUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags) {
    return kbusStatePostUnload(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelBus_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    kbusStateDestroy(pGpu, (struct KernelBus *)(((unsigned char *)pKernelBus) - __nvoc_rtti_KernelBus_OBJENGSTATE.offset));
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->bFlaDummyPageEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bFlaDummyPageEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bP2pMailboxClientAllocatedBug3466714VoltaAndUp
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bP2pMailboxClientAllocatedBug3466714VoltaAndUp = ((NvBool)(0 == 0));
    }

    // Hal field -- bBug2751296LimitBar2PtSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bBug2751296LimitBar2PtSize = ((NvBool)(0 == 0));
    }

    // Hal field -- bAllowReflectedMappingAccess
    // default
    {
        pThis->bAllowReflectedMappingAccess = ((NvBool)(0 != 0));
    }

    // Hal field -- bIsEntireBar2RegionVirtuallyAddressible
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bIsEntireBar2RegionVirtuallyAddressible = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bIsEntireBar2RegionVirtuallyAddressible = ((NvBool)(0 != 0));
    }

    // Hal field -- bSkipBar2TestOnGc6Exit
    // default
    {
        pThis->bSkipBar2TestOnGc6Exit = ((NvBool)(0 != 0));
    }

    // Hal field -- bReadCpuPointerToFlush
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bReadCpuPointerToFlush = ((NvBool)(0 == 0));
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, ((NvBool)(0 != 0)));
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
    pThis->__kbusStatePreInitLocked__ = &kbusStatePreInitLocked_GM107;

    pThis->__kbusStateInitLocked__ = &kbusStateInitLocked_IMPL;

    // Hal function -- kbusStatePreLoad
    pThis->__kbusStatePreLoad__ = &kbusStatePreLoad_GM107;

    // Hal function -- kbusStateLoad
    pThis->__kbusStateLoad__ = &kbusStateLoad_GM107;

    // Hal function -- kbusStatePostLoad
    pThis->__kbusStatePostLoad__ = &kbusStatePostLoad_GM107;

    // Hal function -- kbusStatePreUnload
    pThis->__kbusStatePreUnload__ = &kbusStatePreUnload_GM107;

    // Hal function -- kbusStateUnload
    pThis->__kbusStateUnload__ = &kbusStateUnload_GM107;

    // Hal function -- kbusStatePostUnload
    pThis->__kbusStatePostUnload__ = &kbusStatePostUnload_GM107;

    // Hal function -- kbusStateDestroy
    pThis->__kbusStateDestroy__ = &kbusStateDestroy_GM107;

    // Hal function -- kbusTeardownBar2CpuAperture
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusTeardownBar2CpuAperture__ = &kbusTeardownBar2CpuAperture_GH100;
    }
    else
    {
        pThis->__kbusTeardownBar2CpuAperture__ = &kbusTeardownBar2CpuAperture_GM107;
    }

    // Hal function -- kbusGetP2PMailboxAttributes
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetP2PMailboxAttributes__ = &kbusGetP2PMailboxAttributes_GH100;
    }
    else
    {
        pThis->__kbusGetP2PMailboxAttributes__ = &kbusGetP2PMailboxAttributes_GM200;
    }

    // Hal function -- kbusCreateP2PMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusCreateP2PMapping__ = &kbusCreateP2PMapping_GH100;
    }
    else
    {
        pThis->__kbusCreateP2PMapping__ = &kbusCreateP2PMapping_GP100;
    }

    // Hal function -- kbusRemoveP2PMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusRemoveP2PMapping__ = &kbusRemoveP2PMapping_GH100;
    }
    else
    {
        pThis->__kbusRemoveP2PMapping__ = &kbusRemoveP2PMapping_GP100;
    }

    // Hal function -- kbusGetEgmPeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetEgmPeerId__ = &kbusGetEgmPeerId_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetEgmPeerId__ = &kbusGetEgmPeerId_56cd7a;
    }

    // Hal function -- kbusGetPeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetPeerId__ = &kbusGetPeerId_GH100;
    }
    else
    {
        pThis->__kbusGetPeerId__ = &kbusGetPeerId_GP100;
    }

    // Hal function -- kbusGetNvSwitchPeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__kbusGetNvSwitchPeerId__ = &kbusGetNvSwitchPeerId_GA100;
    }
    // default
    else
    {
        pThis->__kbusGetNvSwitchPeerId__ = &kbusGetNvSwitchPeerId_c732fb;
    }

    // Hal function -- kbusGetUnusedPciePeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_TU102;
    }
    else
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_GM107;
    }

    // Hal function -- kbusIsPeerIdValid
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusIsPeerIdValid__ = &kbusIsPeerIdValid_GH100;
    }
    else
    {
        pThis->__kbusIsPeerIdValid__ = &kbusIsPeerIdValid_GP100;
    }

    // Hal function -- kbusGetNvlinkP2PPeerId
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_56cd7a;
    }
    else
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_GA100;
    }

    // Hal function -- kbusWriteP2PWmbTag
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusWriteP2PWmbTag__ = &kbusWriteP2PWmbTag_GH100;
    }
    else
    {
        pThis->__kbusWriteP2PWmbTag__ = &kbusWriteP2PWmbTag_GM200;
    }

    // Hal function -- kbusSetupP2PDomainAccess
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusSetupP2PDomainAccess__ = &kbusSetupP2PDomainAccess_GH100;
    }
    else
    {
        pThis->__kbusSetupP2PDomainAccess__ = &kbusSetupP2PDomainAccess_GM200;
    }

    // Hal function -- kbusNeedWarForBug999673
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusNeedWarForBug999673__ = &kbusNeedWarForBug999673_491d52;
    }
    else
    {
        pThis->__kbusNeedWarForBug999673__ = &kbusNeedWarForBug999673_GM200;
    }

    // Hal function -- kbusCreateP2PMappingForC2C
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusCreateP2PMappingForC2C__ = &kbusCreateP2PMappingForC2C_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateP2PMappingForC2C__ = &kbusCreateP2PMappingForC2C_46f6a7;
    }

    // Hal function -- kbusRemoveP2PMappingForC2C
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusRemoveP2PMappingForC2C__ = &kbusRemoveP2PMappingForC2C_GH100;
    }
    // default
    else
    {
        pThis->__kbusRemoveP2PMappingForC2C__ = &kbusRemoveP2PMappingForC2C_46f6a7;
    }

    // Hal function -- kbusUnreserveP2PPeerIds
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_46f6a7;
    }
    else
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_GP100;
    }

    // Hal function -- kbusIsBar1P2PCapable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusIsBar1P2PCapable__ = &kbusIsBar1P2PCapable_GH100;
    }
    // default
    else
    {
        pThis->__kbusIsBar1P2PCapable__ = &kbusIsBar1P2PCapable_bf6dfa;
    }

    // Hal function -- kbusEnableStaticBar1Mapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusEnableStaticBar1Mapping__ = &kbusEnableStaticBar1Mapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusEnableStaticBar1Mapping__ = &kbusEnableStaticBar1Mapping_395e98;
    }

    // Hal function -- kbusDisableStaticBar1Mapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusDisableStaticBar1Mapping__ = &kbusDisableStaticBar1Mapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusDisableStaticBar1Mapping__ = &kbusDisableStaticBar1Mapping_d44104;
    }

    // Hal function -- kbusGetBar1P2PDmaInfo
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetBar1P2PDmaInfo__ = &kbusGetBar1P2PDmaInfo_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetBar1P2PDmaInfo__ = &kbusGetBar1P2PDmaInfo_395e98;
    }

    // Hal function -- kbusUpdateStaticBar1VAMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusUpdateStaticBar1VAMapping__ = &kbusUpdateStaticBar1VAMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusUpdateStaticBar1VAMapping__ = &kbusUpdateStaticBar1VAMapping_395e98;
    }

    // Hal function -- kbusGetStaticFbAperture
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetStaticFbAperture__ = &kbusGetStaticFbAperture_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetStaticFbAperture__ = &kbusGetStaticFbAperture_395e98;
    }

    // Hal function -- kbusCreateP2PMappingForBar1P2P
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusCreateP2PMappingForBar1P2P__ = &kbusCreateP2PMappingForBar1P2P_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateP2PMappingForBar1P2P__ = &kbusCreateP2PMappingForBar1P2P_395e98;
    }

    // Hal function -- kbusRemoveP2PMappingForBar1P2P
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusRemoveP2PMappingForBar1P2P__ = &kbusRemoveP2PMappingForBar1P2P_GH100;
    }
    // default
    else
    {
        pThis->__kbusRemoveP2PMappingForBar1P2P__ = &kbusRemoveP2PMappingForBar1P2P_395e98;
    }

    // Hal function -- kbusHasPcieBar1P2PMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusHasPcieBar1P2PMapping__ = &kbusHasPcieBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusHasPcieBar1P2PMapping__ = &kbusHasPcieBar1P2PMapping_bf6dfa;
    }

    // Hal function -- kbusIsPcieBar1P2PMappingSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusIsPcieBar1P2PMappingSupported__ = &kbusIsPcieBar1P2PMappingSupported_GH100;
    }
    // default
    else
    {
        pThis->__kbusIsPcieBar1P2PMappingSupported__ = &kbusIsPcieBar1P2PMappingSupported_bf6dfa;
    }

    // Hal function -- kbusCheckFlaSupportedAndInit
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusCheckFlaSupportedAndInit__ = &kbusCheckFlaSupportedAndInit_GA100;
    }
    else
    {
        pThis->__kbusCheckFlaSupportedAndInit__ = &kbusCheckFlaSupportedAndInit_ac1694;
    }

    // Hal function -- kbusDetermineFlaRangeAndAllocate
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_GH100;
    }
    // default
    else
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_395e98;
    }

    // Hal function -- kbusAllocateFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_GH100;
    }
    // default
    else
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_395e98;
    }

    // Hal function -- kbusGetFlaRange
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_395e98;
    }

    // Hal function -- kbusAllocateLegacyFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusAllocateLegacyFlaVaspace__ = &kbusAllocateLegacyFlaVaspace_GA100;
    }
    else
    {
        pThis->__kbusAllocateLegacyFlaVaspace__ = &kbusAllocateLegacyFlaVaspace_395e98;
    }

    // Hal function -- kbusAllocateHostManagedFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_GA100;
    }
    else
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_395e98;
    }

    // Hal function -- kbusDestroyFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_GH100;
    }
    // default
    else
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_d44104;
    }

    // Hal function -- kbusGetFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_GA100;
    }
    else
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_395e98;
    }

    // Hal function -- kbusDestroyHostManagedFlaVaspace
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_GA100;
    }
    else
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_d44104;
    }

    // Hal function -- kbusVerifyFlaRange
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_GA100;
    }
    else
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_bf6dfa;
    }

    // Hal function -- kbusConstructFlaInstBlk
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_GA100;
    }
    else
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_395e98;
    }

    // Hal function -- kbusDestructFlaInstBlk
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_GA100;
    }
    else
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_d44104;
    }

    // Hal function -- kbusValidateFlaBaseAddress
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_GA100;
    }
    else
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_395e98;
    }

    // Hal function -- kbusSetupUnbindFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_GH100;
    }
    else
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_46f6a7;
    }

    // Hal function -- kbusSetupBindFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_GH100;
    }
    else
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_46f6a7;
    }

    // Hal function -- kbusCacheBAR1ResizeSize_WAR_BUG_3249028
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_d44104;
    }
    else
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GA100;
    }

    // Hal function -- kbusRestoreBAR1ResizeSize_WAR_BUG_3249028
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_ac1694;
    }
    else
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GA100;
    }

    // Hal function -- kbusIsDirectMappingAllowed
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GM107;
    }
    else
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GA100;
    }

    // Hal function -- kbusUseDirectSysmemMap
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GM107;
    }
    else
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GA100;
    }

    // Hal function -- kbusWriteBAR0WindowBase
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_GH100;
    }
    else
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_395e98;
    }

    // Hal function -- kbusReadBAR0WindowBase
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_GH100;
    }
    else
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_13cd8d;
    }

    // Hal function -- kbusValidateBAR0WindowBase
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_GH100;
    }
    else
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_ceaee8;
    }

    // Hal function -- kbusSetBAR0WindowVidOffset
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusSetBAR0WindowVidOffset__ = &kbusSetBAR0WindowVidOffset_GH100;
    }
    else
    {
        pThis->__kbusSetBAR0WindowVidOffset__ = &kbusSetBAR0WindowVidOffset_GM107;
    }

    // Hal function -- kbusGetBAR0WindowVidOffset
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetBAR0WindowVidOffset__ = &kbusGetBAR0WindowVidOffset_GH100;
    }
    else
    {
        pThis->__kbusGetBAR0WindowVidOffset__ = &kbusGetBAR0WindowVidOffset_GM107;
    }

    // Hal function -- kbusVerifyBar2
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_GH100;
    }
    else
    {
        pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_GM107;
    }

    // Hal function -- kbusVerifyCoherentLink
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusVerifyCoherentLink__ = &kbusVerifyCoherentLink_GH100;
    }
    // default
    else
    {
        pThis->__kbusVerifyCoherentLink__ = &kbusVerifyCoherentLink_56cd7a;
    }

    // Hal function -- kbusFlushPcieForBar0Doorbell
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusFlushPcieForBar0Doorbell__ = &kbusFlushPcieForBar0Doorbell_GH100;
    }
    else
    {
        pThis->__kbusFlushPcieForBar0Doorbell__ = &kbusFlushPcieForBar0Doorbell_56cd7a;
    }

    // Hal function -- kbusCreateCoherentCpuMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusCreateCoherentCpuMapping__ = &kbusCreateCoherentCpuMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateCoherentCpuMapping__ = &kbusCreateCoherentCpuMapping_46f6a7;
    }

    // Hal function -- kbusMapCoherentCpuMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusMapCoherentCpuMapping__ = &kbusMapCoherentCpuMapping_GV100;
    }
    // default
    else
    {
        pThis->__kbusMapCoherentCpuMapping__ = &kbusMapCoherentCpuMapping_9e2234;
    }

    // Hal function -- kbusUnmapCoherentCpuMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusUnmapCoherentCpuMapping__ = &kbusUnmapCoherentCpuMapping_GV100;
    }
    // default
    else
    {
        pThis->__kbusUnmapCoherentCpuMapping__ = &kbusUnmapCoherentCpuMapping_d44104;
    }

    // Hal function -- kbusTeardownCoherentCpuMapping
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusTeardownCoherentCpuMapping__ = &kbusTeardownCoherentCpuMapping_GV100;
    }
    // default
    else
    {
        pThis->__kbusTeardownCoherentCpuMapping__ = &kbusTeardownCoherentCpuMapping_d44104;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelBus_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_thunk_KernelBus_engstateStatePreInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelBus_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__ = &__nvoc_thunk_KernelBus_engstateStatePreLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelBus_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelBus_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_KernelBus_engstateStatePreUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelBus_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__ = &__nvoc_thunk_KernelBus_engstateStatePostUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelBus_engstateStateDestroy;

    pThis->__kbusStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbusStateInitUnlocked;

    pThis->__kbusInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kbusInitMissing;

    pThis->__kbusStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kbusStatePreInitUnlocked;

    pThis->__kbusIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kbusIsPresent;
}

void __nvoc_init_funcTable_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelBus_1(pThis, pRmhalspecowner);
}

NvU32 kbusGetP2PWriteMailboxAddressSize_STATIC_DISPATCH(struct OBJGPU *pGpu) {
    ChipHal *chipHal = &staticCast(pGpu, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;

    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        return kbusGetP2PWriteMailboxAddressSize_GH100(pGpu);
    }
    else
    {
        return kbusGetP2PWriteMailboxAddressSize_474d46(pGpu);
    }

    NV_ASSERT_FAILED("No hal impl found for kbusGetP2PWriteMailboxAddressSize");

    return 0;
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

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelBus), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(KernelBus));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelBus);

    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.createFlags = createFlags;

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
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelBus));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBus(KernelBus **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBus(ppThis, pParent, createFlags);

    return status;
}

