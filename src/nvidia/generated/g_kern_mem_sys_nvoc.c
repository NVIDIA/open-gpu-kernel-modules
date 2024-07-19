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

// 9 down-thunk(s) defined to bridge methods in KernelMemorySystem from superclasses

// kmemsysConstructEngine: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateConstructEngine(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, ENGDESCRIPTOR arg3) {
    return kmemsysConstructEngine(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg3);
}

// kmemsysStatePreInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    return kmemsysStatePreInitLocked(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

// kmemsysStateInitLocked: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateInitLocked(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    return kmemsysStateInitLocked(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

// kmemsysStatePreLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePreLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

// kmemsysStatePostLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePostLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePostLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

// kmemsysStateLoad: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateLoad(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStateLoad(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

// kmemsysStatePreUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStatePreUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStatePreUnload(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

// kmemsysStateUnload: virtual override (engstate) base (engstate)
static NV_STATUS __nvoc_down_thunk_KernelMemorySystem_engstateStateUnload(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem, NvU32 flags) {
    return kmemsysStateUnload(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), flags);
}

// kmemsysStateDestroy: virtual override (engstate) base (engstate)
static void __nvoc_down_thunk_KernelMemorySystem_engstateStateDestroy(OBJGPU *pGpu, struct OBJENGSTATE *pKernelMemorySystem) {
    kmemsysStateDestroy(pGpu, (struct KernelMemorySystem *)(((unsigned char *) pKernelMemorySystem) - __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}


// 5 up-thunk(s) defined to bridge methods in KernelMemorySystem to superclasses

// kmemsysInitMissing: virtual inherited (engstate) base (engstate)
static void __nvoc_up_thunk_OBJENGSTATE_kmemsysInitMissing(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    engstateInitMissing(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

// kmemsysStatePreInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStatePreInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

// kmemsysStateInitUnlocked: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStateInitUnlocked(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateStateInitUnlocked(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
}

// kmemsysStatePostUnload: virtual inherited (engstate) base (engstate)
static NV_STATUS __nvoc_up_thunk_OBJENGSTATE_kmemsysStatePostUnload(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate, NvU32 arg3) {
    return engstateStatePostUnload(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset), arg3);
}

// kmemsysIsPresent: virtual inherited (engstate) base (engstate)
static NvBool __nvoc_up_thunk_OBJENGSTATE_kmemsysIsPresent(struct OBJGPU *pGpu, struct KernelMemorySystem *pEngstate) {
    return engstateIsPresent(pGpu, (struct OBJENGSTATE *)(((unsigned char *) pEngstate) + __nvoc_rtti_KernelMemorySystem_OBJENGSTATE.offset));
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
    // default
    {
        pThis->bDisableTiledCachingInvalidatesWithEccBug1521641 = ((NvBool)(0 != 0));
    }

    // Hal field -- bGpuCacheEnable
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bGpuCacheEnable = ((NvBool)(0 == 0));
    }

    // Hal field -- bNumaNodesAdded
    pThis->bNumaNodesAdded = ((NvBool)(0 != 0));

    // Hal field -- bL2CleanFbPull
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->bL2CleanFbPull = ((NvBool)(0 == 0));
    }

    // Hal field -- l2WriteMode
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0ffe0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
    {
        pThis->l2WriteMode = FB_CACHE_WRITE_MODE_WRITETHROUGH;
    }

    // Hal field -- bPreserveComptagBackingStoreOnSuspend
    pThis->bPreserveComptagBackingStoreOnSuspend = ((NvBool)(0 != 0));

    // Hal field -- bBug3656943WAR
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->bBug3656943WAR = ((NvBool)(0 == 0));
    }
    // default
    else
    {
        pThis->bBug3656943WAR = ((NvBool)(0 != 0));
    }

    // Hal field -- overrideToGMK
    pThis->overrideToGMK = 0;

    // Hal field -- bDisablePlcForCertainOffsetsBug3046774
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->bDisablePlcForCertainOffsetsBug3046774 = ((NvBool)(0 != 0));
    }
    else if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000002UL) )) /* RmVariantHal: PF_KERNEL_ONLY */ 
    {
        pThis->bDisablePlcForCertainOffsetsBug3046774 = ((NvBool)(0 != 0));
    }
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

