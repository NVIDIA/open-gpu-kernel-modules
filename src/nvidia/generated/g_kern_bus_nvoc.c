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

// Down-thunk(s) to bridge KernelBus methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelBus_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags);    // this
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePostUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags);    // this
void __nvoc_down_thunk_KernelBus_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus);    // this

// 10 down-thunk(s) defined to bridge methods in KernelBus from superclasses

// kbusConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBus_engstateConstructEngine(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, ENGDESCRIPTOR arg3) {
    return kbusConstructEngine(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbusStatePreInitLocked: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStatePreInitLocked(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}

// kbusStateInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateInitLocked(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    return kbusStateInitLocked(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}

// kbusStatePreLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3) {
    return kbusStatePreLoad(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbusStateLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3) {
    return kbusStateLoad(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbusStatePostLoad: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePostLoad(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3) {
    return kbusStatePostLoad(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbusStatePreUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePreUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 arg3) {
    return kbusStatePreUnload(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), arg3);
}

// kbusStateUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStateUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags) {
    return kbusStateUnload(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), flags);
}

// kbusStatePostUnload: virtual halified (singleton optimized) override (engstate) base (engstate) body
NV_STATUS __nvoc_down_thunk_KernelBus_engstateStatePostUnload(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus, NvU32 flags) {
    return kbusStatePostUnload(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)), flags);
}

