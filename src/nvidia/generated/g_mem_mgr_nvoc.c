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

static NV_STATUS __nvoc_thunk_MemoryManager_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, ENGDESCRIPTOR arg0) {
    return memmgrConstructEngine(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStatePreInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    return memmgrStateInitLocked(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStateLoad(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStatePostLoad(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStatePreUnload(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStateUnload(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_MemoryManager_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    memmgrStateDestroy(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrStatePreLoad(POBJGPU pGpu, struct MemoryManager *pEngstate, NvU32 arg0) {
    return engstateStatePreLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrStatePostUnload(POBJGPU pGpu, struct MemoryManager *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrStateInitUnlocked(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_memmgrInitMissing(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NvBool __nvoc_thunk_OBJENGSTATE_memmgrIsPresent(POBJGPU pGpu, struct MemoryManager *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bFbRegionsSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- bPmaEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bPmaEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bClientPageTablesPmaManaged
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bAllowNoncontiguousAllocation = ((NvBool)(0 == 0));
    }

    // Hal field -- bLocalEgmSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bLocalEgmSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bLocalEgmSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bScrubOnFreeEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bScrubOnFreeEnabled = ((NvBool)(0 == 0));
    }

    // Hal field -- bFastScrubberEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bFastScrubberEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bFastScrubberEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bMonitoredFenceSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bMonitoredFenceSupported = ((NvBool)(0 == 0));
    }

    // Hal field -- b64BitSemaphoresSupported
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->b64BitSemaphoresSupported = ((NvBool)(0 == 0));
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->b64BitSemaphoresSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bGenericKindSupport
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->bGenericKindSupport = ((NvBool)(0 == 0));
    }

    // Hal field -- bLocalizedMemPoolEnabled
    // default
    {
        pThis->bLocalizedMemPoolEnabled = ((NvBool)(0 != 0));
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

    pThis->__memmgrConstructEngine__ = &memmgrConstructEngine_IMPL;

    pThis->__memmgrStatePreInitLocked__ = &memmgrStatePreInitLocked_IMPL;

    pThis->__memmgrStateInitLocked__ = &memmgrStateInitLocked_IMPL;

    pThis->__memmgrStateLoad__ = &memmgrStateLoad_IMPL;

    pThis->__memmgrStatePostLoad__ = &memmgrStatePostLoad_IMPL;

    pThis->__memmgrStatePreUnload__ = &memmgrStatePreUnload_IMPL;

    pThis->__memmgrStateUnload__ = &memmgrStateUnload_IMPL;

    pThis->__memmgrStateDestroy__ = &memmgrStateDestroy_IMPL;

    // Hal function -- memmgrMemUtilsSec2CtxInit
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_GH100;
    }
    // default
    else
    {
        pThis->__memmgrMemUtilsSec2CtxInit__ = &memmgrMemUtilsSec2CtxInit_46f6a7;
    }

    // Hal function -- memmgrMemUtilsCheckMemoryFastScrubEnable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_GH100;
    }
    else
    {
        pThis->__memmgrMemUtilsCheckMemoryFastScrubEnable__ = &memmgrMemUtilsCheckMemoryFastScrubEnable_491d52;
    }

    // Hal function -- memmgrAllocDetermineAlignment
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GM107;
    }
    else
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GA100;
    }

    // Hal function -- memmgrGetMaxContextSize
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

    // Hal function -- memmgrScrubRegistryOverrides
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GM107;
    }
    else
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GA100;
    }

    // Hal function -- memmgrGetPteKindBl
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_474d46;
    }
    else
    {
        pThis->__memmgrGetPteKindBl__ = &memmgrGetPteKindBl_GM107;
    }

    // Hal function -- memmgrGetPteKindPitch
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_474d46;
    }
    else
    {
        pThis->__memmgrGetPteKindPitch__ = &memmgrGetPteKindPitch_GM107;
    }

    // Hal function -- memmgrChooseKindCompressC
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_474d46;
    }
    else
    {
        pThis->__memmgrChooseKindCompressC__ = &memmgrChooseKindCompressC_GP100;
    }

    // Hal function -- memmgrGetFlaKind
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_46f6a7;
    }
    else
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_GA100;
    }

    // Hal function -- memmgrIsApertureSupportedByFla
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x11f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 */ 
    {
        pThis->__memmgrIsApertureSupportedByFla__ = &memmgrIsApertureSupportedByFla_GA100;
    }
    // default
    else
    {
        pThis->__memmgrIsApertureSupportedByFla__ = &memmgrIsApertureSupportedByFla_46f6a7;
    }

    // Hal function -- memmgrDetermineComptag
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrDetermineComptag__ = &memmgrDetermineComptag_TU102;
    }
    else
    {
        pThis->__memmgrDetermineComptag__ = &memmgrDetermineComptag_13cd8d;
    }

    // Hal function -- memmgrCheckReservedMemorySize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrCheckReservedMemorySize__ = &memmgrCheckReservedMemorySize_GK104;
    }
    // default
    else
    {
        pThis->__memmgrCheckReservedMemorySize__ = &memmgrCheckReservedMemorySize_56cd7a;
    }

    // Hal function -- memmgrReadMmuLock
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_GA100;
    }
    else
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_e133c0;
    }

    // Hal function -- memmgrBlockMemLockedMemory
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_GA100;
    }
    else
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_56cd7a;
    }

    // Hal function -- memmgrInsertUnprotectedRegionAtBottomOfFb
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_GA100;
    }
    else
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_56cd7a;
    }

    // Hal function -- memmgrGetDisablePlcKind
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_b3696a;
    }
    else
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_GA100;
    }

    // Hal function -- memmgrEnableDynamicPageOfflining
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

    // Hal function -- memmgrGetBlackListPages
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GM107;
    }
    else
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GA100;
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_MemoryManager_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_thunk_MemoryManager_engstateStatePreInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_MemoryManager_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_MemoryManager_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_thunk_MemoryManager_engstateStatePostLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_MemoryManager_engstateStatePreUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_MemoryManager_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_MemoryManager_engstateStateDestroy;

    pThis->__memmgrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePreLoad;

    pThis->__memmgrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePostUnload;

    pThis->__memmgrStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_memmgrStateInitUnlocked;

    pThis->__memmgrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_memmgrInitMissing;

    pThis->__memmgrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked;

    pThis->__memmgrIsPresent__ = &__nvoc_thunk_OBJENGSTATE_memmgrIsPresent;
}

void __nvoc_init_funcTable_MemoryManager(MemoryManager *pThis, RmHalspecOwner *pRmhalspecowner) {
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

NV_STATUS __nvoc_objCreate_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    MemoryManager *pThis;
    RmHalspecOwner *pRmhalspecowner;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MemoryManager), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(MemoryManager));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryManager);

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

    __nvoc_init_MemoryManager(pThis, pRmhalspecowner);
    status = __nvoc_ctor_MemoryManager(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_MemoryManager_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MemoryManager_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MemoryManager));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_MemoryManager(ppThis, pParent, createFlags);

    return status;
}

