#define NVOC_KERN_MEM_SYS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kern_mem_sys_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x7faff1 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJENGSTATE;

void __nvoc_init_KernelMemorySystem(KernelMemorySystem*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelMemorySystem(KernelMemorySystem*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelMemorySystem(KernelMemorySystem*, RmHalspecOwner* );
void __nvoc_init_dataField_KernelMemorySystem(KernelMemorySystem*, RmHalspecOwner* );
void __nvoc_dtor_KernelMemorySystem(KernelMemorySystem*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMemorySystem;

static const struct NVOC_RTTI __nvoc_rtti_KernelMemorySystem_KernelMemorySystem = {
    /*pClassDef=*/          &__nvoc_class_def_KernelMemorySystem,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelMemorySystem,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMemorySystem_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelMemorySystem_OBJENGSTATE = {
    /*pClassDef=*/          &__nvoc_class_def_OBJENGSTATE,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelMemorySystem, __nvoc_base_OBJENGSTATE),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelMemorySystem = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_KernelMemorySystem_KernelMemorySystem,
        &__nvoc_rtti_KernelMemorySystem_OBJENGSTATE,
        &__nvoc_rtti_KernelMemorySystem_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMemorySystem = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelMemorySystem),
        /*classId=*/            classId(KernelMemorySystem),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelMemorySystem",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelMemorySystem,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelMemorySystem,
    /*pExportInfo=*/        &__nvoc_export_info_KernelMemorySystem
};

static NV_STATUS __nvoc_thunk_KernelMemorySystem_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, ENGDESCRIPTOR arg0) {
    return kmemsysConstructEngine(pGpu, (struct KernelMemorySystem *)(((unsigned char *)pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_KernelMemorySystem_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    return kmemsysStateInitLocked(pGpu, (struct KernelMemorySystem *)(((unsigned char *)pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_KernelMemorySystem_engstateStatePreLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePreLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *)pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

static void __nvoc_thunk_KernelMemorySystem_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    kmemsysStateDestroy(pGpu, (struct KernelMemorySystem *)(((unsigned char *)pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysReconcileTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return engstateReconcileTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStateLoad(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return engstateStateLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStateUnload(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return engstateStateUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStatePostUnload(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStatePreUnload(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return engstateStatePreUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStateInitUnlocked(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static void __nvoc_thunk_OBJENGSTATE_kmemsysInitMissing(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStatePreInitLocked(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStatePreInitLocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysGetTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return engstateGetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysCompareTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunables1, void *pTunables2) {
    return engstateCompareTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), pTunables1, pTunables2);
}

static void __nvoc_thunk_OBJENGSTATE_kmemsysFreeTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    engstateFreeTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), pTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysStatePostLoad(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg0) {
    return engstateStatePostLoad(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg0);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysAllocTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void **ppTunableState) {
    return engstateAllocTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), ppTunableState);
}

static NV_STATUS __nvoc_thunk_OBJENGSTATE_kmemsysSetTunableState(POBJGPU pGpu, struct KernelMemorySystem *pEngstate, void *pTunableState) {
    return engstateSetTunableState(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), pTunableState);
}

static NvBool __nvoc_thunk_OBJENGSTATE_kmemsysIsPresent(POBJGPU pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *)pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelMemorySystem = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_dtor_KernelMemorySystem(KernelMemorySystem *pThis) {
    __nvoc_kmemsysDestruct(pThis);
    __nvoc_dtor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelMemorySystem(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    // Hal field -- bDisableTiledCachingInvalidatesWithEccBug1521641
    if (0)
    {
    }
    // default
    else
    {
        pThis->bDisableTiledCachingInvalidatesWithEccBug1521641 = ((NvBool)(0 != 0));
    }

    // Hal field -- bGpuCacheEnable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bGpuCacheEnable = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bGpuCacheEnable = ((NvBool)(0 != 0));
    }

    // Hal field -- bNumaNodesAdded
    pThis->bNumaNodesAdded = ((NvBool)(0 != 0));

    // Hal field -- bL2CleanFbPull
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->bL2CleanFbPull = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bL2CleanFbPull = ((NvBool)(0 != 0));
    }

    // Hal field -- bPreserveComptagBackingStoreOnSuspend
    pThis->bPreserveComptagBackingStoreOnSuspend = ((NvBool)(0 != 0));
}

NV_STATUS __nvoc_ctor_OBJENGSTATE(OBJENGSTATE* );
NV_STATUS __nvoc_ctor_KernelMemorySystem(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    if (status != NV_OK) goto __nvoc_ctor_KernelMemorySystem_fail_OBJENGSTATE;
    __nvoc_init_dataField_KernelMemorySystem(pThis, pRmhalspecowner);
    goto __nvoc_ctor_KernelMemorySystem_exit; // Success

__nvoc_ctor_KernelMemorySystem_fail_OBJENGSTATE:
__nvoc_ctor_KernelMemorySystem_exit:

    return status;
}

static void __nvoc_init_funcTable_KernelMemorySystem_1(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {
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

    pThis->__kmemsysConstructEngine__ = &kmemsysConstructEngine_IMPL;

    pThis->__kmemsysStateInitLocked__ = &kmemsysStateInitLocked_IMPL;

    pThis->__kmemsysStatePreLoad__ = &kmemsysStatePreLoad_IMPL;

    pThis->__kmemsysStateDestroy__ = &kmemsysStateDestroy_IMPL;

    // Hal function -- kmemsysGetFbNumaInfo
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_GV100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_56cd7a;
    }

    // Hal function -- kmemsysReadUsableFbSize
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GP102;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GA102;
    }
    // default
    else
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_5baef9;
    }

    // Hal function -- kmemsysInitFlushSysmemBuffer
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GM107;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GA100;
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
    }

    // Hal function -- kmemsysProgramSysmemFlushBuffer
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GM107;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GA100;
        }
        else if (0)
        {
        }
        else if (0)
        {
        }
    }

    // Hal function -- kmemsysIsPagePLCable
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_GA100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000f800UL) )) /* ChipHal: GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_GA102;
        }
        // default
        else
        {
            pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_510167;
        }
    }

    // Hal function -- kmemsysReadMIGMemoryCfg
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x0000fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 */ 
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_GA100;
        }
        // default
        else
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_46f6a7;
        }
    }

    // Hal function -- kmemsysInitMIGMemoryPartitionTable
    if (0)
    {
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x00000400UL) )) /* ChipHal: GA100 */ 
        {
            pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_GA100;
        }
        // default
        else
        {
            pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_56cd7a;
        }
    }

    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_thunk_KernelMemorySystem_engstateConstructEngine;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_thunk_KernelMemorySystem_engstateStateInitLocked;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__ = &__nvoc_thunk_KernelMemorySystem_engstateStatePreLoad;

    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_thunk_KernelMemorySystem_engstateStateDestroy;

    pThis->__kmemsysReconcileTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysReconcileTunableState;

    pThis->__kmemsysStateLoad__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStateLoad;

    pThis->__kmemsysStateUnload__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStateUnload;

    pThis->__kmemsysStatePostUnload__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStatePostUnload;

    pThis->__kmemsysStatePreUnload__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStatePreUnload;

    pThis->__kmemsysStateInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStateInitUnlocked;

    pThis->__kmemsysInitMissing__ = &__nvoc_thunk_OBJENGSTATE_kmemsysInitMissing;

    pThis->__kmemsysStatePreInitLocked__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStatePreInitLocked;

    pThis->__kmemsysStatePreInitUnlocked__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked;

    pThis->__kmemsysGetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysGetTunableState;

    pThis->__kmemsysCompareTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysCompareTunableState;

    pThis->__kmemsysFreeTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysFreeTunableState;

    pThis->__kmemsysStatePostLoad__ = &__nvoc_thunk_OBJENGSTATE_kmemsysStatePostLoad;

    pThis->__kmemsysAllocTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysAllocTunableState;

    pThis->__kmemsysSetTunableState__ = &__nvoc_thunk_OBJENGSTATE_kmemsysSetTunableState;

    pThis->__kmemsysIsPresent__ = &__nvoc_thunk_OBJENGSTATE_kmemsysIsPresent;
}

void __nvoc_init_funcTable_KernelMemorySystem(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_KernelMemorySystem_1(pThis, pRmhalspecowner);
}

void __nvoc_init_OBJENGSTATE(OBJENGSTATE*);
void __nvoc_init_KernelMemorySystem(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelMemorySystem = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object;
    pThis->__nvoc_pbase_OBJENGSTATE = &pThis->__nvoc_base_OBJENGSTATE;
    __nvoc_init_OBJENGSTATE(&pThis->__nvoc_base_OBJENGSTATE);
    __nvoc_init_funcTable_KernelMemorySystem(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    KernelMemorySystem *pThis;
    RmHalspecOwner *pRmhalspecowner;

    pThis = portMemAllocNonPaged(sizeof(KernelMemorySystem));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(KernelMemorySystem));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMemorySystem);

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

    __nvoc_init_KernelMemorySystem(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMemorySystem(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMemorySystem_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_KernelMemorySystem_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMemorySystem(ppThis, pParent, createFlags);

    return status;
}

