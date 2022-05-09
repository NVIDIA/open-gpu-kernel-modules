#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvlink_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xce6818 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvlink;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelNvlink(KernelNvlink*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelNvlink(KernelNvlink*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelNvlink(KernelNvlink*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelNvlink(KernelNvlink*, RmHalspecOwner* );
void __nvoc_dtor_KernelNvlink(KernelNvlink*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelNvlink;

static const struct NVOC_RTTI __nvoc_rtti_KernelNvlink_KernelNvlink = {
    /*pClassDef=*/          &__nvoc_class_def_KernelNvlink,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelNvlink,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvlink_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelNvlink_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelNvlink = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelNvlink_KernelNvlink,
        &__nvoc_rtti_KernelNvlink_OBJENGSTATE,
        &__nvoc_rtti_KernelNvlink_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvlink = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelNvlink),
        /*classId=*/            classId(KernelNvlink),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelNvlink",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelNvlink,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelNvlink,
    /*pExportInfo=*/        &__nvoc_export_info_KernelNvlink
};

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateConstructEngine(OBJGPU *arg0, struct OBJENGSTATE *arg1, ENGDESCRIPTOR arg2) {
    return knvlinkConstructEngine(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateStatePreInitLocked(OBJGPU *arg0, struct OBJENGSTATE *arg1) {
    return knvlinkStatePreInitLocked(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateStateLoad(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return knvlinkStateLoad(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateStatePostLoad(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return knvlinkStatePostLoad(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateStateUnload(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return knvlinkStateUnload(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg2);
}

static NV_STATUS __nvoc_thunk_KernelNvlink_engstateStatePostUnload(OBJGPU *arg0, struct OBJENGSTATE *arg1, NvU32 arg2) {
    return knvlinkStatePostUnload(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg2);
}

static NvBool __nvoc_thunk_KernelNvlink_engstateIsPresent(OBJGPU *arg0, struct OBJENGSTATE *arg1) {
    return knvlinkIsPresent(arg0, (struct KernelNvlink *)(((unsigned char *)arg1) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkReconcileTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkStateInitLocked(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkStatePreLoad(POBJGPU pGpu, struct KernelNvlink *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_OBJENGSTATE_knvlinkStateDestroy(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkStatePreUnload(POBJGPU pGpu, struct KernelNvlink *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkStateInitUnlocked(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_knvlinkInitMissing(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked(POBJGPU pGpu, struct KernelNvlink *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkGetTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkCompareTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_knvlinkFreeTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkAllocTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_knvlinkSetTunableState(POBJGPU pGpu, struct KernelNvlink *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), pTunableState);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelNvlink = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelNvlink(KernelNvlink *pThis) {
    __nvoc_knvlinkDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_IS_MISSING
    if (0)
    {
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING
    pThis->setProperty(pThis, PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING, ((NvBool)(0 != 0)));

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED, ((NvBool)(0 != 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED, ((NvBool)(0 == 0)));
    }

    pThis->fabricBaseAddr = (+18446744073709551615ULL);
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelNvlink_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelNvlink(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelNvlink_exit; // Success

__nvoc_ctor_KernelNvlink_fail_OBJENGSTATE:
__nvoc_ctor_KernelNvlink_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelNvlink_1(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__knvlinkConstructEngine__ = &knvlinkConstructEngine_IMPL;

    pThis->__knvlinkStatePreInitLocked__ = &knvlinkStatePreInitLocked_IMPL;

    pThis->__knvlinkStateLoad__ = &knvlinkStateLoad_IMPL;

    pThis->__knvlinkStatePostLoad__ = &knvlinkStatePostLoad_IMPL;

    pThis->__knvlinkStateUnload__ = &knvlinkStateUnload_IMPL;

    pThis->__knvlinkStatePostUnload__ = &knvlinkStatePostUnload_IMPL;

    pThis->__knvlinkIsPresent__ = &knvlinkIsPresent_IMPL;

    // Hal function -- knvlinkValidateFabricBaseAddress
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_GA100;
        }
        // default
        else
        {
            pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_46f6a7;
        }
    }

    // Hal function -- knvlinkGetConnectedLinksMask
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_15a734;
        }
    }

    // Hal function -- knvlinkEnableLinksPostTopology
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkEnableLinksPostTopology__ = &knvlinkEnableLinksPostTopology_GV100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ff80UL) )) /* ChipHal: TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkEnableLinksPostTopology__ = &knvlinkEnableLinksPostTopology_56cd7a;
        }
    }

    // Hal function -- knvlinkOverrideConfig
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_GV100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_56cd7a;
        }
    }

    // Hal function -- knvlinkFilterBridgeLinks
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (0)
        {
        }
        else if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkFilterBridgeLinks__ = &knvlinkFilterBridgeLinks_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkFilterBridgeLinks__ = &knvlinkFilterBridgeLinks_46f6a7;
        }
    }

    // Hal function -- knvlinkGetUniquePeerIdMask
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkGetUniquePeerIdMask__ = &knvlinkGetUniquePeerIdMask_GP100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ff80UL) )) /* ChipHal: TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkGetUniquePeerIdMask__ = &knvlinkGetUniquePeerIdMask_15a734;
        }
    }

    // Hal function -- knvlinkGetUniquePeerId
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkGetUniquePeerId__ = &knvlinkGetUniquePeerId_GP100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ff80UL) )) /* ChipHal: TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkGetUniquePeerId__ = &knvlinkGetUniquePeerId_c732fb;
        }
    }

    // Hal function -- knvlinkRemoveMapping
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_GP100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_56cd7a;
        }
    }

    // Hal function -- knvlinkGetP2POptimalCEs
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkGetP2POptimalCEs__ = &knvlinkGetP2POptimalCEs_GP100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkGetP2POptimalCEs__ = &knvlinkGetP2POptimalCEs_56cd7a;
        }
    }

    // Hal function -- knvlinkConstructHal
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_GV100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_56cd7a;
        }
    }

    // Hal function -- knvlinkSetupPeerMapping
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkSetupPeerMapping__ = &knvlinkSetupPeerMapping_GP100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkSetupPeerMapping__ = &knvlinkSetupPeerMapping_b3696a;
        }
    }

    // Hal function -- knvlinkProgramLinkSpeed
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (0)
        {
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkProgramLinkSpeed__ = &knvlinkProgramLinkSpeed_GV100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
        {
            pThis->__knvlinkProgramLinkSpeed__ = &knvlinkProgramLinkSpeed_56cd7a;
        }
    }

    // Hal function -- knvlinkPoweredUpForD3
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
        {
            pThis->__knvlinkPoweredUpForD3__ = &knvlinkPoweredUpForD3_TU102;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ff80UL) )) /* ChipHal: TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__knvlinkPoweredUpForD3__ = &knvlinkPoweredUpForD3_491d52;
        }
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelNvlink_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_thunk_KernelNvlink_engstateStatePreInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_KernelNvlink_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_KernelNvlink_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_KernelNvlink_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__ = &__nvoc_thunk_KernelNvlink_engstateStatePostUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateIsPresent__ = &__nvoc_thunk_KernelNvlink_engstateIsPresent;

    pThis->__knvlinkReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkReconcileTunableState;

    pThis->__knvlinkStateInitLocked__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStateInitLocked;

    pThis->__knvlinkStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStatePreLoad;

    pThis->__knvlinkStateDestroy__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStateDestroy;

    pThis->__knvlinkStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStatePreUnload;

    pThis->__knvlinkStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStateInitUnlocked;

    pThis->__knvlinkInitMissing__ = &__nvoc_thunk_OBJENGSTATE_knvlinkInitMissing;

    pThis->__knvlinkStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked;

    pThis->__knvlinkGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkGetTunableState;

    pThis->__knvlinkCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkCompareTunableState;

    pThis->__knvlinkFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkFreeTunableState;

    pThis->__knvlinkAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkAllocTunableState;

    pThis->__knvlinkSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_knvlinkSetTunableState;
}

void __nvoc_init_funcTable_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelNvlink_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelNvlink = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelNvlink(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelNvlink(KernelNvlink **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelNvlink *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelNvlink));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelNvlink));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelNvlink);

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

    __nvoc_init_KernelNvlink(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelNvlink(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelNvlink_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelNvlink_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelNvlink(KernelNvlink **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelNvlink(ppThis, pParent, createFlags);

    return status;
}