// kbusStateDestroy: virtual halified (singleton optimized) override (engstate) base (engstate)
void __nvoc_down_thunk_KernelBus_engstateStateDestroy(struct OBJGPU *pGpu, struct OBJENGSTATE *pKernelBus) {
    kbusStateDestroy(pGpu, (struct KernelBus *)(((unsigned char *) pKernelBus) - NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}


// Up-thunk(s) to bridge KernelBus methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_kbusInitMissing(struct OBJGPU *pGpu, struct KernelBus *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbusStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBus *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbusStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBus *pEngstate);    // this
NvBool __nvoc_up_thunk_OBJENGSTATE_kbusIsPresent(struct OBJGPU *pGpu, struct KernelBus *pEngstate);    // this

// 4 up-thunk(s) defined to bridge methods in KernelBus to superclasses

// kbusInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_kbusInitMissing(struct OBJGPU *pGpu, struct KernelBus *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}

// kbusStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbusStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelBus *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}

// kbusStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kbusStateInitUnlocked(struct OBJGPU *pGpu, struct KernelBus *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
}

// kbusIsPresent: virtual inherited (engstate) base (engstate)
NvBool __nvoc_up_thunk_OBJENGSTATE_kbusIsPresent(struct OBJGPU *pGpu, struct KernelBus *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelBus, __nvoc_base_OBJENGSTATE)));
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
        pThis->bFlaDummyPageEnabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bFlaDummyPageEnabled = NV_FALSE;
    }

    // Hal field -- bP2pMailboxClientAllocatedBug3466714VoltaAndUp
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bP2pMailboxClientAllocatedBug3466714VoltaAndUp = NV_TRUE;
    }

    // Hal field -- bBug2751296LimitBar2PtSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bBug2751296LimitBar2PtSize = NV_TRUE;
    }

    // Hal field -- bAllowReflectedMappingAccess
    // default
    {
        pThis->bAllowReflectedMappingAccess = NV_FALSE;
    }

    // Hal field -- bIsEntireBar2RegionVirtuallyAddressible
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bIsEntireBar2RegionVirtuallyAddressible = NV_TRUE;
    }
    // default
    else
    {
        pThis->bIsEntireBar2RegionVirtuallyAddressible = NV_FALSE;
    }

    // Hal field -- bSkipBar2TestOnGc6Exit
    // default
    {
        pThis->bSkipBar2TestOnGc6Exit = NV_FALSE;
    }

    // Hal field -- bReadCpuPointerToFlush
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->bReadCpuPointerToFlush = NV_TRUE;
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_IS_MISSING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR, NV_FALSE);
    }

    // Hal field -- bBar1Disabled
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->bBar1Disabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bBar1Disabled = NV_FALSE;
    }

    // Hal field -- bCpuVisibleBar2Disabled
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->bCpuVisibleBar2Disabled = NV_TRUE;
    }
    // default
    else
    {
        pThis->bCpuVisibleBar2Disabled = NV_FALSE;
    }

    // Hal field -- bBar1DiscontigEnabled
    // default
    {
        pThis->bBar1DiscontigEnabled = NV_TRUE;
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

// Vtable initialization
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

    // kbusInitBarsSize -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusInitBarsSize__ = &kbusInitBarsSize_VF;
    }
    else
    {
        pThis->__kbusInitBarsSize__ = &kbusInitBarsSize_KERNEL;
    }

    // kbusBar2IsReady -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusBar2IsReady__ = &kbusBar2IsReady_VBAR2_SRIOV;
    }
    else
    {
        pThis->__kbusBar2IsReady__ = &kbusBar2IsReady_VBAR2;
    }

    // kbusMapBar2Aperture -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusMapBar2Aperture__ = &kbusMapBar2Aperture_VBAR2_SRIOV;
    }
    else
    {
        pThis->__kbusMapBar2Aperture__ = &kbusMapBar2Aperture_VBAR2;
    }

    // kbusValidateBar2ApertureMapping -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusValidateBar2ApertureMapping__ = &kbusValidateBar2ApertureMapping_VBAR2_SRIOV;
    }
    else
    {
        pThis->__kbusValidateBar2ApertureMapping__ = &kbusValidateBar2ApertureMapping_VBAR2;
    }

    // kbusUnmapBar2ApertureWithFlags -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusUnmapBar2ApertureWithFlags__ = &kbusUnmapBar2ApertureWithFlags_VBAR2_SRIOV;
    }
    else
    {
        pThis->__kbusUnmapBar2ApertureWithFlags__ = &kbusUnmapBar2ApertureWithFlags_VBAR2;
    }

    // kbusGetVaLimitForBar2 -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusGetVaLimitForBar2__ = &kbusGetVaLimitForBar2_IMPL;
    }
    else
    {
        pThis->__kbusGetVaLimitForBar2__ = &kbusGetVaLimitForBar2_FWCLIENT;
    }

    // kbusCalcCpuInvisibleBar2Range -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusCalcCpuInvisibleBar2Range__ = &kbusCalcCpuInvisibleBar2Range_GP100;
    }
    else
    {
        pThis->__kbusCalcCpuInvisibleBar2Range__ = &kbusCalcCpuInvisibleBar2Range_f2d351;
    }

    // kbusCalcCpuInvisibleBar2ApertureSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusCalcCpuInvisibleBar2ApertureSize__ = &kbusCalcCpuInvisibleBar2ApertureSize_GV100;
    }
    else
    {
        pThis->__kbusCalcCpuInvisibleBar2ApertureSize__ = &kbusCalcCpuInvisibleBar2ApertureSize_13cd8d;
    }

    // kbusCommitBar2 -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusCommitBar2__ = &kbusCommitBar2_GM107;
    }
    else
    {
        pThis->__kbusCommitBar2__ = &kbusCommitBar2_KERNEL;
    }

    // kbusRewritePTEsForExistingMapping -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusRewritePTEsForExistingMapping__ = &kbusRewritePTEsForExistingMapping_VBAR2;
    }
    else
    {
        pThis->__kbusRewritePTEsForExistingMapping__ = &kbusRewritePTEsForExistingMapping_92bfc3;
    }

    // kbusPatchBar1Pdb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusPatchBar1Pdb__ = &kbusPatchBar1Pdb_56cd7a;
    }
    else
    {
        pThis->__kbusPatchBar1Pdb__ = &kbusPatchBar1Pdb_GSPCLIENT;
    }

    // kbusPatchBar2Pdb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusPatchBar2Pdb__ = &kbusPatchBar2Pdb_56cd7a;
    }
    else
    {
        pThis->__kbusPatchBar2Pdb__ = &kbusPatchBar2Pdb_GSPCLIENT;
    }

    // kbusConstructVirtualBar2CpuInvisibleHeap -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusConstructVirtualBar2CpuInvisibleHeap__ = &kbusConstructVirtualBar2CpuInvisibleHeap_VBAR2;
    }
    else
    {
        pThis->__kbusConstructVirtualBar2CpuInvisibleHeap__ = &kbusConstructVirtualBar2CpuInvisibleHeap_56cd7a;
    }

    // kbusMapCpuInvisibleBar2Aperture -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusMapCpuInvisibleBar2Aperture__ = &kbusMapCpuInvisibleBar2Aperture_VBAR2;
    }
    else
    {
        pThis->__kbusMapCpuInvisibleBar2Aperture__ = &kbusMapCpuInvisibleBar2Aperture_46f6a7;
    }

    // kbusUnmapCpuInvisibleBar2Aperture -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusUnmapCpuInvisibleBar2Aperture__ = &kbusUnmapCpuInvisibleBar2Aperture_VBAR2;
    }
    else
    {
        pThis->__kbusUnmapCpuInvisibleBar2Aperture__ = &kbusUnmapCpuInvisibleBar2Aperture_b3696a;
    }

    // kbusTeardownBar2CpuAperture -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusTeardownBar2CpuAperture__ = &kbusTeardownBar2CpuAperture_GM107;
    }
    else
    {
        pThis->__kbusTeardownBar2CpuAperture__ = &kbusTeardownBar2CpuAperture_GH100;
    }

    // kbusAllocP2PMailboxBar1 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusAllocP2PMailboxBar1__ = &kbusAllocP2PMailboxBar1_GM200;
    }
    else
    {
        pThis->__kbusAllocP2PMailboxBar1__ = &kbusAllocP2PMailboxBar1_GH100;
    }

    // kbusGetP2PMailboxAttributes -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusGetP2PMailboxAttributes__ = &kbusGetP2PMailboxAttributes_GM200;
    }
    else
    {
        pThis->__kbusGetP2PMailboxAttributes__ = &kbusGetP2PMailboxAttributes_GH100;
    }

    // kbusCreateP2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusCreateP2PMapping__ = &kbusCreateP2PMapping_GP100;
    }
    else
    {
        pThis->__kbusCreateP2PMapping__ = &kbusCreateP2PMapping_GH100;
    }

    // kbusRemoveP2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusRemoveP2PMapping__ = &kbusRemoveP2PMapping_GP100;
    }
    else
    {
        pThis->__kbusRemoveP2PMapping__ = &kbusRemoveP2PMapping_GH100;
    }

    // kbusGetEgmPeerId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusGetEgmPeerId__ = &kbusGetEgmPeerId_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetEgmPeerId__ = &kbusGetEgmPeerId_56cd7a;
    }

    // kbusGetPeerId -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusGetPeerId__ = &kbusGetPeerId_GP100;
    }
    else
    {
        pThis->__kbusGetPeerId__ = &kbusGetPeerId_GH100;
    }

    // kbusGetNvlinkPeerId -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kbusGetNvlinkPeerId__ = &kbusGetNvlinkPeerId_GA100;
        }
        // default
        else
        {
            pThis->__kbusGetNvlinkPeerId__ = &kbusGetNvlinkPeerId_c732fb;
        }
    }
    // default
    else
    {
        pThis->__kbusGetNvlinkPeerId__ = &kbusGetNvlinkPeerId_c732fb;
    }

    // kbusGetNvSwitchPeerId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusGetNvSwitchPeerId__ = &kbusGetNvSwitchPeerId_GA100;
    }
    // default
    else
    {
        pThis->__kbusGetNvSwitchPeerId__ = &kbusGetNvSwitchPeerId_c732fb;
    }

    // kbusGetUnusedPciePeerId -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_TU102;
    }
    else
    {
        pThis->__kbusGetUnusedPciePeerId__ = &kbusGetUnusedPciePeerId_GM107;
    }

    // kbusIsPeerIdValid -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusIsPeerIdValid__ = &kbusIsPeerIdValid_GP100;
    }
    else
    {
        pThis->__kbusIsPeerIdValid__ = &kbusIsPeerIdValid_GH100;
    }

    // kbusGetNvlinkP2PPeerId -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusGetNvlinkP2PPeerId__ = &kbusGetNvlinkP2PPeerId_VGPU;
    }
    else
    {
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
    }

    // kbusWriteP2PWmbTag -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kbusWriteP2PWmbTag__ = &kbusWriteP2PWmbTag_f2d351;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusWriteP2PWmbTag__ = &kbusWriteP2PWmbTag_GM200;
    }
    else
    {
        pThis->__kbusWriteP2PWmbTag__ = &kbusWriteP2PWmbTag_GH100;
    }

    // kbusSetupP2PDomainAccess -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kbusSetupP2PDomainAccess__ = &kbusSetupP2PDomainAccess_100832;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusSetupP2PDomainAccess__ = &kbusSetupP2PDomainAccess_GM200;
    }
    else
    {
        pThis->__kbusSetupP2PDomainAccess__ = &kbusSetupP2PDomainAccess_GH100;
    }

    // kbusNeedWarForBug999673 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusNeedWarForBug999673__ = &kbusNeedWarForBug999673_GM200;
    }
    else
    {
        pThis->__kbusNeedWarForBug999673__ = &kbusNeedWarForBug999673_3dd2c9;
    }

    // kbusCreateP2PMappingForC2C -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusCreateP2PMappingForC2C__ = &kbusCreateP2PMappingForC2C_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateP2PMappingForC2C__ = &kbusCreateP2PMappingForC2C_46f6a7;
    }

    // kbusRemoveP2PMappingForC2C -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusRemoveP2PMappingForC2C__ = &kbusRemoveP2PMappingForC2C_GH100;
    }
    // default
    else
    {
        pThis->__kbusRemoveP2PMappingForC2C__ = &kbusRemoveP2PMappingForC2C_46f6a7;
    }

    // kbusUnreserveP2PPeerIds -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_46f6a7;
    }
    else
    {
        pThis->__kbusUnreserveP2PPeerIds__ = &kbusUnreserveP2PPeerIds_GP100;
    }

    // kbusGetBar1P2PDmaInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusGetBar1P2PDmaInfo__ = &kbusGetBar1P2PDmaInfo_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetBar1P2PDmaInfo__ = &kbusGetBar1P2PDmaInfo_395e98;
    }

    // kbusCreateP2PMappingForBar1P2P -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusCreateP2PMappingForBar1P2P__ = &kbusCreateP2PMappingForBar1P2P_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateP2PMappingForBar1P2P__ = &kbusCreateP2PMappingForBar1P2P_395e98;
    }

    // kbusRemoveP2PMappingForBar1P2P -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusRemoveP2PMappingForBar1P2P__ = &kbusRemoveP2PMappingForBar1P2P_GH100;
    }
    // default
    else
    {
        pThis->__kbusRemoveP2PMappingForBar1P2P__ = &kbusRemoveP2PMappingForBar1P2P_395e98;
    }

    // kbusHasPcieBar1P2PMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusHasPcieBar1P2PMapping__ = &kbusHasPcieBar1P2PMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusHasPcieBar1P2PMapping__ = &kbusHasPcieBar1P2PMapping_d69453;
    }

    // kbusIsPcieBar1P2PMappingSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusIsPcieBar1P2PMappingSupported__ = &kbusIsPcieBar1P2PMappingSupported_GH100;
    }
    // default
    else
    {
        pThis->__kbusIsPcieBar1P2PMappingSupported__ = &kbusIsPcieBar1P2PMappingSupported_d69453;
    }

    // kbusCheckFlaSupportedAndInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusCheckFlaSupportedAndInit__ = &kbusCheckFlaSupportedAndInit_GA100;
    }
    // default
    else
    {
        pThis->__kbusCheckFlaSupportedAndInit__ = &kbusCheckFlaSupportedAndInit_ac1694;
    }

    // kbusDetermineFlaRangeAndAllocate -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_GH100;
    }
    // default
    else
    {
        pThis->__kbusDetermineFlaRangeAndAllocate__ = &kbusDetermineFlaRangeAndAllocate_395e98;
    }

    // kbusAllocateFlaVaspace -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_GH100;
    }
    // default
    else
    {
        pThis->__kbusAllocateFlaVaspace__ = &kbusAllocateFlaVaspace_395e98;
    }

    // kbusGetFlaRange -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetFlaRange__ = &kbusGetFlaRange_395e98;
    }

    // kbusAllocateLegacyFlaVaspace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusAllocateLegacyFlaVaspace__ = &kbusAllocateLegacyFlaVaspace_GA100;
    }
    // default
    else
    {
        pThis->__kbusAllocateLegacyFlaVaspace__ = &kbusAllocateLegacyFlaVaspace_395e98;
    }

    // kbusAllocateHostManagedFlaVaspace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_GA100;
    }
    // default
    else
    {
        pThis->__kbusAllocateHostManagedFlaVaspace__ = &kbusAllocateHostManagedFlaVaspace_395e98;
    }

    // kbusDestroyFla -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_GH100;
    }
    // default
    else
    {
        pThis->__kbusDestroyFla__ = &kbusDestroyFla_d44104;
    }

    // kbusGetFlaVaspace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_GA100;
    }
    // default
    else
    {
        pThis->__kbusGetFlaVaspace__ = &kbusGetFlaVaspace_395e98;
    }

    // kbusDestroyHostManagedFlaVaspace -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_GA100;
    }
    // default
    else
    {
        pThis->__kbusDestroyHostManagedFlaVaspace__ = &kbusDestroyHostManagedFlaVaspace_d44104;
    }

    // kbusVerifyFlaRange -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_GA100;
    }
    // default
    else
    {
        pThis->__kbusVerifyFlaRange__ = &kbusVerifyFlaRange_d69453;
    }

    // kbusConstructFlaInstBlk -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_GA100;
    }
    // default
    else
    {
        pThis->__kbusConstructFlaInstBlk__ = &kbusConstructFlaInstBlk_395e98;
    }

    // kbusDestructFlaInstBlk -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_GA100;
    }
    // default
    else
    {
        pThis->__kbusDestructFlaInstBlk__ = &kbusDestructFlaInstBlk_d44104;
    }

    // kbusValidateFlaBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000400UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GA100 | GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_GA100;
    }
    // default
    else
    {
        pThis->__kbusValidateFlaBaseAddress__ = &kbusValidateFlaBaseAddress_395e98;
    }

    // kbusSetupUnbindFla -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_GH100;
    }
    // default
    else
    {
        pThis->__kbusSetupUnbindFla__ = &kbusSetupUnbindFla_46f6a7;
    }

    // kbusSetupBindFla -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_GH100;
    }
    // default
    else
    {
        pThis->__kbusSetupBindFla__ = &kbusSetupBindFla_46f6a7;
    }

    // kbusSendSysmembarSingle -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSendSysmembarSingle__ = &kbusSendSysmembarSingle_56cd7a;
    }
    else
    {
        pThis->__kbusSendSysmembarSingle__ = &kbusSendSysmembarSingle_KERNEL;
    }

    // kbusCacheBAR1ResizeSize_WAR_BUG_3249028 -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_d44104;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GA100;
    }
    else
    {
        pThis->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__ = &kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GH100;
    }

    // kbusRestoreBAR1ResizeSize_WAR_BUG_3249028 -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_ac1694;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GA100;
    }
    else
    {
        pThis->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__ = &kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GH100;
    }

    // kbusIsDirectMappingAllowed -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GM107;
    }
    else
    {
        pThis->__kbusIsDirectMappingAllowed__ = &kbusIsDirectMappingAllowed_GA100;
    }

    // kbusUseDirectSysmemMap -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GM107;
    }
    else
    {
        pThis->__kbusUseDirectSysmemMap__ = &kbusUseDirectSysmemMap_GA100;
    }

    // kbusWriteBAR0WindowBase -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_395e98;
    }
    else
    {
        pThis->__kbusWriteBAR0WindowBase__ = &kbusWriteBAR0WindowBase_GB100;
    }

    // kbusReadBAR0WindowBase -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_13cd8d;
    }
    else
    {
        pThis->__kbusReadBAR0WindowBase__ = &kbusReadBAR0WindowBase_GB100;
    }

    // kbusValidateBAR0WindowBase -- halified (4 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GB10B */ 
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_GB10B;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_72a2e1;
    }
    else
    {
        pThis->__kbusValidateBAR0WindowBase__ = &kbusValidateBAR0WindowBase_GB100;
    }

    // kbusSetBAR0WindowVidOffset -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSetBAR0WindowVidOffset__ = &kbusSetBAR0WindowVidOffset_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbusSetBAR0WindowVidOffset__ = &kbusSetBAR0WindowVidOffset_GM107;
        }
        else
        {
            pThis->__kbusSetBAR0WindowVidOffset__ = &kbusSetBAR0WindowVidOffset_GH100;
        }
    }

    // kbusGetBAR0WindowVidOffset -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusGetBAR0WindowVidOffset__ = &kbusGetBAR0WindowVidOffset_GM107;
    }
    else
    {
        pThis->__kbusGetBAR0WindowVidOffset__ = &kbusGetBAR0WindowVidOffset_GH100;
    }

    // kbusSetupBar0WindowBeforeBar2Bootstrap -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSetupBar0WindowBeforeBar2Bootstrap__ = &kbusSetupBar0WindowBeforeBar2Bootstrap_56cd7a;
    }
    else
    {
        pThis->__kbusSetupBar0WindowBeforeBar2Bootstrap__ = &kbusSetupBar0WindowBeforeBar2Bootstrap_GM107;
    }

    // kbusRestoreBar0WindowAfterBar2Bootstrap -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusRestoreBar0WindowAfterBar2Bootstrap__ = &kbusRestoreBar0WindowAfterBar2Bootstrap_b3696a;
    }
    else
    {
        pThis->__kbusRestoreBar0WindowAfterBar2Bootstrap__ = &kbusRestoreBar0WindowAfterBar2Bootstrap_GM107;
    }

    // kbusVerifyBar2 -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_GB202;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_GM107;
        }
        else
        {
            pThis->__kbusVerifyBar2__ = &kbusVerifyBar2_GH100;
        }
    }

    // kbusBar2BootStrapInPhysicalMode -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusBar2BootStrapInPhysicalMode__ = &kbusBar2BootStrapInPhysicalMode_VF;
    }
    else
    {
        pThis->__kbusBar2BootStrapInPhysicalMode__ = &kbusBar2BootStrapInPhysicalMode_56cd7a;
    }

    // kbusBindBar2 -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbusBindBar2__ = &kbusBindBar2_TU102;
        }
        else
        {
            pThis->__kbusBindBar2__ = &kbusBindBar2_GH100;
        }
    }
    else
    {
        pThis->__kbusBindBar2__ = &kbusBindBar2_5baef9;
    }

    // kbusInstBlkWriteAddrLimit -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kbusInstBlkWriteAddrLimit__ = &kbusInstBlkWriteAddrLimit_GP100;
        }
        else
        {
            pThis->__kbusInstBlkWriteAddrLimit__ = &kbusInstBlkWriteAddrLimit_b3696a;
        }
    }
    else
    {
        pThis->__kbusInstBlkWriteAddrLimit__ = &kbusInstBlkWriteAddrLimit_f2d351;
    }

    // kbusInitInstBlk -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusInitInstBlk__ = &kbusInitInstBlk_GP100;
    }
    else
    {
        pThis->__kbusInitInstBlk__ = &kbusInitInstBlk_ac1694;
    }

    // kbusBar2InstBlkWrite -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusBar2InstBlkWrite__ = &kbusBar2InstBlkWrite_GP100;
    }
    else
    {
        pThis->__kbusBar2InstBlkWrite__ = &kbusBar2InstBlkWrite_d44104;
    }

    // kbusSetupBar2PageTablesAtBottomOfFb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSetupBar2PageTablesAtBottomOfFb__ = &kbusSetupBar2PageTablesAtBottomOfFb_GM107;
    }
    else
    {
        pThis->__kbusSetupBar2PageTablesAtBottomOfFb__ = &kbusSetupBar2PageTablesAtBottomOfFb_22ba1e;
    }

    // kbusTeardownBar2PageTablesAtBottomOfFb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusTeardownBar2PageTablesAtBottomOfFb__ = &kbusTeardownBar2PageTablesAtBottomOfFb_GM107;
    }
    else
    {
        pThis->__kbusTeardownBar2PageTablesAtBottomOfFb__ = &kbusTeardownBar2PageTablesAtBottomOfFb_566dba;
    }

    // kbusSetupBar2InstBlkAtBottomOfFb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSetupBar2InstBlkAtBottomOfFb__ = &kbusSetupBar2InstBlkAtBottomOfFb_GM107;
    }
    else
    {
        pThis->__kbusSetupBar2InstBlkAtBottomOfFb__ = &kbusSetupBar2InstBlkAtBottomOfFb_22ba1e;
    }

    // kbusTeardownBar2InstBlkAtBottomOfFb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusTeardownBar2InstBlkAtBottomOfFb__ = &kbusTeardownBar2InstBlkAtBottomOfFb_GM107;
    }
    else
    {
        pThis->__kbusTeardownBar2InstBlkAtBottomOfFb__ = &kbusTeardownBar2InstBlkAtBottomOfFb_566dba;
    }

    // kbusSetupBar2PageTablesAtTopOfFb -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusSetupBar2PageTablesAtTopOfFb__ = &kbusSetupBar2PageTablesAtTopOfFb_GM107;
    }
    else
    {
        pThis->__kbusSetupBar2PageTablesAtTopOfFb__ = &kbusSetupBar2PageTablesAtTopOfFb_22ba1e;
    }

    // kbusCommitBar2PDEs -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusCommitBar2PDEs__ = &kbusCommitBar2PDEs_GM107;
    }
    else
    {
        pThis->__kbusCommitBar2PDEs__ = &kbusCommitBar2PDEs_22ba1e;
    }

    // kbusVerifyCoherentLink -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        pThis->__kbusVerifyCoherentLink__ = &kbusVerifyCoherentLink_GH100;
    }
    // default
    else
    {
        pThis->__kbusVerifyCoherentLink__ = &kbusVerifyCoherentLink_56cd7a;
    }

    // kbusTeardownMailbox -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusTeardownMailbox__ = &kbusTeardownMailbox_GM107;
    }
    else
    {
        pThis->__kbusTeardownMailbox__ = &kbusTeardownMailbox_GH100;
    }

    // kbusBar1InstBlkVasUpdate -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusBar1InstBlkVasUpdate__ = &kbusBar1InstBlkVasUpdate_GM107;
    }
    else
    {
        pThis->__kbusBar1InstBlkVasUpdate__ = &kbusBar1InstBlkVasUpdate_56cd7a;
    }

    // kbusFlushPcieForBar0Doorbell -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__kbusFlushPcieForBar0Doorbell__ = &kbusFlushPcieForBar0Doorbell_56cd7a;
    }
    else
    {
        pThis->__kbusFlushPcieForBar0Doorbell__ = &kbusFlushPcieForBar0Doorbell_GH100;
    }

    // kbusCreateCoherentCpuMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusCreateCoherentCpuMapping__ = &kbusCreateCoherentCpuMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusCreateCoherentCpuMapping__ = &kbusCreateCoherentCpuMapping_46f6a7;
    }

    // kbusMapCoherentCpuMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusMapCoherentCpuMapping__ = &kbusMapCoherentCpuMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusMapCoherentCpuMapping__ = &kbusMapCoherentCpuMapping_395e98;
    }

    // kbusUnmapCoherentCpuMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusUnmapCoherentCpuMapping__ = &kbusUnmapCoherentCpuMapping_GH100;
    }
    // default
    else
    {
        pThis->__kbusUnmapCoherentCpuMapping__ = &kbusUnmapCoherentCpuMapping_d44104;
    }

    // kbusTeardownCoherentCpuMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B */ 
    {
        pThis->__kbusTeardownCoherentCpuMapping__ = &kbusTeardownCoherentCpuMapping_GV100;
    }
    // default
    else
    {
        pThis->__kbusTeardownCoherentCpuMapping__ = &kbusTeardownCoherentCpuMapping_b3696a;
    }

    // kbusBar1InstBlkBind -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
        {
            pThis->__kbusBar1InstBlkBind__ = &kbusBar1InstBlkBind_TU102;
        }
        else
        {
            pThis->__kbusBar1InstBlkBind__ = &kbusBar1InstBlkBind_GH100;
        }
    }
    else
    {
        pThis->__kbusBar1InstBlkBind__ = &kbusBar1InstBlkBind_92bfc3;
    }

    // kbusGetEccCounts -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kbusGetEccCounts__ = &kbusGetEccCounts_GH100;
    }
    // default
    else
    {
        pThis->__kbusGetEccCounts__ = &kbusGetEccCounts_4a4dee;
    }

    // kbusGetPFBar1Spa -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kbusGetPFBar1Spa__ = &kbusGetPFBar1Spa_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
            ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec1UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB202 | GB203 | GB205 | GB206 | GB207 */ 
        {
            pThis->__kbusGetPFBar1Spa__ = &kbusGetPFBar1Spa_GB100;
        }
        // default
        else
        {
            pThis->__kbusGetPFBar1Spa__ = &kbusGetPFBar1Spa_46f6a7;
        }
    }
} // End __nvoc_init_funcTable_KernelBus_1 with approximately 204 basic block(s).


