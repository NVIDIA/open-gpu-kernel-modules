#define NVOC_MEM_MGR_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mem_mgr_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x22ad47 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryManager;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_MemoryManager(MemoryManager*, RmHalspecOwner* );
void __nvoc_init_funcTable_MemoryManager(MemoryManager*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_MemoryManager(MemoryManager*, RmHalspecOwner* );
void __nvoc_init_dataField_MemoryManager(MemoryManager*, RmHalspecOwner* );
void __nvoc_dtor_MemoryManager(MemoryManager*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryManager;

static const struct NVOC_RTTI __nvoc_rtti_MemoryManager_MemoryManager = {
    /*pClassDef=*/          &__nvoc_class_def_MemoryManager,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MemoryManager,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryManager_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MemoryManager_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MemoryManager, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MemoryManager = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_MemoryManager_MemoryManager,
        &__nvoc_rtti_MemoryManager_OBJENGSTATE,
        &__nvoc_rtti_MemoryManager_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryManager = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MemoryManager),
        /*classId=*/            classId(MemoryManager),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MemoryManager",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MemoryManager,
    /*pCastInfo=*/          &__nvoc_castinfo_MemoryManager,
    /*pExportInfo=*/        &__nvoc_export_info_MemoryManager
};

// 8 down-thunk(s) defined to bridge methods in MemoryManager from superclasses

// memmgrConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, ENGDESCRIPTOR arg3) {
    return memmgrConstructEngine(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStatePreInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStatePreInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

// memmgrStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStateInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

// memmgrStateLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStateLoad(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStatePostLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStatePostLoad(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStatePreUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStatePreUnload(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_MemoryManager_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg3) {
    return memmgrStateUnload(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_MemoryManager_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    memmgrStateDestroy(pGpu, (struct MemoryManager *)(((unsigned char *) pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}


// 6 up-thunk(s) defined to bridge methods in MemoryManager to superclasses

// memmgrInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_memmgrInitMissing(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

// memmgrStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

// memmgrStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStateInitUnlocked(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

// memmgrStatePreLoad: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePreLoad(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_memmgrStatePostUnload(struct OBJGPU *pGpu, struct MemoryManager *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg3);
}

// memmgrIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_memmgrIsPresent(struct OBJGPU *pGpu, struct MemoryManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MemoryManager = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_MemoryManager(MemoryManager *pThis) {
    __nvoc_memmgrDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MemoryManager(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bFbRegionsSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bFbRegionsSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bPmaEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bPmaEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bClientPageTablesPmaManaged
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bClientPageTablesPmaManaged = ((NvBool)(0 == 0));
    }

    // Hal field -- bScanoutSysmem
    // default
    {
        pThis->bScanoutSysmem = ((NvBool)(0 != 0));
    }

    // Hal field -- bDisallowSplitLowerMemory
    // default
    {
        pThis->bDisallowSplitLowerMemory = ((NvBool)(0 != 0));
    }

    // Hal field -- bSmallPageCompression
    // default
    {
        pThis->bSmallPageCompression = ((NvBool)(0 != 0));
    }

    // Hal field -- bSysmemCompressionSupportDef
    // default
    {
        pThis->bSysmemCompressionSupportDef = ((NvBool)(0 != 0));
    }

    // Hal field -- bBug2301372IncreaseRmReserveMemoryWar
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->bBug2301372IncreaseRmReserveMemoryWar = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBug2301372IncreaseRmReserveMemoryWar = ((NvBool)(0 != 0));
    }

    pThis->bEnableDynamicPageOfflining = ((NvBool)(0 != 0));

    // Hal field -- bVgpuPmaSupport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bVgpuPmaSupport = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bVgpuPmaSupport = ((NvBool)(0 != 0));
    }

    pThis->bScrubChannelSetupInProgress = ((NvBool)(0 != 0));

    // Hal field -- bBug3922001DisableCtxBufOnSim
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bBug3922001DisableCtxBufOnSim = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBug3922001DisableCtxBufOnSim = ((NvBool)(0 != 0));
    }

    pThis->bEnableDynamicGranularityPageArrays = ((NvBool)(0 != 0));

    // Hal field -- bAllowNoncontiguousAllocation
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bAllowNoncontiguousAllocation = ((NvBool)(0 == 0));
    }

    // Hal field -- bLocalEgmSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->bLocalEgmSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bLocalEgmSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bScrubOnFreeEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bScrubOnFreeEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bFastScrubberEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->bFastScrubberEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bFastScrubberEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bMonitoredFenceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bMonitoredFenceSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- b64BitSemaphoresSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->b64BitSemaphoresSupported = ((NvBool)(0 == 0));
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->b64BitSemaphoresSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bGenericKindSupport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bGenericKindSupport = ((NvBool)(0 == 0));
    }
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_MemoryManager(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_MemoryManager_fail_OBJENGSTATE;
    __nvoc_init_dataField_MemoryManager(pThis, pRmhalspecowner);
    goto __nvoc_ctor_MemoryManager_exit; // Success

__nvoc_ctor_MemoryManager_fail_OBJENGSTATE:
__nvoc_ctor_MemoryManager_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MemoryManager_1(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // memmgrConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__memmgrConstructEngine__ = &memmgrConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_MemoryManager_engstateConstructEngine;

    // memmgrStatePreInitLocked -- virtual override (engstate) base (engstate)
    pThis->__memmgrStatePreInitLocked__ = &memmgrStatePreInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_MemoryManager_engstateStatePreInitLocked;

    // memmgrStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__memmgrStateInitLocked__ = &memmgrStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_MemoryManager_engstateStateInitLocked;

    // memmgrStateLoad -- virtual override (engstate) base (engstate)
    pThis->__memmgrStateLoad__ = &memmgrStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_MemoryManager_engstateStateLoad;

    // memmgrStatePostLoad -- virtual override (engstate) base (engstate)
    pThis->__memmgrStatePostLoad__ = &memmgrStatePostLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_MemoryManager_engstateStatePostLoad;

    // memmgrStatePreUnload -- virtual override (engstate) base (engstate)
    pThis->__memmgrStatePreUnload__ = &memmgrStatePreUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_MemoryManager_engstateStatePreUnload;

    // memmgrStateUnload -- virtual override (engstate) base (engstate)
    pThis->__memmgrStateUnload__ = &memmgrStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_MemoryManager_engstateStateUnload;

    // memmgrStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__memmgrStateDestroy__ = &memmgrStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_MemoryManager_engstateStateDestroy;

    // memmgrAllocateConsoleRegion -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_56cd7a;
    }
    else
    {
        pThis->__memmgrAllocateConsoleRegion__ = &memmgrAllocateConsoleRegion_IMPL;
    }

    // memmgrMemUtilsSec2CtxInit -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_GH100;
    }
    // default
    else
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_46f6a7;
    }

    // memmgrMemUtilsCheckMemoryFastScrubEnable -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_GH100;
    }
    else
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_491d52;
    }

    // memmgrAllocDetermineAlignment -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GM107;
    }
    else
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GA100;
    }

    // memmgrGetMaxContextSize -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x01f00000UL) )) /* ChipHal: AD102 | AD103 | AD104 | AD106 | AD107 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_AD102;
    }
    else
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_GA100;
    }

    // memmgrGetFbTaxSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetFbTaxSize__ = &memmgrGetFbTaxSize_VF;
    }
    else
    {
        pThis->__memmgrGetFbTaxSize__ = &memmgrGetFbTaxSize_4a4dee;
    }

    // memmgrScrubRegistryOverrides -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GM107;
    }
    else
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GA100;
    }

    // memmgrGetPteKindBl -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_474d46;
    }
    else
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_GM107;
    }

    // memmgrGetPteKindPitch -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_474d46;
    }
    else
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_GM107;
    }

    // memmgrChooseKindCompressC -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_474d46;
    }
    else
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_GP100;
    }

    // memmgrGetFlaKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_46f6a7;
    }
    else
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_GA100;
    }

    // memmgrIsMemDescSupportedByFla -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_GB100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_GA100;
    }
    // default
    else
    {
        pThis->__memmgrIsMemDescSupportedByFla__ = &memmgrIsMemDescSupportedByFla_46f6a7;
    }

    // memmgrDetermineComptag -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrDetermineComptag__ = &memmgrDetermineComptag_TU102;
    }
    else
    {
        pThis->__memmgrDetermineComptag__ = &memmgrDetermineComptag_13cd8d;
    }

    // memmgrGetGrHeapReservationSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetGrHeapReservationSize__ = &memmgrGetGrHeapReservationSize_VF;
    }
    else
    {
        pThis->__memmgrGetGrHeapReservationSize__ = &memmgrGetGrHeapReservationSize_4a4dee;
    }

    // memmgrGetRunlistEntriesReservedFbSpace -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetRunlistEntriesReservedFbSpace__ = &memmgrGetRunlistEntriesReservedFbSpace_4a4dee;
    }
    else
    {
        pThis->__memmgrGetRunlistEntriesReservedFbSpace__ = &memmgrGetRunlistEntriesReservedFbSpace_GM107;
    }

    // memmgrGetUserdReservedFbSpace -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrGetUserdReservedFbSpace__ = &memmgrGetUserdReservedFbSpace_4a4dee;
    }
    else
    {
        pThis->__memmgrGetUserdReservedFbSpace__ = &memmgrGetUserdReservedFbSpace_GM107;
    }

    // memmgrCheckReservedMemorySize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrCheckReservedMemorySize__ = &memmgrCheckReservedMemorySize_GK104;
    }
    // default
    else
    {
        pThis->__memmgrCheckReservedMemorySize__ = &memmgrCheckReservedMemorySize_56cd7a;
    }

    // memmgrReadMmuLock -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_GA100;
    }
    else
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_e133c0;
    }

    // memmgrBlockMemLockedMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_GA100;
    }
    else
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_56cd7a;
    }

    // memmgrInsertUnprotectedRegionAtBottomOfFb -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_GA100;
    }
    else
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_56cd7a;
    }

    // memmgrInitBaseFbRegions -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrInitBaseFbRegions__ = &memmgrInitBaseFbRegions_VF;
    }
    else
    {
        pThis->__memmgrInitBaseFbRegions__ = &memmgrInitBaseFbRegions_FWCLIENT;
    }

    // memmgrGetDisablePlcKind -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_b3696a;
    }
    else
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_GA100;
    }

    // memmgrEnableDynamicPageOfflining -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_b3696a;
    }
    else
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_GA102;
    }

    // memmgrSetPartitionableMem -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrSetPartitionableMem__ = &memmgrSetPartitionableMem_56cd7a;
    }
    else
    {
        // default
        pThis->__memmgrSetPartitionableMem__ = &memmgrSetPartitionableMem_IMPL;
    }

    // memmgrAllocMIGGPUInstanceMemory -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrAllocMIGGPUInstanceMemory__ = &memmgrAllocMIGGPUInstanceMemory_VF;
    }
    else
    {
        pThis->__memmgrAllocMIGGPUInstanceMemory__ = &memmgrAllocMIGGPUInstanceMemory_PF;
    }

    // memmgrGetBlackListPages -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GM107;
    }
    else
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GA100;
    }

    // memmgrDiscoverMIGPartitionableMemoryRange -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__memmgrDiscoverMIGPartitionableMemoryRange__ = &memmgrDiscoverMIGPartitionableMemoryRange_VF;
    }
    else
    {
        pThis->__memmgrDiscoverMIGPartitionableMemoryRange__ = &memmgrDiscoverMIGPartitionableMemoryRange_46f6a7;
    }

    // memmgrGetFBEndReserveSizeEstimate -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__memmgrGetFBEndReserveSizeEstimate__ = &memmgrGetFBEndReserveSizeEstimate_GB100;
    }
    // default
    else
    {
        pThis->__memmgrGetFBEndReserveSizeEstimate__ = &memmgrGetFBEndReserveSizeEstimate_GM107;
    }

    // memmgrInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrInitMissing;

    // memmgrStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked;

    // memmgrStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStateInitUnlocked;

    // memmgrStatePreLoad -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrStatePreLoad__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePreLoad;

    // memmgrStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrStatePostUnload;

    // memmgrIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__memmgrIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_memmgrIsPresent;
} // End __nvoc_init_funcTable_MemoryManager_1 with approximately 81 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_MemoryManager(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 42 per-object function pointer(s).
    __nvoc_init_funcTable_MemoryManager_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_MemoryManager(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_MemoryManager = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_MemoryManager(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MemoryManager *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryManager), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MemoryManager));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryManager);

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

    __nvoc_init_MemoryManager(pThis, pRmhalspecowner);
    status = __nvoc_ctor_MemoryManager(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryManager_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryManager_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryManager));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_MemoryManager(ppThis, pParent, createFlags);

    return status;
}

