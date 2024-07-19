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

// 7 down-thunk(s) defined to bridge methods in KernelNvlink from superclasses

// knvlinkConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return knvlinkConstructEngine(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStatePreInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePreInitLocked(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return knvlinkStatePreInitLocked(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

// knvlinkStateLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStateLoad(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStatePostLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStatePostLoad(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStateUnload(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStatePostUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStatePostUnload(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkIsPresent: virtual override (engstate) base (engstate)
static NvBool __nvoc_down_thunk_KernelNvlink_engstateIsPresent(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return knvlinkIsPresent(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}


// 7 up-thunk(s) defined to bridge methods in KernelNvlink to superclasses

// knvlinkInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_knvlinkInitMissing(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

// knvlinkStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

// knvlinkStateInitLocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitLocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

// knvlinkStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
}

// knvlinkStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreLoad(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStatePreUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreUnload(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset), arg3);
}

// knvlinkStateDestroy: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_knvlinkStateDestroy(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelNvlink_OBJENGSTATE.offset));
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
    // default
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_IS_MISSING, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
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

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_MINION_GFW_BOOT
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_MINION_GFW_BOOT, ((NvBool)(0 == 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_MINION_GFW_BOOT, ((NvBool)(0 != 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED, ((NvBool)(0 != 0)));
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED, ((NvBool)(0 == 0)));
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_ENCRYPTION_ENABLED
    pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED, ((NvBool)(0 != 0)));

    pThis->fabricBaseAddr = (+18446744073709551615ULL);

    pThis->fabricEgmBaseAddr = (+18446744073709551615ULL);
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

// Vtable initialization
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

    // knvlinkConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__knvlinkConstructEngine__ = &knvlinkConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelNvlink_engstateConstructEngine;

    // knvlinkStatePreInitLocked -- virtual override (engstate) base (engstate)
    pThis->__knvlinkStatePreInitLocked__ = &knvlinkStatePreInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePreInitLocked;

    // knvlinkStateLoad -- virtual override (engstate) base (engstate)
    pThis->__knvlinkStateLoad__ = &knvlinkStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelNvlink_engstateStateLoad;

    // knvlinkStatePostLoad -- virtual override (engstate) base (engstate)
    pThis->__knvlinkStatePostLoad__ = &knvlinkStatePostLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePostLoad;

    // knvlinkStateUnload -- virtual override (engstate) base (engstate)
    pThis->__knvlinkStateUnload__ = &knvlinkStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelNvlink_engstateStateUnload;

    // knvlinkStatePostUnload -- virtual override (engstate) base (engstate)
    pThis->__knvlinkStatePostUnload__ = &knvlinkStatePostUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePostUnload;

    // knvlinkIsPresent -- virtual override (engstate) base (engstate)
    pThis->__knvlinkIsPresent__ = &knvlinkIsPresent_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateIsPresent__ = &__nvoc_down_thunk_KernelNvlink_engstateIsPresent;

    // knvlinkSetUniqueFabricBaseAddress -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_GV100;
    }
    // default
    else
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_46f6a7;
    }

    // knvlinkClearUniqueFabricBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkClearUniqueFabricBaseAddress__ = &knvlinkClearUniqueFabricBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkClearUniqueFabricBaseAddress__ = &knvlinkClearUniqueFabricBaseAddress_b3696a;
    }

    // knvlinkSetUniqueFabricEgmBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkSetUniqueFabricEgmBaseAddress__ = &knvlinkSetUniqueFabricEgmBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkSetUniqueFabricEgmBaseAddress__ = &knvlinkSetUniqueFabricEgmBaseAddress_46f6a7;
    }

    // knvlinkClearUniqueFabricEgmBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkClearUniqueFabricEgmBaseAddress__ = &knvlinkClearUniqueFabricEgmBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkClearUniqueFabricEgmBaseAddress__ = &knvlinkClearUniqueFabricEgmBaseAddress_b3696a;
    }

    // knvlinkHandleFaultUpInterrupt -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkHandleFaultUpInterrupt__ = &knvlinkHandleFaultUpInterrupt_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkHandleFaultUpInterrupt__ = &knvlinkHandleFaultUpInterrupt_46f6a7;
    }

    // knvlinkValidateFabricBaseAddress -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_GA100;
    }
    // default
    else
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_46f6a7;
    }

    // knvlinkValidateFabricEgmBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkValidateFabricEgmBaseAddress__ = &knvlinkValidateFabricEgmBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkValidateFabricEgmBaseAddress__ = &knvlinkValidateFabricEgmBaseAddress_46f6a7;
    }

    // knvlinkGetConnectedLinksMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000380UL) )) /* ChipHal: TU106 | TU116 | TU117 | GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_15a734;
    }
    else
    {
        pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_TU102;
    }

    // knvlinkEnableLinksPostTopology -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkEnableLinksPostTopology__ = &knvlinkEnableLinksPostTopology_GV100;
    }
    else
    {
        pThis->__knvlinkEnableLinksPostTopology__ = &knvlinkEnableLinksPostTopology_56cd7a;
    }

    // knvlinkOverrideConfig -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_56cd7a;
    }
    else
    {
        pThis->__knvlinkOverrideConfig__ = &knvlinkOverrideConfig_GA100;
    }

    // knvlinkFilterBridgeLinks -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkFilterBridgeLinks__ = &knvlinkFilterBridgeLinks_46f6a7;
    }
    else
    {
        pThis->__knvlinkFilterBridgeLinks__ = &knvlinkFilterBridgeLinks_TU102;
    }

    // knvlinkGetUniquePeerIdMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkGetUniquePeerIdMask__ = &knvlinkGetUniquePeerIdMask_GP100;
    }
    else
    {
        pThis->__knvlinkGetUniquePeerIdMask__ = &knvlinkGetUniquePeerIdMask_15a734;
    }

    // knvlinkGetUniquePeerId -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkGetUniquePeerId__ = &knvlinkGetUniquePeerId_GP100;
    }
    else
    {
        pThis->__knvlinkGetUniquePeerId__ = &knvlinkGetUniquePeerId_c732fb;
    }

    // knvlinkRemoveMapping -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_GP100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_56cd7a;
    }
    else
    {
        pThis->__knvlinkRemoveMapping__ = &knvlinkRemoveMapping_GA100;
    }

    // knvlinkGetP2POptimalCEs -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkGetP2POptimalCEs__ = &knvlinkGetP2POptimalCEs_56cd7a;
    }
    else
    {
        pThis->__knvlinkGetP2POptimalCEs__ = &knvlinkGetP2POptimalCEs_GP100;
    }

    // knvlinkConstructHal -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000380UL) )) /* ChipHal: TU106 | TU116 | TU117 | GB100 | GB102 */ 
    {
        pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_56cd7a;
    }
    else
    {
        pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_GV100;
    }

    // knvlinkSetupPeerMapping -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkSetupPeerMapping__ = &knvlinkSetupPeerMapping_b3696a;
    }
    else
    {
        pThis->__knvlinkSetupPeerMapping__ = &knvlinkSetupPeerMapping_GP100;
    }

    // knvlinkProgramLinkSpeed -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000380UL) )) /* ChipHal: TU106 | TU116 | TU117 */ 
    {
        pThis->__knvlinkProgramLinkSpeed__ = &knvlinkProgramLinkSpeed_56cd7a;
    }
    else
    {
        pThis->__knvlinkProgramLinkSpeed__ = &knvlinkProgramLinkSpeed_GV100;
    }

    // knvlinkPoweredUpForD3 -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->__knvlinkPoweredUpForD3__ = &knvlinkPoweredUpForD3_TU102;
    }
    else
    {
        pThis->__knvlinkPoweredUpForD3__ = &knvlinkPoweredUpForD3_491d52;
    }

    // knvlinkIsAliSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkIsAliSupported__ = &knvlinkIsAliSupported_GH100;
    }
    else
    {
        pThis->__knvlinkIsAliSupported__ = &knvlinkIsAliSupported_56cd7a;
    }

    // knvlinkPostSetupNvlinkPeer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkPostSetupNvlinkPeer__ = &knvlinkPostSetupNvlinkPeer_GH100;
    }
    else
    {
        pThis->__knvlinkPostSetupNvlinkPeer__ = &knvlinkPostSetupNvlinkPeer_56cd7a;
    }

    // knvlinkDiscoverPostRxDetLinks -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkDiscoverPostRxDetLinks__ = &knvlinkDiscoverPostRxDetLinks_GH100;
    }
    else
    {
        pThis->__knvlinkDiscoverPostRxDetLinks__ = &knvlinkDiscoverPostRxDetLinks_46f6a7;
    }

    // knvlinkLogAliDebugMessages -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_46f6a7;
    }

    // knvlinkDumpCallbackRegister -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__knvlinkDumpCallbackRegister__ = &knvlinkDumpCallbackRegister_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkDumpCallbackRegister__ = &knvlinkDumpCallbackRegister_b3696a;
    }

    // knvlinkGetEffectivePeerLinkMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkGetEffectivePeerLinkMask__ = &knvlinkGetEffectivePeerLinkMask_GH100;
    }
    else
    {
        pThis->__knvlinkGetEffectivePeerLinkMask__ = &knvlinkGetEffectivePeerLinkMask_b3696a;
    }

    // knvlinkGetNumLinksToBeReducedPerIoctrl -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkGetNumLinksToBeReducedPerIoctrl__ = &knvlinkGetNumLinksToBeReducedPerIoctrl_GH100;
    }
    else
    {
        pThis->__knvlinkGetNumLinksToBeReducedPerIoctrl__ = &knvlinkGetNumLinksToBeReducedPerIoctrl_4a4dee;
    }

    // knvlinkIsBandwidthModeOff -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkIsBandwidthModeOff__ = &knvlinkIsBandwidthModeOff_GH100;
    }
    else
    {
        pThis->__knvlinkIsBandwidthModeOff__ = &knvlinkIsBandwidthModeOff_491d52;
    }

    // knvlinkDirectConnectCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__knvlinkDirectConnectCheck__ = &knvlinkDirectConnectCheck_GH100;
    }
    else
    {
        pThis->__knvlinkDirectConnectCheck__ = &knvlinkDirectConnectCheck_b3696a;
    }

    // knvlinkIsGpuReducedNvlinkConfig -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__knvlinkIsGpuReducedNvlinkConfig__ = &knvlinkIsGpuReducedNvlinkConfig_GA100;
    }
    else
    {
        pThis->__knvlinkIsGpuReducedNvlinkConfig__ = &knvlinkIsGpuReducedNvlinkConfig_491d52;
    }

    // knvlinkIsFloorSweepingNeeded -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkIsFloorSweepingNeeded__ = &knvlinkIsFloorSweepingNeeded_GH100;
    }
    else
    {
        pThis->__knvlinkIsFloorSweepingNeeded__ = &knvlinkIsFloorSweepingNeeded_491d52;
    }

    // knvlinkGetSupportedCounters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__knvlinkGetSupportedCounters__ = &knvlinkGetSupportedCounters_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkGetSupportedCounters__ = &knvlinkGetSupportedCounters_46f6a7;
    }

    // knvlinkInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkInitMissing;

    // knvlinkStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked;

    // knvlinkStateInitLocked -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitLocked;

    // knvlinkStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitUnlocked;

    // knvlinkStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreLoad;

    // knvlinkStatePreUnload -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreUnload;

    // knvlinkStateDestroy -- virtual inherited (engstate) base (engstate)
    pThis->__knvlinkStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateDestroy;
} // End __nvoc_init_funcTable_KernelNvlink_1 with approximately 88 basic block(s).


// Initialize vtable(s) for 45 virtual method(s).
void __nvoc_init_funcTable_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 45 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelNvlink(KernelNvlink **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelNvlink *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelNvlink), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelNvlink));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelNvlink);

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

    __nvoc_init_KernelNvlink(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelNvlink(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelNvlink_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelNvlink_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelNvlink));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelNvlink(KernelNvlink **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelNvlink(ppThis, pParent, createFlags);

    return status;
}