// Initialize vtable(s) for 102 virtual method(s).
void __nvoc_init_funcTable_KernelBus(KernelBus *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelBus vtable = {
        .__kbusConstructEngine__ = &kbusConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelBus_engstateConstructEngine,    // virtual
        .__kbusStatePreInitLocked__ = &kbusStatePreInitLocked_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelBus_engstateStatePreInitLocked,    // virtual
        .__kbusStateInitLocked__ = &kbusStateInitLocked_IMPL,    // virtual override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelBus_engstateStateInitLocked,    // virtual
        .__kbusStatePreLoad__ = &kbusStatePreLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePreLoad__ = &__nvoc_down_thunk_KernelBus_engstateStatePreLoad,    // virtual
        .__kbusStateLoad__ = &kbusStateLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelBus_engstateStateLoad,    // virtual
        .__kbusStatePostLoad__ = &kbusStatePostLoad_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelBus_engstateStatePostLoad,    // virtual
        .__kbusStatePreUnload__ = &kbusStatePreUnload_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelBus_engstateStatePreUnload,    // virtual
        .__kbusStateUnload__ = &kbusStateUnload_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelBus_engstateStateUnload,    // virtual
        .__kbusStatePostUnload__ = &kbusStatePostUnload_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate) body
        .OBJENGSTATE.__engstateStatePostUnload__ = &__nvoc_down_thunk_KernelBus_engstateStatePostUnload,    // virtual
        .__kbusStateDestroy__ = &kbusStateDestroy_GM107,    // virtual halified (singleton optimized) override (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelBus_engstateStateDestroy,    // virtual
        .__kbusInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kbusInitMissing,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
        .__kbusStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbusStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
        .__kbusStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kbusStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
        .__kbusIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kbusIsPresent,    // virtual inherited (engstate) base (engstate)
        .OBJENGSTATE.__engstateIsPresent__ = &engstateIsPresent_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_vtable = &vtable.OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_vtable = &vtable;    // (kbus) this

    // Initialize vtable(s) with 88 per-object function pointer(s).
    __nvoc_init_funcTable_KernelBus_1(pThis, pRmhalspecowner);
}

NvU32 kbusGetP2PWriteMailboxAddressSize_STATIC_DISPATCH(struct OBJGPU *pGpu) {
    ChipHal *chipHal = &staticCast(pGpu, RmHalspecOwner)->chipHal;
    const unsigned long chipHal_HalVarIdx = (unsigned long)chipHal->__nvoc_HalVarIdx;

    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        return kbusGetP2PWriteMailboxAddressSize_GH100(pGpu);
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000ec0UL) )) /* ChipHal: GB100 | GB102 | GB202 | GB203 | GB205 | GB206 | GB207 */ 
    {
        return kbusGetP2PWriteMailboxAddressSize_GB100(pGpu);
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

NV_STATUS __nvoc_objCreate_KernelBus(KernelBus **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelBus *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelBus), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelBus));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelBus);

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

    __nvoc_init_KernelBus(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelBus(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelBus_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelBus_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelBus));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelBus(KernelBus **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelBus(ppThis, pParent, createFlags);

    return status;
}

