#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_nvlink_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xce6818 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvlink;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

// Forward declarations for KernelNvlink
void __nvoc_init__OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init__KernelNvlink(KernelNvlink*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelNvlink(KernelNvlink*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelNvlink(KernelNvlink*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_dataField_KernelNvlink(KernelNvlink*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelNvlink(KernelNvlink*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelNvlink;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelNvlink;

// Down-thunk(s) to bridge KernelNvlink methods from ancestors (if any)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePreInitLocked(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3);    // this
NvBool __nvoc_down_thunk_KernelNvlink_engstateIsPresent(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this);    // this

// Up-thunk(s) to bridge KernelNvlink methods to ancestors (if any)
void __nvoc_up_thunk_OBJENGSTATE_knvlinkInitMissing(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitLocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreLoad(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3);    // this
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreUnload(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3);    // this
void __nvoc_up_thunk_OBJENGSTATE_knvlinkStateDestroy(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate);    // this

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
    /*pCastInfo=*/          &__nvoc_castinfo__KernelNvlink,
    /*pExportInfo=*/        &__nvoc_export_info__KernelNvlink
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelNvlink __nvoc_metadata__KernelNvlink = {
    .rtti.pClassDef = &__nvoc_class_def_KernelNvlink,    // (knvlink) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelNvlink,
    .rtti.offset    = 0,
    .metadata__OBJENGSTATE.rtti.pClassDef = &__nvoc_class_def_OBJENGSTATE,    // (engstate) super
    .metadata__OBJENGSTATE.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.rtti.offset    = NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE),
    .metadata__OBJENGSTATE.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__OBJENGSTATE.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__OBJENGSTATE.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),

    .vtable.__knvlinkConstructEngine__ = &knvlinkConstructEngine_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelNvlink_engstateConstructEngine,    // virtual
    .vtable.__knvlinkStatePreInitLocked__ = &knvlinkStatePreInitLocked_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePreInitLocked,    // virtual
    .vtable.__knvlinkStateLoad__ = &knvlinkStateLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateLoad__ = &__nvoc_down_thunk_KernelNvlink_engstateStateLoad,    // virtual
    .vtable.__knvlinkStatePostLoad__ = &knvlinkStatePostLoad_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePostLoad,    // virtual
    .vtable.__knvlinkStateUnload__ = &knvlinkStateUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateUnload__ = &__nvoc_down_thunk_KernelNvlink_engstateStateUnload,    // virtual
    .vtable.__knvlinkStatePostUnload__ = &knvlinkStatePostUnload_IMPL,    // virtual override (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePostUnload__ = &__nvoc_down_thunk_KernelNvlink_engstateStatePostUnload,    // virtual
    .metadata__OBJENGSTATE.vtable.__engstateIsPresent__ = &__nvoc_down_thunk_KernelNvlink_engstateIsPresent,    // virtual
    .vtable.__knvlinkInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkInitMissing,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateInitMissing__ = &engstateInitMissing_IMPL,    // virtual
    .vtable.__knvlinkStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreInitUnlocked__ = &engstateStatePreInitUnlocked_IMPL,    // virtual
    .vtable.__knvlinkStateInitLocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitLocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitLocked__ = &engstateStateInitLocked_IMPL,    // virtual
    .vtable.__knvlinkStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitUnlocked,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateInitUnlocked__ = &engstateStateInitUnlocked_IMPL,    // virtual
    .vtable.__knvlinkStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreLoad,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreLoad__ = &engstateStatePreLoad_IMPL,    // virtual
    .vtable.__knvlinkStatePreUnload__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreUnload,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStatePreUnload__ = &engstateStatePreUnload_IMPL,    // virtual
    .vtable.__knvlinkStateDestroy__ = &__nvoc_up_thunk_OBJENGSTATE_knvlinkStateDestroy,    // virtual inherited (engstate) base (engstate)
    .metadata__OBJENGSTATE.vtable.__engstateStateDestroy__ = &engstateStateDestroy_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelNvlink = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__KernelNvlink.rtti,    // [0]: (knvlink) this
        &__nvoc_metadata__KernelNvlink.metadata__OBJENGSTATE.rtti,    // [1]: (engstate) super
        &__nvoc_metadata__KernelNvlink.metadata__OBJENGSTATE.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

// 7 down-thunk(s) defined to bridge methods in KernelNvlink from superclasses

// knvlinkConstructEngine: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateConstructEngine(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, ENGDESCRIPTOR arg3) {
    return knvlinkConstructEngine(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStatePreInitLocked: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePreInitLocked(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return knvlinkStatePreInitLocked(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}

// knvlinkStateLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStateLoad(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStatePostLoad: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostLoad(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStatePostLoad(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStateUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStateUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStateUnload(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStatePostUnload: virtual override (engstate) base (engstate)
NV_STATUS __nvoc_down_thunk_KernelNvlink_engstateStatePostUnload(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this, NvU32 arg3) {
    return knvlinkStatePostUnload(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkIsPresent: virtual halified (2 hals) override (engstate) base (engstate)
NvBool __nvoc_down_thunk_KernelNvlink_engstateIsPresent(struct OBJGPU *arg1, struct OBJENGSTATE *arg_this) {
    return knvlinkIsPresent(arg1, (struct KernelNvlink *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}


// 7 up-thunk(s) defined to bridge methods in KernelNvlink to superclasses

// knvlinkInitMissing: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_knvlinkInitMissing(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}

// knvlinkStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}

// knvlinkStateInitLocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitLocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}

// knvlinkStateInitUnlocked: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStateInitUnlocked(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}

// knvlinkStatePreLoad: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreLoad(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStatePreUnload: virtual inherited (engstate) base (engstate)
NV_STATUS __nvoc_up_thunk_OBJENGSTATE_knvlinkStatePreUnload(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate, NvU32 arg3) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)), arg3);
}

// knvlinkStateDestroy: virtual inherited (engstate) base (engstate)
void __nvoc_up_thunk_OBJENGSTATE_knvlinkStateDestroy(struct OBJGPU *pGpu, struct KernelNvlink *pEngstate) {
    engstateStateDestroy(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + NV_OFFSETOF(KernelNvlink, __nvoc_base_OBJENGSTATE)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelNvlink = 
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
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_IS_MISSING, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc60UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0fc00UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED, NV_TRUE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000060UL) )) /* ChipHal: TU102 | TU104 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_L2_POWER_STATE_FOR_LONG_IDLE, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING
    pThis->setProperty(pThis, PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING, NV_FALSE);

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_MINION_GFW_BOOT
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x70000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_MINION_GFW_BOOT, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_MINION_GFW_BOOT, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf1f0ffe0UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_SYSMEM_SUPPORT_ENABLED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED, NV_FALSE);
    }

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_ENCRYPTION_ENABLED
    pThis->setProperty(pThis, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED, NV_FALSE);

    // NVOC Property Hal field -- PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED, NV_TRUE);
    }
    // default
    else
    {
        pThis->setProperty(pThis, PDB_PROP_KNVLINK_RBM_LINK_COUNT_ENABLED, NV_FALSE);
    }

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

    // knvlinkIsPresent -- virtual halified (2 hals) override (engstate) base (engstate)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__knvlinkIsPresent__ = &knvlinkIsPresent_3dd2c9;
    }
    // default
    else
    {
        pThis->__knvlinkIsPresent__ = &knvlinkIsPresent_IMPL;
    }

    // knvlinkSetUniqueFabricBaseAddress -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkSetUniqueFabricBaseAddress__ = &knvlinkSetUniqueFabricBaseAddress_46f6a7;
    }

    // knvlinkClearUniqueFabricBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkClearUniqueFabricBaseAddress__ = &knvlinkClearUniqueFabricBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkClearUniqueFabricBaseAddress__ = &knvlinkClearUniqueFabricBaseAddress_b3696a;
    }

    // knvlinkSetUniqueFabricEgmBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkSetUniqueFabricEgmBaseAddress__ = &knvlinkSetUniqueFabricEgmBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkSetUniqueFabricEgmBaseAddress__ = &knvlinkSetUniqueFabricEgmBaseAddress_46f6a7;
    }

    // knvlinkClearUniqueFabricEgmBaseAddress -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xf0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GH100 | GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_GH100;
    }
    // default
    else
    {
        pThis->__knvlinkValidateFabricBaseAddress__ = &knvlinkValidateFabricBaseAddress_46f6a7;
    }

    // knvlinkValidateFabricEgmBaseAddress -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkValidateFabricEgmBaseAddress__ = &knvlinkValidateFabricEgmBaseAddress_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkValidateFabricEgmBaseAddress__ = &knvlinkValidateFabricEgmBaseAddress_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkValidateFabricEgmBaseAddress__ = &knvlinkValidateFabricEgmBaseAddress_46f6a7;
    }

    // knvlinkGetConnectedLinksMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_TU102;
    }
    else
    {
        pThis->__knvlinkGetConnectedLinksMask__ = &knvlinkGetConnectedLinksMask_15a734;
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc60UL) )) /* ChipHal: TU102 | TU104 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_GV100;
    }
    else
    {
        pThis->__knvlinkConstructHal__ = &knvlinkConstructHal_56cd7a;
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
        pThis->__knvlinkPoweredUpForD3__ = &knvlinkPoweredUpForD3_3dd2c9;
    }

    // knvlinkIsAliSupported -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkIsAliSupported__ = &knvlinkIsAliSupported_56cd7a;
    }
    else
    {
        pThis->__knvlinkIsAliSupported__ = &knvlinkIsAliSupported_GH100;
    }

    // knvlinkPostSetupNvlinkPeer -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkPostSetupNvlinkPeer__ = &knvlinkPostSetupNvlinkPeer_56cd7a;
    }
    else
    {
        pThis->__knvlinkPostSetupNvlinkPeer__ = &knvlinkPostSetupNvlinkPeer_GH100;
    }

    // knvlinkDiscoverPostRxDetLinks -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkDiscoverPostRxDetLinks__ = &knvlinkDiscoverPostRxDetLinks_46f6a7;
    }
    else
    {
        pThis->__knvlinkDiscoverPostRxDetLinks__ = &knvlinkDiscoverPostRxDetLinks_GH100;
    }

    // knvlinkLogAliDebugMessages -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xe0000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e6UL) )) /* ChipHal: GB100 | GB102 | GB10B | GB110 | GB112 | GB202 | GB203 | GB205 | GB206 | GB207 | GB20B */ 
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkLogAliDebugMessages__ = &knvlinkLogAliDebugMessages_46f6a7;
    }

    // knvlinkDumpCallbackRegister -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkDumpCallbackRegister__ = &knvlinkDumpCallbackRegister_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkDumpCallbackRegister__ = &knvlinkDumpCallbackRegister_b3696a;
    }

    // knvlinkGetEffectivePeerLinkMask -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkGetEffectivePeerLinkMask__ = &knvlinkGetEffectivePeerLinkMask_GH100;
    }
    else
    {
        pThis->__knvlinkGetEffectivePeerLinkMask__ = &knvlinkGetEffectivePeerLinkMask_b3696a;
    }

    // knvlinkGetNumLinksToBeReducedPerIoctrl -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkGetNumLinksToBeReducedPerIoctrl__ = &knvlinkGetNumLinksToBeReducedPerIoctrl_4a4dee;
    }
    else
    {
        pThis->__knvlinkGetNumLinksToBeReducedPerIoctrl__ = &knvlinkGetNumLinksToBeReducedPerIoctrl_GH100;
    }

    // knvlinkIsBandwidthModeOff -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkIsBandwidthModeOff__ = &knvlinkIsBandwidthModeOff_3dd2c9;
    }
    else
    {
        pThis->__knvlinkIsBandwidthModeOff__ = &knvlinkIsBandwidthModeOff_GH100;
    }

    // knvlinkIsBwModeSupported -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkIsBwModeSupported__ = &knvlinkIsBwModeSupported_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkIsBwModeSupported__ = &knvlinkIsBwModeSupported_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkIsBwModeSupported__ = &knvlinkIsBwModeSupported_3dd2c9;
    }

    // knvlinkGetHshubSupportedRbmModes -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkGetHshubSupportedRbmModes__ = &knvlinkGetHshubSupportedRbmModes_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkGetHshubSupportedRbmModes__ = &knvlinkGetHshubSupportedRbmModes_46f6a7;
    }

    // knvlinkPostSchedulingEnableCallbackRegister -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkPostSchedulingEnableCallbackRegister__ = &knvlinkPostSchedulingEnableCallbackRegister_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkPostSchedulingEnableCallbackRegister__ = &knvlinkPostSchedulingEnableCallbackRegister_b3696a;
    }

    // knvlinkPostSchedulingEnableCallbackUnregister -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkPostSchedulingEnableCallbackUnregister__ = &knvlinkPostSchedulingEnableCallbackUnregister_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkPostSchedulingEnableCallbackUnregister__ = &knvlinkPostSchedulingEnableCallbackUnregister_b3696a;
    }

    // knvlinkGetSupportedBwMode -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkGetSupportedBwMode__ = &knvlinkGetSupportedBwMode_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
             ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkGetSupportedBwMode__ = &knvlinkGetSupportedBwMode_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkGetSupportedBwMode__ = &knvlinkGetSupportedBwMode_46f6a7;
    }

    // knvlinkDirectConnectCheck -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__knvlinkDirectConnectCheck__ = &knvlinkDirectConnectCheck_b3696a;
    }
    else
    {
        pThis->__knvlinkDirectConnectCheck__ = &knvlinkDirectConnectCheck_GH100;
    }

    // knvlinkIsGpuReducedNvlinkConfig -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__knvlinkIsGpuReducedNvlinkConfig__ = &knvlinkIsGpuReducedNvlinkConfig_GA100;
    }
    else
    {
        pThis->__knvlinkIsGpuReducedNvlinkConfig__ = &knvlinkIsGpuReducedNvlinkConfig_3dd2c9;
    }

    // knvlinkIsFloorSweepingNeeded -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__knvlinkIsFloorSweepingNeeded__ = &knvlinkIsFloorSweepingNeeded_GH100;
    }
    else
    {
        pThis->__knvlinkIsFloorSweepingNeeded__ = &knvlinkIsFloorSweepingNeeded_3dd2c9;
    }

    // knvlinkGetSupportedCounters -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkGetSupportedCounters__ = &knvlinkGetSupportedCounters_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkGetSupportedCounters__ = &knvlinkGetSupportedCounters_46f6a7;
    }

    // knvlinkIsEncryptEnSet -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__knvlinkIsEncryptEnSet__ = &knvlinkIsEncryptEnSet_3dd2c9;
    }
    else
    {
        pThis->__knvlinkIsEncryptEnSet__ = &knvlinkIsEncryptEnSet_GB100;
    }

    // knvlinkIsNvleEnabled -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__knvlinkIsNvleEnabled__ = &knvlinkIsNvleEnabled_3dd2c9;
    }
    else
    {
        pThis->__knvlinkIsNvleEnabled__ = &knvlinkIsNvleEnabled_GB100;
    }

    // knvlinkEncryptionGetGpuIdentifiers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkEncryptionGetGpuIdentifiers__ = &knvlinkEncryptionGetGpuIdentifiers_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkEncryptionGetGpuIdentifiers__ = &knvlinkEncryptionGetGpuIdentifiers_46f6a7;
    }

    // knvlinkEncryptionUpdateTopology -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x60000000UL) ) ||
        ( ((chipHal_HalVarIdx >> 5) == 2UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000006UL) )) /* ChipHal: GB100 | GB102 | GB110 | GB112 */ 
    {
        pThis->__knvlinkEncryptionUpdateTopology__ = &knvlinkEncryptionUpdateTopology_GB100;
    }
    // default
    else
    {
        pThis->__knvlinkEncryptionUpdateTopology__ = &knvlinkEncryptionUpdateTopology_46f6a7;
    }
} // End __nvoc_init_funcTable_KernelNvlink_1 with approximately 90 basic block(s).


// Initialize vtable(s) for 54 virtual method(s).
void __nvoc_init_funcTable_KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 41 per-object function pointer(s).
    __nvoc_init_funcTable_KernelNvlink_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelNvlink(KernelNvlink *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_pbase_KernelNvlink = pThis;    // (knvlink) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelNvlink.metadata__OBJENGSTATE.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr = &__nvoc_metadata__KernelNvlink.metadata__OBJENGSTATE;    // (engstate) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelNvlink;    // (knvlink) this

    // Initialize per-object vtables.
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

    __nvoc_init__KernelNvlink(pThis, pRmhalspecowner);
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

