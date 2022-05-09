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

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStatePreUnload(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_MemoryManager_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager, NvU32 arg0) {
    return memmgrStateUnload(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static void __nvoc_thunk_MemoryManager_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pMemoryManager) {
    memmgrStateDestroy(pGpu, (struct MemoryManager *)(((unsigned char *)pMemoryManager) - __nvoc_rtti_MemoryManager_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrReconcileTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), pTunableState);
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

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrGetTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrCompareTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_memmgrFreeTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrStatePostLoad(POBJGPU pGpu, struct MemoryManager *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrAllocTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_memmgrSetTunableState(POBJGPU pGpu, struct MemoryManager *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_MemoryManager_OBJENGSTATE.offset), pTunableState);
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bFbRegionsSupported = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bFbRegionsSupported = ((NvBool)(0 != 0));
    }

    // Hal field -- bPmaEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bPmaEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bPmaEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bClientPageTablesPmaManaged
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bClientPageTablesPmaManaged = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bClientPageTablesPmaManaged = ((NvBool)(0 != 0));
    }

    // Hal field -- bScanoutSysmem
    if (0)
    {
    }
    // default
    else
    {
        pThis->bScanoutSysmem = ((NvBool)(0 != 0));
    }

    // Hal field -- bDisallowSplitLowerMemory
    if (0)
    {
    }
    // default
    else
    {
        pThis->bDisallowSplitLowerMemory = ((NvBool)(0 != 0));
    }

    // Hal field -- bSmallPageCompression
    if (0)
    {
    }
    // default
    else
    {
        pThis->bSmallPageCompression = ((NvBool)(0 != 0));
    }

    // Hal field -- bSysmemCompressionSupportDef
    if (0)
    {
    }
    // default
    else
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
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bVgpuPmaSupport = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bVgpuPmaSupport = ((NvBool)(0 != 0));
    }

    pThis->bSupportCCProtectedMemoryAlloc = ((NvBool)(0 != 0));

    // Hal field -- bAllowNoncontiguousAllocation
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bAllowNoncontiguousAllocation = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bAllowNoncontiguousAllocation = ((NvBool)(0 != 0));
    }

    // Hal field -- bScrubOnFreeEnabled
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bScrubOnFreeEnabled = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bScrubOnFreeEnabled = ((NvBool)(0 != 0));
    }

    // Hal field -- bFastScrubberEnabled
    if (0)
    {
    }
    // default
    else
    {
        pThis->bFastScrubberEnabled = ((NvBool)(0 != 0));
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

    pThis->__memmgrStatePreUnload__ = &memmgrStatePreUnload_IMPL;

    pThis->__memmgrStateUnload__ = &memmgrStateUnload_IMPL;

    pThis->__memmgrStateDestroy__ = &memmgrStateDestroy_IMPL;

    // Hal function -- memmgrAllocDetermineAlignment
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrAllocDetermineAlignment__ = &memmgrAllocDetermineAlignment_GA100;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- memmgrGetMaxContextSize
    if (0)
    {
    }
    else if (0)
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
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrGetMaxContextSize__ = &memmgrGetMaxContextSize_GA100;
    }
    else if (0)
    {
    }
    else if (0)
    {
    }

    // Hal function -- memmgrScrubRegistryOverrides
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrScrubRegistryOverrides__ = &memmgrScrubRegistryOverrides_GA100;
    }
    else if (0)
    {
    }

    // Hal function -- memmgrGetFlaKind
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetFlaKind__ = &memmgrGetFlaKind_46f6a7;
    }

    // Hal function -- memmgrDetermineComptag
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrDetermineComptag__ = &memmgrDetermineComptag_TU102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
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
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrReadMmuLock__ = &memmgrReadMmuLock_e133c0;
    }

    // Hal function -- memmgrBlockMemLockedMemory
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrBlockMemLockedMemory__ = &memmgrBlockMemLockedMemory_56cd7a;
    }

    // Hal function -- memmgrInsertUnprotectedRegionAtBottomOfFb
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fbe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrInsertUnprotectedRegionAtBottomOfFb__ = &memmgrInsertUnprotectedRegionAtBottomOfFb_56cd7a;
    }

    // Hal function -- memmgrGetDisablePlcKind
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetDisablePlcKind__ = &memmgrGetDisablePlcKind_b3696a;
    }

    // Hal function -- memmgrEnableDynamicPageOfflining
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_GA102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrEnableDynamicPageOfflining__ = &memmgrEnableDynamicPageOfflining_b3696a;
    }

    // Hal function -- memmgrGetBlackListPages
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GM107;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__memmgrGetBlackListPages__ = &memmgrGetBlackListPages_GA100;
    }
    else if (0)
    {
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_MemoryManager_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_thunk_MemoryManager_engstateStatePreInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_MemoryManager_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_thunk_MemoryManager_engstateStateLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_thunk_MemoryManager_engstateStatePreUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_thunk_MemoryManager_engstateStateUnload;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_MemoryManager_engstateStateDestroy;

    pThis->__memmgrReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrReconcileTunableState;

    pThis->__memmgrStatePreLoad__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePreLoad;

    pThis->__memmgrStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePostUnload;

    pThis->__memmgrStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_memmgrStateInitUnlocked;

    pThis->__memmgrInitMissing__ = &__nvoc_thunk_OBJENGSTATE_memmgrInitMissing;

    pThis->__memmgrStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePreInitUnlocked;

    pThis->__memmgrGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrGetTunableState;

    pThis->__memmgrCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrCompareTunableState;

    pThis->__memmgrFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrFreeTunableState;

    pThis->__memmgrStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_memmgrStatePostLoad;

    pThis->__memmgrAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrAllocTunableState;

    pThis->__memmgrSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_memmgrSetTunableState;

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

    pThis = portMemAllocNonPaged(sizeof(MemoryManager));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(MemoryManager));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MemoryManager);

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
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MemoryManager(MemoryManager **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_MemoryManager(ppThis, pParent, createFlags);

    return status;
}