// Vtable initialization
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

    // kmemsysConstructEngine -- virtual override (engstate) base (engstate)
    pThis->__kmemsysConstructEngine__ = &kmemsysConstructEngine_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__ = &__nvoc_down_thunk_KernelMemorySystem_engstateConstructEngine;

    // kmemsysStatePreInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStatePreInitLocked__ = &kmemsysStatePreInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreInitLocked;

    // kmemsysStateInitLocked -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStateInitLocked__ = &kmemsysStateInitLocked_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateInitLocked;

    // kmemsysStatePreLoad -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStatePreLoad__ = &kmemsysStatePreLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreLoad;

    // kmemsysStatePostLoad -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStatePostLoad__ = &kmemsysStatePostLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePostLoad;

    // kmemsysStateLoad -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStateLoad__ = &kmemsysStateLoad_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateLoad__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateLoad;

    // kmemsysStatePreUnload -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStatePreUnload__ = &kmemsysStatePreUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStatePreUnload;

    // kmemsysStateUnload -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStateUnload__ = &kmemsysStateUnload_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateUnload__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateUnload;

    // kmemsysStateDestroy -- virtual override (engstate) base (engstate)
    pThis->__kmemsysStateDestroy__ = &kmemsysStateDestroy_IMPL;
    pThis->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__ = &__nvoc_down_thunk_KernelMemorySystem_engstateStateDestroy;

    // kmemsysGetFbNumaInfo -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_56cd7a;
    }
    else
    {
        pThis->__kmemsysGetFbNumaInfo__ = &kmemsysGetFbNumaInfo_GV100;
    }

    // kmemsysReadUsableFbSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000007e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 | GA100 */ 
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GP102;
    }
    else
    {
        pThis->__kmemsysReadUsableFbSize__ = &kmemsysReadUsableFbSize_GA102;
    }

    // kmemsysGetUsableFbSize -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetUsableFbSize__ = &kmemsysGetUsableFbSize_5baef9;
    }
    else
    {
        pThis->__kmemsysGetUsableFbSize__ = &kmemsysGetUsableFbSize_KERNEL;
    }

    // kmemsysCacheOp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysCacheOp__ = &kmemsysCacheOp_GH100;
    }
    else
    {
        pThis->__kmemsysCacheOp__ = &kmemsysCacheOp_GM200;
    }

    // kmemsysDoCacheOp -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysDoCacheOp__ = &kmemsysDoCacheOp_GH100;
    }
    else
    {
        pThis->__kmemsysDoCacheOp__ = &kmemsysDoCacheOp_GM107;
    }

    // kmemsysReadL2SysmemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysReadL2SysmemInvalidateReg__ = &kmemsysReadL2SysmemInvalidateReg_68b109;
    }
    else
    {
        pThis->__kmemsysReadL2SysmemInvalidateReg__ = &kmemsysReadL2SysmemInvalidateReg_TU102;
    }

    // kmemsysWriteL2SysmemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysWriteL2SysmemInvalidateReg__ = &kmemsysWriteL2SysmemInvalidateReg_f2d351;
    }
    else
    {
        pThis->__kmemsysWriteL2SysmemInvalidateReg__ = &kmemsysWriteL2SysmemInvalidateReg_TU102;
    }

    // kmemsysReadL2PeermemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysReadL2PeermemInvalidateReg__ = &kmemsysReadL2PeermemInvalidateReg_68b109;
    }
    else
    {
        pThis->__kmemsysReadL2PeermemInvalidateReg__ = &kmemsysReadL2PeermemInvalidateReg_TU102;
    }

    // kmemsysWriteL2PeermemInvalidateReg -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysWriteL2PeermemInvalidateReg__ = &kmemsysWriteL2PeermemInvalidateReg_f2d351;
    }
    else
    {
        pThis->__kmemsysWriteL2PeermemInvalidateReg__ = &kmemsysWriteL2PeermemInvalidateReg_TU102;
    }

    // kmemsysInitFlushSysmemBuffer -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_56cd7a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GM107;
        }
        else
        {
            pThis->__kmemsysInitFlushSysmemBuffer__ = &kmemsysInitFlushSysmemBuffer_GA100;
        }
    }

    // kmemsysProgramSysmemFlushBuffer -- halified (5 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_b3696a;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GH100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GB100;
        }
        else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GM107;
        }
        else
        {
            pThis->__kmemsysProgramSysmemFlushBuffer__ = &kmemsysProgramSysmemFlushBuffer_GA100;
        }
    }

    // kmemsysGetFlushSysmemBufferAddrShift -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_4a4dee;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_GB100;
        }
        else
        {
            pThis->__kmemsysGetFlushSysmemBufferAddrShift__ = &kmemsysGetFlushSysmemBufferAddrShift_GM107;
        }
    }

    // kmemsysIsPagePLCable -- halified (4 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysIsPagePLCable__ = &kmemsysIsPagePLCable_cbe027;
    }
    else
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

    // kmemsysReadMIGMemoryCfg -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_46f6a7;
    }
    else
    {
        if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd1f0fc00UL) )) /* ChipHal: GA100 | GA102 | GA103 | GA104 | GA106 | GA107 | AD102 | AD103 | AD104 | AD106 | AD107 | GH100 | GB100 | GB102 */ 
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_GA100;
        }
        // default
        else
        {
            pThis->__kmemsysReadMIGMemoryCfg__ = &kmemsysReadMIGMemoryCfg_46f6a7;
        }
    }

    // kmemsysInitMIGMemoryPartitionTable -- halified (3 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysInitMIGMemoryPartitionTable__ = &kmemsysInitMIGMemoryPartitionTable_56cd7a;
    }
    else
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

    // kmemsysSwizzIdToVmmuSegmentsRange -- halified (2 hals)
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysSwizzIdToVmmuSegmentsRange__ = &kmemsysSwizzIdToVmmuSegmentsRange_GH100;
    }
    else
    {
        pThis->__kmemsysSwizzIdToVmmuSegmentsRange__ = &kmemsysSwizzIdToVmmuSegmentsRange_GA100;
    }

    // kmemsysNumaAddMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysNumaAddMemory__ = &kmemsysNumaAddMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaAddMemory__ = &kmemsysNumaAddMemory_56cd7a;
    }

    // kmemsysNumaRemoveMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysNumaRemoveMemory__ = &kmemsysNumaRemoveMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaRemoveMemory__ = &kmemsysNumaRemoveMemory_b3696a;
    }

    // kmemsysNumaRemoveAllMemory -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysNumaRemoveAllMemory__ = &kmemsysNumaRemoveAllMemory_GH100;
    }
    // default
    else
    {
        pThis->__kmemsysNumaRemoveAllMemory__ = &kmemsysNumaRemoveAllMemory_b3696a;
    }

    // kmemsysPopulateMIGGPUInstanceMemConfig -- halified (2 hals) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysPopulateMIGGPUInstanceMemConfig__ = &kmemsysPopulateMIGGPUInstanceMemConfig_56cd7a;
    }
    else
    {
        // default
        pThis->__kmemsysPopulateMIGGPUInstanceMemConfig__ = &kmemsysPopulateMIGGPUInstanceMemConfig_KERNEL;
    }

    // kmemsysSetupAllAtsPeers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysSetupAllAtsPeers__ = &kmemsysSetupAllAtsPeers_46f6a7;
    }
    else
    {
        pThis->__kmemsysSetupAllAtsPeers__ = &kmemsysSetupAllAtsPeers_GV100;
    }

    // kmemsysRemoveAllAtsPeers -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysRemoveAllAtsPeers__ = &kmemsysRemoveAllAtsPeers_b3696a;
    }
    else
    {
        pThis->__kmemsysRemoveAllAtsPeers__ = &kmemsysRemoveAllAtsPeers_GV100;
    }

    // kmemsysAssertFbAckTimeoutPending -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000000UL) )) /* ChipHal: GH100 */ 
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_GH100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xc0000000UL) )) /* ChipHal: GB100 | GB102 */ 
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_GB100;
    }
    // default
    else
    {
        pThis->__kmemsysAssertFbAckTimeoutPending__ = &kmemsysAssertFbAckTimeoutPending_491d52;
    }

    // kmemsysGetMaxFbpas -- halified (3 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x10000400UL) )) /* ChipHal: GA100 | GH100 */ 
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_GA100;
    }
    else if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_TU102;
    }
    // default
    else
    {
        pThis->__kmemsysGetMaxFbpas__ = &kmemsysGetMaxFbpas_4a4dee;
    }

    // kmemsysGetEccDedCountSize -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysGetEccDedCountSize__ = &kmemsysGetEccDedCountSize_GH100;
    }
    else
    {
        pThis->__kmemsysGetEccDedCountSize__ = &kmemsysGetEccDedCountSize_TU102;
    }

    // kmemsysGetEccDedCountRegAddr -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0xd0000000UL) )) /* ChipHal: GH100 | GB100 | GB102 */ 
    {
        pThis->__kmemsysGetEccDedCountRegAddr__ = &kmemsysGetEccDedCountRegAddr_GH100;
    }
    else
    {
        pThis->__kmemsysGetEccDedCountRegAddr__ = &kmemsysGetEccDedCountRegAddr_TU102;
    }

    // kmemsysGetMaximumBlacklistPages -- halified (2 hals) body
    if (( ((chipHal_HalVarIdx >> 5) == 1UL) && ((1UL << (chipHal_HalVarIdx & 0x1f)) & 0x000003e0UL) )) /* ChipHal: TU102 | TU104 | TU106 | TU116 | TU117 */ 
    {
        pThis->__kmemsysGetMaximumBlacklistPages__ = &kmemsysGetMaximumBlacklistPages_GM107;
    }
    else
    {
        pThis->__kmemsysGetMaximumBlacklistPages__ = &kmemsysGetMaximumBlacklistPages_GA100;
    }

    // kmemsysGetFbInfos -- halified (2 hals)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__kmemsysGetFbInfos__ = &kmemsysGetFbInfos_VF;
    }
    else
    {
        pThis->__kmemsysGetFbInfos__ = &kmemsysGetFbInfos_ac1694;
    }

    // kmemsysInitMissing -- virtual inherited (engstate) base (engstate)
    pThis->__kmemsysInitMissing__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysInitMissing;

    // kmemsysStatePreInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kmemsysStatePreInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStatePreInitUnlocked;

    // kmemsysStateInitUnlocked -- virtual inherited (engstate) base (engstate)
    pThis->__kmemsysStateInitUnlocked__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStateInitUnlocked;

    // kmemsysStatePostUnload -- virtual inherited (engstate) base (engstate)
    pThis->__kmemsysStatePostUnload__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysStatePostUnload;

    // kmemsysIsPresent -- virtual inherited (engstate) base (engstate)
    pThis->__kmemsysIsPresent__ = &__nvoc_up_thunk_OBJENGSTATE_kmemsysIsPresent;
} // End __nvoc_init_funcTable_KernelMemorySystem_1 with approximately 90 basic block(s).


// Initialize vtable(s) for 42 virtual method(s).
void __nvoc_init_funcTable_KernelMemorySystem(KernelMemorySystem *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 42 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelMemorySystem *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelMemorySystem), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelMemorySystem));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelMemorySystem);

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

    __nvoc_init_KernelMemorySystem(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelMemorySystem(pThis, pRmhalspecowner);
    if (status != NV_OK) goto __nvoc_objCreate_KernelMemorySystem_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelMemorySystem_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_OBJENGSTATE.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelMemorySystem));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelMemorySystem(KernelMemorySystem **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_KernelMemorySystem(ppThis, pParent, createFlags);

    return status;
}

